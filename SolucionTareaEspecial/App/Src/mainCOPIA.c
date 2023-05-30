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


//PLL
PLL_Handler_t handlerPll = {0};

// PWM
PWM_Handler_t handlerPwmLEDX;
PWM_Handler_t handlerPwmLEDY;
PWM_Handler_t handlerPwmLEDZ;
uint16_t duttyX = {0};
uint16_t duttyY = {0};
uint16_t duttyZ = {0};

//I2C
GPIO_Handler_t handler_i2cSDA = {0};	//PB8
GPIO_Handler_t handler_i2cSCL = {0}; //PB9
I2C_Handler_t handlerAccelerometer = {0};

GPIO_Handler_t handler_i2cSDA_LCD = {0};	//PB8
GPIO_Handler_t handler_i2cSCL_LCD = {0}; //PB9
I2C_Handler_t handlerLCD = {0};

//Timers
BasicTimer_Handler_t	handlerBlinkyTimer	= {0};  //TIM2
BasicTimer_Handler_t	handlerPwmTimer	= {0};  //TIM4
BasicTimer_Handler_t	handlerLcdTimer	= {0};  //TIM3

//USART
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart6Comm = {0};
uint8_t usart2DataReceived = 0;
uint8_t rxData = 0;
char bufferData[64] = "Accel ADXL345 testing...";
uint8_t i2cBuffer = {0};

char BufferX[64] = {0};
char BufferY[64] = {0};
char BufferZ[64] = {0};
char BufferBW[64] = {0};
char BufferoOFFX[64] = {0};
char BufferoOFFY[64] = {0};

uint16_t counterSampling = {0};
uint8_t samplingFlag = {0};
uint8_t lcdFlag = {0};
float arrayX[2000] = {0};
float arrayY[2000] = {0};
float arrayZ[2000] = {0};

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
	writeMsg(&usart6Comm, bufferData);
	LCD_Init(&handlerLCD);
	config_SysTick_ms(PLL_CLOCK_CONFIGURED);

	delay_ms(10);

	LCD_setCursor(&handlerLCD, 0, 0);
	LCD_sendSTR(&handlerLCD, "Taller V 2.0");
	LCD_setCursor(&handlerLCD, 0, 1);
	LCD_sendSTR(&handlerLCD, "Tarea Especial");
	LCD_setCursor(&handlerLCD, 0, 2);
	LCD_sendSTR(&handlerLCD, "Jhony Aristizabal");
	LCD_setCursor(&handlerLCD, 0, 3);
	LCD_sendSTR(&handlerLCD, ":)");
	delay_ms(4000);
	LCD_Clear(&handlerLCD);

	delay_ms(10);

	LCD_setCursor(&handlerLCD, 0, 0);
	LCD_sendSTR(&handlerLCD, "aX =");
	LCD_setCursor(&handlerLCD, 0, 1);
	LCD_sendSTR(&handlerLCD, "aY = ");
	LCD_setCursor(&handlerLCD, 0, 2);
	LCD_sendSTR(&handlerLCD, "aZ = ");




	uint8_t rate = i2c_readSingleRegister(&handlerAccelerometer, BW_RATE);
	uint8_t ofX = i2c_readSingleRegister(&handlerAccelerometer, OFSX);
	uint8_t ofY = i2c_readSingleRegister(&handlerAccelerometer, OFSY);

	sprintf(BufferBW, "RT=%d OFX=%d OFY=%d ", rate,ofX,ofY);
	LCD_setCursor(&handlerLCD, 0, 3);
	LCD_sendSTR(&handlerLCD, BufferBW);

//	LCD_setCursor(&handlerLCD, 5, 3);
//	LCD_sendSTR(&handlerLCD, "aZ2 = ");





