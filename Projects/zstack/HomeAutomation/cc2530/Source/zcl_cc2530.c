#include <stdio.h>
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

//-- extra libs
#include "ds18b20.h"
#include "colors.h"
#include "cc2530_io_ports.h"

#include <string.h>
#include "utils.h"

//-- OLED
#include "hal_oled12864.h"
#include "font_v_picture.h"
#include "cc2530_io_ports.h"


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

//-- Init & Draw SSD1306 OLED
void SSD1306Draw(void);

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
  osal_start_reload_timer(zclcc2530_TaskID, HAL_KEY_EVENT, TIMER_INTERVAL_HAL_KEY_EVENT);
  
  //-- запускаем повторяемый таймер для информирования о температуре (мс)
  osal_start_reload_timer(zclcc2530_TaskID, cc2530_REPORTING_EVT, TIMER_INTERVAL_REPORTING_EVT);
  
  // Старт процесса возвращения в сеть
  bdb_StartCommissioning(BDB_COMMISSIONING_MODE_PARENT_LOST);
  
  //-- init UART to use "printf" for serial monitor
  HalUARTInit();
  uint8 state = initUart0(uart0RxCb);

  printf(FONT_COLOR_GREEN);
  printf("\nUART initiated\n");
  printf(STYLE_COLOR_RESET);

  SSD1306Draw();

  /*
  halOLED12864ClearScreen();
  halOLED12864ShowX16(0, 0, "CC2530!МАЙК!IAR");
  halOLED12864ShowX16(1, 0, "---------------");
  halOLED12864ShowX16(2, 0, "UART initiated!");
  */

  //-- Test1 - Show 8x16 Char
  halOLED12864ClearScreen();
  //halOLED12864ShowX16(0, 0, "9ЁёАБ");
  /*
  halOLED12864ShowX16(0, 0, "123456789012345678901234567890");
  halOLED12864ShowX16(1, 0, "1прЁё");
  halOLED12864ShowX16(2, 0, "1прЁё");
  halOLED12864ShowX16(3, 0, "1прЁё");
  */
  halOLED12864ShowX16(0, 0, "АБВГДЕЖЗИЙКЛМНОП");
	halOLED12864ShowX16(1, 0, "РСТУФХЦЧШЩЪЫЬЭЮЯ");
	halOLED12864ShowX16(2, 0, "абвгдежзийклмноп");
	halOLED12864ShowX16(3, 0, "рстуфхцчшщъыьэюя");
	halOLED12864ShowX16(3, 0, "Ёё");

	halOLED12864ShowX16(0, 0, "ABCDEFGHIJKLMNOP");
	halOLED12864ShowX16(1, 0, "QRSTUVWXYZabcdef");
	halOLED12864ShowX16(2, 0, "ghijklmnopqrstuv");
	halOLED12864ShowX16(3, 0, "wxyz0123456789.,");

	//halOLED12864ShowX16(3, 0, "aaaaaaaaaaaaaaaa");

	halOLED12864ShowX16(2, 0, "\"'?!@_*#$%&()+-/");
	halOLED12864ShowX16(3, 0, ":;<=>[\\]^`{|}~");


  /*
  //-- RUS #1
  А - 208:144 = 1040 => 0
  Й - 208:153 = 1049
  Я - 208:175 = 1071
  а - 208:176 = 1072
  й - 208:185 = 1081
  п - 208:191 = 1087 => 47
  
  //-- RUS #2
  р - 209:128 = 1088 => 0
  я - 209:143 = 1103 => 15
  
  //-- RUS #3
  Ё - 208:129 = 1025
  ё - 209:145 = 1105
  */

  //halOLED12864ShowX16(0, 0, "0123456789");
  //halOLED12864ShowX16(1, 0, "abcdefghiABCDE");
  //halOLED12864ShowX16(2, 0, "{}[]()!@#$%-_/");
  //halOLED12864ShowX16(3, 0, "абвгдеёйАБВГЁЙ");

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
            
            printf(FONT_COLOR_GREEN);
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
      
      printf(FONT_COLOR_RED);
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
      osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_BLINK, TIMER_INTERVAL_EVT_BLINK);
      
      printf(FONT_COLOR_YELLOW);
      printf("Start Commissioning...\n");
  		printf(STYLE_COLOR_RESET);
    }
    
    return (events ^ cc2530_EVT_LONG);
  }
  
  // событие cc2530_REPORTING_EVT
  if(events & cc2530_REPORTING_EVT) {
    
    zclcc2530_ReportTemp();
    
    return (events ^ cc2530_REPORTING_EVT);
  }
  
  // событие опроса кнопок
  if (events & HAL_KEY_EVENT)
  {
    /* Считывание кнопок */
    cc2530_HalKeyPoll();

    return events ^ HAL_KEY_EVENT;
  }
  
  // Отбросим необработаные сообщения
  return 0;
}


