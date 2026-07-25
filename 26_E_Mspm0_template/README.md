# 26_E_Mspm0_template —— TI 杯竞赛小车控制系统

## 项目概述

本项目是基于 **TI MSPM0G3507**（Cortex-M0+, 80MHz, 128KB Flash, 32KB RAM）的竞赛小车完整控制系统，适用于 TI 杯全国大学生电子设计竞赛。

**主要功能模块：**

- 灰度巡线（8/12/16 通道可选）
- 双轮直流电机速度闭环控制（支持 AT8236 / TB6612 驱动）
- IMU 姿态解算与偏航角 PID 控制（支持 ICM42688 / IMU660RA）
- 无刷云台电机 CANFD 控制
- LoRa 无线数传模块
- VOFA+ 在线 PID 调参
- HMI 串口触控屏任务调度
- K230 AI 视觉模块
- 舵机、步进电机、继电器、激光等外设控制

**开发环境：** Keil MDK 5.39+，TI mspm0_sdk_2_02_00_05，SysConfig 1.20+

**文件编码：** GB2312/GBK（源文件中含有中文注释，请勿更改编码格式）

---

## 一、硬件平台与接线

### 1.1 核心板

LP_MSPM0G3507 LaunchPad 开发板（LQFP-64 封装）

### 1.2 完整外设接线表

| 外设 | 接口 | 发送/输出引脚 | 接收/输入引脚 | 备注 |
|---|---|---|---|---|
| 调试/VOFA | UART0 | PA10 (TX) | PA11 (RX) | 115200 baud |
| HMI 串口屏 | UART1 | PA17 (TX) | PA18 (RX) | 115200 baud，淘晶驰/卓岚协议 |
| DL-20 无线串口通信（多车通信，已经配置为广播通信，假如有多车） | UART2 | PA21 (TX) | PB16 (RX) | 9600 baud |
| DL-20 无线串口通信（已经配置为点对点通信，使用鑲嵌金边的ttl转usb） | UART0 | 115200 baud |
| 香橙派 视觉 | UART3 | PB2 (TX) | PB3 (RX) | 115200 baud（需要确认） |
| 云台电机 | CANFD0 | PA12 (TX) | PA13 (RX) | QGimbal 无刷云台 |
| IMU (SPI) | SPI0 | PA9 (PICO), PB18 (SCLK) | PB19 (POCI) | CS=PA2, 5MHz |
| 左轮编码器 | GPIO 中断 | — | PB5 (B), PB6 (A) | 4 倍频正交解码 |
| 右轮编码器 | GPIO 中断 | — | PA29 (A), PA30 (B) | 4 倍频正交解码 |
| 五向按键 | GPIO 输入 | — | PB13 (下), PB14 (上), PB17 (左), PB19 (右), PB20 (中) | 上拉输入，下降沿中断 |
| 灰度传感器 | ADC0 + GPIO MUX | PA27 (ADC 输入) | S0~S3 (MUX 选通) | 16 通道灰度阵列 |
| 舵机 | PWM (TIMG12) | 见 SysConfig | — | 双路舵机 |
| 步进电机 | CANFD0 | 见 SysConfig | 见 SysConfig | 张大头 Emm_V5.0 闭环 |
| 蜂鸣器 | GPIO | PB12 | — | 有源蜂鸣器 |
| 激光 | GPIO | 见 SysConfig | — | 双路激光 |
| 继电器 | GPIO | PB13 | — | 单路继电器 |
| SWD 调试 | SWD | PA19 (SWDIO) | PA20 (SWCLK) | XDS-110 / DAPLink |

> **注意：** 部分引脚可能会因 SysConfig 配置版本不同而有差异，请以 Keil 工程中当前生效的 `ti_msp_dl_config.h` 为准。

---

## 二、开发环境搭建

### 2.1 必需软件

| 软件 | 版本要求 | 安装路径建议 |
|---|---|---|
| Keil MDK | 5.39 或更高 | 默认路径 |
| MSPM0G3507 DFP Pack | 1.3.1 或更高 | Keil Pack Installer 自动安装 |
| TI MSPM0 SDK | mspm0_sdk_2_02_00_05 | `C:\ti\mspm0_sdk_2_02_00_05` |
| SysConfig | 1.20.0 或更高 | `C:\ti\sysconfig_1.20.0` |

### 2.2 路径配置

如果 SDK 或 SysConfig 安装在不同路径，需要修改以下两处：

