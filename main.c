#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_can.h"
#include "string.h"
#include "CAN_config.h"
#include "math.h"
#include "Functions.h"
#include "stm32f4xx_tim.h"

#define MAX_RX_MSG  15u
#define MAX_TX_MSG  10u

CanTxMsg TxMessage;
CAN_message_Rx CAN_Rx[MAX_RX_MSG]; 
CAN_message_Tx CAN_Tx[] = {
		{0xA, 8, {20,1,22,2,50,1,70,8},500,"Motor"},
		{0xB, 8, {17,18,26,20,6,13,60,10},500,"Usa_dreapta_fata"},
		{0xC, 8, {17,18,26,20,6,13,60,10},500},
		{0xD, 8, {12,2,21,23,52,12,80,11},500},
		{0xE, 8, {12,2,21,23,52,12,80,11},500},
};
FlagStatus status;
uint32_t contor = 0;
int time = 0;
uint8_t number_msg_tx= sizeof(CAN_Tx)/sizeof(CAN_message_Tx);
uint8_t number_msg_rx = 0;
uint8_t contor_linie=2; // mesajele sunt afisate pe LCD incepand cu linia 2

void Init()
{
	for(int i=0;i<MAX_RX_MSG;i++)
	{
		CAN_Rx[i].Id = 0;
		CAN_Rx[i].DLC = 0;
		CAN_Rx[i].Time = 0;
		CAN_Rx[i].T0 = 0;
		CAN_Rx[i].T1 = 0;
		for(int j=0;j<8;j++)
		{
			CAN_Rx[i].DATA[j] = 0;
		}
	}
}

void Update_Time(uint8_t Id)
{
		CAN_Rx[Id].T1 = (TIM_GetCounter(TIM2))/2;
		CAN_Rx[Id].Time = (CAN_Rx[Id].T1 - CAN_Rx[Id].T0)/1000;
		CAN_Rx[Id].T0 = CAN_Rx[Id].T1;
}

void Tx_Message(uint8_t i)
{
	uint32_t uwCounter = 0;
    uint8_t TransmitMailbox = 0;
    TxMessage.StdId = CAN_Tx[i].Id;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.DLC = CAN_Tx[i].DLC;
    TxMessage.Data[0] = CAN_Tx[i].DATA[0];
    TxMessage.Data[1] = CAN_Tx[i].DATA[1];
    TxMessage.Data[2] = CAN_Tx[i].DATA[2];
    TxMessage.Data[3] = CAN_Tx[i].DATA[3];
    TxMessage.Data[4] = CAN_Tx[i].DATA[4];
    TxMessage.Data[5] = CAN_Tx[i].DATA[5];
    TxMessage.Data[6] = CAN_Tx[i].DATA[6];
    TxMessage.Data[7] = CAN_Tx[i].DATA[7];

    TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
    uwCounter = 0;
    while((CAN_TransmitStatus(CAN1, TransmitMailbox)  !=  CANTXOK) && (uwCounter  !=  0xFFFF))
    {
        uwCounter++;
    }

    uwCounter = 0;

    while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (uwCounter  !=  0xFFFF))
    {
        uwCounter++;
    }
}

void Rx_Message()
{
	  CanRxMsg RxMessage;
      RxMessage.StdId = 0;
      RxMessage.RTR = CAN_RTR_DATA;
      RxMessage.IDE = CAN_ID_STD;
      RxMessage.DLC = 8;
      RxMessage.Data[0] = 0x00;
      RxMessage.Data[1] = 0x00;
      RxMessage.Data[2] = 0x00;
      RxMessage.Data[3] = 0x00;
      RxMessage.Data[4] = 0x00;
      RxMessage.Data[5] = 0x00;
      RxMessage.Data[6] = 0x00;
      RxMessage.Data[7] = 0x00;

      status = CAN_GetFlagStatus(CAN1, CAN_FLAG_FMP0);

      if(status==SET)
      {
    	  CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
    	  if(RxMessage.StdId<4095){


    	  for(int i=0;i<MAX_RX_MSG;i++)
    	  {

    		  if((RxMessage.StdId != CAN_Rx[i].Id)&&(CAN_Rx[i].Id == 0))
    		  {

    			  Update_Time(i);
    			  number_msg_rx++;
    			  CAN_Rx[i].Id = RxMessage.StdId;
    			  CAN_Rx[i].DLC = RxMessage.DLC;
    			  for(int j=0;j<8;j++)
    			  {
    				  CAN_Rx[i].DATA[j] = RxMessage.Data[j];
    			  }
    			  break;
    		  }

    		  else if ((RxMessage.StdId == CAN_Rx[i].Id)&&(RxMessage.StdId != 0))
    		  {

    			  Update_Time(i);
    			  for(int j=0;j<8;j++)
    			  {
    				  CAN_Rx[i].DATA[j] = RxMessage.Data[j];
    			  }break;
    		  }

    	  }
    	  }
      }
}

