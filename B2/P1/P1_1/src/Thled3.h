#include "stm32f4xx_hal.h"
#include "cmsis_os2.h" 

extern osThreadId_t tid_Thread3; 
int Init_Thread3 (void);
void Thread3 (void *argument);
void ledInit3 (void);