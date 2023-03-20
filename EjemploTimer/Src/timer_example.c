/*
 * timer_example.c
 *
 *  Created on: Mar 14, 2023
 *      Author: aristizabal
 */


#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"

//para manipular el pin se debe crear una variable tipo handler
GPIO_Handler_t handlerOnBoardLed = {0};

int main(void){
	//Establecer las configuraciones del pin A5 como salida
	handlerOnBoardLed.pGPIOx = GPIOA;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;	// configurar el PIN como salida
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;	//velocidad
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;

	GPIO_Config(&handlerOnBoardLed);

	GPIO_WritePin(&handlerOnBoardLed, SET);

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //es lo mismo que (1<<0)

	TIM2->CR1 &= ~TIM_CR1_DIR; //lo mismo que ~(0b..........) PARA PONER EN 0 EL BIT

	TIM2->PSC	= 16000; //16millones / 1000 para que cuente cada 1ms=1kHZ de frec
	TIM2->ARR	= 250;	//definimos el periodo en 250 ms
	TIM2->CR1	|= TIM_CR1_CEN;


	while(1){
		if(TIM2->SR & TIM_SR_UIF){
			GPIO_TogglePin(&handlerOnBoardLed);
			TIM2->SR &= ~TIM_SR_UIF;
		}


	}
}
