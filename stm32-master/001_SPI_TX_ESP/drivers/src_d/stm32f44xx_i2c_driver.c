/*
 * stm32f44xx_i2c_driver.c
 *
 *  Created on: 30-Jan-2026
 *      Author: nachiket
 */

#include "stm32f44xx_i2c_driver.h"

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr)
{
	SlaveAddr=SlaveAddr<<1;
	SlaveAddr&=~(1<<0); //slave adddr is slave addr + r/nw bit (0 for write)
	pI2Cx->DR=SlaveAddr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx,uint8_t SlaveAddr)
{
	SlaveAddr=SlaveAddr<<1;
	SlaveAddr|=(1<<0); //slave adddr is slave addr + r/nw bit (1 for read)
	pI2Cx->DR=SlaveAddr;
}

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1|=(1<<I2C_CR1_START);
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;
	//check for device mode
	if(pI2CHandle->pI2Cx->SR2 & (1<<I2C_SR2_MSL)) //device is in master mode
	{
		if(pI2CHandle->TxRxState==I2C_BUSY_IN_TX)
		{
			if(pI2CHandle->RxSize==1)
			{
				//1. disbale acking
				I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

				//clr the addr flag (read sr1 and then sr2)
				dummy_read=pI2CHandle->pI2Cx->SR1;
				dummy_read=pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}
		}
		else
		{
			//clr the addr flag (read sr1 and then sr2)
			dummy_read=pI2CHandle->pI2Cx->SR1;
			dummy_read=pI2CHandle->pI2Cx->SR2;
			(void)dummy_read;
		}
	}
	else if(pI2CHandle->pI2Cx->SR2 & (1<<I2C_SR2_MSL)) //device is in slave mode
	{
		//clr the addr flag (read sr1 and then sr2)
		dummy_read=pI2CHandle->pI2Cx->SR1;
		dummy_read=pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;
	}
}

static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1|=(1<<I2C_CR1_STOP);
}

void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnOrDi)
{
	if(EnOrDi==ENABLE)
	{
		pI2Cx->CR2|=(1<<I2C_CR2_ITEVTEN);
		pI2Cx->CR2|=(1<<I2C_CR2_ITERREN);
		pI2Cx->CR2|=(1<<I2C_CR2_ITBUFEN);
	}
	else
	{
		pI2Cx->CR2&=~(1<<I2C_CR2_ITEVTEN);
		pI2Cx->CR2&=~(1<<I2C_CR2_ITERREN);
		pI2Cx->CR2&=~(1<<I2C_CR2_ITBUFEN);
	}
}

