#ifndef __MOOTOR_H
#define __MOTOR_H

#include "stm32f4xx.h"

/*

    in1 -> DCMI D0 -> PC6
    in2 -> DCMI D1 -> PC7

 */
#define IN1_PIN GPIO_Pin_6
#define IN2_PIN GPIO_Pin_7
#define MOTOR_GPIO_PORT GPIOC
#define RCC_AHB1Periph_MOTOR_GPIO_PORT RCC_AHB1Periph_GPIOC
void Motor_Init(void);
void Motor_SetSpeed(int8_t Speed);
#endif
