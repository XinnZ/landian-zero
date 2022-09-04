/*
 * Looking.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include "Looking.h"

#include <math.h>
#include <stdlib.h>

#include "../APP/MT9V034.h"
#include "TwoPass.h"

/**
 * @brief  : Find the Beacon and determine the coordinates
 * @param  : look:
 * @param  : camera:
 * @param  : con:
 */
void Looking_Target(Looking_t *look, MT9V034_t camera, CON_t con) {
    uint8 x, y, cCount = 0;
    int32 index = 0;
    uint32 SumX = 0, SumY = 0;  // 特征光斑坐标和
    CON_Node *SubCON = NULL;    // 子连通域
    CON_Node *FitCON = NULL;    // 符合条件的子连通域

    if (Mode_Looking) {
        /* *************************  横行二点判别法  ************************* */
        /* *********** 远距离信标灯光斑非常小  使用横杠三点远距离较难判断 *********** */
        for (y = IMAGE_ignoreTop; y < IMAGE_H - IMAGE_ignoreBottom; y++) {
            for (x = 1; x < IMAGE_W; x++) {
                if (camera.BIN[y][x - 1] &&  // 连续二个点判断为信标
                    camera.BIN[y][x]) {      // && camera.BIN[y][x + 1]
                    SumX += x;
                    SumY += y;
                    index += 1;
                }
            }
        }

        if (index) {
            look->Y = (uint8)(SumY / index);
            look->X = (uint8)(SumX / index);
            look->EUREKA = 1;  // 发现信标灯
        } else {
            look->Y = 0;
            look->X = 0;
            look->EUREKA = 0;  // 未发现信标灯
        }
    } else {
        /* *****************************  连通域判别法  ***************************** */
        if (con.count) {
            SubCON = (CON_Node *)malloc(sizeof(CON_Node) * con.count);
            memset(SubCON, 0, sizeof(CON_Node) * con.count);

            for (y = IMAGE_ignoreTop; y < IMAGE_H - IMAGE_ignoreBottom; y++) {
                for (x = 0; x < IMAGE_W; x++) {
                    index = con.IMAGE[y][x] - 1;
                    if (index >= 0) {
                        if (SubCON[index].nsX == 0)
                            SubCON[index].nsX = x;  // Start X
                        if (SubCON[index].nsY == 0)
                            SubCON[index].nsY = y;  // Start Y

                        SubCON[index].nsX = x < SubCON[index].nsX ? x : SubCON[index].nsX;  // Start X
                        SubCON[index].nsY = y < SubCON[index].nsY ? y : SubCON[index].nsY;  // Start Y
                        SubCON[index].neX = x > SubCON[index].neX ? x : SubCON[index].neX;  // End X
                        SubCON[index].neY = y > SubCON[index].neY ? y : SubCON[index].neY;  // End Y
                        SubCON[index].nArea += 1;                                           // 面积

                        if (x == 0 || y == 0)  // 边长
                        {
                            CONV.EDGES[y][x] = (uint16)index + 1;
                            SubCON[index].nEdge += 1;
                        } else {
                            if (!(con.IMAGE[y + 1][x] && con.IMAGE[y - 1][x] &&
                                  con.IMAGE[y][x + 1] && con.IMAGE[y][x - 1])) {
                                CONV.EDGES[y][x] = (uint16)index + 1;
                                SubCON[index].nEdge += 1;
                            }
                        }
                    }
                }
            }

            // 连通域基本条件
            for (index = 0; index < con.count; index++) {
                // >>长需要大于或等于宽
                // if ((SubCON[index].neX - SubCON[index].nsX + 1) < (SubCON[index].neY - SubCON[index].nsY + 1))
                //    continue;
                // >>连通域面积需要占矩形面积的一半以上
                // if ((SubCON[index].nArea * 2) < ((SubCON[index].neX - SubCON[index].nsX + 1) * (SubCON[index].neY - SubCON[index].nsY + 1)))
                //    continue;
                // >>轮廓圆形度需要在1附近
                // SubCON[index].nRound = (float)(4.0f * 3.1415927f * SubCON[index].nArea) / (float)(SubCON[index].nEdge * SubCON[index].nEdge);
                // if (fabs(SubCON[index].nRound - 1.0f) > 1.0f)
                //    continue;

                cCount = (uint8)index + 1;
            }
        }

        if (cCount) {
            FitCON = &SubCON[cCount - 1];

            look->EUREKA = 1;
            look->count = cCount;

            look->sX = FitCON->nsX;
            look->sY = FitCON->nsY;
            look->eX = FitCON->neX;
            look->eY = FitCON->neY;
            look->cArea = FitCON->nArea;
            look->cEdge = FitCON->nEdge;
            look->cRound = FitCON->nRound;

            look->X = (look->eX + look->sX) / 2;
            look->Y = (look->eY + look->sY) / 2;
        } else {
            look->EUREKA = 0;
            look->X = 0;
            look->Y = 0;
            look->sX = 0;
            look->sY = 0;
            look->eX = 0;
            look->eY = 0;
            look->cArea = 0;
            look->cEdge = 0;
            look->cRound = 0.0f;
            look->count = 0;
        }

        free(SubCON);
    }
}

