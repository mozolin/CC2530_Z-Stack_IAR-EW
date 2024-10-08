//-- TFT only!
#if HAL_LCD_TYPE == HAL_LCD_TYPE_TFT

#include "hal_tft.h"
#include "hal_lcd_spi.h"
#include "cc2530_io_ports.h"

#include <stdio.h>
#include "hal_uart.h"
#include "colors.h"


/* Write a uint16 data to TFT. */
#define HAL_TFT_WRITE_UINT16(val) do {    \
    halLcdSpiTxData((uint8)((val) >> 8)); \
    halLcdSpiTxData((uint8)(val));        \
} while(0)
                  
static void halTFTReset(void);   

/* TFT096 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_TFT096)
static void ST7735RInit(void);

static void halTFTSetRegion(uint16 xs, uint16 ys, uint16 xe, uint16 ye);
#define HAL_TFT_SET_REGION(xs, ys, xe, ye)  halTFTSetRegion(xs, ys, xe, ye)
#endif /* TFT096 */

/* IPS096 */
#if (defined LCD_TYPE) && ((LCD_TYPE == LCD_IPS096_T1) || /* IPS096 - Normal type(BOE) */ \
                           (LCD_TYPE == LCD_IPS096_T2))   /* IPS096 - Special type(HanCai) */

#define ST7735S_FLAG_MY     (1 << 7)
#define ST7735S_FLAG_MX     (1 << 6)
#define ST7735S_FLAG_MV     (1 << 5)
#define ST7735S_FLAG_ML     (1 << 4)
#define ST7735S_FLAG_RGB    (1 << 3)
#define ST7735S_FLAG_MH     (1 << 2)

#define ST7735S_MADCTL_RGB           (ST7735S_FLAG_RGB)
#define ST7735S_MADCTL_MY_MV_RGB     (ST7735S_FLAG_MY | ST7735S_FLAG_MV | ST7735S_FLAG_RGB)
#define ST7735S_MADCTL_MY_MX_RGB     (ST7735S_FLAG_MY | ST7735S_FLAG_MX | ST7735S_FLAG_RGB)
#define ST7735S_MADCTL_MX_MV_ML_RGB  (ST7735S_FLAG_MX | ST7735S_FLAG_MV | ST7735S_FLAG_ML | ST7735S_FLAG_RGB)

#define ST7735S_MADCTL  ST7735S_MADCTL_MX_MV_ML_RGB

static void ST7735SInit(uint8 madctl);

static void halTFTSetRegion(uint8 madctl, uint16 xs, uint16 ys, uint16 xe, uint16 ye);
#define HAL_TFT_SET_REGION(xs, ys, xe, ye)  halTFTSetRegion(ST7735S_MADCTL, xs, ys, xe, ye)
#endif /* IPS096 */

static void halTFTSetPosition(uint16 x, uint16 y);
static void halTFTDrawPixel(uint16 xs, uint16 ys, uint16 color);

static void halTFTShowChar8x16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch);
static void halTFTShowCharOther(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch);
static void halTFTShowCharRus1(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch);
static void halTFTShowCharRus2(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch);
static uint8 halTFTShowCharGyver(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 chL, uint8 chR);
void halTFTParseFont(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 fontTableNum, uint16 charIndex);

void halTFTInit(uint16 screenColor)
{    
  /* Init SPI-GPIO */
  halLcdSpiInit();
  #if (defined LCD_TYPE) && (LCD_TYPE == LCD_TFT096)
    ST7735RInit();
  #elif (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T1)
    ST7735SInit(ST7735S_MADCTL);
  #elif (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T2)
    ST7735SInit(ST7735S_MADCTL);
  #else
    #error "Unknow LCD_TYPE."
  #endif

  /* Setting */
  halTFTSetPosition(0, 0);
  halTFTSetScreen(screenColor);
}

