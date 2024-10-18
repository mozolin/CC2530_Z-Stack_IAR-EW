#if USE_DHT11

#ifndef DHT11_H
#define DHT11_H

#ifdef __cplusplus
extern "C" {
#endif


extern uchar humiH, humiL, tempDec, tempH, tempL, humiDec;

//-- Temperature and humidity sensor start
extern uint8 halDHT11GetData(void);
extern uint8 errorMsg(uint8 req);

/* DHT11 GPIO */
//-- can be: P04!,P07!
// -p02,-p03,-p05,-p06,-p15,-p16,-p21
//#define HAL_DHT11_PORT  0 //!< Port
//#define HAL_DHT11_PIN   7 //!< Pin

/* Boolean value. */
#define HAL_DHT11_FALSE         0
#define HAL_DHT11_TRUE          1

/* DHT11 Status Code. */
#define HAL_DHT11_SC_ERR                HAL_DHT11_FALSE
#define HAL_DHT11_SC_OK                 HAL_DHT11_TRUE
#define HAL_DHT11_SC_HUMI_OUTOFRANGE    0xF1
#define HAL_DHT11_SC_TEMP_OUTOFRANGE    0xF2
#define HAL_DHT11_SC_HT_OUTOFRANGE      0xF3

/* Delay Functions. */   
#define HAL_DHT11_DELAY_US(x)   delayUsIn32Mhz((x))
#define HAL_DHT11_DELAY_MS(x)   delayMs(SYSCLK_32MHZ ,(x))
   
/* Set DHT11 GPIO mode. */
#define HAL_DHT11_IO_OUTPUT()   CC2530_IOCTL(HAL_DHT11_PORT, HAL_DHT11_PIN, CC2530_OUTPUT)
#define HAL_DHT11_IO_INPUT()    CC2530_IOCTL(HAL_DHT11_PORT, HAL_DHT11_PIN, CC2530_INPUT_PULLDOWN)

/* Set DHT11 GPIO Level. */ 
#define HAL_DHT11_IO_SET(port, pin, level) do { \
  if(level) CC2530_GPIO_SET(port, pin);         \
  else CC2530_GPIO_CLEAR(port, pin);            \
} while(0)

#define HAL_DHT11_IO_SET_LO()  HAL_DHT11_IO_SET(HAL_DHT11_PORT, HAL_DHT11_PIN, 0)
#define HAL_DHT11_IO_SET_HI()  HAL_DHT11_IO_SET(HAL_DHT11_PORT, HAL_DHT11_PIN, 1)

/*  Get DHT11 GPIO Status. */
#define HAL_DHT11_IO_GET(port, pin) CC2530_GPIO_GET(port, pin)
#define HAL_DHT11_IO()              HAL_DHT11_IO_GET(HAL_DHT11_PORT, HAL_DHT11_PIN)

/* DHT11 Measurement range detection. */ 
#define HAL_DHT11_TEMP_OK(t)    ((t) <= 50)
#define HAL_DHT11_HUMI_OK(h)    ((h) >= 20 && (h) <= 95)


#ifdef __cplusplus
}
#endif

#endif //-- #ifndef DHT11_H

#endif //-- #if USE_DHT11
