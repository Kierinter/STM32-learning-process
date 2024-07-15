#include "./EXIT/exit.h"
#include "./GY39/gy39.h"
#include "./KEY/key.h"
#include "./LED/led.h"
#include "./MOTOR/motor.h"
#include "./OLED/OLED.h"
#include "./delay/delay.h"
#include "./usart/usart.h"
#include "IWDG.h"
#include "stm32f4xx.h"
#include "u8g2.h"

uint8_t u8x8_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2_Init(u8g2_t *u8g2);
void draw(u8g2_t *u8g2);

/**
 * @brief Main function of the program.
 * @return Integer value indicating the exit status of the program.
 */
int main(void)
{
      sys_stm32_clock_init(336, 8, 2, 7);
      delay_init(168);
      int work_mode = 0;
      uint8_t key = 0;
      led_init();
      key_init();
      exit_init();
      gy39_init();
      Motor_Init();
      usart_init(84, 9600);
      IWDG_Configuration(); // Initialize watchdog
      OLED_I2C_Init();      // Initialize OLED
      u8g2_t u8g2;
      u8g2_Init(&u8g2); // Initialize U8g2
      delay_ms(100);

      u8g2_DrawLine(&u8g2, 0, 0, 127, 63); // Draw a line from (0,0) to (127,63)
      u8g2_SendBuffer(&u8g2);              // Send buffer data
      u8g2_DrawLine(&u8g2, 127, 0, 0, 63);
      u8g2_SendBuffer(&u8g2);
      delay_ms(300);

      u8g2_ClearBuffer(&u8g2); // Clear buffer data
      draw(&u8g2);
      u8g2_SendBuffer(&u8g2);
      delay_ms(1000);

      u8g2_ClearBuffer(&u8g2);
      IWDG_FeedDog(); // Feed the watchdog to prevent CPU reset
      u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
      u8g2_DrawUTF8(&u8g2, 0, 43, "monitor start");
      u8g2_SendBuffer(&u8g2);
      LED0(0);
      delay_ms(1300);
      while (1)
      {
            unsigned int gy39_data[5] = {0};

            u8g2_ClearBuffer(&u8g2); // Clear buffer data
            get_gy39_data(gy39_data);

            key = key_scan(0);
            char strTemp[32];
            char strHumi[32];
            char strPres[32];
            char strLux[32];
            char strAlti[32];
            delay_ms(50);

            // Check the key value
            if (key == WKUP_PRES)
                  work_mode = 0;
            if (key == KEY1_PRES)
                  work_mode = 1;

            if (work_mode == 0)
            {
                  gy39_start(0);
                  u8g2_FirstPage(&u8g2);
                  do
                  {
                        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                        sprintf(strLux, "Lux:%d", gy39_data[0]);
                        printf("Lux:%d\r\n", gy39_data[0]);
                        u8g2_DrawStr(&u8g2, 30, 10, strLux);
                        if (gy39_data[0] > 300)
                        {
                              LED1_TOGGLE();
                              Motor_SetSpeed(50);
                              u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                              u8g2_DrawGlyph(&u8g2, 30, 50, 0x2600);
                        }
                        else
                        {
                              LED0_TOGGLE();
                              Motor_SetSpeed(0);
                              u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                              u8g2_DrawGlyph(&u8g2, 30, 50, 0x2601);
                        }
                  } while (u8g2_NextPage(&u8g2));
                  delay_ms(100);
            }
            if (work_mode == 1)
            {
                  gy39_start(1);
                  u8g2_FirstPage(&u8g2);
                  do
                  {
                        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                        sprintf(strTemp, "Temp:%d", gy39_data[1]);
                        u8g2_DrawStr(&u8g2, 0, 10, strTemp);
                        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                        sprintf(strHumi, "Humi:%d", gy39_data[3]);
                        u8g2_DrawStr(&u8g2, 0, 30, strHumi);
                        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                        sprintf(strPres, "Pres:%d", gy39_data[2]);
                        u8g2_DrawStr(&u8g2, 0, 50, strPres);
                        u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                        sprintf(strAlti, "Alti:%d", gy39_data[4]);
                        u8g2_DrawStr(&u8g2, 64, 10, strAlti);
                  } while (u8g2_NextPage(&u8g2));
                  delay_ms(100);
            }
            IWDG_FeedDog(); // Feed the watchdog to prevent CPU reset
      }
}

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
      static uint8_t buffer[32];
      static uint8_t buf_idx;
      uint8_t *data;

      switch (msg)
      {
      case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while (arg_int > 0)
            {
                  buffer[buf_idx++] = *data;
                  data++;
                  arg_int--;
            }
            break;

      case U8X8_MSG_BYTE_INIT:
            break;

      case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;

      case U8X8_MSG_BYTE_END_TRANSFER:
            HW_I2cWrite(buffer, buf_idx); // 硬件I2C写字节
            break;

      default:
            return 0;
      }
      return 1;
}

uint8_t u8g2_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
      switch (msg)
      {
      case U8X8_MSG_GPIO_AND_DELAY_INIT:
            OLED_I2C_Init(); // 初始化
            break;

      case U8X8_MSG_DELAY_MILLI:
            delay_ms(arg_int); // 延时
            break;

      case U8X8_MSG_GPIO_I2C_CLOCK:
            break;

      case U8X8_MSG_GPIO_I2C_DATA:
            break;

      default:
            return 0;
      }
      return 1; // command processed successfully.
}

void u8g2_Init(u8g2_t *u8g2)
{
      u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8g2_gpio_and_delay); // 初始化 u8g2，硬件I2C
      u8g2_InitDisplay(u8g2);                                                                       // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
      u8g2_SetPowerSave(u8g2, 0);                                                                   // 打开显示器
      u8g2_SetContrast(u8g2, 88);                                                                   // 设置屏幕亮度
      u8g2_ClearBuffer(u8g2);                                                                       // 清除缓冲区
}

void draw(u8g2_t *u8g2)
{
      u8g2_SetFontMode(u8g2, 1);              /*字体模式选择*/
      u8g2_SetFontDirection(u8g2, 0);         /*字体方向选择*/
      u8g2_SetFont(u8g2, u8g2_font_inb24_mf); /*字库选择*/
      u8g2_DrawStr(u8g2, 0, 20, "U");

      u8g2_SetFontDirection(u8g2, 1);
      u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
      u8g2_DrawStr(u8g2, 21, 8, "8");

      u8g2_SetFontDirection(u8g2, 0);
      u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
      u8g2_DrawStr(u8g2, 51, 30, "g");
      u8g2_DrawStr(u8g2, 67, 30, "\xb2");

      u8g2_DrawHLine(u8g2, 2, 35, 47);
      u8g2_DrawHLine(u8g2, 3, 36, 47);
      u8g2_DrawVLine(u8g2, 45, 32, 12);
      u8g2_DrawVLine(u8g2, 46, 33, 12);

      u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
      u8g2_DrawStr(u8g2, 1, 54, "github.com/olikraus/u8g2");
}
