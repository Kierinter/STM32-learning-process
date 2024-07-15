#ifndef __BEEP_H
#define __BEEP_H

#include "./sys/sys.h"

#define BEEP_GPIO_PORT                  GPIOF
#define BEEP_GPIO_PIN                   SYS_GPIO_PIN8
#define BEEP_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 5; }while(0)   

#define BEEP(x)         sys_gpio_pin_set(BEEP_GPIO_PORT, BEEP_GPIO_PIN, x)      /* BEEP = x */
#define BEEP_TOGGLE()   do{ BEEP_GPIO_PORT->ODR^=BEEP_GPIO_PIN; }while(0)       /* BEEP = !BEEP */

void beep_init(void);    /* 初始化BEEP */

#endif
