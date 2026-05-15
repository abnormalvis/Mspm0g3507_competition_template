/*
 * LongQiu Technology internal hardware materials. Do not distribute or use commercially.
 * Taobao: www.lckfb.com
 * Technical support forum for any questions:
 * LongQiu Forum: https://oshwhub.com/forum
 * Follow bilibili: [LongQiu Technology] for latest updates.
 * LongQiu is dedicated to serving engineers worldwide.
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-08     LCKFB-LP    first version
 */


#include "hal_at24c02.h"
#include "stdio.h"
#include "hal_delay.h"

storFloatData data_test;

// SLAVE ADDRESS+W = 0xA0, SLAVE ADDRESS+R = 0xA1
#define AT24C02_ADDRESS_READ                0xA0
#define AT24C02_ADDRESS_WRITE               0xA1



/******************************************************************
 * Function Name: IIC_Start
 * Description:   IIC start signal
 * Input:         None
 * Output:        None
 * Author:        LC
 * Note:          None
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
 * Function Name: IIC_Stop
 * Description:   IIC stop signal
 * Input:         None
 * Output:        None
 * Author:        LC
 * Note:          None
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
 * Function Name: IIC_Send_Ack
 * Description:   Send ACK or NACK signal
 * Input:         0: ACK, 1: NACK
 * Output:        None
 * Author:        LC
 * Note:          None
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
 * Function Name: I2C_WaitAck
 * Description:   Wait for slave ACK
 * Input:         None
 * Output:        0: ACK received, 1: ACK timeout
 * Author:        LC
 * Note:          None
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
 * Function Name: Send_Byte
 * Description:   Write one byte via IIC
 * Input:         dat - data to write
 * Output:        None
 * Author:        LC
 * Note:          None
******************************************************************/
void Send_Byte(uint8_t dat)
{
    int i = 0;
    SDA_OUT();
    SCL(0);// Pull clock low to start data transfer

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
 * Function Name: Read_Byte
 * Description:   IIC read timing
 * Input:         None
 * Output:        Received data
 * Author:        LC
 * Note:          None
******************************************************************/
unsigned char Read_Byte(void)
{
	unsigned char i,receive=0;

	SDA_IN();// Set SDA as input

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
 * Function Name: AT24C02_WriteByte
 * Description:   Write one byte to AT24C02
 * Input:         WordAddress - address to write, Data - data to write
 * Output:        None
 * Author:        LC
 * Note:          None
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
 * Function Name: AT24C02_ReadByte
 * Description:   Read one byte from AT24C02
 * Input:         WordAddress - address to read
 * Output:        Data read
 * Author:        LC
 * Note:          None
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
Function Name: Storage_WriteFloatNum
Description:   Write 1 float number to specified address
Input:         addr, data
Output:        None
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
Function Name: Storage_ReadFloatNum
Description:   Read 1 float number from specified address
Input:         addr
Output:        float number
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




