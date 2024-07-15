/**
 * @file btim.c
 * @brief This file contains the implementation of the BTIM module.
 */

#include "btim.h"
#include "./LED/led.h"

/**
 * @brief Interrupt handler for BTIM_TIMX.
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    if(BTIM_TIMX_INT->SR & 0x0001)  /* Check update interrupt */
    {
        LED1_TOGGLE();
    }
    BTIM_TIMX_INT->SR &= ~(1 << 0);     /* Clear interrupt flag */
}

/**
 * @brief Initializes the BTIM_TIMX interrupt.
 * @param arr The auto-reload value for the counter.
 * @param psc The prescaler value.
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    BTIM_TIMX_INT_CLK_ENABLE();
    BTIM_TIMX_INT->ARR = arr;           /* Set counter auto-reload value */
    BTIM_TIMX_INT->PSC = psc;           /* Set prescaler value */
    BTIM_TIMX_INT->DIER |= 1 << 0;      /* Enable update interrupt */
    BTIM_TIMX_INT->CR1 |= 1 << 0;       /* Enable timer TIMX */
    sys_nvic_init(1, 3, BTIM_TIMX_INT_IRQn, 2); /* Preemption 1, subpriority 3, group 2 */
}