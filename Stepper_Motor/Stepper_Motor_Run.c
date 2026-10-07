#include "stm32f4xx.h"
#include "Stepper_Motor_Init.h"
#include "Stepper_Motor_Run.h"
#include <math.h>

static StepperRampData srd[STEPPER_MOTOR_COUNT] = {0};

static void Stepper_SetDirection(StepperMotorId motor_id, StepperDirection direction)
{
    GPIO_TypeDef *dir_port;
    uint16_t dir_pin;

    switch (motor_id)
    {
        case STEPPER_CN1:
            dir_port = GPIOB;
            dir_pin = GPIO_Pin_1;
            break;
        case STEPPER_CN2:
            dir_port = GPIOB;
            dir_pin = GPIO_Pin_11;
            break;
        case STEPPER_CN3:
            dir_port = GPIOE;
            dir_pin = GPIO_Pin_10;
            break;
        case STEPPER_CN4:
            dir_port = GPIOE;
            dir_pin = GPIO_Pin_15;
            break;
        case STEPPER_CN6:
            dir_port = GPIOC;
            dir_pin = GPIO_Pin_3;
            break;
        case STEPPER_CN8:
            dir_port = GPIOC;
            dir_pin = GPIO_Pin_5;
            break;
        default:
            return;
    }

    //CCW输出高电平，CW输出低电平
    if (direction == STEPPER_CCW)
    {
        GPIO_SetBits(dir_port, dir_pin);
    }
    else
    {
        GPIO_ResetBits(dir_port, dir_pin);
    }
}

static void Stepper_SetARR(StepperMotorId motor_id, uint32_t arr_value)
{
    if (arr_value == 0)      //防止period-1发生下溢
    {
        return;
    }
    switch (motor_id)
    {
        case STEPPER_CN1:
        case STEPPER_CN2:
        case STEPPER_CN3:
        case STEPPER_CN4:
            TIM_SetAutoreload(TIM2,arr_value - 1);         
            break;
        case STEPPER_CN6:
            TIM_SetAutoreload(TIM3,arr_value - 1);
            break;
        case STEPPER_CN8:
            TIM_SetAutoreload(TIM4,arr_value - 1);
            break;
        default:
            break;
    }
}

static void Stepper_SetCCR(StepperMotorId motor_id, uint32_t ccr_value)
{
    switch (motor_id)
    {
        case STEPPER_CN1:
            TIM_SetCompare1(TIM2,ccr_value);
            break;
        case STEPPER_CN2:
						TIM_SetCompare2(TIM2,ccr_value);
            break;
        case STEPPER_CN3:
            TIM_SetCompare3(TIM2,ccr_value);
            break;
        case STEPPER_CN4:
						TIM_SetCompare4(TIM2,ccr_value);
            break;
        case STEPPER_CN6:
            TIM_SetCompare1(TIM3,ccr_value);
            break;
        case STEPPER_CN8:
            TIM_SetCompare1(TIM4,ccr_value);
            break;
        default:
            break;
    }
}

static void StepCounter(StepperMotorId motor_id,StepperDirection inc)
{
    if (inc == STEPPER_CCW)
    {
        srd[motor_id].position++;
    }
    else
    {
        srd[motor_id].position--;
    }
}

static void Stepper_ChannelCmd(StepperMotorId motor_id,uint16_t state)
{
    switch (motor_id)
    {
        case STEPPER_CN1:
            TIM_CCxCmd(TIM2,TIM_Channel_1,state);
            break;
        case STEPPER_CN2:
            TIM_CCxCmd(TIM2,TIM_Channel_2,state);
            break;
        case STEPPER_CN3:
            TIM_CCxCmd(TIM2,TIM_Channel_3,state);
            break;
        case STEPPER_CN4:
            TIM_CCxCmd(TIM2,TIM_Channel_4,state);
            break;
        case STEPPER_CN6:
            TIM_CCxCmd(TIM3,TIM_Channel_1,state);
            break;
        case STEPPER_CN8:
            TIM_CCxCmd(TIM4,TIM_Channel_1,state);
            break;
        default:
            break;
    }
}

