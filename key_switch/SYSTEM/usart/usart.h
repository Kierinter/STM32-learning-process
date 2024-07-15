

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"


/**
 * @brief 
 * 
 */ @file usart.h
 * @brief Header file for USART configuration and functions.
 */

#ifndef USART_H_
#define USART_H_

#include <stdint.h>

#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   SYS_GPIO_PIN9
#define USART_TX_GPIO_AF                    7                           
#define USART_TX_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 0; }while(0)   

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   SYS_GPIO_PIN10
#define USART_RX_GPIO_AF                    7                           
#define USART_RX_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 0; }while(0)   

#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ RCC->APB2ENR |= 1 << 4; }while(0)   

#define USART_REC_LEN               200             
#define USART_EN_RX                 1               

extern uint8_t  g_usart_rx_buf[USART_REC_LEN];       
extern uint16_t g_usart_rx_sta;                     

void usart_init(uint32_t sclk, uint32_t baudrate);  

#endif
















