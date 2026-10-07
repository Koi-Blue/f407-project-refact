#include "DM_Motor_Serial.h"

DM_Motor_TypeDef DM_Motor_Data =
{
		DM_MOTOR_ID,
		DM_MOTOR_MASTER_ID,
		0,
		0.0f,
		0.0f,
		0.0f,
		0,
		0
}; //创建DM_Motor_Data，并赋值

void DM_Motor_Data_Reset(void)    //复位
{
		DM_Motor_Data.Motor_ID = DM_MOTOR_ID;
		DM_Motor_Data.Master_ID = DM_MOTOR_MASTER_ID;
		DM_Motor_Data.State = 0;
		DM_Motor_Data.Position = 0.0f;
		DM_Motor_Data.Velocity = 0.0f;
		DM_Motor_Data.Torque = 0.0f;
		DM_Motor_Data.MOS_Temperature = 0;
		DM_Motor_Data.Rotor_Temperature = 0;
}

//仅当前文件可调用
static float DM_Motor_Limit(float Data, float Min, float Max)   //限幅
{
		if (Data < Min)
		{
				return Min;
		}

		if (Data > Max)
		{
				return Max;
		}

		return Data;
}

//把浮点数压缩成指定 bit 数的无符号整数
//Bits：压缩后使用多少位
uint16_t float_to_uint(float Data, float Min, float Max, uint8_t Bits)  
{
		uint32_t Span;    

		if ((Bits == 0) || (Bits > 16) || (Max <= Min))
		{
				return 0;
		}

		Data = DM_Motor_Limit(Data, Min, Max);
		Span = (1UL << Bits) - 1;   //二进制左移

		return (uint16_t)((Data - Min) * Span / (Max - Min));
}


//把 CAN 报文里的压缩整数恢复成实际浮点数
float uint_to_float(uint16_t Data, float Min, float Max, uint8_t Bits)
{
		uint32_t Span;

		if ((Bits == 0) || (Bits > 16) || (Max <= Min))
		{
				return Min;
		}

		Span = (1UL << Bits) - 1; 

		return (float)Data * (Max - Min) / Span + Min;
}

//使能  FF FF FF FF FF FF FF FC
void DM_Motor_Enable_Pack(uint8_t Data[8])
{
		uint8_t i;

		for (i = 0; i < 7; i++)
		{
				Data[i] = 0xFF;
		}

		Data[7] = 0xFC;
}

//失能  FF FF FF FF FF FF FF FD
void DM_Motor_Disable_Pack(uint8_t Data[8])
{
		uint8_t i;

		for (i = 0; i < 7; i++)
		{
				Data[i] = 0xFF;
		}

		Data[7] = 0xFD;
}

//生成 MIT 模式控制帧
//电机输出 ≈Kp × 位置误差+ Kd × 速度误差+ Torque   
void DM_Motor_MIT_Pack(float Position,
							 float Velocity,
							 float Kp,
							 float Kd,
							 float Torque,
							 uint8_t Data[8])
{
		//用于保存压缩后的整数
		uint16_t Position_Temp;
		uint16_t Velocity_Temp;
		uint16_t Kp_Temp;
		uint16_t Kd_Temp;
		uint16_t Torque_Temp;

		Position_Temp = float_to_uint(Position, -DM_MOTOR_P_MAX, DM_MOTOR_P_MAX, 16);
		Velocity_Temp = float_to_uint(Velocity, -DM_MOTOR_V_MAX, DM_MOTOR_V_MAX, 12);
		Kp_Temp = float_to_uint(Kp, 0.0f, DM_MOTOR_KP_MAX, 12);
		Kd_Temp = float_to_uint(Kd, 0.0f, DM_MOTOR_KD_MAX, 12);
		Torque_Temp = float_to_uint(Torque, -DM_MOTOR_T_MAX, DM_MOTOR_T_MAX, 12);

		Data[0] = Position_Temp >> 8;   //>> 8 表示右移 8 位，取出位置的高 8 位
		Data[1] = Position_Temp;        //位置低8位
		Data[2] = Velocity_Temp >> 4;   //取速度的高 8 位
		Data[3] = (Velocity_Temp & 0x0F) << 4 | Kp_Temp >> 8;   //高4位：速度剩余的4位 低4位：Kp的高4位
		Data[4] = Kp_Temp;
		Data[5] = Kd_Temp >> 4;
		Data[6] = (Kd_Temp & 0x0F) << 4 | Torque_Temp >> 8;
		Data[7] = Torque_Temp;
}


//解析电机返回的 CAN 数据
void DM_Motor_Feedback_Decode(uint16_t ID, const uint8_t Data[8])
{
		uint16_t Position_Temp;
		uint16_t Velocity_Temp;
		uint16_t Torque_Temp;

		if (ID != DM_MOTOR_MASTER_ID)
		{
				return;
		}

		//高4位：状态  低4位：电机ID
		DM_Motor_Data.State = Data[0] >> 4;
		DM_Motor_Data.Motor_ID = Data[0] & 0x0F;
		
		//两个 8 位字节拼成一个 16 位位置整数
		Position_Temp = (uint16_t)Data[1] << 8 | Data[2];
		//12 位速度  Data[3]全部8位 + Data[4]高4位
		Velocity_Temp = (uint16_t)Data[3] << 4 | Data[4] >> 4;
		//12 位力矩  Data[4]低4位 + Data[5]全部8位
		Torque_Temp = (uint16_t)(Data[4] & 0x0F) << 8 | Data[5];

		DM_Motor_Data.Position = uint_to_float(Position_Temp, -DM_MOTOR_P_MAX, DM_MOTOR_P_MAX, 16);
		DM_Motor_Data.Velocity = uint_to_float(Velocity_Temp, -DM_MOTOR_V_MAX, DM_MOTOR_V_MAX, 12);
		DM_Motor_Data.Torque = uint_to_float(Torque_Temp, -DM_MOTOR_T_MAX, DM_MOTOR_T_MAX, 12);
		DM_Motor_Data.MOS_Temperature = Data[6];
		DM_Motor_Data.Rotor_Temperature = Data[7];
}
