#ifndef SPI_COMMON_H
#define SPI_COMMON_H

#include "cc2530_io_ports.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Type adapter */
//typedef uint8_t  uint8;
//typedef uint16_t uint16;
  
/*
 *  SPI parameters: alternate, bit order, clock polarity, clock phase.
 */
// alternate
#define HW_SPI1_ALT2        3  // SCK:P1_5, MISO:P1_6, MOSI:P1_7
// bit order
#define SPI_BITORDER_MSB 	0  // MSB
#define SPI_BITORDER_LSB 	1  // LSB
// clock polarity
#define SPI_CPOL_LOW     	0  // low level
#define SPI_CPOL_HIGH    	1  // high level
// clock phase
#define SPI_CPHA_FIRST   	0  // sample in first edge
#define SPI_CPHA_SECOND  	1  // sample in second edge
  
/** @brief   Ignore GPIO.
 */
#define SPI_IGNORE_IO     	255

/* 
 *  GPIO Function: Output, Input, Output High/Low Level, Get.
 */
// Output 
#define SPI_GPIO_OUTPUT(Port, Pin)  CC2530_IOCTL(Port, Pin, CC2530_OUTPUT)
// Input
#define SPI_GPIO_INPUT(Port, Pin)   CC2530_IOCTL(Port, Pin, CC2530_INPUT_PULLUP)
// Output High Level
#define SPI_GPIO_SET(Port, Pin)     CC2530_GPIO_SET(Port, Pin)
// Output Low Level
#define SPI_GPIO_CLEAR(Port, Pin)   CC2530_GPIO_CLEAR(Port, Pin)
// Get
#define SPI_GPIO_GET(Port, Pin)     CC2530_GPIO_GET(Port, Pin)


/*
 * @brief    Init. SPI CS(Chip Select) GPIO
 * @param 	CS_Port, CS_Pin - CS(Chip Select) GPIO
 */
#define SPI_CS_INIT(CS_Port, CS_Pin) do {   \
 	SPI_GPIO_OUTPUT(CS_Port, CS_Pin);       \
 	SPI_GPIO_SET(CS_Port, CS_Pin);          \
} while(0)

/*
 * @brief	Select/Release SPI-Chip
 * @param 	CS_Port, CS_Pin - CS(Chip Select) GPIO
 */
#define SPI_CS_SELECT(CS_Port, CS_Pin)  SPI_GPIO_CLEAR(CS_Port, CS_Pin)
#define SPI_CS_RELEASE(CS_Port, CS_Pin) SPI_GPIO_SET(CS_Port, CS_Pin)


#ifdef __cplusplus
}
#endif

#endif /* #ifndef SPI_COMMON_H */