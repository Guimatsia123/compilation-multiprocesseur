#pragma once

void font_init();
void font_begin(int w, int h);
void font_add_text(char *s, int inx, int iny, float height);
void font_end(float r, float g, float b);
