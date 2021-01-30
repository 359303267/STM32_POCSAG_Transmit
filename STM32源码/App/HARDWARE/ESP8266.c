/***************************************************************************
 * ESP8266.c 
 * 适用平台：STM32
 * 编译工具：Keil5
 * 编写：小小小日天
 * 最后编辑者：小小小日天
 * 说明：本项目遵循GPL协议，项目中的代码只可作为个人DIY研究，不可商用，商用需授权。
***************************************************************************/

#include "esp8266.h"
#include "delay.h"

void esp8266_loadconfig()
{
	char set_ssid_cmd[45] = "#WIFI+SET+SSID=";
	char set_psk_cmd[44] = "#WIFI+SET+PSK=";
	char ssid[30];
	char psk[30];
	
	USART2_SendStr("#WIFI+STA=OFF");
	delay_ms(500);
//	write_25qxx_ssid("CMCC-dii4");
//	write_25qxx_psk("ujeqgrfn");
	load_25qxx_ssid((u8*)ssid);
	strncpy(set_ssid_cmd + 15, ssid, 30);
//	USART1_SendStr(set_ssid_cmd);
	USART2_SendStr(set_ssid_cmd);
	delay_ms(100);
	load_25qxx_psk((u8*)psk);
	strncpy(set_psk_cmd + 14, psk, 30);
//	USART1_SendStr(set_psk_cmd);
	USART2_SendStr(set_psk_cmd);
	delay_ms(100);
	USART2_SendStr("#WIFI+STA=ON");
}
