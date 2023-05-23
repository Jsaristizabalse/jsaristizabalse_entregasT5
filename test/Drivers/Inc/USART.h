/*
 * USART.h
 *
 *  Created on: Apr 12, 2023
 *      Author: aristizabal
 */

#ifndef INC_USART_H_
#define INC_USART_H_
#include "stm32f4xx.h"
#include "GPIOxDriver.h"



#define USART_BAUDRATE_115200	115200
#define USART_BAUDRATE_57600	57600
#define USART_BAUDRATE_9600		9600

#define USART_TR_ENABLE		1
#define USART_TR_DISABLE	0

typedef struct
{
	uint8_t		Usartx_baudrate; 	// Up or dowm
	uint32_t	Usartx_stop;		//
	uint8_t		Usartx_mode;		//entrada o salida
	uint8_t		Usartx_parity;		//paridad del usart
	uint8_t		Usartx_w_lenght;	//

}Usart_Config_t;

typedef struct
{
	USART_TypeDef	*ptrUsartx;
	Usart_Config_t	Usartx_Config;
}Usart_Handler_t;


void Usart_Config(Usart_Handler_t *ptrUsartHandler);


#endif /* INC_USART_H_ */
