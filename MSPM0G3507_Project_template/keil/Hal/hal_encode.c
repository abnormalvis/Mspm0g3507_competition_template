
#include "ti/driverlib/dl_gpio.h"
#include <ti/driverlib/m0p/dl_interrupt.h>
#include "hal_encode.h"
#include "imu_filter.h"                  // Device header
#include "math.h"
#include "hal_jy62.h"
int32_t enc_cnt[2];
encoder NEncoder;
sensor smartcar_imu;
move_filter_struct left_speed_cmps, right_speed_cmps;
float wheel_radius_cm = 2.3f;

void hal_Encoder_Init(void)
{
	  NVIC_EnableIRQ(GPIOA_INT_IRQn);	//ʹ���ⲿ�ж�
	  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}


void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
    case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
    {
        volatile uint32_t mis = GPIOB->CPU_INT.MIS;
        volatile uint32_t din = GPIOB->DIN31_0;
        uint32_t a_state = (din >> 5) & 1;
        uint32_t b_state = (din >> 6) & 1;

        if (mis & (1 << 5))
            enc_cnt[1] += (a_state == b_state) ? -1 : 1;
        if (mis & (1 << 6))
            enc_cnt[1] += (b_state == a_state) ? 1 : -1;

        GPIOB->CPU_INT.ICLR = (1 << 5) | (1 << 6);
    }
    break;
    case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
    {
        volatile uint32_t mis = GPIOA->CPU_INT.MIS;
        volatile uint32_t din = GPIOA->DIN31_0;
        uint32_t a_state = (din >> 29) & 1;
        uint32_t b_state = (din >> 30) & 1;

        if (mis & (1 << 29))
            enc_cnt[0] += (a_state == b_state) ? -1 : 1;
        if (mis & (1 << 30))
            enc_cnt[0] += (b_state == a_state) ? 1 : -1;

        GPIOA->CPU_INT.ICLR = (1 << 29) | (1 << 30);
    }
    break;
    default:
        break;
    }
}

static float get_left_motor_speed(void)
{
	NEncoder.left_motor_cnt = enc_cnt[1];
	//���ٶ�ת����תÿ����
	NEncoder.left_motor_speed_rpm = 60.0f*(NEncoder.left_motor_cnt/pulse_num_per_circle) 
																/(left_motor_period_ms*0.001f);	
	//���ٶ�ת����תÿ��
	NEncoder.left_motor_speed_cmps=2.0f*3.14f*wheel_radius_cm*(NEncoder.left_motor_speed_rpm/60.0f);	 
	move_filter_calc(&left_speed_cmps,NEncoder.left_motor_speed_cmps);	
	
	enc_cnt[1] = 0;	
	
	//return NEncoder.left_motor_speed_cmps;
	//return NEncoder.left_motor_cnt;
	return left_speed_cmps.data_average;

}
/***************************************
������:	void get_right_motor_speed(void)
˵��: ��ȡ�ұ�����ʵ���ٶ�ֵ
���:	��
����:	��
��ע:	����λʱ���ڵ�������,ת����rpm��cm/s
����:	��������
***************************************/
static float get_right_motor_speed(void)
{
	NEncoder.right_motor_cnt = enc_cnt[0];

	//���ٶ�ת����תÿ����
	NEncoder.right_motor_speed_rpm = 60.0f*(NEncoder.right_motor_cnt/pulse_num_per_circle)
																/(right_motor_period_ms*0.001f);	
	//���ٶ�ת����תÿ��
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
	
	
	//��������ƽ���ٶ�
	smartcar_imu.state_estimation.speed=0.5f*(smartcar_imu.left_motor_speed_cmps+smartcar_imu.right_motor_speed_cmps);
	
	point_actual[0] += smartcar_imu.state_estimation.speed*cos(pi/180*imu.yaw)*0.01f;
	point_actual[1] += smartcar_imu.state_estimation.speed*sin(pi/180*imu.yaw)*0.01f;
	
	//ƽ���ٶ�ֱ�ӻ����ܾ���
	smartcar_imu.state_estimation.distance+=(int)(smartcar_imu.state_estimation.speed*5);//�ٶȳ���5����5ms
	//distance_inter = smartcar_imu.state_estimation.distance/1000;

	actual_position_l+=(int)(smartcar_imu.left_motor_speed_cmps*10);//10�ǲ�������10ms
	actual_position_r+=(int)(smartcar_imu.right_motor_speed_cmps*10);//10�ǲ�������10ms
	distance_l = actual_position_l/1000;//ֱ�Ӵ�ӡactual_position_l = actual_position_l/1000;��actual_position_l����ӡ��������Ҫ��һ��ý��
	distance_r = actual_position_r/1000;	

	
	distance_inter = (distance_l+distance_r) * 0.5;
}



