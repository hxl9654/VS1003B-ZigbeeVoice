#include "stc15.h"
#include <intrins.h>
#include "UART.h"
#include "queue.h"
#include "vs1003.h"
#include "init.h"
sbit INT0 = P3 ^ 2;

const unsigned char header[64] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00,
0x52, 0x49, 0x46, 0x46, 0xff, 0xff, 0xff, 0xff,
0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20, /*|RIFF....WAVEfmt |*/
0x12, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
//0x40, 0x1f, 0x00, 0x00, 0x80, 0x3e, 0x00, 0x00, /*|........@...OE...|*/
0xa0, 0x0f, 0x00, 0x00, 0xa0, 0x0f, 0x00, 0x00, /*|........@...OE...|*/	
//0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x64, 0x61,
0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x64, 0x61,
0x74, 0x61, 0xff, 0xff, 0xff, 0xff
};

bit PlayStatu = 0;
bit RecordStatu = 0;
bit RecordStatuStart = 0;
bit RecordStatuStop = 0;
unsigned char Send_Temp[270];
void Response()
{
	unsigned int RecordQueueStatu, PlayQueueStatu;
	Send_Temp[0] = 0x96; Send_Temp[1] = 0x38; Send_Temp[2] = 0x52; Send_Temp[3] = 0x74; Send_Temp[4] = 0x10;
	RecordQueueStatu = RecordQueue_GetStatu();
	PlayQueueStatu = PlayQueue_GetStatu();
	Send_Temp[5] = PlayQueueStatu >> 8;
	Send_Temp[6] = RecordQueueStatu >> 8;
	if(RecordStatu && Send_Temp[6] == 0)
		return ;
	if(PlayStatu)
		Send_Temp[7] = 0x10;
	else if(RecordStatuStart)
	{
		Send_Temp[7] = 0x01;
		RecordStatuStart = 0;
	}
	else if(RecordStatuStop && RecordQueueStatu < 256)
	{
		Send_Temp[7] = 0x03;
		RecordStatuStop = 0;
	}
	else if(RecordStatu || (RecordStatuStop && RecordQueueStatu >= 256))
		Send_Temp[7] = 0x02;
	else
		Send_Temp[7] = 0x00;
	if((RecordStatu || RecordStatuStop) && RecordQueueStatu >= 512)
	{
		Send_Temp[8] = 0x01;
		UARTQueue_Out(Send_Temp + 9, 256);
		UART_SendString(Send_Temp, 265);
		UARTQueue_Out(Send_Temp + 9, 256);
		UART_SendString(Send_Temp, 265);
	}
	else if((RecordStatu || RecordStatuStop) && RecordQueueStatu >= 256)
	{
		Send_Temp[8] = 0x01;
		RecordQueue_Out(Send_Temp + 9, 256);
		UART_SendString(Send_Temp, 265);
	}
	else 
	{
		Send_Temp[8] = 0x00;
		UART_SendString(Send_Temp, 9);
	}	
}
void UART_Action(unsigned char *dat, unsigned int len)
{
	if(dat[0] == 0x14 && dat[1] == 0x72 && dat[2] == 0x58 && dat[3] == 0x36 && dat[4] == 0x90)
	{
		//WatchDogTimerFeed();
		switch(dat[5])
		{
			case(0x00): if(PlayStatu) {PlayStatu = 0; PlayQueue_Reset();}			break;
			case(0x01): if(!PlayStatu){PlayStatu = 1; PlayQueue_In(header, 64);}	break;
			case(0x02): if(!PlayStatu){PlayStatu = 1; PlayQueue_In(header, 64);}	break;
			case(0x03): PlayStatu = 0; PlayQueue_Reset();							break;
			case(0x10): 				 											break;
			case(0x99): SystemReset();												break;
			default: 																break;	
		}
		if(dat[6] != 0x00)
			PlayQueue_In(dat + 7, len - 7);
		if(!RecordStatu)
			Response();
	}
}
void main()
{
	SystemInit();
	while(1)
	{				
		UART_Driver();
		if(PlayStatu)VS1003_Play();
		else if(RecordStatu)VS1003_Record();
		else VS1003_Fill2048Zero();
	}
}
void Timer1_Interrupt() interrupt 3
{
	UART_Driver();
	if(RecordStatu || RecordStatuStop)
		Response();
}
void INT0_Interrupt() interrupt 0
{
	UART_Driver();
	if(INT0 == 1 && RecordStatu == 1)
	{
		RecordStatu = 0;
		RecordStatuStop = 1;
	}
	else if(RecordStatu == 0)
	{
		if(PlayStatu == 0)
		{
			RecordQueue_Reset();
			RecordStatu = 1;
			RecordStatuStart = 1;			
		}
	}
}