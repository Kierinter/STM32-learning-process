#include "stm32f4xx.h"
#include "gec_key.h"
#include "gec_led.h"
/*
	gec_exti_init��������KEY���ⲿ�ж�
*/
void gec_exti_init(void)
{
	/*
		�����ʼ����Ϣ�ṹ�����
	*/
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	/*
		��ʼ��GPIO������
	*/
	gec_key_init();													//�ú�����ʼ���˰�����Ӧ��GPIO

	/*
		����SYSCFGѡ����
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);	//PA0�����ⲿ�ж�
	
	/*
		����EXTI�ⲿ�жϿ�����
	*/
	EXTI_InitStruct.EXTI_Line		=	EXTI_Line0;					//GPIO���ű��0�����ⲿ�ж�0
	EXTI_InitStruct.EXTI_Mode		=	EXTI_Mode_Interrupt;		//�ⲿ��·������Ϊ�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger	=	EXTI_Trigger_Falling;		//���ԭ��ͼ,�½��ؼ��������µ�˲��
	EXTI_InitStruct.EXTI_LineCmd	=	ENABLE;						//�����ⲿ�ж�0,����NVIC�����ж�����
	EXTI_Init(&EXTI_InitStruct);

	/*
		����NVIC�жϿ�����
	*/
	NVIC_InitStruct.NVIC_IRQChannel			=	EXTI0_IRQn;			//EXTI0�����ж�����
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=	2;		//û������Ҫ��,���ȼ�����
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			=	2;
	NVIC_InitStruct.NVIC_IRQChannelCmd					=	ENABLE;	//����CPU����EXTI0�ж�
	NVIC_Init(&NVIC_InitStruct);

	/*
		����������ɺ�,���ԭ��ͼ��֪
			�� KEY0������һ˲��ͻ���� EXTI0�ж�
			---> NVIC�յ�EXTI0���ж�����,�ͻᱨ��CPU�жϲ���
				ͬʱ,NVIC����CPU������ִ��,���ҽ� EXTI0���жϱ�Ÿ�֪CPU(xPSR[7:0])
				CPU�ͻ���� xPSR[7:0]�жϱ�����ж����������ҵ���Ӧ�������ĵ�ַ
				����ת���õ�ִַ����Ӧ���жϴ���

			���,���������ж�,����Ҫ���ж�������涨�ĺ�����,дһ���жϴ�����
			������: �������û��޸��ж��������е��������ı�
					��ǧ��Ҫ�ı� �ж�������� ˳��!!!!
	*/
	
}


/*
	EXTI0_IRQHandler:EXTI0�ⲿ�ж�0���жϴ�����
					������,���ж��������е� ��עΪEXTI Line0  ��Ӧ������һ��
					����EXTI0�жϲ�����,���޷��ɹ�ִ�д�����

					��������ǵ�������EXTI0�жϺ�(KEY0���� ˲�� )��ϵͳ����
					����Ҫ �û�����
*/
void EXTI0_IRQHandler(void)
{
	int i;
	if(EXTI_GetITStatus(EXTI_Line0) == SET)		//�����û����������ж�ִ��ͬһ��������,
	{											//ȷ����EXTI0�������ж�
	
		if(KEY0 == KEY_DOWN)					//����EXTI0����������һ��GPIO���Ϊ0��GPIO���Ų���
		{										//ȷ����KEY0�������ж�(PA0)	
			for(i=0;i<0x6666;i++);
			if(KEY0 == KEY_DOWN)
			{
				LED1_CHANGE;
			}
		}

		EXTI_ClearITPendingBit(EXTI_Line0);		//����ⲿ�ж�	
	}
}