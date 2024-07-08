#include "stm32f4xx.h"

/*
	gec_key_init:��������KEY����
*/
void gec_key_init(void)
{
	/* ������� */
	GPIO_InitTypeDef  GPIO_InitStruct;								//�����ʼ��GPIO��Ϣ�ṹ�����

	/*	1.ʹ��GPIO����ʱ��	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);			//ʹ��GPIOA��ʱ��

	/*	2.��ʼ������GPIO*/
	GPIO_InitStruct.GPIO_Pin		=	GPIO_Pin_0;	
	GPIO_InitStruct.GPIO_Mode		=	GPIO_Mode_IN;				//����ģʽ
	GPIO_InitStruct.GPIO_OType		=	GPIO_OType_PP;				//�������
	GPIO_InitStruct.GPIO_Speed		=	GPIO_Speed_50MHz;			//�������50MHz
	GPIO_InitStruct.GPIO_PuPd		=	GPIO_PuPd_UP;				//����������
	GPIO_Init(GPIOA,&GPIO_InitStruct);								//��������ĸ�ֵ,������PF9��PF10
}