void halTFTSetScreen(uint16 pixelVal)
{
    uint16 x, y;
    
    HAL_TFT_SET_REGION(0, HAL_TFT_Y_OFFSET, HAL_TFT_X-1, HAL_TFT_Y-1);
    
    for (x = 0; x < HAL_TFT_X; x++)
        for (y = HAL_TFT_Y_OFFSET; y < HAL_TFT_Y; y++) HAL_TFT_WRITE_UINT16(pixelVal);
}

void halTFTShowX16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, const uint8 *str)
{
  if(!str) return;
  
  y += HAL_TFT_Y_OFFSET;
  const uint8 *ptext = str; // text

  /* Show text */
  while(*ptext != 0) {
    //-- ASCII Code: 0~127
    if((*ptext) < 128) {
      //-- End of line
      if((x + 8) > HAL_TFT_X) return;
      
      //-- Show 8x16 ASCII Char
      halTFTShowChar8x16(x, y, fontColor, backgroundColor, *ptext);
    } else {
      if((*ptext) == 208 && *(ptext+1) == 129) {
      	//-- Other 8x16 characters #3 (Ё)
      	halTFTShowCharOther(x, y, fontColor, backgroundColor, 0);
      } else if((*ptext) == 209 && *(ptext+1) == 145) {
      	//-- Other 8x16 characters #3 (ё)
      	halTFTShowCharOther(x, y, fontColor, backgroundColor, 1);
      } else if((*ptext) == 194 && *(ptext+1) == 176) {
      	//-- Other 8x16 characters #3 (° - degrees Celsius)
      	halTFTShowCharOther(x, y, fontColor, backgroundColor, 2);
      } else if((*ptext) == 208) {
      	//-- Russian 8x16 characters #1
        halTFTShowCharRus1(x, y, fontColor, backgroundColor, *(ptext+1));
      } else if((*ptext) == 209) {
      	//-- Russian 8x16 characters #2
        halTFTShowCharRus2(x, y, fontColor, backgroundColor, *(ptext+1));
      } else {
      	//-- skip...
      }  
    }

    if(*ptext != 208 && *ptext != 209) {
    	x += 8;
    }
    ptext++;

  } //-- while(*ptext != 0)
}

void halTFTShowX8(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, const uint8 *str)
{
  if(!str) return;
  
  y += HAL_TFT_Y_OFFSET;
  const uint8 *ptext = str; // text

    //-- state
  uint8 state = 0;

  /* Show text */
  while(*ptext != 0) {
    //-- End of line
    if((x + 6) > HAL_TFT_X) return;
    
    if((*ptext) < 128) {
      //-- ASCII Code: 0~127
      state = halTFTShowCharGyver(x, y, fontColor, backgroundColor, *ptext, 0);
    } else {
      state = halTFTShowCharGyver(x, y, fontColor, backgroundColor, *ptext, *(ptext + 1));
    }

    if(state == 1) {
    	x += 6;
    }
    ptext++;

  } //-- while(*ptext != 0)
}

