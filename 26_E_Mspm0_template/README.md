# 26_E_Mspm0_template —— TI 杯竞赛小车控制系统

## 项目概述

本项目是基于 **TI MSPM0G3507**（Cortex-M0+, 80MHz, 128KB Flash, 32KB RAM）的竞赛小车完整控制系统，适用于 TI 杯全国大学生电子设计竞赛。

**主要功能模块：**

- 灰度巡线（8/12/16 通道可选）
- 双轮直流电机速度闭环控制（支持 AT8236 / TB6612 驱动）
- IMU 姿态解算与偏航角 PID 控制（支持 ICM42688 / IMU660RA）
- 无刷云台电机 CANFD 控制
- LoRa 无线数传模块（实际M0和RK3588通信未用到）
- VOFA+ 在线 PID 调参
- HMI 串口触控屏任务调度
- K230 AI 视觉模块（RK3588未用到）
- 舵机、步进电机、继电器、激光（无）等外设控制

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
| DL-20 无线串口通信（实际未使用） | UART2 | PA21 (TX) | PB16 (RX) | 9600 baud |
| DL-20 无线串口通信（实际未使用） | UART0 | 115200 baud |
| 香橙派 视觉 | UART3 | PB2 (TX) | PB3 (RX) | 115200 baud（需要确认） |
| 云台电机 | CANFD0 | PA12 (TX) | PA13 (RX) | QGimbal 无刷云台 |
| IMU (SPI) | SPI0 | PA9 (PICO), PB18 (SCLK) | PB19 (POCI) | CS=PA2, 5MHz |
| 左轮编码器 | GPIO 中断 | — | PB5 (B), PB6 (A) | 4 倍频正交解码 |
| 右轮编码器 | GPIO 中断 | — | PA29 (A), PA30 (B) | 4 倍频正交解码 |
| 五向按键 | GPIO 输入 | — | PB13 (下), PB14 (上), PB17 (左), PB19 (右), PB20 (中) | 上拉输入，下降沿中断 |
| 灰度传感器 | ADC0 + GPIO MUX | PA27 (ADC 输入) | S0~S3 (MUX 选通) | 16 通道灰度阵列 |
| 舵机 | PWM (TIMG12) | 见 SysConfig | — | 双路舵机 |
| 蜂鸣器 | GPIO | PB12 | — | 有源蜂鸣器 |
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

### 4.6 上电自动回工件坐标系 (Auto-Home)

**文件：** [keil/Arm/arm_home.h](keil/Arm/arm_home.h)

```c
#define AUTO_HOME_ON_STARTUP 1   // 1 = 上电自动使能电机并回到工件坐标位置
```

上电后延迟 3 秒，自动使能全部 3 个机械臂关节电机，并运动到预设的工件坐标系角度（默认 M0=5.26 rad, M1=1.89 rad, M2=0 rad）。可通过 VOFA P62 更新工件坐标（运行时全局变量，掉电丢失），P63 恢复默认值。

> **注意：** 每次上电使用编译期默认值（5.26 / 1.89 / 0 rad），可通过 P62 运行时更新、P63 恢复默认值（掉电丢失）。上电不再自动执行 set-zero（零点由 QD4310 上位机设置，P58 为手动备用）。

```
注意: 每次烧录完毕后需要手动下电再重新上电来进行手动复位，要同时复位HMI串口屏！！
```
### 4.7 HMI 调试捕获模式

**文件：** [main.c](main.c)（第 34 行附近）

```c
#define HMI_CAPTURE_DEBUG 0   // 1 = 关闭 VOFA 和 HMI 遥测输出，UART0 仅回传 HMI 原始字节
```

用于调试 HMI 串口屏通信协议时捕获原始数据帧。

### 4.8 UART 有线测试模式

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
| `Arm/` | **机械臂控制**：SCARA 三关节电机 + 继电器 + 舵机升降 (`arm_control.c`)、RK3588 握手机制 (`arm_task.c`)、上电自动回工件坐标 (`arm_home.c`)、通信协议 (`arm_protocol.h`) |
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

#### 小车底盘 (P1~P19)

| ID | 变量 | 说明 |
|---|---|---|
| **P1** | Speed Kp | 左右轮速度环比例系数 |
| **P2** | Speed Ki | 左右轮速度环积分系数 |
| **P3** | Speed Kd | 左右轮速度环微分系数 |
| **P4** | Yaw Target | 偏航角目标值（角度模式，单位：度） |
| **P5** | Angle Mode | 0 = 速度模式，1 = 角度模式 |
| **P6** | Yaw Kp | 偏航角 PID 比例系数 |
| **P7** | Yaw Ki | 偏航角 PID 积分系数 |
| **P8** | Yaw Kd | 偏航角 PID 微分系数 |
| **P14** | Track Kp | 循迹 PID 比例系数 |
| **P15** | Track in_a | 循迹低通滤波系数 |
| **P16** | Track Kd | 循迹 PID 微分系数 |
| **P17** | Speed Target | 速度目标值 |
| **P18** | IMU Telemetry | 1 = 开启 IMU 遥测，0 = 关闭 |
| **P19** | Track Debug | 1 = 开启 16 通道循迹遥测 |

