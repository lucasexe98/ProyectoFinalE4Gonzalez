#include <stdint.h>
#include "stm32f4xx.h"

#define TIM4_PSC    16
#define TIM4_ARR    1000
#define PWM_COUNT   300

void timerInit(void);
void measurePWM(float *dutyCycle);