/*
 * main.c
 *
 *  Created on: May 31, 2023
 *      Author: aristizabal
 */

#include <stm32f4xx.h>

#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"
#include "PllDriver.h"
#include "PwmDriver.h"
#include "LCDDriver.h"

#include "arm_math.h"
#include <math.h>

#include "string.h"
#include "ctype.h"

#define ROWS		3
#define COLS		2
#define HSI_CLOCK_CONFIGURED		0
#define HSE_CLOCK_CONFIGURED		1
#define PLL_CLOCK_CONFIGURED		2

/*
 * Definicion de los handlers
 */

// GPIO


GPIO_Handler_t handlerBlinkyPin			= {0};
GPIO_Handler_t gpio_StructHandlers[ROWS][COLS]			= {0};

//TIMERS
BasicTimer_Handler_t handlerBlinkyTimer					= {0};
BasicTimer_Handler_t handlerBrailleTimer				= {0};

//USART
GPIO_Handler_t handlerPinTX								= {0};
GPIO_Handler_t handlerPinRX								= {0};
USART_Handler_t usart2Comm								= {0};

char bufferData[64]										={0};

int state[ROWS][COLS] = {
		{0,0},
		{0,0},
		{0,0}
};

//PINES

int ledPins[ROWS][COLS] = {
		{PIN_10,PIN_6},
		{PIN_11,PIN_7},
		{PIN_12,PIN_8}
};

uint8_t flagBraille			= RESET;
uint8_t auxBraille			= 0;
uint8_t rxData				= 0;
uint32_t counter			= 0;

//Definicion de las cabecera de las funciones del main

void init_Hardware(void);
void clearLEDMatrix(void);
void updateLEDMatrix(int state[ROWS][COLS]);
void alfabetoBraille(char letra);


int main(void){
	init_Hardware();
	sprintf(bufferData,"\nTraduccion de texto a braille\n");
	writeMsg(&usart2Comm, bufferData);

	char str[] = "Hola mundo";
	int len =strlen(str);


	while(1){
//		if (flagBraille == SET) {
//			alfabetoBraille(str[auxBraille]);
//			flagBraille = RESET;
//			auxBraille++;
//		}

	}//Fin del while
	return 0;
}//Fin del main

void init_Hardware(void){
	//Configuracion del BLinky
	handlerBlinkyPin.pGPIOx											= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);


	for(int row = 0; row < ROWS ; row++){
		for(int col = 0; col < COLS ; col++){
			gpio_StructHandlers[row][col].pGPIOx 								= (col == 0) ? GPIOC : GPIOA;
			gpio_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinNumber			= ledPins[row][col];
			gpio_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
			gpio_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
			gpio_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
			gpio_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
			gpio_StructHandlers[row][col].GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
		}
	};


	//Cargamos la cofiguracion de los leds

	for(int row ; row < ROWS ; row++){
		for(int col ; col < COLS ; col++){
			GPIO_Config(&gpio_StructHandlers[row][col]);

		}
	}

	//Configurando los timers


	//TIMER2
	handlerBlinkyTimer.ptrTIMx										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//TIMER3

	handlerBrailleTimer.ptrTIMx										= TIM3;
	handlerBrailleTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBrailleTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_1ms;
	handlerBrailleTimer.TIMx_Config.TIMx_period						= 1000;
	handlerBrailleTimer.TIMx_Config.TIMx_interruptEnable			= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBrailleTimer);


	//USART

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
	usart2Comm.USART_Config.USART_enableIntTX						= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart2Comm);



	config_SysTick_ms(HSI_CLOCK_CONFIGURED);




}

void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerBlinkyPin);
}

void BasicTimer3_Callback(void){
	flagBraille = SET;
	counter++;
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void clearLEDMatrix(void){
	int state[ROWS][COLS] = {
			{0,0},
			{0,0},
			{0,0}
	};
	updateLEDMatrix(state);
}

void updateLEDMatrix(int state[ROWS][COLS]){
	for(int i = 0; i < ROWS;i++){
		for(int j = 0; j<COLS;j++){
			GPIO_WritePin(&gpio_StructHandlers[i][j], state[i][j]);
		}
	}
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
		case '1':
		{
			int new_state[ROWS][COLS] = {
					{1,0},
					{0,0},
					{0,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'b':
		{
			int new_state[ROWS][COLS] = {
				{1,0},
				{1,0},
				{0,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'h':
		{
			int new_state[ROWS][COLS] = {
					{1,0},
					{1,1},
					{0,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'o':
		{
			int new_state[ROWS][COLS] = {
					{1,0},
					{0,0},
					{0,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'l':
		{
			int new_state[ROWS][COLS] = {
					{1,0},
					{1,0},
					{1,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'm':
		{
			int new_state[ROWS][COLS] = {
					{1,1},
					{0,0},
					{1,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'u':
		{
			int new_state[ROWS][COLS] = {
					{1,0},
					{0,0},
					{1,1}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'n':
		{
			int new_state[ROWS][COLS] = {
					{1,1},
					{0,1},
					{1,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case 'd':
		{
			int new_state[ROWS][COLS] = {
					{1,1},
					{0,1},
					{0,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
		case ' ':
		{
			int new_state[ROWS][COLS] = {
					{0,0},
					{0,0},
					{0,0}
			};
			memcpy(state,new_state,sizeof(state));
			break;
		}
	default:
		break;
	}
	updateLEDMatrix(state);
}
















