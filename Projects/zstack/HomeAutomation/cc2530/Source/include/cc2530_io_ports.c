#include <stdio.h>
#include "colors.h"
#include <ioCC2530.h>
//#include "cc2530_ioctl.h"
#include "cc2530_io_ports.h"


/**
 * P0_4: General-purpose I/O, Output, Pullup, Interrupt enabled, Rising edge
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * uint8 gpioPort = 0;             (0-2)
 * uint8 gpioBit = 4;              (0-4: for port 2, 0-7: for ports 0 and 1)
 * bool gpioGeneralPurpose = true; (true: General purpose, false: Peripheral function)
 * bool gpioOutput = true;         (true: Output, false: Input)
 * bool gpioPullUpDn = 0;          (0: Pullup, 1: Pulldown, 2: 3-state, -1: None)
 * bool gpioIntrEnable = true;     (true: Interrupt enabled, false: Interrupt disabled)
 * uint8 gpioIntrCtrl = 1;         (0: Rising edge, 1: Falling edge, -1: None)
 *
 * initGPIO(gpioPort, gpioBit, gpioGeneralPurpose, gpioOutput, gpioPullUpDn, gpioIntrEnable, gpioIntrCtrl);
 * initGPIO(0, 4, true, false, 0, true, 1);
 */

void initGPIO(
	uint8 gpioPort,
	uint8 gpioBit,
	bool  gpioGeneralPurpose,
	bool  gpioOutput,
	uint8 gpioPullUpDn,
	bool gpioIntrEnable,
	int8 gpioIntrCtrl
)
{
	switch(gpioPort)
  {
    //-- P0
    case 0:
      //-- P0_0 ... P0_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
        //-- P0SEL (0xF3) – Port 0 Function Select
        if(gpioGeneralPurpose) {
					//-- set "gpioBit" to 0 => General-purpose I/O
					P0SEL &= ~(1 << gpioBit);
				} else {
					//-- set "gpioBit" to 1 => Peripheral function
					P0SEL |= (1 << gpioBit);
				}
				//--- P0DIR (0xFD) – Port 0 Direction
				if(gpioOutput) {
					//-- set "gpioBit" to 1 => Output
					P0DIR |= (1 << gpioBit);
				} else {
					//-- set "gpioBit" to 0 => Input
					P0DIR &= ~(1 << gpioBit);
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
				//-- Interrupt Enable
				if(gpioIntrEnable) {
					//-- Enable interrupts
  				EA = 1;
  				//-- set "gpioBit" to 0 => Clear the pin interrupt flag
					P0IFG &= ~(1 << gpioBit);
					P0IF = 0;
					//-- set bit "gpioBit" to 1 => Interrupts are enabled
					P0IEN |= (1 << gpioBit);
					//-- Port 0, inputs 7 to 0 interrupt configuration
					if(gpioIntrCtrl != -1) {
						if(gpioIntrCtrl == 0) {
  						//-- set bit "0" to 0 => Rising edge on input gives interrupt
							PICTL &= ~(1 << 0);
						} else {
							//-- set bit "0" to 1 => Falling edge on input gives interrupt
							PICTL |= (1 << 0);
						}
					}
					//-- set bit "5" to 1 => Port 0 Interrupt enabled
					IEN1 |= (1 << 5);
				} else {
					//-- Disable interrupts
  				EA = 0;
					//-- set bit "5" to 0 => Port 0 Interrupt disabled
					IEN1 &= ~(1 << 5);
 					//-- set bit "gpioBit" to 0 => Interrupts are disabled
					P0IEN &= ~(1 << gpioBit);
				}
			}
			
			/*
			printf("PICTL=1,P0IFG=16,P0IEN=4,P0IF=0,IEN1=37,EA=1\r\n");
			*/
			if(gpioPort == 0 && gpioBit == 7) {
				printf(FONT_COLOR_RED);
  			printf("BUTTON (P%d.%d): PICTL=%d,P0IFG=%d,P0IEN=%d,P0IF=%d,IEN1=%d,EA=%d\r\n", gpioPort,gpioBit,PICTL,P0IFG,P0IEN,P0IF,IEN1,EA);
  			printf(STYLE_COLOR_RESET);
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
				if(gpioOutput) {
					//-- set "gpioBit" to 1 => Output
					P1DIR |= (1 << gpioBit);
				} else {
					//-- set "gpioBit" to 0 => Input
					P1DIR &= ~(1 << gpioBit);
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
				//-- Interrupt Enable
				if(gpioIntrEnable) {
					//-- Enable interrupts
  				EA = 1;
  				//-- set "gpioBit" to 0 => Clear the pin interrupt flag
					P1IFG &= ~(1 << gpioBit);
					P1IF = 0;
					//-- set bit "gpioBit" to 1 => Interrupts are enabled
					P1IEN |= (1 << gpioBit);
					//-- !!! P1_0 ... P1_3 !!!
      		if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 3) {
						//-- Port 1, inputs 3 to 0 interrupt configuration
						if(gpioIntrCtrl != -1) {
							if(gpioIntrCtrl == 0) {
  							//-- set bit "1" to 0 => Rising edge on input gives interrupt
								PICTL &= ~(1 << 1);
							} else {
								//-- set bit "1" to 1 => Falling edge on input gives interrupt
								PICTL |= (1 << 1);
							}
						}
					} else {
						//-- Port 1, inputs 7 to 4 interrupt configuration
						if(gpioIntrCtrl != -1) {
							if(gpioIntrCtrl == 0) {
  							//-- set bit "2" to 0 => Rising edge on input gives interrupt
								PICTL &= ~(1 << 2);
							} else {
								//-- set bit "2" to 1 => Falling edge on input gives interrupt
								PICTL |= (1 << 2);
							}
						}
					}
					//-- set bit "4" to 1 => Port 1 Interrupt enabled
					IEN2 |= (1 << 4);
				} else {
					//-- Disable interrupts
  				EA = 0;
					//-- set bit "4" to 0 => Port 1 Interrupt disabled
					IEN2 &= ~(1 << 4);
					//-- set bit "gpioBit" to 0 => Interrupts are enabled
					P1IEN &= ~(1 << gpioBit);
				}
			}
      
      //printf("PICTL=1,P0IFG=16,P0IEN=4,P0IF=0,IEN1=37,EA=1\r\n");
			/*
			printf(FONT_COLOR_RED);
  		printf("BUTTON (P%d.%d): PICTL=%d,P1IFG=%d,P1IEN=%d,P1IF=%d,IEN2=%d,EA=%d\r\n", gpioPort,gpioBit,PICTL,P1IFG,P1IEN,P1IF,IEN2,EA);
  		printf(STYLE_COLOR_RESET);
  		*/
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
				if(gpioOutput) {
					//-- set "gpioBit" to 1 => Output
					P1DIR |= (1 << gpioBit);
				} else {
					//-- set "gpioBit" to 0 => Input
					P1DIR &= ~(1 << gpioBit);
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
				//-- Interrupt Enable
				if(gpioIntrEnable) {
					//-- Enable interrupts
  				EA = 1;
  				//-- set "gpioBit" to 0 => Clear the pin interrupt flag
					P2IFG &= ~(1 << gpioBit);
					P2IF = 0;
					//-- set bit "gpioBit" to 1 => Interrupts are enabled
					P2IEN |= (1 << gpioBit);
					//-- Port 2, inputs 4 to 0 interrupt configuration
					if(gpioIntrCtrl != -1) {
						if(gpioIntrCtrl == 0) {
  						//-- set bit "3" to 0 => Rising edge on input gives interrupt
							PICTL &= ~(1 << 3);
						} else {
							//-- set bit "3" to 1 => Falling edge on input gives interrupt
							PICTL |= (1 << 3);
						}
					}
					//-- set bit "1" to 1 => Port 2 Interrupt enabled
					IEN2 |= (1 << 1);
				} else {
					//-- Disable interrupts
  				EA = 0;
					//-- set bit "1" to 0 => Port 2 Interrupt disabled
					IEN2 &= ~(1 << 1);
					//-- set bit "gpioBit" to 0 => Interrupts are enabled
					P2IEN &= ~(1 << gpioBit);
				}
			}
      break;
    
    default:
    	break;
	}
}


