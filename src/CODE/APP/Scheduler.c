/*
 * Scheduler.c
 *
 *  Created on: 2021��8��1��
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
    Key_Scan();         //>����ɨ��
    UI_Parm_Tasks();    //>��������
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

//>ϵͳ�������ã�������ִͬ��Ƶ�ʵġ��̡߳�
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
//>�������鳤�� �ж��߳�����
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

    //>��ʼ�������
    for (index = 0; index < TASK_NUM; index++)
    {
        //>����ÿ���������ʱ������
        sched_tasks[index].interval_ticks = (unsigned short int)(1000.0f / sched_tasks[index].rate_hz);
        //>�������Ϊ1
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

    //>ѭ���ж������߳� �Ƿ�Ӧ��ִ��
    for (index = 0; index < TASK_NUM; index++)
    {
        systick_start(STM2);             //>ʹ��STM2
        count = systick_getval_us(STM2); //>��ȡSTM2
        //>�����ж� �����ǰʱ���ȥ��һ��ִ�е�ʱ�� ���ڵ��ڸ��̵߳�ִ������ ��ִ���߳�
        //>ͬʱ�ж� �������1msִ�����������㹻ʱ��ִ��������ִ�У��������������ִ��
        if (systime_ms - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks && 1000 - count >= sched_tasks[index].run_time)
        {
            //>�����̵߳�ִ��ʱ�� ������һ���ж�
            sched_tasks[index].last_run = systime_ms;
            //>ִ���̺߳��� ʹ�õ��Ǻ���ָ��
            sched_tasks[index].task_func();
            //>��¼�߳�ִ��ʱ��
            sched_tasks[index].run_time = systick_getval_us(STM2) - count;
        }
    }
}