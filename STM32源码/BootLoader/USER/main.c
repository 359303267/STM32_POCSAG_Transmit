/***************************************************************************
 * main.c 
 * 适用平台：STM32
 * 编译工具：Keil5
 * 编写：小小小日天
 * 最后编辑者：小小小日天
 * 说明：本项目遵循GPL协议，项目中的代码只可作为个人DIY研究，不可商用，商用需授权。
***************************************************************************/

#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "stmflash.h"   
#include "iap.h"
#include "oled.h"

#define PTT_ON GPIO_ResetBits(GPIOC,GPIO_Pin_4)
#define PTT_OFF GPIO_SetBits(GPIOC,GPIO_Pin_4)
#define DATA_LOW GPIO_ResetBits(GPIOB,GPIO_Pin_3)
#define DATA_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define PTT_LED_ON GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define PTT_LED_OFF GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define DATA_LED_ON GPIO_ResetBits(GPIOC,GPIO_Pin_5)
#define DATA_LED_OFF GPIO_SetBits(GPIOC,GPIO_Pin_5)
#define HC245_OE_LOW GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define HC245_OE_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define HC245_DIR_LOW GPIO_ResetBits(GPIOB,GPIO_Pin_4)
#define HC245_DIR_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define WIFIMOD_ENABLE GPIO_SetBits(GPIOC, GPIO_Pin_10)
#define WIFIMOD_DISABLE GPIO_ResetBits(GPIOC, GPIO_Pin_10)
#define VHFMOD_LOW_POWER GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define VHFMOD_HIGH_POWER GPIO_SetBits(GPIOC, GPIO_Pin_3)

#define READY_ON 0
#define READY_YES 1
#define REC_ROM_DONE 2
#define BOOT_FLASH_ROM 3

int main(void)
{ 
	u8 t;
	u8 status = 255;
	u16 oldcount=0;	//老的串口接收数据值
	u16 applenth=0;	//接收到的app代码长度

	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	WIFIMOD_DISABLE;
	HC245_OE_LOW;
	HC245_DIR_HIGH;
	VHFMOD_LOW_POWER;
	PTT_OFF;
	PTT_LED_OFF;
	DATA_LED_OFF;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为256000
	Initial_M096128x64_ssd1306();
	KEY_Init();				//按键初始化 	
	
	fill_picture(0x00);
	//printf("波特率：115200\r\n");
	while(1)
	{
		if(status  == 255)
			status = BOOT_FLASH_ROM;
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == 0)
		{
			LCD_P8x16Str(0,0,"Update Mode");
			status = READY_ON;
		}
		if(status == READY_ON)
		{
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == 1)
			{
				printf("Has entered update mode.\r\n");
				status = READY_YES;
			}
		}
		if(status == READY_YES)
		{
			if(USART_RX_CNT)
			{
				if(oldcount==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
				{
					applenth=USART_RX_CNT;
					oldcount=0;
					USART_RX_CNT=0;
					printf("ROM reception completed!\r\n");
					printf("ROM size:%dBytes\r\n",applenth);
					status = REC_ROM_DONE;
				}else oldcount=USART_RX_CNT;			
			}
			t++;
			delay_ms(10);
			if(t==30)
			{
				t=0;
			}
		}
		
		if(status == REC_ROM_DONE)
		{
			if(applenth)
			{
				printf("Start updating firmware...\r\n");
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码   
					printf("Firmware update Success!\r\n");	
					//status = BOOT_FLASH_ROM;
					status = READY_YES;
				}else 
				{
					printf("Firmware error!\r\n");
					status = READY_YES;
				}
 			}else 
			{
				printf("No firmware to update!\r\n");
			}
		}
		
		if(status == BOOT_FLASH_ROM)
		{
			printf("Start boot...\r\n\r\n\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else 
			{
				printf("Unable to execute!\r\n");
				status = READY_ON;
			}									 
		}
		
	}   	   
}












