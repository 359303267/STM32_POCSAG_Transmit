#ifndef __IIC_H
#define __IIC_H
//#include "sys.h"
#include "stm32f10x.h"
//Mini STM32开发板
//IIC 驱动函数
//正点原子@ALIENTEK
//2010/6/10 

#define IIC_SCL_PORT              GPIOB
#define IIC_SCL_CLK               RCC_APB2Periph_GPIOB
#define IIC_SCL_PIN               GPIO_Pin_13

#define IIC_SDA_PORT              GPIOB
#define IIC_SDA_CLK               RCC_APB2Periph_GPIOB
#define IIC_SDA_PIN               GPIO_Pin_12

//extern GPIO_InitTypeDef  GPIO_InitStructure;   	   		   
//IO方向设置
//#define SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
//#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}


//IO操作函数	 
//#define IIC_SCL    PCout(12) //SCL
#define Set_IIC_SCL  {GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN);}
#define Clr_IIC_SCL  {GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN);} 
//#define IIC_SDA    PCout(11) //SDA
#define Set_IIC_SDA  {GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN);}
#define Clr_IIC_SDA  {GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN);} 
//#define READ_SDA   PCin(11)  //输入SDA 
#define READ_SDA    (GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN))


//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void SDA_IN(void);
void SDA_OUT(void);
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















