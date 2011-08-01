#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_i2c.h"

#include "hw_config.h"
#include "platform_config.h"
#include "fifo.h"
#include "timer.h"
#include "button.h"
#include "sht1x.h"
#include "lcd.h"
#include "battery.h"
#include "buzzer.h"
#include "LSM303.h"
#include "sim18.h"

volatile uint16_t ADC_Value[ADC_DMA_SIZE] = { 
	0, 0, 0
};
#define ADC1_DR_Address    ((uint32_t) 0x4001244C)

#define USART_FIFO_SIZE   128
int32_t sys_clock_freq_stepping;

volatile uint8_t uart1_fifo[USART_FIFO_SIZE];
volatile uint16_t uart1_tail;
volatile uint16_t uart1_head;
volatile uint8_t uart2_fifo[USART_FIFO_SIZE];
volatile uint16_t uart2_tail;
volatile uint16_t uart2_head;


void Set_System(void)
{
	/* Setup the microcontroller system. Initialize the Embedded Flash Interface,
		initialize the PLL and update the SystemFrequency variable. */
	SystemInit();

	sys_clock_freq_stepping = SystemFrequency;
	SysTick_Config(sys_clock_freq_stepping / 1000);

	printf_init(USART1_Send_Buffer);

	/*Enables the clock to Backup and power interface peripherals    */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);

	/* Enable UART1 clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);

	/* Enable GPIOA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Enable GPIOB */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Enable GPIOC */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* Enable GPIOD */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	RCC_APB2PeriphResetCmd(
			RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,
			DISABLE);

	/* Configure GPIO */
	GPIO_Configuration();

	/* Configure RTC */
	RTC_Configuration();

	/* Configure USART */
	USART_Config();

	/* Setup Interrupt table */
	Interrupts_Config();

	/* Setup SHT1x	*/
	SHT1x_Config();

	/* Configure the timer */
	TIM_Configuration();

	/* Configure the ADC */
	ADC_Configuration();

	SPI2_Configuration();

	I2C_Configuration();

	SIM18_Configuration();

	/* Flash Configuration */
	FLASH_Unlock();

}

/*--------------------------------------------------
 * void Set_USBClock(void)
 * {
 *   / * USBCLK = PLLCLK / 1.5 * /
 *   RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
 *   / * Enable USB clock * /
 *   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
 * }
 *--------------------------------------------------*/

/*--------------------------------------------------
 * void Enter_LowPowerMode(void)
 * {
 *   / * Set the device state to suspend * /
 *   bDeviceState = SUSPENDED;
 * }
 *--------------------------------------------------*/

/*--------------------------------------------------
 * void Leave_LowPowerMode(void)
 * {
 *   DEVICE_INFO *pInfo = &Device_Info;
 *   
 *   / * Set the device state to the correct state * /
 *   if (pInfo->Current_Configuration != 0) {
 *     / * Device configured * /
 *     bDeviceState = CONFIGURED;
 *   } else { / * if (pInfo->Current_Configuration != 0) * /
 *     bDeviceState = ATTACHED;
 *   } / * if (pInfo->Current_Configuration != 0) * /
 * }
 *--------------------------------------------------*/

void Interrupts_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* NVIC Configuration */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the DMA1 Channel1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/*--------------------------------------------------
	 *   / * Enable the EXTI15_10 Interrupt (clock syncho / get rssi)* /
	 *   NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	 *   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 *   NVIC_Init(&NVIC_InitStructure);
	 *--------------------------------------------------*/

}

/*--------------------------------------------------
 * void USB_Interrupts_Config(void)
 * {
 *   NVIC_InitTypeDef NVIC_InitStructure;
 *   
 *   NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
 *   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 *   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 *   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 *   NVIC_Init(&NVIC_InitStructure);
 * }
 *--------------------------------------------------*/

/*--------------------------------------------------
 * void USB_Cable_Config (FunctionalState NewState)
 * {
 *   if (NewState != DISABLE) {
 *     GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
 *   } else { / * GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN); * /
 *     GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
 *   } / * GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN); * /
 * }
 *--------------------------------------------------*/

