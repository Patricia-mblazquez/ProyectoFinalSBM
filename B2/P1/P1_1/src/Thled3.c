#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "Thled2.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread3;                        // thread id
static int cont = 0;

void Thread3 (void *argument);                   // thread function

static GPIO_InitTypeDef GPIO_InitStruct;
void ledInit3 (void);
 
int Init_Thread3 (void) { //Creacion del thread
 
  tid_Thread3 = osThreadNew(Thread3, NULL, NULL);
  if (tid_Thread3 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread3 (void *argument) {
		
    ledInit3();
	
  while (1) {
		osThreadFlagsWait(0x0002U, osFlagsWaitAny, osWaitForever); //Cuando reciba la señal del led2
		for (cont = 1; cont <= 30; cont ++){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
			osDelay (287); //Cambio y hago que se encineda durante 200
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
			osDelay (287); //Cambio con el toogle y hago que se enciemda durante 800ms
			
			if (cont == 25) { //A los 25 ciclos, retroalimenta al LED2
            osThreadFlagsSet(tid_Thread2, 0x0001U);
        }
		}

  ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void ledInit3 (void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