#### 云台电机 (P28~P41) — 详见第七节

| ID | 功能 | 示例 |
|---|---|---|
| **P28** | 全部云台电机使能 | `#P28=1!` |
| **P29~P32** | 云台电机 0~3 使能/禁能 | `#P29=1!` / `#P29=0!` |
| **P33~P36** | 云台电机 0~3 速度目标 (rpm) | `#P33=30!` |
| **P37~P40** | 云台电机 0~3 手动角度 (rad) | `#P37=1.57!` |
| **P41** | 手动角度模式开关 | `#P41=1!` 进入 / `#P41=0!` 退出 |

#### 机械臂控制 (P42~P65) — 详见第八节

| ID | 功能 | 示例 |
|---|---|---|
| **P42** | 使能/禁能全部 3 个机械臂电机 | `#P42=1!` / `#P42=0!` |
| **P43~P45** | 机械臂电机 0~2 单独使能/禁能 | `#P43=1!` |
| **P46~P48** | 机械臂电机 0~2 目标角度 (**rad**, 0~2π) | `#P46=5.26!` |
| **P49~P51** | 机械臂电机 0~2 速度目标 (rpm) | `#P49=30!` |
| **P52** | 启动机械臂抓取任务 | `#P52=1!` |
| **P53** | 紧急停止机械臂任务 | `#P53=1!` |
| **P54** | 机械臂 VOFA 遥测开关 | `#P54=1!` |
| **P55** | 正弦速度幅值 (rpm) | `#P55=50!` |
| **P56** | 正弦速度周期 (步/循环) | `#P56=100!` |
| **P57** | 正弦电机位掩码 (bit0=m0) | `#P57=7!` (全部) |
| **P58** | 手动 set-zero (备用) | `#P58=1!` |
| **P59** | 舵机 2 角度 (0~180°) | `#P59=90!` |
| **P60** | 启动形状追踪任务 | `#P60=1!` |
| **P61** | 末端舵机升降 (0=上升, !=0=下降) | `#P61=1!` |
| **P62** | 保存当前角度为工件坐标 (运行时全局变量) | `#P62=1!` |
| **P63** | 清除工件坐标 (恢复默认值) | `#P63=1!` |
| **P64** | 继电器手动测试 (0=断开, !=0=吸合) | `#P64=1!` |
| **P65** | 蜂鸣器测试 (0=关, !=0=开) | `#P65=1!` |

> **注意：** 机械臂角度 (P46-P48) 使用**弧度制** (0~2π)，与 QD4310 电机协议一致。默认工件坐标：M0=5.26 rad, M1=1.89 rad, M2=0 rad。

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

## 七、QGimbal 无刷云台电机 CAN 控制

### 7.1 硬件连接

| 信号 | MCU 引脚 | 备注 |
|---|---|---|
| CAN TX | PA12 | QD4310 无刷云台电机 CANFD 接口 |
| CAN RX | PA13 | 需外接 CAN 收发器（TJA1050 等） |
| CAN GND | GND | MCU 与电机板共地 |

- CAN 波特率：**1.000 MHz**（与 STM32 QGimbal 参考一致）
- CAN 终端电阻：总线两端各 **120Ω**
- 电机需 **24V 独立供电**（CAN 收发器由 24V 供电）

### 7.2 电机架构

支持最多 **4 个 QD4310 无刷电机**，CAN 命令 ID = `0x400 + motor_id`，反馈 ID = `0x500 + motor_id`。

| 索引 | CAN 命令 ID | CAN 反馈 ID | 角色 | 控制模式 |
|------|------------|------------|------|---------|
| 0 | 0x400 | 0x500 | Yaw 偏航 | 速度 / 增稳 PID / 手动角度 |
| 1 | 0x401 | 0x501 | Pitch 俯仰 | 速度 / 增稳 PID / 手动角度 |
| 2 | 0x402 | 0x502 | 辅助 | 速度 / 手动角度 |
| 3 | 0x403 | 0x503 | 辅助 | 速度 / 手动角度 |

QD4310 协议帧格式：**标准帧 (11-bit ID)，DLC=3**，数据 = `[cmd, value_LSB, value_MSB]`（小端）。

| 命令 | 编码 | 值范围 | 说明 |
|------|------|--------|------|
| ENABLE | 0x01 | — | 使能电机 |
| DISABLE | 0x02 | — | 禁能电机 |
| SPEED | 0x04 | ±1000 rpm | 速度控制 |
| ANGLE | 0x05 | 0 ~ 2π rad | 绝对角度控制（uint16 映射） |
| CURRENT | 0x03 | ±10 A | 电流（力矩）控制 |

### 7.3 VOFA 电机控制命令

所有电机控制通过 VOFA 发送 `#P<ID>=<值>!`，P 编号按 `base + motor_idx` 排列：

