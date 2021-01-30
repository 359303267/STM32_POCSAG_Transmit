/***************************************************************************
 * pocsag2.c 
 * ����ƽ̨��STM32
 * ���빤�ߣ�Keil5
 * ��д��ССС����
 * ���༭�ߣ�ССС����
 * ˵��������Ŀ��ѭGPLЭ�飬��Ŀ�еĴ���ֻ����Ϊ����DIY�о����������ã���������Ȩ��
***************************************************************************/

#include "pocsag.h"

unsigned char Tx_Num;//��ַ�뷢�����
unsigned char beep;//����λ��1��2��3��4

/**********
�������ܣ�����POCSAG�����е�BCHУ�����żУ��
������δ����У���POCSAG����
����ֵ���Ѿ�У���POCSAG����
**********/
unsigned long calc_bch_and_parity(unsigned long cw_e) //BCHУ�����żУ�麯��
{ 
	unsigned char i;
	unsigned char  parity = 0; //��żУ�����    
	unsigned long local_cw; //��ʱ�����	  
	local_cw=cw_e;//����cw_e����ֵ
	for(i=1;i<=21; i++,cw_e<<=1)	   
		if (cw_e & 0x80000000) cw_e ^= 0xED200000;  
	cw_e=cw_e&0xFFC00000;//����ǰ10λ��BCHУ��ֵ��11λ��ֻ����ǰ10λ��Ч����	  
	local_cw |= (cw_e >> 21); //BCHУ����������22λ��31λ��BCH��10λ������ԭʼ�������
	cw_e=local_cw; 	
	for(i=0; i<31; i++, cw_e<<=1) if(cw_e&0x80000000) parity++;        
	if(parity%2) local_cw+=1;//��1��31λ�ж�Ϊ����������1����Ϊż��
	return local_cw;	 
}  

/**********
�������ܣ������ַ������֡�͹���λ
��������ַ�룬����λ����
����ֵ��POCSAG��ַ����
**********/
unsigned long calc_addr(unsigned long add,unsigned char fun) //��ַת������1����Ϊ��ַ����2����Ϊ����
{
	unsigned long adr;
	unsigned long tem;    
	Tx_Num=(unsigned char)(add&0x00000007);//��ȡ��ַ�����֡λ�Σ�111λ��7֡����3λ��ַ�������ز����ͣ����հ�֡λ��ԭ
	adr=0x00;
	adr=add&0xFFFFFFF8;	//ȥ����ַ���3λ
	adr=adr<<10;  //��ַ����10λ
	tem=0x00;
	tem=fun;  //����λ
	tem=tem<<11;//����λ����11λ������λΪ00 01 10 11����״̬������4����ַ��
	adr=adr|tem; //��ַ��͹���λ�ϳɵ�ַ�룻
	return adr;
}
/**********
�������ܣ��Ӵ��ڻ���������ȡ���ϳɵ�ַ��
��������
����ֵ����
**********/
void GetAddrNumber()
{
	unsigned char i;
	unsigned long tem;
	unsigned long addr_tmp;
	
	addr_tmp = (UartBuff[2] - '0')*1000000;//��ȡ��ַ��
	addr_tmp += (UartBuff[3] - '0')*100000;
	addr_tmp += (UartBuff[4] - '0')*10000;
	addr_tmp += (UartBuff[5] - '0')*1000;
	addr_tmp += (UartBuff[6] - '0')*100;
	addr_tmp += (UartBuff[7] - '0')*10;
	addr_tmp += (UartBuff[8] - '0');
	
	beep = UartBuff[9] - '0';//��ȡ����λ

	tem=calc_addr(addr_tmp,beep);//ǰ���ǵ�ַ�룬������BB����00 01 10 11  ����0��1��2��3�ֲ�ͬ������
	
	for(i = 0; i < 8; i++)
	{
		if(i == Tx_Num)//��ַ������Ӧ��֡��
			TxBuff[i*2] = calc_bch_and_parity(tem);//ȡ��BCHУ���ĵ�ַ������ 
		else
		{
			TxBuff[i*2] = NILL_DATA;//����֡���������
			TxBuff[i*2+1] = NILL_DATA;
		}
	}
}

