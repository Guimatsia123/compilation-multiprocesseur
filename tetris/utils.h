#pragma once

#include <GL/gl.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define SWAP(a,b) {int *x = &(a); int *y = &(b); int t = *x; *x = *y; *y = t;}
#define CLAMP(lo,x,hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

int check_shader_errors(GLuint shader, char *name);
int check_program_errors(GLuint shader, char *name);
char *file2str(char *filename);
unsigned int file2shader(unsigned int type, char *filename);
