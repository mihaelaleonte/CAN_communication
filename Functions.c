#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_flash.h"
#include "Functions.h"
#include "stm32f4xx_tim.h"
#include "math.h"
#include "misc.h"

void GPIO_Configuration(void)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // enable clock for port B
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 ; // RX--->PD0
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 ; //TX--->PD1
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);
}

void Init_Buttons()
{

		GPIO_InitTypeDef GPIO_InitDef;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		GPIO_InitDef.GPIO_Pin = GPIO_Pin_11;// BUTON 1 PD11
		GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
		GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOD,&GPIO_InitDef);

	    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		GPIO_InitDef.GPIO_Pin = GPIO_Pin_15;// BUTON 2 PA15
		GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
		GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOA,&GPIO_InitDef);

}

void Clock_Configuration()
{

		RCC_DeInit();// Resets the clock configuration to the default reset state
		RCC_HSEConfig(RCC_HSE_ON);// Enable external crystal (HSE)

		// Wait until HSE ready to use or not
		ErrorStatus errorStatus = RCC_WaitForHSEStartUp();

		if (errorStatus == SUCCESS)
		{

			RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);// Configure the PLL for 168MHz SysClk and 48MHz for USB OTG, SDIO
			RCC_PLLCmd(ENABLE);// Enable PLL
			while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);// Wait until main PLL clock ready
			FLASH_SetLatency(FLASH_Latency_5);// Set flash latency

			RCC_HCLKConfig(RCC_SYSCLK_Div1);// AHB 168MHz
			RCC_PCLK1Config(RCC_HCLK_Div4);// APB1 42MHz
			RCC_PCLK2Config(RCC_HCLK_Div2);// APB2 84 MHz
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);// Set SysClk using PLL

		}
			else
			{
					        // Do something to indicate that error clock configuration
					        while (1);
		    }

}

void Timer2_Configuration()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 41999;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 0xFFFFFFFF;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);
}

void Timer4_Configuration()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 839;
	timerInitStructure.TIM_Period = 9999;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4, &timerInitStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}


int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}


void ftoa(float n, char *res)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 1);

    // check for display option after point

    res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
    fpart = fpart * pow(10, 3);

    intToStr((int)fpart, res + i + 1, 3);

}
