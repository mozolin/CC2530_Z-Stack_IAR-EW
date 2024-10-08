//-- Standard libs
#include <stdio.h>  //-- printf, sprintf
#include <string.h> //-- memset

//-- Z-Stack libs
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "MT_SYS.h"
#include "nwk_util.h"
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_ms.h"
#include "zcl_diagnostic.h"
#include "zcl_cc2530.h"
#include "bdb.h"
#include "bdb_interface.h"
#include "gp_interface.h"
#include "onboard.h"

//-- HAL libs
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_drivers.h"
#include "hal_uart.h"

//-- Extra libs
#include "cc2530_io_ports.h"
#include "utils.h"
#include "ds18b20.h"
#include "colors.h"
#include "cc2530_io_ports.h"

#if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
  //-- OLED libs
  #include "hal_oled.h"
  #include "img_v_picture.h"
  #include "images.h"
#else
  //-- TFT libs
  #include "hal_tft.h"
  #include "img_rgb_picture.h"
  #define PX_RED     HAL_TFT_PIXEL_RED
  #define PX_GREEN   HAL_TFT_PIXEL_GREEN
  #define PX_BLUE    HAL_TFT_PIXEL_BLUE
  #define PX_BLACK   HAL_TFT_PIXEL_BLACK
  #define PX_WHITE   HAL_TFT_PIXEL_WHITE
  #define PX_YELLOW  HAL_TFT_PIXEL_YELLOW
  #define PX_GRAY    HAL_TFT_PIXEL_GRAY
  #define PX_CYAN    HAL_TFT_PIXEL_CYAN
	#define PX_MAGENTA HAL_TFT_PIXEL_MAGENTA
#endif
#include "images.h"

//-- DHT11 driver
int dht11Idx = 0;
#include "hal_dht11.h" 


// Идентификатор задачи нашего приложения
byte zclcc2530_TaskID;

// Состояние сети
devStates_t zclcc2530_NwkState = DEV_INIT;

// Состояние кнопок
static uint8 halKeySavedKeys;

// Состояние реле
uint8 RELAY_STATE = 0;

// Данные о температуре
int16 zclcc2530_MeasuredValue;

// Структура для отправки отчета
afAddrType_t zclcc2530_DstAddr;
// Номер сообщения
uint8 SeqNum = 0;

static void zclcc2530_HandleKeys(byte shift, byte keys);
static void zclcc2530_BasicResetCB(void);
static void zclcc2530_ProcessIdentifyTimeChange(uint8 endpoint);

static void zclcc2530_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg);

// Функции обработки входящих сообщений ZCL Foundation команд/ответов
static void zclcc2530_ProcessIncomingMsg(zclIncomingMsg_t *msg);
#ifdef ZCL_READ
static uint8 zclcc2530_ProcessInReadRspCmd(zclIncomingMsg_t *pInMsg);
#endif
#ifdef ZCL_WRITE
static uint8 zclcc2530_ProcessInWriteRspCmd(zclIncomingMsg_t *pInMsg);
#endif
static uint8 zclcc2530_ProcessInDefaultRspCmd(zclIncomingMsg_t *pInMsg);
#ifdef ZCL_DISCOVER
static uint8 zclcc2530_ProcessInDiscCmdsRspCmd(zclIncomingMsg_t *pInMsg);
static uint8 zclcc2530_ProcessInDiscAttrsRspCmd(zclIncomingMsg_t *pInMsg);
static uint8 zclcc2530_ProcessInDiscAttrsExtRspCmd(zclIncomingMsg_t *pInMsg);
#endif

// Изменение состояние реле
static void updateRelay(bool);
// Отображение состояния реле на пинах
static void applyRelay(void);
// Выход из сети
void zclcc2530_LeaveNetwork(void);
// Отправка отчета о состоянии реле
void zclcc2530_ReportOnOff(void);
// Отправка отчета о температуре
void zclcc2530_ReportTemp(void);

uint8 initUart0(halUARTCBack_t pf);
void uart0RxCb(uint8 port, uint8 event);

#if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
  //-- Init & Draw SSD1306 OLED
  void SSD1306Draw(void);
#else
  //-- Init & Draw TFT
  void TFTDraw(void);
#endif

//-- report DHT11 sensor
void zclcc2530_ReportDHT11(void);

/*********************************************************************
 * Таблица обработчиков основных ZCL команд
 */
static zclGeneral_AppCallbacks_t zclcc2530_CmdCallbacks =
{
  zclcc2530_BasicResetCB,               // Basic Cluster Reset command
  NULL,                                   // Identify Trigger Effect command
  zclcc2530_OnOffCB,                    // On/Off cluster commands
  NULL,                                   // On/Off cluster enhanced command Off with Effect
  NULL,                                   // On/Off cluster enhanced command On with Recall Global Scene
  NULL,                                   // On/Off cluster enhanced command On with Timed Off
#ifdef ZCL_LEVEL_CTRL
  NULL,                                   // Level Control Move to Level command
  NULL,                                   // Level Control Move command
  NULL,                                   // Level Control Step command
  NULL,                                   // Level Control Stop command
#endif
#ifdef ZCL_GROUPS
  NULL,                                   // Group Response commands
#endif
#ifdef ZCL_SCENES
  NULL,                                  // Scene Store Request command
  NULL,                                  // Scene Recall Request command
  NULL,                                  // Scene Response command
#endif
#ifdef ZCL_ALARMS
  NULL,                                  // Alarm (Response) commands
#endif
#ifdef SE_UK_EXT
  NULL,                                  // Get Event Log command
  NULL,                                  // Publish Event Log command
#endif
  NULL,                                  // RSSI Location command
  NULL                                   // RSSI Location Response command
};

