#include "stm32f4xx.h"

/*
	debug_init 调试串口(USART1)的配置
			@baudrate: 串口波特率
*/
void debug_init(int baudrate)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	/*
		1.配置USART1对应的GPIO
				USART1_TX   PA9
				USART1_Rx	PA10
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Pin			=	GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode			=	GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd			=	GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	/*
		2.配置USART
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_InitStruct.USART_BaudRate				=	baudrate;						//波特率
	USART_InitStruct.USART_WordLength			=	USART_WordLength_8b;			//数据位8
	USART_InitStruct.USART_Parity				=	USART_Parity_No;				//不要校验
	USART_InitStruct.USART_StopBits				=	USART_StopBits_1;				//停止位1
	USART_InitStruct.USART_HardwareFlowControl	=	USART_HardwareFlowControl_None;	//不要硬件控制流
	USART_InitStruct.USART_Mode					=	USART_Mode_Tx | USART_Mode_Rx;	//收发模式
	USART_Init(USART1, &USART_InitStruct);

	/*
		3.使能中断(如果需要中断)
	*/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);	//接收数据寄存器不为空中断 ==>有数据可以接收时触发中断

	/*
		4.配置NVIC(只要用了中断就要NVIC)
	*/
	NVIC_InitStruct.NVIC_IRQChannel			=	USART1_IRQn;	//usart1中断请求线
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=	2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 	2;
	NVIC_InitStruct.NVIC_IRQChannelCmd		=	ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	/*
		5.开启串口
	*/
	USART_Cmd(USART1,ENABLE);

}

/*
	debug_send_byte 调试串口发送1个字节
*/
void debug_send_byte(char ch)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) != SET);	//等待发送数据寄存器为空
	USART_SendData(USART1,ch);									//从串口发送1个字节
}

/*
	debug_send_str 调试输出一个字符串
*/
void debug_send_str(char* s,int len)
{
	int i =0;
	for(i=0;i<len;i++)
	{
		debug_send_byte(s[i]);
	}
}


/*
	串口中断,主要用来接收数据
	如果您收到的数据需要保存,那最好是有一个  "通信协议"
	"通信协议": 通信双发约定,第一个字符发什么?
							  或 最后一个字符是什么?
							  或 发多少字符?
	比如:
		约定: 以 \n 结尾
*/
char recv_buf[512] = {0}; 	//保存接收到的字符串
int  recv_len   = 0;	  	//收到的字符串长度; 
int  recv_cnt	= 0;      	//接收数量
int  recv_flag = 0;		  	// 接收完成标志   		1接收完成    	0未完成
void USART1_IRQHandler(void)
{
	unsigned char ch;

	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		ch = USART_ReceiveData(USART1);		//从串口1 接收数据
		//debug_send_byte(ch);
		recv_buf[recv_cnt]	=	ch;			//将收到的数据保存
		if(ch == '\n' || recv_cnt==510)     //收完或收不下了
		{//接收完成
			recv_flag = 1;
			recv_len  = recv_cnt;
			recv_cnt  = 0;
		}
		else
		{
			recv_cnt++;
		}
	}
}

/*
		printf函数的实现,其实最终是调用了 fputc

		也就是说,在标准库里面,printf函数的原型用伪代码表示如下
		int printf(const char *format ,void * arg[])
		{
			....

			for()
			{
				fputc()
			}
		}


		因此在,单片机中,我们可以利用 重定向fputc函数的方式,将printf的输出转换到我们想要的位置
		比如  USART1
		====> 
				只需要自己编写 fputc函数就可以了

	使用printf函数的两个步骤
		1) 重定义 fputc函数,如下所示
		2) 将 options 中的, Use MicroLib勾选

*/
#include <stdio.h>

int fputc(int c,FILE* stream)
{
	debug_send_byte(c&0xff); //将fputc重定向到了 串口0(可以作为一个调试输出)
}



