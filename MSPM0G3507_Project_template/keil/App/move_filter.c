/*********************************************************************************************************************
* Moving Average Filter Module
* Based on SEEKFREE TC264 open source library
* Copyright (c) 2022 SEEKFREE
*
* This library is open source under GPL (GNU General Public License) v3.0.
* GPL v3.0 allows modification, redistribution, and commercial use.
* See https://www.gnu.org/licenses/ for details.
*
* You should have received a copy of the GNU GPL along with this library.
* Commercial use beyond GPL 3.0 requires written permission.
* See libraries/doc/GPL3_permission_statement.txt or the LICENSE file.
* For inquiries, contact the author via official channels.
*
* Module name:    move_filter
* Description:    Moving average filter
* Version:         see libraries/doc version description
* Toolchain:      ADS v1.8.0
* Platform:       TC264D
* Shop:           https://seekfree.taobao.com/
*
* Changelog:
* Date              Author              Notes
* 2023-02-01       pudding             first version
********************************************************************************************************************/

#include "move_filter.h"

move_filter_struct speed_filter;

//-------------------------------------------------------------------------------------------------------------------
// Function:    moving average filter calculation
// Parameter:   move_filter     filter struct pointer
// Parameter:   new_data        latest raw data value
// Usage:       move_filter_calc(&speed_filter, 400);
// Returns:     None
//-------------------------------------------------------------------------------------------------------------------
void move_filter_calc(move_filter_struct *move_filter, float new_data)
{

    // Update sliding sum with new data, remove oldest
    move_filter->data_sum = move_filter->data_sum + new_data - move_filter->data_buffer[move_filter->index];
    // Permission statement:
    move_filter->data_average = move_filter->data_sum / move_filter->buffer_size;

    // Calculate average
    move_filter->data_buffer[move_filter->index] = new_data;
    move_filter->index ++;
    if(move_filter->buffer_size <= move_filter->index)
    {
        move_filter->index = 0;
    }
}


//-------------------------------------------------------------------------------------------------------------------
// Function:    moving average filter initialization
// Parameter:   move_filter     filter struct pointer
// Usage:       move_filter_init(&speed_filter);
// Returns:     None
//-------------------------------------------------------------------------------------------------------------------
void move_filter_init(move_filter_struct *move_filter)
{
	  uint8_t i;
    move_filter->data_average   = 0;
    move_filter->data_sum       = 0;
    move_filter->index          = 0;
    //Set filter buffer size
    move_filter->buffer_size    = MOVE_AVERAGE_SIZE;


    for(i = 0; i < move_filter->buffer_size; i ++)
    {
        move_filter->data_buffer[i] = 0;
    }
}

void move_filter_reduce(move_filter_struct *move_filter,float theta)
{
		uint8_t i = 0;
    move_filter->data_average   *= theta;
    move_filter->data_sum       *= theta;
    for( i = 0; i < move_filter->buffer_size; i ++)
    {
        move_filter->data_buffer[i] *= theta;
    }
}





