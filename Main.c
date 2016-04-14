#include <reg52.h>
#include <intrins.h>
#include "UART.h"

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define true 1
#define flase 0
sfr AUXR = 0x8E;

void InitPortVS1003(void);	
void TestVS1003B(void);
void VS1003BRecord();
void VS1003_PlayFile(unsigned char *dat, unsigned int len) ;

void UART_Action(unsigned char *dat, unsigned int len)
{
	VS1003_PlayFile(dat, len);
}
void Timer0Init(void)		
{
	AUXR &= 0x7F;		
	TMOD &= 0xF0;		
	TMOD |= 0x01;
	ET0 = 1;	
	EA = 1;
	TL0 = 0x00;		
	TH0 = 0x94;		
	TF0 = 0;		
	TR0 = 1;		
}

sbit P10 = P1 ^ 0;
void main()
{
	UART_Conf(11520);
	Timer0Init();
	InitPortVS1003();
	while(1)
	{		
		P10 = ~P10;
		//VS1003BRecord();
		//TestVS1003B();
		UART_Driver();
	}
}
void Timer0_Interrupt() interrupt 1
{
	TL0 = 0x00;		
	TH0 = 0x94;	
	UART_RxMonitor(30);
}
