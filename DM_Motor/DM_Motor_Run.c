#include "stm32f4xx.h"
#include "DM_Motor_Serial.h"
#include "DM_Motor_Run.h"

//执行 CAN 发送的函数
static void DM_Motor_CAN_Send(uint16_t ID, const uint8_t Data[8])
{
		//CanTxMsg 是 STM32F4 标准外设库提供的 CAN 发送结构体
		CanTxMsg TxMessage;
		uint8_t i;

		TxMessage.StdId = ID;  //标准CAN ID
		TxMessage.ExtId = 0;   //扩展CAN ID
		TxMessage.IDE = CAN_Id_Standard;   //选择使用标准ID还是扩展ID
		TxMessage.RTR = CAN_RTR_Data;     //数据帧还是远程帧
		TxMessage.DLC = 8;       //有几个数据字节

		for (i = 0; i < 8; i++)
		{
				TxMessage.Data[i] = Data[i];
		}

		CAN_Transmit(CAN1, &TxMessage);  //把 TxMessage 交给 CAN1 外设，请求发送
}

void DM_Motor_Run_Enable(void)
{
		uint8_t Data[8];

		DM_Motor_Enable_Pack(Data);
		DM_Motor_CAN_Send(DM_MOTOR_ID, Data);
}

void DM_Motor_Run_Disable(void)
{
		uint8_t Data[8];

		DM_Motor_Disable_Pack(Data);
		DM_Motor_CAN_Send(DM_MOTOR_ID, Data);
}

//MIT控制发送
void DM_Motor_Run_MIT(float Position,
							float Velocity,
							float Kp,
							float Kd,
							float Torque)
{
		uint8_t Data[8];

		DM_Motor_MIT_Pack(Position, Velocity, Kp, Kd, Torque, Data);
		DM_Motor_CAN_Send(DM_MOTOR_ID, Data);
}

void DM_Motor_Run_Receive(uint16_t ID, const uint8_t Data[8])
{
		DM_Motor_Feedback_Decode(ID, Data);
}

//CAN1 的 FIFO0 接收中断函数
void CAN1_RX0_IRQHandler(void)
{
		CanRxMsg RxMessage;  //CanRxMsg 是标准外设库提供的接收结构体

		if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
		{
				CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

				if ((RxMessage.IDE == CAN_Id_Standard) &&
						(RxMessage.RTR == CAN_RTR_Data) &&
						(RxMessage.DLC == 8))
				{
						DM_Motor_Run_Receive(RxMessage.StdId, RxMessage.Data);
				}

				CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		}
}
