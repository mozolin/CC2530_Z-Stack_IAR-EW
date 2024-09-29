#include <ioCC2530.h>


//#include "INTUART.H" 
//#include <string.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

#define DATA_PIN P0_6

//Definition of temperature and humidity
uchar ucharFLAG,uchartemp;
uchar humiH,humiL,tempH,tempL=4;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;

uchar tempDec,humiDec;


/*****************************************************************************
* Function name: Delay_us
* Function description: Use Time1 timer mode for precise positioning
*:
* Parameter: k: Delay k us
* Return value: None
******************************************************************************/

void Delay_us(unsigned int k)//us delay function
{ 

    T1CC0L = 0x10;
    T1CC0H = 0x00;
    T1CCTL0 |= (1 << 2);  //The analog mode can only use channel 0
    T1CTL = 0x02; 
    //--16M per second = 16000K = 16000000 times
    while(k)
    {
        while( ( T1STAT & (1 << 0) ) != 1);
        T1STAT &= ~(1 << 0);
        k--;
    }
    T1CTL = 0x00; //Turn off the timer
}


void Delay_10us() //10 us delay
{
  Delay_us(10);
}

void Delay_ms(uint Time)//n ms delay
{
    while(Time--)
    {
        Delay_us(1000);
    }
}

//Temperature and humidity sensor
void COM(void)    //Temperature and humidity writing
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);
        Delay_10us();
        Delay_10us();
        Delay_10us();
        uchartemp=0;
        if(DATA_PIN)uchartemp=1;
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);   
        if(ucharFLAG==1)break;    
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp; 
    }    
}

void DHT11(void)   //Temperature and humidity sensor start
{
    //char qqq[3];
    //memset(qqq, 0, 3);
    
    
    DATA_PIN=0;
    Delay_ms(19);  //>18MS
    DATA_PIN=1; 
    P0DIR &= ~0x80; //Reconfigure IO port direction
    Delay_10us();
    Delay_10us();                        
    Delay_10us();
    Delay_10us();

    /*
    qqq[0] = (int)DATA_PIN + 0x30;
    sendString("QQQ:", 4);
    sendString(qqq, 2);
    sendString("\n", 1);
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

        /*
        qqq[0] = (int)uchartemp + 0x30;
    		sendString("temp:", 5);
    		sendString(qqq, 2);
    		sendString("\n", 1);
    		*/
        
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
    else //If the read is unsuccessful, return 0
    {
        tempH=0;
        tempL=0;
        tempDec=0;
        
        humiH=0;
        humiL=0;
        humiDec=0;
    } 
    
    P0DIR |= 0x80; //IO port needs to be reconfigured 
}
