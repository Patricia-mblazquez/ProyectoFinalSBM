/* ** Fichero mod_LCD.h ** */
#ifndef MOD_LCD_H
#define MOD_LCD_H

#include <stdint.h>
#include "cmsis_os2.h"

// Estructura del mensaje
typedef struct {
    uint8_t Buf[40];
    uint8_t linea;
} MSGQUEUE_LCD_t;

extern osMessageQueueId_t mid_LCD;

int Init_Thread_LCD (void);
int Init_Thread_LCD_test (void);

#endif