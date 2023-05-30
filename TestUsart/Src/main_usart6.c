/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
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


//Definicion de handlers
GPIO_Handler_t handlerBlinkyPin = {0};

GPIO_Handler_t handlerUserButton = {0};
EXTI_Config_t handlerUserButtonExti = {0};

BasicTimer_Handler_t	handlerBlinkyTimer	= {0};  //Timer del led BLinky

/* Elemento para hacer la comunicacion serial */
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t Usart2Comm = {0};
uint8_t sendMsg = 0;


char mensaje[] = "\nPrueba de sonido!\n";
char bufferMsg[64] = {0};

// Definicion de las cabeceras de las funciones del main
void init_Hardware(void);
/*
 * Funcion principal del programa
 * Esta funcion es el corazon del programa!!
 */

int main(void){
	//Inicializamos todos los elementos del sistema
	init_Hardware();

	while(1){
		if(sendMsg >4){
		writeChar(&Usart2Comm, 'G');

		writeMsg(&Usart2Comm, mensaje);

		sprintf(bufferMsg, "Valor de sendMsg = %d \n",sendMsg);

		writeMsg(&Usart2Comm, bufferMsg);

		sendMsg = 0;
		}

	}	//Fin del ciclo princial
	return 0;
}		//Fin del Main



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

	handlerUserButton.pGPIOx										= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber					= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

/* ==================================== Configurando los TIMERS =============================================*/
	//Configurando el TIM2 el cual le da la tasa de encendido al LED
	handlerBlinkyTimer.ptrTIMx										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);
/* ==================================== Configurando los EXTI =============================================*/
	handlerUserButtonExti.pGPIOHandler								= &handlerUserButton;
	handlerUserButtonExti.edgeType									= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);
/* ==================================== Configurando los USART =============================================*/
	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTX);
	//PINRX del usart para Usart6(PA12 AF8) Para Usart2(PA3 AF7)
	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);
	//cuando el micro este a 80MHz deben usarse los baudrate terminados en _80MHz
	Usart2Comm.ptrUSARTx = USART1;
	Usart2Comm.USART_Config.USART_baudrate = USART_BAUDRATE_80MHz_115200;
	Usart2Comm.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	Usart2Comm.USART_Config.USART_mode = USART_MODE_RXTX;
	Usart2Comm.USART_Config.USART_parity = USART_PARITY_NONE;
	Usart2Comm.USART_Config.USART_stopbits = USART_STOPBIT_1;
	Usart2Comm.USART_Config.USART_enableIntTX = USART_TX_INTERRUPT_DISABLE;
	Usart2Comm.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&Usart2Comm);
}

/* ===================== Rutinas de atencion o callbacks ===============================================*/

void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerBlinkyPin);
	sendMsg++;
}

void callback_extInt13(void){
	__NOP();
}
