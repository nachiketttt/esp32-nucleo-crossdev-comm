/*
 * stm32f44xx_rcc_driver.c
 *
 *  Created on: 17-Feb-2026
 *      Author: nachiket
 */

#include "stm32f44xx_rcc_driver.h"

uint16_t AHB_PreScaler[8]={2,4,8,16,64,128,256,512};
uint16_t APB1_PreScaler[4]={2,4,8,16};
uint16_t APB2_PreScaler[4]={2,4,8,16};

int RCC_GetPLLOutputClock()
{
	return 0;
}
uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t pclk1,SystemClk;
	uint8_t clksrc=((RCC->CFGR>>2)&(0x3<<0));
	uint8_t temp,ahbp,apb1p;

	if(clksrc==0)
	{
		SystemClk=16000000; //hsi clk 16mhz
	}
	else if(clksrc==1)
	{
		SystemClk=8000000; //hsi clk 8mhz
	}
	else if(clksrc==2)
	{
		SystemClk=RCC_GetPLLOutputClock();
	}

	//ahbp (ahb prescaler)
	temp=((RCC->CFGR>>4)&(0xF));
	if(temp<8)
	{
		ahbp=1;
	}
	else
	{
		ahbp=AHB_PreScaler[temp-8];
	}

	//apb1 (apb1 pescaler)
	temp=((RCC->CFGR>>10)&(0x7));
	if(temp<4)
	{
		apb1p=1;
	}
	else
	{
		apb1p=APB1_PreScaler[temp-4];
	}

	pclk1=((SystemClk/ahbp)/apb1p);
	return pclk1;
}

uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t pclk2,SystemClk;
	uint8_t clksrc=((RCC->CFGR>>2)&(0x3<<0));
	uint8_t temp,ahbp,apb2p;

	if(clksrc==0)
	{
		SystemClk=16000000; //hsi clk 16mhz
	}
	else if(clksrc==1)
	{
		SystemClk=8000000; //hsi clk 8mhz
	}
	else if(clksrc==2)
	{
		SystemClk=RCC_GetPLLOutputClock();
	}

	//ahbp (ahb prescaler)
	temp=((RCC->CFGR>>4)&(0xF));
	if(temp<8)
	{
		ahbp=1;
	}
	else
	{
		ahbp=AHB_PreScaler[temp-8];
	}

	//apb2 (apb1 pescaler)
	temp=((RCC->CFGR>>13)&(0x7));
	if(temp<4)
	{
		apb2p=1;
	}
	else
	{
		apb2p=APB1_PreScaler[temp-4];
	}

	pclk2=((SystemClk/ahbp)/(apb2p));
	return pclk2;
}
