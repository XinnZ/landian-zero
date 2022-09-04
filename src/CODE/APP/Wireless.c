/*
 * Wireless.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include <APP/Wireless.h>
#include <BSP/gpio.h>
#include "TC377_config.h"
#include "zf_stm_systick.h"
#include "zf_uart.h"
#include "zf_gpio.h"

uint8 wireless_rx_buffer;
//-------------------------------------------------------------------------------------------------------------------
//--> @brief      无线转串口模块 串口中断函数
//--> @param      void
//--> @return     void
//--> Sample usage:
//--> @note       该函数在ISR文件 串口2中断程序被调用
//-------------------------------------------------------------------------------------------------------------------
void Wireless_uart_callback(void)
{
    while (uart_query(DEBUG_UART, &wireless_rx_buffer))
        ;
    //-->读取收到的所有数据
}

//-------------------------------------------------------------------------------------------------------------------
//--> @brief      无线转串口模块 发送函数
//--> @param      buff        需要发送的数据地址
//--> @param      len         发送长度
//--> @return     uint32      剩余未发送的字节数
//--> Sample usage:
//--> @note
//-------------------------------------------------------------------------------------------------------------------
uint32 Wireless_Send(uint8 *buff, uint32 len)
{
    while (len > 30)
    {
        if (gpio_get(WL_RTSp))
        {
            return len; //-->模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        }
        //while(gpio_get(RTS_PIN)); //-->如果RTS为低电平，则继续发送数据
        uart_putbuff(DEBUG_UART, buff, 30);

        buff += 30; //-->地址偏移
        len -= 30;  //-->数量
    }

    if (gpio_get(WL_RTSp))
    {
        return len; //-->模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
    }
    //while(gpio_get(RTS_PIN)); //-->如果RTS为低电平，则继续发送数据
    uart_putbuff(DEBUG_UART, buff, len); //-->发送最后的数据

    return 0;
}

/*
 * @brief   : VOFA_Send()   向上位机发送数据
 * @param   : data1
 *            data2
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void VOFA_Send(float data1, float data2, float data3)
{
    float datas[3];
    uint8 data[16];
    datas[0] = data1;
    datas[1] = data2;
    datas[2] = data3;

    memcpy(data, (uint8 *)datas, sizeof(datas));

    data[12] = 0x00;
    data[13] = 0x00;
    data[14] = 0x80;
    data[15] = 0x7f;

    Wireless_Send(data, 16);
}
