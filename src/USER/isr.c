#include "isr_config.h"
#include "isr.h"

// PIT0_0_Motor Control_5ms
IFX_INTERRUPT(cc60_pit_ch0_isr, CCU6_0_CH0_INT_VECTAB_NUM, CCU6_0_CH0_ISR_PRIORITY)
{
	enableInterrupts();
	PIT_CLEAR_FLAG(CCU6_0, PIT_CH0);

    if (IfxCpu_acquireMutex(&g_mutexLockMOTOR))
    {
		Motor.TIME += 5;	// 5ms Timer

        if (Motor.ENABLE)
        {
            Car_Looking();
            Car_Going();
        }
        IfxCpu_releaseMutex(&g_mutexLockMOTOR);
    }
}
// PIT0_1_Charge Progress_5ms
IFX_INTERRUPT(cc60_pit_ch1_isr, CCU6_0_CH1_INT_VECTAB_NUM, CCU6_0_CH1_ISR_PRIORITY)
{
	enableInterrupts();
	PIT_CLEAR_FLAG(CCU6_0, PIT_CH1);

	Charge_Progress();
}
// PIT1_0_Coordinate X Change_2.5ms
IFX_INTERRUPT(cc61_pit_ch0_isr, CCU6_1_CH0_INT_VECTAB_NUM, CCU6_1_CH0_ISR_PRIORITY)
{
	enableInterrupts();
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH0);

	Looking_Coordinates(&Looking);
}
// PIT_1_1_Scheduler_1ms
IFX_INTERRUPT(cc61_pit_ch1_isr, CCU6_1_CH1_INT_VECTAB_NUM, CCU6_1_CH1_ISR_PRIORITY)
{
	enableInterrupts();
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH1);

	Scheduler_Run();

	if (Flag_Save)
        UI_Parm_Save(); //>保存设置
}




IFX_INTERRUPT(eru_ch0_ch4_isr, ERU_CH0_CH4_INT_VECTAB_NUM, ERU_CH0_CH4_INT_PRIO)
{
	enableInterrupts();
	if (GET_GPIO_FLAG(ERU_CH0_REQ4_P10_7)) //通道0
	{
		CLEAR_GPIO_FLAG(ERU_CH0_REQ4_P10_7);
	}

	if (GET_GPIO_FLAG(ERU_CH4_REQ13_P15_5)) //通道4
	{
		CLEAR_GPIO_FLAG(ERU_CH4_REQ13_P15_5);
	}
}

IFX_INTERRUPT(eru_ch1_ch5_isr, ERU_CH1_CH5_INT_VECTAB_NUM, ERU_CH1_CH5_INT_PRIO)
{
	enableInterrupts();
	if (GET_GPIO_FLAG(ERU_CH1_REQ5_P10_8)) //通道1
	{
		CLEAR_GPIO_FLAG(ERU_CH1_REQ5_P10_8);
	}

	if (GET_GPIO_FLAG(ERU_CH5_REQ1_P15_8)) //通道5
	{
		CLEAR_GPIO_FLAG(ERU_CH5_REQ1_P15_8);
	}
}

//>由于摄像头pclk引脚默认占用了 2通道，用于触发DMA，因此这里不再定义中断函数
/*IFX_INTERRUPT(eru_ch2_ch6_isr, ERU_CH2_CH6_INT_VECTAB_NUM, ERU_CH2_CH6_INT_PRIO)
{
    enableInterrupts();
	if(GET_GPIO_FLAG(ERU_CH2_REQ7_P00_4))       //>通道2
	{
		CLEAR_GPIO_FLAG(ERU_CH2_REQ7_P00_4);

	}
	if(GET_GPIO_FLAG(ERU_CH6_REQ9_P20_0))       //>通道6
	{
		CLEAR_GPIO_FLAG(ERU_CH6_REQ9_P20_0);

	}
}*/
IFX_INTERRUPT(dma_ch5_isr, ERU_DMA_INT_VECTAB_NUM, ERU_DMA_INT_PRIO)
{
	enableInterrupts();

	MT9V034_DMA();
}

