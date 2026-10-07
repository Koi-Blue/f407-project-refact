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
