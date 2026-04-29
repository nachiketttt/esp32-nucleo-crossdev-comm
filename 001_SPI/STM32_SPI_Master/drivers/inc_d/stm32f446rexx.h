/*
 * stm32f44xx.h
 *
 *  Created on: 15-Jan-2026
 *      Author: nachiket
 */

#ifndef INC_D_STM32F44XX_H_
#define INC_D_STM32F44XX_H_

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
/*
 * ******************Processor specific macros (arm m4)***************************
 */
//arm cortex m4 processor nvic iserx reg addresses
#define NVIC_ISER0           ((volatile uint32_t*)0xE000E100)
#define NVIC_ISER1           ((volatile uint32_t*)0xE000E104)
#define NVIC_ISER2           ((volatile uint32_t*)0xE000E108)
#define NVIC_ISER3           ((volatile uint32_t*)0xE000E10C)

//arm cortex m4 processor nvic iserx reg addresses
#define NVIC_ICER0           ((volatile uint32_t*)0XE000E180)
#define NVIC_ICER1           ((volatile uint32_t*)0XE000E184)
#define NVIC_ICER2           ((volatile uint32_t*)0XE000E188)
#define NVIC_ICER3           ((volatile uint32_t*)0XE000E18C)

//arm cortex m4 processor priority reg addresses
#define NVIC_PR_BASE_ADDR    ((volatile uint32_t*)0xE000E400)
#define N0_PR_BITS_IMPLEMENTED              4

//base addresses of sram and flash memories
#define FLASH_BASEADDR       0x08000000U
#define SRAM1_BASEADDR       0x20000000U
#define SRAM                 SRAM1_BASEADDR
#define SRAM2_BASEADDR       0x2001C000U
#define ROM                  0x1FFF0000U

//AHBX AND APBX Peripheral base addresses
#define PERIPH_BASEADDR      0x40000000U
#define APB1PERIPH_BASEADDR  PERIPH_BASEADDR
#define APB2PERIPH_BASEADDR  0x40010000U
#define AHB1PERIPH_BASEADDR  0x40020000U
#define AHB2PERIPH_BASEADDR  0x50000000U


//Base addresses of peripherals which are hanging on AHB1 bus
#define GPIOA_BASEADDR       (AHB1PERIPH_BASEADDR+0x0000)
#define GPIOB_BASEADDR       (AHB1PERIPH_BASEADDR+0x0400)
#define GPIOC_BASEADDR       (AHB1PERIPH_BASEADDR+0x0800)
#define GPIOD_BASEADDR       (AHB1PERIPH_BASEADDR+0x0C00)
#define GPIOE_BASEADDR       (AHB1PERIPH_BASEADDR+0x1000)
#define GPIOF_BASEADDR       (AHB1PERIPH_BASEADDR+0x1400)
#define GPIOG_BASEADDR       (AHB1PERIPH_BASEADDR+0x1800)
#define GPIOH_BASEADDR       (AHB1PERIPH_BASEADDR+0x1C00)

#define RCC_BASEADDR         (AHB1PERIPH_BASEADDR+0x3800)


//Base addresses of peripherals which are hanging on APB1 bus
#define I2C1_BASEADDR        (APB1PERIPH_BASEADDR+0x5400)
#define I2C2_BASEADDR        (APB1PERIPH_BASEADDR+0x5800)
#define I2C3_BASEADDR        (APB1PERIPH_BASEADDR+0x5C00)

#define SPI2_BASEADDR        (APB1PERIPH_BASEADDR+0x3800)
#define SPI3_BASEADDR        (APB1PERIPH_BASEADDR+0x3C00)

#define USART2_BASEADDR      (APB1PERIPH_BASEADDR+0x4400)
#define USART3_BASEADDR      (APB1PERIPH_BASEADDR+0x4800)
#define UART4_BASEADDR       (APB1PERIPH_BASEADDR+0x4C00)
#define UART5_BASEADDR       (APB1PERIPH_BASEADDR+0x5000)

