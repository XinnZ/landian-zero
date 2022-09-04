/*
 * gpio.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_BSP_GPIO_H_
#define CODE_BSP_GPIO_H_

#include "zf_gpio.h"

#define     LED0        P20_9       // LED0
#define     LED0_ON     gpio_set(LED0, 0)
#define     LED0_OFF    gpio_set(LED0, 1)
#define     LED0_T      gpio_toggle(LED0)

#define     LED1        P20_8       // LED1
#define     LED1_ON     gpio_set(LED1, 0)
#define     LED1_OFF    gpio_set(LED1, 1)
#define     LED1_T      gpio_toggle(LED1)

#define     LED2        P21_5       // LED2
#define     LED2_ON     gpio_set(LED2, 0)
#define     LED2_OFF    gpio_set(LED2, 1)
#define     LED2_T      gpio_toggle(LED2)

#define     LED3        P21_4       // LED2
#define     LED3_ON     gpio_set(LED3, 0)
#define     LED3_OFF    gpio_set(LED3, 1)
#define     LED3_T      gpio_toggle(LED3)

#define     KEY0        P33_13      // Five way key 1
#define     KEY1        P23_1       // Five way key 2
#define     KEY2        P32_4       // Five way key 3
#define     KEY3        P33_11      // Five way key 4
#define     KEY4        P33_12      // Five way key 5

#define     DIP0        P20_0       // DIP Switch 0
#define     DIP1        P21_6       // DIP Switch 1
#define     DIP2        P22_2       // DIP Switch 2
#define     DIP3        P22_0       // DIP Switch 3
#define     DIP4        P00_12      // DIP Switch 4
#define     DIP5        P00_9       // DIP Switch 5
#define     DIP6        P00_8       // DIP Switch 6
#define     DIP7        P33_5       // DIP Switch 7

#define     WL_RTSp     P33_10      // Wireless RTS

enum
{
    Key_None,       //>无操作
    Key_Up,         //>五轴上
    Key_Down,       //>五轴下
    Key_Left,       //>五轴左
    Key_Right,      //>五轴右
    Key_Press,      //>五轴按下
};

extern uint8 Mode[2][4];
extern volatile uint8 Key;

void GPIO_Init(void);
void Key_Scan(void);

#endif /* CODE_BSP_GPIO_H_ */
