#ifndef __CHECKUART_H_
#define __CHECKUART_H_

#include <ioCC2530.h>

#define UART0 1
#define RX_BUFFER_SIZE 50  //Define the receive buffer size to 50
  #if (UART0 > 0)  
    extern void sendNewLine();
    extern void setSystemCLK();
    extern void initUart();
    extern void sendChar(char ch);
    extern void sendString(char * str, int len);
    extern char readChar();
    extern int readString(char * str, int len);
    extern void setUART0_TXISR( void (*inUART0_TXISR_function)());
    extern void setUART0_RXISR( void (*inUART0_RXISR_function)());
  #endif
#endif