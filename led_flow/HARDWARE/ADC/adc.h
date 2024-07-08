#ifndef __ADC_H
#define __ADC_H

#include"./sys/sys.h"

/* ADC及引脚 定义 */

#define ADC_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_ADCX_CHY_GPIO_PIN               SYS_GPIO_PIN5 
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()      do{ RCC->AHB1ENR |= 1 << 0; }while(0)   /* PA口时钟使能 */

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CHY                        5                                       /* 通道Y,  0 <= Y <= 19 */ 
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ RCC->APB2ENR |= 1 << 8; }while(0)   /* ADC1 时钟使能 */

/* ADC单通道/多通道 DMA采集 DMA数据流相关 定义 
 * 注意: 这里我们的通道还是使用上面的定义.
 */
#define ADC_ADCX_DMASx                      DMA2_Stream4
#define ADC_ADCX_DMASx_Channel              0                                       /* 通道0 */
#define ADC_ADCX_DMASx_IRQn                 DMA2_Stream4_IRQn
#define ADC_ADCX_DMASx_IRQHandler           DMA2_Stream4_IRQHandler

#define ADC_ADCX_DMASx_IS_TC()              ( DMA2->HISR & (1 << 5) )   /* 判断 DMA2_Stream4 传输完成标志, 这是一个假函数形式,
                                                                         * 不能当函数使用, 只能用在if等语句里面 
                                                                         */
#define ADC_ADCX_DMASx_CLR_TC()             do{ DMA2->HIFCR |= 1 << 5; }while(0)    /* 清除 DMA2_Stream4 传输完成标志 */

/* ADC 温度传感器通道 定义 */ 
#define ADC_TEMPSENSOR_CHX                  16


void adc_init(void);                    /* ADC初始化 */
void adc_channel_set(ADC_TypeDef *adcx, uint8_t ch, uint8_t stime); /* ADC通道设置 */
uint32_t adc_get_result(uint8_t ch);/* 获得某个通道值  */
uint32_t adc_get_result_average(uint8_t ch, uint8_t times); /* 得到某个通道给定次数采样的平均值 */
void  adc_dma_init(uint32_t mar);       /* ADC DMA采集初始化 */
void adc_dma_enable( uint16_t ndtr);    /* 使能一次ADC DMA采集传输 */
void adc_nch_dma_init(uint32_t mar);    /* ADC多通道 DMA采集初始化 */
void adc_temperature_init(void);        /* ADC温度采集初始化函数 */
short adc_get_temperature(void);        /* 获取内部温度传感器温度值 */

#endif