#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f4xx.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "LCDDriver.h"

BasicTimer_Handler_t 	handlerTimer2 			= {0};

GPIO_Handler_t 			handlerBlinkyLed 		= {0};
GPIO_Handler_t			handlerSCL				= {0};
GPIO_Handler_t			handlerSDA				= {0};

I2C_Handler_t 			handlerLCD				= {0};

// Definimos las variables que utilizaremos

uint8_t flagStatus 				= 0;

char dataLCD[64] 				= {0};

// Definimos las funciones que vamos a utilizar:

//Funcion de estado con el User LED

void statusLED(void);

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

// Funcion principal del programa

int main(void){

	InitSystem();

	//Inicializamos la LCD, poniendo en las dos primeras filas mensajes

	LCD_Clear(&handlerLCD);
	delay_10();
	LCD_Init(&handlerLCD);
	delay_10();
	sprintf(dataLCD, "E");
	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,dataLCD);
	sprintf(dataLCD, "H");
	LCD_setCursor(&handlerLCD,0,1);
	LCD_sendSTR(&handlerLCD,dataLCD);

	/* Ciclo infinito del main */
	while(1){
		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			statusLED();
		}
		return 0;
	}

}

void InitSystem(void){

	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerSCL.pGPIOx 											= GPIOB;
	handlerSCL.GPIO_PinConfig.GPIO_PinNumber 					= PIN_8;
	handlerSCL.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
	handlerSCL.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_OPENDRAIN;
	handlerSCL.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerSCL.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_PULLUP;
	handlerSCL.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
	GPIO_Config(&handlerSCL);

	handlerSDA.pGPIOx 											= GPIOB;
	handlerSDA.GPIO_PinConfig.GPIO_PinNumber 					= PIN_9;
	handlerSDA.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
	handlerSDA.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_OPENDRAIN;
	handlerSDA.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerSDA.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_PULLUP;
	handlerSDA.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
	GPIO_Config(&handlerSDA);

	handlerTimer2.ptrTIMx 										= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period						= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer2);

	handlerLCD.ptrI2Cx											= I2C1;
	handlerLCD.modeI2C											= I2C_MODE_SM;
	handlerLCD.slaveAddress										= LCD_ADDRESS;
	i2c_config(&handlerLCD);
}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void statusLED(void){
	GPIOxTogglePin(&handlerBlinkyLed);
}
