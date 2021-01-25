#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
//Mini STM32开发板
//SPI 驱动 V1.1
//正点原子@ALIENTEK
//2010/6/13	

// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3
						  	    													  
void SPIx_Init(void);			 //初始化SPI口
void SPIx_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPIx_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif

