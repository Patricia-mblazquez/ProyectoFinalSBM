#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file


extern osThreadId_t tid_Thread2; 
int Init_Thread2 (void);
void Thread2 (void *argument);
void ledInit2 (void);