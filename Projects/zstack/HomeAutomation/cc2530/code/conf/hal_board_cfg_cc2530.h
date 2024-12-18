#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"

#define xHAL_PA_LNA

//-- Частота процессора
#define HAL_CPU_CLOCK_MHZ     32

//-- Флаг наличия кварца
#ifdef IS_CLOCK_CRYSTAL
  #define HAL_CLOCK_CRYSTAL
#else
  #define OSC32K_CRYSTAL_INSTALLED FALSE
#endif

//-- 32 kHz clock source select in CLKCONCMD
#if !defined (OSC32K_CRYSTAL_INSTALLED) || (defined (OSC32K_CRYSTAL_INSTALLED) && (OSC32K_CRYSTAL_INSTALLED == TRUE))
  //-- external 32 KHz xosc
  #define OSC_32KHZ  0x00
#else
  //-- internal 32 KHz rcosc
  #define OSC_32KHZ  0x80
#endif

#define HAL_CLOCK_STABLE()    st( while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); )


// Конфигурация светодиодов

// Количество светодиодов
#define HAL_NUM_LEDS            3

#define HAL_LED_BLINK_DELAY()   st( { volatile uint32 i; for (i=0; i<0x5800; i++) { }; } )

//-- LED #1 - P1_0 Зеленый
#define LED1_BV           BV(0)
#define LED1_SBIT         P1_0
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_LOW

//-- LED #2 - P1_1 Красный
#define LED2_BV           BV(1)
#define LED2_SBIT         P1_1
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_LOW

//-- LED #3 - P1_4 Зеленый
#define LED3_BV           BV(4)
#define LED3_SBIT         P1_4
#define LED3_DDR          P1DIR
#define LED3_POLARITY     ACTIVE_LOW

#define LED4_BV
#define LED4_SBIT
#define LED4_DDR
#define LED4_POLARITY  
#define LED4_SET_DIR()

// Конфигурация кнопок

#define ACTIVE_LOW        !
//-- double negation forces result to be '1'
#define ACTIVE_HIGH       !!

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


/*
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
*/

// OSAL NV - постоянная внутренняя flash-память

// Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK    16
// Flash is constructed of 128 pages of 2 KB.
#define HAL_FLASH_PAGE_SIZE        2048
#define HAL_FLASH_WORD_SIZE        4

// CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP         0x8000

// The last 16 bytes of the last available page are reserved for flash lock bits.
// NV page definitions must coincide with segment declaration in project *.xcl file.
#if defined NON_BANKED
  #define HAL_FLASH_LOCK_BITS        16
  #define HAL_NV_PAGE_END            30
  #define HAL_NV_PAGE_CNT            2
#else
  #define HAL_FLASH_LOCK_BITS        16
  #define HAL_NV_PAGE_END            126
  #define HAL_NV_PAGE_CNT            6
#endif

// Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE        8
#define HAL_FLASH_IEEE_PAGE       (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET       (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET        0xC

#define HAL_FLASH_DEV_PRIVATE_KEY_OSET     0x7D2
#define HAL_FLASH_CA_PUBLIC_KEY_OSET       0x7BC
#define HAL_FLASH_IMPLICIT_CERT_OSET       0x78C

#define HAL_NV_PAGE_BEG           (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)

// Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH              0
#define HAL_DMA_CH_RX              3
#define HAL_DMA_CH_TX              4

#define HAL_NV_DMA_GET_DESC()      HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a)     HAL_DMA_SET_ADDR_DESC0((a))


/* ------------------------------------------------------------------------------------------------
 *  Serial Boot Loader: reserving the first 4 pages of flash and other memory in cc2530-sb.xcl.
 * ------------------------------------------------------------------------------------------------
 */

#define HAL_SB_IMG_ADDR       0x2000
#define HAL_SB_CRC_ADDR       0x2090
// Size of internal flash less 4 pages for boot loader, 6 pages for NV, & 1 page for lock bits.
#define HAL_SB_IMG_SIZE      (0x40000 - 0x2000 - 0x3000 - 0x0800)

   
// Инициализация RF-frontend усилителя
#if defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590 || \
    defined HAL_PA_LNA_SE2431L || defined HAL_PA_LNA_CC2592
  extern void MAC_RfFrontendSetup(void);
  #define HAL_BOARD_RF_FRONTEND_SETUP() MAC_RfFrontendSetup()
#else
  #define HAL_BOARD_RF_FRONTEND_SETUP()
#endif

//----------- Cache Prefetch control ----------
#define PREFETCH_ENABLE()     st( FCTL = 0x08; )
#define PREFETCH_DISABLE()    st( FCTL = 0x04; )

//-- external buttons initialization function
extern void cc2530_HalKeyPoll(void);
extern void cc2530_HalKeyInit(void);

//-- external relays initialization function
extern void cc2530_HalRelayInit(void);

