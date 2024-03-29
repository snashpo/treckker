#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "stm32f10x.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_gpio.h"

#include "timer.h"
#include "hw_config.h"
#include "clock_calendar.h"
#include "button.h"
#include "sht1x.h"

#include "version.h"


#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

// Remote request reset
static bool request_reset = 0;
static bool request_sleep = 0;

void reset_request()
{
  request_reset = 1;
}

void sleep_request()
{
 request_sleep = 1;
}


int main(void)
{
	uint32_t len = 1;
	tick_t timer = 0;
	/*--------------------------------------------------
	* bool clock_speed = FAST;
	*--------------------------------------------------*/

	Set_System();
	
	rtc_Init();

	SHT1x_Init();

	printf("STM32 NROSSERO (C) 2011\n");
	printf("Boussole Version %d.%d / %s @ %s\n", 
			VERSION_MAJOR, VERSION_MINOR, __DATE__, __TIME__);
	printf("Serial Number: %08x-%08x-%08x\n", 
			(unsigned int) *(__IO uint32_t*)(0x1FFFF7E8), 
			(unsigned int) *(__IO uint32_t*)(0x1FFFF7EC),
			(unsigned int) *(__IO uint32_t*)(0x1FFFF7F0));


	len = TICK_1S * 4;

	while (len) {

		mdelay(1250);

		rtc_print();
		alarm_Mgmt();

		Button_Mgmt();

		SHT1x_acquire_data();
/*--------------------------------------------------
* #ifdef DEW_POINT
* 		DEBUGF("Temp %d°C, RH %d%, dw %d°C.\n", SHT1x_get_data(0)
* 				, SHT1x_get_data(1)
* 				, SHT1x_get_data(2));
* #else		
* 		DEBUGF("Temp %d°C, RH %d%.\n", SHT1x_get_data(0)
* 				, SHT1x_get_data(1) );
* #endif
*--------------------------------------------------*/

		if( button_state ){
			LED_ON();
		}else{
			LED_OFF();
		}

		/*--------------------------------------------------
		* if (expire_timer(0, 30)) {
		* 	sleep_request();
		* }
		*--------------------------------------------------*/

		if (request_sleep == 1) {
			PWR_WakeUpPinCmd(ENABLE);
			PWR_EnterSTANDBYMode();
		}

		if (request_reset == 1){
			if(timer == 0) {
				timer = tick_1khz();
			} else if (expire_timer(timer, len)) {
				len = 0;
			} /* if ((request_reset == TRUE) && (expire_timer(timer, len))) */
		}

	} /* 	while (len != 0) */

	cpu_reset(); 

	return 0;
}

