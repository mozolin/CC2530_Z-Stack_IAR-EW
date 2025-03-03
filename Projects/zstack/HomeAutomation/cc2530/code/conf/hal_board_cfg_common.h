#ifndef HAL_BOARD_CFG_COMMON_H
#define HAL_BOARD_CFG_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif


#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"


/**************************
 *                        *
 *  System configuration  *
 *                        *
 **************************/

#define xHAL_PA_LNA

//-- Processor frequency
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


/************************
 *                      *
 *  LEDs configuration  *
 *                      *
 ************************/

//-- LED flashing delay
#define HAL_LED_BLINK_DELAY()   st( { volatile uint32 i; for (i=0; i<0x5800; i++) { }; } )


/**************************
 *                        *
 *  Buttons configuration  *
 *                        *
 **************************/

#define ACTIVE_LOW        !
//-- double negation forces result to be '1'
#define ACTIVE_HIGH       !!

//-- Макрос защиты от дребезга контактов
#define HAL_DEBOUNCE(expr)    { int i; for (i=0; i<500; i++) { if (!(expr)) i = 0; } }


/************************************************
 *                                              *
 *  OSAL NV - permanent internal flash memory   *
 *                                              *
 ************************************************/

//-- Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK    16
//-- Flash is constructed of 128 pages of 2 KB.
#define HAL_FLASH_PAGE_SIZE        2048
#define HAL_FLASH_WORD_SIZE        4

//-- CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP         0x8000

//-- The last 16 bytes of the last available page are reserved for flash lock bits.
//-- NV page definitions must coincide with segment declaration in project *.xcl file.
#if defined NON_BANKED
  #define HAL_FLASH_LOCK_BITS        16
  #define HAL_NV_PAGE_END            30
  #define HAL_NV_PAGE_CNT            2
#else
  #define HAL_FLASH_LOCK_BITS        16
  #define HAL_NV_PAGE_END            126
  #define HAL_NV_PAGE_CNT            6
#endif

//-- Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE        8
#define HAL_FLASH_IEEE_PAGE       (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET       (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET        0xC

#define HAL_FLASH_DEV_PRIVATE_KEY_OSET     0x7D2
#define HAL_FLASH_CA_PUBLIC_KEY_OSET       0x7BC
#define HAL_FLASH_IMPLICIT_CERT_OSET       0x78C

#define HAL_NV_PAGE_BEG           (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)

//-- Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH              0
#define HAL_DMA_CH_RX              3
#define HAL_DMA_CH_TX              4

#define HAL_NV_DMA_GET_DESC()      HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a)     HAL_DMA_SET_ADDR_DESC0((a))


/**************************************************************
 *                                                            *
 *  Serial Boot Loader: reserving the first 4 pages of flash  *
 *  and other memory in cc2530-sb.xcl.                        *
 *                                                            *
 **************************************************************/

#define HAL_SB_IMG_ADDR       0x2000
#define HAL_SB_CRC_ADDR       0x2090
//-- Size of internal flash less 4 pages for boot loader, 6 pages for NV, & 1 page for lock bits.
#define HAL_SB_IMG_SIZE      (0x40000 - 0x2000 - 0x3000 - 0x0800)

   
//-- Инициализация RF-frontend усилителя
#if defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590 || \
    defined HAL_PA_LNA_SE2431L || defined HAL_PA_LNA_CC2592
  extern void MAC_RfFrontendSetup(void);
  #define HAL_BOARD_RF_FRONTEND_SETUP() MAC_RfFrontendSetup()
#else
  #define HAL_BOARD_RF_FRONTEND_SETUP()
#endif

//-- Cache Prefetch control
#define PREFETCH_ENABLE()     st( FCTL = 0x08; )
#define PREFETCH_DISABLE()    st( FCTL = 0x04; )


/* ----------- XNV ---------- */
#define XNV_SPI_BEGIN()       st(P1_3 = 0;)
#define XNV_SPI_TX(x)         st(U1CSR &= ~0x02; U1DBUF = (x);)
#define XNV_SPI_RX()          U1DBUF
#define XNV_SPI_WAIT_RXRDY()  st(while (!(U1CSR & 0x02));)
#define XNV_SPI_END()         st(P1_3 = 1;)

//-- The TI reference design uses UART1 Alt. 2 in SPI mode.
#define XNV_SPI_INIT()                            \
st(                                               \
  /* Mode select UART1 SPI Mode as master. */     \
  U1CSR = 0;                                      \
                                                  \
  /* Setup for 115200 baud. */                    \
  U1GCR = 11;                                     \
  U1BAUD = 216;                                   \
                                                  \
  /* Set bit order to MSB */                      \
  U1GCR |= BV(5);                                 \
                                                  \
  /* Set UART1 I/O to alternate 2 location */     \
  /* on P1 pins. */                               \
  PERCFG |= 0x02;  /* U1CFG */                    \
                                                  \
  /* Select peripheral function on I/O pins */    \
  /* but SS is left as GPIO for separate */       \
  /* control. */                                  \
  P1SEL |= 0xE0;  /* SELP1_[7:4] */               \
  /* P1.1,2,3: reset, LCD CS, XNV CS. */          \
  P1SEL &= ~0x0E;                                 \
  P1 |= 0x0E;                                     \
  P1_1 = 0;                                       \
  P1DIR |= 0x0E;                                  \
                                                  \
  /* Give UART1 priority over Timer3. */          \
  P2SEL &= ~0x20;  /* PRI2P1 */                   \
                                                  \
  /* When SPI config is complete, enable it. */   \
  U1CSR |= 0x40;                                  \
  /* Release XNV reset. */                        \
  P1_1 = 1;                                       \
)

/* ----------- Minimum safe bus voltage ---------- */

//-- Vdd/3 / Internal Reference X ENOB --> (Vdd / 3) / 1.15 X 127
//-- 2.0 V required to safely read/write internal flash.
#define VDD_2_0  74
//-- 2.7 V required for the Numonyx device.
#define VDD_2_7  100
//-- VDD_MIN_RUN = VDD_MIN_NV
#define VDD_MIN_RUN  (VDD_2_0+4)
//-- 5% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_NV   (VDD_2_0+4)
//-- 10% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_GOOD (VDD_2_0+8)
//-- 5% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_XNV  (VDD_2_7+5)



#ifdef __cplusplus
}
#endif

#endif //-- HAL_BOARD_CFG_COMMON_H