/*********************************************************************
 * TODO: Add other callback structures for any additional application specific 
 *       Clusters being used, see available callback structures below.
 *
 *       bdbTL_AppCallbacks_t 
 *       zclApplianceControl_AppCallbacks_t 
 *       zclApplianceEventsAlerts_AppCallbacks_t 
 *       zclApplianceStatistics_AppCallbacks_t 
 *       zclElectricalMeasurement_AppCallbacks_t 
 *       zclGeneral_AppCallbacks_t 
 *       zclGp_AppCallbacks_t 
 *       zclHVAC_AppCallbacks_t 
 *       zclLighting_AppCallbacks_t 
 *       zclMS_AppCallbacks_t 
 *       zclPollControl_AppCallbacks_t 
 *       zclPowerProfile_AppCallbacks_t 
 *       zclSS_AppCallbacks_t  
 *
 */

// Функция инициализации задачи приложения
void zclcc2530_Init(byte task_id)
{
  zclcc2530_TaskID = task_id;
  
  // Регистрация описания профиля Home Automation Profile
  bdb_RegisterSimpleDescriptor(&zclcc2530_SimpleDesc);

  // Регистрация обработчиков ZCL команд
  zclGeneral_RegisterCmdCallbacks(cc2530_ENDPOINT, &zclcc2530_CmdCallbacks);
  
  // TODO: Register other cluster command callbacks here

  // Регистрация атрибутов кластеров приложения
  zcl_registerAttrList(cc2530_ENDPOINT, zclcc2530_NumAttributes, zclcc2530_Attrs);

  // Подписка задачи на получение сообщений о командах/ответах
  zcl_registerForMsg(zclcc2530_TaskID);

#ifdef ZCL_DISCOVER
  // Регистрация списка команд, реализуемых приложением
  zcl_registerCmdList(cc2530_ENDPOINT, zclCmdsArraySize, zclcc2530_Cmds);
#endif

  // Подписка задачи на получение всех событий для кнопок
  RegisterForKeys(zclcc2530_TaskID);

  bdb_RegisterCommissioningStatusCB(zclcc2530_ProcessCommissioningStatus);
  bdb_RegisterIdentifyTimeChangeCB(zclcc2530_ProcessIdentifyTimeChange);

#ifdef ZCL_DIAGNOSTIC
  // Register the application's callback function to read/write attribute data.
  // This is only required when the attribute data format is unknown to ZCL.
  zcl_registerReadWriteCB(cc2530_ENDPOINT, zclDiagnostic_ReadWriteAttrCB, NULL);

  if (zclDiagnostic_InitStats() == ZSuccess)
  {
    // Here the user could start the timer to save Diagnostics to NV
  }
#endif
  
  // Установка адреса и эндпоинта для отправки отчета
  zclcc2530_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  zclcc2530_DstAddr.endPoint = 0;
  zclcc2530_DstAddr.addr.shortAddr = 0;
  
  // инициализируем NVM для хранения RELAY STATE
  if(SUCCESS == osal_nv_item_init(NV_cc2530_RELAY_STATE_ID, 1, &RELAY_STATE)) {
    // читаем значение RELAY STATE из памяти
    osal_nv_read(NV_cc2530_RELAY_STATE_ID, 0, 1, &RELAY_STATE);
  }
  // применяем состояние реле
  applyRelay();

  // запускаем повторяемый таймер события HAL_KEY_EVENT (мс)
  osal_start_reload_timer(zclcc2530_TaskID, HAL_KEY_EVENT, TIMER_INTERVAL_HAL_KEY_EVT);
  
  //-- запускаем повторяемый таймер для информирования о температуре (мс)
  osal_start_reload_timer(zclcc2530_TaskID, cc2530_EVT_REPORTING, TIMER_INTERVAL_REPORTING_EVT);
  //-- запускаем повторяемый таймер для информирования о температуре (мс)
  osal_start_reload_timer(zclcc2530_TaskID, cc2530_EVT_REFRESH, TIMER_INTERVAL_REFRESH_EVT);
  
  // Старт процесса возвращения в сеть
  bdb_StartCommissioning(BDB_COMMISSIONING_MODE_PARENT_LOST);
  
  //-- init UART to use "printf" for serial monitor
  HalUARTInit();
  uint8 state = initUart0(uart0RxCb);

  printf(FONT_COLOR_STRONG_GREEN);
  printf("\nUART initiated\n");
  printf(STYLE_COLOR_RESET);

  
  setSystemClk32MHZ();
  #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
    //-- init & draw OLED
    halOLED128x64Init();
    SSD1306Draw();
  #else
    //-- init & draw TFT
    halTFTInit(HAL_TFT_PIXEL_BLACK);
    TFTDraw();
  #endif

  delayMs32MHZ(4000);
  //zclcc2530_ReportDHT11();
}

