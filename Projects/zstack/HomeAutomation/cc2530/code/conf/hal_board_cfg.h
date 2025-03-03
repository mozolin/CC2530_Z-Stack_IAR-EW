#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H

#ifdef __cplusplus
extern "C" {
#endif


#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"



/************************
 *                      *
 *  LEDs configuration  *
 *                      *
 ************************/

//-- Number of LEDs
#define HAL_NUM_LEDS      3

//-- LED #1 on board (Green) - P1_0
#define LED1_BV           BV(0)
#define LED1_SBIT         P1_0
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_LOW

//-- LED #2 on board (Red) - P1_1
#define LED2_BV           BV(1)
#define LED2_SBIT         P1_1
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_LOW

//-- LED #3 on board (Green) - P1_4
#define LED3_BV           BV(4)
#define LED3_SBIT         P1_4
#define LED3_DDR          P1DIR
#define LED3_POLARITY     ACTIVE_LOW

#define LED4_BV
#define LED4_SBIT
#define LED4_DDR
#define LED4_POLARITY  
#define LED4_SET_DIR()


/**************************
 *                        *
 *  Buttons configuration  *
 *                        *
 **************************/

//-- BTN #1 - P0_1
#define PUSH1_BV          BV(1)
#define PUSH1_SBIT        P0_1
#define PUSH1_POLARITY    ACTIVE_LOW
#define PUSH1_PORT        P0
#define PUSH1_SEL         P0SEL
#define PUSH1_DIR         P0DIR
#define PUSH1_IEN         IEN1  //-- CPU interrupt mask register
#define PUSH1_IENBIT      BV(5) //-- Mask bit for all of Port_0
#define PUSH1_ICTL        P0IEN //-- Port Interrupt Control register
#define PUSH1_ICTLBIT     BV(1) //-- P0IEN - P0.1 enable/disable bit

//-- BTN #2 - P2_0
#define PUSH2_BV          BV(0)      //-- Bit 0 (pin number in port 2)
#define PUSH2_SBIT        P2_0       //-- GPIO
#define PUSH2_POLARITY    ACTIVE_LOW //-- State
#define PUSH2_PORT        P2         //-- Port number (P2)
#define PUSH2_SEL         P2SEL      //-- Port 2 Function Select (P2SEL)
#define PUSH2_DIR         P2DIR      //-- Port 2 Direction (P2DIR)
#define PUSH2_IEN         IEN2       //-- Port 2 Interrupt Mask (P2IEN)
#define PUSH2_IENBIT      BV(1)      //-- Mask bit for all of Port 2 (for IEN2)
#define PUSH2_ICTL        P2IEN      //-- Port Interrupt Control register (IEN2)
#define PUSH2_ICTLBIT     BV(0)      //-- Bit 0 for P2IEN: enable/disable Interrupts


//-- BTN #3 - P1_5

//-- Bit 5 (gpioBit, pin number in port 1)
//-- P1INP &= ~(1 << gpioBit); => Pullup or pulldown (0)
//-- P1INP |= (1 << gpioBit);  => 3-state (1)
//-- P1INP |= PUSH3_BV;        => set 3-state for bit 5
#define PUSH3_BV          BV(5)
//-- GPIO
//-- uses: HAL_PUSH_BUTTON3() (PUSH3_POLARITY(PUSH3_SBIT))
#define PUSH3_SBIT        P1_5
//-- State
//-- uses: HAL_PUSH_BUTTON3() (PUSH3_POLARITY(PUSH3_SBIT))
#define PUSH3_POLARITY    ACTIVE_LOW
//-- Port number (P1)
//-- uses: ???
#define PUSH3_PORT        P1
//-- Port 1 Function Select (P1SEL)
//-- P1SEL &= ~(1 << gpioBit); => GPIO_FUNC_GENERAL_PURPOSE (0)
//-- P1SEL |= (1 << gpioBit);  => GPIO_FUNC_PERIPHERAL (1)
//-- PUSH3_SEL &= ~(PUSH3_BV); => set general-purpose finction for pin 5
#define PUSH3_SEL         P1SEL
//-- Port 1 Direction (P1DIR)
//-- P1DIR &= ~(1 << gpioBit); => GPIO_DIR_INPUT (0)
//-- P1DIR |= (1 << gpioBit);  => GPIO_DIR_OUTPUT (1)
//-- PUSH3_DIR &= ~(PUSH3_BV); => set Input for bit 5
#define PUSH3_DIR         P1DIR
//-- Bit 5 for all of Port 1 (for IEN1)
#define PUSH3_IENBIT      BV(5)
//-- Port 1 Interrupt Mask (P1IEN)
//-- P1IEN &= ~(1 << gpioBit); => Interrupts are disabled (0)
//-- P1IEN |= (1 << gpioBit);  => Interrupts are enabled (1)
//-- PUSH3_IEN &= ~(PUSH3_IENBIT); => Clear interrupt enable for bit 5
#define PUSH3_IEN         IEN1
//-- Bit 4 for P2IEN: enable/disable Interrupts
#define PUSH3_ICTLBIT     BV(4)
//-- Port Interrupt Control register (IEN1)
//-- IEN2 &= ~(1 << 4);        => Port 1 Interrupt disabled (0)
//-- IEN2 |= (1 << 4);         => Port 1 Interrupt enabled (1)
//-- PUSH3_ICTL &= ~(PUSH3_ICTLBIT); => Don't generate interrupt for port 1
#define PUSH3_ICTL        P1IEN


//-- external buttons initialization function
extern void cc2530_HalKeyPoll(void);
extern void cc2530_HalKeyInit(void);

//-- external relays initialization function
extern void cc2530_HalRelayInit(void);

//-- Initialization of equipment (for modules with amplifier cc2590, cc2591)
#define HAL_BOARD_INIT()                                 \
{                                                        \
  uint16 i;                                              \
                                                         \
  /* turn on 16MHz RC and 32MHz XOSC */                  \
  SLEEPCMD &= ~OSC_PD;                                   \
  /* wait for 32MHz XOSC stable */                       \
  while (!(SLEEPSTA & XOSC_STB));                        \
  /* chip bug workaround */                              \
  asm("NOP");                                            \
  /* Require 63us delay for all revs */                  \
  for (i=0; i<504; i++) asm("NOP");                      \
  /* Select 32MHz XOSC and the source for 32K clock */   \
  CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ);             \
  /* Wait for the change to be effective */              \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ));    \
  /* turn off 16MHz RC */                                \
  SLEEPCMD |= OSC_PD;                                    \
                                                         \
  /* Turn on cache prefetch mode */                      \
  PREFETCH_ENABLE();                                     \
                                                         \
  /* set direction for GPIO outputs  */                  \
  HAL_TURN_OFF_LED1();                                   \
  LED1_DDR |= LED1_BV;                                   \
  HAL_TURN_OFF_LED2();                                   \
  LED2_DDR |= LED2_BV;                                   \
  HAL_TURN_OFF_LED3();                                   \
  LED3_DDR |= LED3_BV;                                   \
  HAL_TURN_OFF_LED4();                                   \
  LED4_SET_DIR();                                        \
                                                         \
  /* Set PA/LNA HGM control P0_7 */                      \
  /*P0DIR |= BV(7);*/                                        \
                                                         \
  /* configure tristates */                              \
  P0INP |= PUSH2_BV;                                     \
                                                         \
  /* init key & relay handlers */                        \
  cc2530_HalKeyInit();                                   \
  cc2530_HalRelayInit();                                 \
                                                         \
  /* setup RF frontend if necessary */                   \
  HAL_BOARD_RF_FRONTEND_SETUP();                         \
}


