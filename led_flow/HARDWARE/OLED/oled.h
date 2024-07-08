#ifndef __OLED_H
#define __OLED_H

#include"stdlib.h"
#include"./sys/sys.h"

/* OLED模式设置
 * 0: 4线串行模式  （模块的BS1，BS2均接GND）
 * 1: 并行8080模式 （模块的BS1，BS2均接VCC）
 */
#define OLED_MODE       1   /* 默认使用8080并口模式 */

/******************************************************************************************/
/* OLED SPI模式引脚 定义 */
/* 注意:这里仅定义了 OLED 4线SPI模式驱动时的 引脚定义. 8位并口访问, 由于引脚太多,就不单独定义了. */

#define OLED_SPI_RST_PORT               GPIOG
#define OLED_SPI_RST_PIN                SYS_GPIO_PIN15
#define OLED_SPI_RST_CLK_ENABLE()       do{ RCC->AHB1ENR |= 1 << 6; }while(0)       /* PG口时钟使能 */

#define OLED_SPI_CS_PORT                GPIOB
#define OLED_SPI_CS_PIN                 SYS_GPIO_PIN7
#define OLED_SPI_CS_CLK_ENABLE()        do{ RCC->AHB1ENR |= 1 << 1; }while(0)       /* PB口时钟使能 */

#define OLED_SPI_RS_PORT                GPIOD
#define OLED_SPI_RS_PIN                 SYS_GPIO_PIN6
#define OLED_SPI_RS_CLK_ENABLE()        do{ RCC->AHB1ENR |= 1 << 3; }while(0)       /* PD口时钟使能 */

#define OLED_SPI_SCLK_PORT              GPIOC
#define OLED_SPI_SCLK_PIN               SYS_GPIO_PIN6
#define OLED_SPI_SCLK_CLK_ENABLE()      do{ RCC->AHB1ENR |= 1 << 2; }while(0)       /* PC口时钟使能 */

#define OLED_SPI_SDIN_PORT              GPIOC
#define OLED_SPI_SDIN_PIN               SYS_GPIO_PIN7
#define OLED_SPI_SDIN_CLK_ENABLE()      do{ RCC->AHB1ENR |= 1 << 2; }while(0)       /* PC口时钟使能 */

/******************************************************************************************/

/* OLED SPI模式相关端口控制函数 定义 
 * 注意:OLED_RST/OLED_CS/OLED_RS,这三个是和80并口模式共用的,即80模式也必须实现这3个函数!
 */
#define OLED_RST(x)     sys_gpio_pin_set(OLED_SPI_RST_PORT,  OLED_SPI_RST_PIN,  x)  /* 设置RST引脚 */
#define OLED_CS(x)      sys_gpio_pin_set(OLED_SPI_CS_PORT,   OLED_SPI_CS_PIN,   x)  /* 设置CS引脚 */
#define OLED_RS(x)      sys_gpio_pin_set(OLED_SPI_RS_PORT,   OLED_SPI_RS_PIN,   x)  /* 设置RS引脚 */
#define OLED_SCLK(x)    sys_gpio_pin_set(OLED_SPI_SCLK_PORT, OLED_SPI_SCLK_PIN, x)  /* 设置SCLK引脚 */
#define OLED_SDIN(x)    sys_gpio_pin_set(OLED_SPI_SDIN_PORT, OLED_SPI_SDIN_PIN, x)  /* 设置SDIN引脚 */

/* OLED 80并口模式WR,RD端口控制函数 定义 */
#define OLED_WR(x)      sys_gpio_pin_set(GPIOA, SYS_GPIO_PIN4, x)   /* 设置WR引脚 */
#define OLED_RD(x)      sys_gpio_pin_set(GPIOD, SYS_GPIO_PIN7, x)   /* 设置RD引脚 */

/* 命令/数据 定义 */
#define OLED_CMD        0       /* 写命令 */
#define OLED_DATA       1       /* 写数据 */

/******************************************************************************************/
    
static void oled_wr_byte(uint8_t data, uint8_t cmd);    /* 写一个字节到OLED */
static uint32_t oled_pow(uint8_t m, uint8_t n);         /* OLED求平方函数 */


void oled_init(void);           /* OLED初始化 */
void oled_clear(void);          /* OLED清屏 */
void oled_display_on(void);     /* 开启OLED显示 */
void oled_display_off(void);    /* 关闭OLED显示 */
void oled_refresh_gram(void);   /* 更新显存到OLED */ 
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot);    /* OLED画点 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);        /* OLED区域填充 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode); /* OLED显示字符 */
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);  /* OLED显示数字 */
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size);           /* OLED显示字符串 */

#endif