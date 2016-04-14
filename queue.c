#include<reg52.h>
unsigned char dat[2048] = {0};
unsigned int dataIndexIn = 0;
unsigned int dataIndexOut = 0;
sbit P11 = P1 ^ 1;
bit QueueIn(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;	
	//P11 = ~ P11;
    for(i = 0; i < len; i++)
    {
		//P11 = ~ P11;
        dat[dataIndexIn] = p[i];
        dataIndexIn++;
        if(dataIndexIn == 2048)
            dataIndexIn = 0;
        if(dataIndexIn == dataIndexOut)
            return 1;
    }
	return 0;
}
bit QueueOut(unsigned char *p, unsigned int len)
{
    unsigned int i = 0;
    for(i = 0; i < len; i++)
    {
		if(dataIndexIn == dataIndexOut)
            return 1;
        p[i] = dat[dataIndexOut];
        dataIndexOut++;
        if(dataIndexOut == 2048)
            dataIndexOut = 0;        
    }
	return 0;
}
