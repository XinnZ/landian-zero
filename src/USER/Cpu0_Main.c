#include "Cpu0_Main.h"
#include "headfile.h"
#pragma section all "cpu0_dsram" // CPU0 RAM

Charge_t        Charge;
UI_Focus_t      UI_Focus;
Parm_Focus_t    Parm_Focus;

int core0_main(void)
{
    MCU_Init();         // Initialize System Clock
    GPIO_Init();        // Initialize GPIO
    Charge_Init();      // Initialize Charge
    IPS114_Init();      // Initialize IPS114
    UI_Init();          // Initialize UI
    Scheduler_Init();   // Initialize Time Scheduler

    UI_Parm_Read();     // Read Parameters

    if (Mode[0][3])     // DIP: - - - 1
        UI_Focus.CurrentUI = ui_RUN; // Run£¡
    RUN_Init();         // Initialize RUN Go Go Go!

    // Wait for CPU cores to initialize
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts(); // Initial completion enable interrupts

	while (TRUE)
	{
        switch(UI_Focus.CurrentUI)
        {
            case ui_Main:       UI_Main();      break;
            case ui_Motor:      UI_Motor();     break;
            case ui_MT9V034:    UI_MT9V034();   break;
            case ui_Modules:    UI_Modules();   break;
            case ui_RUN:        RUN_GoGoGo();   break;
        }
	}
}

#pragma section all restore
