
/************************************
   
   Application Definitions

*************************************/
//-- use output to terminal via UART (prinft)
#define DEBUG_PRINT_UART                  TRUE
//-- use DS18B20 (Temperature & Humidity) sensor
#define USE_DS18B20                       FALSE
//-- use DHT11 (Temperature & Humidity) sensor
#define USE_DHT11                         TRUE
//-- use LCD
#define USE_LCD                           TRUE
//-- use LCD test
//-- !!! There may not be enough memory for the firmware, !!!
//-- !!! due to the large images for this test            !!!
#define DEBUG_LCD_TEST                    FALSE
//-- trying to get local time
#define USE_LOCAL_TIME                    TRUE


//-- repeating timer interval for HAL_KEY_EVENT (мс)
#define TIMER_INTERVAL_HAL_KEY_EVT        100
//-- repeating timer interval for DS18B20 temperature notification (ms)
#define TIMER_INTERVAL_DS18B20_EVT        10000
//-- repeat timer interval for DHT11 temperature notification (ms)
#define TIMER_INTERVAL_DHT11_EVT          10000
//-- timer interval for long press (ms)
#define TIMER_INTERVAL_LONG_PRESS_EVT     5000
//-- timer interval for double press (ms)
#define TIMER_INTERVAL_DOUBLE_PRESS_EVT   600
//-- Parent not found, attempt to rejoin again after a fixed delay
#define TIMER_INTERVAL_ENDDEVICE_REJOIN   10000
//-- repeating timer interval for local time notification (ms)
#define TIMER_INTERVAL_LOCAL_TIME_EVT     3000
//-- repeating timer interval to switch LCD (ms)
#define TIMER_INTERVAL_SWITCH_SCREEN_EVT  5000

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
//#define LEGACY_LCD_DEBUG
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
#include "conf/hal_board_cfg.h"
#include "conf/hal_board_cfg_common.h"
#include "conf/hal_relay_cfg.h"
#include "conf/hal_led_cfg.h"
