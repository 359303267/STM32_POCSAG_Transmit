/***************************************************************************
 * main.c 
 * 适用平台：STM32
 * 编译工具：Keil5
 * 编写：小小小日天
 * 最后编辑者：小小小日天
 * 说明：本项目遵循GPL协议，项目中的代码只可作为个人DIY研究，不可商用，商用需授权。
***************************************************************************/

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "pocsag.h"
#include "gpio.h"
#include "oled.h"
#include "stdio.h"
#include "w25qxx.h"
#include "string.h"
#include "select.h"
#include "vhfmod.h"
#include "stdlib.h"
#include "esp8266.h"

u8 display_flag = 1;
u8 Key0_flag = 1;

void NVIC_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 


	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

void EXTIX_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);


	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);


	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void LED_Blink()
{
	PTT_LED_ON;
	DATA_LED_ON;
	DATA_LOW;
	delay_ms(200);
	PTT_LED_OFF;
	DATA_LED_OFF;
	DATA_HIGH;
	delay_ms(200);
}

void load_PTTDelay()
{
	u8 delay[4];
	load_25qxx_delay1(delay);
	delay1 = (delay[0] - '0') *1000 + (delay[1] - '0') *100 + (delay[2] - '0') *10 + (delay[3] - '0');
	load_25qxx_delay2(delay);
	delay2 = (delay[0] - '0') *1000 + (delay[1] - '0') *100 + (delay[2] - '0') *10 + (delay[3] - '0');
}

int main(void)
{
	u8 tmp[1];
	SCB->VTOR = FLASH_BASE | 0x10000;
	NVIC_Config();
	delay_init();    //初始化延时函数，不加这条语句会导致卡死
	
	GPIO_Config();
	USART1_Config();
	USART2_Config();
	USART3_Config();
	EXTIX_Init();
	HC245_OE_LOW;
	HC245_DIR_HIGH;
	PTT_LED_OFF;
	DATA_LED_OFF;
	PTT_OFF;
	SPI_Flash_Init();
	WIFIMOD_ENABLE;
	set_vhfmod_power(0);
	
	USART1_SendStr("USART1 Test!\r\n");
	USART2_SendStr("USART1 Test!\r\n");
	USART3_SendStr("USART1 Test!\r\n");
	USART3_SendStr("AT+DMOSETGROUP=0,152.7000,152.8250,1,2,1,1\r\n");
	
	Initial_M096128x64_ssd1306();
	//fill_picture(0xff);
	fill_picture(0x00);
	LCD_P8x16Str(0,0,"V1.2");
	LCD_P8x16Str(0,2,"2020-07-21");
	LCD_P8x16Str(0,4,"Create:Honghao");
	delay_ms(1000);
	fill_picture(0x00);
	LCD_P6x8Str(0,0,"Wifi:");
	LCD_P6x8Str(30,0,"AP");
	LCD_P6x8Str(90,0,"Num:");
	LCD_P6x8Str(115,0,"01");
	LCD_P8x16Str(10,2,"Freq:");
	LCD_P8x16Str(50,2,"152.7000");
	LCD_P8x16Str(15,4,"Addr:");
	LCD_P8x16Str(55,4,"0000000");
	LCD_P6x8Str(0, 1,"Rate:");
	LCD_P6x8Str(30,1,"1200");
	LCD_P6x8Str(83,1,"Phase:");
	LCD_P6x8Str(120,1,"P");

	load_PTTDelay();
	
	load_25qxx_oled_status(tmp);
	if(tmp[0] == '0')
		OLED_DisplayOff();
	else
		OLED_DisplayOn();
	
	esp8266_loadconfig();
	
	load_25qxx_wifionoff(tmp);
	if(tmp[0] == '0')
	{
		LCD_P6x8Str(30,0,"Off    ");
		WIFIMOD_DISABLE;
	}
	else
	{
		LCD_P6x8Str(30,0,"AP    ");
		WIFIMOD_ENABLE;
		//USART2_SendStr("#WIFI+STA=ON");
	}
	
	
	//init_25qxx_all();
	
	while(1)
	{
		if(UartBuff[0] == '+' || (UartBuff[0] == '#' && UartBuff[1] == '#'))
		{
			Empty_UartBuff();
			UartCount = 0;
		}
	}
}

void EXTI0_IRQHandler(void)
{
  delay_ms(10);
	if(KEY0==0)
	{
		if(Key0_flag == 0)
		{
			Key0_flag = 1;
			USART2_SendStr("#WIFI+GETIP");
		}
		else if(Key0_flag == 1)
		{
			Key0_flag = 0;
			USART2_SendStr("#WIFI+GETRSSI");
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}
 void EXTI1_IRQHandler(void)
{			
	delay_ms(10);	 
	if(KEY1==0)	{
		if(display_flag)
		{
			OLED_DisplayOff();
			display_flag = 0;
		}
		else
		{
			OLED_DisplayOn();
			display_flag = 1;
		}
	}
 	 EXTI_ClearITPendingBit(EXTI_Line1);
}


void EXTI2_IRQHandler(void)
{
  delay_ms(10); 
  if(KEY2==0)	{
		if(power_flag)
		{
			//set_vhfmod_power(0);
			LCD_P8x16Str(0,6,"                ");
			LCD_P8x16Str(0,6,"   Low  Power   ");
			power_flag = 0;
		}
		else
		{
			//set_vhfmod_power(1);
			LCD_P8x16Str(0,6,"                ");
			LCD_P8x16Str(0,6,"   High Power   ");
			power_flag = 1;
		}
	}
	 EXTI_ClearITPendingBit(EXTI_Line2);
}
