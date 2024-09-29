#include "hal_oled_128x64.h"
#include "font_mike_8x16.h"
#include "font_gyver_5x8.h"
#include "icons_7x7.h"
#include "icons_8x8.h"
#include "hal_lcd_spi.h"
#include "cc2530_io_ports.h"

#include <stdio.h>
#include "hal_uart.h"
#include "colors.h"

static void halOLED128x64Reset(void);
static void halOLED128x64ChipInit(void);
static void halOLED128x64SetPosition(uint8 page, uint8 x);

static void halOLEDShowChar8x16(uint16 x, uint16 page, uint8 ch);
static void halOLEDShowCharRus1(uint16 x, uint16 page, uint8 ch);
static void halOLEDShowCharRus2(uint16 x, uint16 page, uint8 ch);
static void halOLEDShowCharOther(uint16 x, uint16 page, uint8 ch);
static uint8 halOLEDShowCharGyver(uint16 x, uint16 page, uint8 chL, uint8 chR);


void halOLED128x64Init(void)
{    
    //-- Init SPI-GPIO
    halLcdSpiInit();

    //-- Prevent UART output
    HalUARTInit();
    
    //-- Init Chip
    halOLED128x64ChipInit();

    //-- Setting
    halOLED128x64ClearScreen();
    halOLED128x64SetPosition(0,0);
}

void halOLED128x64ClearScreen(void)
{
    uint8 page, x;
    
    for (page = 0; page < HAL_OLED128x64_PAGE; page++) {
        halLcdSpiTxCmd(0xb0 + page);
        halLcdSpiTxCmd(0x01);
        halLcdSpiTxCmd(0x10);
        
        for (x = 0; x < HAL_OLED128x64_X; x++) halLcdSpiTxData(0);
    }
}

void halOLED128x64ShowX16(uint8 line, uint8 column, const uint8 *str)
{
  if (!str || line > 3) return;

  //-- 2 page per line
  uint8 page = line * 2;
  //-- text
  const uint8 *ptext = str;
  
  //-- Show text
  while(*ptext != 0) {
    //-- End of line
    if((column + 8) > HAL_OLED128x64_X) return;
    
    if((*ptext) < 128) {
      //-- ASCII Code: 0~127
     	halOLEDShowChar8x16(column, page, *ptext);
    } else {
      if((*ptext) == 208 && *(ptext+1) == 129) {
      	//-- Other 8x16 characters #3 (Ё)
      	halOLEDShowCharOther(column, page, 0);
      } else if((*ptext) == 209 && *(ptext+1) == 145) {
      	//-- Other 8x16 characters #3 (ё)
      	halOLEDShowCharOther(column, page, 1);
      } else if((*ptext) == 194 && *(ptext+1) == 176) {
      	//-- Other 8x16 characters #3 (° - degrees Celsius)
      	halOLEDShowCharOther(column, page, 2);
      } else if((*ptext) == 208) {
      	//-- Russian 8x16 characters #1
        halOLEDShowCharRus1(column, page, *(ptext+1));
      } else if((*ptext) == 209) {
      	//-- Russian 8x16 characters #2
        halOLEDShowCharRus2(column, page, *(ptext+1));
      } else {
      	//-- skip...
      }
    }

    if(*ptext != 208 && *ptext != 209) {
    	column += 8;
    }
    ptext++;
  } //-- while(*ptext != 0)
}

void halOLED128x64ShowX8(uint8 line, uint8 column, const uint8 *str)
{
  //-- up to 8 rows on screen
  if(!str || line > 7) return;

  //-- state
  uint8 state = 0;

  //-- text
  const uint8 *ptext = str;
  
  //-- Show text
  while(*ptext != 0) {
    //-- End of line
    if((column + 6) > HAL_OLED128x64_X) return;
    
    if((*ptext) < 128) {
    	//-- ASCII Code: 0~127
    	state = halOLEDShowCharGyver(column, line, *ptext, 0);
    } else {
    	state = halOLEDShowCharGyver(column, line, *ptext, *(ptext + 1));
    }

    if(state == 1) {
    	column += 6;
    }
    ptext++;
    
  } //-- while(*ptext != 0)
}

