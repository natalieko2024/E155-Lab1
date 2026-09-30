// lab4_starter.c
// Fur Elise, E155 Lab 4
// Updated Fall 2024

#include "STM32L432KC_RCC.h"
#include "TIM15.h"
#include "TIM16.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_FLASH.h"

// Pitch in Hz, duration in ms
const int notes[][2] = {
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{416,	125},
{494,	125},
{523,	250},
{  0,	125},
{330,	125},
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{523,	125},
{494,	125},
{440,	250},
{  0,	125},
{494,	125},
{523,	125},
{587,	125},
{659,	375},
{392,	125},
{699,	125},
{659,	125},
{587,	375},
{349,	125},
{659,	125},
{587,	125},
{523,	375},
{330,	125},
{587,	125},
{523,	125},
{494,	250},
{  0,	125},
{330,	125},
{659,	125},
{  0,	250},
{659,	125},
{1319,	125},
{  0,	250},
{623,	125},
{659,	125},
{  0,	250},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{416,	125},
{494,	125},
{523,	250},
{  0,	125},
{330,	125},
{659,	125},
{623,	125},
{659,	125},
{623,	125},
{659,	125},
{494,	125},
{587,	125},
{523,	125},
{440,	250},
{  0,	125},
{262,	125},
{330,	125},
{440,	125},
{494,	250},
{  0,	125},
{330,	125},
{523,	125},
{494,	125},
{440,	500},
{  0,	0}};

int main(void) {

  // configure FLASH
  configureFlash();
  // Set up PLL as main clock
  configureClock();

  // Turn on clock and write to APB2 register enable so it go to timer 15 and 16
  RCC->APB2ENR |= (0b11 << 16);

  RCC->AHB2ENR |= (1 << 0);    // enable GPIOA

  initTIM15();
  initTIM16();
  
  pinMode(6, GPIO_ALT);   // set up GPIO pin function
  
  GPIO->AFRL |= (0b1110 << 24); // set pin a6 as output (24 because pin 6 and each as 4 bits, 14 because channel 1 of TIM16)

  for (int i = 0; i < (sizeof(notes) / sizeof(notes[0])); i++) {
    configureTIM16(notes[i][0]);
    configureTIM15(notes[i][1]);
  };
  

  //Configure clocks in RCC (as shown earlier in this lecture; take note of system clock frequency)
  //Turn on clock to timer in RCC
  //Select correct clock source in TIM control (make sure slave mode is disabled)
  //Configure counter
  //Prescaler register (TIMx_PSC)
  //Auto-reload register (TIMx_ARR)
  //Enable counter CEN in TIMx_CR
	
}