//	i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, Latent);
//	sprintf(bufferConf, " %x ", (unsigned int) i2cBuffer);
//	moveCursor_inLCD(&handlerLCD, 3, 10);
//	sendSTR_toLCD(&handlerLCD, bufferConf);

	while(1){
		pwmLed();
		accelSensor();
		//Con la bandera garantizamos que se actualice el valor en pantalla cada vez que el timer
		//complete su ciclo
		if (lcdFlag == 1) {
			sprintf(BufferX, "%.2f m/s2  ", ((float)valueAX() / 220.f) * 9.78);
			LCD_setCursor(&handlerLCD, 5, 0);
			LCD_sendSTR(&handlerLCD, BufferX);

			sprintf(BufferY, "%.2f m/s2  ", ((float)valueAY() / 220.f) * 9.78);
			LCD_setCursor(&handlerLCD, 5, 1);
			LCD_sendSTR(&handlerLCD, BufferY);

			sprintf(BufferZ, "%.2f m/s2  ", ((float)valueAZ() / 220.f) * 9.78);
			LCD_setCursor(&handlerLCD, 5, 2);
			LCD_sendSTR(&handlerLCD, BufferZ);

			lcdFlag = 0;
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
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_80MHz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//Configurando el TIM4 del muestreo
	handlerPwmTimer.ptrTIMx											= TIM4;
	handlerPwmTimer.TIMx_Config.TIMx_mode							= BTIMER_MODE_UP;
	handlerPwmTimer.TIMx_Config.TIMx_speed							= BTIMER_SPEED_80MHz_100us;
	handlerPwmTimer.TIMx_Config.TIMx_period							= 10;
	handlerPwmTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerPwmTimer);

	//Configurando el TIM4 del muestreo
	handlerLcdTimer.ptrTIMx											= TIM5;
	handlerLcdTimer.TIMx_Config.TIMx_mode							= BTIMER_MODE_UP;
	handlerLcdTimer.TIMx_Config.TIMx_speed							= BTIMER_SPEED_80MHz_100us;
	handlerLcdTimer.TIMx_Config.TIMx_period							= 10000;
	handlerLcdTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerLcdTimer);
/* ==================================== Configurando los EXTI =============================================*/
	/*Configuracion PIN para el exti (debe ser un pin como entrada)*/
	handlerpinEXTI.pGPIOx = GPIOB;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerpinEXTI);

	Exti.edgeType                                           = EXTERNAL_INTERRUPT_FALLING_EDGE;
	Exti.pGPIOHandler                                       = &handlerpinEXTI;
	extInt_Config(&Exti);

/* ==================================== Configurando los USART =============================================*/
	handlerPinTX.pGPIOx												= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber						= PIN_11;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode					= AF8;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx												= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber						= PIN_12;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode					= AF8;
	GPIO_Config(&handlerPinRX);

	usart6Comm.ptrUSARTx											= USART6;
	usart6Comm.USART_Config.USART_baudrate							= USART_BAUDRATE_80MHz_115200;
	usart6Comm.USART_Config.USART_datasize							= USART_DATASIZE_8BIT;
	usart6Comm.USART_Config.USART_parity							= USART_PARITY_NONE;
	usart6Comm.USART_Config.USART_stopbits							= USART_STOPBIT_1;
	usart6Comm.USART_Config.USART_mode								= USART_MODE_RXTX;
	usart6Comm.USART_Config.USART_enableIntRX						= USART_RX_INTERRUPT_ENABLE;
	usart6Comm.USART_Config.USART_enableIntTX						=USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart6Comm);

/* ========================== PLL ======================================================================*/

	handlerPll.clkSpeed = FREQUENCY_80MHz;
	configPLL(&handlerPll);

/* ========================== I2C ======================================================================*/
	i2cACCEL();
	i2cLCD();




//-----------------------------------------PWM

	handlerLEDX.pGPIOx = GPIOB;
	handlerLEDX.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	handlerLEDX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerLEDX.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLEDX.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&handlerLEDX);
	//Configuracion PWMX
	handlerPwmLEDX.ptrTIMx = TIM3;
	handlerPwmLEDX.config.channel = PWM_CHANNEL_1;
	handlerPwmLEDX.config.duttyCicle = duttyX;
	handlerPwmLEDX.config.periodo = 20000;
	handlerPwmLEDX.config.prescaler = 80;
	pwm_Config(&handlerPwmLEDX);
	enableOutput(&handlerPwmLEDX);
	startPwmSignal(&handlerPwmLEDX);

	/*Configuracion PIN para el PWMY (debe ser un pin como Funcion)*/
	handlerLEDY.pGPIOx = GPIOB;
	handlerLEDY.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLEDY.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerLEDY.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDY.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDY.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLEDY.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&handlerLEDY);
	//Configuracion PWMY
	handlerPwmLEDY.ptrTIMx = TIM3;
	handlerPwmLEDY.config.channel = PWM_CHANNEL_2;
	handlerPwmLEDY.config.duttyCicle = duttyY;
	handlerPwmLEDY.config.periodo = 20000;
	handlerPwmLEDY.config.prescaler = 80;
	pwm_Config(&handlerPwmLEDY);
	enableOutput(&handlerPwmLEDY);
	startPwmSignal(&handlerPwmLEDY);

	/*Configuracion PIN para el PWMZ (debe ser un pin como Funcion)*/
	handlerLEDZ.pGPIOx = GPIOB;
	handlerLEDZ.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	handlerLEDZ.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerLEDZ.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDZ.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDZ.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLEDZ.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&handlerLEDZ);

	//Configuracion PWMZ

	handlerPwmLEDZ.ptrTIMx = TIM3;
	handlerPwmLEDZ.config.channel = PWM_CHANNEL_3;
	handlerPwmLEDZ.config.duttyCicle = duttyZ;
	handlerPwmLEDZ.config.periodo = 20000;
	handlerPwmLEDZ.config.prescaler = 80;

	// Activando señal
	pwm_Config(&handlerPwmLEDZ);
	enableOutput(&handlerPwmLEDZ);
	startPwmSignal(&handlerPwmLEDZ);

}

