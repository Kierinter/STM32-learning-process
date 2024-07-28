#include "./sys/sys.h"
#include "./usart/usart.h"
#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "./GY39/gy39.h"

/* ���ʹ��os,����������ͷ�ļ�����. */
#if SYS_SUPPORT_OS
#include "os.h"   /* os ʹ�� */
#endif

/******************************************************************************************/
/* �������´���, ֧��printf����, ������Ҫѡ��use MicroLIB */

#if 1
#if (__ARMCC_VERSION >= 6010050)            /* ʹ��AC6������ʱ */
__asm(".global __use_no_semihosting\n\t");  /* ������ʹ�ð�����ģʽ */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6����Ҫ����main����Ϊ�޲�����ʽ�����򲿷����̿��ܳ��ְ�����ģʽ */

#else
/* ʹ��AC5������ʱ, Ҫ�����ﶨ��__FILE �� ��ʹ�ð�����ģʽ */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* ��ʹ�ð�����ģʽ��������Ҫ�ض���_ttywrch\_sys_exit\_sys_command_string����,��ͬʱ����AC6��AC5ģʽ */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE �� stdio.h���涨��. */
FILE __stdout;

/* �ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

    USART_UX->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    return ch;
}
#endif
/******************************************************************************************/


#if USART_EN_RX     /* ���ʹ���˽��� */

/* ���ջ���, ���USART_REC_LEN���ֽ�. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  ����״̬
 *  bit15��      ������ɱ�־
 *  bit14��      ���յ�0x0d
 *  bit13~0��    ���յ�����Ч�ֽ���Ŀ
*/
uint16_t g_usart_rx_sta = 0;

/**
 * @brief       ����X�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART_UX_IRQHandler(void)
{
    uint8_t rxdata;
#if SYS_SUPPORT_OS  /* ���SYS_SUPPORT_OSΪ�棬����Ҫ֧��OS. */
    OSIntEnter();
#endif

    if (USART_UX->SR & (1 << 5))                /* ���յ����� */
    {
        rxdata = USART_UX->DR;

        if ((g_usart_rx_sta & 0x8000) == 0)     /* ����δ���? */
        {
            if (g_usart_rx_sta & 0x4000)        /* ���յ���0x0d? */
            {
                if (rxdata != 0x0a)             /* ���յ���0x0a? (�����Ƚ��յ���0x0d,�ż��0x0a) */
                {
                    g_usart_rx_sta = 0;         /* ���մ���, ���¿�ʼ */
                }
                else
                {
                    g_usart_rx_sta |= 0x8000;   /* �յ���0x0a,��ǽ�������� */
                }
            }
            else      /* ��û�յ�0x0d */
            {
                if (rxdata == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;   /* ��ǽ��յ��� 0x0d */
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;   /* �洢���ݵ� g_usart_rx_buf */
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))g_usart_rx_sta = 0;/* �����������, ���¿�ʼ���� */
                }
            }
        }
    }

#if SYS_SUPPORT_OS  /* ���SYS_SUPPORT_OSΪ�棬����Ҫ֧��OS. */
    OSIntExit();
#endif
}
#endif

/**
 * @brief       ����X��ʼ������
 * @param       sclk: ����X��ʱ��ԴƵ��(��λ: MHz)
 *              ����1 �� ����6 ��ʱ��Դ����: rcc_pclk2 = 84Mhz
 *              ����2 - 5 / 7 / 8 ��ʱ��Դ����: rcc_pclk1 = 42Mhz
 * @note        ע��: ����������ȷ��sclk, ���򴮿ڲ����ʾͻ������쳣.
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @retval      ��
 */
void usart_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp;
    /* IO �� ʱ������ */
    USART_TX_GPIO_CLK_ENABLE(); /* ʹ�ܴ���TX��ʱ�� */
    USART_RX_GPIO_CLK_ENABLE(); /* ʹ�ܴ���RX��ʱ�� */
    USART_UX_CLK_ENABLE();      /* ʹ�ܴ���ʱ�� */

    sys_gpio_set(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* ����TX�� ģʽ���� */

    sys_gpio_set(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* ����RX�� ģʽ���� */

    sys_gpio_af_set(GPIOA, USART_TX_GPIO_PIN, USART_TX_GPIO_AF);    /* TX�� ���ù���ѡ��, ����������ȷ */
    sys_gpio_af_set(GPIOA, USART_RX_GPIO_PIN, USART_RX_GPIO_AF);    /* RX�� ���ù���ѡ��, ����������ȷ */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;              /* �õ�USARTDIV@OVER8 = 0, ��������������� */
    /* ���������� */
    USART_UX->BRR = temp;       /* ����������@OVER8 = 0 */
    USART_UX->CR1 = 0;          /* ����CR1�Ĵ��� */
    USART_UX->CR1 |= 0 << 12;   /* ����M = 0, ѡ��8λ�ֳ� */
    USART_UX->CR1 |= 0 << 15;   /* ����OVER8 = 0, 16�������� */
    USART_UX->CR1 |= 1 << 3;    /* ���ڷ���ʹ�� */
#if USART_EN_RX  /* ���ʹ���˽��� */
    /* ʹ�ܽ����ж� */
    USART_UX->CR1 |= 1 << 2;    /* ���ڽ���ʹ�� */
    USART_UX->CR1 |= 1 << 5;    /* ���ջ������ǿ��ж�ʹ�� */
    sys_nvic_init(3, 3, USART_UX_IRQn, 2); /* ��2��������ȼ� */
#endif
    USART_UX->CR1 |= 1 << 13;   /* ����ʹ�� */
}

void usart2_init(uint32_t BaudRate)//PA2,PA3
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	/*GPIO������*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	/*USART����*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//�շ�ģʽ��ȫ˫��
	USART_InitStruct.USART_Parity = USART_Parity_No;//��У��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8bits����λ
	USART_Init(USART2,&USART_InitStruct);
	/* USART�ж�����*/
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);//���������ݴ����ж�
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/*�򿪴���*/
	USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{
	uint8_t data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)//�������������ж�
	{
		data = USART_ReceiveData(USART2);//��ȡ���յ�������
		recvbuf[data_len++] = data;
		if((data_len==1 && recvbuf[0]!=0x5A) || 
			(data_len==2 && recvbuf[0]!=0x5A && recvbuf[1]!=0x5A))//֡ͷ������0x5A,0x5A
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
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);//�����־λ
	}
}

void USART_SendDatas(USART_TypeDef * USARTx,uint8_t *buf, uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USARTx, buf[i]);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);//�ȴ��������
	}
}













