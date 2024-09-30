#include <ioCC2530.h>
#include "cc2530_io_ports.h"

//-- can be: P04,P06,P07
// -p02,-p03,-p05,-p15,-p16,-p21
#define DATA_PIN P0_4

//-- Definition of temperature and humidity
uchar ucharFLAG,uchartemp;
uchar humiH,humiL,tempH,tempL=4;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;

uchar tempDec,humiDec;

//-- Temperature and humidity sensor
void COM(void)
{     
  uchar i;         
  for(i=0;i<8;i++)    
  {
    ucharFLAG=2; 
    while((!DATA_PIN)&&ucharFLAG++);
    
    delay10US();
  	delay10US();
  	delay10US();
  	/*
    delayUsIn32Mhz(10);
    delayUsIn32Mhz(10);
    delayUsIn32Mhz(10);
    */

    uchartemp=0;
    if(DATA_PIN)uchartemp=1;
    ucharFLAG=2;
    while((DATA_PIN)&&ucharFLAG++);   
    if(ucharFLAG==1)break;    
    ucharcomdata<<=1;
    ucharcomdata|=uchartemp; 
  }    
}
//-- Temperature and humidity sensor start
void DHT11(void)
{
  DATA_PIN=0;
  
  //-- > 18 ms
  delayMs32MHZ(19);
  
  DATA_PIN=1; 
  //-- Reconfigure IO port direction
  P0DIR &= ~0x80;
  
  delay10US();
  delay10US();
  delay10US();
  delay10US();
  /*
  delayUsIn32Mhz(10);
  delayUsIn32Mhz(10);
  delayUsIn32Mhz(10);
  delayUsIn32Mhz(10);
  */
  
  if(!DATA_PIN) 
  {
    ucharFLAG=2; 
    while((!DATA_PIN)&&ucharFLAG++);
    ucharFLAG=2;
    while((DATA_PIN)&&ucharFLAG++); 
    COM();
    ucharRH_data_H_temp=ucharcomdata;
    COM();
    ucharRH_data_L_temp=ucharcomdata;
    COM();
    ucharT_data_H_temp=ucharcomdata;
    COM();
    ucharT_data_L_temp=ucharcomdata;
    COM();
    ucharcheckdata_temp=ucharcomdata;
    DATA_PIN=1; 
    uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);

    if(uchartemp==ucharcheckdata_temp)
    {
      ucharRH_data_H=ucharRH_data_H_temp;
      ucharRH_data_L=ucharRH_data_L_temp;
      ucharT_data_H=ucharT_data_H_temp;
      ucharT_data_L=ucharT_data_L_temp;
      ucharcheckdata=ucharcheckdata_temp;
    }

    tempH=ucharT_data_H/10;
    tempL=ucharT_data_H%10;

    tempDec=ucharT_data_L%10;

    humiH=ucharRH_data_H/10;
    humiL=ucharRH_data_H%10;

    humiDec=ucharRH_data_L%10;
  } 
  else
  {
    //-- If the read is unsuccessful, return 0
    tempH=0;
    tempL=0;
    tempDec=0;
    
    humiH=0;
    humiL=0;
    humiDec=0;
  } 
  //-- IO port needs to be reconfigured 
  P0DIR |= 0x80;
}