// Обработчик нажатий клавиш
static void zclcc2530_HandleKeys(byte shift, byte keys)
{
  if(keys & HAL_KEY_SW_1) {
    //-- Запускаем таймер для определения долгого нажания 5сек
    osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_LONG, 5000);
    //-- Переключаем реле
    updateRelay(RELAY_STATE == 0);
    halOLED12864ShowX16(2, 0, "Key #1 pressed");
  } else {
    //-- Останавливаем таймер ожидания долгого нажатия
    osal_stop_timerEx(zclcc2530_TaskID, cc2530_EVT_LONG);
  }

  if(keys & HAL_KEY_SW_2) {
  	printf("Key #2 pressed\n");
  	HalLedSet(HAL_LED_3, HAL_LED_MODE_TOGGLE);
  	halOLED12864ShowX16(2, 0, "Key #2 pressed");
  }

  if(keys & HAL_KEY_SW_3) {
  	P0_4 = (P0_4 == 0) ? 1 : 0;
  	printf("Key #3 pressed: %d\n", P0_4);
  	halOLED12864ShowX16(2, 0, "Key #3 pressed");
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
        osal_start_timerEx(zclcc2530_TaskID, cc2530_END_DEVICE_REJOIN_EVT, cc2530_END_DEVICE_REJOIN_DELAY);
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

  printf(FONT_COLOR_YELLOW);
  printf("DS18B20 sensor: ");
  
  printf(STYLE_COLOR_BOLD);
  printf(FONT_COLOR_CYAN);
  printNumber(number, 2);

  printf(FONT_COLOR_YELLOW);
  printf(" °С\n");
  printf(STYLE_COLOR_RESET);
  */

  /*
  char buffer2[sizeof(int) * 8 + 1];
  char* str2 = itoa(number, buffer2);
  halOLED12864ShowX16(3, 0, "DS18B20:");
  halOLED12864ShowX16(3, 9, str2);
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

void SSD1306Draw(void)
{
  /*
  //-- LED initiated as output...
  CC2530_IO_OUTPUT(1, 2);
  P1_2 = 1;

  CC2530_IO_OUTPUT(1, 3);
  P1_3 = 1;

  CC2530_IO_OUTPUT(1, 7);
  P1_7 = 1;

  CC2530_IO_OUTPUT(0, 0);
  P0_0 = 1;
  
  CC2530_IO_OUTPUT(0, 4);
  P0_4 = 1;
  */

  setSystemClk32MHZ();

  halOLED12864Init();

  /*
  //-- Test1 - Show 8x16 Char
  halOLED12864ShowX16(0, 0, "0123456789");
  halOLED12864ShowX16(1, 0, "abcdefghiABCDE");
  halOLED12864ShowX16(2, 0, "{}[]()!@#$%");
  halOLED12864ShowX16(3, 0, "абвгдеёйАБВГЁЙ");

  delayMs32MHZ(4000);
  halOLED12864ClearScreen();

  //-- Test2 - Show 8x16 Char and 16x16 Chinese Char
  halOLED12864ShowX16(0, 0,  "�������£�");
  halOLED12864ShowX16(1, 30, "�¶ȣ�22 ��");
  halOLED12864ShowX16(2, 30, "ʪ�ȣ�30 %");

  delayMs32MHZ(4000);
  halOLED12864ClearScreen();

  //-- Test3 - Show 32x32 Picture in point: (30, 30)
  halOLED12864ShowPicture(30, 30, 32, 32, Picture_32x32_AppleIco);

  delayMs32MHZ(4000);
  halOLED12864ClearScreen();

  //-- Test4 - Show 128x64 Picture in point: (0, 0)
  halOLED12864ShowPicture(0, 0, 128, 64, Picture_128x128_SuccessPic);
  */
}
