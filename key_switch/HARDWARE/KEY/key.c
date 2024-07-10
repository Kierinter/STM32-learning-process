#include "key.h"
#include "delay.h"

void key_init(void)
{
    KEY0_GPIO_CLK_ENABLE(); /* 使能KEY0引脚时钟 */
    KEY1_GPIO_CLK_ENABLE(); /* 使能KEY1引脚时钟 */
    KEY2_GPIO_CLK_ENABLE(); /* 使能KEY2引脚时钟 */
    KEY3_GPIO_CLK_ENABLE(); /* 使能KEY3引脚时钟 */

    sys_gpio_set(KEY0_GPIO_PORT, KEY0_GPIO_PIN, SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_FAST, SYS_GPIO_PUPD_PU); /* 设置WK_UP引脚为输入 */
    sys_gpio_set(KEY1_GPIO_PORT, KEY1_GPIO_PIN, SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_FAST, SYS_GPIO_PUPD_PU); /* 设置KEY1引脚为输入 */
    sys_gpio_set(KEY2_GPIO_PORT, KEY2_GPIO_PIN, SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_FAST, SYS_GPIO_PUPD_PU); /* 设置KEY2引脚为输入 */
    sys_gpio_set(KEY3_GPIO_PORT, KEY3_GPIO_PIN, SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_FAST, SYS_GPIO_PUPD_PU); /* 设置KEY3引脚为输入 */
}

uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up=1;/* 按键按松开标志 */
    uint8_t keyval=0;

    if(mode) key_up=1; /* 支持连按 */
    if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY0==1))
    {
        delay_ms(10); /* 延时10ms */
        key_up=0;
        if(KEY1==0) keyval=KEY1_PRES;
        else if(KEY2==0) keyval=KEY2_PRES;
        else if(KEY3==0) keyval=KEY3_PRES;
        else if(KEY0==1) keyval=KEY0_PRES;
    }
    else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY0==0) key_up=1;
    return keyval;
}