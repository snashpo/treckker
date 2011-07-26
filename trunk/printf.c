#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

char printf_buffer[PRINTF_BUFFER_SIZE];
static unsigned short printf_buffer_index = 0;
static unsigned char (*printf_flush_callback) (unsigned char *buffer, 
					       unsigned char num) = NULL;

static void printf_putchar(char c)
{
  printf_buffer[printf_buffer_index++] = c;

  if (printf_buffer_index == PRINTF_BUFFER_SIZE) {

    if (printf_flush_callback != NULL) {
      printf_flush_callback(printf_buffer, printf_buffer_index);
    } /* if (printf_flush_callback != NULL) */

    printf_buffer_index = 0;

  } /* if (printf_buffer_index == PRINTF_BUFFER_SIZE) */
}

static void printchar(char **str, int c)
{
  if (str) {
    **str = c;
    ++(*str);
  } else { /* if (str) */
    (void) printf_putchar(c);
  } /* if (str) */
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
  register int pc = 0, padchar = ' ';
  
  if (width > 0) {
    register int len = 0;
    register const char *ptr;
    
    for (ptr = string; *ptr; ++ptr) {
      ++len;
    } /* for (ptr = string; *ptr; ++ptr) */
    
    if (len >= width) {
      width = 0;
    } else { /* if (len >= width) */
      width -= len;
    } /* if (len >= width) */
    
    if (pad & PAD_ZERO) {
      padchar = '0';
    } /* if (pad & PAD_ZERO) */

  } /* if (width > 0) */

  if (!(pad & PAD_RIGHT)) {
    for ( ; width > 0; --width) {
      printchar (out, padchar);
      ++pc;
    } /* for ( ; width > 0; --width) */
  } /* if (!(pad & PAD_RIGHT)) */

  for ( ; *string ; ++string) {
    printchar (out, *string);
    ++pc;
  } /* for ( ; *string ; ++string) */

  for ( ; width > 0; --width) {
    printchar (out, padchar);
    ++pc;
  } /* for ( ; width > 0; --width) */
  
  return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, 
		  int width, int pad, int letbase)
{
  char print_buf[PRINT_BUF_LEN];
  register char *s;
  register int t, neg = 0, pc = 0;
  register unsigned int u = i;
  
  if (i == 0) {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return prints (out, print_buf, width, pad);
  } /* if (i == 0) */
  
  if (sg && b == 10 && i < 0) {
    neg = 1;
    u = -i;
  } /* if (sg && b == 10 && i < 0) */
  
  s = print_buf + PRINT_BUF_LEN-1;
  *s = '\0';
  
  while (u) {
    t = u % b;
    if( t >= 10 ) {
      t += letbase - '0' - 10;
    } /* if( t >= 10 ) */
    *--s = t + '0';
    u /= b;
  } /* while (u) */
  
  if (neg) {
    if( width && (pad & PAD_ZERO) ) {
      printchar (out, '-');
      ++pc;
      --width;
    } else { /* if( width && (pad & PAD_ZERO) ) */
      *--s = '-';
    } /* if( width && (pad & PAD_ZERO) ) */
  } /* if (neg) */
  
  return pc + prints (out, s, width, pad);
}

static int print(char **out, const char *format, va_list args )
{
  register int width, pad;
  register int pc = 0;
  char scr[2];
  
  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      width = pad = 0;
   
      if (*format == '\0')  {
	break; 
      } /* if (*format == '\0') */

      if (*format == '%') {
	goto out;
      } /* if (*format == '%') */

      if (*format == '-') {
	++format;
	pad = PAD_RIGHT;
      } /* if (*format == '-') */

      while (*format == '0') {
	++format;
	pad |= PAD_ZERO;
      } /* while (*format == '0') */

      for ( ; *format >= '0' && *format <= '9'; ++format) {
	width *= 10;
	width += *format - '0';
      } /* for ( ; *format >= '0' && *format <= '9'; ++format) */
 
      if ( *format == 's' ) {
	register char *s = (char *)va_arg( args, int );
	pc += prints (out, s?s:"(null)", width, pad);
	continue;
      } /* if ( *format == 's' ) */

      if ( *format == 'd' ) {
	pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
	continue;
      } /* if ( *format == 'd' ) */

      if ( *format == 'x' ) {
	pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
	continue;
      } /* if ( *format == 'x' ) */

      if ( *format == 'X' ) {
	pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
	continue;
      } /* if ( *format == 'X' ) */

      if ( *format == 'u' ) {
	pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
	continue;
      } /* if ( *format == 'u' ) */

      if ( *format == 'c' ) {
	/* char are converted to int then pushed on the stack */
	scr[0] = (char)va_arg( args, int );
	scr[1] = '\0';
	pc += prints (out, scr, width, pad);
	continue;
      } /* if ( *format == 'c' ) */
    } else { /* if (*format == '%') */
    out:
      printchar (out, *format);
      ++pc;
    } /* if (*format == '%') */
  } /* for (; *format != 0; ++format) */

  if (out) {
    **out = '\0';
  } /* if (out) */

  va_end( args );
  return pc;
}

int puts(const char *s)
{
  return printf("%s\n", s);
}

#undef putchar

int putchar(int c)
{
  return printf("%c", c);
}


int printf(const char *format, ...)
{
  va_list args;
  int n;

  va_start( args, format );
  
  n = print( 0, format, args );
 
  if (printf_flush_callback != NULL) {
    printf_flush_callback(printf_buffer, printf_buffer_index);
  } /* if (printf_flush_callback != NULL) */
  
  printf_buffer_index = 0;

  return n;
}

int sprintf(char *out, const char *format, ...)
{
  va_list args;
  
  va_start( args, format );
  return print( &out, format, args );
}

void printf_init(unsigned char (*callback) (unsigned char *buffer, 
					    unsigned char num)) 
{
  printf_flush_callback = callback;
}

void *memset(void *s, int c, uint32_t n)
{
  uint8_t *p = s;

  while ( n-- ) {
    *p++ = (uint8_t) c;
  } /* while ( n-- ) */
}


#undef memcpy

void * memcpy(void * dst, const void * src, size_t n)
{
  uint8_t *d;
  uint8_t *s;

  d = dst;
  s = src;

  if (!d || !s) {
    return NULL;
  }

  if (d < s) {

    for (; n > 0; --n) {
      *(d++) = *(s++);
    } /* for (; n > 0; --n) */

  } else { /* if (d < s) */

     for (s += n,d += n; n > 0; --n) {
      *(--d) = *(--s);
    } /* for (s += n,d += n; n > 0; --n) */

  } /* if (d < s) */

  return dst;
}

int8_t memcmp(const void *vp1, const void *vp2, size_t n)
{

  uint8_t * up1;
  uint8_t * up2;

  /* If invalid, just return zero since there is no error recovery */
  if (!vp1 || !vp2) {
    return 0;
  } /* if (!vp1 || !vp2) */

  for (up1 = (uint8_t *) vp1, up2 = (uint8_t *)vp2; n > 0; ++up1, ++up2, --n) {
    if (*up1 != *up2) {
      return ((*up1 < *up2) ? -1 : 1);
    } /* if (*up1 != *up2) */
  } /* for (up1 = vp1, up2 = vp2; n > 0; ++up1, ++up2, --n) */
  return 0;
}


