#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

#include "stm32f10x.h"

#define USB_DISCONNECT                      GPIOB  
#define USB_DISCONNECT_PIN                  GPIO_Pin_10
#define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOB

#define USB_POWER                           GPIOB  
#define USB_POWER_PIN                       GPIO_Pin_0

#define ADC_AIN_CHANNEL                     ADC_Channel_9

#define PAGE_SIZE  (uint16_t)0x400

void printf_init(unsigned char (*callback) (unsigned char *buffer,
                                            unsigned char num));

#endif /* __PLATFORM_CONFIG_H */
