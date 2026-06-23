/*
 * nucleo_slave.c
 *
 *  Created on: Jun 23, 2026
 *      Author: nachiket
 */


#include "main.h"

// I2C1_SCL ---> PB6
// I2C1_SDA ---> PB7

I2C_Handle_t I2C1Handle;

//rcv buffer
uint8_t tx_buffer[32]="STM32 SLAVE mode testing.... \n";

//flag variable
uint8_t rx_cmplt = RESET;


int main(void)
{
	//init gpio pins to alt func 4 mode (for i2c1)
	I2C1_GPIOInits();

	//init and configure the i2c1 peripheral
	I2C1_Inits();

	//i2c irq configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);  //enable event irq
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);  //enable error irq

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	//config the button pin
	GPIO_ButtonInit();

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	I2C_ManageAcking(I2C1, ENABLE);

	while(1);
}

/* Peripheral and GPIO configurations */
void I2C1_GPIOInits(void)
{
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx=GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_NO_PUPD;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode=4;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_FAST;

	//scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	//sda
	I2CPins.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_7;
	GPIO_Init(&I2CPins);
}

void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx=I2C1;

	I2C1Handle.I2CHandle.I2C_ACKControl=I2C_ACK_ENABLE;
	I2C1Handle.I2CHandle.I2C_DeviceAddress=MY_ADDR;
	I2C1Handle.I2CHandle.I2C_FMDutyCycle=I2C_FM_DUTY_2;
	I2C1Handle.I2CHandle.I2C_SCLSpeed=I2C_SCL_SPEED_SM; //scl freq is 100khz and below

	I2C_Init(&I2C1Handle);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn;

	//this is btn gpio configuration
	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);
}

void delay(void)
{
	for(uint32_t i=0;i<500000;i++);
}

/* IRQ Handlers */

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

/* Callback handlers */

void I2C_ApplicationEventCallback(I2C_Handle_t *pHandle,uint8_t AppEv)
{
	static uint8_t commandCode=0;
	static uint8_t Cnt=0;

	if(AppEv == I2C_EV_DATA_REQ)
	{
		//master wants some data so slave has to send it
		if(commandCode == 0x51)
		{
			//send the length info to the master
			I2C_SlaveSendData(pHandle->pI2Cx, strlen((char*)tx_buffer));
		}
		else if(commandCode == 0x52)
		{
			//send tx_buffer contents
			I2C_SlaveSendData(I2C1, tx_buffer[Cnt++]);
		}

	}
	else if(AppEv == I2C_EV_DATA_RCV)
	{
		//data is waiting for slave to be read
		commandCode = I2C_SlaveReceiveData(pHandle->pI2Cx);
		printf("Received data %x", commandCode);
	}
	else if(AppEv == I2C_ERROR_AF)
	{
		//this happens only during slave txing
		//master has sent the NACK so slave should understand that
		//master doesnt need more data
		commandCode = 0xff;
		Cnt = 0;
	}
	else if(AppEv == I2C_EV_STOP)
	{
		//this happens only during slave reception
		//master has ended i2c communication with slave
	}

}