static void Motor_Run(StepperMotorId motor_id, int32_t step, uint32_t accel, uint32_t decel,uint32_t speed)
{
    uint32_t max_s_lim;    //达到最大速度需要的步数 
    uint32_t accel_lim;    //根据总步数计算的加速步数
    uint32_t step_abs;     // step的绝对值 

    StepperRampData *motor_srd;
    
    if (motor_id >= STEPPER_MOTOR_COUNT)     // 检查电机编号 
    {
        return;
    }
    motor_srd = &srd[motor_id];        //取得当前电机的梯形运行数据 

    if ((step == 0) ||(accel == 0) ||(decel == 0) ||(speed == 0))     //step为0时无任务，accel/decel会在梯形算法中做除数
    {
        return;
    }
		
    if (motor_srd->busy != 0)      //即之前的Motor_X_State=1，表示仍在运行
    {
        return;
    }

		//判断方向，并对step_abs赋值
    if (step > 0)
    {
        motor_srd->dir = STEPPER_CCW;
        step_abs = (uint32_t)step;
    }
    else
    {
        motor_srd->dir = STEPPER_CW;
        step_abs = (uint32_t)(-(int64_t)step);  //先扩展为int64_t再求绝对值，避免step为最小负数时直接取反溢出
    }

    Stepper_SetDirection(motor_id,motor_srd->dir);   //设置该电机的DIR引脚


    //初始化本次运动数据
    //position是累计位置，不在这里清零
    motor_srd->busy = 1;
    motor_srd->step_count = 0;
    motor_srd->rest = 0;
    motor_srd->last_accel_delay = 0;


     //只运行一步时，不需要完整的梯形曲线
    if (step_abs == 1)
    {
        motor_srd->accel_count = -1;
        motor_srd->run_state = STEPPER_DECEL;
        motor_srd->step_delay = ARR;
    }
    else
    {
        //计算最小时间间隔--最大速度
        motor_srd->min_delay = (uint32_t)(A_T_x10 / speed);

        if (motor_srd->min_delay == 0) {motor_srd->min_delay = 1;}

        //计算时间间隔--启动速度
        motor_srd->step_delay =(uint32_t)((T1_FREQ_148 *sqrt(A_SQ / accel)) / 10);

        if (motor_srd->step_delay == 0)	{motor_srd->step_delay = 1;}

        //计算达到最大速度用的步数
        max_s_lim =(uint32_t)(((float)speed * (float)speed) / (A_x200 * accel / 10));
				
				if (max_s_lim == 0)	{max_s_lim = 1;}

        //计算开始减速的步数
        accel_lim =(uint32_t)(((uint64_t)step_abs * decel) /(accel + decel));

        if (accel_lim == 0)	{accel_lim = 1;}


        //判断能否到达最大速度 即 是走三角状还是梯形
        if (accel_lim <= max_s_lim)                 //没跑到
        {
            motor_srd->decel_count = (int32_t)accel_lim - (int32_t)step_abs;
        }
        else                                        //跑到
        {
            motor_srd->decel_count =-(int32_t)(((uint64_t)max_s_lim * accel) /decel );
        }

        if (motor_srd->decel_count == 0)	{motor_srd->decel_count = -1;}


        //计算减速开始位置
        motor_srd->decel_position =(uint32_t)((int32_t)step_abs + motor_srd->decel_count );


        //判断启动状态 直接匀速 还是 先加速后匀速
        if (motor_srd->step_delay <=motor_srd->min_delay)      //启动速度大于最大速度
        {
            motor_srd->step_delay = motor_srd->min_delay;
            motor_srd->last_accel_delay = motor_srd->min_delay;

            motor_srd->run_state =STEPPER_RUN;
        }
        else
        {
            motor_srd->run_state =STEPPER_ACCEL;
        }
        motor_srd->accel_count = 0;
    }

    Stepper_SetARR(motor_id, motor_srd->step_delay );
    Stepper_SetCCR(motor_id,motor_srd->step_delay / 2 );

    switch (motor_id)      //根据电机编号开启对应的定时器通道，然后启动相应的定时器
    {
        case STEPPER_CN1:
            TIM_CCxCmd(TIM2, TIM_Channel_1, TIM_CCx_Enable);
            TIM_Cmd(TIM2, ENABLE);
            break;
        case STEPPER_CN2:
            TIM_CCxCmd(TIM2, TIM_Channel_2,TIM_CCx_Enable );
            TIM_Cmd(TIM2, ENABLE);
            break;
        case STEPPER_CN3:
            TIM_CCxCmd(TIM2,TIM_Channel_3,TIM_CCx_Enable);
            TIM_Cmd(TIM2, ENABLE);
            break;
        case STEPPER_CN4:
            TIM_CCxCmd(TIM2,TIM_Channel_4,TIM_CCx_Enable);
            TIM_Cmd(TIM2, ENABLE);
            break;
        case STEPPER_CN6:
            TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
            TIM_Cmd(TIM3, ENABLE);
            break;
        case STEPPER_CN8:
            TIM_CCxCmd(TIM4,TIM_Channel_1,TIM_CCx_Enable);
            TIM_Cmd(TIM4, ENABLE);
            break;
        default:
            motor_srd->busy = 0;
            motor_srd->run_state = STEPPER_STOP;
            break;
    }
}

