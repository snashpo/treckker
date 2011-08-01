#ifndef __LCD_H
#define __LCD_H

void lcd_spi_init(void);
void lcd_write_str(uint8_t lin, uint8_t col, uint8_t *data, int8_t len);
void lcd_write_char(uint8_t lin, uint8_t col, uint8_t data);
void lcd_clear_text(uint8_t col);
void lcd_off(void);

#define LOW_COL_START_ADDR(x)        (x & 0x0f)
#define HIGH_COL_STAR_ADDR(x)        ((x & 0x0f) | 0x10)
#define MEMORY_ADDR_MODE             0x20
#define SET_COLUMN_ADDRESS           0x21
#define SET_PAGE_ADDRESS             0x22
#define SET_DISPLAY_START_LINE(x)    ((x) | 0x40)
#define SET_CONTRAST_BANK0           0x81
#define SET_BRIGHTNESS_BANK0         0x82
#define SET_LOOK_UP_TABLE            0x91
#define SET_BANK_COLOR_PAGE0_1_16    0x92
#define SET_BANK_COLOR_PAGE0_17_32   0x93
#define SET_SEGMENT_REMAP(x)         ((x) | 0xa0)
#define SET_DISPLAY_GDRAM            0xa4
#define SET_DISPLAY_ON               0xa5
#define SET_DISPLAY_NORMAL           0xa6
#define SET_DISPLAY_INVERSE          0xa7
#define SET_MULTIPLEX_RATIO          0xa8
#define DIM_MODE_SETTING             0xab
#define MASTER_CONFIGURATION         0xad
#define SET_DISPLAY_DIM_BRITHNESS    0xac
#define SET_DISPLAY_OFF              0xae
#define SET_DISPLAY_FULL_BRITHNESS   0xaf
#define SET_START_PAGE_ADDRESS(x)    ((x) | 0xb0)
#define SET_COM_OUTPUT_SCAN_DIR_UP   0xc0
#define SET_COM_OUTPUT_SCAN_DIR_DOWN 0xc8
#define SET_DISPLAY_OFFSET           0xd3
#define SET_DISPLAY_CLOCK_DIVIDE     0xd5
#define SET_DISPLAY_OSC_FREQ         0xd5
#define SET_AREA_COLOR_MODE          0xd8
#define SET_LOW_POWER_DISPLAY_MODE   0xd8
#define SET_PRECHARGE_PERIOD         0xd9
#define SET_COM_PIN_HARD_CONF        0xda
#define SET_VCOMH_DESELECT_LEVEL     0xdb
#define ENTER_READ_MODIFY_WRITE      0xe0
#define LCD_NOP                      0xe3
#define EXIT_READ_MODIFY_WRITE       0xee

#define HORIZONTAL_SCROLL_RIGHT      0x26
#define HORIZONTAL_SCROLL_LEFT       0x27
#define VERTICAL_SCROLL_UP           0x29
#define VERTICAL_SCROLL_DOWN         0x2a
#define DEACTIVATE_SCROLL            0x2e
#define ACTIVATE_SCROLL              0x2f

#define SET_VERTICAL_SCROLL_AREA     0xa3

#define PAGE_ADDRESSING_MODE         0x02
#define HORIZONTAL_ADDRESSING_MODE   0x00
#define VERTICAL_ADDRESSING_MODE     0x01

#define LCD_SPI_COMMAND              0x01
#define LCD_SPI_DATA                 0x00

#define LCD_CS_PORT                  GPIOB
#define LCD_CS_PIN                   GPIO_Pin_12

#define LCD_DC_PORT                  GPIOB
#define LCD_DC_PIN                   GPIO_Pin_1

#define PSU_12V_ENA_PORT             GPIOC
#define PSU_12V_ENA_PIN              GPIO_Pin_8

#define LCD_RESET_PORT               GPIOC
#define LCD_RESET_PIN                GPIO_Pin_10

#define RESET_OFF                    0
#define RESET_ON                     1

#define PSU_12V_OFF                  0
#define PSU_12V_ON                   1

#define LCD_H_SIZE                   20

#endif
