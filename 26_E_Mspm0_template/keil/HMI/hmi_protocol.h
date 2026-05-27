/**
 * @file hmi_protocol.h
 * @brief 串口屏触控事件协议解析 (MSPM0G3507)
 */
#ifndef __HMI_PROTOCOL_H__
#define __HMI_PROTOCOL_H__

#include "hmi_config.h"
#include <stdint.h>

/* 解析后的触控事件 */
typedef struct {
    uint8_t  frame_type;   // 帧头: 0x65 = touch
    uint16_t page_id;      // 页面ID (大端)
    uint8_t  widget_id;    // 控件ID
    uint8_t  value;        // 键值
} hmi_event_t;

/**
 * @brief 解析接收到的帧数据
 * @param raw  原始字节数组 (不包含\r\n)
 * @param len  有效数据长度
 * @param out  输出事件结构体
 * @return 0=成功, -1=解析失败
 */
int hmi_parse_frame(const uint8_t *raw, uint8_t len, hmi_event_t *out);

/**
 * @brief 分发事件到系统状态机
 * @param evt 已解析的事件
 */
void hmi_dispatch_event(const hmi_event_t *evt);

#endif // __HMI_PROTOCOL_H__
