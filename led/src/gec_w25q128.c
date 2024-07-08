#include "stm32f4xx.h"
#include "gec_w25q128.h"
/*
	spi:
		PB3		SPI1_SCK
		PB4		SPI1_MISO
		PB5		SPI1_MOSI
	#ss
		PB14	F_CS		===> ����Ƭѡ�źź�
*/
void w25q128_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	SPI_InitTypeDef   SPI_InitStruct;
	/*
		����GPIO����:
			1)����Ƭѡ��		<--- ͨ�����
			2)����SPI����<-- ���ù���
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Pin			=		GPIO_Pin_14;		//W25Q128(���豸)��Ƭѡ��
	GPIO_InitStruct.GPIO_Mode			=		GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType			=		GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed			=		GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd			=		GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin			=		GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;	//SPI��
	GPIO_InitStruct.GPIO_Mode			=		GPIO_Mode_AF;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
																					
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);	//SCK   //�����Ǹ�STM32������,Ҳ����STM32��Ϊ���豸
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);	//MISO	==>  SDI
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);	//MOSI	==>  SDO
	

	/*
		����SPI������
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	SPI_InitStruct.SPI_BaudRatePrescaler		=	SPI_BaudRatePrescaler_4;
	//��DI�ϴ������������ʱ�ӵ������ر�����ġ�MSB ���ȱ����䡣
	SPI_InitStruct.SPI_CPOL						=	SPI_CPOL_Low;   	//ʱ�Ӽ���,ʱ���߿���ʱ��״̬
	SPI_InitStruct.SPI_CPHA						=	SPI_CPHA_1Edge; 	//ʱ����λ,�ڵڼ������زɼ�����
	SPI_InitStruct.SPI_DataSize					=	SPI_DataSize_8b;	
	SPI_InitStruct.SPI_Direction				=	SPI_Direction_2Lines_FullDuplex;	//ȫ˫��
	SPI_InitStruct.SPI_FirstBit					=	SPI_FirstBit_MSB;	//��λ�ȷ�
	SPI_InitStruct.SPI_Mode						=	SPI_Mode_Master;	//STM32��Ϊ���豸
	SPI_InitStruct.SPI_NSS						=	SPI_NSS_Soft;	
	SPI_InitStruct.SPI_CRCPolynomial			=	7;					//CRCУ�����ʽ,����1����
	SPI_Init(SPI1,&SPI_InitStruct);


	/*
		ʹ��SPI
	*/
	SPI_Cmd(SPI1,ENABLE);	
}


/*
	spi_send_byte: SPI���ͺ���,������SPI���߷���1���ֽ�
*/
char spi_send_byte(char data)
{
	char recv = 0;
	/*
		�ȷ������ݽ����Ϊ�պ�,�ٷ���һ���ֽڵ�SPI
	*/
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) != SET);		//���SPI��TXE��־δ������,��һֱ�ȴ�
	SPI_SendData(SPI1,data);


	/*
		�ڷ���ʱ,SDI���ϵ�����(��������Ч����)
		Ҳ�ᱻ������

		��SPI���������,��ȫ˫����
		�����������ݵ�ʱ��,SDI���Ͽ���û����Ч����
		����SPI������  �� ��������Э��������
	*/
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)	!= SET);
	recv = SPI_ReceiveData(SPI1);		//��SPI��������(�ڷ���ʱ,��������Ч����)
}

/*

	����ֵ: ���յ���1���ֽ�����
*/
char spi_recv_byte(void)
{
	char recv = 0;
	// ����д�Ĵ�����Ϊ���豸ʱ
	// STM32F407 ��Ϊ���豸,��ôSPI������ֻ���ڷ�������ʱ�Ż����ʱ��
	// ����,��Ϊ���ܹ��յ����豸������,���뷢��һ�� "��Ч����"  ==> Ϊ�˲���ʱ��,�Ӷ��ý�������
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) != SET);		//���SPI��TXE��־δ������,��һֱ�ȴ�
	SPI_SendData(SPI1,0xff);

	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)	!= SET);
	recv = SPI_ReceiveData(SPI1);

	return recv;
}


