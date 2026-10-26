#include "cmsis_os2.h"                          // CMSIS RTOS header file 
#include "stm32f4xx.h" 
#include "Thjoy.h"
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Thread;                        // thread id
 
void Thread (void *argument);                   // thread function

void Init_Joystick (void);
void Init_Led (void);
static GPIO_InitTypeDef GPIO_InitStruct;
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew(Thread, NULL, NULL);
  if (tid_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread (void *argument) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
	
 
	Init_Led ();
	Init_Joystick();
	
	//Estado de incio: todos ledf OFF
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	while (1) { //Para los rebotes, cuando cd vez q aparece un flanco más bajo de 50ms reinica el timer
        //Espera a la flag mientras esta bloquedo 
        osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
        osTimerStart(timer_id, 50U); //Cd vez que recible la señal reinicia a 50ms el timer
        
    }
	
  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}

void Init_Led (void) {
	//LEDS 
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

		GPIO_InitStruct.Pin = GPIO_PIN_0;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);		
}

void Init_Joystick (void) {
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Inicializar Joystick 
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}
