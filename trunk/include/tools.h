#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <sys/types.h>

void push_int32(unsigned char *buf, unsigned char *indice, unsigned int data);

void push_int16(unsigned char *buf, unsigned char *indice, unsigned short data);

void pop_int32(unsigned char *buf, unsigned char *indice, unsigned int *data);

void pop_int16(unsigned char *buf, unsigned char *indice, unsigned short *data);

void print_buf(unsigned char *buf, int len);

void print_date(void);
#endif
