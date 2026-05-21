#include "Solve.h"
#include "K230.h"

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>


int check_chess[3];
int prev_board[9];
float Pos_pieces_task3[9][2] = {0};

static const int WIN_LINES[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},
    {0,3,6}, {1,4,7}, {2,5,8},
    {0,4,8}, {2,4,6}
};

static bool check_winner(const int board[BOARD_SIZE], int color) {
    for (int i = 0; i < 8; ++i) {
        int a = WIN_LINES[i][0];
        int b = WIN_LINES[i][1];
        int c = WIN_LINES[i][2];
        if (board[a] == color && board[b] == color && board[c] == color)
            return true;
    }
    return false;
}

static bool is_full(const int board[BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i)
        if (board[i] == -1) return false;
    return true;
}

static int evaluate(const int board[BOARD_SIZE], int my_color) {
    if (check_winner(board, my_color)) return 10;
    else if (check_winner(board, 1 - my_color)) return -10;
    else return 0;
}

static int minimax(int board[BOARD_SIZE], int current_player, int my_color) {
    if (check_winner(board, 0) || check_winner(board, 1) || is_full(board))
        return evaluate(board, my_color);

    if (current_player == my_color) {
        int best = -INT_MAX;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (board[i] == -1) {
                board[i] = current_player;
                int score = minimax(board, 1 - current_player, my_color);
                board[i] = -1;
                if (score > best) best = score;
            }
        }
        return best;
    } else {
        int best = INT_MAX;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (board[i] == -1) {
                board[i] = current_player;
                int score = minimax(board, 1 - current_player, my_color);
                board[i] = -1;
                if (score < best) best = score;
            }
        }
        return best;
    }
}

static int get_current_player(const int board[BOARD_SIZE]) {
    int cnt_black = 0, cnt_white = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i] == 1) cnt_black++;
        else if (board[i] == 0) cnt_white++;
    }
    return (cnt_black == cnt_white) ? 1 : 0;
}

// 位置优先级：中心(4) > 角(0,2,6,8) > 边(1,3,5,7)
static int position_priority(int pos) {
    if (pos == 4) return 0;      // 中心最高优先级
    if (pos == 0 || pos == 2 || pos == 6 || pos == 8) return 1; // 角
    return 2;  // 边
}

// 修改点：直接返回 0~8，棋盘满返回 -1
int best_move(const int board[BOARD_SIZE]) {
    int current_player = get_current_player(board);
    int best_pos = -1;
    int best_score = -INT_MAX;

    int mutable_board[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) mutable_board[i] = board[i];

    // ==============================================
    // 【超级加速】第一步直接下中心，不用算！！！
    // ==============================================
    int empty_cnt = 0;
    for (int i=0; i<9; i++) if (board[i] == -1) empty_cnt++;
    if (empty_cnt == 9) {
        best_pos = 4; // 第一步必下中心
        goto UPDATE_BOARD; // 直接跳过所有计算
    }

    // ==============================================
    // 【新增】当棋盘仅有一颗黑棋在中心时，白棋直接下角（避免搜索）
    // ==============================================
    // 统计非空棋子数量及黑棋位置
    int non_empty_count = 0;
    int black_pos = -1;
    for (int i = 0; i < 9; i++) {
        if (board[i] != -1) {
            non_empty_count++;
            if (board[i] == 0) black_pos = i; // 0 代表黑棋（假设玩家0为黑棋）
        }
    }
    if (non_empty_count == 1 && black_pos == 4 && current_player == 1) {
        // 白棋直接下任意角（这里选择位置 0）
        best_pos = 0;
        goto UPDATE_BOARD;
    }

    // 正常搜索
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (mutable_board[i] == -1) {
            mutable_board[i] = current_player;
            int score = minimax(mutable_board, 1 - current_player, current_player);
            mutable_board[i] = -1;

            if (score > best_score ||
                (score == best_score && position_priority(i) < position_priority(best_pos))) {
                best_score = score;
                best_pos = i;
            }
        }
    }

UPDATE_BOARD:
    // 更新上一步棋盘
    for(int i = 0; i < 9; i++){
        prev_board[i] = board[i];
    }
    if (best_pos != -1) {
        prev_board[best_pos] = current_player;
    }
    return best_pos;
}

// 外部必须定义这个数组用于接收结果：int check_chess[3];
void check_board_change(const int curr_board[9])
{
    // 先默认正常
    check_chess[0] = 0;
    check_chess[1] = 0;
    check_chess[2] = 0;

    int change_pos = -1;       // 记录唯一变化的位置
    int change_count = 0;      // 变化位置总数
    int from = -1, to = -1;    // 违规移动：从哪 → 到哪

    // 遍历所有位置，对比前后棋盘
    for (int i = 0; i < 9; i++) {
        if (prev_board[i] != curr_board[i]) {
            change_count++;
            // 情况1：之前有子 → 现在空（被挪走）
            if (prev_board[i] != -1 && curr_board[i] == -1) {
                from = i;
            }
            // 情况2：之前空 → 现在有子（新下/被挪来）
            else if (prev_board[i] == -1 && curr_board[i] != -1) {
                to = i;
                change_pos = i;
            }
        }
    }

    // ==============================================
    // 违规情况1：移动了一颗已下的棋子（最关键）
    // ==============================================
    if (change_count == 2 && from != -1 && to != -1) {
        check_chess[0] = 1;
        check_chess[1] = from;
        check_chess[2] = to;
        return;
    }

    // ==============================================
    // 正常情况：只变了一个位置，且是从空 → 有子
    // 这里加入你的 get_current_player 检查顺序
    // ==============================================
    if (change_count == 1 && change_pos != -1 && prev_board[change_pos] == -1) {
        // 上一棋盘该谁下？
        int expected = get_current_player(prev_board);
        // 实际下的子是什么？
        int real = curr_board[change_pos];

        // 顺序正确 → 保持 [0,0,0]
        if (real == expected) {
            return;
        }
    }

    // 其他所有情况（正常落子、多颗变化、违规删子等）都视为正常 0 0 0
}

