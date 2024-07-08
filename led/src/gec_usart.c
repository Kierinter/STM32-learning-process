#include "stm32f4xx.h"

/*
	debug_init ���Դ���(USART1)������
			@baudrate: ���ڲ�����
*/
void debug_init(int baudrate)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	/*
		1.����USART1��Ӧ��GPIO
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
		2.����USART
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_InitStruct.USART_BaudRate				=	baudrate;						//������
	USART_InitStruct.USART_WordLength			=	USART_WordLength_8b;			//����λ8
	USART_InitStruct.USART_Parity				=	USART_Parity_No;				//��ҪУ��
	USART_InitStruct.USART_StopBits				=	USART_StopBits_1;				//ֹͣλ1
	USART_InitStruct.USART_HardwareFlowControl	=	USART_HardwareFlowControl_None;	//��ҪӲ��������
	USART_InitStruct.USART_Mode					=	USART_Mode_Tx | USART_Mode_Rx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStruct);

	/*
		3.ʹ���ж�(�����Ҫ�ж�)
	*/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);	//�������ݼĴ�����Ϊ���ж� ==>�����ݿ��Խ���ʱ�����ж�

	/*
		4.����NVIC(ֻҪ�����жϾ�ҪNVIC)
	*/
	NVIC_InitStruct.NVIC_IRQChannel			=	USART1_IRQn;	//usart1�ж�������
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=	2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 	2;
	NVIC_InitStruct.NVIC_IRQChannelCmd		=	ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	/*
		5.��������
	*/
	USART_Cmd(USART1,ENABLE);

}

/*
	debug_send_byte ���Դ��ڷ���1���ֽ�
*/
void debug_send_byte(char ch)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) != SET);	//�ȴ��������ݼĴ���Ϊ��
	USART_SendData(USART1,ch);									//�Ӵ��ڷ���1���ֽ�
}

/*
	debug_send_str �������һ���ַ���
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
	�����ж�,��Ҫ������������
	������յ���������Ҫ����,���������һ��  "ͨ��Э��"
	"ͨ��Э��": ͨ��˫��Լ��,��һ���ַ���ʲô?
							  �� ���һ���ַ���ʲô?
							  �� �������ַ�?
	����:
		Լ��: �� \n ��β
*/
char recv_buf[512] = {0}; 	//������յ����ַ���
int  recv_len   = 0;	  	//�յ����ַ�������; 
int  recv_cnt	= 0;      	//��������
int  recv_flag = 0;		  	// ������ɱ�־   		1�������    	0δ���
void USART1_IRQHandler(void)
{
	unsigned char ch;

	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		ch = USART_ReceiveData(USART1);		//�Ӵ���1 ��������
		//debug_send_byte(ch);
		recv_buf[recv_cnt]	=	ch;			//���յ������ݱ���
		if(ch == '\n' || recv_cnt==510)     //������ղ�����
		{//�������
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
		printf������ʵ��,��ʵ�����ǵ����� fputc

		Ҳ����˵,�ڱ�׼������,printf������ԭ����α�����ʾ����
		int printf(const char *format ,void * arg[])
		{
			....

			for()
			{
				fputc()
			}
		}


		�����,��Ƭ����,���ǿ������� �ض���fputc�����ķ�ʽ,��printf�����ת����������Ҫ��λ��
		����  USART1
		====> 
				ֻ��Ҫ�Լ���д fputc�����Ϳ�����

	ʹ��printf��������������
		1) �ض��� fputc����,������ʾ
		2) �� options �е�, Use MicroLib��ѡ

*/
#include <stdio.h>

int fputc(int c,FILE* stream)
{
	debug_send_byte(c&0xff); //��fputc�ض����� ����0(������Ϊһ���������)
}



