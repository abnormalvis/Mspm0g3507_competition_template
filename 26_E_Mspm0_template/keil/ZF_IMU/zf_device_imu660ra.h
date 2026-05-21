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
* File name          : zf_device_imu660ra
* Company name       : Chengdu SeekFree Technology Co., Ltd.
* Version info       : See version file under libraries/doc directory
* Dev environment    : MDK 5.38a
* Target platform    : MSPM0G3507
* Contact            : https://seekfree.taobao.com/
********************************************************************************************************************/

/*********************************************************************************************************************
* Hardware configuration: See interface macro definitions in zf_device_imu_interface.h
********************************************************************************************************************/

#ifndef _zf_device_imu660ra_h_
#define _zf_device_imu660ra_h_

// zf_common typedef include
#include "zf_common_typedef.h"

// This section lists currently supported functions
// Search in the source file for the actual definition, then navigate to the corresponding function
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// imu660ra_get_physical_acc                                                    // Read IMU660RA accelerometer physical data
// imu660ra_get_physical_gyro                                                   // Read IMU660RA gyroscope physical data

// imu660ra_get_acc                                                             // Read IMU660RA accelerometer data
// imu660ra_get_gyro                                                            // Read IMU660RA gyroscope data

// imu660ra_set_config                                                          // IMU660RA module config
// imu660ra_init                                                                // Initialize IMU660RA
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section defines IMU660RA-related structs, data, and detailed information. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// IMU660RA_ACC_OUTPUT_RATE_BASE corresponds to the default IMU660RA accelerometer output frequency, which is 50Hz
// Actual frequency may deviate; check the corresponding chip datasheet. The corresponding low to high configurations are:
// IMU660RA_ACC_OUTPUT_RATE_64_DIV      ->  0.78    Hz
// IMU660RA_ACC_OUTPUT_RATE_32_DIV      ->  1.5     Hz
// IMU660RA_ACC_OUTPUT_RATE_16_DIV      ->  3.1     Hz
// IMU660RA_ACC_OUTPUT_RATE_8_DIV       ->  6.25    Hz
// IMU660RA_ACC_OUTPUT_RATE_4_DIV       ->  12.5    Hz
// IMU660RA_ACC_OUTPUT_RATE_2_DIV       ->  25      Hz
// IMU660RA_ACC_OUTPUT_RATE_BASE        ->  50      Hz
// IMU660RA_ACC_OUTPUT_RATE_2_MUL       ->  100     Hz
// IMU660RA_ACC_OUTPUT_RATE_4_MUL       ->  200     Hz
// IMU660RA_ACC_OUTPUT_RATE_8_MUL       ->  400     Hz
// IMU660RA_ACC_OUTPUT_RATE_16_MUL      ->  800     Hz
// IMU660RA_ACC_OUTPUT_RATE_32_MUL      ->  1600    Hz
typedef enum
{
    IMU660RA_ACC_OUTPUT_RATE_64_DIV = 2 ,                                       // Accelerometer output rate 64 div (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_32_DIV     ,                                       // Accelerometer output rate 32 div (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_16_DIV     ,                                       // Accelerometer output rate 16 div (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_8_DIV      ,                                       // Accelerometer output rate 8  div (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_4_DIV      ,                                       // Accelerometer output rate 4  div (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_2_DIV      ,                                       // Accelerometer output rate 2  div (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_BASE       ,                                       // Accelerometer output rate base (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_2_MUL      ,                                       // Accelerometer output rate 2  mul (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_4_MUL      ,                                       // Accelerometer output rate 4  mul (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_8_MUL      ,                                       // Accelerometer output rate 8  mul (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_16_MUL     ,                                       // Accelerometer output rate 16 mul (ACC = Accelerometer)
    IMU660RA_ACC_OUTPUT_RATE_32_MUL     ,                                       // Accelerometer output rate 32 mul (ACC = Accelerometer)
}imu660ra_acc_output_rate_enum;

typedef enum
{
    IMU660RA_ACC_RANGE_SGN_2G       = 2 ,                                       // Accelerometer range +-2 g (g = gravitational acceleration, typically 9.8 m/s^2 as standard)
    IMU660RA_ACC_RANGE_SGN_4G           ,                                       // Accelerometer range +-4 g
    IMU660RA_ACC_RANGE_SGN_8G           ,                                       // Accelerometer range +-8 g
    IMU660RA_ACC_RANGE_SGN_16G          ,                                       // Accelerometer range +-16g
}imu660ra_acc_range_enum;

