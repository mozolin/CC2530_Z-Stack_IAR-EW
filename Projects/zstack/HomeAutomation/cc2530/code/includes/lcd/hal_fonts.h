#ifndef HAL_FONTS_H
#define HAL_FONTS_H

#include "font_mike_8x16.h"
#include "font_gyver_5x8.h"
#include "icons_7x7.h"
#include "icons_8x8.h"

#include "font_h_8x16.h"
#include "font_mike_8x16_h.h"
#include "font_chinese_h_16x16.h"


#ifdef __cplusplus
extern "C" {
#endif


/** @brief   Font Table Configuretion.
 *  8x16
 */
#define FONT_TABLE_8x16_H         FontTable_8X16_H
#define FONT_TABLE_MIKE_8x16_H    Mike_8x16_H
#define FONT_TABLE_MIKE_RUS1_H    Mike_8x16_Rus1_H
#define FONT_TABLE_MIKE_RUS2_H    Mike_8x16_Rus2_H
#define FONT_TABLE_MIKE_OTHER_H   Mike_8x16_Other_H
#define FONT_TABLE_CH_16x16       FontTable_Chinese_H_16X16
#define FONT_TABLE_CH_SIZE        FontTable_Chinese_H_16x16_Num

#define FONT_TABLE_8x16           FontTable_8X16
#define FONT_TABLE_MIKE_8x16      Mike_8x16
#define FONT_TABLE_MIKE_RUS1      Mike_8x16_Rus1
#define FONT_TABLE_MIKE_RUS2      Mike_8x16_Rus2
#define FONT_TABLE_MIKE_OTHER     Mike_8x16_Other
#define FONT_TABLE_GYVER          Gyver_5x8
#define ICON_TABLE_7x7            Icons_7x7
#define ICON_TABLE_8x8            Icons_8x8


#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_FONTS_H */
