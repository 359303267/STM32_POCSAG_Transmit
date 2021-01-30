/***************************************************************************
 * pocsag.h
 * 适用平台：STM32
 * 编译工具：Keil5
 * 编写：小小小日天
 * 最后编辑者：小小小日天
 * 说明：本项目遵循GPL协议，项目中的代码只可作为个人DIY研究，不可商用，商用需授权。
***************************************************************************/

#ifndef __POCSAG_H
#define __POCSAG_H

#include "usart.h"

#define NILL_DATA            0x7A89C197  //???
#define SYNC_DATA            0x7CD215D8  //???
#define DATA_START           12     //?????????
#define TEXT_OR_NUM          11     //????????????
#define SPEED                10     //?????????
#define PHASE                1     //?????????
#define TXBUFF_SIZE          UARTBUFF_SIZE / 2    //TX?????

extern u16 delay1;
extern u16 delay2;

unsigned long calc_bch_and_parity(unsigned long cw_e);
unsigned long calc_addr(unsigned long add,unsigned char fun);
void GetAddrNumber(void);
void calc_NumberData(void);
void calc_TextData(void);
void Empty_TxBuff(void);
void SendTxBuff(void);
void Send_Num(unsigned long s);
void WaitTIM3(void);
void Send_process(void);

#endif
