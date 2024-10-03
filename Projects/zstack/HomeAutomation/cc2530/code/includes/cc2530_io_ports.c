#include <stdio.h>
#include "colors.h"
#include <ioCC2530.h>
#include "cc2530_io_ports.h"


/**
 * P0_4: General-purpose I/O, Output, Pullup, Interrupt enabled, Rising edge
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * uint8 gpioPort = 0;             (0-2)
 * uint8 gpioBit = 4;              (0-4: for port 2, 0-7: for ports 0 and 1)
 * int8 gpioGeneralPurpose = true; (true: General purpose, false: Peripheral function, -1: None)
 * bool gpioOutput = true;         (true: Output, false: Input)
 * int8 gpioPullUpDn = 0;          (0: Pullup, 1: Pulldown, 2: 3-state, -1: None)
 * int8 gpioIntrEnable = true;     (true: Interrupt enabled, false: Interrupt disabled, -1: None)
 * int8 gpioIntrCtrl = 1;          (0: Rising edge, 1: Falling edge, -1: None)
 *
 * initGPIO(gpioPort, gpioBit, gpioGeneralPurpose, gpioOutput, gpioPullUpDn, gpioIntrEnable, gpioIntrCtrl);
 * initGPIO(0, 4, true, false, 0, true, 1);
 */

