#ifndef __FIFO_H
#define __FIFO_H

#define FIFO_NEXT(i, s)     (((i) == ((s)-1)) ? (i)=0 : (i)++)
#define FIFO_INIT(t, h)     (t) = 0;  (h) = 0
#define FIFO_FULL(t, h, s)  ((h > t) ? ((h - t) == (s-1)) : ((t - h) == 1) )
#define FIFO_EMPTY(t, h, s) ((h == t))
#define FIFO_LEN(t, h, s)   ((h > t) ? (h - t) : (s + h - t))

#endif


