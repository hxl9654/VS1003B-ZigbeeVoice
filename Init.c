#include "stc15.h"
#include "vs1003.h"
#include "queue.h"
#include "uart.h"
sbit INT0 = P3 ^ 2;

void Interrupt_Init()
{
	PS = 1;
	//PX0 = 1;
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
void SystemReset()
{
	VS1003_Reset();
	VS1003_InitPort();
	QueueReset();
}
void SystemInit()
{
	VS1003_Reset();
	Uart_Init();
	Timer0_Init();
	Timer1_Init();
	INT0_Init();
	Interrupt_Init();
	VS1003_InitPort();
	QueueReset();
	VS1003_Beep(0x44);
	//WatchDogTimerConfig();
}
