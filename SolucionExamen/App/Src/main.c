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
#include "RTCxDriver.h"
#include "AdcDriver.h"

#include "arm_math.h"
#include <math.h>


#define HSI_CLOCK_CONFIGURED		0
#define HSE_CLOCK_CONFIGURED		1
#define PLL_CLOCK_CONFIGURED		2


#define ACCEL_ADDRESS          	0x1D
#define ACCEL_XOUT_L            50     //DATAX0
#define ACCEL_XOUT_H            51     //DATAX1
#define ACCEL_YOUT_L            52     //DATAYO
#define ACCEL_YOUT_H            53     //DATAY1
#define ACCEL_ZOUT_L            54     //DATAZ0
#define ACCEL_ZOUT_H            55     //DATAZ1

#define BW_RATE					44
#define POWER_CTL               45



/* Definicion de handlers */


//PINES
GPIO_Handler_t handlerBlinkyPin		= {0};	//PH1
GPIO_Handler_t handlerpinEXTI		= {0};
GPIO_Handler_t clockTest            = {0};


//PLL
PLL_Handler_t handlerPll	= {0};
MCO1_Handler_t handlerMCO1	= {0};


//I2C
GPIO_Handler_t handler_i2cSDA = {0};	//PB8
GPIO_Handler_t handler_i2cSCL = {0}; //PB9
I2C_Handler_t handlerAccelerometer = {0};





//Timers
BasicTimer_Handler_t	handlerBlinkyTimer	= {0};  //TIM2
BasicTimer_Handler_t	handlerAccelTimer	= {0};  //TIM4


//USART
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart6Comm = {0};
uint8_t usart2DataReceived = 0;
uint8_t rxData = 0;
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
unsigned int thirdParameter = 0;
uint8_t counterReception = 0;

//RTC
RTC_Handler_t handlerRTC = {0};
uint16_t *dataRTC = 0;
uint8_t segundos	= 0;
uint8_t minutos		= 0;
uint8_t hora		= 0;
uint8_t dia			= 0;
uint8_t mes			= 0;
uint8_t año			= 0;


char bufferData[64] = "Testing Examen";
char bufferReception[64] = {0};
char cmd[64] = {0};
char bufferMsg[64] = {0};
char userMsg[64] = {0};

bool stringComplete = false;
bool adcComplete = false;
uint8_t i2cBuffer = {0};



//ADC
PWM_Handler_t  handlerPWM = {0};
GPIO_Handler_t handlerPinPWM = {0};
ADC_Config_t adcConfig = {0};






uint8_t adcIsComplete		= 0;
uint16_t adcCounter			= 0;
uint16_t dataADC[512] = {0};
uint16_t data1ADC[256];
uint16_t data2ADC[256];
uint16_t dataCounter = 0;
uint16_t adcAuxCounter = 0;
uint16_t freqInput = 0;
uint16_t time = 66;
uint16_t dutty = 33;

float arrayX[512] = {0};
float arrayY[512] = {0};
float arrayZ[512] = {0};

EXTI_Config_t Exti		= {0};

uint8_t counter = 0;

//Banderas
uint8_t flagADC = {0};
uint8_t flagAccel = {0};

// Definicion de las cabeceras de las funciones del main
void init_Hardware(void);
void i2cACCEL(void);
void parseCommands(char  *ptrbufferReception);
void testingADC(void);
void accelSampling(void);


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



	while(1){

		if(rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			if(rxData == '@'){
				stringComplete = true;

				bufferReception[counterReception] = '\0';
				counterReception = 0;
			}
			rxData = '\0';
		}

		if(stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}
//		stringComplete = false;

		if (adcComplete) {

			for(int i = 0 ; i< 255 ; i++){
				sprintf(bufferData,"%u\t%u \n",(unsigned int)data1ADC[i],(unsigned int)data2ADC[i]);
				writeMsg(&usart6Comm,bufferData);

			}
			adcComplete = false;


		}


	}//Fin del While
	return(0);
}//Fin del Main