//Base addresses of peripherals which are hanging on APB2 bus
#define EXTI_BASEADDR        (APB2PERIPH_BASEADDR+0x3C00)
#define USART1_BASEADDR      (APB2PERIPH_BASEADDR+0x1000)
#define USART6_BASEADDR      (APB2PERIPH_BASEADDR+0x1400)
#define SPI1_BASEADDR        (APB2PERIPH_BASEADDR+0x3000)
#define SPI4_BASEADDR        (APB2PERIPH_BASEADDR+0x3400)
#define SYSCFG_BASEADDR      (APB2PERIPH_BASEADDR+0x3800)


/**********************peripheral register definition structures**************/
typedef struct
{
	volatile uint32_t MODER;      /*GPIO port mode register (offset:0x00)*/
	volatile uint32_t OTYPER;     /*GPIO port output type register (offset:0x04)*/
	volatile uint32_t OSPEEDER;   /*GPIO port output speed register (offset:0x08)*/
	volatile uint32_t PUPDR;      /*GPIO port pull up/pull down register (offset:0x0C)*/
	volatile uint32_t IDR;        /*GPIO port input data register (offset:0x10)*/
	volatile uint32_t ODR;        /*GPIO port output data register (offset:0x14)*/
	volatile uint32_t BSRR;       /*GPIO port bit set/reset register (offset:0x18)*/
	volatile uint32_t LCKR;       /*GPIO port config lock register (offset:0x1C)*/
	volatile uint32_t AFR[2];     /*GPIO port alt function low register (offset:0x20) GPIO port alt function high register (offset:0x24)*/
	//afr0 is low register and afr1 is high
}GPIO_RegDef_t;


typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	volatile uint32_t RESERVED0;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t RESERVED1[2];
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	volatile uint32_t RESERVED2;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t RESERVED3[2];
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t AHB3LPENR;
	volatile uint32_t RESERVED4;
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	volatile uint32_t RESERVED5[2];
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t RESERVED6[2];
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	volatile uint32_t PLLSAICFGR;
	volatile uint32_t DCKCFGR;
	volatile uint32_t CKGATENR;
	volatile uint32_t DCKCFGR2;
}RCC_RegDef_t;

/*
 * peripheral defination structure for exti
 */
typedef struct
{
	volatile uint32_t IMR;  //Interrupt mask register OFFSET:0x00
	volatile uint32_t EMR;  //event mask register OFFSET:0x04
	volatile uint32_t RTSR; //Rising trigger selection register OFFSET:0x08
	volatile uint32_t FTSR; //Falling trigger selection register OFFSET:0x0C
	volatile uint32_t SWIER;//Software interrupt event register OFFSET:0x10
	volatile uint32_t PR;   //pending register OFFSET:0x14
}EXTI_RegDef_t;

/*
 * peripheral defination structure for syscfg
 */
typedef struct
{
	volatile uint32_t MEMRMP;     //SYSCFG memory remap register OFFSET:0x00
	volatile uint32_t PMC;        //SYSCFG peripheral mode configuration register OFFSET:0x04
	/*volatile uint32_t EXTICR1;  //SYSCFG external interrupt configuration register 1 OFFSET:0x08
	volatile uint32_t EXTICR2;    //SYSCFG external interrupt configuration register 2 OFFSET:0x0C
	volatile uint32_t EXTICR3;    //SYSCFG external interrupt configuration register 3 OFFSET:0x10
	volatile uint32_t EXTICR4;    //SYSCFG external interrupt configuration register 4 OFFSET:0x14*/
	volatile uint32_t EXTICR[4];
	uint32_t RESERVED1[2];
	volatile uint32_t CMPCR;      //Compensation cell control register OFFSET:0x20
	uint32_t RESERVED2[2];
	volatile uint32_t CFGR;       //SYSCFG configuration register OFFSET:0x2C
}SYSCFG_RegDef_t;

/*
 * peripheral defination structure for spi
 */
