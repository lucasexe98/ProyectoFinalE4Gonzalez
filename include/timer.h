#include <stdint.h>
#include "stm32f4xx.h"

#define TIM4_PSC    16 // 1KHz
#define TIM4_ARR    1000
#define PWM_COUNT   500  //(ciclo util)

void timerInit(void);
void measurePWM(float *dutyCycle);