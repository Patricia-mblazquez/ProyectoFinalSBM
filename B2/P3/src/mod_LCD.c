/* ** Fichero mod_LCD .c ** */
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "mod_LCD.h"
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
#include "Arial12x12.h"

// Creelo , se añadirá aquí todo el código que maneja el LCD
// Debe contener dos threads , uno de gestión y otro de test
// También se irán añadiendo las funciones de inicialización del LCD

osThreadId_t tid_Thread_LCD;                        // thread id de LCD
osThreadId_t tid_Thread_LCD_test;                   // thread id de test

uint32_t spi_errors = 0;
static unsigned char buffer[512]; 
static uint16_t positionL1 = 0;
static uint16_t positionL2 = 0;

static GPIO_InitTypeDef GPIO_InitStruct;
 
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

//Funcion hilos LCD
void th_LCD (void *argument);                       // thread function LCD
void th_LCD_test (void *argument);                  // thread function test

//Funciones del LCD 
void LCD_Reset (void);
void LCD_Init (void);
void LCD_update(void);

//Funciones de los pines 
static void Pin_A0(void);
static void Pin_Reset(void);
static void Pin_CS(void);

//Funcion de callback
static void cb_spi(uint32_t event);

//Funciones LCD
void cuadradroArribaIzquierda (void);
void letraAIzquierda (void);
void letraABajoIzquierda (void); 
void barraCreciente (void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void Escribe_L1(uint8_t *cadena);
void LCD_symbolToLocalBuffer_L2(uint8_t symbol); 
void Escribe_L2(uint8_t *cadena);

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

    // Llamadas a las funciones de inicialización 
    LCD_Reset();
    LCD_Init();   
    LCD_update(); // Limpia la pantalla al arrancar

    while (1) {
       // El hilo se queda bloqueado aquí durmiendo hasta que th_LCD_test manda el flag 0x001
			osThreadFlagsWait(0x001, osFlagsWaitAny, osWaitForever);
        
      //cuadradroArribaIzquierda ();
      //letraAIzquierda ();
  		//letraABajoIzquierda (); 
			//barraCreciente (); 
      Escribe_L1((uint8_t *)"Practica 3 LCD");
			Escribe_L2((uint8_t *)"Que bien funciona");
			LCD_update();
       
    }
}

void th_LCD_test (void *argument) {
 (void)argument;
	
  while (1) {
		// Esperar 1 segundo
		osDelay(1000);
		
		// Enviar una señal a th_LCD para representar información en el LCD
		osThreadFlagsSet(tid_Thread_LCD, 0x01);
  }
}

//Configuracion de la interfaz SPI
void LCD_Reset (void) 
{
	//Inicializacion y configuracion del driver SPI para gestionar el LCD
	SPIdrv -> Initialize(cb_spi); //Esto llama al callBack
	SPIdrv -> PowerControl(ARM_POWER_FULL);
	SPIdrv -> Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 2000000); //CAMBIADO
	SPIdrv -> Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	
	//Configura los pines IO que sean necesarios
	Pin_A0();
	Pin_Reset(); 
	Pin_CS();    
	
	//Generear la señal de reset	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	osDelay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	osDelay(1);
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
	SPIdrv->Send(&data, sizeof(data));
    // Esperar al flag enviado desde la cb_spi () de transferencia finalizada;
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
    // Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
