/*****************************************************************************************
* ������ֻ��ѧϰʹ��,δ���������,�������������κ���;
* GECM4������
* LED��������
* ������Ƕͨ�ſƼ��ɷ����޹�˾���Ϸֹ�˾
* ��������:2020-04-20
* �汾: V1.0
* �緢����Ȩ��Ϊ,����˾�������������Ȩ��
*****************************************************************************************/

#ifndef __GEC_LED_H__
#define __GEC_LED_H__

/* �����״̬ */
#define LED0_ON			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET)	
#define LED0_OFF		GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET)	
#define LED0_CHANGE		GPIO_ToggleBits(GPIOF,GPIO_Pin_9)	

#define LED1_ON			GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET)	
#define LED1_OFF		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_SET)	
#define LED1_CHANGE		GPIO_ToggleBits(GPIOF,GPIO_Pin_10)	

/*
	gec_led_init: ����LED��Ӧ��GPIO����
		@void: �޲���

	����ֵ:
		�޷���ֵ
*/
void gec_led_init(void);

#endif