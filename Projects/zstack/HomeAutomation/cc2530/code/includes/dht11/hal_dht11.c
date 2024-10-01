#include <stdio.h>
#include <stdlib.h>

#include <ioCC2530.h>
#include "cc2530_io_ports.h"
#include "hal_dht11.h"
#include "colors.h"

//-- Definition of temperature and humidity
uchar ucharFlag, uchartemp;
uchar humiH, humiL, tempH, tempL;
uchar ucharT_data_H, ucharT_data_L, ucharH_data_H, ucharH_data_L, ucharcheckdata;
uchar ucharT_data_H_temp, ucharT_data_L_temp, ucharH_data_H_temp, ucharH_data_L_temp, ucharcheckdata_temp;
uchar ucharCOMdata;
uchar tempDec, humiDec;

uchar halDHT11ReadByte(void);
uint8_t halDHT11CheckData(uint8_t TempI, uint8_t HumiI);


uchar halDHT11ReadByte(void)
{     
  uchar i;         
  for(i=0; i<8; i++)    
  {
    ucharFlag = 2; 
    while((!HAL_DHT11_IO()) && ucharFlag++);
    
  	HAL_DHT11_DELAY_US(50);

    uchartemp = 0;
    if(HAL_DHT11_IO())uchartemp=1;
    ucharFlag = 2;
    while((HAL_DHT11_IO()) && ucharFlag++);
    if(ucharFlag == 1) {
    	break;
    }
    ucharCOMdata <<= 1;
    ucharCOMdata |= uchartemp; 
  }
  return ucharCOMdata;
}

uint8 halDHT11GetData(void)
{
  uint8 result = 1;

  /* >18ms, keeping gpio low-level */
  HAL_DHT11_IO_SET_LO();
  HAL_DHT11_DELAY_MS(30);
  
  HAL_DHT11_IO_SET_HI();
  
  //-- Reconfigure IO port direction
  //P0DIR &= ~0x80;
  
  HAL_DHT11_DELAY_US(32);
  
  if(!HAL_DHT11_IO())
  {
    ucharFlag=2; 
    while((!HAL_DHT11_IO()) && ucharFlag++);
    ucharFlag=2;
    while((HAL_DHT11_IO()) && ucharFlag++); 
    
    ucharH_data_H_temp = halDHT11ReadByte();
    ucharH_data_L_temp = halDHT11ReadByte();
    ucharT_data_H_temp = halDHT11ReadByte();
    ucharT_data_L_temp = halDHT11ReadByte();
    ucharcheckdata_temp = halDHT11ReadByte();

    HAL_DHT11_IO_SET_HI();
    uchartemp = (ucharT_data_H_temp + ucharT_data_L_temp + ucharH_data_H_temp + ucharH_data_L_temp);

    if(uchartemp == ucharcheckdata_temp)
    {
      ucharH_data_H = ucharH_data_H_temp;
      ucharH_data_L = ucharH_data_L_temp;
      ucharT_data_H = ucharT_data_H_temp;
      ucharT_data_L = ucharT_data_L_temp;
      ucharcheckdata = ucharcheckdata_temp;
    }

    //-- first char of temperature
    tempH = ucharT_data_H/10;
    //-- second char of temperature
    tempL = ucharT_data_H%10;
    //-- decimal char of temperature
    tempDec = ucharT_data_L%10;
    //-- Example: 25.8°C => 2|5|8 => tempH|tempL|tempDec
    
    //-- first char of humidity
    humiH = ucharH_data_H/10;
    //-- second char of humidity
    humiL =ucharH_data_H%10;
    //-- decimal char of humidity
    humiDec = ucharH_data_L%10;
    //-- Example: 32.6% => 3|2|6 => humiH|humiL|humiDec

    
    char resStr[200] = {0};
    //-- convert temperature to integer
    sprintf(resStr, "%d%d.%d", tempH, tempL, tempDec);
  	uint8 tempNum = atoi(resStr);
  	//-- convert humidity to integer
  	sprintf(resStr, "%d%d.%d", humiH, humiL, humiDec);
  	uint8 humiNum = atoi(resStr);
  	
  	//-- check data
  	result = halDHT11CheckData(tempNum, humiNum);
  } else {
    //-- If the read is unsuccessful, return 0
    tempH = 0; tempL = 0; tempDec = 0;
    humiH = 0; humiL = 0; humiDec = 0;
    //-- return FALSE
    result = 0;
  } 
  //-- IO port needs to be reconfigured 
  //P0DIR |= 0x80;

  return result;
}

uint8_t halDHT11CheckData(uint8_t TempI, uint8_t HumiI)
{
  if(HAL_DHT11_HUMI_OK(HumiI)) {
    if(HAL_DHT11_TEMP_OK(TempI)) {
    	return HAL_DHT11_SC_OK;
    } else {
    	return HAL_DHT11_SC_TEMP_OUTOFRANGE;
    }
  }
  
  if(HAL_DHT11_TEMP_OK(TempI)) {
  	return HAL_DHT11_SC_HUMI_OUTOFRANGE;
  } else {
  	return HAL_DHT11_SC_HT_OUTOFRANGE;
  }
}

uint8 errorMsg(uint8 req)
{
	uint8 error = 0;
	if(req > 1) {
    printf(FONT_COLOR_RED);
    switch(req) {
      case HAL_DHT11_SC_HUMI_OUTOFRANGE:
		   	printf("DHT11: Humidity out of range!\n");
		   	break;
    	case HAL_DHT11_SC_TEMP_OUTOFRANGE:
	    	printf("DHT11: Temperature out of range!\n");
	    	break;
    	case HAL_DHT11_SC_HT_OUTOFRANGE:
	    	printf("DHT11: Temperature & Humidity out of range!\n");
  	  	break;
  	  default:
  	  	printf("DHT11: Unknown error!\n");
  	  	break;
    };
    printf(STYLE_COLOR_RESET);
    error = 1;
  }
	return error;
}