// IMU660RA_GYRO_OUTPUT_RATE_BASE corresponds to the default IMU660RA gyroscope output frequency, which is 50Hz
// Actual frequency may deviate; check the corresponding chip datasheet. The corresponding low to high configurations are:
// IMU660RA_GYRO_OUTPUT_RATE_2_DIV      -> 25       Hz
// IMU660RA_GYRO_OUTPUT_RATE_BASE       -> 50       Hz
// IMU660RA_GYRO_OUTPUT_RATE_2_MUL      -> 100      Hz
// IMU660RA_GYRO_OUTPUT_RATE_4_MUL      -> 200      Hz
// IMU660RA_GYRO_OUTPUT_RATE_8_MUL      -> 400      Hz
// IMU660RA_GYRO_OUTPUT_RATE_16_MUL     -> 800      Hz
// IMU660RA_GYRO_OUTPUT_RATE_32_MUL     -> 1600     Hz
// IMU660RA_GYRO_OUTPUT_RATE_64_MUL     -> 3200     Hz
typedef enum
{
    IMU660RA_GYRO_OUTPUT_RATE_2_DIV = 7 ,                                       // Gyroscope output rate 2  div (GYRO = Gyroscope)
    IMU660RA_GYRO_OUTPUT_RATE_BASE      ,                                       // Gyroscope output rate base   (GYRO = Gyroscope)
    IMU660RA_GYRO_OUTPUT_RATE_2_MUL     ,                                       // Gyroscope output rate 2  mul (GYRO = Gyroscope)
    IMU660RA_GYRO_OUTPUT_RATE_4_MUL     ,                                       // Gyroscope output rate 4  mul (GYRO = Gyroscope)
    IMU660RA_GYRO_OUTPUT_RATE_8_MUL     ,                                       // Gyroscope output rate 8  mul (GYRO = Gyroscope)
    IMU660RA_GYRO_OUTPUT_RATE_16_MUL    ,                                       // Gyroscope output rate 16 mul (GYRO = Gyroscope)
    IMU660RA_GYRO_OUTPUT_RATE_32_MUL    ,                                       // Gyroscope output rate 32 mul (GYRO = Gyroscope)
    IMU660RA_GYRO_OUTPUT_RATE_64_MUL    ,                                       // Gyroscope output rate 64 mul (GYRO = Gyroscope)
}imu660ra_gyro_output_rate_enum;

typedef enum
{
    IMU660RA_GYRO_RANGE_SGN_125DPS  = 2 ,                                       // Gyroscope range +-125DPS  (DPS = Degree Per Second)
    IMU660RA_GYRO_RANGE_SGN_250DPS      ,                                       // Gyroscope range +-250DPS
    IMU660RA_GYRO_RANGE_SGN_500DPS      ,                                       // Gyroscope range +-500DPS
    IMU660RA_GYRO_RANGE_SGN_1000DPS     ,                                       // Gyroscope range +-1000DPS
    IMU660RA_GYRO_RANGE_SGN_2000DPS     ,                                       // Gyroscope range +-2000DPS
}imu660ra_gyro_range_enum;

#define IMU660RA_TIMEOUT_COUNT              ( 10 )                              // IMU660RA timeout count

#define IMU660RA_ACC_OUTPUT_RATE_DEFAULT    ( IMU660RA_ACC_OUTPUT_RATE_BASE   ) // Default accelerometer init output rate
#define IMU660RA_ACC_RANGE_DEFAULT          ( IMU660RA_ACC_RANGE_SGN_8G       ) // Default accelerometer init range
#define IMU660RA_GYRO_OUTPUT_RATE_DEFAULT   ( IMU660RA_GYRO_OUTPUT_RATE_4_MUL ) // Default gyroscope init output rate
#define IMU660RA_GYRO_RANGE_DEFAULT         ( IMU660RA_GYRO_RANGE_SGN_2000DPS ) // Default gyroscope init range

#define IMU660RA_CONFIG_TYPE_OFFSET         ( 4     )                           // Accumulated shift offset, 16bit start
#define IMU660RA_CONFIG_TYPE_MASK           ( 0x0F  )                           // Same as above, requires appending to config enum, same 4bit, mask is 0xF

#define IMU660RA_CONFIG_VALUE_OFFSET        ( 0     )                           // Accumulated shift offset, 16bit start
#define IMU660RA_CONFIG_VALUE_MASK          ( 0x0F  )                           // Same as above, requires appending to config enum, same 4bit, mask is 0xF

