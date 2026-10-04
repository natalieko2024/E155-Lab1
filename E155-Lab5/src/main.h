// main.h
// Josh Brake
// jbrake@hmc.edu
// 10/31/22

#ifndef MAIN_H
#define MAIN_H

#include "STM32L432KC.h"
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Custom defines
///////////////////////////////////////////////////////////////////////////////

#define ENCODER_A_PIN PA6
#define ENCODER_B_PIN PA9
#define SPEED_TIM TIM2
#define PRINT_TIM TIM15
#define CCW 0
#define CW 1
#define ROTATION 408

#endif // MAIN_H