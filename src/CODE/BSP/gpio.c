/*
 * gpio.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include <BSP/gpio.h>
#include "zf_gpio.h"

uint8 Mode[2][4];
volatile uint8 Key;

/*
 * @brief   : GPIO_Init  GPIO初始化
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void GPIO_Init(void)
{
    gpio_init(LED0,     GPO,    1,  PUSHPULL);  //>核心板LED0
    gpio_init(LED1,     GPO,    1,  PUSHPULL);  //>核心板LED1
    gpio_init(LED2,     GPO,    1,  PUSHPULL);  //>核心板LED2
    gpio_init(LED3,     GPO,    1,  PUSHPULL);  //>核心板LED3

    gpio_init(KEY0,     GPI,    1,  PULLUP);    //>主控板五向1
    gpio_init(KEY1,     GPI,    1,  PULLUP);    //>主控板五向2
    gpio_init(KEY2,     GPI,    1,  PULLUP);    //>主控板五向3
    gpio_init(KEY3,     GPI,    1,  PULLUP);    //>主控板五向4
    gpio_init(KEY4,     GPI,    1,  PULLUP);    //>主控板五向5

    gpio_init(DIP0,     GPI,    1,  PULLUP);    //>主控板DIP0
    gpio_init(DIP1,     GPI,    1,  PULLUP);    //>主控板DIP1
    gpio_init(DIP2,     GPI,    1,  PULLUP);    //>主控板DIP2
    gpio_init(DIP3,     GPI,    1,  PULLUP);    //>主控板DIP3
    gpio_init(DIP4,     GPI,    1,  PULLUP);    //>主控板DIP4
    gpio_init(DIP5,     GPI,    1,  PULLUP);    //>主控板DIP5
    gpio_init(DIP6,     GPI,    1,  PULLUP);    //>主控板DIP6
    gpio_init(DIP7,     GPI,    1,  PULLUP);    //>主控板DIP7

    gpio_init(WL_RTSp,  GPI,    0,  PULLDOWN);  //>主控板无线串口流控引脚
}

/*
 * @brief   : Key_Scan  读取五向开关
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void Key_Scan(void)
{
    static _Bool Flag_Loos = 1; //>按键仅仅单次触发
    uint8 Value;

    Value = (gpio_get(KEY0)     |  (gpio_get(KEY1)<<1) |
            (gpio_get(KEY2)<<2) |  (gpio_get(KEY3)<<3) |
            (gpio_get(KEY4)<<4) );

    switch (Value)
    {
        case 0x1f:  Key = Key_None;     break;
        case 0x0f:  Key = Key_Up;       break;
        case 0x1b:  Key = Key_Down;     break;
        case 0x17:  Key = Key_Left;     break;
        case 0x1e:  Key = Key_Right;    break;
        case 0x1d:  Key = Key_Press;    break;
        default:    Key = Key_None;     break;
    }

    if (Key != Key_None)
    {
        if (!Flag_Loos)
        {
            Key = Key_None;
        }
        Flag_Loos = 0;
    }
    else
        Flag_Loos = 1;
}
