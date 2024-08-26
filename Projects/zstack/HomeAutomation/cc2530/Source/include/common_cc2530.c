#include <stdio.h>
#include "colors.h"
#include "common_cc2530.h"
#include "cc2530_io_ports.h"
//#include "cc2530_ioctl.h"

#define LED       P0_4
#define BUTTON    P0_7

void initLed(void);
void initButton(void);
__interrupt void buttonISR(void);

void cc2530_init()
{
  initLed();
  initButton();
}

//-- Interrupt from key was pressed
#pragma vector = P0INT_VECTOR
__interrupt void buttonISR(void)
{ 
  delayMs(10);

  if(BUTTON == BUTTON_DOWN) {
    LED = (LED == LED_ON)? LED_OFF : LED_ON;
  }
  printf("common_cc2530.c: Key -> %d, LED -> %d\r\n", BUTTON, LED);

  resetGPIO(GPIO_PORT_0, GPIO_PIN_7);
} 

void initLed()
{
	initShortGPIO(GPIO_PORT_0, GPIO_PIN_4, GPIO_DIR_OUTPUT);
  
  LED = LED_OFF;
  //printf("common_cc2530.c: initLed()\r\n");
}

void initButton(void)
{
	initGPIO(
		GPIO_PORT_0,
		GPIO_PIN_7,
		GPIO_GENERAL_PURPOSE,
		GPIO_DIR_INPUT,
		GPIO_PULL_UP,
		GPIO_INTERRUPT_ENABLED,
		GPIO_INTERRUPT_CTRL_FALL
	);
}

