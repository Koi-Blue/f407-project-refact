#include "Mechanism.h"
#include "Stepper_Motor_Run.h"

void Car_Lift(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run_CN6(step, accel, decel, speed);
}

void Car_Extend(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run_CN8(step, accel, decel, speed);
}
