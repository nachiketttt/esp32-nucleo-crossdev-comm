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

void SPI1_Inits(void);
void SPI_GPIOInits(void);

int SPI_VerifyResponse(uint8_t ackbyte);

void led_control(void);
void sensor_read(void);
void led_read(void);
void print(void);
void id_read(void);

//command codes
#define COMMAND_LED_CTRL          0x50
#define COMMAND_SENSOR_READ       0x51
#define COMMAND_LED_READ          0x52
#define COMMAND_PRINT             0x53
#define COMMAND_ID_READ           0x54

#define LED_ON                    1
#define LED_OFF                   0

//esp32 led
#define LED_PIN                   2

#define ANALOG_PIN0               0
#define ANALOG_PIN1               1
#define ANALOG_PIN2               2
#define ANALOG_PIN3               3
#define ANALOG_PIN4               4
#endif /* MAIN_H_ */
