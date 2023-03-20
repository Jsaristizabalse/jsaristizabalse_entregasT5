/*
 * Taller4_2023.c
 *
 *  Created on: Mar 2, 2023
 *      Author: aristizabal
 */

#include <stdint.h>

char var1 = 0;
int var2 = 0;
short var3 = 0;
long var4 = 0;

uint8_t var5 = 0;//con esto garantizamos que ocupa 8bits de memoria
uint8_t var6 = 0; //al no tener la u, no es "unsigned"
int16_t var7 = 0; //aqui tenemos 16 bitssudo apt-get install libncurses5
uint16_t resultado = 0;
//int64_t = 0;
//el shift a la izquierda duplica la variable
int main(void){

	uint16_t testShift = 0b000011010110101;
	uint16_t testMask = 0b0000000000111000;

	while(1){
		resultado = testShift | testMask;
		resultado = testShift & (~testMask);
//		testMask = testMask <<3;
//		testMask = ~testMask;
//		testMask = testShift & testMask;

	}
}
