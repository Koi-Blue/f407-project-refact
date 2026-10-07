# 模块接口清单

本文只盘点当前仓库的对外接口和调用关系，供下一步统一 API 使用。本步不改运动算法、不合并路径表、不抽 BSP、不引入状态机。

读码范围：`User/`、`System/`、`Stepper_Motor/`、`DM_Motor/`、`Servo/`、`USART/`、`Car_Move/`，以及 `Project.uvprojx` 的源文件组。标准外设库与 CMSIS 只在「未编入工程」和中断向量处点名，不逐个列出库函数。

结论先说：

- 应用层没有「下层去包含上层」的头文件倒置。乱的是全局变量直读、两套底盘命名，以及机构层和路径层走了不同的步进入口。
- 达妙的 `DM_Motor_Run.c` / `DM_Motor_Serial.c` 原先在目录里、却不在 Keil 工程里。`DM_Motor_Init()` 一调用就会缺 `DM_Motor_Data_Reset`。本步已把这四个文件编入 `DM_Motor` 组。`main` 仍不调用达妙和舵机初始化。
- `system.h` 里一批任务旗标只有 `extern`、全仓库没有定义；`sys.h` 里一批正点原子函数同样没有定义。本步已删声明。
- 步进忙标志原来只放在 `static` 的 `srd[].busy` 里，外面查不到。本步增加 `Stepper_IsBusy()`，不改梯形加减速。

## 分层和实际调用

```
User/main
  ├─ System_Init ───────── delay_init、NVIC 分组
  ├─ Stepper_Motor_Init
  └─ USART2_Init ───────── USART2_IRQHandler（Serial.c）
                            └─ 写 Serial_RxPacket / DataLength / Serial_RxFlag
                                 └─ Agreement_Recv（main 里仍是注释）

Car_Move/Chassis.c          Car_Path01…14（当前无调用者）
  └─ Chassis_Move_X / Y
       └─ Motor_Run_CN1…CN4

Car_Move/Mechanism.c        Car_Lift / Car_Extend（当前无调用者）
  └─ 直接 Motor_Run_CN6 / CN8

Stepper_Motor/Chassis_Move.c
  └─ Chassis_Move_Turn（当前无调用者）→ Motor_Run_CN1…CN4

DM_Motor（main 未调用）
  DM_Motor_Init → DM_Motor_Data_Reset
  DM_Motor_Run_Enable / Disable / MIT → 组帧 + CAN1 发送
  CAN1_RX0_IRQHandler → DM_Motor_Run_Receive → 写 DM_Motor_Data

Servo（main 未调用）
  PWM_Init、Servo_SetAngle1 / 2
```

`main` 里被注释、尚未执行的调用：`Chassis_Move_X/Y`、`Motor_Run_CN1/2/3/4/6/8`、`Agreement_Recv`。达妙和舵机连注释调用都没有，本步只在 `main` 里说明了建议的调用时机。

## 各模块：建议公开、当前导出、实际调用者

「建议公开」是按 Init / 命令 / 状态查询收口后应该留给别的模块用的符号。「当前导出」是头文件里现在就能看见的符号。内部 `static` 函数不列入导出。

### User

| 建议公开 | 当前导出 | 实际调用者 |
| --- | --- | --- |
| `main` 只做初始化和调度，不把电机、协议头直接铺开 | `main` | 启动文件 |
| 异常处理留在 `stm32f4xx_it.c` | `NMI_Handler`、`HardFault_Handler`、`MemManage_Handler`、`BusFault_Handler`、`UsageFault_Handler`、`SVC_Handler`、`DebugMon_Handler`、`PendSV_Handler`、`SysTick_Handler`（空） | 启动向量；`SysTick_Handler` 在 `SYSTEM_SUPPORT_OS==0` 时只有这一份空函数 |
| CMSIS 时钟 | `SystemInit`、`SystemCoreClockUpdate`、`SystemCoreClock` | 启动文件 / 库 |

