/*********************************************************************************************************************
* MSPM0G3507 Open Source Library - a lightweight open source library for official SDK interfaces
* Copyright (c) 2022 SEEKFREE (SeekFree Technology)
*
* This file is part of the MSPM0G3507 Open Source Library
*
* MSPM0G3507 Open Source Library is free software.
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
* File name          : zf_device_imu_interface
* Company name       : Chengdu SeekFree Technology Co., Ltd.
* Version info       : See version file under libraries/doc directory
* Dev environment    : MDK 5.38a
* Target platform    : MSPM0G3507
* Contact            : https://seekfree.taobao.com/
********************************************************************************************************************/

/*********************************************************************************************************************
* The function of this file is to decouple the IMU driver from the platform.
* It provides interfaces for IMU-related operations.
* When porting to a new platform, you only need to implement the interfaces in this file
* and you can directly use all of SeekFree's V4 IMU drivers.
********************************************************************************************************************/

#ifndef _zf_device_imu_interface_h_
#define _zf_device_imu_interface_h_

// zf_common typedef include
#include "zf_common_typedef.h"

#include "ti_msp_dl_config.h"

// This section defines IMU-related interface macros. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define IMU_INTERFACE_SPI           ( 0 )                                       // Use hardware SPI mode
#define IMU_INTERFACE_SOFT_IIC      ( 1 )                                       // Use software IIC mode
#define IMU_INTERFACE_IIC           ( 2 )                                       // Use hardware IIC mode

#define IMU_SPI_INDEX               ( SPI_IMU_INST )
#define IMU_CS_PORT                 ( IMU_PORT )
#define IMU_CS_PIN                  ( IMU_IMU_CS_PIN )
#define IMU_CS(x)                   ((x) ? (DL_GPIO_setPins(IMU_CS_PORT, IMU_CS_PIN)): (DL_GPIO_clearPins(IMU_CS_PORT, IMU_CS_PIN)))

typedef void    (*imu_delay_ms_func             )   (uint32 parameter);

typedef uint8   (*imu_assert_func               )   (uint8 pass, char *str, char *file, int line);

typedef void    (*imu_write_8bit_register_func  )   (uint8 addr, uint8 reg, uint8 data);
typedef void    (*imu_write_8bit_registers_func )   (uint8 addr, uint8 reg, const uint8 *data, uint32 len);
typedef uint8   (*imu_read_8bit_register_func   )   (uint8 addr, uint8 reg);
typedef void    (*imu_read_8bit_registers_func  )   (uint8 addr, uint8 reg, uint8 *data, uint32 len);

typedef struct                                                                  // IMU660RA module abstraction struct
{
    uint8                           interface_type          ;                   // Required interface type

    imu_delay_ms_func               delay_ms                ;                   // Delay interface, ms delay

    imu_assert_func                 assert                  ;                   // Assert interface

    imu_write_8bit_register_func    write_8bit_register     ;                   // Write register
    imu_write_8bit_registers_func   write_8bit_registers    ;                   // Multi-write register
    imu_read_8bit_register_func     read_8bit_register      ;                   // Read register
    imu_read_8bit_registers_func    read_8bit_registers     ;                   // Multi-read register
}imu_interface_obj_struct;

extern imu_interface_obj_struct imu_interface_default_obj;
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section lists all function declarations [ including macro-defined functions ]. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU delay
// Description    : ms              millisecond delay value
// Return Value   : void
// Usage Example  : system_delay_ms(ms);
// Note
//-------------------------------------------------------------------------------------------------------------------
void imu_delay_ms (uint32 ms);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU assert interface macro
// Description    : pass            condition check value
// Description    : *str            message to output when check fails
// Description    : *file           file path info
// Description    : line            line number
// Return Value   : uint8           ZF_NO_ERROR - 0 - OK / !ZF_NO_ERROR - !0 - Error
// Usage Example  : imu_assert(pass, str, file, line);
// Note
//-------------------------------------------------------------------------------------------------------------------
uint8 imu_assert (uint8 pass, char *str, char *file, int line);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU write register 8bit
// Description    : addr            IIC device address, write 0 for SPI mode
// Description    : reg             register address
// Description    : data            data
// Return Value   : void
// Usage Example  : imu_write_8bit_register(addr, reg, data);
// Note
//-------------------------------------------------------------------------------------------------------------------
void imu_write_8bit_register (uint8 addr, uint8 reg, uint8 data);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU write registers 8bit
// Description    : addr            IIC device address, write 0 for SPI mode
// Description    : reg             register address
// Description    : data            data buffer
// Description    : len             data length
// Return Value   : void
// Usage Example  : imu_write_8bit_registers(addr, reg, data, len);
// Note
//-------------------------------------------------------------------------------------------------------------------
void imu_write_8bit_registers (uint8 addr, uint8 reg, const uint8 *data, uint32 len);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU read register 8bit
// Description    : addr            IIC device address, write 0 for SPI mode
// Description    : reg             register address
// Return Value   : uint8           data
// Usage Example  : imu_read_8bit_register(addr, reg);
// Note
//-------------------------------------------------------------------------------------------------------------------
uint8 imu_read_8bit_register (uint8 addr, uint8 reg);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU read registers 8bit
// Description    : addr            IIC device address, write 0 for SPI mode
// Description    : reg             register address
// Description    : data            data buffer
// Description    : len             data length
// Return Value   : void
// Usage Example  : imu_read_8bit_registers(addr, reg, data, len);
// Note
//-------------------------------------------------------------------------------------------------------------------
void imu_read_8bit_registers (uint8 addr, uint8 reg, uint8 *data, uint32 len);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU raw SPI transfer
// Description    : data_out        output data buffer, can be NULL
// Description    : data_in         input data buffer, can be NULL
// Description    : data_len        data length
// Return Value   : void
// Usage Example  : imu_spi_transfer(tx, rx, len);
// Note           : This helper keeps chip-select handling inside the platform
//-------------------------------------------------------------------------------------------------------------------
void imu_spi_transfer (const uint8 *data_out, uint8 *data_in, uint32 data_len);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU interface deinit
// Description    : void
// Return Value   : uint8           0-success 1-failure
// Usage Example  : imu_interface_deinit(addr);
// Note
//-------------------------------------------------------------------------------------------------------------------
uint8 imu_interface_deinit (void);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU interface init, internal call
// Description    : void
// Return Value   : uint8           0-success 1-failure
// Usage Example  : imu_interface_init();
// Note
//-------------------------------------------------------------------------------------------------------------------
uint8 imu_interface_init (void);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#endif
