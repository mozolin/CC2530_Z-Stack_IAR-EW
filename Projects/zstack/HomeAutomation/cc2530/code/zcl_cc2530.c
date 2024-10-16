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
#include "zcl_cc2530_data.h"

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

#include "hal_lcd_common.h"

//-- DHT11 driver
int dht11Idx = 0;
#include "hal_dht11.h" 


//-- The task ID of application
byte zclcc2530_TaskID;

//-- Network status
devStates_t zclcc2530_NwkState = DEV_INIT;

//-- Button status
static uint8 halKeySavedKeys;

//-- relay number
int8 RELAY_NUM = -1;

//-- click counters
uint8 KEY_CLICK_NUM_1 = 0;
//-- screen number
uint8 SCREEN_NUM = 0;
#ifdef HAL_LCD_TEST
  uint8 TOTAL_SCREENS = 3;
#else
  uint8 TOTAL_SCREENS = 2;
#endif

//-- temperature data
int16 zclcc2530_MeasuredValue;

//-- Structure for sending a report
afAddrType_t zclcc2530_DstAddr;
//-- Sequence number
uint8 SeqNum = 0;

static void zclcc2530_HandleKeys(byte shift, byte keys);


//-- Change relay state
static void updateRelay(bool, uint8 num);
//-- Displaying the relay status on the pins
static void applyRelay(void);
//-- Sending a relay status report
void zclcc2530_ReportOnOff(uint8 cc2530_ENDPOINT, uint8 cc2530_STATE);
//-- Sending temperature report
void zclcc2530_ReportTemp(void);

uint8 initUart0(halUARTCBack_t pf);
void uart0RxCb(uint8 port, uint8 event);

//-- report DHT11 sensor
void zclcc2530_ReportDHT11(void);

void drawScreen1(void);
void switchScreen(void);


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

