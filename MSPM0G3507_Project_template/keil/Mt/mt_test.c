#include "mt_test.h"
#include "hal_encode.h"
#include "hal_math.h"
#include "hal_tb6612.h"
#include "hal_jy62.h"
#include "hal_gray.h"
#include "hal_pid.h"
#include "hal_uart.h"
#include "mt_flag.h"
#include "user_interrupt.h"
#include "hal_led.h"
#include "2024DS_Duty.h"
#include "math.h"
#include "hal_beep.h"
#include "app.h"
#include "hal_vofa.h"
#include "hal_math.h"
#include "hal_tb6612.h"
#include "hal_jy62.h"
#include "hal_gray.h"
#include "hal_pid.h"
#include "hal_uart.h"
#include "mt_flag.h"
#include "user_interrupt.h"
#include "hal_led.h"
#include "2024DS_Duty.h"
#include "math.h"
#include "hal_beep.h"
#include "app.h"

static float speed_error[2] = {0, 0}, speed_expect[2] = {speed_expect_default, speed_expect_default}, speed_feedback[2] = {0, 0}; // �ٶ����? �����ٶ� �����ٶ�
static float speed_error_last[2] = {0, 0};
// static float position_kp = position_kp_default,position_ki = position_ki_default,position_kd = position_kd_default;
// ��ɫ��
// float speed_kp_l=0.5f,speed_ki_l=0.12f,speed_kd=speed_kd_default;
// float speed_kp_r=0.64f,speed_ki_r=0.15f;
float speed_kp_l = 0.9f, speed_ki_l = 0.2f, speed_kd = speed_kd_default;
float speed_kp_r = 0.9f, speed_ki_r = 0.2f;

float position_kp = 6.0;
float position_ki = 0;
float position_kd = 0.3;

float yaw_kp = 0.6;
float yaw_ki = 0;
float yaw_kd = 0.5;
// �ڳ�
// float speed_kp_l=1.1f,speed_ki_l=0.2f;
// float speed_kp_r=0.9f,speed_ki_r=0.23f;
// float speed_kp=speed_kp_default,speed_ki=speed_ki_default,speed_kd=speed_kd_default;
static float position_error[2] = {0, 0}, position_feedback[2] = {0, 0}, position_output[2] = {0, 0}, yaw_out[2] = {0, 0}; // λ�����? λ���ٶ� ����λ��
static float yaw_feedback = 0, yaw_error = 0, yaw = 0, target_yaw = 0;													  // �ǶȻ� �����ٶ�λ�ô���Pidһ�£�

float v_target_l = 0, v_target_r = 0; // ��λcm/s ��ʼ���ٶȻ�Ŀ���ٶ�
float p_target_l = 100, p_target_r = 100;
float speed_integral[2] = {0, 0}, speed_output[2] = {0, 0};
float left_pwm, right_pwm; // ���ҵ�����յ����ֵ

