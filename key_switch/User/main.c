#include "./BEEP/beep.h"
#include "./KEY/key.h"
#include "./LED/led.h"
#include "./OLED/oled.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

int main(void)
{
    sys_stm32_clock_init(336, 8, 2, 7); /* ����ʱ��,168Mhz */
    uint8_t key_value = 0;              /* ����Ĭ��ֵ */
    delay_init(168);                    /* ��ʱ��ʼ�� */
    led_init();                         /* ��ʼ��LED */
    beep_init();
    key_init();

    LED0(0); /* LED0 �� */
    LED1(1); /* LED1 �� */
    delay_ms(500);
    LED0(1); /* LED0 �� */
    LED1(0); /* LED1 �� */
    delay_ms(500);


    key_value = key_scan(0); /* ɨ�谴�� */
    
    if (key_value)
    {
        switch (key_value)
        {
        case KEY0_PRES: /* KEY0 ���� */
            LED0(0);
            delay_ms(500);
            /* code */
            break;
        case KEY1_PRES: /* KEY1 ���� */
            LED1(0);
            delay_ms(500);
            /* code */
            break;
        case KEY2_PRES:
            LED0(0);
            LED1(0);
            delay_ms(500);
            break;
        case KEY3_PRES:
            LED0(1);
            LED1(1);
            delay_ms(500);
            break;

        default:
            break;
        }
    }
}
