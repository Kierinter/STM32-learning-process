

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./sys/sys.h"


/******************************************************************************************/
/* 
 * This file contains the definitions and function prototypes for USART configuration.
 */

#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   SYS_GPIO_PIN9
#define USART_TX_GPIO_AF                    7                           /* AF selection */
#define USART_TX_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 0; }while(0)   /* Enable GPIOA clock */

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   SYS_GPIO_PIN10
#define USART_RX_GPIO_AF                    7                           /* AF selection */
#define USART_RX_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 0; }while(0)   /* Enable GPIOA clock */

#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* Enable USART1 clock */

/******************************************************************************************/


#define USART_REC_LEN               200             /* Maximum number of bytes to receive is 200 */
#define USART_EN_RX                 1               /* Enable receiving: 1 / Disable receiving: 0 */

extern uint8_t  g_usart_rx_buf[USART_REC_LEN];      /* USART receive buffer, with size of USART_REC_LEN bytes */
extern uint16_t g_usart_rx_sta;                     /* USART receive status */

void usart_init(uint32_t sclk, uint32_t baudrate);  /* Initialize USART with specified system clock and baud rate */
void USART_SendDatas(USART_TypeDef * USARTx,uint8_t *buf, uint8_t len);
#endif
