//-- Application task initialization function
void zclcc2530_Init(byte task_id)
{
  zclcc2530_TaskID = task_id;
  
  //-- Registering Home Automation Profile
  bdb_RegisterSimpleDescriptor(&zclcc2530_SimpleDesc_1);
  bdb_RegisterSimpleDescriptor(&zclcc2530_SimpleDesc_2);
  //bdb_RegisterSimpleDescriptor(&zclcc2530_SimpleDesc_3);
  //bdb_RegisterSimpleDescriptor(&zclcc2530_SimpleDesc_4);

  //-- Registering ZCL Command Handlers
  zclGeneral_RegisterCmdCallbacks(cc2530_ENDPOINT_1, &zclcc2530_CmdCallbacks1);
  zclGeneral_RegisterCmdCallbacks(cc2530_ENDPOINT_2, &zclcc2530_CmdCallbacks2);
  //zclGeneral_RegisterCmdCallbacks(cc2530_ENDPOINT_3, &zclcc2530_CmdCallbacks3);
  //zclGeneral_RegisterCmdCallbacks(cc2530_ENDPOINT_4, &zclcc2530_CmdCallbacks4);
  
  // TODO: Register other cluster command callbacks here

  //-- Registering Application Cluster Attributes
  zcl_registerAttrList(cc2530_ENDPOINT_1, zclcc2530_NumAttributes1, zclcc2530_Attrs1);
  zcl_registerAttrList(cc2530_ENDPOINT_2, zclcc2530_NumAttributes2, zclcc2530_Attrs2);
  //zcl_registerAttrList(cc2530_ENDPOINT_3, zclcc2530_NumAttributes3, zclcc2530_Attrs3);
  //zcl_registerAttrList(cc2530_ENDPOINT_4, zclcc2530_NumAttributes4, zclcc2530_Attrs4);

  //-- Subscribing a task to receive command/response messages
  zcl_registerForMsg(zclcc2530_TaskID);

  /*
  #ifdef ZCL_DISCOVER
    //-- Registering a list of commands implemented by the application
    zcl_registerCmdList(cc2530_ENDPOINT_1, zclCmdsArraySize, zclcc2530_Cmds);
    zcl_registerCmdList(cc2530_ENDPOINT_2, zclCmdsArraySize, zclcc2530_Cmds);
    zcl_registerCmdList(cc2530_ENDPOINT_3, zclCmdsArraySize, zclcc2530_Cmds);
    zcl_registerCmdList(cc2530_ENDPOINT_4, zclCmdsArraySize, zclcc2530_Cmds);
  #endif
  */

  //-- Subscribe task to receive all events for buttons
  RegisterForKeys(zclcc2530_TaskID);

  bdb_RegisterCommissioningStatusCB(zclcc2530_ProcessCommissioningStatus);
  bdb_RegisterIdentifyTimeChangeCB(zclcc2530_ProcessIdentifyTimeChange);

  #ifdef ZCL_DIAGNOSTIC
    // Register the application's callback function to read/write attribute data.
    // This is only required when the attribute data format is unknown to ZCL.
    zcl_registerReadWriteCB(cc2530_ENDPOINT_1, zclDiagnostic_ReadWriteAttrCB, NULL);
    zcl_registerReadWriteCB(cc2530_ENDPOINT_2, zclDiagnostic_ReadWriteAttrCB, NULL);
    zcl_registerReadWriteCB(cc2530_ENDPOINT_3, zclDiagnostic_ReadWriteAttrCB, NULL);
    zcl_registerReadWriteCB(cc2530_ENDPOINT_4, zclDiagnostic_ReadWriteAttrCB, NULL);

    if(zclDiagnostic_InitStats() == ZSuccess) {
      // Here the user could start the timer to save Diagnostics to NV
    }
  #endif
  
  //-- Setting the address and endpoint for sending the report
  zclcc2530_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  zclcc2530_DstAddr.endPoint = 0;
  zclcc2530_DstAddr.addr.shortAddr = 0;
  
  //-- initialize NVM to store RELAY STATE
  if(SUCCESS == osal_nv_item_init(NV_cc2530_RELAY_STATE_ID, 1, &RELAY_STATES[0])) {
    //-- read the RELAY STATE value from memory
    osal_nv_read(NV_cc2530_RELAY_STATE_ID, 0, 1, &RELAY_STATES[0]);
  }
  //-- apply relay state
  applyRelay();

  //-- start a repeating timer for the HAL_KEY_EVENT event
  osal_start_reload_timer(zclcc2530_TaskID, HAL_KEY_EVENT, TIMER_INTERVAL_HAL_KEY_EVT);
  
  //-- start a repeating timer to inform about the temperature DS18B20
  osal_start_reload_timer(zclcc2530_TaskID, cc2530_EVT_REPORTING, TIMER_INTERVAL_REPORTING_EVT);
  //-- start a repeating timer to inform about the temperature DHT11
  osal_start_reload_timer(zclcc2530_TaskID, cc2530_EVT_REFRESH, TIMER_INTERVAL_REFRESH_EVT);
  
  //-- Start of the process of returning to the network
  bdb_StartCommissioning(BDB_COMMISSIONING_MODE_PARENT_LOST);
  
  //-- init UART to use "printf" for serial monitor
  HalUARTInit();
  uint8 state = initUart0(uart0RxCb);

  //-- Output to terminal via UART
  printf(FONT_COLOR_STRONG_GREEN);
  printf("\nUART initiated\n");
  printf(STYLE_COLOR_RESET);

  
  setSystemClk32MHZ();
  halLCDInit();
  switchScreen();
}

