#include "Chassis_Move.h"
#include "Stepper_Motor_Run.h"

void Chassis_Move_X(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run_CN1( step, accel, decel, speed);
    Motor_Run_CN2( step, accel, decel, speed);
    Motor_Run_CN3(-step, accel, decel, speed);
    Motor_Run_CN4(-step, accel, decel, speed);
}

void Chassis_Move_Y(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run_CN1( step, accel, decel, speed);
    Motor_Run_CN2(-step, accel, decel, speed);
    Motor_Run_CN3( step, accel, decel, speed);
    Motor_Run_CN4(-step, accel, decel, speed);
}

void Chassis_Move_Turn(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run_CN1(step, accel, decel, speed);
    Motor_Run_CN2(step, accel, decel, speed);
    Motor_Run_CN3(step, accel, decel, speed);
    Motor_Run_CN4(step, accel, decel, speed);
}
