
int str2int(char* str);
int str2int2(char* str);
char* int2str(int num, char str[]);
char* itoa(int num, char str[]);

double str2float(char* str);
//char* float2str(float num, char str[]);

#if DEBUG_PRINT_UART
	void printNumber(long double number, int decimals);
	void osal_printf(char str[]);
#endif

int8 encodeU8to8(uint8 byte);
uint16 implodeU8toU16(uint8 byte1, uint8 byte2);
uint8* explodeU16toU8(uint16 byte);
uint16 convert16toU16(int16 in);

char* int2hex(int value, uint8 upperCase, uint8 prefix);
char* str2upper(char* str, uint8 length);