void initGPIO(
	uint8 gpioPort,
	uint8 gpioBit,
	int8  gpioGeneralPurpose,
	bool  gpioOutput,
	int8  gpioPullUpDn,
	int8  gpioIntrEnable,
	int8  gpioIntrCtrl
)
{
	switch(gpioPort)
  {
    //-- Port #0
    case 0:
      //-- P0_0 ... P0_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
        if(gpioGeneralPurpose != GPIO_FUNC_NONE) {
          //-- P0SEL (0xF3) – Port 0 Function Select
          if(gpioGeneralPurpose == GPIO_FUNC_GENERAL_PURPOSE) {
						//-- set "gpioBit" to 0 => General-purpose I/O
						P0SEL &= ~(1 << gpioBit);
					} else {
						//-- set "gpioBit" to 1 => Peripheral function
						P0SEL |= (1 << gpioBit);
					}
				}
				//--- P0DIR (0xFD) – Port 0 Direction
				if(gpioOutput == GPIO_DIR_OUTPUT) {
					//-- set "gpioBit" to 1 => Output
					P0DIR |= (1 << gpioBit);
				} else {
					//-- set "gpioBit" to 0 => Input
					P0DIR &= ~(1 << gpioBit);
				}
				if(gpioPullUpDn != GPIO_PULL_NONE) {
					switch(gpioPullUpDn) {
						//-- Pullup
						case GPIO_PULL_UP:
							//-- set "gpioBit" to 0 => Pullup or pulldown
							P0INP &= ~(1 << gpioBit);
							//-- set bit 5 to 0 => Pullup (Port 0)
							P2INP &= ~(1 << 5);
							break;
						//-- Pulldown
						case GPIO_PULL_DOWN:
							//-- set "gpioBit" to 0 => Pullup or pulldown
							P0INP &= ~(1 << gpioBit);
							//-- //-- set bit 5 to 1 => Pulldown (Port 0)
							P2INP |= (1 << 5);
							break;
						//-- 3-state
						case GPIO_PULL_TRISTATE:
							//-- set "gpioBit" to 1 => 3-state
							P0INP |= (1 << gpioBit);
							break;
						default:
							break;
					}
				}
				//-- Interrupt Enable
				if(gpioIntrEnable != GPIO_INTERRUPT_NONE) {
					if(gpioIntrEnable == GPIO_INTERRUPT_ENABLED) {
  					resetGPIO(gpioPort, gpioBit);
  					/*
  					//-- set "gpioBit" to 0 => Clear the pin interrupt flag
						P0IFG &= ~(1 << gpioBit);
						P0IF = 0;
						*/
						//-- set bit "gpioBit" to 1 => Interrupts are enabled
						P0IEN |= (1 << gpioBit);
						//-- Port 0, inputs 7 to 0 interrupt configuration
						if(gpioIntrCtrl != GPIO_INTERRUPT_CTRL_NONE) {
							if(gpioIntrCtrl == GPIO_INTERRUPT_CTRL_RISE) {
  							//-- set bit "0" to 0 => Rising edge on input gives interrupt
								PICTL &= ~(1 << 0);
							} else {
								//-- set bit "0" to 1 => Falling edge on input gives interrupt
								PICTL |= (1 << 0);
							}
						}
					} else {
 						//-- set bit "gpioBit" to 0 => Interrupts are disabled
						P0IEN &= ~(1 << gpioBit);
					}
				}
				
				delayMs16MHZ(10);
				printf(FONT_COLOR_STRONG_GREEN);
  			printf("P%d.%d: initiated\n", gpioPort, gpioBit);
  			printf(STYLE_COLOR_RESET);
			}
      break;
    //-- Port #1
    case 1:
      //-- P1_0 ... P1_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
        if(gpioGeneralPurpose != GPIO_FUNC_NONE) {
        	if(gpioGeneralPurpose == GPIO_FUNC_GENERAL_PURPOSE) {
						P1SEL &= ~(1 << gpioBit);
					} else {
						P1SEL |= (1 << gpioBit);
					}
				}
				if(gpioOutput == GPIO_DIR_OUTPUT) {
					//-- set "gpioBit" to 1 => Output
					P1DIR |= (1 << gpioBit);
				} else {
					//-- P1_0 & P1_1 - Output only!
					if(gpioBit == 0 || gpioBit == 1) {
						printf(FONT_COLOR_STRONG_RED);
  					printf("P%d.%d cannot be set to Input (P1.0 & P1.1 are Output only)\n", gpioPort, gpioBit);
  					printf(STYLE_COLOR_RESET);
						break;
					}
					//-- set "gpioBit" to 0 => Input
					P1DIR &= ~(1 << gpioBit);
				}
				//-- !!! P1_2 ... P1_7 !!!
      	if(gpioBit >= 2 && gpioBit <= 7) {
					if(gpioPullUpDn != GPIO_PULL_NONE) {
						switch(gpioPullUpDn) {
							//-- Pullup
							case GPIO_PULL_UP:
								//-- set "gpioBit" to 0 => Pullup or pulldown
								P1INP &= ~(1 << gpioBit);
								//-- set bit 6 to 0 => Pullup (Port 1)
								P2INP &= ~(1 << 6);
								break;
							//-- Pulldown
							case GPIO_PULL_DOWN:
								//-- set "gpioBit" to 0 => Pullup or pulldown
								P1INP &= ~(1 << gpioBit);
								//-- //-- set bit 6 to 1 => Pulldown (Port 1)
								P2INP |= (1 << 6);
								break;
							//-- 3-state
							case GPIO_PULL_TRISTATE:
								//-- set "gpioBit" to 1 => 3-state
								P1INP |= (1 << gpioBit);
								break;
							default:
								break;
						}
					}
				}
				//-- Interrupt Enable
				if(gpioIntrEnable != GPIO_INTERRUPT_NONE) {
					if(gpioIntrEnable == GPIO_INTERRUPT_ENABLED) {
  					resetGPIO(gpioPort, gpioBit);
  					/*
  					//-- set "gpioBit" to 0 => Clear the pin interrupt flag
						P1IFG &= ~(1 << gpioBit);
						P1IF = 0;
						*/
						//-- set bit "gpioBit" to 1 => Interrupts are enabled
						P1IEN |= (1 << gpioBit);
						//-- !!! P1_0 ... P1_3 !!!
      			if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 3) {
							//-- Port 1, inputs 3 to 0 interrupt configuration
							if(gpioIntrCtrl != GPIO_INTERRUPT_CTRL_NONE) {
								if(gpioIntrCtrl == GPIO_INTERRUPT_CTRL_RISE) {
  								//-- set bit "1" to 0 => Rising edge on input gives interrupt
									PICTL &= ~(1 << 1);
								} else {
									//-- set bit "1" to 1 => Falling edge on input gives interrupt
									PICTL |= (1 << 1);
								}
							}
						} else {
							//-- Port 1, inputs 7 to 4 interrupt configuration
							if(gpioIntrCtrl != GPIO_INTERRUPT_CTRL_NONE) {
								if(gpioIntrCtrl == GPIO_INTERRUPT_CTRL_RISE) {
  								//-- set bit "2" to 0 => Rising edge on input gives interrupt
									PICTL &= ~(1 << 2);
								} else {
									//-- set bit "2" to 1 => Falling edge on input gives interrupt
									PICTL |= (1 << 2);
								}
							}
						}
					} else {
						//-- set bit "gpioBit" to 0 => Interrupts are disabled
						P1IEN &= ~(1 << gpioBit);
					}
				}
				
				delayMs16MHZ(10);
				printf(FONT_COLOR_STRONG_GREEN);
  			printf("P%d.%d: initiated\n", gpioPort, gpioBit);
  			printf(STYLE_COLOR_RESET);
			}
      break;
    
    //-- Port #2
    case 2:
      //-- P2_0 ... P2_4
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 4) {
        if(gpioGeneralPurpose != GPIO_FUNC_NONE) {
          if(gpioGeneralPurpose == GPIO_FUNC_GENERAL_PURPOSE) {
						P2SEL &= ~(1 << gpioBit);
					} else {
						P2SEL |= (1 << gpioBit);
					}
				}
				if(gpioOutput == GPIO_DIR_OUTPUT) {
					//-- set "gpioBit" to 1 => Output
					P1DIR |= (1 << gpioBit);
				} else {
					//-- set "gpioBit" to 0 => Input
					P1DIR &= ~(1 << gpioBit);
				}
				if(gpioPullUpDn != GPIO_PULL_NONE) {
					switch(gpioPullUpDn) {
						//-- Pullup
						case GPIO_PULL_UP:
							//-- set "gpioBit" to 0 => Pullup or pulldown
							P2INP &= ~(1 << gpioBit);
							//-- set bit 7 to 0 => Pullup (Port 2)
							P2INP &= ~(1 << 7);
							break;
						//-- Pulldown
						case GPIO_PULL_DOWN:
							//-- set "gpioBit" to 0 => Pullup or pulldown
							P2INP &= ~(1 << gpioBit);
							//-- //-- set bit 7 to 1 => Pulldown (Port 2)
							P2INP |= (1 << 7);
							break;
						//-- 3-state
						case GPIO_PULL_TRISTATE:
							//-- set "gpioBit" to 1 => 3-state
							P2INP |= (1 << gpioBit);
							break;
						default:
							break;
					}
				}
				//-- Interrupt Enable
				if(gpioIntrEnable != GPIO_INTERRUPT_NONE) {
					if(gpioIntrEnable == GPIO_INTERRUPT_ENABLED) {
  					resetGPIO(gpioPort, gpioBit);
  					/*
  					//-- set "gpioBit" to 0 => Clear the pin interrupt flag
						P2IFG &= ~(1 << gpioBit);
						P2IF = 0;
						*/
						//-- set bit "gpioBit" to 1 => Interrupts are enabled
						P2IEN |= (1 << gpioBit);
						//-- Port 2, inputs 4 to 0 interrupt configuration
						if(gpioIntrCtrl != GPIO_INTERRUPT_CTRL_NONE) {
							if(gpioIntrCtrl == GPIO_INTERRUPT_CTRL_RISE) {
  							//-- set bit "3" to 0 => Rising edge on input gives interrupt
								PICTL &= ~(1 << 3);
							} else {
								//-- set bit "3" to 1 => Falling edge on input gives interrupt
								PICTL |= (1 << 3);
							}
						}
					} else {
						//-- set bit "gpioBit" to 0 => Interrupts are enabled
						P2IEN &= ~(1 << gpioBit);
					}
				}

				delayMs16MHZ(10);
				printf(FONT_COLOR_STRONG_GREEN);
  			printf("P%d.%d: initiated\n", gpioPort, gpioBit);
  			printf(STYLE_COLOR_RESET);
			}
      break;
    
    default:
    	break;
	}
}