`stm32f4xx_it.c` 没有外设中断。外设 IRQ 写在各模块 `.c` 里，见下文。

### System

| 符号 | 建议 | 当前谁在用 |
| --- | --- | --- |
| `System_Init` | 公开。板级一次性初始化 | `main`。内部调用 `delay_init(168)` 和 `NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2)` |
| `delay_init` / `delay_ms` / `delay_us` | 公开 | 只有 `delay_init` 被 `System_Init` 调用。`delay_ms` / `delay_us` 无其它调用者 |
| `delay_xms` | 保持文件内部。未放进头文件，也未标 `static` | 仅 `delay_ms` |
| `Timer7_Init`、`TIM7_IRQHandler` | 定时器模块的 Init。中断函数不必放头文件 | 无调用者。`TIM7_IRQHandler` 体是空的，定时器初始化后保持 `DISABLE` |
| `sys.h` 位带宏 `PAout`…`PGin` | 若继续当 GPIO 捷径可保留；当前业务代码没用 | 无 `.c` 引用 |
| `WFI_SET`、`INTX_DISABLE`、`INTX_ENABLE`、`MSR_MSP`、`Stm32_Clock_Init`、`JTAG_Set`、`OLED_SHOW` | 不应公开 | 本步已从 `sys.h` 删除。`sys.c` 没有这些函数 |

`system.h` 仍是伞头：包含 `math.h`、`sys.h`、`delay.h`、`Timer.h`。`Timer.c` 反过来包含 `system.h`。能编译，但是系统层和定时器层缠在一起。本步去掉了 `system.h` 对自己的包含。

### Stepper_Motor

| 建议公开 | 当前导出 | 实际调用者 |
| --- | --- | --- |
| `Stepper_Motor_Init` | 同左 | `main` |
| 一条运动命令入口（下一阶段再收成 `Stepper_Move(id, …)`） | `Motor_Run_CN1/2/3/4/6/8` | `Chassis_Move.c` 调用 CN1–CN4；`Mechanism.c` 调用 CN6、CN8；`main` 里有注释掉的直接调用 |
| `Stepper_IsBusy(id)` | 本步新增 | 尚无调用者。给上层在发下一条命令前查询 |
| 类型：`StepperMotorId`、`StepperDirection`、`StepperRunState` | 同左，另外还导出了 `StepperRampData` | 类型只在本模块 `.c` 使用。`StepperRampData` 的实例是 `static srd[]`，外部拿不到对象，却能看见结构体布局 |
| 速度宏 `T1_FREQ`、`SPR`、`A_T_x10` 等 | 写在 `Stepper_Motor_Run.h` | 仅 `Stepper_Motor_Run.c`。这些宏依赖 `Stepper_Motor_Init.h` 的 `TIM_PRESCALER`、`TIM_PERIOD`、`ARR`，但 `Run.h` 自己没有包含 `Init.h`。现在能编过，是因为 `.c` 先包含了 `Init.h` |

内部函数（不建议公开）：`Motor_Run`、`Speed_Decision`、`Stepper_SetDirection`、`Stepper_SetARR`、`Stepper_SetCCR`、`StepCounter`、`Stepper_ChannelCmd`。

中断：`TIM2_IRQHandler`、`TIM3_IRQHandler`、`TIM4_IRQHandler` 定义在 `Stepper_Motor_Run.c`，没有放进头文件。CN1–CN4 共用 TIM2，各自仍有独立 `busy`。

`Stepper_IsBusy` 只读已有的 `srd[id].busy`：`Motor_Run` 在启动时置 1，`Speed_Decision` 在 `STEPPER_STOP` 时清 0。非法编号返回 0。运动被拒绝的条件（步数为 0、加速度/减速度/速度为 0、已经在忙）保持原样。

### Chassis_Move（目录在 `Stepper_Motor/`，不是 `Car_Move/`）

