#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_spi.h"

#include "MS5607.h"
#include "hw_config.h"

MS5607_values_t	pressure_mydata;
enum MS5607_settings{
	MS5607_SENS,
	MS5607_OFF,	
	MS5607_TCS,	
	MS5607_TCO,	
	MS5607_Tref,	
	MS5607_TEMPSENS,
	MS5607_LAST
};
static uint16_t MS5607_settings[MS5607_LAST];
#define SENS			MS5607_settings[MS5607_SENS]
#define OFF				MS5607_settings[MS5607_OFF]
#define TCS				MS5607_settings[MS5607_TCS]
#define TCO				MS5607_settings[MS5607_TCO]
#define Tref			MS5607_settings[MS5607_Tref]
#define TEMPSENS		MS5607_settings[MS5607_TEMPSENS]

void MS5607_select(void){
	GPIO_ResetBits(MS5607_CS_Port, MS5607_CS_Pin);
	ndelay(50);
}

void MS5607_unselect(void){
	ndelay(50);
	GPIO_SetBits(MS5607_CS_Port, MS5607_CS_Pin);
}

void MS5607_spi_init(void){
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
  SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_Init(SPI2, &SPI);

  // Enable SPI2
  SPI_Cmd(SPI2, ENABLE);

}

void MS5607_spi_desinit(void){
	SPI_I2S_DeInit(SPI2);
}

uint8_t MS5607_spi_Write(uint8_t data){
	SPI_I2S_SendData(SPI2, data);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
}

uint8_t MS5607_spi_Read(void){
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI_I2S_ReceiveData(SPI2);
}
uint8_t MS5607_spi_WriteRead(uint8_t data){
	SPI_I2S_SendData(SPI2, data);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI_I2S_ReceiveData(SPI2);

}


void MS5607_reset(void){

	MS5607_spi_init();
	MS5607_select();
	
	MS5607_spi_Write(MS5607_CMD_RESET);

	MS5607_unselect();
	MS5607_spi_desinit();

	mdelay(4);
}

void MS5607_get_settings(void){
	uint8_t i, index;

	MS5607_spi_init();
	MS5607_select();

	for(i = MS5607_SENS; i < MS5607_LAST; i++){
		index = MS5607_CMD_PROM_READ_SENS + (i << 1);
		MS5607_spi_Write(index);
		MS5607_settings[i] = ((uint16_t)MS5607_spi_Read()) << 8;
		MS5607_settings[i] |= MS5607_spi_Read();
	}

	MS5607_unselect();
	MS5607_spi_desinit();
}

void MS5607_adjust_value(uint32_t *temp, uint32_t *pressure){

	/*--------------------------------------------------
	* int32_t T2 = 0;
	* int32_t OFF2 = 0;
	* int32_t SENS2 = 0;
	* int32_t OFF, SENS;
	* long dT;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* if (temp < 20){
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 	int32_t val = ((*temp - 2000) * (*temp - 2000));
	* 	int32_t dT = *temp - Tref;
	* 	T2 = (dT * dT) >> 31;
	* 	OFF2 = (61 * val) >> 4;
	* 	SENS2 = 2 * val;
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 	if (temp < -15){
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* 		int32_t val = (*temp + 1500) * (*temp + 1500);
	* 		OFF2 = OFF2 + 15 * val;
	* 		SENS2 = SENS2 + 8 *val;
	* 	}
	* }
	* *temp -= T2;
	* OFF -= OFF2;
	* SENS -= SENS2;
	*--------------------------------------------------*/
}

/*--------------------------------------------------
*	Public functions 
*--------------------------------------------------*/
void MS5607_Init(void){
	pressure_mydata.lock = 0;
	pressure_mydata.temperature = 0;
	pressure_mydata.pressure = 0;

	MS5607_get_settings();
}

void MS5607_Configuration(void){

}

void MS5607_measure(void){
	int32_t temp = 0;
	uint32_t	press = 0;
	uint8_t i;

	pressure_mydata.lock = 1;
	MS5607_spi_init();

	MS5607_select();
	MS5607_spi_Write(MS5607_CMD_CONVERT_D1);
	mdelay(MS5607_CMD_CONVERT_TIME);
	for(i = 0; i < 4; i++){
		press <<= 8;
		press = MS5607_spi_Read();
	}
	MS5607_unselect();
	press &= 0x00FFFFFF;

	mdelay(MS5607_CMD_CONVERT_TIME);

	MS5607_select();
	MS5607_spi_Write(MS5607_CMD_CONVERT_D2);
	mdelay(MS5607_CMD_CONVERT_TIME);
	for(i = 0; i < 4; i++){
		temp <<= 8;
		temp = MS5607_spi_Read();
	}
	MS5607_unselect();
	temp &= 0x00FFFFFF;

	MS5607_spi_desinit();

	MS5607_adjust_value(&temp, &press);

	pressure_mydata.temperature = temp;
	pressure_mydata.pressure = press;
	pressure_mydata.lock = 0;
}