void USART_Config(void)
{
	/* USART1 default configuration */
	/* USART1 configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No Parity
		- Hardware flow control desabled
		- Receive and transmit enabled
		*/
	USART_InitTypeDef USART_InitStructure;

	// Release reset and enable clock
	USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	/* Configure the USART1 */
	USART_Init(USART1, &USART_InitStructure);
	FIFO_INIT(uart1_tail, uart1_head);
	USART_Cmd(USART1, ENABLE);

	// Release reset and enable clock
	USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = 4800;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	/* Configure the USART2 */
	USART_Init(USART2, &USART_InitStructure);

}

void USART1_Wait_Empty()
{
	while(FIFO_EMPTY(uart1_tail, uart1_head, USART_FIFO_SIZE) == FALSE);
}

void USART1_Send_Char(uint8_t data)
{
	while(FIFO_FULL(uart1_tail, uart1_head, USART_FIFO_SIZE) == TRUE);

	uart1_fifo[uart1_head] = data;
	FIFO_NEXT(uart1_head, USART_FIFO_SIZE);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

bool USART1_GetFifo(uint8_t *c)
{
	if (FIFO_EMPTY(uart1_tail, uart1_head, USART_FIFO_SIZE) == TRUE) {
		return FALSE;
	} /* if (FIFO_EMPTY(uart1_tail, uart1_head, USART1_FIFO_SIZE) == TRUE) */

	*c = uart1_fifo[uart1_tail];

	FIFO_NEXT(uart1_tail, USART_FIFO_SIZE);

	return TRUE;
}

void USART1_Istr()
{
	uint8_t c;

	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
		if (USART1_GetFifo(&c) == TRUE) {
			USART_SendData(USART1, c);
		} else { /* if (USART1_GetFifo(&c) == TRUE) */
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		} /* if (USART1_GetFifo(&c) == TRUE) */
	} /* if (USART_GetITStatus(USART1, USART_IT_TXE) == SET) */

}
void USART2_Wait_Empty()
{
	while(FIFO_EMPTY(uart2_tail, uart2_head, USART_FIFO_SIZE) == FALSE);
}

void USART2_Send_Char(uint8_t data)
{
	while(FIFO_FULL(uart2_tail, uart2_head, USART_FIFO_SIZE) == TRUE);

	uart1_fifo[uart2_head] = data;
	FIFO_NEXT(uart2_head, USART_FIFO_SIZE);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

bool USART2_GetFifo(uint8_t *c)
{
	if (FIFO_EMPTY(uart2_tail, uart2_head, USART_FIFO_SIZE) == TRUE) {
		return FALSE;
	} /* if (FIFO_EMPTY(uart1_tail, uart1_head, USART1_FIFO_SIZE) == TRUE) */

	*c = uart2_fifo[uart1_tail];

	FIFO_NEXT(uart2_tail, USART_FIFO_SIZE);

	return TRUE;
}

void USART2_Istr()
{
	uint8_t c;

	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
		if (USART2_GetFifo(&c) == TRUE) {
			USART_SendData(USART2, c);
		} else { /* if (USART2_GetFifo(&c) == TRUE) */
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		} /* if (USART1_GetFifo(&c) == TRUE) */
	} 

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		sim18_process_frame((char)USART_get_RX_char());
	} 
}


uint8_t USART1_Send_Buffer(uint8_t* data_buffer, uint8_t Nb_bytes)
{
	uint32_t i;

	for (i = 0; i < Nb_bytes; i++) {
		if (*(data_buffer + i) == '\n') {
			/* silently send a carriage return */
			USART1_Send_Char('\r');
		} else { /* if (data[i]== '\n') */
			if (*(data_buffer + i) == '\r') {
				/* silently send a newline */
				USART1_Send_Char('\n');
			} /* if (*(data_buffer + i) == '\r') */
		} /* if (*(data_buffer + i) == '\n') */
		USART1_Send_Char(*(data_buffer + i));
	} /* for (i = 0; i < Nb_bytes; i++) */

	return Nb_bytes;
}

uint8_t USART2_Send_Buffer(uint8_t* data_buffer, uint8_t Nb_bytes)
{
	uint32_t i;

	for (i = 0; i < Nb_bytes; i++) {
		USART2_Send_Char(*(data_buffer + i));
	} /* for (i = 0; i < Nb_bytes; i++) */

	return Nb_bytes;
}

