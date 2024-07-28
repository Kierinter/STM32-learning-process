#include "./sys/sys.h"
#include "./usart/usart.h"
#include "./GY39/gy39.h"

/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h"   /* os 使用 */
#endif

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1
#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */

    USART_UX->DR = (uint8_t)ch;             /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/


#if USART_EN_RX     /* 如果使能了接收 */

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t g_usart_rx_sta = 0;

/**
 * @brief       串口X中断服务函数
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{
    uint8_t rxdata;
#if SYS_SUPPORT_OS  /* 如果SYS_SUPPORT_OS为真，则需要支持OS. */
    OSIntEnter();
#endif

    if (USART_UX->SR & (1 << 5))                /* 接收到数据 */
    {
        rxdata = USART_UX->DR;

        if ((g_usart_rx_sta & 0x8000) == 0)     /* 接收未完成? */
        {
            if (g_usart_rx_sta & 0x4000)        /* 接收到了0x0d? */
            {
                if (rxdata != 0x0a)             /* 接收到了0x0a? (必须先接收到到0x0d,才检查0x0a) */
                {
                    g_usart_rx_sta = 0;         /* 接收错误, 重新开始 */
                }
                else
                {
                    g_usart_rx_sta |= 0x8000;   /* 收到了0x0a,标记接收完成了 */
                }
            }
            else      /* 还没收到0x0d */
            {
                if (rxdata == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;   /* 标记接收到了 0x0d */
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;   /* 存储数据到 g_usart_rx_buf */
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))g_usart_rx_sta = 0;/* 接收数据溢出, 重新开始接收 */
                }
            }
        }
    }

#if SYS_SUPPORT_OS  /* 如果SYS_SUPPORT_OS为真，则需要支持OS. */
    OSIntExit();
#endif
}
#endif

/**
 * @brief       串口X初始化函数
 * @param       sclk: 串口X的时钟源频率(单位: MHz)
 *              串口1 和 串口6 的时钟源来自: rcc_pclk2 = 84Mhz
 *              串口2 - 5 / 7 / 8 的时钟源来自: rcc_pclk1 = 42Mhz
 * @note        注意: 必须设置正确的sclk, 否则串口波特率就会设置异常.
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp;
    /* IO 及 时钟配置 */
    USART_TX_GPIO_CLK_ENABLE(); /* 使能串口TX脚时钟 */
    USART_RX_GPIO_CLK_ENABLE(); /* 使能串口RX脚时钟 */
    USART_UX_CLK_ENABLE();      /* 使能串口时钟 */

    sys_gpio_set(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* 串口TX脚 模式设置 */

    sys_gpio_set(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* 串口RX脚 模式设置 */

    sys_gpio_af_set(GPIOA, USART_TX_GPIO_PIN, USART_TX_GPIO_AF);    /* TX脚 复用功能选择, 必须设置正确 */
    sys_gpio_af_set(GPIOA, USART_RX_GPIO_PIN, USART_RX_GPIO_AF);    /* RX脚 复用功能选择, 必须设置正确 */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;              /* 得到USARTDIV@OVER8 = 0, 采用四舍五入计算 */
    /* 波特率设置 */
    USART_UX->BRR = temp;       /* 波特率设置@OVER8 = 0 */
    USART_UX->CR1 = 0;          /* 清零CR1寄存器 */
    USART_UX->CR1 |= 0 << 12;   /* 设置M = 0, 选择8位字长 */
    USART_UX->CR1 |= 0 << 15;   /* 设置OVER8 = 0, 16倍过采样 */
    USART_UX->CR1 |= 1 << 3;    /* 串口发送使能 */
#if USART_EN_RX  /* 如果使能了接收 */
    /* 使能接收中断 */
    USART_UX->CR1 |= 1 << 2;    /* 串口接收使能 */
    USART_UX->CR1 |= 1 << 5;    /* 接收缓冲区非空中断使能 */
    sys_nvic_init(3, 3, USART_UX_IRQn, 2); /* 组2，最低优先级 */
#endif
    USART_UX->CR1 |= 1 << 13;   /* 串口使能 */
}

void usart2_init(uint32_t BaudRate)//PA2,PA3
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	/*GPIO口配置*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//输入悬空
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	/*USART配置*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//收发模式，全双工
	USART_InitStruct.USART_Parity = USART_Parity_No;//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8bits数据位
	USART_Init(USART2,&USART_InitStruct);
	/* USART中断配置*/
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);//设置来数据触发中断
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/*打开串口*/
	USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{
	u8 data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)//由来数据引发中断
	{
		data = USART_ReceiveData(USART2);//读取接收到的数据
		recvbuf[data_len++] = data;
		if((data_len==1 && recvbuf[0]!=0x5A) || 
			(data_len==2 && recvbuf[0]!=0x5A && recvbuf[1]!=0x5A))//帧头必须是0x5A,0x5A
		{
			data_len = 0;
		}
		
		if(gy39_mode == GUANGZHAO)
		{
			if(data_len == 9)
			{
				recv_ok = 1;
			}
		}
		else if(gy39_mode == OTHER)
		{
			if(data_len == 15)
			{
				recv_ok = 1;
			}
		}
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);//清除标志位
	}
}

void USART_SendDatas(USART_TypeDef * USARTx,uint8_t *buf, uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USARTx, buf[i]);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);//等待发送完成
	}
}