| P# | 功能 | 示例 |
|----|------|------|
| **P28** | 全部电机使能 | `#P28=1!` |
| **P29** | 电机 0 (yaw) 使能/禁能 | `#P29=1!` 使能 / `#P29=0!` 禁能 |
| **P30** | 电机 1 (pitch) 使能/禁能 | `#P30=1!` |
| **P31** | 电机 2 使能/禁能 | `#P31=1!` |
| **P32** | 电机 3 使能/禁能 | `#P32=1!` |
| **P33** | 电机 0 速度目标 (rpm) | `#P33=30!` |
| **P34** | 电机 1 速度目标 (rpm) | `#P34=20!` |
| **P35** | 电机 2 速度目标 (rpm) | `#P35=50!` |
| **P36** | 电机 3 速度目标 (rpm) | `#P36=50!` |
| **P37** | 电机 0 手动角度 (rad, 0~2π) | `#P37=1.57!` (90°) |
| **P38** | 电机 1 手动角度 (rad) | `#P38=0.5!` |
| **P39** | 电机 2 手动角度 (rad) | `#P39=3.14!` |
| **P40** | 电机 3 手动角度 (rad) | `#P40=0!` |
| **P41** | 手动角度模式开关 | `#P41=1!` 进入 / `#P41=0!` 退出 |

### 7.4 使用示例

```bash
# 基本测试流程
#P29=1!       使能 yaw 电机
#P37=1.57!    进入手动角度模式 → yaw 转到 90°（自动发送 ANGLE 命令）
#P37=3.14!    yaw 转到 180°
#P37=0!       yaw 回正到 0°
#P41=0!       退出手动角度模式 → 恢复速度/增稳控制

# 速度模式测试
#P29=1!       使能 yaw
#P33=30!      yaw 以 30 rpm 转动

# 多电机同时控制
#P28=1!       全部使能
#P37=1.57!    yaw 到 90°
#P38=0.3!     pitch 到 0.3 rad
```

> **注意：** P37–P40 发送时会自动进入手动角度模式（`manual_angle_mode=1`），云台控制循环会暂停速度/增稳 PID 输出，改为每 10ms 发送一次 ANGLE 命令。发送 `#P41=0!` 可退出手动模式。

### 7.5 遥测通道

电机状态通过 VOFA JustFloat 遥测（每 500ms 发送一次）：

| 通道 | 来源 | 内容 |
|------|------|------|
| ch0 | `QGimbal_CAN_Status` | 电机 0 (yaw) enabled 标志 |
| ch1 | `QGimbal_CAN_Status` | 电机 1 (pitch) enabled 标志 |
| ch2 | `QGimbal_CAN_Status` | 电机 0 实际角度 (rad) |
| ch3 | `QGimbal_CAN_Status` | 电机 1 实际角度 (rad) |

### 7.6 CAN 诊断

启用 `QGimbal_CAN_Diag()` 可获取 CAN 总线健康状态（当前已注释，位于 [main.c](main.c) 主循环中）：

| 通道 | 内容 |
|------|------|
| ch0 | tx_count（发送计数，每 500ms +1） |
| ch1 | TXBTO[0]（1 = 最近一次 Buffer 0 发送成功） |
| ch2 | busOffStatus（1 = CAN 总线离线，需检查硬件连接） |
| ch3 | TEC（TX Error Counter，持续上升 = 无设备应答） |

### 7.7 代码架构

| 文件 | 功能 |
|------|------|
| [keil/Gimbal/hal_qgimbal_can.h](keil/Gimbal/hal_qgimbal_can.h) | CAN 收发接口 + 电机状态结构体 |
| [keil/Gimbal/hal_qgimbal_can.c](keil/Gimbal/hal_qgimbal_can.c) | CAN 初始化、命令发送、反馈解析 |
| [keil/Gimbal/gimbal_control.h](keil/Gimbal/gimbal_control.h) | `GimbalMotor`（每电机 PID + 目标值） + `GimbalController` 结构体 |
| [keil/Gimbal/gimbal_control.c](keil/Gimbal/gimbal_control.c) | 云台控制主循环（手动角度 / 速度 / 增稳 PID 三模式） |
| [main.c](main.c) `vofa_handle_gimbal()` | VOFA P28–P41 命令映射到电机索引 |

---

## 八、SCARA 机械臂与 RK3588 通信协议 (Arm ↔ RK3588 Protocol)

本节面向 **RK3588 (Orange Pi) 端开发者**，描述 MSPM0 与 RK3588 之间的握手通信协议。机械臂为 **SCARA 构型**（3 个 QD4310 关节电机 + 1 路电磁铁继电器），用于竞赛中的铁片碎片自动吸取与放置任务。

### 8.1 硬件连接

| 信号 | MSPM0 引脚 | RK3588 引脚 | 备注 |
|---|---|---|---|
| UART TX | **PA21** | RX | 3.3V TTL 电平 |
| UART RX | **PB16** | TX | 3.3V TTL 电平 |
| GND | GND | GND | **必须共地** |

