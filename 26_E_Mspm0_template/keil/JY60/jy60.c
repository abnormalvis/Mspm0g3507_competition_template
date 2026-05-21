#include "jy60.h"

static uint8_t RxBuffer1[11];
static uint8_t RxBuffer2[11];
static uint8_t RxBuffer3[11];
/*接收数据数组*/
static volatile uint8_t RxState = 0;/*接收状态标志位*/
static uint8_t RxIndex = 0;/*接受数组索引*/
static float last_yaw_deg;
float Roll,Pitch,Yaw,ax,ay,az,wx,wy,wz,YawInt;/*角度信息，如果只需要整数可以改为整数类型*/



/**
 * @brief       数据包处理函数
 * @param       串口接收的数据RxData
 * @retval      无
 */
void jy60_ReceiveData(uint8_t RxData)
{
	uint8_t i,sum=0;
	
	if (RxState == 0)	//等待包头
	{
		if (RxData == 0x55)	//收到包头
		{
			RxBuffer1[RxIndex] = RxData;
			RxBuffer2[RxIndex] = RxData;
			RxBuffer3[RxIndex] = RxData;
			RxState = 1;
			RxIndex = 1; //进入下一状态
		}
	}
	
	else if (RxState == 1)
	{
		if (RxData == 0x53)	/*判断数据内容，修改这里可以改变要读的数据内容，0x53为角度输出*/
		{
			RxBuffer1[RxIndex] = RxData;
			RxState = 2;
			RxIndex = 2; //进入下一状态
		}
		if (RxData == 0x52)	/*判断数据内容，修改这里可以改变要读的数据内容，0x53为角度输出*/
		{
			RxBuffer2[RxIndex] = RxData;
			RxState = 3;
			RxIndex = 2; //进入下一状态
		}
		if (RxData == 0x51)	/*判断数据内容，修改这里可以改变要读的数据内容，0x53为角度输出*/
		{
			RxBuffer3[RxIndex] = RxData;
			RxState = 4;
			RxIndex = 2; //进入下一状态
		}
	}
	
	else if (RxState == 2)	//接收数据
	{
		RxBuffer1[RxIndex++] = RxData;
		if(RxIndex == 11)	//接收完成
		{
			for(i=0;i<10;i++)
			{
				sum = sum + RxBuffer1[i]; //计算校验和
			}
			if(sum == RxBuffer1[10])		//校验成功
			{
				/*计算数据，根据数据内容选择对应的计算公式*/
				Roll = ((uint16_t) ((uint16_t) RxBuffer1[3] << 8 | (uint16_t) RxBuffer1[2])) / 32768.0f * 180.0f;
				if(Roll>180){Roll=Roll-360;}
				Pitch = ((uint16_t) ((uint16_t) RxBuffer1[5] << 8 | (uint16_t) RxBuffer1[4])) / 32768.0f * 180.0f;
				if(Pitch>180){Pitch=Pitch-360;}
				Yaw = ((uint16_t) ((uint16_t) RxBuffer1[7] << 8 | (uint16_t) RxBuffer1[6])) / 32768.0f * 180.0f;
				// if(Yaw>180){Yaw=Yaw-360;}
				float delta = Yaw - last_yaw_deg;
				delta = (delta > 180) ? (delta - 360) : (delta < -180) ? (delta + 360) : delta;
				YawInt += delta;
				last_yaw_deg = Yaw;
			}
			RxState = 0;
			RxIndex = 0; //读取完成，回到最初状态，等待包头
		}
	}
	else if (RxState == 3)	//接收数据
	{
		RxBuffer2[RxIndex++] = RxData;
		if(RxIndex == 11)	//接收完成
		{
			for(i=0;i<10;i++)
			{
				sum = sum + RxBuffer2[i]; //计算校验和
			}
			if(sum == RxBuffer2[10])		//校验成功
			{
				/*计算数据，根据数据内容选择对应的计算公式*/
				wx = ((uint16_t) ((uint16_t) RxBuffer2[3] << 8 | (uint16_t) RxBuffer2[2])) / 32768.0f * 2000.0f;
				if(wx>2000){wx=wx-4000;}
				wy = ((uint16_t) ((uint16_t) RxBuffer2[5] << 8 | (uint16_t) RxBuffer2[4])) / 32768.0f * 2000.0f;
				if(wy>2000){wy=wy-4000;}
				wz = ((uint16_t) ((uint16_t) RxBuffer2[7] << 8 | (uint16_t) RxBuffer2[6])) / 32768.0f * 2000.0f;
				if(wz>2000){wz=wz-4000;}
			}
			RxState = 0;
			RxIndex = 0; //读取完成，回到最初状态，等待包头
		}
	}
	else if (RxState == 4)	//接收数据
	{
		RxBuffer3[RxIndex++] = RxData;
		if(RxIndex == 11)	//接收完成
		{
			for(i=0;i<10;i++)
			{
				sum = sum + RxBuffer3[i]; //计算校验和
			}
			if(sum == RxBuffer3[10])		//校验成功
			{
				/*计算数据，根据数据内容选择对应的计算公式*/
				ax = ((uint16_t) ((uint16_t) RxBuffer3[3] << 8 | (uint16_t) RxBuffer3[2])) / 32768.0f * 16.0f;
				if(ax>16){ax=ax-32;}
				ay = ((uint16_t) ((uint16_t) RxBuffer3[5] << 8 | (uint16_t) RxBuffer3[4])) / 32768.0f * 16.0f;
				if(ay>16){ay=ay-32;}
				az = ((uint16_t) ((uint16_t) RxBuffer3[7] << 8 | (uint16_t) RxBuffer3[6])) / 32768.0f * 16.0f;
				if(az>16){az=az-32;}
			}
			RxState = 0;
			RxIndex = 0; //读取完成，回到最初状态，等待包头
		}
	}
}

