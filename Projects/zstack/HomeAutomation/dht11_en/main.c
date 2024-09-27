/****************************************************************************
* Name: main.c
* Description: Send the collected temperature and humidity through the serial
* port to the serial port debugging assistant for display 115200 8N1
****************************************************************************/
#include <ioCC2530.h>
#include <stdio.h>
#include <string.h>
#include "INTUART.H" 
#include "DHT11.H" 

/****************************************************************************
* Program entry function
****************************************************************************/
void main(void)
{  
  char temp[3];
  char tmp[3];

  char humidity[3];
  //char hmd[3];

  char strTemp[13] = "Temperature:";
  char strHumidity[10] = "Humidity:";
  setSystemCLK();
  Delay_ms(1000);          //Make the device stable
  initUart();              //Serial port initialization
  while(1)
  {         
      memset(temp, 0, 3);
      memset(tmp, 0, 3);
      memset(humidity, 0, 3);
      //memset(hmd, 0, 3);
      DHT11(); //Get temperature and humidity

      
      
      //Convert temperature and humidity into a string
      temp[0] = tempH + 0x30;
      temp[1] = tempL + 0x30;

      tmp[0] = tempDec + 0x30;

      humidity[0] = humiH + 0x30;
      humidity[1] = humiL + 0x30;
      
      //hmd[0] = humiDec + 0x30;


      //The obtained temperature and humidity are output to the computer through the serial port
      sendString(strTemp, 12);
      sendString(temp, 2);
      sendString(".", 1);
      sendString(tmp, 1);
      sendString("C | ", 4);
      sendString(strHumidity, 9);
      sendString(humidity, 2);
      sendString("%", 1);
      //sendString(".", 1);
      //sendString(hmd, 1);
      sendString("\n", 1);
      
      Delay_ms(5000);  //Delay, read once every 2S?
	}
}
