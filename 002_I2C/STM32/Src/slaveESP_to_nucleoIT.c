/*
 * slaveESP_to_nucleoIT.c
 *
 *  Created on: Jun 23, 2026
 *      Author: nachiket
 */


#include "main.h"

// I2C1_SCL ---> PB6
// I2C1_SDA ---> PB7

I2C_Handle_t I2C1Handle;

//rcv buffer
uint8_t rcv_buffer[32];

//flag variable
uint8_t rx_cmplt = RESET;


int main(void)
{
	uint8_t command_code;
	uint8_t len;

	//init gpio pins to alt func 4 mode (for i2c1)
	I2C1_GPIOInits();

	//init and configure the i2c1 peripheral
	I2C1_Inits();

	//i2c irq configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);  //enable event irq
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);  //enable error irq

	//config the button pin
	GPIO_ButtonInit();

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	I2C_ManageAcking(I2C1, ENABLE);

	//wait for button press
	while(1)
	{
		memset(rcv_buffer,0,sizeof(rcv_buffer));
		len=0;

		//user button is high by default when pressed it returns low
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));

		//to avoid button de bounching issues
		delay();

		command_code=0x51; //command to req data length from slave
		while(I2C_MasterSendDataIT(&I2C1Handle, &command_code, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);
		delay();
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		while(rx_cmplt != SET);
		printf("Lenght of data to be received from ESP32 slave: %d \n",len);
		rx_cmplt=RESET;


		command_code=0x52; //command to req actual data from slave
		while(I2C_MasterSendDataIT(&I2C1Handle, &command_code, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);
		delay();
		while(I2C_MasterReceiveDataIT(&I2C1Handle, rcv_buffer, len, SLAVE_ADDR, I2C_DISABLE_SR) != I2C_READY);


		while(rx_cmplt != SET);
		printf("Data to be received from ESP32 slave: %s\n",rcv_buffer);
		rx_cmplt=RESET;
	}

	while(1);

}

/* Peripheral and GPIO configurations */
void I2C1_GPIOInits(void)
{
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx=GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
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
	if(AppEv == I2C_EV_TX_CMPLT)
	{
		printf("Tx is complete\n");
	}
	else if(AppEv == I2C_EV_RX_CMPLT)
	{
		printf("Rx is complete\n");
		rx_cmplt = SET;
	}
	else if(AppEv == I2C_ERROR_AF)
	{
		printf("Error : ACK failiure\n");
		//in master ack faliure happens when slave does NOT ack to the
		//data byte sent by the master
		I2C_CloseSendData(&I2C1Handle);

		//generate stop condition to release the bus
		I2C_GenerateStopCondition(I2C1);

		//hang in infinite loop
		while(1);
	}

}

