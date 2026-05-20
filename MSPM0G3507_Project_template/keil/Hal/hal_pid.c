#include "hal_pid.h"

pid_t turn_pid;
pid_t track_pid;
pid_t motorL;
pid_t motorR;

float Float_Abs(float value)
{
    if (value < 0)
        return -value;
    else
        return value;
}

/* Yaw error with -180~180 wrapping, shortest path */
float Yaw_Error_Cal(float Target, float Now)
{
    static float error;
    if (Target >= 0)
    {
        if (Now < 0)
        {
            if (Float_Abs(Now) < (180 - Target))
                error = -(Float_Abs(Now) + Target);
            else
                error = (180 - Target) + (180 - Float_Abs(Now));
        }
        else
        {
            error = Now - Target;
        }
    }
    else
    {
        if (Now >= 0)
        {
            if (Now >= (Target + 180))
                error = -((180 - Now) + (180 - Float_Abs(Target)));
            else
                error = (Float_Abs(Target) + Now);
        }
        else
        {
            error = Float_Abs(Target) - Float_Abs(Now);
        }
    }
    return error;
}

void Pid_Turn_Cal(pid_t *pid)
{
    pid->error[0] = Yaw_Error_Cal(pid->target, pid->now);

    if (pid->pid_mode == DELTA_PID)
    {
        pid->pout = pid->p * (pid->error[0] - pid->error[1]);
        pid->iout = pid->i * pid->error[0];
        pid->dout = pid->d * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
        pid->out += pid->pout + pid->iout + pid->dout;
    }
    else if (pid->pid_mode == POSITION_PID)
    {
        pid->pout = pid->p * pid->error[0];
        pid->iout += pid->i * pid->error[0];

        /* D-term noise filter: skip D when error change is small */
        float d_error = pid->error[0] - pid->error[1];
        if (Float_Abs(d_error) < 0.5f)
            pid->dout = 0;
        else
            pid->dout = pid->d * d_error;

        pid->out = pid->pout + pid->iout + pid->dout;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];

    if (pid->out >= MAX_Speed)  pid->out = MAX_Speed;
    if (pid->out <= MIX_Speed)  pid->out = MIX_Speed;
}

void Pid_Cal(pid_t *pid)
{
    pid->error[0] = pid->target - pid->now;

    if (pid->pid_mode == DELTA_PID)
    {
        pid->pout = pid->p * (pid->error[0] - pid->error[1]);
        pid->iout = pid->i * pid->error[0];
        pid->dout = pid->d * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
        pid->out += pid->pout + pid->iout + pid->dout;
    }
    else if (pid->pid_mode == POSITION_PID)
    {
        pid->pout = pid->p * pid->error[0];
        pid->iout += pid->i * pid->error[0];
        pid->dout = pid->d * (pid->error[0] - pid->error[1]);
        pid->out = pid->pout + pid->iout + pid->dout;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];

    if (pid->out >= MAX_DUTY)  pid->out = MAX_DUTY;
    if (pid->out <= MIX_DUTY)  pid->out = MIX_DUTY;
}

void Pid_Init(pid_t *pid, uint32_t mode, float p, float i, float d)
{
    pid->pid_mode = mode;
    pid->p = p;
    pid->i = i;
    pid->d = d;
}
