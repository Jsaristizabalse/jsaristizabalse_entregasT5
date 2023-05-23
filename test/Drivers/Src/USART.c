/*
 * USART.c
 *
 *  Created on: Apr 12, 2023
 *      Author: aristizabal
 */

//habilitar el USART en CR1 escribiendo el bit 1 en USART_CR1


# include "USART.h"




USART_TypeDef *ptrUsartUsed;


void Usart_Config_t (Usart_Handler_t *ptrUsartHandler){
	//escribir el bit UE en CR1 como 1
	// Guardamos una referencia al periferico que estamos utilizando...
	ptrUsartUsed = ptrUsartHandler->ptrUsartx;

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();
	/* 1. Activar la señal de reloj del periférico requerido */
	if (ptrUsartHandler->ptrUsartx == USART1){
		RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}
	else if(ptrUsartHandler->ptrUsartx == USART2){
		RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	}
	else if(ptrUsartHandler->ptrUsartx == USART6){
		RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	}
	else{
		__NOP();
	}


	// PARA TRANSMISIÓN

	// 1. Habilitamos el USART poniendo 1 en el bit UE de USART_CR1
	if (ptrUsartHandler->Usartx_Config.Usartx_mode == 1){
//t enable /diable
//r enable / disable
//t y r enable /disable
		ptrUsartHandler->ptrUsartx->CR1 &= ~USART_CR1_UE;
		ptrUsartHandler->ptrUsartx->CR1 |= USART_CR1_UE;
		/*
		 * 2. Configuramos el bit M que define el largo del WORD
		 * 0 para 8 bits
		 * 1 para 1 bits
		 */
		ptrUsartHandler->ptrUsartx->CR1 &= ~USART_CR1_M;
//		ptrUsartHandler->ptrUsartx->CR1 |= USART_CR1_M;

		/*
		 * 3. configuramos los stop bits en USART_CR2
		 * 00 para 1 stop bit
		 *
		 */
		ptrUsartHandler->ptrUsartx->CR2 &= ~USART_CR2_STOP;


		//5. Configuramos el Baud rate register
		// Establecemos el baud rate a 15200 baudios REVISAR
		ptrUsartHandler->ptrUsartx->BRR = 104;

		/*
		 * Habilitamos/deshabilitamos la transmisión
		 * 0 para disable
		 * 1 para enable
		 */

		ptrUsartHandler->ptrUsartx->CR1 &= ~USART_CR1_TE;//se pone un 0 para limpiar (tambien puede ser disable)
		ptrUsartHandler->ptrUsartx->CR1 |= USART_CR1_TE; //se pone un 1 para el ENABLE


		/*
		 * 7. Escribir los datos a enviar en el registro USART_DR eso limpia el TXE bit
		 * Hacer esto para cada dato a transmitir
		 */
		ptrUsartHandler->ptrUsartx->DR = 0b0;



	}

}