/**********
�������ܣ��Ӵ�������ȡ���ϳ���ֵ���͵�POCSAG����
��������
����ֵ����
**********/
void calc_NumberData() //������ֵ���ݣ����ֻ���
{
	unsigned long Num_Negate[UARTBUFF_SIZE] = {0};
	unsigned int i, j, k, n;
	unsigned char byte_tmp[10], byte_tmp_negate[10];
	float TxCount = 0.0;
	
	USART1_SendStr("UartBuff:\r\n");
//	for(i = 0; i < UARTBUFF_SIZE;i++)
//	{
//		USART1_SendStr(&UartBuff[i]);  //�Ӵ���������ڻ������������������Ӵ����Ƿ����쳣
//	}
	USART1_SendStr("\r\n");
	
	i = 0;
	for(n = (UartCount - 1 - DATA_START) % 5; n < 5; n++) //POCSAG������ÿ�����ֿ��Դ洢5�����֣���˻���������'0'������ÿո���������
	{
		i++;
		UartBuff[UartCount - 2 + i] = ' ';
		
	}
	
	for(i = DATA_START; i < UARTBUFF_SIZE; i++)  //�����ַ��滻
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
			UartBuff[i] = 0x0E;
		if(UartBuff[i] == 'F' || UartBuff[i] == ']')
			UartBuff[i] = 0x0F;
	}
	
	n = DATA_START;
	for(i = DATA_START; i < UARTBUFF_SIZE; i++)      //���ֵ�ASIIC��ֻ�õ�����λ�����Խ��������ֵĵ���λ�ϲ�������λ����
	{
		if(i % 2 == 0)
			UartBuff[n] = UartBuff[i] << 4;
		else
		{
			UartBuff[n] |= UartBuff[i] & 0x0f;
			n++;
		}
	}
	
	k = Tx_Num * 2 + 1;                               //��λ����ַ����������ڵ�λ��
	for(i = DATA_START; i < UARTBUFF_SIZE; i += 5)                      //����λȡ�������ڸ���λ׼������
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
		
		
		for(j = 0; j < 10; j++)                           //����
		{
			for(n = 0; n < 8; n++)
			{
				byte_tmp_negate[j] <<= 1;
				byte_tmp_negate[j] |= (byte_tmp[j] >> n) & 0x01;
			}
		}
		
		Num_Negate[k] = 0x80000000;                                        //������ϲ���˳��洢����
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
	
	n = Tx_Num * 2 + 1;                                                   //�����ݺϲ���Tx���ͻ�����������У����
	TxCount = 0.0;
	for(i = Tx_Num * 2 + 1; i < UARTBUFF_SIZE; i++)
	{
		if(TxCount >= (UartCount - DATA_START - 1))
			break;
		if(i % 16 == 0)                                                     //ÿ8֡��16�����֣�����һ��ͬ����
			TxBuff[n++] = SYNC_DATA;
		TxBuff[n++] = calc_bch_and_parity( Num_Negate[i]);
		TxCount += 5;                                                     //���ڽ���5�������൱��POCSAG����һ������
		if(n >= TXBUFF_SIZE || i >= UARTBUFF_SIZE)                        //�����������˳�
			break;
	}
	
	USART1_SendStr("\r\nTxBuff:\r\n");
//	for(i = 0; i < TXBUFF_SIZE;i++)
//	{
//		printf("0x%08lx  ",TxBuff[i]);  //�Ӵ���������ͻ������������������Ӵ����Ƿ����쳣
//	}
	USART1_SendStr("\r\n");

}

/**********
�������ܣ��Ӵ�������ȡ���ϳɺ������͵�POCSAG����
��������
����ֵ����
**********/
void calc_TextData()  //���㺺������
{
	unsigned long Text_Negate[UARTBUFF_SIZE] = {0};
	unsigned int n, i, k, byte_tmp;
//	float TxCount = 0.0;
	unsigned char chinese_flag;
	
	USART1_SendStr("UartBuff:\r\n");
//	for(i = 0; i < UARTBUFF_SIZE;i++)
//	{
//		printf("%c",UartBuff[i]);  //�Ӵ���������ڻ������������������Ӵ����Ƿ����쳣
//	}
	USART1_SendStr("\r\n");
	
	
	n = DATA_START;
	if(~((UartBuff[DATA_START] > 0) && (UartBuff[DATA_START] < 127)))
	{
		chinese_flag = 1;  //���Ϊ���֣���chinese_flag��1
		n++;
	}
	if((UartBuff[DATA_START] > 0) && (UartBuff[DATA_START] < 127))
	{
		for(k = UARTBUFF_SIZE; k > DATA_START; k--)  //���Ϊascii�룬�����������������ݴ�ascii�뿪ʼλ�������һ����λ
			UartBuff[k] = UartBuff[k - 1];
		UartBuff[DATA_START] = 0x0F;  //��ascii��ǰ�����0x0F�����ַ�
		chinese_flag = 0;  //��chinese_flag��0
		n+=2;    //����һ���ַ�������ʼ�ж�
	}
	
	while(n < UARTBUFF_SIZE)  //�ں���ǰ�����0x0E�����ַ�����ascii��ǰ�����0x0F�����ַ�
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
	
	
	for(k = DATA_START; k < UARTBUFF_SIZE; k++)         //�ֽڰ�λ����
	{
		byte_tmp = UartBuff[k];
		UartBuff[k] = 0;
		for(n = 0; n < 8; n++)   //����
		{
			UartBuff[k] <<= 1;
			UartBuff[k] |= (byte_tmp >> n) & 0x01;
		}
		UartBuff[k] &= 0xFE;    //����������λ
	}

	k = Tx_Num * 2 + 1;
	for(n = DATA_START; n < UARTBUFF_SIZE; n += 20)                 //��λ��20���ֽ�һѭ��
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
		if(i % 16 == 0)                                                     //ÿ8֡��16�����֣�����һ��ͬ����
			TxBuff[n++] = SYNC_DATA;
		TxBuff[n++] = calc_bch_and_parity( Text_Negate[i]);
	}
	
	USART1_SendStr("\r\nTxBuff:\r\n");
