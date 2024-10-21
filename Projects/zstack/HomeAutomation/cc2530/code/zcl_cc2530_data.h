//#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_ms.h"

#include "zcl_cc2530.h"

// версия устройства и флаги
#define cc2530_DEVICE_VERSION     0
#define cc2530_FLAGS              0

// версия оборудования
#define cc2530_HWVERSION          1
// версия ZCL
#define cc2530_ZCLVERSION         1

// версия кластеров
const uint16 zclcc2530_clusterRevision_all = 0x0001; 

// переменные/константы Basic кластера

// версия оборудования
const uint8 zclcc2530_HWRevision = cc2530_HWVERSION;
// версия ZCL
const uint8 zclcc2530_ZCLVersion = cc2530_ZCLVERSION;
// производитель
const uint8 zclcc2530_ManufacturerName[] = {17, 'T','e','x','a','s',' ','I','n','s','t','r','u','m','e','n','t','s'};
// модель устройства
const uint8 zclcc2530_ModelId[] = {15, 'M','I','K','E','.','C','C','2','5','3','0','.','I','A','R'};
// дата версии
const uint8 zclcc2530_DateCode[] = {8, '2','0','2','4','0','9','1','2'};
// вид питания POWER_SOURCE_MAINS_1_PHASE - питание от сети с одной фазой
const uint8 zclcc2530_PowerSource = POWER_SOURCE_MAINS_1_PHASE;
// расположение устройства
//uint8 zclcc2530_LocationDescription[17] = {16, ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
uint8 zclcc2530_LocationDescription[19] = {18, 'B','a','s','e','d',' ','o','n',' ','T','I',' ','C','C','2','5','3','0'};
uint8 zclcc2530_PhysicalEnvironment = 0;
uint8 zclcc2530_DeviceEnable = DEVICE_ENABLED;

// переменные/константы Identify кластера

// время идентификации
uint16 zclcc2530_IdentifyTime;

//-- NVM IDs
#define NV_cc2530_RELAY_STATE_ID_1      0xFFF1
#define NV_cc2530_RELAY_STATE_ID_2      0xFFF2
#define NV_cc2530_RELAY_STATE_ID_3      0xFFF3
#define NV_cc2530_RELAY_STATE_ID_4      0xFFF4

//-- Relays state
extern uint8 RELAY_STATES[4] = {0,0,0,0};
extern uint8 TOTAL_RELAYS_NUM = 4;

//-- Device endpoint numbers
#define cc2530_ENDPOINT_1               1
#define cc2530_ENDPOINT_2               2
#define cc2530_ENDPOINT_3               3
#define cc2530_ENDPOINT_4               4

#if USE_DS18B20 || USE_DHT11
	// Данные о температуре
	#define MAX_MEASURED_VALUE  10000  // 100.00C
	#define MIN_MEASURED_VALUE  -10000  // -100.00C
	extern int16 zclcc2530_TemperatureMeasuredValue;
	const int16 zclcc2530_MinMeasuredValue = MIN_MEASURED_VALUE; 
	const uint16 zclcc2530_MaxMeasuredValue = MAX_MEASURED_VALUE;
#endif

#if USE_LOCAL_TIME
  extern int16 zclcc2530_LocalTimeValue;
  extern void zclcc2530_ReportTime(void);
#endif

// Выход из сети
void zclcc2530_LeaveNetwork(void);
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

// Список входящих кластеров EP1
const cId_t zclcc2530_InClusterList1[] =
{
  ZCL_CLUSTER_ID_GEN_BASIC,
  ZCL_CLUSTER_ID_GEN_IDENTIFY,
  ZCL_CLUSTER_ID_GEN_GROUPS,
  ZCL_CLUSTER_ID_GEN_ON_OFF,
  #if USE_DS18B20 || USE_DHT11
  	ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
  #endif
  ZCL_CLUSTER_ID_GEN_TIME,
};
#define ZCLcc2530_MAX_INCLUSTERS1   (sizeof(zclcc2530_InClusterList1) / sizeof(zclcc2530_InClusterList1[0]))