| 建议公开 | 当前导出 | 实际调用者 |
| --- | --- | --- |
| `Chassis_Move_X` / `Y` / `Turn` 作为底盘命令 | 同左 | `Car_Move/Chassis.c` 只调用 X 和 Y。`Turn` 无调用者。`main` 注释里调用了 X/Y |
| 忙查询可以暂不新做，用四路 `Stepper_IsBusy(STEPPER_CN1…CN4)` 组合 | 无单独的 `Chassis_IsBusy` | — |

正方向约定写在 `Chassis_Move.h`：X 正方向为右，Y 正方向为前，Turn 正方向为右转。

### Car_Move

| 建议公开 | 当前导出 | 实际调用者 |
| --- | --- | --- |
| 路径命令 `Car_Path01`…`Car_Path14` | 同左，注释写了场地点 1–9 和 A/B 的方向 | 无。没有任何 `.c` 调用，`main` 里也没有注释调用 |
| 机构命令 `Car_Lift`（CN6）、`Car_Extend`（CN8） | 同左 | 无 |

路径函数体都是单次 `Chassis_Move_X` 或 `Chassis_Move_Y`，没有多段路径，也没有「走到了没有」的查询。这是下一步路径表要处理的内容，本步不动。

### DM_Motor

| 建议公开 | 当前导出 | 实际调用者 |
| --- | --- | --- |
| `DM_Motor_Init` | 同左 | 无。`main` 未调用。函数内部会 `DM_Motor_Data_Reset()`，并配置 CAN1（PA11/PA12，1 Mbps）和 `CAN1_RX0` 中断 |
| 命令：`DM_Motor_Run_Enable`、`DM_Motor_Run_Disable`、`DM_Motor_Run_MIT` | 同左 | 无外部调用者 |
| 状态查询：建议以后加只读接口，不要让上层直接改 `DM_Motor_Data` | 全局 `DM_Motor_Data`；`DM_Motor_Run_Receive` 也在头文件里 | `DM_Motor_Run_Receive` 只被本文件的 `CAN1_RX0_IRQHandler` 调用。反馈由 `DM_Motor_Feedback_Decode` 写入 `DM_Motor_Data` |
| 组帧属于模块内部 | `DM_Motor_Data_Reset`、`DM_Motor_Enable_Pack`、`DM_Motor_Disable_Pack`、`DM_Motor_MIT_Pack`、`DM_Motor_Feedback_Decode`、`float_to_uint`、`uint_to_float`，以及 `DM_MOTOR_ID`、量程宏 | `Data_Reset` 被 `DM_Motor_Init` 调用；Pack/Decode 被 `DM_Motor_Run.c` 调用；两个转换函数只在 `DM_Motor_Serial.c` 内部使用，却写在头文件里 |

`DM_Motor_CAN_Send` 是 `Run.c` 的 `static`，不公开。

启用达妙之前必须保证工程已包含 `DM_Motor_Run.c` 和 `DM_Motor_Serial.c`。否则 `DM_Motor_Init` 链接不到 `DM_Motor_Data_Reset`，接收中断也只会落到启动文件里的弱定义。本步已经把 `DM_Motor_Run.c/.h`、`DM_Motor_Serial.c/.h` 加进 `Project.uvprojx` 的 `DM_Motor` 组。`main` 仍不调用 `DM_Motor_Init()`。要启用时，放在 `System_Init()`、`Stepper_Motor_Init()` 之后、进入 `while` 之前；`DM_Motor_Run_Enable` 和 `DM_Motor_Run_MIT` 放到具体任务里。

### Servo

| 建议公开 | 当前导出 | 实际调用者 |
| --- | --- | --- |
| `PWM_Init` 作为 Init | 同左 | 无。`main` 未调用。TIM9，PE5/PE6，周期 20 ms |
| `Servo_SetAngle1`（转盘，360° 映射）、`Servo_SetAngle2`（爪子，270° 映射）作为命令 | 同左 | 无 |
| 若 `Servo_State_1/2` 表示到位或目标，应有查询函数去写它们 | `extern uint8_t Servo_State_1/2`，定义处只赋 0 | 无读写 |