IFX_INTERRUPT(eru_ch3_ch7_isr, ERU_CH3_CH7_INT_VECTAB_NUM, ERU_CH3_CH7_INT_PRIO)
{
	enableInterrupts();

	if (GET_GPIO_FLAG(ERU_CH3_REQ6_P02_0)) //>通道3 MT9V034 VSYNC
	{
		CLEAR_GPIO_FLAG(ERU_CH3_REQ6_P02_0);

		MT9V034_Vsync();
	}
	if (GET_GPIO_FLAG(ERU_CH7_REQ16_P15_1)) //>通道7
	{
		CLEAR_GPIO_FLAG(ERU_CH7_REQ16_P15_1);
	}
}




// UART_0 None
IFX_INTERRUPT(uart0_tx_isr, UART0_INT_VECTAB_NUM, UART0_TX_INT_PRIO)
{
	enableInterrupts();
	IfxAsclin_Asc_isrTransmit(&uart0_handle);
}
IFX_INTERRUPT(uart0_rx_isr, UART0_INT_VECTAB_NUM, UART0_RX_INT_PRIO)
{
	enableInterrupts();
	IfxAsclin_Asc_isrReceive(&uart0_handle);
}
IFX_INTERRUPT(uart0_er_isr, UART0_INT_VECTAB_NUM, UART0_ER_INT_PRIO)
{
	enableInterrupts();
	IfxAsclin_Asc_isrError(&uart0_handle);
}

// UART_1 MT9V034
IFX_INTERRUPT(uart1_tx_isr, UART1_INT_VECTAB_NUM, UART1_TX_INT_PRIO)
{
	enableInterrupts();
	IfxAsclin_Asc_isrTransmit(&uart1_handle);
}
IFX_INTERRUPT(uart1_rx_isr, UART1_INT_VECTAB_NUM, UART1_RX_INT_PRIO)
{
	enableInterrupts();
	IfxAsclin_Asc_isrReceive(&uart1_handle);

	MT9V034_Uart_Callback();    //>摄像头数据接收
}
IFX_INTERRUPT(uart1_er_isr, UART1_INT_VECTAB_NUM, UART1_ER_INT_PRIO)
{
	enableInterrupts();
	IfxAsclin_Asc_isrError(&uart1_handle);
}

// UART_2 Wireless Upper
IFX_INTERRUPT(uart2_tx_isr, UART2_INT_VECTAB_NUM, UART2_TX_INT_PRIO)
{
    enableInterrupts();
    IfxAsclin_Asc_isrTransmit(&uart2_handle);
}
IFX_INTERRUPT(uart2_rx_isr, UART2_INT_VECTAB_NUM, UART2_RX_INT_PRIO)
{
    enableInterrupts();
    IfxAsclin_Asc_isrReceive(&uart2_handle);

    Wireless_uart_callback();   //>无线串口数据接收
}
IFX_INTERRUPT(uart2_er_isr, UART2_INT_VECTAB_NUM, UART2_ER_INT_PRIO)
{
    enableInterrupts();
    IfxAsclin_Asc_isrError(&uart2_handle);
}

// UART_3 None
IFX_INTERRUPT(uart3_tx_isr, UART3_INT_VECTAB_NUM, UART3_TX_INT_PRIO)
{
    enableInterrupts();
    IfxAsclin_Asc_isrTransmit(&uart3_handle);
}
IFX_INTERRUPT(uart3_rx_isr, UART3_INT_VECTAB_NUM, UART3_RX_INT_PRIO)
{
    enableInterrupts();
    IfxAsclin_Asc_isrReceive(&uart3_handle);
}
IFX_INTERRUPT(uart3_er_isr, UART3_INT_VECTAB_NUM, UART3_ER_INT_PRIO)
{
    enableInterrupts();
    IfxAsclin_Asc_isrError(&uart3_handle);
}
