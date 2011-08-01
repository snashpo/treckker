#include <stdio.h>
#include "stm32f10x_it.h"

#include "lcd.h"
#include "logo_acom.h"
#include "font5x8.h"
#include "crc.h"


void lcd_init(void){

  SPI_InitTypeDef  SPI;

  // Reset SPI Interface
  SPI_I2S_DeInit(SPI2);
  SPI_StructInit(&SPI);
		     
  // SPI2 configuration
  SPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI.SPI_Mode = SPI_Mode_Master;
  SPI.SPI_DataSize = SPI_DataSize_8b;
  SPI.SPI_CPOL = SPI_CPOL_High;
  SPI.SPI_CPHA = SPI_CPHA_2Edge;
  SPI.SPI_NSS = SPI_NSS_Soft;
  SPI.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_Init(SPI2, &SPI);

  // Enable SPI2
  SPI_Cmd(SPI2, ENABLE);

}

void lcd_reset(uint8_t on_off)
{
	if (on_off) {
		GPIO_ResetBits(LCD_RESET_PORT, LCD_RESET_PIN);
	} else { /* if (on_off) */
		GPIO_SetBits(LCD_RESET_PORT, LCD_RESET_PIN);
	} /* if (on_off) */
}

void lcd_psu_12v(uint8_t on_off)
{
	if (on_off) {
		GPIO_SetBits(PSU_12V_ENA_PORT, PSU_12V_ENA_PIN);
	} else { /* if (on_off) */
		GPIO_ResetBits(PSU_12V_ENA_PORT, PSU_12V_ENA_PIN);
	} /* if (on_off) */
}

void lcd_spi_start(void)
{
	lcd_init();
	GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN);
}

void lcd_spi_stop(void)
{
	GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN);
}

void lcd_data_command(uint8_t data_command)
{
	if (data_command) {
		GPIO_ResetBits(LCD_DC_PORT, LCD_DC_PIN);
	} else { /* if (data_command) */
		GPIO_SetBits(LCD_DC_PORT, LCD_DC_PIN);
	} /* if (data_command) */
}

void lcd_write_command(uint8_t value)
{
	lcd_data_command(LCD_SPI_COMMAND);

	lcd_spi_start();

	SPI_I2S_SendData(SPI2, value);

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);

	lcd_spi_stop();
}

void lcd_write_data(uint8_t value)
{
	lcd_data_command(LCD_SPI_DATA);

	lcd_spi_start();

	SPI_I2S_SendData(SPI2, value);

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);

	lcd_spi_stop();
}

void lcd_set_start_page(uint8_t d)
{
	/* Set Page Start Address for Page Addressing Mode */
	lcd_write_command(SET_START_PAGE_ADDRESS(d)); 
}

void lcd_set_start_column(uint8_t d)
{
	/* Set Lower Column Start Address for Page Addressing Mode */
	lcd_write_command(LOW_COL_START_ADDR(d)); 

	/* Set Higher Column Start Address for Page Addressing Mode */
	lcd_write_command(HIGH_COL_STAR_ADDR(d >> 4));
}

void lcd_fill_ram(uint8_t data)
{
	uint8_t page, column;

	for (page=0;page<8;page++) {

		lcd_set_start_page(page);
		lcd_set_start_column(0x00);

		for (column=0; column<132; column++) {
			lcd_write_data(data);
		} /* for (column=0; column<132; column++) */
	} /* for (page=0;page<8;page++) */

}

void lcd_clear_text(uint8_t col)
{
	uint8_t page, column;

	for (page=0;page<8;page++) {

		lcd_set_start_page(page);
		lcd_set_start_column(col);

		for (column=col; column<132; column++) {
			lcd_write_data(0x00);
		} /* for (column=col; column<132; column++) */
	} /* for (page=0;page<8;page++) */

}

void lcd_load_logo()
{
	uint8_t page, column, i, data;
	uint16_t logo;

	logo = 0;
	for (page=0;page<8;page++) {

		lcd_set_start_page(page);
		lcd_set_start_column(0);

		for (column=0; column<64; column++) {

			data = 0;
			for (i=0; i<8; i++) {
				if ((logo_acom[logo + (8*i)] & (0x1 << (column % 8))) != 0) {
					data |= (0x1 << i);
				} /* if ((logo_acom[logo + (8*i)] & (0x80 >> (column % 8))) != 0) */
			} /* for (i=0; i<8; i++) */

			lcd_write_data(data);

			if ((column % 8) == 7) {
				logo++;
			} /* if ((column % 8) == 7) */

		} /* for (column=0; column<64; column++) */

		logo += 56;
	} /* for (page=0;page<8;page++) */

}

void lcd_write_str(uint8_t lin, uint8_t col, uint8_t *data, int8_t len)
{
	int i;

	for (i=0; i<len; i++) {
		lcd_write_char(lin, col+i, data[i]);
	} /* for (i=0; i<len; i++) */

}

