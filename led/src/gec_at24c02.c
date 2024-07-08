#include "stm32f4xx.h"

/*

		PB8		IIC_SCL
		PB9		IIC_SDA
*/
void at24c02_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	I2C_InitTypeDef   I2C_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitStruct.GPIO_Pin			=	GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_OType			=	GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Mode			=	GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed			= 	GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd			=	GPIO_PuPd_UP;			//提高IIC驱动能力
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);



	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_InitStruct.I2C_Mode					=	I2C_Mode_I2C;
	I2C_InitStruct.I2C_ClockSpeed			=	400000;
	I2C_InitStruct.I2C_DutyCycle			=	I2C_DutyCycle_2;
	I2C_InitStruct.I2C_AcknowledgedAddress	=	I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_OwnAddress1			=	0x55;	//主设备地址随意
	I2C_InitStruct.I2C_Ack					=	I2C_Ack_Enable;;
	I2C_Init(I2C1,&I2C_InitStruct);


	I2C_Cmd(I2C1,ENABLE);
}

/*
	wait_iic_event,等待事件发生(有限等待,工程实际应用中一般不会死等)

	返回值:
		0 超时,事件没有发生
		1 事件产生了
*/
int wait_iic_event(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT,u32 timeout)
{
	while((I2C_CheckEvent(I2Cx,I2C_EVENT)==ERROR)  && --timeout)  //事件没发生,或者没有超时,就继续等待
	{
		gec_delay_us(1);	//每隔1us判断一次事件,这个函数只会等待 timeout us
	}

	if(timeout == 0)
	{
		return 0;
	}

	return 1;
}

/*
	at24c02_is_busy 确保AT24C02芯片空闲

	返回值:
		0 空闲
		1 忙绿
*/
int at24c02_is_busy(void)
{
	//I2C_GenerateSTART(I2C1,ENABLE);
	//while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) == ERROR); //一般实际工程中,不会死等,而是有限等待

	int t = 20; //超时时间
	int r = 1;

	while(t--)
	{
		I2C_GenerateSTART(I2C1,ENABLE);
		if( wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000) == 0 ) //等待超时,事件未产生
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			continue;
		}

		I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Transmitter); //如果总线不忙碌,发送7bit设备地址
		if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED, 1000) == 0)
		{
			r = 0;  //AT24C02响应了地址,说明空闲,可以通信
			break;
		}
	}


	I2C_GenerateSTOP(I2C1,ENABLE);
	return r;
}


/*
	write_a_byte_to_AT24C02 用来向AT24C02写入一个字节数据
			@word_addr: 指定字地址
			@data写入的数据
	
*/
u8  write_a_byte_to_AT24C02(u8 word_addr,u8 data)
{
	int r = 0;
	if( at24c02_is_busy() == -1 )
	{
		printf("AT24C02 is BUSY\n");
		r = -20;
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//发送起始信号
	I2C_GenerateSTART(I2C1, ENABLE);
	
	//等待一个事仿指示START信号发送成势
	if ( !wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000))
	{
		r = -5;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//2. 发送一个从设备的地址
	I2C_Send7bitAddress(I2C1, 0xa0, I2C_Direction_Transmitter);

	if (!wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
			1000))
	{
		r = -6;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	
	//发送字地址 (数据)
	I2C_SendData(I2C1,word_addr);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING, 1000) == 0)
	{
		r = -3;  //AT24C02响应了地址,说明空闲,可以通信
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//发送真正要存储的数据
	I2C_SendData(I2C1,data);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING, 1000) == 0)
	{
		r = -4;  //AT24C02响应了地址,说明空闲,可以通信
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
err:
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	I2C_GenerateSTOP(I2C1,ENABLE);
	return r;

}

/*
	write_byte_to_AT24C02 用来向 AT24C02中写入数据
		@word_addr:字地址,写入数据的起始地址
		@data: 要写入的数据
		@count: 要写入的字节数
*/
int write_byte_to_AT24C02(uint8_t word_addr,uint8_t *data,int count)
{
	
}

/*
	read_byte_to_AT24C02 用来从 AT24C02中读取数据
		@word_addr:字地址,指定从哪个地址开始读取数据
		@data: 指定空间(数组名),用来存储读取到的数据
		@count: 要读取的字节数
*/
int read_byte_to_AT24C02(uint8_t word_addr,uint8_t *data,int count)
{
	int i = 0;
	int r = 0;
	/*
	if( at24c02_is_busy() == 1 )
	{
		printf("AT24C02 is BUSY\n");
		r = -20;
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	*/
	
	/*
		看AT24C02手册第10页第2点
	*/
//告诉24C02 "我"要读你哪个地方的数据	
	//发送起始信号
	I2C_GenerateSTART(I2C1, ENABLE);


	//等待一个事仿指示START信号发送成势
	if ( !wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000))
	{
		r = -5;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//2. 发送一个从设备的地址
	I2C_Send7bitAddress(I2C1, 0xa0, I2C_Direction_Transmitter);

	if (!wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
			1000))
	{
		r = -6;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	
	//发送字地址 (数据)
	I2C_SendData(I2C1,word_addr);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING, 1000) == 0)
	{
		r = -3;  //AT24C02响应了地址,说明空闲,可以通信
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);

//读取24c02的数据
	//发送起始信号
	I2C_GenerateSTART(I2C1,ENABLE);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000) == 0 ) //等待超时,事件未产生
	{
		r = -1;
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//发送7bit地址
	I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Receiver); //如果总线不忙碌,发送7bit设备地址
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, 1000) == 0)
	{
		r = -2;  //AT24C02响应了地址,说明空闲,可以通信
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	for(i=0;i<count-1;i++)   // -1是因为接收最后一个字节自前,要回复NA,所以放到后面单独接收
	{
		if(wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 1000) == 0)   //RXNE未被设置
		{
			r = -16;
			goto read_err;
		}
		data[i] = I2C_ReceiveData(I2C1);
		//由于在初始化IIC时,使能了ACK,所以接收1字节后会自动回复
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//回复 NA ,只有当对方收到了NA应答,对方才会停止发数据
	I2C_AcknowledgeConfig(I2C1,DISABLE);  
	I2C_GenerateSTOP(I2C1,ENABLE);
	//接收最后一个字节
	if(wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 1000) == 0)   //RXNE未被设置
	{
		r = -16;
		goto read_err;
	}
	data[i] = I2C_ReceiveData(I2C1); 
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);	
read_err:
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	I2C_GenerateSTOP(I2C1,ENABLE);
	return r;
}

