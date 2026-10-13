#ifndef INPUT_CAPTURE_H
#define INPUT_CAPTURE_H

#include "stm32f4xx_hal.h"

// Le decimos al resto del programa que el htim4 existe aquí
extern TIM_HandleTypeDef tim4; 

// Funciones que el main.c va a usar
void InputCapture_Init(void);

#endif