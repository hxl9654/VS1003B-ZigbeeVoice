#include "stc15.h"
unsigned char datPlay[1537] = {0};
unsigned int dataPlayIndexIn = 0;
unsigned int dataPlayIndexOut = 0;
bit PlayQueueIn(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
    for(i = 0; i < len; i++)
    {
        datPlay[dataPlayIndexIn] = p[i];
        dataPlayIndexIn++;
        if(dataPlayIndexIn == 1537)
            dataPlayIndexIn = 0;
        if(dataPlayIndexIn == dataPlayIndexOut)
            return 1;
    }
	return 0;
}
bit PlayQueueOut(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(dataPlayIndexIn == dataPlayIndexOut)
            return 1;
        p[i] = datPlay[dataPlayIndexOut];
        dataPlayIndexOut++;
        if(dataPlayIndexOut == 1537)
            dataPlayIndexOut = 0;        
    }
	return 0;
}
unsigned char GetPlayQueueStatu()
{
	if(dataIndexIn > dataIndexOut)
		return dataIndexIn - dataIndexOut;
	else if(dataIndexIn < dataIndexOut)
		return dataIndexIn + 1537 - dataIndexOut;
	else return 0;
	return 0;
}
unsigned char datRecord[1025] = {0};
unsigned int dataRecordIndexIn = 0;
unsigned int dataRecordIndexOut = 0;
bit RecordQueueIn(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
    for(i = 0; i < len; i++)
    {
        datRecord[dataRecordIndexIn] = p[i];
        dataRecordIndexIn++;
        if(dataRecordIndexIn == 1025)
            dataRecordIndexIn = 0;
        if(dataRecordIndexIn == dataRecordIndexOut)
            return 1;
    }
	return 0;
}
bit RecordQueueOut(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(dataRecordIndexIn == dataRecordIndexOut)
            return 1;
        p[i] = datRecord[dataRecordIndexOut];
        dataRecordIndexOut++;
        if(dataRecordIndexOut == 1025)
            dataRecordIndexOut = 0;        
    }
	return 0;
}
unsigned char GetRecordQueueStatu()
{
	if(dataRecordIndexIn > dataRecordIndexOut)
		return dataRecordIndexIn - dataRecordIndexOut;
	else if(dataRecordIndexIn < dataRecordIndexOut)
		return dataRecordIndexIn + 1025 - dataRecordIndexOut;
	else return 0;
	return 0;
}