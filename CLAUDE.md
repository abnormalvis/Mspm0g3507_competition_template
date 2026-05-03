# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Templates

This repository contains TI MSPM0G3507 project templates for the 2027 TI Cup competition:

- **MSPM0G3507_Project_template** - Main template (base project with OLED, VOFA, PID, TB6612, AT24C02 modules)
- **MSPM0G3507_Project_xunji_pid** - Line-following PID template
- **MSPM0G3507_Project_H** - 2024 H题 reference implementation
- **MSPM0G3507_Project_speed_pid** - Speed loop PID template

## Build

Keil uVision 5.39+ required. Open `.uvprojx` file and build.

Dependencies (installed in C:\ti\):
- TI mspm0_sdk_2_02_00_05
- sysconfig (for .syscfg file generation)

## Architecture

Each Keil project follows layered architecture:

```
App/        - Application layer tasks
Duty/       - Competition task state machines
Hal/        - Hardware drivers (spi, oled, vofa, pid, tb6612, at24c02, encoder, uart, timer, etc.)
Mt/         - Middle layer (testing, interrupts)
OS/         - Task scheduler ( cooperative OS )
ZF_IMU/     - ZhuFei IMU library (ICM20602/660RA)
source/     - TI driverlib source files
```

Key modules in Hal/:
- hal_vofa.c/h - VOFA+上位机调参
- hal_pid.c/h - PID控制器
- hal_tb6612.c/h - 双H桥电机驱动
- hal_at24c02.c/h - EEPROM存储
- hal_encode.c/h - 编码器读取
- drv_oled.c/h, ssd1306.c - OLED显示屏驱动

## Common Issues

If linking errors occur with `DL_SYSCTL_configSYSPLL` or `DL_SYSCTL_switchMCLKfromSYSOSCtoHSCLK`, add `source/ti/driverlib/sysctl/dl_sysctl_mspm0g1x0x_g3x0x.c` to Keil project.

## MCU

TI MSPM0G3507 (Cortex-M0+, 128KB Flash, 32KB RAM)