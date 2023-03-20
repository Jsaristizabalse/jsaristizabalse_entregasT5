/*
 * StructureExample.h
 *
 *  Created on: Mar 7, 2023
 *      Author: aristizabal
 */
#include <stdint.h>
#ifndef STRUCTUREEXAMPLE_H_
#define STRUCTUREEXAMPLE_H_

typedef struct
{
	char		ID;
	uint8_t		repetitions;
	uint8_t		dummy;
	uint8_t		counterUp;
	uint32_t	timestamp;
} BasicExample;



#endif /* STRUCTUREEXAMPLE_H_ */
