#ifndef __FLASH_H
#define __FLASH_H			    
#include "stm32f10x.h"
//Mini STM32开发板
//W25X16 驱动函数 
//正点原子@ALIENTEK
//2010/6/13
//V1.0

#define SPI_FLASH_CS_PORT              GPIOA
#define SPI_FLASH_CS_CLK               RCC_APB2Periph_GPIOA  
#define SPI_FLASH_CS_PIN               GPIO_Pin_1

//#define	SPI_FLASH_CS PAout(2)  //选中FLASH
#define Set_SPI_FLASH_CS  {GPIO_SetBits(SPI_FLASH_CS_PORT,SPI_FLASH_CS_PIN);}
#define Clr_SPI_FLASH_CS  {GPIO_ResetBits(SPI_FLASH_CS_PORT,SPI_FLASH_CS_PIN);} 					 
////////////////////////////////////////////////////////////////////////////
//W25X16读写
#define FLASH_ID 0XEF14
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F

#define NUM_CONFIG_OFFSET 0
#define NUM_MESSAGE_OFFSET 260
#define GROUP_CONFIG_OFFSET 5200
#define DISP_CONFIG_OFFSET 5700
#define DELAY1_CONFIG_OFFSET 5702
#define DELAY2_CONFIG_OFFSET 5706
#define WIFI_SWITCH_CONFIG_OFFSET 5710
#define WIFI_SSID_CONFIG_OFFSET 5715
#define WIFI_PSK_CONFIG_OFFSET 5750
#define WIFI_ONOFF_CONFIG_OFFSET 5798
#define NUM_FREQ_OFFSET  5800
#define GROUP_FREQ_OFFSET  5900

void SPI_Flash_Init(void);
u16  SPI_Flash_ReadID(void);  	    //读取FLASH ID
u8	 SPI_Flash_ReadSR(void);        //读取状态寄存器 
void SPI_FLASH_Write_SR(u8 sr);  	//写状态寄存器
void SPI_FLASH_Write_Enable(void);  //写使能 
void SPI_FLASH_Write_Disable(void);	//写保护
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void SPI_Flash_Erase_Chip(void);    	  //整片擦除
void SPI_Flash_Erase_Sector(u32 Dst_Addr);//扇区擦除
void SPI_Flash_Wait_Busy(void);           //等待空闲
void SPI_Flash_PowerDown(void);           //进入掉电模式
void SPI_Flash_WAKEUP(void);			  //唤醒

void load_25qxx_freq(u8* str, u8 num);
void write_25qxx_freq(u8 * str, u8 num);
void load_25qxx_group(u8* str, u8 num);
void write_25qxx_group(u8 * str, u8 num);
void load_25qxx_config(u8* str, u8 num);
void write_25qxx_config(u8* str, u8 num);
void load_25qxx_config_message(u8* str, u8 num);
void write_25qxx_config_message(u8* str, u8 num);
void load_25qxx_oled_status(u8* str);
void write_25qxx_oled_status(u8* str);
void load_25qxx_delay1(u8* str);
void write_25qxx_delay1(u8* str);
void load_25qxx_delay2(u8* str);
void write_25qxx_delay2(u8* str);
void load_25qxx_ssid(u8* str);
void write_25qxx_ssid(u8* str);
void load_25qxx_psk(u8* str);
void write_25qxx_psk(u8* str);
void load_25qxx_wifionoff(u8* str);
void write_25qxx_wifionoff(u8* str);
void init_25qxx_all(void);

#endif
















