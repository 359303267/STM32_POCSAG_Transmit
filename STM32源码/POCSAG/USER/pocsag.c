#include "pocsag.h"
#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "tim.h"
#include "gpio.h"
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "select.h"

//#define DEBUG 1

unsigned long TxBuff[TXBUFF_SIZE] = {0};//TX?????
unsigned char Tx_Num;//???????
unsigned char beep;//???,1,2,3,4
u16 delay1 = 200;
u16 delay2 = 200;


/**********
????:??POCSAG????BCH???????
??:??????POCSAG??
???:?????POCSAG??
**********/
unsigned long calc_bch_and_parity(unsigned long cw_e) //BCH?????????
{ 
	unsigned char i;
	unsigned char  parity = 0; //??????    
	unsigned long local_cw; //?????	  
	local_cw=cw_e;//??cw_e???
	for(i=1;i<=21; i++,cw_e<<=1)	   
		if (cw_e & 0x80000000) cw_e ^= 0xED200000;  
	cw_e=cw_e&0xFFC00000;//???10?,BCH????11?,????10?????	  
	local_cw |= (cw_e >> 21); //BCH??????22??31?,BCH?10?,????????
	cw_e=local_cw; 	
	for(i=0; i<31; i++, cw_e<<=1) if(cw_e&0x80000000) parity++;        
	if(parity%2) local_cw+=1;//?1?31??????????1?????
	return local_cw;	 
}  

/**********
????:????????????
??:???,?????
???:POCSAG????
**********/
unsigned long calc_addr(unsigned long add,unsigned char fun) //????,?1?????,?2?????
{
	unsigned long adr;
	unsigned long tem;    
	Tx_Num=(unsigned char)(add&0x00000007);//??????????,111??7?,?3??????????,???????
	adr=0x00;
	adr=add&0xFFFFFFF8;	//??????3?
	adr=adr<<10;  //????10?
	tem=0x00;
	tem=fun;  //???
	tem=tem<<11;//?????11?,????00 01 10 11????,??4????
	adr=adr|tem; //????????????;
	return adr;
}
 
/**********
????:???????
??:?
???:?
**********/
void WaitTIM3(void)
{
	while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == RESET);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//????
}


/**********
????:??POCSAG??
??:?????POCSAG??
???:?
**********/
void Send_Num(unsigned long s)//????
{
   unsigned char n; 
   for (n=0;n<32;n++)
    {  
	  if(s&0x80000000)  //?????
		{
			if(UartBuff[1] == 'N')//??????,??
			{
				DATA_LOW;
				DATA_LED_ON;
			}
			else
			{
				DATA_HIGH;
				DATA_LED_OFF;
			}
		}
		else
		{
			if(UartBuff[1] == 'N')
			{
				DATA_HIGH;
				DATA_LED_OFF;
			}
			else
			{
				DATA_LOW;
				DATA_LED_ON;
			}
		}
	  WaitTIM3();  //???????	          
	  s<<=1;      //???
    }   
}
/**********
????:????????POCSAG?????
??:?
???:?
**********/
void Empty_TxBuff()//?????
{
	memset(TxBuff, 0x00, sizeof(TxBuff));
#ifdef DEBUG
	USART1_SendStr("TxBuff Emptied.\r\n");
#endif
}

/**********
????:???????????????
??:?
???:?
**********/
void GetAddrNumber()
{
	unsigned char i;
	unsigned long tem;
	unsigned long addr_tmp;
	
	addr_tmp = (UartBuff[2] - '0')*1000000;//?????
	addr_tmp += (UartBuff[3] - '0')*100000;
	addr_tmp += (UartBuff[4] - '0')*10000;
	addr_tmp += (UartBuff[5] - '0')*1000;
	addr_tmp += (UartBuff[6] - '0')*100;
	addr_tmp += (UartBuff[7] - '0')*10;
	addr_tmp += (UartBuff[8] - '0');

	sprintf(pocsag_addr, "%07ld", addr_tmp);
	
	beep = UartBuff[9] - '0';//?????

	tem=calc_addr(addr_tmp,beep);//??????,???BB??00 01 10 11  ??0,1,2,3??????
	
	for(i = 0; i < 8; i++)
	{
		if(i == Tx_Num)//??????????
			TxBuff[i*2] = calc_bch_and_parity(tem);//??BCH????????? 
		else
		{
			TxBuff[i*2] = NILL_DATA;//????????
			TxBuff[i*2+1] = NILL_DATA;
		}
	}
}

