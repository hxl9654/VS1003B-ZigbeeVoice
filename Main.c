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

void UART_Action(unsigned char *dat, unsigned int len)
{
	//P11 = ~ P11;
	QueueIn(dat, len);
}
void Timer0Init(void)		
{
	AUXR |= 0x80;		
	TMOD &= 0xF0;		
	ET0 = 1;	
	EA = 1;
	TL0 = 0x9A;		
	TH0 = 0xA9;		
	TF0 = 0;		
	TR0 = 1;		
}

void main()
{
	Mp3Reset();
	UART_Conf(11520);
	Timer0Init();
	InitPortVS1003();
	while(1)
	{				
		UART_Driver();
		VS1003_Play();
	}
}
void Timer0_Interrupt() interrupt 1
{
	//P11 = ~ P11;
	UART_RxMonitor(1);
}
