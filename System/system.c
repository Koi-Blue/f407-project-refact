#include "stm32f4xx.h"                  // Device header
#include "system.h"


void System_Init(void)
{
		delay_init(168);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2

		
}
