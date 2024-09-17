#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

#define TIMER_NAMES \
X(SDL_Delay), \
    X(do_events), \
    X(update_player), \
    X(update_particles), \
    X(draw_start), \
    X(draw_menu), \
    X(draw_player), \
    X(draw_particles), \
    X(draw_end), \
    X(SDL_GL_SwapWindow),

#include "mtimer.h"

extern int timer_curr_id;
extern unsigned long long timer_then;
extern unsigned long long timer_times[timer_ + 1];

void timer_print(char *buf, size_t n, bool show_all);
