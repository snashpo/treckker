#include "stm32f10x.h"

/*
 * Uncomment the line corresponding to the desired System clock (SYSCLK)
 * frequency (after reset the HSI is used as SYSCLK source) 
 */
//#define SYSCLK_FREQ_HSE    HSE_Value
//#define SYSCLK_FREQ_20MHz  20000000
//#define SYSCLK_FREQ_36MHz  36000000
//#define SYSCLK_FREQ_48MHz  48000000
//#define SYSCLK_FREQ_56MHz  56000000
#define SYSCLK_FREQ_72MHz  72000000

/*
 * Uncomment the  following line if you need to use external SRAM mounted
 * on STM3210E-EVAL board (STM32 High density devices) as data memory  
 */ 
#ifdef STM32F10X_HD
/* #define DATA_IN_ExtSRAM */
#endif /* STM32F10X_HD */

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
#ifdef SYSCLK_FREQ_HSE
/* System Clock Frequency (Core Clock) */
const uint32_t SystemFrequency					 = SYSCLK_FREQ_HSE;        
/* System clock                        */
const uint32_t SystemFrequency_SysClk         = SYSCLK_FREQ_HSE;        
/* AHB System bus speed                */
const uint32_t SystemFrequency_AHBClk         = SYSCLK_FREQ_HSE;        
/* APB Peripheral bus 1 (low)  speed   */
const uint32_t SystemFrequency_APB1Clk        = SYSCLK_FREQ_HSE;        
/* APB Peripheral bus 2 (high) speed   */
const uint32_t SystemFrequency_APB2Clk        = SYSCLK_FREQ_HSE;      

#elif defined SYSCLK_FREQ_20MHz

/* System Clock Frequency (Core Clock) */
const uint32_t SystemFrequency					 = SYSCLK_FREQ_20MHz;      
/* System clock                        */
const uint32_t SystemFrequency_SysClk         = SYSCLK_FREQ_20MHz;      
/* AHB System bus speed                */
const uint32_t SystemFrequency_AHBClk         = SYSCLK_FREQ_20MHz;      
/* APB Peripheral bus 1 (low)  speed   */
const uint32_t SystemFrequency_APB1Clk        = SYSCLK_FREQ_20MHz;      
/* APB Peripheral bus 2 (high) speed   */
const uint32_t SystemFrequency_APB2Clk        = SYSCLK_FREQ_20MHz;   

#elif defined SYSCLK_FREQ_36MHz

/* System Clock Frequency (Core Clock) */
const uint32_t SystemFrequency					 = SYSCLK_FREQ_36MHz;      
/* System clock                        */
const uint32_t SystemFrequency_SysClk         = SYSCLK_FREQ_36MHz;      
/* AHB System bus speed                */
const uint32_t SystemFrequency_AHBClk         = SYSCLK_FREQ_36MHz;      
/* APB Peripheral bus 1 (low)  speed   */
const uint32_t SystemFrequency_APB1Clk        = SYSCLK_FREQ_36MHz;      
/* APB Peripheral bus 2 (high) speed   */
const uint32_t SystemFrequency_APB2Clk        = SYSCLK_FREQ_36MHz;   

#elif defined SYSCLK_FREQ_48MHz

/* System Clock Frequency (Core Clock) */
const uint32_t SystemFrequency					 = SYSCLK_FREQ_48MHz;      
/* System clock                        */
const uint32_t SystemFrequency_SysClk			 = SYSCLK_FREQ_48MHz;      
/* AHB System bus speed                */
const uint32_t SystemFrequency_AHBClk         = SYSCLK_FREQ_48MHz;      
/* APB Peripheral bus 1 (low)  speed   */
const uint32_t SystemFrequency_APB1Clk        = (SYSCLK_FREQ_48MHz/2);  
/* APB Peripheral bus 2 (high) speed   */
const uint32_t SystemFrequency_APB2Clk			 = SYSCLK_FREQ_48MHz; 

#elif defined SYSCLK_FREQ_56MHz

/* System Clock Frequency (Core Clock) */
const uint32_t SystemFrequency					 = SYSCLK_FREQ_56MHz;      
/* System clock                        */
const uint32_t SystemFrequency_SysClk         = SYSCLK_FREQ_56MHz;      
/* AHB System bus speed                */
const uint32_t SystemFrequency_AHBClk         = SYSCLK_FREQ_56MHz;      
/* APB Peripheral bus 1 (low)  speed   */
const uint32_t SystemFrequency_APB1Clk        = (SYSCLK_FREQ_56MHz/2);  
/* APB Peripheral bus 2 (high) speed   */  
const uint32_t SystemFrequency_APB2Clk        = SYSCLK_FREQ_56MHz;  

