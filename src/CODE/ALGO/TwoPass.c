/*
 * TwoPass.c
 *
 *  Created on: 2021Äê8ÔÂ1ÈÕ
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 *        Link: https://blog.csdn.net/icvpr/article/details/10259577
 */

#include "TwoPass.h"

#include <stdlib.h>
#include <string.h>

#include "../APP/MT9V034.h"

uint16 parent[IMAGE_W * IMAGE_H];  // union-find root

/**
 * @brief  : Find the root of label x
 * @param  : x:
 * @param  : parent:
 * @return : uint16:
 */
uint16 TwoPass_Find(uint16 x, uint16 parent[]) {
    // x < MAX;
    uint16 i = x;
    while (0 != parent[i])
        i = parent[i];
    return i;
}

/**
 * @brief  : Set label x and label y to the same connected-region
 * @param  : x:
 * @param  : y:
 * @param  : parent:
 */
void TwoPass_Union(uint16 x, uint16 y, uint16 parent[]) {
    // x < MAX && y < MAX
    uint16 i = x;
    uint16 j = y;
    while (0 != parent[i])
        i = parent[i];
    while (0 != parent[j])
        j = parent[j];
    if (i != j)
        parent[i] = j;
}

/**
 * @brief  : Two-Pass Connected Component Labeling - Four
 * @param  : camera:
 * @param  : con:
 */
void CON_TwoPass(const MT9V034_t camera, CON_t *con) {
    uint8 x, y;
    uint16 label = 0,
           up,
           left;

    // Initialize to 0
    memset(con, 0, sizeof(CON_t));
    memset(parent, 0, sizeof(uint16) * IMAGE_W * IMAGE_H);

    // First Pass - INIT label IMAGE
    for (y = IMAGE_ignoreTop; y < IMAGE_H - IMAGE_ignoreBottom; y++) {
        for (x = 0; x < IMAGE_W; x++) {
            if (camera.BIN[y][x] != 0) {
                // Left and up labels
                left = (x - 1 < 0) ? 0 : con->IMAGE[y][x - 1];
                up = (y - 1 < 0) ? 0 : con->IMAGE[y - 1][x];

                // At least one label is marked
                if (left != 0 || up != 0) {
                    // Two labels are marked
                    if (left != 0 && up != 0) {
                        // Adopt smaller label
                        con->IMAGE[y][x] = up > left ?  // MIN
                                               left
                                                     : up;
                        if (left <= up)
                            TwoPass_Union(up, left, parent);
                        else if (up < left)
                            TwoPass_Union(left, up, parent);
                    } else {
                        // Adopt larger label
                        con->IMAGE[y][x] = up > left ?  // MAX
                                               up
                                                     : left;
                    }
                } else {
                    // Non of two labels is markded, so add a new label
                    con->IMAGE[y][x] = ++label;
                }
            }
        }
    }

    // Second Pass
    for (y = IMAGE_ignoreTop; y < IMAGE_H - IMAGE_ignoreBottom; y++) {
        for (x = 0; x < IMAGE_W; x++) {
            if (camera.BIN[y][x] != 0)
                con->IMAGE[y][x] = TwoPass_Find(con->IMAGE[y][x], parent);

            if (con->IMAGE[y][x] > con->count)
                con->count = con->IMAGE[y][x];
        }
    }
}
