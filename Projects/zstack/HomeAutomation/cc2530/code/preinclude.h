
/************************************
   
   Application Definitions

*************************************/
//-- use output to terminal via UART (prinft)
#define DEBUG_PRINT_UART                  TRUE
//-- use cyclic switching of all relays
#define DEBUG_SWITCH_RELAY                FALSE
//-- use DS18B20 (Temperature & Humidity) sensor
#define USE_DS18B20                       FALSE
//-- use DHT11 (Temperature & Humidity) sensor
#define USE_DHT11                         TRUE
//-- use LCD test
//-- !!! There may not be enough memory for the firmware, !!!
//-- !!! due to the large images for this test            !!!
#define DEBUG_LCD_TEST                    FALSE

//-- интервал повторяемого таймера события HAL_KEY_EVENT (мс)
#define TIMER_INTERVAL_HAL_KEY_EVT        100
//-- интервал повторяемого таймера для информирования о температуре DS18B20 (мс)
#define TIMER_INTERVAL_DS18B20_EVT        10000
//-- интервал повторяемого таймера для информирования о температуре DHT11 (мс)
#define TIMER_INTERVAL_DHT11_EVT          10000
//-- интервал таймера для долгого нажатия (мс)
#define TIMER_INTERVAL_LONG_PRESS_EVT     5000
//-- интервал таймера для двойного нажатия (мс)
#define TIMER_INTERVAL_DOUBLE_PRESS_EVT   600
//-- интервал таймера для мигания (мс)
#define TIMER_INTERVAL_BLINK_EVT          1000
//-- Parent not found, attempt to rejoin again after a fixed delay
#define TIMER_INTERVAL_ENDDEVICE_REJOIN   10000

/************************************
   
   Preinclude Definitions

*************************************/
#define SECURE                            1
#define TC_LINKKEY_JOIN
#define NV_INIT
#define NV_RESTORE
#define xZTOOL_P1
#define MT_TASK
#define MT_APP_FUNC
#define MT_SYS_FUNC
#define MT_ZDO_FUNC
#define MT_ZDO_MGMT
#define MT_APP_CNF_FUNC
#define LEGACY_LCD_DEBUG
//#define LCD_SUPPORTED                     DEBUG
#define MULTICAST_ENABLED                 FALSE
#define ZCL_READ
#define ZCL_WRITE
#define ZCL_BASIC
#define ZCL_IDENTIFY
#define ZCL_SCENES
#define ZCL_GROUPS
#define ZCL_ON_OFF
#define ZCL_REPORTING_DEVICE

#define DISABLE_GREENPOWER_BASIC_PROXY
#define MAX_CHANNELS_24GHZ                0x07FFF800

#define HAL_UART                          TRUE
//#define HAL_UART_ISR                      1
#define HAL_UART_DMA                      1


/************************************
   
   Operands Definitions

*************************************/
#define uint8_t uint8
#define uint16_t uint16
#define bit(b) (1UL << (b))
#define true 1
#define false 0


/************************************
   
   Сonfigs

*************************************/
#include "conf/hal_dht11_cfg.h"
#include "conf/hal_lcd_cfg.h"
#include "conf/hal_board_cfg_cc2530.h"
