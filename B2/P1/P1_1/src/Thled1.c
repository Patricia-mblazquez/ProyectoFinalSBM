#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread1;                        // thread id
 
void Thread1 (void *argument);                   // thread function

static GPIO_InitTypeDef GPIO_InitStruct;
void ledInit1 (void);
 
int Init_Thread1 (void) { //Creacion del thread
 
  tid_Thread1 = osThreadNew(Thread1, NULL, NULL);
  if (tid_Thread1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread1 (void *argument) {
		
    ledInit1();
	
  while (1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		osDelay (200); //Cambio y hago que se encineda durante 200
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		osDelay (800); //Cambio con el toogle y hago que se enciemda durante 800ms

  ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void ledInit1 (void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



