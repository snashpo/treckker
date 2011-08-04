#ifndef __TOOLS_H__
#define __TOOLS_H__

void push_int32(unsigned char *buf, unsigned char *indice, unsigned int data);
void push_int16(unsigned char *buf, unsigned char *indice, unsigned short data);
void pop_int32(unsigned char *buf, unsigned char *indice, unsigned int *data);
void pop_int16(unsigned char *buf, unsigned char *indice, unsigned short *data);
void print_buf(unsigned char *buf, int len);
void print_date(void);
//--------------------------------------------------
// uint32_t strncmp(const uint8_t *s1, const uint8_t *s2, uint32_t n);
// char * strchr(const uint8_t *s1, const uint8_t c);
// uint32_t strlen(const uint8_t *s1);
//-------------------------------------------------- 
/*--------------------------------------------------
 * uint32_t strncmp(const char *s1, const char *s2, int n);
 *-------------------------------------------------*/
#endif
