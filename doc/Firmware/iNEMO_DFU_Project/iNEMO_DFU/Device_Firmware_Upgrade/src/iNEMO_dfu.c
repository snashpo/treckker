/**
  ******************************************************************************
  * @file    iNEMO_dfu.c
  * @author  ART Team IMS-Systems Lab
  * @version V3.2.1
  * @date    09/20/2010
  * @brief   This file provides
  *            - set of firmware functions to manage iNEMO USB for DFU Functionality
  *
  * @details
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
  * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
  * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "iNEMO_dfu_mal.h"
#include "usb_lib.h"
#include "iNEMO_usb_desc.h"
#include "usb_pwr.h"
#include "iNEMO_usb_common.h"
#include "iNEMO_dfu.h"
#include "iNEMO_button.h"


/** @addtogroup iNEMO_DFU_Program
  * @{
  */

/** @addtogroup Device_Firmware_Upgrade
  * @{
  */

/** @addtogroup DFU_Functionality
  * @{
  */

/** @defgroup DFU_Configuration_Functions
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Configures the DFU selector Button to enter DFU Mode
  * @param  None
  * @retval None
  */
void DFU_Button_Config(void)
{
  /* Configure "DFU enter" button */
  iNEMO_Button_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
}

/**
  * @brief  Reads the DFU selector Button to enter DFU in Hardware Mode 
  * @param  None
  * @retval Status
  */
uint8_t DFU_Button_Read (void)
{
  return iNEMO_Button_Get_State(BUTTON_KEY);
}


/**
  * @brief  Reads the Option Byte to enter DFU in Software Mode
  * @param  None
  * @retval Option Byte Value
  */
uint8_t Option_Byte_Read (void)
{
  return (uint8_t) (*(__IO uint32_t*) Option_address);
}

/**
  * @brief  Reset the device
  * @param  None
  * @retval None
  */
void Reset_Device(void)
{
#ifdef Enable_SW_Mode
  Leave_DFU_SW_Mode();
#endif
  USB_Cable_Config(DISABLE);
  NVIC_SystemReset();
}

/**
  * @brief  Program the Option Byte to leave DFU in Software Mode
  * @param  none.
  * @retval None
  */
void Leave_DFU_SW_Mode(void)
{
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();
  FLASH_EraseOptionBytes();  
  FLASH_ProgramOptionByteData(Option_address, 0x00);
  FLASH_Lock();
}


/**
  * @brief  Create the serial number string descriptor for DFU
  * @param  none.
  * @retval None
  */
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);

  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &DFU_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &DFU_StringSerial[18], 4);
  }
}

/**
  * @brief  Convert Hex 32Bits value into char
  * @param  none.
  * @retval None
  */
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}


/**
  * @}
  */ /* end of group DFU_Configuration_Functions */

/**
  * @}
  */ /* end of group DFU_Functionality */

/**
  * @}
  */ /* end of group Device_Firmware_Upgrade */

/**
  * @}
  */ /* end of group iNEMO_DFU_Program */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
