
#include "stm32f4xx.h"
#include "gec_led.h"

/*
	TIM8_UP_TIM13_IRQHandler: TIM8�ĸ���(���)�ж� �� TIM13���жϴ���
*/
void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM13,TIM_IT_Update)==SET)
	{
		LED1_CHANGE;	//ÿ����һ�ζ�ʱ��13����ж�,��LED1��״̬

		TIM_ClearITPendingBit(TIM13,TIM_IT_Update);
	}
}


/*
	timer13_interrupt: ���ö�ʱ���ж�
*/
void timer13_interrupt(void)
{
	TIM_TimeBaseInitTypeDef  t;
	NVIC_InitTypeDef  NVIC_InitStruct;
	/*
		1. ����ʱ����Ԫ
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);	//ʹ��TIM13ʱ��

	t.TIM_Prescaler					=	8399;					//����Ԥ��Ƶֵ,16bits
										/*
											8400��Ƶ
											Fin = APB1_CLK * 2 == 84M
											TIM13_CLK = Fin/(TIM_Prescaler+1)
													  =	84000K/8400
													  = 10K
											1���������� = 1/10K = 0.1ms
												==> ����1����Ҫ 0.1ms
										*/
	t.TIM_CounterMode				=	TIM_CounterMode_Up;		//TIM13����������
	t.TIM_Period					=	9999;					//Nֵ
										/*
											��ʱ�����ڼ����ﵽ 9999+1ʱ�����ж�
												0->1	0.1ms
												1->2	0.1ms
												....
												9999 -> 10000 0.1ms

												0->10000  10000*0.1ms ==> 1000ms
										*/
	TIM_TimeBaseInit(TIM13,&t);
	/*
		2. ���ö�ʱ���ж�
	*/
	TIM_ITConfig(TIM13,TIM_IT_Update,ENABLE);				//ʹ��TIM13���(����)�ж�

	/*
		3. ����NVIC
	*/
	NVIC_InitStruct.NVIC_IRQChannel			=	TIM8_UP_TIM13_IRQn;	//TIM8�ĸ����жϺ�TIM13�����ж�������
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=	2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 	2;
	NVIC_InitStruct.NVIC_IRQChannelCmd		=	ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/*
		4. ������ʱ��
	*/
	TIM_Cmd(TIM13,ENABLE);

	/*
		�����Ҫ�ظ������ж�,������ʹ���Զ�����
	*/
	TIM_ARRPreloadConfig(TIM13,ENABLE);
}



/*
	timer13_out_init ��ʱ��13����Ƚ�����
*/
void timer13_out_init(int n_value,int ccr_value)
{
	TIM_OCInitTypeDef  TIM_OCInitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	/*
		1.���ö�ʱ��ͨ����Ӧ��GPIO����
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	GPIO_InitStruct.GPIO_Pin							=	GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode							=	GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd							=	GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);								//PF8��Ϊ��ʱ��13��ͨ��
	/*
		2. ����ʱ����Ԫ
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);								//ʹ��TIM13ʱ��

	TIM_TimeBaseInitStruct.TIM_Prescaler				=	8399;						//����Ԥ��Ƶֵ,16bits
	TIM_TimeBaseInitStruct.TIM_CounterMode				=	TIM_CounterMode_Up;			//TIM13����������
	TIM_TimeBaseInitStruct.TIM_Period					=	n_value;					//Nֵ
	TIM_TimeBaseInit(TIM13,&TIM_TimeBaseInitStruct);

	/*
		���ö�ʱ�����ͨ��
	*/
	TIM_OCInitStruct.TIM_OCMode							=	TIM_OCMode_PWM2;			//	CNT < CCR  ����͵�ƽ
																						//  ���ڵ���������˵,�Ƚ�ֵԽС,�͵�ƽʱ��Խ��
	TIM_OCInitStruct.TIM_OutputState					=	TIM_OutputState_Enable;		
	TIM_OCInitStruct.TIM_OCPolarity						=	TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_Pulse							=	ccr_value;					//	�Ƚ�ֵ CCR	
	TIM_OC1Init(TIM13,&TIM_OCInitStruct);


	/*
		ʹ�ܶ�ʱ��
	*/
	TIM_Cmd(TIM13,ENABLE);
	/*
		�����Ҫ�������
	*/
	TIM_OC1PreloadConfig(TIM13,TIM_OCPreload_Enable);									//	�Ƚ�ֵ�Զ�����
	TIM_ARRPreloadConfig(TIM13,ENABLE);													//  �Զ����ؼ���ֵ

	

}