void setInterrupts(bool isEnabled)
{
  if(isEnabled) {
    //-- Enable interrupts
  	EA = 1;
		//-- set bit "5" to 1 => Port 0 Interrupt enabled
		IEN1 |= (1 << 5);
		//-- set bit "4" to 1 => Port 1 Interrupt enabled
		IEN2 |= (1 << 4);
		//-- set bit "1" to 1 => Port 2 Interrupt enabled
		IEN2 |= (1 << 1);
  } else {
    //-- Disable interrupts
  	EA = 0;
		//-- set bit "5" to 0 => Port 0 Interrupt disabled
		IEN1 &= ~(1 << 5);
		//-- set bit "4" to 0 => Port 1 Interrupt disabled
		IEN2 &= ~(1 << 4);
		//-- set bit "1" to 0 => Port 2 Interrupt disabled
		IEN2 &= ~(1 << 1);
  }
}

void resetGPIO(uint8 gpioPort, uint8 gpioBit)
{
	switch(gpioPort)
	{
		//-- Port #0
    case 0:
      //-- P0_0 ... P0_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
    		//-- set "gpioBit" to 0 => Clear the pin interrupt flag
				P0IFG &= ~(1 << gpioBit);
				P0IF = 0;
      }
      break;
		//-- Port #1
    case 1:
      //-- P1_0 ... P1_7
      if((gpioBit == 0 || gpioBit > 0) && gpioBit <= 7) {
    		//-- set "gpioBit" to 0 => Clear the pin interrupt flag
				P1IFG &= ~(1 << gpioBit);
				P1IF = 0;
      }
      break;
		//-- Port #2
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

void initInputGPIO(int gpioPort, int gpioBit, int gpioPullUpDn)
{
	switch(gpioPullUpDn)
	{
		case GPIO_PULL_UP:
			_CC2530_IOCTL(gpioPort, gpioBit, CC2530_INPUT_PULLUP);
			break;
		case GPIO_PULL_DOWN:
			_CC2530_IOCTL(gpioPort, gpioBit, CC2530_INPUT_PULLDOWN);
			break;
		case GPIO_PULL_TRISTATE:
			_CC2530_IOCTL(gpioPort, gpioBit, CC2530_INPUT_TRISTATE);
			break;
		default:
			break;
	}
}

void initOutputGPIO(int gpioPort, int gpioBit)
{
	_CC2530_IOCTL(gpioPort, gpioBit, CC2530_OUTPUT);
}

#pragma optimize=none
void delayUsIn32Mhz(uint16 nUs)
{
  for(uint16 i = 0; i < nUs; i++) {
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
void delayMs16MHZ(uint16 nMs)
{
	uint16 i, j;
  
  for(i = 0; i < nMs; i++) {
  	for(j = 0; j < 535; j++) {
  		//-- do nothing, just waiting...
  	};
  }
} 

#pragma optimize=none
void delayMs32MHZ(uint16 nMs)
{
	uint16 i, j;
  
  for(i = 0; i < nMs; i++) {
  	for(j = 0; j < 1070; j++) {
  		//-- do nothing, just waiting...
  	};
  }
} 

#pragma optimize=none
void delayMs(halDelaySysClk_t sysClk, uint16 nMs)
{
  uint16 i, j, loop1Ms;
  
  loop1Ms = (sysClk == SYSCLK_16MHZ) ? 535 : 1070;
  
  for(i = 0; i < nMs; i++) {
  	for(j = 0; j < loop1Ms; j++) {
  		//-- do nothing, just waiting...
  	};
  }
}

/*****************************************************************************
* Function name: DelayUS
* Function description: Use Time1 timer mode for precise positioning
*-----------------------------------------------------------------------------
* Parameter: k: Delay k us
* Return value: None
******************************************************************************/
void delayUS(unsigned int k)
{ 
  T1CC0L = 0x10;
  T1CC0H = 0x00;
  T1CCTL0 |= (1 << 2);  //The analog mode can only use channel 0
  T1CTL = 0x02; 
  //--16M per second = 16000K = 16000000 times
  while(k)
  {
      while( ( T1STAT & (1 << 0) ) != 1);
      T1STAT &= ~(1 << 0);
      k--;
  }
  T1CTL = 0x00; //Turn off the timer
}
//-- 10 us delay
void delay10US()
{
  delayUS(10);
}
//-- "Time" ms delay
void delayMS(uint Time)
{
  while(Time--)
  {
    delayUS(1000);
  }
}


int _CC2530_IOCTL_BV(int reg, int bit)
{
	reg |= (1 << bit);
	return reg;
}

void _CC2530_REGCFG_PxSEL(int port, int bit, int val)
{
  if(val == 0) {
  	if(port == 0) {
  		P0SEL &= ~(1 << bit);
  	}
  	if(port == 1) {
  		P1SEL &= ~(1 << bit);
  	}
  	if(port == 2) {
  		P2SEL &= ~(1 << bit);
  	}
  } else {
  	if(port == 0) {
  		P0SEL |= (1 << bit);
  	}
  	if(port == 1) {
  		P1SEL |= (1 << bit);
  	}
  	if(port == 2) {
  		P2SEL |= (1 << bit);
  	}
  }
}

void _CC2530_REGCFG_PxDIR(int port, int bit, int val)
{
  if(val == 0) {
  	if(port == 0) {
  		P0DIR &= ~(1 << bit);
  	}
  	if(port == 1) {
  		P1DIR &= ~(1 << bit);
  	}
  	if(port == 2) {
  		P2DIR &= ~(1 << bit);
  	}
  } else {
  	if(port == 0) {
  		P0DIR |= (1 << bit);
  	}
  	if(port == 1) {
  		P1DIR |= (1 << bit);
  	}
  	if(port == 2) {
  		P2DIR |= (1 << bit);
  	}
  }
}

void _CC2530_REGCFG_PxINP(int port, int bit, int val)
{
  if(val == 0) {
  	if(port == 0) {
  		P0INP &= ~(1 << bit);
  	}
  	if(port == 1) {
  		P1INP &= ~(1 << bit);
  	}
  	if(port == 2) {
  		P2INP &= ~(1 << bit);
  	}
  } else {
  	if(port == 0) {
  		P0INP |= (1 << bit);
  	}
  	if(port == 1) {
  		P1INP |= (1 << bit);
  	}
  	if(port == 2) {
  		P2INP |= (1 << bit);
  	}
  }
}

void _CC2530_IO_OUTPUT(int port, int pin)
{
  _CC2530_REGCFG_PxDIR(port , pin , 1);
  _CC2530_REGCFG_PxSEL(port , pin , 0);
}

void _CC2530_IO_INPUT(int port, int pin, int mode)
{
  if(port == 1 && (pin == 0 || pin == 1)) {
  	return;
  }

  _CC2530_REGCFG_PxDIR(port , pin , 0);
  _CC2530_REGCFG_PxSEL(port , pin , 0);

  if(mode == CC2530_INPUT_TRISTATE) {
  	_CC2530_REGCFG_PxINP(port , pin , 1);
  } else {
    _CC2530_REGCFG_PxINP(port , pin , 0);
    if(mode == CC2530_INPUT_PULLUP) {
    	_CC2530_REGCFG_PxINP(2 , (5 + port), 0);
    } else {
    	_CC2530_REGCFG_PxINP(2 , (5 + port), 1);
    }
  }
}

void _CC2530_IOCTL(int port, int pin, int mode)
{
  if(port > 2 || pin > 7) {
  	return;
  }

  if(mode == CC2530_OUTPUT) {
  	_CC2530_IO_OUTPUT(port, pin);
  } else {
  	_CC2530_IO_INPUT(port, pin, mode);
  }
}
