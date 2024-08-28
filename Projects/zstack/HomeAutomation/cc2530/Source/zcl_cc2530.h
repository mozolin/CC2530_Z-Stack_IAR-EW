#ifndef ZCL_cc2530_H
#define ZCL_cc2530_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "zcl.h"

//-- IEEE address: 0x00124b0009674658
//-- Network address: 0x4021

//-- Номер эндпойнта устройства
#define cc2530_ENDPOINT            1

//-- События приложения
#define cc2530_EVT_BLINK                0x0001
#define cc2530_EVT_LONG                 0x0002
#define cc2530_END_DEVICE_REJOIN_EVT    0x0003
#define cc2530_REPORTING_EVT            0x0004
  
  
// NVM IDs
#define NV_cc2530_RELAY_STATE_ID        0x0402


extern SimpleDescriptionFormat_t zclcc2530_SimpleDesc;

extern CONST zclCommandRec_t zclcc2530_Cmds[];

extern CONST uint8 zclCmdsArraySize;

// Список атрибутов
extern CONST zclAttrRec_t zclcc2530_Attrs[];
extern CONST uint8 zclcc2530_NumAttributes;

// Атрибуты идентификации
extern uint16 zclcc2530_IdentifyTime;
extern uint8  zclcc2530_IdentifyCommissionState;

// TODO: Declare application specific attributes here

// Инициализация задачи
extern void zclcc2530_Init( byte task_id );

// Обработчик сообщений задачи
extern UINT16 zclcc2530_event_loop( byte task_id, UINT16 events );

// Сброс всех атрибутов в начальное состояние
extern void zclcc2530_ResetAttributesToDefaultValues(void);

// Функции работы с кнопками
extern void cc2530_HalKeyInit( void );
extern void cc2530_HalKeyPoll ( void );

// Функции команд управления
static void zclcc2530_OnOffCB(uint8);

#ifdef __cplusplus
}
#endif

#endif /* ZCL_cc2530_H */