#include "hal_lcd_common.h"

#if HAL_LCD_TYPE == HAL_LCD_TYPE_TFT
  uint16 px_colors[] = {
    PX_RED,
    PX_GREEN,
    PX_BLUE,
    PX_BLACK,
    PX_WHITE,
    PX_YELLOW,
    PX_GRAY,
    PX_CYAN,
    PX_MAGENTA,
  };
#endif

void halLCDInit(void)
{
  #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
    halOLEDInit();
  #else
    halTFTInit(HAL_TFT_PIXEL_BLACK);
  #endif
}
#if DEBUG_LCD_TEST
  void halLCDStartTest()
  {
  	#if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
  	  halOLEDDraw();
  	#else
  	  halTFTDraw();
  	#endif
  }
#endif
void halLCDClearScreen()
{
	#if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
    halOLEDClearScreen();
  #else
    halTFTSetScreen(PX_BLACK);
  #endif
}

#if DEBUG_LCD_TEST
  #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
    
    void halOLEDDraw(void)
    {
      //-- !! halOLEDShowX8:  21 chars in row (+2 px) !!
      //-- !! halOLEDShowX16: 16 chars in row (exact) !!
      
      //-- ASCII table #1 (8x16)
      halOLEDClearScreen();
      halOLEDShowX16(0, 0, "ABCDEFGHIJKLMNOP");
      halOLEDShowX16(1, 0, "QRSTUVWXYZabcdef");
      halOLEDShowX16(2, 0, "ghijklmnopqrstuv");
      halOLEDShowX16(3, 0, "wxyz0123456789.,");
    
      //-- ASCII table #2 (8x16)
      delayMs32MHZ(4000);
      halOLEDClearScreen();
      halOLEDShowX16(0, 0, "\"'?!@_*#$%&()+-/");
      halOLEDShowX16(1, 0, ":;<=>[\\]^`{|}~");
      //halOLEDShowX16(2, 0, "................");
      //halOLEDShowX16(3, 0, "................");
    
      //-- Russian table #1 (8x16)
      delayMs32MHZ(4000);
      halOLEDClearScreen();
      halOLEDShowX16(0, 0, "АБВГДЕЖЗИЙКЛМНОП");
      halOLEDShowX16(1, 0, "РСТУФХЦЧШЩЪЫЬЭЮЯ");
      halOLEDShowX16(2, 0, "абвгдежзийклмноп");
      halOLEDShowX16(3, 0, "рстуфхцчшщъыьэюя");
    
      //-- Russian table #2 (8x16)
      delayMs32MHZ(4000);
      halOLEDClearScreen();
      halOLEDShowX16(0, 0, "Ёё °C");
      //halOLEDShowX16(1, 0, "................");
      //halOLEDShowX16(2, 0, "................");
      //halOLEDShowX16(3, 0, "................");
    
      //-- ASCII table #1 (8x8)
      delayMs32MHZ(4000);
      halOLEDClearScreen();
      halOLEDShowX8(0, 0, "ABCDEFGHIJKLMNOPQRSTU");
      halOLEDShowX8(1, 0, "VWXYZabcdefghijklmnop");
      halOLEDShowX8(2, 0, "qrstuvwxyz0123456789.");
      halOLEDShowX8(3, 0, ",\"'?!@_*#$%&()+-/:;<=");
      halOLEDShowX8(4, 0, ">[\\]^`{|}~");
      //halOLEDShowX8(5, 0, ".....................");
      //halOLEDShowX8(6, 0, ".....................");
      //halOLEDShowX8(7, 0, ".....................");
    
      //-- Russian table #1 (8x8)
      delayMs32MHZ(4000);
      halOLEDClearScreen();
      halOLEDShowX8(0, 2, "ЁАБВГДЕЖЗИЙКЛМНОПРСТУ");
      halOLEDShowX8(1, 2, "ФХЦЧШЩЪЫЬЭЮЯабвгдежзи");
      halOLEDShowX8(2, 2, "йклмнопрстуфхцчшщъыьэ");
      halOLEDShowX8(3, 2, "юяё °C");
      //halOLEDShowX8(4, 2, ".....................");
      //halOLEDShowX8(5, 2, ".....................");
      //halOLEDShowX8(6, 2, ".....................");
      //halOLEDShowX8(7, 2, ".....................");
    
      //-- Icons table
      delayMs32MHZ(4000);
      halOLEDClearScreen();
      halOLEDShowIcon(10, 0, 7, 0);
      halOLEDShowIcon(20, 1, 8, 0);
      halOLEDShowIcon(30, 2, 7, 1);
      halOLEDShowIcon(40, 3, 8, 1);
      halOLEDShowIcon(50, 4, 7, 2);
      halOLEDShowIcon(60, 5, 8, 2);
      halOLEDShowIcon(70, 6, 7, 3);
      halOLEDShowIcon(80, 7, 8, 3);
      halOLEDShowIcon(90, 8, 7, 4);
    
      //-- Pictures
      delayMs32MHZ(4000);
      halOLEDClearScreen();
      halOLEDShowPicture(0, 0, 128, 64, zigbee_logo2);
    
    
      delayMs32MHZ(4000);
      
      halOLEDClearScreen();
    
      halOLEDShowX16(0, 112, "А");
      halOLEDShowX8(3, 112, "А");
    
      
      halOLEDShowIcon(20, 0, 8, 0);
      halOLEDShowIcon(20, 2, 7, 0);
    
      halOLEDShowPicture(0, 8, 16, 16, danger_16x16);
      halOLEDShowPicture(0, 24, 16, 16, empty_16x16);
      halOLEDShowPicture(0, 48, 16, 16, motion_16x16);
      
      halOLEDShowPicture(36, 0, 32, 32, apple_32x32);
      halOLEDShowPicture(36, 32, 32, 32, toxic_32x32);
    
      halOLEDShowPicture(76, 0, 16, 16, zigbee_connected);
      halOLEDShowPicture(76, 24, 16, 16, zigbee_disconnected);
      halOLEDShowPicture(76, 48, 16, 16, zigbee_image);
    
      halOLEDShowIcon(96, 1, 8, 0);
      halOLEDShowIcon(96, 3, 7, 0);
      
      
      halOLEDShowX8(4, 112, "Ё");
      halOLEDShowX16(3, 112, "Ё");
    }
  
  #else
    
    void halTFTDraw(void)
    {
      //-- !! halTFTShowX8:  26x10 chars in row !!
      //-- !! halTFTShowX16: 20x5 chars in row !!
      
      //-- ASCII table (8x16)
      halTFTSetScreen(PX_BLACK);
      halTFTShowX16(0,  0, PX_RED,    PX_BLACK, "ABCDEFGHIJKLMNOPQRST");
      halTFTShowX16(0, 16, PX_GREEN,  PX_BLACK, "UVWXYZabcdefghijklmn");
      halTFTShowX16(0, 32, PX_BLUE,   PX_BLACK, "opqrstuvwxyz01234567");
      halTFTShowX16(0, 48, PX_WHITE,  PX_BLACK, "89.,\"'?!@_*#$%&()+-/");
      halTFTShowX16(0, 64, PX_YELLOW, PX_BLACK, ":;<=>[\\]^`{|}~");
    
      //-- Russian table (8x16)
      delayMs32MHZ(4000);
      halTFTSetScreen(PX_BLACK);
      halTFTShowX16(0,   0, PX_RED,    PX_BLACK, "АБВГДЕЖЗИЙКЛМНОПРСТУ");
      halTFTShowX16(0,  16, PX_GREEN,  PX_BLACK, "ФХЦЧШЩЪЫЬЭЮЯабвгдежз");
      halTFTShowX16(0,  32, PX_BLUE,   PX_BLACK, "ийклмнопрстуфхцчшщъы");
      halTFTShowX16(0,  48, PX_WHITE,  PX_BLACK, "ьэюяЁё °C");
      
      //-- ASCII table (8x8)
      delayMs32MHZ(4000);
      halTFTSetScreen(PX_BLACK);
      halTFTShowX8(0,  0, PX_RED,    PX_BLACK, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      halTFTShowX8(0, 16, PX_GREEN,  PX_BLACK, "abcdefghijklmnopqrstuvwxyz");
      halTFTShowX8(0, 32, PX_BLUE,   PX_BLACK, "0123456789.,\"'?!@_*#$%&()+");
      halTFTShowX8(0, 48, PX_WHITE,  PX_BLACK, "-/:;<=>[\\]^`{|}~");
      
      //-- Russian table (8x8)
      delayMs32MHZ(4000);
      halTFTSetScreen(PX_BLACK);
      halTFTShowX8(0,  0, PX_RED,    PX_BLACK, "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩ");
      halTFTShowX8(0, 16, PX_GREEN,  PX_BLACK, "ЪЫЬЭЮЯабвгдежзийклмнопрст");
      halTFTShowX8(0, 32, PX_BLUE,   PX_BLACK, "уфхцчшщъыьэюяЁё °C");
      
      //-- Icons table
      delayMs32MHZ(4000);
      halTFTSetScreen(PX_BLACK);
      halTFTShowIcon(0,  0, PX_RED,     PX_BLACK, 7, 0);
      halTFTShowIcon(0,  8, PX_GREEN,   PX_BLACK, 8, 0);
      halTFTShowIcon(0, 16, PX_BLUE,    PX_BLACK, 7, 1);
      halTFTShowIcon(0, 24, PX_WHITE,   PX_BLACK, 8, 1);
      halTFTShowIcon(0, 32, PX_YELLOW,  PX_BLACK, 7, 2);
      halTFTShowIcon(0, 40, PX_GRAY,    PX_BLACK, 8, 2);
      halTFTShowIcon(0, 48, PX_CYAN,    PX_BLACK, 7, 3);
      halTFTShowIcon(0, 56, PX_MAGENTA, PX_BLACK, 8, 3);
      halTFTShowIcon(0, 64, PX_BLACK,   PX_WHITE, 7, 4);
  
      //-- Pictures
      delayMs32MHZ(4000);
      halTFTSetScreen(PX_BLACK);
      halTFTShowPicture(0, 20, 160, 38, tft_zigbee_logo_160x38);
  
      uint16 big;
      int num = sizeof(px_colors) / sizeof(px_colors[0]);
      char s1[] = "Img2Lcd:16bTrueColor";
      char s2[] = "Colors wrong if not!";
      for(uint8 i=0; i<num; i++) {
        big = px_colors[i];
        if(big == PX_BLACK) {
        	//-- skip Black on Black
        	continue;
        }
        halTFTShowX16(0,  0, big, PX_BLACK, (uint8 const *)s1);
      	halTFTShowX16(0, 64, big, PX_BLACK, (uint8 const *)s2);
      	delayMs32MHZ(2000);
      }
      
      /*
      //-- Composition of symbols & pictures
      delayMs32MHZ(4000);
      halTFTSetScreen(PX_BLACK);
      halTFTShowX16(112, 0, PX_RED, PX_BLACK, "А");
      halTFTShowX8(112, 64, PX_GREEN, PX_BLACK, "А");
      
      halTFTShowIcon(20, 20, PX_BLUE, PX_BLACK, 8, 0);
      halTFTShowIcon(20, 40, PX_YELLOW, PX_BLACK, 7, 0);
    
      //-- 512 bytes
      halTFTShowPicture(0, 8, 16, 16, tft_danger_16x16);
      //-- 512 bytes
      halTFTShowPicture(0, 28, 16, 16, tft_empty_16x16);
      //-- 512 bytes
      halTFTShowPicture(0, 48, 16, 16, tft_motion_16x16);
      
      //-- 512 bytes
      halTFTShowPicture(36, 8, 16, 16, tft_danger_red_16x16);
      //-- 512 bytes
      halTFTShowPicture(36, 32, 16, 16, tft_toxic_16x16);
    
      //-- 512 bytes
      halTFTShowPicture(76, 0, 16, 16, tft_wifi_green_16x16);
      //-- 512 bytes
      halTFTShowPicture(76, 24, 16, 16, tft_wifi_red_16x16);
      //-- 512 bytes
      halTFTShowPicture(76, 48, 16, 16, tft_zigbee_16x16);
    
      halTFTShowIcon(96, 16, PX_RED, PX_BLACK, 8, 0);
      halTFTShowIcon(96, 48, PX_GREEN, PX_BLACK, 7, 0);
      
      halTFTShowX8(112,  24, PX_BLUE, PX_BLACK, "Ё");
      halTFTShowX16(112, 40, PX_YELLOW, PX_BLACK, "Ё");
      
      //-- 3200 bytes
      halTFTShowPicture(32, 0, 40, 40, Picture_40x40_WeiXinIco);
      //-- 3200 bytes
      halTFTShowPicture(32, 40, 40, 40, Picture_40x40_ITunesIco);
      */

      //-- final picture
      //delayMs32MHZ(4000);
      //halTFTSetScreen(PX_BLACK);
      //-- 12800 bytes
      //halTFTShowPicture(0, 0, 80, 80, Picture_80x80_Sea);
    }
  
  #endif //-- HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
#endif //-- DEBUG_LCD_TEST
