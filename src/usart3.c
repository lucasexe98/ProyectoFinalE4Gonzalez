#include "usart3.h"

uint32_t compute_usart_div(uint32_t PeriphClk, uint32_t BaudRate, uint8_t over8);
void set_usart_div(USART_TypeDef *USART, uint32_t PeriphClk, uint32_t BaudRate, uint8_t over8);

void usart3_init(void){
    /*Se conecta el reloj del puerto B*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	/*Se configura PB10 y PB11 como Alternate Function(AF)*/
	GPIOB->MODER &= ~(1U<<20);
	GPIOB->MODER |= (1U<<21);
	GPIOB->MODER &= ~(1U<<22);
	GPIOB->MODER |= (1U<<23);
	/*Se configuran los pines PB10 y PB11 en AF7(USART3)*/
	GPIOB->AFR[1] &= ~(0xF<<8);
	GPIOB->AFR[1] |= (7U<<8);
	GPIOB->AFR[1] &= ~(0xF<<12);
	GPIOB->AFR[1] |= (7U<<12);
	/*Se conecta el reloj USART3*/
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    /*Se configura el BaudRate*/
	set_usart_div(USART3, APB1_FREQ, BAUD_RATE, 1);
	/*Se activa módulo USART, junto con la recepcion y la transmision*/
	USART3->CR1 = (1U<<15)|(1U<<13)|(1U<<3)|(1U<<2);//sacar (1U<<15) si over8=0
}

void set_usart_div(USART_TypeDef *USART, uint32_t PeriphClk, uint32_t BaudRate, uint8_t over8){
	USART->BRR = compute_usart_div(PeriphClk, BaudRate, over8);
}

uint32_t compute_usart_div(uint32_t PeriphClk, uint32_t BaudRate, uint8_t over8){
	float usart_div_f= PeriphClk/(8*(2-over8)*BaudRate);
	int mantissa = (int)usart_div_f;
	int div= (int)(16*(usart_div_f - mantissa));
	return (mantissa<<4)|(div);
}

void usart3_tx_c(char c)
{
	//Pongo el caracter a transmitir en el DR
	USART3->DR = c;
	while(!(USART3->SR & 0x00000080));
	while(!(USART3->SR & 0x00000040));
}

void usart3_tx_s( char *msg)
{
	int cnt = 0;;
	while(msg[cnt] != '\0'){
		usart3_tx_c(msg[cnt]);
		cnt++;
	}
}

void newLine()
{
	usart3_tx_c(10); // nueva linea
	usart3_tx_c(13); // retorno de cursor
}

signed int string2int(char *s) //Funcion de conversion de cadena de caracteres a entero
{
   signed int result;
   unsigned int sign, index;
   char c;
   index = 0;
   sign = 0;
   result = 0;
   c = s[index];
   index++;
   if (c == '-')
   {
      sign = 1;
      c = s[index];
		index++;
   }
   else if (c == '+')
   {
      c = s[index];
	  index++;
   }
   if (c >= '0' && c <= '9')
   {
	 while (c >= '0' && c <= '9')
	 {
		result = 10*result + (c - '0');
		c = s[index];
		index++;
	 }
   }
   if (sign == 1) result = -result;
   return(result);
}

char * int2string(signed int num, char * s)
{
     unsigned int temp=1;
     unsigned int i,sign=0,cnt=0;
     char c;

     if(num<0) {  									
         sign=1;
         num*=-1;
     }
     while(temp>0) {

         s[cnt]=(num%10)+'0';    					

         cnt++;

		 temp=(num/10);
         num=temp;
     }
     if(sign==1) {
         s[cnt]=0x2D;      							
         cnt++;
     }
     for(i = 0;i<(int)(cnt/2);i++) {

         c=s[i];
         s[i]=s[cnt-i-1];        					
         s[cnt-i-1]=c;
     }
     s[cnt]='\0';     								
     return s;
}

char * float2string(float num,char *cadena)
{
	double nbf=num;
	int inc=0,i=0,end=0;
	int nb;

	if(nbf<0)
	{
		cadena[0]='-';
		nbf=-nbf;
		++i;
	}

	while(nbf>=1)
	{
		nbf/=10;
		++inc;
	}

	while(!end)
	{
		nbf*=10;
		nb=(int)nbf;

		if(inc == -2) end=1; //Convierto solo 2 lugares despues de la coma

		nb%=10;

		if(inc==0)
		{
			cadena[i]='.';
			++i;
		}
		cadena[i]=nb+'0';
		inc--;
		i++;
	}

	cadena[i]='\0';
	return(cadena);
}