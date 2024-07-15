#include "stm32f4xx.h"

#define APB1_TIM_clock SystemCoreClock/4*2    //

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//启用TIM2时钟
	
	TIM_InternalClockConfig(TIM2);								//设置TIM2使用内部时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;			//定义结构体，配置定时器
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//设置1分频（不分频）
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//设置计数模式为向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;			//设置最大计数值，达到最大值触发更新事件，因为从0开始计数，所以计数1000次是1000-1
	TIM_TimeBaseInitStructure.TIM_Prescaler = 84 - 1;//设置时钟预分频，84-1就是每 时钟频率(84Mhz)/84=1Mhz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;		//重复计数器（高级定时器才有，所以设置0）
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);			//初始化TIM2定时器
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);			//清除更新中断标志位
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		//开启更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//设置中断优先级分组
	
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体，配置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//指定中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//中断使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//设置响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);							//开启定时器
}


void TIM2_IRQHandler(void)			//更新中断函数
{
    static uint32_t counter = 0; // 定义一个静态计数器并初始化
    
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		//获取TIM2定时器的更新中断标志位
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			//清除更新中断标志位
        counter++; // 计数器加1
        if (counter == 1000) // 如果计数器达到1000，即1秒钟
        {
            counter = 0; // 计数器清零
            // 在这里执行1秒钟一次的操作，例如LED闪烁
            GPIO_ToggleBits(GPIOA, GPIO_Pin_6); // 切换PA6的状态
        }
	}
}

