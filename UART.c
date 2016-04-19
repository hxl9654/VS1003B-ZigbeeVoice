#include "stc15.h"
#include "UART.h"
#include "queue.h"

extern void UART_Action(unsigned char dat, unsigned char len);
//此函数须另行编写：当串口完成一个字符串结束后会自动调用

unsigned char UART_Temp[2] = {0};
bit UART_SendFlag = 0;                  //串口发送完成标志
bit UART_PendingDataFlag = 0;			//串口接收到数据但未接收完成标准
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：Uart_Init
*函数功能：配置串口
*返回值：无
*版本：1.0
*作者：何相龙
*日期：2014年12月9日
*////////////////////////////////////////////////////////////////////////////////////
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

/*///////////////////////////////////////////////////////////////////////////////////
*函数名：UART_SendString
*函数功能：向串口发送一个字符串
*参数列表：
*   *dat
*       参数类型：unsigned char型指针
*       参数描述：要发送的字符串的首地址
*   len
*       参数类型：unsigned char型数据
*       参数描述：要发送的字符串的长度
*返回值：无
*版本：1.0
*作者：何相龙
*日期：2014年12月9日
*////////////////////////////////////////////////////////////////////////////////////
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
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：UART_Driver
*函数功能：串口通信监控函数，在主循环中调用。
*           如果接收到字符串，会自动调用另行编写的UART_Action(unsigned char *dat,unsigned char len)
*参数列表：
*   无
*返回值：无
*版本：1.0
*作者：何相龙
*日期：2014年12月9日
*////////////////////////////////////////////////////////////////////////////////////
void UART_Driver()
{
	unsigned int i;
	unsigned int len_buff;                      	//缓冲区中数据的长度
	data unsigned char temp[5] = {0};
	ET1 = 0;
	P34=0;
	len_buff = UARTQueue_GetStatu();  		//得到已缓存数据的长度
	if(UART_PendingDataFlag)
	{
		if(len_buff >= 256)
		{			
			UART_Action(UART_Temp[0], UART_Temp[1]);
			
			UART_PendingDataFlag = 0;
			len_buff = UARTQueue_GetStatu();
		}
		else {P34=1;ET1 = 1;return;}
	}
	if(len_buff >= 7)
	{
		for(i = 0; i < len_buff; i++)
		{
			temp[0] = temp[1]; temp[1] = temp[2]; temp[2] = temp[3]; temp[3] = temp[4];
			UARTQueue_Out(temp + 4, 1);
			if(temp[0] == 0x14 && temp[1] == 0x72 && temp[2] == 0x58 && temp[3] == 0x36 && temp[4] == 0x90)
			{
				UARTQueue_Out(UART_Temp, 2);
				if(UART_Temp[1] == 0x01 && UARTQueue_GetStatu() < 256)
				{
					UART_PendingDataFlag = 1;
					P34=1; ET1 = 1;
					return ;
				}					
				UART_Action(UART_Temp[0], UART_Temp[1]);    //调用UART_Action函数，将接收到的数据及数据长度作为参数
			}
		}		
	}P34=1;
	ET1 = 1;
}
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：interrupt_UART
*函数功能：串口中断函数
*参数列表：
*   无
*返回值：无
*版本：1.0
*作者：何相龙
*日期：2014年12月9日
*////////////////////////////////////////////////////////////////////////////////////
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
