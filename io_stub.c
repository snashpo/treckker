#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

//#include "stm32f10x_usart.h"

#undef INTERNAL_HEAP

int _write(int fd, const char *buf, int size)
{
	USART_Send_Buffer(buf, size);

	return size;
}

int _read(int fd, const char *buf, int size)
{
	return 0;
}

int _fstat(int fd, struct stat *buf)
{
	return 0;
}

int _close(int fd)
{
	return 0;
}

off_t _lseek(int fd, off_t offset, int whence)
{
	return -1;
}

int _isatty(int fd)
{
	return 0;
}

#ifdef INTERNAL_HEAP

#ifndef HEAP_SIZE
#define HEAP_SIZE      1024 * 8
#endif

static char heap_start[HEAP_SIZE];

#else

/* 
 * start address for the .heap section. defined in linker script 
 */
extern unsigned long _sheap;

/* 
 * end address for the .heap section. defined in linker script 
 */
extern unsigned long _eheap;

#endif

static char *heap_ptr = 0;

void *_sbrk(char *p, ptrdiff_t nbytes)
{
	char *base;

#ifdef INTERNAL_HEAP
	/* Initialise heap pointer */
	if (heap_ptr == 0) {
		heap_ptr = heap_start;
	}			/* if ( heap_ptr == 0 ) */
#else
	/* Initialise heap pointer */
	if (heap_ptr == 0) {
		heap_ptr = (char *)&_sheap;
	}			/* if ( heap_ptr == 0 ) */
#endif

	base = heap_ptr;

#ifdef INTERNAL_HEAP
	if ((base + nbytes) < (heap_start + HEAP_SIZE)) {
		heap_ptr += nbytes;
		return base;
	}			/* if ((base + nbytes) < (heap_start + HEAP_SIZE)) */
#else
	if ((base + nbytes) < ((char *)&_eheap)) {
		heap_ptr += nbytes;
		return base;
	}			/* if ((base + nbytes) < _eheap) */
#endif

	errno = ENOMEM;
	return (void *)-1;
}
