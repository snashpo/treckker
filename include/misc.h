#ifndef __MISC_H
#define __MISC_H

#include "stm32f10x.h"

/*
 * NVIC Init Structure definition  
 */

typedef struct {
  uint8_t NVIC_IRQChannel;
  uint8_t NVIC_IRQChannelPreemptionPriority;
  uint8_t NVIC_IRQChannelSubPriority;
  FunctionalState NVIC_IRQChannelCmd;
} NVIC_InitTypeDef;

/*
 * Vector_Table_Base 
 */

#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)
#define IS_NVIC_VECTTAB(VECTTAB)					\
(((VECTTAB) == NVIC_VectTab_RAM) || ((VECTTAB) == NVIC_VectTab_FLASH))

/*
 * System_Low_Power 
 */

#define NVIC_LP_SEVONPEND            ((uint8_t)0x10)
#define NVIC_LP_SLEEPDEEP            ((uint8_t)0x04)
#define NVIC_LP_SLEEPONEXIT          ((uint8_t)0x02)

#define IS_NVIC_LP(LP)				       \
  (((LP) == NVIC_LP_SEVONPEND) || ((LP) == NVIC_LP_SLEEPDEEP) ||	\
   ((LP) == NVIC_LP_SLEEPONEXIT))

/*
 * Preemption_Priority_Group 
 */

/* 0 bits for pre-emption priority / 4 bits for subpriority */
#define NVIC_PriorityGroup_0         ((uint32_t)0x700) 

/* 1 bits for pre-emption priority / 3 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x600) 

/* 2 bits for pre-emption priority / 2 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x500) 

/* 3 bits for pre-emption priority / 1 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x400)

/* 4 bits for pre-emption priority / 0 bits for subpriority */
#define NVIC_PriorityGroup_4         ((uint32_t)0x300) 


#define IS_NVIC_PRIORITY_GROUP(GROUP)					\
  (((GROUP) == NVIC_PriorityGroup_0) || ((GROUP) == NVIC_PriorityGroup_1) || \
   ((GROUP) == NVIC_PriorityGroup_2) || ((GROUP) == NVIC_PriorityGroup_3) || \
   ((GROUP) == NVIC_PriorityGroup_4))

#define IS_NVIC_PREEMPTION_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

#define IS_NVIC_SUB_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

#define IS_NVIC_OFFSET(OFFSET)  ((OFFSET) < 0x0007FFFF)

/*
 * SysTick_clock_source 
 */

#define SysTick_CLKSource_HCLK_Div8    ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK         ((uint32_t)0x00000004)

#define IS_SYSTICK_CLK_SOURCE(SOURCE)					\
  (((SOURCE) == SysTick_CLKSource_HCLK) ||				\
   ((SOURCE) == SysTick_CLKSource_HCLK_Div8))

void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
void NVIC_SystemLPConfig(uint8_t LowPowerMode, FunctionalState NewState);
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource);

#endif /* __MISC_H */

