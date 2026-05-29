#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_spi.h>

// orientation
#define USE_HORIZONTAL 0

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 280
#else
#define LCD_W 280
#define LCD_H 240
#endif

/* LCD GPIO fallback: no longer managed by sysconfig; LCD replaced by serial screen */
#ifndef LCD_PORT
#define LCD_PORT            GPIOB
#define LCD_RST_PIN         DL_GPIO_PIN_14
#define LCD_DC_PIN          DL_GPIO_PIN_14
#define LCD_LCD_CS_PIN      DL_GPIO_PIN_14
#define LCD_BLK_PIN         DL_GPIO_PIN_14
#endif

#define LCD_RST_PORT   LCD_PORT
#define LCD_DC_PORT    LCD_PORT
#define LCD_LCD_CS_PORT LCD_PORT
#define LCD_BLK_PORT   LCD_PORT

#define LCD_RES_Clr()  DL_GPIO_clearPins(LCD_RST_PORT,    LCD_RST_PIN)
#define LCD_RES_Set()  DL_GPIO_setPins  (LCD_RST_PORT,    LCD_RST_PIN)

#define LCD_DC_Clr()   DL_GPIO_clearPins(LCD_DC_PORT,     LCD_DC_PIN)
#define LCD_DC_Set()   DL_GPIO_setPins  (LCD_DC_PORT,     LCD_DC_PIN)

#define LCD_CS_Clr()   DL_GPIO_clearPins(LCD_LCD_CS_PORT, LCD_LCD_CS_PIN)
#define LCD_CS_Set()   DL_GPIO_setPins  (LCD_LCD_CS_PORT, LCD_LCD_CS_PIN)

#define LCD_BLK_Clr()  DL_GPIO_clearPins(LCD_BLK_PORT, LCD_BLK_PIN)
#define LCD_BLK_Set()  DL_GPIO_setPins  (LCD_BLK_PORT, LCD_BLK_PIN)

void LCD_GPIO_Init(void);
void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_Init(void);

#endif
