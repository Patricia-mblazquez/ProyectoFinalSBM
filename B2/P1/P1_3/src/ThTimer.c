#include "cmsis_os2.h"  
#include "stm32f4xx.h" 
#include "ThTimer.h"

//VARIABLES GLOBALES
osThreadId_t tid_Control;
osTimerId_t timer_id;
static GPIO_InitTypeDef GPIO_InitStruct;


//FUNCIONES

//Creacion y funcionamiento del hilo
void Thread_Control(void *argument) {
    while (1) {
        //Espera a la flag mientras esta bloquedo 
        osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
        
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) { //Este reset lo que significa es si el led rojo esta apagado
            osTimerStart(timer_id, 3000U); //Reiniciamos la cuenta de 3s 
        }
    }
}

void TimerLed_CallBack (void *arg){
	// Fin 3s, encendemos rojo y apagamos verde
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

void ThTimer_Init(void){
	//LEDS 3 y 1
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		
		//Estado de incio: verde ON, rojo OFF
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		
		//Creamos el hilo
		tid_Control = osThreadNew(Thread_Control, NULL, NULL);

		//Creamos timer que solo se dispara una vez y lo arrancamos
		timer_id = osTimerNew(TimerLed_CallBack, osTimerOnce, NULL, NULL);
		osTimerStart(timer_id, 3000U);
}





