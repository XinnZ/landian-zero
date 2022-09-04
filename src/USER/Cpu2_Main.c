#include "headfile.h"
#pragma section all "cpu2_dsram" // CPU2 RAM

Motor_t Motor;
ICM_t   ICM;
PID_t   PID_Angle,
        PID_Omega,
        PID_Velocity_R,
        PID_Velocity_L,
        PID_Location;

void core2_main(void)
{
    disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

    Motor_Init();       // Initialize Motor
    ICM42605_Init();    // Initialize ICM42605

    Motor.TIME = 4000;  // Start time

    // Wait for CPU cores to initialize
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts(); // Initial completion enable interrupts

    while (TRUE)
    {

    }
}

#pragma section all restore
