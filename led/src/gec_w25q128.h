#ifndef __GEC_W25Q128_H__
#define __GEC_W25Q128_H__

#define 	W25Q128_ENBALE	GPIO_ResetBits(GPIOB,GPIO_Pin_14)		//#ss�͵�ƽ��Ч
#define 	W25Q128_DISBALE	GPIO_SetBits(GPIOB,GPIO_Pin_14)

/*
	spi:
		PB3		SPI1_SCK
		PB4		SPI1_MISO
		PB5		SPI1_MOSI
	#ss
		PB14	F_CS		===> ����Ƭѡ�źź�
*/
void w25q128_gpio_init(void);

#endif
