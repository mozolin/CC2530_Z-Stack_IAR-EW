#ifndef HAL_LCD_CFG_H
#define HAL_LCD_CFG_H

//#include "sw_spi.h"
//#include "hw_spi.h" 

#ifdef __cplusplus
extern "C" {
#endif

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
#define HAL_LCD_SPI_CS_PIN    		3
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
