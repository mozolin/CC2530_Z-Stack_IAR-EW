#ifndef HAL_OLED_128X64_H
#define HAL_OLED_128X64_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief   OLED128x64 parameters.
 */
#define HAL_OLED128x64_PAGE      8    //!< Total pages
#define HAL_OLED128x64_X         128  //!< Max X
#define HAL_OLED128x64_Y         64   //!< Max Y

/** @brief   Font Table Configuretion.
 *  8x16
 */
#define FONT_TABLE_8x16           FontTable_H_8X16                //!< 8x16 ASCII Char.
#define FONT_TABLE_MIKE_8x16      Mike_8x16                 //!< 8x16 ASCII Char.
#define FONT_TABLE_MIKE_RUS1      Mike_8x16_Rus1
#define FONT_TABLE_MIKE_RUS2      Mike_8x16_Rus2
#define FONT_TABLE_MIKE_OTHER     Mike_8x16_Other
#define FONT_TABLE_GYVER          Gyver_5x8
#define ICON_TABLE_7x7            Icons_7x7
#define ICON_TABLE_8x8            Icons_8x8

/**
 * @fn      halOLED128x64Init
 * 
 * @brief	Init. OLED128x64.
 */
extern void halOLED128x64Init(void);

/**
 * @fn      halOLED128x64ClearScreen
 * 
 * @brief	Clear screen.
 */
void halOLED128x64ClearScreen(void);
   
/**
 * @fn      halOLED128x64Show
 * 
 * @brief	Show x16(Height: 16) String, Supported Font: 
 *          1. ASCII - 8x16 2. Russian 8x16 characters
 *
 * @param   line - 0 ~ 3
 * @param   column - 0 ~ 127
 * @param   str - string
 */
void halOLED128x64ShowX16(uint8 line, uint8 column, const uint8 *str);
void halOLED128x64ShowX8(uint8 line, uint8 column, const uint8 *str);

/**
 * @fn      halOLED128x64ShowPicture
 * 
 * @brief	Show Picture
 *
 * @param   x - 0 ~ 127
 * @param   y - 0 ~ 64
 * @param   picWidth - max: 128
 * @param   picHeight - max: 64
 * @param   pic - picture
 */
void halOLED128x64ShowPicture(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic);
void halOLED128x64ShowPictureChina(uint8 x, uint8 y, uint8 picWidth, uint8 picHeight, const uint8 *pic);
void halOLED128x64ShowIcon(uint8 x, uint8 y, uint8 size, uint8 idx);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_OLED_128X64_H */