/*******************************************************************
 * @fn               - I2C_PeriClkCtrl
 *
 * @brief            - this func enables the peripheral clk control for i2c
 *
 * @param[in]        - base addr of the i2c peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void I2C_PeriClkCtrl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		if(pI2Cx==I2C1)
		{
			I2C1_PCLK_EN();
		}
		else if(pI2Cx==I2C2)
		{
			I2C2_PCLK_EN();
		}
		else if(pI2Cx==I2C3)
		{
			I2C3_PCLK_EN();
		}

	}
	else
	{
		if(pI2Cx==I2C1)
		{
			I2C1_PCLK_DI();
		}
		else if(pI2Cx==I2C2)
		{
			I2C2_PCLK_DI();
		}
		else if(pI2Cx==I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}

/*******************************************************************
 * @fn               - I2C_Init
 *
 * @brief            - this func sets various parameters of a i2c pin
 *
 * @param[in]        - base addr of the i2c peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	//enabling the peripheral clock
	I2C_PeriClkCtrl(pI2CHandle->pI2Cx,ENABLE);

	uint32_t tempreg=0;
	//ack control bit
	tempreg=pI2CHandle->I2CHandle.I2C_ACKControl<<I2C_CR1_ACK;
	pI2CHandle->pI2Cx->CR1=tempreg;

	//configure the freq field of cr2
	tempreg=0;
	tempreg|=(RCC_GetPCLK1Value()/1000000U);
	pI2CHandle->pI2Cx->CR2=(tempreg&0x3F);

	//program the device own address
	tempreg=0;
	tempreg|=pI2CHandle->I2CHandle.I2C_DeviceAddress<<1;
	tempreg|=1<<14;
	pI2CHandle->pI2Cx->OAR1=tempreg;

	//CCR calculations
	uint16_t ccr_value=0;
	tempreg=0;
	if(pI2CHandle->I2CHandle.I2C_SCLSpeed<=I2C_SCL_SPEED_SM)
	{
		//MODE IS STANDARD
		ccr_value=(RCC_GetPCLK1Value()/(2*pI2CHandle->I2CHandle.I2C_SCLSpeed));
		tempreg|=(ccr_value&0xFFF);
	}
	else
	{
		//mode is fast
		tempreg|=(1<<15);
		tempreg|=(pI2CHandle->I2CHandle.I2C_FMDutyCycle<<14);
		if(pI2CHandle->I2CHandle.I2C_FMDutyCycle==I2C_FM_DUTY_2)
		{
			ccr_value=(RCC_GetPCLK1Value()/(3*pI2CHandle->I2CHandle.I2C_SCLSpeed));
		}
		else
		{
			ccr_value=(RCC_GetPCLK1Value()/(25*pI2CHandle->I2CHandle.I2C_SCLSpeed));
		}
		tempreg|=(ccr_value&0xFFF);
	}
	pI2CHandle->pI2Cx->CCR=tempreg;
	tempreg=0;

	//TRISE config
	if(pI2CHandle->I2CHandle.I2C_SCLSpeed<=I2C_SCL_SPEED_SM)
	{
		//MODE IS STANDARD
		tempreg=((RCC_GetPCLK1Value()/1000000U)+1);
	}
	else
	{
		//mode is fast
		tempreg=(((RCC_GetPCLK1Value()/1000000U)*300)/1000U)+1;
	}
	pI2CHandle->pI2Cx->TRISE=(tempreg&0x3F);
}

/*******************************************************************
 * @fn               - I2C_MasterSendData
 *
 * @brief            - this func sends data
 *
 * @param[in]        - base addr of the I2C peripheral
 * @param[in]        - tx buffer addr (ptr to the data)
 * @param[in]        - length of the data to be sent
 * @param[in]        - address of the slave
 * @param[in]        - repeated start condition bit
 *
 * @return           - none
 *
 * @Note             - this is a blocking call using while loops (polling type api function)
 *
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	//1. generate the start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the SB flag in the sr1
	//note: until sb (start bit) is cleared scl will be stretched (pulled to low)
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB)));

	//3. send the addr of the slave with the r/nw bit set to w(0) (total 8 bits)
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx,SlaveAddr);

	//4. confirm the addr phase is completed by checking the addr flag in the sr1 register
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR)));

	//5. Clear the addr flag according to its software sequence (pulled to low)
	//note: until addr is cleared scl will be stretched (pulled to low)
	I2C_ClearADDRFlag(pI2CHandle);

	//6. send data until len is 0
	while(len>0)
	{
		while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_TXE)));//WAIT UNTIL TXE IS SET
		pI2CHandle->pI2Cx->DR=*pTxbuffer;
		pTxbuffer++;
		len--;
	}

	//7. when len becomes 0 wait for txe=1 and btf=1 before generating the stop condition
	//note: txe=1, btf=1 means that both sr and dr are empty and next transmission should begin
	//when btf=1 scl will be stetched (pulled to low)
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_TXE)));
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_BTF)));

	//8. generate stop condition and master need nt wait for the completion of the stop condition
	//note: generation STOP, automatically clears btf bit
	if(Sr==I2C_DISABLE_SR)
	{
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}
}

/*******************************************************************
 * @fn               - I2C_MasterReceiveData
 *
 * @brief            - this func receives data
 *
 * @param[in]        - base addr of the I2C peripheral
 * @param[in]        - rx buffer addr
 * @param[in]        - length of the data to be received
 * @param[in]        - address of the slave
 * @param[in]        - repeated start condition bit
 *
 * @return           - none
 *
 * @Note             - this is a blocking call using while loops (polling type api function)
 *
 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	//1. generate the start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start geneartion is completed be checking the SB flag in the SR1
	// note: until SB is cleared SCL will be stretched (pulled to low)
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB)));

	//3. send the addr of the slave with r/nw bit set to R(1) (total 8 bits)
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx,SlaveAddr);

	//4. wait until addr phase is completed by checking the addr flag in SR1
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR)));

	//procedure to read only 1 byte from slave
	if(len==1)
	{
		//disable acking
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

		//clr the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//wait until RXNE becomes 1
		while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_RXNE)));

		//generate STOP condition
		if(Sr==I2C_DISABLE_SR)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		//read data in to buffer
		*pRxbuffer=pI2CHandle->pI2Cx->DR;
	}

	//procedure to read more than 1 byte from slave
	if(len>1)
	{
		//clr the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);
		//read data until len becomes 0
		for(uint32_t i=len;i>0;i--)
		{
			//wait until RXNE becomes 1
			while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_RXNE)));

			if(i==2) //if last 2 bytes are remaining
			{
				//clr the ACK bit (disable acking)
				I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);

				//generate STOP condition
				if(Sr==I2C_DISABLE_SR)
				{
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
				}
			}

			//read data from DR (data register) in to buffer
			*pRxbuffer=pI2CHandle->pI2Cx->DR;

			//increment the buffer addr
			pRxbuffer++;
		}
	}
	//re-enable acking
	if(pI2CHandle->I2CHandle.I2C_ACKControl==I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
	}
}


uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx,uint32_t FlagName)
{
	if(pI2Cx->SR1&FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*
 * deinit
 */
