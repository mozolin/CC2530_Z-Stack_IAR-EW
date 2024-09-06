#include "chip.h" 
#include "delay.h" 
#include "u8x8.h"
#include "u8x8cb.h"

//uint8_t u8x8_gpio_and_delay_Ultriot(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
uint8 u8x8_gpio_and_delay_Ultriot(u8x8_t *u8x8, uint8 msg, uint8 arg_int, void *arg_ptr)
{
  switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      /* only support for software I2C*/
      break;
    case U8X8_MSG_DELAY_NANO:
      /* not required for SW I2C */
      break;
    case U8X8_MSG_DELAY_10MICRO:
      /* not used at the moment */
      break;
    case U8X8_MSG_DELAY_100NANO:
      /* not used at the moment */
      break;
    case U8X8_MSG_DELAY_MILLI:
      //delay_micro_seconds(arg_int*1000UL);
      break;
    case U8X8_MSG_DELAY_I2C:
      /* arg_int is 1 or 4: 100KHz (5us) or 400KHz (1.25us) */
      // delay_micro_seconds(arg_int<=2?5:1);
      break;
    case U8X8_MSG_GPIO_I2C_CLOCK:
      break;
    case U8X8_MSG_GPIO_I2C_DATA:
      break;
    /*
    case U8X8_MSG_GPIO_MENU_SELECT:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_SELECT_PORT, KEY_SELECT_PIN));
      break;
    case U8X8_MSG_GPIO_MENU_NEXT:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_NEXT_PORT, KEY_NEXT_PIN));
      break;
    case U8X8_MSG_GPIO_MENU_PREV:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_PREV_PORT, KEY_PREV_PIN));
      break;
    case U8X8_MSG_GPIO_MENU_HOME:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_HOME_PORT, KEY_HOME_PIN));
      break;
    */
    default:
      //u8x8_SetGPIOResult(u8x8, 1);
      break;
  }
  return 1;
}
