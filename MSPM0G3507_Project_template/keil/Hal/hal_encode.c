
#include "ti/driverlib/dl_gpio.h"
#include <ti/driverlib/m0p/dl_interrupt.h>
#include "hal_encode.h"
#include "imu_filter.h" // Device header
#include "math.h"
#include "hal_jy62.h"
int32_t enc_cnt[2];
encoder NEncoder;
sensor smartcar_imu;
move_filter_struct left_speed_cmps, right_speed_cmps;
float wheel_radius_cm = 3.35f;

void hal_Encoder_Init(void)
{
	NVIC_EnableIRQ(GPIOA_INT_IRQn); // Enable external interrupt GPIOA
	NVIC_EnableIRQ(GPIOB_INT_IRQn); // Enable external interrupt GPIOB
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
			enc_cnt[1] += (a_state == b_state) ? 1 : -1;
		if (mis & (1 << 6))
			enc_cnt[1] += (b_state == a_state) ? -1 : 1;

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
			enc_cnt[0] += (a_state == b_state) ? 1 : -1;
		if (mis & (1 << 30))
			enc_cnt[0] += (b_state == a_state) ? -1 : 1;

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
	// Convert pulse count to RPM
	// NEncoder.left_motor_speed_rpm = 60.0f * (NEncoder.left_motor_cnt / pulse_num_per_circle) / (left_motor_period_ms * 0.001f);
	// Convert RPM to cm/s
	NEncoder.left_motor_speed_cmps = convert_current_rpm_to_cms(pulse_num_per_circle, NEncoder.left_motor_cnt, motor_countint_phases, motor_reduction_ratio, wheel_radius_cm * 2);
	move_filter_calc(&left_speed_cmps, NEncoder.left_motor_speed_cmps);

	enc_cnt[1] = 0;

	// return NEncoder.left_motor_speed_cmps;
	// return NEncoder.left_motor_cnt;
	return left_speed_cmps.data_average;
}
/***************************************
Function:   float get_right_motor_speed(void)
Description: Get actual speed of right wheel
Input:      None
Output:     None
Note:       Count pulses per unit time, convert to rpm and cm/s
Author:     FJTI
***************************************/
static float get_right_motor_speed(void)
{
	NEncoder.right_motor_cnt = enc_cnt[0];

	// Convert pulse count to RPM
	NEncoder.right_motor_speed_rpm = 60.0f * (float)NEncoder.right_motor_cnt / (right_motor_period_ms * 0.001f * pulse_num_per_circle);
	// Convert RPM to cm/s
	NEncoder.right_motor_speed_cmps = convert_current_rpm_to_cms(pulse_num_per_circle, NEncoder.right_motor_cnt, motor_countint_phases, motor_reduction_ratio, wheel_radius_cm * 2);
	move_filter_calc(&right_speed_cmps, NEncoder.right_motor_speed_cmps);
	enc_cnt[0] = 0;
	// return NEncoder.right_motor_speed_cmps;
	// return NEncoder.right_motor_cnt;
	return right_speed_cmps.data_average;
}
float distance_inter = 0, distance_l = 0, distance_r = 0, distance_point = 0;
float distance_x = 0, distance_y = 0;
float point_A[2], point_B[2], point_C[2], point_D[2], point_actual[2];

float actual_position_l = 0, actual_position_r = 0;
void get_wheel_speed(void)
{
	smartcar_imu.left_motor_speed_cmps = get_left_motor_speed();
	smartcar_imu.right_motor_speed_cmps = get_right_motor_speed();

	// Two-wheel average speed
	smartcar_imu.state_estimation.speed = 0.5f * (smartcar_imu.left_motor_speed_cmps + smartcar_imu.right_motor_speed_cmps);

	point_actual[0] += smartcar_imu.state_estimation.speed * cos(pi / 180 * imu.yaw) * 0.01f;
	point_actual[1] += smartcar_imu.state_estimation.speed * sin(pi / 180 * imu.yaw) * 0.01f;

	// Average speed integrates directly to total distance
	smartcar_imu.state_estimation.distance += (int)(smartcar_imu.state_estimation.speed * 5); // Speed * 5ms period
																							  // distance_inter = smartcar_imu.state_estimation.distance/1000;

	actual_position_l += (int)(smartcar_imu.left_motor_speed_cmps * 10);  // 10ms sampling period
	actual_position_r += (int)(smartcar_imu.right_motor_speed_cmps * 10); // 10ms sampling period
	distance_l = actual_position_l / 1000;								  // Use intermediate variable for distance_l
	distance_r = actual_position_r / 1000;

	distance_inter = (distance_l + distance_r) * 0.5;
}
/*
Set the target speed as cm/s with the unit system in rpm
*/
float convert_current_rpm_to_cms(int Encoder_lines, int Encoder_counts, int Phases, int Reduction_ratio, float wheel_radius_cms)
{
	float Pulses_per_round = Encoder_lines * Phases * Reduction_ratio; // Total pulses per wheel revolution
	float wheel_circumference_cm = 3.14159f * wheel_radius_cms;

	// Convert pulse counts collected in one sampling period directly to cm/s.
	// wheel_radius_cms is passed in as diameter here (radius * 2), so pi * wheel_radius_cms = circumference.
	return (1000.0f * (float)Encoder_counts * wheel_circumference_cm) / (right_motor_period_ms * Pulses_per_round);
}
