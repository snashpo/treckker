#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"

#include "crc.h"

uint16_t crc16_update(uint16_t crc, uint8_t a)
{
  int i;

  crc ^= a;

  for (i = 0; i < 8; ++i) {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  } /* for (i = 0; i < 8; ++i) */
  
  return crc;
}

uint16_t ntohs(uint16_t s)
{
  return (((s & 0xff00) >> 8) | ((s & 0x00ff) << 8)) ;
}

uint32_t ntohl(uint32_t l)
{
  return (((l & 0xff000000) >> 24) | 
	  ((l & 0x00ff0000) >> 8)  |
	  ((l & 0x0000ff00) << 8)  |
	  ((l & 0x000000ff) << 24));
}

uint16_t htons(uint16_t s)
{
  return (((s & 0xff00) >> 8) | ((s & 0x00ff) << 8)) ;
}

uint32_t htonl(uint32_t l)
{
  return (((l & 0xff000000) >> 24) | 
	  ((l & 0x00ff0000) >> 8)  |
	  ((l & 0x0000ff00) << 8)  |
	  ((l & 0x000000ff) << 24));
}

void push_long(uint8_t *buf, uint8_t *index, uint32_t data)
{
  uint32_t l;

  l = htonl(data);
  memcpy(buf + *index, &l, sizeof(uint32_t));
  *index+= sizeof(uint32_t);
}

void push_short(uint8_t *buf, uint8_t *index, uint16_t data)
{
  uint16_t s;

  s = htons(data);
  memcpy(buf + *index, &s, sizeof(uint16_t));
  *index+= sizeof(uint16_t);
}

void pop_long(uint8_t *buf, uint8_t *index, uint32_t *data)
{
  uint32_t l;

  memcpy(&l, buf + *index, sizeof(uint32_t));
  *data = ntohl(l);
  *index+= sizeof(uint32_t);
}

void pop_short(uint8_t *buf, uint8_t *index, uint16_t *data)
{
  uint16_t s;

  memcpy(&s, buf + *index, sizeof(uint16_t));
  *data = ntohs(s);
  *index+= sizeof(uint16_t);
}
