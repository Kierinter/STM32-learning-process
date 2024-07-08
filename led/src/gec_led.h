/*****************************************************************************************
* 本程序只供学习使用,未经作者许可,不得用于其它任何用途
* GECM4开发板
* LED驱动代码
* 广州粤嵌通信科技股份有限公司湖南分公司
* 创建日期:2020-04-20
* 版本: V1.0
* 如发现侵权行为,本公司具有问责和起诉权。
*****************************************************************************************/

#ifndef __GEC_LED_H__
#define __GEC_LED_H__

/* 定义灯状态 */
#define LED0_ON			GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET)	
#define LED0_OFF		GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET)	
#define LED0_CHANGE		GPIO_ToggleBits(GPIOF,GPIO_Pin_9)	

#define LED1_ON			GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET)	
#define LED1_OFF		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_SET)	
#define LED1_CHANGE		GPIO_ToggleBits(GPIOF,GPIO_Pin_10)	

/*
	gec_led_init: 配置LED对应的GPIO引脚
		@void: 无参数

	返回值:
		无返回值
*/
void gec_led_init(void);

#endif