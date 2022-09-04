/*
 * Charge.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include <APP/Charge.h>
#include "zf_ccu6_pit.h"
#include "zf_vadc.h"

float LIMIT_V_MAX = 8.0f;   // Voltage limit Max
float LIMIT_V_MIN = 3.3f;   // Voltage limit Min

uint16 adcVSCP = 0;
uint16 adcASCP = 0;

/*
 * @brief   : Charge_Init()
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void Charge_Init(void)
{
    adc_init(ADC_0, chV_V);
    adc_init(ADC_0, chV_A);

    pit_interrupt_ms(CCU6_0, PIT_CH1, 5);   // CCU6_0_1-5ms Timer
}

/*
 * @brief   : Charge_Progress()
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void Charge_Progress(void)
{
    adcVSCP = adc_mean_filter(ADC_0, chV_V, ADC_12BIT, 50);
    adcASCP = adc_mean_filter(ADC_0, chV_A, ADC_12BIT, 50);

    Charge.V8417 = ch_VREF * adcASCP / 4095;                // Voltage value of AD8417
    Charge.V = ch_VISN * ch_VREF * adcVSCP / 4095;          // Voltage value of SuperCapacitor
    Charge.A = (Charge.V8417 - ch_ORGV) / 60 / ch_R2MOHM;   // Current: 60(实际电压差倍数)

    Charge.V = (float)((int16)(Charge.V * 10 + 0.5f) / 10.0f);
    Charge.A = (float)((int16)(Charge.A * 10 + 0.5f) / 10.0f);

    Charge.P = Charge.V * Charge.A; // P=UI     Power calculation
    //Charge.Q += Charge.P * 0.005;   // Q=PT     U*I*0.005(5ms) Integral period
}
