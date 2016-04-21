#ifndef _QUEUE_H_
#define _QUEUE_H_

bit PlayQueue_In(unsigned char *p, unsigned int len);
bit PlayQueue_Out(unsigned char *p, unsigned int len);
unsigned int PlayQueue_GetStatu();
void PlayQueue_Reset();

bit RecordQueue_In(unsigned char *p, unsigned int len);
bit RecordQueue_Out(unsigned char *p, unsigned int len);
unsigned int RecordQueue_GetStatu();
void RecordQueue_Reset();

bit UARTQueue_In(unsigned char *p, unsigned int len);
bit UARTQueue_Out(unsigned char *p, unsigned int len);
unsigned int UARTQueue_GetStatu();
void UARTQueue_Reset();

bit UARTQueue_TO_PlayQueue(unsigned int len);
void Queue_Reset();
#endif // _QUEUE_H_
