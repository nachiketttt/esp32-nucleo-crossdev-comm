/*
 * stm32f44xx_spi_driver.c
 *
 *  Created on: 24-Jan-2026
 *      Author: nachiket
 */
#include "stm32f44xx_spi_driver.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pHandle);

/*******************************************************************
 * @fn               - SPI_PeriClkCtrl
 *
 * @brief            - this func enables the pripheral clk control for spi
 *
 * @param[in]        - base addr of the spi peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void SPI_PeriClkCtrl(SPI_RegDef_t *pSPIx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		if(pSPIx==SPI1)
		{
			SPI1_PCLK_EN();
		}
		else if(pSPIx==SPI2)
		{
			SPI2_PCLK_EN();
		}
		else if(pSPIx==SPI3)
		{
			SPI3_PCLK_EN();
		}
		else if(pSPIx==SPI4)
		{
			SPI4_PCLK_EN();
		}
	}
	else
	{
		if(pSPIx==SPI1)
		{
			SPI1_PCLK_DI();
		}
		else if(pSPIx==SPI2)
		{
			SPI2_PCLK_DI();
		}
		else if(pSPIx==SPI3)
		{
			SPI3_PCLK_DI();
		}
		else if(pSPIx==SPI4)
		{
			SPI4_PCLK_DI();
		}
	}
}

/*******************************************************************
 * @fn               - SPI_Init
 *
 * @brief            - this func sets various parameters of a SPI pin
 *
 * @param[in]        - base addr of the spi peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	//spi peripheral clk enable
	SPI_PeriClkCtrl(pSPIHandle->pSPIx,ENABLE);
	//configuring the spi_cr1 register
	uint32_t tempreg=0;

	//1. configure the device mode
	tempreg|=pSPIHandle->SPIConfig.SPI_DeviceMode<<2;
	//2. config the bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig==SPI_BUS_CONFIG_FD)
	{
		//bidi mode is cleared
		tempreg&=~(1<<SPI_CR1_BIDI_MODE);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig==SPI_BUS_CONFIG_HD)
	{
		//bidi mode should be set
		tempreg&=~(1<<SPI_CR1_BIDI_MODE); //clear the 15th bit
		tempreg|=(1<<SPI_CR1_BIDI_MODE);  //setting the 15th bit
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig==SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//bidi mode clr
		tempreg&=~(1<<SPI_CR1_BIDI_MODE);
		//rxonly bit must be set
		tempreg&=~(1<<SPI_CR1_RX_ONLY);//clearing the 10th bit
		tempreg|=(1<<SPI_CR1_RX_ONLY); //setting the 10th bit
	}
	//3. config the spi serial clk speed (baud rate)
	tempreg|=pSPIHandle->SPIConfig.SPI_SclkSpeed<<SPI_CR1_BR;

	//4. config the spi data frame format (DFF)
	tempreg|=pSPIHandle->SPIConfig.SPI_DFF<<SPI_CR1_DFF;

	//5. config the CPOL
	tempreg|=pSPIHandle->SPIConfig.SPI_CPOL<<SPI_CR1_CPOL;

	//6. config the CPHA
	tempreg|=pSPIHandle->SPIConfig.SPI_CPHA<<SPI_CR1_CPHA;

	//7. config the SSM bit
	tempreg|=pSPIHandle->SPIConfig.SPI_SSM<<SPI_CR1_SSM;

	pSPIHandle->pSPIx->CR1=tempreg;
}

/*
 * deinit
 */
/*******************************************************************
 * @fn               - SPI_DeInit
 *
 * @brief            - this func resets various parameters of a SPI pin
 *
 * @param[in]        - base addr of the SPI peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx==SPI1)
	{
		SPI1_REG_RESET();
		SPI1_PCLK_DI();
	}
	else if(pSPIx==SPI2)
	{
		SPI2_REG_RESET();
		SPI2_PCLK_DI();
	}
	else if(pSPIx==SPI3)
	{
		SPI3_REG_RESET();
		SPI3_PCLK_DI();
	}
	else if(pSPIx==SPI4)
	{
		SPI4_REG_RESET();
		SPI4_PCLK_DI();
	}
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx,uint32_t FlagName)
{
	if(pSPIx->SR&FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*******************************************************************
 * @fn               - SPI_SendData
 *
 * @brief            - this func sends data
 *
 * @param[in]        - base addr of the spi peripheral
 * @param[in]        - tx buffer addr (ptr to the data)
 * @param[in]        - length of the data to be sent
 *
 * @return           - none
 *
 * @Note             - this is a blocking call using while loops (polling type api function)
 *
 */
void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t *pTxBuffer,uint32_t len)
{
	while(len>0)
	{
		//1. wait until TXE is set
		while(SPI_GetFlagStatus(pSPIx,SPI_TXE_FLAG)==FLAG_RESET);

		//2. check the dff bit in cr1
		if( (pSPIx->CR1) & (1<<SPI_CR1_DFF) )
		{
			//16 bit dff
			//1. load the data into the data register (DR)
			pSPIx->DR=*((uint16_t*)(pTxBuffer));
			len--;
			len--; //using len-- 2 times because the data is 16 bits and it is equal to 2 bytes
			pTxBuffer+=2;
		}
		else
		{
			//8 bit data
			pSPIx->DR=*pTxBuffer;
			len--;
			(pTxBuffer)++;
		}
	}
}

