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

    LED_Init();
    BEEP_Init();
    KEY_Init();
    gy39_Init();
    USART1_Init(9600);
    delay_ms(10);
    while(1)
    {
        if(KEY_Scan(0) == 1)
        {
            BEEP = 1;
            delay_ms(100);
            BEEP = 0;
        }
        else
        {
            BEEP = 0;
        }
    }
    return 0;
}