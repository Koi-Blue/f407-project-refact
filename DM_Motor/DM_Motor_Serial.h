#ifndef __DM_MOTOR_SERIAL_H
#define __DM_MOTOR_SERIAL_H

#include <stdint.h>

#define DM_MOTOR_ID            0x01      //电机的CAN ID
#define DM_MOTOR_MASTER_ID     0x00      //电机反馈用的ID

#define DM_MOTOR_P_MAX         12.5f    //位置范围
#define DM_MOTOR_V_MAX         45.0f    //速度范围
#define DM_MOTOR_T_MAX         10.0f    //力矩范围
#define DM_MOTOR_KP_MAX        500.0f
#define DM_MOTOR_KD_MAX        5.0f

typedef struct
{
		uint8_t Motor_ID;
		uint8_t Master_ID;
		uint8_t State;         //电机当前状态
		float Position;        //电机当前位置
		float Velocity;        //电机当前速度
		float Torque;          //电机当前力矩
		uint8_t MOS_Temperature;
		uint8_t Rotor_Temperature;
} DM_Motor_TypeDef;

extern DM_Motor_TypeDef DM_Motor_Data;       //全局变量声明

void DM_Motor_Data_Reset(void);

uint16_t float_to_uint(float Data, float Min, float Max, uint8_t Bits);   
float uint_to_float(uint16_t Data, float Min, float Max, uint8_t Bits);

void DM_Motor_Enable_Pack(uint8_t Data[8]);
void DM_Motor_Disable_Pack(uint8_t Data[8]);
void DM_Motor_MIT_Pack(float Position,
							 float Velocity,
							 float Kp,
							 float Kd,
							 float Torque,
							 uint8_t Data[8]);
void DM_Motor_Feedback_Decode(uint16_t ID, const uint8_t Data[8]);

#endif
