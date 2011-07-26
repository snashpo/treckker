#ifndef __SERIAL_FLASH_H
#define __SERIAL_FLASH_H

#define SF_WRITE_ENABLE          0x06
#define SF_WRITE_DISABLE         0x04
#define SF_READ_ID               0x9f
#define SF_READ_STATUS           0x05
#define SF_WRITE_STATUS          0x01
#define SF_READ                  0x03
#define SF_READ_FAST             0x0B
#define SF_PAGE_PROGRAM          0x02
#define SF_SECTOR_ERASE          0xd8
#define SF_BULK_ERASE            0xc7
#define SF_DEEP_POWER_DOWN       0xb9
#define SF_WAKE_UP               0xab

#define SF_CS_PORT               GPIOA 
#define SF_CS_PIN                GPIO_Pin_4

#define SF_SPI_PADDING           0xff

#define SF_SECTOR_MASK           0xFFFF8000

#define SF_WRITE_IN_PROGRESS     0x01

#define SF_DONGLE_CODE           0xdd
#define SF_DONGLE_DELTADORE_CODE 0xde
#define SF_EMETTOR_CODE          0xee

struct dongle_code_info_t {
  uint8_t  major;
  uint8_t  minor;
  uint32_t length;
  uint16_t crc;
  uint8_t fc_major;
  uint8_t fc_minor;
  uint32_t fc_length;
  uint16_t fc_crc;
} __attribute__((__packed__));

typedef struct dongle_code_info_t dongle_code_info;

struct emettor_code_info_t {
  uint8_t  major;
  uint8_t  minor;
  uint16_t length;
  uint16_t crc;
} __attribute__((__packed__));

typedef struct emettor_code_info_t emettor_code_info;

struct flash_code_info_t {
  uint8_t type;
  union {
    dongle_code_info bci;
    emettor_code_info eci;
  };
  uint16_t hd_crc;
} __attribute__((__packed__));

typedef struct flash_code_info_t flash_code_info;
 
void serial_flash_init(void);
void serial_flash_read(uint32_t address, uint8_t *buf, uint16_t len);
uint16_t serial_flash_readid(void);
uint8_t serial_flash_sector_erase(uint32_t address);
uint8_t serial_flash_erase(void);
uint8_t serial_flash_program(uint32_t address, uint8_t *buf, uint8_t len);
uint8_t serial_flash_read_status(void);
void serial_write_enable(void);
void serial_write_disable(void);
void serial_flash_download_init(void);
void serial_flash_code_info(uint8_t *buf);
uint8_t serial_flash_page_request(uint16_t page);
uint8_t serial_flash_page(uint8_t *buf, uint16_t len);
void serial_flash_wait_write(void);
void serial_flash_spi_config(void);
void cpu_reset(void);

#endif
