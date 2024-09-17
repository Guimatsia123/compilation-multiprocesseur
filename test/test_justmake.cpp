#include <fcntl.h>
#include <jml.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utils.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <chrono>
#include <fstream>
#include <string>

#include "inf3173.h"

/*
 * Mesurer le délai d'exécution est une vérification simple, mais peu fiable.
 * Avec une activité en arrière plan, il se peut que le délai dépasse la
 * tolérance, alors que le programme fonctionne normalement.
 *
 * On utilise des délais particulièrement longs (1 seconde) pour rendre le test
 * plus robuste, mais ce n'est pas infaillible. Fermer tous les programmes
 * inutiles pour éviter les perturbations.
 *
 * Si vous utilisez valgrind pour vérifier les fuites mémoires, alors il faut
 * faire passer le test avant, car la macro REQUIRE() déclenche une exception
 * C++ et le reste de la fonction de test incluant le nettoyage normal, ne
 * s'exécute pas.
 */

static const double tol = 0.1;  // tolérance de 10%

#define TEST_DIR SOURCE_DIR "/demo"
#define DATA_DIR SOURCE_DIR "/test/data"

using ms = std::chrono::milliseconds;

/*
 * Lister les fichiers d'entêtes d'un fichier source. Si un de ces fichier a été
 * modifié, alors il doit être recompilé. Fonctionnalité inutilisée, car les
 * sources sont recompilées de manière inconditionnelles.
 */
TEST_CASE("ParseDeps") {
  struct list* lst = list_new(NULL, free);
  const char* filepath = DATA_DIR "/source.deps";

  std::string s;
  {
    std::ifstream f(filepath, std::ios::in | std::ios::binary | std::ios::ate);
    auto size = f.tellg();
    s.resize(size);
    f.seekg(0, std::ios::beg);
    f.read(s.data(), size);
  }
  split_string(lst, s.c_str(), " :\\\n", 1);
  REQUIRE(list_size(lst) == 29);
  list_free(lst);
}

/*
 * Comparaison de deux estampille de temps. Serait utile pour détecter si un
 * fichier source a été modifié depuis la dernière compilation. Inutilisé.
 */
TEST_CASE("ComparaisonTemps") {
  struct timespec t1 = {.tv_sec = 1, .tv_nsec = 1};
  struct timespec t2 = {.tv_sec = 2, .tv_nsec = 1};
  struct timespec t3 = {.tv_sec = 1, .tv_nsec = 2};

  CHECK(timespec_cmp(&t1, &t1) == 0);

  CHECK(timespec_cmp(&t1, &t2) < 0);
  CHECK(timespec_cmp(&t2, &t1) > 0);

  CHECK(timespec_cmp(&t1, &t3) < 0);
  CHECK(timespec_cmp(&t3, &t1) > 0);
};

/*
 * Vérification de la construction d'une commande. Ces fonctions sont fournies.
 */
TEST_CASE("CommandBuilder") {
  static char buf[32];
  int n = 10;
  struct command c;
  jml_command_init(&c);
  for (int i = 0; i < n; i++) {
    std::sprintf(buf, "%d", i);
    jml_command_append(&c, buf);
  }

  SECTION("nombre d'arguments") {
    REQUIRE(c.nargs == n);
  }
  SECTION("terminé par NULL") {
    REQUIRE(c.args[n] == NULL);
  }

  SECTION("contenu") {
    for (int i = 0; i < n; i++) {
      std::sprintf(buf, "%d", i);
      /*
       * En utilisant la comparaison avec std::string plutôt que strcmp() == 0,
       * la macro REQUIRE affiche le contenu plutôt qu'un entier, ce qui est
       * plus pratique.
       */
      REQUIRE(std::string(c.args[i]) == std::string(buf));
    }
  }

  SECTION("CommandPrint") {
    // Imprimer en mémoire pour vérifier la sortie
    char* ptr;
    size_t size;
    FILE* out = open_memstream(&ptr, &size);
    jml_command_print(&c, out);
    fclose(out);
    REQUIRE(size == 21);
    REQUIRE(std::string(ptr) == "0 1 2 3 4 5 6 7 8 9 \n");
    free(ptr);
  }

  jml_command_destroy(&c);
}

/*
 * Vérification du lancement d'une commande.
 */
TEST_CASE("CommandExecOneOk") {
  struct command cmd;
  jml_command_init(&cmd);
  jml_command_append(&cmd, "sleep");
  jml_command_append(&cmd, "1");
  const auto t1 = std::chrono::steady_clock::now();
  REQUIRE(jml_command_exec_one(&cmd) == 0);
  const auto t2 = std::chrono::steady_clock::now();
  const auto dt = std::chrono::duration_cast<ms>(t2 - t1).count();
  REQUIRE_THAT(dt, Catch::Matchers::WithinRel(1000.0, tol));
  jml_command_destroy(&cmd);
}