// Основной цикл обработки событий задачи
uint16 zclcc2530_event_loop(uint8 task_id, uint16 events)
{
  afIncomingMSGPacket_t *MSGpkt;

  //-- Intentionally unreferenced parameter
  (void)task_id;

  if (events & SYS_EVENT_MSG)
  {
    while ((MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclcc2530_TaskID)))
    {
      switch (MSGpkt->hdr.event)
      {
        case ZCL_INCOMING_MSG:
          //-- Обработка входящего сообщения ZCL Foundation команды/ответа
          zclcc2530_ProcessIncomingMsg((zclIncomingMsg_t *)MSGpkt);
          break;

        case KEY_CHANGE:
          zclcc2530_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
          break;

        case ZDO_STATE_CHANGE:
          zclcc2530_NwkState = (devStates_t)(MSGpkt->hdr.status);

          //-- Теперь мы в сети
          if((zclcc2530_NwkState == DEV_ZB_COORD) ||
            (zclcc2530_NwkState == DEV_ROUTER)   ||
            (zclcc2530_NwkState == DEV_END_DEVICE)) {
            
            printf(FONT_COLOR_STRONG_GREEN);
            printf("Joined network!\n");
            printf(STYLE_COLOR_RESET);
            
            //-- отключаем мигание
            osal_stop_timerEx(zclcc2530_TaskID, HAL_LED_BLINK_EVENT);
            HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
            
            //-- отправляем отчет
            zclcc2530_ReportOnOff();
          }
          break;

        default:
          break;
      }

      osal_msg_deallocate((uint8 *)MSGpkt);
    }

    // возврат необработаных сообщений
    return (events ^ SYS_EVENT_MSG);
  }

  /* Обработка событий приложения */
  
  //-- событие cc2530_EVT_BLINK
  if(events & cc2530_EVT_BLINK)
  {
    printf("Blinking...\n");
    //-- переключим светодиод
    HalLedSet(HAL_LED_2, HAL_LED_MODE_TOGGLE);
    return (events ^ cc2530_EVT_BLINK);
  }
  // событие cc2530_EVT_LONG
  if(events & cc2530_EVT_LONG)
  {
    printf("Long pressed!\n");
    // Проверяем текущее состояние устройства
    // В сети или не в сети?
    if(bdbAttributes.bdbNodeIsOnANetwork) {
      //-- покидаем сеть
      zclcc2530_LeaveNetwork();
      
      printf(FONT_COLOR_STRONG_RED);
      printf("Leave Network\n");
      printf(STYLE_COLOR_RESET);
    }
    else 
    {
      // инициируем вход в сеть
      bdb_StartCommissioning(
        BDB_COMMISSIONING_MODE_NWK_FORMATION | 
        BDB_COMMISSIONING_MODE_NWK_STEERING | 
        BDB_COMMISSIONING_MODE_FINDING_BINDING | 
        BDB_COMMISSIONING_MODE_INITIATOR_TL
     );
      // будем мигать пока не подключимся
      osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_BLINK, TIMER_INTERVAL_BLINK_EVT);
      
      printf(FONT_COLOR_STRONG_YELLOW);
      printf("Start Commissioning...\n");
      printf(STYLE_COLOR_RESET);
    }
    
    return (events ^ cc2530_EVT_LONG);
  }
  
  //-- cc2530_EVT_REPORTING event
  if(events & cc2530_EVT_REPORTING) {
    
    //printf(FONT_COLOR_STRONG_WHITE);
    //printf("cc2530_EVT_REPORTING\n");
    //printf(STYLE_COLOR_RESET);
    
    zclcc2530_ReportTemp();
    
    return (events ^ cc2530_EVT_REPORTING);
  }

  //-- cc2530_EVT_REFRESH event
  if(events & cc2530_EVT_REFRESH) {
    
    //printf(FONT_COLOR_STRONG_MAGENTA);
    //printf("cc2530_EVT_REFRESH\n");
    //printf(STYLE_COLOR_RESET);
    
    //zclcc2530_ReportDHT11();
    
    return (events ^ cc2530_EVT_REFRESH);
  }
  
  // событие опроса кнопок
  if (events & HAL_KEY_EVENT)
  {
    /* Считывание кнопок */
    cc2530_HalKeyPoll();

    return (events ^ HAL_KEY_EVENT);
  }
  
  // Отбросим необработаные сообщения
  return 0;
}


// Обработчик нажатий клавиш
static void zclcc2530_HandleKeys(byte shift, byte keys)
{
  if(keys & HAL_KEY_SW_1) {
    printf("Key1\n");
    //-- Запускаем таймер для определения долгого нажатия 5сек
    osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_LONG, 5000);
    //-- Переключаем реле
    updateRelay(RELAY_STATE == 0);
    
    //halOLED128x64ShowX16(0, 0, "Key1:clear in 4s");
    if(RELAY_STATE == 0) {
      #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
        halOLED128x64ClearScreen();
      #else
        halTFTSetScreen(PX_BLACK);
      #endif
    } else {
      #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
        SSD1306Draw();
      #else
        TFTDraw();
      #endif
    }
  } else {
    //-- Останавливаем таймер ожидания долгого нажатия
    osal_stop_timerEx(zclcc2530_TaskID, cc2530_EVT_LONG);
  }

  if(keys & HAL_KEY_SW_2) {
    printf("Key2:refresh DHT11\n");
    HalLedSet(HAL_LED_3, HAL_LED_MODE_TOGGLE);
    
    zclcc2530_ReportDHT11();
    //halOLED128x64ShowX16(0, 0, "Key2:show in 4s");
  }

  if(keys & HAL_KEY_SW_3) {
    P0_4 = (P0_4 == 0) ? 1 : 0;
    printf("Key3:%d\n", P0_4);
    //halOLED128x64ShowX16(2, 0, "Key #3 pressed");
  }

}


// Обработчик изменения статусов соединения с сетью
static void zclcc2530_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg)
{
  switch(bdbCommissioningModeMsg->bdbCommissioningMode)
  {
    case BDB_COMMISSIONING_FORMATION:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        //After formation, perform nwk steering again plus the remaining commissioning modes that has not been process yet
        bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | bdbCommissioningModeMsg->bdbRemainingCommissioningModes);
      }
      else
      {
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_NWK_STEERING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        //YOUR JOB:
        //We are on the nwk, what now?
      }
      else
      {
        //See the possible errors for nwk steering procedure
        //No suitable networks found
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_FINDING_BINDING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        //YOUR JOB:
      }
      else
      {
        //YOUR JOB:
        //retry?, wait for user interaction?
      }
    break;
    case BDB_COMMISSIONING_INITIALIZATION:
      //Initialization notification can only be successful. Failure on initialization
      //only happens for ZED and is notified as BDB_COMMISSIONING_PARENT_LOST notification

      //YOUR JOB:
      //We are on a network, what now?

    break;
