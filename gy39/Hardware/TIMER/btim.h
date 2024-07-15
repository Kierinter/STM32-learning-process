#ifndef __BTIM_H
#define __BTIM_H

#include "./sys/sys.h"

/******************************************************************************************/
/* 基本定时器 定义 */

/* TIMX 中断定义 
 * 默认是针对TIM6/TIM7
 * 注意: 通过修改这4个宏定义,可以支持TIM1~TIM14任意一个定时器.
 */

#define BTIM_TIMX_INT                       TIM6
#define BTIM_TIMX_INT_IRQn                  TIM6_DAC_IRQn
#define BTIM_TIMX_INT_IRQHandler            TIM6_DAC_IRQHandler
#define BTIM_TIMX_INT_CLK_ENABLE()          do{ RCC->APB1ENR |= 1 << 4; }while(0)  /* TIM6 时钟使能 */

void btim_timx_int_init(uint16_t arr, uint16_t psc);    /* 基本定时器 定时中断初始化函数 */
#endif
