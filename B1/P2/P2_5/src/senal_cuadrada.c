#include "senal_cuadrada.h" 

// Variables globales 

static TIM_HandleTypeDef tim2;
static uint32_t p = 41999; 
static uint32_t pulso = 10500; // 25% de 42000 (42000 * 0.25 = 10500)

// Llamada de las funciones internas
static void initPIN_OUTPUT(void);
static void initTimer(void);
static void initBoton1(void);

//Iniciacion de las funciones
void SenalCuadrada_Init(void) {
    initPIN_OUTPUT();
    initTimer();
    initBoton1();
}

//Declaracion de las funciones bitin, pin, timer
static void initBoton1(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE(); // SIEMPRE PARA CD COSA OJOOO
    
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); // Habilitar la interrupción
}

static void initPIN_OUTPUT(void) { // Este fabrica la onda para el boton con el TIM2
    GPIO_InitTypeDef GPIO_InitStruct = {0};
        
    // Enable clock to GPIO-B
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // Set GPIOB Pin
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    
    // Init GPIOB Pin
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void initTimer(void) { //Cambiamos el modo de OC por el de PWM 
    TIM_OC_InitTypeDef TIM_Channel_InitStruct = {0};
    
    // Enable clock to timer-2
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    tim2.Instance = TIM2;
    tim2.Init.Prescaler = 0; // no prescaler (TIM2 clk 84 MHz)
    tim2.Init.Period = p;    // Inicializado a 41999 al inicio
    HAL_TIM_PWM_Init(&tim2);  // modo de comparacion con salida

    /* Finally initialize Timer-2, for Output */
    TIM_Channel_InitStruct.OCMode = TIM_OCMODE_PWM1;
		TIM_Channel_InitStruct.Pulse = pulso; //Añadimos esto que es el ciclo de trabajo pedido
    TIM_Channel_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
    TIM_Channel_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&tim2, &TIM_Channel_InitStruct, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&tim2, TIM_CHANNEL_4); // Arranca el timer 2 uso el canal 4
}