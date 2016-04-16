#ifndef _QUEUE_H_
#define _QUEUE_H_
bit PlayQueueOut(unsigned char *p, unsigned int len);
bit PlayQueueIn(unsigned char *p, unsigned int len);
unsigned int GetPlayQueueStatu();
void PlayQueueReset();
bit RecordQueueOut(unsigned char *p, unsigned int len);
bit RecordQueueIn(unsigned char *p, unsigned int len);
unsigned int GetRecordQueueStatu();
void RecordQueueReset();
void QueueReset();
#endif // _QUEUE_H_
