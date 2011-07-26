#include <stdio.h>
#include "stm32f10x.h"

#include "timer.h"

#undef DEBUG

#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif


volatile uint32_t timer_1khz = 0;

char expire_timer(uint32_t last, uint32_t expire)
{
  uint32_t t;

  t = timer_1khz;

  DEBUGF("expire %d %d %d\n", t, last, expire);

  if ((last + expire) > last) {
    return (t > (last + expire));
  } else { /* if ((last + expire) > last) */
    return (t > (last + expire) && (t < last));
  } /* if ((last + expire) > last) */
  
}

uint32_t tick_1khz(void) 
{
  return timer_1khz;
}

void tick_increment()
{
  timer_1khz++;
}
