#include <stdio.h>
#include "stm32f10x.h"

#include "battery.h"
#include "hw_config.h"


#ifdef DEBUG
#define DEBUGF(x, args...) printf(x, ##args)
#else
#define DEBUGF(x, args...)
#endif

bool bat_charge_progress, bat_charge_done, bat_power_good;

void battery_init(void) 
{
  bat_charge_progress = FALSE;
  bat_charge_done = FALSE;
  bat_power_good = FALSE;
}

void battery_manage(void) 
{
   bat_charge_progress =  
     GPIO_ReadInputDataBit(BATTERY_CHARG_PROGRESS_PORT, 
			   BATTERY_CHARG_PROGRESS_PIN) == RESET;

   bat_charge_done = 
     GPIO_ReadInputDataBit(BATTERY_CHARG_DONE_PORT, 
			   BATTERY_CHARG_DONE_PIN) == RESET;

   bat_power_good = 
     GPIO_ReadInputDataBit(BATTERY_POWER_GOOD_PORT, 
			   BATTERY_POWER_GOOD_PIN) == RESET;

    //memset(buf, ' ', LCD_H_SIZE);
    DEBUGF("Bat status C%c D%c P%c B %d mV", 
		bat_charge_progress ? 'Y' : 'n', 
		bat_charge_done ? 'Y' : 'n', 
		bat_power_good ? 'Y' : 'n',
		vbat_value());

    //lcd_write_str(7, 0, buf, LCD_H_SIZE);
}

