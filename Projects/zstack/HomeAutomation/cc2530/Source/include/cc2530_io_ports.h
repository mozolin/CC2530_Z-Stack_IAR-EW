
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

void initOutputGPIO(uint8 gpioPort, uint8 gpioBit);

void delayMs(uint16 nMs);