// Инициализация оборудования (для модулей с усилителем cc2590, cc2591)
#define HAL_BOARD_INIT()                                         \
{                                                                \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                       /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));            /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");          /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                        /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  /* set direction for GPIO outputs  */                          \
  HAL_TURN_OFF_LED1();                                           \
  LED1_DDR |= LED1_BV;                                           \
  HAL_TURN_OFF_LED2();                                           \
  LED2_DDR |= LED2_BV;                                           \
  HAL_TURN_OFF_LED3();                                           \
  LED3_DDR |= LED3_BV;                                           \
  HAL_TURN_OFF_LED4();                                           \
  LED4_SET_DIR();                                                \
                                                                 \
  /*LED3_DDR |= LED3_BV;*/                                           \
                                                                 \
  /* Set PA/LNA HGM control P0_7 */                              \
  P0DIR |= BV(7);                                                \
                                                                 \
  /* configure tristates */                                      \
  P0INP |= PUSH2_BV;                                             \
  cc2530_HalKeyInit();                                           \
                                                                 \
  cc2530_HalRelayInit();                                         \
                                                                 \
  /* setup RF frontend if necessary */                           \
  HAL_BOARD_RF_FRONTEND_SETUP();                                 \
}

// Макрос защиты от дребезга контактов
#define HAL_DEBOUNCE(expr)    { int i; for (i=0; i<500; i++) { if (!(expr)) i = 0; } }

// Макросы для проверки кнопок
#define HAL_PUSH_BUTTON1()        (PUSH1_POLARITY(PUSH1_SBIT))
#define HAL_PUSH_BUTTON2()        (PUSH2_POLARITY(PUSH2_SBIT))
#define HAL_PUSH_BUTTON3()        (0)
//#define HAL_PUSH_BUTTON3()        (PUSH3_POLARITY(PUSH3_SBIT))
#define HAL_PUSH_BUTTON4()        (0)
#define HAL_PUSH_BUTTON5()        (0)
#define HAL_PUSH_BUTTON6()        (0)

// Макросы для управления светодиодами
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
      

/* ----------- XNV ---------- */
#define XNV_SPI_BEGIN()             st(P1_3 = 0;)
#define XNV_SPI_TX(x)               st(U1CSR &= ~0x02; U1DBUF = (x);)
#define XNV_SPI_RX()                U1DBUF
#define XNV_SPI_WAIT_RXRDY()        st(while (!(U1CSR & 0x02));)
#define XNV_SPI_END()               st(P1_3 = 1;)

// The TI reference design uses UART1 Alt. 2 in SPI mode.
#define XNV_SPI_INIT() \
st( \
  /* Mode select UART1 SPI Mode as master. */\
  U1CSR = 0; \
  \
  /* Setup for 115200 baud. */\
  U1GCR = 11; \
  U1BAUD = 216; \
  \
  /* Set bit order to MSB */\
  U1GCR |= BV(5); \
  \
  /* Set UART1 I/O to alternate 2 location on P1 pins. */\
  PERCFG |= 0x02;  /* U1CFG */\
  \
  /* Select peripheral function on I/O pins but SS is left as GPIO for separate control. */\
  P1SEL |= 0xE0;  /* SELP1_[7:4] */\
  /* P1.1,2,3: reset, LCD CS, XNV CS. */\
  P1SEL &= ~0x0E; \
  P1 |= 0x0E; \
  P1_1 = 0; \
  P1DIR |= 0x0E; \
  \
  /* Give UART1 priority over Timer3. */\
  P2SEL &= ~0x20;  /* PRI2P1 */\
  \
  /* When SPI config is complete, enable it. */\
  U1CSR |= 0x40; \
  /* Release XNV reset. */\
  P1_1 = 1; \
)

/* ----------- Minimum safe bus voltage ---------- */

// Vdd/3 / Internal Reference X ENOB --> (Vdd / 3) / 1.15 X 127
#define VDD_2_0  74   // 2.0 V required to safely read/write internal flash.
#define VDD_2_7  100  // 2.7 V required for the Numonyx device.
#define VDD_MIN_RUN  (VDD_2_0+4)  // VDD_MIN_RUN = VDD_MIN_NV
#define VDD_MIN_NV   (VDD_2_0+4)  // 5% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_GOOD (VDD_2_0+8)  // 10% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_XNV  (VDD_2_7+5)  // 5% margin over minimum to survive a page erase and compaction.


// Конфигурация драйвера

// Аппаратные таймеры
#ifndef HAL_TIMER
  #define HAL_TIMER FALSE
#endif

// Использование ADC
#ifndef HAL_ADC
  #define HAL_ADC TRUE
#endif

// Использование DMA
#ifndef HAL_DMA
  #define HAL_DMA TRUE
#endif

// Использование Flash
#ifndef HAL_FLASH
  #define HAL_FLASH TRUE
#endif

// Использование AES
#ifndef HAL_AES
  #define HAL_AES TRUE
#endif

#ifndef HAL_AES_DMA
  #define HAL_AES_DMA TRUE
#endif

// Использование LCD
#define HAL_LCD FALSE

// Использование светодиодов
#ifndef HAL_LED
  #define HAL_LED TRUE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
  #define BLINK_LEDS
#endif

// Использование кнопок
#define HAL_KEY FALSE

// Использование UART
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

/* USB is not used for cc2530 configuration */
#define HAL_UART_USB  0
#endif


#ifndef HAL_OLED 
 #define HAL_OLED TRUE 
#endif
