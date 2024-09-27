#include "hal_tft.h"
#include "font_h_8x16.h"
#include "font_chinese_h_16x16.h"
#include "hal_delay.h"

/* Write a uint16 data to TFT. */
#define HAL_TFT_WRITE_UINT16(val) do {    \
    halLcdSpiTxData((uint8)((val) >> 8)); \
    halLcdSpiTxData((uint8)(val));        \
} while(0)
                  
static void halTFTReset(void);   
static void ST7735RInit(void);
static void halTFTSetRegion(uint16 xs, uint16 ys, uint16 xe, uint16 ye);
static void halTFTSetPosition(uint16 x, uint16 y);
static void halTFTDrawPixel(uint16 xs, uint16 ys, uint16 color);
static void halTFTShowChar8x16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch);
static void halTFTShowChineseChar16x16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 chL, uint8 chR);

void halTFTInit(uint16 screenColor)
{    
    /* Init SPI-GPIO */
    halLcdSpiInit();
    
    /* Init LCD Chip: ST7735R */
    ST7735RInit();
    
    /* Setting */
    halTFTSetPosition(0, 0);
    halTFTSetScreen(screenColor);
}

void halTFTSetScreen(uint16 pixelVal)
{
    uint16 x, y;
    
    halTFTSetRegion(0, HAL_TFT_Y_OFFSET, HAL_TFT_X-1, HAL_TFT_Y-1);
    
    for (x = 0; x < HAL_TFT_X; x++)
        for (y = HAL_TFT_Y_OFFSET; y < HAL_TFT_Y; y++) HAL_TFT_WRITE_UINT16(pixelVal);
}

void halTFTShowX16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, const uint8 *str)
{
    if (!str) return;
    
    y += HAL_TFT_Y_OFFSET;
    const uint8 *ptext = str; // text

    /* Show text */
    while(*ptext != 0) {
        /* ASCII Code: 0~127 */
        if ((*ptext) < 128) {
            /* End of line */
            if((x + 8) > HAL_TFT_X) return;
          
            /* Show 8x16 ASCII Char. */
            halTFTShowChar8x16(x, y, fontColor, backgroundColor, *ptext);
            x += 8;
            
            ptext++;
        }
        /* Chinese 16x16 characters */
        else {
            /* End of line */
            if((x + 16) > HAL_TFT_X) return;
            
            halTFTShowChineseChar16x16(x, y, fontColor, backgroundColor, *ptext, *(ptext + 1));
            
            x += 16;
            
            ptext += 2;
        }
    } /* while(*ptext != 0) */
}

void halTFTShowPicture(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic)
{
    uint8 picL, picH;
    const uint8 *pPic = pic;
    uint16 picSize = (uint16)picWidth * picHeight;

    y += HAL_TFT_Y_OFFSET;
    
    if (!pPic || (x + picWidth)  > HAL_TFT_X ||  (y + picHeight) > HAL_TFT_Y) return;

    /* Set region */
    halTFTSetRegion(x, y, x + picWidth - 1, y + picHeight - 1);

    /* Show Picture */
    for (uint16 i = 0; i < picSize; i++) {	
    #ifdef HAL_TFT_PIC_MSB
        picH = *(pPic + i*2);     // High 8Bit
        picL = *(pPic + i*2 + 1); // Low 8Bit
    #else
        picL = *(pPic + i*2);     // Low 8Bit
        picH = *(pPic + i*2 + 1); // High 8Bit
    #endif
          
        HAL_TFT_WRITE_UINT16((uint16)picH<<8 | picL);  						
    }
}

static void halTFTReset(void)
{
    SPI_GPIO_SET(HAL_LCD_SPI_RST_PORT, HAL_LCD_SPI_RST_PIN);
    delayMs(SYSCLK_32MHZ, 100);
    
    SPI_GPIO_CLEAR(HAL_LCD_SPI_RST_PORT, HAL_LCD_SPI_RST_PIN);
    delayMs(SYSCLK_32MHZ, 100);
    
    SPI_GPIO_SET(HAL_LCD_SPI_RST_PORT, HAL_LCD_SPI_RST_PIN);
    delayMs(SYSCLK_32MHZ, 100);
}