float turn_output = 0, turn_output_last = 0; // ����������?
controller seektrack_ctrl[2];				 // ����Ѱ���������ṹ��,seektrack_ctrl[0]�Ҷȹ�ѭ��;seektrack_ctrl[1]OpenMVѭ��
float turn_ctrl_pwm = 0;					 // ת��������
// float	turn_scale=turn_scale_default;//ת����Ʋ���ϵ��?  0.15 ת����������ת����������������ʱ������ת��ϵ��
float turn_scale = 0.06;				   // ת����Ʋ���ϵ��?  0.15 ת����������ת����������������ʱ������ת��ϵ��
float speed_setup = 70, speed_adjust = 35; // �ٶ��趨ֵ  �ٶ�Ŀ��ֵ���������ʽ������?
float yaw_target = 0;
/*�ٶȻ� λ�û� ���� �ǶȻ�*/
// void speed_control(void)
//{
//	speed_feedback[0]=smartcar_imu.left_motor_speed_cmps;//��ȡ����ʵ��ֵ
//	speed_error[0]=v_target_l-speed_feedback[0];
//	speed_error[0]=Xianfu_float(speed_error[0],speed_err_max);
//	speed_integral[0]+=speed_ki_l*speed_error[0];
//	speed_integral[0]=Xianfu_float(speed_integral[0],speed_integral_max);
//	speed_output[0]=speed_integral[0]+speed_kp_l*speed_error[0];
//	speed_output[0]=Xianfu_float(speed_output[0],speed_ctrl_output_max);
//
//	speed_feedback[1]=smartcar_imu.right_motor_speed_cmps;//����
//	speed_error[1]=v_target_r-speed_feedback[1];//�����ٶȼ�ȥʵ���ٶȵõ��ٶ����?
//
//	speed_error[1]=Xianfu_float(speed_error[1],speed_err_max);//���ٶ������Լ��?
//	speed_integral[1]+=speed_ki_r*speed_error[1]; //�ٶȻ���
//	speed_integral[1]=Xianfu_float(speed_integral[1],speed_integral_max);//�Եõ����ٶȻ�����Լ��
//	speed_output[1]=speed_integral[1]+speed_kp_r*speed_error[1];//pid�õ����?
//	speed_output[1]=Xianfu_float(speed_output[1],speed_ctrl_output_max);//�������Լ��?
//
// }
float add_limit[2] = {};
float speed_theta = 0;
void speed_control(void)
{

	speed_feedback[0] = smartcar_imu.left_motor_speed_cmps; // ��ȡ����ʵ��ֵ
	speed_error[0] = v_target_l - speed_feedback[0];
	speed_error[0] = Xianfu_float(speed_error[0], speed_err_max);
	speed_integral[0] = speed_ki_l * speed_error[0];
	//	speed_integral[0]=Xianfu_float(speed_integral[0],speed_integral_max);
	add_limit[0] = speed_integral[0] + speed_kp_l * (speed_error[0] - speed_error_last[0]);
	if (ABS(add_limit[0]) > 0.5)
		speed_output[0] += add_limit[0];
	speed_output[0] = Xianfu_float(speed_output[0], speed_ctrl_output_max);
	speed_error_last[0] = speed_error[0];

	speed_feedback[1] = smartcar_imu.right_motor_speed_cmps;	  // ����
	speed_error[1] = v_target_r - speed_feedback[1];			  // �����ٶȼ�ȥʵ���ٶȵõ��ٶ����?
	speed_error[1] = Xianfu_float(speed_error[1], speed_err_max); // ���ٶ������Լ��?
	speed_integral[1] = speed_ki_r * speed_error[1];			  // �ٶȻ���
	//	speed_integral[1]=Xianfu_float(speed_integral[1],speed_integral_max);//�Եõ����ٶȻ�����Լ��
	add_limit[1] = speed_integral[1] + speed_kp_r * (speed_error[1] - speed_error_last[1]);
	if (ABS(add_limit[1]) > 0.5)
		speed_output[1] += add_limit[1];									// pid�õ����?
	speed_output[1] = Xianfu_float(speed_output[1], speed_ctrl_output_max); // �������Լ��?
	speed_error_last[1] = speed_error[1];

	/* Update speed PID from VOFA and send feedback */
	float kp_temp, ki_temp;
	{
		uint8_t pid_mask = vofa_get_speed_pid(&kp_temp, &ki_temp);
		if (pid_mask & 1)
			speed_kp_l = kp_temp;
		if (pid_mask & 2)
			speed_ki_l = ki_temp;
	}
	/* Check for speed target command */
	float target = vofa_get_speed_target();
	static float last_target = 0;
	if (target > 0.1f || target < -0.1f)
	{
		last_target = target;
	}
	else
	{
		/* Keep using last target if no new command */
		target = last_target;
	}
	if (target > 0.1f || target < -0.1f)
	{
		/* #4 is angular velocity (rad/s), convert to cm/s */
		v_target_l = target * wheel_radius_cm;
		v_target_r = target * wheel_radius_cm;
	}
	if (vofa_get_debug_mode() == 1)
	{
		/* Auto-enable motor output in debug mode */
		if (!Flag.Start_Car)
			Flag.Start_Car = 1;
		/* Send 5 channels: target(rad/s), left(rad/s), right(rad/s), kp, ki */
		vofa_send_speed_feedback(target,
								 smartcar_imu.left_motor_speed_cmps / wheel_radius_cm,
								 smartcar_imu.right_motor_speed_cmps / wheel_radius_cm,
								 speed_kp_l, speed_ki_l);
	}
}

