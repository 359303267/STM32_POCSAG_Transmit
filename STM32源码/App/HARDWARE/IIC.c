#include "IIC.h"
#include "delay.h"
//STM32软件模拟IIC，STM32的硬件IIC太难用了！
//Mini STM32开发板
//IIC 驱动函数
//正点原子@ALIENTEK
//2010/6/10 
//初始化IIC
void IIC_Init(void)
	{					     
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	IIC_SCL_CLK | IIC_SDA_CLK, ENABLE );	
						  
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN | IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
	Set_IIC_SCL;
	Set_IIC_SDA;
	}
	
void SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}
	
void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	Set_IIC_SDA;	  	  
	Set_IIC_SCL;
	//delay_us(4);
 	Clr_IIC_SDA;//START:when CLK is high,DATA change form high to low 
	//delay_us(4);
	Clr_IIC_SCL;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	Clr_IIC_SCL;
	Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
 	//delay_us(4);
	Set_IIC_SCL; 
	Set_IIC_SDA;//发送I2C总线结束信号
	//delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	Set_IIC_SDA;//delay_us(1);   
	Set_IIC_SCL;//delay_us(1); 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	Clr_IIC_SCL;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Clr_IIC_SDA;
	//delay_us(2);
	Set_IIC_SCL;
	//delay_us(2);
	Clr_IIC_SCL;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Set_IIC_SDA;
	//delay_us(2);
	Set_IIC_SCL;
	//delay_us(2);
	Clr_IIC_SCL;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    Clr_IIC_SCL;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7) 	Set_IIC_SDA
		else Clr_IIC_SDA;
        txd<<=1; 	  
		//delay_us(2);   //对TEA5767这三个延时都是必须的
		Set_IIC_SCL;
		//delay_us(2); 
		Clr_IIC_SCL;	
		//delay_us(2);
    }
		Set_IIC_SCL;
		//delay_us(2); 
		Clr_IIC_SCL;	
		//delay_us(2);		
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        Clr_IIC_SCL; 
        //delay_us(2);
		Set_IIC_SCL;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



























