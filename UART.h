#ifndef _HEAD_UART_
#define _HEAD_UART_

#include <intrins.h>
#include <config.h>
/*///////////////////////////////////////////////////////////////////////////////////
*��������UART_Conf
*�������ܣ����ô���
*�����б���
*   baud
*       �������ͣ�unsigned char������
*       ����������Ҫ���õĲ�����
*////////////////////////////////////////////////////////////////////////////////////
void UART_Conf(unsigned int baud);
/*///////////////////////////////////////////////////////////////////////////////////
*��������UART_SendString
*�������ܣ��򴮿ڷ���һ���ַ���
*�����б���
*   *dat
*       �������ͣ�unsigned char��ָ��
*       ����������Ҫ���͵��ַ������׵�ַ
*   len
*       �������ͣ�unsigned char������
*       ����������Ҫ���͵��ַ����ĳ���
*////////////////////////////////////////////////////////////////////////////////////
void UART_SendString(unsigned char *dat,unsigned char len);
/*///////////////////////////////////////////////////////////////////////////////////
*��������UART_RxMonitor
*�������ܣ������ַ������ս����жϣ��ڶ�ʱ��0�жϺ����е���
*�����б���
*   ms
*       �������ͣ�unsigned char������
*       ������������ʱ����ʱʱ������λ��ms��
*////////////////////////////////////////////////////////////////////////////////////
void UART_RxMonitor(unsigned char ms);
/*///////////////////////////////////////////////////////////////////////////////////
*��������UART_Driver
*�������ܣ�����ͨ�ż�غ���������ѭ���е��á�
*           ������յ��ַ��������Զ��������б�д��UART_Action(unsigned char *dat,unsigned char len)
*////////////////////////////////////////////////////////////////////////////////////
void UART_Driver();


#endif // _HEAD_UART_