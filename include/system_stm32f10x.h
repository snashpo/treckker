#ifndef __SYSTEM_STM32F10X_H
#define __SYSTEM_STM32F10X_H

enum SystemFrequency_Mode_n {
	LOW,
	HIGH,
};

/* System Clock Frequency (Core Clock) */
extern const uint32_t SystemFrequency;

/* System clock                        */
extern const uint32_t SystemFrequency_SysClk;

/* AHB System bus speed                */
extern const uint32_t SystemFrequency_AHBClk;

/* APB Peripheral Bus 1 (low)  speed   */
extern const uint32_t SystemFrequency_APB1Clk; 

/* APB Peripheral Bus 2 (high) speed   */
extern const uint32_t SystemFrequency_APB2Clk;

extern void SystemInit(void);

void SetSysClock_FullSpeed(void);
void SetSysClock_LowSpeed(void);

#endif /*__SYSTEM_STM32F10X_H */
