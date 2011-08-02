#include <stdio.h>
#include <string.h>

#include "stm32f10x_it.h"

#include "button.h"
#include "timer.h"
#include "hw_config.h"

#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

enum button_state_n button_state = BUTTON_RELEASED;

/*--------------------------------------------------
* static uint32_t state = 0;
*--------------------------------------------------*/
static uint32_t last_change = 0;

#define VALID_DELAY			(3 * TICK_1S)
void Button_Mgmt()
{
	enum button_state_n new_button_state;

	if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_SET) {
		new_button_state = BUTTON_PRESSED;		
	} else { /* if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_SET) */
		new_button_state = BUTTON_RELEASED;
	} /* if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_SET) */

	if( new_button_state != button_state){
		/* Status has not changed */
		if (new_button_state == BUTTON_PRESSED) {
			/* Timer expire */
			if (expire_timer(last_change, VALID_DELAY) == TRUE) {
				button_state = 1; 
				DEBUGF("Button pressed detected.\n");
				last_change = tick_1khz();
			} /* if (expire_timer(last_change, VALID_DELAY) == TRUE) */
		} else { /* if (new_button_state == BUTTON_PRESSED) */
			if (button_state) {
				button_state = 0;
				DEBUGF("Button released detected.\n");
			} /* if (state) */
			last_change = tick_1khz();
		} /* if (new_button_state == BUTTON_PRESSED) */
	}
}

 
