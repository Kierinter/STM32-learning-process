#include "stm32f4xx.h"

#include "./LED/led.h"
#include "./delay/delay.h"
#include "./KEY/key.h"
#include "./BEEP/beep.h"
#include "./GY39/gy39.h"
#include "./usart/usart.h"
#include "./sys/sys.h"


int main(void)
{
    unsigned int gy39_data[5] = {0};
    get_gy39_data(gy39_data);
    int key = 0;

    LED_Init();
    BEEP_Init();
    KEY_Init();
    exit_init();
    gy39_Init();
    usart_init(84, 9600);
    delay_ms(10);
    LED0(0);
    LED1(1);
    while(1)
    {
        key = key_scan(0);
        if(key == WKUP_PRES)
        {
            LED0_TOGGLE();
		    printf("Raw light intensity is %d\r\n", gy39_data[0]);
            key = 0;

        }
        delay_ms(50);
        if(key == KEY1_PRES)
        {
            LED1_TOGGLE();
            BEEP(1);
            delay_ms(10);
		    printf("Temperature is %d, Humidity is %d, Air Pressure is %d, Altitude is %d\r\n", gy39_data[1], gy39_data[3], gy39_data[2], gy39_data[4]);
            key = 0;
        }
    }
    return 0;
}