#include "task_manager.h"
#include "task_one.h"
#include "task_two.h"
#include "task_three.h"
#include "task_four.h"
#include "tracking_loop.h"

void task_manager_init(void)
{
    g_motor_left_out = 0;
    g_motor_right_out = 0;
    task_running = 0;
    g_current_task = TASK_NONE;
    tracking_loop_init();
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
