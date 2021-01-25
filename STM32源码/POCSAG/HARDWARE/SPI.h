#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
//Mini STM32������
//SPI ���� V1.1
//����ԭ��@ALIENTEK
//2010/6/13	

// SPI�����ٶ����� 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3
						  	    													  
void SPIx_Init(void);			 //��ʼ��SPI��
void SPIx_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPIx_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif

