#ifndef __TIMER_H
#define __TIMER_H

enum {
  /* Number of ticks in a millisecond */
  TICK_1MS = 1,
  /* Number of ticks in a second */
  TICK_1S = 1000
};

char expire_timer(uint32_t last, uint32_t expire);
uint32_t tick_1khz(void);
void tick_increment(void);

typedef uint32_t                        tick_t;

#endif
