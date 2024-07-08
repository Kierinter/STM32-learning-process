#include "stm32f4xx.h"
#include "gec_w25q128.h"
/*
	spi:
		PB3		SPI1_SCK
		PB4		SPI1_MISO
		PB5		SPI1_MOSI
	#ss
		PB14	F_CS		===> 配置片选信号宏
*/
void w25q128_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	SPI_InitTypeDef   SPI_InitStruct;
	/*
		配置GPIO引脚:
			1)配置片选线		<--- 通用输出
			2)配置SPI引脚<-- 复用功能
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Pin			=		GPIO_Pin_14;		//W25Q128(从设备)的片选线
	GPIO_InitStruct.GPIO_Mode			=		GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType			=		GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed			=		GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd			=		GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin			=		GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;	//SPI线
	GPIO_InitStruct.GPIO_Mode			=		GPIO_Mode_AF;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
																					
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);	//SCK   //代码是给STM32在运行,也就是STM32作为主设备
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);	//MISO	==>  SDI
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);	//MOSI	==>  SDO
	

	/*
		配置SPI控制器
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	SPI_InitStruct.SPI_BaudRatePrescaler		=	SPI_BaudRatePrescaler_4;
	//在DI上传输的数据是在时钟的上升沿被锁存的。MSB 首先被传输。
	SPI_InitStruct.SPI_CPOL						=	SPI_CPOL_Low;   	//时钟极性,时钟线空闲时的状态
	SPI_InitStruct.SPI_CPHA						=	SPI_CPHA_1Edge; 	//时钟相位,在第几个边沿采集数据
	SPI_InitStruct.SPI_DataSize					=	SPI_DataSize_8b;	
	SPI_InitStruct.SPI_Direction				=	SPI_Direction_2Lines_FullDuplex;	//全双工
	SPI_InitStruct.SPI_FirstBit					=	SPI_FirstBit_MSB;	//高位先发
	SPI_InitStruct.SPI_Mode						=	SPI_Mode_Master;	//STM32作为主设备
	SPI_InitStruct.SPI_NSS						=	SPI_NSS_Soft;	
	SPI_InitStruct.SPI_CRCPolynomial			=	7;					//CRC校验多项式,大于1即可
	SPI_Init(SPI1,&SPI_InitStruct);


	/*
		使能SPI
	*/
	SPI_Cmd(SPI1,ENABLE);	
}


/*
	spi_send_byte: SPI发送函数,用来向SPI总线发送1个字节
*/
char spi_send_byte(char data)
{
	char recv = 0;
	/*
		等发送数据结存器为空后,再发送一个字节到SPI
	*/
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) != SET);		//如果SPI的TXE标志未被设置,则一直等待
	SPI_SendData(SPI1,data);


	/*
		在发送时,SDI下上的数据(可能是无效数据)
		也会被保存啦

		在SPI的物理层上,是全双工的
		在您发送数据的时候,SDI线上可能没有有效数据
		但是SPI控制器  在 按照您的协议收数据
	*/
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)	!= SET);
	recv = SPI_ReceiveData(SPI1);		//从SPI接收数据(在发送时,可能是无效数据)
}

/*

	返回值: 接收到的1个字节数据
*/
char spi_recv_byte(void)
{
	char recv = 0;
	// 当您写的代码作为主设备时
	// STM32F407 作为主设备,那么SPI控制器只有在发送数据时才会产生时钟
	// 所以,您为了能够收到从设备的数据,必须发送一个 "无效数据"  ==> 为了产生时钟,从而好接受数据
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) != SET);		//如果SPI的TXE标志未被设置,则一直等待
	SPI_SendData(SPI1,0xff);

	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)	!= SET);
	recv = SPI_ReceiveData(SPI1);

	return recv;
}