1. **Keil 工程头文件搜索路径：**
   - 打开 Keil → Project → Options for Target → C/C++ → Include Paths
   - 修改指向 SDK 目录的路径

2. **SysConfig 预编译脚本路径：**
   - 位于 `mspm0_sdk_2_02_00_05\tools\keil\syscfg.bat`
   - 修改其中的 `SYSCFG_PATH` 变量为实际安装路径

### 2.3 编译与下载

```
1. 用 Keil 打开 keil/empty_LP_MSPM0G3507_nortos_keil.uvprojx
2. 点击 Project → Rebuild all target files （编译前会自动运行 SysConfig 生成配置文件）
3. 点击 Flash → Download 下载程序到核心板
```

**下载方式：**

- **SWD 方式（推荐）：** DAPLink / XDS-110 的 SWDIO 接 PA19，SWCLK 接 PA20
- **BSL 方式：** USB 转 TTL 通过 UniFlash 编程工具下载（按住 BSL 按键上电进入 Bootloader 模式）

---

## 三、SysConfig 图形化配置方法

SysConfig 是 TI 提供的图形化引脚和外设配置工具。修改 SysConfig 后会自动重新生成 `ti_msp_dl_config.h` 和 `ti_msp_dl_config.c`。

### 3.1 在 Keil 中打开 SysConfig

在 Keil 项目树（Project 窗口）中，展开 **Source** 组，双击 **`empty.syscfg`** 文件，系统会自动启动 SysConfig 工具并加载当前配置。

> **说明：** 虽然文件名为 `empty`，但本项目中的 `empty.syscfg` 已经包含完整的外设配置（4 路 UART、SPI、CANFD、ADC、DMA、PWM、Timer、GPIO 等），并非空白模板。项目根目录下还有 `main.syscfg` 和 `untitled.syscfg` 两个历史版本的配置文件，当前以 `empty.syscfg` 为准。

### 3.2 修改流程

1. 在 SysConfig GUI 中增删或修改外设模块
2. 修改外设参数（波特率、时钟分频、引脚映射等）
3. 点击 File → Save 保存
4. SysConfig 自动重新生成 `ti_msp_dl_config.h` 和 `ti_msp_dl_config.c`

### 3.3 编译时自动生成

Keil 工程已配置预编译步骤（Pre-build），每次编译前自动执行：
```
cmd.exe /C "$P../../tools/keil/syscfg.bat '$P' empty.syscfg"
```
确保每次编译时 SysConfig 配置与代码同步。

### 3.4 注意事项

- **不要手动修改 `ti_msp_dl_config.h` 和 `ti_msp_dl_config.c`**，它们会被 SysConfig 覆盖
- 如需自定义初始化逻辑，在 `main.c` 中调用 `SYSCFG_DL_init()` 之后添加
- 程序中引用的外设名称（如 `UART_debug`、`SPI_IMU`、`PWM_Motor`）须与 SysConfig 中配置的 Name 一致

---

## 四、工程编译配置标志位说明

编译标志位通过 `#define` 宏控制，集中分布在以下头文件中。修改后需重新编译生效。

### 4.1 IMU 传感器选择

**文件：** [keil/Hal/hal_imu.h](keil/Hal/hal_imu.h)

```c
//#define IMU_USE_IMU660RA 1   // 取消注释 = 使用 SeekFree IMU660RA
                                // 保持注释 = 使用 ICM42688（默认）
```

| 选项 | 说明 |
|---|---|
| **ICM42688（默认）** | TDK InvenSense 六轴传感器，SPI 接口，自研 Madgwick AHRS 姿态融合，带陀螺仪零偏自校准 |
| **IMU660RA** | SeekFree 出品（兼容 BMI088），SPI 接口，自带 Mahony AHRS 姿态解算 |

两种 IMU 共用相同的 `imu` 全局结构体（yaw/pitch/roll），上层 PID 控制和 VOFA 遥测代码无需修改。

### 4.2 电机驱动模块选择

**文件：** [keil/Motor/Motor.h](keil/Motor/Motor.h)

```c
//#define MOTOR_DRIVER_C107A     // 取消注释 = TB6612 驱动（带方向 GPIO 引脚）
#define MOTOR_DRIVER_AT8236 1   // 取消注释 = AT8236 驱动（互补 PWM，无需方向引脚）
```

