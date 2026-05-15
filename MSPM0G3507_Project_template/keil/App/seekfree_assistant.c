#include "seekfree_assistant.h"
#include "hal_uart.h"
#include "hal_vofa.h"


static fifo_obj_struct  seekfree_assistant_fifo;
static uint8_t        seekfree_assistant_buffer[SEEKFREE_ASSISTANT_BUFFER_SIZE];                                  // Data storage buffer
float               seekfree_assistant_parameter[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT] = {0};                  // Parameters received from host computer
vuint8_t              seekfree_assistant_parameter_update_flag[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT] = {0};      // Parameter update flags
seekfree_assistant_oscilloscope_struct          seekfree_assistant_oscilloscope_data;                         // Oscilloscope data

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Assistant string transfer function
// Description    : *buff           Address of data to send
// Description    : length          Length to send
// Return Value   : uint32_t        Remaining unsent data length
// Usage Example
//-------------------------------------------------------------------------------------------------------------------
 void seekfree_assistant_transfer (const uint8_t *buff, uint32_t length)
{

    UART1_send(buff, length);

}

fifo_obj_struct                 debug_uart_fifo;
//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Read debug ring buffer data
// Description    : *buff       Data buffer pointer for storing read data
// Description    : len         Length to read
// Return Value   : uint32_t      Actual length of data read
// Usage Example
// Note           : Requires DEBUG_UART_USE_INTERRUPT macro definition to be enabled for use
//-------------------------------------------------------------------------------------------------------------------
uint32_t debug_read_ring_buffer (uint8_t *buff, uint32_t len)
{
    fifo_read_buffer(&debug_uart_fifo, buff, &len, FIFO_READ_AND_CLEAN);
    return len;
}


//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Assistant byte data receive function
// Description    : *buff           Address to store received data
// Description    : length          Maximum length to receive
// Return Value   : uint32_t        Length of received data
// Usage Example
//-------------------------------------------------------------------------------------------------------------------
uint32_t seekfree_assistant_receive (uint8_t *buff, uint32_t length)
{
    uint32_t len = 0;

    len = debug_read_ring_buffer(buff, length);

    return len;
}



//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Assistant checksum function
// Description    : *buffer         Address of data to checksum
// Description    : length          Checksum length
// Return Value   : uint8_t         Checksum value
// Usage Example
//-------------------------------------------------------------------------------------------------------------------
static uint8_t seekfree_assistant_sum (uint8_t *buffer, uint32_t length)
{
    uint8_t temp_sum = 0;

    while(length--)
    {
        temp_sum += *buffer++;
    }

    return temp_sum;
}


//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Assistant init
// Description
// Return Value   : void
// Usage Example
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_init (void)
{
    fifo_init(&seekfree_assistant_fifo, FIFO_DATA_8BIT, seekfree_assistant_buffer, SEEKFREE_ASSISTANT_BUFFER_SIZE);
}

//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Assistant byte data receive and process function
// Description    : void
// Return Value   : void
// Usage Example  : This function just needs to be placed in the main loop or PIT interrupt
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_data_analysis (void)
{
    uint8_t  temp_sum;
    uint32_t read_length;
    seekfree_assistant_parameter_struct *receive_packet;

    // Using uint32_t definition to ensure 4-byte alignment
    uint32_t  temp_buffer[SEEKFREE_ASSISTANT_BUFFER_SIZE / 4];

    // Try to read data. The underlying transport may read from the interrupt ring buffer directly
    read_length = seekfree_assistant_receive((uint8_t *)temp_buffer, SEEKFREE_ASSISTANT_BUFFER_SIZE);

    if(read_length)
    {
        // Write the read data into FIFO for protocol processing
        fifo_write_buffer(&seekfree_assistant_fifo, (uint8_t *)temp_buffer, read_length);

        // Forward raw bytes to VOFA for JustFloat serial plotting (lightweight, no checksum)
        uint8_t *p = (uint8_t *)temp_buffer;
        for(uint32_t i = 0; i < read_length; i++)
        {
            vofa_uart_rx_callback(p[i]);
        }
    }


    while(sizeof(seekfree_assistant_parameter_struct) <= fifo_used(&seekfree_assistant_fifo))
    {
        read_length = sizeof(seekfree_assistant_parameter_struct);
        fifo_read_buffer(&seekfree_assistant_fifo, (uint8_t *)temp_buffer, &read_length, FIFO_READ_ONLY);

        if(SEEKFREE_ASSISTANT_RECEIVE_HEAD != ((uint8_t *)temp_buffer)[0])
        {
            // No frame header found, discard one byte from FIFO
            read_length = 1;
        }
        else
        {
            // Frame header found
            receive_packet = (seekfree_assistant_parameter_struct *)temp_buffer;
            temp_sum = receive_packet->check_sum;
            receive_packet->check_sum = 0;
            if(temp_sum == seekfree_assistant_sum((uint8_t *)temp_buffer, sizeof(seekfree_assistant_parameter_struct)))
            {
                // Checksum passed, save parameter
                seekfree_assistant_parameter[receive_packet->channel - 1] = receive_packet->data;
                seekfree_assistant_parameter_update_flag[receive_packet->channel - 1] = 1;
            }
            else
            {
                read_length = 1;
            }
        }


        // Remove used data
        fifo_read_buffer(&seekfree_assistant_fifo, (uint8_t *)temp_buffer, &read_length, FIFO_READ_AND_CLEAN);
    }
}
//-------------------------------------------------------------------------------------------------------------------
// Function Name  : Assistant oscilloscope send function
// Description    : *seekfree_assistant_oscilloscope  Oscilloscope data struct
// Return Value   : void
// Usage Example  : seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_oscilloscope_send (seekfree_assistant_oscilloscope_struct *seekfree_assistant_oscilloscope)
{
    uint8_t packet_size;

    // Sanitize channel count
    seekfree_assistant_oscilloscope->channel_num &= 0x0f;


    // Frame header
    seekfree_assistant_oscilloscope->head         = SEEKFREE_ASSISTANT_SEND_HEAD;

    // Fill length info
    packet_size                         = sizeof(seekfree_assistant_oscilloscope_struct) - (SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT - seekfree_assistant_oscilloscope->channel_num) * 4;
    seekfree_assistant_oscilloscope->length       = packet_size;

    // Fill function and channel count
    seekfree_assistant_oscilloscope->channel_num |= SEEKFREE_ASSISTANT_CAMERA_OSCILLOSCOPE;

    // Fill checksum
    seekfree_assistant_oscilloscope->check_sum    = 0;
    seekfree_assistant_oscilloscope->check_sum    = seekfree_assistant_sum((uint8_t *)seekfree_assistant_oscilloscope, packet_size);

    // Before calling this function, the user layer should write the data to send into seekfree_assistant_oscilloscope_data.data[]

    seekfree_assistant_transfer((const uint8_t *)seekfree_assistant_oscilloscope, packet_size);
}