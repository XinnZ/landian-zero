/*
 * IPS114.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_APP_IPS114_H_
#define CODE_APP_IPS114_H_

#include "common.h"

#define IPS114_SPIN_PIN     SPI_0               //>定义使用的SPI号
#define IPS114_SCL_PIN      SPI0_SCLK_P20_11    //>定义SPI_SCK引脚
#define IPS114_SDA_PIN      SPI0_MOSI_P20_14    //>定义SPI_MOSI引脚
#define IPS114_SDA_IN_PIN   SPI0_MISO_P20_12    //>定义SPI_MISO引脚  IPS没有MISO引脚，但是这里任然需要定义，在spi的初始化时需要使用
#define IPS114_CS_PIN       SPI0_CS2_P20_13     //>定义SPI_CS引脚

#define IPS114_BL_PIN       P20_12              //>液晶背光引脚定义 由于实际通讯未使用P15_4因此 这里复用为GPIO
#define IPS114_REST_PIN     P15_1               //>液晶复位引脚定义
#define IPS114_DC_PIN       P15_0               //>液晶命令位引脚定义

#define IPS114_BL(x)        gpio_set(IPS114_BL_PIN, x)
#define IPS114_DC(x)        gpio_set(IPS114_DC_PIN, x)
#define IPS114_REST(x)      gpio_set(IPS114_REST_PIN, x)

//阴码 逐行式 字宽16 字高16 逆向

//16位BMP 40X40 QQ图像取模数据
//Image2LCD取模选项设置
//水平扫描
//16位
//40X40
//不包含图像头数据
//自左至右
//自顶至底
//低位在前

#define Color_WHITE     0xFFFF      //>白色
#define Color_BLACK     0x0000      //>黑色
#define Color_RED       0xF800      //>红色
#define Color_BLUE      0x001F      //>蓝色
#define Color_YELLOW    0xFFE0      //>黄色
#define Color_GREEN     0x07E0      //>绿色
#define Color_GRAY      0X8430      //>灰色
#define Color_BROWN     0XBC40      //>棕色
#define Color_PURPLE    0XF81F      //>紫色
#define Color_PINK      0XFE19      //>粉色

#define IPS114_PENCOLOR Color_WHITE //>定义写字笔的颜色
#define IPS114_BGCOLOR  Color_BLACK //>定义背景颜色

#define IPS114_W 135
#define IPS114_H 240

//定义显示方向
//0 竖屏模式
//1 竖屏模式  旋转180
//2 横屏模式
//3 横屏模式  旋转180
#define IPS114_DISPLAY_DIR 3

#if (0 == IPS114_DISPLAY_DIR || 1 == IPS114_DISPLAY_DIR)
#define IPS114_X_MAX IPS114_W //液晶X方宽度
#define IPS114_Y_MAX IPS114_H //液晶Y方宽度

#elif (2 == IPS114_DISPLAY_DIR || 3 == IPS114_DISPLAY_DIR)
#define IPS114_X_MAX IPS114_H //液晶X方宽度
#define IPS114_Y_MAX IPS114_W //液晶Y方宽度

#else
#error "IPS114_DISPLAY_DIR 定义错误"

#endif

void IPS114_Init(void);
void IPS114_Clear(uint16 color);
void IPS114_Draw_Point(uint16 x, uint16 y, uint16 color);
void IPS114_Draw_Part(uint16 xs, uint16 ys, uint16 xe, uint16 ye, uint16 color_dat);
void IPS114_Draw_Rectangle(uint16 xs, uint16 ys, uint16 xe, uint16 ye, uint16 color_dat);
void IPS114_Char(uint16 x, uint16 y, const int8 dat, uint16 word_color, uint16 back_color);
void IPS114_String(uint16 x, uint16 y, const int8 dat[], uint16 word_color, uint16 back_color);
void IPS114_IMAGE032(uint8 *p, uint16 width, uint16 height);
void IPS114_IMAGE032_BIN(uint8 *p, uint8 threshold);
void IPS114_IMAGE032_CON(uint16 *p, uint16 total);
//void IPS114_Chinese(uint16 x, uint16 y, uint8 size, const uint8 *p, uint8 number, uint16 color);

#endif
