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


#include "hal_at24c02.h"
#include "stdio.h"
#include "hal_delay.h"

storFloatData data_test;

// SLAVE ADDRESS+W为0xA0，SLAVE ADDRESS+R为0xA1
#define AT24C02_ADDRESS_READ                0xA0
#define AT24C02_ADDRESS_WRITE               0xA1



/******************************************************************
 * 函 数 名 称：IIC_Start
 * 函 数 说 明：IIC起始时序
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Start(void)
{
        SDA_OUT();

        SDA(1);
        hal_delay_us(5);
        SCL(1);
        hal_delay_us(5);

        SDA(0);
        hal_delay_us(5);
        SCL(0);
        hal_delay_us(5);

}
/******************************************************************
 * 函 数 名 称：IIC_Stop
 * 函 数 说 明：IIC停止信号
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Stop(void)
{
        SDA_OUT();
        SCL(0);
        SDA(0);

        SCL(1);
        hal_delay_us(5);
        SDA(1);
        hal_delay_us(5);

}

/******************************************************************
 * 函 数 名 称：IIC_Send_Ack
 * 函 数 说 明：主机发送应答或者非应答信号
 * 函 数 形 参：0发送应答  1发送非应答
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Send_Ack(unsigned char ack)
{
        SDA_OUT();
        SCL(0);
        SDA(0);
        hal_delay_us(5);
        if(!ack) SDA(0);
        else     SDA(1);
        SCL(1);
        hal_delay_us(5);
        SCL(0);
        SDA(1);
}


/******************************************************************
 * 函 数 名 称：I2C_WaitAck
 * 函 数 说 明：等待从机应答
 * 函 数 形 参：无
 * 函 数 返 回：0有应答  1超时无应答
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned char I2C_WaitAck(void)
{

    char ack = 0;
    unsigned char ack_flag = 10;
    SCL(0);
    SDA(1);
    SDA_IN();
    hal_delay_us(5);
    SCL(1);
    hal_delay_us(5);

    while( (SDA_GET()==1) && ( ack_flag ) )
    {
        ack_flag--;
        hal_delay_us(5);
    }

    if( ack_flag <= 0 )
    {
        IIC_Stop();
        return 1;
    }
    else
    {
        SCL(0);
        SDA_OUT();
    }
    return ack;
}

/******************************************************************
 * 函 数 名 称：Send_Byte
 * 函 数 说 明：写入一个字节
 * 函 数 形 参：dat要写人的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void Send_Byte(uint8_t dat)
{
    int i = 0;
    SDA_OUT();
    SCL(0);//拉低时钟开始数据传输

    for( i = 0; i < 8; i++ )
    {
        SDA( (dat & 0x80) >> 7 );
        hal_delay_us(1);
        SCL(1);
        hal_delay_us(5);
        SCL(0);
        hal_delay_us(5);
        dat<<=1;
    }
}

/******************************************************************
 * 函 数 名 称：Read_Byte
 * 函 数 说 明：IIC读时序
 * 函 数 形 参：无
 * 函 数 返 回：读到的数据
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned char Read_Byte(void)
{
	unsigned char i,receive=0;

	SDA_IN();//SDA设置为输入

	for(i=0;i<8;i++ )
	{
		SCL(0);
		hal_delay_us(5);
		SCL(1);
		hal_delay_us(5);
		receive<<=1;
		if( SDA_GET() )
		{
			receive|=1;
		}
		hal_delay_us(5);
	}

	SCL(0);

	return receive;
}





/******************************************************************
 * 函 数 名 称：AT24C02_WriteByte
 * 函 数 说 明：AT24C02写入一个字节
 * 函 数 形 参：WordAddress 要写入字节的地址  Data 要写入的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void AT24C02_WriteByte(unsigned char WordAddress,unsigned char Data)
{
        IIC_Start();
        Send_Byte(AT24C02_ADDRESS_READ);
        I2C_WaitAck();
        Send_Byte(WordAddress);
        I2C_WaitAck();
        Send_Byte(Data);
        I2C_WaitAck();
        IIC_Stop();
}

/******************************************************************
 * 函 数 名 称：AT24C02_ReadByte
 * 函 数 说 明：AT24C02读取一个字节
 * 函 数 形 参：WordAddress 要读出字节的地址
 * 函 数 返 回：读出的数据
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned char AT24C02_ReadByte(unsigned char WordAddress)
{
        unsigned char Data;
        IIC_Start();
        Send_Byte(AT24C02_ADDRESS_READ);
        I2C_WaitAck();
        Send_Byte(WordAddress);
        I2C_WaitAck();
        IIC_Start();
        Send_Byte(AT24C02_ADDRESS_WRITE);
        I2C_WaitAck();
        Data=Read_Byte();
        IIC_Send_Ack(1);
        IIC_Stop();
        return Data;
}

/********************************************************************************************************
函数名称：
函数功能：向指定地址写入1个浮点数
详    述：
输    入：
*********************************************************************************************************/

void Storage_WriteFloatNum( uint16_t addr, storFloatData data )
{
    uint8_t i = 0;
    uint8_t a = 0;

    for( i = 0; i <= FLOAT_BYTE_NUM - 1; i++ )
    {
      AT24C02_WriteByte( addr + i, data.byte[i]);
			hal_delay_ms(5);
    }
return;
}

void AT24C02_Write_Float(uint16_t WriteAddr,float *pBuffer)
{
    uint8_t i = 0;
    union FB
    {
        float a;
        uint8_t b[4];
    }fb;
    
    fb.a = *pBuffer;
    
	
    for(i = 0; i < 4; i++)
    {
        AT24C02_WriteByte(WriteAddr,fb.b[i]);
        WriteAddr++;
    }
	
}
/********************************************************************************************************
函数名称：
函数功能：从指定地址读出1个浮点数
详    述：
输    入：
*********************************************************************************************************/
storFloatData Storage_ReadFloatNum( uint16_t addr )
{
    uint8_t i = 0;
    storFloatData read_data;

    for( i = 0; i <= FLOAT_BYTE_NUM - 1; i++ )
    {
        read_data.byte[i] = AT24C02_ReadByte( addr + i );
    }

    return read_data;
}




void Write_Flaot_data(uint32_t add,float data)
{
	storFloatData f_data_buff;
	f_data_buff.value=data;
	add=0+add*4;
	
	AT24C02_WriteByte(add,f_data_buff.value);
}