### USART（Serial / Agreement）

| 建议公开 | 当前导出 | 实际调用者 |
| --- | --- | --- |
| `USART2_Init` | 同左。USART2，PD5/PD6，RXNE 中断 | `main` 以 115200 调用 |
| `USART2_Send_U8`、`USART2_Send_ArrayU8` | 同左 | `Agreement_Send` 使用数组发送；单字节发送只被数组发送使用。`fputc` 重定向到 USART2，没有放进头文件 |
| 收包查询，例如「取走一帧」 | 直接导出 `Serial_RxPacket[]`、`Serial_RxFlag`、`DataLength`，以及未被使用的 `Serial_TxPacket[]` | `USART2_IRQHandler` 写这三项；`Agreement_Recv` 读它们并清 `Serial_RxFlag`。`Agreement_Recv` 在 `main` 里仍是注释 |
| 协议类型与收发 | `Command_t`、`Path_t`、`Number_t`、`CommData_t`、`CommData`、`Agreement_Recv`、`Agreement_Send` | `Agreement_Send` 无调用者。`CommData` 只在 `Agreement_Recv` 里写入，无外部读取 |

帧格式是 `0xAA + 长度 + 载荷 + 0xFF`。载荷长度 1 用于路径结束、微调结束、抓取开始；长度 2 用于路径号和数字；长度 9 用于 X/Y 偏移。缓冲区长度是 `SERIAL_PACKET_SIZE`（9）。长度字节大于 9 时，中断里把 `DataLength` 钳到 9 并丢弃这一帧，不置 `Serial_RxFlag`，因此不会把截断数据当成合法偏移帧。写入 `Serial_RxPacket` 前还有下标上界判断。

`Agreement_Send` 只发出 `CMD_PATH_START`、`CMD_FINE_START`、`CMD_RECOGNIZE_START`，帧长固定 4 字节。

## 跨层和耦合

这些是事实，不是本步要改的结构：

1. **协议层直接摸串口全局量。** `Agreement.c` 包含 `Serial.h`，自己读 `Serial_RxFlag`、`DataLength`、`Serial_RxPacket`。方向是对的（协议在串口之上），缺的是收包函数，所以两边共用三块全局状态。
2. **机构层和路径层入口不一致。** `Car_Move/Chassis.c` 走 `Chassis_Move_X/Y`；`Car_Move/Mechanism.c` 跨目录直接叫 `Motor_Run_CN6/8`。没有下层包含上层。
3. **两套 Chassis。** `Car_Move/Chassis.*` 是场地路径，`Stepper_Motor/Chassis_Move.*` 是 XY 平移和旋转。同名不同职责。路径表合并留给后续步骤。
4. **`system.h` 伞包含。** 包含延时、定时器和 `math.h`。`Timer.c` 通过这把伞再包含自己的头。业务 `.c` 只要包含 `system.h` 就会看见定时器接口。
5. **中断服务函数不在 `stm32f4xx_it.c`。** `USART2_IRQHandler` 在 `Serial.c`，`TIM2/3/4_IRQHandler` 在 `Stepper_Motor_Run.c`，`CAN1_RX0_IRQHandler` 在 `DM_Motor_Run.c`，`TIM7_IRQHandler` 在 `Timer.c`。名字与 `startup_stm32f40_41xxx.s` 的向量一致，链接上没有第二份强定义。
6. **步进头文件宏依赖没有写在头里。** 见上表。只包含 `Stepper_Motor_Run.h` 再使用 `T1_FREQ` 一类宏会编译失败。
7. **达妙 Init 依赖组帧文件。** `DM_Motor_Init.c` 调用 `DM_Motor_Data_Reset()`。这是同一模块内的依赖，但 Init 头文件看不出来；工程若只加入 `DM_Motor_Init.c` 就会链接失败。本步已补齐源文件。
8. **`main` 同时看见底盘、单轴和协议头。** 当前真正执行的只有三行初始化，其余调用是注释。

## 僵尸声明

