#ifndef __DMA_H
#define __DMA_H

#include"./sys/sys.h"

void dma_enable(DMA_Stream_TypeDef *dma_streamx, uint16_t ndtr);                                                /* 使能一次DMA传输 */
void dma_usart_tx_config(DMA_Stream_TypeDef *dma_streamx, uint8_t ch, uint32_t par, uint32_t mar);              /* 串口1 TX DMA初始化 */
void dma_basic_config(DMA_Stream_TypeDef *dma_streamx,uint32_t ch, uint32_t par, uint32_t m0ar, uint32_t m1ar); /* DMA基本配置 */
#endif