#include "GPIOxDriver.h"
#include "stm32f411xx_hal.h"
#include <stdio.h>

void delay()
{
  unsigned int i;
  for(i=0; i<1600000; i++);
}

uint16_t USER_BUTTON = 0;
/* Función principal del programa. Es acá donde se ejecuta todo */
int main(void){
	// *******
	// Definimos el handler para el PIN que deseamos configurar. Estoy creando un objeto GPIO_Handler_t
	GPIO_Handler_t handlerUserLedPin = {0};

	// Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;      // Ninguna función

// Tercer punto
// Definición de los pines y de los puertos
//Definición GPIOC
	GPIO_Handler_t handlerPB8 = {0};

	handlerPB8.pGPIOx = GPIOB;
	handlerPB8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPB8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPB8.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPB8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPB8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPB8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC9 = {0};

	handlerPC9.pGPIOx = GPIOC;
	handlerPC9.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
	handlerPC9.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC9.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC9.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC9.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC9.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC7 = {0};

	handlerPC7.pGPIOx = GPIOC;
	handlerPC7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPC7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC7.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC8 = {0};

	handlerPC8.pGPIOx = GPIOC;
	handlerPC8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
	handlerPC8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC8.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPC6 = {0};

	handlerPC6.pGPIOx = GPIOC;
	handlerPC6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPC6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPC6.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPC6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPC6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;


	GPIO_Handler_t handlerPA6 = {0};

	handlerPA6.pGPIOx = GPIOA;
	handlerPA6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
	handlerPA6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPA6.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPA6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPA6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPA6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	GPIO_Handler_t handlerPA7 = {0};

	handlerPA7.pGPIOx = GPIOA;
	handlerPA7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerPA7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerPA7.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPA7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerPA7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerPA7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	//Configuración User Button
	//
	GPIO_Handler_t handlerPC13 = {0};
	handlerPC13.pGPIOx = GPIOC;
	handlerPC13.GPIO_PinConfig.GPIO_PinNumber       = PIN_13;
	handlerPC13.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_IN;
	handlerPC13.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerPC13.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;


	// Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
	GPIO_Config(&handlerPC9);
	GPIO_Config(&handlerPB8);
	GPIO_Config(&handlerPC7);
	GPIO_Config(&handlerPC8);
	GPIO_Config(&handlerPC6);
	GPIO_Config(&handlerPA6);
	GPIO_Config(&handlerPA7);
	GPIO_Config(&handlerPC13);

	// Hacemos que el PIN_A5 quede encendido



	// Este es el ciclo principal, donde se ejecuta todo el programa

	int c, con, k, estado;   // inicializando variables
	con = 1; //contador para los LEDS, es el numero que se va a alumnbrar
	estado = 0; //0 Subiendo 1 Bajando  es lo que debe cambiar el botón
	while(1){

		GPIO_ReadPin(&handlerUserLedPin); // Prueba lectura del valor PIN5
		GPIOxTogglePin(&handlerUserLedPin); //

		for (c = 6;c >= 0;c--){ // entra en el contador para pasar la representación del contador a binario
			k = con >> c;   //shift de c posiciones y el for los resta
			if(c == 6){     // menos significativo PC9
				if (k&1){ //si es 1 puesto que el and da true o false
					GPIO_WritePin(&handlerPC9, SET);
			    }else {
			    	GPIO_WritePin(&handlerPC9, CLEAR);
			    }
			}
			if(c == 5){
				if (k&1){ //guardo en la variable k el shift
			    	GPIO_WritePin(&handlerPC6, SET);
			    }else {
			    	GPIO_WritePin(&handlerPC6, CLEAR);
			    }
			}
			if(c == 4){
				if (k&1){
			    	GPIO_WritePin(&handlerPB8, SET);
			    }else {
			    	GPIO_WritePin(&handlerPB8, CLEAR);
			    }
			}
			if(c == 3){
				if (k&1){
					GPIO_WritePin(&handlerPA6, SET);
				}else {
					GPIO_WritePin(&handlerPA6, CLEAR);
			    }
			}
			if(c == 2){
				if (k&1){
					GPIO_WritePin(&handlerPC7, SET);
				}else {
					GPIO_WritePin(&handlerPC7, CLEAR);
			    }
			}
			if(c == 1){
				if (k&1){
					GPIO_WritePin(&handlerPC8, SET);
			    }else {
			    	GPIO_WritePin(&handlerPC8, CLEAR);
			    }
			}
			if(c == 0){//mas significativo
				if (k&1){
					GPIO_WritePin(&handlerPA7, SET);
			   }else {
				   GPIO_WritePin(&handlerPA7, CLEAR);
			   }
			}
		}
		//restricción que no puede ser más de 60 ni menos de 1
		if(con > 60){con = 1;
		}
		if(con < 1){con = 60;
		}
		delay();

		USER_BUTTON= GPIO_ReadPin(&handlerPC13);

		if( USER_BUTTON == 0){
			estado=1;
		}else {
			estado=0;
		}

		if(estado == 0){con++;}
		else{con--;} //si el estado es 0 hay un aumento en el contador y de lo contrario hay un decremento
	}
	return 0;

}
