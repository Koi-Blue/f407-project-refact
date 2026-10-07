#ifndef __STEPPER_MOTOR_INIT_H
#define __STEPPER_MOTOR_INIT_H

#define ARR (100-1)
#define TIM_PRESCALER     (84 - 1)
#define TIM_PERIOD				(100 - 1)

void Stepper_Motor_Init(void);

#endif
