#include "utils.h"
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>

int check_shader_errors(GLuint shader, char *name) {
  GLint success;
  GLchar log[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success)
    return 0;
  glGetShaderInfoLog(shader, 1024, NULL, log);
  fprintf(stderr, "ERROR in %s shader program: %s\n", name, log);
  exit(1);
  return 1;
}

int check_program_errors(GLuint shader, char *name) {
  GLint success;
  GLchar log[1024];
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (success)
    return 0;
  glGetProgramInfoLog(shader, 1024, NULL, log);
  fprintf(stderr, "ERROR in %s shader: %s\n", name, log);
  exit(1);
  return 1;
}

// please free() the returned string
char *file2str(char *filename) {
  FILE *f;

#if defined(_MSC_VER) && _MSC_VER >= 1400
  if (fopen_s(&f, filename, "r"))
    f = NULL;
#else
  f = fopen(filename, "r");
#endif

  if (!f)
    goto bad;
  fseek(f, 0, SEEK_END);
  size_t sz = ftell(f);
  rewind(f);
  char *buf = calloc(sz + 1, sizeof *buf);
  if (fread(buf, 1, sz, f) != sz)
    goto bad;
  fclose(f);
  return buf;

bad:
  fprintf(stderr, "Failed to open/read %s\n", filename);
  return NULL;
}

unsigned int file2shader(unsigned int type, char *filename) {
  /*
   * HACK: On assume que le fichier est relatif au fichier source, ce qui n'est
   * pas une bonne solution en général.
   *
   * Ceci ne fonctionne pas pour distribuer le programme, car le chemin sera
   * codé en dur dans l'exécutable et ne fonctionne que sur la machine sur
   * laquelle on compile. Cependant, cela permet de simplifier le TP et c'est
   * suffisant.
   *
   * La bonne pratique serait d'include le vertex et fragment shader dans
   * l'exécutable, ou copier les fichiers relativement à l'emplacement de
   * l'exécutable.
   */
  char path[1024];
  char *dir = strdup(__FILE__);
  char *tmp = dirname(dir);
  strcpy(path, tmp);
  strcat(path, "/");
  strcat(path, filename);
  free(dir);
  char *code = file2str(path);
  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, (const char *const *)&code, NULL);
  glCompileShader(id);
  check_shader_errors(id, filename);
  free(code);
  return id;
}
