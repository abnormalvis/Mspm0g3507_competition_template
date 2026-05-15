#ifndef __PID_H
#define __PID_H
#include "stdint.h"

typedef struct{
	float kp;					//Proportional gain
	float ki;					//Integral gain
	float	kd;					//Derivative gain
	float error;			//Error value
	float	expect;			//Expected value
	float	feedback;		//Feedback value
	float	last_error;	//Last error
	float dis_error;	//Derivative term
	float	integral;		//Integral controller output
	float output;			//Current controller output
	float last_output;//Last controller output
	float error_backup[20];			    //Historical error values
	uint8_t dis_error_gap_cnt;
  uint8_t error_limit_flag;				//Error limit flag
	float error_limit_max;					//Max error limit value
	uint8_t integral_separate_flag;	//Integral separation flag
	float integral_separate_limit;	//Integral separation error threshold
	float integral_limit_max;				//Integral limit max value
	float output_limit_max;             //Controller total output limit value
	uint8_t init_flag;
	float dis_error_lpf;
//	lpf_param lpf_params;
//	lpf_buf lpf_buffer;
//	systime _time;
}controller;

typedef struct {
    float p;
    float i;
    float d;
}PID;

float pid_control_run(controller *ctrl);
void pid_control_init(controller *ctrl,
											float kp,
											float ki,
											float kd,
											float err_max,
											float integral_max,
											float output_max,
											uint8_t err_limit_flag,
											uint8_t integral_separate_flag,
											float integral_separate_limit,
											uint8_t dis_error_gap_cnt);

#endif

