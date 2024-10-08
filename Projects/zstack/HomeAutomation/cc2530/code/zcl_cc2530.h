#ifndef ZCL_cc2530_H
#define ZCL_cc2530_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "zcl.h"

//-- IEEE address: 0x00124b0009674658
//-- Network address: 0x4021

//-- Device endpoint number
#define cc2530_ENDPOINT                 1

//-- Application events
#define cc2530_EVT_BLINK                0x0001 //-- Event A (2^0, 1)
#define cc2530_EVT_LONG                 0x0002 //-- Event B (2^1, 2)
#define cc2530_EVT_END_DEVICE_REJOIN    0x0004 //-- Event C (2^2, 4)
#define cc2530_EVT_REPORTING            0x0008 //-- Event D (2^3, 8)
//#define cc2530_EVT_...                0x0010 //-- Event E (2^4, 16)
/*********************************************************************
 If it uses 0x0010, it blocks any other events!
**********************************************************************/
#define cc2530_EVT_REFRESH              0x0020 //-- Event F (2^5, 32)
/*
#define cc2530_EVT_... 0x0001 //-- Event A (2^0, 1)
#define cc2530_EVT_... 0x0002 //-- Event B (2^1, 2)
#define cc2530_EVT_... 0x0004 //-- Event C (2^2, 4)
#define cc2530_EVT_... 0x0008 //-- Event D (2^3, 8)
#define cc2530_EVT_... 0x0010 //-- Event E (2^4, 16)
#define cc2530_EVT_... 0x0020 //-- Event F (2^5, 32)
#define cc2530_EVT_... 0x0040 //-- Event G (2^6, 64)
#define cc2530_EVT_... 0x0080 //-- Event H (2^7, 128)
#define cc2530_EVT_... 0x0100 //-- Event I (2^8, 256)
#define cc2530_EVT_... 0x0200 //-- Event J (2^9, 512)
#define cc2530_EVT_... 0x0400 //-- Event K (2^10, 1024)
#define cc2530_EVT_... 0x0800 //-- Event L (2^11, 2048)
#define cc2530_EVT_... 0x1000 //-- Event M (2^12, 4096)
#define cc2530_EVT_... 0x2000 //-- Event N (2^13, 8192)
#define cc2530_EVT_... 0x4000 //-- Event O (2^14, 16384)
*/  
  
//-- NVM IDs
#define NV_cc2530_RELAY_STATE_ID        0x0402


extern SimpleDescriptionFormat_t zclcc2530_SimpleDesc;

extern CONST zclCommandRec_t zclcc2530_Cmds[];

extern CONST uint8 zclCmdsArraySize;

// Список атрибутов
extern CONST zclAttrRec_t zclcc2530_Attrs[];
extern CONST uint8 zclcc2530_NumAttributes;

//-- Identification attributes
extern uint16 zclcc2530_IdentifyTime;
extern uint8  zclcc2530_IdentifyCommissionState;

//-- Initialization for the task
extern void zclcc2530_Init( byte task_id );

//-- Event Process for the task
extern uint16 zclcc2530_event_loop( byte task_id, uint16 events );

//-- Reset all writable attributes to their default values
extern void zclcc2530_ResetAttributesToDefaultValues(void);

//-- Keys control functions
extern void cc2530_HalKeyInit( void );
extern void cc2530_HalKeyPoll ( void );

//-- Functions of control commands
static void zclcc2530_OnOffCB(uint8);


#ifdef __cplusplus
}
#endif

#endif /* ZCL_cc2530_H */
