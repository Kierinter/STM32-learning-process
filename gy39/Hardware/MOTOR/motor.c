#include"motor.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include "Timer.h"

void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_MOTOR_GPIO_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin = IN1_PIN|IN2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_GPIO_PORT, &GPIO_InitStructure);
    Timer_Init();
}
void Motor_SetSpeed(int8_t Speed)
{
    if (Speed >= 0)							
    {
        GPIO_SetBits(GPIOC, IN1_PIN);	
        GPIO_ResetBits(GPIOC, IN2_PIN);	
        PWM_SetCompare3(Speed);				
    }
    else									
    {
        GPIO_ResetBits(GPIOC, IN1_PIN);	
        GPIO_SetBits(GPIOC, IN2_PIN);	
        PWM_SetCompare3(-Speed);			
    }
}