//-- Macros for buttons control
#define HAL_PUSH_BUTTON1()        (PUSH1_POLARITY(PUSH1_SBIT))
#define HAL_PUSH_BUTTON2()        (PUSH2_POLARITY(PUSH2_SBIT))
#define HAL_PUSH_BUTTON3()        (PUSH3_POLARITY(PUSH3_SBIT))
#define HAL_PUSH_BUTTON4()        (0)
#define HAL_PUSH_BUTTON5()        (0)
#define HAL_PUSH_BUTTON6()        (0)

//-- Macros for LEDs control
#define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
#define HAL_TURN_OFF_LED2()       st( LED2_SBIT = LED2_POLARITY (0); )
#define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
#define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

#define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
#define HAL_TURN_ON_LED2()        st( LED2_SBIT = LED2_POLARITY (1); )
#define HAL_TURN_ON_LED3()        st( LED3_SBIT = LED3_POLARITY (1); )
#define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

#define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
#define HAL_TOGGLE_LED2()         st( if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} )
#define HAL_TOGGLE_LED3()         st( if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} )
#define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

#define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
#define HAL_STATE_LED2()          (LED2_POLARITY (LED2_SBIT))
#define HAL_STATE_LED3()          (LED3_POLARITY (LED3_SBIT))
#define HAL_STATE_LED4()          HAL_STATE_LED1()
      