#if ZG_BUILD_ENDDEVICE_TYPE    
    case BDB_COMMISSIONING_PARENT_LOST:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED)
      {
        //We did recover from losing parent
      }
      else
      {
        //Parent not found, attempt to rejoin again after a fixed delay
        osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_END_DEVICE_REJOIN, cc2530_END_DEVICE_REJOIN_DELAY);
      }
    break;
#endif 
  }
}


// Обработчик изменения времени идентификации
static void zclcc2530_ProcessIdentifyTimeChange(uint8 endpoint)
{
  (void) endpoint;

  if (zclcc2530_IdentifyTime > 0)
  {
    //HalLedBlink (HAL_LED_2, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
  }
  else
  {
    //HalLedSet (HAL_LED_2, HAL_LED_MODE_OFF);
  }
}


//-- Обработчик команды сброса в Basic кластере
static void zclcc2530_BasicResetCB(void)
{
  /* TODO: remember to update this function with any application-specific cluster attribute variables */
  
  zclcc2530_ResetAttributesToDefaultValues();
}

/******************************************************************************
 *
 *  Functions for processing ZCL Foundation incoming Command/Response messages
 *
 *****************************************************************************/

// Функция обработки входящих ZCL Foundation команд/ответов
static void zclcc2530_ProcessIncomingMsg(zclIncomingMsg_t *pInMsg)
{
  switch (pInMsg->zclHdr.commandID)
  {
#ifdef ZCL_READ
    case ZCL_CMD_READ_RSP:
      zclcc2530_ProcessInReadRspCmd(pInMsg);
      break;
#endif
#ifdef ZCL_WRITE
    case ZCL_CMD_WRITE_RSP:
      zclcc2530_ProcessInWriteRspCmd(pInMsg);
      break;
#endif
    case ZCL_CMD_CONFIG_REPORT:
    case ZCL_CMD_CONFIG_REPORT_RSP:
    case ZCL_CMD_READ_REPORT_CFG:
    case ZCL_CMD_READ_REPORT_CFG_RSP:
    case ZCL_CMD_REPORT:
      //bdb_ProcessIncomingReportingMsg(pInMsg);
      break;
      
    case ZCL_CMD_DEFAULT_RSP:
      zclcc2530_ProcessInDefaultRspCmd(pInMsg);
      break;
#ifdef ZCL_DISCOVER
    case ZCL_CMD_DISCOVER_CMDS_RECEIVED_RSP:
      zclcc2530_ProcessInDiscCmdsRspCmd(pInMsg);
      break;

    case ZCL_CMD_DISCOVER_CMDS_GEN_RSP:
      zclcc2530_ProcessInDiscCmdsRspCmd(pInMsg);
      break;

    case ZCL_CMD_DISCOVER_ATTRS_RSP:
      zclcc2530_ProcessInDiscAttrsRspCmd(pInMsg);
      break;

    case ZCL_CMD_DISCOVER_ATTRS_EXT_RSP:
      zclcc2530_ProcessInDiscAttrsExtRspCmd(pInMsg);
      break;
#endif
    default:
      break;
  }

  if (pInMsg->attrCmd)
    osal_mem_free(pInMsg->attrCmd);
}

#ifdef ZCL_READ
// Обработка ответа команды Read
static uint8 zclcc2530_ProcessInReadRspCmd(zclIncomingMsg_t *pInMsg)
{
  zclReadRspCmd_t *readRspCmd;
  uint8 i;

  readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
  for(i = 0; i < readRspCmd->numAttr; i++) {
    // Notify the originator of the results of the original read attributes
    // attempt and, for each successfull request, the value of the requested
    // attribute
  }

  return (true);
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
// Обработка ответа команды Write
static uint8 zclcc2530_ProcessInWriteRspCmd(zclIncomingMsg_t *pInMsg)
{
  zclWriteRspCmd_t *writeRspCmd;
  uint8 i;

  writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < writeRspCmd->numAttr; i++)
  {
    // Notify the device of the results of the its original write attributes
    // command.
  }

  return (true);
}
#endif // ZCL_WRITE

// Обработка ответа команды по-умолчанию
static uint8 zclcc2530_ProcessInDefaultRspCmd(zclIncomingMsg_t *pInMsg)
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;

  // Device is notified of the Default Response command.
  (void)pInMsg;

  return (true);
}

#ifdef ZCL_DISCOVER
// Обработка ответа команды Discover
static uint8 zclcc2530_ProcessInDiscCmdsRspCmd(zclIncomingMsg_t *pInMsg)
{
  zclDiscoverCmdsCmdRsp_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverCmdsCmdRsp_t *)pInMsg->attrCmd;
  for (i = 0; i < discoverRspCmd->numCmd; i++)
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return (true);
}

// Обработка ответа команды Discover Attributes
static uint8 zclcc2530_ProcessInDiscAttrsRspCmd(zclIncomingMsg_t *pInMsg)
{
  zclDiscoverAttrsRspCmd_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverAttrsRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < discoverRspCmd->numAttr; i++)
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return (true);
}

