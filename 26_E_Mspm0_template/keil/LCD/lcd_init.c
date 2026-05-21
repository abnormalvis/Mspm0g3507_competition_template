#include "ti_msp_dl_config.h"
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_spi.h>
#include <stdint.h>
#include "lcd.h"
#include "lcd_init.h"
#include "Delay.h"

void LCD_GPIO_Init(void)
{
    // GPIO init for PORTB pins is handled by SysConfig generated code (SYSCFG_DL_GPIO_init)
}

void LCD_Writ_Bus(uint8_t dat)
{
    LCD_CS_Clr();

    DL_SPI_transmitData8(SPI_LCD_INST, dat);
    while(DL_SPI_isBusy(SPI_LCD_INST));

    LCD_CS_Set();
}

void LCD_WR_DATA8(uint8_t dat)
{
    LCD_Writ_Bus(dat);
}

void LCD_WR_DATA(uint16_t dat)
{
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}

void LCD_WR_REG(uint8_t dat)
{
    LCD_DC_Clr();
    LCD_Writ_Bus(dat);
    LCD_DC_Set();
}

void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    if(USE_HORIZONTAL==0)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1+20);
        LCD_WR_DATA(y2+20);
        LCD_WR_REG(0x2c);
    }
    else if(USE_HORIZONTAL==1)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1+20);
        LCD_WR_DATA(y2+20);
        LCD_WR_REG(0x2c);
    }
    else if(USE_HORIZONTAL==2)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1+20);
        LCD_WR_DATA(x2+20);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);
    }
    else
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1+20);
        LCD_WR_DATA(x2+20);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);
    }
}

void LCD_Init(void)
{
    LCD_GPIO_Init();

    LCD_RES_Clr();
    Delay_ms(100);
    LCD_RES_Set();
    Delay_ms(100);

    LCD_WR_REG(0x11);
    Delay_ms(120);

    LCD_WR_REG(0x36);
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x15);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20);

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F);

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
}