/**
 * @brief  : Gentle change of coordinates
 * @param  : look:
 */
void Looking_Coordinates(Looking_t *look) {
    static int16 LastX = 0,
                 LastY = 0;

    if (look->X > LastX) {
        LastX += 1;

        if (LastX > IMAGE_W - 1)
            LastX = IMAGE_W - 1;

        look->SoftX = (uint8)(LastX);
    }
    if (look->X < LastX) {
        LastX -= 1;

        if (LastX < 0)
            LastX = 0;

        look->SoftX = (uint8)(LastX);
    }

    if (look->Y > LastY) {
        LastY += 2;

        if (LastY > IMAGE_H - 1)
            LastY = IMAGE_H - 1;

        look->SoftY = (uint8)(LastY);
    }
    if (look->Y < LastY) {
        LastY -= 2;

        if (LastY < 0)
            LastY = 0;

        look->SoftY = (uint8)(LastY);
    }
}

int16 MAP_distortX[IMAGE_H][IMAGE_W];  // 去畸变图像横坐标对应的原图像横坐标
int16 MAP_distortY[IMAGE_H][IMAGE_W];  // 去畸变图像纵坐标对应的原图像纵坐标
/**
 * @brief  : Image distort coordinate map
 */
void Image_Undistort_MAP(void) {
    uint8 v, u;
    double x1, y1, x2, y2, r2;

    for (v = 0; v < IMAGE_H; v++)
        for (u = 0; u < IMAGE_W; u++) {
            x1 = (u - DIS_Cx) / DIS_Fx;
            y1 = (v - DIS_Cy) / DIS_Fy;

            r2 = pow(x1, 2) + pow(y1, 2);
            x2 = x1 * (1 + DIS_K1 * r2 + DIS_K2 * pow(r2, 2)) + 2 * DIS_P1 * x1 * y1;         // + DIS_P2 * (r2 + 2 * x1 * x1);
            y2 = y1 * (1 + DIS_K1 * r2 + DIS_K2 * pow(r2, 2)) + DIS_P1 * (r2 + 2 * y1 * y1);  // + 2 * DIS_P2 * x1 * y1;

            // 最近邻插值 四舍五入
            MAP_distortX[v][u] = (int16)((DIS_Fx * x2 + DIS_Cx + 0.5f));
            MAP_distortY[v][u] = (int16)((DIS_Fy * y2 + DIS_Cy + 0.5f));
        }
}

/**
 * @brief  : Image undistort (170°总钻风摄像头图像去畸变)
 * @param  : cam:
 */
void Image_Undistort(MT9V034_t *cam) {
    uint8 v, u;
    int16 x, y;

    for (v = 0; v < IMAGE_H; v++)
        for (u = 0; u < IMAGE_W; u++) {
            x = MAP_distortX[v][u];
            y = MAP_distortY[v][u];

            if (x >= 0 && y >= 0 && x < IMAGE_W && y < IMAGE_H)
                cam->UNDST[v][u] = cam->IMAGE[y][x];
            else
                cam->UNDST[v][u] = 0;
        }
}

/**
 * @brief  : Camera image binarization
 * @param  : cam:
 */
