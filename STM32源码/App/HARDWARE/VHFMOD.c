#include "VHFMOD.h"

u8 power_flag = 0;

/**************************
����ģ�鹦�ʣ�
1Ϊ�߹���
0Ϊ�͹���
**************************/
void set_vhfmod_power(char power)
{
	if(power == 0)
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);
	else
		GPIO_SetBits(GPIOC, GPIO_Pin_3);
}