| 选项 | 说明 |
|---|---|
| **AT8236（默认）** | 互补 PWM 控制，IN1 和 IN2 双引脚输出，无需额外的方向 GPIO。PWM 通道使用 TIMA0 |
| **TB6612 (C107A)** | PWM + DIR 方向引脚控制。方向引脚：PA24(L_A)、PA25(L_B)、PA26(R_A)、PA22(R_B)。PWM 使用 TIMG0 |

> **注意：** 两个宏互斥，只能同时启用一个。如果都不定义会报 `#error` 编译错误。

### 4.3 灰度传感器通道数

**文件：** [keil/Hal/hal_gray.h](keil/Hal/hal_gray.h)

```c
#define GRAY_SENSOR_16CH        // 16 通道（当前激活）
// #define GRAY_SENSOR_12CH      // 12 通道
// 两者都注释 = 8 通道（默认）
```

| 选项 | 通道数 | 路口判断阈值 | 位置查找表条目 |
|---|---|---|---|
| `GRAY_SENSOR_16CH` | 16 | 12 | 31 |
| `GRAY_SENSOR_12CH` | 12 | 9 | 23 |
| 8 通道（默认） | 8 | 6 | 15 |

> 两个宏**互斥**，同时定义会报 `#error`。通道数的选择会影响数组大小、MUX 选通位宽（3-bit / 4-bit）、循迹 PID 默认参数等。

### 4.4 LCD 显示方向

**文件：** [keil/LCD/lcd_init.h](keil/LCD/lcd_init.h)

```c
#define USE_HORIZONTAL 0   // 0 = 竖屏 (240x280)，1 = 横屏 (280x240)
```

> 注意：当前工程主要使用 HMI 串口屏进行显示交互，LCD 仅为辅助显示设备。

### 4.5 上电自动启动

**文件：** [main.c](main.c)（第 37 行附近）

```c
#define AUTO_START_TRACKING 0   // 1 = 上电跳过 HMI 菜单，直接启动任务一（巡线）
```

设置为 `1` 可用于比赛现场快速启动，无需手动点击 HMI 屏幕。

```
注意: 每次烧录完毕后需要手动下电再重新上电来进行手动复位，要同时复位HMI串口屏！！
```
### 4.6 HMI 调试捕获模式

**文件：** [main.c](main.c)（第 34 行附近）

```c
#define HMI_CAPTURE_DEBUG 0   // 1 = 关闭 VOFA 和 HMI 遥测输出，UART0 仅回传 HMI 原始字节
```

用于调试 HMI 串口屏通信协议时捕获原始数据帧。

### 4.7 UART 有线测试模式

**文件：** [keil/Test/uart_wired_test.h](keil/Test/uart_wired_test.h)

```c
#define UART_WIRED_HEARTBEAT 1   // 1 = 每 1 秒通过 UART2 发送 "TICK\r\n" 心跳包
#define UART_WIRED_SNIFFER   0   // 1 = 将收到的字节以 16 进制打印到 UART0（调试用）
#define UART_WIRED_ECHO      1   // 1 = 收到的字节原样回发（回环测试）
```

三个模式可独立开启，用于验证 LoRa 无线链路的收发通路。

---

## 五、keil 目录结构说明

`keil/` 目录下按功能模块组织代码，各子目录说明如下：

