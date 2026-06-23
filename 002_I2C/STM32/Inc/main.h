/*
 * main.h
 *
 *  Created on: Jun 21, 2026
 *      Author: nachiket
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f446rexx.h"

//#define MY_ADDR		0x61
#define SLAVE_ADDR  0x68
#define MY_ADDR		SLAVE_ADDR

void I2C1_GPIOInits(void);
void I2C1_Inits(void);
void GPIO_ButtonInit(void);
void delay(void);

#endif /* MAIN_H_ */
