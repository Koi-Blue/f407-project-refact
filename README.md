# f407-project-refact

基于 **STM32F407VGTx** 的 Keil µVision 工程，面向底盘运动控制（步进电机、达妙电机、舵机）与串口协议通信。

## 硬件与工具链

| 项 | 说明 |
| --- | --- |
| 芯片 | STM32F407VGTx（Cortex-M4 + FPU） |
| IDE | Keil µVision（uVision 工程：`Project.uvprojx`） |
| 编译器 | ARM Compiler 5（ARMCC，工程内为 V5.06 update 7） |
| 设备包 | Keil.STM32F4xx_DFP（工程记录为 2.17.1） |
| 外设库 | ST 标准外设库（`Library/`），宏定义 `STM32F40_41xxx`、`USE_STDPERIPH_DRIVER` |

## 快速打开与编译

1. 用 Keil µVision 打开根目录下的 `Project.uvprojx`。
2. 确认已安装 STM32F4xx Device Family Pack（与工程 PackID 匹配或更新）。
3. 选择目标 **Target 1**，执行 Rebuild。
4. 输出目录为 `Objects/`（已由 `.gitignore` 忽略，无需提交构建产物）。

下载/调试按你本机调试器（如 ST-Link）在 Options for Target → Debug / Utilities 中配置即可。

## 目录结构

```
f407-project-refact/
├── Project.uvprojx      # Keil 工程
├── User/                # 入口与中断、系统时钟相关
│   └── main.c
├── Car_Move/            # 场地路径与机构动作（Chassis / Mechanism）
├── Stepper_Motor/       # 步进电机初始化、运行与底盘移动
├── DM_Motor/            # 达妙电机初始化、运行与串口侧
├── Servo/               # 舵机控制
├── USART/               # 串口与通信协议（Serial / Agreement）
├── System/              # 延时、定时器、系统初始化等
├── Start/               # 启动文件与 CMSIS 核心头文件
├── Library/             # STM32F4 标准外设库源码
├── Objects/             # 编译输出（不入库）
└── Listings/            # 列表文件（不入库）
```

## 模块说明（简要）

- **User**：`main` 中完成 `System_Init`、步进电机初始化、`USART2`（115200）初始化；主循环预留底盘/电机与协议接收调用。
- **Car_Move**：场地路径点运动接口（如 `Car_Path01` …）与机构相关逻辑。
- **Stepper_Motor**：步进电机初始化与运行，以及 `Chassis_Move` 底盘移动。
- **DM_Motor**：达妙电机相关初始化、运行与串口交互。
- **Servo**：舵机驱动。
- **USART**：`USART2` 收发与 `Agreement` 协议层。
- **System / Start / Library**：板级支持、启动与标准库。

## 仓库约定

- 请勿提交密钥、本机调试口令或私有配置。
- `Objects/`、`Listings/`、`*.uvguix.*`、构建日志等已忽略；克隆后本地 Rebuild 即可生成。

## 许可

未单独声明许可时，默认仅供本项目协作使用。如需开源协议，请在仓库中补充 `LICENSE`。
