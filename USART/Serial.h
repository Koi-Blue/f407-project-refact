#ifndef __SERIAL_H_
#define __SERIAL_H_

#include "stm32f4xx.h"

/* 收发载荷缓冲区长度。偏移结果帧 DataLength==9，与该长度一致。 */
#define SERIAL_PACKET_SIZE 9u

extern uint8_t Serial_TxPacket[SERIAL_PACKET_SIZE];
/* 下列变量由 USART2 中断写入、主循环读出，必须为 volatile。 */
extern volatile uint8_t Serial_RxPacket[SERIAL_PACKET_SIZE];
extern volatile uint8_t Serial_RxFlag;
extern volatile uint8_t DataLength;

void USART2_Init(u32 bound);
void USART2_Send_U8(uint8_t ch);
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);

#endif
