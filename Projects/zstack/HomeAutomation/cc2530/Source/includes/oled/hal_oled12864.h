#ifndef HAL_OLED12864_H
#define HAL_OLED12864_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief   OLED12864 parameters.
 */
#define HAL_OLED12864_PAGE      8    //!< Total pages
#define HAL_OLED12864_X         128  //!< Max X
#define HAL_OLED12864_Y         64   //!< Max Y

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
 * @fn      halOLED12864Init
 * 
 * @brief	Init. OLED12864.
 */
extern void halOLED12864Init(void);

/**
 * @fn      halOLED12864ClearScreen
 * 
 * @brief	Clear screen.
 */
void halOLED12864ClearScreen(void);
   
/**
 * @fn      halOLED12864Show
 * 
 * @brief	Show x16(Height: 16) String, Supported Font: 
 *          1. ASCII - 8x16 2. Russian 8x16 characters
 *
 * @param   line - 0 ~ 3
 * @param   column - 0 ~ 127
 * @param   str - string
 */
void halOLED12864ShowX16(uint8 line, uint8 column, const uint8 *str);
void halOLED12864ShowX8(uint8 line, uint8 column, const uint8 *str);

/**
 * @fn      halOLED12864ShowPicture
 * 
 * @brief	Show Picture
 *
 * @param   x - 0 ~ 127
 * @param   y - 0 ~ 64
 * @param   picWidth - max: 128
 * @param   picHeight - max: 64
 * @param   pic - picture
 */
void halOLED12864ShowPicture(unsigned char x, unsigned char y, unsigned char picWidth, unsigned char picHeight, const unsigned char *pic);
void halOLED12864ShowIcon(uint8 x, uint8 y, uint8 size, uint8 idx);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_OLED12864_H */