| 目录 | 功能说明 |
|---|---|
| `App/` | 应用层：默认遥测显示 (`app.c`)、HMI 菜单与任务调度 (`menu_task.c`)、移动滤波 (`move_filter.c`) |
| `Buzzer/` | 蜂鸣器 GPIO 驱动（PB12） |
| `Delay/` | 阻塞延时函数（us/ms/s） |
| `Duty/` | 竞赛任务实现：`task_one`（巡线）、`task_two`、`task_three`、`task_four`、`task_manager`（任务调度器）、`duty_chess`（棋局操作） |
| `Encoder/` | 车轮正交编码器：速度/距离计算、滤波初始化 |
| `Gimbal/` | 无刷云台控制：CANFD 通讯 (`hal_qgimbal_can`) + PID 稳定控制 (`gimbal_control`) |
| `Hal/` | **硬件抽象层（主要开发目录）**：灰度传感器 (`hal_gray`)、IMU (`hal_imu`)、按键 (`hal_key`)、继电器 (`hal_relay`)、延时 (`hal_delay`) |
| `Hardware/` | ⚠️ 旧版硬件驱动（encoder, gray_detection, motor, oled, steer, track, uart, jy61p），**不推荐使用** |
| `HMI/` | 串口触控屏驱动（淘晶驰/卓岚 TJC 协议）：帧解析 (`hmi_protocol`)、控件操作 (`zuolan_hmi`)、UART 适配 (`zuolan_usart`) |
| `ICM42688/` | ICM42688 IMU SPI 底层驱动 (`icm42688`) + 自研 AHRS 姿态融合 (`imu_icm42688`) |
| `interrupt_config/` | NVIC 中断使能与优先级统一配置 |
| `JY60/` | JY60 IMU 串口驱动（备用） |
| `K230/` | K230 AI 视觉模块 UART3 数据收发协议 |
| `Key/` | 五向按键驱动（状态机：单击/长按/连发） |
| `LCD/` | ST7735 LCD SPI 显示驱动（字库 `lcdfont.h`、图片 `pic.h`、绘图） |
| `Laser/` | 双路激光 + LED GPIO 驱动 |
| `LoRa/` | LoRa 无线模块：自定义帧协议 + AT 命令透传 |
| `Motor/` | 直流电机 PWM 驱动（双驱动兼容 AT8236 / TB6612） |
| `Navigation/` | 航位推算导航：根据轮速 + 偏航角计算 XY 坐标 |
| `Objects/` | Keil 编译输出目录（.o, .d, .axf, .hex） |
| `OLED/` | OLED I2C 显示驱动（备用） |
| `OS/` | 协作式调度器（部分使用，目前主循环使用 task_manager 调度方案） |
| `pid/` | 标准 PID 控制器：位置式/增量式，速度 PID、偏航 PID、循迹 PID |
| `Serial/` | 多路 UART 驱动：FIFO 收发、printf 重定向、4 路 UART ISR |
| `Servo/` | 舵机 PWM 驱动 |
| `Solve/` | 路径求解算法（三子棋最佳走法等） |
| `StepperMotor/` | 步进电机驱动（张大头 Emm_V5.0 闭环，UART 协议控制） |
| `System/` | ⚠️ 旧版系统代码（delay, motor_control, pid, soft_i2c, track_control 等），**不推荐使用** |
| `Test/` | UART 有线回环测试（心跳/嗅探/回声模式） |
| `Track/` | 巡线核心算法：`tracking_loop`（主循环）、`corner_detect`（直角弯/路口检测）、`track_position_ctrl`（位置控制） |
| `vofa/` | VOFA+ 遥测协议：JustFloat 数据发送 + `#P<id>=<value>!` 参数在线接收 |
| `ZF_IMU/` | SeekFree IMU 框架：IMU660RA 驱动、FIFO 缓冲、AHRS 姿态融合、通用工具库 |

---

## 六、VOFA+ 在线调参步骤

VOFA+ 是一个免费的串口数据可视化与调试上位机，本项目使用它来实时观测小车状态并在线调整 PID 参数。

### 6.1 硬件连接

- 使用 USB 转 TTL 模块 或者使用 DL_20无线串口模块连接 **UART0**：PA10 (TX) 接模块 RX，PA11 (RX) 接模块 TX
- 波特率：**115200**
```
const uint8_t vofa_sent_ch_count = 4;

/* tracking-loop PID tuning channels (JustFloat order = VOFA ch0..3) */
float pid_ch[4] = {
    (float)tracking_result.position_error,  /* ch0: track position error (PID input, target 0) */
    tracking_result.pid_correction,         /* ch1: track PID output (differential correction) */
    (float)Motor_speedL,                    /* ch2: left wheel measured speed */
    (float)Motor_speedR,                    /* ch3: right wheel measured speed */
};
vofa_send_floats(pid_ch, vofa_sent_ch_count);
```
需要调试什么内容，只需要在**pid_ch**中添加即可，并同步修改vofa_sent_ch_count

```
static void on_vofa_param(uint16_t id, float value)
{
    switch (id)
    {
    case 1:
        MotorLSpeedPID.Kp = value;
        MotorRSpeedPID.Kp = value;
        break;
    case 2:
        MotorLSpeedPID.Ki = value;
        MotorRSpeedPID.Ki = value;
        break;
    case 3:
        MotorLSpeedPID.Kd = value;
        MotorRSpeedPID.Kd = value;
        break;
    case 4:
        //vofa_speed_target = value;
        vofa_yaw_target = value;
        break;
    ......
}
```
vofa上位机端发送的数据格式以及接收解包逻辑已经定义好，如果希望接收到id=1的数据作为当前左电机的速度环的KP，可以这样写
```
case 1:
        MotorLSpeedPID.Kp = value;
```
### 6.2 打开 VOFA+

