#include "move_filter.h"

void move_filter_calc(move_filter_struct *move_filter, float new_data)
{
    move_filter->data_sum = move_filter->data_sum + new_data - move_filter->data_buffer[move_filter->index];
    move_filter->data_average = move_filter->data_sum / move_filter->buffer_size;

    move_filter->data_buffer[move_filter->index] = new_data;
    move_filter->index++;
    if (move_filter->buffer_size <= move_filter->index)
    {
        move_filter->index = 0;
    }
}

void move_filter_init(move_filter_struct *move_filter)
{
    uint8_t i;
    move_filter->data_average = 0;
    move_filter->data_sum     = 0;
    move_filter->index        = 0;
    move_filter->buffer_size  = MOVE_AVERAGE_SIZE;

    for (i = 0; i < move_filter->buffer_size; i++)
    {
        move_filter->data_buffer[i] = 0;
    }
}

void lowpass_filter_init(lowpass_filter_struct *f, float alpha)
{
    f->alpha = alpha;
    f->output = 0.0f;
}

float lowpass_filter_calc(lowpass_filter_struct *f, float input)
{
    f->output += f->alpha * (input - f->output);
    return f->output;
}

void lowpass_filter_set_alpha(lowpass_filter_struct *f, float alpha)
{
    f->alpha = alpha;
}