void cameraToReal()
{
    double x , y;
    // 提取摄像头三个点的坐标
    double x1 = k230_RxPacket[0], y1 = k230_RxPacket[1];
    double x2 = k230_RxPacket[2], y2 = k230_RxPacket[3];
    double x3 = k230_RxPacket[4], y3 = k230_RxPacket[5];

    // 计算仿射变换分母（避免重复计算）
    double denominator = (x1 - x2) * (y3 - y2) - (y1 - y2) * (x3 - x2);

    x = k230_RxPacket[6],y = k230_RxPacket[7];
    double u = ((x - x2) * (y3 - y2) - (y - y2) * (x3 - x2)) / denominator;
    double v = ((y - y2) * (x1 - x2) - (x - x2) * (y1 - y2)) / denominator;
    // 计算现实坐标（核心公式）
    k230_RxPacket[6] = 9.6 * u;  // 现实X坐标
    k230_RxPacket[7] = 9.6 * v;  // 现实Y坐标
        
    x = k230_RxPacket[8],y = k230_RxPacket[9];
    u = ((x - x2) * (y3 - y2) - (y - y2) * (x3 - x2)) / denominator;
    v = ((y - y2) * (x1 - x2) - (x - x2) * (y1 - y2)) / denominator;
    // 计算现实坐标（核心公式）
    k230_RxPacket[8] = 9.6 * u;  // 现实X坐标
    k230_RxPacket[9] = 9.6 * v;  // 现实Y坐标    
    
    
    x = k230_RxPacket[10],y = k230_RxPacket[11];
    u = ((x - x2) * (y3 - y2) - (y - y2) * (x3 - x2)) / denominator;
    v = ((y - y2) * (x1 - x2) - (x - x2) * (y1 - y2)) / denominator;
    // 计算现实坐标（核心公式）
    k230_RxPacket[10] = 9.6 * u;  // 现实X坐标
    k230_RxPacket[11] = 9.6 * v;  // 现实Y坐标
}

/**
 * @brief 计算正方形九宫格所有方格中心坐标
 * @param x1,y1  正方形顺时针第一个角 (对应7号格)
 * @param x2,y2  正方形顺时针第二个角 (对应1号格，原点)
 * @param x3,y3  正方形顺时针第三个角
 * 无返回值，结果存入全局数组 Pos_pieces_task3
 */
void calcNineGridCenter()
{
    double x1 = k230_RxPacket[6],  y1 = k230_RxPacket[7],  x2 = k230_RxPacket[8],  y2 = k230_RxPacket[9],  x3 = k230_RxPacket[10],  y3 = k230_RxPacket[11];
    // 1. 计算正方形单条边的向量（x2->x3 横向，x2->x1 纵向）
    double vec_w_x = x3 - x2;  // 横向向量x分量（1→2→3方向）
    double vec_w_y = y3 - y2;  // 横向向量y分量
    double vec_h_x = x1 - x2;  // 纵向向量x分量（1→4→7方向）
    double vec_h_y = y1 - y2;  // 纵向向量y分量

    // 2. 计算九宫格单格的步长（正方形边长按3等分）
    double step_x = vec_w_x / 3.0;
    double step_y = vec_w_y / 3.0;
    double step_vx = vec_h_x / 3.0;
    double step_vy = vec_h_y / 3.0;

    // 3. 1号格中心坐标（基准点：x2,y2 偏移半个格子）
    double center1_x = x2 + step_x / 2.0 + step_vx / 2.0;
    double center1_y = y2 + step_y / 2.0 + step_vy / 2.0;

    // 4. 按编号计算9个格子中心（严格对应你的规则）
    // 第一行：1、2、3
    Pos_pieces_task3[0][0] = center1_x;
    Pos_pieces_task3[0][1] = center1_y;

    Pos_pieces_task3[1][0] = center1_x + step_x;
    Pos_pieces_task3[1][1] = center1_y + step_y;

    Pos_pieces_task3[2][0] = center1_x + 2 * step_x;
    Pos_pieces_task3[2][1] = center1_y + 2 * step_y;

    // 第二行：4、5、6
    Pos_pieces_task3[3][0] = center1_x + step_vx;
    Pos_pieces_task3[3][1] = center1_y + step_vy;

    Pos_pieces_task3[4][0] = center1_x + step_vx + step_x;
    Pos_pieces_task3[4][1] = center1_y + step_vy + step_y;

    Pos_pieces_task3[5][0] = center1_x + step_vx + 2 * step_x;
    Pos_pieces_task3[5][1] = center1_y + step_vy + 2 * step_y;

    // 第三行：7、8、9
    Pos_pieces_task3[6][0] = center1_x + 2 * step_vx;
    Pos_pieces_task3[6][1] = center1_y + 2 * step_vy;

    Pos_pieces_task3[7][0] = center1_x + 2 * step_vx + step_x;
    Pos_pieces_task3[7][1] = center1_y + 2 * step_vy + step_y;

    Pos_pieces_task3[8][0] = center1_x + 2 * step_vx + 2 * step_x;
    Pos_pieces_task3[8][1] = center1_y + 2 * step_vy + 2 * step_y;
    
    for(int i = 0; i < 9; i++){
        Pos_pieces_task3[i][0] = Pos_pieces_task3[i][0] + 0.6;
        Pos_pieces_task3[i][1] = Pos_pieces_task3[i][1] + 2.2;
    }
}