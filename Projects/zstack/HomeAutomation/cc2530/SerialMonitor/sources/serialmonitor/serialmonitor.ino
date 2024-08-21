
#include "serialmonitor.h"
#include <Arduino.h>


void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.printf("Serial monitor is started via UART\n");
}

void loop()
{
	Serial.printf("Serial monitor is started via UART\n");
	delay(1000);
}