void lcd_write_char(uint8_t lin, uint8_t col, uint8_t data)
{
	uint8_t i;

	if (data >= 32 && data <= 127) {
		data -= 32; 
	} else { /* if (data >= 32 && data <= 127) */
		data = 0;
	} /* if (data >= 32 && data <= 127) */

	lcd_set_start_page(lin);
	lcd_set_start_column(col * 6);

	for(i = 0; i < 5; i++) {
		lcd_write_data(font5x8[(5 * data) + i]); 
	} /* for(i = 0; i < 5; i++) */

	lcd_write_data(0x00);
}


void lcd_spi_init(void)
{ 
	uint8_t i, j;

	lcd_reset(RESET_ON);

	mdelay(100);

	lcd_reset(RESET_OFF);

	/* display off (0x00/0x01) */
	lcd_write_command(SET_DISPLAY_OFF);

	/* set display clock 80 frames/sec */
	lcd_write_command(SET_DISPLAY_OSC_FREQ);
	lcd_write_command(0xa0); // 0xf0
	/* set multiplex ratio 1/64 duty (0x0f ~ 0x5f) */
	lcd_write_command(SET_MULTIPLEX_RATIO);
	lcd_write_command(0x3f);
	/* set display offset ; shift maping RAM counter */
	lcd_write_command(SET_DISPLAY_OFFSET);
	lcd_write_command(0x00);
	/* set start line ; set maping ram display start */
	lcd_write_command(SET_DISPLAY_START_LINE(0));

	/* set master config ; disable embedded dc/dc */
	lcd_write_command(MASTER_CONFIGURATION);
	lcd_write_command(0x8e);
	/* set area color */
	lcd_write_command(SET_AREA_COLOR_MODE);
	lcd_write_command(0x05);
	/* set LUT */
	lcd_write_command(SET_LOOK_UP_TABLE);
	lcd_write_command(0x3F);
	lcd_write_command(0x3F);
	lcd_write_command(0x3F);
	lcd_write_command(0x3F);
	/* set addressing mode */
	lcd_write_command(MEMORY_ADDR_MODE);
	lcd_write_command(PAGE_ADDRESSING_MODE);
	/* set segment remap */
	lcd_write_command(SET_SEGMENT_REMAP(1));
	/* set common remap */
	lcd_write_command(SET_COM_OUTPUT_SCAN_DIR_DOWN);
	/* set common config */
	lcd_write_command(SET_COM_PIN_HARD_CONF);
	lcd_write_command(0x12);
	/* set precharge period */
	lcd_write_command(SET_PRECHARGE_PERIOD);
	lcd_write_command(0xd2); // 0x11
	/* set precharge period */
	lcd_write_command(SET_PRECHARGE_PERIOD);
	lcd_write_command(0xd2);
	/* set contrast current */
	lcd_write_command(SET_CONTRAST_BANK0);
	lcd_write_command(0xee); // 0x8f
	/* set area brightness */
	lcd_write_command(SET_BRIGHTNESS_BANK0);
	lcd_write_command(0xee);
	/* set VCOMH */
	lcd_write_command(SET_VCOMH_DESELECT_LEVEL);
	lcd_write_command(0x34);
	/* set entire display */
	lcd_write_command(SET_DISPLAY_GDRAM);
	/* set inverse display */
	lcd_write_command(SET_DISPLAY_NORMAL);

	lcd_fill_ram(0x00);
	lcd_load_logo();

	for (i=0; i<8; i++) {
		for (j=0; j<10; j++) {
			lcd_write_char(i, j + 11, 32 + (i*10) + j);
		} /* for (j=0; j<10; j++) */
	} /* for(i=0; i<8; i++) */

	lcd_psu_12v(PSU_12V_ON);

	mdelay(100);

	/* display on (0x00/0x01) */
	lcd_write_command(SET_DISPLAY_FULL_BRITHNESS);
}

void lcd_off(void)
{
	lcd_write_command(SET_DISPLAY_OFF);
	lcd_psu_12v(PSU_12V_OFF);
}

void lcd_action(uint8_t *buf)
{
	uint8_t x, y, l, n;

	n = 0;

	x = buf[n++];
	y = buf[n++];
	l = buf[n++];

	if (x == 0 && l==0) {
		lcd_clear_text(y);
	} else { /* if (x == 0 && l==0) */
		lcd_write_str(x, y, buf + n, ((l + x) > LCD_H_SIZE) ? LCD_H_SIZE - x : l);
	} /* if (x == 0 && l==0) */

#if 0
	uint8_t lcdbuf[LCD_H_SIZE];
	uint8_t n;

	lcd_clear_text(0);

	memset(lcdbuf, ' ', LCD_H_SIZE);
	snprintf(lcdbuf, LCD_H_SIZE, "lcd_action");
	lcd_write_str(0, 0, lcdbuf, LCD_H_SIZE);

	memset(lcdbuf, ' ', LCD_H_SIZE);
	snprintf(lcdbuf, LCD_H_SIZE, "%x %x %x", buf[0], buf[1], buf[2]);
	lcd_write_str(1, 0, lcdbuf, LCD_H_SIZE);
#endif
}

