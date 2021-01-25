#ifndef	__OLED_H
#define	__OLED_H

/*************Pin Define***************/
#include "iic.h"

#define I2CCFG          (*(unsigned char volatile xdata *)0xfe80)
#define I2CMSCR         (*(unsigned char volatile xdata *)0xfe81)
#define I2CMSST         (*(unsigned char volatile xdata *)0xfe82)
#define I2CSLCR         (*(unsigned char volatile xdata *)0xfe83)
#define I2CSLST         (*(unsigned char volatile xdata *)0xfe84)
#define I2CSLADR        (*(unsigned char volatile xdata *)0xfe85)
#define I2CTXD          (*(unsigned char volatile xdata *)0xfe86)
#define I2CRXD          (*(unsigned char volatile xdata *)0xfe87)

extern char pocsag_addr[8];
extern char pocsag_rate;
extern char pocsag_phase;

void Initial_M096128x64_ssd1306(void);
void fill_picture(unsigned char fill_Data);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
void LCD_Set_Pos(unsigned char x, unsigned char y);
void LCD_P8x16Str(unsigned char x, unsigned char y,char ch[]);
void LCD_P6x8Str(unsigned char x, unsigned char y,char ch[]);
void OLED_DisplayOff(void);
void OLED_DisplayOn(void);

#endif