// Обработка ответа команды Discover Attributes Ext
static uint8 zclcc2530_ProcessInDiscAttrsExtRspCmd(zclIncomingMsg_t *pInMsg)
{
  zclDiscoverAttrsExtRsp_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverAttrsExtRsp_t *)pInMsg->attrCmd;
  for (i = 0; i < discoverRspCmd->numAttr; i++)
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return (true);
}
#endif // ZCL_DISCOVER


// Инициализация работы кнопок (входов)
void cc2530_HalKeyInit(void)
{
  /* Сбрасываем сохраняемое состояние кнопок в 0 */
  halKeySavedKeys = 0;

  PUSH1_SEL &= ~(PUSH1_BV); /* Выставляем функцию пина - GPIO */
  PUSH1_DIR &= ~(PUSH1_BV); /* Выставляем режим пина - Вход */
  
  PUSH1_ICTL &= ~(PUSH1_ICTLBIT); /* Не генерируем прерывания на пине */
  PUSH1_IEN &= ~(PUSH1_IENBIT);   /* Очищаем признак включения прерываний */
  
  PUSH2_SEL &= ~(PUSH2_BV); /* Set pin function to GPIO */
  PUSH2_DIR &= ~(PUSH2_BV); /* Set pin direction to Input */
  
  PUSH2_ICTL &= ~(PUSH2_ICTLBIT); /* don't generate interrupt */
  PUSH2_IEN &= ~(PUSH2_IENBIT);   /* Clear interrupt enable bit */

  //-- LED initiated as output...
  initOutputGPIO(GPIO_PORT_0, GPIO_PIN_4);
  P0_4 = 0;

  //-- PUSH3 initiated as input...
  initInputGPIO(GPIO_PORT_1, GPIO_PIN_5, GPIO_PULL_UP);
}

// Считывание кнопок
void cc2530_HalKeyPoll (void)
{
  uint8 keys = 0;

  // нажата кнопка 1 ?
  if(HAL_PUSH_BUTTON1()) {
    keys |= HAL_KEY_SW_1;
  }
  
  // нажата кнопка 2 ?
  if(HAL_PUSH_BUTTON2()) {
    keys |= HAL_KEY_SW_2;
  }

  // нажата кнопка 3 ?
  if(HAL_PUSH_BUTTON3()) {
    keys |= HAL_KEY_SW_3;
  }
  
  if(keys == halKeySavedKeys) {
    // Выход - нет изменений
    return;
  }
  // Сохраним текущее состояние кнопок для сравнения в след раз
  halKeySavedKeys = keys;

  // Вызовем генерацию события изменения кнопок
  OnBoard_SendKeys(keys, HAL_KEY_STATE_NORMAL);
}

//-- Изменение состояния реле
void updateRelay(bool value)
{
  if(value) {
    RELAY_STATE = 1;
  } else {
    RELAY_STATE = 0;
  }

  printf("Relay (updateRelay): %d\n", RELAY_STATE);

  //-- сохраняем состояние реле
  osal_nv_write(NV_cc2530_RELAY_STATE_ID, 0, 1, &RELAY_STATE);
  //-- Отображаем новое состояние
  applyRelay();
  //-- отправляем отчет
  zclcc2530_ReportOnOff();
}
  
// Применение состояние реле
void applyRelay (void)
{
  // если выключено
  if(RELAY_STATE == 0) {
    // то гасим светодиод 1
    HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
  } else {
    // иначе включаем светодиод 1
    HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
  }
}


//-- Инициализация выхода из сети
void zclcc2530_LeaveNetwork(void)
{
  zclcc2530_ResetAttributesToDefaultValues();
  
  NLME_LeaveReq_t leaveReq;
  //-- Set every field to 0
  osal_memset(&leaveReq, 0, sizeof(NLME_LeaveReq_t));

  //-- This will enable the device to rejoin the network after reset.
  leaveReq.rejoin = false;

  //-- Set the NV startup option to force a "new" join.
  zgWriteStartupOptions(ZG_STARTUP_SET, ZCD_STARTOPT_DEFAULT_NETWORK_STATE);

  //-- Leave the network, and reset afterwards
  if (NLME_LeaveReq(&leaveReq) != ZSuccess) {
    //-- Couldn't send out leave; prepare to reset anyway
    ZDApp_LeaveReset(false);
  }
}

// Обработчик команд кластера OnOff
static void zclcc2530_OnOffCB(uint8 cmd)
{
  printf("OnOff? -> %d (on=%d, off=%d, toggle=%d)\n", cmd, COMMAND_ON, COMMAND_OFF, COMMAND_TOGGLE);
  // запомним адрес откуда пришла команда
  // чтобы отправить обратно отчет
  afIncomingMSGPacket_t *pPtr = zcl_getRawAFMsg();
  zclcc2530_DstAddr.addr.shortAddr = pPtr->srcAddr.addr.shortAddr;
  
  // Включить
  if(cmd == COMMAND_ON) {
    updateRelay(true);
  }
  // Выключить
  else if(cmd == COMMAND_OFF) {
    updateRelay(false);
  }
  // Переключить
  else if(cmd == COMMAND_TOGGLE) {
    updateRelay(RELAY_STATE == 0);
  }
}

