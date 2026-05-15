/*********************************************************************************************************************
* TC264 Open Source Library - a lightweight open source library for official SDK interfaces
* Copyright (c) 2022 SEEKFREE (SeekFree Technology)
*
* This file is part of the TC264 Open Source Library
*
* TC264 Open Source Library is free software.
* You can redistribute it and/or modify it under the terms of the
* GPL (GNU General Public License) version 3 (GPL3.0) or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GPL for more details.
*
* You should have received a copy of the GPL along with this library.
* If not, see <https://www.gnu.org/licenses/>
*
* Important note:
* This source code uses the GPL3.0 open source license agreement.
* Please refer to the English version in GPL3_permission_statement.txt under libraries/doc directory.
* See the LICENSE file under the libraries directory for details.
* Welcome to use and distribute. When modifying, please retain the SeekFree copyright.
*
* File name          : move_filter
* Company name       : Chengdu SeekFree Technology Co., Ltd.
* Version info       : See version file under libraries/doc directory
* Dev environment    : ADS v1.8.0
* Target platform    : TC264D
* Contact            : https://seekfree.taobao.com/
*
* Change Log:
* Date              Author              Notes
* 2023-02-01        pudding             first version
********************************************************************************************************************/
#ifndef _MOVE_FILTER_H
#define _MOVE_FILTER_H

#define MOVE_AVERAGE_SIZE   4  // Moving window filter size

#include "stdint.h"

typedef struct
{
    uint8_t index;                            // Index
    uint8_t buffer_size;                      // Buffer size
    float data_buffer[MOVE_AVERAGE_SIZE];   // Data buffer
    float data_sum;                         // Data sum
    float data_average;                     // Sliding average
}move_filter_struct;

extern move_filter_struct speed_filter;


void move_filter_init(move_filter_struct *move_average);
void move_filter_calc(move_filter_struct *move_average, float new_data);
void move_filter_reduce(move_filter_struct *move_filter,float theta);

/* First-order low-pass filter: y[n] = y[n-1] + alpha * (x[n] - y[n-1]) */
typedef struct
{
    float alpha;      /* filter coefficient (0~1], smaller = smoother */
    float output;     /* filtered output */
} lowpass_filter_struct;

void lowpass_filter_init(lowpass_filter_struct *f, float alpha);
float lowpass_filter_calc(lowpass_filter_struct *f, float input);
void lowpass_filter_set_alpha(lowpass_filter_struct *f, float alpha);

#endif /* CODE_MOVE_FILTER_H_ */
