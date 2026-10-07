#ifndef __AGREEMENT_H_
#define __AGREEMENT_H_

#include <stdint.h>

typedef enum
{
	CMD_PATH_START = 0x10,
	CMD_PATH_END = 0x11,
	CMD_FINE_START = 0x20,
	CMD_FINE_END = 0x21,
	CMD_RECOGNIZE_START = 0x30,
	CMD_GRAB_START = 0x31,
	CMD_PATH_RESULT = 0x41,
	CMD_NUMBER_RESULT = 0x42,
	CMD_OFFSET_RESULT = 0x43
} Command_t;

typedef enum
{
	PATH_NONE = 0,
	PATH_01 = 1,
	PATH_02,
	PATH_03,
	PATH_04,
	PATH_05,
	PATH_06,
	PATH_07,
	PATH_08,
	PATH_09,
	PATH_10,
	PATH_11,
	PATH_12,
	PATH_13,
	PATH_14
} Path_t;

typedef enum
{
	NUMBER_NONE = 0,
	NUMBER_1 = 1,
	NUMBER_2,
	NUMBER_3
} Number_t;

typedef struct
{
	Path_t Path;
	Number_t Number;
	int32_t XOffset;
	int32_t YOffset;
} CommData_t;

extern CommData_t CommData;

void Agreement_Recv(void);
void Agreement_Send(Command_t cmd);

#endif