/**********
????:??????????????POCSAG??
??:?
???:?
**********/
void calc_NumberData() //??????(???)
{
	unsigned long Num_Negate[UARTBUFF_SIZE] = {0};
	unsigned int i, j, k, n;
	unsigned char byte_tmp[10], byte_tmp_negate[10];
	float TxCount = 0.0;
	
#ifdef DEBUG
	USART1_SendStr("UartBuff:\r\n");
	for(i = 0; i < UARTBUFF_SIZE;i++)
	{
		printf("%c",UartBuff[i]);  //?????????????????????????
	}
	USART1_SendStr("\r\n");
#endif
	
	i = 0;
	for(n = (UartCount - 1 - DATA_START) % 5; n < 5; n++) //POCSAG???????????5???,????????'0',??????????
	{
		i++;
		UartBuff[UartCount - 1 + i] = ' ';
		
	}
	
	for(i = DATA_START; i < UARTBUFF_SIZE; i++)  //??????
	{
		if(UartBuff[i] == 'A' || UartBuff[i] == '\n')
			UartBuff[i] = 0x0A;
		if(UartBuff[i] == 'B' || UartBuff[i] == 'U')
			UartBuff[i] = 0x0B;
		if(UartBuff[i] == 'C' || UartBuff[i] == ' ')
			UartBuff[i] = 0x0C;
		if(UartBuff[i] == 'D' || UartBuff[i] == '-')
			UartBuff[i] = 0x0D;
		if(UartBuff[i] == 'E' || UartBuff[i] == '[')
			UartBuff[i] = 0x0F;
		if(UartBuff[i] == 'F' || UartBuff[i] == ']')
			UartBuff[i] = 0x0E;
	}
	
	n = DATA_START;
	for(i = DATA_START; i < UARTBUFF_SIZE; i++)      //???ASIIC???????,?????????????,?????
	{
		if(i % 2 == 0)
			UartBuff[n] = UartBuff[i] << 4;
		else
		{
			UartBuff[n] |= UartBuff[i] & 0x0f;
			n++;
		}
	}
	
	k = Tx_Num * 2 + 1;                               //??????????????
	for(i = DATA_START; i < UARTBUFF_SIZE; i += 5)                      //???????????????
	{
		byte_tmp[0] = (UartBuff[i] & 0xf0);
		byte_tmp[1] = (UartBuff[i] & 0x0f) << 4;
		byte_tmp[2] = (UartBuff[i + 1] & 0xf0);
		byte_tmp[3] = (UartBuff[i + 1] & 0x0f) << 4;
		byte_tmp[4] = (UartBuff[i + 2] & 0xf0);
		byte_tmp[5] = (UartBuff[i + 2] & 0x0f) << 4;
		byte_tmp[6] = (UartBuff[i + 3] & 0xf0);
		byte_tmp[7] = (UartBuff[i + 3] & 0x0f) << 4;
		byte_tmp[8] = (UartBuff[i + 4] & 0xf0);
		byte_tmp[9] = (UartBuff[i + 4] & 0x0f) << 4;
		
		
		for(j = 0; j < 10; j++)                           //??
		{
			for(n = 0; n < 8; n++)
			{
				byte_tmp_negate[j] <<= 1;
				byte_tmp_negate[j] |= (byte_tmp[j] >> n) & 0x01;
			}
		}
		
		Num_Negate[k] = 0x80000000;                                        //????????????
		Num_Negate[k] |= (unsigned long)byte_tmp_negate[0] << 27;
		Num_Negate[k] |= (unsigned long)byte_tmp_negate[1] << 23;
		Num_Negate[k] |= (unsigned long)byte_tmp_negate[2] << 19;
		Num_Negate[k] |= (unsigned long)byte_tmp_negate[3] << 15;
		Num_Negate[k] |= (unsigned long)byte_tmp_negate[4] << 11;
		Num_Negate[k + 1] = 0x80000000;
		Num_Negate[k + 1] |= (unsigned long)byte_tmp_negate[5] << 27;
		Num_Negate[k + 1] |= (unsigned long)byte_tmp_negate[6] << 23;
		Num_Negate[k + 1] |= (unsigned long)byte_tmp_negate[7] << 19;
		Num_Negate[k + 1] |= (unsigned long)byte_tmp_negate[8] << 15;
		Num_Negate[k + 1] |= (unsigned long)byte_tmp_negate[9] << 11;
		k = k + 2;
		
	}
	
	n = Tx_Num * 2 + 1;                                                   //??????Tx???????????
	TxCount = 0.0;
	for(i = Tx_Num * 2 + 1; i < UARTBUFF_SIZE; i++)
	{
		if(TxCount >= (UartCount - DATA_START - 1))
			break;
		if(i % 16 == 0)                                                     //?8?(16???)???????
			TxBuff[n++] = SYNC_DATA;
		TxBuff[n++] = calc_bch_and_parity( Num_Negate[i]);
		TxCount += 5;                                                     //????5??????POCSAG??????
		if(n >= TXBUFF_SIZE || i >= UARTBUFF_SIZE)                        //???????
			break;
	}
#ifdef DEBUG
	USART1_SendStr("\r\nTxBuff:\r\n");
	for(i = 0; i < TXBUFF_SIZE;i++)
	{
		printf("0x%08lx  ",TxBuff[i]);  //?????????????????????????
	}
	USART1_SendStr("\r\n");
#endif
}

