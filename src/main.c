#include <stdint.h>
#include "stm32f4xx.h"
#include "timer.h"
#include "usart3.h"

#define BUTTON_PRESSED (GPIOA->IDR & (1U<<0))

/*
PD12: PWM TIM4 OUTPUT
PB10: UART3 TX
PA6:  TIM3 INPUT PWM MEASURE
PA0:  User Button
*/

int main(void){
    /*Inicializo TIMER*/
    timerInit();
    /*Inicializo UART*/
    usart3_init();
    /*Inicializo pulsador PA0 para realizar la medicion*/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER &= ~(0x3<<0);
    /*Inicializo variables locales*/
    float dutyCycle = 0;
    char auxstr[20]={0};
    while(1){
        if(BUTTON_PRESSED){
            measurePWM(&dutyCycle);
            usart3_tx_s("Ciclo de trabajo: 0");
            usart3_tx_s(float2string(dutyCycle, auxstr));
            newLine();
        }
    }
}

