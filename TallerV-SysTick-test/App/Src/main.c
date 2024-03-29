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
#include "SysTickDriver.h"

#include "arm_math.h"
#include <math.h>


#define HSI_CLOCK_CONFIGURED		0
#define HSE_CLOCK_CONFIGURED		1
#define PLL_CLOCK_CONFIGURED		2



//Definicion de handlers

GPIO_Handler_t handlerBlinkyPin = {0};	//PA5

GPIO_Handler_t handlerUserButton = {0};
EXTI_Config_t handlerUserButtonExti = {0};


/* Elemento para hacer la comunicacion serial */
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};


BasicTimer_Handler_t	handlerBlinkyTimer	= {0};  //TIM2



uint8_t rxData = 0;
char bufferData[64] = "Probando el SysTick...";



uint8_t sendMsg = 0;
uint8_t counter_dummy = 0;
uint8_t usart2DataReceived = 0;


// Definicion de las cabeceras de las funciones del main
void init_Hardware(void);


int main(void){
	//Activamos el coprocesador matematico
	SCB->CPACR |= (0xF <<20);

	//Inicializamos todos los elementos del sistema
	init_Hardware();

	config_SysTick_ms(HSI_CLOCK_CONFIGURED);
	writeMsg(&usart2Comm, bufferData);

	while(1){

		if (sendMsg > 4) {
			writeMsg(&usart2Comm, "\nHola Mundo!!!\n");
			delay_ms(3000);
			writeMsg(&usart2Comm, "Primero 300 ms\n");
			delay_ms(3000);
			GPIOxTogglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTogglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTogglePin(&handlerBlinkyPin);
			delay_ms(300);
			GPIOxTogglePin(&handlerBlinkyPin);

			sprintf(bufferData, "Cambiamos a 1000ms!!!!\n");
			writeMsg(&usart2Comm, bufferData);
			delay_ms(3000);

			GPIOxTogglePin(&handlerBlinkyPin);
			delay_ms(1000);
			GPIOxTogglePin(&handlerBlinkyPin);
			delay_ms(1000);
			GPIOxTogglePin(&handlerBlinkyPin);
			delay_ms(1000);

		}

		}
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

	handlerUserButton.pGPIOx										= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber					= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

/* ==================================== Configurando los TIMERS =============================================*/
	//Configurando el TIM2 el cual le da la tasa de encendido al LED
	handlerBlinkyTimer.ptrTIMx										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_80MHz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);
/* ==================================== Configurando los EXTI =============================================*/
	handlerUserButtonExti.pGPIOHandler								= &handlerUserButton;
	handlerUserButtonExti.edgeType									= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);
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

/* ========================== PLL ======================================================================*/

//	handlerPll.clkSpeed = FREQUENCY_80MHz;
//	configPLL(&handlerPll);

	/* ========================== I2C ======================================================================*/
}

/* ===================== Rutinas de atencion o callbacks ===============================================*/

void BasicTimer2_Callback(void){
//	GPIOxTogglePin(&handlerBlinkyPin);
	sendMsg++;
	counter_dummy++;
}

void callback_extInt13(void){
	__NOP();
}

/*
 * Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2
 */

void usart6Rx_Callback(void){
	rxData = getRxData();

}


