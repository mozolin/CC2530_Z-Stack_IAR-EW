#ifndef HAL_DELAY_H
#define HAL_DELAY_H

//#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { SYSCLK_16MHZ, SYSCLK_32MHZ } halDelaySysClk_t;

void delayUsIn32Mhz(uint16 nUs);
void delayMs(halDelaySysClk_t sysClk, uint16 nMs);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef HAL_DELAY_H */
