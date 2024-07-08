#ifndef __KEY_H
#define __KEY_H

#include "./sys/sys.h"



//引脚定义
//KEY0 PA0 KEY1 PE1 KEY2 PE2 KEY3 PE3

#define WKUP_GPIO_PORT                  GPIOA
#define WKUP_GPIO_PIN                   SYS_GPIO_PIN0
#define WKUP_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define KEY1_GPIO_PORT                  GPIOE
#define KEY1_GPIO_PIN                   SYS_GPIO_PIN1
#define KEY1_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */

#define KEY2_GPIO_PORT                  GPIOE
#define KEY2_GPIO_PIN                   SYS_GPIO_PIN2
#define KEY2_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */

#define KEY3_GPIO_PORT                  GPIOE
#define KEY3_GPIO_PIN                   SYS_GPIO_PIN3
#define KEY3_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 4; }while(0)   /* PE口时钟使能 */

#define WK_UP      sys_gpio_pin_get(WKUP_GPIO_PORT, WKUP_GPIO_PIN)      /* 读取WK_UP按键 */
#define KEY1       sys_gpio_pin_get(KEY1_GPIO_PORT, KEY1_GPIO_PIN)      /* 读取KEY1按键 */
#define KEY2       sys_gpio_pin_get(KEY2_GPIO_PORT, KEY2_GPIO_PIN)      /* 读取KEY2按键 */
#define KEY3       sys_gpio_pin_get(KEY3_GPIO_PORT, KEY3_GPIO_PIN)      /* 读取KEY3按键 */

#define KEY1_PRES  1    /*KEY1按下*/
#define KEY2_PRES  2    /*KEY2按下*/
#define KEY3_PRES  3    /*KEY3按下*/
#define WKUP_PRES  4    /*WK_UP按下*/

void key_init(void);    /* 初始化按键 */
uint8_t key_scan(uint8_t mode);    /* 按键扫描 */
#endif