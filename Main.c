#include "stc15.h"
#include <intrins.h>
#include "UART.h"
#include "queue.h"
#include "vs1003.h"
sbit INT0 = P3 ^ 2;
void Timer0_Init();
void Timer1_Init();
void INT0_Init();
void Interrupt_Init();
bit PlayStatu = 0;
bit RecordStatu = 0;
unsigned char temp[265] = {0};
void SendRecordData()
{
	bit t = 0;
	temp[0] = 0x96; temp[1] = 0x38; temp[2] = 0x52; temp[3] = 0x74; temp[4] = 0x13;
	t = RecordQueueOut(temp + 5, 256);
	if(t == 0)
		UART_SendString(temp, 261);
	else
	{
		temp[4] = 0x1c;
		UART_SendString(temp, 5);
		return;
	}
	RecordQueueOut(temp, 256);
	UART_SendString(temp, 256);
}
void ReadVoiceData(unsigned char *dat, unsigned int len)
{
	bit t = 0;
	t = PlayQueueIn(dat, len);
	
	temp[0] = 0x96; temp[1] = 0x38; temp[2] = 0x52; temp[3] = 0x74; temp[4] = 0x14;
	if(t == 1)
		temp[4] = 0x1d;
	UART_SendString(temp, 5);
}
void SystemReset()
{
	VS1003_Reset();
	Uart_Init();
	Timer0_Init();
	Timer1_Init();
	INT0_Init();
	Interrupt_Init();
	VS1003_InitPort();
	QueueReset();
	
	//WatchDogTimerConfig();
	
	temp[0] = 0x96; temp[1] = 0x38; temp[2] = 0x52; temp[3] = 0x74; temp[4] = 0x11;
	UART_SendString(temp, 5);
}
void GetSystemStatu()
{
	unsigned int RecordQueueStatu, PlayQueueStatu;
	RecordQueueStatu = GetRecordQueueStatu();
	PlayQueueStatu = GetPlayQueueStatu();
	temp[0] = 0x96; temp[1] = 0x38; temp[2] = 0x52; temp[3] = 0x74; temp[4] = 0x12;
	temp[5] = RecordQueueStatu >> 8; temp[6] = RecordQueueStatu & 0xff;
	temp[7] = PlayQueueStatu >> 8; temp[8] = PlayQueueStatu & 0xff;
	if(PlayStatu)
		temp[9] = 0x01;
	else if(RecordStatu)
		temp[9] = 0x02;
	else
		temp[9] = 0x00;
	UART_SendString(temp, 10);
}
void ErrorResponse()
{
	temp[0] = 0x96; temp[1] = 0x38; temp[2] = 0x52; temp[3] = 0x74; temp[4] = 0x00;
	temp[5] = 'E'; temp[6] = 'R'; temp[7] = 'R'; temp[8] = 'O'; temp[9] = 'R';
	UART_SendString(temp, 10);
}
void StartPlay()
{
	temp[0] = 0x96; temp[1] = 0x38; temp[2] = 0x52; temp[3] = 0x74; temp[4] = 0x15;
	if(RecordStatu == 0)
		PlayStatu = 1;
	else
		temp[4] = 0x1e;
	UART_SendString(temp, 5);
}
void StopPlay()
{
	PlayStatu = 0;
	VS1003_SoftReset();
	temp[0] = 0x96; temp[1] = 0x38; temp[2] = 0x52; temp[3] = 0x74; temp[4] = 0x16;
	UART_SendString(temp, 5);
}
void UART_Action(unsigned char *dat, unsigned int len)
{
	if(dat[0] == 0x14 && dat[1] == 0x72 && dat[2] == 0x58 && dat[3] == 0x36)
	{
		//WatchDogTimerFeed();
		switch(dat[4])
		{
			case(0x90): VS1003_Beep(0x44);					break;
			case(0x91): SystemReset(); 						break;
			case(0x92): GetSystemStatu(); 					break;
			case(0x93): SendRecordData(); 					break;
			case(0x94): ReadVoiceData(dat + 5, len - 5); 	break;
			case(0x95): StartPlay();						break;
			case(0x96): StopPlay();							break;
			default: 	ErrorResponse();					break;	
		}
	}
	else
		ErrorResponse();
}
void Interrupt_Init()
{
	PS = 1;
	PX0 = 1;
	EA = 1;
}
void Timer0_Init(void)		
{
	AUXR |= 0x80;		
	TMOD &= 0xF0;		
	ET0 = 1;		
	TL0 = 0x5C;		
	TH0 = 0xF7;		
	TF0 = 0;		
	TR0 = 1;		
}
void Timer1_Init(void)		
{
	AUXR &= 0xBF;		
	TMOD &= 0x0F;		
	TL1 = 0x00;		
	TH1 = 0x00;	
	ET1 = 1;
	TF1 = 0;		
	TR1 = 1;		
}

void INT0_Init()
{
	INT0 = 1;
	IT0 = 0;
	EX0 = 1;
}
void main()
{
	SystemReset();
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
}
void Timer0_Interrupt() interrupt 1
{
	UART_RxMonitor(100);
}
void INT0_Interrupt() interrupt 0
{
	if(INT0 == 1)
		RecordStatu = 0;
	else 
	{
		if(PlayStatu == 0)
			RecordStatu = 1;
	}
}