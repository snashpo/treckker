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

enum time_alarm_mode_n{
	RTC_TIME,
	RTC_ALARM
};

enum alarm_state_n {
	RTC_ALARM_WAITING,
	RTC_ALARM_STARTED,
	RTC_ALARM_PENDING
};

enum alarm_mode_n{
	ALARM_MODE_DISABLE,
	ALARM_MODE_ONCE,
	ALARM_MODE_DAYLY
	//--------------------------------------------------
	// ALARM_MODE_WEEKLY,
	// ALARM_MODE_MONTHLY,
	// ALARM_MODE_YEARLY,
	//-------------------------------------------------- 
};

/* Date Structure definition */
typedef struct {
  uint16_t Year;
  uint8_t Month;
  uint8_t mDay;
  uint8_t wDay;
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
  uint8_t leap;
  uint16_t time_correct;
  enum alarm_mode_n mode;
} rtc_t;

//--------------------------------------------------
// extern struct rtc_t s_DateStructVar;
// extern struct rtc_t s_AlarmDateStructVar;
//-------------------------------------------------- 
/* Exported constants --------------------------------------------------------*/
#define BATTERY_REMOVED 98
#define BATTERY_RESTORED 99
#define SECONDS_IN_DAY 86399
#define CONFIGURATION_DONE 0xAAAA
#define CONFIGURATION_RESET 0x0000
#define OCTOBER_FLAG_SET 0x4000
#define MARCH_FLAG_SET 0x8000
#define LEAP 1
#define NOT_LEAP 0

#define DEFAULT_MDAY 20
#define DEFAULT_WDAY 3
#define DEFAULT_MONTH 7
#define DEFAULT_YEAR 2011
#define DEFAULT_HOURS 19
#define DEFAULT_MINUTES 0
#define DEFAULT_SECONDS 0
#define DEFAULT_TIME_CORRECT_FLAG MARCH_FLAG_SET

#define DEFAULT_ALARM_MDAY 20
#define DEFAULT_ALARM_WDAY 3
#define DEFAULT_ALARM_MONTH 7
#define DEFAULT_ALARM_YEAR 2011
#define DEFAULT_ALARM_HOURS 19
#define DEFAULT_ALARM_MINUTES 10
#define DEFAULT_ALARM_SECONDS 0
#define DEFAULT_ALARM_MODE			ALARM_MODE_ONCE


#define RTC_COUNTER_SET_VALUE		1
#define RTC_ALARM_SET_VALUE		1
#define RTC_PERIOD_VALUE			((RTC_COUNTER_SET_VALUE)*(RTC_ALARM_SET_VALUE))

#define READ_BKP_CONFIGURATION()   BKP_ReadBackupRegister(BKP_DR1)
#define READ_BKP_SUMMERTIME()      BKP_ReadBackupRegister(BKP_DR2)
#define READ_BKP_CLOCK_YEAR()      BKP_ReadBackupRegister(BKP_DR3)
#define READ_BKP_CLOCK_MONTH()     BKP_ReadBackupRegister(BKP_DR4)
#define READ_BKP_CLOCK_MDAY()      BKP_ReadBackupRegister(BKP_DR5)
#define READ_BKP_CLOCK_WDAY()      BKP_ReadBackupRegister(BKP_DR6)
#define READ_BKP_CLOCK_TIME_HI()   BKP_ReadBackupRegister(BKP_DR8)
#define READ_BKP_CLOCK_TIME_LO()   BKP_ReadBackupRegister(BKP_DR9)
#define READ_BKP_ALARM_YEAR()      BKP_ReadBackupRegister(BKP_DR10)
#define READ_BKP_ALARM_MONTH()     BKP_ReadBackupRegister(BKP_DR11)
#define READ_BKP_ALARM_MDAY()      BKP_ReadBackupRegister(BKP_DR12)
#define READ_BKP_ALARM_TIME_HI()   BKP_ReadBackupRegister(BKP_DR13)
#define READ_BKP_ALARM_TIME_LO()   BKP_ReadBackupRegister(BKP_DR14)
#define READ_BKP_ALARM_MODE()		  BKP_ReadBackupRegister(BKP_DR15)

