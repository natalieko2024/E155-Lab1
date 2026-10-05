#include "main.h"
#include <stdio.h>
#include "stm32l432xx.h"

int pinA;
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
    // Enable encoder pins as inputs
    gpioEnable(GPIO_PORT_A);
    pinMode(ENCODER_A_PIN, GPIO_INPUT);
    pinMode(ENCODER_B_PIN, GPIO_INPUT);
    // Both pins either pull-up or pull-down so their "logic levels" are the same
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(ENCODER_A_PIN)); // Set PA6 as pull-up (PUPD6 = 01)
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(ENCODER_B_PIN)); // Set PA9 as pull-up (PUPD9 = 01)

    // Initialize timers
    RCC->APB1ENR1 |= (1 << 0); // TIM2EN
    initTIM(SPEED_TIM, 4e6);
    RCC->APB2ENR |= (0b01 << 16); // TIM15EN
    initTIM(PRINT_TIM, 10000);

    // Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= (1 << 0); // SYSCFGEN
    // Configure EXTICR for the input button interrupt
    // EXTI6 is bits 10:8 of EXTICR2 (EXTICR[1] in C). Port A is 0b000, so clearing the field selects PA6.
    SYSCFG->EXTICR[1] &= ~(0b111 << 8);

    // Enable interrupts globally
    __enable_irq();
    // Set priority of interrupts
    // Want print timer to have priority over the interrupts so that all our clocks can be synchronized
    __NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 1);
    __NVIC_SetPriority(EXTI9_5_IRQn, 2);

    // Configure interrupt for both rising and falling edge for both encoder pins
    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_A_PIN));   // Configure mask bit
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODER_A_PIN)); // Enable falling edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_A_PIN));  // Enable rising edge trigger

    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_B_PIN));   // Configure mask bit
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODER_B_PIN)); // Enable falling edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_B_PIN));  // Enable rising edge trigger

    // Turn on EXTI interrupt in NVIC_ISER (EXTI9_5 is IRQ 23)
    NVIC->ISER[0] |= (1 << 23);                       

    // ALWAYS RUN THIS. NO DELAY.
    while(1){
        
        // Calculate speed in rev/s
        speed = ((float) countA/4.0f) / 408.0f;

        // Print at a frequency of 1Hz (so print every second)
        if (PRINT_TIM->CNT == 10000) {
            // Reset count every 1 second (so speed calculation uses max ticks in 1 sec)
            countA = 0;
            // Print speed and direction
            printf("Speed (rev/s): %f ", speed);
            printf("Direction (1=CW, 0=CCW): %d\n", direction);
            // Reset the count for the print timer so we keep the 1Hz frequency
            PRINT_TIM->SR &= ~(0x1); // Clear UIF
            PRINT_TIM->CNT = 0;      // Reset count
        }
        
    }

}

// EXTI lines 5-9 share this handler
void EXTI9_5_IRQHandler(void){
    // Check that the rising or falling edge of encoder A was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(ENCODER_A_PIN))){

        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 = (1 << gpioPinOffset(ENCODER_A_PIN));

        // Read the value of pin A and B -> this helps determine direction
        pinA = digitalRead(ENCODER_A_PIN);
        pinB = digitalRead(ENCODER_B_PIN);

        // Rising edge of A
        if (pinA == 1) {
            // Determine direction of motor spinning
            if (pinB == 0) {
                direction = CW;
            } else {
                direction = CCW;
            }
        } else {
            // Falling edge of A
            if (pinB == 1) {
                direction = CW;
            } else {
                direction = CCW;
            }
        }

        // Increment count of how many rising edges we have seen
        countA++;

    }

    // Check that the rising edge of encoder B was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(ENCODER_B_PIN))){

        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 = (1 << gpioPinOffset(ENCODER_B_PIN));;

        // Increment count of how many rising edges we have seen
        countA++;

    }
}
