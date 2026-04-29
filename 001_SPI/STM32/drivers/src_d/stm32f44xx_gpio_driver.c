/*
 * STM32F44xx_gpio.c
 *
 *  Created on: 17-Jan-2026
 *      Author: nachiket
 */
#include "stm32f44xx_gpio_driver.h"
#include "stm32f446rexx.h"
/*
 *init func
 */
/*******************************************************************
 * @fn               - GPIO_Init
 *
 * @brief            - this func sets various parameters of a gpio pin
 *
 * @param[in]        - base addr of the gpio peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	//ENABLE THE PERIPHERAL CLOCK
	GPIO_PeriClkCtrl(pGPIOHandle->pGPIOx,ENABLE);

	uint32_t temp=0;//temp register
	//1. Config mode of gpio pin
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode<=GPIO_MODE_ANALOG)
	{
		temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode<<(2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		//pGPIOHandle->pGPIOx->MODER&=~(0x3<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		pGPIOHandle->pGPIOx->MODER&=~(0x3<<(2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER|=temp;

	}
	else
	{
		//code later (interrupt part)
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_IT_FT)
		{
			//1. config the falling trigger selectin register (FTSR)
			EXTI->FTSR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//clr the corresponding rtsr bit
			EXTI->RTSR&=~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_IT_RT)
		{
			//1. config the rtsr
			EXTI->RTSR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//clr the corresponding ftsr bit
			EXTI->FTSR&=~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_IT_RFT)
		{
			//1. config both ftsr and rtsr
			EXTI->RTSR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->FTSR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		//2. config the gpio port selection in SYSCFG_EXTICR
		uint8_t temp1=(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)/4;
		uint8_t temp2=(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)%4;
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1]=portcode<<(temp2*4);
		//3. enable the exti interrupt delivery using interrupt mask register (IMR)
		EXTI->IMR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
	temp=0;
	//2. config the speed
	temp=pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed<<(2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	//pGPIOHandle->pGPIOx->OSPEEDER&=~(0x3<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OSPEEDER&=~(0x3<<(2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDER|=temp;
	temp=0;
	//3. config the pupd settings
	temp=pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl<<(2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	//pGPIOHandle->pGPIOx->PUPDR&=~(0x3<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->PUPDR&=~(0x3<<(2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR|=temp;
	temp=0;
	//4. config the otype
	temp=pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType<<(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER&=~(0x1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER|=temp;
	temp=0;
	//5. config the alt functionality
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_ALTFN)
	{
		//config the alt function register
		uint8_t temp1,temp2;//temp1 is used to set afr register whether low or high
		temp1=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8;
		temp2=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%8;//temp2 is used for setting the bits wrt pins
		pGPIOHandle->pGPIOx->AFR[temp1]&=~(0xf<<(4*temp2));//clearing
		pGPIOHandle->pGPIOx->AFR[temp1]|=pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode<<(4*temp2);
	}
}

/*
 * deinit
 */
/*******************************************************************
 * @fn               - GPIO_DeInit
 *
 * @brief            - this func resets various parameters of a gpio pin
 *
 * @param[in]        - base addr of the gpio peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx==GPIOA)
	{
		GPIOA_REG_RESET();
		GPIOA_PCLK_DI();
	}
	else if(pGPIOx==GPIOB)
	{
		GPIOB_REG_RESET();
		GPIOB_PCLK_DI();
	}
	else if(pGPIOx==GPIOC)
	{
		GPIOC_REG_RESET();
		GPIOC_PCLK_DI();
	}
	else if(pGPIOx==GPIOD)
	{
		GPIOD_REG_RESET();
		GPIOD_PCLK_DI();
	}
	else if(pGPIOx==GPIOE)
	{
		GPIOE_REG_RESET();
		GPIOE_PCLK_DI();
	}
	else if(pGPIOx==GPIOF)
	{
		GPIOF_REG_RESET();
		GPIOF_PCLK_DI();
	}
	else if(pGPIOx==GPIOG)
	{
		GPIOG_REG_RESET();
		GPIOG_PCLK_DI();
	}
	else if(pGPIOx==GPIOH)
	{
		GPIOH_REG_RESET();
		GPIOH_PCLK_DI();
	}
}


/*
 *peripheral clk setup
 */