//Funcion que configura el hardware, timers y extis
void init_Hardware(void){

	//Configuracion del BLinky
	handlerBlinkyPin.pGPIOx											= GPIOH;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_1;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

/* ==================================== Configurando los TIMERS =============================================*/
	//Configurando el TIM2 el cual le da la tasa de encendido al LED
	handlerBlinkyTimer.ptrTIMx										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100MHz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period						= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);


	handlerAccelTimer.ptrTIMx										= TIM4;
	handlerAccelTimer.TIMx_Config.TIMx_mode							= BTIMER_MODE_UP;
	handlerAccelTimer.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100MHz_100us;
	handlerAccelTimer.TIMx_Config.TIMx_period						= 50;
	handlerAccelTimer.TIMx_Config.TIMx_interruptEnable				= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerAccelTimer);


/* ==================================== Configurando los EXTI =============================================*/


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
	usart6Comm.USART_Config.USART_baudrate							= USART_BAUDRATE_100MHz_115200;
	usart6Comm.USART_Config.USART_datasize							= USART_DATASIZE_8BIT;
	usart6Comm.USART_Config.USART_parity							= USART_PARITY_NONE;
	usart6Comm.USART_Config.USART_stopbits							= USART_STOPBIT_1;
	usart6Comm.USART_Config.USART_mode								= USART_MODE_RXTX;
	usart6Comm.USART_Config.USART_enableIntRX						= USART_RX_INTERRUPT_ENABLE;
	usart6Comm.USART_Config.USART_enableIntTX						= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart6Comm);

/* ========================== PLL ======================================================================*/

	handlerPll.clkSpeed = FREQUENCY_100MHz;
	//Calibramos el clock
	RCC->CR &= ~RCC_CR_HSITRIM;
	RCC->CR |= (12<<RCC_CR_HSITRIM_Pos);
	configPLL(&handlerPll);

	//Configuracion del PA8 como función alternativa para usarlo como prueba es mejor poner esto en el main
	 clockTest.pGPIOx = GPIOA;
	 clockTest.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	 clockTest.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	 clockTest.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	 clockTest.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	 clockTest.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	 clockTest.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

	 //Cargamos la configuracion del Pin en el registro
	 GPIO_Config(&clockTest);

/* ========================== I2C ======================================================================*/
	 i2cACCEL();

/* ========================== ADC ======================================================================*/

	adcConfig.channels[0] = ADC_CHANNEL_0;
	adcConfig.channels[1] = ADC_CHANNEL_1;
	adcConfig.dataAlignment = ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution = ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod = ADC_SAMPLING_PERIOD_84_CYCLES;
	adcConfig.adcEvent = 5;
	adcConfig.typeEvent = 1;
	//Se carga la configuración del ADC
	ADC_ConfigMultichannel(&adcConfig, 2);
	adcExternalConfig(&adcConfig);

/* ========================== PWM ======================================================================*/

	handlerPinPWM.pGPIOx = GPIOB;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;

	GPIO_Config(&handlerPinPWM);

	handlerPWM.ptrTIMx = TIM3;
	handlerPWM.config.channel = PWM_CHANNEL_3;
	handlerPWM.config.duttyCicle = 33;
	handlerPWM.config.periodo = 60;
	handlerPWM.config.prescaler = 100;

	// Activar señal
	pwm_Config(&handlerPWM);
	enableOutput(&handlerPWM);
	startPwmSignal(&handlerPWM);




}

/* ===================== Rutinas de atencion o callbacks ===============================================*/

void BasicTimer2_Callback(void){
	GPIOxTogglePin(&handlerBlinkyPin);
	counter++;
}


void BasicTimer4_Callback(void){
	if (flagAccel == 1) {
		dataCounter++;
	}
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
//	i2c_writeSingleRegister(&handlerAccelerometer,BW_RATE, 0xE);
}



void testingADC(void){
	if(adcIsComplete == 1){
		stopPwmSignal(&handlerPWM);
		for(int i= 0; i < 512; i+=2){
			sprintf(bufferData, "%u\t%u \n", dataADC[i], dataADC[i-1]);
			writeMsg(&usart6Comm, bufferData);
		}
		startPwmSignal(&handlerPWM);
		adcIsComplete = 0;
	}
}

void adcComplete_Callback(void){
	if(adcCounter == 0){
		data1ADC[adcAuxCounter]=getADC();
	}else{
		data2ADC[adcAuxCounter]=getADC();
		adcAuxCounter++;
	}
	adcCounter++;
	if(adcAuxCounter == 256){
		adcAuxCounter = 0;
		stopPwmSignal(&handlerPWM);
		adcIsComplete = true;
	}
	if(adcCounter == 2){
		adcCounter = 0;
	}


}



