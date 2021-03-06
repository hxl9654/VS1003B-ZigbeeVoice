#include "main.h"
bit PlayStatu = 0;
bit RecordStatu = 0;
bit RecordStatuStart = 0;
bit RecordStatuStop = 0;
bit MainLoopNormalFlag = 0;
unsigned char Send_Temp[270];

void main()
{
	SystemInit();
	while(1)
	{				
		MainLoopNormalFlag = 1;
		TR1 = 0; UART_Driver(); TR1 = 1;
		if(PlayStatu)VS1003_Play();
		else if(RecordStatu)VS1003_Record();
		//else VS1003_Fill2048Zero();
	}
}
bit Response_WorkingFlag = 0;
void Response()
{
	unsigned int RecordQueueStatu, PlayQueueStatu;
	if(Response_WorkingFlag)
		return;
	Response_WorkingFlag = 1;
	Send_Temp[0] = 0x96; Send_Temp[1] = 0x38; Send_Temp[2] = 0x52; Send_Temp[3] = 0x74; Send_Temp[4] = 0x10;
	RecordQueueStatu = RecordQueue_GetStatu();
	PlayQueueStatu = PlayQueue_GetStatu();
	Send_Temp[5] = PlayQueueStatu >> 8;
	Send_Temp[6] = RecordQueueStatu >> 8;
	if(RecordStatu && Send_Temp[6] == 0)
	{
		Response_WorkingFlag = 0;
		return ;
	}
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
	if((RecordStatu || RecordStatuStop) && RecordQueueStatu >= 256)
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
	if(RecordStatuStop)
		IAP_CONTR = 0x20;	//������λ
	RecordStatuStop = 0;	
	Response_WorkingFlag = 0;
	if(MainLoopNormalFlag)
	{
		MainLoopNormalFlag = 0;
		WatchDogTimerFeed();
	}
}
void Play_Start()
{
	PlayQueue_In(header, 64);
	PlayStatu = 1;	
}
void Play_Stop()
{
	PlayStatu = 0;
	PlayQueue_Reset();
	VS1003_OutOfWAV();
}
void UART_Action(unsigned char dat, unsigned char len)
{
	switch(dat)
	{
		case(0x00): if( PlayStatu) Play_Stop();			break;
		case(0x01): if(!PlayStatu) Play_Start();		break;
		case(0x02): if(!PlayStatu) Play_Start();		break;
		case(0x03): if( PlayStatu) Play_Stop();			break;
		case(0x10): 				 					break;
		case(0x99): SystemReset();						break;
		default: 										break;	
	}
	if(len != 0x00)
		UARTQueue_TO_PlayQueue(256);
	if(!RecordStatu)
		Response();
}
void Timer1_Interrupt() interrupt 3
{
	TR1 = 0;
	UART_Driver();
	TR1 = 1;
}
void Timer0_Interrupt() interrupt 1
{
	TR0 = 0;
	if(RecordStatu || RecordStatuStop)
		Response();
	TR0 = 1;
}
void INT0_Interrupt() interrupt 0
{
	UART_Driver();
	if(INT0 == 0 && RecordStatu == 1)
	{
		RecordStatu = 0;
		RecordStatuStop = 1;
	}
	else if(INT0 == 1 && RecordStatu == 0)
	{
		if(PlayStatu == 0)
		{
			RecordQueue_Reset();
			RecordStatu = 1;
			RecordStatuStart = 1;			
		}
	}
}