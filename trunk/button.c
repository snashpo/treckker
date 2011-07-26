#include <stdio.h>
#include <string.h>

#include "stm32f10x_it.h"

#include "message.h"
#include "button.h"
#include "timer.h"
#include "message.h"
#include "hw_config.h"

pairing_status pairing_state = NO_PAIRING;

static uint32_t state = 0;
static uint32_t last_change = 0;

extern led_status led_state;

#define VALID_DELAY			(3 * TICK_1S)
#define DELETE_ONE_DELAY	(15 * TICK_1S)	
#define DELETE_ALL_DELAY	(25 * TICK_1S)	
#define OVER_ALL_DELAY		(60 * TICK_1S)	

void Button_Mgmt()
{
	button_status new_button_state;

	if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_SET) {
		new_button_state = BUTTON_PRESSED;		
	} else { /* if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_SET) */
		new_button_state = BUTTON_RELEASED;
	} /* if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_SET) */

	/* Status has not changed */
	if (new_button_state == BUTTON_PRESSED) {

		/* Timer expire */
		if (expire_timer(last_change, VALID_DELAY) == TRUE) {
			led_state = PENDING_TRANSMITOR_PAIRING_STATE;
			state = 1;

			if (expire_timer(last_change, DELETE_ONE_DELAY) == TRUE) {
				led_state = PENDING_ERASE_ONE_STATE;
				state = 2;

				if (expire_timer(last_change, DELETE_ALL_DELAY) == TRUE) {
					led_state = PENDING_ERASE_ALL_STATE;
					state = 3;

					/*--------------------------------------------------
					* if (expire_timer(last_change, OVER_ALL_DELAY) == TRUE) {
					* 	led_state = NORMAL_STATE;
					* 	state = 4;
					* } / * if (expire_timer(last_change, OVER_ALL_DELAY) == TRUE) * /
					*--------------------------------------------------*/

				} /* if (expire_timer(last_change, DELETE_ALL_DELAY) == TRUE) */

			} /* if (expire_timer(last_change, DELETE_ONE_DELAY) == TRUE) */

		} /* if (expire_timer(last_change, VALID_DELAY) == TRUE) */

	} else { /* if (new_button_state == BUTTON_PRESSED) */

		if (state) {
			switch (state) {
				case 1:
					pairing_state = TRANSMITORS_PAIRING;
					break;
				case 2:
					pairing_state = ERASE_ONE;
					break;
				case 3:
					pairing_state = ERASE_ALL;
					break;
				case 4:
					pairing_state = DEFAULT_PAIRING;
					break;
				default:
					printf("Button_Mgmt: led state (%d) not handled\n", led_state);
					break;
			} /* switch (led_state) */
			button_store_pairing(pairing_state);
			
			state = 0;

		} /* if (state) */

		last_change = tick_1khz();

	} /* if (new_button_state == BUTTON_PRESSED) */
}

void button_store_pairing(pairing_status status) 
{
  uint8_t msg[PAIRING_NOTIFY_LEN];
  uint8_t i;

  memset(msg, 0, PAIRING_NOTIFY_LEN);

  i = 0;
  msg[i++] = BUTTON_PAIRING_NOTIFY;
  msg[i++] = status;

  message_store(PAIRING_MSG, (char *) msg, PAIRING_NOTIFY_LEN);
	printf("(BT) Pairing code %02x, status %02x.\n",msg[0], msg[1]); 
}
 