- 波特率：**115200** bps
- UART 外设：MSPM0 UART2（`UART_wired_INST`）
- ⚠️ **注意**：UART2 原本用于 LoRa 无线串口模块（9600 bps），使用 RK3588 时需断开 LoRa 模块，MCU 代码会在 `Arm_Init()` 中自动将 UART2 波特率切换为 115200

### 8.2 通信流程概览

```
RK3588 (Orange Pi)                         MSPM0G3507
       |                                         |
       |                                         |  VOFA #P52=1 触发任务
       |<———— 0x01 + JustFloat(6ch arm state) ——|  (29 bytes)
       |                                         |
  视觉识别 + 逆运动学解算                           |
       |                                         |
       |———— JustFloat(pick[3]+place[3]) ——————→|  (28 bytes)
       |                                         |-- 机械臂移动到吸取位
       |                                         |-- Relay_On (电磁铁吸合)
       |                                         |-- 机械臂移动到放置位
       |                                         |-- Relay_Off (电磁铁释放)
       |<———— 0x02 + JustFloat(6ch arm state) ——|  (29 bytes, 请求下一片)
       |                                         |
  视觉识别 + 逆运动学解算 (第二片)                   |
       |                                         |
       |———— JustFloat(pick[3]+place[3]) ——————→|  (28 bytes)
       |                                         |  ... (循环, 最多 4 片)
       |                                         |
       |———— JustFloat([-1,0,0,0,0,0]) ————————→|  Sentinel: 已无更多碎片
       |                                         |-- 机械臂归零
       |<———— 0x03 + JustFloat(6ch arm state) ——|  (29 bytes, 任务完成)
```

### 8.3 JustFloat 帧格式 (通用)

JustFloat 是本项目中 MSPM0 ↔ VOFA ↔ RK3588 之间通用的二进制浮点数组传输协议：

```
帧格式: [N × float32 LE] [00 00 80 7F]
         └─ N×4 bytes ─┘ └── 4-byte tail ──┘
```

- **数据区**：N 个 IEEE 754 单精度浮点数，**小端序 (Little-Endian)**
- **尾帧 (Tail)**：固定 4 字节 `00 00 80 7F`，即 IEEE 754 正无穷大 (+Infinity)
- MSPM0 端 RX 解析器通过**逐字节状态机**检测尾帧，能正确处理假尾帧（数据中恰好出现 `00 00 80 7F` 子序列的情况）

### 8.4 MSPM0 → RK3588 帧格式 (上行帧, 29 bytes)

RK3588 **接收**此帧以获取任务指令和机械臂当前状态。

```
Offset  Size  Content          Description
─────────────────────────────────────────────
[0]     1B    cmd_byte         命令字节 (见下表)
[1-24]  24B   6 × float32 LE   机械臂状态反馈 (6 通道)
[25-28] 4B    00 00 80 7F      JustFloat 尾帧
─────────────────────────────────────────────
Total:  29 bytes
```

**命令字节 (cmd_byte)：**

| 命令值 | 宏定义 | 含义 | RK3588 应执行的操作 |
|---|---|---|---|
| `0x01` | START | 任务开始 | 开始视觉识别，计算第一片碎片的吸取+放置角度 |
| `0x02` | CONTINUE | 继续请求 | 当前碎片已处理完毕，计算下一片的吸取+放置角度 |
| `0x03` | COMPLETE | 任务完成 | 机械臂已归零，任务结束，可准备下一轮 |

**6 通道浮点数据 (arm state feedback, 角度单位=度)：**

| 通道 | 数据类型 | 含义 |
|---|---|---|
| ch0 | float | 电机 0 实际角度 (底座旋转, 0~360°) |
| ch1 | float | 电机 1 实际角度 (肘关节, 0~360°) |
| ch2 | float | 电机 2 实际角度 (电磁铁旋转, 0~360°) |
| ch3 | float | 继电器状态 (1.0=吸合, 0.0=释放) |
| ch4 | float | 当前碎片序号 (0, 1, 2, 3) |
| ch5 | float | 状态码 (0.0=正常) |

> **说明**：这 6 个浮点通道仅用于监控和调试，RK3588 的逆运动学解算**不需要**当前电机角度——解算只需要机械臂运动学模型（臂长等硬编码参数）+ 视觉目标坐标。

### 8.5 RK3588 → MSPM0 帧格式 (下行帧, 28 bytes)

RK3588 **发送**此帧以告知 MSPM0 吸取位置和放置位置的关节角度。

```
Offset  Size  Content          Description
─────────────────────────────────────────────
[0-23]  24B   6 × float32 LE   pick[3] + place[3] 关节角度
[24-27] 4B    00 00 80 7F      JustFloat 尾帧
─────────────────────────────────────────────
Total:  28 bytes (标准 JustFloat 6 通道)
```

**6 通道浮点数据 — SCARA 关节角度 (单位=度)：**