void position_control(void) // �����ٶ�λ�ô���PID
{
	static float err_l, err_last_l, err_r, err_last_r, err_sum_l, err_sum_r;

	position_feedback[0] = distance_l;
	err_l = position_error[0] = p_target_l - position_feedback[0];

	if (ABS(err_l) < 0.8)
	{
		position_output[0] = 0;
	}
	else // λ�û����� PID
	{
		err_sum_l += err_l;
		err_sum_l = Xianfu_float(err_sum_l, 60);																 // �����޷�
		position_output[0] = position_kp * err_l + position_ki * err_sum_l + position_kd * (err_l - err_last_l); // λ�û����?
		if (ABS(err_r) < 4 && ABS(err_r) > 0.8)
		{
			position_output[0] = (position_output[0] / ABS(position_output[0])) * 10 + position_output[0];
		}
		err_last_l = err_l;

		position_output[0] = Xianfu_float(position_output[0], 30); // 50��Ŀ���ٶ� ��λ����������޷�?
	}
	v_target_l = position_output[0];

	position_feedback[1] = distance_r;
	err_r = position_error[1] = p_target_r - position_feedback[1];
	if (ABS(err_r) < 0.8)
	{
		position_output[1] = 0;
	}
	else
	{
		err_sum_r += err_r;
		err_sum_r = Xianfu_float(err_sum_r, 60);																 // �����޷�
		position_output[1] = position_kp * err_r + position_ki * err_sum_r + position_kd * (err_r - err_last_r); // λ�û����?
		if (ABS(err_r) < 4 && ABS(err_r) > 0.8)
		{
			position_output[1] = (position_output[1] / ABS(position_output[1])) * 10 + position_output[1];
		}
		err_last_r = err_r;
		position_output[1] = Xianfu_float(position_output[1], 30); // 50��Ŀ���ٶ� ��λ����������޷�?
	}
	v_target_r = position_output[1];
}

// ƫ���ǽǶȻ�  ʹ��jy62
void Yaw_control(float target)
{
	static float err, err_last, err_sum;
	yaw_feedback = angle.z;
	err = target - yaw_feedback;
	if (err >= 180)
	{
		err = 360 - err;
	}
	else if (err <= (-180))
	{
		err = -360 - err;
	}
	if (ABS(err) < 2)
	{
		yaw_out[1] = 0;
		yaw_out[0] = 0;
	}
	else
	{
		err_sum += err;
		err_sum = Xianfu_float(err_sum, 400);
		yaw_out[0] = yaw_kp * err + yaw_ki * err_sum + yaw_kd * (err - err_last);
		if (ABS(err) < 5 && ABS(err) > 1)
		{
			yaw_out[0] = (yaw_out[0] / ABS(yaw_out[0])) * 10 + yaw_out[0];
		}
		yaw_out[0] = -Xianfu_float(yaw_out[0], 60);
		yaw_out[1] = -yaw_out[0];
		err_last = err;
	}

	v_target_l = yaw_out[0];
	v_target_r = yaw_out[1];
}

// �Ҷ�ѭ��
void ctrl_params_init(void)
{
	pid_control_init(&seektrack_ctrl[0], // ����ʼ���������ṹ�壬�Ҷȹ�ѭ��
					 turn_kp_default1,	 // ��������
					 turn_ki_default1,	 // ���ֲ���
					 turn_kd_default1,	 // ΢�ֲ���
					 20,				 // ƫ���޷�ֵ
					 0,					 // �����޷�ֵ
					 500,				 // ����������޷��?
					 1,					 // ƫ���޷���־λ
					 0, 0,				 // ���ַ����־λ��������ֿ���ʱ���޷�ֵ
					 6);				 // ΢�ּ��ʱ��?

	//	pid_control_init(&seektrack_ctrl[1],//����ʼ���������ṹ�壬OpenMVѭ��
	//										turn_kp_default2,//��������
	//										turn_ki_default2,//���ֲ���
	//										turn_kd_default2,//΢�ֲ���
	//										20, //ƫ���޷�ֵ
	//										0,  //�����޷�ֵ
	//										500,//����������޷��?
	//										1,	//ƫ���޷���־λ
	//										0,0,//���ַ����־λ��������ֿ���ʱ���޷�ֵ
	//										1); //΢�ּ��ʱ��?
}
// float turn_kp	=		20.0f ;
// float turn_ki	=		0.0f	;
// float turn_kd	=		110		;

