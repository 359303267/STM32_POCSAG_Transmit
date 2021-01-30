/***************************************************************************
 * select.c 
 * ?????STM32
 * ?????Keil5
 * ????????
 * ???????????
 * ????????GPL???????????????DIY??????????????
***************************************************************************/

#include "select.h"
#include "string.h"
#include "oled.h"
#include "w25qxx.h"
#include "stdio.h"
#include "usart.h"
#include "pocsag.h"
#include "delay.h"
#include "gpio.h"

//#define DEBUG 1

u8 select;
unsigned char StartTx_Flag = 0;
char pocsag_freq[9];

void Select_Fun(void)
{
	switch(select)
	{
		case IDLE: input_error(); break;
		case START_TX: start_tx(); break;
		case SET_FREQ: set_freq(); break;
		case SET_NUM_CONFIGURE: set_num_configure(); break;
		case SET_GROUP_CONFIGURE: set_group_configure(); break;
		case SET_DISPLAY: set_dispaly(); break;
		case SET_DELAY1: set_PTTdelay(); break;
		case SET_DELAY2: set_PTTdelay(); break;
		case LOAD_NUM_CONFIG: load_num_config(); break;
		case LOAD_GROUP_CONFIG: load_group_config(); break;
		case HERD_CALL: herd_call(); break;
		case GROUP_CALL: group_call(); break;
		case INPUT_ERROR: input_error(); break;
		case SET_NUM_FREQ: set_num_freq(); break;
		case WIFI_SET_SSID: wifi_set_ssid(); break;
		case WIFI_SET_PSK: wifi_set_psk(); break;
		case WIFI_STA_ON: wifi_sta_on(); break;
		case WIFI_STA_OFF: wifi_sta_off(); break;
		case WIFI_SHOW_IP: wifi_show_ip(); break;
		case WIFI_SHOW_RSSI: wifi_show_rssi(); break;
		case WIFI_GET_IP: wifi_get_ip(); break;
		case WIFI_GET_RSSI: wifi_get_rssi(); break;
		case SET_MESSAGE: set_message(); break;
		case WIFI_OK: wifi_ok(); break;
		case WIFI_FAIL: wifi_fail(); break;
		case SET_VHFMODULE_POWER: set_vhfmodule_power(); break;
		case WIFI_SET_ONOFF: wifi_set_onoff(); break;
		case FACTORY_RESET: factory_reset(); break;
		case WIFI_SAVE_SSID: wifi_save_ssid(); break;
		case WIFI_SAVE_PSK: wifi_save_psk(); break;
		default:input_error(); break;
	}
}

void wifi_save_ssid()
{
	char ssid[30];
	memset(ssid, 0x00, sizeof(ssid));
	strncpy(ssid, (char *)UartBuff + 15, 30);
	write_25qxx_ssid((u8*)ssid);
#ifdef DEBUG
	USART1_SendStr(ssid);
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  SAVE SSID OK  ");
#endif
	Empty_UartBuff();
	UartCount = 0;
}

void wifi_save_psk()
{
	char psk[30];
	memset(psk, 0x00, sizeof(psk));
	strncpy(psk, (char *)UartBuff + 14, 30);
	write_25qxx_psk((u8*)psk);
#ifdef DEBUG
	USART1_SendStr(psk);
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  SAVE PSK OK   ");
#endif
	Empty_UartBuff();
	UartCount = 0;
}

void factory_reset()
{
	Empty_UartBuff();
	UartCount = 0;
	fill_picture(0x00);
	init_25qxx_all();
	LCD_P8x16Str(0,0,"Factory reset");
	LCD_P8x16Str(0,2,"Please press the");
	LCD_P8x16Str(0,4,"reset button!");
	
}

void wifi_set_onoff()
{
	char tmp[2];
	strncpy(tmp, (char *)UartBuff + 13, 1);
	write_25qxx_wifionoff((u8 *)tmp);
	if(tmp[0] == '0')
	{
		WIFIMOD_DISABLE;
		LCD_P8x16Str(0,6,"                ");
		LCD_P8x16Str(0,6,"    Wifi off    ");
		LCD_P6x8Str(30,0,"Off    ");
	}
	else
	{
		LCD_P8x16Str(0,6,"                ");
		LCD_P8x16Str(0,6,"    Wifi On     ");
		WIFIMOD_ENABLE;
		LCD_P6x8Str(30,0,"AP    ");
	}
}

void set_vhfmodule_power()
{
	char power[2];
	strncpy(power, (char *)UartBuff + 12, 1);
	if(power[0] == 'H')
	{
		set_vhfmod_power(1);
		LCD_P8x16Str(0,6,"                ");
		LCD_P8x16Str(0,6,"   High Power   ");
	}
	if(power[0] == 'L')
	{
		set_vhfmod_power(0);
		LCD_P8x16Str(0,6,"                ");
		LCD_P8x16Str(0,6,"   Low  Power   ");
	}
	Empty_UartBuff();
	UartCount = 0;
}

