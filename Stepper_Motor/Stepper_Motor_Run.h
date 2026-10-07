#ifndef STEPPER_MOTOR_RUN_H
#define STEPPER_MOTOR_RUN_H

#include "stm32f4xx.h"
#include <stdint.h>


#define T1_FREQ           ((SystemCoreClock/2)/(TIM_PRESCALER+1))  //频率ft
#define Basic_T						(1/((T1_FREQ)/(TIM_PERIOD + 1))) * 1000000		//基础周期 

#define STEP_ANGLE				1.8								//步距角
#define FSPR              200   //(360.0/1.8)       //一圈所需脉冲

#define MICRO_STEP        16          			//细分
#define SPR               3200		          //细分后一圈需要脉冲数

#define ALPHA             ((float)(2*3.1415926/SPR))     
#define A_T_x10           ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148       ((float)((T1_FREQ*0.69)/10)) // 0.69为误差修正值
#define A_SQ              ((float)(2*100000*ALPHA)) 
#define A_x200            ((float)(200*ALPHA))


//六个步进电机编号
typedef enum
{
    STEPPER_CN1 = 0,
    STEPPER_CN2,
    STEPPER_CN3,
    STEPPER_CN4,
    STEPPER_CN6,
    STEPPER_CN8,

    STEPPER_MOTOR_COUNT

} StepperMotorId;


//电机运行方向
typedef enum
{
    STEPPER_CW = 0,
    STEPPER_CCW

} StepperDirection;


//梯形加减速运行状态
typedef enum
{
    STEPPER_STOP = 0,
    STEPPER_ACCEL,
    STEPPER_RUN,
    STEPPER_DECEL

} StepperRunState;


//单个电机的梯形加减速数据
typedef struct
{
    volatile StepperRunState run_state;
    volatile uint8_t busy;                  //原来的Motor_X_State，用于标志运行状态

    StepperDirection dir;

    //运动步数
    volatile uint32_t step_count;
    volatile int32_t position;

    //速度对应的定时器周期
    uint32_t min_delay;
    volatile uint32_t step_delay;       ////启动时的脉冲间隔--启动速度-----ARR
    uint32_t last_accel_delay;

    //加减速计算参数
    uint32_t decel_position;
    int32_t accel_count;
    int32_t decel_count;
    int32_t rest;

} StepperRampData;

void Motor_Run_CN1(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void Motor_Run_CN2(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void Motor_Run_CN3(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void Motor_Run_CN4(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void Motor_Run_CN6(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void Motor_Run_CN8(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* 查询指定电机是否仍在梯形运动中。非法编号返回 0。不改变加减速算法。 */
uint8_t Stepper_IsBusy(StepperMotorId motor_id);


#endif
