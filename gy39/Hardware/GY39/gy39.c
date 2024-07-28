#include "./GY39/gy39.h"
#include "./usart/usart.h"
#include "stdio.h"
#include "string.h"
unsigned char GY39_cmd1[] = {0xA5, 0x81, 0x26};
unsigned char GY39_cmd2[] = {0xA5, 0x82, 0x27};
unsigned char gy39_mode = 0;
unsigned char recvbuf[32] = {0};
unsigned char data_len = 0;
unsigned char recv_ok = 0;
unsigned int databuf = 0;

void gy39_init(void)
{
	usart2_init(9600);
}

void gy39_start(unsigned char mode)
{
	gy39_mode = mode;
	if (gy39_mode == GUANGZHAO)
	{
		USART_SendDatas(USART2, GY39_cmd1, strlen((char *)GY39_cmd1));
	}
	else if (gy39_mode == OTHER)
	{
		USART_SendDatas(USART2, GY39_cmd2, strlen((char *)GY39_cmd2));
	}
}

/**
 * @brief Get the gy39 data object
 * @note 传感器输出光照数据时会出现错误的值，采用数据缓存的方式，当光照强度小于1000时，更新databuf和data[0]为新值，否则保持data[0]为databuf的值
 * @param data[]
 * 		data[0] is the light intensity
 * 		data[1] is the temperature
 * 		data[2] is the air pressure
 * 		data[3] is the humidity
 * 		data[4] is the altitude
 */
void get_gy39_data(unsigned int data[])
{
	if (gy39_mode == GUANGZHAO)
	{
		// 先从recvbuf中获取数据
		unsigned int lightData = recvbuf[4] << 24 | recvbuf[5] << 16 | recvbuf[6] << 8 | recvbuf[7];
		lightData = lightData / 100;
		printf("Raw light intensity is %d\r\n", &lightData);

		// 当光照强度小于1000时，更新databuf和data[0]为新值
		if (lightData < 1000)
		{
			databuf = lightData;
			data[0] = lightData;
			printf("Updated light intensity is %d\r\n", data[0]);
		}
		else
		{
			// 当光照强度大于等于1000时，保持data[0]为databuf的值，不更新databuf
			data[0] = databuf;
			printf("Light intensity exceeds threshold, using databuf value: %d\r\n", data[0]);
		}
		printf("Final databuf value is %d\r\n", databuf);
	}

	else if (gy39_mode == OTHER)
	{
		data[1] = recvbuf[4] << 8 | recvbuf[5];
		data[1] = data[1] / 100;
		data[2] = recvbuf[6] << 24 | recvbuf[7] << 16 | recvbuf[8] << 8 | recvbuf[9];
		data[2] = data[2] / 100;
		data[3] = recvbuf[10] << 8 | recvbuf[11];
		data[3] = data[3] / 100;
		data[4] = recvbuf[12] << 8 | recvbuf[13];
		printf("Temperature is %d, Humidity is %d, Air Pressure is %d, Altitude is %d\r\n", data[1], data[3], data[2], data[4]);
	}

	recv_ok = 0;
	data_len = 0;
}
