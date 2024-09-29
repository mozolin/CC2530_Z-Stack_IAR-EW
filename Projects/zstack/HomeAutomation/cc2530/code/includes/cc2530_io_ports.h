#ifndef CC2530_IO_PORTS_H
#define CC2530_IO_PORTS_H

#include <ioCC2530.h>

#define CC2530_IOCTL_REVISION   1

#ifdef __cplusplus
extern "C" {
#endif


#define CC2530_OUTPUT          0 //!< Output.
#define CC2530_INPUT_PULLUP    1 //!< pullup input.
#define CC2530_INPUT_PULLDOWN  2 //!< pulldown input.
#define CC2530_INPUT_TRISTATE  3 //!< 3-state.


typedef enum { SYSCLK_16MHZ, SYSCLK_32MHZ } halDelaySysClk_t;

/*****************************
 * HAL_BOARD_ELEMENTS:       *
 * ~~~~~~~~~~~~~~~~~~~       *
 *   - LED #1 - P1_0 Green   *
 *   - LED #2 - P1_1 Red     *
 *   - LED #3 - P1_4 Green   *
 *   - BTN #1 - P0_1         *
 *   - BTN #2 - P2_0         *
 *****************************/

//-- Port number
#define GPIO_PORT_0                  0
#define GPIO_PORT_1                  1
#define GPIO_PORT_2                  2

//-- Pin number
#define GPIO_PIN_0                   0
#define GPIO_PIN_1                   1
#define GPIO_PIN_2                   2
#define GPIO_PIN_3                   3
#define GPIO_PIN_4                   4
#define GPIO_PIN_5                   5
#define GPIO_PIN_6                   6
#define GPIO_PIN_7                   7

//-- Function: General purpose or not
#define GPIO_FUNC_GENERAL_PURPOSE    0
#define GPIO_FUNC_PERIPHERAL         1
#define GPIO_FUNC_NONE               -1

//-- Input or output
#define GPIO_DIR_INPUT               0
#define GPIO_DIR_OUTPUT              1
                                     
//-- Pullup, pulldown, 3-state button or none
#define GPIO_PULL_UP                 0
#define GPIO_PULL_DOWN               1
#define GPIO_PULL_TRISTATE           2
#define GPIO_PULL_NONE               -1

//-- Interrupt enabled/disabled
#define GPIO_INTERRUPT_DISABLED      0
#define GPIO_INTERRUPT_ENABLED       1
#define GPIO_INTERRUPT_NONE          -1
                                     
//-- Rising edge on input gives interrupt, Falling edge or None
#define GPIO_INTERRUPT_CTRL_RISE     0
#define GPIO_INTERRUPT_CTRL_FALL     1
#define GPIO_INTERRUPT_CTRL_NONE     -1

//-- Led state
#define LED_OFF         0 //-- off
#define LED_ON          1 //-- on

//-- Button state
#define BUTTON_DOWN     0 //-- press down
#define BUTTON_NORMAL   1 //-- normal state


void initGPIO(
	uint8 gpioPort,
	uint8 gpioBit,
	int8  gpioGeneralPurpose,
	bool  gpioInput,
	int8 gpioPullUpDn,
	int8 gpioIntrEnable,
	int8 gpioIntrCtrl
);

void setInterrupts(bool isEnabled);

void resetGPIO(uint8 gpioPort, uint8 gpioBit);

void initOutputGPIO(int gpioPort, int gpioBit);
void initInputGPIO(int gpioPort, int gpioBit, int gpioPullUpDn);


void delayUsIn32Mhz(uint16 nUs);
void delayMs16MHZ(uint16 nMs);
void delayMs32MHZ(uint16 nMs);
void delayMs(halDelaySysClk_t sysClk, uint16 nMs);


int _CC2530_IOCTL_BV(int reg, int bit);
void _CC2530_REGCFG_PxSEL(int port, int bit, int val);
void _CC2530_REGCFG_PxDIR(int port, int bit, int val);
void _CC2530_REGCFG_PxINP(int port, int bit, int val);
void _CC2530_IO_OUTPUT(int port, int pin);
void _CC2530_IO_INPUT(int port, int pin, int mode);
void _CC2530_IOCTL(int port, int pin, int mode);


/** @brief  Left shift of bit. */
#define CC2530_IOCTL_BV(x)      (1<<(x))
  
/** @brief  Config register: PxSEL. */ 
#define CC2530_REGCFG_PxSEL(port, pin, val) do {        \
    if(val == 0) P##port##SEL &= ~CC2530_IOCTL_BV(pin); \
    else P##port##SEL |= CC2530_IOCTL_BV(pin);          \
} while(0)
 
/** @brief  Config register: PxDIR. */ 
#define CC2530_REGCFG_PxDIR(port, pin, val) do {        \
    if(val == 0) P##port##DIR &= ~CC2530_IOCTL_BV(pin); \
    else P##port##DIR |= CC2530_IOCTL_BV(pin);          \
} while(0)

/** @brief  Config register: PxINP. */
#define CC2530_REGCFG_PxINP(port, pin, val) do {        \
    if(val == 0) P##port##INP &= ~CC2530_IOCTL_BV(pin); \
    else P##port##INP |= CC2530_IOCTL_BV(pin);          \
} while(0)

/** @brief  Config GPIO mode as output. */
#define CC2530_IO_OUTPUT(port, pin) do {    \
    CC2530_REGCFG_PxDIR(port , pin , 1);    \
    CC2530_REGCFG_PxSEL(port , pin , 0);    \
} while(0)

/** @brief  Config GPIO mode as input. */
#define CC2530_IO_INPUT(port, pin, mode) do {                                  \
    if ((port) == 1 && ((pin) == 0 || (pin) == 1)) break;                      \
                                                                               \
    CC2530_REGCFG_PxDIR(port , pin , 0);                                       \
    CC2530_REGCFG_PxSEL(port , pin , 0);                                       \
                                                                               \
    if (mode == CC2530_INPUT_TRISTATE) CC2530_REGCFG_PxINP(port , pin , 1);    \
    else {                                                                     \
        CC2530_REGCFG_PxINP(port , pin , 0);                                   \
        if (mode == CC2530_INPUT_PULLUP) CC2530_REGCFG_PxINP(2 , (5+port), 0); \
        else CC2530_REGCFG_PxINP(2 , (5+port), 1);                             \
    }                                                                          \
} while(0)


/** @brief    Config gpio mode.
 *  @warning  P1_0, P1_1 can't configed as input mode.
 */
#define CC2530_IOCTL(port, pin, mode) do {                  \
    if (port > 2 || pin > 7) break;                         \
                                                            \
    if (mode == CC2530_OUTPUT) CC2530_IO_OUTPUT(port, pin); \
    else CC2530_IO_INPUT(port, pin, mode);                  \
} while(0)

/** @brief    Set/Clear GPIO state.
 *  @warning  GPIO must in output mode.
 */
#define CC2530_GPIO_SET(port, pin)      P##port##_##pin = 1
#define CC2530_GPIO_CLEAR(port, pin)    P##port##_##pin = 0

/** @brief    Get GPIO state.
 */
#define CC2530_GPIO_GET(port, pin)      P##port##_##pin

/** @brief    Set system clock as 32MHZ.
 */
#define setSystemClk32MHZ() do { \
    CLKCONCMD &= ~0x40;          \
    while(CLKCONSTA & 0x40);     \
    CLKCONCMD &= ~0x47;          \
} while(0)


#ifdef __cplusplus
}
#endif

#endif /* #ifndef CC2530_IO_PORTS_H */
