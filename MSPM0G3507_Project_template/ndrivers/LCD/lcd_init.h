#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_spi.h>

#define LCD_GPIO_PORT PORTB_PORT
#define LCD_SPI_INST SPI_0_INST

// orientation
#define USE_HORIZONTAL 0

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 280
#else
#define LCD_W 280
#define LCD_H 240
#endif

// Map LCD control pins to target project PORTB pins
#define LCD_RES_Clr()  DL_GPIO_clearPins(LCD_GPIO_PORT, PORTB_RST_PIN)
#define LCD_RES_Set()  DL_GPIO_setPins(LCD_GPIO_PORT, PORTB_RST_PIN)

#define LCD_DC_Clr()   DL_GPIO_clearPins(LCD_GPIO_PORT, PORTB_DC_PIN)
#define LCD_DC_Set()   DL_GPIO_setPins(LCD_GPIO_PORT, PORTB_DC_PIN)

#define LCD_CS_Clr()   DL_GPIO_clearPins(LCD_GPIO_PORT, PORTB_CS_PIN)
#define LCD_CS_Set()   DL_GPIO_setPins(LCD_GPIO_PORT, PORTB_CS_PIN)

// Backlight: use PB26 if available (fallback to PORTB_CS_PIN if not)
#ifndef PORTB_BLK_PIN
#define LCD_BLK_Clr()  DL_GPIO_clearPins(LCD_GPIO_PORT, DL_GPIO_PIN_26)
#define LCD_BLK_Set()  DL_GPIO_setPins(LCD_GPIO_PORT, DL_GPIO_PIN_26)
#else
#define LCD_BLK_Clr()  DL_GPIO_clearPins(LCD_GPIO_PORT, PORTB_BLK_PIN)
#define LCD_BLK_Set()  DL_GPIO_setPins(LCD_GPIO_PORT, PORTB_BLK_PIN)
#endif

// SPI instance used for LCD
#ifndef SPI_LCD_INST
#define SPI_LCD_INST LCD_SPI_INST
#endif

void LCD_GPIO_Init(void);
void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_Init(void);

#endif
