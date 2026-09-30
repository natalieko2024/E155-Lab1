#include "TIM15.h"

int MAXCOUNT15;
const int PSC15 = 3999;

void initTIM15(void) {
    TIM15->PSC = PSC15;  // set prescaler
    
    TIM15->SMCR &= ~(1 << 16);    // Turn slave mode select off, so we can use Ug and Counter EN as inputs
    TIM15->SMCR &= ~(0b111 << 0);
    
    TIM15->EGR |= (1 << 0);    // Write 1 to UG which makes us write to all registers
    TIM15->CR1 |= (1 << 0);   // Bit 0 of CR1 is CEN, this makes the timer use the system clock
    TIM15->CNT = 0;        // count starts at 0
}

void configureTIM15(int duration) {
    
    MAXCOUNT15 = (duration)/(0.05);
    TIM15->ARR = MAXCOUNT15;   // set maxcount

    TIM15->EGR |= (1 << 0);    // Write 1 to UG which makes us write to all registers

    // Clear UIF
    TIM15->SR &= ~(1 << 0);

    TIM15->CNT = 0;        // count starts at 0

    while ((TIM15->SR & 1) == 0); 

    TIM15->SR |= ~(1 << 0);
}