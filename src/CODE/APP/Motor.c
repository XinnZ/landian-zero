/*
 * Motor.c
 *
 *  Created on: 2021Äê8ÔÂ1ÈÕ
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include <APP/Motor.h>
#include <ALGO/PID.h>
#include "zf_ccu6_pit.h"
#include "zf_gtm_pwm.h"
#include "zf_gpt12.h"
#include "zf_gpio.h"

/*
 * @brief   : Motor_Init()
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void Motor_Init(void)
{
    pit_interrupt_ms(CCU6_0,  PIT_CH0,  5); // CCU6_0_0-5ms_Timer Controller

    gtm_pwm_init(Motor_R_S, Motor_Freq, 0);
    gtm_pwm_init(Motor_R_D, Motor_Freq, 0);
    gtm_pwm_init(Motor_L_S, Motor_Freq, 0);
    gtm_pwm_init(Motor_L_D, Motor_Freq, 0);

    gpt12_init(Motor_R_T,   Motor_R_A,  Motor_R_B);
    gpt12_init(Motor_L_T,   Motor_L_A,  Motor_L_B);

    gpio_init(Motor_STBY,   GPO,    0,  PUSHPULL);

    PID_Init(&PID_Angle);           // PID Looking
    PID_Init(&PID_Omega);           // PID YAW
    PID_Init(&PID_Velocity_R);      // PID Velocity R
    PID_Init(&PID_Velocity_L);      // PID Velocity L
    PID_Init(&PID_Location);        // PID Location
}

/*
 * @brief   : Motor_Speed()
 * @param   : motor_R
 *            motor_L
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void Motor_Speed(int32 motor_R, int32 motor_L)
{
    if (motor_R >= 0)
    {
        pwm_duty(Motor_R_D, (uint32)motor_R);
        pwm_duty(Motor_R_S, 0);
    }
    else
    {
        pwm_duty(Motor_R_D, 0);
        pwm_duty(Motor_R_S, (uint32)(0 - motor_R));
    }

    if (motor_L >= 0)
    {
        pwm_duty(Motor_L_D, (uint32)motor_L);
        pwm_duty(Motor_L_S, 0);
    }
    else
    {
        pwm_duty(Motor_L_D, 0);
        pwm_duty(Motor_L_S, (uint32)(0 - motor_L));
    }
}

/*
 * @brief   : Motor_Stop()
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void Motor_Stop(void)
{
    pwm_duty(Motor_R_S, 0);
    pwm_duty(Motor_R_D, 0);

    pwm_duty(Motor_L_S, 0);
    pwm_duty(Motor_L_D, 0);
}
