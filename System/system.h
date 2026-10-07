#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "stm32f4xx.h"                  // Device header

#include "math.h"
#include "sys.h"
#include "delay.h"
#include "Timer.h"

/* 历史任务旗标（rcount、stepPosition、Stop_flag、Grab/Place 等）在本仓库无定义、无引用，已删除 extern。 */

void System_Init(void);

#endif
