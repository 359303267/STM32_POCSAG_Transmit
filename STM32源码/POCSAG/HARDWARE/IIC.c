#include "IIC.h"
#include "delay.h"
//STM32���ģ��IIC��STM32��Ӳ��IIC̫�����ˣ�
//Mini STM32������
//IIC ��������
//����ԭ��@ALIENTEK
//2010/6/10 
//��ʼ��IIC
void IIC_Init(void)
	{					     
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	IIC_SCL_CLK | IIC_SDA_CLK, ENABLE );	
						  
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN | IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}
	
void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	Set_IIC_SDA;	  	  
	Set_IIC_SCL;
	//delay_us(4);
 	Clr_IIC_SDA;//START:when CLK is high,DATA change form high to low 
	//delay_us(4);
	Clr_IIC_SCL;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	Clr_IIC_SCL;
	Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
 	//delay_us(4);
	Set_IIC_SCL; 
	Set_IIC_SDA;//����I2C���߽����ź�
	//delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
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
	Clr_IIC_SCL;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    Clr_IIC_SCL;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7) 	Set_IIC_SDA
		else Clr_IIC_SDA;
        txd<<=1; 	  
		//delay_us(2);   //��TEA5767��������ʱ���Ǳ����
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



























