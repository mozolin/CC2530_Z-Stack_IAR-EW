#include "zcl_cc2530_attr1_extra.h"

// Структура описания эндпоинта
SimpleDescriptionFormat_t zclcc2530_SimpleDesc1 =
{
  cc2530_ENDPOINT_1,                 //  int Endpoint;
  ZCL_HA_PROFILE_ID,                 //  uint16 AppProfId;
  // TODO: Replace ZCL_HA_DEVICEID_ON_OFF_LIGHT with application specific device ID
  ZCL_HA_DEVICEID_ON_OFF_LIGHT,      //  uint16 AppDeviceId; 
  cc2530_DEVICE_VERSION,             //  int   AppDevVer:4;
  cc2530_FLAGS,                      //  int   AppFlags:4;
  ZCLcc2530_MAX_INCLUSTERS1,         //  byte  AppNumInClusters;
  (cId_t *)zclcc2530_InClusterList1, //  byte *pAppInClusterList;
  ZCLcc2530_MAX_OUTCLUSTERS1,        //  byte  AppNumInClusters;
  (cId_t *)zclcc2530_OutClusterList1 //  byte *pAppInClusterList;
};

/*********************************************************************
 * Таблица обработчиков основных ZCL команд
 */
static zclGeneral_AppCallbacks_t zclcc2530_CmdCallbacks1 =
{
  zclcc2530_BasicResetCB, // Basic Cluster Reset command
  NULL,                   // Identify Trigger Effect command
  zclcc2530_OnOffCB1,     // On/Off cluster commands
  NULL,                   // On/Off cluster enhanced command Off with Effect
  NULL,                   // On/Off cluster enhanced command On with Recall Global Scene
  NULL,                   // On/Off cluster enhanced command On with Timed Off
  #ifdef ZCL_LEVEL_CTRL
    NULL,                 // Level Control Move to Level command
    NULL,                 // Level Control Move command
    NULL,                 // Level Control Step command
    NULL,                 // Level Control Stop command
  #endif
  #ifdef ZCL_GROUPS
    NULL,                 // Group Response commands
  #endif
  #ifdef ZCL_SCENES
    NULL,                 // Scene Store Request command
    NULL,                 // Scene Recall Request command
    NULL,                 // Scene Response command
  #endif
  #ifdef ZCL_ALARMS
    NULL,                 // Alarm (Response) commands
  #endif
  #ifdef SE_UK_EXT
    NULL,                 // Get Event Log command
    NULL,                 // Publish Event Log command
  #endif
  NULL,                   // RSSI Location command
  NULL                    // RSSI Location Response command
};

// Определение атрибутов приложения
CONST zclAttrRec_t zclcc2530_Attrs1[] =
{
  // *** Атрибуты Basic кластера ***
  {//1
    ZCL_CLUSTER_ID_GEN_GROUPS,
    //ZCL_CLUSTER_ID_GEN_BASIC,             // ID кластера - определен в zcl.h
    { // версия оборудования
      ATTRID_BASIC_HW_VERSION,            // ID атрибута - определен в zcl_general.h
      ZCL_DATATYPE_UINT8,                 // Тип данных  - определен zcl.h
      ACCESS_CONTROL_READ,                // Тип доступа к атрибута - определен в zcl.h
      (void *)&zclcc2530_HWRevision     // Указатель на переменную хранящую значение
    }
  },
  {//2
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия ZCL
      ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_ZCLVersion
    }
  },
  {//3
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия приложения
      ATTRID_BASIC_APPL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_ZCLVersion
    }
  },
  {//4
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия стека
      ATTRID_BASIC_STACK_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_ZCLVersion
    }
  },
  {//5
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия прошивки
      ATTRID_BASIC_SW_BUILD_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_DateCode
    }
  },
  {//6
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // производитель
      ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_ManufacturerName
    }
  },
  {//7
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // модель
      ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_ModelId
    }
  },
  {//8
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // дата версии
      ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclcc2530_DateCode
    }
  },
  {//9
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // тип питания
      ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_ENUM8,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_PowerSource
    }
  },
  {//10
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // расположение
      ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE), // может быть изменен
      (void *)zclcc2530_LocationDescription
    }
  },
  {//11
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_ENUM8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclcc2530_PhysicalEnvironment
    }
  },
  {//12
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
    {//13
      ZCL_CLUSTER_ID_GEN_IDENTIFY,
      { // время идентификации
        ATTRID_IDENTIFY_TIME,
        ZCL_DATATYPE_UINT16,
        (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
        (void *)&zclcc2530_IdentifyTime
      }
    },
	#endif
  {//14
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // версия Basic кластера
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_clusterRevision_all
    }
  },
  {//15
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    { // версия Identify кластера
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclcc2530_clusterRevision_all
    }
  },
  // *** Атрибуты On/Off кластера ***
  {//16
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    { // состояние
      ATTRID_ON_OFF,
      ZCL_DATATYPE_BOOLEAN,
      ACCESS_CONTROL_READ,
      (void *)&RELAY_STATES[0]
    }
  },
  {//17
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    {  // версия On/Off кластера
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ | ACCESS_CLIENT,
      (void *)&zclcc2530_clusterRevision_all
    }
  },
  #if USE_DS18B20 || USE_DHT11
    // *** Атрибуты Temperature Measurement кластера ***
    {//18
      ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
      { // Значение температуры
        ATTRID_MS_TEMPERATURE_MEASURED_VALUE,
        ZCL_DATATYPE_INT16,
        ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
        (void *)&zclcc2530_TemperatureMeasuredValue
      }
    },
    {//19
      ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
      { // минимальное значение температуры
        ATTRID_MS_TEMPERATURE_MIN_MEASURED_VALUE,
        ZCL_DATATYPE_INT16,
        ACCESS_CONTROL_READ,
        (void *)&zclcc2530_MinMeasuredValue
      }
    },
    {//20
      ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
      { // максимальное значение температуры
        ATTRID_MS_TEMPERATURE_MAX_MEASURED_VALUE,
        ZCL_DATATYPE_INT16,
        ACCESS_CONTROL_READ,
        (void *)&zclcc2530_MaxMeasuredValue
      }
    },
    {//21
      ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
      {  // версия кластера
        ATTRID_CLUSTER_REVISION,
        ZCL_DATATYPE_UINT16,
        ACCESS_CONTROL_READ,
        (void *)&zclcc2530_clusterRevision_all
      }
    },
  #endif
  #if USE_LOCAL_TIME
    {//22
      ZCL_CLUSTER_ID_GEN_TIME,
      {  // time
        ATTRID_TIME_TIME,
        ZCL_DATATYPE_UTC,
        (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE), //-- can be changed
        (void *)&zclcc2530_LocalTimeValue
      }
    },
  #endif
};

uint8 CONST zclcc2530_NumAttributes1 = ( sizeof(zclcc2530_Attrs1) / sizeof(zclcc2530_Attrs1[0]) );