void wifi_ok()
{
	LCD_P8x16Str(0,6,"                ");
	LCD_P6x8Str(30,0,"AP+STA");
	USART2_SendStr("#WIFI+GETIP");
}

void wifi_fail()
{
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"   WIFI Fail!   ");
	LCD_P6x8Str(30,0,"AP    ");
}

void wifi_set_ssid()
{
	char ssid[45]="#WIFI+SET+SSID=";
	strncpy(ssid + 15, (char *)UartBuff + 15, 30);
	USART2_SendStr(ssid);
	memset(ssid, 0x00, sizeof(ssid));
	strncpy(ssid, (char *)UartBuff + 15, 30);
	write_25qxx_ssid((u8*)ssid);
#ifdef DEBUG
	USART1_SendStr(ssid);
#endif
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  SET SSID OK   ");
	Empty_UartBuff();
	UartCount = 0;
}
void wifi_set_psk()
{
	char psk[44]="#WIFI+SET+PSK=";
	strncpy(psk + 14, (char *)UartBuff + 14, 30);
	USART2_SendStr(psk);
	memset(psk, 0x00, sizeof(psk));
	strncpy(psk, (char *)UartBuff + 14, 30);
	write_25qxx_psk((u8*)psk);
#ifdef DEBUG
	USART1_SendStr(psk);
#endif
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  SET PSK OK    ");
	Empty_UartBuff();
	UartCount = 0;
}
void wifi_sta_on()
{
	USART2_SendStr("#WIFI+STA=ON");
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  WIFI STA ON   ");
	Empty_UartBuff();
	UartCount = 0;
}
void wifi_sta_off()
{
	USART2_SendStr("#WIFI+STA=OFF");
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  WIFI STA OFF  ");
	Empty_UartBuff();
	UartCount = 0;
}
void wifi_show_ip()
{
	u8 i;
	char IPAddr[17]="                ";
	for(i = 0; i < 15; i++)
	{
		if((UartBuff[i + 13] >= '0' && UartBuff[i + 13] <= '9') || UartBuff[i + 13] == '.')
		{
			IPAddr[i] = UartBuff[i + 13];
		}
		else
			break;
	}
	//strncpy(IPAddr, (char *)UartBuff + 13, 15);
	LCD_P8x16Str(0, 6, (char *)IPAddr);
	Empty_UartBuff();
	UartCount = 0;
}
void wifi_show_rssi()
{
	char lcdbuff[17] = "Wifi RSSI:";
	strncpy(lcdbuff + 10, (char *)UartBuff + 15, 1);
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,lcdbuff);
	Empty_UartBuff();
	UartCount = 0;
}

void wifi_get_ip()
{
	USART2_SendStr("#WIFI+GETIP");
}

void wifi_get_rssi()
{
	USART2_SendStr("#WIFI+GETRSSI");
}

void set_message()
{
	char num;
	char message[200];
	strncpy(message, (char*)UartBuff + 16, sizeof(message));
	num = ((UartBuff[13] - '0') * 10) + (UartBuff[14] - '0');
	write_25qxx_config_message((u8*)message, num);
#ifdef DEBUG
	USART1_SendStr("SET MESSAGE OK");
#endif
}

void herd_call()
{
	char num;
	char num_str[3];
	char selnum[20];
	char load_pre_message_flag;
	char num_config[11];
	char num_freq[8];
	char message[200];
	char set_freqbuff[44] = "AT+DMOSETGROUP=0,150.0250,150.0250,1,2,1,1\r\n";
	if(UartBuff[12] != 0x00)
	{
		if(UartBuff[6] == 'Y')
			load_pre_message_flag = 1;
		else
			load_pre_message_flag = 0;
		strncpy(selnum, (char*)UartBuff + 7, 20);
		strncpy(message, (char*)UartBuff + 28, sizeof(message));
		for(num = 0; num < 20; num++)
		{
			if(selnum[num] == '1')
			{
				sprintf(num_str, "%02d", num + 1);
				LCD_P6x8Str(115,0,num_str);
				load_25qxx_freq((u8 *)num_freq, num);
				strncpy(set_freqbuff + 17, num_freq, 8);
				USART3_SendStr(set_freqbuff);
				memset(pocsag_freq, 0x00, sizeof(pocsag_freq));
				strncpy(pocsag_freq, num_freq, 8);
				delay_ms(400);
				load_25qxx_config((u8 *)num_config, num);
				
				if(UartBuff[0] == '+')
				{
					Empty_UartBuff();
					return;
				}
				Empty_UartBuff();
				UartBuff[0] = '#';
				strncpy((char*)UartBuff + 1, num_config, sizeof(num_config));
				strncpy((char*)UartBuff + 12, message, sizeof(message));
				if(load_pre_message_flag)
				{
					memset((char*)UartBuff + 12, 0x00, sizeof(UartBuff) - 12);
					memset(message, 0x00, sizeof(message));
					load_25qxx_config_message((u8*)message, num);
					strncpy((char*)UartBuff + 12, message, sizeof(message));
				}
				start_tx();
				//USART2_SendStr("HERD OK");
				//USART1_SendStr("Herd call OK");
			}
		}
	}
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  Herd call OK  ");
	Empty_UartBuff();
	UartCount = 0;
	return;
}

