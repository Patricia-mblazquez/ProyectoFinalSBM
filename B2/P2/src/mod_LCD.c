/* ** Fichero mod_LCD .c ** */
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "mod_LCD.h"
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"

// Creelo , seañadirá aquí todo el có digo que maneja el LCD

// Debe contener dos threads , uno de gestión y otro de test

// También se irán añadiendo las funciones de inicialización del LCD

osThreadId_t tid_Thread_LCD;                        // thread id de LCD
osThreadId_t tid_Thread_LCD_test;                   // thread id de test

uint32_t spi_errors = 0;

static GPIO_InitTypeDef GPIO_InitStruct;
 
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

//static int flag = 0; 

void th_LCD (void *argument);                       // thread function LCD
void th_LCD_test (void *argument);                  // thread function test

void LCD_Reset (void);
void LCD_Init (void);

static void cb_spi(uint32_t event);

//static void cb_spi(void);

static void Pin_A0(void);
static void Pin_Reset(void);
static void Pin_CS(void);

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
	LCD_Reset ();
	LCD_Init ();
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

//Configuracion de la interfaz SPI
void LCD_Reset (void) {
	//Inicializacion y configuracion del driver SPI para gestionar el LCD
	SPIdrv -> Initialize(cb_spi); //Esto llama al callBack
	SPIdrv -> PowerControl(ARM_POWER_FULL);
	SPIdrv -> Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 1000000); //CAMBIADO
	SPIdrv -> Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	
	//Configura los pines IO que sean necesarios
	Pin_A0();
	Pin_Reset(); 
	Pin_CS();    
	
	//Generear la señal de reset	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(1);
}

//CallBack
static void cb_spi(uint32_t event)
{
	if (event & ARM_SPI_EVENT_TRANSFER_COMPLETE){
		osThreadFlagsSet (tid_Thread_LCD, 0x02);
	}
	if (event & ARM_SPI_EVENT_DATA_LOST) {
		spi_errors++; //perdida de datos
	}
	if (event & ARM_SPI_EVENT_MODE_FAULT) {
		spi_errors++; //Conflicto bus
}
	}

void LCD_wr_data ( unsigned char data )
{
    // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    // Seleccionar A0 = 1;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
    // Escribir un dato ( data ) usando la función SPIDrv -> Send ();
	SPIdrv->Send(&data, 1);
    // Esperar al flag enviado desde la cb_spi () de transferencia finalizada;
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
    // Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

// LCD_wr_cmd : Función que escribe un comando en el LCD.

void LCD_wr_cmd ( unsigned char cmd )
{
    // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    // Seleccionar A0 = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
    // Escribir un comando (cmd) usando la función SPIDrv -> Send ();
	SPIdrv->Send(&cmd, 1);
    // Esperar al flag enviado desde la cb_spi () de transferencia finalizada;
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
    // Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}	


void LCD_Init(){
    LCD_wr_cmd(0xAE);
    LCD_wr_cmd(0xA2);
    LCD_wr_cmd(0xA0);
    LCD_wr_cmd(0xC8);
    LCD_wr_cmd(0x22);
    LCD_wr_cmd(0x2F);
    LCD_wr_cmd(0x40);
    LCD_wr_cmd(0xAF);
    LCD_wr_cmd(0x81);
    LCD_wr_cmd(0x3F); // ajustamos el contraste de la pantalla con un valor de 3F
    LCD_wr_cmd(0xA4);
    LCD_wr_cmd(0xA6);
}

//Pines de GPIO 

static void Pin_A0() //a 0 instrucción, a 1 dato.
{
  __HAL_RCC_GPIOF_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

static void Pin_Reset() 
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Pin = GPIO_PIN_6;
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); //Set es porque empieza en nivel alto sino podriamos get
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void Pin_CS() 
{
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); //Set es porque empieza en nivel alto sino podriamos get
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


