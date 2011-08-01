#ifndef __BATTERY_H
#define __BATTERY_H

#define BATTERY_VOLTAGE_PORT             GPIOC
#define BATTERY_VOLTAGE_PIN              GPIO_Pin_0
#define BATTERY_VOLTAGE_ADC              ADC10

#define BATTERY_VOLTAGE_ENABLE_PORT      GPIOC
#define BATTERY_VOLTAGE_ENABLE_PIN       GPIO_Pin_1

#define BATTERY_CHARG_PROGRESS_PORT      GPIOB
#define BATTERY_CHARG_PROGRESS_PIN       GPIO_Pin_6

#define BATTERY_CHARG_DONE_PORT          GPIOB
#define BATTERY_CHARG_DONE_PIN           GPIO_Pin_7

#define BATTERY_POWER_GOOD_PORT          GPIOC
#define BATTERY_POWER_GOOD_PIN           GPIO_Pin_9

void battery_init(void);
int16_t battery_voltage(void); 
void battery_manage(void);

#endif