//-- Main task event processing loop
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
          //-- Processing an incoming ZCL Foundation command/response message
          zclcc2530_ProcessIncomingMsg((zclIncomingMsg_t *)MSGpkt);
          break;

        case KEY_CHANGE:
          zclcc2530_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
          break;

        case ZDO_STATE_CHANGE:
          zclcc2530_NwkState = (devStates_t)(MSGpkt->hdr.status);

          //-- Now we are in the zigbee network
          if((zclcc2530_NwkState == DEV_ZB_COORD) ||
            (zclcc2530_NwkState == DEV_ROUTER)   ||
            (zclcc2530_NwkState == DEV_END_DEVICE)) {
            
            //-- Output to terminal via UART
            printf(FONT_COLOR_STRONG_GREEN);
            printf("Joined network!\n");
            printf(STYLE_COLOR_RESET);
            
            //-- disable blinking
            osal_stop_timerEx(zclcc2530_TaskID, HAL_LED_BLINK_EVENT);
            HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
            
            //-- send report
            zclcc2530_ReportOnOff(cc2530_ENDPOINT_1, RELAY_STATES[0]);
          }
          break;

        default:
          break;
      }

      osal_msg_deallocate((uint8 *)MSGpkt);
    }

    //-- return unprocessed messages
    return (events ^ SYS_EVENT_MSG);
  }

  //-- Handling application events
  
  //-- event cc2530_EVT_BLINK
  if(events & cc2530_EVT_BLINK)
  {
    printf("Blinking...\n");
    //-- переключим светодиод
    HalLedSet(HAL_LED_2, HAL_LED_MODE_TOGGLE);
    return (events ^ cc2530_EVT_BLINK);
  }
  
  //-- cc2530_EVT_DOUBLE event
  if(events & cc2530_EVT_DOUBLE)
  {
  	if(KEY_CLICK_NUM_1 > 1) {
  		//-- double, triple, etc click
  		printf(FONT_COLOR_STRONG_MAGENTA);
    	printf("Double press #1\n");
    	printf(STYLE_COLOR_RESET);
    } else {
    	//-- single click
    	printf(FONT_COLOR_STRONG_GRAY);
	    printf("Single press #1\n");
    	printf(STYLE_COLOR_RESET);

    	SCREEN_NUM++;
    	//-- last screen => go to #1
    	if(SCREEN_NUM == TOTAL_SCREENS) {
    		SCREEN_NUM = 0;
    	}
    	switchScreen();
    	
    	/*
    	updateRelay(RELAY_STATES[0] == 0);
    	if(RELAY_STATES[0] == 0) {
        
    	} else {
        
    	}
    	*/
    }
  	
  	//-- clear click counter
  	KEY_CLICK_NUM_1 = 0;

  	return (events ^ cc2530_EVT_DOUBLE);
  }

  //-- cc2530_EVT_LONG event
  if(events & cc2530_EVT_LONG)
  {
    printf("Long press\n");
    //-- Check the current status of the device: in the zigbee network or not in the network?
    if(bdbAttributes.bdbNodeIsOnANetwork) {
      //-- Leave Network
      zclcc2530_LeaveNetwork();
      
      //-- Output to terminal via UART
      printf(FONT_COLOR_STRONG_RED);
      printf("Leave Network\n");
      printf(STYLE_COLOR_RESET);

    } else {
      //-- initiate login to zigbee network
      bdb_StartCommissioning(
        BDB_COMMISSIONING_MODE_NWK_FORMATION | 
        BDB_COMMISSIONING_MODE_NWK_STEERING | 
        BDB_COMMISSIONING_MODE_FINDING_BINDING | 
        BDB_COMMISSIONING_MODE_INITIATOR_TL
      );
      //-- blinking until connect
      osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_BLINK, TIMER_INTERVAL_BLINK_EVT);
      
      //-- Output to terminal via UART
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
    
    switchScreen();
    
    return (events ^ cc2530_EVT_REFRESH);
  }
  
  //-- button poll event
  if (events & HAL_KEY_EVENT)
  {
    //-- Reading buttons
    cc2530_HalKeyPoll();

    return (events ^ HAL_KEY_EVENT);
  }
  
  //-- discard unprocessed messages
  return 0;
}


//-- Keystroke handler
static void zclcc2530_HandleKeys(byte shift, byte keys)
{
  if(keys & HAL_KEY_SW_1) {
    
    //-- increase click counter
    KEY_CLICK_NUM_1++;
    printf("Key1 pressed: %d\n", RELAY_STATES[0]);
    //-- Start the timer to detect a long press
    osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_LONG, TIMER_INTERVAL_LONG_PRESS_EVT);
    //-- Start the timer to detect double clicks
    osal_start_timerEx(zclcc2530_TaskID, cc2530_EVT_DOUBLE, TIMER_INTERVAL_DOUBLE_PRESS_EVT);
    
  } else {
    //-- Stopping the long press timer
    osal_stop_timerEx(zclcc2530_TaskID, cc2530_EVT_LONG);
    //printf("Key1 stopped timer\n");
  }

  if(keys & HAL_KEY_SW_2) {
    HalLedSet(HAL_LED_3, HAL_LED_MODE_TOGGLE);
    
    //-- single click
    printf(FONT_COLOR_STRONG_GRAY);
	  printf("Single press #2\n");
    printf(STYLE_COLOR_RESET);

    RELAY_NUM++;
		if(RELAY_NUM == TOTAL_RELAYS_NUM) {
			RELAY_NUM = 0;
		}
		printf("Relay #%d\n", RELAY_NUM);
		updateRelay(RELAY_STATES[RELAY_NUM] == 0, RELAY_NUM);
		switchScreen();

  }

  if(keys & HAL_KEY_SW_3) {
    P0_4 = (P0_4 == 0) ? 1 : 0;
    printf("Key3:%d\n", P0_4);
    //halOLED128x64ShowX16(2, 0, "Key #3 pressed");
  }

}




