#include "OLED.h"
#include "OLED_Font.h"
#include "Delay.h"

#define I2C_TIMEOUT_MS  (10)

//OLED的显存
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127	

// 封装延时接口，统一调用
void delay_ms(uint32_t ms)
{
    Delay_ms(ms);
}

// ========== I2C相关函数（保留MSPM0硬件操作） ==========
static int mspm0_i2c_disable(void)
{
    DL_I2C_reset(I2C_OLED_INST);
    DL_GPIO_initDigitalOutput(GPIO_I2C_OLED_IOMUX_SCL);
    DL_GPIO_initDigitalInputFeatures(GPIO_I2C_OLED_IOMUX_SDA,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_clearPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
    DL_GPIO_enableOutput(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
    return 0;
}

static int mspm0_i2c_enable(void)
{
    DL_I2C_reset(I2C_OLED_INST);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_OLED_IOMUX_SDA,
        GPIO_I2C_OLED_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_OLED_IOMUX_SCL,
        GPIO_I2C_OLED_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_OLED_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_OLED_IOMUX_SCL);
    DL_I2C_enablePower(I2C_OLED_INST);
    SYSCFG_DL_I2C_OLED_init();
    return 0;
}

void oled_i2c_sda_unlock(void)
{
    uint8_t cycleCnt = 0;
    mspm0_i2c_disable();
    do
    {
        DL_GPIO_clearPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
        delay_ms(1);
        DL_GPIO_setPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
        delay_ms(1);

        if(DL_GPIO_readPins(GPIO_I2C_OLED_SDA_PORT, GPIO_I2C_OLED_SDA_PIN))
            break;
    }while(++cycleCnt < 100);
    mspm0_i2c_enable();
}

// ========== 替换为STM32版本的核心函数 ==========
/**
  * @brief  OLED写命令（STM32版本移植）
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
    unsigned char ptr[2];
    uint32_t start, cur;

    ptr[0] = 0x00;  // 写命令
    ptr[1] = Command;

    get_clock_ms(&start);

    DL_I2C_fillControllerTXFIFO(I2C_OLED_INST, ptr, 2);
    DL_I2C_clearInterruptStatus(I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);
    while (!(DL_I2C_getControllerStatus(I2C_OLED_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_OLED_INST, 0x3C, DL_I2C_CONTROLLER_DIRECTION_TX, 2);

    while (!DL_I2C_getRawInterruptStatus(I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE))
    {
        get_clock_ms(&cur);
        if(cur >= (start + I2C_TIMEOUT_MS))
        {
            oled_i2c_sda_unlock();
            break;
        }
    }
}

/**
  * @brief  OLED写数据（STM32版本移植）
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
    unsigned char ptr[2];
    uint32_t start, cur;

    ptr[0] = 0x40;  // 写数据
    ptr[1] = Data;

    get_clock_ms(&start);

    DL_I2C_fillControllerTXFIFO(I2C_OLED_INST, ptr, 2);
    DL_I2C_clearInterruptStatus(I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);
    while (!(DL_I2C_getControllerStatus(I2C_OLED_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_OLED_INST, 0x3C, DL_I2C_CONTROLLER_DIRECTION_TX, 2);

    while (!DL_I2C_getRawInterruptStatus(I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE))
    {
        get_clock_ms(&cur);
        if(cur >= (start + I2C_TIMEOUT_MS))
        {
            oled_i2c_sda_unlock();
            break;
        }
    }
}

/**
  * @brief  OLED设置光标位置（STM32版本移植，替换原有Set_Pos）
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);					//设置Y位置
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
    OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

// 兼容原有函数名
#define OLED_Set_Pos(Y, X) OLED_SetCursor(Y, X)

/**
  * @brief  OLED清屏（STM32版本移植，替换原有Clear）
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for(i = 0; i < 128; i++)
        {
            OLED_WriteData(0x00);
        }
    }
}

/**
  * @brief  OLED显示一个字符（STM32版本移植，修复字体数组命名问题）
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
    uint8_t i;
    uint8_t c = Char - ' '; // 计算字符在字体数组中的索引
    
    // 边界检查：防止字符超出ASCII可见范围
    if (c > 95) {
        c = 0; // 超出范围显示空格
    }
    
    // 设置光标位置在上半部分（16像素高的字符，分两行显示）
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		
    for (i = 0; i < 8; i++)
    {
        // 使用原代码的 asc2_1608 字体数组（16x8）
        OLED_WriteData(asc2_1608[c][i]);			
    }
    
    // 设置光标位置在下半部分
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	
    for (i = 0; i < 8; i++)
    {
        // 修复：i+8 改为 i，因为 asc2_1608[c][8~15] 是下半部分
        OLED_WriteData(asc2_1608[c][i + 8]);		
    }
}
/**
  * @brief  OLED次方函数（STM32版本移植，替换原有oled_pow）
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）（STM32版本移植）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)							
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十进制，带符号数）（STM32版本移植，替换原有版本）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)							
    {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十六进制，正数）（新增STM32版本函数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++)							
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
  * @brief  OLED显示数字（二进制，正数）（新增STM32版本函数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)							
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}

/**
  * @brief  OLED显示字符串（STM32版本移植）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

// ========== 保留原有功能函数（非重复部分） ==========
// 反显函数
void OLED_ColorTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_WriteCommand(0xA6);//正常显示
    }
    if(i==1)
    {
        OLED_WriteCommand(0xA7);//反色显示
    }
}

// 屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_WriteCommand(0xC8);//正常显示
        OLED_WriteCommand(0xA1);
    }
    if(i==1)
    {
        OLED_WriteCommand(0xC0);//反转显示
        OLED_WriteCommand(0xA0);
    }
}

// 开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WriteCommand(0X8D);
    OLED_WriteCommand(0X14);
    OLED_WriteCommand(0XAF);
}

// 关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WriteCommand(0X8D);
    OLED_WriteCommand(0X10);
    OLED_WriteCommand(0XAE);
}

/**
  * @brief  OLED初始化（STM32版本移植，替换原有版本）
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
    // 检查并解锁I2C总线
    if(DL_I2C_getSDAStatus(I2C_OLED_INST) == DL_I2C_CONTROLLER_SDA_LOW)
        oled_i2c_sda_unlock();

    delay_ms(200);	//上电延时
	
    OLED_WriteCommand(0xAE);	//关闭显示
	
    OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0x80);
	
    OLED_WriteCommand(0xA8);	//设置多路复用率
    OLED_WriteCommand(0x3F);
	
    OLED_WriteCommand(0xD3);	//设置显示偏移
    OLED_WriteCommand(0x00);
	
    OLED_WriteCommand(0x40);	//设置显示开始行
	
    OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
    OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

    OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
    OLED_WriteCommand(0x12);
	
    OLED_WriteCommand(0x81);	//设置对比度控制
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);	//设置预充电周期
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

    OLED_WriteCommand(0xA6);	//设置正常/倒转显示

    OLED_WriteCommand(0x8D);	//设置充电泵
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);	//开启显示
		
    OLED_Clear();				//OLED清屏
}

void OLED_ShowChess(uint8_t Pos){
    uint8_t i[9] = {0};
    i[Pos] = 1;
    OLED_ShowNum(1, 1, i[2], 1);
    OLED_ShowNum(1, 3, i[5], 1);
    OLED_ShowNum(1, 5, i[8], 1);
    OLED_ShowNum(2, 1, i[1], 1);
    OLED_ShowNum(2, 3, i[4], 1);
    OLED_ShowNum(2, 5, i[7], 1);
    OLED_ShowNum(3, 1, i[0], 1);
    OLED_ShowNum(3, 3, i[3], 1);
    OLED_ShowNum(3, 5, i[6], 1);
}

void OLED_ShowChess_pro(const int p[9]){
    char i[9];
    for(int j = 0; j < 9; j++){
        if(p[j] == -1){
            i[j] = '-';
        }else if (p[j] == 1) {
            i[j] = 'B';
        }else if (p[j] == 0) {
            i[j] = 'W';
        }
    }
    OLED_ShowChar(1, 1, i[2]);
    OLED_ShowChar(1, 3, i[5]);
    OLED_ShowChar(1, 5, i[8]);
    OLED_ShowChar(2, 1, i[1]);
    OLED_ShowChar(2, 3, i[4]);
    OLED_ShowChar(2, 5, i[7]);
    OLED_ShowChar(3, 1, i[0]);
    OLED_ShowChar(3, 3, i[3]);
    OLED_ShowChar(3, 5, i[6]);
}