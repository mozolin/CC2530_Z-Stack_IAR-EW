#include "hal_oled12864.h"
#include "font_v_picture.h"
#include "cc2530_ioctl.h"
#include "hal_delay.h"

void main(void)
{
    setSystemClk32MHZ();

    halOLED12864Init();

    while(1)
    {
        /* Test1 - Show 8x16 Char */
        halOLED12864ShowX16(0, 0, "0123456789");
        halOLED12864ShowX16(1, 0, "abcdefghiABCDE");
        halOLED12864ShowX16(2, 0, "{}[]()!@#$%");
        halOLED12864ShowX16(3, 0, "==========>");

        delayMs(SYSCLK_32MHZ, 4000);
        halOLED12864ClearScreen();

        /* Test2 - Show 8x16 Char and 16x16 Chinese Char */
        halOLED12864ShowX16(0, 0,  "今天气温：");
        halOLED12864ShowX16(1, 30, "温度：22 ℃");
        halOLED12864ShowX16(2, 30, "湿度：30 %");

        delayMs(SYSCLK_32MHZ, 4000);
        halOLED12864ClearScreen();

        /* Test3 - Show 32x32 Picture in point: (30, 30) */
        halOLED12864ShowPicture(30, 30, 32, 32, Picture_32x32_AppleIco);

        delayMs(SYSCLK_32MHZ, 4000);
        halOLED12864ClearScreen();

        /* Test4 - Show 128x64 Picture in point: (0, 0) */
        halOLED12864ShowPicture(0, 0, 128, 64, Picture_128x128_SuccessPic);

        delayMs(SYSCLK_32MHZ, 4000);
        halOLED12864ClearScreen();
    }
}