void Display_Rx()
{

	  char Id[5];
	  char data[40];
	  char message_LCD[20];
	  char Time[20];
	  messageData array_char[8];
	  int linie = number_msg_tx + 2;// mesajele receptionate sunt afisate dupa mesajele trimise +2 (afisarea incepe de pe linia 2)
	  int a=0;

	  while((a<MAX_RX_MSG)&&(CAN_Rx[a].Id!=0))
	  {
		  itoa(CAN_Rx[a].Id, Id, 16);
		  ftoa(CAN_Rx[a].Time, Time);
		  for(int i=0;i<CAN_Rx[a].DLC;i++)
		  {
	           itoa(CAN_Rx[a].DATA[i],array_char[i].data,16);
		  }

		  for(int i=0;i<CAN_Rx[a].DLC;i++)
		  {
			  if(i==0)
			  {
				  if(strlen(array_char[i].data)==1)
				  {
					  strcpy(data,"0");
					  strcat(data,array_char[i].data);
					  strcat(data," ");
				  }
				  else
				  {
					  strcpy(data,array_char[i].data);
					  strcat(data," ");
				  }
			  }
			  else
			  {
				  if(strlen(array_char[i].data)==1)
				  {
					  strcat(data,"0");
					  strcat(data,array_char[i].data);
					  strcat(data," ");
				  }
				  else
				  {
					  strcat(data,array_char[i].data);
					  strcat(data," ");
				  }

			  }
		  }

	  strcpy(message_LCD,Time);
      strcat(message_LCD," ");

      if(strlen(Id)==1)
 	  {
 		 strcat(message_LCD,Id);
 		 strcat(message_LCD,"  ");
 	  }
 	  else if(strlen(Id)==2)
 	  {
 		 strcat(message_LCD,Id);
 		 strcat(message_LCD," ");
 	  }
 	  else if(strlen(Id)==3)
 	  {
		 strcat(message_LCD,Id);
 	  }

      strcat(message_LCD," ");
	  strcat(message_LCD,"Rx");
	  strcat(message_LCD," ");
	  strcat(message_LCD,data);

	  if(linie==contor_linie)
	  {
		  LCD_SetBackColor(LCD_COLOR_BLUE);
	  }

	  LCD_DisplayStringLine(LINE(linie), (uint8_t *)message_LCD);
	  LCD_SetBackColor(LCD_COLOR_BLACK);
	  a++;
	  linie++;
	  }
}

void Display_Tx()
{

	char Id[4];
	char data[40];
	char message_LCD[20];
	messageData array_char[8];
	char Time[20];
	int linie = 2;
	int a = 0;

	while(a<number_msg_tx)
	{
		itoa(CAN_Tx[a].Id,Id,16);
		itoa(CAN_Tx[a].Time, Time,10);
		for(int i=0;i<CAN_Tx[a].DLC;i++)
		{
			itoa(CAN_Tx[a].DATA[i],array_char[i].data,16);
		}
		for(int i=0;i<CAN_Tx[a].DLC;i++)
		{
			if(i==0)
			{
			  if(strlen(array_char[i].data)==1)
			  {
				  strcpy(data,"0");
				  strcat(data,array_char[i].data);
				  strcat(data," ");
			  }
			  else
			  {
				  strcpy(data,array_char[i].data);
				  strcat(data," ");
			  }
		   }
		   else
		   {
			  if(strlen(array_char[i].data)==1)
			  {
			  	  strcat(data,"0");
			  	  strcat(data,array_char[i].data);
			  	  strcat(data," ");
			  }
			  else
			  {
				  strcat(data,array_char[i].data);
				  strcat(data," ");
			  }
		  }
	 }
	 strcpy(message_LCD,"0.");
	 strcat(message_LCD,Time);
	 strcat(message_LCD," ");

	 if(strlen(Id)==1)
	 {
		 strcat(message_LCD,Id);
		 strcat(message_LCD,"  ");
	 }
	 else if(strlen(Id)==2)
	 {
		 strcat(message_LCD,Id);
		 strcat(message_LCD," ");
	 }
	 else if(strlen(Id)==3)
	 {
		 strcat(message_LCD,Id);
	 }

     strcat(message_LCD," ");
	 strcat(message_LCD,"Tx");
	 strcat(message_LCD," ");
	 strcat(message_LCD,data);
	 if(linie==contor_linie)
	 {
		 LCD_SetBackColor(LCD_COLOR_BLUE);
	 }
	 LCD_DisplayStringLine(LINE(linie), (uint8_t *)message_LCD);
	 LCD_SetBackColor(LCD_COLOR_BLACK);
	 a++;
	 linie++;
	}
}

