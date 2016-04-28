#include "stc15.h"
#include "UART.h"
#include "queue.h"

extern void UART_Action(unsigned char dat, unsigned char len);

bit UART_SendFlag = 0;                  //串口发送完成标志
bit UART_PendingDataFlag = 0;			//串口接收到数据但未接收完成标准

void Uart_Init(void)		//115200bps@33.1776MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xB8;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	EA = 1;
	ES = 1;
}

//向串口发送一个字符串
void UART_SendString(unsigned char *dat, unsigned int len)
{
	while(len)
	{
		len --;                     //每发送一位，长度减1
		SBUF = *dat;                //发送一位数据
		dat ++;                     //数据指针移向下一位
		while(! UART_SendFlag);     //等待串口发送完成标志
		UART_SendFlag = 0;          //清空串口发送完成标志
	}
}


//串口通信监控函数，在主循环中调用。
//如果接收到字符串，会自动调用另行编写的UART_Action
bit UARTDriver_WorkingFlag = 0;
data unsigned char UART_Driver_Temp[5] = {0};
data unsigned char UART_Temp[2] = {0};
void UART_Driver() small reentrant
{
	unsigned int i;
	unsigned int len_buff;                      	//缓冲区中数据的长度
	if(UARTDriver_WorkingFlag)
		return;
	UARTDriver_WorkingFlag = 1;
	len_buff = UARTQueue_GetStatu();  		//得到已缓存数据的长度
	if(UART_PendingDataFlag)
	{
		if(len_buff >= 256)
		{			
			UART_Action(UART_Temp[0], UART_Temp[1]);
			
			UART_PendingDataFlag = 0;
			len_buff = UARTQueue_GetStatu();
		}
		else {UARTDriver_WorkingFlag = 0; return;}
	}
	if(len_buff >= 7)
	{
		for(i = 0; i < len_buff; i++)
		{
			UART_Driver_Temp[0] = UART_Driver_Temp[1]; UART_Driver_Temp[1] = UART_Driver_Temp[2]; UART_Driver_Temp[2] = UART_Driver_Temp[3]; UART_Driver_Temp[3] = UART_Driver_Temp[4];
			UARTQueue_Out(UART_Driver_Temp + 4, 1);
			if(UART_Driver_Temp[0] == 0x14 && UART_Driver_Temp[1] == 0x72 && UART_Driver_Temp[2] == 0x58 && UART_Driver_Temp[3] == 0x36 && UART_Driver_Temp[4] == 0x90)
			{
				UARTQueue_Out(UART_Temp, 2);
				if(UART_Temp[1] == 0x01 && UARTQueue_GetStatu() < 256)
				{
					UART_PendingDataFlag = 1;
					UARTDriver_WorkingFlag = 0;
					return ;
				}					
				UART_Action(UART_Temp[0], UART_Temp[1]);    //调用UART_Action函数，将接收到的数据及数据长度作为参数
			}
		}		
	}
	UARTDriver_WorkingFlag = 0;
}

void interrupt_UART() interrupt 4
{
	unsigned char dat;
	if(TI)                                  //如果串口发送完成
	{
		TI = 0;                             //清空系统标志位
		UART_SendFlag = 1;                  //设置串口发送完成标志
	}
	else if(RI)                             //如果串口接收完成
	{
		RI = 0;                             //清空系统标志位
		dat = SBUF;
		UARTQueue_In(&dat, 1);			    //将接收到的数据放到串口缓冲队列
	}
}
