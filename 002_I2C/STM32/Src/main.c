#include "main.h"

// I2C1_SCL ---> PB6
// I2C1_SDA ---> PB7

I2C_Handle_t I2C1Handle;

uint8_t some_data[]="We are testing I2C master tx\n";



int main(void)
{
	//init gpio pins to alt func 4 mode (for i2c1)
	I2C1_GPIOInits();

	//init and configure the i2c1 peripheral
	I2C1_Inits();

	//config the button pin
	GPIO_ButtonInit();

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	//wait for button press
	while(1)
	{
		//user button is high by default when pressed it returns low
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));

		//to avoid button de_bounching issues
		delay();

		//send some data to the slave
		I2C_MasterSendData(&I2C1Handle, some_data, strlen((char*)some_data), SLAVE_ADDR, I2C_DISABLE_SR);

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
	for(uint32_t i=0;i<500000/2;i++);
}