#define WRITE_BKP_CONFIGURATION(x) BKP_WriteBackupRegister(BKP_DR1, (uint16_t)(x))
#define WRITE_BKP_SUMMERTIME(x)    BKP_WriteBackupRegister(BKP_DR2, (uint16_t)(x))
#define WRITE_BKP_CLOCK_YEAR(x)    BKP_WriteBackupRegister(BKP_DR3, (uint16_t)(x))
#define WRITE_BKP_CLOCK_MONTH(x)   BKP_WriteBackupRegister(BKP_DR4, (uint16_t)(x))
#define WRITE_BKP_CLOCK_MDAY(x)    BKP_WriteBackupRegister(BKP_DR5, (uint16_t)(x))
#define WRITE_BKP_CLOCK_WDAY(x)    BKP_WriteBackupRegister(BKP_DR6, (uint16_t)(x))
#define WRITE_BKP_CLOCK_TIME_HI(x) BKP_WriteBackupRegister(BKP_DR8, (uint16_t)(x))
#define WRITE_BKP_CLOCK_TIME_LO(x) BKP_WriteBackupRegister(BKP_DR9, (uint16_t)(x))
#define WRITE_BKP_ALARM_YEAR(x)    BKP_WriteBackupRegister(BKP_DR10, (uint16_t)(x))
#define WRITE_BKP_ALARM_MONTH(x)   BKP_WriteBackupRegister(BKP_DR11, (uint16_t)(x))
#define WRITE_BKP_ALARM_MDAY(x)    BKP_WriteBackupRegister(BKP_DR12, (uint16_t)(x))
#define WRITE_BKP_ALARM_TIME_HI(x) BKP_WriteBackupRegister(BKP_DR13, (uint16_t)(x))
#define WRITE_BKP_ALARM_TIME_LO(x) BKP_WriteBackupRegister(BKP_DR14, (uint16_t)(x))
#define WRITE_BKP_ALARM_MODE(x)	  BKP_WriteBackupRegister(BKP_DR15, (uint16_t)(x))


#define READ_BKP_CLOCK_TIME()			(((uint32_t)READ_BKP_CLOCK_TIME_HI()) << 16) \
													| READ_BKP_CLOCK_TIME_LO()

#define READ_BKP_ALARM_TIME()			(((uint32_t)READ_BKP_ALARM_TIME_HI()) << 16) \
													| READ_BKP_ALARM_TIME_LO()

#define WRITE_BKP_CLOCK_TIME(x)	\
{\
	WRITE_BKP_CLOCK_TIME_HI(((x) & 0xFFFF0000) >> 16);\
	WRITE_BKP_CLOCK_TIME_LO((x) & 0x0000FFFF);\
}while(0);

#define WRITE_BKP_ALARM_TIME(x)	\
{\
	WRITE_BKP_ALARM_TIME_HI(((x) & 0xFFFF0000) >> 16);\
	WRITE_BKP_ALARM_TIME_LO((x) & 0x0000FFFF);\
}while(0);

extern uint16_t SummerTimeCorrect;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void rtc_Init(void);
void RTC_Configuration(void);

uint32_t get_sec_counter(void);
void set_sec_counter(uint32_t now);
uint32_t inc_sec_counter(void);

void set_time(rtc_t *rtc);
void set_alarm(rtc_t *rtc);
void get_time(rtc_t * rtc);
void get_alarm(rtc_t * rtc);

void alarm_Mgmt(void);
void rtc_print(void);

//--------------------------------------------------
// void DateUpdate(void);
// void TimeUpdate(void);
// uint16_t WeekDay(uint16_t,uint8_t,uint8_t);
// void CheckForDaysElapsed(void);
// void ManualClockCalibration(void);
// //-------------------------------------------------- 

#endif /* __CLOCK_CALENDAR_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