//-- Initialization of buttons (inputs)
void cc2530_HalKeyInit(void)
{
  //-- Reset the saved state of the buttons to 0
  halKeySavedKeys = 0;

  //-- Выставляем функцию пина - GPIO
  PUSH1_SEL &= ~(PUSH1_BV);
  //-- Выставляем режим пина - Вход
  PUSH1_DIR &= ~(PUSH1_BV);
  //-- Не генерируем прерывания на пине
  PUSH1_ICTL &= ~(PUSH1_ICTLBIT);
  //-- Очищаем признак включения прерываний
  PUSH1_IEN &= ~(PUSH1_IENBIT);
  //-- Set pin function to GPIO
  PUSH2_SEL &= ~(PUSH2_BV);
  //-- Set pin direction to Input
  PUSH2_DIR &= ~(PUSH2_BV);
  //-- don't generate interrupt
  PUSH2_ICTL &= ~(PUSH2_ICTLBIT);
  //-- Clear interrupt enable bit
  PUSH2_IEN &= ~(PUSH2_IENBIT);

  //-- LED initiated as output...
  initOutputGPIO(GPIO_PORT_0, GPIO_PIN_4);
  P0_4 = 0;

  //-- PUSH3 initiated as input...
  initInputGPIO(GPIO_PORT_1, GPIO_PIN_5, GPIO_PULL_UP);
}

//-- Key poll
void cc2530_HalKeyPoll (void)
{
  uint8 keys = 0;

  //-- Button #1 pressed?
  if(HAL_PUSH_BUTTON1()) {
    keys |= HAL_KEY_SW_1;
  }
  
  //-- Button #2 pressed?
  if(HAL_PUSH_BUTTON2()) {
    keys |= HAL_KEY_SW_2;
  }

  //-- Button #3 pressed?
  if(HAL_PUSH_BUTTON3()) {
    keys |= HAL_KEY_SW_3;
  }
  
  if(keys == halKeySavedKeys) {
    //-- no changes => exit
    return;
  }
  //-- save the current state of the buttons for comparison next time
  halKeySavedKeys = keys;

  //-- call the generation of the button change event
  OnBoard_SendKeys(keys, HAL_KEY_STATE_NORMAL);
}

//-- Change relay state
void updateRelay(bool value, uint8 num)
{
  if(value) {
    RELAY_STATES[num] = 1;
  } else {
    RELAY_STATES[num] = 0;
  }

  printf("Relay (updateRelay): %d\n", RELAY_STATES[num]);

  //-- save relay state
  osal_nv_write(NV_cc2530_RELAY_STATE_ID, 0, 1, &RELAY_STATES[num]);
  //-- Displaying the new state
  applyRelay();
  //-- send report
  switch(num)
  {
  	case 0:
  		zclcc2530_ReportOnOff(cc2530_ENDPOINT_1, RELAY_STATES[num]);
  		break;
  	case 1:
  		zclcc2530_ReportOnOff(cc2530_ENDPOINT_2, RELAY_STATES[num]);
  		break;
  	case 2:
  		zclcc2530_ReportOnOff(cc2530_ENDPOINT_3, RELAY_STATES[num]);
  		break;
  	case 3:
  		zclcc2530_ReportOnOff(cc2530_ENDPOINT_4, RELAY_STATES[num]);
  		break;
  }
}
  
//-- Displaying the new state
void applyRelay (void)
{
  //-- if OFF
  if(RELAY_STATES[0] == 0) {
    //-- set LED1 to OFF
    HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
  } else {
    //-- set LED1 to ON
    HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
  }
}

