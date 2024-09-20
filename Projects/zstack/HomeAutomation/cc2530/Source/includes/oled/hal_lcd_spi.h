#ifndef HAL_LCD_SPI_H
#define HAL_LCD_SPI_H

#include "sw_spi.h"
#include "hw_spi.h" 

#ifdef __cplusplus
extern "C" {
#endif

void halLcdSpiInit(void);
void halLcdSpiTxCmd(uint8 cmd);
void halLcdSpiTxData(uint8 dat);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_LCD_SPI_H */
