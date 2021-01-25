#ifndef __GPIO_H
#define __GPIO_H

#define PTT_ON GPIO_ResetBits(GPIOC,GPIO_Pin_4)
#define PTT_OFF GPIO_SetBits(GPIOC,GPIO_Pin_4)
#define DATA_LOW GPIO_ResetBits(GPIOB,GPIO_Pin_3)
#define DATA_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define PTT_LED_ON GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define PTT_LED_OFF GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define DATA_LED_ON GPIO_ResetBits(GPIOC,GPIO_Pin_5)
#define DATA_LED_OFF GPIO_SetBits(GPIOC,GPIO_Pin_5)
#define HC245_OE_LOW GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define HC245_OE_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define HC245_DIR_LOW GPIO_ResetBits(GPIOB,GPIO_Pin_4)
#define HC245_DIR_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_4)

#define WIFIMOD_ENABLE GPIO_SetBits(GPIOC, GPIO_Pin_10)
#define WIFIMOD_DISABLE GPIO_ResetBits(GPIOC, GPIO_Pin_10)

#define KEY0 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)
#define KEY1 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)
#define KEY2 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)

void GPIO_Config(void);

#endif
