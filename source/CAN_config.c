#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_flash.h"
#include "CAN_config.h"


void CAN_Configuration(void)
{

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); //enable clock for can 1
	CAN_InitTypeDef CAN_InitStructure;

	CAN_DeInit(CAN1);

    CAN_InitStructure.CAN_TTCM = DISABLE; // Time-triggered communication mode = DISABLED
    CAN_InitStructure.CAN_ABOM = DISABLE; // Automatic bus-off management mode = DISABLED
    CAN_InitStructure.CAN_AWUM = DISABLE; // Automatic wake-up mode = DISABLED
    CAN_InitStructure.CAN_NART = ENABLE; // Non-automatic retransmission mode = DISABLED
    CAN_InitStructure.CAN_RFLM = DISABLE; // Receive FIFO locked mode = DISABLED
    CAN_InitStructure.CAN_TXFP = DISABLE; // Transmit FIFO priority = DISABLED

    // Baudrate 500kbs -> APB1 = 42Mhz
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_14tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
    CAN_InitStructure.CAN_Prescaler = 4;

    CAN_Init(CAN1, &CAN_InitStructure);
}

void CAN_Filters(void)
{

	    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	    CAN_FilterInitStructure.CAN_FilterNumber = 0;
	    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	    CAN_FilterInit(&CAN_FilterInitStructure);
}




