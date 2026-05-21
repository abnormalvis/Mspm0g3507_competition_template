/*
 * SysConfig Configuration Steps:
 *   I2C:
 *     1. Add an I2C module.
 *     2. Name it as "I2C_OLED".
 *     3. Check the box "Enable Controller Mode".
 *     4. Set "Standard Bus Speed" to "Fast Mode (400kHz)". (optional)
 *     5. Set the pins according to your needs.
 */
 
#ifndef __OLED_H
#define __OLED_H

#include "ti_msp_dl_config.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

// 兼容原有宏定义（确保旧代码可正常编译）
#define OLED_WR_Byte(dat, cmd)  ((cmd == OLED_CMD) ? OLED_WriteCommand(dat) : OLED_WriteData(dat))

//OLED控制用函数
void delay_ms(uint32_t ms);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);

// 新增STM32版本核心函数（主接口）
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t Data);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);

// 兼容原有函数声明（确保头文件与实现匹配）
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no,uint8_t sizey);
void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t sizex, uint8_t sizey,uint8_t BMP[]);
void OLED_Init(void);
void oled_i2c_sda_unlock(void);
void OLED_ShowChess(uint8_t Pos);
void OLED_ShowChess_pro(const int p[9]);

#endif /* #ifndef __OLED_HARDWARE_I2C_H */