#include "stm32f4xx.h"

/*

		PB8		IIC_SCL
		PB9		IIC_SDA
*/
void at24c02_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	I2C_InitTypeDef   I2C_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitStruct.GPIO_Pin			=	GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_OType			=	GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Mode			=	GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed			= 	GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd			=	GPIO_PuPd_UP;			//���IIC��������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);



	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_InitStruct.I2C_Mode					=	I2C_Mode_I2C;
	I2C_InitStruct.I2C_ClockSpeed			=	400000;
	I2C_InitStruct.I2C_DutyCycle			=	I2C_DutyCycle_2;
	I2C_InitStruct.I2C_AcknowledgedAddress	=	I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_OwnAddress1			=	0x55;	//���豸��ַ����
	I2C_InitStruct.I2C_Ack					=	I2C_Ack_Enable;;
	I2C_Init(I2C1,&I2C_InitStruct);


	I2C_Cmd(I2C1,ENABLE);
}

/*
	wait_iic_event,�ȴ��¼�����(���޵ȴ�,����ʵ��Ӧ����һ�㲻������)

	����ֵ:
		0 ��ʱ,�¼�û�з���
		1 �¼�������
*/
int wait_iic_event(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT,u32 timeout)
{
	while((I2C_CheckEvent(I2Cx,I2C_EVENT)==ERROR)  && --timeout)  //�¼�û����,����û�г�ʱ,�ͼ����ȴ�
	{
		gec_delay_us(1);	//ÿ��1us�ж�һ���¼�,�������ֻ��ȴ� timeout us
	}

	if(timeout == 0)
	{
		return 0;
	}

	return 1;
}

/*
	at24c02_is_busy ȷ��AT24C02оƬ����

	����ֵ:
		0 ����
		1 æ��
*/
int at24c02_is_busy(void)
{
	//I2C_GenerateSTART(I2C1,ENABLE);
	//while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) == ERROR); //һ��ʵ�ʹ�����,��������,�������޵ȴ�

	int t = 20; //��ʱʱ��
	int r = 1;

	while(t--)
	{
		I2C_GenerateSTART(I2C1,ENABLE);
		if( wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000) == 0 ) //�ȴ���ʱ,�¼�δ����
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			continue;
		}

		I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Transmitter); //������߲�æµ,����7bit�豸��ַ
		if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED, 1000) == 0)
		{
			r = 0;  //AT24C02��Ӧ�˵�ַ,˵������,����ͨ��
			break;
		}
	}


	I2C_GenerateSTOP(I2C1,ENABLE);
	return r;
}


/*
	write_a_byte_to_AT24C02 ������AT24C02д��һ���ֽ�����
			@word_addr: ָ���ֵ�ַ
			@dataд�������
	
*/
u8  write_a_byte_to_AT24C02(u8 word_addr,u8 data)
{
	int r = 0;
	if( at24c02_is_busy() == -1 )
	{
		printf("AT24C02 is BUSY\n");
		r = -20;
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//������ʼ�ź�
	I2C_GenerateSTART(I2C1, ENABLE);
	
	//�ȴ�һ���·�ָʾSTART�źŷ��ͳ���
	if ( !wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000))
	{
		r = -5;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//2. ����һ�����豸�ĵ�ַ
	I2C_Send7bitAddress(I2C1, 0xa0, I2C_Direction_Transmitter);

	if (!wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
			1000))
	{
		r = -6;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	
	//�����ֵ�ַ (����)
	I2C_SendData(I2C1,word_addr);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING, 1000) == 0)
	{
		r = -3;  //AT24C02��Ӧ�˵�ַ,˵������,����ͨ��
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//��������Ҫ�洢������
	I2C_SendData(I2C1,data);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING, 1000) == 0)
	{
		r = -4;  //AT24C02��Ӧ�˵�ַ,˵������,����ͨ��
		goto err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
err:
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	I2C_GenerateSTOP(I2C1,ENABLE);
	return r;

}

/*
	write_byte_to_AT24C02 ������ AT24C02��д������
		@word_addr:�ֵ�ַ,д�����ݵ���ʼ��ַ
		@data: Ҫд�������
		@count: Ҫд����ֽ���
*/
int write_byte_to_AT24C02(uint8_t word_addr,uint8_t *data,int count)
{
	
}

/*
	read_byte_to_AT24C02 ������ AT24C02�ж�ȡ����
		@word_addr:�ֵ�ַ,ָ�����ĸ���ַ��ʼ��ȡ����
		@data: ָ���ռ�(������),�����洢��ȡ��������
		@count: Ҫ��ȡ���ֽ���
*/
int read_byte_to_AT24C02(uint8_t word_addr,uint8_t *data,int count)
{
	int i = 0;
	int r = 0;
	/*
	if( at24c02_is_busy() == 1 )
	{
		printf("AT24C02 is BUSY\n");
		r = -20;
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	*/
	
	/*
		��AT24C02�ֲ��10ҳ��2��
	*/
//����24C02 "��"Ҫ�����ĸ��ط�������	
	//������ʼ�ź�
	I2C_GenerateSTART(I2C1, ENABLE);


	//�ȴ�һ���·�ָʾSTART�źŷ��ͳ���
	if ( !wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000))
	{
		r = -5;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//2. ����һ�����豸�ĵ�ַ
	I2C_Send7bitAddress(I2C1, 0xa0, I2C_Direction_Transmitter);

	if (!wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
			1000))
	{
		r = -6;
		//printf("%s L_%d\n", __FUNCTION__, __LINE__);
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	
	//�����ֵ�ַ (����)
	I2C_SendData(I2C1,word_addr);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING, 1000) == 0)
	{
		r = -3;  //AT24C02��Ӧ�˵�ַ,˵������,����ͨ��
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);

//��ȡ24c02������
	//������ʼ�ź�
	I2C_GenerateSTART(I2C1,ENABLE);
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_MODE_SELECT, 1000) == 0 ) //�ȴ���ʱ,�¼�δ����
	{
		r = -1;
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//����7bit��ַ
	I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Receiver); //������߲�æµ,����7bit�豸��ַ
	if( wait_iic_event(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, 1000) == 0)
	{
		r = -2;  //AT24C02��Ӧ�˵�ַ,˵������,����ͨ��
		goto read_err;
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	for(i=0;i<count-1;i++)   // -1����Ϊ�������һ���ֽ���ǰ,Ҫ�ظ�NA,���Էŵ����浥������
	{
		if(wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 1000) == 0)   //RXNEδ������
		{
			r = -16;
			goto read_err;
		}
		data[i] = I2C_ReceiveData(I2C1);
		//�����ڳ�ʼ��IICʱ,ʹ����ACK,���Խ���1�ֽں���Զ��ظ�
	}
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	//�ظ� NA ,ֻ�е��Է��յ���NAӦ��,�Է��Ż�ֹͣ������
	I2C_AcknowledgeConfig(I2C1,DISABLE);  
	I2C_GenerateSTOP(I2C1,ENABLE);
	//�������һ���ֽ�
	if(wait_iic_event(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 1000) == 0)   //RXNEδ������
	{
		r = -16;
		goto read_err;
	}
	data[i] = I2C_ReceiveData(I2C1); 
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);	
read_err:
	printf("DEBUG:-%s-%s-%d\n",__FILE__,__FUNCTION__,__LINE__);
	I2C_GenerateSTOP(I2C1,ENABLE);
	return r;
}

