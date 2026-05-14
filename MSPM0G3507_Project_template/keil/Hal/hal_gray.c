/*******************************************************************************
  * @�ļ�      �� wangming
  * @wechat    :DeepCoderMing
  * @qq      �� 3201935299
  * @���?      �� 2025��05��01��
  * @��Ȩ����  �� �����ο�ѧϰ��δ��������ֹ����
********************************************************************************/
#include "hal_gray.h"
#include "ti_msp_dl_config.h"
#include "ti/driverlib/dl_gpio.h"
#include "ti/driverlib/dl_adc12.h"
#include "mt_flag.h"

/* 8·���Ҷȵ�ADCֵ */
uint16_t LQ_Tracking_Value[8] = {0};

/* ��λ�ж� threshold ���ã�С��Ϊѽ����? */
uint16_t gray_threshold[8] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

/* 12·���Ҷȵ���Ϣ��
 * ע��: ��Ϊ8·ADC+3·IO
 * ��S2,S1,S0ѡ��ͨ��
 */
_gray_state gray_state;
float gray_status = 0, gray_status_backup = 0;
uint32_t gray_status_worse = 0;
char stop_flag = 0;

/* ��S2,S1,S0ѡ��ͨ��(S2,S1,S0)=(0,0,0)~(1,1,1) ��Ӧͨ��1~8 */
void Tracking_IO_Set(unsigned char s2, unsigned char s1, unsigned char s0)
{
    if(s0) DL_GPIO_setPins(Tracking_SO_PORT, Tracking_SO_PIN);
    else DL_GPIO_clearPins(Tracking_SO_PORT, Tracking_SO_PIN);

    if(s1) DL_GPIO_setPins(Tracking_S1_PORT, Tracking_S1_PIN);
    else DL_GPIO_clearPins(Tracking_S1_PORT, Tracking_S1_PIN);

    if(s2) DL_GPIO_setPins(Tracking_S2_PORT, Tracking_S2_PIN);
    else DL_GPIO_clearPins(Tracking_S2_PORT, Tracking_S2_PIN);
}

/* ��ȡ8·���Ҷȵ��ADCֵ */
void gray_8data_read(void)
{
    unsigned char i;
    DL_ADC12_startConversion(ADC12_0_INST);
    for(i = 0; i < 8; i++)
    {
        Tracking_IO_Set(i >> 2, (i >> 1) & 0x01, i & 0x01);  //ѡ��ͨ�� i
        /* �ӳٶ�ȡADCֵ */
        for(volatile unsigned short j = 0; j < 500; j++);
        LQ_Tracking_Value[i] = DL_ADC12_getMemResult(ADC12_0_INST, ADC12_0_ADCMEM_0);
    }

    /* ��λȡ״̬ */
    gray_state.gray.bit1 = LQ_Tracking_Value[0] < gray_threshold[0] ? 1 : 0;
    gray_state.gray.bit2 = LQ_Tracking_Value[1] < gray_threshold[1] ? 1 : 0;
    gray_state.gray.bit3 = LQ_Tracking_Value[2] < gray_threshold[2] ? 1 : 0;
    gray_state.gray.bit4 = LQ_Tracking_Value[3] < gray_threshold[3] ? 1 : 0;
    gray_state.gray.bit5 = LQ_Tracking_Value[4] < gray_threshold[4] ? 1 : 0;
    gray_state.gray.bit6 = LQ_Tracking_Value[5] < gray_threshold[5] ? 1 : 0;
    gray_state.gray.bit7 = LQ_Tracking_Value[6] < gray_threshold[6] ? 1 : 0;
    gray_state.gray.bit8 = LQ_Tracking_Value[7] < gray_threshold[7] ? 1 : 0;

    /* ����ǰ������Ϊλ�� */
    gray_status_backup = gray_status;
    switch(gray_state.state)
    {
        case 0x0001: gray_status = 14; gray_status_worse /= 2; break;
        case 0x0003: gray_status = 13; gray_status_worse /= 2; break;
        case 0x0002: gray_status = 12; gray_status_worse /= 2; break;
        case 0x0006: gray_status = 11; gray_status_worse /= 2; break;
        case 0x0004: gray_status = 10; gray_status_worse /= 2; break;
        case 0x000C: gray_status = 9; gray_status_worse /= 2; break;
        case 0x0008: gray_status = 8; gray_status_worse /= 2; break;
        case 0x0018: gray_status = 7; gray_status_worse /= 2; break;
        case 0x0010: gray_status = 6; gray_status_worse /= 2; break;
        case 0x0030: gray_status = 5; gray_status_worse /= 2; break;
        case 0x0020: gray_status = 4; gray_status_worse /= 2; break;
        case 0x0060: gray_status = 3; gray_status_worse /= 2; break;
        case 0x0040: gray_status = 2; gray_status_worse /= 2; break;
        case 0x00C0: gray_status = 1; gray_status_worse /= 2; break;
        case 0x0080: gray_status = 0; gray_status_worse /= 2; break;
        case 0x0000: gray_status = gray_status_backup; gray_status_worse++; break;
        default:
            gray_status = 0;
            gray_status_worse++;
            break;
    }

    if(gray_status_worse > 25)
    {
        Flag.gray_worse = 1;
    }
    else
    {
        Flag.gray_worse = 0;
    }
}

/* ��ʼ���õĲ�ѹ��ֵ */
void gray_set_threshold(uint16_t* threshold)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        gray_threshold[i] = threshold[i];
    }
}