| 浮点索引 | 对应电机 | 关节 | 含义 |
|---|---|---|---|
| `[0]` | Motor 0 (CAN ID=0) | J1 底座旋转 | **吸取位** 底座目标角度 (0~360°) |
| `[1]` | Motor 1 (CAN ID=1) | J2 肘关节 | **吸取位** 肘关节目标角度 (0~360°) |
| `[2]` | Motor 2 (CAN ID=2) | J3 磁铁旋转 | **吸取位** 电磁铁旋转角度 (0~360°) |
| `[3]` | Motor 0 (CAN ID=0) | J1 底座旋转 | **放置位** 底座目标角度 (0~360°) |
| `[4]` | Motor 1 (CAN ID=1) | J2 肘关节 | **放置位** 肘关节目标角度 (0~360°) |
| `[5]` | Motor 2 (CAN ID=2) | J3 磁铁旋转 | **放置位** 电磁铁旋转角度 (0~360°) |

**哨兵值 (Sentinel) — 表示"已无更多碎片"：**
- 将 `float[0]` 设为 **`-1.0f`**（IEEE 754: `0xBF800000`）
- 其余 5 个 float 可填任意值（通常填 0）
- 收到后 MSPM0 不再等待后续碎片，直接执行**归零 → 发送 0x03 COMPLETE**

> 正常关节角度范围为 [0, 360]，-1.0 是一个不会有歧义的哨兵值。

### 8.6 SCARA 机械臂关节映射

| 电机索引 | CAN 命令 ID | CAN 反馈 ID | 关节 | 功能描述 |
|---|---|---|---|---|
| 0 | `0x400` | `0x500` | **J1 底座** | 控制整个机械臂的水平旋转（绕 Z 轴） |
| 1 | `0x401` | `0x501` | **J2 肘关节** | 控制前臂的俯仰/伸缩 |
| 2 | `0x402` | `0x502` | **J3 磁铁旋转** | 控制电磁铁的 Z 轴旋转角度 |
| — | — | — | **继电器 (PB13)** | 电磁铁吸合/释放（高电平有效） |

### 8.7 任务时序与参数

| 参数 | 值 | 说明 |
|---|---|---|
| 碎片上限 | **4 片** | 超过后自动归零并发送 0x03 |
| 运动到达判定 | **±2°** | 3 个电机全部进入目标角度 ±2° 范围内即视为到位 |
| 电磁铁吸取延迟 | **500 ms** | Relay_On 后等待磁铁稳定吸附 |
| 电磁铁释放延迟 | **500 ms** | Relay_Off 后等待铁片完全脱离 |
| RK3588 响应超时 | **5 秒** | 超时后 MSPM0 放弃等待，执行归零（不发送 0x03） |
| 运动到位超时 | **3 秒** | 单次移动超过 3 秒未到位则进入 ERROR 状态 |
| 零位角度 | **0°** (全部电机) | 任务完成后机械臂回到的初始位置 |
| 舵机升降延时 | **500 ms** | 取/放前下降舵机、取/放后上升舵机的稳定时间 |
| 蜂鸣器提示 | **1 秒** | 任务完成 (SEND_COMPLETE) 后自动蜂鸣 |

### 8.8 Auto-Home 上电自动回工件坐标系

机械臂上电后自动使能全部 3 个关节电机并运动到工件坐标系位置：

- **触发条件：** `AUTO_HOME_ON_STARTUP = 1`（默认开启）
- **延时：** 上电后等待 **3 秒**（等待电机驱动板上电就绪）
- **目标角度：** 默认 M0=5.26 rad / M1=1.89 rad / M2=0 rad
- **到位判定：** 全部电机进入目标 ±0.035 rad (≈2°) 范围
- **超时：** 5 秒未到位则结束（不阻塞后续操作）
- **持久化：** 通过 VOFA P62 可将当前角度存入全局变量（仅运行时有效，掉电丢失）；P63 恢复默认值

### 8.9 RK3588 端 Python 参考实现

以下是一个完整的 Python 示例，RK3588 开发者可以直接复制并在此基础上集成视觉识别和逆运动学解算：

