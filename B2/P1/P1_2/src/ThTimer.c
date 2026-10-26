#include "cmsis_os2.h"  
#include "stm32f4xx.h" 
#include "ThTimer.h"

//VARIABLE GLOBAL
osTimerId_t timer_id;

void TimerLed_CallBack (void *arg){
    // Si no llega la flag pasa esto Fin 3s, encendemos rojo y apagamos verde
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

void ThTimer_Init(void){
    // Creamos timer que solo se dispara una vez, pero NO lo arrancamos todavía
    timer_id = osTimerNew(TimerLed_CallBack, osTimerOnce, NULL, NULL);
}