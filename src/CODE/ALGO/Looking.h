/*
 * Looking.h
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_ALGO_LOOKING_H_
#define CODE_ALGO_LOOKING_H_

#include "../APP/MT9V034.h"
#include "TwoPass.h"
#include "common.h"

typedef struct
{
    _Bool EUREKA;  // 找到目标标志位
    uint8 X;       // 目标中心横坐标
    uint8 Y;       // 目标中心纵坐标
    uint8 SoftX;   // 目标中心横坐标 柔和处理
    uint8 SoftY;   // 目标中心纵坐标 柔和处理

    uint8 sX;      // 目标连通域起始坐标x
    uint8 sY;      // 目标连通域起始坐标y
    uint8 eX;      // 目标连通域结束坐标x
    uint8 eY;      // 目标连通域结束坐标y
    uint8 cArea;   // 目标连通域面积
    uint8 cEdge;   // 目标连通域边长
    float cRound;  // 目标连通域圆形度
    uint8 count;   // 满足基本条件的连通域个数
} Looking_t;
extern Looking_t Looking;
extern _Bool Mode_Undistort;
extern _Bool Mode_Looking;

void Looking_Target(Looking_t *look, MT9V034_t camera, CON_t con);
void Looking_Coordinates(Looking_t *look);
void Image_Undistort_MAP(void);
void Image_Undistort(MT9V034_t *cam);
void Image_BIN(MT9V034_t *cam);
void Image_Threshold(MT9V034_t *cam);
void Image_thresholdGray2Binary(MT9V034_t *cam);

#endif /* CODE_ALGO_LOOKING_H_ */