typedef struct
{
	volatile uint32_t CR1;    //SPI control register 1 (SPI_CR1) (not used in I2S mode)  Address offset: 0x00
	volatile uint32_t CR2;    //SPI control register 2 (SPI_CR2)                         Address offset: 0x04
	volatile uint32_t SR;     //SPI status register (SPI_SR)                             Address offset: 0x08
	volatile uint32_t DR;     //SPI data register (SPI_DR)                               Address offset: 0x0C
	volatile uint32_t CRCPR;  //SPI CRC polynomial register (SPI_CRCPR) (not used in I2S mode)  Address offset: 0x10
	volatile uint32_t RXCRCR; //SPI RX CRC register (SPI_RXCRCR) (not used in I2S mode)  Address offset: 0x14
	volatile uint32_t TXCRCR; //SPI TX CRC register (SPI_TXCRCR) (not used in I2S mode)  Address offset: 0x18
	volatile uint32_t I2SCFGR;//SPI_I2S configuration register (SPI_I2SCFGR)             Address offset: 0x1C
	volatile uint32_t I2SPR;  //SPI_I2S prescaler register (SPI_I2SPR)                   Address offset: 0x20
}SPI_RegDef_t;

/*
 * peripheral defination structure for i2c
 */
typedef struct
{
	volatile uint32_t CR1;    //I2C control register 1 (I2C_CR1) Address offset: 0x00
	volatile uint32_t CR2;    //I2C control register 2 (I2C_CR2) Address offset: 0x04
	volatile uint32_t OAR1;   //I2C own address register 1 (I2C_OAR1) Address offset: 0x08
	volatile uint32_t OAR2;   //I2C own address register 2 (I2C_OAR2) Address offset: 0x0C
	volatile uint32_t DR;     //I2C data register (I2C_DR) Address offset: 0x10
	volatile uint32_t SR1;    //I2C status register 1 (I2C_SR1) Address offset: 0x14
	volatile uint32_t SR2;    //I2C status register 2 (I2C_SR2) Address offset: 0x18
	volatile uint32_t CCR;    //I2C clock control register (I2C_CCR) Address offset: 0x1C
	volatile uint32_t TRISE;  //I2C TRISE register (I2C_TRISE) Address offset: 0x20
	volatile uint32_t FLTR;   //I2C FLTR register (I2C_FLTR) Address offset: 0x24
}I2C_RegDef_t;

/*
 * peripheral defination structure for usart
 */
typedef struct
{
	volatile uint32_t SR;     //Status register (USART_SR) Address offset: 0x00
	volatile uint32_t DR;     //Data register (USART_DR) Address offset: 0x04
	volatile uint32_t BRR;    //Baud rate register (USART_BRR) Address offset: 0x08
	volatile uint32_t CR1;    //Control register 1 (USART_CR1) Address offset: 0x0C
	volatile uint32_t CR2;    //Control register 2 (USART_CR2) Address offset: 0x10
	volatile uint32_t CR3;    //Control register 3 (USART_CR3) Address offset: 0x14
	volatile uint32_t GTPR;   //Guard time and prescaler register (USART_GTPR) Address offset: 0x18
}USART_RegDef_t;

//peripheral definition (peripheral base addresss typecasted to xxxx_RegDef_t
#define GPIOA                ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB                ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC                ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD                ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE                ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF                ((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG                ((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH                ((GPIO_RegDef_t*)GPIOH_BASEADDR)

#define RCC                  ((RCC_RegDef_t*)RCC_BASEADDR)

#define EXTI                 ((EXTI_RegDef_t*)EXTI_BASEADDR)

#define SYSCFG               ((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)

#define SPI1                 ((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2                 ((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3                 ((SPI_RegDef_t*)SPI3_BASEADDR)
#define SPI4                 ((SPI_RegDef_t*)SPI4_BASEADDR)

#define I2C1                 ((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2                 ((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C3                 ((I2C_RegDef_t*)I2C3_BASEADDR)

#define USART1               ((USART_RegDef_t*)USART1_BASEADDR)
#define USART2               ((USART_RegDef_t*)USART2_BASEADDR)
#define USART3               ((USART_RegDef_t*)USART3_BASEADDR)
#define UART4                ((USART_RegDef_t*)UART4_BASEADDR)
#define UART5                ((USART_RegDef_t*)UART5_BASEADDR)
#define USART6               ((USART_RegDef_t*)USART6_BASEADDR)

