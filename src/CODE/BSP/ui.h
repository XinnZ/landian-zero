/*
 * UI.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_BSP_UI_H_
#define CODE_BSP_UI_H_

typedef enum
{
    ui_Main,    //>主界面
    ui_Motor,   //>电机调试界面
    ui_MT9V034, //>摄像头调试界面
    ui_Modules, //>外部模块调试界面
    ui_RUN      //>跑车界面
} UI_e;

typedef struct
{
    unsigned char CurrentUI;    //>当前界面
    unsigned char LastFocus;    //>上次聚焦界面
    unsigned char CurrentFocus; //>当前聚焦界面
} UI_Focus_t;                   //>界面聚焦
extern UI_Focus_t UI_Focus;

void UI_Init(void);
void UI_Charge(void);
void UI_Main(void);
void UI_Motor(void);
void UI_MT9V034(void);
void UI_Modules(void);

#endif /* CODE_BSP_UI_H_ */
