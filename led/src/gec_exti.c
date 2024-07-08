#include "stm32f4xx.h"
#include "gec_key.h"
#include "gec_led.h"
/*
	gec_exti_init用来配置KEY的外部中断
*/
void gec_exti_init(void)
{
	/*
		定义初始化信息结构体变量
	*/
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	/*
		初始化GPIO控制器
	*/
	gec_key_init();													//该函数初始化了按键对应的GPIO

	/*
		配置SYSCFG选择器
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);	//PA0产生外部中断
	
	/*
		配置EXTI外部中断控制器
	*/
	EXTI_InitStruct.EXTI_Line		=	EXTI_Line0;					//GPIO引脚编号0产生外部中断0
	EXTI_InitStruct.EXTI_Mode		=	EXTI_Mode_Interrupt;		//外部电路产生的为中断模式
	EXTI_InitStruct.EXTI_Trigger	=	EXTI_Trigger_Falling;		//结合原理图,下降沿即按键按下的瞬间
	EXTI_InitStruct.EXTI_LineCmd	=	ENABLE;						//开启外部中断0,会向NVIC发起中断请求
	EXTI_Init(&EXTI_InitStruct);

	/*
		配置NVIC中断控制器
	*/
	NVIC_InitStruct.NVIC_IRQChannel			=	EXTI0_IRQn;			//EXTI0输入中断请求
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=	2;		//没有特殊要求,优先级随意
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			=	2;
	NVIC_InitStruct.NVIC_IRQChannelCmd					=	ENABLE;	//会向CPU报告EXTI0中断
	NVIC_Init(&NVIC_InitStruct);

	/*
		上述步骤完成后,结合原理图可知
			当 KEY0按下那一瞬间就会产生 EXTI0中断
			---> NVIC收到EXTI0的中断请求,就会报告CPU中断产生
				同时,NVIC会打断CPU的正常执行,并且将 EXTI0的中断编号告知CPU(xPSR[7:0])
				CPU就会根据 xPSR[7:0]中断编号在中断向量表中找到对应处理函数的地址
				并跳转到该地址执行相应的中断处理

			因此,您配置了中断,就需要按中断向量表规定的函数名,写一个中断处理函数
			函数名: 可以由用户修改中断向量表中的名称来改变
					但千万不要改变 中断向量表的 顺序!!!!
	*/
	
}


/*
	EXTI0_IRQHandler:EXTI0外部中断0的中断处理函数
					函数名,与中断向量表中的 备注为EXTI Line0  对应的名称一致
					否则EXTI0中断产生后,将无法成功执行处理函数

					这个函数是当产生了EXTI0中断后(KEY0按下 瞬间 )由系统调用
					不需要 用户干涉
*/
void EXTI0_IRQHandler(void)
{
	int i;
	if(EXTI_GetITStatus(EXTI_Line0) == SET)		//由于用户可以令多个中断执行同一个处理函数,
	{											//确保是EXTI0产生的中断
	
		if(KEY0 == KEY_DOWN)					//由于EXTI0可以是任意一组GPIO编号为0的GPIO引脚产生
		{										//确保由KEY0产生的中断(PA0)	
			for(i=0;i<0x6666;i++);
			if(KEY0 == KEY_DOWN)
			{
				LED1_CHANGE;
			}
		}

		EXTI_ClearITPendingBit(EXTI_Line0);		//清除外部中断	
	}
}