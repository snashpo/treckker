#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"

#include "sim18.h"
#include "nmea.h"
#include "sirf.h"
#include "tools.h"
#include "timer.h"


#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif


/**************** sim18_variables ********************/
struct sim18_serial_settings_s sim18_port_config;
struct sim18_data_s gps_mydata;
uint8_t sim18_in_buf[128];
uint8_t sim18_out_buf[128];

void sim18_reset(void){
	GPIO_SetBits(SIM18_Port_AUX, SIM18_NRESET);
	mdelay(100);
	GPIO_ResetBits(SIM18_Port_AUX, SIM18_NRESET);
	mdelay(100);
}

void sim18_v_ant_enable(void){
	GPIO_SetBits(SIM18_Port, SIM18_V_ANT);
}

void sim18_v_ant_disable(void){
	GPIO_ResetBits(SIM18_Port, SIM18_V_ANT);
}

bool sim18_awake(void){
	GPIO_ReadInputDataBit(SIM18_Port, SIM18_WAKEUP);
}

void sim18_on_off_pulse(void){
	static tick_t last_pulse = 0;
	
	if (expire_timer(last_pulse, 1000)){
		GPIO_SetBits(SIM18_Port, SIM18_ON_OFF);
		mdelay(10);
		GPIO_ResetBits(SIM18_Port, SIM18_ON_OFF);
		last_pulse = tick_1khz();
	}

}

/**************** sim18_functions ********************/
void sim18_set_baudrate(uint32_t baudrate){
	USART_InitTypeDef USART_InitStructure;

	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

	// Release reset and enable clock
	USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	/* Configure the USART2 */
	USART_Init(USART2, &USART_InitStructure);

	sim18_port_config.baudrate =  baudrate;

	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

}


void sim18_switch_to_nmea(void)
{
	sim18_port_config.protocol = sim18_NMEA;

	memset(sim18_in_buf, 0, sizeof(sim18_in_buf));
	memset(sim18_out_buf, 0, sizeof(sim18_out_buf));

	nmea_init();
}

void sim18_switch_to_sirf(void)
{
	sim18_port_config.protocol = sim18_SIRF;

	memset(sim18_in_buf, 0, sizeof(sim18_in_buf));
	memset(sim18_out_buf, 0, sizeof(sim18_out_buf));

	/*--------------------------------------------------
	* sirf_init();
	*--------------------------------------------------*/

}
void sim18_Configuration(void){

	sim18_set_baudrate(sim18_4800);
	sim18_switch_to_nmea();

}

void sim18_Init(void){
	/*TODO
	 * Read last params from SD_CRAD and call a warn start.
	 * if there aren't caal for a cold start
	 */
	sim18_on_off_pulse();

	/*--------------------------------------------------
	 * if(){	
	 *--------------------------------------------------*/
	nmea_warn_restart();
	/*--------------------------------------------------
	 * }else{
	 *	nmea_hot_start();
	 * }
	 *--------------------------------------------------*/
	nmea_switch_to_sirf(sim18_115200);
	sim18_set_baudrate(sim18_115200);
	sim18_switch_to_sirf();
}

void sim18_Stop(void){
	if(sim18_port_config.protocol == sim18_NMEA){
		 nmea_stop();
	}else{ 
		/*--------------------------------------------------
		* sirf_stop();
		*--------------------------------------------------*/
	}
}

/********** Low level functions	************/

void sim18_read_data(uint8_t read_value){

	if(sim18_port_config.protocol == sim18_NMEA){

		 nmea_get_frame((char)read_value);

	}else{ 

		/*--------------------------------------------------
		* sirf_get_frame(read_value);
		* / *--------------------------------------------------
		* * sirf_parse_data();
		* *--------------------------------------------------* /
		*--------------------------------------------------*/

	}
}

void sim18_write_data( uint32_t length){

	USART2_Send_Buffer(sim18_out_buf, length);
	*sim18_out_buf = 0;
}