void halTFTShowPicture(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic)
{
  uint8 picL, picH;
  const uint8 *pPic = pic;
  uint16 picSize = (uint16)picWidth * picHeight;

  y += HAL_TFT_Y_OFFSET;
  
  if(!pPic || (x + picWidth)  > HAL_TFT_X ||  (y + picHeight) > HAL_TFT_Y) return;

  //-- Set region
  HAL_TFT_SET_REGION(x, y, x + picWidth - 1, y + picHeight - 1);

  //-- Show Picture
  for(uint16 i = 0; i < picSize; i++) {
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

void halTFTShowIcon(uint8 x, uint8 y, uint16 fontColor, uint16 backgroundColor, uint8 size, uint8 idx)
{
  //-- 7x7 or 8x8 only!
  if(size != 7 && size != 8) {
  	return;
  }

  uint8 lastCol = 159 - size;
  uint8 lastRow = 79 - size;

  //-- do not draw an icon that extends beyond the screen!
  //-- x: 0~127, y:0~7
  if(x > lastCol || y > lastRow) {
  	return;
  }

  uint16 charIndex = idx * size, ch, xxx, yyy;

  for(int l = 0; l < size; l++) {
  	for(int c = 0; c < 8; c++) {
  		xxx = x + l;
  		//-- rotate font
  		yyy = y + 7 - c;
  		if(size == 7) {
  		  ch = ICON_TABLE_7x7[charIndex + l];
  		}
  		if(size == 8) {
  		  ch = ICON_TABLE_8x8[charIndex + l];
  		}
			if(ch & (0x80 >> c)) {
	    	halTFTDrawPixel(xxx, yyy, fontColor);
		  } else {
  			halTFTDrawPixel(xxx, yyy, backgroundColor);
	  	}
  	}
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

/* TFT096 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_TFT096)
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
#endif /* #if (defined LCD_TYPE) && (LCD_TYPE == LCD_TFT096) */

/* IPS096-T1 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T1)
static void ST7735SInit(uint8 madctl)
{
    /* Reset TFT */
    halTFTReset();

    halLcdSpiTxCmd(0x11);
	delayMs(SYSCLK_32MHZ, 120);
	
    halLcdSpiTxCmd(0xB1);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);

	halLcdSpiTxCmd(0xB2);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);

	halLcdSpiTxCmd(0xB3);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);

	halLcdSpiTxCmd(0xB4);
	halLcdSpiTxData(0x03);

	halLcdSpiTxCmd(0xC0);
	halLcdSpiTxData(0x0E);
	halLcdSpiTxData(0x0E);
	halLcdSpiTxData(0x04);

	halLcdSpiTxCmd(0xC1);
	halLcdSpiTxData(0xC5);

	halLcdSpiTxCmd(0xC2);
	halLcdSpiTxData(0x0D);
	halLcdSpiTxData(0x00);

	halLcdSpiTxCmd(0xC3);
	halLcdSpiTxData(0x8D);
	halLcdSpiTxData(0x2A);

	halLcdSpiTxCmd(0xC4);
	halLcdSpiTxData(0x8D);
	halLcdSpiTxData(0xEE);

	halLcdSpiTxCmd(0xC5);
	halLcdSpiTxData(0x06);

	halLcdSpiTxCmd(0x36);
	halLcdSpiTxData(madctl);

	halLcdSpiTxCmd(0x3A);
	halLcdSpiTxData(0x55);
		
	halLcdSpiTxCmd(0xE0);
	halLcdSpiTxData(0x0B);
	halLcdSpiTxData(0x17);
	halLcdSpiTxData(0x0A);
	halLcdSpiTxData(0x0D);
	halLcdSpiTxData(0x1A);
	halLcdSpiTxData(0x19);
	halLcdSpiTxData(0x16);
	halLcdSpiTxData(0x1D);
	halLcdSpiTxData(0x21);
	halLcdSpiTxData(0x26);
	halLcdSpiTxData(0x37);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0x09);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x10);

	halLcdSpiTxCmd(0xE1);
	halLcdSpiTxData(0x0C);
	halLcdSpiTxData(0x19);
	halLcdSpiTxData(0x09);
	halLcdSpiTxData(0x0D);
	halLcdSpiTxData(0x1B);
	halLcdSpiTxData(0x19);
	halLcdSpiTxData(0x15);
	halLcdSpiTxData(0x1D);
	halLcdSpiTxData(0x21);
	halLcdSpiTxData(0x26);
	halLcdSpiTxData(0x39);
	halLcdSpiTxData(0x3E);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0x09);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x10);
	halLcdSpiTxCmd(0x29);
}
#endif /* #if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T1) */

