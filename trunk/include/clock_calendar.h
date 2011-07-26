/**
  ******************************************************************************
  * @file RTC/inc/clock_calendar.h 
  * @author  MCD Application Team
  * @version  V2.0.0
  * @date  04/27/2009
  * @brief  This files contains the Clock Calendar functions prototypes
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CLOCK_CALENDAR_H
#define __CLOCK_CALENDAR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/* Exported types ------------------------------------------------------------*/
/* Time Structure definition */
//--------------------------------------------------
// struct Time_s
// {
//   uint8_t SecLow;
//   uint8_t SecHigh;
//   uint8_t MinLow;
//   uint8_t MinHigh;
//   uint8_t HourLow;
//   uint8_t HourHigh;
// };
// extern struct Time_s s_TimeStructVar;
// 
// /* Alarm Structure definition */
// struct AlarmTime_s
// {
//   uint8_t SecLow;
//   uint8_t SecHigh;
//   uint8_t MinLow;
//   uint8_t MinHigh;
//   uint8_t HourLow;
//   uint8_t HourHigh;
// };
// extern struct AlarmTime_s s_AlarmStructVar;
//-------------------------------------------------- 

/* Date Structure definition */
struct Date_s
{
  uint8_t Month;
  uint8_t Day;
  uint16_t Year;
  uint32_t time;
};
extern struct Date_s s_DateStructVar;
extern struct Date_s s_AlarmDateStructVar;

//--------------------------------------------------
// /* Alarm Date Structure definition */
// struct AlarmDate_s
// {
//   uint8_t Month;
//   uint8_t Day;
//   uint16_t Year;
//   uint32_t time;
// };
//-------------------------------------------------- 

enum dateTime_settings_n{
	RTC_MODE_SETTINGS_CURRENT,
	RTC_MODE_SETTINGS_ALARM,
};

enum alarm_state_n {
	RTC_ALARM_DISABLE,
	RTC_ALARM_WAITING,
	RTC_ALARM_STARTED,
	RTC_ALARM_PENDING,
};

/* Exported constants --------------------------------------------------------*/
#define BATTERY_REMOVED 98
#define BATTERY_RESTORED 99
#define SECONDS_IN_DAY 86399
#define CONFIGURATION_DONE 0xAAAA
#define CONFIGURATION_RESET 0x0000
#define OCTOBER_FLAG_SET 0x4000
#define MARCH_FLAG_SET 0x8000
#define DEFAULT_DAY 20
#define DEFAULT_MONTH 7
#define DEFAULT_YEAR 2011
#define DEFAULT_HOURS 19
#define DEFAULT_MINUTES 0
#define DEFAULT_SECONDS 0
#define DEFAULT_TIME_CORRECT_FLAG MARCH_FLAG_SET
#define LEAP 1
#define NOT_LEAP 0


#define RTC_COUNTER_SET_VALUE		1
#define RTC_ALARM_SET_VALUE		1
#define RTC_PERIOD_VALUE			((RTC_COUNTER_SET_VALUE)*(RTC_ALARM_SET_VALUE))

extern uint16_t SummerTimeCorrect;
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void rtc_Init(void);
void RTC_Configuration(void);
void SetTime(enum dateTime_settings_n, uint8_t,uint8_t,uint8_t);
void SetDate(enum dateTime_settings_n, uint8_t,uint8_t,uint16_t);
void alarm_Mgmt(void);
void DateUpdate(void);
uint16_t WeekDay(uint16_t,uint8_t,uint8_t);
static uint8_t CheckLeap(uint16_t);
void CalculateTime(void);
void RTC_NVIC_Configuration(void);
void rtc_Init(void);
void GPIO_Configuration(void);
void DelayLowPower(__IO uint32_t nCount);
void SysTickConfig(void);
void CheckForDaysElapsed(void);
static void SummerTimeCorrection(void);
void ManualClockCalibration(void);
void ReturnFromStopMode(void);
#endif /* __CLOCK_CALENDAR_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
