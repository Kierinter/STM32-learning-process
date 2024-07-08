#include "stm32f4xx.h"

/*
	gec_key_init:用于配置KEY按键
*/
void gec_key_init(void)
{
	/* 定义变量 */
	GPIO_InitTypeDef  GPIO_InitStruct;								//定义初始化GPIO信息结构体变量

	/*	1.使能GPIO分组时钟	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);			//使能GPIOA组时钟

	/*	2.初始化配置GPIO*/
	GPIO_InitStruct.GPIO_Pin		=	GPIO_Pin_0;	
	GPIO_InitStruct.GPIO_Mode		=	GPIO_Mode_IN;				//输入模式
	GPIO_InitStruct.GPIO_OType		=	GPIO_OType_PP;				//输出推挽
	GPIO_InitStruct.GPIO_Speed		=	GPIO_Speed_50MHz;			//输出速率50MHz
	GPIO_InitStruct.GPIO_PuPd		=	GPIO_PuPd_UP;				//带上拉输入
	GPIO_Init(GPIOA,&GPIO_InitStruct);								//根据上面的赋值,配置了PF9和PF10
}