void resetGPIO(uint8 gpioPort, uint8 gpioBit)
{
	switch(gpioPort)
	{
		//-- P0
    case 0:
      //-- P0_0 ... P0_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
    		//-- set "gpioBit" to 0 => Clear the pin interrupt flag
				P0IFG &= ~(1 << gpioBit);
				P0IF = 0;
      }
      break;
		//-- P1
    case 1:
      //-- P1_0 ... P1_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
    		//-- set "gpioBit" to 0 => Clear the pin interrupt flag
				P1IFG &= ~(1 << gpioBit);
				P1IF = 0;
      }
      break;
		//-- P2
    case 2:
      //-- P2_0 ... P2_4
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 4) {
    		//-- set "gpioBit" to 0 => Clear the pin interrupt flag
				P2IFG &= ~(1 << gpioBit);
				P2IF = 0;
      }
      break;
    default:
    	break;
	}
}

void initShortGPIO(uint8 gpioPort, uint8 gpioBit, bool gpioOutput)
{
	initGPIO(
		gpioPort,
		gpioBit,
		GPIO_GENERAL_PURPOSE,
		gpioOutput,
		GPIO_PULL_NONE,
		GPIO_INTERRUPT_DISABLED,
		GPIO_INTERRUPT_CTRL_NONE
	);
}

void delayMs(uint16 nMs)
{
  uint16 i,j;
  for(i = 0; i < nMs; i++) {
  	for (j = 0; j < 535; j++) {
  		//-- do nothing, just waiting...
  	}
  };
}

