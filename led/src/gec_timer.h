#ifndef __GEC_TIMER_H__
#define __GEC_TIMER_H__

/*
	timer13_interrupt: ���ö�ʱ���ж�
*/
void timer13_interrupt(void);



/*
	timer13_out_init ��ʱ��13����Ƚ�����
*/
void timer13_out_init(int n_value,int ccr_value);
#endif