void Image_BIN(MT9V034_t *cam) {
    uint8 width, height;
    uint8 *src_img;

    if (Mode_Undistort)
        src_img = *cam->UNDST;
    else
        src_img = *cam->IMAGE;

    for (height = IMAGE_ignoreTop; height < IMAGE_H - IMAGE_ignoreBottom; height++) {
        for (width = 0; width < IMAGE_W; width++) {
            if (*(src_img++) > cam->threshold)
                cam->BIN[height][width] = 1;
            else
                cam->BIN[height][width] = 0;
        }
    }
}

/**
 * @brief  : Camera image mean thresholding
 * @param  : cam:
 */
void Image_Threshold(MT9V034_t *cam) {
    uint8 width, height;
    uint32 PixelSum = 0;
    uint8 *src_img;

    if (Mode_Undistort)
        src_img = *cam->UNDST;
    else
        src_img = *cam->IMAGE;

    for (height = 0; height < IMAGE_H; height++)
        for (width = 0; width < IMAGE_W; width++)
            PixelSum += (uint32) * (src_img++);

    cam->threshold = (uint8)(PixelSum / IMAGE_W / IMAGE_H) + cam->threshold_offset;
}

/**
 * @brief  : Limit OTSU and 2-Mode Method
 * @param  : cam:
 */
void Image_thresholdGray2Binary(MT9V034_t *cam) {
    uint8 *src_img;
    uint8 *img_binary = *cam->BIN;
    float AllGrayScale[256];
    uint16 AllGrayVal[256] = {0};  // 所有灰度值的个数矩阵
    uint16 i, gray_val;
    float w0, w1;       // 前景和背景的像素点占比
    float u0, u1;       // 前景 背景和总体的平均灰度
    float g, maxg = 0;  // 类间方差

    uint16 MinTop[2] = {0};
    uint16 MaxTop[2] = {0};

    if (Mode_Undistort)
        src_img = *cam->UNDST;
    else
        src_img = *cam->IMAGE;

    for (i = 0; i < IMAGE_SIZE; i++)
        AllGrayVal[*(src_img++)]++;

    for (i = 0; i < 256; i++)
        AllGrayScale[i] = (float)AllGrayVal[i] / IMAGE_SIZE;

    for (gray_val = 0; gray_val < 256; gray_val++)  // 找最大类间方差时的阈值
    {
        w0 = w1 = u0 = u1 = 0;
        for (i = 0; i < 256; i++) {
            if (i <= gray_val)  // 小于阈值,前景部分
            {
                w0 += AllGrayScale[i];
                u0 += i * AllGrayScale[i];
            } else  // 背景
            {
                // w1 += AllGrayScale[i];
                u1 += i * AllGrayScale[i];
            }
        }
        w1 = 1.0f - w0;
        u0 /= w0;                             // 前景均灰度
        u1 /= w1;                             // 背景均灰度
        g = w0 * w1 * (u0 - u1) * (u0 - u1);  // g=ω0ω1(μ0-μ1)^2
        if (g > maxg && gray_val < 128)       // 有更大的类间方差
        {
            maxg = g;
            cam->threshold = (uint8)gray_val;  // 找到最大类间方差对应的阈值
        }
    }

    // 双峰
    for (i = cam->threshold; i > 0; i--) {
        if (MinTop[1] < AllGrayVal[i]) {
            MinTop[1] = AllGrayVal[i];
            MinTop[0] = i;
            break;
        }
    }
    for (i = cam->threshold; i < 128; i++) {
        if (MaxTop[1] < AllGrayVal[i]) {
            MaxTop[1] = AllGrayVal[i];
            MaxTop[0] = i;
            break;
        }
    }
    cam->threshold = (MinTop[0] + MaxTop[0]) / 2;

    if (cam->threshold < 10)
        cam->threshold = 255;

    if (Mode_Undistort)  // 开始使用阈值二值化
        src_img = *cam->UNDST;
    else
        src_img = *cam->IMAGE;

    for (i = 0; i < IMAGE_SIZE; i++) {
        if (*(src_img++) > cam->threshold)
            *(img_binary++) = 1;
        else
            *(img_binary++) = 0;
    }
}