void group_call()
{
	char group;
	char num;
	char num_str[3];
	char selnum[20];
	char load_pre_message_flag;
	char num_config[11];
	char num_freq[8];
	char message[200];
	char set_freqbuff[44] = "AT+DMOSETGROUP=0,150.0250,150.0250,1,2,1,1\r\n";
	
	group = ((UartBuff[8] - '0') * 10) + (UartBuff[9] - '0');
	load_25qxx_group((u8*)selnum, group);
	
	if(UartBuff[11] != 0x00)
	{
		if(UartBuff[7] == 'Y')
			load_pre_message_flag = 1;
		else
			load_pre_message_flag = 0;
		strncpy(message, (char*)UartBuff + 11, sizeof(message));
		for(num = 0; num < 20; num++)
		{
			if(selnum[num] == '1')
			{
				sprintf(num_str, "%02d", num + 1);
				LCD_P6x8Str(115,0,num_str);
				load_25qxx_freq((u8 *)num_freq, num);
				strncpy(set_freqbuff + 17, num_freq, 8);
				USART3_SendStr(set_freqbuff);
				memset(pocsag_freq, 0x00, sizeof(pocsag_freq));
				strncpy(pocsag_freq, num_freq, 8);
				delay_ms(400);
				load_25qxx_config((u8 *)num_config, num);
				
				if(UartBuff[0] == '+')
				{
					Empty_UartBuff();
					return;
				}
				Empty_UartBuff();
				UartBuff[0] = '#';
				strncpy((char*)UartBuff + 1, num_config, sizeof(num_config));
				strncpy((char*)UartBuff + 12, message, sizeof(message));
				if(load_pre_message_flag)
				{
					memset((char*)UartBuff + 12, 0x00, sizeof(UartBuff) - 12);
					memset(message, 0x00, sizeof(message));
					load_25qxx_config_message((u8*)message, num);
					strncpy((char*)UartBuff + 12, message, sizeof(message));
				}
				start_tx();
				//USART2_SendStr("Group call OK");
				//USART1_SendStr("Group call OK");
			}
		}
	}
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6," Group call OK  ");	
	Empty_UartBuff();
	UartCount = 0;
	return;
}


void set_num_configure(void)
{
	u16 i;
	u8 num;
	char Buff[11] = "";
	char message[200];
	char lcdbuff[17];
	num = ((UartBuff[9] - '0') * 10) + (UartBuff[10] - '0');
	sprintf(lcdbuff, "%02d Config Saved ", num + 1);
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,lcdbuff);
	for(i = 0; i < 11; i++)
	{
		Buff[i] = UartBuff[i + 12];
	}
	write_25qxx_config((u8*)Buff, num);
	strncpy(message, (char *)UartBuff + 13, 200);
	write_25qxx_config_message((u8 *)message, num);
#ifdef DEBUG
	memset(Buff, 0x00, sizeof(Buff));  //Çå¿Õ»º³å
	memset(message, 0x00, sizeof(message));
	load_25qxx_config((u8*)Buff, num);
	load_25qxx_config_message((u8 *)message, num);
	LCD_P8x16Str(0, 6, (char *)Buff);
	USART1_SendStr(Buff);
	USART1_SendStr(message);
	USART1_SendStr("\r\n");
#endif
	Empty_UartBuff();
	UartCount = 0;
}

void load_group_config()
{
	load_config(TYPE_GROUP);
}
void load_config(u8 type)
{
	u8 num;
	//char num_str[3];
	char Buff[BUFF_SIZE] = "";
	
	for(num = 0; num < 20; num++)
	{
		//USART1_SendStr("&");
		//sprintf(num_str, "%d", num);
		//USART1_SendStr(num_str);
		//USART1_SendStr("=");
		if(type == TYPE_NUM)
		{
			load_25qxx_freq((u8 *)Buff, num);
			USART1_SendStr(Buff);
			load_25qxx_config((u8 *)Buff, num);
			USART1_SendStr(Buff);
			load_25qxx_config_message((u8 *)Buff, num);
			USART1_SendStr(Buff);
		}
		else if(type == TYPE_GROUP)
		{
			load_25qxx_group((u8*)Buff, num);
			USART1_SendStr(Buff);
		}
		//if(num < 19)
			USART1_SendStr("&=");
		memset(Buff, 0x00, sizeof(Buff));
	}
	Empty_UartBuff();
	UartCount = 0;
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,"  Uplode Done   ");
}

