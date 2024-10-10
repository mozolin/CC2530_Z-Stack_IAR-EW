#ifndef HAL_LCD_COMMON_H
#define HAL_LCD_COMMON_H

#include <stdio.h>  //-- printf, sprintf
#include "utils.h"
#include "cc2530_io_ports.h"


#ifdef __cplusplus
extern "C" {
#endif


#if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
  //-- OLED libs
  #include "hal_oled.h"
  //#include "img_oled_picture.h"
  #include "images_oled.h"
  #ifdef HAL_LCD_TEST
    //-- Draw OLED
    void halOLEDDraw(void);
  #endif
#else
  //-- TFT libs
  #include "hal_tft.h"
  #include "img_tft_picture.h"
  #include "images_tft.h"
  #define PX_RED     HAL_TFT_PIXEL_RED
  #define PX_GREEN   HAL_TFT_PIXEL_GREEN
  #define PX_BLUE    HAL_TFT_PIXEL_BLUE
  #define PX_BLACK   HAL_TFT_PIXEL_BLACK
  #define PX_WHITE   HAL_TFT_PIXEL_WHITE
  #define PX_YELLOW  HAL_TFT_PIXEL_YELLOW
  #define PX_GRAY    HAL_TFT_PIXEL_GRAY
  #define PX_CYAN    HAL_TFT_PIXEL_CYAN
  #define PX_MAGENTA HAL_TFT_PIXEL_MAGENTA
  #ifdef HAL_LCD_TEST
    //-- Draw TFT
    void halTFTDraw(void);
  #endif
#endif


void halLCDInit(void);
void halLCDClearScreen(void);
#ifdef HAL_LCD_TEST
  void halLCDStartTest(void);
#endif

#ifdef __cplusplus
}
#endif

#endif //-- #ifndef HAL_LCD_COMMON_H
