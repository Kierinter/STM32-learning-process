#ifndef __GEC_KEY_H__
#define __GEC_KEY_H__

/* ���尴�� */
#define KEY0 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

/* ͨ��ԭ��ͼ�������º��ɿ�ʱ ���Ż�õ�״ֵ̬ */
#define KEY_DOWN	0	
#define KEY_UP		1

/*
	gec_key_init:��������KEY����
*/
void gec_key_init(void);



#endif
