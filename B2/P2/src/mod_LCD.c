/* ** Fichero mod_LCD .c ** */
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "mod_LCD.h"

// Creelo , seañadirá aquí todo el có digo que maneja el LCD

// Debe contener dos threads , uno de gestión y otro de test

// También se irán añadiendo las funciones de inicialización del LCD

osThreadId_t tid_Thread_LCD;                        // thread id de LCD
osThreadId_t tid_Thread_LCD_test;                   // thread id de test
 
void th_LCD (void *argument);                       // thread function LCD
void th_LCD_test (void *argument);                  // thread function test

// Inicialización del hilo de gestión
int Init_Thread_LCD (void) {
 
  tid_Thread_LCD = osThreadNew(th_LCD, NULL, NULL);
  if (tid_Thread_LCD == NULL) {
    return(-1);
  }
 
  return(0);
}

// Inicialización del hilo de test 
int Init_Thread_LCD_test (void) {
 
  tid_Thread_LCD_test = osThreadNew(th_LCD_test, NULL, NULL);
  if (tid_Thread_LCD_test == NULL) {
    return(-1);
  }
 
  return(0);
}

// Thread de gestión
void th_LCD (void *argument) {

// Llamadas a las funciones de inicialización (del hardware, no del hilo)

while (1) {

// Esperar un flag desde el thread de test
	osThreadFlagsWait(0x001, osFlagsWaitAny, osWaitForever);
// Representar en el LCD la información

}

}

// Thread de test
void th_LCD_test (void *argument) {

// Su función es probar th_LCD

while (1) {

// Esperar 1 segundo
	osDelay(1000); 
        
// Enviar una señal a th_LCD para representar información en el LCD
	if (tid_Thread_LCD != NULL) {
            osThreadFlagsSet(tid_Thread_LCD, 0x001);
        }
}

}