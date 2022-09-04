/*
 * PID.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include "PID.h"

/**
 * @brief  : Limiting
 * @param  : amt:
 * @param  : low:
 * @param  : high:
 * @return : int32:
 */
int32 PID_Limiter(int32 amt, int32 low, int32 high) {
    return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)));
}

/**
 * @brief  : Limiting
 * @param  : amt:
 * @param  : low:
 * @param  : high:
 * @return : float:
 */
float PID_Limiter_float(float amt, float low, float high) {
    return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)));
}

/**
 * @brief  : PID Initialize
 * @param  : pid:
 */
void PID_Init(PID_t *pid) {
    pid->kP = 0.0f;
    pid->kI = 0.0f;
    pid->kD = 0.0f;

    pid->integral_limit = 0;
    pid->integrator = 0.0f;
    pid->last_error = 0;
    pid->last_derivative = 0;
}

/**
 * @brief  : PID Clear
 * @param  : pid:
 */
void PID_Clear(PID_t *pid) {
    pid->integrator = 0.0f;
    pid->last_error = 0;
    pid->last_derivative = 0;
}

/**
 * @brief  : PID 位置式控制器输出
 * @param  : pid:
 * @param  : actual:
 * @param  : set:
 * @return : int32:
 */
int32 PID_Position(PID_t *pid, int32 actual, int32 set) {
    int32 iError;    // 当前误差
    int32 Position;  // 最后得出的实际输出

    /*计算当前误差*/
    iError = set - actual;

    /* 累积误差 - 此车模位置式仅使用PD */
    pid->integrator += pid->kI * iError;

    /* 误差限幅 */
    pid->integrator = PID_Limiter_float(pid->integrator, (float)(0 - pid->integral_limit), (float)(pid->integral_limit));

    /* 增量的计算 */
    Position = (int32)(pid->kP * iError + pid->integrator + pid->kD * (iError - pid->last_error));

    /* 更新上次误差 */
    pid->last_error = iError;

    return Position;
}

/**
 * @brief  : PID 增量式控制器输出
 * @param  : pid:
 * @param  : actual:
 * @param  : set:
 * @return : int32:
 */
int32 PID_Increase(PID_t *pid, int32 actual, int32 set) {
    int32 iError;    // 当前误差
    int32 Increase;  // 最后得出的实际输出

    /* 计算当前误差 */
    iError = set - actual;

    /* 增量的计算 */
    Increase = (int32)(pid->kP * (iError - pid->last_error) + pid->kI * iError + pid->kD * (iError - 2 * pid->last_error + pid->last_derivative));

    /* 更新前次误差 */
    pid->last_derivative = pid->last_error;
    /* 更新上次误差 */
    pid->last_error = iError;

    return Increase;
}
