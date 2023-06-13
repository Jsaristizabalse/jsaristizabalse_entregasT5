/**
 ******************************************************************************
 * @file           : main.c
 * @author         : aristizabal by STM32CubeIDE
 * @Nombre          : Jhony Steven Aristizabal Serna
 * @brief          : main Tarea Especial
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
#include "AdcDriver.h"

#include "arm_math.h"
#include <math.h>


#define HSI_CLOCK_CONFIGURED		0
#define HSE_CLOCK_CONFIGURED		1
#define PLL_CLOCK_CONFIGURED		2



/* Definicion de handlers */


//PINES
GPIO_Handler_t handlerBlinkyPin = {0};	//PA5
GPIO_Handler_t handlerpinEXTI            = {0};
GPIO_Handler_t handlerLEDX = {0};	//PA5
GPIO_Handler_t handlerLEDY = {0};	//PA5
GPIO_Handler_t handlerLEDZ = {0};	//PA5
GPIO_Handler_t handlerUserButton = {0};	//PA5

//ADC
ADC_Config_t AdcChannel = {0};
uint8_t adciscomplete = {0};
uint16_t adcData = {0};


//PLL
PLL_Handler_t handlerPll = {0};

//ADC


//I2C
GPIO_Handler_t handler_i2cSDA = {0};	//PB8
GPIO_Handler_t handler_i2cSCL = {0}; //PB9
I2C_Handler_t handlerAccelerometer = {0};

GPIO_Handler_t handler_i2cSDA_LCD = {0};	//PB8
GPIO_Handler_t handler_i2cSCL_LCD = {0}; //PB9
I2C_Handler_t handlerLCD = {0};

//Timers
BasicTimer_Handler_t	handlerBlinkyTimer	= {0};  //TIM2
BasicTimer_Handler_t	handlerAdcTimer	= {0};  //TIM4

//USART
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint8_t usart2DataReceived = 0;
uint8_t rxData = 0;
char bufferData[64] = "Accel ADXL345 testing...";

uint16_t counterSampling = {0};

uint8_t dataADC = {0};


//Exti

EXTI_Config_t Exti		= {0};

uint8_t counter = 0;


#define ACCEL_ADDRESS          	0x1D
#define ACCEL_XOUT_L            50     //DATAX0
#define ACCEL_XOUT_H            51     //DATAX1
#define ACCEL_YOUT_L            52     //DATAYO
#define ACCEL_YOUT_H            53     //DATAY1
#define ACCEL_ZOUT_L            54     //DATAZ0
#define ACCEL_ZOUT_H            55     //DATAZ1

#define BW_RATE					44
#define POWER_CTL               45
#define WHO_AM_I                0      //DEVID
#define OFSX					30
#define OFSY					31



// Definicion de las cabeceras de las funciones del main
void init_Hardware(void);
void accelSensor(void);
void pwmLed(void);
void i2cLCD(void);
void i2cACCEL(void);
int16_t valueAX(void);
int16_t valueAY(void);
int16_t valueAZ(void);
/*
 * Funcion principal del programa
 * Esta funcion es el corazon del programa!!
 */

int main(void){
	//Activamos el coprocesador matematico
	SCB->CPACR |= (0xF <<20);


	//Inicializamos todos los elementos del sistema
	init_Hardware();
	writeMsg(&usart2Comm, bufferData);

	config_SysTick_ms(PLL_CLOCK_CONFIGURED);




	while(1){

		if (adciscomplete ==1) {
			sprintf(bufferData, "Data Canal 1: %u",dataADC);
			writeMsg(&usart2Comm, bufferData);

			adciscomplete = 0;
		}


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

/* ==================================== Configurando los TIMERS =============================================*/
	//Configurando el TIM2 el cual le da la tasa de encendido al LED
	handlerBlinkyTimer.ptrTIMx										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//Configurando el TIM4 del muestreo
	handlerAdcTimer.ptrTIMx											= TIM4;
	handlerAdcTimer.TIMx_Config.TIMx_mode							= BTIMER_MODE_UP;
	handlerAdcTimer.TIMx_Config.TIMx_speed							= BTIMER_SPEED_1ms;
	handlerAdcTimer.TIMx_Config.TIMx_period							= 50;
	handlerAdcTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerAdcTimer);


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
//
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

/* ========================== PLL ======================================================================*/

//	handlerPll.clkSpeed = FREQUENCY_80MHz;
//	configPLL(&handlerPll);

/* ========================== ADC ======================================================================*/
	AdcChannel.channel			= ADC_CHANNEL_0;
	AdcChannel.dataAlignment	= ADC_ALIGNMENT_RIGHT;
	AdcChannel.resolution		= ADC_RESOLUTION_12_BIT;
	AdcChannel.samplingPeriod	= ADC_SAMPLING_PERIOD_84_CYCLES;

	adc_Config(&AdcChannel);





}

/* ===================== Rutinas de atencion o callbacks ===============================================*/

void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerBlinkyPin);
	counter++;
}

void BasicTimer4_Callback(void){

	startSingleADC();
}


void callback_extInt13(void){
	__NOP();
}

/*
 * Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART6
 */

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void adcComplete_Callback(void){
	dataADC = getADC();
	adciscomplete = 1;
}

