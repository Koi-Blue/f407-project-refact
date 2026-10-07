#include "stm32f4xx.h"                  // Device header
#include "Servo.h"

uint8_t Servo_State_1 = 0;
uint8_t Servo_State_2 = 0;

void PWM_Init(void)
{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

		GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_TIM9);

		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;         
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
		GPIO_Init(GPIOE,&GPIO_InitStructure);       

		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
		TIM_TimeBaseInitStructure.TIM_Period = 20000-1;     //ARR
		TIM_TimeBaseInitStructure.TIM_Prescaler = 168-1;    //PSC
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);

		TIM_OCInitTypeDef TIM_OCInitStructure;
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC1Init(TIM9,&TIM_OCInitStructure);
		TIM_OC2Init(TIM9,&TIM_OCInitStructure);

		TIM_OC1PreloadConfig(TIM9,TIM_OCPreload_Enable);
		TIM_OC2PreloadConfig(TIM9,TIM_OCPreload_Enable);

		TIM_ARRPreloadConfig(TIM9,ENABLE);
		TIM_Cmd(TIM9,ENABLE);
}

//в╙ел
void Servo_SetAngle1(float Angle)			
{
		TIM_SetCompare1(TIM9, Angle / 360 * 2000 + 500);
}

//в╕вс
void Servo_SetAngle2(float Angle)		
{
		TIM_SetCompare2(TIM9, Angle / 270 * 2000 + 500);
}
