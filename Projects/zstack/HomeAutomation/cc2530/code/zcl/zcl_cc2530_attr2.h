
// Структура описания эндпоинта
SimpleDescriptionFormat_t zclcc2530_SimpleDesc2 =
{
  cc2530_ENDPOINT_2,                //  int Endpoint;
  ZCL_HA_PROFILE_ID,                //  uint16 AppProfId;
  ZCL_HA_DEVICEID_ON_OFF_LIGHT,     //  uint16 AppDeviceId; 
  cc2530_DEVICE_VERSION,            //  int   AppDevVer:4;
  cc2530_FLAGS,                     //  int   AppFlags:4;
  ZCLcc2530_MAX_INCLUSTERS,         //  byte  AppNumInClusters;
  (cId_t *)zclcc2530_InClusterList, //  byte *pAppInClusterList;
  ZCLcc2530_MAX_OUTCLUSTERS,        //  byte  AppNumOutClusters;
  (cId_t *)zclcc2530_OutClusterList //  byte *pAppOutClusterList;
};

/*********************************************************************
 * Таблица обработчиков основных ZCL команд
 */
static zclGeneral_AppCallbacks_t zclcc2530_CmdCallbacks2 =
{
  zclcc2530_BasicResetCB, // Basic Cluster Reset command
  NULL,                   // Identify Trigger Effect command
  zclcc2530_OnOffCB2,     // On/Off cluster commands
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
CONST zclAttrRec_t zclcc2530_Attrs2[] =
{
  // *** Атрибуты On/Off кластера ***
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    { // состояние
      ATTRID_ON_OFF,
      ZCL_DATATYPE_BOOLEAN,
      ACCESS_CONTROL_READ,
      (void *)&RELAY_STATES[1]
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
};

uint8 CONST zclcc2530_NumAttributes2 = ( sizeof(zclcc2530_Attrs2) / sizeof(zclcc2530_Attrs2[0]) );
