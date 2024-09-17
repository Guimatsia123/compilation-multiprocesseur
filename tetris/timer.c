#include "timer.h"

int timer_curr_id = timer_;
unsigned long long timer_then = 0;
unsigned long long timer_times[timer_ + 1] = {0};

char timernamesprint[][80] = {
#define X(x) #x
    TIMER_NAMES
#undef X
    "uncounted"};

void timer_print(char *buf, size_t n, bool show_all) {
  char *p = buf;
  int i = 0;
  unsigned long long sum = 0;
  unsigned long long freq = SDL_GetPerformanceFrequency();

  for (i = 0; i <= timer_; i++)
    sum += timer_times[i];

  for (i = 0; i <= timer_; i++) {
    float secs = (float)timer_times[i] / (float)freq;
    float pct = 100.f * (float)timer_times[i] / sum;
    if ((show_all && secs > 0.f) || pct >= 0.1f || secs >= 0.01f)
      p += snprintf(p, n - (p - buf), "%6.1f  %2.0f%%  %s\n", secs, pct,
                    timernamesprint[i]);
  }
}