void TIM4_IRQHandler()
{
    // Checks whether the TIM4 interrupt has occurred or not
    if (TIM_GetITStatus(TIM4, TIM_IT_Update))
    {
		contor++;
		time = 100*contor;
		for(int i=0;i<number_msg_tx;i++)
		{
			if((time % CAN_Tx[i].Time) == 0)
			{
					Tx_Message(i);
			}
       // Clears the TIM4 interrupt pending bit
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        }
    }
}

void Enter_message_Tx(CAN_message_Tx message)
{
	LCD_Clear(LCD_COLOR_BLACK);
	messageData array_char[8];
	char Id[2];
	char FirstLine[20];
	char data[40];
	char ind[5];
	int a=2;
	strcpy(FirstLine,"MESSAGE ID: ");
	itoa(message.Id, Id, 16);
	strcat(FirstLine,Id);
	strcat(FirstLine,"  NAME: ");
	strcat(FirstLine,message.name);
	LCD_DisplayStringLine(LINE(1), (uint8_t *)FirstLine);

	for(int i=0;i<message.DLC;i++)
	{
			 itoa(message.DATA[i],array_char[i].data,16);
			 itoa(i,ind,10);
			 strcpy(data,"Data ");
			 strcat(data,ind);
			 strcat(data," : ");
			 strcat(data,array_char[i].data);
			 LCD_DisplayStringLine(LINE(a), (uint8_t *)data);
			 a++;
	}
}

void Enter_message_Rx(CAN_message_Rx message)
{
	LCD_Clear(LCD_COLOR_BLACK);
	messageData array_char[8];
	char Id[2];
	char FirstLine[20];
	char data[40];
	char ind[5];
	int a=2;
	strcpy(FirstLine,"MESSAGE ID: ");
	itoa(message.Id, Id, 16);
	strcat(FirstLine,Id);
	LCD_DisplayStringLine(LINE(1), (uint8_t *)FirstLine);

	for(int i=0;i<message.DLC;i++)
	{
			 itoa(message.DATA[i],array_char[i].data,16);
			 itoa(i,ind,10);
			 strcpy(data,"data ");
			 strcat(data,ind);
			 strcat(data," : ");
			 strcat(data,array_char[i].data);
			 LCD_DisplayStringLine(LINE(a), (uint8_t *)data);
			 a++;
	}
}
int main(void)
{
	Clock_Configuration();
	GPIO_Configuration();
	CAN_Configuration();
	CAN_Filters();
	STM32f4_Discovery_LCD_Init();
	LCD_SetBackColor(LCD_COLOR_BLACK);
	Init_Buttons();
	Init();
	Timer2_Configuration();
	Timer4_Configuration();

	RCC_ClocksTypeDef checkClocks;
	RCC_GetClocksFreq(&checkClocks);

	int contor_enter=0;
	int i = 0;
	int j = 0;
	int a = number_msg_tx+2;

	while(1)
	{
		LCD_SetTextColor(LCD_COLOR_RED);
		char header[20] = "TIME  ID  DIR  DATA";
		LCD_DisplayStringLine(LINE(1), (uint8_t *)header);
		LCD_SetTextColor(LCD_COLOR_WHITE);


		Display_Tx();
		Rx_Message();
		Display_Rx();

		if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)==1)
		{
			contor_linie++;
			if(contor_linie==number_msg_rx+number_msg_tx+2)
			{
				contor_linie=2;
			}
			for(int delay=1000000;delay>0;delay--);
		}

		contor_enter=contor_linie;

		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==1)
		{
			if(contor_enter<number_msg_tx+2)
			{
				i = contor_enter - 2;
				Enter_message_Tx(CAN_Tx[i]);

			}
			else if(contor_enter>=number_msg_tx+1)
			{

				j = contor_enter-a;
				Enter_message_Rx(CAN_Rx[j]);
			}
			while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)==0)
			{
				Rx_Message();
			}
			LCD_Clear(LCD_COLOR_BLACK);
			//for(int delay=1000000;delay>0;delay--);
		}
	}
}
