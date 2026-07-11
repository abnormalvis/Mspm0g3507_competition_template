#include "hal_gray.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_adc12.h>

uint16_t LQ_Tracking_Value[GRAY_CHANNEL_COUNT] = {0};
uint16_t gray_threshold[GRAY_CHANNEL_COUNT]    = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500,
#ifdef GRAY_SENSOR_16CH
                                                   50, 50, 50, 50, 50, 50, 50, 50
#endif
                                                  };

_gray_state gray_state;
float       gray_status        = 0;
float       gray_status_backup = 0;
uint32_t    gray_status_worse  = 0;
char        stop_flag          = 0;

static uint8_t gray_worse_flag = 0;

uint8_t gray_is_worse(void) { return gray_worse_flag; }

/* ---- one-time hardware init (16ch: S4 already configured by syscfg; hook for future) ---- */
void gray_init(void)
{
#ifdef GRAY_SENSOR_16CH
    /* S4 (PB4) is already initialized by SYSCFG_DL_GPIO_init()
     * via TRACK_S4_IOMUX. No extra config needed. */
#endif
}

static void Tracking_IO_Set(unsigned char sel_mask)
{
#ifdef GRAY_SENSOR_16CH
    /* 4-bit MUX: S1(bit0) S2(bit1) S3(bit2) S4(bit3) */
    if (sel_mask & 0x01) DL_GPIO_setPins  (TRACK_S1_PORT, TRACK_S1_PIN);
    else                  DL_GPIO_clearPins(TRACK_S1_PORT, TRACK_S1_PIN);

    if (sel_mask & 0x02) DL_GPIO_setPins  (TRACK_S2_PORT, TRACK_S2_PIN);
    else                  DL_GPIO_clearPins(TRACK_S2_PORT, TRACK_S2_PIN);

    if (sel_mask & 0x04) DL_GPIO_setPins  (TRACK_S3_PORT, TRACK_S3_PIN);
    else                  DL_GPIO_clearPins(TRACK_S3_PORT, TRACK_S3_PIN);

    if (sel_mask & 0x08) DL_GPIO_setPins  (TRACK_S4_PORT, TRACK_S4_PIN);
    else                  DL_GPIO_clearPins(TRACK_S4_PORT, TRACK_S4_PIN);
#else
    /* 3-bit MUX (original 8ch): S1(bit0) S2(bit1) S3(bit2) */
    if (sel_mask & 0x01) DL_GPIO_setPins  (TRACK_S1_PORT, TRACK_S1_PIN);
    else                  DL_GPIO_clearPins(TRACK_S1_PORT, TRACK_S1_PIN);

    if (sel_mask & 0x02) DL_GPIO_setPins  (TRACK_S2_PORT, TRACK_S2_PIN);
    else                  DL_GPIO_clearPins(TRACK_S2_PORT, TRACK_S2_PIN);

    if (sel_mask & 0x04) DL_GPIO_setPins  (TRACK_S3_PORT, TRACK_S3_PIN);
    else                  DL_GPIO_clearPins(TRACK_S3_PORT, TRACK_S3_PIN);
#endif
}

#ifdef GRAY_SENSOR_16CH
/* ---- 16ch single-channel read: 5-sample, discard first 3, scale 0-4095 -> 0-100 ---- */
static uint16_t gray_read_channel_16ch(unsigned char ch)
{
    uint16_t data;
    uint16_t sum = 0;
    const unsigned char NUM_SAMPLES     = 5;
    const unsigned char DISCARD_SAMPLES = 3;

    for (unsigned char i = 0; i < NUM_SAMPLES; i++)
    {
        Tracking_IO_Set(ch);  /* ch = 0-15, 4-bit MUX select */
        for (volatile unsigned short j = 0; j < 500; j++);
        DL_ADC12_startConversion(ADC_track_INST);
        while (!(DL_ADC12_getRawInterruptStatus(ADC_track_INST,
                    DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED)));
        data = DL_ADC12_getMemResult(ADC_track_INST, ADC_track_ADCMEM_0);
        DL_ADC12_clearInterruptStatus(ADC_track_INST,
            DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);

        /* scale 0-4095 to 0-100 */
        data = (uint16_t)((float)data * 0.02442f);
        if (data > 100) data = 100;

        if (i >= DISCARD_SAMPLES)
            sum += data;
    }
    return sum / (NUM_SAMPLES - DISCARD_SAMPLES);
}
#else
/* ---- 8ch single-channel read (original behavior) ---- */
static uint16_t gray_read_channel_8ch(unsigned char ch)
{
    Tracking_IO_Set(ch);
    for (volatile unsigned short j = 0; j < 500; j++);
    DL_ADC12_startConversion(ADC_track_INST);
    while (!(DL_ADC12_getRawInterruptStatus(ADC_track_INST,
                DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED)));
    uint16_t val = DL_ADC12_getMemResult(ADC_track_INST, ADC_track_ADCMEM_0);
    DL_ADC12_clearInterruptStatus(ADC_track_INST,
        DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);
    return val;
}
#endif

