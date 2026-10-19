#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread2;                        // thread id
 
void Thread2 (void *argument);                   // thread function

static GPIO_InitTypeDef GPIO_InitStruct;
void ledInit2 (void);
 
int Init_Thread2 (void) { //Creacion del thread
 
  tid_Thread2 = osThreadNew(Thread2, NULL, NULL);
  if (tid_Thread2 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread2 (void *argument) {
		
    ledInit2();
	
  while (1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		osDelay (137); //Cambio y hago que se encineda durante 200
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		osDelay (137); //Cambio con el toogle y hago que se enciemda durante 800ms

  ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void ledInit2 (void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



