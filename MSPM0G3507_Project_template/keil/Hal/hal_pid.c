#include "hal_pid.h"
#include "hal_math.h"


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
											uint8_t dis_error_gap_cnt)
{
	ctrl->kp=kp;
	ctrl->ki=ki;
	ctrl->kd=kd;
	ctrl->error_limit_max=err_max;
	ctrl->integral_limit_max=integral_max;
	ctrl->output_limit_max=output_max;
	ctrl->error_limit_flag=err_limit_flag;
	ctrl->integral_separate_flag =integral_separate_flag;
	ctrl->integral_separate_limit=integral_separate_limit;
	
	ctrl->dis_error_gap_cnt=dis_error_gap_cnt;
	
	ctrl->init_flag=1;
}

/***************************************************
Function:   float pid_control_run(controller *ctrl)
Description: PID controller calculation
Input:      controller *ctrl - control structure pointer
Output:     None
Note:       None
****************************************************/
float pid_control_run(controller *ctrl)
{
  /******* Error calculation *********************/
	for(uint16_t i=19;i>0;i--) // Shift history buffer for derivative calc
	{
		ctrl->error_backup[i]=ctrl->error_backup[i-1];
	}
	ctrl->error_backup[0]=ctrl->error;
	
  ctrl->last_error=ctrl->error;// Save previous error
  ctrl->error= ctrl->expect - ctrl->feedback;// Target minus feedback = error
	ctrl->dis_error=ctrl->error-ctrl->error_backup[ctrl->dis_error_gap_cnt-1];// Derivative error        ctrl->dis_error=ctrl->error-ctrl->last_error;// Original derivative
	
  if(ctrl->error_limit_flag==1)// Error limit flag
  {
    if(ctrl->error>= ctrl->error_limit_max)  ctrl->error= ctrl->error_limit_max;
    if(ctrl->error<=-ctrl->error_limit_max)  ctrl->error=-ctrl->error_limit_max;
  }
  /******* Integral calculation ******************/
	  if(ctrl->integral_separate_flag==1)// Integral separation flag
  {
		// Only accumulate integral when error is small
    if(ABS(ctrl->error)<=ctrl->integral_separate_limit)	 ctrl->integral+=ctrl->ki*ctrl->error;
  }
  else
  {
    ctrl->integral+=ctrl->ki*ctrl->error;
  }
	/******* Integral limit ************************/
	if(ctrl->integral>=ctrl->integral_limit_max)   ctrl->integral=ctrl->integral_limit_max;
	if(ctrl->integral<=-ctrl->integral_limit_max)  ctrl->integral=-ctrl->integral_limit_max;
	
	/******* Output calculation ********************/
	  ctrl->last_output=ctrl->output;// Save previous output
  ctrl->output=ctrl->kp*ctrl->error// Proportional
								+ctrl->integral// Integral
							+ctrl->kd*ctrl->dis_error;// Derivative
	
	/******* Output limit **************************/
  if(ctrl->output>= ctrl->output_limit_max)  ctrl->output = ctrl->output_limit_max;
  if(ctrl->output<=-ctrl->output_limit_max)  ctrl->output = -ctrl->output_limit_max;
  /******* Return output *************************/
  return ctrl->output;
}

