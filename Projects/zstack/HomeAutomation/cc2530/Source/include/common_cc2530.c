#include <stdio.h>
#include "colors.h"
#include "common_cc2530.h"
#include "cc2530_io_ports.h"
#include "cc2530_ioctl.h"

#define LED        P0_4
#define BUTTON1    P0_2
#define BIT1       2
#define BUTTON2    P1_5
#define BIT2       5

void initLed(void);
void initButton(void);
__interrupt void buttonISR(void);

void cc2530_init()
{
  initLed();
  //initButton();
}

//-- Interrupt from key was pressed
#pragma vector = P0INT_VECTOR
__interrupt void buttonISR(void)
{ 
  delayMs(10);

  if(BUTTON1 == BUTTON_DOWN) {
    LED = (LED == LED_ON)? LED_OFF : LED_ON;
    printf("common_cc2530.c: Key1, LED -> %d\n", LED);
  }
  if(BUTTON2 == BUTTON_DOWN) {
    LED = (LED == LED_ON)? LED_OFF : LED_ON;
    printf("common_cc2530.c: Key2, LED -> %d\n", LED);
  }

  //resetGPIO(GPIO_PORT_0, GPIO_PIN_2);
  resetGPIO(GPIO_PORT_1, GPIO_PIN_2);

  /*
  P0IFG &= ~(1 << BIT1); // P0_BIT1: Port 0 Interrupt Status Flag
  P0IF  = 0;             // P0: Clear the interrupt status flag
  
  P1IFG &= ~(1 << BIT2); // P1_BIT2: Port 1 Interrupt Status Flag
  P1IF = 0; 	           // P1: Clear the interrupt status flag
  */
} 

void initLed()
{
	initOutputGPIO(GPIO_PORT_0, GPIO_PIN_4);
  
  LED = LED_OFF;
  //printf("common_cc2530.c: initLed() -> %d\n", LED);
}

void initButton(void)
{
	/*
	//#define BUTTON_PORT     0       //!< Button port.
	//#define BUTTON_PIN      1       //!< Button pin.
	//#define BUTTON          P0_1    //!< Button GPIO.
	//#define BUTTON_NORMAL   1 //!< normal state.
	//#define BUTTON_DOWN     0 //!< press-down.
	P0SEL &= ~(1<<1);  // P0_1: General-purpose I/O
  P0DIR &= ~(1<<1);  // P0_1: Input
  P0INP &= ~(1<<1);  // P0_1: Pullup or Pulldown mode
  P2INP &= ~(1<<5);  // Pullup

  PICTL |= 0x01;     // Falling edge on input gives interrupt.
  P0IFG = 0x00; 	   // Clear the pin interrupt flag
  P0IEN |= (1<<1);   // Enable P0.1 interrupt
  P0IF = 0x00; 	     // Clear the port0 interrupt status flag
  IEN1 |= 0x20; 	   // Enable P0 interrupt
  EA = 1; 		       // enable interrupts
  */

  /*
  PICTL |= (1 << 0);      // Falling edge on input gives interrupt.
  P0IFG &= ~(1 << BIT1);  // Clear the pin interrupt flag
  P0IEN |= (1 << BIT1);   // Enable P0.BIT1 interrupt
  P0IF = 0;               // Clear the port0 interrupt status flag
  IEN1 |= (1 << 5);       // Enable P0 interrupt
  EA = 1; 		            // enable interrupts
  */

  /*
  P1SEL &= ~(1<<5);   // P1_5: General-purpose I/O
  P1DIR &= ~(1<<5);   // P1_5: Input
  P1INP &= ~(1<<5);   // P1_5: Pullup or Pulldown mode
  P2INP &= ~(1<<6);   // P1: Pullup
  */

  /*
  PICTL |= (1 << 1);     // P1_BIT2: Falling edge on input gives interrupt
  P1IFG &= ~(1 << BIT2); // P1_BIT2: Port 1 Interrupt Status Flag
  P1IEN |= (1 << BIT2);  // P1_BIT2: Enable P1.BIT2 interrupt
  P1IF = 0; 	           // P1: Clear the interrupt status flag
  IEN2 |= (1 << 4);      // P1: Enable interrupt
  EA = 1; 		           // Enable interrupts
  */

  setInterrupts(true);
  initGPIO(
		GPIO_PORT_1,
		GPIO_PIN_5,
		GPIO_FUNC_GENERAL_PURPOSE,
		GPIO_DIR_INPUT,
		GPIO_PULL_UP,
		GPIO_INTERRUPT_ENABLED,
		GPIO_INTERRUPT_CTRL_FALL
	);

  //CC2530_IOCTL(1, 5, CC2530_INPUT_PULLUP);

	/*
	initGPIO(
		GPIO_PORT_1,
		GPIO_PIN_5,
		GPIO_FUNC_GENERAL_PURPOSE,
		GPIO_DIR_INPUT,
		GPIO_PULL_UP,
		GPIO_INTERRUPT_ENABLED,
		GPIO_INTERRUPT_CTRL_FALL
	);
	*/

	/*
	setInterrupts(true);

  initGPIO(
		GPIO_PORT_1,
		GPIO_PIN_5,
		GPIO_FUNC_GENERAL_PURPOSE,
		GPIO_DIR_INPUT,
		GPIO_PULL_UP,
		GPIO_INTERRUPT_ENABLED,
		GPIO_INTERRUPT_CTRL_FALL
	);
	*/
}