/*******************************************************************
 * @fn               - GPIO_PeriClkCtrl
 *
 * @brief            - this func enables or disables the peripheral clk for the given gpio port
 *
 * @param[in]        - base addr of the gpio peripheral
 * @param[in]        - ENABLE or DISABLE macros
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_PeriClkCtrl(GPIO_RegDef_t *pGPIOx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		if(pGPIOx==GPIOA)
		{
			GPIOA_PCLK_EN();
		}
		else if(pGPIOx==GPIOB)
		{
			GPIOB_PCLK_EN();
		}
		else if(pGPIOx==GPIOC)
		{
			GPIOC_PCLK_EN();
		}
		else if(pGPIOx==GPIOD)
		{
			GPIOD_PCLK_EN();
		}
		else if(pGPIOx==GPIOE)
		{
			GPIOE_PCLK_EN();
		}
		else if(pGPIOx==GPIOF)
		{
			GPIOF_PCLK_EN();
		}
		else if(pGPIOx==GPIOG)
		{
			GPIOG_PCLK_EN();
		}
		else if(pGPIOx==GPIOH)
		{
			GPIOH_PCLK_EN();
		}
	}
	else
	{
		if(pGPIOx==GPIOA)
		{
			GPIOA_PCLK_DI();
		}
		else if(pGPIOx==GPIOB)
		{
			GPIOB_PCLK_DI();
		}
		else if(pGPIOx==GPIOC)
		{
			GPIOC_PCLK_DI();
		}
		else if(pGPIOx==GPIOD)
		{
			GPIOD_PCLK_DI();
		}
		else if(pGPIOx==GPIOE)
		{
			GPIOE_PCLK_DI();
		}
		else if(pGPIOx==GPIOF)
		{
			GPIOF_PCLK_DI();
		}
		else if(pGPIOx==GPIOG)
		{
			GPIOG_PCLK_DI();
		}
		else if(pGPIOx==GPIOH)
		{
			GPIOH_PCLK_DI();
		}
	}
}

/*
 *data read and write
 */
/*******************************************************************
 * @fn               - GPIO_ReadFromInputPin
 *
 * @brief            - this func reads from the input pins and displays the input
 *
 * @param[in]        - base addr of the gpio peripheral
 * @param[in]        - pin number
 * @param[in]        -
 *
 * @return           - uint8_t value which is either 0 or 1
 *
 * @Note             - none
 *
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber)
{
	uint8_t value;
	value=(uint8_t)((pGPIOx->IDR>>PinNumber)&0x00000001);
	return value;
}

/*******************************************************************
 * @fn               - GPIO_ReadFromInputPort
 *
 * @brief            - this func reads from the input port and displays the input
 *
 * @param[in]        - base addr of the gpio peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - uint16_t values which is either 0 or 1
 *
 * @Note             - none
 *
 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;
	value=(uint16_t)(pGPIOx->IDR);
	return value;
}

/*******************************************************************
 * @fn               - GPIO_WriteToOutputPin
 *
 * @brief            - this func writes from machine the outputs the data at the desired pin
 *
 * @param[in]        - base addr of the gpio peripheral
 * @param[in]        - pin number
 * @param[in]        - value to be written
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber,uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		//WRITE 1 TO THE OUTPUT DATA REG AT THE BIT FIELD CORRESPONDING TO THE PIN NUMBER
		pGPIOx->ODR|=(1<<PinNumber);
	}
	else
	{
		pGPIOx->ODR&=~(1<<PinNumber);
	}
}
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,uint16_t Value)
{
	pGPIOx->ODR=Value;
}

/*******************************************************************
 * @fn               - GPIO_ToggleOutputPin
 *
 * @brief            - this func toggles the output pin
 *
 * @param[in]        - base addr of the gpio peripheral
 * @param[in]        - pin number
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber)
{
	pGPIOx->ODR^=(1<<PinNumber);
}

/*
 *IRQ config and ISR handling
 */
/*******************************************************************
 * @fn               - GPIO_IRQInterruptConfig
 *
 * @brief            - this func configures the interrupts
 *
 * @param[in]        - irq num
 * @param[in]        - irq priority
 * @param[in]        - en or di macro
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		if(IRQNumber<=31)
		{
			//prog iser0 register
			*NVIC_ISER0|=(1<<IRQNumber);
		}
		else if(IRQNumber>=32 && IRQNumber<64)
		{
			//prog iser1 register
			*NVIC_ISER1|=(1<<(IRQNumber%32));
		}
		else if(IRQNumber>=64 && IRQNumber<96)
		{
			//prog iser2 register
			*NVIC_ISER2|=(1<<(IRQNumber%32));
		}
	}
	else
	{
		if(IRQNumber<=31)
		{
			//prog icer0 register
			*NVIC_ICER0|=(1<<IRQNumber);
		}
		else if(IRQNumber>=32 && IRQNumber<64)
		{
			//prog icer1 register
			*NVIC_ICER1|=(1<<(IRQNumber%32));
		}
		else if(IRQNumber>=64 && IRQNumber<96)
		{
			//prog icer2 register
			*NVIC_ICER2|=(1<<(IRQNumber%32));
		}
	}
}

/*******************************************************************
 * @fn               - GPIO_IRQPriorityConfig
 *
 * @brief            - this func configures the ipr (interrupt priority registers)
 *
 * @param[in]        - IRQNumber
 * @param[in]        - irq priority
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. finding ipr register
	uint8_t iprx=IRQNumber/4;
	uint8_t iprx_section=IRQNumber%4;
	uint8_t shift_amt=(iprx_section*8)+(8-N0_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR+iprx)|=(IRQPriority<<(shift_amt));
}

/*******************************************************************
 * @fn               - GPIO_IRQHandling
 *
 * @brief            - this func manages the interrupts
 *
 * @param[in]        - pin number
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void GPIO_IRQHandling(uint8_t PinNumber)
{
	//clear the exti priority register corresponding to the pin number
	if(EXTI->PR & (1<<PinNumber))
	{
		//CLR
		EXTI->PR|=(1<<PinNumber); //by writing one we have to clr the register as per the manual
	}
}
