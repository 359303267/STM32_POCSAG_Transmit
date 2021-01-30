/***************************************************************************
 * pocsag2.c 
 * 适用平台：STM32
 * 编译工具：Keil5
 * 编写：小小小日天
 * 最后编辑者：小小小日天
 * 说明：本项目遵循GPL协议，项目中的代码只可作为个人DIY研究，不可商用，商用需授权。
***************************************************************************/

#include "pocsag.h"

unsigned char Tx_Num;//地址码发射次序
unsigned char beep;//功能位，1，2，3，4

/**********
函数功能：生成POCSAG编码中的BCH校验和奇偶校验
参数：未进行校验的POCSAG码字
返回值：已经校验的POCSAG码字
**********/
unsigned long calc_bch_and_parity(unsigned long cw_e) //BCH校验和奇偶校验函数
{ 
	unsigned char i;
	unsigned char  parity = 0; //奇偶校验计数    
	unsigned long local_cw; //临时存放数	  
	local_cw=cw_e;//保存cw_e参数值
	for(i=1;i<=21; i++,cw_e<<=1)	   
		if (cw_e & 0x80000000) cw_e ^= 0xED200000;  
	cw_e=cw_e&0xFFC00000;//保留前10位，BCH校验值共11位，只保留前10位有效数据	  
	local_cw |= (cw_e >> 21); //BCH校验数移至第22位到31位，BCH共10位，并和原始数据相加
	cw_e=local_cw; 	
	for(i=0; i<31; i++, cw_e<<=1) if(cw_e&0x80000000) parity++;        
	if(parity%2) local_cw+=1;//从1至31位判断为奇数则后面加1补充为偶数
	return local_cw;	 
}  

/**********
函数功能：计算地址码所在帧和功能位
参数：地址码，功能位数字
返回值：POCSAG地址码字
**********/
unsigned long calc_addr(unsigned long add,unsigned char fun) //地址转换，第1参数为地址，第2参数为功能
{
	unsigned long adr;
	unsigned long tem;    
	Tx_Num=(unsigned char)(add&0x00000007);//获取地址发射的帧位次，111位第7帧，后3位地址数据隐藏不发送，接收按帧位还原
	adr=0x00;
	adr=add&0xFFFFFFF8;	//去掉地址码后3位
	adr=adr<<10;  //地址左移10位
	tem=0x00;
	tem=fun;  //功能位
	tem=tem<<11;//功能位左移11位，功能位为00 01 10 11四种状态，代表4个地址码
	adr=adr|tem; //地址码和功能位合成地址码；
	return adr;
}
/**********
函数功能：从串口缓冲区中提取并合成地址码
参数：无
返回值：无
**********/
void GetAddrNumber()
{
	unsigned char i;
	unsigned long tem;
	unsigned long addr_tmp;
	
	addr_tmp = (UartBuff[2] - '0')*1000000;//提取地址码
	addr_tmp += (UartBuff[3] - '0')*100000;
	addr_tmp += (UartBuff[4] - '0')*10000;
	addr_tmp += (UartBuff[5] - '0')*1000;
	addr_tmp += (UartBuff[6] - '0')*100;
	addr_tmp += (UartBuff[7] - '0')*10;
	addr_tmp += (UartBuff[8] - '0');
	
	beep = UartBuff[9] - '0';//提取功能位

	tem=calc_addr(addr_tmp,beep);//前面是地址码，后面是BB机内00 01 10 11  代表0，1，2，3种不同的声音
	
	for(i = 0; i < 8; i++)
	{
		if(i == Tx_Num)//地址码放入对应的帧中
			TxBuff[i*2] = calc_bch_and_parity(tem);//取得BCH校验后的地址码序列 
		else
		{
			TxBuff[i*2] = NILL_DATA;//其他帧填充闲置码
			TxBuff[i*2+1] = NILL_DATA;
		}
	}
}

