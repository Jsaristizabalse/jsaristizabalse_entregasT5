/*
 * I2CDriver.c
 *
 *  Created on: May 14, 2022
 *      Author: namontoy
 */


/*
 * Recordar que se debe configurar los pines para el I2C (SDA y SCL),
 * para lo cual se necesita el modulo GPIO y los pines configurados
 * en el modo Alternate Function.
 * Además, estos pines deben ser configurados como salidas open-drain
 * y con la resistencia en modo pull-up.
 */
#include <stdint.h>
#include "I2CDriver.h"

/* Se deben configurar los pines para SDA y SCL utilizando
 * GPIO y ALTFN
 * Configurados como OPEN DRAIN con resistencias en PULL UP */

/* Configuración periférico */
void i2c_config(I2C_Handler_t *ptrHandlerI2C){

	/* Activar la señal de reloj para el I2C seleccionado */
	if(ptrHandlerI2C->ptrI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	/* Reinicio de periferico a un estado conocido */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_SWRST;

	/* Indicar la velocidad del reloj principal, señal usada por el periférico
	 * para el reloj del bus I2C */
	ptrHandlerI2C->ptrI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos);
	ptrHandlerI2C->ptrI2Cx->CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C << I2C_CR2_FREQ_Pos);

	/* Configuración de modo I2C en el que el sistema funciona
	 * Incluye velocidad de reloj y tiempo máximo para cambio de señal
	 * Registros en 0 */
	ptrHandlerI2C->ptrI2Cx->CCR = 0;
	ptrHandlerI2C->ptrI2Cx->TRISE = 0;

	if(ptrHandlerI2C->modeI2C == I2C_MODE_SM){
		/* Modo estándar SM */
		ptrHandlerI2C->ptrI2Cx->CCR &= ~I2C_CCR_FS;

		/* Registro para generar señal de reloj */
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);

		/* Registro que controla TRISE máximo */
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM;
	}
	else {
		/* Modo rápido FM */
		ptrHandlerI2C->ptrI2Cx->CCR |= I2C_CCR_FS;

		/* Registro para generar señal de reloj */
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz << I2C_CCR_CCR_Pos);

		/* Registro que controla TRISE máximo */
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM;
	}

	/* Activar módulo I2C */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_PE;
}

/* Inicio de intercambio de datos */
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* Verificación que la linea no está ocupada CR2 BUSY bit */
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/* Generar señal de inicio */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	/* Esperar bandera de evento "start" */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}

/* Fin de intercambio de datos */
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C){
	/*Generar condición de parada */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;
}

/* Reinicio de intercambio */
void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* Generar señal de inicio */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	/* Esperar bandera de evento "start" */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}

}

/* Indicación ACK a esclavo */
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C){
	/* Escribir 0 en ACK de CR1 */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_ACK;
}

/* Indicación NACK a esclavo*/
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C){
	/* Escribir 1 en ACK de CR1 */
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

/* */
void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite){
	uint8_t auxByte = 0;
	(void) auxByte;

	/* Envia la dirección de Slave con bit que indica escritura (0) o lectura (1) */
	ptrHandlerI2C->ptrI2Cx->DR = (slaveAddress << 1) | readOrWrite;

	/* Esperamos hasta que la bandera "addr" se levante */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* Limpiar bandera de recepción ACK de "addr" leyendo SR1 y SR2 */
	auxByte = ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte = ptrHandlerI2C->ptrI2Cx->SR2;
}

void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memoryAddress){
	/* Enviar memoria que se desea leer */
	ptrHandlerI2C->ptrI2Cx->DR = memoryAddress;

	/* Esperamos hasta que el byte transmita */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}

}

void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	/* Cargar el valor que desea escribir */
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;

	/* Esperar que el byte transmita */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
		__NOP();
	}
}

uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C){
	/* Esperar la recepción del byte */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE)){
		__NOP();
	}

	/* Guardar dato en el variable de la estructura */
	ptrHandlerI2C->dataI2C = ptrHandlerI2C->ptrI2Cx->DR;
	return ptrHandlerI2C->dataI2C;
}

uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t registerToRead){

	uint8_t auxRead = 0;

	/* Generar condicion de inicio */
	i2c_startTransaction(ptrHandlerI2C);

	/* Enviar dirección de dispositivo e indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* Enviar dirección de memoria a leer */
	i2c_sendMemoryAddress(ptrHandlerI2C, registerToRead);

	/* Crear condición de RESTART */
	i2c_reStartTransaction(ptrHandlerI2C);

	/* Se vuelve a enviar la dirección con indicación de LECTURA */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	/* Leer dato que envía el esclavo */
	auxRead = i2c_readDataByte(ptrHandlerI2C);

	/* Generar condición NACK para que el esclavo solo envíe 1 byte */
	i2c_sendNoAck(ptrHandlerI2C);

	/* Generar condición de parada */
	i2c_stopTransaction(ptrHandlerI2C);

	return auxRead;
}

void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue){
	/* Generamos la condición de start */
	i2c_startTransaction(ptrHandlerI2C);

	/* Enviamos dirección del esclavo y la acción de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* Enviar dirección de memoria donde se desea escribir */
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	/* Enviar valor que se desea escribir */
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	/* Enviar condición de parada */
	i2c_stopTransaction(ptrHandlerI2C);
}
