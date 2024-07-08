#include "stm32f4xx.h"

unsigned int delay_time = 0;
#define rSysTickCTL	*((volatile unsigned long *)0xE000E010)

void my_SysTick_Handler(void)
{
	if(delay_time > 0)
	{
		delay_time--;
	}
}

/*
	�κ�һ���жϴ���������,�������ö�ʱ��ʵ�ֵ���ʱ
*/
void gec_delay_ms(unsigned int n)
{
	delay_time = n*1000;
	SysTick_Config(SystemCoreClock/1000000);	//����Ϊms��Systick
	NVIC_SetPriority(SysTick_IRQn,1);			//��SysTick���ж����ȼ�����Ϊ���
	while(delay_time != 0);	
	rSysTickCTL &= ~1;
}

void gec_delay_us(unsigned int n)
{
	delay_time = n;
	SysTick_Config(SystemCoreClock/1000000);	//����Ϊms��Systick
	NVIC_SetPriority(SysTick_IRQn,1);			//��SysTick���ж����ȼ�����Ϊ���	
	while(delay_time != 0);	
	rSysTickCTL &= ~1;
}


