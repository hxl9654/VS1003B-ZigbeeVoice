#include "stc15.h"
#include <intrins.h>
#include "UART.h"
#include "queue.h"

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define true 1
#define flase 0

void InitPortVS1003(void);	
void TestVS1003B(void);
void VS1003BRecord();
void VS1003_Play() ;
void Mp3Reset();
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
	}
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
	Mp3Reset();
	UART_Conf(11520);
	Timer0Init();
	InitPortVS1003();
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
	temp[0] = 'E'; temp[1] = 'R'; temp[2] = 'R'; temp[3] = 'O'; temp[4] = 'R';
	temp[5] = ' '; temp[6] = ' '; temp[7] = 0x40; temp[8] = 0x30; temp[9] = '\n';
	UART_SendString(temp, 10);
}
void UART_Action(unsigned char *dat, unsigned int len)
{
	if(dat[0] == 0x14 && dat[1] == 0x72 && dat[2] == 0x58 && dat[3] == 0x36)
	{
		//WatchDogTimerFeed();
		switch(dat[4])
		{
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
void Timer0Init(void)		
{
	AUXR |= 0x80;		
	TMOD &= 0xF0;		
	ET0 = 1;	
	EA = 1;
	TL0 = 0x5C;		
	TH0 = 0xF7;		
	TF0 = 0;		
	TR0 = 1;		
}

void main()
{
	SystemReset();
	while(1)
	{				
		UART_Driver();
		VS1003_Play();
	}
}
void Timer0_Interrupt() interrupt 1
{
	//P11 = ~ P11;
	UART_RxMonitor(100);
}
