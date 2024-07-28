#ifndef __SYS_H
#define __SYS_H

#include "stm32f4xx.h"

/**
 * SYS_SUPPORT_OS���ڶ���ϵͳ�ļ����Ƿ�֧��OS
 * 0,��֧��OS
 * 1,֧��OS
 */
#define SYS_SUPPORT_OS 0

/* sys_nvic_ex_configר�ú궨�� */
#define SYS_GPIO_FTIR 1 /* �½��ش��� */
#define SYS_GPIO_RTIR 2 /* �����ش��� */
#define SYS_GPIO_BTIR 3 /* ������ش��� */

/* GPIO����ר�ú궨�� */
#define SYS_GPIO_MODE_IN 0  /* ��ͨ����ģʽ */
#define SYS_GPIO_MODE_OUT 1 /* ��ͨ���ģʽ */
#define SYS_GPIO_MODE_AF 2  /* AF����ģʽ */
#define SYS_GPIO_MODE_AIN 3 /* ģ������ģʽ */

#define SYS_GPIO_SPEED_LOW 0  /* GPIO�ٶ�(����,2M) */
#define SYS_GPIO_SPEED_MID 1  /* GPIO�ٶ�(����,25M) */
#define SYS_GPIO_SPEED_FAST 2 /* GPIO�ٶ�(����,50M) */
#define SYS_GPIO_SPEED_HIGH 3 /* GPIO�ٶ�(����,100M) */

#define SYS_GPIO_PUPD_NONE 0 /* ���������� */
#define SYS_GPIO_PUPD_PU 1   /* ���� */
#define SYS_GPIO_PUPD_PD 2   /* ���� */
#define SYS_GPIO_PUPD_RES 3  /* ���� */

#define SYS_GPIO_OTYPE_PP 0 /* ������� */
#define SYS_GPIO_OTYPE_OD 1 /* ��©��� */

/* GPIO����λ�ú궨��  */
#define SYS_GPIO_PIN0 1 << 0
#define SYS_GPIO_PIN1 1 << 1
#define SYS_GPIO_PIN2 1 << 2
#define SYS_GPIO_PIN3 1 << 3
#define SYS_GPIO_PIN4 1 << 4
#define SYS_GPIO_PIN5 1 << 5
#define SYS_GPIO_PIN6 1 << 6
#define SYS_GPIO_PIN7 1 << 7
#define SYS_GPIO_PIN8 1 << 8
#define SYS_GPIO_PIN9 1 << 9
#define SYS_GPIO_PIN10 1 << 10
#define SYS_GPIO_PIN11 1 << 11
#define SYS_GPIO_PIN12 1 << 12
#define SYS_GPIO_PIN13 1 << 13
#define SYS_GPIO_PIN14 1 << 14
#define SYS_GPIO_PIN15 1 << 15

static void sys_nvic_priority_group_config(uint8_t group);

void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);
void sys_nvic_init(uint8_t pprio, uint8_t sprio, uint8_t ch, uint8_t group);
void sys_nvic_ex_config(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t tmode);
void sys_gpio_af_set(GPIO_TypeDef *gpiox, uint16_t pinx, uint8_t afx);
void sys_gpio_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint32_t mode, uint32_t otype, uint32_t ospeed, uint32_t pupd);

void sys_gpio_pin_set(GPIO_TypeDef *p_gpiox, uint16_t pinx, uint8_t status);
uint8_t sys_gpio_pin_get(GPIO_TypeDef *p_gpiox, uint16_t pinx);
void sys_standby(void);
void sys_soft_reset(void);

uint8_t sys_clock_set(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);
void sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);
void sys_qspi_enable_memmapmode(uint8_t ftype);

void sys_wfi_set(void);
void sys_intx_disable(void);
void sys_intx_enable(void);
void sys_msr_msp(uint32_t addr);

#endif
