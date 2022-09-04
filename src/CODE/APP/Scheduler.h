/*
 * Scheduler.h
 *
 *  Created on: 2021Äê8ÔÂ1ÈÕ
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_APP_SCHEDULER_H_
#define CODE_APP_SCHEDULER_H_

#include "headfile.h"

typedef struct
{
        void (*task_func)(void);
        float rate_hz;
        unsigned short int interval_ticks;
        unsigned int last_run;
        unsigned int run_time;
} sched_task_t;

void Scheduler_Init(void);
void Scheduler_Run(void);

#endif /* CODE_APP_SCHEDULER_H_ */
