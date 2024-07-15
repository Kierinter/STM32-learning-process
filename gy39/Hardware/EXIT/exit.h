#ifndef __EXIT_H
#define __EXIT_H

#include "./sys/sys.h"

// GPIO and pin definitions for WKUP_INT -> PA0
#define WKUP_INT_GPIO_PORT  GPIOA 
#define WKUP_INT_GPIO_PIN   SYS_GPIO_PIN0 
#define WKUP_INT_GPIO_CLK_ENABLE()  do{ RCC->AHB1ENR |= 1 << 0; }while(0)   /* Enable clock for GPIOA */
#define WKUP_INT_IRQn       EXTI0_IRQn
#define WKUP_INT_IRQHandler EXTI0_IRQHandler

// GPIO and pin definitions for KEY1_INT
#define KEY1_INT_GPIO_PORT  GPIOE
#define KEY1_INT_GPIO_PIN   SYS_GPIO_PIN2 
#define KEY1_INT_GPIO_CLK_ENABLE()  do{ RCC->AHB1ENR |= 1 << 4; }while(0)   /* Enable clock for GPIOE */
#define KEY1_INT_IRQn       EXTI2_IRQn
#define KEY1_INT_IRQHandler EXTI2_IRQHandler

// GPIO and pin definitions for KEY2_INT -> PE3
#define KEY2_INT_GPIO_PORT  GPIOE
#define KEY2_INT_GPIO_PIN   SYS_GPIO_PIN3
#define KEY2_INT_GPIO_CLK_ENABLE()  do{ RCC->AHB1ENR |= 1 << 4; }while(0)   /* Enable clock for GPIOE */
#define KEY2_INT_IRQn       EXTI3_IRQn
#define KEY2_INT_IRQHandler EXTI3_IRQHandler

// GPIO and pin definitions for KEY3_INT -> PE4
#define KEY3_INT_GPIO_PORT  GPIOE
#define KEY3_INT_GPIO_PIN   SYS_GPIO_PIN4
#define KEY3_INT_GPIO_CLK_ENABLE()  do{ RCC->AHB1ENR |= 1 << 4; }while(0)   /* Enable clock for GPIOE */
#define KEY3_INT_IRQn       EXTI4_IRQn
#define KEY3_INT_IRQHandler EXTI4_IRQHandler

void exit_init(void);

void KEY1_INT_IRQHandler(void);
void KEY2_INT_IRQHandler(void);
void KEY3_INT_IRQHandler(void);
void WKUP_INT_IRQHandler(void);

#endif
