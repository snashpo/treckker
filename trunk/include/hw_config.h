#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "platform_config.h"

#define MASS_MEMORY_START     0x04002000
#define BULK_MAX_PACKET_SIZE  0x00000040

#define CHARGE_DONE_PORT        GPIOC
#define CHARGE_DONE_PIN         GPIO_Pin_15

#define CHARGE_ON_PORT          GPIOC
#define CHARGE_ON_PIN           GPIO_Pin_14

#define LED_PORT            GPIOC
#define LED_PIN             GPIO_Pin_12

#define LED_ON()				\
  GPIO_ResetBits(LED_PORT, LED_PIN)

#define LED_OFF()				\
  GPIO_SetBits(LED_PORT, LED_PIN)

#define LED_TOGGLE()			\
		(GPIO_ReadInputDataBit(LED_PORT, LED_PIN)?LED_ON():LED_OFF())

#define BUTTON_PORT            GPIOA
#define BUTTON_PIN             GPIO_Pin_0

#define ADC_VBAT              0
#define ADC_TEMP_VALUE        1
#define ADC_REF_VALUE         2

#define ADC_DMA_SIZE          3

#define ADC_NUM_CHANNELS                    ADC_DMA_SIZE
#define ADC_AIN_VBAT                        ADC_Channel_10
#define ADC_AIN_TEMP_VALUE                  ADC_Channel_16
#define ADC_AIN_REF_VALUE                   ADC_Channel_17
#define PSU_VOLTAGE           5000
#define PSU_NO_VOLTAGE           0
enum clock_speed_n{
	SLOW = 0, 
	FAST, 
};

extern int32_t sys_clock_freq_stepping;


void Set_System(void);
void USART_Config(void);
void USART_Send_Char(uint8_t data);
uint8_t USART1_Send_Buffer(uint8_t* data_buffer, uint8_t Nb_bytes);
uint8_t USART2_Send_Buffer(uint8_t* data_buffer, uint8_t Nb_bytes);
void USART1_Istr(void);
void USART2_Istr(void);
void GPIO_Configuration(void);
void Get_SerialNum(void);
void TIM_Configuration(void);
void ADC_Configuration(void);
void I2C_Configuration(void);
void SPI2_Configuration(void);

//--------------------------------------------------
// void SPI2_Unconfiguration(void);
// void EXTI_Configuration(void);
// void DIO_Configuration(bool in_out);
// void EXTI13_Configuration(FunctionalState State);
// void EXTI14_Configuration(FunctionalState State);
//-------------------------------------------------- 
void Interrupts_Config(void);
void mdelay(uint16_t ms);
void udelay(uint16_t us);
void ndelay(uint16_t ns);
void stepping_switch(bool fast_slow);
void cpu_reset(void);

#endif  /*__HW_CONFIG_H*/

