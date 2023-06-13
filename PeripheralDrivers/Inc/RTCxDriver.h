/*
 * RTCxDriver.h
 *
 *  Created on: Jun 9, 2023
 *      Author: aristizabal
 */

#ifndef RTCXDRIVER_H_
#define RTCXDRIVER_H_

#include <stm32f4xx.h>

#define TIME_NOTATION_AM_OR_24	0
#define TIME_NOTATION_PM		1

#define  TIME_FORMAT_24_HOUR	0
#define  TIME_FORMAT_AM_OR_PM	1

typedef struct{ /**Configuración RTC**/
uint16_t RTC_Hours; //Configuración de las horas
uint16_t RTC_Minutes; //Configuración de los minutos
uint16_t RTC_Seconds; //Configuración de los segundos
uint16_t RTC_Year; //Configuración del año
uint16_t RTC_Format; // Configuración del formato
uint16_t RTC_Notation; //Configuración de la notación
uint16_t RTC_Month; //Configuración del mes
uint16_t RTC_Day; //Configuración valor día

}RTC_Config_t;

typedef struct{
	RTC_TypeDef 	*ptrRTC; //Dirección
	RTC_Config_t	RTC_Config; //Configuración

}RTC_Handler_t;

//Cabecera función
void RTC_Config(RTC_Handler_t *ptrRtcHandler);
void *read_date(void);


#endif /* RTCXDRIVER_H_ */
