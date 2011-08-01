#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_it.h"

#include "sim18.h"
#include "nmea.h"
#include "sirf.h"
#include "tools.h"


#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif


/**************** sim18_variables ********************/
struct sim18_serial_settings_s sim18_port_config;

struct sim18_data_s mydata;
/*--------------------------------------------------
* static uint8_t sim18_in_buf[256];
* static uint8_t sim18_out_buf[256];
* uint8_t * ptr_in_buf, * ptr_out_buf;
*--------------------------------------------------*/

/**************** sim18_functions ********************/
/*--------------------------------------------------
* struct sim18_data_s sim18_get_data(void){
* 
* }
*--------------------------------------------------*/
void sim18_Init(void){
/*TODO
 * Read last params from SD_CRAD and call a warn start.
 * if there aren't caal for a cold start
 */
	sim18_port_config.protocol = sim18_NMEA;
	sim18_port_config.baudrate = sim18_4800;

	/*--------------------------------------------------
	* ptr_in_buf = sim18_in_buf;
	* ptr_out_buf = sim18_out_buf;
	*--------------------------------------------------*/

/*--------------------------------------------------
* if(){	
*--------------------------------------------------*/
	nmea_warn_restart();
/*--------------------------------------------------
* }
* *--------------------------------------------------*/

	nmea_switch_to_sirf();
	sirf_init();
}

static void sim18_set_baudrate(uint32_t baudrate){
	USART_InitTypeDef USART_InitStructure;

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
}

/********** Low level functions	************/



/********** high level functions	************/

int sim18_process_frame(char data){

	if(sim18_port_config.protocol == sim18_NMEA){

		if(! nmea_get_frame(data)){
			print_date();
			nmea_parse_data((char*)data);
		}

	}else{ 

		if(! sirf_get_frame(data)){
			print_date();
			sirf_parse_data((uint8_t*)data);
		}

	}
	return 0;
}



