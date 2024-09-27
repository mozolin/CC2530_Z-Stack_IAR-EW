#include "hal_dht11.h"
#include "hal_oled12864.h"
#include "hal_tft.h"
#include "hal_delay.h"
#include <stdio.h>

/* Select LCD type. */  
#define LCD_OLED12864

void main(void)
{
    halDHT11Data_t dht11Dat;
    uint8 tempStr[50], humiStr[50], idxStr[50];
    uint8 idx = 0;

    setSystemClk32MHZ();

#ifdef LCD_OLED12864
    halOLED12864Init();
#else
    halTFTInit(HAL_TFT_PIXEL_WHITE);
#endif

    halDHT11Init();

    while(1)
    {
        halOLED12864ClearScreen();
        
        idx++;
        sprintf((char *)idxStr, "Idx: %d", idx);
        halOLED12864ShowX16(0,0, idxStr);
        
        dht11Dat = halDHT11GetData();
        if (dht11Dat.ok) {
            sprintf((char *)tempStr, "Temp: %d", dht11Dat.temp);
            sprintf((char *)humiStr, "Humi: %d", dht11Dat.humi);
          
        #ifdef LCD_OLED12864
            halOLED12864ShowX16(1,0, tempStr);
            halOLED12864ShowX16(2,0, humiStr);
        #else
            halTFTShowX16(0,0, HAL_TFT_PIXEL_RED, HAL_TFT_PIXEL_WHITE, tempStr);
            halTFTShowX16(0,16, HAL_TFT_PIXEL_RED, HAL_TFT_PIXEL_WHITE, humiStr);
        #endif
        }
        
        delayMs(SYSCLK_32MHZ, 4000);
    } /* while */
}
