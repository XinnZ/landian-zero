/*
 * RUN.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_ALGO_RUN_H_
#define CODE_ALGO_RUN_H_

#include "common.h"
#include "headfile.h"

typedef struct
{
    bool Mode_CAP;  // 大小电容

    bool FirstCharge;  // 第一次充电
    bool Charged;      // 充电完成
    bool ToCharge;     // 需要充电
    bool ChargeReady;  // 充电就绪

    uint8 IndexV;     // 速度决策索引
    bool FindTarget;  // 找到信标灯
} Flag_t;

typedef struct
{
    float TatgetP;   // 正常运行位置外环P
    int32 UphillT;   // 爬坡减速偏差
    float ChatgeP;   // 充电位置外环P
    int32 UphillC;   // 爬坡偏差(充电)
    float AngleP;    // 角度环P
    int32 AngleCOR;  // 角度修正
    float OmegaP;    // 角速度环P
} Stage_t;

void RUN_Init(void);
void RUN_GoGoGo(void);

#endif /* CODE_ALGO_RUN_H_ */
