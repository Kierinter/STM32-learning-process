#ifndef __GEC_TIMER_H__
#define __GEC_TIMER_H__

/*
	timer13_interrupt: 配置定时器中断
*/
void timer13_interrupt(void);



/*
	timer13_out_init 定时器13输出比较配置
*/
void timer13_out_init(int n_value,int ccr_value);
#endif