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
#include "hal_motor.h"
#include "move_filter.h"

float position_kp = 6.0;
float position_ki = 0;
float position_kd = 0.3;

float yaw_kp = 0.6;
float yaw_ki = 0;
float yaw_kd = 0.5;

static float position_error[2] = {0, 0}, position_feedback[2] = {0, 0}, position_output[2] = {0, 0}, yaw_out[2] = {0, 0};
static float yaw_feedback = 0, yaw_error = 0, yaw = 0, target_yaw = 0;

float v_target_l = 0, v_target_r = 0;
float p_target_l = 100, p_target_r = 100;
float left_pwm, right_pwm;

float turn_output = 0, turn_output_last = 0;
pid_t seektrack_pid[2];
float turn_ctrl_pwm = 0;
float turn_scale = 0.06;
float speed_setup = 70, speed_adjust = 35;
float yaw_target = 0;

void position_control(void) // Cascaded speed-position PID
{
	static float err_l, err_last_l, err_r, err_last_r, err_sum_l, err_sum_r;

	position_feedback[0] = distance_l;
	err_l = position_error[0] = p_target_l - position_feedback[0];

	if (ABS(err_l) < 0.8)
	{
		position_output[0] = 0;
	}
		else // Position loop PID
	{
		err_sum_l += err_l;
			err_sum_l = Xianfu_float(err_sum_l, 60); // Limit integral
			position_output[0] = position_kp * err_l + position_ki * err_sum_l + position_kd * (err_l - err_last_l); // Position loop PID output
		if (ABS(err_r) < 4 && ABS(err_r) > 0.8)
		{
			position_output[0] = (position_output[0] / ABS(position_output[0])) * 10 + position_output[0];
		}
		err_last_l = err_l;

			position_output[0] = Xianfu_float(position_output[0], 30); // Limit position loop output (max target speed)
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
			err_sum_r = Xianfu_float(err_sum_r, 60); // Limit integral
			position_output[1] = position_kp * err_r + position_ki * err_sum_r + position_kd * (err_r - err_last_r); // Position loop PID output
		if (ABS(err_r) < 4 && ABS(err_r) > 0.8)
		{
			position_output[1] = (position_output[1] / ABS(position_output[1])) * 10 + position_output[1];
		}
		err_last_r = err_r;
			position_output[1] = Xianfu_float(position_output[1], 30); // Limit position loop output
	}
	v_target_r = position_output[1];
}

	// Yaw/angle loop using JY62 IMU
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

void pid_params_init(void)
{
    Pid_Init(&seektrack_pid[0], POSITION_PID, turn_kp_default1, turn_ki_default1, turn_kd_default1);
    Pid_Init(&motorL, POSITION_PID, 1.0f, 0.0f, 0.0f);
    Pid_Init(&motorR, POSITION_PID, 1.0f, 0.0f, 0.0f);
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

	seektrack_pid[0].p = kp;
	seektrack_pid[0].i = 0;
	seektrack_pid[0].d = kd;
	turn_output_last = turn_output;
	seektrack_pid[0].target = 0;
	seektrack_pid[0].now = -gray_status;
	Pid_Cal(&seektrack_pid[0]);
	turn_output = seektrack_pid[0].out;

		//	if(turn_output>0) turn_output+=steer_deadzone; // Dead zone compensation, value=50
	//	if(turn_output<0) turn_output-=steer_deadzone;
	turn_output = Xianfu_float(turn_output, 70);

	*output = 0.75f * turn_output + 0.25f * turn_output_last;
}
	float cam_turn_kp = 0.f;  // Camera position tracking proportional gain
	float cam_turn_ki = 0.0f; // Integral only active at forks, for camera seektrack position loop KI
float cam_turn_kd = 0.0f; //
void openmv_openmv_duty_run(float *output)
{
	seektrack_pid[1].p = cam_turn_kp;
	seektrack_pid[1].i = cam_turn_ki;
	seektrack_pid[1].d = cam_turn_kd;
	turn_output_last = turn_output;
	seektrack_pid[1].target = 0;
	seektrack_pid[1].now = error_openmv;
	Pid_Cal(&seektrack_pid[1]);
	turn_output = seektrack_pid[1].out;
		// Turn dead zone compensation to reduce jitter
	//	if(turn_output>0) turn_output+=steer_deadzone;//steer_deadzone dead zone = 50
	//	if(turn_output<0) turn_output-=steer_deadzone;
		// Output limit
		turn_output = Xianfu_float(turn_output, 50); // Limit turn control PWM

		*output = 0.75f * turn_output + 0.25f * turn_output_last; // Weighted blend of current and previous output
}
void openmv_duty_run(void)
{
	openmv_openmv_duty_run(&turn_ctrl_pwm);
		v_target_l = speed_setup + turn_ctrl_pwm;
		v_target_r = speed_setup - turn_ctrl_pwm;
		motorL.target = v_target_l;
		motorR.target = v_target_r;
		Pid_Motor_Control();
}

