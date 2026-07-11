#ifndef __HAL_QGIMBAL_CAN_H
#define __HAL_QGIMBAL_CAN_H

#include <stdint.h>

/* Motor ID constants */
#define QGIMBAL_MOTOR_YAW   0x00
#define QGIMBAL_MOTOR_PITCH 0x01

/* Command byte values */
typedef enum {
    QGIMBAL_CMD_NOP        = 0,
    QGIMBAL_CMD_ENABLE     = 1,
    QGIMBAL_CMD_DISABLE    = 2,
    QGIMBAL_CMD_CURRENT    = 3,
    QGIMBAL_CMD_SPEED      = 4,
    QGIMBAL_CMD_ANGLE      = 5,
    QGIMBAL_CMD_LOW_SPEED  = 6,
    QGIMBAL_CMD_STEP_ANGLE = 7,
} QGimbal_Command;

/* Per-motor feedback state (updated by CAN RX ISR) */
typedef struct {
    uint8_t  enabled;
    float    current_a;       /* actual current (A) */
    float    speed_rpm;       /* actual speed (rpm) */
    float    angle_rad;       /* actual angle (radians) */
    uint8_t  feedback_valid;  /* set by ISR, cleared by consumer */
} QGimbal_MotorState;

extern QGimbal_MotorState g_gimbal_yaw;
extern QGimbal_MotorState g_gimbal_pitch;

/* CAN init: filters, interrupt routing, NVIC enable */
void QGimbal_CAN_Init(void);

/* Low-level: send raw command value via CAN */
void QGimbal_SendCommand(uint8_t motor_id, QGimbal_Command cmd, int16_t value);

/* High-level motor control helpers */
void QGimbal_Enable(uint8_t motor_id);
void QGimbal_Disable(uint8_t motor_id);
void QGimbal_SetAngle(uint8_t motor_id, float angle_rad);
void QGimbal_SetSpeed(uint8_t motor_id, float speed_rpm);
void QGimbal_SetCurrent(uint8_t motor_id, float current_a);

/* Called from CAN ISR to parse a received feedback frame */
void QGimbal_ProcessFeedback(void);

#endif /* __HAL_QGIMBAL_CAN_H */
