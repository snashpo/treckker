#include "stm32f10x.h"

#include "buzzer.h"
#include "timer.h"


static uint32_t buzzer_start = 0;
static uint32_t buzzer_delay = 0;

void buzzer_init()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;

  /* Enable TIM2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /* Set TIM2 on internal clock */
  TIM_InternalClockConfig(TIM2);

  /* Init TIM2 to 500ms period */
  TIM_TimeBaseInitStruct.TIM_Prescaler = 10000;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Down;
  TIM_TimeBaseInitStruct.TIM_Period = 2;
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

  TIM_ARRPreloadConfig(TIM2, ENABLE);

  /* Setup Output channel 2 */
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = 0;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC2Init(TIM2, &TIM_OCInitStruct);

  TIM_OC2PreloadConfig(TIM2, ENABLE);

}

void buzzer_on()
{
  /* Enable TIM */
  TIM_Cmd(TIM2, ENABLE);
}

void buzzer_off()
{
  /* Enable TIM */
  TIM_Cmd(TIM2, DISABLE);
}

void buzzer_mgmt()
{
  if (buzzer_delay) {
    buzzer_on();
  } else { /* if (buzzer_delay) */
    buzzer_off();
  } /* if (buzzer_delay) */

  if (expire_timer(buzzer_start, buzzer_delay)) {
    buzzer_delay = 0;
  } /* if (expire_timer(buzzer_start, buzzer_delay)) */
}

void buzzer_play(uint32_t t)
{
  buzzer_start = tick_1khz();

  buzzer_delay = t; 
}
