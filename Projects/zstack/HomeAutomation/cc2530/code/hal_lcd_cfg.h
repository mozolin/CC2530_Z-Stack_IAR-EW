#ifndef HAL_LCD_CFG_H
#define HAL_LCD_CFG_H

#ifdef __cplusplus
extern "C" {
#endif


//-- !!! only one option "HAL_LCD_TYPE" can be defined !!!
#define HAL_LCD_TYPE              HAL_LCD_TYPE_TFT  //-- set LCD type to TFT
//#define HAL_LCD_TYPE              HAL_LCD_TYPE_OLED //-- set LCD type to TFT

/*
 *  HAL_LCD_SPI_SW: SW-SPI Bus
 *  HAL_LCD_SPI_HW: HW-SPI Bus
 */     
#if !defined(HAL_LCD_SPI_SW) && !defined(HAL_LCD_SPI_HW)
  #define HAL_LCD_SPI_SW
#endif
   
#ifdef HAL_LCD_SPI_SW
  /* SCK: Clock GPIO */
  #define HAL_LCD_SPI_SCK_PORT  	1
  #define HAL_LCD_SPI_SCK_PIN   	2
  
  /* SDA: Data GPIO */
  #define HAL_LCD_SPI_SDA_PORT  	1
  #define HAL_LCD_SPI_SDA_PIN   	3
#endif

/* CS: Chip Select GPIO */
#define HAL_LCD_SPI_CS_PORT   		0
#define HAL_LCD_SPI_CS_PIN    		2
/* DC: Data/Command GPIO */
#define HAL_LCD_SPI_DC_PORT   		0
#define HAL_LCD_SPI_DC_PIN    		0
/* RST: Reset GPIO */
#define HAL_LCD_SPI_RST_PORT  		1
#define HAL_LCD_SPI_RST_PIN   		7


#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_LCD_CFG_H */