/*******************************************************************
 * @fn               - SPI_PeripheralControl
 *
 * @brief            - this func enables the spix peripheral
 *
 * @param[in]        - base addr of the spi peripheral
 * @param[in]        - enable or disable macro
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             -
 *
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		pSPIx->CR1|=(1<<SPI_CR1_SPE);
	}
	else
	{
		pSPIx->CR1&=~(1<<SPI_CR1_SPE);
	}
}

/*******************************************************************
 * @fn               - SPI_ReceiveData
 *
 * @brief            - this func receives data
 *
 * @param[in]        - base addr of the spi peripheral
 * @param[in]        - rx buffer addr (ptr to the data)
 * @param[in]        - length of the data to be sent
 *
 * @return           - none
 *
 * @Note             - this is a blocking call using while loops (polling type api function)
 *
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx,uint8_t *pRxBuffer,uint32_t len)
{
	while(len>0)
	{
		//1. wait until RXNE is set
		while(SPI_GetFlagStatus(pSPIx,SPI_RXNE_FLAG)==FLAG_RESET);

		//2. check the dff bit in cr1
		if( (pSPIx->CR1) & (1<<SPI_CR1_DFF) )
		{
			//16 bit dff
			//1. load the data from the data register (DR) into the Rxbuffer into
			*((uint16_t*)(pRxBuffer))=pSPIx->DR;
			len--;
			len--; //using len-- 2 times because the data is 16 bits and it is equal to 2 bytes
			(pRxBuffer)+=2;
		}
		else
		{
			//8 bit data
			*((uint8_t*)(pRxBuffer))=pSPIx->DR;
			len--;
			(pRxBuffer)++;
		}
	}
}

/*******************************************************************
 * @fn               - SPI_SendDataIT
 *
 * @brief            - this func receives data
 *
 * @param[in]        - addr of the spi peripheral handle structure
 * @param[in]        - rx buffer addr (ptr to the data)
 * @param[in]        - length of the data to be sent
 *
 * @return           - none
 *
 * @Note             - this is NOT a blocking call and uses INTERRUPTS
 *
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer,uint32_t len)
{
	uint8_t state=pSPIHandle->TxState;
	if(state!=SPI_BUSY_IN_TX)
	{
		//1. save the tx buffer addr and len info in some global variable
		pSPIHandle->pTxBuffer=pTxBuffer;
		pSPIHandle->TxLen=len;

		//2. mark the spi state as busy in transmission
		//no other code can take over same spi peripheral until transmission is over
		pSPIHandle->TxState=SPI_BUSY_IN_TX;

		//3. enable the txeie ctrl bit to get interrupt whenever txe flag is set in sr reg
		pSPIHandle->pSPIx->CR2|=(1<<SPI_CR2_TXEIE);
	}
	return state;
	//4. data transmission willl be handled by isr code (will implement later)
}

/*******************************************************************
 * @fn               - SPI_ReceiveDataIT
 *
 * @brief            - addr of the spi peripheral handles structure
 *
 * @param[in]        - base addr of the spi peripheral
 * @param[in]        - rx buffer addr (ptr to the data)
 * @param[in]        - length of the data to be sent
 *
 * @return           - none
 *
 * @Note             - this is NOT a blocking call and uses INTERRUPTS
 *
 */
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pRxBuffer,uint32_t len)
{
	uint8_t state=pSPIHandle->RxState;
	if(state!=SPI_BUSY_IN_RX)
	{
		//1. save the rx buffer addr and len info in some global variable
		pSPIHandle->pRxBuffer=pRxBuffer;
		pSPIHandle->RxLen=len;

		//2. mark the spi state as busy in transmission
		//no other code can take over same spi peripheral until transmission is over
		pSPIHandle->RxState=SPI_BUSY_IN_RX;

		//3. enable the rxeie ctrl bit to get interrupt whenever rxe flag is set in sr reg
		pSPIHandle->pSPIx->CR2|=(1<<SPI_CR2_RXNEIE);
	}
	return state;
}

/*
 *IRQ config and ISR handling
 */
