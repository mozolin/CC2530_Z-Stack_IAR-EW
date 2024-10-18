#include <stdio.h>
#include "colors.h"
#include "common_cc2530.h"
#include "cc2530_io_ports.h"
//#include "cc2530_ioctl.h"

#define LED        P0_4
#define BUTTON     P0_2
#define BIT        2

void initLed(void);
void initButton(void);
__interrupt void buttonISR(void);

void cc2530_init()
{
  #if DEBUG_PRINT_UART
  	//printf("LED|BUTTON -> %d|%d\n", LED, BUTTON);
  #endif
  
  //initLed();
  //initButton();
}

//-- Interrupt from key was pressed
#pragma vector = P0INT_VECTOR
__interrupt void buttonISR(void)
{ 
  delayMs(10);

  if(BUTTON == BUTTON_DOWN) {
    LED = (LED == LED_ON)? LED_OFF : LED_ON;
  }

  resetGPIO(GPIO_PORT_0, GPIO_PIN_2);
} 

void initLed()
{
	initOutputGPIO(GPIO_PORT_0, GPIO_PIN_4);
  LED = LED_OFF;
}

void initButton(void)
{
  initInputGPIO(GPIO_PORT_1, GPIO_PIN_5, GPIO_PULL_UP);
}
