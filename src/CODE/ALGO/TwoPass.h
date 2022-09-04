/*
 * TwoPass.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 *        Link: https://blog.csdn.net/icvpr/article/details/10259577
 */

#ifndef CODE_ALGO_TWOPASS_H_
#define CODE_ALGO_TWOPASS_H_

#include <APP/MT9V034.h>

#include "common.h"

typedef struct CON_Node {
    uint8 nsX;     // 连通域起始坐标x
    uint8 nsY;     // 连通域起始坐标y
    uint8 neX;     // 连通域结束坐标x
    uint8 neY;     // 连通域结束坐标y
    uint8 nArea;   // 连通域面积
    uint8 nEdge;   // 连通域边长
    float nRound;  // 连通域圆形度
} CON_Node;

typedef struct
{
    uint16 IMAGE[IMAGE_H][IMAGE_W];  // 连通域图像数据
    uint16 EDGES[IMAGE_H][IMAGE_W];  // 连通域边界数据
    volatile uint16 count;           // 连通域个数
} CON_t;
extern CON_t CONV;

void CON_TwoPass(const MT9V034_t camera, CON_t *con);

#endif /* CODE_ALGO_TWOPASS_H_ */
