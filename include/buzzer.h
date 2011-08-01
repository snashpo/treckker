#ifndef __BUZZER_H
#define __BUZZER_H

#define BUZZER_PIN        GPIO_Pin_1
#define BUZZER_PORT       GPIOA

void buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);
void buzzer_play(uint32_t t);

#endif