void Motor_Run_CN1(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run(STEPPER_CN1, step, accel, decel, speed);
}

void Motor_Run_CN2(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run(STEPPER_CN2, step, accel, decel, speed);
}

void Motor_Run_CN3(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run(STEPPER_CN3, step, accel, decel, speed);
}

void Motor_Run_CN4(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run(STEPPER_CN4, step, accel, decel, speed);
}

void Motor_Run_CN6(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run(STEPPER_CN6, step, accel, decel, speed);
}

void Motor_Run_CN8(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
    Motor_Run(STEPPER_CN8, step, accel, decel, speed);
}

uint8_t Stepper_IsBusy(StepperMotorId motor_id)
{
    if (motor_id >= STEPPER_MOTOR_COUNT)
    {
        return 0;
    }

    /* busy 由 Motor_Run 置位、定时器中断清零，这里只读。 */
    return srd[motor_id].busy;
}

static void Speed_Decision(StepperMotorId motor_id)
{
    uint32_t new_step_delay = 0;  //下一个脉冲的时间间隔

    StepperRampData *motor_srd;
    TIM_TypeDef *TIMx;

    if (motor_id >= STEPPER_MOTOR_COUNT)
    {
        return;
    }
    motor_srd = &srd[motor_id];

    switch (motor_id)       //根据电机编号选择定时器
    {
        case STEPPER_CN1:
        case STEPPER_CN2:
        case STEPPER_CN3:
        case STEPPER_CN4:
            TIMx = TIM2;
            break;
        case STEPPER_CN6:
            TIMx = TIM3;
            break;
        case STEPPER_CN8:
            TIMx = TIM4;
            break;
        default:
            return;
    }

    Stepper_SetARR(motor_id,motor_srd->step_delay);
    Stepper_SetCCR(motor_id,motor_srd->step_delay / 2);
    TIM_Cmd(TIMx, ENABLE);

        switch (motor_srd->run_state)
        {
            //停止状态
            case STEPPER_STOP:
                motor_srd->step_count = 0;
                motor_srd->rest = 0;

                //关闭PWM输出
                Stepper_ChannelCmd(motor_id,TIM_CCx_Disable);
                motor_srd->busy = 0;
                break;

            //加速状态
            case STEPPER_ACCEL:
                StepCounter(motor_id,motor_srd->dir);
						
                //开启PWM输出
                Stepper_ChannelCmd(motor_id,TIM_CCx_Enable);
                TIM_Cmd(TIMx, ENABLE);

                motor_srd->step_count++;
                motor_srd->accel_count++;
						
                //由0往上加到减速点
                new_step_delay =motor_srd->step_delay -((2 * motor_srd->step_delay +motor_srd->rest) /(4 * motor_srd->accel_count +1));

                motor_srd->rest =(2 * motor_srd->step_delay +motor_srd->rest) %(4 * motor_srd->accel_count +1);

                //判断1：是否走到减速点---之后开始减速
                if (motor_srd->step_count >=motor_srd->decel_position)
                {
										//让accel_count变成负数，从负数往上加
                    motor_srd->accel_count =motor_srd->decel_count;
                    motor_srd->run_state =STEPPER_DECEL;
									
                }
                //判断2：是否走到最大速度点---之后开始匀速
                else if (new_step_delay <=motor_srd->min_delay)
                {
                    motor_srd->last_accel_delay =new_step_delay;
                    new_step_delay =motor_srd->min_delay;
                    motor_srd->rest = 0;
                    motor_srd->run_state =STEPPER_RUN;
                }

                break;


            //匀速状态
            case STEPPER_RUN:
                StepCounter(motor_id,motor_srd->dir);

                //开启PWM输出
                Stepper_ChannelCmd(motor_id,TIM_CCx_Enable);
                TIM_Cmd(TIMx, ENABLE);
                motor_srd->step_count++;
               
                //固定为最高速度运动
                new_step_delay =motor_srd->min_delay;

                //判断：是否走到减速点
                if (motor_srd->step_count >=motor_srd->decel_position)
                {
                    motor_srd->accel_count =motor_srd->decel_count;
                 
                    //让减速从加速的最后一刻开始，实现平滑减速
                    new_step_delay =motor_srd->last_accel_delay;
                    motor_srd->run_state =STEPPER_DECEL;
                }

                break;


            //减速状态
            case STEPPER_DECEL:
                StepCounter(motor_id,motor_srd->dir);

                //开启PWM输出
                Stepper_ChannelCmd(motor_id,TIM_CCx_Enable);
                TIM_Cmd(TIMx, ENABLE);
                motor_srd->step_count++;

                // 由负数加到0
                motor_srd->accel_count++;
						
                new_step_delay =motor_srd->step_delay -((2 * motor_srd->step_delay +motor_srd->rest) /(4 * motor_srd->accel_count +1));

                motor_srd->rest =(2 * motor_srd->step_delay +motor_srd->rest) %(4 * motor_srd->accel_count +1);

                //判断：减速部分是否走完
                if (motor_srd->accel_count >= 0)
                {
                    motor_srd->run_state =STEPPER_STOP;
                }

                break;
            default:
                motor_srd->run_state =STEPPER_STOP;

                break;
        }
        
    //存入给下次中断使用。
    motor_srd->step_delay =new_step_delay;
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

        if (srd[STEPPER_CN1].busy != 0)
        {
            Speed_Decision(STEPPER_CN1);
        }
        if (srd[STEPPER_CN2].busy != 0)
        {
            Speed_Decision(STEPPER_CN2);
        }
        if (srd[STEPPER_CN3].busy != 0)
        {
            Speed_Decision(STEPPER_CN3);
        }
        if (srd[STEPPER_CN4].busy != 0)
        {
            Speed_Decision(STEPPER_CN4);
        }

        if ((srd[STEPPER_CN1].busy == 0) &&(srd[STEPPER_CN2].busy == 0) &&(srd[STEPPER_CN3].busy == 0) &&(srd[STEPPER_CN4].busy == 0))
        {
            TIM_Cmd(TIM2, DISABLE);
        }
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);

        if (srd[STEPPER_CN6].busy != 0)
        {
            Speed_Decision(STEPPER_CN6);
        }
        if (srd[STEPPER_CN6].busy == 0)
        {
            TIM_Cmd(TIM3, DISABLE);
        }
    }
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);

        if (srd[STEPPER_CN8].busy != 0)
        {
            Speed_Decision(STEPPER_CN8);
        }
        if (srd[STEPPER_CN8].busy == 0)
        {
            TIM_Cmd(TIM4, DISABLE);
        }
    }
}