//-- original Chinese function (maybe will be used or not)
void halOLED128x64ShowPictureChina(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic)
{
  if(x > 127 || y > 64) return;
  
  uint16 charIndex = 0;
  uint8 xs = x, xe = x + picWidth - 1;
  uint8 ys =  y / 8, ye = (y + picHeight - 1) / 8;

  for(uint8 line = ys; line < ye; line++) {
    halOLED128x64SetPosition(line, x);
    for(uint8 column = xs; column < (xe + 1); column++) {
    	halLcdSpiTxData(pic[charIndex++]);
    }
  }
}

//-- "x" can have any available value
//-- "y" must be a multiple of 8
void halOLED128x64ShowPicture(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic)
{
  if(x > 127 || y > 64) return;

  uint16 charIndex = 0;
  uint8 xs = x, xe = x + picWidth - 1;
  uint8 ys =  y / 8, ye = (y + picHeight - 1) / 8;

  //-- if it's not a Сhinese image, add another line
  //-- (to work with ImageProcessor, LCDAssistant, my own converters and similar software)
  ye++;
 
  for(uint8 line = ys; line < ye; line++) {
    halOLED128x64SetPosition(line, x);
    for(uint8 column = xs; column < (xe + 1); column++) {
    	halLcdSpiTxData(pic[charIndex++]);
    }
  }
}

void halOLED128x64ShowIcon(uint8 x, uint8 y, uint8 size, uint8 idx)
{
  //-- 7x7 or 8x8 only!
  if(size != 7 && size != 8) {
  	return;
  }

  uint8 lastCol = 127 - size;
  uint8 lastRow = 7;
  
  //printf("%d > %d\n", y, totalRows);
  
  //-- do not draw an icon that extends beyond the screen!
  //-- x: 0~127, y:0~7
  if(x > lastCol || y > lastRow) {
  	return;
  }

  uint16 charIndex = idx * size;
 
  halOLED128x64SetPosition(y, x);
  //-- print byte sequence for icon
  for(uint8 i = 0; i < size; i++) {
  	if(size == 7) {
  		halLcdSpiTxData(ICON_TABLE_7x7[charIndex++]);
  	}
  	if(size == 8) {
  		halLcdSpiTxData(ICON_TABLE_8x8[charIndex++]);
  	}
  }

}


static void halOLED128x64Reset(void)
{
  /* Reset OLED128x64 */
  SPI_GPIO_CLEAR(HAL_LCD_SPI_RST_PORT, HAL_LCD_SPI_RST_PIN);
  delayMs32MHZ(60);
  SPI_GPIO_SET(HAL_LCD_SPI_RST_PORT, HAL_LCD_SPI_RST_PIN);
}

static void halOLED128x64ChipInit(void)
{
    halOLED128x64Reset();
  
    halLcdSpiTxCmd(0xae);  // --turn off oled panel
    halLcdSpiTxCmd(0x00);  // ---set low column address
    halLcdSpiTxCmd(0x10);  // ---set high column address
    halLcdSpiTxCmd(0x40);  // --set start line address  Set Mapping  
                                    //   RAM Display Start Line (0x00~0x3F)
    halLcdSpiTxCmd(0x81);  // --set contrast control register
    halLcdSpiTxCmd(0xcf);  // --Set SEG Output Current Brightness
    halLcdSpiTxCmd(0xa1);  // --Set SEG/Column Mapping     
    halLcdSpiTxCmd(0xc8);  // --Set COM/Row Scan Direction  
    halLcdSpiTxCmd(0xa6);  // --set normal display
    halLcdSpiTxCmd(0xa8);  // --set multiplex ratio(1 to 64)
    halLcdSpiTxCmd(0x3f);  // --1/64 duty
    halLcdSpiTxCmd(0xd3);  // --set display offset Shift Mapping RAM 
                                    //   Counter(0x00~0x3F)
    halLcdSpiTxCmd(0x00);  // --not offset
    halLcdSpiTxCmd(0xd5);  // --set display clock divide
                                    //   ratio/oscillator oscillator frequency
    halLcdSpiTxCmd(0x80);  // --set divide ratio, Set Clock as 100 
                                    //   Frames/Sec
    halLcdSpiTxCmd(0xd9);  // --set pre-charge period
    halLcdSpiTxCmd(0xf1);  // --Set Pre-Charge as 15 Clocks & Discharge 
                                    //   as 1 Clock
    halLcdSpiTxCmd(0xda);  // --set com pins hardware configuration
    halLcdSpiTxCmd(0x12);
    halLcdSpiTxCmd(0xdb);  // --set vcomh
    halLcdSpiTxCmd(0x40);  // --Set VCOM Deselect Level
    halLcdSpiTxCmd(0x20);  // --Set Page Addressing Mode (0x00/0x01/0x02)
    halLcdSpiTxCmd(0x02);  //
    halLcdSpiTxCmd(0x8d);  // --set Charge Pump enable/disable
    halLcdSpiTxCmd(0x14);  // --set(0x10) disable
    halLcdSpiTxCmd(0xa4);  // --Disable Entire Display On (0xa4/0xa5)
    halLcdSpiTxCmd(0xa6);  // --Disable Inverse Display On (0xa6/a7) 
    halLcdSpiTxCmd(0xaf);  // --turn on oled panel
}