#elif defined SYSCLK_FREQ_72MHz

/* System Clock Frequency (Core Clock) */
const uint32_t SystemFrequency					 = SYSCLK_FREQ_72MHz;      
/* System clock                        */
const uint32_t SystemFrequency_SysClk         = SYSCLK_FREQ_72MHz;      
/* AHB System bus speed                */
const uint32_t SystemFrequency_AHBClk         = SYSCLK_FREQ_72MHz;      
/* APB Peripheral bus 1 (low)  speed   */
const uint32_t SystemFrequency_APB1Clk        = (SYSCLK_FREQ_72MHz/2);  
/* APB Peripheral bus 2 (high) speed   */
const uint32_t SystemFrequency_APB2Clk        = SYSCLK_FREQ_72MHz;      

/* HSI Selected as System Clock source */
#else 

/* System Clock Frequency (Core Clock) */
const uint32_t SystemFrequency					 = HSI_Value;                
/* System clock                        */
const uint32_t SystemFrequency_SysClk         = HSI_Value;                
/* AHB System bus speed                */
const uint32_t SystemFrequency_AHBClk         = HSI_Value;                
/* APB Peripheral bus 1 (low)  speed   */
const uint32_t SystemFrequency_APB1Clk        = HSI_Value;                
/* APB Peripheral bus 2 (high) speed   */
const uint32_t SystemFrequency_APB2Clk        = HSI_Value;                
#endif

static void SetSysClock(void);

#ifdef SYSCLK_FREQ_HSE
  static void SetSysClockToHSE(void);
#elif defined SYSCLK_FREQ_20MHz
  static void SetSysClockTo20(void);
#elif defined SYSCLK_FREQ_36MHz
  static void SetSysClockTo36(void);
#elif defined SYSCLK_FREQ_48MHz
  static void SetSysClockTo48(void);
#elif defined SYSCLK_FREQ_56MHz
  static void SetSysClockTo56(void);  
#elif defined SYSCLK_FREQ_72MHz
  static void SetSysClockTo72(void);
#endif

/*
 * Setup the microcontroller system
 *        Initialize the Embedded Flash Interface,  
 *  initialize the PLL and update th SystemFrequency variable
 * @param None.
 * @arg None.
 * @note : This function should be used only after reset.
 * @retval value: None.
 */
void SystemInit (void)
{

  /* RCC system reset(for debug purpose) */
	RCC_DeInit();

  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Wait till HSI is ready */
	while((RCC->CR & (uint32_t)RCC_CR_HSIRDY) != 0x0000002);

	/*--------------------------------------------------
	* / * Select HSI as system clock source * /
	* RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	* RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;    
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* / * Wait till HSI is used as system clock source * /
	* while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x00);
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* / * enable Prefetch Buffer * /
	* FLASH->ACR |= (uint32_t)(FLASH_ACR_PRFTBE);
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* / * Flash 2 wait state * /
	* FLASH->ACR &= (uint32_t)((uint32_t)~(FLASH_ACR_LATENCY));
	* FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
	*--------------------------------------------------*/

  /* Reset SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], 
   * ADCPRE[1:0] and MCO[2:0] bits 
   */
  RCC->CFGR &= (uint32_t)0xF8FF0000;  

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

	/* Disable PLL */
	RCC->CR &= (uint32_t)~(RCC_CR_PLLON);

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x009F0000;

  SetSysClock();

}


/*
 * Configures the System clock frequency, HCLK, PCLK2 and PCLK1 
 *        prescalers.
 * @param None.
 * @arg None.
 * @note : None.
 * @retval value: None.
 */
static void SetSysClock(void)
{
#ifdef SYSCLK_FREQ_HSE
  SetSysClockToHSE();
#elif defined SYSCLK_FREQ_20MHz
  SetSysClockTo20();
#elif defined SYSCLK_FREQ_36MHz
  SetSysClockTo36();
#elif defined SYSCLK_FREQ_48MHz
  SetSysClockTo48();
#elif defined SYSCLK_FREQ_56MHz
  SetSysClockTo56();  
#elif defined SYSCLK_FREQ_72MHz
  SetSysClockTo72();
#endif
 
 /* 
  * If none of the define above is enabled, the HSI is used as System clock
  * source (default after reset) 
  */ 
}

