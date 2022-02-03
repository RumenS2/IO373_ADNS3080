/*
 * ADNS3080.h
 *
 *  Created on: 19.04.2019 ã.
 *      Author: ru
 */

#ifndef ADNS3080_H_
#define ADNS3080_H_

#include "hw_config.h"

void ADNS_Reset(void);
uint8_t ReadMot(int8_t* arr); //return 0 if no motion
void ReadBurstMotion(int8_t* arr);  //7bytes
void ReadBurstSomething(uint8_t* arr,uint8_t beg, uint16_t len); //NOT tested!!!!!!!!!!!!!!!!!!!!!!

#endif /* TABLE_H_ */
