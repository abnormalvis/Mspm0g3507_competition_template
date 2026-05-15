/*
 * LongQiu Technology internal hardware materials. Do not distribute or use commercially.
 * Taobao: www.lckfb.com
 * Technical support forum: https://oshwhub.com/forum
 * Follow bilibili: [LongQiu Technology] for latest updates.
 * LongQiu is dedicated to serving engineers worldwide.
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-08     LCKFB-LP    first version
 */

#ifndef _BSP_AT24C02_H_
#define _BSP_AT24C02_H_

#include "ti_msp_dl_config.h"


// Set SDA output mode
#define SDA_OUT()   {                                                  \
                        DL_GPIO_initDigitalOutput(GPIO_SDA_IOMUX);     \
                        DL_GPIO_setPins(GPIO_PORT, GPIO_SDA_PIN);      \
                        DL_GPIO_enableOutput(GPIO_PORT, GPIO_SDA_PIN); \
                    }
// Set SDA input mode
#define SDA_IN()    { DL_GPIO_initDigitalInput(GPIO_SDA_IOMUX); }
// Read SDA pin level
#define SDA_GET()   ( ( ( DL_GPIO_readPins(GPIO_PORT,GPIO_SDA_PIN) & GPIO_SDA_PIN ) > 0 ) ? 1 : 0 )
// SDA and SCL control macros
#define SDA(x)      ( (x) ? (DL_GPIO_setPins(GPIOA,GPIO_SDA_PIN)) : (DL_GPIO_clearPins(GPIO_PORT,GPIO_SDA_PIN)) )
#define SCL(x)      ( (x) ? (DL_GPIO_setPins(GPIOA,GPIO_SCL_PIN)) : (DL_GPIO_clearPins(GPIO_PORT,GPIO_SCL_PIN)) )

#define FLOAT_BYTE_NUM 4 // Float type byte count

// Float storage union type
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