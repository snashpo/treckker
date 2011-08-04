#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"

#include "sim18.h"
#include "nmea.h"
#include "sirf.h"
#include "tools.h"
#include "timer.h"
#include "hw_config.h"


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



static void sim18_v_ant_enable(void){
	GPIO_SetBits(SIM18_Port, SIM18_V_ANT);
}

static void sim18_v_ant_disable(void){
	GPIO_ResetBits(SIM18_Port, SIM18_V_ANT);
}

static void sim18_on_off_pulse(void){
	GPIO_SetBits(SIM18_Port, SIM18_ON_OFF);
	mdelay(10);
	GPIO_ResetBits(SIM18_Port, SIM18_ON_OFF);
}

static void sim18_enable_int(void){
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

static void sim18_disable_int(void){
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

void sim18_reset(void){
	GPIO_SetBits(SIM18_Port_AUX, SIM18_NRESET);
	mdelay(100);
	GPIO_ResetBits(SIM18_Port_AUX, SIM18_NRESET);
	mdelay(100);
}

bool sim18_awake(void){
	GPIO_ReadInputDataBit(SIM18_Port, SIM18_WAKEUP);
}

void sim18_set_baudrate(enum sim18_BAUDRATE baudrate){
	USART_InitTypeDef USART_InitStructure;

	sim18_disable_int();

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
	sim18_disable_int();
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
	sirf_init();
}

void sim18_Configuration(void){

	sim18_set_baudrate(sim18_4800);
	sim18_switch_to_nmea();
}

/*--------------------------------------------------
* void sim18_timer_init(uint16_t delay){
*   TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
* 
*   / * Enable TIM3 clock * /
*   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
*   
*   / * Set TIM3 on internal clock * /
*   TIM_InternalClockConfig(TIM3);
* 
*   / * Init TIM3 to Xms period * /
*   TIM_TimeBaseInitStruct.TIM_Prescaler = delay;
*   TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Down;
*   TIM_TimeBaseInitStruct.TIM_Period = 1;
*   TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
* 
*   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
* }
*--------------------------------------------------*/

void sim18_Init(void){
	/*TODO
	 * Read last params from SD_CRAD and call a warn start.
	 * if there aren't caal for a cold start
	 */
	sim18_on_off_pulse();
	mdelay(100);
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
		sirf_stop();
	}
}

void sim18_read_data(uint8_t read_value){
	if(sim18_port_config.protocol == sim18_NMEA){
		 nmea_get_frame((char)read_value);
	}else{ 
		sirf_get_frame(read_value);
	}
}

void sim18_write_data( uint32_t length){
	USART2_Send_Buffer(sim18_out_buf, length);
	*sim18_out_buf = 0;
}

void sim18_start_measure(void){
	sim18_v_ant_enable();
	sim18_on_off_pulse();
	*sim18_in_buf = 0;
	*sim18_out_buf = 0;
	sim18_enable_int();
}


void sim18_sleep(void){
	/*--------------------------------------------------
	* TIM_Cmd(TIM3, DISABLE);
	*--------------------------------------------------*/
	sim18_disable_int();
	sim18_on_off_pulse();
	sim18_v_ant_disable();
}

/*--------------------------------------------------
* void sim18_timer_istr(void){
* 	sim18_sleep();
* 	TIM_Cmd(TIM3, DISABLE);
* }
*--------------------------------------------------*/


