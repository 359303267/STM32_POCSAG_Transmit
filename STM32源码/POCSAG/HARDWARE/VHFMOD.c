#include "VHFMOD.h"

u8 power_flag = 0;

/**************************
设置模块功率：
1为高功率
0为低功率
**************************/
void set_vhfmod_power(char power)
{
	if(power == 0)
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);
	else
		GPIO_SetBits(GPIOC, GPIO_Pin_3);
}
