#pragma once

#ifndef TIMER_NAMES
#define TIMER_NAMES
#endif

// You must X-define TIMER_NAMES before including this file, e.g:
//
// #define TIMER_NAMES X(create_hmap), X(update_world), X(update_player),
//

#define TIMER(name)                                                            \
  {                                                                            \
    unsigned long long now = SDL_GetPerformanceCounter();                      \
    if (!timer_then)                                                           \
      timer_then = now;                                                        \
    timer_times[timer_curr_id] += now - timer_then;                            \
    timer_curr_id = timer_##name;                                              \
    timer_then = now;                                                          \
  }

// FIXME: push id onto stack if re-entering
#define TIMECALL(f, args)                                                      \
  {                                                                            \
    unsigned long long now = SDL_GetPerformanceCounter();                      \
    if (!timer_then)                                                           \
      timer_then = now;                                                        \
    timer_times[timer_curr_id] += now - timer_then;                            \
    timer_then = now;                                                          \
    (f) args;                                                                  \
    now = SDL_GetPerformanceCounter();                                         \
    timer_times[timer_##f] += now - timer_then;                                \
    timer_curr_id = timer_;                                                    \
    timer_then = now;                                                          \
  }

enum timernames {
#define X(x) timer_##x
  TIMER_NAMES
#undef X
      timer_
};

extern char timernamesprint[][80];
