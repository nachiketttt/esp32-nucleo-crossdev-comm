/*
 * main.h
 *
 *  Created on: Jun 26, 2026
 *      Author: nachiket
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f446rexx.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void delay(void);
void GPIO_ButtonInit(void);

void USART1_Inits(void);
void USART1_GPIOInits(void);


#endif /* MAIN_H_ */