static void ST7735RInit(void)
{
    /* Reset TFT */
    halTFTReset();
    
    /* Sleep Exit */
    halLcdSpiTxCmd(0x11);
    delayMs(SYSCLK_32MHZ, 120);
    
    /* Frame Rate */
    halLcdSpiTxCmd(0xB1); 
    halLcdSpiTxData(0x01); 
    halLcdSpiTxData(0x2C); 
    halLcdSpiTxData(0x2D); 

    halLcdSpiTxCmd(0xB2); 
    halLcdSpiTxData(0x01);
    halLcdSpiTxData(0x2C);
    halLcdSpiTxData(0x2D);

    halLcdSpiTxCmd(0xB3);
    halLcdSpiTxData(0x01);
    halLcdSpiTxData(0x2C);
    halLcdSpiTxData(0x2D);
    halLcdSpiTxData(0x01);
    halLcdSpiTxData(0x2C);
    halLcdSpiTxData(0x2D);
    
    halLcdSpiTxCmd(0xB4);
    halLcdSpiTxData(0x07); 
    
    /* Power Sequence */
    halLcdSpiTxCmd(0xC0); 
    halLcdSpiTxData(0xA2);
    halLcdSpiTxData(0x02);
    halLcdSpiTxData(0x84);
    halLcdSpiTxData(0xC1);
    halLcdSpiTxData(0xC5);

    halLcdSpiTxCmd(0xC2); 
    halLcdSpiTxData(0x0A); 
    halLcdSpiTxData(0x00); 

    halLcdSpiTxCmd(0xC3); 
    halLcdSpiTxData(0x8A);
    halLcdSpiTxData(0x2A);
    halLcdSpiTxData(0xC4);
    halLcdSpiTxData(0x8A);
    halLcdSpiTxData(0xEE);
    
    halLcdSpiTxCmd(0xC5);
    halLcdSpiTxData(0x0E); 
    
    /* MX, MY, RGB mode */
    halLcdSpiTxCmd(0x36);
    halLcdSpiTxData(0xC8); 
    
    /* Gamma Sequence */
    halLcdSpiTxCmd(0xe0); 
    halLcdSpiTxData(0x0f); 
    halLcdSpiTxData(0x1a); 
    halLcdSpiTxData(0x0f); 
    halLcdSpiTxData(0x18); 
    halLcdSpiTxData(0x2f); 
    halLcdSpiTxData(0x28); 
    halLcdSpiTxData(0x20); 
    halLcdSpiTxData(0x22); 
    halLcdSpiTxData(0x1f); 
    halLcdSpiTxData(0x1b); 
    halLcdSpiTxData(0x23);
    halLcdSpiTxData(0x37); 
    halLcdSpiTxData(0x00); 
    halLcdSpiTxData(0x07); 
    halLcdSpiTxData(0x02); 
    halLcdSpiTxData(0x10);

    halLcdSpiTxCmd(0xe1); 
    halLcdSpiTxData(0x0f); 
    halLcdSpiTxData(0x1b);
    halLcdSpiTxData(0x0f);
    halLcdSpiTxData(0x17);
    halLcdSpiTxData(0x33);
    halLcdSpiTxData(0x2c);
    halLcdSpiTxData(0x29);
    halLcdSpiTxData(0x2e);
    halLcdSpiTxData(0x30);
    halLcdSpiTxData(0x30);
    halLcdSpiTxData(0x39);
    halLcdSpiTxData(0x3f);
    halLcdSpiTxData(0x00);
    halLcdSpiTxData(0x07);
    halLcdSpiTxData(0x03);
    halLcdSpiTxData(0x10);
    
    halLcdSpiTxCmd(0x2a);
    halLcdSpiTxData(0x00);
    halLcdSpiTxData(0x00);
    halLcdSpiTxData(0x00);
    halLcdSpiTxData(0x7f);

    halLcdSpiTxCmd(0x2b);
    halLcdSpiTxData(0x00);
    halLcdSpiTxData(0x00);
    halLcdSpiTxData(0x00);
    halLcdSpiTxData(0x9f);
    
    /* Enable test command */
    halLcdSpiTxCmd(0xF0);  
    halLcdSpiTxData(0x01); 
    
    /* Disable ram power save mode */
    halLcdSpiTxCmd(0xF6);  
    halLcdSpiTxData(0x00); 
    
    /* 65k mode */
    halLcdSpiTxCmd(0x3A);
    halLcdSpiTxData(0x05); 
    
    /* Display on */
    halLcdSpiTxCmd(0x29);
}