/*
 * Setup the external memory controller. Called in startup_stm32f10x.s 
 *        before jump to __main
 * @param None.
 * @arg None.
 * @note : None.
 * @retval value: None.
  */ 
#ifdef DATA_IN_ExtSRAM
/*
 * Setup the external memory controller. 
 *        Called in startup_stm32f10x_xx.s/.c before jump to main.
 * 	   This function configures the external SRAM mounted on STM3210E-EVAL
 *      board (STM32 High density devices). This SRAM will be used as program
 *      data memory (including heap and stack).
 * @param None.
 * @arg None.
 * @note : None.
 * @retval value: None.
 */ 
void SystemInit_ExtMemCtl(void) 
{
  /* 
   * FSMC Bank1 NOR/SRAM3 is used for the STM3210E-EVAL, if another Bank is 
   * required, then adjust the Register Addresses 
   */

  /* Enable FSMC clock */
  RCC->AHBENR = 0x00000114;
  
  /* Enable GPIOD, GPIOE, GPIOF and GPIOG clocks */  
  RCC->APB2ENR = 0x000001E0;
  
  /* --------------  SRAM Data lines, NOE and NWE configuration --------------*/
  /*---------------  SRAM Address lines configuration ------------------------*/
  /*---------------  NOE and NWE configuration -------------------------------*/
  /*---------------  NE3 configuration ---------------------------------------*/
  /*---------------  NBL0, NBL1 configuration --------------------------------*/
  
  GPIOD->CRL = 0x44BB44BB;  
  GPIOD->CRH = 0xBBBBBBBB;

  GPIOE->CRL = 0xB44444BB;  
  GPIOE->CRH = 0xBBBBBBBB;

  GPIOF->CRL = 0x44BBBBBB;  
  GPIOF->CRH = 0xBBBB4444;

  GPIOG->CRL = 0x44BBBBBB;  
  GPIOG->CRH = 0x44444B44;
   
  /*---------------  FSMC Configuration --------------------------------------*/
  /*---------------  Enable FSMC Bank1_SRAM Bank -----------------------------*/
  
  FSMC_Bank1->BTCR[4] = 0x00001011;
  FSMC_Bank1->BTCR[5] = 0x00000200;
}
#endif /* DATA_IN_ExtSRAM */



#ifdef SYSCLK_FREQ_HSE
/*
 * Selects HSE as System clock source and configure HCLK, PCLK2
 *        and PCLK1 prescalers.
 * @param None.
 * @arg None.
 * @note : This function should be used only after reset.
 * @retval value: None.
 */
static void SetSysClockToHSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
    HSEStatus = (uint32_t)0x01;
  } else { /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */
    HSEStatus = (uint32_t)0x00;
  } /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */
 
  if (HSEStatus == (uint32_t)0x01) {

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 0 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
    
    /* Select HSE as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;    

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x04);

  } else {  /* if (HSEStatus == (uint32_t)0x01) */
    /*
     * If HSE fails to start-up, the application will have wrong clock 
     * configuration. User can add here some code to deal with this error 
     */    

    /* Go to infinite loop */
    while (1);
  } /* if (HSEStatus == (uint32_t)0x01) */

}

#elif defined SYSCLK_FREQ_20MHz

/*
 * Sets System clock frequency to 20MHz and configure HCLK, PCLK2 
 *        and PCLK1 prescalers.
 * @param None.
 * @arg None.
 * @note : This function should be used only after reset.
 * @retval value: None.
 */
static void SetSysClockTo20(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
    HSEStatus = (uint32_t)0x01;
  } else { /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */
    HSEStatus = (uint32_t)0x00;
  } /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */

  if (HSEStatus == (uint32_t)0x01) {

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 0 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
    
    /* PLLCLK = (8MHz / 2) * 5 = 20 MHz */
    RCC->CFGR &= (uint32_t)
      ((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= 
      (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL5);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);

  } else { /* if (HSEStatus == (uint32_t)0x01) */
    /*
     * If HSE fails to start-up, the application will have wrong clock 
     * configuration. User can add here some code to deal with this error 
     */    

    /* Go to infinite loop */
    while (1);
  } /* if (HSEStatus == (uint32_t)0x01) */
}