/* ===================== Rutinas de atencion o callbacks ===============================================*/

void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerBlinkyPin);
	counter++;
}

void BasicTimer4_Callback(void){
	if (samplingFlag == 1) {
		counterSampling++;
	}
}

void BasicTimer5_Callback(void){
	lcdFlag = 1;
}

void callback_extInt13(void){
	__NOP();
}

/*
 * Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART6
 */

void usart6Rx_Callback(void){
	rxData = getRxData();
}

void accelSensor(void){
	if (rxData != '\0') {
		writeChar(&usart6Comm, rxData);

		if (rxData == 'w') {
			sprintf(bufferData, "WHO_AM_I? (r)\n");
			writeMsg(&usart6Comm, bufferData);

			i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
			sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
			writeMsg(&usart6Comm, bufferData);
			rxData = '\0';
		}
		else if (rxData == 'p') {
			sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
			writeMsg(&usart6Comm, bufferData);

			i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer,
					POWER_CTL);
			sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
			writeMsg(&usart6Comm, bufferData);
			rxData = '\0';
		}
		else if (rxData == 'r') {
			sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
			writeMsg(&usart6Comm, bufferData);

			i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x2D);
			rxData = '\0';
		}
		else if (rxData == 'x') {
			sprintf(bufferData, "Axis X data (r) \n");
			writeMsg(&usart6Comm, bufferData);


			sprintf(bufferData, "AccelX = %.2f \n", ((float)valueAX() / 220.f) * 9.78);
			writeMsg(&usart6Comm, bufferData);
			rxData = '\0';
		}
		else if (rxData == 'y') {
			sprintf(bufferData, "Axis Y data (r)\n");
			writeMsg(&usart6Comm, bufferData);

			sprintf(bufferData, "AccelY = %.2f \n", ((float)valueAY() / 220.f) * 9.78);
			writeMsg(&usart6Comm, bufferData);
			rxData = '\0';
		}
		else if (rxData == 'z') {
			sprintf(bufferData, "Axis Z data (r)\n");
			writeMsg(&usart6Comm, bufferData);


			sprintf(bufferData, "AccelZ = %.2f \n", ((float)valueAZ() / 200.f) * 9.78);
			writeMsg(&usart6Comm, bufferData);
			rxData = '\0';
		}
		if (rxData == 'd') {
			samplingFlag = 1;
			counterSampling = 0;

			while (counterSampling < 2000) {
				//Por cada "Tick" del timer la bandera se levanta una vez y almacena datos en los arreglos

				arrayX[counterSampling] = ((float)valueAX() / 256.f) * 9.78;
				arrayY[counterSampling] = ((float)valueAY() / 256.f) * 9.78;
				arrayZ[counterSampling] = ((float)valueAZ() / 256.f) * 9.78;
			}
			samplingFlag = 0;
			counterSampling = 0;

			for (int i = 0; i < 2000; i++) {
				//Mostramos 2000 almacenados anteriormente en los arreglos
				//Estos se imprimen en pantalla linea por linea
				//En un formato de tabla, con el numero(i), X, Y Z
				sprintf(bufferData, "%u X = %.2f ; Y = %.2f ; Z = %.2f \n", i,
						arrayX[i], arrayY[i], arrayZ[i]);
				writeMsg(&usart6Comm, bufferData);
			}
			rxData = '\0';
		}
		else {
			rxData = '\0';
		}

	}

}


void pwmLed(void){

	//Creamos una funcion que convierta los valores de las aceleraciones
	//En funciones lineales para verlo con leds o con osciloscopio

//	duttyX = 2000*powf(M_E,((float)valueAX()/220.f)*9.78)-1000;
//	updateDuttyCycle(&handlerPwmLEDX, duttyX);

	duttyX = 2000 *(((float) valueAX() / 220.f) * 9.78) + 1000;
	updateDuttyCycle(&handlerPwmLEDX, duttyX);


//	duttyY = 2000*powf(M_E,((float)valueAY()/220.f)*9.78)-1000;
//	updateDuttyCycle(&handlerPwmLEDY, duttyY);

	duttyY = 2000 *(((float) valueAY() / 220.f) * 9.78) + 1000;
	updateDuttyCycle(&handlerPwmLEDY, duttyY);


//	duttyZ = 2000*powf(M_E,((float)valueAZ()/220.f)*9.78)-1000;
//	updateDuttyCycle(&handlerPwmLEDZ, duttyZ);
	duttyZ = 2000 *(((float) valueAZ() / 220.f) * 9.78) + 1000;
	updateDuttyCycle(&handlerPwmLEDZ, duttyZ);
}


