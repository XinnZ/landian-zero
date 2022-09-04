/*
 * Scheduler.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include <APP/Scheduler.h>

volatile unsigned long int systime_ms = 0;

/********************************************************************************
* Routine: Loop_1000Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_1000Hz(void)
{

}

/********************************************************************************
* Routine: Loop_500Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_500Hz(void)
{

}

/********************************************************************************
* Routine: Loop_333Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_333Hz(void)
{

}

/********************************************************************************
* Routine: Loop_200Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_200Hz(void)
{
    Key_Scan();         //>按键扫描
    UI_Parm_Tasks();    //>界面任务
}

/********************************************************************************
* Routine: Loop_100Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_100Hz(void)
{

}

/********************************************************************************
* Routine: Loop_50Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_50Hz(void)
{

}

/********************************************************************************
* Routine: Loop_20Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_20Hz(void)
{

}

/********************************************************************************
* Routine: Loop_5Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_5Hz(void)
{

}

/********************************************************************************
* Routine: Loop_2Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_2Hz(void)
{
    gpio_toggle(LED0);
}

/********************************************************************************
* Routine: Loop_1Hz
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
static void Loop_1Hz(void)
{

}

//>系统任务配置，创建不同执行频率的“线程”
static sched_task_t sched_tasks[] =
    {
        {Loop_1000Hz, 1000, 0, 0, 0},
        {Loop_500Hz,   500, 0, 0, 0},
        {Loop_333Hz,   333, 0, 0, 0},
        {Loop_200Hz,   200, 0, 0, 0},
        {Loop_100Hz,   100, 0, 0, 0},
        {Loop_50Hz,     50, 0, 0, 0},
        {Loop_20Hz,     20, 0, 0, 0},
        {Loop_5Hz,       5, 0, 0, 0},
        {Loop_2Hz,       2, 0, 0, 0},
        {Loop_1Hz,       1, 0, 0, 0},
};
//>根据数组长度 判断线程数量
#define TASK_NUM (sizeof(sched_tasks) / sizeof(sched_task_t))

/********************************************************************************
* Routine: Scheduler_Init
* Description:
* Parameter: void
* Return: void
* Notes:
**********************************************************************************/
void Scheduler_Init(void)
{
    unsigned char index = 0;

    // CCU6_1_1 - 1ms
    pit_interrupt_ms(CCU6_1, PIT_CH1, 1);

    //>初始化任务表
    for (index = 0; index < TASK_NUM; index++)
    {
        //>计算每个任务的延时周期数
        sched_tasks[index].interval_ticks = (unsigned short int)(1000.0f / sched_tasks[index].rate_hz);
        //>最短周期为1
        if (sched_tasks[index].interval_ticks < 1)
        {
            sched_tasks[index].interval_ticks = 1;
        }
    }
}

/********************************************************************************
* Routine: Scheduler_Run
* Description:
* Parameter: NULL
* Return: NULL
* Notes:
**********************************************************************************/
void Scheduler_Run(void)
{
    unsigned char index = 0;
    unsigned int count = 0;

    systime_ms++;

    //>循环判断所有线程 是否应该执行
    for (index = 0; index < TASK_NUM; index++)
    {
        systick_start(STM2);             //>使用STM2
        count = systick_getval_us(STM2); //>获取STM2
        //>进行判断 如果当前时间减去上一次执行的时间 大于等于该线程的执行周期 则执行线程
        //>同时判断 如果本次1ms执行周期内有足够时间执行任务，则执行，否则存入下周期执行
        if (systime_ms - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks && 1000 - count >= sched_tasks[index].run_time)
        {
            //>更新线程的执行时间 用于下一次判断
            sched_tasks[index].last_run = systime_ms;
            //>执行线程函数 使用的是函数指针
            sched_tasks[index].task_func();
            //>记录线程执行时长
            sched_tasks[index].run_time = systick_getval_us(STM2) - count;
        }
    }
}
