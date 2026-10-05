#include "main.h"
#include <stdio.h>
#include "stm32l432xx.h"

int pinB;
int direction;
int countA;
float speed;

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

int main(void) {
    // Enable LED as output
    gpioEnable(GPIO_PORT_A);
    pinMode(ENCODER_A_PIN, GPIO_INPUT);
    pinMode(ENCODER_B_PIN, GPIO_INPUT);
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(ENCODER_A_PIN)); // Set PA7 as pull-up (PUPD7 = 01)
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(ENCODER_B_PIN)); // Set PA7 as pull-up (PUPD7 = 01)

    // Initialize timer
    RCC->APB1ENR1 |= (1 << 0); // TIM2EN
    initTIM(SPEED_TIM, 4e6);
    RCC->APB2ENR |= (0b01 << 16); // TIM15EN
    initTIM(PRINT_TIM, 10000);

    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= (1 << 0); // SYSCFGEN
    // 2. Configure EXTICR for the input button interrupt
    // EXTI7 is bits 14:12 of EXTICR2 (EXTICR[1] in C). Port A is 0b000, so clearing the field selects PA6.
    SYSCFG->EXTICR[1] &= ~(0b111 << 8);

    // Enable interrupts globally
    __enable_irq();
    // set priority of interrupts
    __NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 1);
    __NVIC_SetPriority(EXTI9_5_IRQn, 2);

    // Configure interrupt for falling edge of GPIO pin for button
    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_A_PIN));   // 1. Configure mask bit
    EXTI->FTSR1 &= ~(1 << gpioPinOffset(ENCODER_A_PIN)); // 2. Disable falling edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_A_PIN));  // 3. Enable rising edge trigger
    NVIC->ISER[0] |= (1 << 23);                       // 4. Turn on EXTI interrupt in NVIC_ISER (EXTI9_5 is IRQ 23)

    while(1){
        
        if (pinB == 0) {
            direction = CW;
        } else {
            direction = CCW;
        }
        
        speed = (float) countA / 408.0f;

        if (PRINT_TIM->CNT == 10000) {
            countA = 0;
            printf("Speed: %f ", speed);
            printf("Direction (1=CW, 0=CW): %d\n", direction);
            PRINT_TIM->SR &= ~(0x1); // Clear UIF
            PRINT_TIM->CNT = 0;      // Reset count
        }
        
    }

}

// EXTI lines 5-9 share this handler
void EXTI9_5_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(ENCODER_A_PIN))){

        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 = (1 << gpioPinOffset(ENCODER_A_PIN));

        pinB = digitalRead(ENCODER_B_PIN);

        countA++;

    }
}
