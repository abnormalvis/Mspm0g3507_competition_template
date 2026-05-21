#ifndef __DUTY_CHESS_H
#define __DUTY_CHESS_H

#include <stdint.h>
#include "hal_key.h"

extern float Pos_black[5][2];
extern float Pos_white[5][2];
extern float Pos_pieces_base[9][2];
extern int   pieces_state[9];

void duty_chess_init(void);
void duty_chess_on_key(KEY_VALUE_TYPEDEF k);
void duty_chess_run(void);

#endif
