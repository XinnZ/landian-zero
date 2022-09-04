/*
 * Controller.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include "Controller.h"

/**
 * @brief  : Angle and Omega(ω) Control
 */
void Car_Looking(void) {
    /* 数据获取 - 陀螺仪角速度 */
    ICM42605_GyroData();

    /* 算法处理 - 角度外环 */
    Motor.Turn = PID_Position(&PID_Angle, Motor.CurrentX, (int32)(IMAGE_MIDDLE));

    /* 算法处理 - 角速度内环 */
    Motor.Turn = PID_Position(&PID_Omega, (int32)(ICM.GYRO.Z * 10), Motor.Turn);
    Motor.Turn = PID_Limiter(Motor.Turn, -6000, 6000);
}

/**
 * @brief  : Velocity Control
 */
void Car_Going(void) {
    /* 数据获取 - 编码器值 */
    Motor.Encoder_R = gpt12_get(Motor_R_T);
    gpt12_clear(Motor_R_T);
    Motor.Encoder_L = -gpt12_get(Motor_L_T);
    gpt12_clear(Motor_L_T);

    /* 数据获取 - 真实速度 */
    Motor.Speed = (Motor.Encoder_R + Motor.Encoder_L) * 0.016349f;
    // ((((Motor.Encoder_R + Motor.Encoder_L) / 2) * 30 * 3.1415926 * 0.0675) / 38 / 1024) / 0.005;
    // 30πd / 38 / 1024 = 1(一个脉冲对应的路程)

    /* 算法处理 - 位置外环 */
    Motor.TargetV = PID_Position(&PID_Location, Motor.CurrentY, (int32)(IMAGE_H));

    /* 算法处理 - 速度内环 */
    Motor.PWM_R += PID_Increase(&PID_Velocity_R, Motor.Encoder_R * 10, Motor.TargetV + Motor.Turn);
    Motor.PWM_L += PID_Increase(&PID_Velocity_L, Motor.Encoder_L * 10, Motor.TargetV - Motor.Turn);

    /* 速度限幅 - 限制占空比 */
    Motor.PWM_R = PID_Limiter(Motor.PWM_R, -Motor_MAX_LIMIT, Motor_MAX_LIMIT);
    Motor.PWM_L = PID_Limiter(Motor.PWM_L, -Motor_MAX_LIMIT, Motor_MAX_LIMIT);

    /* 速度控制 */
    Motor_Speed(Motor.PWM_R, Motor.PWM_L);
}