typedef enum                                                                    // Enum: IMU660RA config options. Do not modify.
{
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_64_DIV          = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_64_DIV) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_32_DIV          = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_32_DIV) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_16_DIV          = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_16_DIV) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_8_DIV           = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_8_DIV ) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_4_DIV           = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_4_DIV ) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_2_DIV           = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_2_DIV ) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_BASE            = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_BASE  ) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_2_MUL           = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_2_MUL ) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_4_MUL           = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_4_MUL ) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_8_MUL           = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_8_MUL ) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_16_MUL          = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_16_MUL) ,
    IMU660RA_CONFIG_ACC_OUTPUT_RATE_32_MUL          = ((0x00 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_OUTPUT_RATE_32_MUL) ,

    IMU660RA_CONFIG_ACC_RANGE_SGN_2G                = ((0x01 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_RANGE_SGN_2G    )   ,
    IMU660RA_CONFIG_ACC_RANGE_SGN_4G                = ((0x01 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_RANGE_SGN_4G    )   ,
    IMU660RA_CONFIG_ACC_RANGE_SGN_8G                = ((0x01 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_RANGE_SGN_8G    )   ,
    IMU660RA_CONFIG_ACC_RANGE_SGN_16G               = ((0x01 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_ACC_RANGE_SGN_16G   )   ,

    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_2_DIV          = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_2_DIV ),
    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_BASE           = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_BASE  ),
    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_2_MUL          = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_2_MUL ),
    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_4_MUL          = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_4_MUL ),
    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_8_MUL          = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_8_MUL ),
    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_16_MUL         = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_16_MUL),
    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_32_MUL         = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_32_MUL),
    IMU660RA_CONFIG_GYRO_OUTPUT_RATE_64_MUL         = ((0x02 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_OUTPUT_RATE_64_MUL),

    IMU660RA_CONFIG_GYRO_RANGE_SGN_125DPS           = ((0x03 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_RANGE_SGN_125DPS ) ,
    IMU660RA_CONFIG_GYRO_RANGE_SGN_250DPS           = ((0x03 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_RANGE_SGN_250DPS ) ,
    IMU660RA_CONFIG_GYRO_RANGE_SGN_500DPS           = ((0x03 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_RANGE_SGN_500DPS ) ,
    IMU660RA_CONFIG_GYRO_RANGE_SGN_1000DPS          = ((0x03 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_RANGE_SGN_1000DPS) ,
    IMU660RA_CONFIG_GYRO_RANGE_SGN_2000DPS          = ((0x03 << IMU660RA_CONFIG_TYPE_OFFSET) | IMU660RA_GYRO_RANGE_SGN_2000DPS) ,
}imu660ra_config_item_enum;

typedef enum
{
    IMU660RA_INTERFACE_SPI      ,
    IMU660RA_INTERFACE_SOFT_IIC ,
    IMU660RA_INTERFACE_IIC      ,
}imu660ra_interface_type_enum;

typedef enum
{
    IMU660RA_STATE_NO_ERROR                     ,

    IMU660RA_STATE_DATA_BUFFER_ERROR            ,
    IMU660RA_STATE_INTERFACE_INIT_ERROR         ,
    IMU660RA_STATE_ID_CHECK_ERROR               ,
    IMU660RA_STATE_SELF_CHECK_ERROR             ,
    IMU660RA_STATE_SET_CONFIG_ERROR             ,
    IMU660RA_STATE_SET_ACC_OUTPUT_RATE_ERROR    ,
    IMU660RA_STATE_SET_ACC_RANGE_ERROR          ,
    IMU660RA_STATE_SET_GYRO_OUTPUT_RATE_ERROR   ,
    IMU660RA_STATE_SET_GYRO_RANGE_ERROR         ,
}imu660ra_state_enum;

typedef struct                                                                  // IMU660RA data format model, for storing IMU660RA raw data
{
    int16       x   ;                                                           // X axis raw data
    int16       y   ;                                                           // Y axis raw data
    int16       z   ;                                                           // Z axis raw data
}imu660ra_measurement_data_struct;

typedef struct                                                                  // IMU660RA data format model, for storing IMU660RA physical data
{
    float       x   ;                                                           // X axis physical data
    float       y   ;                                                           // Y axis physical data
    float       z   ;                                                           // Z axis physical data
}imu660ra_physical_data_struct;
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section defines IMU660RA register addresses and internal parameters. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Map IMU660RA internal addresses
#define IMU660RA_DEV_ADDR           ( 0x69 )                                    // 7bit address format. SA0 to GND: 0x68, SA0 to VCC: 0x69. Default on module.
#define IMU660RA_SPI_W              ( 0x00 )
#define IMU660RA_SPI_R              ( 0x80 )

#define IMU660RA_CHIP_ID            ( 0x00 )
#define IMU660RA_PWR_CONF           ( 0x7C )
#define IMU660RA_PWR_CTRL           ( 0x7D )
#define IMU660RA_INIT_CTRL          ( 0x59 )
#define IMU660RA_INIT_DATA          ( 0x5E )
#define IMU660RA_INT_STA            ( 0x21 )
#define IMU660RA_ACC_ADDRESS        ( 0x0C )
#define IMU660RA_GYRO_ADDRESS       ( 0x12 )
#define IMU660RA_ACC_CONF           ( 0x40 )
#define IMU660RA_ACC_RANGE          ( 0x41 )
#define IMU660RA_GYR_CONF           ( 0x42 )
#define IMU660RA_GYR_RANGE          ( 0x43 )
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section lists all function declarations [ including macro-defined functions ]. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read IMU660RA accelerometer physical data
// Description    : *measurement_data       IMU660RA measurement data buffer pointer
// Parameter      : range                   IMU660RA accelerometer range, see imu660ra_acc_range_enum in zf_device_imu660ra.h
// Description    : *physical_data          IMU660RA physical data buffer pointer
// Return Value   : imu660ra_state_enum     IMU660RA state code, see imu660ra_state_enum in zf_device_imu660ra.h
// Usage Example  : imu660ra_get_physical_acc(imu660_data_buffer, range, physical_data);
// Note           : The conversion process considers the range:
//                 Accelerometer range +-2  g   raw data factor 16384  converted to physical quantity in g (gravitational acceleration, typically 9.8 m/s^2 as standard)
//                 Accelerometer range +-4  g   raw data factor 8192   converted to physical quantity in g
//                 Accelerometer range +-8  g   raw data factor 4096   converted to physical quantity in g
//                 Accelerometer range +-16 g   raw data factor 2048   converted to physical quantity in g
//-------------------------------------------------------------------------------------------------------------------
imu660ra_state_enum imu660ra_get_physical_acc (imu660ra_measurement_data_struct *measurement_data, imu660ra_acc_range_enum range,imu660ra_physical_data_struct *physical_data);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read IMU660RA gyroscope physical data
// Description    : *measurement_data       IMU660RA measurement data buffer pointer
// Parameter      : range                   IMU660RA gyroscope range, see imu660ra_gyro_range_enum in zf_device_imu660ra.h
// Description    : *physical_data          IMU660RA physical data buffer pointer
// Return Value   : imu660ra_state_enum     IMU660RA state code, see imu660ra_state_enum in zf_device_imu660ra.h
// Usage Example  : imu660ra_get_physical_gyro(imu660_data_buffer, range, physical_data);
// Note           : The conversion process considers the range:
//                 Gyroscope range +-125  dps    raw data factor 262.4   converted to physical quantity in deg/s
//                 Gyroscope range +-250  dps    raw data factor 131.2   converted to physical quantity in deg/s
//                 Gyroscope range +-500  dps    raw data factor 65.6    converted to physical quantity in deg/s
//                 Gyroscope range +-1000 dps    raw data factor 32.8    converted to physical quantity in deg/s
//                 Gyroscope range +-2000 dps    raw data factor 16.4    converted to physical quantity in deg/s
//-------------------------------------------------------------------------------------------------------------------
imu660ra_state_enum imu660ra_get_physical_gyro (imu660ra_measurement_data_struct *measurement_data, imu660ra_gyro_range_enum range,imu660ra_physical_data_struct *physical_data);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read IMU660RA accelerometer data
// Description    : *measurement_data       IMU660RA measurement data buffer pointer
// Return Value   : imu660ra_state_enum     IMU660RA state code, see imu660ra_state_enum in zf_device_imu660ra.h
// Usage Example  : imu660ra_get_acc(imu660_data_buffer);
// Note
//-------------------------------------------------------------------------------------------------------------------
imu660ra_state_enum imu660ra_get_acc (imu660ra_measurement_data_struct *measurement_data);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read IMU660RA gyroscope data
// Description    : *measurement_data       IMU660RA measurement data buffer pointer
// Return Value   : imu660ra_state_enum     IMU660RA state code, see imu660ra_state_enum in zf_device_imu660ra.h
// Usage Example  : imu660ra_get_gyro(imu660_data_buffer);
// Note
//-------------------------------------------------------------------------------------------------------------------
imu660ra_state_enum imu660ra_get_gyro (imu660ra_measurement_data_struct *measurement_data);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU660RA module config
// Description    : item                    IMU660RA config parameter, defined in imu660ra_config_item_enum enum in zf_device_imu660ra.h
// Return Value   : imu660ra_state_enum     IMU660RA state code, see imu660ra_state_enum in zf_device_imu660ra.h
// Usage Example  : imu660ra_set_config(item);
// Note
//-------------------------------------------------------------------------------------------------------------------
imu660ra_state_enum imu660ra_set_config (imu660ra_config_item_enum item);

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Initialize IMU660RA
// Description    : void
// Return Value   : imu660ra_state_enum     IMU660RA state code, see imu660ra_state_enum in zf_device_imu660ra.h
// Usage Example  : imu660ra_init();
// Note
//-------------------------------------------------------------------------------------------------------------------
imu660ra_state_enum imu660ra_init (void);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern imu660ra_measurement_data_struct acc_data, gyro_data;

#endif
