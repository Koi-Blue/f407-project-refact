#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx.h"                  // Device header


void Timer7_Init(uint32_t arr, uint16_t psc);
void TIM7_IRQHandler(void);

#endif
