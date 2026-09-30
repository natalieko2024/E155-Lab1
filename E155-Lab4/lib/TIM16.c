#include "TIM16.h"

void initTIM16(void) {
    TIM16->PSC = 19;  // set prescaler
    
    // mode select for PWM and set it as output
    TIM16->CCMR1 &= ~(1 << 16);     // clear OCM1[3:0]
    TIM16->CCMR1 &= ~(0b111 << 4);
    TIM16->CCMR1 |= (0b110 << 4);   // PWM to channel 1
    TIM16->CCMR1 |= (1 << 3);       // enable preload
    TIM16->CCMR1 &= ~(0b11 << 0);    // PWM is output

    TIM16->BDTR |= (1 << 15);   // output enable
    TIM16->CCER &= ~(1 << 1);   // output active high
    TIM16->CCER |= (1 << 0);   // output enable (part 2)
    TIM16->CR1 |= (1 << 7);     // need this to use PWM

    TIM16->EGR |= (1 << 0);     // Write 1 to UG which makes us write to all registers
    TIM16->CR1 |= (1 << 0);     // Bit 0 of CR1 is CEN, this makes the timer use the system clock
    TIM16->CNT &= 0;        // count starts at 0
}

void configureTIM16(int pitch) {
    
    if (pitch == 0) {
        TIM16->ARR &= 0;
    } else {
        int ARR16 = (80000000)/(pitch*20) - 1;
        TIM16->ARR = ARR16;   // set maxcount
    };
    
    TIM16->CCR1 = (((80000000)/(pitch*20) - 1)/2);  //50% duty cycle

    TIM16->EGR |= (1 << 0);    // Write 1 to UG which makes us write to all registers

    TIM16->CNT = 0;        // count starts at 0
}