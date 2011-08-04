#include <stdio.h>

#include "stm32f10x.h"

#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

void push_uint32(unsigned char *buf, unsigned char *indice, uint32_t data)
{
	int i;
	unsigned int mask = 0xff000000;
	for (i = 4; i > 0; i--){
		*(buf + *indice) = (unsigned char)(data & mask) >> ((i - 1) << 8) ;
		*indice += 1;
	}

/*--------------------------------------------------
 * unsigned int l;
*   l = htonl(data);
*   memcpy(buf + *indice, &l, sizeof(unsigned int));
*   *indice+= sizeof(unsigned int);
*--------------------------------------------------*/
}

void push_int16(unsigned char *buf, unsigned char *indice, uint16_t data)
{
	int i;
	unsigned int mask = 0x0000ff00;
	for (i = 2; i > 0; i--){
		*(buf + *indice) = (unsigned char)((uint32_t)data & mask) >> ((i - 1) << 8) ;
		*indice += 1;
	}

	/*--------------------------------------------------
	* unsigned short s;
	* s = htons(data);
	* memcpy(buf + *indice, &s, sizeof(unsigned short));
	* *indice+= sizeof(unsigned short);
	*--------------------------------------------------*/
}

void pop_int32(unsigned char *buf, unsigned char *indice, uint32_t *data)
{
	int i;
	*data = 0;
	for (i = 0; i < 4; i++){
		*data <<= (i << 8);
		*data |= *(buf + *indice);
		*indice += 1;
	}

/*--------------------------------------------------
*   unsigned int l;
* 
*   memcpy(&l, buf + *indice, sizeof(unsigned int));
*   *data = ntohl(l);
*   *indice+= sizeof(unsigned int);
*--------------------------------------------------*/
}

void pop_int16(unsigned char *buf, unsigned char *indice, unsigned short *data)
{
	int i;
	*data = 0;
	for (i = 0; i < 2; i++){
		*data <<= (i << 8);
		*data |= *(buf + *indice);
		*indice += 1;
	}

/*--------------------------------------------------
*   unsigned short s;
* 
*   memcpy(&s, buf + *indice, sizeof(unsigned short));
*   *data = ntohs(s);
*   *indice+= sizeof(unsigned short);
*--------------------------------------------------*/
}

void print_buf(unsigned char *buf, int len)
{
  int i;

  if (len == 0) {
    return;
  } /* if (len == 0) */

  for (i = 0; i < len; i++) {

    if ((i % 8) == 0) {
      DEBUGF("\n0x%04x: ", i);
    } /* if ((i%8) == 0) */

    DEBUGF("0x%02x ", buf[i]);
  
  } /* for (i=0; i<len; i++) */

  DEBUGF("\n");

}

uint32_t strncmp(const uint8_t *s1, const uint8_t *s2, uint32_t n){
	while( n-- ){
		if (*(s1 + n) > *(s2 + n)){
			return 1;
		}else	if (*(s1 + n) < *(s2 + n)){
			return -1;
		}
	}
	return 0;
}

char * strchr(const uint8_t *s1, const uint8_t c){
	uint16_t i = 0;
	while ( *(s1 + i) ){
		if( *(s1 + i) == c){
			return s1 + i;
		}
	}	
	return NULL;
}

uint32_t strlen(const uint8_t *s1){
	uint32_t res;
	while(*(s1 + res++));
	return res;
}

/*--------------------------------------------------
* void print_date(void)
* {
* 	char string[64];
* 	time_t now = time(NULL);
* 	strftime(string, sizeof(string), "%Y/%m/%d - %X", localtime(&now) );
* 	DEBUGF("\ndate :%s\n", string);
* 
* }
*--------------------------------------------------*/
