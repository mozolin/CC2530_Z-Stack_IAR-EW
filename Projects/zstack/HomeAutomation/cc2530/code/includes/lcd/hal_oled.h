//-- OLED only!
#if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED

#ifndef HAL_OLED_H
#define HAL_OLED_H

#include "hal_fonts.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @brief   OLED parameters.
 */
#define HAL_OLED_PAGE      8    //!< Total pages
#define HAL_OLED_X         128  //!< Max X
#define HAL_OLED_Y         64   //!< Max Y

/**
 * @fn      halOLEDInit
 * 
 * @brief	Init. OLED.
 */
extern void halOLEDInit(void);

/**
 * @fn      halOLEDClearScreen
 * 
 * @brief	Clear screen.
 */
void halOLEDClearScreen(void);
   
/**
 * @fn      halOLEDShow
 * 
 * @brief	Show x16(Height: 16) String, Supported Font: 
 *          1. ASCII - 8x16 2. Russian 8x16 characters
 *
 * @param   line - 0 ~ 3
 * @param   column - 0 ~ 127
 * @param   str - string
 */
void halOLEDShowX16(uint8 line, uint8 column, const uint8 *str);
void halOLEDShowX8(uint8 line, uint8 column, const uint8 *str);

/**
 * @fn      halOLEDShowPicture
 * 
 * @brief	Show Picture
 *
 * @param   x - 0 ~ 127
 * @param   y - 0 ~ 64
 * @param   picWidth - max: 128
 * @param   picHeight - max: 64
 * @param   pic - picture
 */
void halOLEDShowPicture(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic);
void halOLEDShowPictureChina(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic);
void halOLEDShowIcon(uint8 x, uint8 y, uint8 size, uint8 idx);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_OLED_H */

#endif //-- HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