/*
	w25q128_get_sr1: 获取W25Q128的状态寄存器1 (SR1)

	返回值: 返回获取到的状态值
*/
u8 w25q128_get_sr1(void)
{
	u8 status = 0;
	/*
	读状态寄存器 1（05H）和读状态寄存器 2（35H）指令
	读状态寄存器指令允许读 8 位状态寄存器位。这条指令是以/CS 拉低开始，然后通过
	DI 在时钟的上升沿传输指令代码 05H（读寄存器 1 指令）或者是 35H（读寄存器 2 指令），
	然后状态寄存器的相应位通过 DO 在时钟的下降沿从高位到低位依次传出。最后以/CS
	拉高结束。读状态寄存指令可以任何时间使用，在擦写，写状态寄存器指令周期中依然
	可以。这样就可以随时检查 BUSY 位，检查相应的指令周期有没有结束，芯片是不是可
	以接受新的指令。
	*/
	W25Q128_ENBALE;				//这条指令是以/CS 拉低开始
	spi_send_byte(0x05);		//STM32F407的SPI控制器发数据时会产生时钟

	status = spi_recv_byte();	//接收一个字节(需要发送一个无意义字节)

	W25Q128_DISBALE;			//最后以/CS拉高结束。

	return status;
}


/*
	w25q128_wait_not_busy 用来等待w25q128空闲
*/
void w25q128_wait_not_busy(void)
{
	//如果忙碌,则一直等待,知道空闲
	u8 status = 0;
	do
	{
		gec_delay_ms(5);
		status = w25q128_get_sr1();
	}
	while( ( status & (0x01<<0) ) != 0);
}


/*
	w25q128_read_id 获取芯片ID
		厂商ID和设备ID
		
		Device ID(2)  90h 	dummy 	dummy 	00h 	(MF7-MF0) 	(ID7-ID0)
*/
u16 w25q128_read_id(void)
{
	u8 cid,did;

	W25Q128_ENBALE;				//拉低,使能芯片
	spi_send_byte(0x90);		//发送指令
	spi_send_byte(0x00);
	spi_send_byte(0x00);
	spi_send_byte(0x00);

	cid = spi_recv_byte();		//获取厂商ID
	did = spi_recv_byte();		//获取设备ID

	W25Q128_DISBALE;			//拉高,禁止芯片

	return cid<<8 | did;
	
}


/*
	read_bytes_from_w25q128: 用来W25Q128存储器中读取数据
		@ addr: 指定w25q128存储器中的存储单元地址
		@ data: 指定一个内存空间,用来存读取到的数据
		@ count: 指定读取数量,单位 字节
	返回值:
		失败返回0
		成功返回 非0
*/
int read_bytes_from_w25q128(uint32_t addr,uint8_t *data,int count)
{
	/*
		如果当 BUSY=1 时执行该指令，该指令将被忽略，并且对正在执行的其他指令不会有任
		何影响。
		读数据指令允许从存储器读一个字节和连续多个字节。该指令是以/CS 拉低开始，然后
		通 DI 在时钟的上升沿来传输指令代码（03H）和 24 位地址。当芯片接受完地址位后，
		相应地址处的值将会，在时钟的下降沿，以高位在前低位在后的方式，在 D0 上传输。
		如果连续的读多个字节的话，地址是自动加 1 的。这意味着可以一次读出整个芯片。该
		指令也是以/CS 拉高来结束的。	
	*/

	int i = 0;
	int max_bytes = 0;
	/* 参数合法性检测 */
	if(addr < 0  || addr>0xffffff)  //地址合法性
	{
		printf("指定的地址不合法\n");
		return -1;
	}

	max_bytes = 0x1000000 - addr;	//给定的地址最大可读取的字节数
	if(count > max_bytes)			//如果要读取的字节数超出了允许范围
	{
		count = max_bytes;			//则只能读取合法内容
	}
	
	
	/* 检测是否忙碌 */
	w25q128_wait_not_busy();

	/* /CS拉低 */
	W25Q128_ENBALE;

	/* 发送指令 和 24bits地址 */
	spi_send_byte(0x03);
	spi_send_byte((addr>>16)&0xff);
	spi_send_byte((addr>> 8)&0xff);
	spi_send_byte((addr>> 0)&0xff);

	for(i=0;i<count;i++)
	{
		data[i] = spi_recv_byte();
	}

	W25Q128_DISBALE;

	return i;
	
}

