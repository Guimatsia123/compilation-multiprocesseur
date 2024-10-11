#include "jml.h"
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cwalk.h"
#include "utils.h"

// Verifie si le fichier a l'extension donnée (par exemple, ".c")
int has_c_extension(const char *filename, const char *ext) {
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot, ext) == 0);
}

// Fonction pour creer un repertoire s'il n'existe pas.
int create_build_directory(const char* builddir) {
    struct stat st;
    if (stat(builddir, &st) == -1) {
        if (mkdir(builddir, 0755) != 0) {
            perror("Error: mkdir()\n");
            return -1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        // If it exists but it's not a directory, return an error
        fprintf(stderr, "Error: %s exists but is not a directory\n", builddir);
        return -1;
    }

    return 0;
}

// Remove the file extension from the filename
void remove_extension(char* filename) {
    char* file = strrchr(filename, '.');
    if (file) *file = '\0';
}

// Point d'entrée du programme principal
int jml_main(struct jml_app* app) {
  /*
   * Phase 1: Lister les fichiers sources à compiler
   */
    struct list *source_files;        // List of source files
    struct list *compile_commands;    // List of compile commands
    struct list *list_obj_files;      // List of object files
    int result;

    source_files = list_new(NULL, free);
    list_obj_files = list_new(NULL, free);
    compile_commands = list_new(NULL, jml_command_free);

    printf("Step 1 : Printing source files from directory %s ...\n", app->sourcedir);
    jml_listdir(source_files, app->sourcedir, ".c");

    struct list_node* node = list_head(source_files);
    while (!list_end(node)) {
      printf("%s \n", (char*)node->data);
      node = node->next;
    }

  /*
   * Phase 2: Compiler les sources
   */
    printf("Step 2 : Compiling source files...\n");

    struct list_node* node1 = list_head(source_files);
    while (!list_end(node1)) {
        char* source_file = (char*)node1->data;
        struct command *compile_cmd;

        compile_cmd = malloc(sizeof(struct command));
        if (!compile_cmd) {
            perror("Error allocating memory for compile_cmd");
            return -1;
        }
        jml_command_init(compile_cmd);

        /*
         * Ensure the build directory exists
         */
        result = create_build_directory(app->builddir);
        if (result != 0) {
            perror("Error creating build directory");
            list_free(source_files);
            list_free(list_obj_files);
            list_free(compile_commands);

            return -1;
        }

        // Construct the compile command (e.g., gcc -c source.c -o source.o)
        jml_command_append(compile_cmd, "gcc");
        jml_command_append(compile_cmd, "-c");  // Compile flag
        jml_command_append(compile_cmd, source_file);  // Source file
        jml_command_append(compile_cmd, "-o");

        // Copy the source file name and remove the extension to create the object file name
        char obj_file[256];
        char filename_copy[256];
        strncpy(filename_copy, source_file, sizeof(filename_copy));
        remove_extension(filename_copy);

        snprintf(obj_file, sizeof(obj_file), "%s/%s.o", app->builddir, strrchr(filename_copy, '/') + 1);  // Object file path
        jml_command_append(compile_cmd, obj_file);  // Output object file

        struct list_node *new_file = list_node_new(strdup(obj_file));
        list_push_back(list_obj_files, new_file);

        // Optionally, add CFLAGS from the app structure if provided
        if (app->cflags) {
            jml_command_append_many(compile_cmd, app->cflags, " ");
        }

        // Append the command to the list of compile commands
        struct list_node *new_cmd = list_node_new(compile_cmd);
        list_push_back(compile_commands, new_cmd);

        node1 = node1->next;
        //jml_command_destroy(compile_cmd);

    }

    // Execute all the compile commands
    if (jml_command_exec_all(compile_commands, app->nproc, app->keep_going) != 0) {
        fprintf(stderr, "Error compiling source files\n");
        list_free(source_files);
        list_free(list_obj_files);
        list_free(compile_commands);

        return -1;
    }

  /*
   * Phase 3: Édition des liens (link)
   */
    printf("Step 3 : Creating exec file...\n");

    struct command edit_link_cmd;
    jml_command_init(&edit_link_cmd);
    jml_command_append(&edit_link_cmd, "gcc");
    jml_command_append(&edit_link_cmd, "-o");  // Compile flag

    char exec_file[256];
    snprintf(exec_file, sizeof(exec_file), "%s/%s", app->builddir, app->progname);
    jml_command_append(&edit_link_cmd, exec_file);

    struct list_node* object_file = list_head(list_obj_files);
    while (!list_end(object_file)) {
        jml_command_append(&edit_link_cmd, object_file->data);
        object_file = object_file->next;
    }

    // Optionally, add libraries if specified
    if (app->libs) {
        jml_command_append_many(&edit_link_cmd, app->libs, " ");
    }

    jml_command_exec_one(&edit_link_cmd);
    jml_command_print(&edit_link_cmd, stdout);

    jml_command_destroy(&edit_link_cmd);
    list_free(source_files);
    list_free(list_obj_files);
    list_free(compile_commands);

    return 0;
}

// Lister les fichiers ayant l'extension ext du répertoire dirname.
int jml_listdir(struct list* files, const char* dirname, const char* ext) {
    DIR* dir = opendir(dirname);
    if (!dir) {
        perror("Error: opendir\n");
        return -1;
    }

    char filepath[1024];
    while (1) {
        struct dirent* item = readdir(dir);
        if (item == NULL) {
            break;
        }
        cwk_path_join(dirname, item->d_name, filepath, sizeof(filepath));

        struct stat st;
        if (has_c_extension(item->d_name, ext) && stat(filepath, &st) == 0 && S_ISREG(st.st_mode)) {
            struct list_node *new_node = list_node_new(strdup(filepath));
            list_push_back(files, new_node);
        }
    }
    closedir(dir);
    return 0;
}

// Exécuter une seule commande
int jml_command_exec_one(struct command* cmd) {
    if (cmd == NULL || cmd->args == NULL || cmd->nargs == 0) {
        fprintf(stderr, "No command to execute\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error: fork\n");
        return -1;
    } else if (pid == 0) {
        execvp(cmd->args[0], cmd->args);
        perror("Error: execvp");
        exit(EXIT_FAILURE);  // Child process exits on failure
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Command exited with non-zero status: %d\n", WEXITSTATUS(status));
                return -1;
            }
        } else {
            fprintf(stderr, "Command did not terminate normally\n");
            return -1;
        }
    }
    return 0;
}

