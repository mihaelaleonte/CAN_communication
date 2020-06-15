
typedef struct
{
	char data[10];

}messageData;

typedef struct
{
	uint8_t Id;
	uint8_t DLC;
	uint8_t DATA[8];
	uint32_t Time;
	char name[20];
}CAN_message_Tx;

typedef struct
{
	uint32_t Id;
	uint8_t DLC;
	uint8_t DATA[8];
	float Time;
	float T0;
	float T1;

}CAN_message_Rx;

void CAN_Configuration(void);
void CAN_Filters(void);
