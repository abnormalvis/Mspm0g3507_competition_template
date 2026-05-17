#ifndef _MT_FLAG_H
#define _MT_FLAG_H

#include "stdint.h"

typedef struct // Struct for storing parameters added by the debug host computer
{
	uint8_t Send1_Step; // Duty 1 step
	uint8_t Send2_Step; // Duty 2 step
	int8_t Send3_Step;  // Duty 3 step
	uint8_t Send4_Step; // Duty 4 step
} Param_InitTypeDef;

typedef struct // Flag struct for PID, angle, grayscale, loop, key, encoder, position, start, end, obstacle, 3 path state machines
{
	uint8_t Start_Car; // Start flag

	uint8_t Stop_Car;

	uint8_t Start_duty_1;
	uint8_t Success_duty_1;

	uint8_t Start_duty2_1;
	uint8_t Success_duty2_1;

	uint8_t Start_duty2_2;
	uint8_t Success_duty2_2;

	uint8_t Start_duty2_3;
	uint8_t Success_duty2_3;

	uint8_t Start_duty2_4;
	uint8_t Success_duty2_4;

	uint8_t Start_duty2_5;
	uint8_t Success_duty2_5;

	uint8_t Start_duty2_6;
	uint8_t Success_duty2_6;

	uint8_t Start_duty2_7;
	uint8_t Success_duty2_7;

	uint8_t Start_duty3_0;
	uint8_t Success_duty3_0;

	uint8_t Start_duty3_1;
	uint8_t Success_duty3_1;

	uint8_t Start_duty3_2;
	uint8_t Success_duty3_2;

	uint8_t Start_duty3_3;
	uint8_t Success_duty3_3;

	uint8_t Start_duty3_7;
	uint8_t Success_duty3_7;

	uint8_t Start_duty3_4;
	uint8_t Success_duty3_4;

	uint8_t Start_duty3_5;
	uint8_t Success_duty3_5;

	uint8_t Start_duty3_6;
	uint8_t Success_duty3_6;

	uint8_t Start_duty3_8;
	uint8_t Success_duty3_8;

	uint8_t Start_duty3_9;
	uint8_t Success_duty3_9;
	uint8_t gray_worse;

	uint8_t yaw_loss;
	uint8_t yaw_loss_ahead;

	uint8_t beep_on;
	uint8_t task_start;
	uint8_t angle_loss;
	uint8_t pid_tuning;  /* PID tuning test mode - bypasses duty state machine */
} Flag_InitTypeDef;

extern Param_InitTypeDef Param; // Parameter struct for duty tasks
extern Flag_InitTypeDef Flag;	// Flag struct for duty tasks
void Flag_Init(void);
#endif