//********************CLK EN MACROS******************************
//clk enable macros for gpiox peripherals
#define GPIOA_PCLK_EN()      (RCC->AHB1ENR|=(1<<0))
#define GPIOB_PCLK_EN()      (RCC->AHB1ENR|=(1<<1))
#define GPIOC_PCLK_EN()      (RCC->AHB1ENR|=(1<<2))
#define GPIOD_PCLK_EN()      (RCC->AHB1ENR|=(1<<3))
#define GPIOE_PCLK_EN()      (RCC->AHB1ENR|=(1<<4))
#define GPIOF_PCLK_EN()      (RCC->AHB1ENR|=(1<<5))
#define GPIOG_PCLK_EN()      (RCC->AHB1ENR|=(1<<6))
#define GPIOH_PCLK_EN()      (RCC->AHB1ENR|=(1<<7))

//clk enable macros for i2cx peripherals
#define I2C1_PCLK_EN()       (RCC->APB1ENR|=(1<<21))
#define I2C2_PCLK_EN()       (RCC->APB1ENR|=(1<<22))
#define I2C3_PCLK_EN()       (RCC->APB1ENR|=(1<<23))

//clk enable macros for spix peripherals
#define SPI1_PCLK_EN()       (RCC->APB2ENR|=(1<<12))
#define SPI2_PCLK_EN()       (RCC->APB1ENR|=(1<<14))
#define SPI3_PCLK_EN()       (RCC->APB1ENR|=(1<<15))
#define SPI4_PCLK_EN()       (RCC->APB2ENR|=(1<<13))

//clk enable macros for USARTx and UARTX peripherals
#define USART1_PCLK_EN()     (RCC->APB2ENR|=(1<<4))
#define USART2_PCLK_EN()     (RCC->APB1ENR|=(1<<17))
#define USART3_PCLK_EN()     (RCC->APB1ENR|=(1<<18))
#define UART4_PCLK_EN()      (RCC->APB1ENR|=(1<<19))
#define UART5_PCLK_EN()      (RCC->APB1ENR|=(1<<20))
#define USART6_PCLK_EN()     (RCC->APB2ENR|=(1<<5))

//clk enable macro for SYSCONFIG peripheral
#define SYSCFG_PCLK_EN()     (RCC->APB2ENR|=(1<<14))


//****************CLK DISABLE MACROS********************************
#define GPIOA_PCLK_DI()      (RCC->AHB1ENR&=~(1<<0))
#define GPIOB_PCLK_DI()      (RCC->AHB1ENR&=~(1<<1))
#define GPIOC_PCLK_DI()      (RCC->AHB1ENR&=~(1<<2))
#define GPIOD_PCLK_DI()      (RCC->AHB1ENR&=~(1<<3))
#define GPIOE_PCLK_DI()      (RCC->AHB1ENR&=~(1<<4))
#define GPIOF_PCLK_DI()      (RCC->AHB1ENR&=~(1<<5))
#define GPIOG_PCLK_DI()      (RCC->AHB1ENR&=~(1<<6))
#define GPIOH_PCLK_DI()      (RCC->AHB1ENR&=~(1<<7))

//clk disable macros for i2cx peripherals
#define I2C1_PCLK_DI()       (RCC->APB1ENR&=~(1<<21))
#define I2C2_PCLK_DI()       (RCC->APB1ENR&=~(1<<22))
#define I2C3_PCLK_DI()       (RCC->APB1ENR&=~(1<<23))

//clk disable macros for spix peripherals
#define SPI1_PCLK_DI()       (RCC->APB2ENR&=~(1<<12))
#define SPI2_PCLK_DI()       (RCC->APB1ENR&=~(1<<14))
#define SPI3_PCLK_DI()       (RCC->APB1ENR&=~(1<<15))
#define SPI4_PCLK_DI()       (RCC->APB2ENR&=~(1<<13))

//clk disable macros for USARTx and UARTX peripherals
#define USART1_PCLK_DI()     (RCC->APB2ENR&=~(1<<4))
#define USART2_PCLK_DI()     (RCC->APB1ENR&=~(1<<17))
#define USART3_PCLK_DI()     (RCC->APB1ENR&=~(1<<18))
#define UART4_PCLK_DI()      (RCC->APB1ENR&=~(1<<19))
#define UART5_PCLK_DI()      (RCC->APB1ENR&=~(1<<20))
#define USART6_PCLK_DI()     (RCC->APB2ENR&=~(1<<5))

//clk disable macro for SYSCONFIG peripheral
#define SYSCFG_PCLK_DI()     (RCC->APB2ENR&=~(1<<14))

