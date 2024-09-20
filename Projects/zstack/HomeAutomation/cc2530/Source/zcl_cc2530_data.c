#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_ms.h"

/* TODO: Дополняйте нужные заголовки для соответствующих кластеров
#include "zcl_poll_control.h"
#include "zcl_electrical_measurement.h"
#include "zcl_diagnostic.h"
#include "zcl_meter_identification.h"
#include "zcl_appliance_identification.h"
#include "zcl_appliance_events_alerts.h"
#include "zcl_power_profile.h"
#include "zcl_appliance_control.h"
#include "zcl_appliance_statistics.h"
#include "zcl_hvac.h"
*/

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

// Состояние реле
extern uint8 RELAY_STATE;

// Данные о температуре
#define MAX_MEASURED_VALUE  10000  // 100.00C
#define MIN_MEASURED_VALUE  -10000  // -100.00C

extern int16 zclcc2530_MeasuredValue;
const int16 zclcc2530_MinMeasuredValue = MIN_MEASURED_VALUE; 
const uint16 zclcc2530_MaxMeasuredValue = MAX_MEASURED_VALUE;

// Таблица реализуемых команд для DISCOVER запроса
#if ZCL_DISCOVER
CONST zclCommandRec_t zclcc2530_Cmds[] =
{
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    COMMAND_BASIC_RESET_FACT_DEFAULT,
    CMD_DIR_SERVER_RECEIVED
  },
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    COMMAND_OFF,
    CMD_DIR_SERVER_RECEIVED
  },
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    COMMAND_OFF,
    CMD_DIR_SERVER_RECEIVED
  },
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    COMMAND_ON,
    CMD_DIR_SERVER_RECEIVED
  },
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    COMMAND_TOGGLE,
    CMD_DIR_SERVER_RECEIVED
  },
};

CONST uint8 zclCmdsArraySize = ( sizeof(zclcc2530_Cmds) / sizeof(zclcc2530_Cmds[0]) );
#endif // ZCL_DISCOVER


// Определение атрибутов приложения
CONST zclAttrRec_t zclcc2530_Attrs[] =
{
  // *** Атрибуты Basic кластера ***
  {
    ZCL_CLUSTER_ID_GEN_BASIC,             // ID кластера - определен в zcl.h
    { // версия оборудования
      ATTRID_BASIC_HW_VERSION,            // ID атрибута - определен в zcl_general.h
      ZCL_DATATYPE_UINT8,                 // Тип данных  - определен zcl.h
      ACCESS_CONTROL_READ,                // Тип доступа к атрибута - определен в zcl.h
      (void *)&zclcc2530_HWRevision     // Указатель на переменную хранящую значение
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия ZCL
      ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_ZCLVersion
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия приложения
      ATTRID_BASIC_APPL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_ZCLVersion
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия стека
      ATTRID_BASIC_STACK_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_ZCLVersion
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия прошивки
      ATTRID_BASIC_SW_BUILD_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_DateCode
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // производитель
      ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_ManufacturerName
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // модель
      ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_ModelId
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // дата версии
      ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_DateCode
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // тип питания
      ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_ENUM8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_PowerSource
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // расположение
      ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE), // может быть изменен
      (void *)zclcc2530_LocationDescription
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_ENUM8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclcc2530_PhysicalEnvironment
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_DEVICE_ENABLED,
      ZCL_DATATYPE_BOOLEAN,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclcc2530_DeviceEnable
    }
  },

#ifdef ZCL_IDENTIFY
  // *** Атрибуты Identify кластера ***
  {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    { // время идентификации
      ATTRID_IDENTIFY_TIME,
      ZCL_DATATYPE_UINT16,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclcc2530_IdentifyTime
    }
  },
