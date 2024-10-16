//-- TFT only!
#if HAL_LCD_TYPE == HAL_LCD_TYPE_TFT

#ifndef HAL_TFT_H
#define HAL_TFT_H

#include "hal_fonts.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LCD_TFT096              0 /* TFT096 */
#define LCD_IPS096_T1           1 /* IPS096 - Normal type(BOE) */
#define LCD_IPS096_T2           2 /* IPS096 - Special type(HanCai) */

/** @brief   LCD type.
 */
#define LCD_TYPE                LCD_IPS096_T2

/** @brief   TFT pixel color.
 */
#define HAL_TFT_PIXEL_RED       0xF000 //-- 61440:240|0
#define HAL_TFT_PIXEL_GREEN     0x07E0 //--  2016:  7|224
#define HAL_TFT_PIXEL_BLUE      0x001F //--    31:  0|31
#define HAL_TFT_PIXEL_BLACK     0x0000 //--     0:  0|0
#define HAL_TFT_PIXEL_WHITE     0xFFFF //-- 65535:255|255
#define HAL_TFT_PIXEL_YELLOW    0xFFE0 //-- 65504:255|224
#define HAL_TFT_PIXEL_GRAY      0xEF7D //-- 61309:239|125
#define HAL_TFT_PIXEL_CYAN      0x07FF //--  2047:  7|255
#define HAL_TFT_PIXEL_MAGENTA   0xF81F //-- 63519:248|31


/** @brief   TFT parameters.
 */
#if (defined LCD_TYPE) && (LCD_TYPE == LCD_TFT096)
#define HAL_TFT_X               128 //!< x(0 ~ 127).
#define HAL_TFT_Y               128 //!< y(64 ~ 127).
#define HAL_TFT_Y_OFFSET        64  //!< Offset.
#elif (defined LCD_TYPE) && ((LCD_TYPE == LCD_IPS096_T1) || (LCD_TYPE == LCD_IPS096_T2))
#define HAL_TFT_X               160
#define HAL_TFT_Y               80
#define HAL_TFT_Y_OFFSET        0
#endif

/**
 * @fn      halTFTInit
 * 
 * @brief	Init. TFT
 *
 * @param 	screenColor - screen color(HAL_TFT_PIXEL_BLACK ...)
 */
void halTFTInit(uint16 screenColor);

/**
 * @fn      halTFTClearScreen
 * 
 * @brief	Set screen color
 *
 * @param 	pixelVal - pixel value
 */
void halTFTSetScreen(uint16 pixelVal);

/**
 * @fn      halTFTShowX16
 * 
 * @brief   Show x16(Height: 16) String, Supported Font: 1. ASCII - 8x16  2. Chinese 16x16 characters.
 *
 * @param   x - 0 ~ 127
 * @param   y - 0 ~ 63
 * @param   fontColor - font color
 * @param   backgroundColor - background color
 * @param   str - string
 *
 * @warning Chinese 16x16 characters must found in table: FONT_TABLE_CHINESE_16x16
 */
void halTFTShowX16(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, const uint8 *str);
void halTFTShowX8(uint16 x, uint16 y, uint16 fontColor, uint16 backgroundColor, const uint8 *str);

/**
 * @fn      halTFTShowPicture
 * 
 * @brief	Show Picture
 *
 * @param   x - 0 ~ 127
 * @param   y - 0 ~ 64
 * @param   picWidth - max: 128
 * @param   picHeight - max: 64
 * @param   pic - picture
 */
void halTFTShowPicture(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic);
void halTFTShowIcon(uint8 x, uint8 y, uint16 fontColor, uint16 backgroundColor, uint8 size, uint8 idx);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_TFT_H */

#endif //-- HAL_LCD_TYPE == HAL_LCD_TYPE_TFT