// Exécuter la liste de commandes avec max_process simultanément
int jml_command_exec_all(struct list* commands, int max_process, int keep_going) {
    struct list_node* node = list_head(commands);
    int active_processes = 0;
    int status;
    int compilation_success = 1;

    while (!list_end(node)) {
        if (active_processes >= max_process) {
            wait(&status);
            active_processes--;
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                compilation_success = 0;
                if (!keep_going) break;
            }
        }

        int pid = fork();
        if (pid == 0) {
            struct command* cmd = (struct command*) node->data;
            execvp(cmd->args[0], cmd->args);
            perror("Error in execvp");
            exit(EXIT_FAILURE);
        } else {
            active_processes++;
        }
        node = node->next;
    }

    while (active_processes > 0) {
        wait(&status);
        active_processes--;
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            compilation_success = 0;
            if (!keep_going) break;
        }
    }

    return compilation_success ? 0 : -1;
}
/*
 * Fonctions fournies pour aider à la création d'une commande.
 */
void jml_command_init(struct command* c) {
  c->capacity = 1;
  c->args = calloc(c->capacity, sizeof(char*));
  c->nargs = 0;
}

void jml_command_append(struct command* c, const char* arg) {
  if (c->capacity == c->nargs + 1) {
    c->capacity *= 2;
    c->args = realloc(c->args, sizeof(char*) * c->capacity);
  }
  c->args[c->nargs++] = strdup(arg);
  c->args[c->nargs] = NULL;
}

void jml_command_destroy(struct command* c) {
  for (int i = 0; i < c->nargs; i++) {
    free(c->args[i]);
  }
  free(c->args);
  c->capacity = 0;
}

void jml_command_print(struct command* c, FILE* out) {
  for (int i = 0; i < c->nargs; i++) {
    fprintf(out, "%s ", c->args[i]);
  }
  fprintf(out, "\n");
}

void jml_command_free(void* cmd) {
  jml_command_destroy(cmd);
  free(cmd);
}

void jml_command_append_many(struct command* c, const char* arg, const char* delim) {
  struct list* tmp = list_new(NULL, free);
  split_string(tmp, arg, delim, 0);

  struct list_node* node = list_head(tmp);
  while (!list_end(node)) {
    jml_command_append(c, node->data);
    node = node->next;
  }

  list_free(tmp);
}

