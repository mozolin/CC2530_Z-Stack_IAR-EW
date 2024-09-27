#ifndef __DHT11_H__
#define __DHT11_H__

#define uchar unsigned char
extern void Delay_ms(unsigned int xms);	//Delay function
extern void COM(void);                  //Temperature and humidity writing
extern void DHT11(void);                //Temperature and humidity sensor start

extern uchar humiH,humiL,tempH,tempL;

extern uchar tempDec,humiDec;

#endif