/*--------------------------------------------------
 * void EXTI_Configuration(void)
 * {
 *   EXTI_InitTypeDef EXTI_GPIO;
 * 
 *   GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
 * 
 *   EXTI_GPIO.EXTI_Line = EXTI_Line12;
 *   EXTI_GPIO.EXTI_Mode = EXTI_Mode_Interrupt;
 *   EXTI_GPIO.EXTI_Trigger = EXTI_Trigger_Falling;
 *   EXTI_GPIO.EXTI_LineCmd = ENABLE;
 *   EXTI_Init(&EXTI_GPIO);
 * }
 *--------------------------------------------------*/

/*--------------------------------------------------
 * void EXTI13_Configuration(FunctionalState State)
 * {
 *   EXTI_InitTypeDef EXTI_GPIO;
 * 
 *   GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
 * 
 *   EXTI_GPIO.EXTI_Line = EXTI_Line13;
 *   EXTI_GPIO.EXTI_Mode = EXTI_Mode_Interrupt;
 *   EXTI_GPIO.EXTI_Trigger = EXTI_Trigger_Falling;
 *   EXTI_GPIO.EXTI_LineCmd = State;
 *   EXTI_Init(&EXTI_GPIO);
 * }
 *--------------------------------------------------*/

void SPI2_Configuration(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);

	// Reset SPI Interface
	SPI_I2S_DeInit(SPI2);
}

void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;

	/* Enable I2C clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C1_Speed;
	I2C_Init(I2C1, &I2C_InitStructure);

	/* I2C Peripheral Enable */
	I2C_Cmd(I2C1, ENABLE);
}