1. 下载并打开 VOFA+ 上位机（[vofa.plus](https://www.vofa.plus/)）
2. 选择正确的 COM 口，波特率 115200
3. 添加 **JustFloat** 协议的数据引擎（用于接收遥测数据）
4. 在波形图中拖入通道 0~3 即可实时观测

### 6.3 发送调参命令

在 VOFA+ 的发送面板中，使用以下格式发送调参命令：

```
#P<参数ID>=<数值>!
```

**示例：**
```
#P1=1.5!        设置速度 Kp 为 1.5
#P5=1!          切换到角度模式
#P17=200!       设置速度目标为 200
```

### 6.4 完整参数表

| 参数 ID | 对应变量 | 说明 |
|---|---|---|
| **P1** | Speed Kp | 左右轮速度环比例系数 |
| **P2** | Speed Ki | 左右轮速度环积分系数 |
| **P3** | Speed Kd | 左右轮速度环微分系数 |
| **P4** | Yaw Target | 偏航角目标值（角度模式，单位：度） |
| **P5** | Angle Mode | 0 = 速度模式，1 = 角度模式（启用偏航 PID） |
| **P6** | Yaw Kp | 偏航角 PID 比例系数 |
| **P7** | Yaw Ki | 偏航角 PID 积分系数 |
| **P8** | Yaw Kd | 偏航角 PID 微分系数 |
| **P14** | Track Kp | 循迹 PID 比例系数 |
| **P15** | Track Ki (in_a) | 循迹低通滤波系数（输入滤波 alpha 值） |
| **P16** | Track Kd | 循迹 PID 微分系数 |
| **P17** | Speed Target | 速度目标值（速度模式下的目标 PWM 占空比） |
| **P18** | IMU Telemetry | 1 = 开启 IMU 遥测通道，0 = 关闭 |

### 6.5 调参流程

**第一步：速度模式调参（P5=0）**

1. 发送 `#P17=200!` 设定期望速度
2. 在 VOFA 波形图中观察：
   - Ch0（黄色）= 位置偏差
   - Ch1（绿色）= PID 修正量
   - Ch2（蓝色）= 左轮实际速度
   - Ch3（红色）= 右轮实际速度
3. 依次调整 **P1 (Kp)** → **P2 (Ki)** → **P3 (Kd)**，使左右轮速度快速且无超调地跟踪目标

**第二步：角度模式调参（P5=1）**

1. 发送 `#P5=1!` 切换到角度模式
2. 发送 `#P4=90!` 设定目标偏航角为 90 度
3. 观察小车是否正确转向 90 度
4. 依次调整 **P6 (Yaw Kp)** → **P7 (Yaw Ki)** → **P8 (Yaw Kd)**，使偏航角响应快且无震荡

**第三步：循迹调参**

1. 将小车放在赛道上运行巡线任务
2. 打开 IMU 遥测：发送 `#P18=1!`
3. 发送 `#P17=xxx!` 设定期望巡线速度
4. 依次调整 **P14 (Track Kp)** → **P15 (滤波系数)** → **P16 (Track Kd)**，使小车巡线稳定不震荡

### 6.6 遥测通道说明

| 遥测通道 | 发送间隔 | Ch0 | Ch1 | Ch2 | Ch3 |
|---|---|---|---|---|---|
| **PID 通道** | 100ms | 位置偏差 | PID 修正量 | 左轮速度 | 右轮速度 |
| **IMU 通道** | 200ms（P18=1 时） | 偏航角 (°) | 俯仰角 (°) | 横滚角 (°) | 陀螺 Z 轴角速度 |

> 遥测使用 **JustFloat** 协议（4 字节小端浮点 + 尾帧 `00 00 80 7F`），在 VOFA+ 中添加数据引擎时选择 JustFloat 即可自动解析。

---

## 七、无线串口通信模块使用说明

### 7.1 硬件连接

- 无线串口通信模块通过 **UART2** 与 MCU 通信：TX = PA21, RX = PB16
- 波特率：**9600**
- 推荐模块：E22 系列（E22-400T22S 等）

### 7.2 帧协议

本项目实现了一套自定义的二进制帧协议，在 LoRa 模块 AT 透传模式之上封装：

```
┌────────┬────────┬────────┬────────┬───────────┬────────┬────────┐
│ 0xAA   │ 0x55   │ CMD    │ LEN    │ DATA...   │ XOR    │ 0xED   │
│ 帧头0  │ 帧头1  │ 命令   │ 长度   │ 数据负载  │ 校验   │ 帧尾   │
└────────┴────────┴────────┴────────┴───────────┴────────┴────────┘
```

- **CMD**：1 字节，数据包类型标识
- **LEN**：1 字节，负载长度（0~255）
- **DATA**：负载数据（0~255 字节）
- **XOR**：1 字节，校验 = CMD ⊕ LEN ⊕ DATA[0] ⊕ ... ⊕ DATA[N-1]
- **帧开销**：每帧 6 字节固定开销

### 7.3 API 接口

**文件：** [keil/LoRa/lora.h](keil/LoRa/lora.h) 和 [keil/LoRa/lora.c](keil/LoRa/lora.c)

| 函数 | 说明 |
|---|---|
| `lora_init()` | 初始化 LoRa 模块的 RX FIFO 和帧解析状态机 |
| `lora_send_packet(cmd, data, len)` | 构建帧（加头、校验、尾）并通过 UART2 发送 |
| `lora_rx_drain()` | 从 RX FIFO 取出字节送入帧解析器（主循环中调用） |
| `lora_send_byte(data)` | 发送单个原始字节（用于 AT 命令配置模块参数） |
| `lora_set_on_packet(cb)` | 注册回调函数：收到完整数据包时调用 |

### 7.4 使用示例

```c
// 初始化
lora_init();

// 发送数据包
uint8_t payload[] = {0x01, 0x02, 0x03};
lora_send_packet(0x10, payload, 3);

// 主循环中接收
while (1) {
    lora_rx_drain();   // 处理收到的数据帧
}

// 通过 AT 命令配置 LoRa 模块参数
lora_send_byte('A');  // 逐字节发送 AT 指令
```

### 7.5 测试模式

**文件：** [keil/Test/uart_wired_test.c](keil/Test/uart_wired_test.c)

| 模式 | 宏 | 功能 |
|---|---|---|
| 心跳模式 | `UART_WIRED_HEARTBEAT 1` | 每 1 秒发送 `TICK\r\n`，验证 TX 通路 |
| 嗅探模式 | `UART_WIRED_SNIFFER 1` | 将收到的数据 16 进制打印到 UART0 |
| 回声模式 | `UART_WIRED_ECHO 1` | 收到的字节原样发回，验证 TX+RX 通路 |

测试模式共用 UART2 的 FIFO，与 LoRa 协议解析器并行工作，互不影响。

---

## 八、竞赛任务系统

### 8.1 任务模式

上电后默认进入菜单模式（`menu_active = 1`），等待操作指令：

- **HMI 串口屏方式：** 触控按键 1~4 分别对应任务一 ~ 四
- **物理按键方式：** 五向按键上下选择、中键确认

### 8.2 四个任务

| 任务 | 文件 | 功能 |
|---|---|---|
| 任务一 | `Duty/task_one.c` | 巡线 + 圈数统计（基于 IMU 偏航角 360° 检测） |
| 任务二 | `Duty/task_two.c` | 预留 |
| 任务三 | `Duty/task_three.c` | 预留 |
| 任务四 | `Duty/task_four.c` | 预留 |

### 8.3 自动启动模式

设置 `main.c` 中的 `#define AUTO_START_TRACKING 1`，上电后自动启动任务一，无需手动操作 HMI 屏幕。适合比赛现场快速启动。

### 8.4 任务停止

- 通过 HMI 串口屏停止按钮触发
- 或通过代码设置 `g_stop_requested` 标志位
- 任务完成后自动回到菜单模式

---

## 九、控制系统架构

### 9.1 双环控制结构

```
┌──────────────────────────────────────────────────────┐
│  外环（任务层）                                       │
│  巡线/偏航/棋局 → 产生左右轮速度目标值                │
│                       ↓                              │
│  内环（速度 PID，10ms）                               │
│  速度误差 → PID 计算 → PWM 占空比 → 电机输出          │
└──────────────────────────────────────────────────────┘
```

### 9.2 定时器分配

| 定时器 | 周期 | 功能 |
|---|---|---|
| TIMER_0 (TIMG0) | 1ms | sys_tick 计时、IMU 更新（每 5ms）、灰度采样请求（每 5ms） |
| TIMER_1 (TIMA1) | 10ms | 编码器读取 → 速度 PID 计算 → 电机 PWM 输出 → 云台控制 |

### 9.3 中断分配

| 中断源 | 功能 |
|---|---|
| GROUP1 GPIO | 编码器 A/B 相正交解码（PB5/PB6 左轮，PA29/PA30 右轮） |
| UART0 | VOFA RX 字节 → vofa_rx_fifo |
| UART1 | HMI 串口屏 RX → HMI 协议解析 |
| UART2 | LoRa RX + UART 测试 RX → lora_rx_fifo + uart_wired_test_fifo |
| UART3 | K230 视觉数据接收 |
| CANFD0 | 云台电机 CAN 反馈（RX FIFO0） |

### 9.4 FIFO 缓冲模式

所有 UART 接收均使用 **FIFO 缓冲模式**：ISR 中仅写入 FIFO（极短执行时间），主循环中逐字节取出处理。避免中断内长时间阻塞，保证系统实时性。

---

## 十、常见问题

### 编译报错

**Q: 编译时报 `cannot open source file "ti_msp_dl_config.h"`**

A: 先执行一次完整编译（Rebuild），预编译步骤会自动运行 SysConfig 生成该文件。如果仍然报错，检查 `tools\keil\syscfg.bat` 中的 `SYSCFG_PATH` 是否正确。

**Q: 链接时报 `undefined symbol: DL_SYSCTL_configSYSPLL`**

A: 需要在 Keil 工程中添加 `source/ti/driverlib/sysctl/dl_sysctl_mspm0g1x0x_g3x0x.c` 源文件。

### 硬件问题

**Q: 电机不转**

A: 检查 `keil/Motor/Motor.h` 中的驱动选择宏是否与实际硬件匹配：
- AT8236 模块 → 启用 `#define MOTOR_DRIVER_AT8236 1`
- TB6612 模块 → 启用 `#define MOTOR_DRIVER_C107A`

**Q: IMU 数据异常（全 0 或无变化）**

A: 检查 `keil/Hal/hal_imu.h` 中的 `IMU_USE_IMU660RA` 是否与实际焊接的 IMU 芯片一致。ICM42688 读取 WHO_AM_I 寄存器 (0x75) 应返回 0x47。

**Q: LoRa 收不到数据**

A: 1) 确认波特率为 9600；2) 检查 LoRa 模块参数（信道、地址）是否配对；3) 开启嗅探模式 `UART_WIRED_SNIFFER 1` 查看是否有原始字节到达。