「僵尸声明」指头文件里有 `extern` 或函数原型，仓库里没有定义。有定义但没人调用的，单独列在后面，本步没有删，避免误伤占位状态。

### 本步已删除（无定义、无引用）

`system.h`：

- `rcount`
- `Receive_Roll`、`Receive_Pitch`、`Receive_Yaw`
- `Turn_Step`、`stepPosition`、`stepPosition_Y`、`stepPosition_Z`、`stepPosition_Turn_Z`
- `map_flag`、`Place_Select`、`send_filish_flag`、`Key_number`
- `Stop_flag`、`Stop_flag_Y`、`Stop_flag_Z`、`Stop_flag_Turn_Z`
- `task_flag`、`Place_task_flag`、`Grab_task_flag`、`start_finish_flag`、`Scan_filish_flag`、`Capture_filish_flag`
- `Grab_Action_Command`、`Place_Action_Command`
- `Place_flag`、`Place_filish_flag`、`Place_task_filish_flag`、`Place_action_filish_flag`
- `Grab_flag`、`Grab_filish_flag`、`Grab_task_filish_flag`、`Grab_action_filish_flag`
- `i`、`a`、`b`、`c`、`d`

`Timer.c` 里被注释掉的 TIM6 示例写过 `i++`。那段代码保持注释，没有恢复 `i`。

`sys.h`：`WFI_SET`、`INTX_DISABLE`、`INTX_ENABLE`、`MSR_MSP`、`Stm32_Clock_Init`、`JTAG_Set`、`OLED_SHOW`。`sys.c` 没有实现。

保留的 `extern` 都有定义：`Serial_TxPacket`、`Serial_RxPacket`、`Serial_RxFlag`、`DataLength`、`CommData`、`Servo_State_1`、`Servo_State_2`、`DM_Motor_Data`、`SystemCoreClock`。

### 有定义但当前没有调用者

这些不是僵尸声明，列出来是避免下一步误当成已经在用的 API：

- `Car_Path01`…`Car_Path14`、`Car_Lift`、`Car_Extend`
- `Chassis_Move_Turn`
- `DM_Motor_Init` 以及全部 `DM_Motor_Run_*` 命令
- `PWM_Init`、`Servo_SetAngle1`、`Servo_SetAngle2`；`Servo_State_1/2` 只有初值 0
- `Agreement_Send`；`Agreement_Recv` 仅出现在 `main` 注释里
- `Serial_TxPacket` 有定义和 `extern`，没有任何读写
- `Timer7_Init`
- `delay_ms`、`delay_us`（除了彼此调用和 `delay_init` 的初始化路径）
- `float_to_uint`、`uint_to_float` 只在 `DM_Motor_Serial.c` 内部使用

## 目录有、但工程未编入的文件

对照 `Project.uvprojx` 与磁盘上的 `.c/.h/.s`：

| 文件 | 本步之后 |
| --- | --- |
| `DM_Motor/DM_Motor_Run.c`、`DM_Motor_Run.h`、`DM_Motor_Serial.c`、`DM_Motor_Serial.h` | 原先未编入。本步已加入 `DM_Motor` 组。`.c` 的 `FileType` 为 1，会参与编译；`.h` 为 5，与工程里其它头文件一致 |
| `Library/stm32f4xx_fmc.c` | 仍然未编入。`stm32f4xx_fmc.h` 在 Library 组里，但 F407 工程实际打开的是 FSMC（`stm32f4xx_conf.h` 在 `STM32F40_41xxx` 下包含 `stm32f4xx_fsmc.h`）。本步不把 FMC 源文件加进工程 |

其余应用层 `.c/.h` 都已在对应组里。`keilkill.bat` 不是源文件。

## 主循环与中断共享的变量

