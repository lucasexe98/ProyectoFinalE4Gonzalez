#include "timer.h"


static void tim4_init(void);                        // Inicializo TIM4 (generación de PWM)
static void tim4_ch1_outputCompare_config(void);    // Configuro el canal 1 de TIM4 para modo Output Compare (PWM)
static void tim3_init(void);                        // Inicializo TIM3 (medición de PWM)
static void tim3_ch1_inputCapture_config(void);     // Configuro el canal 1 de TIM3 para Input Capture

void timerInit(void){
    tim4_init(); // Configuro el tim4 para generar la señal PWM
    tim3_init(); // Configuro el tim3 para medir la señal PWM
}

static void tim4_init(void){
    /*Conecto el reloj del GPIOD*/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /*Conecto el reloj del TIMER4*/
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    /*Configuro PD12 como funcion alternativa*/ 
    GPIOD->MODER |= (1U<<25);
    GPIOD->MODER &= ~(1U<<24);

    /*Configuro PD12 como funcion alternativa 2 (timer4)*/
    GPIOD->AFR[1] |= (1U<<17);
    GPIOD->AFR[1] &= ~((1U<<19)|(1U<<18)|(1U<<16));

    /*Defino el prescaler del contador (como no se configuraron los relojes del uc, llegan 16MHz al TIM4)*/
    TIM4->PSC = TIM4_PSC - 1; // // Prescaler que divide por 16, dejando 1 MHz

    /*Configuro la carga del contador*/
    TIM4->ARR = TIM4_ARR -1; // Cuenta hasta 1000 (1 kHz)

    /*Configuro el canal 1 en modo output compare*/
    tim4_ch1_outputCompare_config(); //Configurarlo como PWM

    /*Fuerzo el contador para que comience a cero*/
    TIM4->CNT = 0; //Reinicio el contador a cero

    /*Habilito el modulo TIM4*/
    TIM4->CR1 |= TIM_CR1_CEN; //Hasta que no le escribo en el registro CR1 un 1 en el bit 0 no empieza a contar
}

static void tim4_ch1_outputCompare_config(void){
    /*Configuro el timer como output*/
    TIM4->CCMR1 &= ~((1U<<0) | (1U<<1));

    /*Configuro el timer en modo output compare PWM*/
    TIM4->CCMR1 |= (1U<<6) | (1U<<5);
    TIM4->CCMR1 &= ~(1U<<4);

    /*Configuro la cuenta que define el ciclo de trabajo de la PWM*/
    TIM4->CCR1 = PWM_COUNT; 

    /*Activo el modo output compare*/
    TIM4->CCER |= TIM_CCER_CC1E;
} 
/*
Compara el valor de la cuenta y a partir de eso genera una salida
*/

static void tim3_init(void) {
    /* Conecto el reloj del GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /*Configuro PA6 en modo AF*/
	GPIOA->MODER |= (1U<<13);
	GPIOA->MODER &= ~(1U<<12);

	/*Configuro el tipo de AF(AF2 para timer 3) para PA6*/
	GPIOA->AFR[0] &= ~(0b1111<<24);
	GPIOA->AFR[0] |= (0b0010<<24);

    /* Conecto el reloj del TIMER3 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    /* Defino el prescaler del contador (como no se configuraron los relojes del uc, llegan 16MHz al TIM3) */
    TIM3->PSC = TIM4_PSC - 1;

    /* Configuro la carga del contador */
    TIM3->ARR = 60000 - 1;

    /* Configuro el canal 1 en modo input capture */
    tim3_ch1_inputCapture_config();

    /* Fuerzo el contador para que comience a cero */
    TIM3->CNT = 0;

    /* Habilito el módulo TIM3 */
    TIM3->CR1 |= TIM_CR1_CEN;
}
/*
A partir de lo que pasa un evento en la entrada cuenta cuanto pasa entre evento y evento.
Para medir la PWM (input compare)
*/

static void tim3_ch1_inputCapture_config(void){
    /*Configuro como input el canal 1 usando como entrada timer input 1*/
	TIM3->CCMR1 &= ~(0x3<<0);
	TIM3->CCMR1 |= (1U<<0);
    /*Activo deteccion de flanco ascendente para el canal 1*/
    TIM3->CCER &= ~TIM_CCER_CC1P;
    TIM3->CCER &= ~TIM_CCER_CC1NP;
    /*Configuro como input el canal 2 usando tambien como entrada timer input 1*/
    TIM3->CCMR1 &= ~(0x3<<8);
    TIM3->CCMR1 |= (1U<<9);
    /*Activo deteccion de flanco descendente para el canal 2*/
    TIM3->CCER |= TIM_CCER_CC2P;
    TIM3->CCER &= ~TIM_CCER_CC1NP;
    /*Configuro la activacion(trigger) a partir de input 1*/
    TIM3->SMCR &= ~(0x7<<4);
    TIM3->SMCR |= (1U<<6)|(1U<<4);
    /*Configuro el reinicio del timer a partir del flanco ascendente del trigger*/
    TIM3->SMCR &= ~(0x7<<0);
    TIM3->SMCR |= (1U<<2);
    /*Habilito el modo input capture para ambos canales*/
    TIM3->CCER |= TIM_CCER_CC1E|TIM_CCER_CC2E;

    /*
    Deberia dar un mensaje de cuando no mida nada de un error, y si mide 
    que aparezca el valor que midio
    */

}

void measurePWM(float *dutyCycle) {
    
    volatile uint16_t periodWidth = TIM3->CCR1 + 1; // Duración total del periodo
    volatile uint16_t pulseWidth = TIM3->CCR2 + 1;  // Duración de la parte activa (alto)

    //Que no solamente mida el pulso, que cuando no dectecte nada detecto un error diciendo que no esta midiendo

    /*Calculo el valor del dutyCycle*/
    *dutyCycle = (float) pulseWidth / periodWidth;
}

