#include "./BEEP/beep.h"
#include "./KEY/key.h"
#include "./LED/led.h"
#include "./OLED/oled.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

int main(void)
{
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz */
    uint8_t key_value = 0;              /* 按键默认值 */
    delay_init(168);                    /* 延时初始化 */
    led_init();                         /* 初始化LED */
    beep_init();
    key_init();

    LED0(0); /* LED0 亮 */
    LED1(1); /* LED1 灭 */
    delay_ms(500);
    LED0(1); /* LED0 灭 */
    LED1(0); /* LED1 亮 */
    delay_ms(500);


    key_value = key_scan(0); /* 扫描按键 */
    
    if (key_value)
    {
        switch (key_value)
        {
        case KEY0_PRES: /* KEY0 按下 */
            LED0(0);
            delay_ms(500);
            /* code */
            break;
        case KEY1_PRES: /* KEY1 按下 */
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
