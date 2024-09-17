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

// Point d'entrée du programme principal
// Les options sont passées dans struct jml_app
int jml_main(struct jml_app* app) {
  /*
   * Phase 1: Lister les fichiers sources à compiler
   */

  /*
   * Phase 2: Compiler les sources
   */

  /*
   * Phase 3: Édition des liens (link)
   */

  return 0;
}

// Lister les fichier ayant l'extension ext du répertoire dirname.
int jml_listdir(struct list* files, const char* dirname, const char* ext) {
  if (!dirname) {
    return -1;
  }

  // À COMPLETER
  return 0;
}

// Exécuter une seule commande
int jml_command_exec_one(struct command* cmd) {
  // À COMPLÉTER
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