/*
	w25q128_get_sr1: ��ȡW25Q128��״̬�Ĵ���1 (SR1)

	����ֵ: ���ػ�ȡ����״ֵ̬
*/
u8 w25q128_get_sr1(void)
{
	u8 status = 0;
	/*
	��״̬�Ĵ��� 1��05H���Ͷ�״̬�Ĵ��� 2��35H��ָ��
	��״̬�Ĵ���ָ������� 8 λ״̬�Ĵ���λ������ָ������/CS ���Ϳ�ʼ��Ȼ��ͨ��
	DI ��ʱ�ӵ������ش���ָ����� 05H�����Ĵ��� 1 ָ������� 35H�����Ĵ��� 2 ָ���
	Ȼ��״̬�Ĵ�������Ӧλͨ�� DO ��ʱ�ӵ��½��شӸ�λ����λ���δ����������/CS
	���߽�������״̬�Ĵ�ָ������κ�ʱ��ʹ�ã��ڲ�д��д״̬�Ĵ���ָ����������Ȼ
	���ԡ������Ϳ�����ʱ��� BUSY λ�������Ӧ��ָ��������û�н�����оƬ�ǲ��ǿ�
	�Խ����µ�ָ�
	*/
	W25Q128_ENBALE;				//����ָ������/CS ���Ϳ�ʼ
	spi_send_byte(0x05);		//STM32F407��SPI������������ʱ�����ʱ��

	status = spi_recv_byte();	//����һ���ֽ�(��Ҫ����һ���������ֽ�)

	W25Q128_DISBALE;			//�����/CS���߽�����

	return status;
}


/*
	w25q128_wait_not_busy �����ȴ�w25q128����
*/
void w25q128_wait_not_busy(void)
{
	//���æµ,��һֱ�ȴ�,֪������
	u8 status = 0;
	do
	{
		gec_delay_ms(5);
		status = w25q128_get_sr1();
	}
	while( ( status & (0x01<<0) ) != 0);
}


/*
	w25q128_read_id ��ȡоƬID
		����ID���豸ID
		
		Device ID(2)  90h 	dummy 	dummy 	00h 	(MF7-MF0) 	(ID7-ID0)
*/
u16 w25q128_read_id(void)
{
	u8 cid,did;

	W25Q128_ENBALE;				//����,ʹ��оƬ
	spi_send_byte(0x90);		//����ָ��
	spi_send_byte(0x00);
	spi_send_byte(0x00);
	spi_send_byte(0x00);

	cid = spi_recv_byte();		//��ȡ����ID
	did = spi_recv_byte();		//��ȡ�豸ID

	W25Q128_DISBALE;			//����,��ֹоƬ

	return cid<<8 | did;
	
}


/*
	read_bytes_from_w25q128: ����W25Q128�洢���ж�ȡ����
		@ addr: ָ��w25q128�洢���еĴ洢��Ԫ��ַ
		@ data: ָ��һ���ڴ�ռ�,�������ȡ��������
		@ count: ָ����ȡ����,��λ �ֽ�
	����ֵ:
		ʧ�ܷ���0
		�ɹ����� ��0
*/
int read_bytes_from_w25q128(uint32_t addr,uint8_t *data,int count)
{
	/*
		����� BUSY=1 ʱִ�и�ָ���ָ������ԣ����Ҷ�����ִ�е�����ָ�������
		��Ӱ�졣
		������ָ������Ӵ洢����һ���ֽں���������ֽڡ���ָ������/CS ���Ϳ�ʼ��Ȼ��
		ͨ DI ��ʱ�ӵ�������������ָ����루03H���� 24 λ��ַ����оƬ�������ַλ��
		��Ӧ��ַ����ֵ���ᣬ��ʱ�ӵ��½��أ��Ը�λ��ǰ��λ�ں�ķ�ʽ���� D0 �ϴ��䡣
		��������Ķ�����ֽڵĻ�����ַ���Զ��� 1 �ġ�����ζ�ſ���һ�ζ�������оƬ����
		ָ��Ҳ����/CS �����������ġ�	
	*/

	int i = 0;
	int max_bytes = 0;
	/* �����Ϸ��Լ�� */
	if(addr < 0  || addr>0xffffff)  //��ַ�Ϸ���
	{
		printf("ָ���ĵ�ַ���Ϸ�\n");
		return -1;
	}

	max_bytes = 0x1000000 - addr;	//�����ĵ�ַ���ɶ�ȡ���ֽ���
	if(count > max_bytes)			//���Ҫ��ȡ���ֽ�������������Χ
	{
		count = max_bytes;			//��ֻ�ܶ�ȡ�Ϸ�����
	}
	
	
	/* ����Ƿ�æµ */
	w25q128_wait_not_busy();

	/* /CS���� */
	W25Q128_ENBALE;

	/* ����ָ�� �� 24bits��ַ */
	spi_send_byte(0x03);
	spi_send_byte((addr>>16)&0xff);
	spi_send_byte((addr>> 8)&0xff);
	spi_send_byte((addr>> 0)&0xff);

	for(i=0;i<count;i++)
	{
		data[i] = spi_recv_byte();
	}

	W25Q128_DISBALE;

	return i;
	
}

