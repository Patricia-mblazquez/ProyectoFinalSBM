#include "input_capture.h"

TIM_HandleTypeDef tim4;	
 
static uint32_t captura1 = 0;      // Guarda el valor del cronómetro en el 1er flanco
static uint32_t captura2 = 0;      // Guarda el valor del cronómetro en el 2º flanco
static uint32_t diferencia = 0;    // Resta entre captura2 y captura1
volatile uint32_t frecuencia = 0;     // Resultado final en Hz (para ver en el Watch)

static void initPIN_INPUT(void);
static void initTimer_IC(void);

//Iniciacion de funciones
void InputCapture_Init(void){
	initPIN_INPUT();
	initTimer_IC();
}

static void initPIN_INPUT(void){ //Fabrica la onda con el tim4
  GPIO_InitTypeDef GPIO_InitStruct = {0};
        
  // Habilitamos el reloj del puerto B
  __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // Configuramos PB6 para el TIM4
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void initTimer_IC(void) { //Ahora usamos IC (In put Capture) en vez de OC (output capture) 
    TIM_IC_InitTypeDef TIM_Channel_InitStruct = {0};
    
    // Enable clock to timer-4
    __HAL_RCC_TIM4_CLK_ENABLE();
    
		HAL_NVIC_EnableIRQ(TIM4_IRQn);               // Habilita la interrupción del TIM4
		
    tim4.Instance = TIM4;
    tim4.Init.Prescaler = 83; // no prescaler (TIM4 clk 84 MHz)
    tim4.Init.Period = 65535;
		tim4.Init.CounterMode = TIM_COUNTERMODE_UP; // Cuenta hacia arriba
    HAL_TIM_IC_Init(&tim4);  // modo de comparacion con salida

    // Configuramos el Canal 1 para sacar foto en cada flanco de subida
    TIM_Channel_InitStruct.ICPolarity  = TIM_ICPOLARITY_RISING;    // Flanco de subida
    TIM_Channel_InitStruct.ICSelection = TIM_ICSELECTION_DIRECTTI; // Conexión directa al pin
    TIM_Channel_InitStruct.ICPrescaler = TIM_ICPSC_DIV1;           // Sin saltar flancos
    TIM_Channel_InitStruct.ICFilter    = 0;                        // Sin filtro
    HAL_TIM_IC_ConfigChannel(&tim4, &TIM_Channel_InitStruct, TIM_CHANNEL_1);

    
		HAL_TIM_IC_Start_IT(&tim4, TIM_CHANNEL_1);   // Arranca con interrupción (_IT) en el CANAL 1
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
		if	(htim->Instance == TIM4 && htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1){ // Comprobamos que viene de nuestro TIM4
    if (captura1 == 0x0000){
       captura1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    }else if (captura2 == 0x0000){
      captura2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
      if (captura2 < captura1){	
        diferencia = (0xFFFF - captura1) + captura2; //El FFFF es por si ha llegado al maximo y vuelve a cero, asi no da negativo
      }else{
        diferencia = captura2 - captura1;
      }
      frecuencia = 1000000 / diferencia; // Calculamos la frecuencia: Reloj del timer / diferencia

      // IMPORTANTE: Reiniciamos a 0 para que siga midiendo cuando se pulsa el botón
      captura1 = 0x0000;
      captura2 = 0x0000;
    }
  }
}




																																																																																																																																																																																																																																																																																																																																																																																																																										 