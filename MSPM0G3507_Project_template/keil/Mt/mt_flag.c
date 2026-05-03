/*******************************************************************************
  * @作者      ： wangming
  * @wechat    :DeepCoderMing
  * @qq      ： 3201935299
  * @日期      ： 2025年05月01日
  * @版权声明  ： 仅供参考学习，未经允许禁止商用
********************************************************************************/
#include "mt_flag.h"
#include "hal_encode.h"
#include "mt_test.h"
#include "2024DS_Duty.h"
Param_InitTypeDef Param;//定义了要用的参数结构体
Flag_InitTypeDef  Flag;//定义了要用的标志位结构体 
extern float	speed_integral[2],speed_output[2];
extern float position_output;
extern float yaw_out[2];
void Flag_Init(void)
{
	  Param.Send1_Step =0;
		Param.Send2_Step =0;
	  Param.Send3_Step =-1;
		Param.Send4_Step =0;
	
		Flag.Start_Car  = 0;
		Flag.Stop_Car = 0;
		Flag.Start_Car= 0;//开始发车
		Flag.Stop_Car= 0;
		
		Flag.Start_duty_1= 0;
		Flag.Success_duty_1= 0;

		Flag.Start_duty2_1= 0;
		Flag.Success_duty2_1= 0;	

		Flag.Start_duty2_2= 0;
		Flag.Success_duty2_2= 0;	

		Flag.Start_duty2_3= 0;
		Flag.Success_duty2_3= 0;	
		Flag.Start_duty2_4= 0;
		Flag.Success_duty2_4= 0;	

		Flag.Start_duty2_5= 0;
		Flag.Success_duty2_5= 0;	

		Flag.Start_duty2_6= 0;
		Flag.Success_duty2_6= 0;	
		
		Flag.Start_duty2_7= 0;
		Flag.Success_duty2_7= 0;	
		
		Flag.Start_duty3_0= 0;
		Flag.Success_duty3_0= 0;
		
		Flag.Start_duty3_1= 0;
		Flag.Success_duty3_1= 0;	

		Flag.Start_duty3_2= 0;
		Flag.Success_duty3_2= 0;	

		Flag.Start_duty3_3= 0;
		Flag.Success_duty3_3= 0;	
		
		Flag.Start_duty3_4= 0;
		Flag.Success_duty3_4= 0;	

		Flag.Start_duty3_5= 0;
		Flag.Success_duty3_5= 0;
		
		Flag.Start_duty3_6= 0;
		Flag.Success_duty3_6= 0;
		
		Flag.Start_duty3_7= 0;
		Flag.Success_duty3_7= 0;	

		Flag.Start_duty3_8= 0;
		Flag.Success_duty3_8= 0;	
		
		Flag.Start_duty3_9= 0;
		Flag.Success_duty3_9= 0;	
		
		Flag.gray_worse= 0;
		//Flag.beep_on	= 0;
		Flag.task_start= 0;	
		Flag.angle_loss = 0;
		Flag.yaw_loss = 0;
		Flag.yaw_loss_ahead = 0;
		num_of_turn = 0;
		gray_cnt = 0;
				speed_output[1] = speed_output[0] = 0;
		position_output = 0;
		yaw_out[0] = 0;
//		point_actual[0] = point_A[0];
//		point_actual[1] = point_A[1];
		
}

//void Flag_clear(void)
//{
//	
//	
//	
//}