/*
	дʹ��
*/
void w25q128_write_enable(void)
{
	W25Q128_ENBALE;
	spi_send_byte(0x06);
	W25Q128_DISBALE;
}

/*
	��������
*/
void sector_erase_to_w25q128(u32 addr)
{
	/*
		ΪʲôҪ�Ȳ���(��������ȫ����Ϊ1)����д???
		��Ϊ  �Լ����ֲ�
		ҳ���ָ������ 1 ��256 �ֽ�д��洢����ĳһҳ����һҳ�����Ǳ��������ģ�Ҳ����ֻ��д
		0������д 1������ʱ��ȫдΪ 1�� ��
	*/
	w25q128_write_enable();	//дʹ��

	W25Q128_ENBALE;
	/* ����ָ�� �� 24bits��ַ */
	spi_send_byte(0x20);
	spi_send_byte((addr>>16)&0xff);
	spi_send_byte((addr>> 8)&0xff);
	spi_send_byte((addr>> 0)&0xff);

	W25Q128_DISBALE;

	w25q128_wait_not_busy(); //�ȴ��������
	
}


void w25q128_page_program(uint32_t addr,uint8_t *data,int count)
{
	int i = 0;
	int max_bytes = 0;
	/* �����Ϸ��Լ�� */
	if(addr < 0  || addr>0xffffff)  //��ַ�Ϸ���
	{
		printf("ָ���ĵ�ַ���Ϸ�\n");
		return -1;
	}

	max_bytes = 256 - (addr & 0xff);	//�����ĵ�ַ���ɶ�ȡ���ֽ���
	if(count > max_bytes)				//���Ҫ��ȡ���ֽ�������������Χ
	{
		count = max_bytes;				//��ֻ�ܶ�ȡ�Ϸ�����
	}


	w25q128_write_enable();	//дʹ��

	/* /CS���� */
	W25Q128_ENBALE;

	/* ����ָ�� �� 24bits��ַ */
	spi_send_byte(0x02);
	spi_send_byte((addr>>16)&0xff);
	spi_send_byte((addr>> 8)&0xff);
	spi_send_byte((addr>> 0)&0xff);

	for(i=0;i<count;i++)
	{
		spi_send_byte(data[i]);
	}
	W25Q128_DISBALE;
	w25q128_wait_not_busy(); //�ȴ�д���

	return i; //ʵ��д���ֽ���
}

u8 buffer[4096]; 	//�����洢��ȡ������һ������������,Ϊʲô������ֲ�������
						// �ر�ע��:  ջ�Ŀռ䲻��,ȫ�ֱ���������ʹ��ջ�ռ�
						//����ֲ���������̫�� �� ̫��,�ᵼ�³������
/*
	write_bytes_from_w25q128: ������W25Q128�洢����ָ���洢��Ԫ�ռ�д������
		@ addr: ָ��W25Q128�ڵĴ洢�ռ��ַ
		@ data: ָ��Ҫд�������
		@ count: ָ��Ҫд�������,��λ�ֽ�
	����ֵ:
		ʧ�ܷ���0
		�ɹ����� ��0
*/	
int write_bytes_from_w25q128(uint32_t addr,uint8_t *data,int count)
{
	/*
		���д������ݳ���һ������,Ӧ����ô����????
			�ȴ����һ������,Ȼ����ڶ�����,Ȼ������������� ....
	*/

	u32 first_sector_addr = addr & ~0xfff;	//��¼Ҫд��������׵�ַ
	int i;
	int write_byte = 0;		//����д����ֽ���
	if(addr > 0xffffff)
	{
		printf("ָ���ĵ�ַ���Ϸ�\n");
		return -1;
	}

	if(count > (0x1000000 - addr))
	{
		count = 0x1000000 - addr;
	}

	/* ��ȡһ������������ */
	read_bytes_from_w25q128(first_sector_addr,buffer,4096);		//��ȡһ������������

	/* ��Ҫд��������滻������ */
	for(i=0;i<count;i++)
	{
		buffer[i+(addr&0xfff)] = data[i];  
	}

	/* �������� */
	sector_erase_to_w25q128(first_sector_addr);

	for(i=0;i<4096;i+=256)
	{
		w25q128_page_program(first_sector_addr+i,buffer+i,256);	//ҳд
	}
	
}

