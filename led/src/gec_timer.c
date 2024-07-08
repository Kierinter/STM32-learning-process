
#include "stm32f4xx.h"
#include "gec_led.h"

/*
	TIM8_UP_TIM13_IRQHandler: TIM8的更新(溢出)中断 和 TIM13的中断处理
*/
void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM13,TIM_IT_Update)==SET)
	{
		LED1_CHANGE;	//每产生一次定时器13溢出中断,求换LED1的状态

		TIM_ClearITPendingBit(TIM13,TIM_IT_Update);
	}
}


/*
	timer13_interrupt: 配置定时器中断
*/
void timer13_interrupt(void)
{
	TIM_TimeBaseInitTypeDef  t;
	NVIC_InitTypeDef  NVIC_InitStruct;
	/*
		1. 配置时基单元
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);	//使能TIM13时钟

	t.TIM_Prescaler					=	8399;					//配置预分频值,16bits
										/*
											8400分频
											Fin = APB1_CLK * 2 == 84M
											TIM13_CLK = Fin/(TIM_Prescaler+1)
													  =	84000K/8400
													  = 10K
											1个计数脉冲 = 1/10K = 0.1ms
												==> 计数1次需要 0.1ms
										*/
	t.TIM_CounterMode				=	TIM_CounterMode_Up;		//TIM13递增计数器
	t.TIM_Period					=	9999;					//N值
										/*
											定时器会在计数达到 9999+1时产生中断
												0->1	0.1ms
												1->2	0.1ms
												....
												9999 -> 10000 0.1ms

												0->10000  10000*0.1ms ==> 1000ms
										*/
	TIM_TimeBaseInit(TIM13,&t);
	/*
		2. 配置定时器中断
	*/
	TIM_ITConfig(TIM13,TIM_IT_Update,ENABLE);				//使能TIM13溢出(更新)中断

	/*
		3. 配置NVIC
	*/
	NVIC_InitStruct.NVIC_IRQChannel			=	TIM8_UP_TIM13_IRQn;	//TIM8的更新中断和TIM13共用中断请求线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=	2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 	2;
	NVIC_InitStruct.NVIC_IRQChannelCmd		=	ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/*
		4. 开启定时器
	*/
	TIM_Cmd(TIM13,ENABLE);

	/*
		如果需要重复产生中断,开可以使能自动加载
	*/
	TIM_ARRPreloadConfig(TIM13,ENABLE);
}



/*
	timer13_out_init 定时器13输出比较配置
*/
void timer13_out_init(int n_value,int ccr_value)
{
	TIM_OCInitTypeDef  TIM_OCInitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	/*
		1.配置定时器通道对应的GPIO引脚
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	GPIO_InitStruct.GPIO_Pin							=	GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode							=	GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd							=	GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);								//PF8作为定时器13的通道
	/*
		2. 配置时基单元
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);								//使能TIM13时钟

	TIM_TimeBaseInitStruct.TIM_Prescaler				=	8399;						//配置预分频值,16bits
	TIM_TimeBaseInitStruct.TIM_CounterMode				=	TIM_CounterMode_Up;			//TIM13递增计数器
	TIM_TimeBaseInitStruct.TIM_Period					=	n_value;					//N值
	TIM_TimeBaseInit(TIM13,&TIM_TimeBaseInitStruct);

	/*
		配置定时器输出通道
	*/
	TIM_OCInitStruct.TIM_OCMode							=	TIM_OCMode_PWM2;			//	CNT < CCR  输出低电平
																						//  对于递增计数来说,比较值越小,低电平时间越短
	TIM_OCInitStruct.TIM_OutputState					=	TIM_OutputState_Enable;		
	TIM_OCInitStruct.TIM_OCPolarity						=	TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_Pulse							=	ccr_value;					//	比较值 CCR	
	TIM_OC1Init(TIM13,&TIM_OCInitStruct);


	/*
		使能定时器
	*/
	TIM_Cmd(TIM13,ENABLE);
	/*
		如果需要反复输出
	*/
	TIM_OC1PreloadConfig(TIM13,TIM_OCPreload_Enable);									//	比较值自动重载
	TIM_ARRPreloadConfig(TIM13,ENABLE);													//  自动加载计数值

	

}
