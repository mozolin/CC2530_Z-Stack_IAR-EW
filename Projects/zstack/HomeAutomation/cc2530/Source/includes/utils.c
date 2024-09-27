#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utils.h"
#include "OSAL.h"

#include "hal_oled_128x64.h"

/***********************************
 * convert String to Integer #1    *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~    *
 * example:                        *
 *   char* str1 = "-5678";         * 
 *   num1 = str2int(str1);         *
 *   printf("num1 = %d\n", num1);  *
 *   | num1 = -5678                *
 ***********************************/
int str2int(char* str)
{
  int num;
  sscanf(str, "%d", &num);
  return num;
}
double str2float(char* str)
{
  return atof(str);
}
/***********************************
 * convert String to Integer #2    *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~    *
 * example:                        *
 *   char* str2 = "-5678";         *
 *   num2 = str2int2(str2);        *
 *   printf("num2 = %d\n", num2);  *
 *   | num2 = -5678                *
 ***********************************/
int str2int2(char* str)
{
  return atoi(str);
}

/*******************************************
 * convert Integer to String #1            *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~            *
 * example:                                *
 *   int num1 = -5678;                     *
 *   char buffer1[sizeof(int) * 8 + 1];    *
 *   char* str1 = int2str(num1, buffer1);  *
 *   printf("str1 = %s\n", str1);          *
 *   | str1 = -5678                        *
 *******************************************/
char* int2str(int num, char str[])
{
  sprintf(str, "%d", num);
  return str;
}
/****************************************
 * convert Integer to String #2         *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~         *
 * example:                             *
 *   int num2 = -5678;                  *
 *   char buffer2[sizeof(int) * 8 + 1]; *
 *   char* str2 = itoa(num2, buffer2);  *
 *   printf("str2 = %s\n", str2);       *
 *   | str2 = -5678                     *
 ****************************************/
char* itoa(int num, char str[])
{
  char const digit[] = "0123456789";
  char* p = str;
  if(num < 0){
    *p++ = '-';
    num *= -1;
  }
  int shifter = num;
  //-- Move to where representation ends
  do {
    ++p;
    shifter = shifter / 10;
  } while(shifter);
  *p = '\0';
  //-- Move back, inserting digits as u go
  do {
    *--p = digit[num%10];
    num = num / 10;
  } while(num);
  return str;
}

void printNumber(double number, int decimals)
{
	int powDec = 1;

	int i;
	for(i = 0; i < decimals; i++) {
		powDec *= 10;
	}
  
  //-- integer part of number
  int n0 = (int)number;
  //-- integer part * powDec
  int n1 = n0 * powDec;
  //-- the whole number * powDec
  int n2 = (long)(number * powDec);
  //-- decimal part of the number
  int n3 = abs(n2 - n1);

  char piece[10];
  char str[100];

  //-- add integer part
  sprintf(piece, "%d", n0);
	strcpy(str, piece);
	//-- if 0, show as an integer
	if(decimals > 0) {
		//-- add dot
		strcat(str, ".");
		//-- add decimal part
		if(n3 == 0) {
			//-- add extra "0"
			for(i = 0; i < decimals; i++) {
				strcat(str, "0");
			}
		} else {
			sprintf(piece, "%d", n3);
			strcat(str, piece);
		}
	}
	printf("%s", str);
	//halOLED128x64ShowX16(0, 0, str);
}
