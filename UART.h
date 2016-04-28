#ifndef _HEAD_UART_
#define _HEAD_UART_

#include <intrins.h>
#include <config.h>
/*///////////////////////////////////////////////////////////////////////////////////
*函数名：Uart_Init
*函数功能：配置串口
*////////////////////////////////////////////////////////////////////////////////////
void Uart_Init();
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
*函数名：UART_Driver
*函数功能：串口通信监控函数，在主循环中调用。
*           如果接收到字符串，会自动调用另行编写的UART_Action(unsigned char *dat,unsigned char len)
*////////////////////////////////////////////////////////////////////////////////////
void UART_Driver() small reentrant;


#endif // _HEAD_UART_
