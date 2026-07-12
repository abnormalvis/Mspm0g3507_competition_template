#include "task_manager.h"
#include "task_one.h"
#include "task_two.h"
#include "task_three.h"
#include "task_four.h"
#include "tracking_loop.h"
#include "StandardPid.h"

void task_manager_init(void)
{
    g_motor_left_out = 0;
    g_motor_right_out = 0;
    task_running = 0;
    g_current_task = TASK_NONE;
    tracking_loop_init();
}

/* 启动一个任务：复位电机输出、循迹结果与循迹PID积分，并调用对应任务的init */
void task_manager_start(TaskID id)
{
    g_current_task    = id;
    task_running      = 1;
    g_motor_left_out  = 0;
    g_motor_right_out = 0;

    tracking_loop_init();          /* 复位 tracking_result */
    track_pid.ErrorInt = 0.0f;     /* 清循迹PID积分，避免上次残留 */

    switch (id)
    {
        case TASK_ONE:   task_one_init();   break;
        case TASK_TWO:   task_two_init();   break;
        case TASK_THREE: task_three_init(); break;
        case TASK_FOUR:  task_four_init();  break;
        default: break;
    }
}

void task_manager_run(void)
{
    switch (g_current_task)
    {
        case TASK_ONE:   task_one_run();   break;
        case TASK_TWO:   task_two_run();   break;
        case TASK_THREE: task_three_run(); break;
        case TASK_FOUR:  task_four_run();  break;
        default: break;
    }
}
