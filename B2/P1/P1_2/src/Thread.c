#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx.h" 
#include "Thread.h"
#include "ThTimer.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
 static GPIO_InitTypeDef GPIO_InitStruct;
 
osThreadId_t tid_Thread;                        // thread id
 
void Thread (void *argument);                   // thread function
void Init_Led (void);
void Init_Boton_Azul (void);
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
	
	Init_Led();
	Init_Boton_Azul ();
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); //Habilitamos la interrupcion para atender lo del boton
	
	//Estado de inicio: verde ON, rojo OFF
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	
	osTimerStart(timer_id, 3000U); //Aqui arrancamos el timer para que si llega a 3s se encienda el led rojo llamando al callback del timer
	
	
  while (1) {
    ; // Insert thread code here...
		 osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
        
     if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) { //Este reset lo que significa es si el led rojo esta apagado
            osTimerStart(timer_id, 3000U); //Reiniciamos la cuenta de 3s 
		}
    osThreadYield();                            // suspend thread
  }
}

void Init_Led (void) {
	//LEDS 3 y 1
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
		__HAL_RCC_GPIOB_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_0;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
}

void Init_Boton_Azul (void) {
	__HAL_RCC_GPIOC_CLK_ENABLE();//SIEMPRE PARA CD COSA OJOOO
	
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