//	for(i = 0; i < TXBUFF_SIZE;i++)
//	{
//		printf("0x%08lx  ",TxBuff[i]);  //�Ӵ���������ͻ������������������Ӵ����Ƿ����쳣
//	}
	USART1_SendStr("\r\n");
}

/**********
�������ܣ���մ��ڻ�������POCSAG���������
��������
����ֵ����
**********/
void Empty_Buff()//��ջ�����
{
	unsigned int i;
	USART1_SendStr("Emptying UartBuffer...\r\n");
	for(i = TEXT_OR_NUM; i < UARTBUFF_SIZE; i++)
	{
		UartBuff[i] = 0x00000000;
	}
	USART1_SendStr("Emptying TxBuffer...\r\n");
	for(i = TEXT_OR_NUM; i < TXBUFF_SIZE; i++)
	{
		TxBuff[i] = 0x00000000;
	}
	USART1_SendStr("Emptied.\r\n");
}

/**********
�������ܣ�POCSAG������ƺ���
��������
����ֵ����
**********/
void SendTxBuff()
{
	unsigned int i;
	USART1_SendStr("Outputting...\r\n");
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);                    //����PTT
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	delay_ms(200);	              //PTT��ʱ�����ݶԽ�����ͬ���е���
	if(UartBuff[SPEED] == 'L')
	{
		TIM3_Config(83,72);		//���ö�ʱ��ֵ
	}
	if(UartBuff[SPEED] == 'H')
	{
		TIM3_Config(83,72);		//���ö�ʱ��ֵ
	}
	 //������ʱ��
	//ES = 0;
	
	for(i = 0; i < 20; i++)//���ٷ���576��ǰ���룬ֻ�ܶ࣬�����٣��˴�����640��
	{
		Send_Num(PREAMBLE);
	}
	
	Send_Num(SYNC_DATA);//����ͬ����

	for(i = 0; i < TXBUFF_SIZE; i++)//��TX���ͻ������е����ݷ���
	{
		if(TxBuff[i] == 0x00000000 || TxBuff[i] == 0xffffffff)//���������������Ϊȫ0��ȫ1���ʾ�������
			break;
		Send_Num(TxBuff[i]);
	}
	Send_Num(NILL_DATA);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	TIM_Cmd(TIM3,DISABLE);
	
	delay_ms(200);                       //�������PTT��ʱ�����ݶԽ�����ͬ���е���
	delay_ms(200);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);                    //����PTT
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	//ES = 1;
	USART1_SendStr("Output ending.\r\n");
}

/**********
�������ܣ����POCSAG����
����������У���POCSAG����
����ֵ����
**********/
void Send_Num(unsigned long s)//��������
{
   unsigned char n; 
   for (n=0;n<32;n++)
    {  
	  if(s&0x80000000)  //�ȷ��͸�λ
		{
			if(UartBuff[1] == 'N')//�ж�������λ����ͬ
			{
				GPIO_ResetBits(GPIOA,GPIO_Pin_5);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			}
			else
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_5);
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
			}
		}
		else
		{
			if(UartBuff[1] == 'N')
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_5);
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
			}
			else
			{
				GPIO_ResetBits(GPIOA,GPIO_Pin_5);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			}
		}
	  WaitTIM3();  //�ȴ���ʱ�����	          
	  s<<=1;      //��һλ
    }   
}

/**********
�������ܣ��ȴ���ʱ�����
��������
����ֵ����
**********/
void WaitTIM3(void)
{
	while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == RESET);
	//while(!TM0_FLAG);//�ȴ���ʱ�����
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//���־λ
}

