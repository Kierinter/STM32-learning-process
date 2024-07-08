
/*
	配置spi1
	SPI_CLK   PB3
	SPI_MISO  PB4
	SPI_MOSI  PB5  
*/


#include "stm32f4xx.h"

/**
  *@简介：初始化spi
  *@形参：无
  *@返回值：无
  */
void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;

	
	/* GPIO的spi复用功能配置 */

	//1.使能分组时钟PB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,  ENABLE);

	//2.PB功能寄存器配置
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//3.引脚功能复用配置
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource3, GPIO_AF_SPI1); // SPI_CLK
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource4, GPIO_AF_SPI1); // SPI_MISO
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource5, GPIO_AF_SPI1); // SPI_MOSI

	/* SPI控制器配置 */

	//1.使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	//2.控制器配置
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1, &SPI_InitStruct);

	/* 中断 */
/*
	//1.spi接收中断
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);

	//2.NVIC中断
	NVIC_InitStruct.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
*/
	
	/* 开启spi */
	SPI_Cmd(SPI1, ENABLE);

}

/**
  *@简介：
  *@形参：data ：表示要发送的数据
  *@返回值：无
  */
void spi_send_byte(unsigned char data)
{
	char p[30] = {0};
	usart_send_str("80  ", strlen("80  "));

	sprintf(p, "data=%#x  ", data);
	usart_send_str(p, strlen(p));
	
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);

	SPI_I2S_SendData(SPI1, data);
}

/**
  *@简介：从从机接收一个字节数据
  *@形参：无
  *@返回值：读取的数据
  */
unsigned char spi_receive_byte(void)
{
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);

	return SPI_I2S_ReceiveData(SPI1);
}

/**
  *@简介：
  *@形参：str ：表示要发送的字符串
  		  str_len ：表示发送字符串的长度
  *@返回值：无
  */
void spi_send_bytes(char *str, unsigned int str_len)
{
	int i = 0;

	for(i = 0; i< str_len; i++)
	{
		spi_send_byte(str[i]);
	}
}

/**
  *@简介：接收从机的数据
  *@形参：无
  *@返回值：无
  */
void SPI1_IRQHandler(void)
{
	u32 data = 0;
	
	if(SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET)
	{
		data = SPI_I2S_ReceiveData(SPI1);

		SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
	}
}