//-- Cluster OnOff Command Handler
static void zclcc2530_OnOffCB1(uint8 cmd)
{
  printf("OnOff?#1-> %d (on=%d, off=%d, toggle=%d)\n", cmd, COMMAND_ON, COMMAND_OFF, COMMAND_TOGGLE);
  //-- save the address from where the command came to send the report back
  afIncomingMSGPacket_t *pPtr = zcl_getRawAFMsg();
  zclcc2530_DstAddr.addr.shortAddr = pPtr->srcAddr.addr.shortAddr;
  if(cmd == COMMAND_ON) {
    //-- set to ON
    updateRelay(true, 0);
  } else if(cmd == COMMAND_OFF) {
    //-- set ot OFF
    updateRelay(false, 0);
  } else if(cmd == COMMAND_TOGGLE) {
    //-- toggle
    updateRelay(RELAY_STATES[0] == 0, 0);
  }
}

static void zclcc2530_OnOffCB2(uint8 cmd)
{
  printf("OnOff?#2-> %d (on=%d, off=%d, toggle=%d)\n", cmd, COMMAND_ON, COMMAND_OFF, COMMAND_TOGGLE);
  //-- save the address from where the command came to send the report back
  afIncomingMSGPacket_t *pPtr = zcl_getRawAFMsg();
  zclcc2530_DstAddr.addr.shortAddr = pPtr->srcAddr.addr.shortAddr;
  if(cmd == COMMAND_ON) {
    //-- set to ON
    updateRelay(true, 1);
  } else if(cmd == COMMAND_OFF) {
    //-- set ot OFF
    updateRelay(false, 1);
  } else if(cmd == COMMAND_TOGGLE) {
    //-- toggle
    updateRelay(RELAY_STATES[1] == 0, 1);
  }
}
static void zclcc2530_OnOffCB3(uint8 cmd)
{
  printf("OnOff?#3-> %d (on=%d, off=%d, toggle=%d)\n", cmd, COMMAND_ON, COMMAND_OFF, COMMAND_TOGGLE);
  //-- save the address from where the command came to send the report back
  afIncomingMSGPacket_t *pPtr = zcl_getRawAFMsg();
  zclcc2530_DstAddr.addr.shortAddr = pPtr->srcAddr.addr.shortAddr;
  if(cmd == COMMAND_ON) {
    //-- set to ON
    updateRelay(true, 2);
  } else if(cmd == COMMAND_OFF) {
    //-- set ot OFF
    updateRelay(false, 2);
  } else if(cmd == COMMAND_TOGGLE) {
    //-- toggle
    updateRelay(RELAY_STATES[2] == 0, 2);
  }
}
static void zclcc2530_OnOffCB4(uint8 cmd)
{
  printf("OnOff?#4-> %d (on=%d, off=%d, toggle=%d)\n", cmd, COMMAND_ON, COMMAND_OFF, COMMAND_TOGGLE);
  //-- save the address from where the command came to send the report back
  afIncomingMSGPacket_t *pPtr = zcl_getRawAFMsg();
  zclcc2530_DstAddr.addr.shortAddr = pPtr->srcAddr.addr.shortAddr;
  if(cmd == COMMAND_ON) {
    //-- set to ON
    updateRelay(true, 3);
  } else if(cmd == COMMAND_OFF) {
    //-- set ot OFF
    updateRelay(false, 3);
  } else if(cmd == COMMAND_TOGGLE) {
    //-- toggle
    updateRelay(RELAY_STATES[3] == 0, 3);
  }
}

//-- reporting about the relay status
void zclcc2530_ReportOnOff(uint8 cc2530_ENDPOINT, uint8 cc2530_STATE)
{
  const uint8 NUM_ATTRIBUTES = 1;

  zclReportCmd_t *pReportCmd;

  pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) +
                              (NUM_ATTRIBUTES * sizeof(zclReport_t)));
  if(pReportCmd != NULL) {
    pReportCmd->numAttr = NUM_ATTRIBUTES;

    pReportCmd->attrList[0].attrID = ATTRID_ON_OFF;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_BOOLEAN;
    pReportCmd->attrList[0].attrData = (void *)(&cc2530_STATE);

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

    zcl_SendReportCmd(cc2530_ENDPOINT_1, &zclcc2530_DstAddr,
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
    //-- Read one byte from UART to ch
    HalUARTRead (port, &ch, 1);
  }
}

