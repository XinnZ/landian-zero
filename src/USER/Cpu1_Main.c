#include "headfile.h"
#pragma section all "cpu1_dsram" // CPU1 RAM

Looking_t   Looking;
MT9V034_t   MT9V034;
CON_t       CONV;

_Bool Mode_Undistort;   //>去畸变模式   1: 去畸变     0: 原始图像
_Bool Mode_Threshold;   //>阈值模式     1: 平均阈值   0: 限幅大津法双峰
_Bool Mode_Looking;     //>寻灯模式     1: 白斑       0: 连通域

void core1_main(void)
{
    disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

    systick_delay_ms(200);

    MT9V034_Init();         // Initialize MT9V034
    Image_Undistort_MAP();  // Generate undistorted coordinate mapping

    //pit_interrupt_ms(CCU6_1, PIT_CH0, 2.5); // CCU6_1_0-2.5ms-Timer Coordinate Change

    // Wait for CPU cores to initialize
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts(); // Initial completion enable interrupts

    Mode_Undistort = (_Bool)Mode[0][0];
    Mode_Threshold = (_Bool)Mode[0][1];
    Mode_Looking   = (_Bool)Mode[0][2];

    while (TRUE)
    {
        if (IfxCpu_acquireMutex(&g_mutexLockIMAGE))
        {
            //LED3_T;
            if (MT9V034.FINISH)
            {
                //LED2_T;

                if (Mode_Undistort) // DIP 0
                    Image_Undistort(&MT9V034);

                if (Mode_Threshold) // DIP 1
                {
                    Image_Threshold(&MT9V034);
                    Image_BIN(&MT9V034);
                }
                else
                    Image_thresholdGray2Binary(&MT9V034);

                if (!Mode_Looking)  // DIP 2
                    CON_TwoPass(MT9V034, &CONV);

                Looking_Target(&Looking, MT9V034, CONV);

                MT9V034.FINISH = 0;
            }
            IfxCpu_releaseMutex(&g_mutexLockIMAGE);
        }
    }
}

#pragma section all restore