/*--------------------------------------------------
 * void EXTI14_Configuration(FunctionalState State)
 * {
 *   EXTI_InitTypeDef EXTI_GPIO;
 * 
 *   GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
 * 
 *   EXTI_GPIO.EXTI_Line = EXTI_Line14;
 *   EXTI_GPIO.EXTI_Mode = EXTI_Mode_Interrupt;
 *   EXTI_GPIO.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
 *   EXTI_GPIO.EXTI_LineCmd = State;
 *   EXTI_Init(&EXTI_GPIO);
 * }
 *--------------------------------------------------*/

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* Configure USART1 Rx (PA.10 ) as input floating */  
	GPIO_PinRemapConfig(GPIO_Remap_USART1,DISABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART1 Tx (PA.9 ) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Tx (PA.2 ) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART2 Rx (PA.3 ) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SHT1x GND */
	GPIO_InitStructure.GPIO_Pin = SHT1x_GND_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(SHT1x_PORT, &GPIO_InitStructure);
	/* SHT1x DATA */
	GPIO_InitStructure.GPIO_Pin = SHT1x_DATA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SHT1x_PORT, &GPIO_InitStructure);
	/* SHT1x CLOCK */
	GPIO_InitStructure.GPIO_Pin = SHT1x_CLOCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT1x_PORT, &GPIO_InitStructure);
	/* SHT1x VCC */
	GPIO_InitStructure.GPIO_Pin = SHT1x_VCC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT1x_PORT, &GPIO_InitStructure);

	/* Heartbit Led */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, LED_PIN);

	/* Button */
	GPIO_InitStructure.GPIO_Pin =  BUTTON_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);
	/*--------------------------------------------------
	 *   / * Button int * /
	 *   GPIO_EXTILineConfig(BUTTON_PORT, BUTTON_PIN);
	 *--------------------------------------------------*/

	/* Buzzer */
	GPIO_InitStructure.GPIO_Pin =  BUZZER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

	/* LCD */
	GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN);  
	GPIO_InitStructure.GPIO_Pin =  LCD_CS_PIN | LCD_DC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(LCD_RESET_PORT, PSU_12V_ENA_PIN | LCD_RESET_PIN);
	GPIO_InitStructure.GPIO_Pin =  PSU_12V_ENA_PIN | LCD_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_RESET_PORT, &GPIO_InitStructure);

	/* Battery */
	GPIO_InitStructure.GPIO_Pin = BATTERY_CHARG_PROGRESS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BATTERY_CHARG_PROGRESS_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BATTERY_CHARG_DONE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BATTERY_CHARG_DONE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BATTERY_POWER_GOOD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BATTERY_POWER_GOOD_PORT, &GPIO_InitStructure);

	GPIO_SetBits(BATTERY_VOLTAGE_ENABLE_PORT, BATTERY_VOLTAGE_ENABLE_PIN);  
	GPIO_InitStructure.GPIO_Pin = BATTERY_VOLTAGE_ENABLE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BATTERY_VOLTAGE_ENABLE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BATTERY_VOLTAGE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(BATTERY_VOLTAGE_PORT, &GPIO_InitStructure);

	/* Spi bus */
	/* Configure SPI2 pins: 
	 * PB.1  as DC 
	 * PB.12 as CS
	 * PB.13 as SCK,
	 * PB.14 as MISO 
	 * PB.15 as MOSI
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure I2C pins: SCL and SDA */
	GPIO_InitStructure.GPIO_Pin =  I2C1_SCL_Pin | I2C1_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(I2C1_Port, &GPIO_InitStructure);

	/*	GPS	*/
	GPIO_InitStructure.GPIO_Pin =  SIM18_ON_OFF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SIM18_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  SIM18_NRESET;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SIM18_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  SIM18_V_ANT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SIM18_ANTENA_Port, &GPIO_InitStructure);

	/*--------------------------------------------------
	 *   / * Configure USB pull-up pin * /
	 *   GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
	 * 
	 *   GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
	 *   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 *   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	 *   GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
	 *--------------------------------------------------*/

	/*--------------------------------------------------
	 *   / * Configure USB power detect pin (PC.4) * /
	 *   GPIO_InitStructure.GPIO_Pin = USB_POWER_PIN;
	 *   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	 *   GPIO_Init(USB_POWER, &GPIO_InitStructure);
	 *--------------------------------------------------*/

	/*--------------------------------------------------
	 *   // Serial Flash Connection
	 *   
	 *   / * Spi bus * /
	 * 
	 *   / * Configure :
	 *    * PA.5 as output push-pull (PCLK)
	 *    * PA.6 as input            (MISO)
	 *    * PA.7 as output push-pull (MOSI)
	 *    * PA.4 as output push-pull (CS_Serial_Flash)
	 *    * /
	 * 
	 *   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	 *   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	 *   GPIO_Init(GPIOA, &GPIO_InitStructure);
	 * 
	 *   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	 *   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	 *   GPIO_Init(GPIOA, &GPIO_InitStructure);
	 * 
	 *   GPIO_SetBits(SF_CS_PORT, SF_CS_PIN);
	 *   GPIO_InitStructure.GPIO_Pin =  SF_CS_PIN;
	 *   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 *   GPIO_Init(SF_CS_PORT, &GPIO_InitStructure);
	 *--------------------------------------------------*/
}

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	/* Enable TIM4 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Set TIM' on internal clock */
	TIM_InternalClockConfig(TIM4);

	/* Init TIM4 to 500ms period */
	TIM_TimeBaseInitStruct.TIM_Prescaler = 35999;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInitStruct.TIM_Period = 1000;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

	TIM_ARRPreloadConfig(TIM4, ENABLE);

	/* Setup Output channel 4 */
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 250;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC4Init(TIM4, &TIM_OCInitStruct);

	TIM_OC4PreloadConfig(TIM4, ENABLE);

	/* Enable TIM */
	TIM_Cmd(TIM4, ENABLE);
}

void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Configures the ADC clock */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	/* DMA1 channel1 configuration ---------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ADC_Value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_DMA_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* Enable the DMA1 Channel1 Transfer complete interrupt */
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = ADC_DMA_SIZE;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Set VrefInt for temp calibration */ 
	ADC_TempSensorVrefintCmd(ENABLE);

	/* ADC1 regular channel configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_AIN_VBAT, 1, 
			ADC_SampleTime_239Cycles5);

	ADC_RegularChannelConfig(ADC1, ADC_AIN_TEMP_VALUE, 2, 
			ADC_SampleTime_239Cycles5);

	ADC_RegularChannelConfig(ADC1, ADC_AIN_REF_VALUE, 3, 
			ADC_SampleTime_239Cycles5);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);

	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);

	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1)); 

}

uint16_t vbat_value()
{
	int32_t value;

	/* wait for 1st mesurement */
	while(ADC_Value[ADC_REF_VALUE] == 0);

	value = ADC_Value[ADC_VBAT];
	value *= 2400;
	value /= ADC_Value[ADC_REF_VALUE];

	return (int16_t) value;
}