/*
 * TEST RELATIFS À VOTRE TRAVAIL
 */

/*
 * Vérification du code de retour d'une commande qui se lance correctement, mais
 * retourne une erreur.
 */
TEST_CASE("CommandExecOneErreurCommande") {
  struct command cmd;
  jml_command_init(&cmd);
  jml_command_append(&cmd, "sleep");
  jml_command_append(&cmd, "berk");
  REQUIRE(jml_command_exec_one(&cmd) < 0);
  jml_command_destroy(&cmd);
}

/*
 * Vérification du cas de l'échec de lancement d'une commande.
 */
TEST_CASE("CommandExecOneErreurExec") {
  struct command cmd;
  jml_command_init(&cmd);
  jml_command_append(&cmd, "berk");
  jml_command_append(&cmd, "berk");
  REQUIRE(jml_command_exec_one(&cmd) < 0);
  jml_command_destroy(&cmd);
}

/*
 * Vérification du lancement de commandes en séquence et simultanément.
 */
TEST_CASE("CommandExecAllOk") {
  struct list* lst = list_new(NULL, jml_command_free);
  for (int i = 0; i < 5; i++) {
    struct command* cmd = static_cast<struct command*>(malloc(sizeof(struct command)));
    jml_command_init(cmd);
    jml_command_append(cmd, "sleep");
    jml_command_append(cmd, "1");
    jml_command_print(cmd, stdout);
    struct list_node* node = list_node_new(cmd);
    list_push_back(lst, node);
  }

  SECTION("Sequentiel") {
    // trivial : le temps est proportionnel au nombre de tâches
    const auto t1 = std::chrono::steady_clock::now();
    REQUIRE(jml_command_exec_all(lst, 1, 1) == 0);
    const auto t2 = std::chrono::steady_clock::now();
    const auto dt = std::chrono::duration_cast<ms>(t2 - t1).count();
    REQUIRE_THAT(dt, Catch::Matchers::WithinRel(1000.0 * 5, tol));
  }

  SECTION("Multiprocesseur") {
    // temps avec 2 processeurs pour 5 tâches : 3 unités de temps
    // (sleep, sleep), (sleep, sleep), (sleep)
    const auto t1 = std::chrono::steady_clock::now();
    REQUIRE(jml_command_exec_all(lst, 2, 1) == 0);
    const auto t2 = std::chrono::steady_clock::now();
    const auto dt = std::chrono::duration_cast<ms>(t2 - t1).count();
    REQUIRE_THAT(dt, Catch::Matchers::WithinRel(1000.0 * 3, tol));
  }

  list_free(lst);
}

/*
 * Gestion des erreurs pendant des commandes simultanées.
 */
TEST_CASE("CommandExecAllKeepGoing") {
  // Simulation d'un échec de la deuxième commande
  struct list* lst = list_new(NULL, jml_command_free);
  for (int i = 0; i < 5; i++) {
    struct command* cmd = static_cast<struct command*>(malloc(sizeof(struct command)));
    jml_command_init(cmd);
    jml_command_append(cmd, "sleep");
    if (i == 1) {
      jml_command_append(cmd, "berk");
    } else {
      jml_command_append(cmd, "1");
    }
    jml_command_print(cmd, stdout);
    struct list_node* node = list_node_new(cmd);
    list_push_back(lst, node);
  }

  SECTION("KeepGoingOn") {
    // 4/5 commandes fonctionnent, à 2 processeurs on a donc environ délais * 2
    const auto t1 = std::chrono::steady_clock::now();
    CHECK(jml_command_exec_all(lst, 2, 1) < 0);
    const auto t2 = std::chrono::steady_clock::now();
    const auto delta = std::chrono::duration_cast<ms>(t2 - t1).count();
    CHECK_THAT(delta, Catch::Matchers::WithinRel(1000.0 * 2, tol));
  }

  SECTION("KeepGoingOff") {
    // puisque la 2e commande échoue avec une limite de
    // processus de 2, on devrait attendre la 1ère commande seulement.
    const auto t1 = std::chrono::steady_clock::now();
    CHECK(jml_command_exec_all(lst, 2, 0) < 0);
    const auto t2 = std::chrono::steady_clock::now();
    const auto delta = std::chrono::duration_cast<ms>(t2 - t1).count();
    CHECK_THAT(delta, Catch::Matchers::WithinRel(1000.0, tol));
  }

  list_free(lst);
}

/*
 * Test pour vérifier que lister les sources d'un répertoire retourne le résulat
 * attendu.
 */
TEST_CASE("ListerRepertoire") {
  struct list* lst = list_new(NULL, free);
  jml_listdir(lst, TEST_DIR, ".c");
  REQUIRE(list_size(lst) == 3);
  list_free(lst);
}