/* ---- uni ed sensor read: all channels -> thresholds -> position lookup ---- */
void gray_read(void)
{
    unsigned char i;

    /* Step 1: read all channels */
    for (i = 0; i < GRAY_CHANNEL_COUNT; i++)
    {
#ifdef GRAY_SENSOR_16CH
        LQ_Tracking_Value[i] = gray_read_channel_16ch(i);
#else
        LQ_Tracking_Value[i] = gray_read_channel_8ch(i);
#endif
    }

    /* Step 2: threshold -> bit pattern (value < threshold = black = line detected) */
    gray_state.state = 0;
    for (i = 0; i < GRAY_CHANNEL_COUNT; i++)
    {
        if (LQ_Tracking_Value[i] < gray_threshold[i])
            gray_state.state |= ((uint16_t)1 << i);
    }

    /* Step 3: lookup-table position calculation */
    gray_status_backup = gray_status;

#ifdef GRAY_SENSOR_16CH
    switch (gray_state.state)
    {
        case 0x0001: gray_status = 30; gray_status_worse /= 2; break;
        case 0x0003: gray_status = 29; gray_status_worse /= 2; break;
        case 0x0002: gray_status = 28; gray_status_worse /= 2; break;
        case 0x0006: gray_status = 27; gray_status_worse /= 2; break;
        case 0x0004: gray_status = 26; gray_status_worse /= 2; break;
        case 0x000C: gray_status = 25; gray_status_worse /= 2; break;
        case 0x0008: gray_status = 24; gray_status_worse /= 2; break;
        case 0x0018: gray_status = 23; gray_status_worse /= 2; break;
        case 0x0010: gray_status = 22; gray_status_worse /= 2; break;
        case 0x0030: gray_status = 21; gray_status_worse /= 2; break;
        case 0x0020: gray_status = 20; gray_status_worse /= 2; break;
        case 0x0060: gray_status = 19; gray_status_worse /= 2; break;
        case 0x0040: gray_status = 18; gray_status_worse /= 2; break;
        case 0x00C0: gray_status = 17; gray_status_worse /= 2; break;
        case 0x0080: gray_status = 16; gray_status_worse /= 2; break;
        case 0x0180: gray_status = 15; gray_status_worse /= 2; break;
        case 0x0100: gray_status = 14; gray_status_worse /= 2; break;
        case 0x0300: gray_status = 13; gray_status_worse /= 2; break;
        case 0x0200: gray_status = 12; gray_status_worse /= 2; break;
        case 0x0600: gray_status = 11; gray_status_worse /= 2; break;
        case 0x0400: gray_status = 10; gray_status_worse /= 2; break;
        case 0x0C00: gray_status =  9; gray_status_worse /= 2; break;
        case 0x0800: gray_status =  8; gray_status_worse /= 2; break;
        case 0x1800: gray_status =  7; gray_status_worse /= 2; break;
        case 0x1000: gray_status =  6; gray_status_worse /= 2; break;
        case 0x3000: gray_status =  5; gray_status_worse /= 2; break;
        case 0x2000: gray_status =  4; gray_status_worse /= 2; break;
        case 0x6000: gray_status =  3; gray_status_worse /= 2; break;
        case 0x4000: gray_status =  2; gray_status_worse /= 2; break;
        case 0xC000: gray_status =  1; gray_status_worse /= 2; break;
        case 0x8000: gray_status =  0; gray_status_worse /= 2; break;
        case 0x0000: gray_status = gray_status_backup; gray_status_worse++; break;
        default:     gray_status = 0; gray_status_worse++; break;
    }
#else
    switch (gray_state.state)
    {
        case 0x0001: gray_status = 14; gray_status_worse /= 2; break;
        case 0x0003: gray_status = 13; gray_status_worse /= 2; break;
        case 0x0002: gray_status = 12; gray_status_worse /= 2; break;
        case 0x0006: gray_status = 11; gray_status_worse /= 2; break;
        case 0x0004: gray_status = 10; gray_status_worse /= 2; break;
        case 0x000C: gray_status =  9; gray_status_worse /= 2; break;
        case 0x0008: gray_status =  8; gray_status_worse /= 2; break;
        case 0x0018: gray_status =  7; gray_status_worse /= 2; break;
        case 0x0010: gray_status =  6; gray_status_worse /= 2; break;
        case 0x0030: gray_status =  5; gray_status_worse /= 2; break;
        case 0x0020: gray_status =  4; gray_status_worse /= 2; break;
        case 0x0060: gray_status =  3; gray_status_worse /= 2; break;
        case 0x0040: gray_status =  2; gray_status_worse /= 2; break;
        case 0x00C0: gray_status =  1; gray_status_worse /= 2; break;
        case 0x0080: gray_status =  0; gray_status_worse /= 2; break;
        case 0x0000: gray_status = gray_status_backup; gray_status_worse++; break;
        default:     gray_status = 0; gray_status_worse++; break;
    }
#endif

    gray_worse_flag = (gray_status_worse > 25) ? 1 : 0;
}

/* ---- backward-compat: original 8ch entry point ---- */
void gray_8data_read(void)
{
    gray_read();
}

void gray_set_threshold(uint16_t *threshold)
{
    uint8_t i;
    for (i = 0; i < GRAY_CHANNEL_COUNT; i++)
        gray_threshold[i] = threshold[i];
}
