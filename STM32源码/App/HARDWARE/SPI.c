#include "spi.h"

//Mini STM32开发板
//SPI 驱动 V1.2
//正点原子@ALIENTEK
//2010/6/13	

//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25X16/24L01/JF24C							  
//SPI口初始化
//这里针是对SPI1的初始化
void SPIx_Init(void)
	{	 
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<2;       //PORTA时钟使能 	 
	//RCC->APB2ENR|=1<<12;      //SPI1时钟使能 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	
	   
	//这里只针对SPI口初始化
	//GPIOA->CRL&=0X000FFFFF; 
	//GPIOA->CRL|=0XBBB00000;//PA5.6.7复用 	    
	//GPIOA->ODR|=0X7<<5;    //PA5.6.7上拉
	/* Configure SPI1 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure I/O for Flash Chip select */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //SPI CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Deselect the FLASH: Chip Select high */
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	
	//SPI1->CR1|=0<<10;//全双工模式	
	//SPI1->CR1|=1<<9; //软件nss管理
	//SPI1->CR1|=1<<8;  
	
	//SPI1->CR1|=1<<2; //SPI主机
	//SPI1->CR1|=0<<11;//8bit数据格式	
	//SPI1->CR1|=1<<1; //空闲模式下SCK为1 CPOL=1
	//SPI1->CR1|=1<<0; //数据采样从第二个时间边沿开始,CPHA=1  
	//SPI1->CR1|=7<<3; //Fsck=Fcpu/256
	//SPI1->CR1|=0<<7; //MSBfirst   
	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	//SPI1->CR1|=1<<6; //SPI设备使能
	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	
	SPIx_ReadWriteByte(0xff);//启动传输		 
	}  
	 
//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
	{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI_SPEED_2)//二分频
	{
	SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
	}else if(SpeedSet==SPI_SPEED_8)//八分频 
	{
	SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
	}else if(SpeedSet==SPI_SPEED_16)//十六分频
	{
	SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
	}else			 	 //256分频
	{
	SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz 低速模式
	}
	//SPI1->CR1|=1<<6; //SPI设备使能	  
	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	} 
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPIx_ReadWriteByte(u8 TxData)
	{		
	u8 retry=0;				 
	//while((SPI1->SR&1<<1)==0)//等待发送区空	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	//SPI1->DR=TxData;	 	  //发送一个byte 
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;
	//while((SPI1->SR&1<<0)==0) //等待接收完一个byte  
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	//return SPI1->DR;          //返回收到的数据
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据				    
	}































