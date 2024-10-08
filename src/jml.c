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

// verifie si le fichier a l'extension .c
int has_c_extension(const char *filename, const char* ext) {
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot, ext) == 0);
}
// Function to create the build directory if it doesn't exist
int create_build_directory(const char* builddir) {
    struct stat st;


    // Check if the directory exists
    if (stat(builddir, &st) == -1) {
        // Directory does not exist, create it
        if (mkdir(builddir, 0755) != 0) {  // 0755 permissions (rwxr-xr-x)
            perror("mkdir");
            return -1;
        }
        printf("Created build directory: %s\n", builddir);
    } else if (!S_ISDIR(st.st_mode)) {
        // If it exists but it's not a directory, return an error
        fprintf(stderr, "Error: %s exists but is not a directory\n", builddir);
        return -1;
    }

    return 0;
}
void remove_extension(char* filename){
    char* file = strrchr(filename, '.');
    if(file) *file = '\0';

}


// Point d'entrée du programme principal
// Les options sont passées dans struct jml_app
int jml_main(struct jml_app* app) {
  /*
   * Phase 1: Lister les fichiers sources à compiler
   */
    struct list *source_files;  // List of source files
    struct list *compile_commands;  // List of compile commands
    struct list *list_obj_files;
    int result;
    source_files = list_new(NULL, free);
    list_obj_files = list_new(NULL, free );
  //  compile_commands = list_new(NULL, free);

    printf("Printing source files from directory %s ...\n", app->sourcedir);
    jml_listdir(source_files, app->sourcedir, ".c");

    struct list_node* node = list_head(source_files);
    while (!list_end(node)) {
      printf("%s \n", (char*)node->data);
      node = node->next;
    }


  /*
   * Phase 2: Compiler les sources
   */

    printf("Compiling source files...\n");

        struct list_node* node1 = list_head(source_files);

        while(!list_end(node1)) {
             char* source_file = ( char*)node1->data;
            struct command compile_cmd;

            // Initialize the compile command
            jml_command_init(&compile_cmd);
            /*
             * Ensure the build directory exists
             */
            result = create_build_directory(app->builddir);
            if (result != 0) {
                perror("Erreur de creation du repetoire app->builddir ");
            }

            // Construct the compile command (e.g., gcc -c source.c -o source.o)
            jml_command_append(&compile_cmd, "gcc");
            jml_command_append(&compile_cmd, "-c");  // Compile flag
            jml_command_append(&compile_cmd, source_file);  // Source file
            jml_command_append(&compile_cmd, "-o");

            // Construct the output object file name
            remove_extension(source_file);

            char obj_file[256];
            snprintf(obj_file, sizeof(obj_file), "%s/%s.o", app->builddir, strrchr(source_file, '/') + 1);  // Object file path

            jml_command_append(&compile_cmd, obj_file);  // Output object file
            struct list_node *new_file =  list_node_new(strdup(obj_file));
            list_push_back(list_obj_files, new_file);

            // Optionally, add CFLAGS from the app structure if provided
            if (app->cflags) {
                jml_command_append_many(&compile_cmd, app->cflags, " ");
            }

            // Append the command to the list of compile commands
           // list_concat(&compile_commands, compile_cmd);
            // Execute the compile command
            result = jml_command_exec_one(&compile_cmd);
            jml_command_print(&compile_cmd, stdout);
            if (result != 0 && !app->keep_going) {
                fprintf(stderr, "Compilation failed for %s\n", source_file);
                return result;
            }

            // Clean up the command after execution
            jml_command_destroy(&compile_cmd);
            node1 = node1->next;
        }


  /*
   * Phase 3: Édition des liens (link)
   */
        struct command edit_link_cmd;
        jml_command_init(&edit_link_cmd);
        jml_command_append(&edit_link_cmd, "gcc");
        jml_command_append(&edit_link_cmd, "-o");  // Compile flag
        char exec_file[256];
        snprintf(exec_file, sizeof(exec_file), "%s/%s", app->builddir, app->progname);
        jml_command_append(&edit_link_cmd, exec_file);

        struct list_node* object_file = list_head(list_obj_files);
        while(!list_end(object_file)){
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

  return 0;
}

// Lister les fichier ayant l'extension ext du répertoire dirname.
int jml_listdir(struct list* files, const char* dirname, const char* ext) {
  if (!dirname) {
    return -1;
  }
  // Ouvrir le répertoire courant, retourne un itérateur
  DIR* dir = opendir(dirname);
  // Lister le contenu du répertoire ouvert


  while (1) {
    struct dirent* item = readdir(dir);
    if (item == NULL) {
      break;
    }
    if(has_c_extension(item->d_name, ext)){
/*
       if (!cwk_path_get_relative(dirname, item->d_name, filepath, sizeof(filepath))) {
           fprintf(stderr, "Failed to get absolute path for %s\n", item->d_name);
           continue;
       }*/
       char filepath[1024];
       //cwk_path_get_absolute(dirname, item->d_name, filepath, sizeof(filepath));
        snprintf(filepath, sizeof(filepath), "%s/%s", dirname, item->d_name);
       struct list_node *new_node =  list_node_new(strdup(filepath));
       list_push_back(files, new_node);
       //printf("%s\n", new_node->data);
      // printf("%s\n", item->d_name);

    }

  }
  closedir(dir);
  return 0;
}

// Exécuter une seule commande
int jml_command_exec_one(struct command* cmd) {
  // À COMPLÉTER
    // On verifie s'il y a une commande a executer
      if (cmd == NULL || cmd->args == NULL || cmd->nargs == 0) {
          fprintf(stderr, "No command to execute\n");
          return -1;
      }

      // On creer un processus enfant
      pid_t pid = fork();

      if (pid < 0) {
          perror("Erreur de creation du fork\n");
          return -1;
      } else if (pid == 0) {
          // Dans le processus enfant, on execute la commande
          execvp(cmd->args[0], cmd->args);  // cmd->args is a NULL-terminated array of arguments
          // si execvp echoue
          perror("Erreur : execvp");
          exit(EXIT_FAILURE);  // Exit child process with failure code
      } else {
          // On demande au processus parent d'attendre la fin d'execution de l'enfant
          int status;
          if (waitpid(pid, &status, 0) == -1) {
              perror("waitpid");
              return -1;
          }

          // Check if the child exited successfully
          if (WIFEXITED(status)) {
              int exit_status = WEXITSTATUS(status);
              if (exit_status == 0) {
                  return 0;  // Success
              } else {
                  fprintf(stderr, "Command exited with non-zero status: %d\n", exit_status);
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
  // À COMPLÉTER
  return 0;
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
