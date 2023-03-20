/*
 * main.c
 *
 *  Created on: Mar 16, 2023
 *      Author: aristizabal
 */


#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

int main(void)
{
	uint16_t test_var= 1;
	uint16_t test_var2= 0;


    /* Definimos el handler para el PIN que deseamos configurar */
	GPIO_Handler_t handlerUserLedPin = {0};
	//deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

//	Cargamos la configuracion del pin especifico
	GPIO_Config(&handlerUserLedPin);
	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);
	test_var = GPIOxTogglePin(&test_var);
	test_var2 = GPIOxTogglePin(&test_var2);


	//Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){
		NOP();
	}
}
