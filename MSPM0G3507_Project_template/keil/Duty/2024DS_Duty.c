/*******************************************************************************
  * @brief     : 2024 DianSai competition task
  * @author   : wangming
  * @wechat   : DeepCoderMing
  * @qq       : 3201935299
  * @date     : 2025-05-01
  * @copyright: Confidential - for demo purposes only
********************************************************************************/
#include "2024DS_Duty.h"
#include "mt_flag.h"
#include <ti/driverlib/dl_timerg.h>
#include "hal_beep.h"
#include "hal_led.h"
#include "mt_test.h"
#include "math.h"
#include "imu_filter.h"                  // Device header
#include "hal_math.h"
#include "hal_jy62.h"
#include "app.h"
#include "hal_encode.h"
float L = 100,target_theta;


void DS2024_duty1(void)//����״̬��
{
	
		switch (Param.Send1_Step)//һ��һ���������ظ�
		{
			case 0:
						Flag.Start_duty_1 = 1;
				
						if(Flag.Success_duty_1 == 1)
						{
							Flag.Start_duty_1 =0;
							Flag.Success_duty_1 = 0;
							Param.Send1_Step=1;	
							Flag.task_start = 0;
							task_num = 0;
						}						
				break;
			case 1:			//������ʾ

					break;
		}
		
	
}

void DS2024_duty2(void)//��Ŀ2״̬��
{
		switch (Param.Send2_Step)//һ��һ���������ظ�
		{
			case 0:
						Flag.Start_duty2_1 = 1;			
						if(Flag.Success_duty2_1 == 1)
						{
							Flag.Start_duty2_1 =0;
							Flag.Success_duty2_1 = 0;
							Param.Send2_Step=6;	
						}						
				break;
			case 6:			
						Flag.Start_duty2_7 = 1;// C -> D 30s  �ȴ�200ms
		
						if(Flag.Success_duty2_7 == 1) 
						{
							Flag.Start_duty2_7 =0;
							Flag.Success_duty2_7 = 0;
							Param.Send2_Step=1;								
						}
					break;						
			case 1:			
						Flag.Start_duty2_2 = 1;//ѭ��
			
						if(Flag.Success_duty2_2 == 1)
						{
							Flag.Start_duty2_2 =0;
							Flag.Success_duty2_2 = 0;
							Param.Send2_Step=2;								
						}
					break;
			case 2:			
						Flag.Start_duty2_3 = 1;//��C��У׼�Ƕ�
			
						if(Flag.Success_duty2_3 == 1)
						{
							Flag.Start_duty2_3 =0;
							Flag.Success_duty2_3 = 0;
							Param.Send2_Step=3;								
						}
					break;						
			case 3:			
						Flag.Start_duty2_4 = 1;// C -> D 30s  ����ѭ��
		
						if(Flag.Success_duty2_4 == 1) 
						{
							Flag.Start_duty2_4 =0;
							Flag.Success_duty2_4 = 0;
							Param.Send2_Step=5;								
						}
					break;
			case 5:			
						Flag.Start_duty2_6 = 1;// C -> D 30s  �ȴ�200ms
		
						if(Flag.Success_duty2_6 == 1) 
						{
							Flag.Start_duty2_6 =0;
							Flag.Success_duty2_6 = 0;
							Param.Send2_Step=4;								
						}
					break;						
			case 4:			
						Flag.Start_duty2_5 = 1;// D -> A 30s ѭ��
		
						if(Flag.Success_duty2_5 == 1) 
						{
							task_num = 0;
							Flag.task_start = 0;
							Flag.Start_duty2_5 =0;
							Flag.Success_duty2_5 = 0;
							Param.Send2_Step=0;								
						}
					break;	
						
		}
		
	
}

//ƫ���ǽǶȻ�  ʹ��jy62
float yaw_feedback = 0,yaw_error = 0,yaw = 0,target_yaw = 0,yaw_out[2]={0,0};//�ǶȻ� �����ٶ�λ�ô���Pidһ�£�
float yaw_track_kp = -0.9f;
float yaw_track_ki = 0;
float yaw_track_kd = -0.232;
 
float yaw_track_kp1 = -0.5f;
float yaw_track_ki1 = 0;
float yaw_track_kd1 = -0.1;

