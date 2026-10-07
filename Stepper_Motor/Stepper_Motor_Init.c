#include "stm32f4xx.h"                  // Device header
#include "Stepper_Motor_Init.h"

void Stepper_Motor_Init(void)  
{
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4,ENABLE);
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE, ENABLE);
	
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);   //CN1
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);   //CN2
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);		//CN3
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);		//CN4
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);		//CN6
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);		//CN8
	
		//stp口
		GPIO_InitTypeDef GPIO_InitStructure;        
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;            //复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	     //速度100MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  	//上拉
	
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;  
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;          
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;          
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		//Dir En口
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_10 | GPIO_Pin_11;          
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;            //复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	     //速度100MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;            //上拉
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4 | GPIO_Pin_5;          
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;            //复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	     //速度100MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;            //上拉
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_10| GPIO_Pin_12 | GPIO_Pin_15;          
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;            //复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	     //速度100MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;            //上拉
		GPIO_Init(GPIOE, &GPIO_InitStructure);	
			
		GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_10 | GPIO_Pin_11);
		GPIO_ResetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4 | GPIO_Pin_5);	
		GPIO_ResetBits(GPIOE, GPIO_Pin_8 | GPIO_Pin_10| GPIO_Pin_12 | GPIO_Pin_15);		
		
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Prescaler = 84-1;                      //PSC
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;        //向上计数
		TIM_TimeBaseStructure.TIM_Period = ARR;                       //ARR
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		
		TIM_OCInitTypeDef TIM_OCInitStructure;
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_Pulse = 0;															//CCR
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;    //在空闲时输出低,这里的设置可以改变TIM_OCPolarity 
		
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);  
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);  
		TIM_OC3Init(TIM2, &TIM_OCInitStructure);  
		TIM_OC4Init(TIM2, &TIM_OCInitStructure);  
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);  
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);  
		
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;  // 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;         // 子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 
		NVIC_Init(&NVIC_InitStructure);
		NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
		NVIC_Init(&NVIC_InitStructure);
		NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; 
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Regular);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
		
		TIM_UpdateRequestConfig(TIM3, TIM_UpdateSource_Regular);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
		
		TIM_UpdateRequestConfig(TIM4, TIM_UpdateSource_Regular);
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
		
		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能在CCR1上的预装载寄存器	
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 
		TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  
		TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  
		
		TIM_ARRPreloadConfig(TIM2, ENABLE);
		TIM_ARRPreloadConfig(TIM3, ENABLE);
		TIM_ARRPreloadConfig(TIM4, ENABLE);
		
		
}
		
		