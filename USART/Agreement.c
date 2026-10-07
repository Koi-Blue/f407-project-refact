#include "stm32f4xx.h"
#include "Agreement.h"
#include "Serial.h"
#include <stdio.h>

#define FRAME_HEAD 0xAA
#define FRAME_TAIL 0xFF

CommData_t CommData;

void Agreement_Recv(void)
{
	Command_t cmd;
	uint32_t value;

	if (Serial_RxFlag != 1)
	{
		return;
	}

	cmd = (Command_t)Serial_RxPacket[0];

	switch (cmd)
	{
		case CMD_PATH_END:
			if (DataLength == 1)
			{
				printf("Path request end\r\n");
			}
			else
			{
				printf("Path end length error\r\n");
			}
			break;

		case CMD_FINE_END:
			if (DataLength == 1)
			{
				printf("Fine tune end\r\n");
			}
			else
			{
				printf("Fine end length error\r\n");
			}
			break;

		case CMD_GRAB_START:
			if (DataLength == 1)
			{
				printf("Grab start\r\n");
			}
			else
			{
				printf("Grab length error\r\n");
			}
			break;

		case CMD_PATH_RESULT:
			if ((DataLength == 2) &&(Serial_RxPacket[1] >= 1) &&(Serial_RxPacket[1] <= 14))
			{
				CommData.Path = (Path_t)Serial_RxPacket[1];
				printf("Path: %d\r\n", (int)CommData.Path);
			}
			else
			{
				printf("Path data error\r\n");
			}
			break;

		case CMD_NUMBER_RESULT:
			if ((DataLength == 2) &&(Serial_RxPacket[1] >= 1) &&(Serial_RxPacket[1] <= 3))
			{
				CommData.Number = (Number_t)Serial_RxPacket[1];
				printf("Number: %d\r\n", (int)CommData.Number);
			}
			else
			{
				printf("Number data error\r\n");
			}
			break;

		case CMD_OFFSET_RESULT:
			if (DataLength == 9)
			{
				value = ((uint32_t)Serial_RxPacket[1]) |
						((uint32_t)Serial_RxPacket[2] << 8) |
						((uint32_t)Serial_RxPacket[3] << 16) |
						((uint32_t)Serial_RxPacket[4] << 24);
				CommData.XOffset = (int32_t)value;

				value = ((uint32_t)Serial_RxPacket[5]) |
						((uint32_t)Serial_RxPacket[6] << 8) |
						((uint32_t)Serial_RxPacket[7] << 16) |
						((uint32_t)Serial_RxPacket[8] << 24);
				CommData.YOffset = (int32_t)value;

				printf("Offset: X=%ld, Y=%ld, unit=0.01 pixel\r\n",
						(long)CommData.XOffset, (long)CommData.YOffset);
			}
			else
			{
				printf("Offset length error\r\n");
			}
			break;

		default:
			printf("Unknown CMD: 0x%02X\r\n", (unsigned int)cmd);
			break;
	}

	Serial_RxFlag = 0;
}

void Agreement_Send(Command_t cmd)
{
	uint8_t packet[4];

	switch (cmd)
	{
		case CMD_PATH_START:
		case CMD_FINE_START:
		case CMD_RECOGNIZE_START:
			break;

		default:
			return;
	}

	packet[0] = FRAME_HEAD;
	packet[1] = 0x01;
	packet[2] = (uint8_t)cmd;
	packet[3] = FRAME_TAIL;
	USART2_Send_ArrayU8(packet, 4);
}
