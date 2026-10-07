#ifndef __SERVO_H_
#define __SERVO_H_

extern uint8_t Servo_State_1;
extern uint8_t Servo_State_2;

void PWM_Init(void);
void Servo_SetAngle1(float Angle)	;
void Servo_SetAngle2(float Angle)	;

#endif
