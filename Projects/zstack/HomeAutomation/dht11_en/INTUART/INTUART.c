#include "INTUART.h"

#if (UART0 > 0)
    char UART0_CHAR[RX_BUFFER_SIZE];   //Define the receive buffer size
    int end,start;
    int sendstatus = 0;
    void (*UART0_RXISR_function)();    //Define the receiving function pointer
    void (*UART0_TXISR_function)();    //Define the send function pointer
    
    
/*****************************************************************************
* Function description: Initialize system clock and run clock
*
* Parameter: None
* Return value: None
******************************************************************************/
    void setSystemCLK()
    {
      CLKCONCMD &= ~(1 << 6); //Set the system clock source to 32MHz
      while( CLKCONCMD & (1 << 6) );
      CLKCONCMD &= ~(7 << 0); //The current system clock frequency is 32MHz
    
    }
    
  
/*****************************************************************************
* Function description: Initialize serial port
*
* Parameter: None
* Return value: None
******************************************************************************/
  void initUart()
  {
    int i;
    for(i = 0; i < RX_BUFFER_SIZE; i++)
    {
      UART0_CHAR[i] = 0;
    }
    
    UART0_RXISR_function = 0;   //Initialize function pointer
    UART0_TXISR_function = 0;   //Initialize function pointer
    end = start = 0;
    PERCFG &= ~(1 << 0); //Set uart0 at position 1
    P0SEL  |= (1 << 2);  //Set P0_2 as a peripheral
    P0SEL  |= (1 << 3);  //Set P0_3 as a peripheral
    P2DIR  &= ~(3 << 6); //Set uart0 to have the highest priority
    U0CSR  |= (1 << 7);  //usart0 as uart0
    //Set the baud rate to 115200
    U0GCR  |= (11 << 0); //Set BAUD_E to 11
    U0BAUD = 216;        //Set U0BAUD_M to 216
    UTX0IF = 0;          //UART0 TX interrupt flag is 0
    U0CSR  |= (1 << 6); //Set the receiving function of Uart0
    IEN0   |= (1 << 2); //UART0 RX interrupt enable
    IEN2   |= (1 << 2); //UART0 TX interrupt enable
    EA     = 1;         //Open the total interrupt
  } 
  
  
  /*****************************************************************************
  * Receive relevant things
  ******************************************************************************/
  #pragma vector = URX0_VECTOR
  __interrupt void UART0_RXISR(void)
  {         

      URX0IF = 0;
      if(end >= RX_BUFFER_SIZE )
        end = 0;
     UART0_CHAR[end++] = U0DBUF;
     if( UART0_RXISR_function != 0)
     {
         UART0_RXISR_function();
     }
  }
  
  /*****************************************************************************
  *  Function introduction: Set the processing function after receiving
  *
  * Parameter: inUART0_TXISR_function will set the function pointer
  * Return value: None
  ******************************************************************************/ 
  void setUART0_RXISR( void (*inUART0_RXISR_function)())
  {
    UART0_RXISR_function = inUART0_RXISR_function;
  }
  
   
  
  /*****************************************************************************
  * Send relevant stuff
  ******************************************************************************/
 
  #pragma vector = UTX0_VECTOR
  __interrupt void UART0_TXISR(void)
  {
      UTX0IF = 0;   
      sendstatus = 0;
      if(UART0_TXISR_function != 0 )
      {
          UART0_TXISR_function();
      }
  }

  
  /*****************************************************************************
  *  Function introduction: Set the processing function after sending
  *
  * Parameter: function pointer to be set by inUART0_RXISR_function
  * Return value: None
  ******************************************************************************/ 
  void setUART0_TXISR( void (*inUART0_TXISR_function)())
  {
    UART0_TXISR_function = inUART0_TXISR_function;
  }
  
  
  /*****************************************************************************
  *  Function description: Send a character
  *
  * Parameter: ch: The character to be sent
  * Return value: None
  ******************************************************************************/
  void sendChar(char ch)
  {
    sendstatus = 1;
    U0DBUF = ch;
    while( sendstatus == 1 );
  }
  
  
  /*****************************************************************************
  *  Function description: Read a character
  *
  * Parameters: None
  * Return value: None
  ******************************************************************************/

  char readChar()
  {
    if(start != end)
    {
      char ch = UART0_CHAR[start];
      UART0_CHAR[start] = 0;
      start++;
      if(start >= RX_BUFFER_SIZE)
      {
        start = 0;
      }
      
      return ch;
    }
    else
    {
      return 0;
    }
     
  }
  
  /*****************************************************************************
  *  Function description: Send string
  *
  * Parameter: str: string pointer
  *: len: string length
  * Return value: None
  ******************************************************************************/

  void sendString(char * str, int len)
  {
      int i = 0;
      while(i < len)
      {
        sendChar(*(str+i));
        i++;
      }
      
  }
  
  /*****************************************************************************
  *  Function description: Read len characters. If there are len characters in the cache, read len. Otherwise, read
  *: All characters in the cache
  * Parameters: str: The location used to save the string
  *: len: The length to be read
  * Return value: The number of characters read
  ******************************************************************************/ 
  int readString(char * str, int len)
  {

      int i = 0;
      if(len <= 0)
      {
        return -1;
      }
      char ch = 0;
      ch = readChar();
      while( (ch != 0) )
      {
         *(str+i) = ch;
          i++;
          if( i < len )
          {
            ch = readChar();
          }
          else
          {
            break;
          }
      }
      return i;
  }
  
  void sendNewLine()
  {
       sendString("\r\n",2);
  }
#endif