/**********
????:??????????????POCSAG??
??:?
???:?
**********/
void calc_TextData()  //??????
{
	unsigned long Text_Negate[UARTBUFF_SIZE] = {0};
	unsigned int n, i, k, byte_tmp;
	unsigned char chinese_flag;
	
#ifdef DEBUG
	USART1_SendStr("UartBuff:\r\n");
	for(i = 0; i < UARTBUFF_SIZE;i++)
	{
		printf("%c",UartBuff[i]);  //?????????????????????????
	}
	USART1_SendStr("\r\n");
	
#endif
	n = DATA_START;
	if(~((UartBuff[DATA_START] > 0) && (UartBuff[DATA_START] < 127)))
	{
		chinese_flag = 1;  //?????,?chinese_flag?1
		n++;
	}
	if((UartBuff[DATA_START] > 0) && (UartBuff[DATA_START] < 127))
	{
		for(k = UARTBUFF_SIZE; k > DATA_START; k--)  //???ascii?,??????????ascii????????????
			UartBuff[k] = UartBuff[k - 1];
		UartBuff[DATA_START] = 0x0F;  //?ascii?????0x0F????
		chinese_flag = 0;  //?chinese_flag?0
		n+=2;    //????????????
	}
	
	while(n < UARTBUFF_SIZE)  //???????0x0E????,?ascii?????0x0F????
	{
		if((!((UartBuff[n] > 0) && (UartBuff[n] <= 127))) && chinese_flag == 0)
		{
			for(k = UARTBUFF_SIZE; k > n; k--)
				UartBuff[k] = UartBuff[k - 1];
			UartBuff[n] = 0x0E;
			chinese_flag = 1;
			n+=2;
		}
		else if((UartBuff[n] > 0) && (UartBuff[n] <= 127) && chinese_flag == 1)
		{
			for(k = UARTBUFF_SIZE; k > n; k--)
				UartBuff[k] = UartBuff[k - 1];
			UartBuff[n] = 0x0F;
			chinese_flag = 0;
			n+=2;
		}
		else
			n++;
	}
	
	
	for(k = DATA_START; k < UARTBUFF_SIZE; k++)         //??????
	{
		byte_tmp = UartBuff[k];
		UartBuff[k] = 0;
		for(n = 0; n < 8; n++)   //??
		{
			UartBuff[k] <<= 1;
			UartBuff[k] |= (byte_tmp >> n) & 0x01;
		}
		UartBuff[k] &= 0xFE;    //????????
	}

	k = Tx_Num * 2 + 1;
	for(n = DATA_START; n < UARTBUFF_SIZE; n += 20)                 //??,20??????
	{
		Text_Negate[k] = 0x80000000;
		Text_Negate[k] |= (unsigned long)UartBuff[n] << 23;
		Text_Negate[k] |= (unsigned long)UartBuff[n + 1] << 16;
		Text_Negate[k] |= (unsigned long)UartBuff[n + 2] << 9;
		Text_Negate[k] &= 0xfffff800;
		
		Text_Negate[k + 1] = 0x80000000;
		Text_Negate[k + 1] |= (unsigned long)UartBuff[n + 2] << 29;
		Text_Negate[k + 1] &= 0xC0000000;
		Text_Negate[k + 1] |= (unsigned long)UartBuff[n + 3] << 22;
		Text_Negate[k + 1] |= (unsigned long)UartBuff[n + 4] << 15;
		Text_Negate[k + 1] |= (unsigned long)UartBuff[n + 5] << 8;
		Text_Negate[k + 1] &= 0xfffff800;
		
		Text_Negate[k + 2] = 0x80000000;
		Text_Negate[k + 2] |= (unsigned long)UartBuff[n + 5] << 28;
		Text_Negate[k + 2] &= 0xE0000000;
		Text_Negate[k + 2] |= (unsigned long)UartBuff[n + 6] << 21;
		Text_Negate[k + 2] |= (unsigned long)UartBuff[n + 7] << 14;
		Text_Negate[k + 2] |= (unsigned long)UartBuff[n + 8] << 7;
		Text_Negate[k + 2] &= 0xfffff800;
		
		Text_Negate[k + 3] = 0x80000000;
		Text_Negate[k + 3] |= (unsigned long)UartBuff[n + 8] << 27;
		Text_Negate[k + 3] &= 0xF0000000;
		Text_Negate[k + 3] |= (unsigned long)UartBuff[n + 9] << 20;
		Text_Negate[k + 3] |= (unsigned long)UartBuff[n + 10] << 13;
		Text_Negate[k + 3] |= (unsigned long)UartBuff[n + 11] << 6;
		Text_Negate[k + 3] &= 0xfffff800;
		
		Text_Negate[k + 4] = 0x80000000;
		Text_Negate[k + 4] |= (unsigned long)UartBuff[n + 11] << 26;
		Text_Negate[k + 4] &= 0xF8000000;
		Text_Negate[k + 4] |= (unsigned long)UartBuff[n + 12] << 19;
		Text_Negate[k + 4] |= (unsigned long)UartBuff[n + 13] << 12;
		Text_Negate[k + 4] |= (unsigned long)UartBuff[n + 14] << 5;
		Text_Negate[k + 4] &= 0xfffff800;
		
		Text_Negate[k + 5] = 0x80000000;
		Text_Negate[k + 5] |= (unsigned long)UartBuff[n + 14] << 25;
		Text_Negate[k + 5] &= 0xFC000000;
		Text_Negate[k + 5] |= (unsigned long)UartBuff[n + 15] << 18;
		Text_Negate[k + 5] |= (unsigned long)UartBuff[n + 16] << 11;
		Text_Negate[k + 5] |= (unsigned long)UartBuff[n + 17] << 4;
		Text_Negate[k + 5] &= 0xfffff800;
		
		Text_Negate[k + 6] = 0x80000000;
		Text_Negate[k + 6] |= (unsigned long)UartBuff[n + 17] << 24;
		Text_Negate[k + 6] &= 0xFE000000;
		Text_Negate[k + 6] |= (unsigned long)UartBuff[n + 18] << 17;
		Text_Negate[k + 6] |= (unsigned long)UartBuff[n + 19] << 10;
		Text_Negate[k + 6] &= 0xfffff800;
		
		k += 7;
	}
	
	n = Tx_Num * 2 + 1;
	for(i = Tx_Num * 2 + 1; i < UARTBUFF_SIZE; i++)
	{
		if(Text_Negate[i] == 0x80000000)
			break;
		if(i % 16 == 0)                                                     //?8?(16???)???????
			TxBuff[n++] = SYNC_DATA;
		TxBuff[n++] = calc_bch_and_parity( Text_Negate[i]);
	}
	
#ifdef DEBUG
	USART1_SendStr("\r\nTxBuff:\r\n");
	for(i = 0; i < TXBUFF_SIZE;i++)
	{
		printf("0x%08lx  ",TxBuff[i]);  //?????????????????????????
	}
	USART1_SendStr("\r\n");
#endif
	
}

