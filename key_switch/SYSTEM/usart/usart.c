#include "sys.h"
#include "usart.h"


/* Enable OS support and include the necessary header files. */
#if SYS_SUPPORT_OS
#include "os.h"   /* Enable OS support */
#endif

/******************************************************************************************/
/* Enable printf support, choose to use MicroLIB */

#if 1
#if (__ARMCC_VERSION >= 6010050)            /* Enable for AC6 compiler */
__asm(".global __use_no_semihosting\n\t");  /* Enable no semihosting mode */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6 requires main function to have no arguments for semihosting mode */

#else
/* Enable for AC5 compiler, need to import __use_no_semihosting */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* Enable no semihosting mode, need to define _ttywrch\_sys_exit\_sys_command_string, for both AC6 and AC5 modes */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* Define _sys_exit() for no semihosting mode */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* Define FILE for stdio.h */
FILE __stdout;

/* Define fputc function, used by printf to output characters */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* Wait for the transmit buffer to be empty */

    USART_UX->DR = (uint8_t)ch;             /* Write the character ch to the DR register */
    return ch;
}
#endif
/******************************************************************************************/


#if USART_EN_RX     /* Enable receive mode */

/* Receive buffer, size is USART_REC_LEN bytes. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/* Receive status
 * bit15: Receive complete flag
 * bit14: Received 0x0d flag
 * bit13~0: Number of valid received bytes
*/
uint16_t g_usart_rx_sta = 0;

/**
 * @brief       USART_X interrupt handler
 * @param       None
 * @retval      None
 */
void USART_UX_IRQHandler(void)
{
    uint8_t rxdata;
#if SYS_SUPPORT_OS  /* Enable OS support */
    OSIntEnter();
#endif

    if (USART_UX->SR & (1 << 5))                /* Received data */
    {
        rxdata = USART_UX->DR;

        if ((g_usart_rx_sta & 0x8000) == 0)     /* Not receiving a complete frame */
        {
            if (g_usart_rx_sta & 0x4000)        /* Received 0x0d */
            {
                if (rxdata != 0x0a)             /* Received 0x0a (to distinguish from receiving 0x0d) */
                {
                    g_usart_rx_sta = 0;         /* Reset the status, start over */
                }
                else
                {
                    g_usart_rx_sta |= 0x8000;   /* Received 0x0a, mark as complete */
                }
            }
            else      /* Haven't received 0x0d */
            {
                if (rxdata == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;   /* Mark as received 0x0d */
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;   /* Store the received data in g_usart_rx_buf */
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))g_usart_rx_sta = 0;/* If the buffer is full, start over */
                }
            }
        }
    }

#if SYS_SUPPORT_OS  /* Enable OS support */
    OSIntExit();
#endif
}
#endif

/**
 * @brief       Initialize USART_X
 * @param       sclk: USART_X clock source frequency (in MHz)
 *              For USART1 - USART6, sclk can be set to rcc_pclk2 = 84Mhz
 *              For USART2 - USART5 / USART7 / USART8, sclk can be set to rcc_pclk1 = 42Mhz
 * @note        Note: Make sure to set the correct sclk value, otherwise exceptions may occur.
 * @param       baudrate: Baud rate, can be set according to the specific requirements
 * @retval      None
 */
void usart_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp;
    /* Enable GPIO clock */
    USART_TX_GPIO_CLK_ENABLE(); /* Enable TX GPIO clock */
    USART_RX_GPIO_CLK_ENABLE(); /* Enable RX GPIO clock */
    USART_UX_CLK_ENABLE();      /* Enable USART clock */

    sys_gpio_set(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* Configure TX pin as AF mode */

    sys_gpio_set(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* Configure RX pin as AF mode */

    sys_gpio_af_set(GPIOA, USART_TX_GPIO_PIN, USART_TX_GPIO_AF);    /* Set TX pin alternate function, make sure it is correct */
    sys_gpio_af_set(GPIOA, USART_RX_GPIO_PIN, USART_RX_GPIO_AF);    /* Set RX pin alternate function, make sure it is correct */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;              /* Calculate USARTDIV for OVER8 = 0, assuming 16 times oversampling */
    /* Configure baud rate */
    USART_UX->BRR = temp;       /* Configure baud rate for OVER8 = 0 */
    USART_UX->CR1 = 0;          /* Configure CR1 register */
    USART_UX->CR1 |= 0 << 12;   /* Set M = 0, select 8-bit data format */
    USART_UX->CR1 |= 0 << 15;   /* Set OVER8 = 0, 16 times oversampling */
    USART_UX->CR1 |= 1 << 3;    /* Enable transmitter */
#if USART_EN_RX  /* Enable receive mode */
    /* Enable receive interrupt */
    USART_UX->CR1 |= 1 << 2;    /* Enable receiver */
    USART_UX->CR1 |= 1 << 5;    /* Enable receive buffer not empty interrupt */
    sys_nvic_init(3, 3, USART_UX_IRQn, 2); /* Set priority to 2 */
#endif
    USART_UX->CR1 |= 1 << 13;   /* Enable USART */
}
