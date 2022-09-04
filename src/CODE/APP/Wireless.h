/*
 * APP_Wireless.h
 *
 *  Created on: 2021Äê8ÔÂ1ÈÕ
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#ifndef CODE_APP_WIRELESS_H_
#define CODE_APP_WIRELESS_H_

unsigned long Wireless_Send(unsigned char *buff, unsigned long len);
void Wireless_uart_callback(void);

void VOFA_Send(float data1, float data2, float data3);

#endif /* CODE_APP_WIRELESS_H_ */