/*******************************************************************
 * @fn               - SPI_IRQInterruptConfig
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
void SPI_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
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
 * @fn               - SPI_IRQPriorityConfig
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
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. finding ipr register
	uint8_t iprx=IRQNumber/4;
	uint8_t iprx_section=IRQNumber%4;
	uint8_t shift_amt=(iprx_section*8)+(8-N0_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR+iprx)|=(IRQPriority<<(shift_amt));
}

/*******************************************************************
 * @fn               - SPI_IRQHandling
 *
 * @brief            - this func manages the interrupts
 *
 * @param[in]        -
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
	uint8_t temp1,temp2;
	//first checking for txe
	temp1=pHandle->pSPIx->SR & (1<<SPI_SR_TXE);
	temp2=pHandle->pSPIx->CR2 & (1<<SPI_CR2_TXEIE);
	if(temp1&&temp2)
	{
		//handle txe
		spi_txe_interrupt_handle(pHandle);
	}

	//second checking for rxne
	temp1=pHandle->pSPIx->SR & (1<<SPI_SR_RXNE);
	temp2=pHandle->pSPIx->CR2 & (1<<SPI_CR2_RXNEIE);
	if(temp1&&temp2)
	{
		//handle rxne
		spi_rxne_interrupt_handle(pHandle);
	}

	//third checking for over
	temp1=pHandle->pSPIx->SR & (1<<SPI_SR_OVR);
	temp2=pHandle->pSPIx->CR2 & (1<<SPI_CR2_ERRIE);
	if(temp1&&temp2)
	{
		//handle rxne
		spi_ovr_err_interrupt_handle(pHandle);
	}
}

/*******************************************************************
 * @fn               - SPI_SSIConfig
 *
 * @brief            - this func manages the SSI bit in the cr1 reg
 *
 * @param[in]        - base addr of peripheral
 * @param[in]        - enordi macro
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		pSPIx->CR1|=(1<<SPI_CR1_SSI);
	}
	else
	{
		pSPIx->CR1&=~(1<<SPI_CR1_SSI);
	}
}

/*******************************************************************
 * @fn               - SPI_SSOEConfig
 *
 * @brief            - this func manages the SSOE bit in the cr2 reg
 *
 * @param[in]        - base addr of peripheral
 * @param[in]        - enordi macro
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		pSPIx->CR2|=(1<<SPI_CR2_SSOE);
	}
	else
	{
		pSPIx->CR2&=~(1<<SPI_CR2_SSOE);
	}
}

//some helper function implementations

static void spi_txe_interrupt_handle(SPI_Handle_t *pHandle)
{
	// check the dff bit in cr1
	if( (pHandle->pSPIx->CR1) & (1<<SPI_CR1_DFF) )
	{
		//16 bit dff
		//1. load the data into the data register (DR)
		pHandle->pSPIx->DR=*((uint16_t*)(pHandle->pTxBuffer));
		pHandle->TxLen--;
		pHandle->TxLen--; //using len-- 2 times because the data is 16 bits and it is equal to 2 bytes
		pHandle->pTxBuffer+=2;
	}
	else
	{
		//8 bit data
		pHandle->pSPIx->DR=*((uint8_t*)(pHandle->pTxBuffer));
		pHandle->TxLen--; //using len-- 2 times because the data is 16 bits and it is equal to 2 bytes
		(pHandle->pTxBuffer)++;
	}
	if(!(pHandle->TxLen))
	{
		//tx len is zero then close the spi transmission and inform the application that tx is over
		//tx over
		//prevents interrupts from setting up the tx flag
		SPI_CloseTransmission(pHandle);
		SPI_ApplicationEventCallback(pHandle,SPI_EVENT_TX_CMPLT);
	}
}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pHandle)
{
	if( (pHandle->pSPIx->CR1) & (1<<SPI_CR1_DFF) )
	{
		//16 bit dff
		//1. load the data into the data register (DR)
		*((uint16_t*)(pHandle->pRxBuffer))=pHandle->pSPIx->DR;
		pHandle->RxLen--;
		pHandle->RxLen--; //using len-- 2 times because the data is 16 bits and it is equal to 2 bytes
	    (pHandle->pRxBuffer)+=2;
	}
	else
	{
		//8 bit data
		*((uint8_t*)(pHandle->pRxBuffer))=pHandle->pSPIx->DR;
		pHandle->RxLen--;
		(pHandle->pRxBuffer)++;
	}
	if(!(pHandle->RxLen))
	{
		//rx over
		//prevents interrupts from setting up the rxneie flag
		SPI_CloseReception(pHandle);
		SPI_ApplicationEventCallback(pHandle,SPI_EVENT_RX_CMPLT);
	}
}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pHandle)
{
	uint8_t temp;
	//1. clr the ovr flag
	if(pHandle->TxState!=SPI_BUSY_IN_TX)
	{
		temp=pHandle->pSPIx->DR;
		temp=pHandle->pSPIx->SR;
	}
	(void)temp;
	//2. inform the application
	SPI_ApplicationEventCallback(pHandle,SPI_EVENT_OVR_ERR);
}

void SPI_CloseTransmission(SPI_Handle_t *pHandle)
{
	pHandle->pSPIx->CR2&=~(1<<SPI_CR2_TXEIE);
	pHandle->pTxBuffer=NULL;
	pHandle->TxLen=0;
	pHandle->TxState=SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pHandle)
{
	pHandle->pSPIx->CR2&=~(1<<SPI_CR2_RXNEIE);
	pHandle->pRxBuffer=NULL;
	pHandle->RxLen=0;
	pHandle->RxState=SPI_READY;
}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp=pSPIx->DR;
	temp=pSPIx->SR;
	(void)temp;
}

__attribute__((weak)) void SPI_ApplicationEventCallback(SPI_Handle_t *pHandle,uint8_t AppEv)
{
	//this is a weak implementation of this function and the user may override it
}


