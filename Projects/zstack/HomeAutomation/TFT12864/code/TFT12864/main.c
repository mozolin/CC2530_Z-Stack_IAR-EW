#include "hal_TFT.h"
#include "font_rgb_picture.h"
#include "hal_delay.h"

#define RED     HAL_TFT_PIXEL_RED
#define GREEN   HAL_TFT_PIXEL_GREEN
#define BLUE    HAL_TFT_PIXEL_BLUE
#define YELLOW  HAL_TFT_PIXEL_YELLOW
#define WHITE   HAL_TFT_PIXEL_WHITE
#define BLACK   HAL_TFT_PIXEL_BLACK

void main(void)
{
    setSystemClk32MHZ();
    
    halTFTInit(HAL_TFT_PIXEL_WHITE);
    delayMs(SYSCLK_32MHZ, 500);

    while(1)
    {
        /* Test1 - Shuabing */
        halTFTSetScreen(RED);
        delayMs(SYSCLK_32MHZ, 500);
        
        halTFTSetScreen(GREEN);
        delayMs(SYSCLK_32MHZ, 500);
        
        halTFTSetScreen(BLUE);
        delayMs(SYSCLK_32MHZ, 500);
        
        halTFTSetScreen(YELLOW);
        delayMs(SYSCLK_32MHZ, 500);

        halTFTSetScreen(WHITE);
        delayMs(SYSCLK_32MHZ, 1000);
        
        /* Test2 - Show 8x16 Char and 16x16 Chinese Char */
        halTFTShowX16(0, 0,  BLACK, WHITE,  "½ñÌì: 2018/1/31");
        halTFTShowX16(0, 16, RED,   WHITE,  "ÆøÎÂ(T & H):");
        halTFTShowX16(0, 32, GREEN,  WHITE,  "Temperature: 25");
        halTFTShowX16(0, 48, BLUE,  WHITE,  "Humidity: 30%");
        
        delayMs(SYSCLK_32MHZ, 4000);
        halTFTSetScreen(WHITE);
        
        /* Test3 - Show 40x40 RGB picture */
        halTFTShowPicture(40,12, 40, 40, Picture_40x40_WeiXinIco);
        delayMs(SYSCLK_32MHZ, 4000);
    }
}
