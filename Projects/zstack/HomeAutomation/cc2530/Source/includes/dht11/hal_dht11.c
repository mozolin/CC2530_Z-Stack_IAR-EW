#include <stdio.h>
#include "hal_dht11.h"
//#include "hal_delay.h"
//#include "cc2530_ioctl.h"
#include "cc2530_io_ports.h"

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

static void halDHT11SetIdle(void);
static uint8_t halDHT11ReadByte(void);
static uint8_t halDHT11CheckData(uint8_t TempI, uint8_t HumiI);

void halDHT11Init(void)
{
    halDHT11SetIdle();
}

halDHT11Data_t  halDHT11GetData(void)
{
    uint8_t HumiI, HumiF, TempI, TempF, CheckSum;
    halDHT11Data_t dht11Dat = { .ok = HAL_DHT11_FALSE };

    /* >18ms, keeping gpio low-level */
    HAL_DHT11_IO_SET_LO();
    HAL_DHT11_DELAY_MS(30);
    
    HAL_DHT11_IO_SET_HI();
    
    /* Wait 20~40us then read ACK */
    HAL_DHT11_DELAY_US(32);
    HAL_DHT11_IO_INPUT();
    if (!HAL_DHT11_IO()) {
        printf("!HAL_DHT11_IO\n");
        
        uint16_t cnt = 1070; // ~1ms
        
        /* Wait for the end of ACK */
        while (!HAL_DHT11_IO() && cnt--);
        if(!cnt) goto Exit;

        printf("111\n");
        
        /* ~80us, DHT11 GPIO will be set after ACK */
        cnt = 1070;  // ~1ms
        HAL_DHT11_DELAY_US(80);
        while (HAL_DHT11_IO() && cnt--);
        if(!cnt) goto Exit;

        printf("222\n");

        /* Read data */
        HumiI = halDHT11ReadByte();
        printf("333:%d\n", HumiI);
        HumiF = halDHT11ReadByte();
        printf("444:%d\n", HumiF);
        TempI = halDHT11ReadByte();
        printf("555\n");
        TempF = halDHT11ReadByte();
        printf("666\n");
        CheckSum = halDHT11ReadByte();
        printf("777\n");

        printf(">> %d|%d|%d|%d|%d\n", HumiI, HumiF, TempI, TempF, CheckSum);
        
        /* Checksum */
        if (CheckSum == (HumiI + HumiF + TempI + TempF)) {
            dht11Dat.temp = TempI;
            dht11Dat.humi = HumiI;

            printf("ttt:%d, hhh:%d\n", TempI, HumiI);
            
            dht11Dat.ok = halDHT11CheckData(TempI, HumiI);
        }
    }
    
Exit:
    halDHT11SetIdle();
    return dht11Dat;
}

static void halDHT11SetIdle(void)
{
    HAL_DHT11_IO_OUTPUT();
    HAL_DHT11_IO_SET_HI();
}

static uint8_t halDHT11ReadByte(void)
{
    uint8_t dat = 0;
    
    printf("%d", 1);
    
    for (uint8_t i = 0; i < 8; i++) {
        uint16_t cnt = 5350;  // ~5ms

        printf("%d:%d:%d|", 2, HAL_DHT11_IO(), cnt);
        
        /* Busy */
        while (!HAL_DHT11_IO() && cnt--);
        if(!cnt) break;
        
        printf("%d", 3);

        /* Read bit based on high-level duration:
         *      26~28us: 0
         *      >70us:   1
         */
        HAL_DHT11_DELAY_US(50);
        if (HAL_DHT11_IO()) {      
            dat <<= 1;
            dat |= 1;
        
        	printf("%d", 4);
        }
        else {
            dat <<= 1;
            printf("%d", 5);
            continue;
        }

        printf("%d", 6);
        
        /* Waiting end */
        cnt = 1070;   // ~1ms
        while(HAL_DHT11_IO() && cnt--);

        printf("%d", 7);

        if(!cnt) break;

        printf("%d", 8);
    }

    printf("\n");
    
    return dat;
}

static uint8_t halDHT11CheckData(uint8_t TempI, uint8_t HumiI)
{
    if (HAL_DHT11_HUMI_OK(HumiI)) {
        if(HAL_DHT11_TEMP_OK(TempI)) return HAL_DHT11_SC_OK;
        else return HAL_DHT11_SC_TEMP_OUTOFRANGE;
    }
    
    if (HAL_DHT11_TEMP_OK(TempI)) return HAL_DHT11_SC_HUMI_OUTOFRANGE;
    else return HAL_DHT11_SC_HT_OUTOFRANGE;
}
