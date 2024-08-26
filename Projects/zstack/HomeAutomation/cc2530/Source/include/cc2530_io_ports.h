
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

//-- Device on pin (Port 0)
#define GPIO_DEVICE_P00              P0_0
#define GPIO_DEVICE_P01              P0_1
#define GPIO_DEVICE_P02              P0_2
#define GPIO_DEVICE_P03              P0_3
#define GPIO_DEVICE_P04              P0_4
#define GPIO_DEVICE_P05              P0_5
#define GPIO_DEVICE_P06              P0_6
#define GPIO_DEVICE_P07              P0_7
//-- Device on pin (Port 1)
#define GPIO_DEVICE_P10              P1_0
#define GPIO_DEVICE_P11              P1_1
#define GPIO_DEVICE_P12              P1_2
#define GPIO_DEVICE_P13              P1_3
#define GPIO_DEVICE_P14              P1_4
#define GPIO_DEVICE_P15              P1_5
#define GPIO_DEVICE_P16              P1_6
#define GPIO_DEVICE_P17              P1_7
//-- Device on pin (Port 2)
#define GPIO_DEVICE_P20              P2_0
#define GPIO_DEVICE_P21              P2_1
#define GPIO_DEVICE_P22              P2_2
#define GPIO_DEVICE_P23              P2_3
#define GPIO_DEVICE_P24              P2_4


//-- General purpose or not
#define GPIO_PERIPHERAL_FUNCTION     0
#define GPIO_GENERAL_PURPOSE         1

//-- Input or output
#define GPIO_DIR_INPUT               0
#define GPIO_DIR_OUTPUT              1
                                     
//-- Pullup, pulldown, 3-state button or none
#define GPIO_PULL_UP                 0
#define GPIO_PULL_DOWN               1
#define GPIO_PULL_3STATE             2
#define GPIO_PULL_NONE               -1

//-- Interrupt enabled/disabled
#define GPIO_INTERRUPT_DISABLED      0
#define GPIO_INTERRUPT_ENABLED       1
                                     
//-- Rising edge on input gives interrupt, Falling edge or None
#define GPIO_INTERRUPT_CTRL_RISE     0
#define GPIO_INTERRUPT_CTRL_FALL     1
#define GPIO_INTERRUPT_CTRL_NONE     -1

//-- Led state
#define LED_OFF         0 //!< off.
#define LED_ON          1 //!< on.

//-- Button state
#define BUTTON_DOWN     0 //!< press-down.
#define BUTTON_NORMAL   1 //!< normal state.


void initGPIO(
	uint8 gpioPort,
	uint8 gpioBit,
	bool  gpioGeneralPurpose,
	bool  gpioInput,
	uint8 gpioPullUpDn,
	bool gpioIntrEnable,
	int8 gpioIntrCtrl
);

void resetGPIO(uint8 gpioPort, uint8 gpioBit);

void initShortGPIO(uint8 gpioPort, uint8 gpioBit, bool gpioOutput);

void delayMs(uint16 nMs);
