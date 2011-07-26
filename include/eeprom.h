#ifndef __EEPROM_H
#define __EEPROM_H

#include "stm32f10x.h"
#include "platform_config.h"

/* EEPROM start address in Flash */
/* EEPROM emulation start address: after 64KByte of used Flash memory */
#define EEPROM_START_ADDRESS    ((uint32_t)0x0801f800) 
                                  

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       \
  ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       \
  ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((uint16_t)0x0000)
#define PAGE1                   ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE           ((uint16_t)0x00AB)

/* Page status definitions */
/* PAGE is empty */
#define ERASED                  ((uint16_t)0xFFFF)     
/* PAGE is marked to receive data */
#define RECEIVE_DATA            ((uint16_t)0xEEEE)     

/* PAGE containing valid data */
#define VALID_PAGE              ((uint16_t)0x0000)     

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE     ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL               ((uint8_t)0x80)

/* Variables' number */
#define NumbOfVar               ((uint8_t) 3)

uint16_t EE_Init(void);
bool EE_ReadUShort(uint16_t VirtAddress, uint16_t* Data);
bool EE_WriteUShort(uint16_t VirtAddress, uint16_t Data);
bool EE_ReadShort(uint16_t VirtAddress, int16_t* Data);
bool EE_WriteShort(uint16_t VirtAddress, int16_t Data);
bool EE_ReadUChar(uint16_t VirtAddress, uint8_t* Data);
bool EE_WriteUChar(uint16_t VirtAddress, uint8_t Data);
bool EE_ReadChar(uint16_t VirtAddress, int8_t* Data);
bool EE_WriteChar(uint16_t VirtAddress, int8_t Data);
bool EE_ReadULong(uint16_t VirtAddress, uint32_t* Data);
bool EE_WriteULong(uint16_t VirtAddress, uint32_t Data);
bool EE_ReadLong(uint16_t VirtAddress, int32_t* Data);
bool EE_WriteLong(uint16_t VirtAddress, int32_t Data);
bool EE_ReadString(uint16_t VirtAddress, char* Data, uint8_t *len);
bool EE_WriteString(uint16_t VirtAddress, char* Data, uint8_t len);

#define RF1_OFFSET_L              0x0100
#define RF1_OFFSET_H              0x0101
#define CALIBRATED                0x0102

#endif 
