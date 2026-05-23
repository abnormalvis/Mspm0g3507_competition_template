#ifndef __MENU_TASK_H__
#define __MENU_TASK_H__

#include "ti_msp_dl_config.h"
#include <stdint.h>

/* ---- task enum ---- */
typedef enum {
    TASK_NONE = 0,
    TASK_ONE,
    TASK_TWO,
    TASK_THREE,
    TASK_FOUR,
} TaskID;

/* ---- menu jump table ---- */
typedef struct {
    uint8_t current;
    uint8_t next;
    uint8_t last;
    uint8_t enter;
    void (*current_operation)(void);
} Menu_table;

/* ---- global state ---- */
extern uint8_t  menu_active;
extern uint8_t  task_running;
extern uint8_t  g_stop_requested;
extern TaskID   g_current_task;
extern float    g_motor_left_out;
extern float    g_motor_right_out;

/* ---- task params (adjustable via menu) ---- */
extern int task1_max_laps;
extern int task4_max_laps;
extern int task_speed_base;

/* ---- functions ---- */
void menu_init(void);
void menu_key_set(void);
void task_manager_init(void);
void task_manager_run(void);

#endif
