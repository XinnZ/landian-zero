/*
 * MT9V034.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include <APP/MT9V034.h>
#include <APP/Wireless.h>
#include "IfxDma.h"
#include "IfxScuEru.h"
#include "zf_stm_systick.h"
#include "zf_gpio.h"
#include "zf_eru.h"
#include "zf_eru_dma.h"

uint8 receive[3];
uint8 receive_num = 0;
vuint8 uart_receive_flag;

uint8 link_list_num;

uint8 *camera_buffer_addr;      //>摄像头数组地址


//>需要配置到摄像头的数据
int16 MT9V034_CFG[CONFIG_FINISH][2] =
{
        {AUTO_EXP,        0},       //>自动曝光设置      范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
                                    //>一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
        {EXP_TIME,      700},       //>曝光时间          摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
        {FPS,           150},       //>图像帧率          摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
        {SET_COL,   IMAGE_W},       //>图像列数量        范围1-752     K60采集不允许超过188
        {SET_ROW,   IMAGE_H},       //>图像行数量        范围1-480
        {LR_OFFSET,       0},       //>图像左右偏移量    正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
        {UD_OFFSET,       0},       //>图像上下偏移量    正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
        {GAIN,           32},       //>图像增益          范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度

        {INIT,            0}        //>摄像头开始初始化
};

//>从摄像头内部获取到的配置数据
int16 GET_CFG[CONFIG_FINISH - 1][2] =
{
        {AUTO_EXP,  0},     //>自动曝光设置
        {EXP_TIME,  0},     //>曝光时间
        {FPS,       0},     //>图像帧率
        {SET_COL,   0},     //>图像列数量
        {SET_ROW,   0},     //>图像行数量
        {LR_OFFSET, 0},     //>图像左右偏移量
        {UD_OFFSET, 0},     //>图像上下偏移量
        {GAIN,      0},     //>图像增益
};

//>-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V034摄像头串口中断函数
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//  @note       此函数在isr.c中 被串口中断函数调用
//-------------------------------------------------------------------------------------------------------------------
void MT9V034_Uart_Callback(void)
{
    while (uart_query(MT9V034_COF_UART, &receive[receive_num]))
    {
        receive_num++;

        if (1 == receive_num && 0XA5 != receive[0])
            receive_num = 0;
        if (3 == receive_num)
        {
            receive_num = 0;
            uart_receive_flag = 1;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      配置摄像头内部配置信息
//  @param      uartn       选择使用的串口
//  @param      buff        发送配置信息的地址
//  @return     void
//  @since      v1.0
//  Sample usage:           调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void set_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH - 1][2])
{
    uint16 temp, i;
    uint8 send_buffer[4];

    uart_receive_flag = 0;

    //>设置参数  具体请参看问题锦集手册
    //>开始配置摄像头并重新初始化
    for (i = 0; i < CONFIG_FINISH; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = (uint8)buff[i][0];
        temp = buff[i][1];
        send_buffer[2] = temp >> 8;
        send_buffer[3] = (uint8)temp;

        uart_putbuff(uartn, send_buffer, 4);
        systick_delay_ms(2);
    }
    //>等待摄像头初始化成功
    while (!uart_receive_flag)
        ;
    uart_receive_flag = 0;
    while ((0xff != receive[1]) || (0xff != receive[2]))
        ;
    //>以上部分对摄像头配置的数据全部都会保存在摄像头上51单片机的eeprom中
    //>利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取摄像头内部配置信息
//  @param      uartn       选择使用的串口
//  @param      buff        接收配置信息的地址
//  @return     void
//  @since      v1.0
//  Sample usage:           调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void get_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH - 1][2])
{
    uint16 temp, i;
    uint8 send_buffer[4];

    for (i = 0; i < CONFIG_FINISH - 1; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = GET_STATUS;
        temp = buff[i][0];
        send_buffer[2] = temp >> 8;
        send_buffer[3] = (uint8)temp;

        uart_putbuff(uartn, send_buffer, 4);

        //等待接受回传数据
        while (!uart_receive_flag)
            ;
        uart_receive_flag = 0;

        buff[i][1] = receive[1] << 8 | receive[2];
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取摄像头固件版本
//  @param      uartn       选择使用的串口
//  @return     void
//  @since      v1.0
//  Sample usage:           调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 get_version(UARTN_enum uartn)
{
    uint16 temp;
    uint8 send_buffer[4];
    send_buffer[0] = 0xA5;
    send_buffer[1] = GET_STATUS;
    temp = GET_VERSION;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;

    uart_putbuff(uartn, send_buffer, 4);

    //>等待接受回传数据
    while (!uart_receive_flag)
        ;
    uart_receive_flag = 0;

    return ((uint16)(receive[1] << 8) | receive[2]);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      单独设置摄像头曝光时间
//  @param      uartn       选择使用的串口
//  @param      light       设置曝光时间越大图像越亮，摄像头收到后会根据分辨率及FPS计算最大曝光时间如果设置的数据过大，那么摄像头将会设置这个最大值
//  @return     uint16      当前曝光值，用于确认是否正确写入
//  @since      v1.0
//  Sample usage:           调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 set_exposure_time(UARTN_enum uartn, uint16 light)
{
    uint16 temp;
    uint8 send_buffer[4];

    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_EXP_TIME;
    temp = light;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;

    uart_putbuff(uartn, send_buffer, 4);

    //>等待接受回传数据
    while (!uart_receive_flag)
        ;
    uart_receive_flag = 0;

    temp = receive[1] << 8 | receive[2];
    return temp;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      对摄像头内部寄存器进行写操作
//  @param      uartn       选择使用的串口
//  @param      addr        摄像头内部寄存器地址
//  @param      data        需要写入的数据
//  @return     uint16      寄存器当前数据，用于确认是否写入成功
//  @since      v1.0
//  Sample usage:			调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 set_mt9v034_reg(UARTN_enum uartn, uint8 addr, uint16 data)
{
    uint16 temp;
    uint8 send_buffer[4];

    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_ADDR;
    temp = addr;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;

    uart_putbuff(uartn, send_buffer, 4);
    systick_delay_ms(10);

    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_DATA;
    temp = data;
    send_buffer[2] = temp >> 8;
    send_buffer[3] = (uint8)temp;

    uart_putbuff(uartn, send_buffer, 4);

    //>等待接受回传数据
    while (!uart_receive_flag)
        ;
    uart_receive_flag = 0;

    temp = receive[1] << 8 | receive[2];
    return temp;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V034摄像头初始化
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:	使用FLEXIO接口采集摄像头
//-------------------------------------------------------------------------------------------------------------------
void MT9V034_Init(void)
{
    uint8 i;
    camera_buffer_addr = MT9V034.IMAGE[0];

    boolean interrupt_state = disableInterrupts();

    uart_init(MT9V034_COF_UART, 9600, MT9V034_COF_UART_TX, MT9V034_COF_UART_RX); //>初始换串口 配置摄像头
    enableInterrupts();                                                          //>开启中断

    //>等待摄像头上电初始化成功 方式有两种 延时或者通过获取配置的方式 二选一
    //systick_delay_ms(500);                //>延时方式
    get_config(MT9V034_COF_UART, GET_CFG);  //>获取配置的方式

    uart_receive_flag = 0;
    set_config(MT9V034_COF_UART, MT9V034_CFG);

    //>获取配置便于查看配置是否正确
    get_config(MT9V034_COF_UART, GET_CFG);

    disableInterrupts();

    //>摄像头采集初始化
    //>初始化 数据引脚
    for (i = 0; i < 8; i++)
    {
        gpio_init((PIN_enum)(MT9V034_DATA_PIN + i), GPI, 0, PULLUP);
    }

    link_list_num = eru_dma_init(MT9V034_DMA_CH, GET_PORT_IN_ADDR(MT9V034_DATA_PIN), camera_buffer_addr, MT9V034_PCLK_PIN, FALLING, IMAGE_W * IMAGE_H);

    eru_init(MT9V034_VSYNC_PIN, FALLING);   //>初始化场中断，并设置为下降沿触发中断
    restoreInterrupts(interrupt_state);
}

uint8 MT9V034_dma_int_num;  //>当前DMA中断次数
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V034摄像头场中断
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				此函数在isr.c中被eru（GPIO中断）中断调用
//-------------------------------------------------------------------------------------------------------------------
void MT9V034_Vsync(void)
{
    CLEAR_GPIO_FLAG(MT9V034_VSYNC_PIN);
    MT9V034_dma_int_num = 0;
    if (!MT9V034.FINISH)    //>查看图像数组是否使用完毕，如果未使用完毕则不开始采集，避免出现访问冲突
    {
        if (1 == link_list_num)
        {
            //>没有采用链接传输模式 重新设置目的地址
            DMA_SET_DESTINATION(MT9V034_DMA_CH, camera_buffer_addr);
        }
        dma_start(MT9V034_DMA_CH);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V034摄像头DMA完成中断
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				此函数在isr.c中被dma中断调用
//-------------------------------------------------------------------------------------------------------------------
void MT9V034_DMA(void)
{
    CLEAR_DMA_FLAG(MT9V034_DMA_CH);
    MT9V034_dma_int_num++;

    if (MT9V034_dma_int_num >= link_list_num)
    {
        //>采集完成
        MT9V034_dma_int_num = 0;
        MT9V034.FINISH      = 1;    //>一副图像从采集开始到采集结束耗时3.8MS左右(50FPS、188*120分辨率)
        dma_stop(MT9V034_DMA_CH);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风摄像头图像发送至上位机查看图像
//  @param      image			需要发送的图像地址
//  @param      width			图像的列
//  @param      height			图像的行
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void MT9V034_Send_IMG(uint8 *image, uint16 width, uint16 height)
{
    uint8 cmd[4] = {0x00, 0xff, 0x01, 0x01};    //>命令
    Wireless_Send(cmd, 4);                 //>发送命令
    Wireless_Send(image, width * height);  //>发送图像
}
