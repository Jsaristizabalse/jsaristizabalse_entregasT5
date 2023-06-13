/*
 * PllDriver.h
 *
 *  Created on: May 23, 2023
 *      Author: aristizabal
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include <stm32f4xx.h>


#define FREQUENCY_16MHz		0
#define FREQUENCY_80MHz		1
#define FREQUENCY_100MHz	2

#define HSI_CLOCK		0
#define LSE_CLOCK		1
#define PLL_CLOCK		2

#define MCO1PRE_2		2
#define MCO1PRE_3		3
#define MCO1PRE_4		4
#define MCO1PRE_5		5



typedef struct{
	uint8_t		clkSpeed;

}PLL_Handler_t;

typedef struct{
	uint8_t		clk;
	uint8_t		psc;

}MCO1_Handler_t;






void configPLL(PLL_Handler_t *ptrPLL_Handler);

uint32_t getConfigPLL(void);

void configMCO1(MCO1_Handler_t *ptrMCO1_Handler);




#endif /* PLLDRIVER_H_ */