/* IPS096-T2 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T2)
static void ST7735SInit(uint8 madctl)
{
    /* Reset TFT */
    halTFTReset();

    halLcdSpiTxCmd(0x11);
	delayMs(SYSCLK_32MHZ, 120);

    halLcdSpiTxCmd(0xB1);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxCmd(0xB2);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxCmd(0xB3);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x05);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxData(0x3C);
	halLcdSpiTxCmd(0xB4);
	halLcdSpiTxData(0x03);
	halLcdSpiTxCmd(0xC0);
	halLcdSpiTxData(0xAB);
	halLcdSpiTxData(0x0B);
	halLcdSpiTxData(0x04);
	halLcdSpiTxCmd(0xC1);
	halLcdSpiTxData(0xC5);
	halLcdSpiTxCmd(0xC2);
	halLcdSpiTxData(0x0D);
	halLcdSpiTxData(0x00);
	halLcdSpiTxCmd(0xC3);
	halLcdSpiTxData(0x8D);
	halLcdSpiTxData(0x6A);
	halLcdSpiTxCmd(0xC4);
	halLcdSpiTxData(0x8D);
	halLcdSpiTxData(0xEE);
	halLcdSpiTxCmd(0xC5);
	halLcdSpiTxData(0x0F);
	halLcdSpiTxCmd(0xE0);
	halLcdSpiTxData(0x07);
	halLcdSpiTxData(0x0E);
	halLcdSpiTxData(0x08);
	halLcdSpiTxData(0x07);
	halLcdSpiTxData(0x10);
	halLcdSpiTxData(0x07);
	halLcdSpiTxData(0x02);
	halLcdSpiTxData(0x07);
	halLcdSpiTxData(0x09);
	halLcdSpiTxData(0x0F);
	halLcdSpiTxData(0x25);
	halLcdSpiTxData(0x36);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0x08);
	halLcdSpiTxData(0x04);
	halLcdSpiTxData(0x10);
	halLcdSpiTxCmd(0xE1);
	halLcdSpiTxData(0x0A);
	halLcdSpiTxData(0x0D);
	halLcdSpiTxData(0x08);
	halLcdSpiTxData(0x07);
	halLcdSpiTxData(0x0F);
	halLcdSpiTxData(0x07);
	halLcdSpiTxData(0x02);
	halLcdSpiTxData(0x07);
	halLcdSpiTxData(0x09);
	halLcdSpiTxData(0x0F);
	halLcdSpiTxData(0x25);
	halLcdSpiTxData(0x35);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0x09);
	halLcdSpiTxData(0x04);
	halLcdSpiTxData(0x10);

	halLcdSpiTxCmd(0xFC);
	halLcdSpiTxData(0x80);

	halLcdSpiTxCmd(0x3A);
	halLcdSpiTxData(0x05);
	halLcdSpiTxCmd(0x36);
	halLcdSpiTxData(madctl);
	halLcdSpiTxCmd(0x21);
	halLcdSpiTxCmd(0x29);
	halLcdSpiTxCmd(0x2A);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0x1A);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0x69);
	halLcdSpiTxCmd(0x2B);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0x01);
	halLcdSpiTxData(0x00);
	halLcdSpiTxData(0xA0);
	halLcdSpiTxCmd(0x2C);
}
#endif /* #if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T2) */

/* TFT096 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_TFT096)
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
#endif /* #if (defined LCD_TYPE) && (LCD_TYPE == LCD_TFT096) */

/* IPS096-T1 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T1)
static void halTFTSetRegion(uint8 madctl, uint16 xs, uint16 ys, uint16 xe, uint16 ye)
{
    if (madctl == ST7735S_MADCTL_RGB) {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs+24);
		HAL_TFT_WRITE_UINT16(xe+24);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys);
		HAL_TFT_WRITE_UINT16(ye);
		halLcdSpiTxCmd(0x2c);
	}
	else if (madctl == ST7735S_MADCTL_MY_MX_RGB) {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs+24);
		HAL_TFT_WRITE_UINT16(xe+24);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys);
		HAL_TFT_WRITE_UINT16(ye);
		halLcdSpiTxCmd(0x2c);
	}
	else if(madctl == ST7735S_MADCTL_MX_MV_ML_RGB) {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs);
		HAL_TFT_WRITE_UINT16(xe);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys+24);
		HAL_TFT_WRITE_UINT16(ye+24);
		halLcdSpiTxCmd(0x2c);
	}
    else {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs);
		HAL_TFT_WRITE_UINT16(xe);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys+24);
		HAL_TFT_WRITE_UINT16(ye+24);
		halLcdSpiTxCmd(0x2c);
	}
}
#endif /* #if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T1) */

