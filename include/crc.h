#ifndef __CRC_H
#define __CRC_H

#define OFFSET_SIZE             0
#define OFFSET_COMMAND          1
#define OFFSET_DATA             2

#define CRC_FEED 0xffff

uint16_t crc16_update(uint16_t crc, uint8_t a);
uint16_t ntohs(uint16_t s);
uint32_t ntohl(uint32_t l);
uint16_t htons(uint16_t s);
uint32_t htonl(uint32_t l);
void push_long(uint8_t *buf, uint8_t *index, uint32_t data);
void push_short(uint8_t *buf, uint8_t *index, uint16_t data);
void pop_long(uint8_t *buf, uint8_t *index, uint32_t *data);
void pop_short(uint8_t *buf, uint8_t *index, uint16_t *data);

#endif
