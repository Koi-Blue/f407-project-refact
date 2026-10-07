#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "stm32f4xx.h"                  // Device header

#include "system.h"
#include "math.h"
#include "sys.h"
#include "delay.h"
#include "Timer.h"



extern float  rcount;
extern float Receive_Roll, Receive_Pitch, Receive_Yaw;
extern int  Turn_Step, stepPosition, stepPosition_Y, stepPosition_Z, stepPosition_Turn_Z;  //步数计数
extern uint8_t 	map_flag, Place_Select, send_filish_flag, Key_number;
extern uint8_t	Stop_flag, Stop_flag_Y, Stop_flag_Z, Stop_flag_Turn_Z;
extern uint8_t  task_flag, Place_task_flag, Grab_task_flag, start_finish_flag, Scan_filish_flag, Capture_filish_flag;
extern char  Grab_Action_Command, Place_Action_Command;
extern uint8_t Place_flag, Place_filish_flag, Place_task_filish_flag, Place_action_filish_flag; 
extern uint8_t Grab_flag,  Grab_filish_flag,  Grab_task_filish_flag,  Grab_action_filish_flag;
extern int  i, a, b, c, d;

void System_Init(void);

#endif