/* IPS096-T2 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T2)
static void halTFTSetRegion(uint8 madctl, uint16 xs, uint16 ys, uint16 xe, uint16 ye)
{
    if (madctl == ST7735S_MADCTL_RGB) {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs+26);
		HAL_TFT_WRITE_UINT16(xe+26);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys+1);
		HAL_TFT_WRITE_UINT16(ye+1);
		halLcdSpiTxCmd(0x2c);
	}
	else if (madctl == ST7735S_MADCTL_MY_MX_RGB) {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs+26);
		HAL_TFT_WRITE_UINT16(xe+26);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys+1);
		HAL_TFT_WRITE_UINT16(ye+1);
		halLcdSpiTxCmd(0x2c);
	}
	else if(madctl == ST7735S_MADCTL_MX_MV_ML_RGB) {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs+1);
		HAL_TFT_WRITE_UINT16(xe+1);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys+26);
		HAL_TFT_WRITE_UINT16(ye+26);
		halLcdSpiTxCmd(0x2c);
	}
    else {
		halLcdSpiTxCmd(0x2a);
		HAL_TFT_WRITE_UINT16(xs+1);
		HAL_TFT_WRITE_UINT16(xe+1);
		halLcdSpiTxCmd(0x2b);
		HAL_TFT_WRITE_UINT16(ys+26);
		HAL_TFT_WRITE_UINT16(ye+26);
		halLcdSpiTxCmd(0x2c);
	}
}
#endif /* #if (defined LCD_TYPE) && (LCD_TYPE == LCD_IPS096_T2) */

static void halTFTSetPosition(uint16 x, uint16 y)
{
    HAL_TFT_SET_REGION(x, y, x, y);
}

static void halTFTDrawPixel(uint16 xs, uint16 ys, uint16 color)
{
    HAL_TFT_SET_REGION(xs, ys, xs+1, ys+1);
    HAL_TFT_WRITE_UINT16(color);
}

void halTFTParseFont(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 fontTableNum, uint16 charIndex)
{
	uint16 currChar = 0, xxx, yyy;

	//-- the first bytes
  for(int l = 0; l < 8; l++) {
  	for(int c = 0; c < 8; c++) {
  		xxx = x + l;
  		//-- rotate font
  		yyy = y + 7 - c;
  		//-- if not 0 => font Color, otherwise => backgroundColor
  		switch(fontTableNum) {
  			case FT_8X16:
  				currChar = FONT_TABLE_8X16[charIndex + l];
  				break;
  			case FT_MIKE_8X16:
  				currChar = FONT_TABLE_MIKE_8X16[charIndex + l];
  				break;
  			case FT_MIKE_OTHER:
  				currChar = FONT_TABLE_MIKE_OTHER[charIndex + l];
  				break;
				case FT_MIKE_RUS1:
  				currChar = FONT_TABLE_MIKE_RUS1[charIndex + l];
  				break;
				case FT_MIKE_RUS2:
  				currChar = FONT_TABLE_MIKE_RUS2[charIndex + l];
  				break;
  		}
  		if(currChar & (0x80 >> c)) {
    		halTFTDrawPixel(xxx, yyy, fontColor);
	    } else {
  	  	halTFTDrawPixel(xxx, yyy, backgroundColor);
    	}
  	}
  }
  //-- the second bytes
  for(int l = 8; l < 16; l++) {
  	for(int c = 0; c < 8; c++) {
  		//-- start from the first position of previous bytes
  		xxx = x + l - 8;
  		//-- rotate font
  		yyy = y + 15 - c;
  		//-- if not 0 => font Color, otherwise => backgroundColor
  		switch(fontTableNum) {
  			case FT_8X16:
  				currChar = FONT_TABLE_8X16[charIndex + l];
  				break;
  			case FT_MIKE_8X16:
  				currChar = FONT_TABLE_MIKE_8X16[charIndex + l];
  				break;
  			case FT_MIKE_OTHER:
  				currChar = FONT_TABLE_MIKE_OTHER[charIndex + l];
  				break;
				case FT_MIKE_RUS1:
  				currChar = FONT_TABLE_MIKE_RUS1[charIndex + l];
  				break;
				case FT_MIKE_RUS2:
  				currChar = FONT_TABLE_MIKE_RUS2[charIndex + l];
  				break;
  		}
  		if(currChar & (0x80 >> c)) {
    		halTFTDrawPixel(xxx, yyy, fontColor);
	    } else {
  	  	halTFTDrawPixel(xxx, yyy, backgroundColor);
    	}
  	}
  }
}

