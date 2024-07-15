#include "stm32f4xx.h"

//使能IWDG，在40KHz LSI下，约等于每隔2s产生IWDG复位
void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能或者失能对寄存器 IWDG_PR 和 IWDG_RLR 的写操作
	IWDG_SetPrescaler(IWDG_Prescaler_64);  //设置 IWDG 预分频值，40khz / 64分频 = 625hz
	IWDG_SetReload(1250);  //设置 IWDG 重装载值，1250 * (1 / 625hz) = 2s
	IWDG_ReloadCounter();  //按照 IWDG 重装载寄存器的值重装载 IWDG 计数器,喂狗
	IWDG_Enable();  //使能 IWDG
}

//喂狗，防止CPU复位
void IWDG_FeedDog(void)
{
	IWDG_ReloadCounter();
}