#ifndef INF3173_TP1_JML_H
#define INF3173_TP1_JML_H

#include <stdio.h>

#include "list.h"

#define MAX_ARGS 64

#ifdef __cplusplus
extern "C" {
#endif

struct jml_app {
  int nproc;
  int keep_going;
  char* sourcedir;
  char* builddir;
  char* progname;
  char* cflags;
  char* libs;
};

struct command {
  char** args;
  int nargs;
  int capacity;
};

/*
 * Point d'entrée du programme
 */

int jml_main(struct jml_app* app);

/*
 * Lister les fichiers réguliers contenus dans un répertoire qui ont l'extension
 * spécifiée. Les fichiers sont ajoutés à la liste fournie en argument.
 */
int jml_listdir(struct list* files, const char* dirname, const char* ext);

/*
 * API pour construction d'une commande sous forme d'un tableau de
 * pointeurs de chaines.
 *
 * jml_command_init : doit être appellé avant d'utiliser la structure de commande.
 * jml_command_append: ajoute un argument à la commande, agrandit le tableau si nécessaire. La chaine est copiée.
 * jml_command_append_many: ajoute plusieurs arguments à la commande, agrandit le tableau si nécessaire. La chaine est
 * copiée. jml_command_print: affiche la commande à la sortie standard jml_command_destroy: libérer la structure à la
 * fin jml_command_destroy: libérer la structure à la fin
 *
 * champs de struct command:
 *   args: tableau de pointeur, toujours terminé par NULL
 *   nargs: nombre d'arguments
 *   capacity: utilisé à l'interne pour savoir s'il faut agrandir le tableau (automatique)
 *
 * Toutes les chaines sont copiées avec strdup() et donc détenues pas l'objet
 * struct command, ce qui fait en sorte que l'on peut les libérer uniformément
 * la mémoire dans jml_command_destroy(). Le but d'éviter d'avoir à faire la
 * différence entre les chaines statiques (que l'on ne doit pas libérer avec
 * free()) et les chaines allouées dynamiquement.
 *
 * jml_command_free libère les éléments de la liste et est passé à list_new().
 * En temps normal, on ne fait pas d'appel directement à cette fonction.
 */
void jml_command_init(struct command* c);
void jml_command_append(struct command* c, const char* arg);
void jml_command_append_many(struct command* c, const char* arg, const char* sep);
void jml_command_print(struct command* c, FILE* out);
void jml_command_destroy(struct command* c);
void jml_command_free(void* c);

/*
 * Exécuter une seule commande. Retourne seulement lorsque la commande est terminée.
 *
 * Retourne 0 si l'exécution a fonctionné, -1 sinon.
 */
int jml_command_exec_one(struct command* cmd);

/*
 * Exécuter une liste de commandes, avec au plus max_process processus simultanés.
 * Si dry_run == 1, alors aucune commande n'est exécutée, seulement affichée.
 * Si keep_going == 1, alors une commande qui échoue n'arrête pas le traitement
 *
 * Retourne 0 si toutes les commandes fonctionnent, -1 sinon.
 */
int jml_command_exec_all(struct list* commands, int max_process, int keep_going);

#ifdef __cplusplus
}
#endif

#endif  // INF3173_TP1_JML_H
