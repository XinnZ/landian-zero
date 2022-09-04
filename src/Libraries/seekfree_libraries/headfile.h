#ifndef _headfile_h
#define _headfile_h


#include "zf_assert.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "stdbool.h"

#include "ifxAsclin_reg.h"
#include "SysSe/Bsp/Bsp.h"
#include "IfxCcu6_Timer.h"
#include "IfxScuEru.h"


#include "zf_gpio.h"
#include "zf_gtm_pwm.h"
#include "zf_uart.h"
#include "zf_ccu6_pit.h"
#include "zf_stm_systick.h"
#include "zf_spi.h"
#include "zf_eru.h"
#include "zf_eru_dma.h"
#include "zf_vadc.h"
#include "zf_gpt12.h"
#include "zf_eeprom.h"


#include "../../CODE/ALGO/Controller.h"
#include "../../CODE/ALGO/Looking.h"
#include "../../CODE/ALGO/PID.h"
#include "../../CODE/ALGO/RUN.h"
#include "../../CODE/ALGO/TwoPass.h"

#include "../../CODE/APP/Charge.h"
#include "../../CODE/APP/ICM42605.h"
#include "../../CODE/APP/IPS114.h"
#include "../../CODE/APP/Motor.h"
#include "../../CODE/APP/MT9V034.h"
#include "../../CODE/APP/Scheduler.h"
#include "../../CODE/APP/Wireless.h"

#include "../../CODE/BSP/gpio.h"
#include "../../CODE/BSP/ui_parm.h"
#include "../../CODE/BSP/ui.h"


#endif
