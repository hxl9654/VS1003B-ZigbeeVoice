#ifndef _QUEUE_H_
#define _QUEUE_H_
bit PlayQueueOut(unsigned char *p, unsigned int len);
bit PlayQueueIn(unsigned char *p, unsigned int len);
unsigned char GetPlayQueueStatu();
bit RecordQueueOut(unsigned char *p, unsigned int len);
bit RecordQueueIn(unsigned char *p, unsigned int len);
unsigned char GetRecordQueueStatu();
#endif // _QUEUE_H_