float yaw_theta = 0.0f;
void Yaw_auto_track(float target)
{
	static float err,err_last,err_sum;
	float kp,kd;
	yaw_feedback  = imu.yaw;
  err = target - yaw_feedback;	
	if(err >= 180)
	{
		err =  err -360; 
	}
	else if(err <= (-180))
	{
		err =  360+ err; 		
	}
	
	
	
	if(ABS(err)<2)
	{
		yaw_out[1] = 0;
		yaw_out[0] = 0;
	}
	else
	{
			if(task_num < 4)
			{
					kp = yaw_track_kp1;
					kd = yaw_track_kd1;
			}
			else
			{
					kp = yaw_track_kp;
					kd = yaw_track_kd;				
			}
		float a = 1 + yaw_theta * ABS(err)/180;
		kp *= a;
		yaw_out[0] = kp*err+ kd*(err - err_last);  

		yaw_out[0] = -Xianfu_float(yaw_out[0],60);
		yaw_out[1] = -yaw_out[0];
	  err_last = err;		
	}
	
//	v_target_l = yaw_out[0];
//	v_target_r = yaw_out[1];
}

float position_error,position_feedback,position_output;//λ����� λ���ٶ� ����λ��
float pos_track_kp = 6.0;
float pos_track_ki = 0;
float pos_track_kd = 0.3;
float pos_out_limH  = 70;
float pos_out_limL  = 40;
float pos_boudary = 40;
float pos_auto_track(float L)//�����ٶ�λ��
{
	static float err_l,err_last_l,err_r,err_last_r,err_sum_l,err_sum_r;
	
	position_feedback = 0;
	
	err_l = position_error = L- position_feedback;
	if(ABS(err_l) < 0.8)
	{
		position_output = 0;
	}
	else//λ�û����� PID 
	{	
		err_sum_l  += err_l;
		err_sum_l = Xianfu_float(err_sum_l,60); //�����޷�
		position_output = pos_track_kp*err_l+ position_kd*(err_l-err_last_l);//λ�û����
		if(ABS(err_r)<4 && ABS(err_r)>0.8)
		{
			position_output =  (position_output/ABS(position_output))*10 + position_output;
		}		
		err_last_l = err_l;
		
//		if(L >pos_boudary)
//			position_output = Xianfu_float(position_output,pos_out_limH);//50��Ŀ���ٶ� ��λ����������޷�
//		else
		
		if(task_num == 4)
				position_output = Xianfu_float(position_output,pos_out_limH);//50��Ŀ���ٶ� ��λ����������޷�
		else
				position_output = Xianfu_float(position_output,30);//50��Ŀ���ٶ� ��λ����������޷�
	}
	return position_output;

}


void auto_track(float *a,float *b)
{


}