float turn_kp = 3.9f;
float turn_ki = 0.0f;
float turn_kd = 5.2f;

float turn_kp_L = 2.f;
float turn_ki_L = 0.0f;
float turn_kd_L = 0.6f;
// turn_ctrl_pwm = output  (-500,500)
float turn_theta = 0.5f;
void gray_turn_control_200hz(float *output) // 200HZ=5ms
{
	float kp, kd;

	if (task_num < 4)
	{
		kp = turn_kp_L;
		kd = turn_kd_L;
	}
	else
	{

		kp = turn_kp;
		kd = turn_kd;
	}
	float a = 1 + turn_theta * ABS(gray_status) / 15;
	kp *= a;

	pid_control_init(&seektrack_ctrl[0],
					 kp,
					 0,
					 kd,
					 10,
					 0,
					 90,
					 1,
					 0, 0,
					 6);
	turn_output_last = turn_output;
	seektrack_ctrl[0].expect = 0;
	seektrack_ctrl[0].feedback = -gray_status;
	pid_control_run(&seektrack_ctrl[0]);
	turn_output = seektrack_ctrl[0].output;

	//	if(turn_output>0) turn_output+=steer_deadzone;//steer_deadzone ֵΪ50
	//	if(turn_output<0) turn_output-=steer_deadzone;
	turn_output = Xianfu_float(turn_output, 70);

	*output = 0.75f * turn_output + 0.25f * turn_output_last;
}
float cam_turn_kp = 0.f;  // KP	 λ�ÿ�����
float cam_turn_ki = 0.0f; // ���ں���Թ�ʶ��켣ʱ,����Ѱ����λ�ÿ��������ֲ���KI
float cam_turn_kd = 0.0f; //
void openmv_openmv_duty_run(float *output)
{
	pid_control_init(&seektrack_ctrl[1], // ����ʼ���������ṹ�壬�Ҷȹ�ѭ��
					 cam_turn_kp,		 // ��������
					 cam_turn_ki,		 // ���ֲ���
					 cam_turn_kd,		 // ΢�ֲ���
					 10,				 // ƫ���޷�ֵ
					 0,					 // �����޷�ֵ
					 50,				 // ����������޷��?
					 1,					 // ƫ���޷���־λ
					 0, 0,				 // ���ַ����־λ��������ֿ���ʱ���޷�ֵ
					 6);				 // ΢�ּ��ʱ��?

	// �����ϴο��������?
	turn_output_last = turn_output;
	// ת��PD����������������ʵʱ��Ҫ��ߣ��������I��ʹ������Ӧ�ͺ�
	seektrack_ctrl[1].expect = 0;			   // ����   seektrack��ѭ������˼
	seektrack_ctrl[1].feedback = error_openmv; // ���� gray_status[0]������Ԫ��״ֵ̬-11~11
	pid_control_run(&seektrack_ctrl[1]);	   // ����������
	turn_output = seektrack_ctrl[1].output;
	// ������������  ������ͼ�������?
	//	if(turn_output>0) turn_output+=steer_deadzone;//steer_deadzoneת������ ֵΪ50
	//	if(turn_output<0) turn_output-=steer_deadzone;
	// ����޷�?
	turn_output = Xianfu_float(turn_output, 50); // ת�����turn_ctrl_pwm����޷�?

	*output = 0.75f * turn_output + 0.25f * turn_output_last; // ����?ǰ�����μ���ľ��?
}
void openmv_duty_run(void)
{
	openmv_openmv_duty_run(&turn_ctrl_pwm);
	v_target_l = speed_setup + turn_ctrl_pwm; //*turn_scale;//��������ٶ�����?  �����������turn_ctrl_pwm*turn_scale
	v_target_r = speed_setup - turn_ctrl_pwm; //*turn_scale;//�ұ������ٶ�����  ����turn_ctrl_pwm*turn_scale�϶�������Ԫ���й�
	speed_control();						  // �������õ��ɼ���ʵ��������
}

