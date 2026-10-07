#ifndef __SERIAL_H_
#define __SERIAL_H_

extern uint8_t Serial_TxPacket[9];
extern uint8_t Serial_RxPacket[9];
extern uint8_t Serial_RxFlag;
extern uint8_t DataLength;

void USART2_Init(u32 bound);
void USART2_Send_U8(uint8_t ch);
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);

#endif
