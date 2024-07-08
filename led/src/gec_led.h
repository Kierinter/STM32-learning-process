
#ifndef __GEC_LED_H__
#define __GEC_LED_H__

/* �����״̬ */
#define LED0_ON			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET)	
#define LED0_OFF		GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET)	
#define LED0_CHANGE		GPIO_ToggleBits(GPIOF,GPIO_Pin_9)	

#define LED1_ON			GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET)	
#define LED1_OFF		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_SET)	
#define LED1_CHANGE		GPIO_ToggleBits(GPIOF,GPIO_Pin_10)	


void gec_led_init(void);

#endif