/**********
函数功能：从串口中提取并合成数值类型的POCSAG编码
参数：无
返回值：无
**********/
void calc_NumberData() //计算数值数据（数字机）
{
	unsigned long Num_Negate[UARTBUFF_SIZE] = {0};
	unsigned int i, j, k, n;
	unsigned char byte_tmp[10], byte_tmp_negate[10];
	float TxCount = 0.0;
	
	USART1_SendStr("UartBuff:\r\n");
//	for(i = 0; i < UARTBUFF_SIZE;i++)
//	{
//		USART1_SendStr(&UartBuff[i]);  //从串口输出串口缓冲区的内容用来监视串口是否发生异常
//	}
	USART1_SendStr("\r\n");
	
	i = 0;
	for(n = (UartCount - 1 - DATA_START) % 5; n < 5; n++) //POCSAG编码中每个码字可以存储5个数字，因此会产生多余的'0'，因此用空格代替空数据
	{
		i++;
		UartBuff[UartCount - 2 + i] = ' ';
		
	}
	
	for(i = DATA_START; i < UARTBUFF_SIZE; i++)  //表意字符替换
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
	for(i = DATA_START; i < UARTBUFF_SIZE; i++)      //数字的ASIIC码只用到第四位，所以将所有数字的低四位合并，高四位丢弃
	{
		if(i % 2 == 0)
			UartBuff[n] = UartBuff[i] << 4;
		else
		{
			UartBuff[n] |= UartBuff[i] & 0x0f;
			n++;
		}
	}
	
	k = Tx_Num * 2 + 1;                               //定位到地址码后数据所在的位置
	for(i = DATA_START; i < UARTBUFF_SIZE; i += 5)                      //按四位取出并放在高四位准备倒序
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
		
		
		for(j = 0; j < 10; j++)                           //倒序
		{
			for(n = 0; n < 8; n++)
			{
				byte_tmp_negate[j] <<= 1;
				byte_tmp_negate[j] |= (byte_tmp[j] >> n) & 0x01;
			}
		}
		
		Num_Negate[k] = 0x80000000;                                        //重新组合并按顺序存储起来
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
	
	n = Tx_Num * 2 + 1;                                                   //将数据合并到Tx发送缓冲区并计算校验码
	TxCount = 0.0;
	for(i = Tx_Num * 2 + 1; i < UARTBUFF_SIZE; i++)
	{
		if(TxCount >= (UartCount - DATA_START - 1))
			break;
		if(i % 16 == 0)                                                     //每8帧（16个码字）插入一个同步码
			TxBuff[n++] = SYNC_DATA;
		TxBuff[n++] = calc_bch_and_parity( Num_Negate[i]);
		TxCount += 5;                                                     //串口接收5个数字相当于POCSAG码中一个码字
		if(n >= TXBUFF_SIZE || i >= UARTBUFF_SIZE)                        //超出缓冲区退出
			break;
	}
	
	USART1_SendStr("\r\nTxBuff:\r\n");
//	for(i = 0; i < TXBUFF_SIZE;i++)
//	{
//		printf("0x%08lx  ",TxBuff[i]);  //从串口输出发送缓冲区的内容用来监视串口是否发生异常
//	}
	USART1_SendStr("\r\n");

}