void parseCommands(char  *ptrbufferReception){
	sscanf(ptrbufferReception,"%s %u %u %u",cmd,&firstParameter,&secondParameter,&thirdParameter);



	//Comando para solicitar ayuda
	if(strcmp(cmd, "help") == 0){
		writeMsg(&usart6Comm, "Help Menu CMDS: \n");
		writeMsg(&usart6Comm, "1) Reloj: 0:HSI ; 1:LSE ; 2:PLL : \"Reloj # @\" \n");
		writeMsg(&usart6Comm, "2) Division del prescaler: 1,2,3,4,5: \"Prescaler # @\" \n");
		writeMsg(&usart6Comm, "3) Establecer la hora \"SetHora h m s @\" \n");
		writeMsg(&usart6Comm, "4) Consultar la hora \"HoraActual @\" \n");
		writeMsg(&usart6Comm, "5) Establecer la Fecha \"SetFecha dd mm yy @\" \n");
		writeMsg(&usart6Comm, "6) Consultar la Fecha \"FechaActual @\" \n");
		writeMsg(&usart6Comm, "7) Acelerometro. 0:Reset ; 1: Prueba de muestreo \"Accel #\" \n");
		writeMsg(&usart6Comm, "8) Muestreo acelerometro 512 datos \"Sample @\" \n");
	}

	//Comando para seleccionar el reloj
	else if(strcmp(cmd, "Reloj") == 0) {

	/*
	 * Para HSI: 0
	 * Para LSE: 1
	 * Para PLL: 2
	 */
		if (firstParameter == 0) {
//			writeMsg(&usart6Comm, "4) test reloj 0\n");

			handlerMCO1.clk = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg,"Se habilitó el reloj HSI");
			writeMsg(&usart6Comm, bufferMsg);
		}

		else if (firstParameter == 1) {
//			writeMsg(&usart6Comm, "4) test reloj 1  \"Prueba %u\"\n");

			handlerMCO1.clk = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg,"Se habilitó el reloj LSE");
			writeMsg(&usart6Comm, bufferMsg);
		}

		else if (firstParameter == 2) {
//			writeMsg(&usart6Comm, "4) test reloj 2\n");

			handlerMCO1.clk = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg,"Se habilitó el reloj PLL");
			writeMsg(&usart6Comm, bufferMsg);
		}

		else{
			writeMsg(&usart6Comm, "Comando erroneo.\n Ingresa \"help @\" para ver la lista de comandos.\n");
		}

	}

//	Fin de comandos clock


	//Comando para cambiar el Prescaler
	else if(strcmp(cmd, "Prescaler") == 0) {
		/*
		 * Division por 2: 2
		 * Division por 3: 3
		 * Division por 4: 4
		 * Division por 5: 5
		 */
		if (firstParameter == 2) {

			handlerMCO1.psc = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg,"Division del prescaler: %u",firstParameter);
			writeMsg(&usart6Comm, bufferMsg);
		}

		else if (firstParameter == 3) {

			handlerMCO1.psc = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg,"Division del prescaler: %u",firstParameter);
			writeMsg(&usart6Comm, bufferMsg);

		}

		else if (firstParameter == 4) {

			handlerMCO1.psc = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg,"Division del prescaler: %u",firstParameter);
			writeMsg(&usart6Comm, bufferMsg);

		}

		else if (firstParameter == 5) {

			handlerMCO1.psc = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsg,"Division del prescaler: %u",firstParameter);
			writeMsg(&usart6Comm, bufferMsg);

		}

		else{
			writeMsg(&usart6Comm, "Comando erroneo.\n Ingresa \"help @\" para ver la lista de comandos.\n");
		}

	}