uint8 req;
void zclcc2530_ReportDHT11(void)
{
  char t[32], h[32], i[32], s[32];

  req = halDHT11GetData();

  if(req) {
    if(errorMsg(req)) {
      return;
    }
    
    //-- Output to terminal via UART
    printf(FONT_COLOR_STRONG_CYAN);
   	printf("DHT11 Initiated\n");
    printf(STYLE_COLOR_RESET);

    //-- make output to LCD
    dht11Idx++;
    sprintf(s, "Screen 0");
    sprintf(i, "Idx: %d", dht11Idx);
    sprintf(t, "Temp: %d%d.%d°C", tempH, tempL, tempDec);
    if(humiDec > 0) {
      sprintf(h, "Humi: %d%d.%d%%", humiH, humiL, humiDec);
    } else {
      sprintf(h, "Humi: %d%d%%", humiH, humiL);
    }

    halLCDClearScreen();
    #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
      halOLED128x64ShowX16(0, 0, (uint8 const *)s);
      halOLED128x64ShowX16(1, 0, (uint8 const *)i);
      halOLED128x64ShowX16(2, 0, (uint8 const *)t);
      halOLED128x64ShowX16(3, 0, (uint8 const *)h);
    #else
      halTFTShowX16(0, 0,  PX_WHITE, PX_BLACK, (uint8 const *)s);
      halTFTShowX16(0, 16, PX_CYAN,  PX_BLACK, (uint8 const *)i);
      halTFTShowX16(0, 32, PX_RED,   PX_BLACK, (uint8 const *)t);
      halTFTShowX16(0, 48, PX_GREEN, PX_BLACK, (uint8 const *)h);
    #endif

    //-- Output to terminal via UART
    printf(FONT_COLOR_STRONG_YELLOW);
    printf("%s, %s, %s\n", i, t, h);
    printf(STYLE_COLOR_RESET);
    
  } else {
    
    //-- Output to terminal via UART
    printf(FONT_COLOR_STRONG_RED);
    printf("DHT11 NOT Initiated!\n");
    printf(STYLE_COLOR_RESET);

  }
}

void drawScreen1(void)
{
	char s1[32],s2[32],s3[32],s4[32];
	sprintf(s1, "Relay #1: %d", RELAY_STATES[0]);
	sprintf(s2, "Relay #2: %d", RELAY_STATES[1]);
	sprintf(s3, "Relay #3: %d", RELAY_STATES[2]);
	sprintf(s4, "Relay #4: %d", RELAY_STATES[3]);

	halLCDClearScreen();
  #if HAL_LCD_TYPE == HAL_LCD_TYPE_OLED
    halOLED128x64ShowX8(0, 0, "Screen 1");
    halOLED128x64ShowX8(1, 0, (uint8 const *)s1);
    halOLED128x64ShowX8(2, 0, (uint8 const *)s2);
    halOLED128x64ShowX8(3, 0, (uint8 const *)s3);
    halOLED128x64ShowX8(4, 0, (uint8 const *)s4);
  #else
    halTFTShowX8(0, 0,  PX_WHITE, PX_BLACK, "Screen 1");
    halTFTShowX8(0, 12, PX_WHITE, PX_BLACK, (uint8 const *)s1);
    halTFTShowX8(0, 24, PX_WHITE, PX_BLACK, (uint8 const *)s2);
    halTFTShowX8(0, 32, PX_WHITE, PX_BLACK, (uint8 const *)s3);
    halTFTShowX8(0, 48, PX_WHITE, PX_BLACK, (uint8 const *)s4);
  #endif
}

void switchScreen(void)
{
	//printf("SN:%d\n", SCREEN_NUM);
	switch(SCREEN_NUM)
	{
		case 0:
			zclcc2530_ReportDHT11();
			break;
		case 1:
			drawScreen1();
			break;
		case 2:
			#ifdef HAL_LCD_TEST
        halLCDStartTest();
    	#endif
			break;
		default:
			break;
	}
}