/*
	写使能
*/
void w25q128_write_enable(void)
{
	W25Q128_ENBALE;
	spi_send_byte(0x06);
	W25Q128_DISBALE;
}

/*
	扇区擦除
*/
void sector_erase_to_w25q128(u32 addr)
{
	/*
		为什么要先擦除(整个区域全部变为1)才能写???
		因为  自己看手册
		页编程指令允许 1 到256 字节写入存储器的某一页，这一页必须是被擦除过的（也就是只能写
		0，不能写 1，擦除时是全写为 1） 。
	*/
	w25q128_write_enable();	//写使能

	W25Q128_ENBALE;
	/* 发送指令 和 24bits地址 */
	spi_send_byte(0x20);
	spi_send_byte((addr>>16)&0xff);
	spi_send_byte((addr>> 8)&0xff);
	spi_send_byte((addr>> 0)&0xff);

	W25Q128_DISBALE;

	w25q128_wait_not_busy(); //等待擦除完成
	
}


void w25q128_page_program(uint32_t addr,uint8_t *data,int count)
{
	int i = 0;
	int max_bytes = 0;
	/* 参数合法性检测 */
	if(addr < 0  || addr>0xffffff)  //地址合法性
	{
		printf("指定的地址不合法\n");
		return -1;
	}

	max_bytes = 256 - (addr & 0xff);	//给定的地址最大可读取的字节数
	if(count > max_bytes)				//如果要读取的字节数超出了允许范围
	{
		count = max_bytes;				//则只能读取合法内容
	}


	w25q128_write_enable();	//写使能

	/* /CS拉低 */
	W25Q128_ENBALE;

	/* 发送指令 和 24bits地址 */
	spi_send_byte(0x02);
	spi_send_byte((addr>>16)&0xff);
	spi_send_byte((addr>> 8)&0xff);
	spi_send_byte((addr>> 0)&0xff);

	for(i=0;i<count;i++)
	{
		spi_send_byte(data[i]);
	}
	W25Q128_DISBALE;
	w25q128_wait_not_busy(); //等待写完成

	return i; //实际写入字节数
}

u8 buffer[4096]; 	//用来存储读取出来的一个扇区的内容,为什么不定义局部变量呢
						// 特别注意:  栈的空间不够,全局变量并不会使用栈空间
						//如果局部变量定义太大 或 太多,会导致程序崩溃
/*
	write_bytes_from_w25q128: 用来向W25Q128存储器的指定存储单元空间写入数据
		@ addr: 指定W25Q128内的存储空间地址
		@ data: 指定要写入的数据
		@ count: 指定要写入的数量,单位字节
	返回值:
		失败返回0
		成功返回 非0
*/	
int write_bytes_from_w25q128(uint32_t addr,uint8_t *data,int count)
{
	/*
		如果写入的数据超过一个扇区,应该怎么完善????
			先处理第一个扇区,然后处理第二扇区,然后处理第三个扇区 ....
	*/

	u32 first_sector_addr = addr & ~0xfff;	//记录要写入的扇区首地址
	int i;
	int write_byte = 0;		//可以写入的字节数
	if(addr > 0xffffff)
	{
		printf("指定的地址不合法\n");
		return -1;
	}

	if(count > (0x1000000 - addr))
	{
		count = 0x1000000 - addr;
	}

	/* 读取一个扇区的数据 */
	read_bytes_from_w25q128(first_sector_addr,buffer,4096);		//读取一个扇区的内容

	/* 将要写入的数据替换该内容 */
	for(i=0;i<count;i++)
	{
		buffer[i+(addr&0xfff)] = data[i];  
	}

	/* 擦除扇区 */
	sector_erase_to_w25q128(first_sector_addr);

	for(i=0;i<4096;i+=256)
	{
		w25q128_page_program(first_sector_addr+i,buffer+i,256);	//页写
	}
	
}

