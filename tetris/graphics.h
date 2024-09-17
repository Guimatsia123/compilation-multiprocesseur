#pragma once

void text(char *fstr, int value);
void draw_setup();
void vertex(float x, float y, float r, float g, float b);
void rect(float x, float y, float w, float h);
void draw_start();
void draw_end();
// set the current draw color to the color assoc. with a shape
void set_color_from_shape(int shape, int shade);
void set_color(int r, int g, int b);
void draw_menu();
void draw_particles();
// draw a single mino (square) of a shape
void draw_mino(int x, int y, int shape, int outline, int part);
void draw_shape(int x, int y, int color, int rot, int flags);
// draw everything in the game on the screen for current player
void draw_player();
void reflow();
// recalculate sizes and positions on resize
void resize(int x, int y);
