#ifndef __SOLVE_H__
#define __SOLVE_H__

#include "ti_msp_dl_config.h"
#include "math.h"
#include "vofa.h"

#define BOARD_SIZE 9

extern int check_chess[3];
extern float Pos_pieces_task3[9][2];

//返回当前棋局最佳走法
int best_move(const int board[BOARD_SIZE]);
void check_board_change(const int curr_board[9]);
void cameraToReal();
void calcNineGridCenter();

#endif