//-- Информирование о состоянии реле
void zclcc2530_ReportOnOff(void) {
  const uint8 NUM_ATTRIBUTES = 1;

  zclReportCmd_t *pReportCmd;

  pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) +
                              (NUM_ATTRIBUTES * sizeof(zclReport_t)));
  if(pReportCmd != NULL) {
    pReportCmd->numAttr = NUM_ATTRIBUTES;

    pReportCmd->attrList[0].attrID = ATTRID_ON_OFF;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_BOOLEAN;
    pReportCmd->attrList[0].attrData = (void *)(&RELAY_STATE);

    zclcc2530_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
    zclcc2530_DstAddr.addr.shortAddr = 0;
    zclcc2530_DstAddr.endPoint = 1;

    zcl_SendReportCmd(cc2530_ENDPOINT, &zclcc2530_DstAddr,
                      ZCL_CLUSTER_ID_GEN_ON_OFF, pReportCmd,
                      ZCL_FRAME_SERVER_CLIENT_DIR, false, SeqNum++);
  }

  osal_mem_free(pReportCmd);
}

//-- temperature report
void zclcc2530_ReportTemp(void)
{
  //-- reading the temperature
  zclcc2530_MeasuredValue = readTemperature();

  /*
  double number = (zclcc2530_MeasuredValue / 100.0);

  printf(FONT_COLOR_STRONG_YELLOW);
  printf("DS18B20 sensor: ");
  
  printf(STYLE_COLOR_BOLD);
  printf(FONT_COLOR_STRONG_CYAN);
  printNumber(number, 2);

  printf(FONT_COLOR_STRONG_YELLOW);
  printf(" °С\n");
  printf(STYLE_COLOR_RESET);
  */

  /*
  char buffer2[sizeof(int) * 8 + 1];
  char* str2 = itoa(number, buffer2);
  halOLED128x64ShowX16(3, 0, "DS18B20:");
  halOLED128x64ShowX16(3, 9, str2);
  */
  
  const uint8 NUM_ATTRIBUTES = 1;

  zclReportCmd_t *pReportCmd;

  pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) +
                              (NUM_ATTRIBUTES * sizeof(zclReport_t)));
  if (pReportCmd != NULL) {
    pReportCmd->numAttr = NUM_ATTRIBUTES;

    pReportCmd->attrList[0].attrID = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_INT16;
    pReportCmd->attrList[0].attrData = (void *)(&zclcc2530_MeasuredValue);

    zclcc2530_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
    zclcc2530_DstAddr.addr.shortAddr = 0;
    zclcc2530_DstAddr.endPoint = 1;

    zcl_SendReportCmd(cc2530_ENDPOINT, &zclcc2530_DstAddr,
                      ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT, pReportCmd,
                      ZCL_FRAME_SERVER_CLIENT_DIR, false, SeqNum++);
  }

  osal_mem_free(pReportCmd);
}

__near_func int putchar(int c)
{
  HalUARTWrite(HAL_UART_PORT_0, (uint8 *)&c, 1);
  return(c);
}

uint8 initUart0(halUARTCBack_t pf)
{
  halUARTCfg_t uartConfig;
  uartConfig.configured           = true;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = false;
  uartConfig.flowControlThreshold = 48;
  uartConfig.rx.maxBufSize        = 128;
  uartConfig.tx.maxBufSize        = 128;
  uartConfig.idleTimeout          = 6;
  uartConfig.intEnable            = true;           
  uartConfig.callBackFunc         = pf;
  return HalUARTOpen (HAL_UART_PORT_0, &uartConfig);
}

void uart0RxCb(uint8 port, uint8 event)
{
  uint8 ch;
  while(Hal_UART_RxBufLen(port)) {
    // Read one byte from UART to ch
    HalUARTRead (port, &ch, 1);
  }
}

