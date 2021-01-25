#include "oled.h"
#include "codetab.h"

char pocsag_addr[8];
char pocsag_rate;
char pocsag_phase;

/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   IIC_Send_Byte(0x78);            //Slave address,SA0=0
   IIC_Send_Byte(0x00);			//write command
   IIC_Send_Byte(IIC_Command); 
   IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
   IIC_Start();
   IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
   IIC_Send_Byte(0x40);			//write data
   IIC_Send_Byte(IIC_Data);
   IIC_Stop();
}
/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		Write_IIC_Command(0xb0+m);		//page0-page1
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				Write_IIC_Data(fill_Data);
			}
	}
}
/******************************************
// picture用来显示一个图片
******************************************/
//void Picture()
//{
//  unsigned char x,y;
//  unsigned int i=0;
//  for(y=0;y<8;y++)
//    {
//      Write_IIC_Command(0xb0+y);
//      Write_IIC_Command(0x00);
//      Write_IIC_Command(0x10);
//      for(x=0;x<128;x++)
//        {
//          Write_IIC_Data(show[i++]);
//        }
//    }
//}

void LCD_Set_Pos(unsigned char x, unsigned char y) 
{ 
    Write_IIC_Command(0xb0+y);
    Write_IIC_Command(((x&0xf0)>>4)|0x10);
    Write_IIC_Command((x&0x0f)|0x01); 
} 

void LCD_P8x16Str(unsigned char x, unsigned char y,char ch[])
{
    unsigned char c=0,i=0,j=0;
    while (ch[j]!='\0')
    {    
        c =ch[j]-32;
        if(x>120){x=0;y++;}
        LCD_Set_Pos(x,y);    
        for(i=0;i<8;i++)     
            Write_IIC_Data(F8X16[c*16+i]);
        LCD_Set_Pos(x,y+1);    
        for(i=0;i<8;i++)     
            Write_IIC_Data(F8X16[c*16+i+8]);  
        x+=8;
        j++;
    }
}

void LCD_P6x8Str(unsigned char x, unsigned char y,char ch[])
{
    unsigned char c=0,i=0,j=0;      
    while (ch[j]!='\0')
    {    
        c =ch[j]-32;
        if(x>126){x=0;y++;}
        LCD_Set_Pos(x,y);    
        for(i=0;i<6;i++)     
            Write_IIC_Data(F6x8[c][i]);  
        x+=6;
        j++;
    }
}

void OLED_DisplayOff()
{
	Write_IIC_Command(0xAE);
}

void OLED_DisplayOn()
{
	Write_IIC_Command(0xAF);
}

void Initial_M096128x64_ssd1306()
{
	IIC_Init();
	Write_IIC_Command(0xAE);   //display off
	Write_IIC_Command(0x20);	//Set Memory Addressing Mode	
	Write_IIC_Command(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Write_IIC_Command(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	Write_IIC_Command(0xc8);	//Set COM Output Scan Direction
	Write_IIC_Command(0x00);//---set low column address
	Write_IIC_Command(0x10);//---set high column address
	Write_IIC_Command(0x40);//--set start line address
	Write_IIC_Command(0x81);//--set contrast control register
	Write_IIC_Command(0x7f);
	Write_IIC_Command(0xa1);//--set segment re-map 0 to 127
	Write_IIC_Command(0xa6);//--set normal display
	Write_IIC_Command(0xa8);//--set multiplex ratio(1 to 64)
	Write_IIC_Command(0x3F);//
	Write_IIC_Command(0xa4);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Write_IIC_Command(0xd3);//-set display offset
	Write_IIC_Command(0x00);//-not offset
	Write_IIC_Command(0xd5);//--set display clock divide ratio/oscillator frequency
	Write_IIC_Command(0xf0);//--set divide ratio
	Write_IIC_Command(0xd9);//--set pre-charge period
	Write_IIC_Command(0x22); //
	Write_IIC_Command(0xda);//--set com pins hardware configuration
	Write_IIC_Command(0x12);
	Write_IIC_Command(0xdb);//--set vcomh
	Write_IIC_Command(0x20);//0x20,0.77xVcc
	Write_IIC_Command(0x8d);//--set DC-DC enable
	Write_IIC_Command(0x14);//
	Write_IIC_Command(0xaf);//--turn on oled panel 
}
