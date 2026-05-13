# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Templates

This repository contains TI MSPM0G3507 project templates for the 2027 TI Cup competition:

- **MSPM0G3507_Project_template** - Main template (base project with OLED, VOFA, PID, TB6612, AT24C02 modules)
- **MSPM0G3507_Project_xunji_pid** - Line-following PID template
- **MSPM0G3507_Project_H** - 2024 H�? reference implementation
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
- hal_vofa.c/h - VOFA+上位机调�?
- hal_pid.c/h - PID控制�?
- hal_tb6612.c/h - 双H桥电机驱�?
- hal_at24c02.c/h - EEPROM存储
- hal_encode.c/h - 编码器�?�取
- drv_oled.c/h, ssd1306.c - OLED显示屏驱�?

## Common Issues

If linking errors occur with `DL_SYSCTL_configSYSPLL` or `DL_SYSCTL_switchMCLKfromSYSOSCtoHSCLK`, add `source/ti/driverlib/sysctl/dl_sysctl_mspm0g1x0x_g3x0x.c` to Keil project.

## Current Work

- LCD driver has been integrated into `MSPM0G3507_Project_template` and the old OLED API is being kept as a compatibility layer.
- `drivers/drv_oled.c` now forwards text and clear operations to the LCD driver so the upper-layer menu code can stay unchanged.
- `ndrivers/LCD/lcd.c` has been reduced to the font sets that actually exist in `lcdfont.h`; unavailable large font tables were removed from the rendering path.
- `main.c` now initializes the LCD, clears the screen with `OLED_CLS()`, and initializes the key state machine with `hal_KeyInit()`.
- `keil/Hal/hal_key.h` now declares `hal_KeyInit()` so the project builds cleanly under C99.
- Current runtime focus: confirm the LCD full-screen clear removes refresh artifacts, and verify the key scan callback still drives menu page changes through `pModeMenu->keyVal`.
- Next follow-up: migrate the missing Chinese font table if Chinese text needs to be rendered on the LCD.

## MCU

TI MSPM0G3507 (Cortex-M0+, 128KB Flash, 32KB RAM)