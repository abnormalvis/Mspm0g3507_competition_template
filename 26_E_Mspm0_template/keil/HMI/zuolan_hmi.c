/**
 * @file zuolan_hmi.c
 * @brief HMI人机交互实现文件 (MSPM0G3507适配�?)
 *
 * 发送命名控件数�?到串口屏. 协�??: 控件�?.属�?=值\xff\xff\xff
 */
#include "zuolan_hmi.h"
#include "Delay.h"
#include <math.h>

/**
 * @brief 向HMI控件发送字符串
 * @param obj_name  控件名称 (�? "page1.t1")
 * @param show_data 要显示的字�?�串
 */
void zuolan_HMI_Send_String(char *obj_name, char *show_data)
{
    zuolan_printf("%s.txt=\"%s\"%s", obj_name, show_data, HMI_END_CMD);
}

/**
 * @brief 向HMI控件发送整�?
 * @param obj_name  控件名称 (�? "page1.n0")
 * @param show_data 要显示的整数�?
 */
void zuolan_HMI_Send_Int(char *obj_name, int show_data)
{
    zuolan_printf("%s.val=%d%s", obj_name, show_data, HMI_END_CMD);
}

/**
 * @brief 向HMI控件发送浮点数
 * @param obj_name    控件名称
 * @param show_data   要显示的�?点数
 * @param point_index 小数点后位数
 */
void zuolan_HMI_Send_Float(char *obj_name, float show_data, int point_index)
{
    int temp = (int)(show_data * pow(10, point_index));
    zuolan_printf("%s.val=%d%s", obj_name, temp, HMI_END_CMD);
}

/**
 * @brief 清除波形控件指定通道
 * @param obj_name 波形控件名称
 * @param ch       通道�? (0~3)
 */
void zuolan_HMI_Wave_Clear(char *obj_name, int ch)
{
    zuolan_printf("cle %s,%d%s", obj_name, ch, HMI_END_CMD);
}

/**
 * @brief 向波形控件逐点添加数据 (低�?)
 * @param obj_name 波形控件名称
 * @param ch       通道�? (0~3)
 * @param val      数据�? (0~255)
 */
void zuolan_HMI_Write_Wave_Low(char *obj_name, int ch, int val)
{
    zuolan_printf("add %s.id,%d,%d%s", obj_name, ch, val, HMI_END_CMD);
}

/**
 * @brief 向波形控件批量发送数�? (高�?)
 * @param obj_name 波形控件名称
 * @param ch       通道�? (0~3)
 * @param len      数据点数 (最�?1024)
 * @param val      数据数组 (每个�?0~255)
 */
void zuolan_HMI_Write_Wave_Fast(char *obj_name, int ch, int len, int *val)
{
    zuolan_printf("addt %s.id,%d,%d%s", obj_name, ch, len, HMI_END_CMD);
    Delay_ms(100);
    for (int i = 0; i < len; i++) {
        hmi_send_buf((const uint8_t *)&val[i], 1);
    }
    zuolan_printf("\x01%s", HMI_END_CMD);
}
