#include "USART.h"
#include "select.h"
#include "string.h"

unsigned char UartBuff[UARTBUFF_SIZE] = "";//?????
unsigned int UartCount = 0;
u8 tmp;
u8 USART1_IDLE;
u8 USART2_IDLE;
u8 USART3_IDLE;

void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_DeInit(USART1);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
}

void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_DeInit(USART2);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}

void USART3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_DeInit(USART3);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART3, &USART_InitStructure);
	
	USART_Cmd(USART3, ENABLE);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
}

void USART1_SendChar(char chr)
{
	USART_SendData(USART1, chr);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
}

void USART2_SendChar(char chr)
{
	USART_SendData(USART2, chr);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}

void USART1_SendStr(char* str)
{
	while(*str)
	{
		USART_SendData(USART1, *str++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
	}
}

void USART2_SendStr(char* str)
{
	while(*str)
	{
		USART_SendData(USART2, *str++);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
	}
}

void USART3_SendStr(char* str)
{
	while(*str)
	{
		USART_SendData(USART3, *str++);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);
	}
}

void Empty_UartBuff(void)
{
	memset(UartBuff, 0, sizeof(UartBuff));
}

void Judgment_UartData(void)
{
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);
	
	if(UartBuff[0] == '#' && (UartBuff[1] == 'P' || UartBuff[1] == 'N') && (UartBuff[11] == 'T' || UartBuff[11] == 'N'))
		select = START_TX;
	else if(strncmp((char *)UartBuff, "#SET+FREQ=", sizeof("#SET+FREQ=") - 1) == 0)
		select = SET_FREQ;
	else if(strncmp((char *)UartBuff, "#SET+NUM=", sizeof("#SET+NUM=") - 1) == 0)
		select = SET_NUM_CONFIGURE;
	else if(strncmp((char *)UartBuff, "#SET+NU_FR=", sizeof("#SET+NU_FR=") - 1) == 0)
		select = SET_NUM_FREQ;
	else if(strncmp((char *)UartBuff, "#SET+GROUP=", sizeof("#SET+GROUP=") - 1) == 0)
		select = SET_GROUP_CONFIGURE;
	else if(strncmp((char *)UartBuff, "#SET+DISP=", sizeof("#SET+DISP=") - 1) == 0)
		select = SET_DISPLAY;
	else if(strncmp((char *)UartBuff, "#SET+DELAY1=", sizeof("#SET+DELAY1=") - 1) == 0)
		select = SET_DELAY1;
	else if(strncmp((char *)UartBuff, "#SET+DELAY2=", sizeof("#SET+DELAY2=") - 1) == 0)
		select = SET_DELAY2;
	else if(strncmp((char *)UartBuff, "#SET+MESSAGE=", sizeof("#SET+MESSAGE=") - 1) == 0)
		select = SET_MESSAGE;
	else if(strncmp((char *)UartBuff, "#SET+MODPOW=", sizeof("#SET+MODPOW=") - 1) == 0)
		select = SET_VHFMODULE_POWER;
	
	
	else if(strncmp((char *)UartBuff, "#LOAD+NUM", sizeof("#LOAD+NUM") - 1) == 0)
		select = LOAD_NUM_CONFIG;
	else if(strncmp((char *)UartBuff, "#LOAD+GROUP", sizeof("#LOAD+GROUP") - 1) == 0)
		select = LOAD_GROUP_CONFIG;

	else if(strncmp((char *)UartBuff, "#HERD:", sizeof("#HERD:") - 1) == 0)
		select = HERD_CALL;
	else if(strncmp((char *)UartBuff, "#GROUP:", sizeof("#GROUP:") - 1) == 0)
		select = GROUP_CALL;
	
	else if(strncmp((char *)UartBuff, "#WIFI+SET+ON=", sizeof("#WIFI+SET+ON=") - 1) == 0)
		select = WIFI_SET_ONOFF;
	else if(strncmp((char *)UartBuff, "#WIFI+SET+SSID=", sizeof("#WIFI+SET+SSID=") - 1) == 0)
		select = WIFI_SET_SSID;
	else if(strncmp((char *)UartBuff, "#WIFI+SET+PSK=", sizeof("#WIFI+SET+PSK=") - 1) == 0)
		select = WIFI_SET_PSK;
	else if(strncmp((char *)UartBuff, "#WIFI+SAVESSID=", sizeof("#WIFI+SAVESSID=") - 1) == 0)
		select = WIFI_SAVE_SSID;
	else if(strncmp((char *)UartBuff, "#WIFI+SAVEPSK=", sizeof("#WIFI+SAVEPSK=") - 1) == 0)
		select = WIFI_SAVE_PSK;
	else if(strncmp((char *)UartBuff, "#WIFI+STA=ON", sizeof("#WIFI+STA=ON") - 1) == 0)
		select = WIFI_STA_ON;
	else if(strncmp((char *)UartBuff, "#WIFI+STA=OFF", sizeof("WIFI+STA=OFF") - 1) == 0)
		select = WIFI_STA_OFF;
	else if(strncmp((char *)UartBuff, "#WIFI+SHOWIP=", sizeof("#WIFI+SHOWIP=") - 1) == 0)
		select = WIFI_SHOW_IP;
	else if(strncmp((char *)UartBuff, "#WIFI+SHOWRSSI=", sizeof("#WIFI+SHOWRSSI=") - 1) == 0)
		select = WIFI_SHOW_RSSI;
	else if(strncmp((char *)UartBuff, "#WIFI+GETIP", sizeof("#WIFI+GETIP") - 1) == 0)
		select = WIFI_GET_IP;
	else if(strncmp((char *)UartBuff, "#WIFI+GETRSSI", sizeof("#WIFI+GETRSSI") - 1) == 0)
		select = WIFI_GET_RSSI;
	else if(strncmp((char *)UartBuff, "#WIFI+OK", sizeof("#WIFI+OK") - 1) == 0)
		select = WIFI_OK;
	else if(strncmp((char *)UartBuff, "#WIFI+FAIL", sizeof("#WIFI+FAIL") - 1) == 0)
		select = WIFI_FAIL;
	
	else if(strncmp((char *)UartBuff, "#FACTORYRESET", sizeof("#FACTORYRESET") - 1) == 0)
		select = FACTORY_RESET;
	
	else
			select = INPUT_ERROR;

	Select_Fun();
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
	Empty_UartBuff();
	UartCount = 0;
}

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE))//?????????
  {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    /* Read one byte from the receive data register */
    tmp = USART_ReceiveData(USART1);
//		if(tmp == '#')              //????
//			UartCount = 0;
		if(UartCount >= UARTBUFF_SIZE)   //???????
			UartCount = 0;
		UartBuff[UartCount] = tmp;
		UartCount++;                     //????
		
  }
	if(USART_GetITStatus(USART1, USART_IT_IDLE))//?????????
  {
		USART1_IDLE = USART1 -> DR;
		Judgment_UartData();
	}
}

void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE))
  {
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    tmp = USART_ReceiveData(USART2);
//		if(tmp == '#')
//			UartCount = 0;
		if(UartCount >= UARTBUFF_SIZE)
			UartCount = 0;
		UartBuff[UartCount] = tmp;
		UartCount++;
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE))
  {
		USART2_IDLE = USART2 -> DR;
		Judgment_UartData();
	}
}

void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE))
  {
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    tmp = USART_ReceiveData(USART3);
//		if(tmp == '#')
//			UartCount = 0;
		if(UartCount >= UARTBUFF_SIZE)
			UartCount = 0;
		UartBuff[UartCount] = tmp;
		UartCount++;
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE))
  {
		USART3_IDLE = USART3 -> DR;
		Judgment_UartData();
	}
}
