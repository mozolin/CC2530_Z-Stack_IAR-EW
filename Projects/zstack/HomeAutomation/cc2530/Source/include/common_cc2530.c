#include "common_cc2530.h"

// Led
#define LED_PORT        0       //!< Led port.
#define LED_PIN         4       //!< Led pin.
#define LED             P0_4    //!< Led GPIO.
#define LED_ON          1 //!< on.
#define LED_OFF         0 //!< off.

// Button
#define BUTTON_PORT     0       //!< Button port.
#define BUTTON_PIN      1       //!< Button pin.
#define BUTTON          P0_1    //!< Button GPIO.
#define BUTTON_NORMAL   1 //!< normal state.
#define BUTTON_DOWN     0 //!< press-down.


/**
 * example: P0_4 = General-purpose I/O, Output
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * uint8 gpioPort = 0;
 * uint8 gpioBit = 4;
 * bool gpioGeneralPurpose = true;
 * bool gpioInput = false;
 * bool gpioPullUpDn = 0;
 *
 * GPIO pin(gpioPort, gpioBit, gpioGeneralPurpose, gpioInput, gpioPullUpDn);
 * GPIO pin(0, 4, true, false, 0);
 */

/*
static void initGPIO(uint8 gpioPort, uint8 gpioBit, bool gpioGeneralPurpose, bool gpioInput, uint8 gpioPullUpDn)
{
	switch(gpioPort)
  {
    //-- P0
    case 0:
      //-- P0_0 ... P0_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
        if(gpioGeneralPurpose) {
					//-- set "gpioBit" to 0 => General-purpose I/O
					P0SEL &= ~(1 << gpioBit);
				} else {
					//-- set "gpioBit" to 1 => Peripheral function
					P0SEL |= (1 << gpioBit);
				}
				if(gpioInput) {
					//-- set "gpioBit" to 0 => Input
					P0DIR &= ~(1 << gpioBit);
				} else {
					//-- set "gpioBit" to 1 => Output
					P0DIR |= (1 << gpioBit);
				}
				switch(gpioPullUpDn) {
					//-- Pullup
					case 0:
						//-- set "gpioBit" to 0 => Pullup or pulldown
						P0INP &= ~(1 << gpioBit);
						//-- set bit 5 to 0 => Pullup (Port 0)
						P2INP &= ~(1 << 5);
						break;
					//-- Pulldown
					case 1:
						//-- set "gpioBit" to 0 => Pullup or pulldown
						P0INP &= ~(1 << gpioBit);
						//-- //-- set bit 5 to 1 => Pulldown (Port 0)
						P2INP |= (1 << 5);
						break;
					//-- 3-state
					case 2:
						//-- set "gpioBit" to 1 => 3-state
						P0INP |= (1 << gpioBit);
						break;
					default:
						break;
				}
			}
      break;
    //-- P1
    case 1:
      //-- P1_0 ... P1_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
        if(gpioGeneralPurpose) {
					P1SEL &= ~(1 << gpioBit);
				} else {
					P1SEL |= (1 << gpioBit);
				}
				if(gpioInput) {
					P1DIR &= ~(1 << gpioBit);
				} else {
					P1DIR |= (1 << gpioBit);
				}
				//-- !!! P1_2 ... P1_7 !!!
      	if(gpioBit >= 2 && gpioBit <= 7) {
					switch(gpioPullUpDn) {
						//-- Pullup
						case 0:
							//-- set "gpioBit" to 0 => Pullup or pulldown
							P1INP &= ~(1 << gpioBit);
							//-- set bit 6 to 0 => Pullup (Port 1)
							P2INP &= ~(1 << 6);
							break;
						//-- Pulldown
						case 1:
							//-- set "gpioBit" to 0 => Pullup or pulldown
							P0INP &= ~(1 << gpioBit);
							//-- //-- set bit 6 to 1 => Pulldown (Port 1)
							P2INP |= (1 << 6);
							break;
						//-- 3-state
						case 2:
							//-- set "gpioBit" to 1 => 3-state
							P0INP |= (1 << gpioBit);
							break;
						default:
							break;
					}
				}
			}
      break;
    
    //-- P2
    case 2:
      //-- P2_0 ... P2_4
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 4) {
        if(gpioGeneralPurpose) {
					P2SEL &= ~(1 << gpioBit);
				} else {
					P2SEL |= (1 << gpioBit);
				}
				if(gpioInput) {
					P2DIR &= ~(1 << gpioBit);
				} else {
					P2DIR |= (1 << gpioBit);
				}
				switch(gpioPullUpDn) {
					//-- Pullup
					case 0:
						//-- set "gpioBit" to 0 => Pullup or pulldown
						P2INP &= ~(1 << gpioBit);
						//-- set bit 7 to 0 => Pullup (Port 2)
						P2INP &= ~(1 << 7);
						break;
					//-- Pulldown
					case 1:
						//-- set "gpioBit" to 0 => Pullup or pulldown
						P2INP &= ~(1 << gpioBit);
						//-- //-- set bit 7 to 1 => Pulldown (Port 2)
						P2INP |= (1 << 7);
						break;
					//-- 3-state
					case 2:
						//-- set "gpioBit" to 1 => 3-state
						P2INP |= (1 << gpioBit);
						break;
					default:
						break;
				}
			}
      break;
    
    default:
    	break;
	}
}
*/
static void delayMs(uint16 nMs)
{
  uint16 i,j;
  for(i = 0; i < nMs; i++) for(j = 0; j < 535; j++);
}

static void initLed()
{
  CC2530_IOCTL(LED_PORT, LED_PIN, CC2530_OUTPUT);
  LED = LED_OFF;
}

static void initButton()
{
  CC2530_IOCTL(BUTTON_PORT, BUTTON_PIN, CC2530_INPUT_PULLUP);
}