void DS2024_duty3(void)
{
	switch (Param.Send3_Step)//һ��һ���������ظ�
		{
			case -1://�Զ�ѭ��ǰ������λ
						Flag.Start_duty3_0 = 1;				
						if(Flag.Success_duty3_0 == 1)
						{
							Flag.Start_duty3_0 =0;
							Flag.Success_duty3_0 = 0;
							Param.Send3_Step=0;	
						}						
				break;
			case 0://A -> C
						Flag.Start_duty3_1 = 1;
//						p_target_l  = 105;
//						p_target_r  = 105;					
						if(Flag.Success_duty3_1 == 1)
						{
							
							Flag.Start_duty3_1 =0;
							Flag.Success_duty3_1 = 0;
							Param.Send3_Step=4;	
						}						
				break;
			case 4:			//C����ת�Һ���
						Flag.Start_duty3_5 = 1;//
		
						if(Flag.Success_duty3_5 == 1) 
						{
							Flag.Start_duty3_5 =0;
							Flag.Success_duty3_5 = 0;
							Param.Send3_Step=7;			
						}
					break;							
			case 7:			//�ҵ�����ͣһͣ
						Flag.Start_duty3_8 = 1;//
		
						if(Flag.Success_duty3_8 == 1) 
						{
							Flag.Start_duty3_8 =0;
							Flag.Success_duty3_8 = 0;
							Param.Send3_Step=1;			//ȥѭ��
						}
					break;				
			case 1:	//ѭ��
						Flag.Start_duty3_2 = 1;
			
						if(Flag.Success_duty3_2 == 1)
						{
							Flag.Start_duty3_2 =0;
							Flag.Success_duty3_2 = 0;
							Param.Send3_Step=6;								
						}
					break;
			case 6://����ѭ��ǰ������λ
						Flag.Start_duty3_7 = 1;				
						if(Flag.Success_duty3_7 == 1)
						{
							
							Flag.Start_duty3_7 =0;
							Flag.Success_duty3_7 = 0;
							Param.Send3_Step=2;	
						}						
				break;						
			case 2:	//B -> D
						Flag.Start_duty3_3 = 1;
			
						if(Flag.Success_duty3_3 == 1)
						{
							Flag.Start_duty3_3 =0;
							Flag.Success_duty3_3 = 0;
							Param.Send3_Step=5;								 
						}
					break;	
				case 5:	//��ת�Һ��� ��һ��ѭ�� 
						Flag.Start_duty3_6 = 1;
			
						if(Flag.Success_duty3_6 == 1)
						{
							Flag.Start_duty3_6 =0;
							Flag.Success_duty3_6 = 0;
							Param.Send3_Step=8;								 
						}
					break;		
				case 8:	//�ҵ�����ͣһͣ ��һ��ѭ�� 
						Flag.Start_duty3_9 = 1;
			
						if(Flag.Success_duty3_9 == 1)
						{
							Flag.Start_duty3_9 =0;
							Flag.Success_duty3_9 = 0;
							Param.Send3_Step=3;								 
						}
					break;						
			case 3:			
						Flag.Start_duty3_4 = 1;//ѭ��
		
						if(Flag.Success_duty3_4 == 1) 
						{
							task_num = 0;
							Flag.Start_duty3_4 =0;
							Flag.Success_duty3_4 = 0;
							Param.Send3_Step=-1;		
							Flag.task_start = 0;							
							
						}
					break;		
		}	
}
char num_of_turn = 0;
void DS2024_duty4(void)
{
	switch (Param.Send3_Step)//һ��һ���������ظ�
		{
			case -1://�Զ�ѭ��ǰ������λ
						Flag.Start_duty3_0 = 1;				
						if(Flag.Success_duty3_0 == 1)
						{
							Flag.Start_duty3_0 =0;
							Flag.Success_duty3_0 = 0;
							Param.Send3_Step=0;	
						}						
				break;
			case 0://A -> C
						Flag.Start_duty3_1 = 1;
//						p_target_l  = 105;
//						p_target_r  = 105;					
						if(Flag.Success_duty3_1 == 1)
						{
							
							Flag.Start_duty3_1 =0;
							Flag.Success_duty3_1 = 0;
							Param.Send3_Step=1;	
						}						
				break;
			
			case 1:	//ѭ��
						Flag.Start_duty3_2 = 1;
			
						if(Flag.Success_duty3_2 == 1)
						{
							Flag.Start_duty3_2 =0;
							Flag.Success_duty3_2 = 0;
							Param.Send3_Step=6;								
						}
					break;
			case 6://����ѭ��ǰ������λ
						Flag.Start_duty3_7 = 1;				
						if(Flag.Success_duty3_7 == 1)
						{
							
							Flag.Start_duty3_7 =0;
							Flag.Success_duty3_7 = 0;
							Param.Send3_Step=2;	
						}						
				break;						
			case 2:	//B -> D
						Flag.Start_duty3_3 = 1;
			
						if(Flag.Success_duty3_3 == 1)
						{
							Flag.Start_duty3_3 =0;
							Flag.Success_duty3_3 = 0;
							Param.Send3_Step=3;								 
						}
					break;	
			
			case 3:			
						Flag.Start_duty3_4 = 1;//ѭ��
		
						if(Flag.Success_duty3_4 == 1) 
						{
							
							num_of_turn++;
							if(num_of_turn < 4)
							{
								//num_of_turn = 0;//���ܼ�
								Flag.task_start = 2;
							}
							else
							{
								v_target_l = 0;
								v_target_r = 0;									
								task_num = 0;		
								//Flag.Start_Car  = 0;									
								Flag.task_start = 0;
							}								
							
							Flag.Start_duty3_4 =0;
							Flag.Success_duty3_4 = 0;
							Param.Send3_Step=-1;		
							
							
						}
					break;		
		}	
}
