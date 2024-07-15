/**
  * @file Timer.c
	* @brief This file contains the implementation of the Timer module.
	*/

#include "stm32f4xx.h"
#include "./TIMER/Timer.h"
//#include <stm32f4xx_tim.h>

#define APB1_TIM_clock SystemCoreClock/4*2   // The clock frequency of the APB1 bus is 2 times the system clock frequency

/**
	* @brief Initializes the Timer module.
	* @param None
	* @retval None
	*/
void Timer_Init(void)
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    // Enable TIM3 clock
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
		
		/* GPIO initialization */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);            
		
		TIM_InternalClockConfig(TIM3);    // Set TIM3 to use internal clock
		
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;    // Define structure to configure the timer
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    // Set clock division to 1 (no division)
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    // Set counter mode to up-counting
		TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;    // Set the maximum count value, triggering an update event when reached. Since counting starts from 0, the count will be 1000-1 after 1000 counts
		TIM_TimeBaseInitStructure.TIM_Prescaler = 84 - 1;    // Set the clock prescaler, 84-1 means the clock frequency (84MHz)/84=1MHz
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;    // Repetition counter (only available for advanced timers, so set to 0)
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);    // Initialize TIM3 timer
		
		TIM_OCInitTypeDef TIM_OCInitStructure;    // Define structure to configure the output compare channel
		TIM_OCStructInit(&TIM_OCInitStructure);    // Initialize the output compare channel structure
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse=0;
		TIM_OC3Init(TIM3,&TIM_OCInitStructure);
		
		TIM_Cmd(TIM3, ENABLE);    // Enable the timer
		TIM_CtrlPWMOutputs (TIM3,ENABLE );
}

/**
	* @brief Sets the compare value for PWM.
	* @param Compare: The value to be written to CCR, range: 0~100.
	* @retval None
	* @note The compare value (CCR) and the auto-reload value (ARR) together determine the duty cycle. This function only sets the value of CCR and does not directly set the duty cycle.
	*       Duty cycle = CCR / (ARR + 1)
	*/
void PWM_SetCompare3(uint16_t Compare)
{
		TIM_SetCompare3(TIM3, Compare);    // Set the value of CCR3
}
