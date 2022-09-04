/*
 * RUN.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include "headfile.h"

Flag_t Flag = {0};
Stage_t Stage[3] = {
    // TP    UT   CP   UC  AP  AC    OP
    {15.0f, 58, 7.0f, 92, 65, 10, 0.33f},
    {17.5f, 66, 7.0f, 92, 70, 10, 0.43f},
    {20.0f, 70, 7.0f, 92, 75, 10, 0.50f}};

/**
 * @brief  : Just an initialization
 */
void RUN_Init(void) {
    Flag.Mode_CAP = (_Bool)Mode[1][0];

    // 拨码开关选择速度
    for (uint8 n = 1; n < 4; n++) {
        if (Mode[1][n])
            Flag.IndexV = n - 1;
    }

    Flag.FirstCharge = 1;
}

/**
 * @brief  : RUN_GoGoGo Go Go Go！！！
 */
void RUN_GoGoGo(void) {
    uint8 Var_aX = 0, Var_aY = 0;     // 信标灯坐标原始数据
    uint32 Timing, ChargeTiming = 0;  // 总计时, 后续充电计时

    LED0_ON;                                 // 充电指示
    IPS114_BL(0);                            // 关屏幕
    Motor.ENABLE = 0;                        // 关电机
    pit_disable_interrupt(CCU6_1, PIT_CH1);  // 关任务调度

    Timing = Motor.TIME;

    while (UI_Focus.CurrentUI == ui_RUN) {
        if (IfxCpu_acquireMutex(&g_mutexLockIMAGE))  // 图像处理互斥
        {
            Flag.FindTarget = Looking.EUREKA;

            // Var_sX = Looking.SoftX; // 目标横坐标 柔和处理
            // Var_sY = Looking.SoftY; // 目标纵坐标 柔和处理
            Var_aX = Looking.X;  // 目标原始横坐标
            Var_aY = Looking.Y;  // 目标原始纵坐标

            IfxCpu_releaseMutex(&g_mutexLockIMAGE);
        }

        if (IfxCpu_acquireMutex(&g_mutexLockMOTOR))  // 电机控制互斥
        {
            PID_Omega.kP = Stage[Flag.IndexV].OmegaP;

            if (Flag.Charged)  // 充电完成
            {
                if (Flag.FindTarget)  // 找到目标
                {
                    if (!Flag.ToCharge)  // 无需充电 跑向目标
                    {
                        Motor.CurrentX = Var_aX + Stage[Flag.IndexV].AngleCOR;  // 当前偏移 ( + 修正 )
                        Motor.CurrentY = Var_aY;                                // 当前距离

                        PID_Location.kP = Stage[Flag.IndexV].TatgetP;

                        if (Var_aY >= 60)  // 减速
                            PID_Location.kP /= 2;
                        if (Var_aY >= Stage[Flag.IndexV].UphillT)  // 给上坡速度
                            Motor.CurrentY = Stage[Flag.IndexV].UphillT;

                        if ((Var_aY > (IMAGE_H / 2)) && (abs(Var_aX - IMAGE_MIDDLE) > 15))
                            PID_Angle.kP = Stage[Flag.IndexV].AngleP + 20;
                        else
                            PID_Angle.kP = Stage[Flag.IndexV].AngleP;
                    } else  // 需要充电 准备充电
                    {
                        Motor.CurrentX = Var_aX;  // 当前偏移
                        Motor.CurrentY = Var_aY;  // 当前距离
                        Flag.ChargeReady = 1;     // 充电准备

                        PID_Angle.kP = Stage[Flag.IndexV].AngleP + 10;
                        PID_Location.kP = Stage[Flag.IndexV].ChatgeP;
                        if (Var_aY >= Stage[Flag.IndexV].UphillC)  // 给上坡速度
                            Motor.CurrentY = Stage[Flag.IndexV].UphillC;
                    }
                } else  // 未找到 旋转找灯
                {
                    if (Flag.ToCharge && Flag.ChargeReady)  // 需要停车充电
                    {
                        Motor.CurrentX = IMAGE_MIDDLE;
                        Motor.CurrentY = Stage[Flag.IndexV].UphillC;  // 给位置环偏差 慢速上灯
                        PID_Location.kP = Stage[Flag.IndexV].ChatgeP;
                        Flag.Charged = 0;  // 开始充电
                        Flag.ChargeReady = 0;
                        ChargeTiming = Motor.TIME;
                    } else {
                        Motor.CurrentX = 0;
                        Motor.CurrentY = Stage[Flag.IndexV].UphillT;
                        ;
                        PID_Angle.kP = Stage[Flag.IndexV].AngleP + 10;
                    }
                }
            } else  // 充电未完成
            {
                Motor.CurrentX = IMAGE_MIDDLE;
                Motor.CurrentY = IMAGE_H;
            }

            if ((Charge.V > (Flag.Mode_CAP ? 9.5f : 11.0f)) && !Flag.FirstCharge)
                goto Charging_Complete;  // 非首次充电
            if ((Charge.V > (Flag.Mode_CAP ? 6.0f : 8.5f)) && !Flag.FirstCharge && (Motor.TIME > 1000 * 160))
                goto Charging_Complete;  // 非首次充电且时间在两分半 加快速度
            if (Flag.ToCharge && !Flag.Charged && !Flag.FirstCharge) {
                if (Charge.A > 4.8f)  // 电流阈值
                    Motor.CurrentY = IMAGE_H;
                else
                    Motor.CurrentY = Stage[Flag.IndexV].UphillC;
                if (Flag.FindTarget)  // 还存在目标
                    goto Charging_Complete;
                if (Charge.A < 4.8f && ((Motor.TIME - ChargeTiming) > 3500))  // 超过3秒半未在充电或电流过低
                    goto Charging_Complete;
            }

            if (Charge.V > (Flag.Mode_CAP ? 7.2f : 11.0f))  // LIMIT_V_MAX)) && Flag.FirstCharge) // 充电完成
            {
            Charging_Complete:
                LED0_OFF;              // 关闭充电指示
                Motor_STBY_H;          // 开电机驱动
                Motor.ENABLE = 1;      // 开电机
                Flag.ToCharge = 0;     // 无需充电
                Flag.Charged = 1;      // 充电完成
                Flag.FirstCharge = 0;  // 第一次充电完成
            }
            if (Charge.V < (Flag.Mode_CAP ? 3.5f : 4.0f) && (Motor.TIME - Timing) <= 1000 * 170)  // LIMIT_V_MIN 电压过低且时间充裕
            {
                LED0_ON;            // 充电指示
                Flag.ToCharge = 1;  // 需要充电
            }

            if ((Motor.TIME - Timing) >= 1000 * 180)  // 三分钟已到
            {
                if (!Flag.FindTarget) {
                    UI_Focus.CurrentUI = ui_Main;
                    LED0_ON;
                    LED1_ON;
                    LED2_ON;
                    LED3_ON;
                    Motor.CurrentX = IMAGE_MIDDLE;
                    Motor.CurrentY = IMAGE_H;
                }
            }

            IfxCpu_releaseMutex(&g_mutexLockMOTOR);
        }
        systick_delay_ms(1);
    }
}