void LCD_wr_cmd ( unsigned char cmd )
{
    // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    // Seleccionar A0 = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
    // Escribir un comando (cmd) usando la función SPIDrv -> Send ();
	SPIdrv->Send(&cmd, sizeof(cmd));
    // Esperar al flag enviado desde la cb_spi () de transferencia finalizada;
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
    // Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}	


void LCD_Init()
{
    LCD_wr_cmd(0xAE);
    LCD_wr_cmd(0xA2);
    LCD_wr_cmd(0xA0);
    LCD_wr_cmd(0xC8);
    LCD_wr_cmd(0x22);
    LCD_wr_cmd(0x2F);
    LCD_wr_cmd(0x40);
    LCD_wr_cmd(0xAF);
    LCD_wr_cmd(0x81);
    LCD_wr_cmd(0x0F); // ajustamos el contraste de la pantalla con un valor de 3F
    LCD_wr_cmd(0xA4);
    LCD_wr_cmd(0xA6);
}

void LCD_update(void)
{
    int i;
    
    LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
    LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
    LCD_wr_cmd(0xB0); // Página 0
    for (i = 0; i < 128; i++) {
        LCD_wr_data(buffer[i]);
    }
    
    LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
    LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
    LCD_wr_cmd(0xB1); // Página 1
    for (i = 128; i < 256; i++) {
        LCD_wr_data(buffer[i]);
    }
    
    LCD_wr_cmd(0x00);
    LCD_wr_cmd(0x10);
    LCD_wr_cmd(0xB2); // Página 2
    for (i = 256; i < 384; i++) {
        LCD_wr_data(buffer[i]);
    }
    
    LCD_wr_cmd(0x00);
    LCD_wr_cmd(0x10);
    LCD_wr_cmd(0xB3); // Página 3
    for (i = 384; i < 512; i++) {
        LCD_wr_data(buffer[i]);
    }
}

void cuadradroArribaIzquierda (void) 
{
// Recorre los primeros 8 bytes del buffer (esquina superior izquierda).
// Pone 0xFF (11111111 en binario), lo que enciende los 8 píxeles verticales de cada columna.
	for(int a = 0; a < 8; a++) {
		buffer[a] = 0xFF; 
  }
	for(int p = 8; p < 512; p++) {
    buffer[p] = 0x00; 
  }
  LCD_update(); // Manda los datos físicos a la pantalla
}

void letraAIzquierda (void) 
{
	unsigned char letra_A[8] = {0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00, 0x00, 0x00};
	for(int i = 0; i < 8; i++) {
		buffer[i] = letra_A[i]; 
   }
   LCD_update(); 
}

void letraABajoIzquierda (void) 
{
// Sumamos 384 a la 'i'. Como cada página tiene 128 columnas (128x3=384), 
// el índice 384 es exactamente el primer byte de la última página (abajo del todo).
	unsigned char letra_A[8] = {0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00, 0x00, 0x00};
	for(int i = 0; i < 8; i++) {
		buffer[384 + i] = letra_A[i]; 
   }
   LCD_update(); 
}

void barraCreciente (void) 
{
// Comprueba que no nos salgamos del ancho de la pantalla (128).
	static int ancho_barra = 0; 
	if (ancho_barra < 128) { 
		buffer[ancho_barra]       = 0xFF; // Pinta 8 píxeles en la Pág 0 (arriba)
		buffer[ancho_barra + 128] = 0xFF; // Pinta 8 píxeles en la Pág 1
		buffer[ancho_barra + 256] = 0xFF; // Pinta 8 píxeles en la Pág 2
		buffer[ancho_barra + 384] = 0xFF; // Pinta 8 píxeles en la Pág 3 (abajo)
            
		ancho_barra++; // Suma 1. Al siguiente segundo, pintará la columna de al lado.
   }
	LCD_update();
}

void LCD_symbolToLocalBuffer_L1(uint8_t symbol) 
{
    uint8_t i, value1, value2;
    uint16_t offset = 0;
    uint8_t char_width = 0;
    
    offset = 25 * (symbol - ' ');
    char_width = Arial12x12[offset]; // Leemos el ancho de la letra actual
    
    // Control para que no sobreescriba al llegar al final
    if (positionL1 + char_width > 128) {
        positionL1 = 0; 
        for(int j = 0; j < 256; j++) {
            buffer[j] = 0x00;
        }
    }
    
    //Dibujamos el carácter
    for (i = 0; i < 12; i++) { //Como maximo son 12 pixeles de ancho, el buble pinta columna vertical por columna vertical
            value1 = Arial12x12[offset + i * 2 + 1]; //Guarda los 8 píxeles de la mitad superior de la letra.
            value2 = Arial12x12[offset + i * 2 + 2]; //Guarda los 4 píxeles restantes de la mitad inferior.
			//Dividimos los dos valores porque son 12 bytes entonces como cada pagina tiene 8 bytes necesitamos llegar a las 
			//dos paginas por eso dividimos el valor     
            buffer[i + positionL1] = value1; //Pagina 0
            buffer[i + 128 + positionL1] = value2; //Pagina 1
    }
    
		positionL1 = positionL1 + char_width; 
	
}

void Escribe_L1 ( uint8_t * cadena ) 
{
	// Se recorre la cadena y se invoca a la funcion LCD_symbolToLocalBuffer_L1
	// con cada uno de los caracteres
    positionL1 = 0;
    
  //Limpiar
    for(int j = 0; j < 256; j++) {
        buffer[j] = 0x00;
    }
    
    while (*cadena != '\0') {
        LCD_symbolToLocalBuffer_L1(*cadena);
        cadena++; // Avanza a la siguiente letra de la frase
    }
}

void LCD_symbolToLocalBuffer_L2(uint8_t symbol) 
{
    uint8_t i, value1, value2;
    uint16_t offset = 0;
    uint8_t char_width = 0;
    
    offset = 25 * (symbol - ' ');
    char_width = Arial12x12[offset]; 
    
    // Limpia
    if (positionL2 + char_width > 128) {
        positionL2 = 0; 
        for(int j = 256; j < 512; j++) {
            buffer[j] = 0x00;
        }
    }
    
    for (i = 0; i < 12; i++) {
        if ((positionL2 + i) < 128) {
            value1 = Arial12x12[offset + i * 2 + 1];
            value2 = Arial12x12[offset + i * 2 + 2];
            
            buffer[i + 256 + positionL2] = value1; // Pagina 2
            buffer[i + 384 + positionL2] = value2; // Pagina 3
        }
    }
    
    positionL2 = positionL2 + char_width; 
}

void Escribe_L2 ( uint8_t * cadena ) 
{
    positionL2 = 0;
    
    // Limpia
    for(int j = 256; j < 512; j++) {
        buffer[j] = 0x00;
    }
     
    while (*cadena != '\0') {
        LCD_symbolToLocalBuffer_L2(*cadena);
        cadena++; 
    }
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


