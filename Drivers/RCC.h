#ifndef __RCC_H__
#define __RCC_H__

#include "stm32f10x.h"   
#ifndef HSE_CLK 
	#define HSE_CLK  8000000ul
#endif
#define HSI_CLK  8000000ul

uint32_t RCC_GetSysClk();
float RCC_GetHClk();
float RCC_GetAbp1Clk();
float RCC_GetAbp2Clk();

#endif /*__RCC_H__*/