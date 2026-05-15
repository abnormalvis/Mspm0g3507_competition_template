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

// zf_common header
#include "zf_common_debug.h"

// Include header file
#include "zf_device_imu_interface.h"


#include "hal_delay.h"
// This section defines variables used in this file. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
imu_interface_obj_struct imu_interface_default_obj =
{
    IMU_INTERFACE_SPI,
    imu_delay_ms,
    imu_assert,
    imu_write_8bit_register,
    imu_write_8bit_registers,
    imu_read_8bit_register,
    imu_read_8bit_registers,
};
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// This section implements IMU-related interfaces. Do not modify.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Part Start >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU delay
// Description    : ms              millisecond delay value
// Return Value   : void
// Usage Example  : system_delay_ms(ms);
// Note
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK void imu_delay_ms (uint32 ms)
{
    hal_delay_ms(ms);
}

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
ZF_WEAK uint8 imu_assert (uint8 pass, char *str, char *file, int line)
{
    return (pass ? ZF_NO_ERROR : debug_message_handler(ZF_DEBUG_OUTPUT_LEVEL_DEVICE, (pass), (str), file, line));
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU SPI communication interface
// Description    : *data_out       output data
// Description    : *data_in        input data
// Description    : data_len        data length
// Return Value   : void
// Usage Example  : spi_transfer_8bit_register(data_out, data_in, data_len);
// Note
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK void spi_transfer_8bit_register (const uint8 *data_out, uint8 *data_in, uint32 data_len)
{
    while(data_len --)
    {
        DL_SPI_transmitData8(IMU_SPI_INDEX, *data_out ++);
        while(DL_SPI_isBusy(IMU_SPI_INDEX));
        if(NULL != data_in)
        {
            *data_in ++ = DL_SPI_receiveData8(IMU_SPI_INDEX);
            while(DL_SPI_isBusy(IMU_SPI_INDEX));
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU write register 8bit
// Description    : addr            IIC device address, write 0 for SPI mode
// Description    : reg             register address
// Description    : data            data
// Return Value   : void
// Usage Example  : imu_write_8bit_register(addr, reg, data);
// Note
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK void imu_write_8bit_register (uint8 addr, uint8 reg, uint8 data)
{
#if (IMU_INTERFACE_SPI == IMU_INTERFACE_TYPE)
    IMU_CS(0);

    DL_SPI_transmitData8(IMU_SPI_INDEX, reg);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));
    DL_SPI_receiveData8(IMU_SPI_INDEX);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));

    DL_SPI_transmitData8(IMU_SPI_INDEX, data);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));
    DL_SPI_receiveData8(IMU_SPI_INDEX);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));

    IMU_CS(1);
#elif (IMU_INTERFACE_SOFT_IIC == IMU_INTERFACE_TYPE)
    soft_iic_write_8bit_register(IMU_SOFT_IIC_INDEX, addr, reg, data);
#elif (IMU_INTERFACE_IIC == IMU_INTERFACE_TYPE)
    iic_write_8bit_register(IMU_IIC_INDEX, addr, reg, data);
#endif
}

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
ZF_WEAK void imu_write_8bit_registers (uint8 addr, uint8 reg, const uint8 *data, uint32 len)
{
#if (IMU_INTERFACE_SPI == IMU_INTERFACE_TYPE)
    IMU_CS(0);

    DL_SPI_transmitData8(IMU_SPI_INDEX, reg);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));
    DL_SPI_receiveData8(IMU_SPI_INDEX);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));

    uint32 send = 0;
    while(len > send)
    {
        DL_SPI_transmitData8(IMU_SPI_INDEX, *(data + send));
        while(DL_SPI_isBusy(IMU_SPI_INDEX));
        DL_SPI_receiveData8(IMU_SPI_INDEX);
        while(DL_SPI_isBusy(IMU_SPI_INDEX));
        send ++;
    }
    IMU_CS(1);
#elif (IMU_INTERFACE_SOFT_IIC == IMU_INTERFACE_TYPE)
    soft_iic_write_8bit_registers(IMU_SOFT_IIC_INDEX, addr, reg, data, len);
#elif (IMU_INTERFACE_IIC == IMU_INTERFACE_TYPE)
    iic_write_8bit_registers(IMU_IIC_INDEX, addr, reg, data, len);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU read register 8bit
// Description    : addr            IIC device address, write 0 for SPI mode
// Description    : reg             register address
// Return Value   : uint8           data
// Usage Example  : imu_read_8bit_register(addr, reg);
// Note
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK uint8 imu_read_8bit_register (uint8 addr, uint8 reg)
{
#if (IMU_INTERFACE_SPI == IMU_INTERFACE_TYPE)
    uint8 data = 0;
    IMU_CS(0);

    DL_SPI_transmitData8(IMU_SPI_INDEX, reg);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));
    DL_SPI_receiveData8(IMU_SPI_INDEX);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));

    DL_SPI_transmitData8(IMU_SPI_INDEX, 0xFF);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));
    data = DL_SPI_receiveData8(IMU_SPI_INDEX);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));

    IMU_CS(1);
    return data;
#elif (IMU_INTERFACE_SOFT_IIC == IMU_INTERFACE_TYPE)
    return soft_iic_read_8bit_register(IMU_SOFT_IIC_INDEX, addr, reg);
#elif (IMU_INTERFACE_IIC == IMU_INTERFACE_TYPE)
    return iic_read_8bit_register(IMU_IIC_INDEX, addr, reg);
#endif
}

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
ZF_WEAK void imu_read_8bit_registers (uint8 addr, uint8 reg, uint8 *data, uint32 len)
{
#if (IMU_INTERFACE_SPI == IMU_INTERFACE_TYPE)
    IMU_CS(0);

    DL_SPI_transmitData8(IMU_SPI_INDEX, reg);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));
    DL_SPI_receiveData8(IMU_SPI_INDEX);
    while(DL_SPI_isBusy(IMU_SPI_INDEX));

    uint32 read = 0;
    while(len > read)
    {
        DL_SPI_transmitData8(IMU_SPI_INDEX, 0xFF);
        while(DL_SPI_isBusy(IMU_SPI_INDEX));
        *(data + read) = DL_SPI_receiveData8(IMU_SPI_INDEX);
        while(DL_SPI_isBusy(IMU_SPI_INDEX));

        read ++;
    }
    IMU_CS(1);
#elif (IMU_INTERFACE_SOFT_IIC == IMU_INTERFACE_TYPE)
    soft_iic_read_8bit_registers(IMU_SOFT_IIC_INDEX, addr, reg, data, len);
#elif (IMU_INTERFACE_IIC == IMU_INTERFACE_TYPE)
    iic_read_8bit_registers(IMU_IIC_INDEX, addr, reg, data, len);
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU interface deinit
// Description    : void
// Return Value   : uint8           0-success 1-failure
// Usage Example  : imu_interface_deinit(addr);
// Note
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK uint8 imu_interface_deinit (void)
{
    uint8 return_state = 1;
    do
    {
        return_state = 0;
    }while(0);
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : IMU interface init, internal call
// Description    : void
// Return Value   : uint8           0-success 1-failure
// Usage Example  : imu_interface_init();
// Note
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK uint8 imu_interface_init (void)
{
    uint8 return_state = 1;
    do
    {
        return_state = 0;
    }while(0);
    return return_state;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Part   End <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
