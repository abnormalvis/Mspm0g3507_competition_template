/**
 * @file hmi_protocol.c
 * @brief 串口屏触控事件协�?解析与分�? (MSPM0G3507)
 *
 * 当前�?�?:
 * - 0x65: 触控事件 (按键按下)
 *
 * 帧格�?: 65 00 pageH pageL widgetID value
 * 总长5字节 (不含\r\n)
 */
#include "hmi_protocol.h"
#include "zuolan_usart.h"
#include "zuolan_hmi.h"
#include "menu_task.h"
#include "Serial.h"

#define HMI_FRAME_MIN_LEN  5   /* header + pageH + pageL + widget + value */

/**
 * @brief 解析接收到的�?
 * @param raw 原�?�字�? (不包含\r\n)
 * @param len 字节�?
 * @param out 输出事件
 * @return 0=成功, -1=格式错�??或类型不�?�?
 */
int hmi_parse_frame(const uint8_t *raw, uint8_t len, hmi_event_t *out)
{
    if (raw == NULL || out == NULL || len < HMI_FRAME_MIN_LEN) {
        return -1;
    }

    switch (raw[0]) {
    case HMI_FRAME_TOUCH:
        out->frame_type = raw[0];
        out->page_id    = ((uint16_t)raw[1] << 8) | raw[2];
        out->widget_id  = raw[3];
        out->value      = raw[4];
        return 0;

    /* 后续扩展帧类型在此�?�添�? case */

    default:
        return -1;
    }
}

/**
 * @brief 分发已解析的事件
 *
 * �?一阶�??: 回显到串口屏�?认通信正常
 * 后续: �?由到菜单/任务状态机
 */
void hmi_dispatch_event(const hmi_event_t *evt)
{
    if (evt == NULL) return;

    if (evt->frame_type != HMI_FRAME_TOUCH) {
        uart_debug_send_byte('?');  /* unknown frame type */
        return;
    }

    /* debug: log page & widget id */
    uart_debug_send_byte('D');
    uart_debug_send_byte(':');
    uart_debug_send_byte('0' + (uint8_t)evt->page_id);
    uart_debug_send_byte(' ');
    uart_debug_send_byte('0' + evt->widget_id);
    uart_debug_send_byte('\r');
    uart_debug_send_byte('\n');

    /* Accept touch from page 0 or page 1 (screen may use either) */
    if (evt->page_id <= 1)
    {
        switch (evt->widget_id)
        {
        case 1: /* bt0 -> Circle Track */
            g_current_task = TASK_ONE;
            menu_active = 0;
            task_running = 1;
            g_motor_left_out = 0;
            g_motor_right_out = 0;
            zuolan_printf("page1.t0.txt=\"执行中\"%s", HMI_END_CMD);
            break;
        case 2: /* bt1 -> Complex Track */
            g_current_task = TASK_TWO;
            menu_active = 0;
            task_running = 1;
            g_motor_left_out = 0;
            g_motor_right_out = 0;
            zuolan_printf("page1.t1.txt=\"执行中\"%s", HMI_END_CMD);
            break;
        case 3: /* bt2 -> Probe Task */
            g_current_task = TASK_THREE;
            menu_active = 0;
            task_running = 1;
            g_motor_left_out = 0;
            g_motor_right_out = 0;
            zuolan_printf("page1.t2.txt=\"执行中\"%s", HMI_END_CMD);
            break;
        case 4: /* bt3 -> Multi-Lap */
            g_current_task = TASK_FOUR;
            menu_active = 0;
            task_running = 1;
            g_motor_left_out = 0;
            g_motor_right_out = 0;
            zuolan_printf("page1.t3.txt=\"执行中\"%s", HMI_END_CMD);
            break;
        default:
            uart_debug_send_byte('U');  /* unknown widget */
            break;
        }
    }
}