//generic macros
#define ENABLE                1
#define DISABLE               0
#define SET                   ENABLE
#define RESET                 DISABLE
#define GPIO_PIN_SET          SET
#define GPIO_PIN_RESET        RESET
#define GPIO_BASEADDR_TO_CODE(x)      ((x==GPIOA)?0:\
		                              (x==GPIOB)?1:\
			                          (x==GPIOC)?2:\
						              (x==GPIOD)?3:\
								      (x==GPIOE)?4:\
									  (x==GPIOF)?5:\
									  (x==GPIOG)?6:\
					          		  (x==GPIOH)?7:0)
#define FLAG_RESET           RESET
#define FLAG_SET             SET
/*
 * Interrupt request (irq) numbers of stm32f446x mcu
 */
//for gpio peripheral
#define IRQ_NO_EXTI0         6
#define IRQ_NO_EXTI1         7
#define IRQ_NO_EXTI2         8
#define IRQ_NO_EXTI3         9
#define IRQ_NO_EXTI4         10
#define IRQ_NO_EXTI9_5       23
#define IRQ_NO_EXTI15_10     40

//for spi peripheral
#define IRQ_NO_SPI1          35
#define IRQ_NO_SPI2          36
#define IRQ_NO_SPI3          51
#define IRQ_NO_SPI4          84

//for i2c peripheral
#define IRQ_NO_I2C1_EV       31
#define IRQ_NO_I2C1_ER       32

#define IRQ_NO_I2C2_EV       33
#define IRQ_NO_I2C2_ER       34

#define IRQ_NO_I2C3_EV       72
#define IRQ_NO_I2C3_ER       73

//for usart peripheral
#define IRQ_NO_USART1        37
#define IRQ_NO_USART2        38
#define IRQ_NO_USART3        39
#define IRQ_NO_UART4         52
#define IRQ_NO_UART5         53
#define IRQ_NO_USART6        71

//macros for priority levels
#define NVIC_IRQ_PRIO0       0
#define NVIC_IRQ_PRIO1       1
#define NVIC_IRQ_PRIO2       2
#define NVIC_IRQ_PRIO3       3
#define NVIC_IRQ_PRIO4       4
#define NVIC_IRQ_PRIO5       5
#define NVIC_IRQ_PRIO6       6
#define NVIC_IRQ_PRIO7       7
#define NVIC_IRQ_PRIO8       8
#define NVIC_IRQ_PRIO9       9
#define NVIC_IRQ_PRIO10      10
#define NVIC_IRQ_PRIO11      11
#define NVIC_IRQ_PRIO12      12
#define NVIC_IRQ_PRIO13      13
#define NVIC_IRQ_PRIO14      14
#define NVIC_IRQ_PRIO15      15

//macros to reset gpiox peripherals
#define GPIOA_REG_RESET()    do{(RCC->AHB1RSTR|=1<<0);(RCC->AHB1RSTR&=~(1<<0));}while(0)
#define GPIOB_REG_RESET()    do{(RCC->AHB1RSTR|=1<<1);(RCC->AHB1RSTR&=~(1<<1));}while(0)
#define GPIOC_REG_RESET()    do{(RCC->AHB1RSTR|=1<<2);(RCC->AHB1RSTR&=~(1<<2));}while(0)
#define GPIOD_REG_RESET()    do{(RCC->AHB1RSTR|=1<<3);(RCC->AHB1RSTR&=~(1<<3));}while(0)
#define GPIOE_REG_RESET()    do{(RCC->AHB1RSTR|=1<<4);(RCC->AHB1RSTR&=~(1<<4));}while(0)
#define GPIOF_REG_RESET()    do{(RCC->AHB1RSTR|=1<<5);(RCC->AHB1RSTR&=~(1<<5));}while(0)
#define GPIOG_REG_RESET()    do{(RCC->AHB1RSTR|=1<<6);(RCC->AHB1RSTR&=~(1<<6));}while(0)
#define GPIOH_REG_RESET()    do{(RCC->AHB1RSTR|=1<<7);(RCC->AHB1RSTR&=~(1<<7));}while(0)