uint16_t vpsu_value()
{
	if (GPIO_ReadInputDataBit(BATTERY_POWER_GOOD_PORT, 
				BATTERY_POWER_GOOD_PIN) == RESET) {
		return PSU_VOLTAGE;
	} /* if (GPIO_ReadInputDataBit(BATTERY_POWER_GOOD_PORT, ... */

	return PSU_NO_VOLTAGE; 
}

uint16_t vdda_value()
{
	int32_t value;

	/* wait for 1st mesurement */
	while(ADC_Value[ADC_REF_VALUE] == 0);

	value = 1200 * 0x1000;
	value /= ADC_Value[ADC_REF_VALUE];

	return (int16_t) value;
}

#define TEMP_AVG_SLOPE        4300  /* uV/C */
int16_t temp_value()
{
	int32_t value; 
	uint32_t ref_value;
	uint16_t ref_temp, ref_val;

	/* wait for 1st mesurement */
	while(ADC_Value[ADC_REF_VALUE] == 0);

	ref_value = 0;//get_temperature_offset();
	ref_temp = ref_value & 0xffff;
	ref_value >>= 16;
	ref_val = ref_value;

	value = ADC_Value[ADC_TEMP_VALUE] * 1000;
	value -= (ref_val * 1000);
	value /= TEMP_AVG_SLOPE;
	value += ref_temp;

	return value;
}


#define MIN_DELAY 2
void ndelay(uint16_t ns)
{
	while(ns--);
}

void udelay(uint16_t us)
{
	uint32_t st, ex;

	st = SysTick->VAL;

	ex = sys_clock_freq_stepping / 1000000;
	ex *= us;

	if (ex < MIN_DELAY) {
		return;
	} else { /* if (ex < MIN_DELAY) */
		ex -= MIN_DELAY;
	} /* if (ex < MIN_DELAY) */

	if (ex > st) {
		/* Wait to zero */
		while(SysTick->VAL < st);
		ex -= st;
		st = SysTick->LOAD - ex;
	} else { /* if (ex > st) */
		st -= ex;
	} /* if (ex > st) */

	while(SysTick->VAL > st);
}

void mdelay(uint16_t ms)
{
	while(ms--) {
		udelay(1000);
	} /* while(delai--) */
}

/**
 * @brief  This function allow to switch system clock frequency.
 * @param  : 0 -> SLOW (8MHz), 1 -> FAST (48MHz)
 * @retval : None
 */
void stepping_switch(bool fast_slow)
{
	__disable_irq();

	/* Set HSION bit */
	RCC->CR |= RCC_CR_HSION;

	/* Wait HSI to be ready */
	while((RCC->CR & RCC_CR_HSIRDY) == 0);

	/* Switch to HSI clock */
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;

	/* Wait HSI to be system clock */
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

	/* Turn off PLL */
	RCC->CR &= ~RCC_CR_PLLON;

	if (fast_slow) {
		/* Fast */

		sys_clock_freq_stepping = SystemFrequency;

		/* Flash 2 wait state */
		FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
		FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    

		/* PLLCLK = 8MHz * 6 = 48 MHz */
		RCC->CFGR &= (uint32_t)
			((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL6);
		/* Enable PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0);

		/* Switch to PLL clock */
		RCC->CFGR &= ~RCC_CFGR_SW;
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait PLL to be system clock */
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

		// /* Reconfigure UART1 bit rate : clk = 48MHz*/
		USART1->BRR = 0x01A1;

	} else { /* if (fast_slow) */
		/* Slow */

		sys_clock_freq_stepping = (HSE_Value * 2);

		/* Flash 0 wait state */
		FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
		FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;    

		/* PLLCLK = 8MHz * 1 = 8 MHz */
		RCC->CFGR &= (uint32_t)
			((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);

		/* Turn on PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Switch to HSE clock */
		RCC->CFGR &= ~RCC_CFGR_SW;
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait HSE to be system clock */
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

		/* Reconfigure UART1 bit rate : clk = 8MHz*/
		USART1->BRR = 0x0045;

	} /* if (fast_slow) */

	SysTick_Config( sys_clock_freq_stepping / 1000 );

	__enable_irq();
}

void cpu_reset(void) 
{
	/* Enable WWDG clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	WWDG_Enable(0x4F);

	while(1);
}


