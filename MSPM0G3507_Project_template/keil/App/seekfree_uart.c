/*******************************************************************************
  * @作者      ： wangming
  * @wechat    :DeepCoderMing
  * @qq      ： 3201935299
  * @日期      ： 2025年05月01日
  * @版权声明  ： 仅供参考学习，未经允许禁止商用
********************************************************************************/
#include "seekfree_uart.h"
#include "seekfree_assistant.h"
#include "mt_test.h"
#include "hal_encode.h"
#include "hal_at24c02.h"
#include "hal_gray.h"
#include "hal_at24c02.h"
#include "mt_flag.h"
#include "hal_jy62.h"
#include "user_interrupt.h"
#include "imu_filter.h"                  // Device header
#include "mt_test.h"
#include "2024DS_Duty.h"
void  uartint(void)
{
    seekfree_assistant_init();
// 初始化逐飞助手示波器的结构体

    seekfree_assistant_oscilloscope_data.data[0] = 0;
    seekfree_assistant_oscilloscope_data.data[1] = 0;
    seekfree_assistant_oscilloscope_data.data[2] = 0;
    seekfree_assistant_oscilloscope_data.data[3] = 0;
    seekfree_assistant_oscilloscope_data.data[4] = 0;
    seekfree_assistant_oscilloscope_data.data[5] = 0;
    seekfree_assistant_oscilloscope_data.data[6] = 0;
    seekfree_assistant_oscilloscope_data.data[7] = 0;
        // 通道数量最大为8个
    seekfree_assistant_oscilloscope_data.channel_num = 8;
}

void  uartwork(void)
{
    seekfree_assistant_data_analysis();
	
    // 遍历
    for (uint8_t i = 0; i < SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT; i++)
    {
        // 更新标志位
        if (seekfree_assistant_parameter_update_flag[i])
        {
            seekfree_assistant_parameter_update_flag[i] = 0;
						//seekfree_assistant_oscilloscope_data.data[4] = seekfree_assistant_parameter[i];	
                switch (i)
                {
                    case 0 :
                        //v_target_l = seekfree_assistant_parameter[0];
										turn_kp_L = seekfree_assistant_parameter[0];
//													p_target_l = seekfree_assistant_parameter[0];
//													turn_kp	=	seekfree_assistant_parameter[0];
//													data_test.value = turn_kp;
//										cam_turn_kp = seekfree_assistant_parameter[0];
//													data_test.value = cam_turn_kp;										
												//	Storage_WriteFloatNum(16,data_test);										
										
//													yaw_track_kp = seekfree_assistant_parameter[0];
//													data_test.value = yaw_track_kp;
//													Storage_WriteFloatNum(15*4,data_test);

                        break;
                    case 1 :
                        turn_kd_L = seekfree_assistant_parameter[1];
//													p_target_r = seekfree_assistant_parameter[1];
//													turn_kd	=	seekfree_assistant_parameter[1];
//													data_test.value = turn_kd;
//										cam_turn_kd = seekfree_assistant_parameter[1];
//													data_test.value = cam_turn_kp;										
												//	Storage_WriteFloatNum(20,data_test);											
//													yaw_track_kd = seekfree_assistant_parameter[1];
//													data_test.value = yaw_track_kd;										
//													Storage_WriteFloatNum(16*4,data_test);			
                        break;
                    case 2 :
                        speed_kp_l = seekfree_assistant_parameter[2];
//										position_kp = seekfree_assistant_parameter[2];
//										data_test.value = position_kp;
									//	Storage_WriteFloatNum(8,data_test);										
//										turn_kp= seekfree_assistant_parameter[2];
//														data_test.value = turn_kp;										
//													Storage_WriteFloatNum(52,data_test);									
                        break;
                    case 3 :
                        speed_ki_l = seekfree_assistant_parameter[3];
//										position_kd = seekfree_assistant_parameter[3];
//										data_test.value = position_kd;
										//Storage_WriteFloatNum(12,data_test);		
//										turn_kd = seekfree_assistant_parameter[3];
                        break;
                    case 4 :
                        speed_kp_r = seekfree_assistant_parameter[4];
                        break;
                    case 5 :
                        speed_ki_r = seekfree_assistant_parameter[5];
                        break;
                    case 6 :
												Flag.Start_Car = seekfree_assistant_parameter[6];
                        //left_pwm = seekfree_assistant_parameter[6];
                        break;
                    case 7 :
//                        yaw_target = seekfree_assistant_parameter[7]; 
//													data_test.value = speed_setup;
//													Storage_WriteFloatNum(4,data_test);
                        break;										
                }
        
            }
            // 通过DEBBUG串口发送信息

					}

	float a = atan((point_D[1] - point_actual[1])/(point_D[0] - point_actual[0]))*180/pi;//目标角度
  float err = a -  angle.z;	
	if(err >= 180)
	{
		err = 360 - err; 
	}
	else if(err <= (-180))
	{
		err = -360 - err; 		
	}   
	extern float yaw_out[2];
	extern float position_output ;
		seekfree_assistant_oscilloscope_data.data[0] = imu.yaw;     //0
    seekfree_assistant_oscilloscope_data.data[1] = target_theta;
    seekfree_assistant_oscilloscope_data.data[2] = position_output;     //0
    seekfree_assistant_oscilloscope_data.data[3] = smartcar_imu.state_estimation.speed;	//
		seekfree_assistant_oscilloscope_data.data[4] = smartcar_imu.left_motor_speed_cmps;			
		seekfree_assistant_oscilloscope_data.data[5] = smartcar_imu.right_motor_speed_cmps;		
		seekfree_assistant_oscilloscope_data.data[6] =	speed_setup;		
		seekfree_assistant_oscilloscope_data.data[7] =	v_target_r;				
					
		seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
    }
