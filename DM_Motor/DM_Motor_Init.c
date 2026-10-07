#include "stm32f4xx.h"
#include "DM_Motor_Serial.h"
#include "DM_Motor_Init.h"

void DM_Motor_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		CAN_InitTypeDef CAN_InitStructure;
		CAN_FilterInitTypeDef CAN_FilterInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		DM_Motor_Data_Reset();

		/* GPIOA、CAN1 时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

		/* PA11=CAN1_RX，PA12=CAN1_TX */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* CAN1，APB1=42MHz，波特率=1Mbps */
		CAN_DeInit(CAN1);
		CAN_StructInit(&CAN_InitStructure);

		CAN_InitStructure.CAN_TTCM = DISABLE;
		CAN_InitStructure.CAN_ABOM = ENABLE;
		CAN_InitStructure.CAN_AWUM = DISABLE;
		CAN_InitStructure.CAN_NART = DISABLE;
		CAN_InitStructure.CAN_RFLM = DISABLE;
		CAN_InitStructure.CAN_TXFP = DISABLE;
		CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
		CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
		CAN_InitStructure.CAN_BS1 = CAN_BS1_11tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
		CAN_InitStructure.CAN_Prescaler = 3;

		if (CAN_Init(CAN1, &CAN_InitStructure) != CAN_InitStatus_Success)
		{
				while (1);
		}

		/* 接收全部标准帧，放入 FIFO0 */
		CAN_FilterInitStructure.CAN_FilterNumber = 0;
		CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
		CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
		CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
		CAN_FilterInitStructure.CAN_FilterIdLow = 0;
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
		CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
		CAN_FilterInit(&CAN_FilterInitStructure);

		/* CAN1 FIFO0 接收中断 */
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}
