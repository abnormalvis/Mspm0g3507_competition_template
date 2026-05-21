#include "track.h"
uint8_t track_value;
float track_depart_level;//偏差程度值
uint8_t turn_flag;//用于开启转向
uint8_t pre_turn_flag;
//转向次数
uint8_t turn_count;
//这里的左右指的是黑线所处位置
void Update_Tarck_Depart_Level()
{
    switch(track_value)
    {
        // 一位0的情况（偏右为负，偏左为正）
        case 127: track_depart_level = -3.0; break;  // 极右
        case 191: track_depart_level = -2.5; break;  // 偏右
        case 223: track_depart_level = -1.5; break;  // 偏右
        case 239: track_depart_level = -0.5; break;  // 微右
        case 247: track_depart_level =  0.5; break;  // 微左
        case 251: track_depart_level =  1.5; break;  // 偏左
        case 253: track_depart_level =  2.5; break;  // 偏左
        case 254: track_depart_level =  3.0; break;  // 极左

        // 两位0的情况（中间值为0）
        case 63:  track_depart_level = -3.0; break;  // 极右
        case 159: track_depart_level = -2.0; break;  // 偏右
        case 207: track_depart_level = -1.0; break;  // 偏右
        case 231: track_depart_level =  0.0; break;  // 居中（强制要求）
        case 243: track_depart_level =  1.0; break;  // 偏左
        case 249: track_depart_level =  2.0; break;  // 偏左
        case 252: track_depart_level =  3.0; break;  // 极左

        default:
            // 处理其他情况（如三位0或全1）
            track_depart_level = 0.0;
            break;
    }
}

//转向标志
void Turn()
{
    switch(track_value)
    {
//			//三个0情况
//        case 223:turn_flag = 1;break;//1111 0010
//        case 239:turn_flag = 1;break;//1110 1100
//        case 247:  break;//1110 1010
//        case 254:  break;//1111 0010
//				case 254:  break;//1111 1000
//				case 254:  break;//1110 1001
//				case 254:  break;//1111 1110
//				case 254:  break;//1101 1010
			//1100 0000
			//1110 0000
			//1111 0000
			case 192:pre_turn_flag = 1;break;
			case 224:pre_turn_flag = 1;break;
			case 240:pre_turn_flag = 1;break;
    }
}

uint8_t Gray_Serial_Read()
{
	uint8_t ret = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		/* 输出时钟下降沿 */
		DL_GPIO_clearPins(TRACK_CLK_PORT, TRACK_CLK_Pin);
		delay_us(2);
		//避免GPIO翻转过快导致反应不及时
		ret |= (DL_GPIO_readPins(TRACK_DAT_PORT, TRACK_DAT_Pin)==0?0:1) << i;

		/* 输出时钟上升沿,让传感器更新数据*/
		DL_GPIO_setPins(TRACK_CLK_PORT, TRACK_CLK_Pin);
	
		/* 延迟需要在5us左右 */
		delay_us(5);
	}
	
	return ret;
}