/*******************************************************************
 * @fn               - I2C_DeInit
 *
 * @brief            - this func resets various parameters of an I2C pin and disbales the peripheral clock
 *
 * @param[in]        - base addr of the I2C peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if(pI2Cx==I2C1)
	{
		I2C1_REG_RESET();
		I2C1_PCLK_DI();
	}
	else if(pI2Cx==I2C2)
	{
		I2C2_REG_RESET();
		I2C2_PCLK_DI();
	}
	else if(pI2Cx==I2C3)
	{
		I2C3_REG_RESET();
		I2C3_PCLK_DI();
	}
}
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi==I2C_ACK_ENABLE)
	{
		pI2Cx->CR1|=(1<<I2C_CR1_ACK); //en ack
	}
	else
	{
		pI2Cx->CR1&=~(1<<I2C_CR1_ACK);//di ack
	}
}

/*******************************************************************
 * @fn               - I2C_MasterSendDataIT
 *
 * @brief            - this func sends data
 *
 * @param[in]        - base addr of the I2C peripheral
 * @param[in]        - tx buffer addr (ptr to the data)
 * @param[in]        - length of the data to be sent
 * @param[in]        - address of the slave
 * @param[in]        - repeated start condition bit
 *
 * @return           - none
 *
 * @Note             - this is NOT a blocking call instead uses interrupts
 *
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	uint8_t busystate=pI2CHandle->TxRxState;

	if((busystate!=I2C_BUSY_IN_TX)&&(busystate!=I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer=pTxbuffer;
		pI2CHandle->TxLen=len;
		pI2CHandle->TxRxState=I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr=SlaveAddr;
		pI2CHandle->Sr=Sr;

		//implement code to generate START condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//implement the code to enable ITBUFEN control bit
		pI2CHandle->pI2Cx->CR2|=(1<<I2C_CR2_ITBUFEN);

		//implement the code to enable ITEVTEN control bit
		pI2CHandle->pI2Cx->CR2|=(1<<I2C_CR2_ITEVTEN);

		//implement the code to enable ITERREN control bit
		pI2CHandle->pI2Cx->CR2|=(1<<I2C_CR2_ITERREN);
	}
	return busystate;
}


/*******************************************************************
 * @fn               - I2C_MasterReceiveDataIT
 *
 * @brief            - this func sends data
 *
 * @param[in]        - base addr of the I2C peripheral
 * @param[in]        - tx buffer addr (ptr to the data)
 * @param[in]        - length of the data to be sent
 * @param[in]        - address of the slave
 * @param[in]        - repeated start condition bit
 *
 * @return           - none
 *
 * @Note             - this is NOT a blocking call instead uses interrupts
 *
 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr)
{
	uint8_t busystate=pI2CHandle->TxRxState;

	if((busystate!=I2C_BUSY_IN_TX)&&(busystate!=I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer=pRxbuffer;
		pI2CHandle->RxLen=len;
		pI2CHandle->TxRxState=I2C_BUSY_IN_RX;
		pI2CHandle->RxSize=len;   //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr=SlaveAddr;
		pI2CHandle->Sr=Sr;

		//implement code to generate START condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//implement the code to enable ITBUFEN control bit
		pI2CHandle->pI2Cx->CR2|=(1<<I2C_CR2_ITBUFEN);

		//implement the code to enable ITEVTEN control bit
		pI2CHandle->pI2Cx->CR2|=(1<<I2C_CR2_ITEVTEN);

		//implement the code to enable ITERREN control bit
		pI2CHandle->pI2Cx->CR2|=(1<<I2C_CR2_ITERREN);
	}
	return busystate;
}

/*******************************************************************
 * @fn               - I2C_IRQInterruptConfig
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
void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi)
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
 * @fn               - I2C_IRQPriorityConfig
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority)
{
	//1. finding ipr register
	uint8_t iprx=IRQNumber/4;
	uint8_t iprx_section=IRQNumber%4;
	uint8_t shift_amt=(iprx_section*8)+(8-N0_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR+iprx)|=(IRQPriority<<(shift_amt));
}

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	//implement the code to disable ITBUFEN Control bit
	pI2CHandle->pI2Cx->CR2&=~(1<<I2C_CR2_ITBUFEN);

	//implement the code to disable ITVTEN Control bit
	pI2CHandle->pI2Cx->CR2&=~(1<<I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState=I2C_READY;
	pI2CHandle->pRxBuffer=NULL;
	pI2CHandle->RxLen=0;
	pI2CHandle->RxSize=0;
	if(pI2CHandle->I2CHandle.I2C_ACKControl==I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
	}
}

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//implement the code to disable ITBUFEN Control bit
	pI2CHandle->pI2Cx->CR2&=~(1<<I2C_CR2_ITBUFEN);

	//implement the code to disable ITVTEN Control bit
	pI2CHandle->pI2Cx->CR2&=~(1<<I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState=I2C_READY;
	pI2CHandle->pTxBuffer=NULL;
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxLen>0)
	{
		//1. load the data in to dr
		pI2CHandle->pI2Cx->DR=*(pI2CHandle->pTxBuffer);

		//2, decment the txlen
		pI2CHandle->TxLen--;

		//3, increment the buffer address
		pI2CHandle->pTxBuffer++;
	}
}

void I2C_SlaveSendData(I2C_RegDef_t *pI2C, uint8_t data)
{
	pI2C->DR=data;
}

uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C)
{
	return (uint8_t)pI2C->DR;
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->RxSize==1)
		{
			//1. load data from dr into rx buffer
			(*pI2CHandle->pRxBuffer)=(pI2CHandle->pI2Cx->DR);

			pI2CHandle->RxLen--;
		}
		if(pI2CHandle->RxSize>1)
		{
			if(pI2CHandle->RxLen==2)
			{
				//clr the ack bit
				I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);
			}
			//read dr
			*pI2CHandle->pRxBuffer=pI2CHandle->pI2Cx->DR;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->RxLen--;
		}
		if(pI2CHandle->RxLen==0)
		{
			//close the i2c data reception and notify the aplication
			//1. generate the stop condition
			if(pI2CHandle->Sr==I2C_DISABLE_SR)
			{
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}

			//2. close i2c rx
			I2C_CloseReceiveData(pI2CHandle);

			//3, notify the application
			I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_RX_CMPLT);
		}
}

/*******************************************************************
 * @fn               - I2C_EV_IRQHandling
 *
 * @brief            - this func handles irq for events in i2c
 *
 * @param[in]        - base address of i2c peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//interrupt handling for both master and slave mode of a device

	uint32_t temp1,temp2,temp3;

	temp1=pI2CHandle->pI2Cx->CR2 & (1<<I2C_CR2_ITEVTEN);
	temp2=pI2CHandle->pI2Cx->CR2 & (1<<I2C_CR2_ITBUFEN);

	temp3=pI2CHandle->pI2Cx->SR1 & (1<<I2C_SR1_SB);
	//1. handle for interrupt generated by SB event
	//note:sb flag is only applicable in master mode
	if(temp1&&temp3)
	{
		//SB flag is set
		//this block will not be executed in slave mode because for slave SB is always zero
		//in this block the address phase is executed
		if(pI2CHandle->TxRxState==I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx,pI2CHandle->DevAddr);
		}
		else if(pI2CHandle->TxRxState==I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx,pI2CHandle->DevAddr);
		}
	}

	temp3=pI2CHandle->pI2Cx->SR1 & (1<<I2C_SR1_ADDR);
	//2. handle for interrupt generated by ADDR event
	//note: when master mode:address is sent
	//note: when slave mode:address matched with own address
	if(temp1&&temp3)
	{
		//ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle);
	}

	temp3=pI2CHandle->pI2Cx->SR1 & (1<<I2C_SR1_BTF);
	//3. handle for interrupt generated by BTF event
	if(temp1&&temp3)
	{
		//BTF flag is set
		if(pI2CHandle->TxRxState==I2C_BUSY_IN_TX)
		{
			//make sure that txe is also set
			if(pI2CHandle->pI2Cx->SR1 & (1<<I2C_SR1_TxE))
			{
				//BTF,TXE=1
				if(pI2CHandle->TxLen==0)
				{
					//1. generate the stop condition
					if(pI2CHandle->Sr==I2C_DISABLE_SR)
					{
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}

					//2. reset all the member elements of the handle structure
					I2C_CloseSendData(pI2CHandle);

					//3. notify the application
					I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_TX_CMPLT);
				}
			}
		}
		else if(pI2CHandle->TxRxState==I2C_BUSY_IN_RX)
		{
			;
		}
	}

	temp3=pI2CHandle->pI2Cx->SR1 & (1<<I2C_SR1_STOPF);
	//4. handle for interrupt generated by STOPF event
	//note:stopf flag is only applicable in slave mode as it wont be set in master mode
	//the below code wont be executed by master as stopf is not set in master
	if(temp1&&temp3)
	{
		//STOPF flag is set
		//clr the stopf flag by reading the sr1 then writing to cr1
		pI2CHandle->pI2Cx->CR1|=0x0000;

		//notify the application that STOP is detected
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_STOP);
	}

	temp3=pI2CHandle->pI2Cx->SR1 & (1<<I2C_SR1_TxE);
	//5. handle for interrupt generated by TxE event
	if(temp1&&temp2&&temp3)
	{
		//Txe flag is set
		//check device mode
		if(pI2CHandle->pI2Cx->SR2 & (1<<I2C_SR2_MSL))
		{
			if(pI2CHandle->TxRxState==I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}
		else
		{
			//slave
			//make sure the slave is really in transmit mode
			if((pI2CHandle->pI2Cx->SR2) & (1<<I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_DATA_REQ);
			}
		}
	}

	temp3=pI2CHandle->pI2Cx->SR1 & (1<<I2C_SR1_RxNE);
	//6. handle for interrupt generated by RxNE event
	if(temp1&&temp2&&temp3)
	{
		if(pI2CHandle->pI2Cx->SR2&(1<<I2C_SR2_MSL))
		{
			//Rxne flag is set
			if(pI2CHandle->TxRxState==I2C_BUSY_IN_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}
		else
		{
			//slave
			//make sure the slave is really in rx mode
			if(!((pI2CHandle->pI2Cx->SR2) & (1<<I2C_SR2_TRA)))
			{
				I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_DATA_RCV);
			}
		}
	}
}

/*******************************************************************
 * @fn               - I2C_ER_IRQHandling
 *
 * @brief            - this func handles irq for erros in i2c
 *
 * @param[in]        - base address of i2c peripheral
 * @param[in]        -
 * @param[in]        -
 *
 * @return           - none
 *
 * @Note             - none
 *
 */
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);

/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error
		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{

		//This is arbitration lost error
		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1&=~(1<<I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);
	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error
		//Implement the code to clear the ack failiure error flag
		pI2CHandle->pI2Cx->SR1&=~(1<<I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun
		//Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1&=~(1<<I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & (1<<I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error
	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1&=~(1<<I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}
}

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		pI2Cx->CR1|=(1<<I2C_CR1_PE);
	}
	else if(EnorDi==DISABLE)
	{
		pI2Cx->CR1&=~(1<<I2C_CR1_PE);
	}
}







