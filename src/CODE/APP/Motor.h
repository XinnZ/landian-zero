/*
 * Motor.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_APP_MOTOR_H_
#define CODE_APP_MOTOR_H_

#include "zf_gpio.h"
#include "zf_gpt12.h"
#include "zf_gtm_pwm.h"

#define Motor_Freq      25000               // PWN Frequency
#define Motor_MAX_LIMIT 9000                // Motor MAX PWN limit

#define Motor_STBY      P02_2               // Drive Standby Mode;  L: Standby / H: Normal
#define Motor_STBY_H    gpio_set(Motor_STBY, 1);
#define Motor_STBY_L    gpio_set(Motor_STBY, 0);

#define Motor_R_T       GPT12_T5            // GPT12
#define Motor_R_A       GPT12_T5INB_P10_3   // A
#define Motor_R_B       GPT12_T5EUDB_P10_1  // B
#define Motor_R_S       ATOM0_CH4_P02_4     // Speed
#define Motor_R_D       ATOM0_CH6_P02_6     // Direction

#define Motor_L_T       GPT12_T2            // GPT12
#define Motor_L_A       GPT12_T2INB_P33_7   // A
#define Motor_L_B       GPT12_T2EUDB_P33_6  // B
#define Motor_L_S       ATOM0_CH0_P02_8     // Direction
#define Motor_L_D       ATOM0_CH5_P02_5     // Speed

typedef struct
{
        volatile _Bool  ENABLE;     //>使能电机标志位
        volatile int32  CurrentX;   //>当前偏移
        volatile int32  CurrentY;   //>当前位置
        volatile int32  TargetV;    //>目标速度
        volatile int32  Velocity;   //>最大速度
        volatile int32  Turn;       //>角速度值
        volatile float  Speed;      //>真实速度
        volatile int32  Encoder_R;  //>右编码器值
        volatile int32  Encoder_L;  //>左编码器值
        volatile int32  PWM_R;      //>右电机PWM
        volatile int32  PWM_L;      //>左电机PWM
        volatile uint32 TIME;       //>电机运行时间
} Motor_t;
extern Motor_t Motor;

void Motor_Init(void);
void Motor_Speed(int32 motor_R, int32 motor_L);
void Motor_Stop(void);

#endif /* CODE_APP_MOTOR_H_ */
