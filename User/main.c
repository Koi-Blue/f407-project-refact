#include "stm32f4xx.h"                  // Device header
#include <stdio.h>
#include "system.h"
#include "Stepper_Motor_Init.h"
#include "Stepper_Motor_Run.h"
#include "Chassis_Move.h"
#include "Serial.h"
#include "Agreement.h"

int main(void)
{ 
		System_Init();
		Stepper_Motor_Init();
		USART2_Init(115200);

		/*
		 * 达妙与舵机初始化当前不调用，避免在没有任务逻辑时改主循环行为。
		 * 启用达妙前，Keil 的 DM_Motor 组必须包含 DM_Motor_Run.c 与 DM_Motor_Serial.c
		 * （本工程已编入）。建议在进入 while 之前再调用：
		 *     DM_Motor_Init();
		 *     PWM_Init();
		 * 使能、MIT 指令和舵机角度放到具体任务里，不要在这里直接打开。
		 * 接口与调用关系见 docs/module-api-inventory.md。
		 */
		
		while (1)
		{

//				Chassis_Move_X(3200*5, 50, 50, 100);
//				Chassis_Move_Y(3200, 50, 50, 100);
//				Motor_Run_CN1(3200*10, 50, 50, 100);
//				Motor_Run_CN2(-3200*10, 50, 50, 50);
//				Motor_Run_CN3(3200*10, 50, 50, 50);
//				Motor_Run_CN4(-3200*10, 50, 50, 50);
//				Motor_Run_CN6(-1600, 100, 100, 10);
//				Motor_Run_CN8(-1600, 100, 100, 10);
//				Agreement_Recv();
		}
		
}
