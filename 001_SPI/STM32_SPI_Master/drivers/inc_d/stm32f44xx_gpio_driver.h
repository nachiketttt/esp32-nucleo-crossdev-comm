/*
 * stm32f44xx_gpio_driver.h
 *
 *  Created on: 17-Jan-2026
 *      Author: nachiket
 */

#ifndef INC_D_STM32F44XX_GPIO_DRIVER_H_
#define INC_D_STM32F44XX_GPIO_DRIVER_H_

#include "stm32f446rexx.h"
//this a config structure for a gpio pin
typedef struct
{
	uint8_t GPIO_PinNumber; //POSSIBLE VALUES FROM @gpio_pin_numbers
	uint8_t GPIO_PinMode;  //possible values from @gpio_pin_modes
	uint8_t GPIO_PinSpeed; //possible values from @gpio_pin_speed
	uint8_t GPIO_PinPuPdControl; //possible values from @gpio_pin_pupd
	uint8_t GPIO_PinOPType; //possible values from @gpio_op_type
	uint8_t GPIO_PinAltFunMode; //possible values from @gpio_pin_alt_modes

}GPIO_PinConfig_t;


//handle structure for a gpio pin
typedef struct
{
	//ptr to hold the base address of the gpio peripheral
	GPIO_RegDef_t *pGPIOx; /*this holds the base addr of the gpio port to which the pin belongs*/
	GPIO_PinConfig_t GPIO_PinConfig; /*this holds gpio pin config settings*/
}GPIO_Handle_t;

/* @gpio_pin_numbers
 * gpio pin numbers
 */
#define GPIO_PIN_NO_0     0
#define GPIO_PIN_NO_1     1
#define GPIO_PIN_NO_2     2
#define GPIO_PIN_NO_3     3
#define GPIO_PIN_NO_4     4
#define GPIO_PIN_NO_5     5
#define GPIO_PIN_NO_6     6
#define GPIO_PIN_NO_7     7
#define GPIO_PIN_NO_8     8
#define GPIO_PIN_NO_9     9
#define GPIO_PIN_NO_10    10
#define GPIO_PIN_NO_11    11
#define GPIO_PIN_NO_12    12
#define GPIO_PIN_NO_13    13
#define GPIO_PIN_NO_14    14
#define GPIO_PIN_NO_15    15


/* @gpio_pin_modes
 * GPIO pin possible modes
 */
#define GPIO_MODE_IN      0
#define GPIO_MODE_OUT     1
#define GPIO_MODE_ALTFN   2
#define GPIO_MODE_ANALOG  3
#define GPIO_MODE_IT_FT   4  //input falling edge
#define GPIO_MODE_IT_RT   5  //input rising edge
#define GPIO_MODE_IT_RFT  6  //input rising falling edge trigger

/* @gpio_op_type
 * GPIO pin possible output types
 */
#define GPIO_OP_TYPE_PP   0  //PUSH PULL STATE
#define GPIO_OP_TYPE_OD   1  //OPEN DRAIN STATE

/* @gpio_pin_speed
 * GPIO pin possible output speeds
 */
#define GPIO_SPEED_LOW    0
#define GPIO_SPEED_MED    1 //MEDIUM SPEED
#define GPIO_SPEED_FAST   2
#define GPIO_SPEED_HIGH   3

/* @gpio_pin_pupd
 * GPIO pin pull up and pull down config macros
 */
#define GPIO_NO_PUPD       0
#define GPIO_PIN_PU        1
#define GPIO_PIN_PD        2

/**************************************************************************************
 *                       APIs supported by this driver
 *      for more info about the APIs check the function definitions
 **************************************************************************************/

/*
 *init deinit
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 *peripheral clk setup
 */
void GPIO_PeriClkCtrl(GPIO_RegDef_t *pGPIOx,uint8_t EnorDi);

/*
 *data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber,uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber);

/*
 *IRQ config and ISR handling
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);


#endif /* INC_D_STM32F44XX_GPIO_DRIVER_H_ */