/**********
????:POCSAG??????
??:?
???:?
**********/
void SendTxBuff()
{
	unsigned int i;
	USART1_SendStr("Outputting...\r\n");
	set_vhfmod_power(power_flag);
	PTT_ON;                    //??PTT
	PTT_LED_ON;
	delay_ms(delay1);	              //PTT??,???????????
	if(UartBuff[SPEED] == 'L')
	{
		TIM3_Config(1926,72);		//??????
	}
	if(UartBuff[SPEED] == 'H')
	{
		TIM3_Config(821,72);		//??????
	}
	if(UartBuff[SPEED] == 'S')
	{
		TIM3_Config(410,72);		//??????
	}
	
	for(i = 0; i < 20; i++)//????576????,???,???,????640?
	{
		Send_Num(0xAAAAAAAA);
	}
	
	Send_Num(SYNC_DATA);//?????

	for(i = 0; i < TXBUFF_SIZE; i++)//?TX???????????
	{
		if(TxBuff[i] == 0x00000000 || TxBuff[i] == 0xffffffff)//??????????0??1???????
			break;
		Send_Num(TxBuff[i]);
	}
	Send_Num(NILL_DATA);
	TIM_Cmd(TIM3,DISABLE);
	delay_ms(delay2);
	PTT_OFF;                    //??PTT
	set_vhfmod_power(0);
	PTT_LED_OFF;
	DATA_LED_OFF;
	USART1_SendStr("Output ending.\r\n");
}

