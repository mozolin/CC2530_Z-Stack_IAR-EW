#if USE_DS18B20

//-- Конфигурация сенсора температуры
#define TSENS_SBIT P1_6
#define TSENS_BV BV(6)
#define TSENS_DIR P1DIR 


int16 readTemperature(void);

#endif //-- #if USE_DS18B20
