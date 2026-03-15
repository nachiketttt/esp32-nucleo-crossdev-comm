/*
 * stm32f44xx_i2c_driver.h
 *
 *  Created on: 30-Jan-2026
 *      Author: nachiket
 */

#ifndef INC_D_STM32F44XX_I2C_DRIVER_H_
#define INC_D_STM32F44XX_I2C_DRIVER_H_

#include "stm32f446rexx.h"

/*
 * config struct for i2cx peripherals
 */
typedef struct
{
	uint32_t I2C_SCLSpeed;
	uint8_t I2C_DeviceAddress;
	uint8_t I2C_ACKControl;
	uint16_t I2C_FMDutyCycle;
}I2C_Config_t;

/*
 * handle struct for i2cx peripherals
 */
typedef struct
{
	I2C_RegDef_t *pI2Cx; //holds value of base addr of i2cx (x:1,2,3) peripherals
	I2C_Config_t I2CHandle;
	uint8_t *pTxBuffer; //to store the application tx buffer address
	uint8_t *pRxBuffer; //to store the application rx buffer address
	uint32_t TxLen;     //to store the tx length
	uint32_t RxLen;     //to store the rx length
	uint8_t  TxRxState; //to store the communication state
	uint8_t  DevAddr;   //to store the slave/device address
	uint8_t  RxSize;    //to store the rx size
	uint8_t  Sr;        //to store the repeated start value
}I2C_Handle_t;

/*
 * @I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_SM        100000  //STD MODE
#define I2C_SCL_SPEED_FM4K      400000  //FAST MODE 2KHZ
#define I2C_SCL_SPEED_FM2K      200000

/*
 * @I2C_ACKControl
 */
#define I2C_ACK_ENABLE          1
#define I2C_ACK_DISABLE         0

/*
 * @I2C_FMDutyCycle
 */
#define I2C_FM_DUTY_2           0
#define I2C_FM_DUTY_16_9        1

/*
 * I2C related status flags definitions
 */
#define I2C_FLAG_TXE            (1<<I2C_SR1_TxE)
#define I2C_FLAG_RXNE           (1<<I2C_SR1_RxNE)
#define I2C_FLAG_SB             (1<<I2C_SR1_SB)
#define I2C_FLAG_BTF            (1<<I2C_SR1_BTF)
#define I2C_FLAG_ADDR           (1<<I2C_SR1_ADDR)
#define I2C_FLAG_STOPF          (1<<I2C_SR1_STOPF)
#define I2C_FLAG_BERR           (1<<I2C_SR1_BERR)
#define I2C_FLAG_ARLO           (1<<I2C_SR1_ARLO)
#define I2C_FLAG_AF             (1<<I2C_SR1_AF)
#define I2C_FLAG_OVR            (1<<I2C_SR1_OVR)
#define I2C_FLAG_TIMEOUT        (1<<I2C_SR1_TIMEOUT)

#define I2C_DISABLE_SR          RESET
#define I2C_ENABLE_SR           SET

/*
 * I2C application states
 */
#define I2C_READY               0
#define I2C_BUSY_IN_RX          1
#define I2C_BUSY_IN_TX          2

/*
 * I2C application events macros
 */
#define I2C_EV_TX_CMPLT         0
#define I2C_EV_RX_CMPLT         1
#define I2C_EV_STOP             2

#define I2C_ERROR_BERR          3
#define I2C_ERROR_ARLO          4
#define I2C_ERROR_AF            5
#define I2C_ERROR_OVR           6
#define I2C_ERROR_TIMEOUT       7

#define I2C_EV_DATA_REQ         8
#define I2C_EV_DATA_RCV         9

/**************************************************************************************
 *                       APIs supported by this driver
 *      for more info about the APIs check the function definitions
 **************************************************************************************/
/*
 *peripheral clk setup
 */
void I2C_PeriClkCtrl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi);

/*
 *init deinit
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);

/*
 * Data Send and Receive
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr);

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxbuffer,uint32_t len,uint8_t SlaveAddr,uint8_t Sr);

void I2C_SlaveSendData(I2C_RegDef_t *pI2C, uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C);


void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
/*
 *IRQ config and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority);
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);
/*
 * other peripheral control apis
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx,uint8_t EnorDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx,uint32_t FlagName);
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx,uint8_t EnorDi);
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,uint8_t EnOrDi);

/*
 * Application call back
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pHandle,uint8_t AppEv);
#endif /* INC_D_STM32F44XX_I2C_DRIVER_H_ */