/**********
函数功能：从串口中提取并合成汉字类型的POCSAG编码
参数：无
返回值：无
**********/
void calc_TextData()  //计算汉字数据
{
	unsigned long Text_Negate[UARTBUFF_SIZE] = {0};
	unsigned int n, i, k, byte_tmp;
//	float TxCount = 0.0;
	unsigned char chinese_flag;
	
	USART1_SendStr("UartBuff:\r\n");
//	for(i = 0; i < UARTBUFF_SIZE;i++)
//	{
//		printf("%c",UartBuff[i]);  //从串口输出串口缓冲区的内容用来监视串口是否发生异常
//	}
	USART1_SendStr("\r\n");
	
	
	n = DATA_START;
	if(~((UartBuff[DATA_START] > 0) && (UartBuff[DATA_START] < 127)))
	{
		chinese_flag = 1;  //如果为汉字，将chinese_flag置1
		n++;
	}
	if((UartBuff[DATA_START] > 0) && (UartBuff[DATA_START] < 127))
	{
		for(k = UARTBUFF_SIZE; k > DATA_START; k--)  //如果为ascii码，将缓冲区内所有内容从ascii码开始位置向后移一个单位
			UartBuff[k] = UartBuff[k - 1];
		UartBuff[DATA_START] = 0x0F;  //在ascii码前面插入0x0F控制字符
		chinese_flag = 0;  //将chinese_flag置0
		n+=2;    //从下一个字符继续开始判断
	}
	
	while(n < UARTBUFF_SIZE)  //在汉字前面添加0x0E控制字符，在ascii码前面添加0x0F控制字符
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
	
	
	for(k = DATA_START; k < UARTBUFF_SIZE; k++)         //字节按位倒序
	{
		byte_tmp = UartBuff[k];
		UartBuff[k] = 0;
		for(n = 0; n < 8; n++)   //倒序
		{
			UartBuff[k] <<= 1;
			UartBuff[k] |= (byte_tmp >> n) & 0x01;
		}
		UartBuff[k] &= 0xFE;    //倒序后丢弃最低位
	}

	k = Tx_Num * 2 + 1;
	for(n = DATA_START; n < UARTBUFF_SIZE; n += 20)                 //移位，20个字节一循环
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
		if(i % 16 == 0)                                                     //每8帧（16个码字）插入一个同步码
			TxBuff[n++] = SYNC_DATA;
		TxBuff[n++] = calc_bch_and_parity( Text_Negate[i]);
	}
	
	USART1_SendStr("\r\nTxBuff:\r\n");
//	for(i = 0; i < TXBUFF_SIZE;i++)
//	{
//		printf("0x%08lx  ",TxBuff[i]);  //从串口输出发送缓冲区的内容用来监视串口是否发生异常
//	}
	USART1_SendStr("\r\n");
}

/**********
函数功能：清空串口缓冲区和POCSAG输出缓冲区
参数：无
返回值：无
**********/
void Empty_Buff()//清空缓冲区
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
函数功能：POCSAG输出控制函数
参数：无
返回值：无
**********/
void SendTxBuff()
{
	unsigned int i;
	USART1_SendStr("Outputting...\r\n");
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);                    //触发PTT
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	delay_ms(200);	              //PTT延时，根据对讲机不同进行调整
	if(UartBuff[SPEED] == 'L')
	{
		TIM3_Config(83,72);		//设置定时初值
	}
	if(UartBuff[SPEED] == 'H')
	{
		TIM3_Config(83,72);		//设置定时初值
	}
	 //启动定时器
	//ES = 0;
	
	for(i = 0; i < 20; i++)//至少发送576个前导码，只能多，不能少，此处发送640个
	{
		Send_Num(PREAMBLE);
	}
	
	Send_Num(SYNC_DATA);//发送同步码

	for(i = 0; i < TXBUFF_SIZE; i++)//将TX发送缓冲区中的数据发出
	{
		if(TxBuff[i] == 0x00000000 || TxBuff[i] == 0xffffffff)//如果缓冲区中数据为全0或全1则表示发送完毕
			break;
		Send_Num(TxBuff[i]);
	}
	Send_Num(NILL_DATA);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	TIM_Cmd(TIM3,DISABLE);
	
	delay_ms(200);                       //发送完毕PTT延时，根据对讲机不同进行调整
	delay_ms(200);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);                    //触发PTT
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	//ES = 1;
	USART1_SendStr("Output ending.\r\n");
}

/**********
函数功能：输出POCSAG编码
参数：经过校验的POCSAG码字
返回值：无
**********/
void Send_Num(unsigned long s)//发送数据
{
   unsigned char n; 
   for (n=0;n<32;n++)
    {  
	  if(s&0x80000000)  //先发送高位
		{
			if(UartBuff[1] == 'N')//判断正负相位，下同
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
	  WaitTIM3();  //等待计时器溢出	          
	  s<<=1;      //下一位
    }   
}

/**********
函数功能：等待定时器溢出
参数：无
返回值：无
**********/
void WaitTIM3(void)
{
	while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == RESET);
	//while(!TM0_FLAG);//等待定时器溢出
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清标志位
}