#if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
  void SSD1306Draw(void)
  {
    //-- !! halOLED128x64ShowX8:  21 chars in row (+2 px) !!
    //-- !! halOLED128x64ShowX16: 16 chars in row (exact) !!
  	
    //-- ASCII table #1 (8x16)
    halOLED128x64ClearScreen();
    halOLED128x64ShowX16(0, 0, "ABCDEFGHIJKLMNOP");
    halOLED128x64ShowX16(1, 0, "QRSTUVWXYZabcdef");
    halOLED128x64ShowX16(2, 0, "ghijklmnopqrstuv");
    halOLED128x64ShowX16(3, 0, "wxyz0123456789.,");
  
    //-- ASCII table #2 (8x16)
    delayMs32MHZ(4000);
    halOLED128x64ClearScreen();
    halOLED128x64ShowX16(0, 0, "\"'?!@_*#$%&()+-/");
    halOLED128x64ShowX16(1, 0, ":;<=>[\\]^`{|}~");
    halOLED128x64ShowX16(2, 0, "................");
    halOLED128x64ShowX16(3, 0, "................");
  
    //-- Russian table #1 (8x16)
    delayMs32MHZ(4000);
    halOLED128x64ClearScreen();
    halOLED128x64ShowX16(0, 0, "АБВГДЕЖЗИЙКЛМНОП");
    halOLED128x64ShowX16(1, 0, "РСТУФХЦЧШЩЪЫЬЭЮЯ");
    halOLED128x64ShowX16(2, 0, "абвгдежзийклмноп");
    halOLED128x64ShowX16(3, 0, "рстуфхцчшщъыьэюя");
  
    //-- Russian table #2 (8x16)
    delayMs32MHZ(4000);
    halOLED128x64ClearScreen();
    halOLED128x64ShowX16(0, 0, "Ёё °C");
    halOLED128x64ShowX16(1, 0, "................");
    halOLED128x64ShowX16(2, 0, "................");
    halOLED128x64ShowX16(3, 0, "................");
  
    //-- ASCII table #1 (8x8)
    delayMs32MHZ(4000);
    halOLED128x64ClearScreen();
    halOLED128x64ShowX8(0, 0, "ABCDEFGHIJKLMNOPQRSTU");
    halOLED128x64ShowX8(1, 0, "VWXYZabcdefghijklmnop");
    halOLED128x64ShowX8(2, 0, "qrstuvwxyz0123456789.");
    halOLED128x64ShowX8(3, 0, ",\"'?!@_*#$%&()+-/:;<=");
    halOLED128x64ShowX8(4, 0, ">[\\]^`{|}~");
    halOLED128x64ShowX8(5, 0, ".....................");
    halOLED128x64ShowX8(6, 0, ".....................");
    halOLED128x64ShowX8(7, 0, ".....................");
  
    //-- Russian table #1 (8x8)
    delayMs32MHZ(4000);
    halOLED128x64ClearScreen();
    halOLED128x64ShowX8(0, 2, "ЁАБВГДЕЖЗИЙКЛМНОПРСТУ");
    halOLED128x64ShowX8(1, 2, "ФХЦЧШЩЪЫЬЭЮЯабвгдежзи");
    halOLED128x64ShowX8(2, 2, "йклмнопрстуфхцчшщъыьэ");
    halOLED128x64ShowX8(3, 2, "юяё °C");
    halOLED128x64ShowX8(4, 2, ".....................");
    halOLED128x64ShowX8(5, 2, ".....................");
    halOLED128x64ShowX8(6, 2, ".....................");
    halOLED128x64ShowX8(7, 2, ".....................");
  
    //-- Icons table
    delayMs32MHZ(4000);
    halOLED128x64ClearScreen();
    halOLED128x64ShowIcon(10, 0, 7, 0);
    halOLED128x64ShowIcon(20, 1, 8, 0);
    halOLED128x64ShowIcon(30, 2, 7, 1);
    halOLED128x64ShowIcon(40, 3, 8, 1);
    halOLED128x64ShowIcon(50, 4, 7, 2);
    halOLED128x64ShowIcon(60, 5, 8, 2);
    halOLED128x64ShowIcon(70, 6, 7, 3);
    halOLED128x64ShowIcon(80, 7, 8, 3);
    halOLED128x64ShowIcon(90, 8, 7, 4);
  
    //-- Pictures
    delayMs32MHZ(4000);
    halOLED128x64ClearScreen();
    halOLED128x64ShowPicture(0, 0, 128, 64, zigbee_logo2);
  
  
    delayMs32MHZ(4000);
    
    halOLED128x64ClearScreen();
  
    halOLED128x64ShowX16(0, 112, "А");
    halOLED128x64ShowX8(3, 112, "А");
  
    
    halOLED128x64ShowIcon(20, 0, 8, 0);
    halOLED128x64ShowIcon(20, 2, 7, 0);
  
    halOLED128x64ShowPicture(0, 8, 16, 16, danger_16x16);
    halOLED128x64ShowPicture(0, 24, 16, 16, empty_16x16);
    halOLED128x64ShowPicture(0, 48, 16, 16, motion_16x16);
    
    halOLED128x64ShowPicture(36, 0, 32, 32, apple_32x32);
    halOLED128x64ShowPicture(36, 32, 32, 32, toxic_32x32);
  
    halOLED128x64ShowPicture(76, 0, 16, 16, zigbee_connected);
    halOLED128x64ShowPicture(76, 24, 16, 16, zigbee_disconnected);
    halOLED128x64ShowPicture(76, 48, 16, 16, zigbee_image);
  
    halOLED128x64ShowIcon(96, 1, 8, 0);
    halOLED128x64ShowIcon(96, 3, 7, 0);
    
    
    halOLED128x64ShowX8(4, 112, "Ё");
    halOLED128x64ShowX16(3, 112, "Ё");
  }
#endif

