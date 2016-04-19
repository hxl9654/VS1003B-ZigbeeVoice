#include "stc15.h"
#define PLAYQUEUEDATALENTH 1025
#define RECORDQUEUEDATALENTH 1025
#define UARTQUEUEDATALENTH 1025

unsigned char PlayQueue_Data[PLAYQUEUEDATALENTH] = {0};
unsigned int PlayQueue_Data_IndexIn = 0;
unsigned int PlayQueue_Data_IndexOut = 0;
bit PlayQueue_In(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
    for(i = 0; i < len; i++)
    {
        PlayQueue_Data[PlayQueue_Data_IndexIn] = p[i];
        PlayQueue_Data_IndexIn++;
        if(PlayQueue_Data_IndexIn == PLAYQUEUEDATALENTH)
            PlayQueue_Data_IndexIn = 0;
        if(PlayQueue_Data_IndexIn == PlayQueue_Data_IndexOut)
            return 1;
    }
	return 0;
}
bit PlayQueue_Out(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(PlayQueue_Data_IndexIn == PlayQueue_Data_IndexOut)
            return 1;
        p[i] = PlayQueue_Data[PlayQueue_Data_IndexOut];
        PlayQueue_Data_IndexOut++;
        if(PlayQueue_Data_IndexOut == PLAYQUEUEDATALENTH)
            PlayQueue_Data_IndexOut = 0;        
    }
	return 0;
}
unsigned int PlayQueue_GetStatu()
{
	if(PlayQueue_Data_IndexIn > PlayQueue_Data_IndexOut)
		return PlayQueue_Data_IndexIn - PlayQueue_Data_IndexOut;
	else if(PlayQueue_Data_IndexIn < PlayQueue_Data_IndexOut)
		return PlayQueue_Data_IndexIn + PLAYQUEUEDATALENTH - PlayQueue_Data_IndexOut;
	else return 0;
	return 0;
}
void PlayQueue_Reset()
{
	PlayQueue_Data_IndexIn = 0;
	PlayQueue_Data_IndexOut = 0;
}
unsigned char RecordQueue_Data[RECORDQUEUEDATALENTH] = {0};
unsigned int RecordQueue_Data_IndexIn = 0;
unsigned int RecordQueue_Data_IndexOut = 0;
bit RecordQueue_In(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
    for(i = 0; i < len; i++)
    {
        RecordQueue_Data[RecordQueue_Data_IndexIn] = p[i];
        RecordQueue_Data_IndexIn++;
        if(RecordQueue_Data_IndexIn == RECORDQUEUEDATALENTH)
            RecordQueue_Data_IndexIn = 0;
        if(RecordQueue_Data_IndexIn == RecordQueue_Data_IndexOut)
            return 1;
    }
	return 0;
}
bit RecordQueue_Out(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(RecordQueue_Data_IndexIn == RecordQueue_Data_IndexOut)
            return 1;
        p[i] = RecordQueue_Data[RecordQueue_Data_IndexOut];
        RecordQueue_Data_IndexOut++;
        if(RecordQueue_Data_IndexOut == RECORDQUEUEDATALENTH)
            RecordQueue_Data_IndexOut = 0;        
    }
	return 0;
}
unsigned int RecordQueue_GetStatu()
{
	if(RecordQueue_Data_IndexIn > RecordQueue_Data_IndexOut)
		return RecordQueue_Data_IndexIn - RecordQueue_Data_IndexOut;
	else if(RecordQueue_Data_IndexIn < RecordQueue_Data_IndexOut)
		return RecordQueue_Data_IndexIn + RECORDQUEUEDATALENTH - RecordQueue_Data_IndexOut;
	else return 0;
	return 0;
}
void RecordQueue_Reset()
{
	RecordQueue_Data_IndexIn = 0;
	RecordQueue_Data_IndexOut = 0;
}
unsigned char UARTQueue_Data[UARTQUEUEDATALENTH] = {0};
unsigned int UARTQueue_Data_IndexIn = 0;
unsigned int UARTQueue_Data_IndexOut = 0;
bit UARTQueue_In(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
    for(i = 0; i < len; i++)
    {
        UARTQueue_Data[UARTQueue_Data_IndexIn] = p[i];
        UARTQueue_Data_IndexIn++;
        if(UARTQueue_Data_IndexIn == UARTQUEUEDATALENTH)
            UARTQueue_Data_IndexIn = 0;
        if(UARTQueue_Data_IndexIn == UARTQueue_Data_IndexOut)
            return 1;
    }
	return 0;
}
bit UARTQueue_Out(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(UARTQueue_Data_IndexIn == UARTQueue_Data_IndexOut)
            return 1;
        p[i] = UARTQueue_Data[UARTQueue_Data_IndexOut];
        UARTQueue_Data_IndexOut++;
        if(UARTQueue_Data_IndexOut == UARTQUEUEDATALENTH)
            UARTQueue_Data_IndexOut = 0;        
    }
	return 0;
}
unsigned int UARTQueue_GetStatu()
{
	if(UARTQueue_Data_IndexIn > UARTQueue_Data_IndexOut)
		return UARTQueue_Data_IndexIn - UARTQueue_Data_IndexOut;
	else if(UARTQueue_Data_IndexIn < UARTQueue_Data_IndexOut)
		return UARTQueue_Data_IndexIn + UARTQUEUEDATALENTH - UARTQueue_Data_IndexOut;
	else return 0;
	return 0;
}
void UARTQueue_Reset()
{
	UARTQueue_Data_IndexIn = 0;
	UARTQueue_Data_IndexOut = 0;
}
bit UARTQueue_TO_PlayQueue(unsigned int len)
{
	unsigned int i;
	if(UARTQueue_GetStatu() < len || PLAYQUEUEDATALENTH - PlayQueue_GetStatu() < len)
		return 1;
	for(i = 0; i < len; i++)
	{
		PlayQueue_Data[PlayQueue_Data_IndexIn] = UARTQueue_Data[UARTQueue_Data_IndexOut];
		PlayQueue_Data_IndexIn++;
		if(PlayQueue_Data_IndexIn >= PLAYQUEUEDATALENTH)
			PlayQueue_Data_IndexIn = 0;
		UARTQueue_Data_IndexOut++;
		if(UARTQueue_Data_IndexOut >= UARTQUEUEDATALENTH)
			UARTQueue_Data_IndexOut = 0;
	}
	return 0;
}
bit RecordQueue_TO_UARTQueue(unsigned int len)
{
	unsigned int i;
	if(RecordQueue_GetStatu() < len || UARTQUEUEDATALENTH - UARTQueue_GetStatu() < len)
		return 1;
	for(i = 0; i < len; i++)
	{
		UARTQueue_Data[UARTQueue_Data_IndexIn] = RecordQueue_Data[RecordQueue_Data_IndexOut];
		UARTQueue_Data_IndexIn++;
		if(UARTQueue_Data_IndexIn >= UARTQUEUEDATALENTH)
			UARTQueue_Data_IndexIn = 0;
		RecordQueue_Data_IndexOut++;
		if(RecordQueue_Data_IndexOut >= RECORDQUEUEDATALENTH)
			RecordQueue_Data_IndexOut = 0;
	}
	return 0;
}
void Queue_Reset()
{
	UARTQueue_Reset();
	PlayQueue_Reset();
	RecordQueue_Reset();
}