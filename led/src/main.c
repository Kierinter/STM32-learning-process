/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/*�����Զ���ͷ�ļ�*/
#include "gec_led.h"
#include "gec_key.h"
#include "gec_exti.h"
#include "gec_delay.h"
#include "gec_usart.h"
#include "gec_w25q128.h"


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	int i = 0;
	u8 addr = 0x10;
	u16 w25q128_id = 0;
	char str[] = "ouyang hualiang zhen shuai!\n";
	char buf[10] = {0};
	/* �����жϵ����ȼ�ռ�� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
			 
	
	gec_led_init();							//��ʼ��LED��	
	//gec_key_init();
	gec_exti_init();

	

	/* ����TIM13����Ƚ� ��������*/
	//timer13_out_init(10000,5000);


	/* ���õ��Դ��� */
	debug_init(115200);
	//gec_delay_ms(5000);

	
	/* IIC at24c02*/
	at24c02_gpio_init();
	gec_delay_ms(1000);
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	read_byte_to_AT24C02(0x10,buf,5);
	printf("д��ǰ:%s\n",buf);
	
	for(i=0;i<5;i++)
	{
		write_a_byte_to_AT24C02(addr++,str[i]);
	}
	read_byte_to_AT24C02(0x10,buf,5);
	printf("д���:%s\n",buf);

	
	/* ����W25Q128 */
	w25q128_gpio_init();
	w25q128_id = w25q128_read_id();
	printf("����ID:0x%x\t�豸ID:0x%x\n",w25q128_id>>8,w25q128_id&0xff);
	
	u8 recv[20] = {0};
	read_bytes_from_w25q128(0x784ffe,recv,20);
	printf("w25q128 read:\n\t");
	for(i=0;i<20;i++)
	{
		printf("0x%-2x ",recv[i]);
	}
	write_bytes_from_w25q128(0x784ffe,"12345678900987654321",20);
	read_bytes_from_w25q128(0x784ffe,recv,20);
	printf("w25q128 read:\n\t");
	for(i=0;i<20;i++)
	{
		printf("0x%-2x ",recv[i]);
	}
	
	while(1)
	{
		LED0_CHANGE;
		gec_delay_ms(1000);
		if(recv_flag == 1)
		{
			debug_send_str(recv_buf,strlen(recv_buf));		//ע��,ʹ�ñ�׼C�⺯��ʱ,��Ҫ��ѡ options�� --> Target --> ��ѡ Use MircoLIB
			recv_flag = 0;									//�������ѡ,�����޷�ִ��
			memset(recv_buf,0,512);
		}													
	}
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
