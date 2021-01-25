#ifndef __USART__H
#define __USART__H

#include "stm32f10x.h"
#include "sys.h"

#define UARTBUFF_SIZE        400    //???????

extern unsigned char UartBuff[UARTBUFF_SIZE];//?????
extern unsigned int UartCount;//??????

void USART1_Config(void);
void USART2_Config(void);
void USART3_Config(void);
void USART1_SendChar(char chr);
void USART1_SendStr(char* str);
void USART2_SendChar(char chr);
void USART2_SendStr(char* str);
void USART3_SendStr(char* str);
void Empty_UartBuff(void);
void Judgment_UartData(void);

#endif