//bit position defination of spi peripherals
//CR1 REGISTER
#define SPI_CR1_CPHA         0
#define SPI_CR1_CPOL         1
#define SPI_CR1_MSTR         2
#define SPI_CR1_BR           3
#define SPI_CR1_SPE          6
#define SPI_CR1_LSBFIRST     7
#define SPI_CR1_SSI          8
#define SPI_CR1_SSM          9
#define SPI_CR1_RX_ONLY      10
#define SPI_CR1_DFF          11
#define SPI_CR1_CRC_NEXT     12
#define SPI_CR1_CRC_EN       13
#define SPI_CR1_BIDI_OE      14
#define SPI_CR1_BIDI_MODE    15

//CR2 REGISTER
#define SPI_CR2_RXDMAEN      0
#define SPI_CR2_TXDMAEN      1
#define SPI_CR2_SSOE         2
#define SPI_CR2_RESERVED     3
#define SPI_CR2_FRF          4
#define SPI_CR2_ERRIE        5
#define SPI_CR2_RXNEIE       6
#define SPI_CR2_TXEIE        7

//SR REGISTER
#define SPI_SR_RXNE          0
#define SPI_SR_TXE           1
#define SPI_SR_CHSIDE        2
#define SPI_SR_UDR           3
#define SPI_SR_CRC_ERR       4
#define SPI_SR_MODF          5
#define SPI_SR_OVR           6
#define SPI_SR_BSY           7
#define SPI_SR_FRE           8

//macros to reset spix peripherals
#define SPI1_REG_RESET()     do{(RCC->APB2RSTR|=1<<12);(RCC->APB2RSTR&=~(1<<12));}while(0)
#define SPI2_REG_RESET()     do{(RCC->APB1RSTR|=1<<14);(RCC->APB1RSTR&=~(1<<14));}while(0)
#define SPI3_REG_RESET()     do{(RCC->APB1RSTR|=1<<15);(RCC->APB1RSTR&=~(1<<15));}while(0)
#define SPI4_REG_RESET()     do{(RCC->APB2RSTR|=1<<13);(RCC->APB2RSTR&=~(1<<13));}while(0)

//bit position defination of i2cx peripherals
//I2C_CR1 REGISTER
#define I2C_CR1_PE           0
#define I2C_CR1_SMBUS        1
#define I2C_CR1_SMBTYPE      3
#define I2C_CR1_ENARP        4
#define I2C_CR1_ENPEC        5
#define I2C_CR1_ENGC         6
#define I2C_CR1_NOSTRETCH    7
#define I2C_CR1_START        8
#define I2C_CR1_STOP         9
#define I2C_CR1_ACK          10
#define I2C_CR1_POS          11
#define I2C_CR1_PEC          12
#define I2C_CR1_ALERT        13
#define I2C_CR1_SWRST        15

//I2C_CR2 REGISTER
#define I2C_CR2_FREQ         0 //5 bits len
#define I2C_CR2_ITERREN      8
#define I2C_CR2_ITEVTEN      9
#define I2C_CR2_ITBUFEN      10
#define I2C_CR2_DMAEN        11
#define I2C_CR2_LAST         12

//I2C_SR1 REGISTER
#define I2C_SR1_SB           0
#define I2C_SR1_ADDR         1
#define I2C_SR1_BTF          2
#define I2C_SR1_ADD10        3
#define I2C_SR1_STOPF        4
#define I2C_SR1_RxNE         6
#define I2C_SR1_TxE          7
#define I2C_SR1_BERR         8
#define I2C_SR1_ARLO         9
#define I2C_SR1_AF           10
#define I2C_SR1_OVR          11
#define I2C_SR1_PECERR       12
#define I2C_SR1_TIMEOUT      14
#define I2C_SR1_SMBALERT     15

//I2C_SR2 REGISTER
#define I2C_SR2_MSL          0
#define I2C_SR2_BUSY         1
#define I2C_SR2_TRA          2
#define I2C_SR2_GENCALL      4
#define I2C_SR2_SMBDEFAULT   5
#define I2C_SR2_SMBHOST      6
#define I2C_SR2_DUALF        7
#define I2C_SR2_PEC07        8 //8 BITS TILL THE LAST 15TH BIT

