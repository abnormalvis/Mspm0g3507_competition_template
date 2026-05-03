
#include "ti/driverlib/dl_gpio.h"
#include "hal_encode.h"
#include "imu_filter.h"                  // Device header
#include "math.h"
#include "hal_jy62.h"
int32_t enc_cnt[2];

void hal_Encoder_Init(void)
{
	  NVIC_EnableIRQ(GPIOA_INT_IRQn);	//使能外部中断
}


void GROUP1_IRQHandler(void)
{
		uint32_t status,flag;
		status = DL_GPIO_getEnabledInterruptStatus(GPIOA,DL_GPIO_PIN_29 | DL_GPIO_PIN_30 | DL_GPIO_PIN_5 | DL_GPIO_PIN_6);
	  DL_GPIO_clearInterruptStatus(GPIOA,status);

//左轮
  if(((status & DL_GPIO_PIN_5) == DL_GPIO_PIN_5 )) 
	{
		if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_5) )	//A相上升沿
		{
			if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_6) == 0)
			{
			
				 enc_cnt[1] ++;
			}
			else
			{
				 enc_cnt[1] --;
			}
		}
		else	//A相下降沿
		{
				if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_6))
				{
				
					 enc_cnt[1] ++;
				}
				else
				{
				
					 enc_cnt[1] --;
				}				
		}
	}
	else if(((status & DL_GPIO_PIN_6) == DL_GPIO_PIN_6 )) 
	{
		if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_6) )	//b相上升沿
		{
			if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_5))
			{
			
				 enc_cnt[1] ++;
			}
			else
			{
				 enc_cnt[1] --;
			}
		}
		else	//b相下降沿
		{
				if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_5)==0)
				{
				
					 enc_cnt[1] ++;
				}
				else
				{
				
					 enc_cnt[1] --;
				}				
		}
	}		
	
//	enc_cnt[0] ++;//右
  if(((status & DL_GPIO_PIN_29) == DL_GPIO_PIN_29 )) 
	{
		if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_29) )	//A相上升沿
		{
			if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_30) == 0)
			{
			
				 enc_cnt[0] ++;
			}
			else
			{
				 enc_cnt[0] --;
			}
		}
		else	//A相下降沿
		{
				if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_30))
				{
				
					 enc_cnt[0] ++;
				}
				else
				{
				
					 enc_cnt[0] --;
				}				
		}
	}
	else if(((status & DL_GPIO_PIN_30) == DL_GPIO_PIN_30 )) 
	{
		if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_30) )	//b相上升沿
		{
			if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_29))
			{
			
				 enc_cnt[0] ++;
			}
			else
			{
				 enc_cnt[0] --;
			}
		}
		else	//b相下降沿
		{
				if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_29)==0)
				{
				
					 enc_cnt[0] ++;
				}
				else
				{
				
					 enc_cnt[0] --;
				}				
		}
	}

	
	
}



sensor smartcar_imu;  //左右轮速度 cm/s
encoder NEncoder;  //计算左右轮速度 cm/s 的过程

move_filter_struct left_speed_cmps,right_speed_cmps;

/***************************************
函数名:	float get_left_motor_speed(void)
说明: 获取左边轮子实际速度值
入口:	无
出口:	无
备注:	将单位时间内的脉冲数,转化成rpm、cm/s
作者:	无名创新
***************************************/
static float get_left_motor_speed(void)
{
	NEncoder.left_motor_cnt = enc_cnt[1];
	//将速度转化成转每分钟
	NEncoder.left_motor_speed_rpm = 60.0f*(NEncoder.left_motor_cnt/pulse_num_per_circle) 
																/(left_motor_period_ms*0.001f);	
	//将速度转化成转每秒
	NEncoder.left_motor_speed_cmps=2.0f*3.14f*wheel_radius_cm*(NEncoder.left_motor_speed_rpm/60.0f);	 
	move_filter_calc(&left_speed_cmps,NEncoder.left_motor_speed_cmps);	
	
	enc_cnt[1] = 0;	
	
	//return NEncoder.left_motor_speed_cmps;
	//return NEncoder.left_motor_cnt;
	return left_speed_cmps.data_average;

}
/***************************************
函数名:	void get_right_motor_speed(void)
说明: 获取右边轮子实际速度值
入口:	无
出口:	无
备注:	将单位时间内的脉冲数,转化成rpm、cm/s
作者:	无名创新
***************************************/
static float get_right_motor_speed(void)
{
	NEncoder.right_motor_cnt = -enc_cnt[0];

	//将速度转化成转每分钟
	NEncoder.right_motor_speed_rpm = 60.0f*(NEncoder.right_motor_cnt/pulse_num_per_circle)
																/(right_motor_period_ms*0.001f);	
	//将速度转化成转每秒
	NEncoder.right_motor_speed_cmps=2.0f*3.14f*wheel_radius_cm*(NEncoder.right_motor_speed_rpm/60.0f);	
	move_filter_calc(&right_speed_cmps,NEncoder.right_motor_speed_cmps);	
	enc_cnt[0] = 0;
	//return NEncoder.right_motor_speed_cmps;
	//return NEncoder.right_motor_cnt;
	return right_speed_cmps.data_average;
}
float distance_inter=0,distance_l=0,distance_r=0,distance_point = 0;
float distance_x = 0,distance_y = 0;
float point_A[2],point_B[2],point_C[2],point_D[2],point_actual[2];

float actual_position_l=0,actual_position_r=0;
void get_wheel_speed(void)
{
	smartcar_imu.left_motor_speed_cmps =get_left_motor_speed();
	smartcar_imu.right_motor_speed_cmps=get_right_motor_speed();	
	
	
	//计算两轮平均速度
	smartcar_imu.state_estimation.speed=0.5f*(smartcar_imu.left_motor_speed_cmps+smartcar_imu.right_motor_speed_cmps);
	
	point_actual[0] += smartcar_imu.state_estimation.speed*cos(pi/180*imu.yaw)*0.01f;
	point_actual[1] += smartcar_imu.state_estimation.speed*sin(pi/180*imu.yaw)*0.01f;
	
	//平均速度直接积分总距离
	smartcar_imu.state_estimation.distance+=(int)(smartcar_imu.state_estimation.speed*5);//速度乘上5，是5ms
	//distance_inter = smartcar_imu.state_estimation.distance/1000;

	actual_position_l+=(int)(smartcar_imu.left_motor_speed_cmps*10);//10是采样周期10ms
	actual_position_r+=(int)(smartcar_imu.right_motor_speed_cmps*10);//10是采样周期10ms
	distance_l = actual_position_l/1000;//直接打印actual_position_l = actual_position_l/1000;的actual_position_l，打印不出来，要有一个媒介
	distance_r = actual_position_r/1000;	

	
	distance_inter = (distance_l+distance_r) * 0.5;
}