void load_num_config()
{
	load_config(TYPE_NUM);
}

void set_freq()
{
	u16 i;
	char Buff[44] = "AT+DMOSETGROUP=0,150.0250,150.0250,1,2,1,1\r\n";
	for(i = 0; i < 8; i++)
	{
		Buff[i + 17] = UartBuff[i + 10];
		pocsag_freq[i] = UartBuff[i + 10];
	}
	USART3_SendStr((char *)Buff);
#ifdef DEBUG
	strncpy(pocsag_freq, (char *)UartBuff + 10, 8);
	USART1_SendStr((char *)pocsag_freq);
#endif
	Empty_UartBuff();
	UartCount = 0;
}

void set_dispaly()
{
	if(UartBuff[10] == '0')
	{
		OLED_DisplayOff();
		write_25qxx_oled_status((u8*)"0");
	}
	else
	{
		OLED_DisplayOn();
		write_25qxx_oled_status((u8*)"1");
	}
	Empty_UartBuff();
	UartCount = 0;
}

void input_error(void)
{
	USART1_SendStr((char *)UartBuff);
	Empty_UartBuff();
	UartCount = 0;
}

void set_PTTdelay()
{
	u8 i;
	u8 delay[4];
	char lcdbuff[17];
	for(i = 0; i < 4; i++)
	{
		delay[i] = UartBuff[i + 12];
	}
	
	if(UartBuff[10] == '1')
	{
		write_25qxx_delay1(delay);
		delay1 = (delay[0] - '0') *1000 + (delay[1] - '0') *100 + (delay[2] - '0') *10 + (delay[3] - '0');
		sprintf(lcdbuff, "PTT Delay1: %04d", delay1);
		LCD_P8x16Str(0,6,"                ");
		LCD_P8x16Str(0,6,lcdbuff);
#ifdef DEBUG
		LCD_P8x16Str(0, 6, (char *)delay);
#endif
	}
	else if(UartBuff[10] == '2')
	{
		write_25qxx_delay2(delay);
		delay2 = (delay[0] - '0') *1000 + (delay[1] - '0') *100 + (delay[2] - '0') *10 + (delay[3] - '0');
		sprintf(lcdbuff, "PTT Delay2: %04d", delay2);
		LCD_P8x16Str(0,6,"                ");
		LCD_P8x16Str(0,6,lcdbuff);
#ifdef DEBUG
		LCD_P8x16Str(0, 6, (char *)delay);
#endif
	}
	
	Empty_UartBuff();
	UartCount = 0;
}

void set_num_freq()
{
	u16 i;
	u8 num;
	char Buff[8];
#ifdef DEBUG
	char lcdbuff[17];
#endif
	num = ((UartBuff[11] - '0') * 10) + (UartBuff[12] - '0');
	for(i = 0; i < 8; i++)
	{
		Buff[i] = UartBuff[i + 14];
	}
	write_25qxx_freq((u8*)Buff, num);
#ifdef DEBUG
	memset(Buff, 0x00, sizeof(Buff));
	load_25qxx_freq((u8*)Buff, num);
	LCD_P8x16Str(0, 6, (char *)Buff);
	USART1_SendStr("\r\nOK\r\n");
	sprintf(lcdbuff, "%02d   Freq Saved", num + 1);
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,lcdbuff);
#endif
	Empty_UartBuff();
	UartCount = 0;
}

void set_group_configure(void)
{
	u16 i;
	u8 group;
	char Buff[20];
	char lcdbuff[17];
	group = ((UartBuff[11] - '0') * 10) + (UartBuff[12] - '0');
	sprintf(lcdbuff, "%02d Group Saved  ", group + 1);
	LCD_P8x16Str(0,6,"                ");
	LCD_P8x16Str(0,6,lcdbuff);
	for(i = 0; i < 20; i++)
	{
		Buff[i] = UartBuff[i + 14];
	}
	write_25qxx_group((u8*)Buff, group);
#ifdef DEBUG
	memset(Buff, 0x00, sizeof(Buff));
	load_25qxx_group((u8*)Buff, group);
	LCD_P8x16Str(0, 6, (char *)Buff);
#endif
	USART1_SendStr("\r\nOK\r\n");
	Empty_UartBuff();
	UartCount = 0;
}

void start_tx()
{
	USART1_SendStr("\r\nOK\r\n");
	StartTx_Flag = 1;
	Send_process();
	Empty_UartBuff();
	UartCount = 0;
}