/***************************
 *                         *
 *  Drivers configuration  *
 *                         *
 ***************************/

//-- Hardware timers
#ifndef HAL_TIMER
  #define HAL_TIMER FALSE
#endif

//-- Using ADC
#ifndef HAL_ADC
  #define HAL_ADC TRUE
#endif

//-- Using DMA
#ifndef HAL_DMA
  #define HAL_DMA TRUE
#endif

//-- Using Flash
#ifndef HAL_FLASH
  #define HAL_FLASH TRUE
#endif

//-- Using AES
#ifndef HAL_AES
  #define HAL_AES TRUE
#endif

#ifndef HAL_AES_DMA
  #define HAL_AES_DMA TRUE
#endif

//-- Using LCD
#define HAL_LCD FALSE

//-- Using LEDs
#ifndef HAL_LED
  #define HAL_LED TRUE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
  #define BLINK_LEDS
#endif

//-- Using buttons
#define HAL_KEY FALSE

//-- Using UART
#ifndef HAL_UART
	#if (defined ZAPP_P1) || (defined ZAPP_P2) || (defined ZTOOL_P1) || (defined ZTOOL_P2)
		#define HAL_UART TRUE
	#else
		#define HAL_UART FALSE
	#endif
#endif

#if HAL_UART
	#ifndef HAL_UART_DMA
		#if HAL_DMA
			#if (defined ZAPP_P2) || (defined ZTOOL_P2)
				#define HAL_UART_DMA  2
			#else
				#define HAL_UART_DMA  1
			#endif
		#else
			#define HAL_UART_DMA  0
		#endif
	#endif

	#ifndef HAL_UART_ISR
		#if HAL_UART_DMA           // Default preference for DMA over ISR.
			#define HAL_UART_ISR  0
		#elif (defined ZAPP_P2) || (defined ZTOOL_P2)
			#define HAL_UART_ISR  2
		#else
			#define HAL_UART_ISR  1
		#endif
	#endif

	#if (HAL_UART_DMA && (HAL_UART_DMA == HAL_UART_ISR))
		#error HAL_UART_DMA & HAL_UART_ISR must be different.
	#endif

	// Used to set P2 priority - USART0 over USART1 if both are defined.
	#if ((HAL_UART_DMA == 1) || (HAL_UART_ISR == 1))
		#define HAL_UART_PRIPO             0x00
	#else
		#define HAL_UART_PRIPO             0x40
	#endif

#else
	#define HAL_UART_DMA  0
	#define HAL_UART_ISR  0
#endif

//-- USB is not used for cc2530 configuration
#define HAL_UART_USB  0


#ifndef HAL_OLED 
 #define HAL_OLED TRUE 
#endif



#ifdef __cplusplus
}
#endif

#endif //-- HAL_BOARD_CFG_H