int16_t valueAX(void){
    uint8_t AccelX_low =  i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
    uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
    int16_t AccelX = AccelX_high << 8 | AccelX_low;
    return AccelX;
}

int16_t valueAY(void){
	uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer,ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;
	return AccelY;
}

int16_t valueAZ(void){
	uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
	return AccelZ;
}


void i2cLCD(void){
	handler_i2cSCL_LCD.pGPIOx												= GPIOB;
	handler_i2cSCL_LCD.GPIO_PinConfig.GPIO_PinNumber						= PIN_10;
	handler_i2cSCL_LCD.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	handler_i2cSCL_LCD.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPE_OPENDRAIN;
	handler_i2cSCL_LCD.GPIO_PinConfig.GPIO_PinPuPdControl        			= GPIO_PUPDR_PULLUP;
	handler_i2cSCL_LCD.GPIO_PinConfig.GPIO_PinSpeed              			= GPIO_OSPEED_FAST;
	handler_i2cSCL_LCD.GPIO_PinConfig.GPIO_PinAltFunMode					= AF4;
	GPIO_Config(&handler_i2cSCL_LCD);

	handler_i2cSDA_LCD.pGPIOx												= GPIOB;
	handler_i2cSDA_LCD.GPIO_PinConfig.GPIO_PinNumber						= PIN_3;
	handler_i2cSDA_LCD.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	handler_i2cSDA_LCD.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPE_OPENDRAIN;
	handler_i2cSDA_LCD.GPIO_PinConfig.GPIO_PinPuPdControl        			= GPIO_PUPDR_PULLUP;
	handler_i2cSDA_LCD.GPIO_PinConfig.GPIO_PinSpeed              			= GPIO_OSPEED_FAST;
	handler_i2cSDA_LCD.GPIO_PinConfig.GPIO_PinAltFunMode					= AF9;
	GPIO_Config(&handler_i2cSDA_LCD);

	handlerLCD.ptrI2Cx                            			= I2C2;
	handlerLCD.modeI2C                            			= I2C_MODE_SM;
	handlerLCD.slaveAddress                       			= LCD_ADDRESS;

	i2c_config(&handlerLCD);

	//	// LCD
//	LCD_Clear(&handlerLCD);
//	LCD_Init(&handlerLCD);
//	delay_10();
//	LCD_Clear(&handlerLCD);
//	delay_10();
//	LCD_setCursor(&handlerLCD, 0, 2);
//	LCD_sendSTR(&handlerLCD, "AccX = ");
	// LCD
}


void i2cACCEL(void){
	handler_i2cSCL.pGPIOx												= GPIOB;
	handler_i2cSCL.GPIO_PinConfig.GPIO_PinNumber						= PIN_8;
	handler_i2cSCL.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	handler_i2cSCL.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPE_OPENDRAIN;
	handler_i2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl        			= GPIO_PUPDR_PULLUP;
	handler_i2cSCL.GPIO_PinConfig.GPIO_PinSpeed              			= GPIO_OSPEED_FAST;
	handler_i2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode					= AF4;
	GPIO_Config(&handler_i2cSCL);

	handler_i2cSDA.pGPIOx												= GPIOB;
	handler_i2cSDA.GPIO_PinConfig.GPIO_PinNumber						= PIN_9;
	handler_i2cSDA.GPIO_PinConfig.GPIO_PinMode							= GPIO_MODE_ALTFN;
	handler_i2cSDA.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPE_OPENDRAIN;
	handler_i2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl        			= GPIO_PUPDR_PULLUP;
	handler_i2cSDA.GPIO_PinConfig.GPIO_PinSpeed              			= GPIO_OSPEED_FAST;
	handler_i2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode					= AF4;
	GPIO_Config(&handler_i2cSDA);

	handlerAccelerometer.ptrI2Cx                            			= I2C1;
	handlerAccelerometer.modeI2C                            			= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress                       			= ACCEL_ADDRESS;

	i2c_config(&handlerAccelerometer);
	i2c_writeSingleRegister(&handlerAccelerometer,BW_RATE, 0xE);
}

