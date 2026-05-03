/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-08     LCKFB-LP    first version
 */

#ifndef _BSP_AT24C02_H_
#define _BSP_AT24C02_H_

#include "ti_msp_dl_config.h"


//设置SDA输出模式
#define SDA_OUT()   {                                                  \
                        DL_GPIO_initDigitalOutput(GPIO_SDA_IOMUX);     \
                        DL_GPIO_setPins(GPIO_PORT, GPIO_SDA_PIN);      \
                        DL_GPIO_enableOutput(GPIO_PORT, GPIO_SDA_PIN); \
                    }
//设置SDA输入模式
#define SDA_IN()    { DL_GPIO_initDigitalInput(GPIO_SDA_IOMUX); }
//获取SDA引脚的电平变化
#define SDA_GET()   ( ( ( DL_GPIO_readPins(GPIO_PORT,GPIO_SDA_PIN) & GPIO_SDA_PIN ) > 0 ) ? 1 : 0 )
//SDA与SCL输出
#define SDA(x)      ( (x) ? (DL_GPIO_setPins(GPIOA,GPIO_SDA_PIN)) : (DL_GPIO_clearPins(GPIO_PORT,GPIO_SDA_PIN)) )
#define SCL(x)      ( (x) ? (DL_GPIO_setPins(GPIOA,GPIO_SCL_PIN)) : (DL_GPIO_clearPins(GPIO_PORT,GPIO_SCL_PIN)) )

#define FLOAT_BYTE_NUM 4 //float类型占用字节数

//浮点数存储共用体
typedef union
{
    float value;
    uint8_t byte[FLOAT_BYTE_NUM];
} storFloatData;

void AT24C02_test(float data);
void AT24C02_WriteByte(unsigned char WordAddress,unsigned char Data);
unsigned char AT24C02_ReadByte(unsigned char WordAddress);
void Storage_WriteFloatNum( uint16_t addr, storFloatData data );
storFloatData Storage_ReadFloatNum( uint16_t addr );


extern storFloatData data_test;
#endif