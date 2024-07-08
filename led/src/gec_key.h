#ifndef __GEC_KEY_H__
#define __GEC_KEY_H__

/* 定义按键 */
#define KEY0 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

/* 通过原理图分析按下和松开时 引脚获得的状态值 */
#define KEY_DOWN	0	
#define KEY_UP		1

/*
	gec_key_init:用于配置KEY按键
*/
void gec_key_init(void);



#endif