```python
#!/usr/bin/env python3
"""
MSPM0 ↔ RK3588 SCARA Arm Handshake Protocol — RK3588 Side
Run on Orange Pi RK3588. Requires: pip install pyserial
"""

import struct
import serial

# ---- JustFloat Protocol Constants ----
TAIL = b'\x00\x00\x80\x7F'          # JustFloat frame tail (IEEE 754 +Inf)
FLOAT_COUNT = 6                     # 6 floats per frame
PAYLOAD_BYTES = FLOAT_COUNT * 4     # 24 bytes
SENTINEL = -1.0                     # "no more fragments" sentinel

# ---- Command Bytes ----
CMD_START    = 0x01
CMD_CONTINUE = 0x02
CMD_COMPLETE = 0x03


def make_jf_frame(floats):
    """Build a standard JustFloat frame: N floats (LE) + tail.
    
    Args:
        floats: list of float values
    
    Returns:
        bytes: complete JustFloat frame ready to send
    """
    return struct.pack(f'<{len(floats)}f', *floats) + TAIL


def recv_jf_frame(ser, float_count=FLOAT_COUNT, timeout=None):
    """Receive a standard JustFloat frame from UART.
    
    Uses a sliding-window state machine to detect the tail marker.
    Handles false tails (tail bytes appearing in float data).
    
    Args:
        ser: pyserial Serial object
        float_count: expected number of floats (default 6)
        timeout: timeout in seconds (None = block forever)
    
    Returns:
        tuple of floats, or None on timeout
    """
    if timeout is not None:
        ser.timeout = timeout
    else:
        ser.timeout = None
    
    data_target = float_count * 4  # e.g. 24 for 6 floats
    buf = bytearray()
    
    while True:
        byte = ser.read(1)
        if not byte:  # timeout
            return None
        buf.append(byte[0])
        
        # Check for tail at the expected position
        if len(buf) >= data_target + 4:
            if buf[data_target:data_target+4] == TAIL:
                # Valid frame — unpack floats
                floats = struct.unpack(f'<{float_count}f', buf[:data_target])
                return floats
            # False tail — slide the window forward by 1 byte
            buf = buf[1:]


def recv_arm_frame(ser, timeout=None):
    """Receive an MSPM0→RK3588 frame: [cmd_byte] [6 floats LE] [tail].
    
    The MSPM0 frame has a 1-byte command BEFORE the 6 floats + tail.
    Total: 1 + 24 + 4 = 29 bytes.
    
    Returns:
        (cmd, floats) tuple, or (None, None) on timeout
    """
    if timeout is not None:
        ser.timeout = timeout
    else:
        ser.timeout = None
    
    payload_bytes = 1 + FLOAT_COUNT * 4  # 25 bytes (cmd + 6 floats)
    buf = bytearray()
    
    while True:
        byte = ser.read(1)
        if not byte:  # timeout
            return None, None
        buf.append(byte[0])
        
        if len(buf) >= payload_bytes + 4:
            if buf[payload_bytes:payload_bytes+4] == TAIL:
                cmd = buf[0]
                floats = struct.unpack(f'<{FLOAT_COUNT}f', buf[1:payload_bytes])
                return cmd, floats
            buf = buf[1:]


def send_pick_place(ser, pick_angles, place_angles):
    """Send pick-up + place angles to MSPM0.
    
    Args:
        ser: pyserial Serial object
        pick_angles: [base_deg, elbow_deg, magnet_deg] for pick-up position
        place_angles: [base_deg, elbow_deg, magnet_deg] for place position
    """
    data = pick_angles + place_angles  # [p0, p1, p2, pl0, pl1, pl2]
    frame = make_jf_frame(data)
    ser.write(frame)
    print(f"  → Sent: pick={pick_angles} place={place_angles}")


def send_sentinel(ser):
    """Send sentinel (-1.0) to tell MSPM0 there are no more fragments."""
    frame = make_jf_frame([SENTINEL, 0.0, 0.0, 0.0, 0.0, 0.0])
    ser.write(frame)
    print("  → Sent: SENTINEL (no more fragments)")


# ================================================================
#  TODO: Replace this dummy IK with your actual inverse kinematics
# ================================================================
def inverse_kinematics(target_x_mm, target_y_mm, target_rot_deg):
    """Dummy IK — replace with actual SCARA inverse kinematics.
    
    Args:
        target_x_mm: target X in millimeters (from camera)
        target_y_mm: target Y in millimeters (from camera)
        target_rot_deg: target magnet rotation in degrees
    
    Returns:
        (base_deg, elbow_deg, magnet_deg) joint angles in degrees
    """
    # ========================================
    # TODO: Implement your SCARA IK here
    # L1 = 150.0  # upper arm length (mm)
    # L2 = 120.0  # forearm length (mm)
    # ...
    # return (theta1_deg, theta2_deg, target_rot_deg)
    # ========================================
    return (90.0, 45.0, target_rot_deg)


# ================================================================
#  Main Handshake Loop
# ================================================================
def main():
    # Open serial port — adjust device path for your Orange Pi
    ser = serial.Serial(
        port='/dev/ttyS1',      # RK3588 UART → MSPM0 UART2
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=0.1
    )
    print("Waiting for MSPM0 arm task start (0x01)...")
    
    fragment_index = 0
    
    while True:
        # ---- Wait for MSPM0 command frame ----
        cmd, state = recv_arm_frame(ser)
        if cmd is None:
            continue  # timeout, keep waiting
        
        # ---- Parse arm state feedback ----
        motor0_deg = state[0]
        motor1_deg = state[1]
        motor2_deg = state[2]
        relay_on   = state[3]
        frag_idx   = state[4]
        status     = state[5]
        
        if cmd == CMD_START:
            print(f"\n[CMD 0x01] TASK START — fragment {int(frag_idx)}")
            print(f"  Arm state: M0={motor0_deg:.1f}° M1={motor1_deg:.1f}° "
                  f"M2={motor2_deg:.1f}° relay={'ON' if relay_on else 'OFF'}")
            
            # ====================================================
            # TODO: Run camera → visual recognition → get (x,y,rot)
            # target_x, target_y, target_rot = camera.detect()
            # ====================================================
            
            # Dummy: hardcoded pick-up and place positions
            pick  = inverse_kinematics(200.0, 100.0, 0.0)    # pick up
            place = inverse_kinematics(300.0, 200.0, 90.0)   # place down
            send_pick_place(ser, list(pick), list(place))
            fragment_index += 1
        
        elif cmd == CMD_CONTINUE:
            print(f"\n[CMD 0x02] CONTINUE — fragment {int(frag_idx)}")
            
            if fragment_index >= 4:
                # No more fragments to process
                send_sentinel(ser)
                print("  All 4 fragments done — sentinel sent.")
            else:
                # TODO: Run camera for next fragment
                pick  = inverse_kinematics(200.0, 100.0, 0.0)
                place = inverse_kinematics(300.0, 200.0, 90.0)
                send_pick_place(ser, list(pick), list(place))
                fragment_index += 1
        
        elif cmd == CMD_COMPLETE:
            print(f"\n[CMD 0x03] TASK COMPLETE — arm at zero, done.")
            break
        
        else:
            print(f"  Unknown command: 0x{cmd:02X}")
    
    ser.close()
    print("Done.")


if __name__ == '__main__':
    main()
```