static void halOLED128x64SetPosition(uint8 page, uint8 x)
{
    halLcdSpiTxCmd(0xb0 + page);
    halLcdSpiTxCmd(((x&0xf0)>>4)|0x10);
    halLcdSpiTxCmd((x&0x0f)|0x01); 
}

static void halOLEDShowChar8x16(uint16 x, uint16 page, uint8 ch)
{
  uint16 charIndex;
  uint8 j;
  
  //-- index of font table, height: 16
  charIndex = (ch > 32) ? (ch - 32) * 16 : 0;
  
  //-- Set first page
  halOLED128x64SetPosition(page, x);
  //for (uint8 j = 0; j < 8; j++)  halLcdSpiTxData( FONT_TABLE_8x16[charIndex + j] );
  for (j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_8x16[charIndex + j]);
  }
  
  //-- Set second page
  halOLED128x64SetPosition(page + 1, x);
  //for (uint8 j = 0; j < 8; j++) halLcdSpiTxData( FONT_TABLE_8x16[charIndex + j + 8] );
  for (j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_8x16[charIndex + j + 8]);
  }
}

static void halOLEDShowCharRus1(uint16 x, uint16 page, uint8 ch)
{
  uint16 charIndex;
  uint8 j;
  
  //-- index of font table, height: 16
  charIndex = (ch > 144) ? (ch - 144) * 16 : 0;
  
  //-- Set first page
  halOLED128x64SetPosition(page, x);
  for (j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_RUS1[charIndex + j]);
  }
  
  //-- Set second page
  halOLED128x64SetPosition(page + 1, x);
  for (j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_RUS1[charIndex + j + 8]);
  }
}

static void halOLEDShowCharRus2(uint16 x, uint16 page, uint8 ch)
{
  uint16 charIndex;
  uint8 j;
  
  //-- index of font table, height: 16
  charIndex = (ch > 128) ? (ch - 128) * 16 : 0;
  
  //-- Set first page
  halOLED128x64SetPosition(page, x);
  for(j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_RUS2[charIndex + j]);
  }
  
  //-- Set second page
  halOLED128x64SetPosition(page + 1, x);
  for(j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_RUS2[charIndex + j + 8]);
  }
}

static void halOLEDShowCharOther(uint16 x, uint16 page, uint8 ch)
{
  uint16 charIndex;
  uint8 j;
  
  //-- index of font table, height: 16
  charIndex = ch * 16;
  
  //-- Set first page
  halOLED128x64SetPosition(page, x);
  for(j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_OTHER[charIndex + j]);
  }
  
  //-- Set second page
  halOLED128x64SetPosition(page + 1, x);
  for(j = 0; j < 8; j++) {
  	halLcdSpiTxData(FONT_TABLE_MIKE_OTHER[charIndex + j + 8]);
  }
}

static uint8 halOLEDShowCharGyver(uint16 x, uint16 page, uint8 chL, uint8 chR)
{
  uint16 charIndex, tableFontRow;
  uint8 j, state = 1;
  
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

  	halOLED128x64SetPosition(page, x);
  	for(j = 0; j < 5; j++) {
  		halLcdSpiTxData(FONT_TABLE_GYVER[charIndex + j]);
  	}
  }

  return state;
}
