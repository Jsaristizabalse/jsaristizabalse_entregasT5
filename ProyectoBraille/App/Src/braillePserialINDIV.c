/**
 ******************************************************************************
 * @file           : main.c
 * @author         : aristizabal by STM32CubeIDE
 * @Nombre          : Jhony Steven Aristizabal Serna
 * @brief          : main Proyecto Braille
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PllDriver.h"
#include "I2CDriver.h"
#include "PwmDriver.h"
#include "LCDDriver.h"

#include "arm_math.h"
#include <math.h>

#include "string.h"
#include "ctype.h"

#define ROWS 3
#define COLS 2
#define HSI_CLOCK_CONFIGURED		0
#define HSE_CLOCK_CONFIGURED		1
#define PLL_CLOCK_CONFIGURED		2

/* Definicion de handlers */



//GPIO
GPIO_Handler_t handlerBlinkyPin		= {0};	//PA5
GPIO_Handler_t handlerLED1			= {0};
GPIO_Handler_t handlerLED2			= {0};
GPIO_Handler_t handlerLED3			= {0};
GPIO_Handler_t handlerLED4			= {0};
GPIO_Handler_t handlerLED5			= {0};
GPIO_Handler_t handlerLED6			= {0};
GPIO_Handler_t GPIO_StructHandlers[ROWS][COLS];

//TIMERS
BasicTimer_Handler_t handlerBlinkyTimer	= {0};  //TIM2
BasicTimer_Handler_t handlerBrailleTimer	= {0};  //TIM3


//USART
GPIO_Handler_t handlerPinTX		= {0};
GPIO_Handler_t handlerPinRX		= {0};
USART_Handler_t usart2Comm		= {0};

char bufferReception[64] = {0};
char cmd[64] = {0};
char bufferMsg[64] = {0};

char bufferData[64]				= {0};


float firstParameter = 0;
unsigned int secondParameter = 0;
unsigned int thirdParameter = 0;
uint8_t counterReception = 0;

bool stringComplete = false;


int state[ROWS][COLS] = {
		{0,0},
		{0,0},
		{0,0}
};

    // Pines
int ledPins[ROWS][COLS] = {
		{PIN_10,PIN_15},
		{PIN_11,PIN_14},
		{PIN_12,PIN_13}
};

uint8_t flagDelayTimer = RESET;
uint8_t flagBraille = RESET;
uint8_t auxBraille = 0;
char rxData = 0;
uint32_t counter = 0;		//Contador para el timer de la matriz de braile

uint8_t buffer[256]; // Definir un búfer para almacenar los datos recibidos

// Definicion de las cabeceras de las funciones del main
void init_Hardware(void);			//Inicializar el hardware
void displayNum(uint8_t num);		//Mostramos el numero en el display
void clearLEDMatrix(void);			//Limpiar la pantalla de LEDS
void updateLEDMatrix(int state[][COLS]);	//Actualizar la matriz
void alfabetoBraille(char letra);			//Diccionario
void traducirBraille(char str);
void parseCommands(char  *ptrbufferReception);





char str[] = "a";


int main(void){
	//Activamos el coprocesador matematico
	SCB->CPACR |= (0xF <<20);
	//Inicializamos todos los elementos del sistema
	init_Hardware();
	sprintf(bufferData, "\nProyecto:\nTraduccion de texto a braille\n");



//	int len = strlen(str);
	writeMsg(&usart2Comm, bufferData);

	while(1){
		alfabetoBraille(rxData);
//		traducirBraille(str);
	}//Fin del While
	return(0);
}//Fin del Main



//Funcion que configura el hardware, timers y extis
void init_Hardware(void){

	//Configuracion del BLinky
	handlerBlinkyPin.pGPIOx											= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);


	for (int row = 0; row < ROWS ;row++){
		for (int col = 0; col < COLS ;col++){
			GPIO_StructHandlers[row][col].pGPIOx 								= (col == 0) ? GPIOC : GPIOB;
	        GPIO_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinNumber			= ledPins[row][col];
	        GPIO_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	        GPIO_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinSpeed       	= GPIO_OSPEED_FAST;
	        GPIO_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	        GPIO_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinOPType      	= GPIO_OTYPE_PUSHPULL;
	        GPIO_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinAltFunMode  	= AF0;
		}
	};

	//Cargamos la configuracion de los LEDS
	for (int row = 0; row < ROWS ;row++){
		for (int col = 0; col < COLS ;col++){
			GPIO_Config(&GPIO_StructHandlers[row][col]);
		}
	};

/* ==================================== Configurando los TIMERS =============================================*/
	//Configurando el TIM2 que controla el Blinky
	handlerBlinkyTimer.ptrTIMx										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);


	//Configurando el TIM3 que controla el refresco de la matriz de leds
	handlerBrailleTimer.ptrTIMx										= TIM3;
	handlerBrailleTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBrailleTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_1ms;
	handlerBrailleTimer.TIMx_Config.TIMx_period						= 1000;
	handlerBrailleTimer.TIMx_Config.TIMx_interruptEnable			= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBrailleTimer);

/* ==================================== Configurando los EXTI =============================================*/
	/*Configuracion PIN para el exti (debe ser un pin como entrada)*/
//	handlerpinEXTI.pGPIOx = GPIOB;
//	handlerpinEXTI.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
//	handlerpinEXTI.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
//	handlerpinEXTI.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
//	handlerpinEXTI.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
//	handlerpinEXTI.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//
//	GPIO_Config(&handlerpinEXTI);

//	Exti.edgeType                                           = EXTERNAL_INTERRUPT_FALLING_EDGE;
//	Exti.pGPIOHandler                                       = &handlerpinEXTI;
//	extInt_Config(&Exti);

