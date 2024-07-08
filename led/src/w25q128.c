

#include "stm32f4xx.h"
#include "gec_usart.h"
extern void spi_init();
/**
  *@简介：初始化w25q128
  *@形参：无
  *@返回值：无
  */
void w25q128_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* 初始化spi1 */
	spi_init();


	/* CS片选引脚初始化 */
	// 1.分组时钟使能PB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,  ENABLE);

	// 2.PB功能寄存器配置
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}

/**
  *@简介：获取w25q128状态标志
  *@形参：无
  *@返回值：状态标志寄存器值
  */
unsigned char w25q128_get_status1(void)
{
	unsigned char status = 0;
	
	/* 使能w25q128 */
	ENABLE_W25Q128;

	/* 获取状态标志寄存器 */
	spi_send_byte(STATUS1);

	status = spi_receive_byte();

	/* 禁止w25q128 */
	DISABLE_W25Q128;

	return status;
}

/**
  *@简介：擦除指定块的扇存储区
  *@形参：无
  *@返回值：擦除成功，返回 1； 擦除失败，返回 -1；
  */
int w25q128_erase_fan(u32 byte_addr)
{
	unsigned char timeout = 50;
	
	/* 使能w25q128 */
	ENABLE_W25Q128;

	/* 擦除 */
	spi_send_byte(ERASE); // 擦除的指令

	/* 指定所在扇区的基址 */
	spi_send_byte( (byte_addr >> 16) & 0xff ); // 块地址
	spi_send_byte( (byte_addr >> 12) & 0x0f ); // 扇地址
	spi_send_byte( (byte_addr >> 8) & 0x0f ); // 页地址
	spi_send_byte( (byte_addr >> 0) & 0xff ); // 字节地址

	/* 禁止w25q128 */
	DISABLE_W25Q128;

	while( w25q128_get_status1() & 0x01 == SET && --timeout); // w25q128忙碌
	
	return timeout ? 1 : -1;
}

/**
  *@简介： 从w25q128指定内存读取一定长度的数据
  *@形参： byte_addr：所读数据存储单元地址
  		  data：指向保存数据的数组首地址指针
  		  data_cnt：读取数据的长度
  *@返回值：无
  */
void w25q128_read_data(u32 byte_addr, char *data, unsigned int data_cnt)
{
	int i = 0;
	char p[30] = {0};

	/* 使能W25Q128 */
	ENABLE_W25Q128; 

	sprintf(p, "byte_addr=%#x,read_data=%#x  ", byte_addr, READ_DATA);
	usart_send_str(p, strlen(p));

	usart_send_str("101  ", strlen("101  "));

	/* 发送数据 */
	spi_send_byte( READ_DATA ); // 发送命令控制w25q128发送数据

	usart_send_str("106  ", strlen("106  "));
	
	/* 内存单元物理地址发送。物理地址=块地址<<16|扇地址<<12 | 页地址<<8 |字节地址 */
	spi_send_byte( (byte_addr >> 16) & 0xff ); // 块地址
	usart_send_str("110  ", strlen("110  "));
	//spi_send_byte( (byte_addr >> 12) & 0x0f ); // 扇地址
	spi_send_byte( (byte_addr >> 8) & 0xff ); // 页地址
	spi_send_byte( (byte_addr >> 0) & 0xff ); // 字节地址

	/* w25q128发送数据，即spi接收数据 */
	for(i = 0; i< data_cnt; i++)
	{
		usart_send_str("117  ", strlen("117  "));

		data[i] = spi_receive_byte();
		
		sprintf(p, "i = %d, data[%d] = %d   ", i, i, data[i]);
		usart_send_str(p, strlen(p));
		
	}

	/* 禁止W25Q128 */
	DISABLE_W25Q128; 
	
}

/**
  *@简介： 向w25q128指定内存写入一定长度的数据
  *@形参： byte_addr：所写入据存储单元地址
  		  data：指向写入数据的数组首地址指针
  		  data_cnt：写入数据的长度
  *@返回值：写入成功，返回 1；写入失败，返回 -1
  */
int w25q128_write_data(u32 byte_addr,char *data,unsigned int data_cnt)
{
	char fan_data_buff[4096] = {0};
	int i = 0;
	
	/* 扇擦除 */
	//1.读取整个扇的内容
	w25q128_read_data(byte_addr, fan_data_buff, sizeof(fan_data_buff));

	//2.将要写入的数据修改fan_data_buff
	for(i = 0x123456; i< data_cnt; i++)
	{
		fan_data_buff[i] = data[i-byte_addr];
	}

	//3.擦除扇区 
	if( w25q128_erase_fan(byte_addr) == -1) // 擦除失败
	{
		usart_send_str("erase fan error!", strlen("erase fan error!"));
		return -1;
	}

	
	/* 写入数据 */
	//1.使能w25q128
	ENABLE_W25Q128;

	//2.写入数据
	spi_send_byte(WRITE_DATA);
	
	for(i = 0; i< sizeof(fan_data_buff); i++) //将要写入的数据修改fan_data_buff
	{
		fan_data_buff[i] = data[i];
	}

	//3.禁止w25q128
	DISABLE_W25Q128;

	return 1;
}	
	