void sdk_duty_run(float *a, float *b)
{
	float x1 = a[0], y1 = a[1], x2 = b[0], y2 = b[1];
	float detla_x = (x2 - x1);
	float leng = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

	// 2022��7�·�ʡ��С��������ʻϵͳ����,����Ȧ����ѭ����
	gray_turn_control_200hz(&turn_ctrl_pwm); // ���ڻҶȶԹܵ�ת����ƣ�gray_turn_control_200hz������PID���������㣩���������ֵ����turn_ctrl_pwm

	if (task_num == 4)
	{
		speed_setup = pos_auto_track(speed_adjust);
		// speed_setup = pos_auto_track(15);
	}
	else
		speed_setup = 30;

	// �����ٶ�
	v_target_l = speed_setup + turn_ctrl_pwm; //*turn_scale;//��������ٶ�����?  �����������turn_ctrl_pwm*turn_scale
	v_target_r = speed_setup - turn_ctrl_pwm; //*turn_scale;//�ұ������ٶ�����  ����turn_ctrl_pwm*turn_scale�϶�������Ԫ���й�
}

void nmotor_output(void)
{
	if (Flag.Start_Car == 1)
	{
		if (speed_output[1])
			right_pwm = speed_output[1] + 4 * (speed_output[1] > 0 ? 1 : -1); // 死区补偿
		else
			right_pwm = 0;

		if (speed_output[0])
			left_pwm = speed_output[0] + 4 * (speed_output[0] > 0 ? 1 : -1); // 死区补偿
		else
			left_pwm = 0;

		left_pwm = Xianfu_float(left_pwm, motor_max_default);
		right_pwm = Xianfu_float(right_pwm, motor_max_default);

		if (right_pwm >= 0)
		{
			Motor_Foreward_Right(right_pwm);
		}
		else
		{
			Motor_Backward_Right(ABS(right_pwm));
		}

		if (left_pwm >= 0)
		{
			Motor_Foreward_Left(left_pwm);
		}
		else
		{
			Motor_Backward_Left(ABS(left_pwm));
		}
	}
	else
	{

		AIN1_OUT(0);
		AIN2_OUT(0);

		BIN1_OUT(0);
		BIN2_OUT(0);
	}
}
#define distance 25
uint16_t gray_cnt = 0;