static void halTFTSetRegion(uint16 xs, uint16 ys, uint16 xe, uint16 ye)
{
    halLcdSpiTxCmd(0x2a);
    halLcdSpiTxData(0x00); 
    halLcdSpiTxData(xs + 2); 
    halLcdSpiTxData(0x00); 
    halLcdSpiTxData(xe + 2);

    halLcdSpiTxCmd(0x2b);
    halLcdSpiTxData(0x00); 
    halLcdSpiTxData(ys + 3); 
    halLcdSpiTxData(0x00); 
    halLcdSpiTxData(ye + 3);
    
    halLcdSpiTxCmd(0x2c);
}

static void halTFTSetPosition(uint16 x, uint16 y)
{
    halTFTSetRegion(x, y, x, y);
}

static void halTFTDrawPixel(uint16 xs, uint16 ys, uint16 color)
{
    halTFTSetRegion(xs, ys, xs+1, ys+1);
    HAL_TFT_WRITE_UINT16(color);
}

static void halTFTShowChar8x16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch)
{
    uint16 charIndex;
    
    /* index of font table, height: 16 */
    if(ch > 32) charIndex = (ch - 32) * 16;
    else charIndex = 0;
    
    /* Show Line */
    for (uint8 l = 0; l < 16; l++)
        /* Show Column: 8column per line */
      for(uint8 c = 0; c < 8; c++) {
            if(HAL_TFT_FONT_TBL_8x16[charIndex + l] & (0x80 >> c)) halTFTDrawPixel(x + c, y + l, fontColor);
            else halTFTDrawPixel(x + c, y + l, backgroundColor);
      }
}

static void halTFTShowChineseChar16x16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 chL, uint8 chR)
{
    for (uint16 i = 0; i < HAL_TFT_FONT_TBL_CHINESE_SIZE; i++) {
        if (HAL_TFT_FONT_TBL_CHINESE_16x16[i].Char16x16[0] != chL || HAL_TFT_FONT_TBL_CHINESE_16x16[i].Char16x16[1] != chR) continue;
        
        /* Show Line */
        for (uint8 l = 0; l < 16; l++) {
            /* Show Column: 16column per line */
            // First 8column
            for (uint8 c1 = 0; c1 < 8; c1++) {
                if(HAL_TFT_FONT_TBL_CHINESE_16x16[i].code[l*2] & (0x80>>c1))
                    halTFTDrawPixel(x + c1, y + l, fontColor);
                else
                    halTFTDrawPixel(x + c1, y + l, backgroundColor);
            }
            // Last 8column
            for (uint8 c2 = 0; c2 < 8; c2++) {
                if(HAL_TFT_FONT_TBL_CHINESE_16x16[i].code[l*2+1] & (0x80>>c2))
                    halTFTDrawPixel(x + c2 + 8, y + l, fontColor);
                else
                    halTFTDrawPixel(x + c2 + 8, y + l, backgroundColor);
            }
        }

        break;
    }
}
