#include "cmsis_os2.h"  
#include "stm32f4xx.h" 
#include "Thjoy.h"


//VARIABLES GLOBALES
osThreadId_t tid_Control;
osTimerId_t timer_id;
static GPIO_InitTypeDef GPIO_InitStruct;

int cont_gestos = 0;

void TimerJoy_CallBack (void *arg){
	
	int valor = 0;
    // Lectura de pines 
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET) { 
        valor = 1; 
    } 
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_SET) { 
        valor = 2; 
    }
    else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_SET) { 
        valor = 3; 
    }
    else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14) == GPIO_PIN_SET) { 
        valor = 4; 
    }
    else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == GPIO_PIN_SET) { 
        valor = 5; 
    }

    if (valor != 0) {
        cont_gestos++;
    }
		
		// Dependiendo de donde hayamos pulsado el joystick pues valor valdra un numero que representa que se enciende.
    switch (valor) {
        case 1: // 001
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;
        case 2: // 010
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;
        case 3: // 011
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;
        case 4: // 100
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
            break;
        case 5: // 101
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
            break;
    }
}

void Thjoy_Init(void){
		//Creamos timer que solo se dispara una vez y lo arrancamos
    timer_id = osTimerNew(TimerJoy_CallBack, osTimerOnce, NULL, NULL);
}





