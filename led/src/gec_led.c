
#include "stm32f4xx.h"

/*
	gec_led_init: 配置LED对应的GPIO引脚
		@void: 无参数

	返回值:
		无返回值
*/
void gec_led_init(void)
{
	/* 定义变量 */
	GPIO_InitTypeDef  GPIO_InitStruct;								//定义初始化GPIO信息结构体变量

	/*	1.使能GPIO分组时钟	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);			//使能GPIOF组时钟

	/*	2.初始化配置GPIO*/
	GPIO_InitStruct.GPIO_Pin		=	GPIO_Pin_9 | GPIO_Pin_10;	
	GPIO_InitStruct.GPIO_Mode		=	GPIO_Mode_OUT;				//输出模式
	GPIO_InitStruct.GPIO_OType		=	GPIO_OType_PP;				//输出推挽
	GPIO_InitStruct.GPIO_Speed		=	GPIO_Speed_50MHz;			//输出速率50MHz
	GPIO_InitStruct.GPIO_PuPd		=	GPIO_PuPd_UP;				//带上拉输出
	GPIO_Init(GPIOF,&GPIO_InitStruct);								//根据上面的赋值,配置了PF9和PF10
}
