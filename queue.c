#include "stc15.h"
#define DATAPLAYLENTH 1025
#define DATARECORDLENTH 1025
unsigned char DataPlay[DATAPLAYLENTH] = {0};
unsigned int DataPlayIndexIn = 0;
unsigned int DataPlayIndexOut = 0;
bit PlayQueueIn(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
    for(i = 0; i < len; i++)
    {
        DataPlay[DataPlayIndexIn] = p[i];
        DataPlayIndexIn++;
        if(DataPlayIndexIn == DATAPLAYLENTH)
            DataPlayIndexIn = 0;
        if(DataPlayIndexIn == DataPlayIndexOut)
            return 1;
    }
	return 0;
}
bit PlayQueueOut(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(DataPlayIndexIn == DataPlayIndexOut)
            return 1;
        p[i] = DataPlay[DataPlayIndexOut];
        DataPlayIndexOut++;
        if(DataPlayIndexOut == DATAPLAYLENTH)
            DataPlayIndexOut = 0;        
    }
	return 0;
}
unsigned int GetPlayQueueStatu()
{
	if(DataPlayIndexIn > DataPlayIndexOut)
		return DataPlayIndexIn - DataPlayIndexOut;
	else if(DataPlayIndexIn < DataPlayIndexOut)
		return DataPlayIndexIn + DATAPLAYLENTH - DataPlayIndexOut;
	else return 0;
	return 0;
}
void PlayQueueReset()
{
	DataPlayIndexIn = 0;
	DataPlayIndexOut = 0;
}
unsigned char DataRecord[DATARECORDLENTH] = {0};
unsigned int DataRecordIndexIn = 0;
unsigned int DataRecordIndexOut = 0;
bit RecordQueueIn(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
    for(i = 0; i < len; i++)
    {
        DataRecord[DataRecordIndexIn] = p[i];
        DataRecordIndexIn++;
        if(DataRecordIndexIn == DATARECORDLENTH)
            DataRecordIndexIn = 0;
        if(DataRecordIndexIn == DataRecordIndexOut)
            return 1;
    }
	return 0;
}
bit RecordQueueOut(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(DataRecordIndexIn == DataRecordIndexOut)
            return 1;
        p[i] = DataRecord[DataRecordIndexOut];
        DataRecordIndexOut++;
        if(DataRecordIndexOut == DATARECORDLENTH)
            DataRecordIndexOut = 0;        
    }
	return 0;
}
unsigned int GetRecordQueueStatu()
{
	if(DataRecordIndexIn > DataRecordIndexOut)
		return DataRecordIndexIn - DataRecordIndexOut;
	else if(DataRecordIndexIn < DataRecordIndexOut)
		return DataRecordIndexIn + DATARECORDLENTH - DataRecordIndexOut;
	else return 0;
	return 0;
}
void RecordQueueReset()
{
	DataRecordIndexIn = 0;
	DataRecordIndexOut = 0;
}
void QueueReset()
{
	RecordQueueReset();
	PlayQueueReset();
}