void Send_process()
{
	if(StartTx_Flag == 1 && (UartBuff[TEXT_OR_NUM] == 'N' || UartBuff[TEXT_OR_NUM] == 'T'))
		{
			StartTx_Flag = 0;
			LCD_P8x16Str(0,6,"                ");
			LCD_P8x16Str(0,6,"   Sending...   ");
			GetAddrNumber();//?????????
			LCD_P8x16Str(55,4,pocsag_addr);
			LCD_P8x16Str(50,2,pocsag_freq);
			pocsag_rate = UartBuff[SPEED];
			if(pocsag_rate == 'S')
				LCD_P6x8Str(30,1,"2400");
			if(pocsag_rate == 'H')
				LCD_P6x8Str(30,1,"1200");
			if(pocsag_rate == 'L')
				LCD_P6x8Str(30,1,"512 ");
			pocsag_phase = UartBuff[PHASE];
			if(pocsag_phase == 'P')
				LCD_P6x8Str(120,1,"P");
			if(pocsag_phase == 'N')
				LCD_P6x8Str(120,1,"N");
			if(UartBuff[TEXT_OR_NUM] == 'N')
				calc_NumberData();//??????
			if(UartBuff[TEXT_OR_NUM] == 'T')
				calc_TextData();//??????
			SendTxBuff();//????
			Empty_TxBuff();//??TX?????????????
			LCD_P8x16Str(0,6,"                ");
			LCD_P8x16Str(0,6," Send Success!  ");
			delay_ms(100);
		}
}