#elif defined SYSCLK_FREQ_36MHz

/*
 * Sets System clock frequency to 36MHz and configure HCLK, PCLK2 
 *        and PCLK1 prescalers. 
 * @param None.
 * @arg None.
 * @note : This function should be used only after reset.
 * @retval value: None.
 */
static void SetSysClockTo36(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));
  
  if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
    HSEStatus = (uint32_t)0x01;
  } else { /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */
    HSEStatus = (uint32_t)0x00;
  } /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */

  if (HSEStatus == (uint32_t)0x01) {

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 1 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
    
    /* PLLCLK = (8MHz / 2) * 9 = 36 MHz */
    RCC->CFGR &= (uint32_t)
      ((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= 
      (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL9);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);

  } else { /* if (HSEStatus == (uint32_t)0x01) */
    /* 
     * If HSE fails to start-up, the application will have wrong clock 
     * configuration. User can add here some code to deal with this error 
     */    

    /* Go to infinite loop */
    while (1);

  } /* if (HSEStatus == (uint32_t)0x01) */
}

#elif defined SYSCLK_FREQ_48MHz

/*
 * Sets System clock frequency to 48MHz and configure HCLK, PCLK2 
 *        and PCLK1 prescalers. 
 * @param None.
 * @arg None.
 * @note : This function should be used only after reset.
 * @retval value: None.
  */
static void SetSysClockTo48(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
    HSEStatus = (uint32_t)0x01;
  } else { /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */
    HSEStatus = (uint32_t)0x00;
  } /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */

  if (HSEStatus == (uint32_t)0x01) {

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 1 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
    /* PLLCLK = 8MHz * 6 = 48 MHz */
    RCC->CFGR &= (uint32_t)
      ((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL6);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);

  } else { /* if (HSEStatus == (uint32_t)0x01) */
    /* 
     * If HSE fails to start-up, the application will have wrong clock 
     * configuration. User can add here some code to deal with this error 
     */    

    /* Go to infinite loop */
    while (1);
  } /* if (HSEStatus == (uint32_t)0x01) */
}

#elif defined SYSCLK_FREQ_56MHz

/*
 * Sets System clock frequency to 56MHz and configure HCLK, PCLK2 
 *        and PCLK1 prescalers. 
 * @param None.
 * @arg None.
 * @note : This function should be used only after reset.
 * @retval value: None.
 */
static void SetSysClockTo56(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
  
  /* Wait till HSE is ready and if Time out is reached exit */
  do {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
    HSEStatus = (uint32_t)0x01;
  } else { /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */
    HSEStatus = (uint32_t)0x00;
  } /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */

  if (HSEStatus == (uint32_t)0x01) {

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 1 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
    /* PLLCLK = 8MHz * 7 = 56 MHz */
    RCC->CFGR &= (uint32_t)
      ((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL7);
    
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);

  } else { /* if (HSEStatus == (uint32_t)0x01) */

    /* 
     * If HSE fails to start-up, the application will have wrong clock 
     * configuration. User can add here some code to deal with this error
     */

    /* Go to infinite loop */
    while (1);

  } /* if (HSEStatus == (uint32_t)0x01) */

}

#elif defined SYSCLK_FREQ_72MHz

/*
 * Sets System clock frequency to 72MHz and configure HCLK, PCLK2 
 *        and PCLK1 prescalers. 
 * @param None.
 * @arg None.
 * @note : This function should be used only after reset.
 * @retval value: None.
 */
static void SetSysClockTo72(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
  
  /* Wait till HSE is ready and if Time out is reached exit */
  do {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
    HSEStatus = (uint32_t)0x01;
  } else { /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */
    HSEStatus = (uint32_t)0x00;
  } /* if ((RCC->CR & RCC_CR_HSERDY) != RESET) */

  if (HSEStatus == (uint32_t)0x01) {

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC->CFGR &= (uint32_t)
      ((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);
    
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    
    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);

  } else { /* if (HSEStatus == (uint32_t)0x01) */
    /* 
     * If HSE fails to start-up, the application will have wrong clock 
     * configuration. User can add here some code to deal with this error
     */    

    /* Go to infinite loop */
    while (1);
  } /* if (HSEStatus == (uint32_t)0x01) */
}
#endif

