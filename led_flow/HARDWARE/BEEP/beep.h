#ifndef __BEEP_H
#define __BEEP_H

#include "./sys/sys.h"

//引脚定义
#define BEEP_GPIO_PORT                  GPIOF
#define BEEP_GPIO_PIN                   SYS_GPIO_PIN8
#define BEEP_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 5; }while(0)   /* PF口时钟使能 */

/*蜂鸣器控制 */
#define BEEP(x)         sys_gpio_pin_set(BEEP_GPIO_PORT, BEEP_GPIO_PIN, x)      /* BEEP = x */

/*蜂鸣器取反 */
#define BEEP_TOGGLE()   do{ BEEP_GPIO_PORT->ODR^=BEEP_GPIO_PIN; }while(0)       /* BEEP = !BEEP */

void beep_init(void);    /* 初始化BEEP */

#endif