//----------------------------Fin de comandos psc



	else if(strcmp(cmd, "SetHora") == 0) {

		if((firstParameter >23) || (secondParameter > 59) || (thirdParameter > 59)){
			writeMsg(&usart6Comm, "\nInvalid input");
		}

		else {
			handlerRTC.RTC_Config.RTC_Hours		= firstParameter;
			handlerRTC.RTC_Config.RTC_Minutes		= secondParameter;
			handlerRTC.RTC_Config.RTC_Seconds		= thirdParameter;

			RTC_Config(&handlerRTC);

			sprintf(bufferMsg,"\nHora configurada: %u:%u:%u",firstParameter,secondParameter,thirdParameter);
			writeMsg(&usart6Comm, bufferMsg);
		}

	}

	else if(strcmp(cmd, "HoraActual") == 0){
		dataRTC = read_date();

		segundos	= dataRTC[0];
		minutos		= dataRTC[1];
		hora		= dataRTC[2];

		sprintf(bufferMsg,"\nHora actual: %u:%u:%u",(unsigned int)hora,minutos,segundos);
		writeMsg(&usart6Comm, bufferMsg);
	}


	else if(strcmp(cmd, "SetFecha") == 0) {

		if((firstParameter >30) || (secondParameter > 12) || (thirdParameter > 3000)){
			writeMsg(&usart6Comm, "\nInvalid input");
		}

		else {
			handlerRTC.RTC_Config.RTC_Day		= firstParameter;
			handlerRTC.RTC_Config.RTC_Month		= secondParameter;
			handlerRTC.RTC_Config.RTC_Year		= thirdParameter;

			RTC_Config(&handlerRTC);

			sprintf(bufferMsg,"\nFecha configurada: %u/%u/%u",firstParameter,secondParameter,thirdParameter);
			writeMsg(&usart6Comm, bufferMsg);
		}

	}


	else if(strcmp(cmd, "FechaActual") == 0){
		dataRTC = read_date();

		dia		= dataRTC[4];
		mes		= dataRTC[5];
		año		= dataRTC[6];

		sprintf(bufferMsg,"\nFecha actual: %u/%u/%u",(unsigned int)dia,mes,año);
		writeMsg(&usart6Comm, bufferMsg);
	}


	else if(strcmp(cmd, "Accel") == 0){
		if (firstParameter == 0) {
			sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
			writeMsg(&usart6Comm, bufferData);
			i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x2D);
		}


		else if (firstParameter == 1) {

			flagAccel = 1;

			while(dataCounter < 20){
				arrayX[dataCounter] = ((float)valueAX() / 256.f) * 9.78;
				arrayY[dataCounter] = ((float)valueAY() / 256.f) * 9.78;
				arrayZ[dataCounter] = ((float)valueAZ() / 256.f) * 9.78;
			}
			flagAccel = 0;
			dataCounter = 0;


			for (int i = 0; i < 3; i++) {
				//Mostramos 12 almacenados anteriormente en los arreglos
				//Estos se imprimen en pantalla linea por linea
				//En un formato de tabla, con el numero(i), X, Y Z
				sprintf(bufferData, "%u X = %.2f ; Y = %.2f ; Z = %.2f \n", i,
						arrayX[i], arrayY[i], arrayZ[i]);
				writeMsg(&usart6Comm, bufferData);
			}

		}
	}


	else if(strcmp(cmd, "ADC") == 0){
		writeMsg(&usart6Comm, "CMD: Activando conversión ADC\n");
		testingADC();
		}

	else if(strcmp(cmd, "Sample") == 0){
		writeMsg(&usart6Comm, "CMD: Realizando muestreo Acelerometro\n");
		//Bandera de muestreo
		flagAccel = 1;

		while(dataCounter < 512){
			arrayX[dataCounter] = ((float)valueAX() / 256.f) * 9.78;
			arrayY[dataCounter] = ((float)valueAY() / 256.f) * 9.78;
			arrayZ[dataCounter] = ((float)valueAZ() / 256.f) * 9.78;
		}
		flagAccel = 0;
		dataCounter = 0;

		for (int i = 0; i < 512; i++) {
			//Mostramos 12 almacenados anteriormente en los arreglos
			//Estos se imprimen en pantalla linea por linea
			//En un formato de tabla, con el numero(i), X, Y Z
			sprintf(bufferData, "%u X = %.2f ; Y = %.2f ; Z = %.2f \n", i,
					arrayX[i], arrayY[i], arrayZ[i]);
			writeMsg(&usart6Comm, bufferData);
		}
	}


	else{
		writeMsg(&usart6Comm, "Comando erroneo.\n Ingresa \"help @\" para ver la lista de comandos.\n");
	}
}