#if HAL_LCD_TYPE == HAL_LCD_TYPE_TFT
  void TFTDraw(void)
  {
    //-- !! halTFTShowX8:  26x10 chars in row !!
    //-- !! halTFTShowX16: 20x5 chars in row !!
    
    //-- ASCII table (8x16)
    halTFTSetScreen(PX_BLACK);
    halTFTShowX16(0,  0, PX_RED,    PX_BLACK, "ABCDEFGHIJKLMNOPQRST");
    halTFTShowX16(0, 16, PX_GREEN,  PX_BLACK, "UVWXYZabcdefghijklmn");
    halTFTShowX16(0, 32, PX_BLUE,   PX_BLACK, "opqrstuvwxyz01234567");
    halTFTShowX16(0, 48, PX_WHITE,  PX_BLACK, "89.,\"'?!@_*#$%&()+-/");
    halTFTShowX16(0, 64, PX_YELLOW, PX_BLACK, ":;<=>[\\]^`{|}~");
  
    //-- Russian table (8x16)
    delayMs32MHZ(4000);
    halTFTSetScreen(PX_BLACK);
    halTFTShowX16(0,   0, PX_RED,    PX_BLACK, "АБВГДЕЖЗИЙКЛМНОПРСТУ");
    halTFTShowX16(0,  16, PX_GREEN,  PX_BLACK, "ФХЦЧШЩЪЫЬЭЮЯабвгдежз");
    halTFTShowX16(0,  32, PX_BLUE,   PX_BLACK, "ийклмнопрстуфхцчшщъы");
    halTFTShowX16(0,  48, PX_WHITE,  PX_BLACK, "ьэюяЁё °C");
    
    //-- ASCII table (8x8)
    delayMs32MHZ(4000);
    halTFTSetScreen(PX_BLACK);
    halTFTShowX8(0,  0, PX_RED,    PX_BLACK, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    halTFTShowX8(0, 16, PX_GREEN,  PX_BLACK, "abcdefghijklmnopqrstuvwxyz");
    halTFTShowX8(0, 32, PX_BLUE,   PX_BLACK, "0123456789.,\"'?!@_*#$%&()+");
    halTFTShowX8(0, 48, PX_WHITE,  PX_BLACK, "-/:;<=>[\\]^`{|}~");
    
    //-- Russian table (8x8)
    delayMs32MHZ(4000);
    halTFTSetScreen(PX_BLACK);
    halTFTShowX8(0,  0, PX_RED,    PX_BLACK, "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩ");
    halTFTShowX8(0, 16, PX_GREEN,  PX_BLACK, "ЪЫЬЭЮЯабвгдежзийклмнопрст");
    halTFTShowX8(0, 32, PX_BLUE,   PX_BLACK, "уфхцчшщъыьэюяЁё °C");
    
    //-- Icons table
    delayMs32MHZ(4000);
    halTFTSetScreen(PX_BLACK);
    halTFTShowIcon(0,  0, PX_RED,     PX_BLACK, 7, 0);
    halTFTShowIcon(0,  8, PX_GREEN,   PX_BLACK, 8, 0);
    halTFTShowIcon(0, 16, PX_BLUE,    PX_BLACK, 7, 1);
    halTFTShowIcon(0, 24, PX_WHITE,   PX_BLACK, 8, 1);
    halTFTShowIcon(0, 32, PX_YELLOW,  PX_BLACK, 7, 2);
    halTFTShowIcon(0, 40, PX_GRAY,    PX_BLACK, 8, 2);
    halTFTShowIcon(0, 48, PX_CYAN,    PX_BLACK, 7, 3);
    halTFTShowIcon(0, 56, PX_MAGENTA, PX_BLACK, 8, 3);
    halTFTShowIcon(0, 64, PX_BLACK,   PX_WHITE, 7, 4);
    
    //-- Pictures
    delayMs32MHZ(4000);
    halTFTSetScreen(PX_BLACK);
    halTFTShowPicture(0, 0, 128, 64, zigbee_logo2);
    
    //-- Composition of symbols & pictures
    delayMs32MHZ(4000);
    halTFTSetScreen(PX_BLACK);
    halTFTShowX16(112, 0, PX_RED, PX_BLACK, "А");
    halTFTShowX8(112, 64, PX_GREEN, PX_BLACK, "А");
    
    halTFTShowIcon(20, 20, PX_BLUE, PX_BLACK, 8, 0);
    halTFTShowIcon(20, 40, PX_YELLOW, PX_BLACK, 7, 0);
  
    halTFTShowPicture(0, 8, 16, 16, danger_16x16);
    halTFTShowPicture(0, 24, 16, 16, empty_16x16);
    halTFTShowPicture(0, 48, 16, 16, motion_16x16);
    
    halTFTShowPicture(36, 0, 32, 32, apple_32x32);
    halTFTShowPicture(36, 32, 32, 32, toxic_32x32);
  
    halTFTShowPicture(76, 0, 16, 16, zigbee_connected);
    halTFTShowPicture(76, 24, 16, 16, zigbee_disconnected);
    halTFTShowPicture(76, 48, 16, 16, zigbee_image);
  
    halTFTShowIcon(96, 16, PX_RED, PX_BLACK, 8, 0);
    halTFTShowIcon(96, 48, PX_GREEN, PX_BLACK, 7, 0);
    
    halTFTShowX8(112,  24, PX_BLUE, PX_BLACK, "Ё");
    halTFTShowX16(112, 40, PX_YELLOW, PX_BLACK, "Ё");
    
    //halTFTShowPicture(32, 0, 40, 40, Picture_40x40_WeiXinIco);
    //halTFTShowPicture(32, 40, 40, 40, Picture_40x40_ITunesIco);

    //-- final picture
    delayMs32MHZ(4000);
    halTFTSetScreen(PX_BLACK);
    halTFTShowPicture(0, 0, 80, 80, Picture_160x80_Sea);
  }
#endif

uint8 req;
void zclcc2530_ReportDHT11(void)
{
  char t[50], h[50], i[50];

  req = halDHT11GetData();

  if(req) {
    if(errorMsg(req)) {
      return;
    }
    
    printf(FONT_COLOR_STRONG_GREEN);
    printf("DHT11 Initiated\n");
    printf(STYLE_COLOR_RESET);
    
    #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
      halOLED128x64ClearScreen();
    #else
      halTFTSetScreen(PX_BLACK);
    #endif

    dht11Idx++;
    sprintf(i, "Idx: %d", dht11Idx);
    sprintf(t, "Temp: %d%d.%d°C", tempH, tempL, tempDec);
    if(humiDec > 0) {
      sprintf(h, "Humi: %d%d.%d%%", humiH, humiL, humiDec);
    } else {
      sprintf(h, "Humi: %d%d%%", humiH, humiL);
    }

    #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
      halOLED128x64ShowX16(0, 0, (uint8 const *)i);
      halOLED128x64ShowX16(1, 0, (uint8 const *)t);
      halOLED128x64ShowX16(2, 0, (uint8 const *)h);
    #else
      halTFTShowX16(0, 0,  PX_WHITE, PX_BLACK, (uint8 const *)i);
      halTFTShowX16(0, 16, PX_RED,   PX_BLACK, (uint8 const *)t);
      halTFTShowX16(0, 32, PX_GREEN, PX_BLACK, (uint8 const *)h);
    #endif

    //-- Output
    printf(FONT_COLOR_STRONG_YELLOW);
    printf("%s, %s, %s\n", i, t, h);
    printf(STYLE_COLOR_RESET);
    
  } else {
    
    printf(FONT_COLOR_STRONG_RED);
    printf("DHT11 NOT Initiated!\n");
    printf(STYLE_COLOR_RESET);

  }
}