**Q: HMI 串口屏无显示**

A: 检查 UART1 的 TX/RX 连接（PA17→屏 RX，PA18→屏 TX），确认串口屏波特率 115200，确认使用淘晶驰/卓岚 TJC 协议。

### 异常诊断

**Q: 程序跑飞 / HardFault**

A: 程序在 HardFault_Handler 中会通过 UART0 输出 `HFLT!` + 8 位堆栈 PC 地址，用 USB 转 TTL 连接 UART0 (PA10) 即可读取。

**Q: 触发 NMI**

A: NMI_Handler 会通过 UART0 输出 `NMI!`。

**Q: 正常启动信号**

A: 程序正常启动后，会通过 UART0 输出 `MSPM0 Ready`，HMI 串口屏也会显示就绪状态。

---

## 十一、软件工具与资源

| 工具/资源 | 下载/链接 |
|---|---|
| Keil MDK 5.39+ | [keil.com](https://www.keil.com/) |
| MSPM0 SDK 2.02.00.05 | [TI 官网](https://www.ti.com/tool/MSPM0-SDK) |
| SysConfig 1.20+ | [TI 官网](https://www.ti.com/tool/SYSCONFIG) |
| VOFA+ 上位机 | [vofa.plus](https://www.vofa.plus/) |
| UniFlash 编程工具 | [TI 官网](https://www.ti.com/tool/UNIFLASH) |
| MSPM0 MCU 学习交流 QQ 群 | **82874622** |
| B 站学习视频 | [MSPM0 系列教程](https://www.bilibili.com/video/BV1Ei421Q7n9/) |

---


---

> **文件编码注意：** 本项目所有源文件使用 **GB2312/GBK** 编码，中文注释请勿以 UTF-8 格式保存，否则 Keil 编译时可能出现乱码或警告。
