#include "ti_msp_dl_config.h"
#include "drv_spi.h"


uint8_t SPI_WriteByte(uint8_t Byte)
{
	while (DL_SPI_isBusy(SPI_0_INST));
	DL_SPI_transmitData8(SPI_0_INST, Byte);
	while(  DL_SPI_isRXFIFOEmpty(SPI_0_INST));
	//while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
	return DL_SPI_receiveData8(SPI_0_INST);
}


/****************SPI***************************/
/*------------ Write one data byte, chip select high -----------*/
void SPI_LCD_WrDat(unsigned char dat)
{
	OLED_CS_Clr();
  OLED_DC_Set();
  SPI_WriteByte(dat);//Send 8-bit data; valid on clock rising edge
	OLED_CS_Set();
}


/*------------ Write command, chip select low -------------*/
void SPI_LCD_WrCmd(unsigned char cmd)
{
	OLED_CS_Clr();
  OLED_DC_Clr();
	SPI_WriteByte(cmd);
  OLED_DC_Set();
	OLED_CS_Set();
}
