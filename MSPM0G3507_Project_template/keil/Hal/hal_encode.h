#ifndef _HAL_ENCODE_H
#define _HAL_ENCODE_H
#include <stdint.h>

typedef enum {
    FORWARD,
    REVERSAL
} ENCODER_DIR;

typedef struct {
    volatile long long temp_count;
    int count;
    ENCODER_DIR dir;
} ENCODER_RES;

extern ENCODER_RES motor_left_encoder;
extern ENCODER_RES motor_right_encoder;

/* Live encoder counts for debug/VOFA display */
extern volatile int encoder_val_left;
extern volatile int encoder_val_right;

/* Zero-valued stubs for legacy path-tracking display code in app.c */
extern float distance_inter, distance_l, distance_r;
extern float point_actual[2], point_A[2], point_B[2], point_C[2], point_D[2];

void  hal_Encoder_Init(void);
int   Get_Encoder_Count(ENCODER_RES *encoder);
ENCODER_DIR Get_Encoder_Dir(ENCODER_RES *encoder);
void  Encoder_Update(ENCODER_RES *encoder);

#endif
