#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "Thled3.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread2;                        // thread id
 
void Thread2 (void *argument);                   // thread function

static GPIO_InitTypeDef GPIO_InitStruct;
static int cont = 0;

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
		osThreadFlagsWait(0x0001U, osFlagsWaitAny, osWaitForever); //Esperando la señal que le viene de 1 que se la manda a los 5 ciclos
		
		for (cont = 1; cont <= 20; cont ++){ //Se enciendo durante 20 ciclos y se apaga
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			osDelay (137); 
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			osDelay (137);
			
			if (cont == 15){//Cuando lleve 15 ciclos le manda una señal al lel 3
				osThreadFlagsSet (tid_Thread3, 0x0002U);
			}
		}
		
		
		
		
		

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



