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
* File name          : zf_common_fifo
* Company name       : Chengdu SeekFree Technology Co., Ltd.
* Version info       : See version file under libraries/doc directory
* Dev environment    : ADS v1.9.4
* Target platform    : TC264D
* Contact            : https://seekfree.taobao.com/
*
* Change Log:
* Date              Author              Notes
* 2022-08-10        Teternal            first version
* 2023-12-06        Teternal            Update operation logic, fix abnormal data read bug
********************************************************************************************************************/

#ifndef _zf_common_fifo_h_
#define _zf_common_fifo_h_

#include "stdint.h"
#include "math.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
typedef enum
{
    FIFO_SUCCESS,                                                               // FIFO operation success

    FIFO_RESET_UNDO,                                                            // FIFO reset operation not executed
    FIFO_CLEAR_UNDO,                                                            // FIFO clear operation not executed
    FIFO_BUFFER_NULL,                                                           // FIFO user buffer exception
    FIFO_WRITE_UNDO,                                                            // FIFO write operation not executed
    FIFO_SPACE_NO_ENOUGH,                                                       // FIFO write operation insufficient space
    FIFO_READ_UNDO,                                                             // FIFO read operation not executed
    FIFO_DATA_NO_ENOUGH,                                                        // FIFO read operation insufficient data length
}fifo_state_enum;                                                               // FIFO state enum

// State machine logic
// Reset operation   Must force FIFO to idle before use
// Write operation   Must complete its own write operation before returning
// Sequential read    Must complete clear and reset operations before returning
// Tail read          Must complete clear and its own write operation before returning
// Read and clear     Must complete clear and its own read operation before returning
// Designed to prevent interrupt nesting from damaging data
typedef enum
{
    FIFO_IDLE       = 0x00,                                                     // Idle state

    FIFO_RESET      = 0x01,                                                     // Executing reset buffer
    FIFO_CLEAR      = 0x02,                                                     // Executing clear buffer
    FIFO_WRITE      = 0x04,                                                     // Executing write buffer
    FIFO_READ       = 0x08,                                                     // Executing read buffer
}fifo_execution_enum;                                                           // FIFO operation state Reserved for nesting use, cannot fully avoid concurrency

typedef enum
{
    FIFO_READ_AND_CLEAN,                                                        // FIFO read mode: read and release corresponding data
    FIFO_READ_ONLY,                                                             // FIFO read mode: read only
}fifo_operation_enum;

typedef enum
{
    FIFO_DATA_8BIT,                                                             // FIFO data bit width 8bit
    FIFO_DATA_16BIT,                                                            // FIFO data bit width 16bit
    FIFO_DATA_32BIT,                                                            // FIFO data bit width 32bit
}fifo_data_type_enum;

typedef struct
{
    uint8_t               execution;                                              // Execution operation
    fifo_data_type_enum type;                                                   // Data type
    void                *buffer;                                                // Buffer pointer
    uint32_t              head;                                                   // Buffer head pointer, always points to empty buffer
    uint32_t              end;                                                    // Buffer tail pointer, always points to non-empty buffer (except when fully empty)
    uint32_t              size;                                                   // Buffer remaining size
    uint32_t              max;                                                    // Buffer total size
}fifo_obj_struct;

fifo_state_enum fifo_clear              (fifo_obj_struct *fifo);
uint32_t          fifo_used               (fifo_obj_struct *fifo);

fifo_state_enum fifo_write_element      (fifo_obj_struct *fifo, uint32_t dat);
fifo_state_enum fifo_write_buffer       (fifo_obj_struct *fifo, void *dat, uint32_t length);
fifo_state_enum fifo_read_element       (fifo_obj_struct *fifo, void *dat, fifo_operation_enum flag);
fifo_state_enum fifo_read_buffer        (fifo_obj_struct *fifo, void *dat, uint32_t *length, fifo_operation_enum flag);
fifo_state_enum fifo_read_tail_buffer   (fifo_obj_struct *fifo, void *dat, uint32_t *length, fifo_operation_enum flag);

fifo_state_enum fifo_init               (fifo_obj_struct *fifo, fifo_data_type_enum type, void *buffer_addr, uint32_t size);

#endif