void sdk_duty_run(float *a, float *b)
{
	float x1 = a[0], y1 = a[1], x2 = b[0], y2 = b[1];
	float detla_x = (x2 - x1);
	float leng = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

		// 2022 July provincial competition driving system, two seektrack switches per lap
		gray_turn_control_200hz(&turn_ctrl_pwm); // Gray sensor turn control (includes PID calculation), result in turn_ctrl_pwm

	if (task_num == 4)
	{
		speed_setup = pos_auto_track(speed_adjust);
		// speed_setup = pos_auto_track(15);
	}
	else
		speed_setup = 30;

		// Set speed
		v_target_l = speed_setup + turn_ctrl_pwm; //*turn_scale; // Left wheel target speed with turn adjustment
		v_target_r = speed_setup - turn_ctrl_pwm; //*turn_scale; // Right wheel target speed with turn adjustment
}

void nmotor_output(void)
{
	
}
#define distance 25
uint16_t gray_cnt = 0;

float tar_theta_limit = 15.0f, sleep_time = 30.f;
static uint8_t Start_duty3_3_cnt = 0, duty3_8_cnt = 0;
float yaw_angle = 0;
void TIMG0_IRQHandler(void) // 10ms
{

	// 	if (Flag.Start_duty_1) //(1) A -> B 15s, position-angle closed loop 100cm
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty_1 = 0;
	// 		// auto_track(point_actual,point_B); // Auto tracking
	// 	if (gray_state.state) // L < distance||
	// 	{
	// 		gray_cnt++;
	// 		if (gray_cnt > 5 && L < distance)
	// 		{
	// 			v_target_l = 0;
	// 			v_target_r = 0;
	// 			gray_cnt = 0;
	// 			Flag.beep_on = 1;
	// 			//	Flag.Start_Car  = 0;
	// 			Flag.Start_duty_1 = 0;
	// 			Flag.Success_duty_1 = 1;
	// 		}
	// 		else
	// 		{
	// 			v_target_l = 15;
	// 			v_target_r = 15;
	// 		}
	// 	}
	// 	} /* End of stage 1 */
	// else if (Flag.Start_duty2_1) //(2) A -> B 30s
	// {

	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty2_1 = 0;
	// 	auto_track(point_actual, point_B);
	// 	if (gray_state.state) // L < distance||
	// 	{
	// 		gray_cnt++;
	// 		if (gray_cnt > 5 && L < distance)
	// 		{
	// 			gray_cnt = 0;
	// 			Start_duty3_3_cnt = Num2;
	// 			Flag.beep_on = 1;
	// 			//	Flag.Start_Car  = 0;
	// 			Flag.Start_duty2_1 = 0;
	// 			Flag.Success_duty2_1 = 1;
	// 		}
	// 		else
	// 		{
	// 			v_target_l = 15;
	// 			v_target_r = 15;
	// 		}
	// 	}
	// }
	// 	else if (Flag.Start_duty2_7 == 1) // Stage 2: Stop
	// {
	// 	// Flag.Start_Car  = 1;
	// 	Flag.Success_duty2_7 = 0;
	// 	v_target_l = 0;
	// 	v_target_r = 0;
	// 	duty3_8_cnt++;
	// 		if (duty3_8_cnt > sleep_time) // Detected horizontal line, stop for 200ms
	// 	{
	// 		duty3_8_cnt = 0;
	// 		Flag.Start_duty2_7 = 0;
	// 		Flag.Success_duty2_7 = 1;
	// 	}
	// }
	// 	else if (Flag.Start_duty2_2 == 1) //(2) B -> C 30s tracking
	// {
	// 	// speed_setup = 70;
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty2_2 = 0;
	// 		sdk_duty_run(point_actual, point_A); // Get speed target values
	// 	if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1) && sqrt((point_C[1] - point_actual[1]) * (point_C[1] - point_actual[1]) + (point_C[0] - point_actual[0]) * (point_C[0] - point_actual[0])) < distance) //||sqrt((point_C[1] - point_actual[1]) * (point_C[1] - point_actual[1]) + (point_C[0] - point_actual[0]) * (point_C[0] - point_actual[0]))<distance
	// 	{
	// 		Flag.beep_on = 1;

	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty2_2 = 0;
	// 		Flag.Success_duty2_2 = 1;
	// 	}
	// }
	// 	else if (Flag.Start_duty2_3) //(2) Calibrate angle at point C
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty2_3 = 0;
	// 	// Yaw_control(-180);
	// 	v_target_l = 5;
	// 	v_target_r = -5;
	// 	if (angle.z < -178 || angle.z > 179)
	// 	{

	// 		Flag.Start_duty2_3 = 0;
	// 		Flag.Success_duty2_3 = 1;
	// 		//	Flag.Start_Car  = 0;
	// 	}
	// }
	// else if (Flag.Start_duty2_4) //(2)  C -> D 30s
	// {
	// 	Flag.Start_Car = 1;

	// 	Flag.Success_duty2_4 = 0;

	// 	auto_track(point_actual, point_D);
	// 	if (gray_state.state) // L < distance||
	// 	{
	// 		gray_cnt++;
	// 		if (gray_cnt > 5 && L < distance)
	// 		{
	// 			gray_cnt = 0;
	// 			Start_duty3_3_cnt = Num2;
	// 			Flag.beep_on = 1;

	// 			// Flag.Start_Car  = 0;
	// 			v_target_l = 0;
	// 			v_target_r = 0;
	// 			Flag.Start_duty2_4 = 0;
	// 			Flag.Success_duty2_4 = 1;
	// 		}
	// 		else
	// 		{
	// 			v_target_l = 15;
	// 			v_target_r = 15;
	// 		}
	// 	}
	// }
	// 	else if (Flag.Start_duty2_6 == 1) // Stage 2: Stop
	// {
	// 	// Flag.Start_Car  = 1;
	// 	Flag.Success_duty2_6 = 0;
	// 	v_target_l = 0;
	// 	v_target_r = 0;
	// 	duty3_8_cnt++;
	// 		if (duty3_8_cnt > sleep_time) // Detected horizontal line, stop for 200ms
	// 	{
	// 		duty3_8_cnt = 0;
	// 		Flag.Start_duty2_6 = 0;
	// 		Flag.Success_duty2_6 = 1;
	// 	}
	// }
	// 	else if (Flag.Start_duty2_5) // Stage 2: D -> A 30s
	// {
	// 	// speed_setup = 70;
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty2_5 = 0;
	// 		sdk_duty_run(point_actual, point_A); // Get speed target values
	// 	if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1) && sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0])) < distance) //||sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0]))<distance
	// 	{
	// 		v_target_l = 0;
	// 		v_target_r = 0;
	// 		Flag.gray_worse = 0;
	// 		Flag.beep_on = 1;

	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty2_5 = 0;
	// 		Flag.Success_duty2_5 = 1;
	// 	}
	// 	} /* End of stage 2 */
	// 	else if (Flag.Start_duty3_0 == 1) //(3) Send3_Step = -1
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_0 = 0;
	// 	v_target_l = 40;
	// 	v_target_r = -30;

	// 	if (ABS(angle.z) > 30)
	// 	{
	// 		Start_duty3_3_cnt = Num2;
	// 		gray_cnt = 0;
	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty3_0 = 0;
	// 		Flag.Success_duty3_0 = 1;
	// 	}
	// }
	// 	else if (Flag.Start_duty3_1 == 1) // Stage 3: A -> C 40s, Send3_Step = 0
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_1 = 0;
	// 	auto_track(point_actual, point_C);
	// 	//		if(L < distance || ABS(gray_status) <= 10)
	// 	//	if(L < distance ||Flag.gray_worse == 0)
	// 		if (gray_state.state && ((Num2 - Start_duty3_3_cnt) > 1)) // Detected horizontal line, stop
	// 	{
	// 		gray_cnt++;
	// 		if (L < distance && gray_cnt > 2)
	// 		{
	// 			gray_cnt = 0;
	// 			Flag.yaw_loss_ahead = 0;
	// 			//			v_target_l = 0;
	// 			//			v_target_r = 0;
	// 			Flag.yaw_loss = 0;
	// 			Flag.beep_on = 1;
	// 			Start_duty3_3_cnt = Num2;
	// 			// Flag.Start_Car  = 0;
	// 			Flag.Start_duty3_1 = 0;
	// 			Flag.Success_duty3_1 = 1;
	// 		}
	// 	}
	// 	else if (L < 5.f)
	// 	{
	// 		if (Flag.yaw_loss == 0)
	// 		{
	// 			Flag.yaw_loss = 1;
	// 			yaw_angle = angle.z;
	// 		}
	// 		if ((yaw_angle - angle.z) < 10 && (yaw_angle - angle.z) > 0)
	// 		{
	// 			Flag.yaw_loss_ahead = 1;
	// 		}
	// 		if (Flag.yaw_loss_ahead)
	// 		{
	// 			v_target_l = 10;
	// 			v_target_r = 15;
	// 		}
	// 		else
	// 		{
	// 			v_target_l = -10;
	// 			v_target_r = 10;
	// 		}
	// 	}
	// }
	// 	else if (Flag.Start_duty3_5 == 1) //(3) Before tracking at point C, Send3_Step = 4
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_5 = 0;
	// 	v_target_l = -10;
	// 	v_target_r = 15;
	// 	//		Flag.beep_on = 1;
	// 	if (gray_state.state) //(gray_state.state <8 && gray_state.state!= 0)
	// 	{
	// 		gray_cnt = 0;

	// 		//			Flag.beep_on = 1;
	// 		Start_duty3_3_cnt = Num2;
	// 		v_target_l = 0;
	// 		v_target_r = 0;
	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty3_5 = 0;
	// 		Flag.Success_duty3_5 = 1;
	// 	}
	// }
	// 	// Stop at point C for 500ms
	// 	else if (Flag.Start_duty3_8 == 1) // Stage 3: A -> C 40s, Send3_Step = 7
	// {
	// 	Flag.Success_duty3_8 = 0;
	// 	v_target_l = 0;
	// 	v_target_r = 0;
	// 	duty3_8_cnt++;
	// 		if (duty3_8_cnt > sleep_time) // Detected horizontal line, stop for 200ms
	// 	{
	// 		gray_cnt = 0;

	// 		duty3_8_cnt = 0;
	// 		Flag.Start_duty3_8 = 0;
	// 		Flag.Success_duty3_8 = 1;
	// 	}
	// }
	// else if (Flag.Start_duty3_2 == 1) //(3) C -> B 40s Send3_Step = 1
	// {
	// 	// speed_setup = 70;
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_2 = 0;
	// 		sdk_duty_run(point_actual, point_B); // Get speed target values
	// 	//		if( distance_inter >= 105 + distance_point)
	// 	if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1) && (sqrt((point_B[1] - point_actual[1]) * (point_B[1] - point_actual[1]) + (point_B[0] - point_actual[0]) * (point_B[0] - point_actual[0])) < distance)) //||sqrt((point_B[1] - point_actual[1]) * (point_B[1] - point_actual[1]) + (point_B[0] - point_actual[0]) * (point_B[0] - point_actual[0]))<distance
	// 	{
	// 		Flag.gray_worse = 0;
	// 		Flag.beep_on = 1;
	// 		gray_cnt = 0;

	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty3_2 = 0;
	// 		Flag.Success_duty3_2 = 1;
	// 		Start_duty3_3_cnt = Num2;
	// 	}
	// 	//		else if(L < 7.5)
	// 	//			{
	// 	//				v_target_l = 5;
	// 	//				v_target_r = -5;
	// 	//			}
	// }
	// 	else if (Flag.Start_duty3_7 == 1) //(3) Before right-angle tracking, align chassis, Send3_Step = 6
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_7 = 0;
	// 	v_target_l = -30;
	// 	v_target_r = 40;

	// 	if (ABS(angle.z) < 150)
	// 	{
	// 		Start_duty3_3_cnt = Num2;
	// 		gray_cnt = 0;

	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty3_7 = 0;
	// 		Flag.Success_duty3_7 = 1;
	// 	}
	// }
	// else if (Flag.Start_duty3_3 == 1) //(3) B -> D 40s   Send3_Step = 8
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_3 = 0;
	// 	auto_track(point_actual, point_D);

	// 		if ((Num2 - Start_duty3_3_cnt) > 1 && gray_state.state) // Detected point D or horizontal line
	// 	{
	// 		gray_cnt++;
	// 		if (L < distance && gray_cnt > 2)
	// 		{
	// 			Flag.beep_on = 1;
	// 			gray_cnt = 0;
	// 			Flag.yaw_loss = 0;

	// 			Flag.Start_duty3_3 = 0;
	// 			Flag.Success_duty3_3 = 1;
	// 			Start_duty3_3_cnt = Num2;
	// 		}
	// 	}
	// 	else if (L < 5.f)
	// 	{
	// 		if (Flag.yaw_loss == 0)
	// 		{
	// 			Flag.yaw_loss = 1;
	// 			yaw_angle = angle.z;
	// 		}
	// 		float err = (yaw_angle - angle.z);
	// 		if (err > 180)
	// 		{
	// 			err -= 360;
	// 		}
	// 		if (err < 0 && err > -10)
	// 		{
	// 			Flag.yaw_loss_ahead = 1;
	// 		}
	// 		if (Flag.yaw_loss_ahead)
	// 		{
	// 			v_target_l = 15;
	// 			v_target_r = 10;
	// 		}
	// 		else
	// 		{
	// 			v_target_l = 10;
	// 			v_target_r = -10;
	// 		}
	// 	}
	// }
	// 	else if (Flag.Start_duty3_6 == 1) //(3) Before tracking at point D, Send3_Step = 2
	// {
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_6 = 0;
	// 	v_target_l = 15;
	// 	v_target_r = -10;
	// 	if (gray_state.state) // gray_state.state <8 && gray_state.state!= 0
	// 	{
	// 		gray_cnt = 0;

	// 		v_target_l = 0;
	// 		v_target_r = 0;
	// 		Start_duty3_3_cnt = Num2;
	// 		//			Flag.beep_on = 1;
	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty3_6 = 0;
	// 		Flag.Success_duty3_6 = 1;
	// 	}
	// }
	// 	else if (Flag.Start_duty3_9 == 1) // Stage 3: A -> C 40s, Send3_Step = 5
	// {
	// 	Flag.Success_duty3_9 = 0;
	// 	v_target_l = 0;
	// 	v_target_r = 0;
	// 	duty3_8_cnt++;
	// 		if (duty3_8_cnt > sleep_time) // Detected horizontal line, stop for 200ms
	// 	{
	// 		gray_cnt = 0;

	// 		duty3_8_cnt = 0;
	// 		Flag.Start_duty3_9 = 0;
	// 		Flag.Success_duty3_9 = 1;
	// 	}
	// }

	// else if (Flag.Start_duty3_4 == 1) //(3) D -> A 40s Send3_Step = 3
	// {
	// 	// speed_setup = 70;
	// 	Flag.Start_Car = 1;
	// 	Flag.Success_duty3_4 = 0;
	// 		sdk_duty_run(point_actual, point_A); // Get speed target values
	// 	//		if( distance_inter >= 105 + distance_point)
	// 	if (((Num2 - Start_duty3_3_cnt) > 1 && Flag.gray_worse == 1)) // &&sqrt((point_A[1] - point_actual[1]) * (point_A[1] - point_actual[1]) + (point_A[0] - point_actual[0]) * (point_A[0] - point_actual[0]))<distance
	// 	{
	// 		gray_cnt = 0;

	// 		Flag.gray_worse = 0;
	// 		Flag.beep_on = 1;
	// 		v_target_l = 0;
	// 		v_target_r = 0;
	// 		// Flag.Start_Car  = 0;
	// 		Flag.Start_duty3_4 = 0;
	// 		Flag.Success_duty3_4 = 1;
	// 	}
	// }

	// /* PID tuning test mode - bypasses duty state machine */
	// if (Flag.pid_tuning)
	// {
	// 	/* Direct PWM output, bypassing PID, for hardware debug */
	// 	Set_Pwm_Debug(2000, 2000);  /* 200 = ~50% duty at 10kHz, adjust as needed */
	// }

	// get_wheel_speed(); // Get wheel speed
	// speed_control();   // Speed loop
	// nmotor_output();   // Motor output

	// 	/* Speed loop */
	// 	get_wheel_speed(); // Get wheel speed
	// 	//	speed_control();     // Speed loop
	// //	nmotor_output();

	// 	/* Cascaded speed-position */
	// 	get_wheel_speed(); // Get wheel speed
	// 	//	position_control();  // Get speed target values
	// 	//	speed_control();     // Speed loop
	// //	nmotor_output();

	// 	/* Yaw/angle loop */
	// //	Yaw_control(yaw_target);
	// 	get_wheel_speed(); // Get wheel speed
	// 	//	speed_control();     // Speed loop
	// //	nmotor_output();

	// 	/* Gray sensor tracking car */
	// 	get_wheel_speed(); // Get wheel speed
	// //		sdk_duty_run();
	// //		nmotor_output();

	// 	/* Camera tracking car */
	// 	get_wheel_speed(); // Get wheel speed
	// //		//sdk_duty_run();
	// //		openmv_duty_run();
	// //		nmotor_output();
}
