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
#include "button.h"

#include "version.h"


#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif



int main(void)
{
	uint32_t len;
	/*--------------------------------------------------
	* bool clock_speed = FAST;
	*--------------------------------------------------*/

	Set_System();
	
	/*--------------------------------------------------
	* rtc_Init();
	*--------------------------------------------------*/

	printf("STM32 NROSSERO (C) 2011\n");
	printf("Boussole Version %d.%d / %s @ %s\n", 
			VERSION_MAJOR, VERSION_MINOR, __DATE__, __TIME__);
	printf("Serial Number: %08x-%08x-%08x\n", 
			(unsigned int) *(__IO uint32_t*)(0x1FFFF7E8), 
			(unsigned int) *(__IO uint32_t*)(0x1FFFF7EC),
			(unsigned int) *(__IO uint32_t*)(0x1FFFF7F0));

	len = 0;

	while (1) {
	
	//	PWR_EnterSTANDBYMode();
		LED_OFF();
		mdelay(1000);
		LED_ON();
		mdelay(1000);

		len++;
		printf("%d\n", len);

	} /* 	while (len != 0) */

	cpu_reset(); 

	return 0;
}