| 变量 | 写入 | 读取 | 本步 |
| --- | --- | --- | --- |
| `Serial_RxFlag` | `USART2_IRQHandler` 置位；`Agreement_Recv` 清零 | `Agreement_Recv` | 改为 `volatile` |
| `DataLength` | 中断 | `Agreement_Recv`，以及中断状态机 | 改为 `volatile`，并钳位到 `SERIAL_PACKET_SIZE` |
| `Serial_RxPacket[]` | 中断 | `Agreement_Recv` | 改为 `volatile` |
| `srd[].busy` 等 | `Motor_Run` 与 `TIM2/3/4` 中断 | 中断，以及新的 `Stepper_IsBusy` | 结构体里 `busy`、`run_state`、`step_count`、`position`、`step_delay` 原本就是 `volatile`，未改算法 |
| `DM_Motor_Data` | `CAN1_RX0_IRQHandler` 路径上的解码函数；`DM_Motor_Data_Reset` 在初始化里写 | 当前没有主循环读取 | **未**改成 `volatile`。主循环还没读它。下一步若增加状态查询，共享字段要标 `volatile` |
| `Servo_State_1/2` | 只有定义时的初值 | 无 | 与中断无关，未改 |

`USART2_IRQHandler` 里的 `RxState`、`pRxPacket` 是中断内 `static`，主循环碰不到，不加 `volatile`。

## 下一阶段统一 API 的形态

只建议，本步不落地。每个执行模块收成三样：

| 形态 | 含义 | 现有最接近的符号 | 缺口 |
| --- | --- | --- | --- |
| Init | 时钟、引脚、外设，不做业务动作 | `System_Init`、`Stepper_Motor_Init`、`USART2_Init`、`DM_Motor_Init`、`PWM_Init`、`Timer7_Init` | 命名不统一（`PWM_Init`、`USART2_Init`、`Timer7_Init`）。`main` 尚未调用达妙和舵机 |
| 命令 | 非阻塞提交。忙则拒绝，不在命令里空等 | `Motor_Run_CNx`、`Chassis_Move_*`、`Car_Path*`、`Car_Lift/Extend`、`DM_Motor_Run_*`、`Servo_SetAngle*`、`Agreement_Send` | 步进是六套 CNx 函数；路径是 14 个薄封装；达妙组帧函数也露在头文件里 |
| 状态查询 | 忙、反馈、最近一帧 | `Stepper_IsBusy`（本步新增） | 底盘、机构、达妙、舵机、串口都没有查询函数。协议层还在直接读全局缓冲 |

建议的收口顺序，仍然不要在这一步做：

1. 串口提供「是否有帧 / 拷贝走一帧」，`Agreement` 不再直接读写 `Serial_RxFlag`。
2. 步进保留 `Motor_Run_CNx` 作为薄封装，内部走同一个 `Stepper_Move(StepperMotorId, …)`；上层用 `Stepper_IsBusy` 决定能不能发下一条。
3. 达妙对外只留 Init、Enable/Disable/MIT 和只读反馈；Pack/Decode/`float_to_uint` 收回 `.c`。
4. 舵机把 `Servo_State_*` 接到 `SetAngle` 上，或删掉这对空全局。
5. 路径表和状态机再往后放。那会改 `Car_Path*` 的组织方式，超出本步。

## 本步代码改动对照

| 改动 | 原因 |
| --- | --- |
| `Project.uvprojx` 的 `DM_Motor` 组加入 Run/Serial 的 `.c/.h` | 目录里有实现，工程原先只编译 `DM_Motor_Init.c`，达妙模块链接不完整 |
| `USART/Serial.c` 钳位 `DataLength`，写入前检查下标 | 长度字节来自线路，原逻辑会按该长度写入只有 9 字节的 `Serial_RxPacket` |
| `Serial_RxFlag`、`DataLength`、`Serial_RxPacket` 改为 `volatile` | 中断和主循环（`Agreement_Recv`）共享 |
| 删除 `system.h`、`sys.h` 中无定义声明 | 避免把不存在的符号当成可用 API |
| 新增 `Stepper_IsBusy` | 忙标志原来是文件内 `static`，模块外不能查询 |
| `main` 只增加注释 | 不在没有任务逻辑时打开达妙和舵机 |