#endif
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия Basic кластера
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    { // версия Identify кластера
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_clusterRevision_all
    }
  },
  // *** Атрибуты On/Off кластера ***
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    { // состояние
      ATTRID_ON_OFF,
      ZCL_DATATYPE_BOOLEAN,
      ACCESS_CONTROL_READ,
      (void *)&RELAY_STATE
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    {  // версия On/Off кластера
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ | ACCESS_CLIENT,
      (void *)&zclcc2530_clusterRevision_all
    }
  },
  // *** Атрибуты Temperature Measurement кластера ***
  {
    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
    { // Значение температуры
      ATTRID_MS_TEMPERATURE_MEASURED_VALUE,
      ZCL_DATATYPE_INT16,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void *)&zclcc2530_MeasuredValue
    }
  },
  {
    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
    { // минимальное значение температуры
      ATTRID_MS_TEMPERATURE_MIN_MEASURED_VALUE,
      ZCL_DATATYPE_INT16,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_MinMeasuredValue
    }
  },
  {
    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
    { // максимальное значение температуры
      ATTRID_MS_TEMPERATURE_MAX_MEASURED_VALUE,
      ZCL_DATATYPE_INT16,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_MaxMeasuredValue
    }
  },
  {
    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
    {  // версия кластера
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_clusterRevision_all
    }
  },
};

uint8 CONST zclcc2530_NumAttributes = ( sizeof(zclcc2530_Attrs) / sizeof(zclcc2530_Attrs[0]) );

// Список входящих кластеров приложения
const cId_t zclcc2530_InClusterList[] =
{
  ZCL_CLUSTER_ID_GEN_BASIC,
  ZCL_CLUSTER_ID_GEN_IDENTIFY,
  ZCL_CLUSTER_ID_GEN_GROUPS,
  ZCL_CLUSTER_ID_GEN_ON_OFF,
  ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
  
  // TODO: Add application specific Input Clusters Here. 
  //       See zcl.h for Cluster ID definitions
  
};
#define ZCLcc2530_MAX_INCLUSTERS   (sizeof(zclcc2530_InClusterList) / sizeof(zclcc2530_InClusterList[0]))

// Список исходящих кластеров приложения
const cId_t zclcc2530_OutClusterList[] =
{
  ZCL_CLUSTER_ID_GEN_BASIC,
  
  // TODO: Add application specific Output Clusters Here. 
  //       See zcl.h for Cluster ID definitions
};
#define ZCLcc2530_MAX_OUTCLUSTERS  (sizeof(zclcc2530_OutClusterList) / sizeof(zclcc2530_OutClusterList[0]))

// Структура описания эндпоинта
SimpleDescriptionFormat_t zclcc2530_SimpleDesc =
{
  cc2530_ENDPOINT,                  //  int Endpoint;
  ZCL_HA_PROFILE_ID,                  //  uint16 AppProfId;
  // TODO: Replace ZCL_HA_DEVICEID_ON_OFF_LIGHT with application specific device ID
  ZCL_HA_DEVICEID_ON_OFF_LIGHT,       //  uint16 AppDeviceId; 
  cc2530_DEVICE_VERSION,            //  int   AppDevVer:4;
  cc2530_FLAGS,                     //  int   AppFlags:4;
  ZCLcc2530_MAX_INCLUSTERS,         //  byte  AppNumInClusters;
  (cId_t *)zclcc2530_InClusterList, //  byte *pAppInClusterList;
  ZCLcc2530_MAX_OUTCLUSTERS,        //  byte  AppNumInClusters;
  (cId_t *)zclcc2530_OutClusterList //  byte *pAppInClusterList;
};

// Сброс атрибутов в значения по-умолчанию  
void zclcc2530_ResetAttributesToDefaultValues(void)
{
  int i;
  
  zclcc2530_LocationDescription[0] = 16;
  for (i = 1; i <= 16; i++)
  {
    zclcc2530_LocationDescription[i] = ' ';
  }
  
  zclcc2530_PhysicalEnvironment = PHY_UNSPECIFIED_ENV;
  zclcc2530_DeviceEnable = DEVICE_ENABLED;
  
#ifdef ZCL_IDENTIFY
  zclcc2530_IdentifyTime = 0;
#endif
}