/* ==================================== Configurando los USART =============================================*/
	handlerPinTX.pGPIOx												= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber						= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode					= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx												= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber						= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode					= AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx											= USART2;
	usart2Comm.USART_Config.USART_baudrate							= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize							= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity							= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits							= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode								= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX						= USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_enableIntTX						=USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart2Comm);



	config_SysTick_ms(HSI_CLOCK_CONFIGURED);

}

/* ===================== Rutinas de atencion o callbacks ===============================================*/

void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerBlinkyPin);
}

void BasicTimer3_Callback(void){
	flagBraille = SET;

	if (flagDelayTimer) {

		flagDelayTimer = RESET;

	}

	counter++;

}





void updateLEDMatrix(int state[][COLS]){
//	clearLEDMatrix();
//	delay_ms(1000);


	for(int i = 0; i < ROWS;i++){
		for(int j = 0; j<COLS;j++){
			GPIO_WritePin(&GPIO_StructHandlers[i][j], state[i][j]);
		}
	}
}

void clearLEDMatrix(void){
	int state[ROWS][COLS] = {
			{0,0},
			{0,0},
			{0,0}
	};
	for(int i = 0; i < ROWS;i++){
		for(int j = 0; j<COLS;j++){
			GPIO_WritePin(&GPIO_StructHandlers[i][j], state[i][j]);
		}
	}

}

void traducirBraille(char str){

	if (flagBraille== SET){
		alfabetoBraille(str);
		auxBraille++;
		flagBraille = RESET;
	}

//	if (str[auxBraille] == '\0') {
//		auxBraille = 0;
//	}




}


void alfabetoBraille(char letra){

	if (isupper(letra)){
		int state[ROWS][COLS] = {
				{0,1},
				{0,0},
				{0,1}
		};
		updateLEDMatrix(state);
		delay_ms(1000);
	}

	if(isdigit(letra)){
		int state[ROWS][COLS] = {
				{0,1},
				{0,1},
				{1,1}
		};
		updateLEDMatrix(state);
		delay_ms(1000);
	}

	switch (tolower(letra)) {
		case 'a':
		case '1': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 0, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case 'b':
		case '2': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 1, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 'c':
		case '3': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 0, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 'd':
		case '4': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 0, 1 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 'e':
		case '5': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 0, 1 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 'f':
		case '6': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 1, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'g':
		case '7': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 1, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'h':
		case '8': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 1, 1 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'i':
		case '9': {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 1, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'j':
		case '0': {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 1, 1 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'k': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 0, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'l': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 1, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'm': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 0, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'n': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 0, 1 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'o': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 0, 1 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'p': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 1, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'q': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 1, 1 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'r': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 1, 1 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 's': {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 1, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 't': {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 1, 1 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case 'u': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 0, 0 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 'v': {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 0, 0 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'w': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 1, 1 },
					{ 0, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'x': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 0, 0 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'y': {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 0, 1 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case 'z': {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 0, 1 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}
		case ' ': {
			int new_state[ROWS][COLS] = {
					{ 0, 0 },
					{ 0, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 160: {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 1, 0 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 130: {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 1, 0 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case 161: {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 0, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 162: {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 0, 0 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case 163: {
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 1, 1 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case 129: {
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 0, 1 },
					{ 0, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case 164: {
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 1, 1 },
					{ 0, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}




		case '.': {
			int new_state[ROWS][COLS] = {
					{ 0, 0 },
					{ 0, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case ',': {
			int new_state[ROWS][COLS] = {
					{ 0, 0 },
					{ 1, 0 },
					{ 0, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case 168:
		case 63:
		{
			int new_state[ROWS][COLS] = {
					{ 1, 1 },
					{ 1, 0 },
					{ 1, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case 173:
		case 33:
		{
			int new_state[ROWS][COLS] = {
					{ 0, 0 },
					{ 1, 1 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case ';':
		{
			int new_state[ROWS][COLS] = {
					{ 0, 0 },
					{ 1, 0 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}


		case '(':
		{
			int new_state[ROWS][COLS] = {
					{ 1, 0 },
					{ 1, 0 },
					{ 0, 1 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		case ')':
		{
			int new_state[ROWS][COLS] = {
					{ 0, 1 },
					{ 0, 1 },
					{ 1, 0 }
			};
			memcpy(state, new_state, sizeof(state));
			break;
		}

		default:
		break;
	}
	updateLEDMatrix( state);
}



void parseCommands(char  *ptrbufferReception){
	sscanf(ptrbufferReception,"%s %f %u %u",cmd,&firstParameter,&secondParameter,&thirdParameter);



	//Comando para solicitar ayuda
	if(strcmp(cmd, "help") == 0){
		writeMsg(&usart2Comm, "Help Menu CMDS: \n");

	}
	else if (strcmp(cmd, "Reset") == 0) {
		auxBraille=0;
		flagBraille = RESET;
		clearLEDMatrix();
		writeMsg(&usart2Comm, "Reinicio del sistema \n");
		delay_ms(5000);
	}

	else if(strcmp(cmd, "Timer") == 0) {


			if (firstParameter > 0) {

				handlerBrailleTimer.TIMx_Config.TIMx_period						= firstParameter*1000;
				BasicTimer_Config(&handlerBrailleTimer);


				sprintf(bufferMsg,"Se configura el tiempo a %.2f s",firstParameter);
				writeMsg(&usart2Comm, bufferMsg);
			}
	}


	else{
		writeMsg(&usart2Comm, "Comando erroneo.\n Ingresa \"help @\" para ver la lista de comandos.\n");
	}
}


void usart2Rx_Callback(void){
	rxData = getRxData();


	sprintf(bufferMsg,"Recibido %c \n",rxData);
	writeMsg(&usart2Comm, bufferMsg);
}


