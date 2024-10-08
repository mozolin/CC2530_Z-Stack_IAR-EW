#ifndef HAL_FONTS_H
#define HAL_FONTS_H

//-- "vertical" fonts
#include "font_v_8x16.h"
#include "font_mike_8x16.h"
#include "font_gyver_5x8.h"
#include "icons_7x7.h"
#include "icons_8x8.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @brief   Font Table Configuretion.
 */
#define FONT_TABLE_8X16           FontTable_8x16
#define FONT_TABLE_MIKE_8X16      Mike_8x16
#define FONT_TABLE_MIKE_RUS1      Mike_8x16_Rus1
#define FONT_TABLE_MIKE_RUS2      Mike_8x16_Rus2
#define FONT_TABLE_MIKE_OTHER     Mike_8x16_Other
#define FONT_TABLE_GYVER          Gyver_5x8
#define ICON_TABLE_7x7            Icons_7x7
#define ICON_TABLE_8x8            Icons_8x8


//-- internal number of font tables 8x16
#define FT_8X16           0
#define FT_MIKE_8X16      1
#define FT_MIKE_OTHER     2
#define FT_MIKE_RUS1      3
#define FT_MIKE_RUS2      4
#define FT_MIKE_RUS2      4


#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_FONTS_H */
