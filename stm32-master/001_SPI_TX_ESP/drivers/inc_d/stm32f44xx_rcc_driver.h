/*
 * stm32f44xx_rcc_driver.h
 *
 *  Created on: 17-Feb-2026
 *      Author: nachiket
 */

#ifndef INC_D_STM32F44XX_RCC_DRIVER_H_
#define INC_D_STM32F44XX_RCC_DRIVER_H_

#include "stm32f446rexx.h"

/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/

//this return APB1 clock value
uint32_t RCC_GetPCLK1Value(void);

//this return APB2 clock value
uint32_t RCC_GetPCLK2Value(void);

//returns value of pll clock
int RCC_GetPLLOutputClock();

#endif /* INC_D_STM32F44XX_RCC_DRIVER_H_ */
