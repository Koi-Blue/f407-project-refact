#include "system.h"

//void TIM6_Init(u16 psc, u16 arr)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE);			//使能时钟
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//计数模式
//	TIM_TimeBaseInitStruct.TIM_Period = arr - 1;					//自动重装载值，范围0~65535,
//	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;					//分频系数，不为1时，定时器频率挂载总线的两倍（看时钟树得出），所以42000000/42000 
//	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
//	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);				//初始化
//	TIM_Cmd(TIM6, ENABLE);											//使能定时器6

//	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); 						//允许定时器6更新中断
//	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;			
//	NVIC_EnableIRQ(TIM6_DAC_IRQn );									//使能TIM6中断
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}

//void TIM6_DAC_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //溢出中断
//	{
//		i++;				
//	}
//	TIM_ClearITPendingBit(TIM6,TIM_IT_Update); //清除中断标志位
//}

// TIM7定时中断初始化
void Timer7_Init(uint32_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	    // 自动重装载值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;        // 定时器分频PSC
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);// 初始化TIM3
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); // 允许定时器更新中断
	TIM_Cmd(TIM7,DISABLE); // 使能定时器

	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;  // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;         // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**************************************************************************
函数功能：TIM7中断服务函数 定时读取编码器数值并清零 10ms进入一次
入口参数：无
返回  值：无
**************************************************************************/
void TIM7_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) //当发生中断时状态寄存器(TIMx_SR)的bit0会被硬件置1
	{

	}
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update); //状态寄存器(TIMx_SR)的bit0置0
}