static void halTFTShowChar8x16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch)
{
  uint16 charIndex;
  
  /* index of font table, height: 16 */
  if(ch > 32) charIndex = (ch - 32) * 16;
  else charIndex = 0;

  halTFTParseFont(x, y, fontColor, backgroundColor, FT_MIKE_8X16, charIndex);
}

static void halTFTShowCharOther(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch)
{
  uint16 charIndex/*, xxx, yyy*/;
  
  //-- index of font table, height: 16
  charIndex = ch * 16;

  halTFTParseFont(x, y, fontColor, backgroundColor, FT_MIKE_OTHER, charIndex);
}

static void halTFTShowCharRus1(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch)
{
  uint16 charIndex;
  
  //-- index of font table, height: 16
  charIndex = (ch > 144) ? (ch - 144) * 16 : 0;

  halTFTParseFont(x, y, fontColor, backgroundColor, FT_MIKE_RUS1, charIndex);
}

static void halTFTShowCharRus2(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 ch)
{
  uint16 charIndex;
  
  //-- index of font table, height: 16
  charIndex = (ch > 128) ? (ch - 128) * 16 : 0;

  halTFTParseFont(x, y, fontColor, backgroundColor, FT_MIKE_RUS2, charIndex);

}

static uint8 halTFTShowCharGyver(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, uint8 chL, uint8 chR)
{
	uint16 charIndex, tableFontRow, xxx, yyy;
  uint8 state = 1;
  
  tableFontRow = 0;

  if(chL < 127) {
  	//-- ASCII Code: 0~127
  	tableFontRow = chL - 32;
  } else {
    if(chL == 209 && chR == 145) {
    	//-- Other 5x8 characters #3 (ё) => row=159
    	tableFontRow = 159;
    } else if(chL == 208 && chR == 129) {
    	//-- Other 5x8 characters #3 (Ё) => row=160
    	tableFontRow = 160;
    } else if(chL == 194 && chR == 176) {
    	//-- Other 5x8 characters #3 (° - degrees Celsius) => row=161
    	tableFontRow = 161;
    } else if(chL == 208) {
    	//-- Russian 5x8 characters from "А" to "п"
      tableFontRow = chR - 49;
    } else if(chL == 209) {
    	//-- Russian 5x8 characters from "р" to "я"
      tableFontRow = chR + 15;
    } else {
    	//-- skip...
    	state = 0;
    }
  }

  if(state == 1) {
  	charIndex = (chL > 32) ? tableFontRow * 5 : 0;

    for(int l = 0; l < 5; l++) {
    	for(int c = 0; c < 8; c++) {
    		xxx = x + l;
    		//-- rotate font
    		yyy = y + 7 - c;
	  		if(FONT_TABLE_GYVER[charIndex + l] & (0x80 >> c)) {
		    	halTFTDrawPixel(xxx, yyy, fontColor);
			  } else {
  				halTFTDrawPixel(xxx, yyy, backgroundColor);
	    	}
    	}
    }
  }

  return state;
}


#endif //-- HAL_LCD_TYPE == HAL_LCD_TYPE_TFT
