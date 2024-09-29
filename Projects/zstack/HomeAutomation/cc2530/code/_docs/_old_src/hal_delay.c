#include "hal_delay.h"

#pragma optimize=none
void delayUsIn32Mhz(uint16 nUs)
{
    for(uint16 i = 0; i < nUs; i++)
    {
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
    }
}

#pragma optimize=none
void delayMs(halDelaySysClk_t sysClk, uint16 nMs)
{
  uint16 i, j;
  uint16 loop1Ms;
  
  if (sysClk == SYSCLK_16MHZ) loop1Ms = 535;
  else loop1Ms = 1070;

  for(i = 0; i < nMs; i++) for(j = 0; j < loop1Ms; j++);
}
