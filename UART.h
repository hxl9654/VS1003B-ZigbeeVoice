#ifndef _HEAD_UART_
#define _HEAD_UART_

#include <intrins.h>
#include <config.h>
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：UART_Conf
*函数功能：配置串口
*参数列表：
*   baud
*       参数类型：unsigned char型数据
*       参数描述：要设置的波特率
*////////////////////////////////////////////////////////////////////////////////////
void UART_Conf(unsigned int baud);
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
*////////////////////////////////////////////////////////////////////////////////////
void UART_SendString(unsigned char *dat,unsigned int len);
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：UART_RxMonitor
*函数功能：串口字符串接收结束判断，在定时器0中断函数中调用
*参数列表：
*   ms
*       参数类型：unsigned char型数据
*       参数描述：定时器延时时长（单位：ms）
*////////////////////////////////////////////////////////////////////////////////////
void UART_RxMonitor(unsigned char ms);
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：UART_Driver
*函数功能：串口通信监控函数，在主循环中调用。
*           如果接收到字符串，会自动调用另行编写的UART_Action(unsigned char *dat,unsigned char len)
*////////////////////////////////////////////////////////////////////////////////////
void UART_Driver();


#endif // _HEAD_UART_
