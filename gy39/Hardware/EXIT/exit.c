/**
 * @file exit.c
 * @brief This file contains the implementation of the exit module.
 */

#include"./EXIT/exit.h"
#include"./sys/sys.h"
#include"./LED/led.h"
#include"./BEEP/beep.h"
#include"./KEY/key.h"
#include"./delay/delay.h"

/**
 * @brief Initializes the exit module.
 * 
 * This function initializes the exit module by configuring the key interrupts and NVIC settings.
 */
void exit_init(void)
{
    key_init();
    sys_nvic_ex_config(WKUP_INT_GPIO_PORT, WKUP_INT_GPIO_PIN, SYS_GPIO_RTIR); /* WKUP configuration as rising edge trigger */
    sys_nvic_ex_config(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN, SYS_GPIO_FTIR); /* KEY1 configuration as falling edge trigger */
    sys_nvic_ex_config(KEY2_INT_GPIO_PORT, KEY2_INT_GPIO_PIN, SYS_GPIO_FTIR); /* KEY2 configuration as falling edge trigger */
    sys_nvic_ex_config(KEY3_INT_GPIO_PORT, KEY3_INT_GPIO_PIN, SYS_GPIO_FTIR); /* KEY3 configuration as falling edge trigger */

    sys_nvic_init(0,2,KEY1_INT_IRQn,2); /* Preemption 0, subpriority 2, group 2 */
    sys_nvic_init(1,2,KEY2_INT_IRQn,2); /* Preemption 1, subpriority 2, group 2 */
    sys_nvic_init(2,2,KEY3_INT_IRQn,2); /* Preemption 2, subpriority 2, group 2 */
    sys_nvic_init(3,2,WKUP_INT_IRQn,2); /* Preemption 3, subpriority 2, group 2 */
}

/**
 * @brief KEY0 external interrupt service routine.
 * 
 * This function is the interrupt service routine for KEY0 external interrupt.
 * It performs debouncing and clears the interrupt flag for KEY0.
 * If KEY1 is pressed, it performs a specific action.
 */
void KEY1_INT_IRQHandler(void)
{
    delay_ms(20);                   /* Debouncing */
    EXTI->PR = KEY1_INT_GPIO_PIN;   /* Clear the interrupt flag for KEY0 interrupt line */

    if (KEY1 == 0)
    {
       //gy39_start(1);// Display temperature, humidity, and atmospheric pressure
    }
}

/**
 * @brief KEY1 external interrupt service routine.
 * 
 * This function is the interrupt service routine for KEY1 external interrupt.
 * It performs debouncing and clears the interrupt flag for KEY1.
 * If KEY2 is pressed, it performs a specific action.
 */
void KEY2_INT_IRQHandler(void)
{ 
    delay_ms(20);                   /* Debouncing */
    EXTI->PR = KEY2_INT_GPIO_PIN;   /* Clear the interrupt flag for KEY1 interrupt line */

    if (KEY2 == 0)
    {
       // Motor_CW();           /* Motor clockwise rotation */
    }
}

/**
 * @brief KEY2 external interrupt service routine.
 * 
 * This function is the interrupt service routine for KEY2 external interrupt.
 * It performs debouncing and clears the interrupt flag for KEY2.
 * If KEY3 is pressed, it performs a specific action.
 */
void KEY3_INT_IRQHandler(void)
{
    delay_ms(20);                   /* Debouncing */
    EXTI->PR = KEY3_INT_GPIO_PIN;   /* Clear the interrupt flag for KEY2 interrupt line */

    if (KEY3 == 0)
    {
        // Motor_Stop();           /* Motor stop */
    }
}

/**
 * @brief WK_UP external interrupt service routine.
 * 
 * This function is the interrupt service routine for WK_UP external interrupt.
 * It performs debouncing and clears the interrupt flag for WK_UP.
 * If WK_UP is pressed, it performs a specific action.
 */
void WKUP_INT_IRQHandler(void)
{ 
    delay_ms(20);                   /* Debouncing */
    EXTI->PR = WKUP_INT_GPIO_PIN;   /* Clear the interrupt flag for WK_UP interrupt line */

    if (WK_UP == 1)
    {
       // Motor_Start(500);              /* Motor clockwise rotation */
    }
}
#include"./EXIT/exit.h"
#include"./sys/sys.h"
#include"./LED/led.h"
#include"./BEEP/beep.h"
#include"./KEY/key.h"
#include"./delay/delay.h"



void exit_init(void)
{
    key_init();
    sys_nvic_ex_config(WKUP_INT_GPIO_PORT, WKUP_INT_GPIO_PIN, SYS_GPIO_RTIR); /* WKUP配置为上升沿触发 */
    sys_nvic_ex_config(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN, SYS_GPIO_FTIR); /* KEY1配置为下降沿触发 */
    sys_nvic_ex_config(KEY2_INT_GPIO_PORT, KEY2_INT_GPIO_PIN, SYS_GPIO_FTIR); /* KEY2配置为下升沿触发 */
    sys_nvic_ex_config(KEY3_INT_GPIO_PORT, KEY3_INT_GPIO_PIN, SYS_GPIO_FTIR); /* KEY3配置为下降沿触发 */

    sys_nvic_init(0,2,KEY1_INT_IRQn,2); /* 抢占0，子优先级2，组2 */
    sys_nvic_init(1,2,KEY2_INT_IRQn,2); /* 抢占1，子优先级2，组2 */
    sys_nvic_init(2,2,KEY3_INT_IRQn,2); /* 抢占2，子优先级2，组2 */
    sys_nvic_init(3,2,WKUP_INT_IRQn,2); /* 抢占3，子优先级2，组2 */
}
 

/**
 * @brief       KEY0 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY1_INT_IRQHandler(void)
{
    delay_ms(20);                   /* 消抖 */
    EXTI->PR = KEY1_INT_GPIO_PIN;   /* 清除KEY0所在中断线 的中断标志位 */

    if (KEY1 == 0)
    {
       //gy39_start(1);//显示温度 湿度 大气压

    }
}

/**
 * @brief       KEY1 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY2_INT_IRQHandler(void)
{ 
    delay_ms(20);                   /* 消抖 */
    EXTI->PR = KEY2_INT_GPIO_PIN;   /* 清除KEY1所在中断线 的中断标志位 */

    if (KEY2 == 0)
    {
       // Motor_CW();           /* 电机正转 */
    }
}

/**
 * @brief       KEY2 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY3_INT_IRQHandler(void)
{
    delay_ms(20);                   /* 消抖 */
    EXTI->PR = KEY3_INT_GPIO_PIN;   /* 清除KEY2所在中断线 的中断标志位 */

    if (KEY3 == 0)
    {
        //Motor_Stop();           /* 电机停止 */
    }
}

/**
 * @brief       WK_UP 外部中断服务程序
 * @param       无
 * @retval      无
 */
void WKUP_INT_IRQHandler(void)
{ 
    delay_ms(20);                   /* 消抖 */
    EXTI->PR = WKUP_INT_GPIO_PIN;   /* 清除WKUP所在中断线 的中断标志位 */

    if (WK_UP == 1)
    {
       //Motor_Start(500);              /* 电机正转 */
    }
}
