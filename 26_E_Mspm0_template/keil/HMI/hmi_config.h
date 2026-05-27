/**
 * @file hmi_config.h
 * @brief HMI串口屏配�?文件 (MSPM0G3507适配�?)
 */
#ifndef __HMI_CONFIG_H__
#define __HMI_CONFIG_H__

#include "ti_msp_dl_config.h"

// UART实例 (SysConfig生成)
#define HMI_UART  UART_display_INST

// 帧头定义
#define HMI_FRAME_TOUCH   0x65   // 触控事件

// HMI 命令结束�? (淘晶�?/TJC 协�??)
#define HMI_END_CMD  "\xff\xff\xff"

// 接收缓冲�?
#define HMI_RX_BUF_SIZE  64

#endif // __HMI_CONFIG_H__
