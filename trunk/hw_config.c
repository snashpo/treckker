#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "hw_config.h"
#include "platform_config.h"
#include "fifo.h"
#include "timer.h"
#include "button.h"


#define USART_FIFO_SIZE   128
int32_t sys_clock_freq_stepping;

volatile uint8_t uart1_fifo[USART_FIFO_SIZE];
volatile uint16_t uart1_tail;
volatile uint16_t uart1_head;


void Set_System(void)
{
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();
  
  sys_clock_freq_stepping = SystemFrequency;
  SysTick_Config(sys_clock_freq_stepping / 1000);

  printf_init(USART_Send_Buffer);

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
/*--------------------------------------------------
*   / * TIM2 clock enable -- PWM Input Mode * /
*   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
*--------------------------------------------------*/

/*--------------------------------------------------
*   / * Enable USB_DISCONNECT GPIO clock * /
*   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);
*--------------------------------------------------*/
  
/*--------------------------------------------------
*   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
*--------------------------------------------------*/
  /* Configure GPIO */
  GPIO_Configuration();

  /* Configure RTC */
//  RTC_Configuration();

  /* Configure USART */
  USART_Config();

 /* Setup Interrupt table */
  Interrupts_Config();

  /* Configure RTC */
//  RTC_Configuration();

/*--------------------------------------------------
*   / * Configure the timer * /
*   TIM_Configuration();
*--------------------------------------------------*/

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

/*--------------------------------------------------
*   / * Enable the EXTI15_10 Interrupt (clock syncho / get rssi)* /
*   NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
*   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
*   NVIC_Init(&NVIC_InitStructure);
*--------------------------------------------------*/

/*--------------------------------------------------
*   / * Enable the SPI2 Interrupt * /
*   NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
*   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
*   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
*   NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
*   NVIC_Init(&NVIC_InitStructure);
*--------------------------------------------------*/

/*--------------------------------------------------
*   / * Enable the DMA1 Channel1 Interrupt * /
*   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
*   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
*   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
*   NVIC_Init(&NVIC_InitStructure);
*--------------------------------------------------*/

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/*--------------------------------------------------
*   / * Enable the TIM1 Interrupt (Delta Dore RX sampling)* /
*   NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
*   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
*   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
*   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
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
}

void USART_Wait_Empty()
{
  while(FIFO_EMPTY(uart1_tail, uart1_head, USART_FIFO_SIZE) == FALSE);
}

void USART_Send_Char(uint8_t data)
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


uint8_t USART_Send_Buffer(uint8_t* data_buffer, uint8_t Nb_bytes)
{
	uint32_t i;

	for (i = 0; i < Nb_bytes; i++) {
		if (*(data_buffer + i) == '\n') {
			/* silently send a carriage return */
			USART_Send_Char('\r');
		} else { /* if (data[i]== '\n') */
			if (*(data_buffer + i) == '\r') {
				/* silently send a newline */
				USART_Send_Char('\n');
			} /* if (*(data_buffer + i) == '\r') */
		} /* if (*(data_buffer + i) == '\n') */
		USART_Send_Char(*(data_buffer + i));
	} /* for (i = 0; i < Nb_bytes; i++) */

	return Nb_bytes;
}

/*--------------------------------------------------
* void SPI2_Configuration(void)
* {
*   SPI_InitTypeDef  SPI;
*   NVIC_InitTypeDef NVIC_InitStructure;
*   GPIO_InitTypeDef GPIO_InitStructure;
* 
*   / * Configure SPI2 pins: 
*    * PB.12 as NSS (Soft NSS Lock use a threshold interrupt),
*    * PB.13 as SCK,
*    * PB.14 as MISO 
*    * PB.15 as MOSI (Not used // Bidirection Data on Miso) 
*    * /
*   
*   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_13;
*   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
*   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
*   GPIO_Init(GPIOB, &GPIO_InitStructure);
* 
*   / * Enable the SPI2 Interrupt * /
*   NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
*   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
*   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
*   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
*   NVIC_Init(&NVIC_InitStructure);
* 
*   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);
*   		       
*   // Reset SPI Interface
*   SPI_I2S_DeInit(SPI2);
*   SPI_StructInit(&SPI);
* 
*   // SPI2 configuration
*   SPI.SPI_Direction = SPI_Direction_1Line_Rx;
*   SPI.SPI_Mode = SPI_Mode_Slave;
*   SPI.SPI_DataSize = SPI_DataSize_8b;
*   / * 2 modes possible, need to be checked:
*       SPI.SPI_CPOL = SPI_CPOL_High;
*       SPI.SPI_CPHA = SPI_CPHA_2Edge;
*       or 
*       SPI.SPI_CPOL = SPI_CPOL_Low;
*       SPI.SPI_CPHA = SPI_CPHA_1Edge;
*   * /
*   SPI.SPI_CPOL = SPI_CPOL_Low;
*   SPI.SPI_CPHA = SPI_CPHA_1Edge;
*   SPI.SPI_NSS = SPI_NSS_Soft;
*   SPI.SPI_FirstBit = SPI_FirstBit_MSB;
*   SPI_Init(SPI2, &SPI);
* 
*   // Enable SPI2
*   SPI_Cmd(SPI2, ENABLE);
*   SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Reset);
* }
*--------------------------------------------------*/

