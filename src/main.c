#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "jml.h"

void
print_usage() {
  fprintf(stderr,
          "Usage: %s "
          "-j N "
          "-k "
          "-s sourcedir "
          "-b builddir "
          "-c cflags "
          "-l libs "
          "--help]\n",
          "justmake");
  exit(1);
}

int main(int argc, char** argv) {
  int ret = EXIT_FAILURE;

  struct option options[] = {
      {"j", 1, 0, 'm'},           //
      {"keep-going", 0, 0, 'k'},  //
      {"sourcedir", 1, 0, 's'},   //
      {"builddir", 1, 0, 'b'},    //
      {"progname", 1, 0, 'p'},    //
      {"cflags", 1, 0, 'c'},      //
      {"libs", 1, 0, 'l'},        //
      {"help", 0, 0, 'h'},        //
      {0, 0, 0, 0}                //
  };

  struct jml_app app = {
      .nproc = 1,                   //
      .keep_going = 0,              //
      .sourcedir = NULL,            //
      .builddir = "jmlbuild",       //
      .progname = "programme.exe",  //
      .cflags = NULL,               //
      .libs = NULL                  //
  };

  int opt;
  int idx;
  static const char* fmt = "j:s:b:p:c:l:hk";
  while ((opt = getopt_long(argc, argv, fmt, options, &idx)) != -1) {
    switch (opt) {
      case 'j':
        app.nproc = atoi(optarg);
        break;
      case 'k':
        app.keep_going = 1;
        break;
      case 's':
        app.sourcedir = optarg;
        break;
      case 'b':
        app.builddir = optarg;
        break;
      case 'p':
        app.progname = optarg;
        break;
      case 'c':
        app.cflags = optarg;
        break;
      case 'l':
        app.libs = optarg;
        break;
      case 'h':
      default:
        print_usage();
    }
  }

  // Affichage des options
  printf("options\n");
  printf("  nproc     : %d\n", app.nproc);
  printf("  sourcedir : %s\n", app.sourcedir);
  printf("  builddir  : %s\n", app.builddir);
  printf("  progname  : %s\n", app.progname);
  printf("  cflags    : %s\n", app.cflags);
  printf("  libs      : %s\n", app.libs);

  if (app.sourcedir == NULL) {
    printf("Erreur: répertoire source manquant\n");
    return -1;
  }

  return jml_main(&app);
}
