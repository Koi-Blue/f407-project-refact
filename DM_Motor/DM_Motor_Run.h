#ifndef __DM_MOTOR_RUN_H
#define __DM_MOTOR_RUN_H

#include <stdint.h>

void DM_Motor_Run_Enable(void);
void DM_Motor_Run_Disable(void);
void DM_Motor_Run_MIT(float Position,
							float Velocity,
							float Kp,
							float Kd,
							float Torque);
void DM_Motor_Run_Receive(uint16_t ID, const uint8_t Data[8]);

#endif
