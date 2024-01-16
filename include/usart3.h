#include <stdint.h>
#include "stm32f4xx.h"

#define APB1_FREQ			16000000
#define BAUD_RATE			9600

void usart3_init(void);
void usart3_tx_c(char c);
void usart3_tx_s( char *msg);
void newLine();
signed int string2int(char *s);
char * int2string(signed int num, char * s);
char * float2string(float num,char *cadena);