// Список исходящих кластеров EP1
const cId_t zclcc2530_OutClusterList1[] =
{
  ZCL_CLUSTER_ID_GEN_BASIC,
  ZCL_CLUSTER_ID_GEN_TIME,
};
#define ZCLcc2530_MAX_OUTCLUSTERS1  (sizeof(zclcc2530_OutClusterList1) / sizeof(zclcc2530_OutClusterList1[0]))

// Список входящих кластеров for other EP
const cId_t zclcc2530_InClusterList[] =
{
  ZCL_CLUSTER_ID_GEN_ON_OFF,
};
#define ZCLcc2530_MAX_INCLUSTERS   (sizeof(zclcc2530_InClusterList) / sizeof(zclcc2530_InClusterList[0]))

// Список исходящих кластеров for other EPs
const cId_t zclcc2530_OutClusterList[] = {NULL};
#define ZCLcc2530_MAX_OUTCLUSTERS  (sizeof(zclcc2530_OutClusterList) / sizeof(zclcc2530_OutClusterList[0]))


// Сброс атрибутов в значения по-умолчанию
void zclcc2530_ResetAttributesToDefaultValues(void);
void zclcc2530_ResetAttributesToDefaultValues(void)
{
  int i;
  
  zclcc2530_LocationDescription[0] = 16;
  for (i = 1; i <= 16; i++) {
    zclcc2530_LocationDescription[i] = ' ';
  }
  
  zclcc2530_PhysicalEnvironment = PHY_UNSPECIFIED_ENV;
  zclcc2530_DeviceEnable = DEVICE_ENABLED;
  
	#ifdef ZCL_IDENTIFY
  	zclcc2530_IdentifyTime = 0;
	#endif
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
//-- Обработчик команды сброса в Basic кластере
static void zclcc2530_BasicResetCB(void)
{
  /* TODO: remember to update this function with any application-specific cluster attribute variables */
  zclcc2530_ResetAttributesToDefaultValues();
}
// Обработчик изменения времени идентификации
static void zclcc2530_ProcessIdentifyTimeChange(uint8 endpoint)
{
  (void) endpoint;

  if (zclcc2530_IdentifyTime > 0) {
    //HalLedBlink (HAL_LED_2, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
  } else {
    //HalLedSet (HAL_LED_2, HAL_LED_MODE_OFF);
  }
}
// Обработчик изменения статусов соединения с сетью
static void zclcc2530_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg)
{
  switch(bdbCommissioningModeMsg->bdbCommissioningMode)
  {
    case BDB_COMMISSIONING_FORMATION:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
        //After formation, perform nwk steering again plus the remaining commissioning modes that has not been process yet
        bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | bdbCommissioningModeMsg->bdbRemainingCommissioningModes);
      } else {
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_NWK_STEERING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
        //YOUR JOB:
        //We are on the nwk, what now?
        #if USE_LOCAL_TIME
        	zclcc2530_ReportTime();
        #endif
      } else {
        //See the possible errors for nwk steering procedure
        //No suitable networks found
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_FINDING_BINDING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
        //YOUR JOB:
      } else {
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
        if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED) {
          //We did recover from losing parent
        } else {
          //Parent not found, attempt to rejoin again after a fixed delay
          osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_END_DEVICE_REJOIN, TIMER_INTERVAL_ENDDEVICE_REJOIN);
        }
      break;
    #endif 
  }
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
    /*
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
    */
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
    for (i = 0; i < writeRspCmd->numAttr; i++) {
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


#include "zcl/zcl_cc2530_attr1.h"
#include "zcl/zcl_cc2530_attr2.h"
#include "zcl/zcl_cc2530_attr3.h"
#include "zcl/zcl_cc2530_attr4.h"
