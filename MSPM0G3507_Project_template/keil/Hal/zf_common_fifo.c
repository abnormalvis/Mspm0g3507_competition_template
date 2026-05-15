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

#include "zf_common_fifo.h"

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : FIFO head pointer offset
// Description    : *fifo               FIFO object pointer
// Description    : offset              Offset
// Return Value   : void
// Usage Example  : fifo_head_offset(fifo, 1);
// Note           : Internal function. Users do not need to pay attention and should not modify.
//-------------------------------------------------------------------------------------------------------------------
static void fifo_head_offset (fifo_obj_struct *fifo, uint32_t offset)
{
    fifo->head += offset;

    while(fifo->max <= fifo->head)                                              // If out of range, reduce by buffer size until less than max buffer size
    {
        fifo->head -= fifo->max;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : FIFO tail pointer offset
// Description    : *fifo               FIFO object pointer
// Description    : offset              Offset
// Return Value   : void
// Usage Example  : fifo_end_offset(fifo, 1);
// Note           : Internal function. Users do not need to pay attention and should not modify.
//-------------------------------------------------------------------------------------------------------------------
static void fifo_end_offset (fifo_obj_struct *fifo, uint32_t offset)
{
    fifo->end += offset;

    while(fifo->max <= fifo->end)                                               // If out of range, reduce by buffer size until less than max buffer size
    {
        fifo->end -= fifo->max;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : FIFO reset buffer
// Description    : *fifo               FIFO object pointer
// Return Value   : void
// Usage Example  : fifo_clear(fifo);
// Note           : Clear current FIFO buffer memory
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_clear (fifo_obj_struct *fifo)
{
    fifo_state_enum return_state = FIFO_SUCCESS;                                // Return state value
    do
    {
//        if(FIFO_IDLE != fifo->execution)                                        // Check if current FIFO is idle
//        {
//            return_state = FIFO_RESET_UNDO;                                     // Reset operation not executed
//            break;
//        }
        fifo->execution |= FIFO_RESET;                                          // Set reset operation bit
        fifo->head      = 0;                                                    // Reset FIFO internal value
        fifo->end       = 0;                                                    // Reset FIFO internal value
        fifo->size      = fifo->max;                                            // Reset FIFO internal value
        switch(fifo->type)
        {
            case FIFO_DATA_8BIT:    memset(fifo->buffer, 0, fifo->max);     break;
            case FIFO_DATA_16BIT:   memset(fifo->buffer, 0, fifo->max * 2); break;
            case FIFO_DATA_32BIT:   memset(fifo->buffer, 0, fifo->max * 4); break;
        }
        fifo->execution = FIFO_IDLE;                                            // Clear operation state
    }while(0);
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : FIFO query current data count
// Description    : *fifo               FIFO object pointer
// Return Value   : uint32_t            Used count
// Usage Example  : uint32_t len = fifo_used(fifo);
// Note
//-------------------------------------------------------------------------------------------------------------------
uint32_t fifo_used (fifo_obj_struct *fifo)
{
    return (fifo->max - fifo->size);                                            // Return current FIFO data element count
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Write single element to FIFO
// Description    : *fifo               FIFO object pointer
// Description    : dat                 Data
// Return Value   : fifo_state_enum     Operation state
// Usage Example  : zf_log(fifo_write_element(&fifo, data) == FIFO_SUCCESS, "fifo_write_byte error");
// Note
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_write_element (fifo_obj_struct *fifo, uint32_t dat)
{
    fifo_state_enum return_state = FIFO_SUCCESS;                                // Return state value

    do
    {
        if((FIFO_RESET | FIFO_WRITE) & fifo->execution)                         // Avoid competing with reset or write states, write priority is highest
        {
            return_state = FIFO_WRITE_UNDO;                                     // Write operation not executed
            break;
        }
        fifo->execution |= FIFO_WRITE;                                          // Set write operation bit

        if(1 <= fifo->size)                                                     // Remaining space sufficient for this element
        {
            switch(fifo->type)
            {
                case FIFO_DATA_8BIT:    ((uint8_t *)fifo->buffer)[fifo->head]  = (uint8_t)dat;  break;
                case FIFO_DATA_16BIT:   ((uint16_t *)fifo->buffer)[fifo->head] = (uint16_t)dat; break;
                case FIFO_DATA_32BIT:   ((uint32_t *)fifo->buffer)[fifo->head] = dat; break;
            }
            fifo_head_offset(fifo, 1);                                          // Head pointer offset
            fifo->size -= 1;                                                    // Decrease buffer remaining length
        }
        else
        {
            return_state = FIFO_SPACE_NO_ENOUGH;                                // Current FIFO is full, write failed, return insufficient space
        }
        fifo->execution &= ~FIFO_WRITE;                                         // Clear write operation bit
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Write buffer to FIFO
// Description    : *fifo               FIFO object pointer
// Description    : *dat                Data source buffer pointer
// Description    : length              Data length to write
// Return Value   : fifo_state_enum     Operation state
// Usage Example  : zf_log(fifo_write_buffer(&fifo, data, 32) == FIFO_SUCCESS, "fifo_write_buffer error");
// Note
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_write_buffer (fifo_obj_struct *fifo, void *dat, uint32_t length)
{
    fifo_state_enum return_state = FIFO_SUCCESS;                                // Return state value
    uint32_t temp_length = 0;

    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;                                    // User buffer exception
            break;
        }
        if((FIFO_RESET | FIFO_WRITE) & fifo->execution)                         // Avoid competing with reset or write states, write priority is highest
        {
            return_state = FIFO_WRITE_UNDO;                                     // Write operation not executed
            break;
        }
        fifo->execution |= FIFO_WRITE;                                          // Set write operation bit

        if(length <= fifo->size)                                                // Remaining space sufficient for this data
        {
            temp_length = fifo->max - fifo->head;                               // Calculate space from head pointer to buffer end

            if(length > temp_length)                                            // If space to buffer end is insufficient for write, split into two segments
            {
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:
                    {
                        memcpy(
                            &(((uint8_t *)fifo->buffer)[fifo->head]),
                            dat, temp_length);                                  // Copy first segment
                        fifo_head_offset(fifo, temp_length);                    // Head pointer offset
                        memcpy(
                            &(((uint8_t *)fifo->buffer)[fifo->head]),
                            &(((uint8_t *)dat)[temp_length]),
                            length - temp_length);                              // Copy second segment
                        fifo_head_offset(fifo, length - temp_length);           // Head pointer offset
                    }break;
                    case FIFO_DATA_16BIT:
                    {
                        memcpy(
                            &(((uint16_t *)fifo->buffer)[fifo->head]),
                            dat, temp_length * 2);                              // Copy first segment
                        fifo_head_offset(fifo, temp_length);                    // Head pointer offset
                        memcpy(
                            &(((uint16_t *)fifo->buffer)[fifo->head]),
                            &(((uint16_t *)dat)[temp_length]),
                            (length - temp_length) * 2);                        // Copy second segment
                        fifo_head_offset(fifo, length - temp_length);           // Head pointer offset
                    }break;
                    case FIFO_DATA_32BIT:
                    {
                        memcpy(
                            &(((uint32_t *)fifo->buffer)[fifo->head]),
                            dat, temp_length * 4);                              // Copy first segment
                        fifo_head_offset(fifo, temp_length);                    // Head pointer offset
                        memcpy(
                            &(((uint32_t *)fifo->buffer)[fifo->head]),
                            &(((uint32_t *)dat)[temp_length]),
                            (length - temp_length) * 4);                        // Copy second segment
                        fifo_head_offset(fifo, length - temp_length);           // Head pointer offset
                    }break;
                }
            }
            else
            {
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:
                    {
                        memcpy(
                            &(((uint8_t *)fifo->buffer)[fifo->head]),
                            dat, length);                                       // Single segment write
                        fifo_head_offset(fifo, length);                         // Head pointer offset
                    }break;
                    case FIFO_DATA_16BIT:
                    {
                        memcpy(
                            &(((uint16_t *)fifo->buffer)[fifo->head]),
                            dat, length * 2);                                   // Single segment write
                        fifo_head_offset(fifo, length);                         // Head pointer offset
                    }break;
                    case FIFO_DATA_32BIT:
                    {
                        memcpy(
                            &(((uint32_t *)fifo->buffer)[fifo->head]),
                            dat, length * 4);                                   // Single segment write
                        fifo_head_offset(fifo, length);                         // Head pointer offset
                    }break;
                }
            }

            fifo->size -= length;                                               // Decrease buffer remaining length
        }
        else
        {
            return_state = FIFO_SPACE_NO_ENOUGH;                                // Current FIFO is full, write failed, return insufficient space
        }
        fifo->execution &= ~FIFO_WRITE;                                         // Clear write operation bit
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read single element from FIFO
// Description    : *fifo               FIFO object pointer
// Description    : *dat                Target buffer pointer
// Description    : flag                Whether to modify FIFO state, select whether to delete read data
// Return Value   : fifo_state_enum     Operation state
// Usage Example  : zf_log(fifo_read_element(&fifo, data, FIFO_READ_ONLY) == FIFO_SUCCESS, "fifo_read_byte error");
// Note
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_read_element (fifo_obj_struct *fifo, void *dat, fifo_operation_enum flag)
{
    fifo_state_enum return_state = FIFO_SUCCESS;                                // Return state value

    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;                                    // User buffer exception
        }
        else
        {
            if((FIFO_RESET | FIFO_CLEAR) & fifo->execution)                     // Check if current FIFO is executing clear or reset
            {
                return_state = FIFO_READ_UNDO;                                  // Read operation not executed
                break;
            }

            if(1 > fifo_used(fifo))
            {
                return_state = FIFO_DATA_NO_ENOUGH;                             // No data in buffer, data length insufficient
                break;                                                          // Exit directly
            }

            fifo->execution |= FIFO_READ;                                       // Set read operation bit
            switch(fifo->type)
            {
                case FIFO_DATA_8BIT:    *((uint8_t *)dat) = ((uint8_t *)fifo->buffer)[fifo->end];   break;
                case FIFO_DATA_16BIT:   *((uint16_t *)dat) = ((uint16_t *)fifo->buffer)[fifo->end]; break;
                case FIFO_DATA_32BIT:   *((uint32_t *)dat) = ((uint32_t *)fifo->buffer)[fifo->end]; break;
            }
            fifo->execution &= ~FIFO_READ;                                      // Clear read operation bit
        }

        if(FIFO_READ_AND_CLEAN == flag)                                         // If read and delete FIFO state is selected
        {
            if((FIFO_RESET | FIFO_CLEAR | FIFO_READ) == fifo->execution)        // Reset, clear, and read states coexisting is abnormal
            {
                return_state = FIFO_CLEAR_UNDO;                                 // Clear operation not executed
                break;
            }
            fifo->execution |= FIFO_CLEAR;                                      // Set clear operation bit
            fifo_end_offset(fifo, 1);                                           // Move FIFO head pointer
            fifo->size += 1;                                                    // Release corresponding length of space
            fifo->execution &= ~FIFO_CLEAR;                                     // Clear clear operation bit
        }
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read buffer from FIFO
// Description    : *fifo               FIFO object pointer
// Description    : *dat                Target buffer pointer
// Description    : *length             Data length to read. This parameter will be modified if insufficient data is read.
// Description    : flag                Whether to modify FIFO state, select whether to delete read data
// Return Value   : fifo_state_enum     Operation state
// Usage Example  : zf_log(fifo_read_buffer(&fifo, data, &length, FIFO_READ_ONLY) == FIFO_SUCCESS, "fifo_read_buffer error");
// Note
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_read_buffer (fifo_obj_struct *fifo, void *dat, uint32_t *length, fifo_operation_enum flag)
{

    fifo_state_enum return_state = FIFO_SUCCESS;                                // Return state value
    uint32_t temp_length = 0;
    uint32_t fifo_data_length = 0;

    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;
        }
        else
        {
            if((FIFO_RESET | FIFO_CLEAR) & fifo->execution)                     // Check if current FIFO is executing clear or reset
            {
                *length = fifo_data_length;                                     // Return length actually read
                return_state = FIFO_READ_UNDO;                                  // Read operation not executed
                break;
            }

            fifo_data_length = fifo_used(fifo);                                 // Get current element count
            if(*length > fifo_data_length)                                      // Check if length is sufficient
            {
                *length = fifo_data_length;                                     // Return length actually read
                return_state = FIFO_DATA_NO_ENOUGH;                             // Flag data insufficient
                if(0 == fifo_data_length)                                       // If no data, exit directly
                {
                    fifo->execution &= ~FIFO_READ;                              // Clear read operation bit
                    break;
                }
            }

            fifo->execution |= FIFO_READ;                                       // Set read operation bit
            temp_length = fifo->max - fifo->end;                                // Calculate space from tail pointer to buffer end
            if(*length <= temp_length)                                          // Sufficient for single read
            {
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:    memcpy(dat, &(((uint8_t *)fifo->buffer)[fifo->end]), *length);        break;
                    case FIFO_DATA_16BIT:   memcpy(dat, &(((uint16_t *)fifo->buffer)[fifo->end]), *length * 2);   break;
                    case FIFO_DATA_32BIT:   memcpy(dat, &(((uint32_t *)fifo->buffer)[fifo->end]), *length * 4);   break;
                }
            }
            else
            {
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:
                    {
                        memcpy(dat, &(((uint8_t *)fifo->buffer)[fifo->end]), temp_length);
                        memcpy(&(((uint8_t *)dat)[temp_length]), fifo->buffer, *length - temp_length);
                    }break;
                    case FIFO_DATA_16BIT:
                    {
                        memcpy(dat, &(((uint16_t *)fifo->buffer)[fifo->end]), temp_length * 2);
                        memcpy(&(((uint16_t *)dat)[temp_length]), fifo->buffer, (*length - temp_length) * 2);
                    }break;
                    case FIFO_DATA_32BIT:
                    {
                        memcpy(dat, &(((uint32_t *)fifo->buffer)[fifo->end]), temp_length * 4);
                        memcpy(&(((uint32_t *)dat)[temp_length]), fifo->buffer, (*length - temp_length) * 4);
                    }break;
                }
            }
            fifo->execution &= ~FIFO_READ;                                      // Clear read operation bit
        }

        if(FIFO_READ_AND_CLEAN == flag)                                         // If read and delete FIFO state is selected
        {
            if((FIFO_RESET | FIFO_CLEAR | FIFO_READ) == fifo->execution)        // Reset, clear, and read states coexisting is abnormal
            {
                return_state = FIFO_CLEAR_UNDO;                                 // Clear operation not executed
                break;
            }
            fifo->execution |= FIFO_CLEAR;                                      // Set clear operation bit
            fifo_end_offset(fifo, *length);                                     // Move FIFO head pointer
            fifo->size += *length;                                              // Release corresponding length of space
            fifo->execution &= ~FIFO_CLEAR;                                     // Clear clear operation bit
        }
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read specified buffer from FIFO tail
// Description    : *fifo               FIFO object pointer
// Description    : *dat                Target buffer pointer
// Description    : *length             Data length to read. This parameter will be modified if insufficient data is read.
// Description    : flag                Whether to modify FIFO state, select whether to delete read data
// Return Value   : fifo_state_enum     Operation state
// Usage Example  : zf_log(fifo_read_tail_buffer(&fifo, data, &length, FIFO_READ_ONLY) == FIFO_SUCCESS, "fifo_read_buffer error");
// Note           : If FIFO_READ_AND_CLEAN flag is used, ALL data will be discarded (entire FIFO cleared), not just the requested length
//                  If FIFO_READ_AND_CLEAN flag is used, ALL data will be discarded (entire FIFO cleared), not just the requested length
//                  If FIFO_READ_AND_CLEAN flag is used, ALL data will be discarded (entire FIFO cleared), not just the requested length
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_read_tail_buffer (fifo_obj_struct *fifo, void *dat, uint32_t *length, fifo_operation_enum flag)
{

    fifo_state_enum return_state = FIFO_SUCCESS;                                // Return state value
    uint32_t temp_length = 0;
    uint32_t fifo_data_length = 0;

    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;
        }
        else
        {
            if((FIFO_RESET | FIFO_CLEAR | FIFO_WRITE) & fifo->execution)        // Check if current FIFO is executing clear or reset
            {
                *length = fifo_data_length;                                     // Return length actually read
                return_state = FIFO_READ_UNDO;                                  // Read operation not executed
                break;
            }

            fifo_data_length = fifo_used(fifo);                                 // Get current element count
            if(*length > fifo_data_length)                                      // Check if length is sufficient
            {
                *length = fifo_data_length;                                     // Return length actually read
                return_state = FIFO_DATA_NO_ENOUGH;                             // Flag data insufficient
                if(0 == fifo_data_length)                                       // If no data, exit directly
                {
                    fifo->execution &= ~FIFO_READ;                              // Clear read operation bit
                    break;
                }
            }

            fifo->execution |= FIFO_READ;                                       // Set read operation bit
            if((fifo->head > fifo->end) || (fifo->head >= *length))
            {
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:    memcpy(dat, &(((uint8_t *)fifo->buffer)[fifo->head - *length]), *length);     break;
                    case FIFO_DATA_16BIT:   memcpy(dat, &(((uint16_t *)fifo->buffer)[fifo->head - *length]), *length * 2);break;
                    case FIFO_DATA_32BIT:   memcpy(dat, &(((uint32_t *)fifo->buffer)[fifo->head - *length]), *length * 4);break;
                }
            }
            else
            {
                temp_length = *length - fifo->head;                             // Calculate space from tail pointer to buffer end
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:
                    {
                        memcpy(dat, &(((uint8_t *)fifo->buffer)[fifo->max - temp_length]), temp_length);
                        memcpy(&(((uint8_t *)dat)[temp_length]), &(((uint8_t *)fifo->buffer)[fifo->head - *length]), (*length - temp_length));
                    }break;
                    case FIFO_DATA_16BIT:
                    {
                        memcpy(dat, &(((uint16_t *)fifo->buffer)[fifo->max - temp_length]), temp_length * 2);
                        memcpy(&(((uint16_t *)dat)[temp_length]), &(((uint16_t *)fifo->buffer)[fifo->head - *length]), (*length - temp_length) * 2);
                    }break;
                    case FIFO_DATA_32BIT:
                    {
                        memcpy(dat, &(((uint32_t *)fifo->buffer)[fifo->max - temp_length]), temp_length * 4);
                        memcpy(&(((uint32_t *)dat)[temp_length]), &(((uint32_t *)fifo->buffer)[fifo->head - *length]), (*length - temp_length) * 4);
                    }break;
                }
            }
            fifo->execution &= ~FIFO_READ;                                      // Clear read operation bit
        }

        if(FIFO_READ_AND_CLEAN == flag)                                         // If read and delete FIFO state is selected
        {
            if((FIFO_RESET | FIFO_CLEAR | FIFO_READ) == fifo->execution)        // Reset, clear, and read states coexisting is abnormal
            {
                return_state = FIFO_CLEAR_UNDO;                                 // Clear operation not executed
                break;
            }
            fifo_clear(fifo);
        }
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : FIFO init, assign corresponding buffer
// Description    : *fifo               FIFO object pointer
// Description    : type                FIFO data bit width
// Description    : *buffer_addr        Buffer to assign
// Description    : size                Buffer size
// Return Value   : fifo_state_enum     Operation state
// Usage Example  : fifo_init(&user_fifo, user_buffer, 64);
// Note
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_init (fifo_obj_struct *fifo, fifo_data_type_enum type, void *buffer_addr, uint32_t size)
{
    fifo_state_enum return_state = FIFO_SUCCESS;
    do
    {
        fifo->buffer    = buffer_addr;
        fifo->execution = FIFO_IDLE;
        fifo->type      = type;
        fifo->head      = 0;
        fifo->end       = 0;
        fifo->size      = size;
        fifo->max       = size;
    }while(0);
    return return_state;
}
