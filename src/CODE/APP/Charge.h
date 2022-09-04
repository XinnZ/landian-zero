/*
 * Charge.h
 *
 *  Created on: 2021Äê8ÔÂ1ÈÕ
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_APP_CHARGE_H_
#define CODE_APP_CHARGE_H_

#include "zf_vadc.h"

#define ch_VREF     3.309f      // Reference voltage
#define ch_ORGV     1.655f      // The chip outputs the original voltage
#define ch_VISN     10.95f      // Voltage scale factor to be calibrated
#define ch_R2MOHM   0.002f      // Resistance parameters to be calibrated

extern  float   LIMIT_V_MAX;    // Voltage limit Max
extern  float   LIMIT_V_MIN;    // Voltage limit Min

#define chV_V   ADC0_CH1_A1     // SuperCapacitor voltage
#define chV_A   ADC0_CH0_A0     // Resistance voltage detection

typedef struct
{
        float V;        // SuperCapacitor voltage
        float V8417;    // AD8417 voltage sample
        float A;        // SuperCapacitor current
        float P;        // Power    P=UI
        //float Q;        // Joule    Q=1/2*C*U*U
} Charge_t;
extern Charge_t Charge;

void Charge_Init(void);
void Charge_Progress(void);

#endif /* CODE_APP_CHARGE_H_ */
