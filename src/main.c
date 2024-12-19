#include <stdint.h>
#include "stm32f4xx.h"
#include "timer.h"
#include "usart3.h"

#define BUTTON_PRESSED (GPIOA->IDR & (1U<<0))

/* Variables Globales */

/*El valor flotante de dutyCycle se convierte texto y es enviado a traves de la UART*/
float dutyCycle = 0; // Ciclo de trabajo calculado
char auxstr[20] = {0}; // Cadena para conversión y transmisión por UART 

void button_init(void) {
    /* Habilita el reloj para GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    /* Configura PA0 como entrada */
    GPIOA->MODER &= ~(0x3 << 0);
}

/*
PD12: PWM TIM4 OUTPUT
PB10: UART3 TX  //PA10 en la bluepill
PA6:  TIM3 INPUT PWM MEASURE
PA0:  User Button
*/

int main(void){
    /*Inicializo TIMER*/
    timerInit();
    /*Inicializo UART*/
    usart3_init();
    button_init(); // Configuracion del boton PA0

    while(1){
        if(BUTTON_PRESSED){ //Si esta el boton prendido medi y mandalo
            measurePWM(&dutyCycle);
            usart3_tx_s("Ciclo de trabajo: 0");
            usart3_tx_s(float2string(dutyCycle, auxstr));
            newLine();
        }
    }
}

