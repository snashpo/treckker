#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <time.h>

#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

void push_int32(unsigned char *buf, unsigned char *indice, unsigned int data)
{
  unsigned int l;

  l = htonl(data);
  memcpy(buf + *indice, &l, sizeof(unsigned int));
  *indice+= sizeof(unsigned int);
}

void push_int16(unsigned char *buf, unsigned char *indice, unsigned short data)
{
  unsigned short s;

  s = htons(data);
  memcpy(buf + *indice, &s, sizeof(unsigned short));
  *indice+= sizeof(unsigned short);
}

void pop_int32(unsigned char *buf, unsigned char *indice, unsigned int *data)
{
  unsigned int l;

  memcpy(&l, buf + *indice, sizeof(unsigned int));
  *data = ntohl(l);
  *indice+= sizeof(unsigned int);
}

void pop_int16(unsigned char *buf, unsigned char *indice, unsigned short *data)
{
  unsigned short s;

  memcpy(&s, buf + *indice, sizeof(unsigned short));
  *data = ntohs(s);
  *indice+= sizeof(unsigned short);
}

void print_buf(unsigned char *buf, int len)
{
  int i;

  if (len == 0) {
    return;
  } /* if (len == 0) */

  for (i=0; i<len; i++) {

    if ((i%8) == 0) {
      DEBUGF("\n0x%04x: ", i);
    } /* if ((i%8) == 0) */

    DEBUGF("0x%02x ", buf[i]);
  
  } /* for (i=0; i<len; i++) */

  DEBUGF("\n");

}

void print_date(void)
{
	char string[64];
	time_t now = time(NULL);
	strftime(string, sizeof(string), "%Y/%m/%d - %X", localtime(&now) );
	DEBUGF("\ndate :%s\n", string);

}
