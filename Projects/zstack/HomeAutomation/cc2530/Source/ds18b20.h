
//-- Конфигурация сенсора температуры
#define TSENS_SBIT P1_3
#define TSENS_BV BV(3)
#define TSENS_DIR P1DIR 


int16 readTemperature(void);