/*--------------------------------------------------
* void SPI2_Unconfiguration(void)
* {
*   SPI_Cmd(SPI2, DISABLE);
*   SPI_I2S_DeInit(SPI2);
* }
*--------------------------------------------------*/


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

/*--------------------------------------------------
* void DIO_Configuration(bool in_out)
* {
*   GPIO_InitTypeDef GPIO_InitStructure;
* 
*   / * Configure SPI2 pins: 
*    * PB.13 as SCK,
*    * PB.14 as MISO  in / out 
*    * /
*   GPIO_InitStructure.GPIO_Pin =  DCLK_PIN;
*   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
*   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
*   GPIO_Init(DCLK_PORT, &GPIO_InitStructure);
* 
*   GPIO_InitStructure.GPIO_Pin =  DIO_PIN;
*   if (in_out) {
*     GPIO_ResetBits(DIO_PORT, DIO_PIN);
*     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
*   } / * if (in_out) * /
*   GPIO_Init(DIO_PORT, &GPIO_InitStructure);
* 
* }
*--------------------------------------------------*/

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

/*--------------------------------------------------
*   / * Configure USB pull-up pin * /
*   GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
* 
*   GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
*   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
*   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
*   GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
*--------------------------------------------------*/

	 GPIO_PinRemapConfig(GPIO_Remap_USART1,DISABLE);

  /* Configure USART1 Rx (PA.10 ) as input floating */  
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART1 Tx (PA.9 ) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

/*--------------------------------------------------
*   / * Configure USB power detect pin (PC.4) * /
*   GPIO_InitStructure.GPIO_Pin = USB_POWER_PIN;
*   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
*   GPIO_Init(USB_POWER, &GPIO_InitStructure);
*--------------------------------------------------*/

  /* Heartbit Led */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, LED_PIN);


     
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

/*--------------------------------------------------
*   / * Bat Manager * /
*   GPIO_InitStructure.GPIO_Pin =  CHARGE_ON_PIN;
*   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
*   GPIO_Init(CHARGE_ON_PORT, &GPIO_InitStructure);
* 
*   GPIO_InitStructure.GPIO_Pin =  CHARGE_DONE_PIN;
*   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
*   GPIO_Init(CHARGE_DONE_PORT, &GPIO_InitStructure);
*--------------------------------------------------*/

  /* Button */
  GPIO_InitStructure.GPIO_Pin =  BUTTON_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);

/*--------------------------------------------------
*   / * Button int * /
*   GPIO_EXTILineConfig(BUTTON_PORT, BUTTON_PIN);
*--------------------------------------------------*/
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

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC->CFGR &= (uint32_t)
      ((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Switch to PLL clock */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait PLL to be system clock */
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    /* Reconfigure UART1 bit rate */
    USART1->BRR = 0x0271;

  } else { /* if (fast_slow) */
    /* Slow */

	  sys_clock_freq_stepping = (HSE_Value * 2);

  /* Flash 0 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;    

	/* PLLCLK = 8MHz * 2 = 16 MHz */
    RCC->CFGR &= (uint32_t)
      ((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL2));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);

    /* Turn on PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Switch to HSE clock */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait HSE to be system clock */
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

   /* Reconfigure UART1 bit rate */
    USART1->BRR = 0x008B;

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