### 8.10 PC 模拟 RK3588 快速测试 (无需香橙派)

在实际连接 RK3588 之前，可以用 PC 端的 USB 转 TTL 模块模拟 RK3588 收发数据来验证协议：

**步骤：**

```
1. 硬件接线：
   USB-TTL RX  → MSPM0 PA21 (TX)
   USB-TTL TX  → MSPM0 PB16 (RX)
   USB-TTL GND → MSPM0 GND

2. 串口工具设置：115200 bps, 8N1, HEX 显示模式

3. VOFA 端操作 (UART0, 115200)：
   #P43=1!       使能电机 0 (底座)
   #P44=1!       使能电机 1 (肘关节)
   #P45=1!       使能电机 2 (磁铁旋转)
   #P52=1!       启动机械臂任务

4. 观察 UART2 输出：
   → 应收到 29 字节帧，首字节为 0x01
   → 帧数据: [01] [24字节浮点] [00 00 80 7F]

5. 用 Python 脚本发送模拟的 RK3588 响应：
   参考上方 Python 代码中的 send_pick_place() 函数。
   发送 28 字节: [6个float LE] + [00 00 80 7F]
   
   或者用串口工具发送原始 HEX 数据（示例：吸取 90°,45°,0° 放置 180°,90°,0°）：
   00 00 B4 42  00 00 34 42  00 00 00 00      ← pick: 90.0, 45.0, 0.0
   00 00 34 43  00 00 B4 42  00 00 00 00      ← place: 180.0, 90.0, 0.0
   00 00 80 7F                                ← tail

6. 观察机械臂动作：
   → 移动到吸取位 → 继电器吸合 (500ms) → 移动到放置位 → 继电器释放 (500ms)
   → UART2 收到 0x02 (继续请求)

7. 发送哨兵：
   00 00 80 BF  00 00 00 00  00 00 00 00      ← -1.0f, 0, 0
   00 00 00 00  00 00 00 00  00 00 00 00      ← 0, 0, 0
   00 00 80 7F                                ← tail

8. 观察：
   → 机械臂归零 → UART2 收到 0x03 (任务完成)
```

### 8.11 RK3588 开发者注意事项

1. **浮点数为小端序**：使用 Python `struct.pack('<f', value)` 即可，`<` 表示 little-endian
2. **角度单位为度 (0~360°)**，不是弧度——与 QD4310 电机 CAN 协议内部的弧度不同
3. **尾帧 `00 00 80 7F` 是 IEEE 754 +Infinity**，不会出现在 [0, 360] 的角度数据中，因此是安全的帧分隔符
4. **MSPM0 上行帧格式特殊**：29 字节 = 1 字节命令 + 24 字节浮点 + 4 字节尾帧，与标准 28 字节 JustFloat 不同
5. **假尾帧处理**：接收端必须逐字节检测尾帧（而非简单地搜索固定字节序列），因为浮点数据可能恰好包含 `00 00 80 7F` 子序列。参考上方 Python 中的滑动窗口实现
6. **UART2 共享**：MCU 端 UART2 原本用于 LoRa 模块 (9600 bps)，连接 RK3588 时物理断开 LoRa 模块即可——代码在 `Arm_Init()` 中会自动切换波特率
7. **机械臂到位反馈**：MCU 不会主动告知"已到吸取位"或"已到放置位"，RK3588 通过观察 MCU 发送的下一条命令帧（0x02 或 0x03）来判断当前动作已完成
8. **超时保护**：MCU 等待 RK3588 响应最长 5 秒，超时后自动归零并进入 IDLE 状态（不发送 0x03）。RK3588 端应**在 3 秒内**完成视觉识别和逆运动学解算并发送响应帧

---

## 九、无线串口通信模块使用说明