//I2C_CCR REGISTER
#define I2C_SR2_CCR          0
#define I2C_SR2_DUTY         14
#define I2C_SR2_FS           15

//macros to reset i2c peripherals
#define I2C1_REG_RESET()     do{(RCC->APB1RSTR|=1<<21);(RCC->APB1RSTR&=~(1<<21));}while(0)
#define I2C2_REG_RESET()     do{(RCC->APB1RSTR|=1<<22);(RCC->APB1RSTR&=~(1<<22));}while(0)
#define I2C3_REG_RESET()     do{(RCC->APB1RSTR|=1<<23);(RCC->APB1RSTR&=~(1<<23));}while(0)

//bit position defination of usartx peripherals
//USART_SR REGISTER
#define USART_SR_PE          0
#define USART_SR_FE          1
#define USART_SR_NF          2
#define USART_SR_ORE         3
#define USART_SR_IDLE        4
#define USART_SR_RXNE        5
#define USART_SR_TC          6
#define USART_SR_TXE         7
#define USART_SR_LBD         8
#define USART_SR_CTS         9

//USART_BRR REGISTER
#define USART_BRR_DIV_Fraction    0 //4 bits
#define USART_BRR_DIV_Mantissa    4 //12 bits

//USART_CR1 REGISTER
#define USART_CR1_SBK        0
#define USART_CR1_RWU        1
#define USART_CR1_RE         2
#define USART_CR1_TE         3
#define USART_CR1_IDLEIE     4
#define USART_CR1_RXNEIE     5
#define USART_CR1_TCIE       6
#define USART_CR1_TXEIE      7
#define USART_CR1_PEIE       8
#define USART_CR1_PS         9
#define USART_CR1_PCE        10
#define USART_CR1_WAKE       11
#define USART_CR1_M          12
#define USART_CR1_UE         13
#define USART_CR1_OVER8      15

//USART_CR2 REGISTER
#define USART_CR2_ADD        0 //4 BITS
#define USART_CR2_LBDL       5
#define USART_CR2_LBDIE      6
#define USART_CR2_LBCL       8
#define USART_CR2_CPHA       9
#define USART_CR2_CPOL       10
#define USART_CR2_CLKEN      11
#define USART_CR2_STOP       12 //2BITS
#define USART_CR2_LINEN      14

//USART_CR3 REGISTER
#define USART_CR3_EIE        0
#define USART_CR3_IREN       1
#define USART_CR3_IRLP       2
#define USART_CR3_HDSEL      3
#define USART_CR3_NACK       4
#define USART_CR3_SCEN       5
#define USART_CR3_DMAR       6
#define USART_CR3_DMAT       7
#define USART_CR3_RTSE       8
#define USART_CR3_CTSE       9
#define USART_CR3_CTSIE      10
#define USART_CR3_ONEBIT     11

//USART_GTPR REGISTER
#define USART_GTPR_PSC       0 //8 BITS
#define USART_GTPR_GT        8 //8 BITS

//macros to reset usart peripherals
#define USART1_REG_RESET()   do{(RCC->APB2RSTR|=1<<4);(RCC->APB1RSTR&=~(1<<4));}while(0)
#define USART6_REG_RESET()   do{(RCC->APB2RSTR|=1<<5);(RCC->APB1RSTR&=~(1<<5));}while(0)

#define USART2_REG_RESET()   do{(RCC->APB1RSTR|=1<<17);(RCC->APB1RSTR&=~(1<<17));}while(0)
#define USART3_REG_RESET()   do{(RCC->APB1RSTR|=1<<18);(RCC->APB1RSTR&=~(1<<18));}while(0)
#define UART4_REG_RESET()   do{(RCC->APB1RSTR|=1<<19);(RCC->APB1RSTR&=~(1<<19));}while(0)
#define UART5_REG_RESET()   do{(RCC->APB1RSTR|=1<<20);(RCC->APB1RSTR&=~(1<<20));}while(0)


#include "stm32f44xx_gpio_driver.h"
#include "stm32f44xx_spi_driver.h"
#include "stm32f44xx_i2c_driver.h"
#include "stm32f44xx_USART_driver.h"
#include "stm32f44xx_rcc_driver.h"
#endif /* INC_D_STM32F44XX_H_ */
