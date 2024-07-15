#include "./OLED/OLED_Font.h"
#include "Delay.h"
#include "stm32f4xx.h"

/* OLED screen address */
#define OLED_ADDRESS 0x78

// I2C timeout time
#define I2C_TIMEOUT 5000

/* Pin initialization */
void OLED_I2C_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);	// Enable I2C1 clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // Enable GPIOB clock

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1); // Enable PB6's alternate function to connect to I2C1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	/* Hardware I2C of STM32F407 chip: PB6 -- SCL; PB7 -- SDA */
	GPIO_InitTypeDef GPIO_InitStructure;									 // Define structure to configure GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					 // Set GPIO mode to alternate function mode
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;			 // Set GPIO speed to 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;				 // Set GPIO to open-drain output
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					 // Set GPIO pull-up mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // Set GPIO pins
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C1); // Reset the I2C1 peripheral registers to their default values
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;																// Working mode
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;												// Clock duty cycle, Tlow/Thigh = 2
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;																	// I2C address of the host, not used, can be set randomly
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;																// Enable acknowledge bit
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // Set address length to 7 bits
	I2C_InitStructure.I2C_ClockSpeed = 400000;																// I2C transmission speed, 400K, check the supported speed in the chip manual.
	I2C_Init(I2C1, &I2C_InitStructure);

	I2C_Cmd(I2C1, ENABLE);
}

void HW_I2cWrite(uint8_t *buf, uint8_t len)
{
	if (len <= 0)
		return;
	uint32_t wait_time = 0;

	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		wait_time++;
		if (wait_time >= I2C_TIMEOUT)
		{
			wait_time = 0;
			break;
		}
	}

	I2C_GenerateSTART(I2C1, ENABLE);														// Enable I2C1
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) /*EV5, master mode*/
	{
		wait_time++;
		if (wait_time >= I2C_TIMEOUT)
		{
			wait_time = 0;
			break;
		}
	}

	I2C_Send7bitAddress(I2C1, OLED_ADDRESS, I2C_Direction_Transmitter); // Device address -- default 0x78
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		wait_time++;
		if (wait_time >= I2C_TIMEOUT)
		{
			wait_time = 0;
			break;
		}
	}

	for (uint8_t i = 0; i < len; i++)
	{
		I2C_SendData(I2C1, buf[i]); // Send data
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			wait_time++;
			if (wait_time >= I2C_TIMEOUT)
			{
				wait_time = 0;
				break;
			}
		}
	}
	I2C_GenerateSTOP(I2C1, ENABLE); // Close I2C1 bus
}