### 9.1 硬件连接

- 无线串口通信模块通过 **UART2** 与 MCU 通信：TX = PA21, RX = PB16
- 波特率：**9600**
- 推荐模块：E22 系列（E22-400T22S 等）

### 9.2 帧协议

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

### 9.3 API 接口

**文件：** [keil/LoRa/lora.h](keil/LoRa/lora.h) 和 [keil/LoRa/lora.c](keil/LoRa/lora.c)

| 函数 | 说明 |
|---|---|
| `lora_init()` | 初始化 LoRa 模块的 RX FIFO 和帧解析状态机 |
| `lora_send_packet(cmd, data, len)` | 构建帧（加头、校验、尾）并通过 UART2 发送 |
| `lora_rx_drain()` | 从 RX FIFO 取出字节送入帧解析器（主循环中调用） |
| `lora_send_byte(data)` | 发送单个原始字节（用于 AT 命令配置模块参数） |
| `lora_set_on_packet(cb)` | 注册回调函数：收到完整数据包时调用 |

### 9.4 使用示例

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

### 9.5 测试模式

**文件：** [keil/Test/uart_wired_test.c](keil/Test/uart_wired_test.c)

| 模式 | 宏 | 功能 |
|---|---|---|
| 心跳模式 | `UART_WIRED_HEARTBEAT 1` | 每 1 秒发送 `TICK\r\n`，验证 TX 通路 |
| 嗅探模式 | `UART_WIRED_SNIFFER 1` | 将收到的数据 16 进制打印到 UART0 |
| 回声模式 | `UART_WIRED_ECHO 1` | 收到的字节原样发回，验证 TX+RX 通路 |

测试模式共用 UART2 的 FIFO，与 LoRa 协议解析器并行工作，互不影响。

---

## 十、竞赛任务系统

### 10.1 任务模式

上电后默认进入菜单模式（`menu_active = 1`），等待操作指令：

- **HMI 串口屏方式：** 触控按键 1~4 分别对应任务一 ~ 四
- **物理按键方式：** 五向按键上下选择、中键确认

### 10.2 四个任务

| 任务 | 文件 | 功能 |
|---|---|---|
| 任务一 | `Duty/task_one.c` | 巡线 + 圈数统计（基于 IMU 偏航角 360° 检测） |
| 任务二 | `Duty/task_two.c` | 预留 |
| 任务三 | `Duty/task_three.c` | 预留 |
| 任务四 | `Duty/task_four.c` | 预留 |

### 10.3 自动启动模式

设置 `main.c` 中的 `#define AUTO_START_TRACKING 1`，上电后自动启动任务一，无需手动操作 HMI 屏幕。适合比赛现场快速启动。

### 10.4 任务停止

- 通过 HMI 串口屏停止按钮触发
- 或通过代码设置 `g_stop_requested` 标志位
- 任务完成后自动回到菜单模式

---

## 十一、控制系统架构

### 11.1 双环控制结构

```
┌──────────────────────────────────────────────────────┐
│  外环（任务层）                                       │
│  巡线/偏航/棋局 → 产生左右轮速度目标值                │
│                       ↓                              │
│  内环（速度 PID，10ms）                               │
│  速度误差 → PID 计算 → PWM 占空比 → 电机输出          │
└──────────────────────────────────────────────────────┘
```

### 11.2 定时器分配

| 定时器 | 周期 | 功能 |
|---|---|---|
| TIMER_0 (TIMG0) | 1ms | sys_tick 计时、IMU 更新（每 5ms）、灰度采样请求（每 5ms） |
| TIMER_1 (TIMA1) | 10ms | 编码器读取 → 速度 PID 计算 → 电机 PWM 输出 → 云台控制 |

### 11.3 中断分配

| 中断源 | 功能 |
|---|---|
| GROUP1 GPIO | 编码器 A/B 相正交解码（PB5/PB6 左轮，PA29/PA30 右轮） |
| UART0 | VOFA RX 字节 → vofa_rx_fifo |
| UART1 | HMI 串口屏 RX → HMI 协议解析 |
| UART2 | LoRa RX + UART 测试 RX → lora_rx_fifo + uart_wired_test_fifo |
| UART3 | K230 视觉数据接收 |
| CANFD0 | 云台电机 CAN 反馈（RX FIFO0） |

### 11.4 FIFO 缓冲模式

所有 UART 接收均使用 **FIFO 缓冲模式**：ISR 中仅写入 FIFO（极短执行时间），主循环中逐字节取出处理。避免中断内长时间阻塞，保证系统实时性。

---

## 十二、常见问题

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

A: 1) 自行确认模块配置的波特率是否配对；2) 检查 LoRa 模块参数（信道、地址）是否配对；3) 开启嗅探模式 `UART_WIRED_SNIFFER 1` 查看是否有原始字节到达。

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

> **文件编码注意：** 本项目所有源文件使用 **GB2312/GBK** 编码，中文注释请勿以 UTF-8 格式保存，否则 Keil 编译时可能出现乱码或警告。
