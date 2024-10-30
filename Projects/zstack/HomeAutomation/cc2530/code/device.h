#include <stdio.h>

//-- manufacturer
extern char device_ManufacturerName[] = "Texas Instruments";
//-- device model
extern char device_ModelId[] = "'MIKE.CC2530.IAR";
//-- version date
extern char device_DateCode[] = "20240912";
//-- device location
extern char device_LocationDescription[] = "Based on TI CC2530";

extern void setCurrentDate(void);


void setCurrentDate(void)
{
	return;
	/*
	//-- parse date & fill array
  char s[32], tmp[32];
  char c[2];

  sprintf(s, "20241030");
  strcpy(tmp, "");
  
  for(uint8 i=0; i<strlen(s); i++) {
    sprintf(c, "%c", s[i]);
    zclcc2530_DateCode[i+1] = c+'\0';

    //c = atoi((char const*)s[i]);
    sprintf((char*)zclcc2530_DateCode[i+1], "%d", c);
    printf("%d-", zclcc2530_DateCode[i+1]);
  }
  printf("\nzclcc2530_DateCode=\"%s\"[%d]\n", zclcc2530_DateCode, sizeof(zclcc2530_DateCode));
  */
}