float tar_theta_limit = 15.0f, sleep_time = 30.f;
static uint8_t Start_duty3_3_cnt = 0, duty3_8_cnt = 0;
float yaw_angle = 0;
void TIMG0_IRQHandler(void) // 10ms
{

	if (Flag.Start_duty_1) //(1) A -> B 15s  λ�ýǶȱջ�100cm
	{
		Flag.Start_Car = 1;
		Flag.Success_duty_1 = 0;
		// auto_track(point_actual,point_B);//�Զ�ѭ��
		if (gray_state.state) // L < distance||
		{
			gray_cnt++;
			if (gray_cnt > 5 && L < distance)
			{
				v_target_l = 0;
				v_target_r = 0;
				gray_cnt = 0;
				Flag.beep_on = 1;
				//	Flag.Start_Car  = 0;
				Flag.Start_duty_1 = 0;
				Flag.Success_duty_1 = 1;
			}
			else
			{
				v_target_l = 15;
				v_target_r = 15;
			}
		}
	} /*�����ǵڶ���*/
	else if (Flag.Start_duty2_1) //(2) A -> B 30s
	{

		Flag.Start_Car = 1;
		Flag.Success_duty2_1 = 0;
		auto_track(point_actual, point_B);
		if (gray_state.state) // L < distance||
		{
			gray_cnt++;
			if (gray_cnt > 5 && L < distance)
			{
				gray_cnt = 0;
				Start_duty3_3_cnt = Num2;
				Flag.beep_on = 1;
				//	Flag.Start_Car  = 0;
				Flag.Start_duty2_1 = 0;
				Flag.Success_duty2_1 = 1;
			}
			else
			{
				v_target_l = 15;
				v_target_r = 15;
			}
		}
	}
	else if (Flag.Start_duty2_7 == 1) // ��2�� ֹͣ
	{
		// Flag.Start_Car  = 1;
		Flag.Success_duty2_7 = 0;
		v_target_l = 0;
		v_target_r = 0;
		duty3_8_cnt++;
		if (duty3_8_cnt > sleep_time) // ʶ�𵽺��� ��ͣס200ms
		{
			duty3_8_cnt = 0;
			Flag.Start_duty2_7 = 0;
			Flag.Success_duty2_7 = 1;
		}
	}
	else if (Flag.Start_duty2_2 == 1) //(2) B -> C 30s ѭ��
	{
		// speed_setup = 70;
		Flag.Start_Car = 1;
		Flag.Success_duty2_2 = 0;
		sdk_duty_run(point_actual, point_A);																																												// �õ��ٶ�Ŀ��ֵ
		if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1) && sqrt((point_C[1] - point_actual[1]) * (point_C[1] - point_actual[1]) + (point_C[0] - point_actual[0]) * (point_C[0] - point_actual[0])) < distance) //||sqrt((point_C[1] - point_actual[1]) * (point_C[1] - point_actual[1]) + (point_C[0] - point_actual[0]) * (point_C[0] - point_actual[0]))<distance
		{
			Flag.beep_on = 1;

			// Flag.Start_Car  = 0;
			Flag.Start_duty2_2 = 0;
			Flag.Success_duty2_2 = 1;
		}
	}
	else if (Flag.Start_duty2_3) //(2) ��C��У׼�Ƕ�
	{
		Flag.Start_Car = 1;
		Flag.Success_duty2_3 = 0;
		// Yaw_control(-180);
		v_target_l = 5;
		v_target_r = -5;
		if (angle.z < -178 || angle.z > 179)
		{

			Flag.Start_duty2_3 = 0;
			Flag.Success_duty2_3 = 1;
			//	Flag.Start_Car  = 0;
		}
	}
	else if (Flag.Start_duty2_4) //(2)  C -> D 30s
	{
		Flag.Start_Car = 1;

		Flag.Success_duty2_4 = 0;

		auto_track(point_actual, point_D);
		if (gray_state.state) // L < distance||
		{
			gray_cnt++;
			if (gray_cnt > 5 && L < distance)
			{
				gray_cnt = 0;
				Start_duty3_3_cnt = Num2;
				Flag.beep_on = 1;

				// Flag.Start_Car  = 0;
				v_target_l = 0;
				v_target_r = 0;
				Flag.Start_duty2_4 = 0;
				Flag.Success_duty2_4 = 1;
			}
			else
			{
				v_target_l = 15;
				v_target_r = 15;
			}
		}
	}
	else if (Flag.Start_duty2_6 == 1) // ��2�� ֹͣ
	{
		// Flag.Start_Car  = 1;
		Flag.Success_duty2_6 = 0;
		v_target_l = 0;
		v_target_r = 0;
		duty3_8_cnt++;
		if (duty3_8_cnt > sleep_time) // ʶ�𵽺��� ��ͣס200ms
		{
			duty3_8_cnt = 0;
			Flag.Start_duty2_6 = 0;
			Flag.Success_duty2_6 = 1;
		}
	}
	else if (Flag.Start_duty2_5) // ��2�� D -> A 30s
	{
		// speed_setup = 70;
		Flag.Start_Car = 1;
		Flag.Success_duty2_5 = 0;
		sdk_duty_run(point_actual, point_A);																																												// �õ��ٶ�Ŀ��ֵ
		if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1) && sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0])) < distance) //||sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0]))<distance
		{
			v_target_l = 0;
			v_target_r = 0;
			Flag.gray_worse = 0;
			Flag.beep_on = 1;

			// Flag.Start_Car  = 0;
			Flag.Start_duty2_5 = 0;
			Flag.Success_duty2_5 = 1;
		}
	} /*�����ǵ�����*/
	else if (Flag.Start_duty3_0 == 1) //(3)   Send3_Step = -1
	{
		Flag.Start_Car = 1;
		Flag.Success_duty3_0 = 0;
		v_target_l = 40;
		v_target_r = -30;

		if (ABS(angle.z) > 30)
		{
			Start_duty3_3_cnt = Num2;
			gray_cnt = 0;
			// Flag.Start_Car  = 0;
			Flag.Start_duty3_0 = 0;
			Flag.Success_duty3_0 = 1;
		}
	}
	else if (Flag.Start_duty3_1 == 1) // ��3�� A -> C 40s  Send3_Step = 0
	{
		Flag.Start_Car = 1;
		Flag.Success_duty3_1 = 0;
		auto_track(point_actual, point_C);
		//		if(L < distance || ABS(gray_status) <= 10)
		//	if(L < distance ||Flag.gray_worse == 0)
		if (gray_state.state && ((Num2 - Start_duty3_3_cnt) > 1)) // ʶ�𵽺��� ��ͣס
		{
			gray_cnt++;
			if (L < distance && gray_cnt > 2)
			{
				gray_cnt = 0;
				Flag.yaw_loss_ahead = 0;
				//			v_target_l = 0;
				//			v_target_r = 0;
				Flag.yaw_loss = 0;
				Flag.beep_on = 1;
				Start_duty3_3_cnt = Num2;
				// Flag.Start_Car  = 0;
				Flag.Start_duty3_1 = 0;
				Flag.Success_duty3_1 = 1;
			}
		}
		else if (L < 5.f)
		{
			if (Flag.yaw_loss == 0)
			{
				Flag.yaw_loss = 1;
				yaw_angle = angle.z;
			}
			if ((yaw_angle - angle.z) < 10 && (yaw_angle - angle.z) > 0)
			{
				Flag.yaw_loss_ahead = 1;
			}
			if (Flag.yaw_loss_ahead)
			{
				v_target_l = 10;
				v_target_r = 15;
			}
			else
			{
				v_target_l = -10;
				v_target_r = 10;
			}
		}
	}
	else if (Flag.Start_duty3_5 == 1) //(3)C��ѭ��ǰ  Send3_Step = 4
	{
		Flag.Start_Car = 1;
		Flag.Success_duty3_5 = 0;
		v_target_l = -10;
		v_target_r = 15;
		//		Flag.beep_on = 1;
		if (gray_state.state) //(gray_state.state <8 && gray_state.state!= 0)
		{
			gray_cnt = 0;

			//			Flag.beep_on = 1;
			Start_duty3_3_cnt = Num2;
			v_target_l = 0;
			v_target_r = 0;
			// Flag.Start_Car  = 0;
			Flag.Start_duty3_5 = 0;
			Flag.Success_duty3_5 = 1;
		}
	}
	// ����C����ͣס 500ms
	else if (Flag.Start_duty3_8 == 1) // ��3�� A -> C 40s  Send3_Step = 7
	{
		Flag.Success_duty3_8 = 0;
		v_target_l = 0;
		v_target_r = 0;
		duty3_8_cnt++;
		if (duty3_8_cnt > sleep_time) // ʶ�𵽺��� ��ͣס200ms
		{
			gray_cnt = 0;

			duty3_8_cnt = 0;
			Flag.Start_duty3_8 = 0;
			Flag.Success_duty3_8 = 1;
		}
	}
	else if (Flag.Start_duty3_2 == 1) //(3) C -> B 40s Send3_Step = 1
	{
		// speed_setup = 70;
		Flag.Start_Car = 1;
		Flag.Success_duty3_2 = 0;
		sdk_duty_run(point_actual, point_B); // �õ��ٶ�Ŀ��ֵ
		//		if( distance_inter >= 105 + distance_point)
		if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1) && (sqrt((point_B[1] - point_actual[1]) * (point_B[1] - point_actual[1]) + (point_B[0] - point_actual[0]) * (point_B[0] - point_actual[0])) < distance)) //||sqrt((point_B[1] - point_actual[1]) * (point_B[1] - point_actual[1]) + (point_B[0] - point_actual[0]) * (point_B[0] - point_actual[0]))<distance
		{
			Flag.gray_worse = 0;
			Flag.beep_on = 1;
			gray_cnt = 0;

			// Flag.Start_Car  = 0;
			Flag.Start_duty3_2 = 0;
			Flag.Success_duty3_2 = 1;
			Start_duty3_3_cnt = Num2;
		}
		//		else if(L < 7.5)
		//			{
		//				v_target_l = 5;
		//				v_target_r = -5;
		//			}
	}
	else if (Flag.Start_duty3_7 == 1) //(3)����ѭ��ǰ ������λ Send3_Step = 6
	{
		Flag.Start_Car = 1;
		Flag.Success_duty3_7 = 0;
		v_target_l = -30;
		v_target_r = 40;

		if (ABS(angle.z) < 150)
		{
			Start_duty3_3_cnt = Num2;
			gray_cnt = 0;

			// Flag.Start_Car  = 0;
			Flag.Start_duty3_7 = 0;
			Flag.Success_duty3_7 = 1;
		}
	}
	else if (Flag.Start_duty3_3 == 1) //(3) B -> D 40s   Send3_Step = 8
	{
		Flag.Start_Car = 1;
		Flag.Success_duty3_3 = 0;
		auto_track(point_actual, point_D);

		if ((Num2 - Start_duty3_3_cnt) > 1 && gray_state.state) // ��D || L <distance(gray_state.state <10 && gray_state.state!= 0))
		{
			gray_cnt++;
			if (L < distance && gray_cnt > 2)
			{
				Flag.beep_on = 1;
				gray_cnt = 0;
				Flag.yaw_loss = 0;

				Flag.Start_duty3_3 = 0;
				Flag.Success_duty3_3 = 1;
				Start_duty3_3_cnt = Num2;
			}
		}
		else if (L < 5.f)
		{
			if (Flag.yaw_loss == 0)
			{
				Flag.yaw_loss = 1;
				yaw_angle = angle.z;
			}
			float err = (yaw_angle - angle.z);
			if (err > 180)
			{
				err -= 360;
			}
			if (err < 0 && err > -10)
			{
				Flag.yaw_loss_ahead = 1;
			}
			if (Flag.yaw_loss_ahead)
			{
				v_target_l = 15;
				v_target_r = 10;
			}
			else
			{
				v_target_l = 10;
				v_target_r = -10;
			}
		}
	}
	else if (Flag.Start_duty3_6 == 1) //(3)D��ѭ��ǰ Send3_Step = 2
	{
		Flag.Start_Car = 1;
		Flag.Success_duty3_6 = 0;
		v_target_l = 15;
		v_target_r = -10;
		if (gray_state.state) // gray_state.state <8 && gray_state.state!= 0
		{
			gray_cnt = 0;

			v_target_l = 0;
			v_target_r = 0;
			Start_duty3_3_cnt = Num2;
			//			Flag.beep_on = 1;
			// Flag.Start_Car  = 0;
			Flag.Start_duty3_6 = 0;
			Flag.Success_duty3_6 = 1;
		}
	}
	else if (Flag.Start_duty3_9 == 1) // ��3�� A -> C 40s  Send3_Step = 5
	{
		Flag.Success_duty3_9 = 0;
		v_target_l = 0;
		v_target_r = 0;
		duty3_8_cnt++;
		if (duty3_8_cnt > sleep_time) // ʶ�𵽺��� ��ͣס200ms
		{
			gray_cnt = 0;

			duty3_8_cnt = 0;
			Flag.Start_duty3_9 = 0;
			Flag.Success_duty3_9 = 1;
		}
	}

	else if (Flag.Start_duty3_4 == 1) //(3) D -> A 40s Send3_Step = 3
	{
		// speed_setup = 70;
		Flag.Start_Car = 1;
		Flag.Success_duty3_4 = 0;
		sdk_duty_run(point_actual, point_A); // �õ��ٶ�Ŀ��ֵ
		//		if( distance_inter >= 105 + distance_point)
		if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1)) // &&sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0]))<distance
		{
			gray_cnt = 0;

			Flag.gray_worse = 0;
			Flag.beep_on = 1;
			v_target_l = 0;
			v_target_r = 0;
			// Flag.Start_Car  = 0;
			Flag.Start_duty3_4 = 0;
			Flag.Success_duty3_4 = 1;
		}
	}

	get_wheel_speed(); // ��ȡ�ٶ�
	speed_control();   // �ٶȻ�
	nmotor_output();   // ������

	/*�ٶȻ�*/
	//	get_wheel_speed();
	//	speed_control();
	//	nmotor_output();

	/*�ٶ�λ�ô���*/
	//	get_wheel_speed();
	//	position_control();  //�õ��ٶ�Ŀ��ֵ
	//	speed_control();     //�ٶȻ�
	//	nmotor_output();

	/*�ǶȻ�*/
	//	Yaw_control(yaw_target);
	//	get_wheel_speed();
	//	speed_control();
	//	nmotor_output();

	/*�Ҷ�Ѳ��С��*/
	//	  get_wheel_speed();
	//		sdk_duty_run();
	//		nmotor_output();

	/*����ͷѭ��С��*/
	//	  get_wheel_speed();
	//		//sdk_duty_run();
	//		openmv_duty_run();
	//		nmotor_output();
}
