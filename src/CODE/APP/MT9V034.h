/*
 * MT9V034.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_APP_MT9V034_H_
#define CODE_APP_MT9V034_H_

#include "common.h"
#include "IfxDma_cfg.h"
#include "zf_uart.h"

//>配置摄像头参数
#define IMAGE_W             162         //>图像宽度 范围1-188
#define IMAGE_H             100         //>图像高度 范围1-120
#define IMAGE_SIZE          IMAGE_W * IMAGE_H     //>图像小于 65536/4 防止图像错误
#define IMAGE_MIDDLE        IMAGE_W / 2

#define IMAGE_ignoreTop     15          //>图像处理忽略顶部像素
#define IMAGE_ignoreBottom  0           //>图像处理忽略底部像素

//>相机的内参和畸变参数 参数出自MATLAB
#define DIS_K1      -0.3030930f         //>径向畸变
#define DIS_K2      0.07450939f
#define DIS_P1      0.0f                //>切向畸变
#define DIS_P2      0.0f;
#define DIS_Fx      71.5098269f         //>使用像素来描述x轴方向焦距的长度
#define DIS_Fy      71.5781746f         //>使用像素来描述y轴方向焦距的长度
#define DIS_Cx      78.2887089f
#define DIS_Cy      40.5534325f

typedef struct
{
    _Bool FINISH;                       //>一场图像采集完成标志位
    uint8 IMAGE[IMAGE_H][IMAGE_W];      //>图像原始数据   必须4字节对齐
    uint8 UNDST[IMAGE_H][IMAGE_W];      //>图像去畸变数据 必须4字节对齐
    uint8 BIN[IMAGE_H][IMAGE_W];        //>图像二值化数据 必须4字节对齐
    uint8 threshold;                    //>阈值
    uint8 threshold_offset;             //>阈值增幅
} MT9V034_t;
extern MT9V034_t MT9V034;

//--------------------------------------------------------------------------------------------------
//>引脚配置
//--------------------------------------------------------------------------------------------------
#define MT9V034_COF_UART UART_1 //>配置摄像头所使用到的串口
#define MT9V034_COF_UART_TX UART1_TX_P11_12
#define MT9V034_COF_UART_RX UART1_RX_P11_10

#define MT9V034_VSYNC_PIN ERU_CH3_REQ6_P02_0    //>场中断引脚 可选范围参考ERU_PIN_enum枚举 不可与像素引脚选择为同一个通道，或者是共用中断的通道 \
                                                //>例如场中断选择ERU_CH3 那么像素引脚就不能选择ERU_CH7，因为3和7的中断是共用的。

#define MT9V034_DATA_PIN P00_0                  //>定义D0数据引脚  假设D0定义为P00_0 那么D1所使用的引脚则为P00_1，依次类推 \
                                                //>可设置参数P00_0、P02_0、P15_0，不能设置为其他参数

#define MT9V034_PCLK_PIN ERU_CH2_REQ2_P10_2     //>定义像素时钟引脚 可选范围参考ERU_PIN_enum枚举 不可与场中断引脚选择为同一个通道，或者是共用中断的通道 \
                                                //>例如场中断选择ERU_CH3 那么像素引脚就不能选择ERU_CH7，因为3和7的中断是共用的。

#define MT9V034_DMA_CH IfxDma_ChannelId_5       //>定义使用的DMA通道 0-47可选  通道号越大优先级越低

//>摄像头命令枚举
typedef enum
{
    INIT = 0,      //>摄像头初始化命令
    AUTO_EXP,      //>自动曝光命令
    EXP_TIME,      //>曝光时间命令
    FPS,           //>摄像头帧率命令
    SET_COL,       //>图像列命令
    SET_ROW,       //>图像行命令
    LR_OFFSET,     //>图像左右偏移命令
    UD_OFFSET,     //>图像上下偏移命令
    GAIN,          //>图像偏移命令
    CONFIG_FINISH, //>非命令位，主要用来占位计数

    COLOR_GET_WHO_AM_I = 0xEF,
    SET_EXP_TIME = 0XF0, //>单独设置曝光时间命令
    GET_STATUS,          //>获取摄像头配置命令
    GET_VERSION,         //>固件版本号命令

    SET_ADDR = 0XFE, //>寄存器地址命令
    SET_DATA         //>寄存器数据命令
} CMD;

extern uint8 receive[3];         //>仅用于接收配置信息时所用  禁止用户使用该变量
extern uint8 receive_num;        //>仅用于接收配置信息时所用  禁止用户使用该变量
extern vuint8 uart_receive_flag; //>仅用于接收配置信息时所用  禁止用户使用该变量

void set_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH - 1][2]);
void get_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH - 1][2]);
uint16 get_version(UARTN_enum uartn);
uint16 set_exposure_time(UARTN_enum uartn, uint16 light);

void MT9V034_Init(void);
void MT9V034_Vsync(void);
void MT9V034_DMA(void);
void MT9V034_Uart_Callback(void);
void MT9V034_Send_IMG(uint8 *image, uint16 width, uint16 height);

#endif
