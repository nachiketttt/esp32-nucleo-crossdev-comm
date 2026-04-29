/*
 * main.c
 *
 *  Created on: 15-March-2026
 *      Author:Nachiket
 */

/*
 * pa4 as SPI1_NSS
 * pa5 as SPI1_SCL
 * pa6 as SPI1_MISO
 * pa7 as SPI1_MOSI
 * af mode 5 (af5 for all pins)
 */
#include <string.h>
#include "stm32f446rexx.h"

//global variables
GPIO_Handle_t SPI1Pins;
SPI_Handle_t SPI1handle;
GPIO_Handle_t GpioNss;


void delay()
{
	for(uint32_t i=1;i<200000;i++);
}


//master pins
void SPI1_GPIOInits(void)
{
	//GPIO_Handle_t SPI1Pins;
	SPI1Pins.pGPIOx=GPIOA;
	SPI1Pins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	SPI1Pins.GPIO_PinConfig.GPIO_PinAltFunMode=5;
	SPI1Pins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	SPI1Pins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_NO_PUPD;
	SPI1Pins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_FAST;

	//SCLK
	SPI1Pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_5;
	GPIO_Init(&SPI1Pins);

	//MOSI
	SPI1Pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_7;
	GPIO_Init(&SPI1Pins);

	//MISO
	SPI1Pins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_6;
	GPIO_Init(&SPI1Pins);
}

void SPI1_Inits(void)
{
	SPI1handle.pSPIx=SPI1;
	SPI1handle.SPIConfig.SPI_BusConfig=SPI_BUS_CONFIG_FD;
	SPI1handle.SPIConfig.SPI_DeviceMode=SPI_DEVICE_MODE_MASTER;
	SPI1handle.SPIConfig.SPI_SclkSpeed=SPI_SCLK_SPEED_DIV8; //GENERATES SCLK OF 2MHZ
	SPI1handle.SPIConfig.SPI_DFF=SPI_DFF_8BITS;
	SPI1handle.SPIConfig.SPI_CPOL=SPI_CPOL_LOW;
	SPI1handle.SPIConfig.SPI_CPHA=SPI_CPHA_LOW;
	SPI1handle.SPIConfig.SPI_SSM=SPI_SSM_EN;

	SPI_Init(&SPI1handle);
}


void GPIO_NSSInit(void)
{
	//nss config
	GpioNss.pGPIOx=GPIOA;
	GpioNss.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_4;
	GpioNss.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_OUT;
	GpioNss.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;

	GPIO_Init(&GpioNss);
}


int main(void)
{
	char user_data[]="Hello World\r";

	//this function is used to initialize the GPIO pins to behave as SPI1 pins
	SPI1_GPIOInits();

	//This function is used to initialize the SPI1 peripheral parameters
	SPI1_Inits();

	//This func is used to initialize the GPIO NSS pin which pulls the cs pin on esp32 to ground
	GPIO_NSSInit();

	//Pulling the cs pin on esp32 to high
	GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_4, GPIO_PIN_SET);

	//this makes NSS signal internally high and avoids MODF error
	SPI_SSIConfig(SPI1,ENABLE);

	//Enable the SPI1 peripheral
	SPI_PeripheralControl(SPI1, ENABLE);

	while(1)
	{
		//Pulling the cs pin on esp32 to ground
		GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_4, GPIO_PIN_RESET);

		//Sending data
		SPI_SendData(SPI1, (uint8_t*)user_data, strlen(user_data));

		//Confirming SPI is not busy
		while(SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG));

		//Pulling the cs pin on esp32 back to high
		GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_4, GPIO_PIN_SET);

		//gives delay before sending another time
		delay();
	}

	//Disabling the SPI1 peripheral
	SPI_PeripheralControl(SPI1, DISABLE);

	while(1);

	return 0;
}



