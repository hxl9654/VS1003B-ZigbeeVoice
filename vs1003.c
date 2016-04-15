#include "vs1003.h"
#include "UART.h"
#include "queue.h"

#include <intrins.h>


sbit VS1003_XRESET  = P3^2;
sbit VS1003_XCS = P3^3;
sbit VS1003_XDCS  = P3^4;
sbit VS1003_DREQ = P3^5;
//sbit c_SPI_SI = P1^3;
//sbit c_SPI_SO = P1^4;
//sbit c_SPI_CLK = P1^5;

void Delay(unsigned int ms)		//@22.1184MHz
{
	unsigned char i, j;
	while(ms--)
	{
		i = 22;
		j = 128;
		do
		{
			while (--j);
		} while (--i);
	}
}


/**********************************************************/
/*  �������� :  InitPortVS1003                            */
/*  �������� �� MCU��vs1003�ӿڵĳ�ʼ��                   */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void VS1003_InitPort(void)
{
	VS1003_DREQ = 1;		
	VS1003_XRESET = 1;
	VS1003_XCS = 1;
	VS1003_XDCS = 1;
}
//��ʼ��SPI
void InitSPI()
{
    SPDAT = 0;                  
    SPSTAT = 0xC0;       
    SPCTL = 0xDD;      
}
//ͨ��SPI����һ���ֽڵ�����
void SPIPutChar(unsigned char SendData)
{      
	SPDAT = SendData;                
    while (!(SPSTAT & 0x80));  
    SPSTAT = 0xC0;
}
//��SPI�ӿڽ���һ���ֽ�	
unsigned char SPI_RecByte(void)
{
	SPDAT = 0xFF;                
    while (!(SPSTAT & 0x80));  
    SPSTAT = 0xC0;
	return SPDAT;
}
//�������� �� дvs1003�Ĵ���
void VS1003_WriteRegister(unsigned char addressbyte, unsigned int databyte)
{
	unsigned char lowbyte, highbyte;
	lowbyte = databyte & 0xff;
	highbyte = databyte >> 8;
	
	VS1003_XCS = 1;
	VS1003_XCS = 0;
	SPIPutChar(VS_WRITE_COMMAND); //����д�Ĵ�������
	SPIPutChar(addressbyte);      //���ͼĴ����ĵ�ַ
	SPIPutChar(highbyte);         //���ʹ�д���ݵĸ�8λ
	SPIPutChar(lowbyte);          //���ʹ�д���ݵĵ�8λ
	VS1003_XCS = 1;
}

//��vs1003�Ĵ���
unsigned int VS1003_ReadRegister(unsigned char addressbyte)
{
	unsigned int resultvalue = 0;
	unsigned char ucReadValue;

	VS1003_XCS = 1;
	VS1003_XCS = 0;
	SPIPutChar(VS_READ_COMMAND); //���Ͷ��Ĵ�������
	SPIPutChar(addressbyte);	 //���ͼĴ����ĵ�ַ

	ucReadValue = SPI_RecByte();
	resultvalue = ucReadValue<<8;

	ucReadValue = SPI_RecByte();
	resultvalue |= ucReadValue;

	VS1003_XCS = 1;
	return resultvalue;                 //����16λ�Ĵ�����ֵ
}

//vs1003�����λ
void VS1003_SoftReset(void)
{
	VS1003_WriteRegister (SPI_MODE, 0x0c04); //�����λ

	Delay(1); //��ʱ1ms
	while (VS1003_DREQ == 0); //�ȴ������λ����
	VS1003_WriteRegister(SPI_CLOCKF, 0x4430);
	VS1003_SetVolume(0x1414);//��������
    Delay(1); //��ʱ1ms
    	
    //��vs1003����4���ֽ���Ч���ݣ���������SPI����
   	VS1003_XDCS = 0;
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	VS1003_XDCS = 1;

}
//vs1003Ӳ����λ
void VS1003_Reset(void)
{	
	VS1003_XRESET = 0;// ��λvs1003      
	InitSPI();
	Delay(100);//��ʱ100ms
	SPIPutChar(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	VS1003_XCS = 1;   
	VS1003_XDCS = 1;    
	VS1003_XRESET =1; 
	Delay(100);            //��ʱ100ms
	while (VS1003_DREQ == 0);//�ȴ�DREQΪ��
    VS1003_SoftReset();//vs1003��λ
}

//����������
void VS1003_Beep(unsigned char f)
{
	VS1003_Reset();  
 	VS1003_WriteRegister(SPI_MODE,0x0c24);//����vs1003�Ĳ���ģʽ
	while (VS1003_DREQ == 0);     //�ȴ�DREQΪ��
 	VS1003_XDCS = 0;      //ѡ��vs1003�����ݽӿ�
 	
 	//��vs1003�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n��vs1003�����������Ҳ���Ƶ��ֵ��������㷽����vs1003��datasheet
   	SPIPutChar(0x53);      
	SPIPutChar(0xef);      
	SPIPutChar(0x6e);      
	SPIPutChar(f);      
	SPIPutChar(0x00);      
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	Delay(250);
	Delay(250);
	VS1003_XDCS = 1;//����ִ�е������Ӧ���ܴӶ�������һ����һƵ�ʵ�����
  
    //�˳����Ҳ���
	VS1003_XDCS = 0;
	SPIPutChar(0x45);
	SPIPutChar(0x78);
	SPIPutChar(0x69);
	SPIPutChar(0x74);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);

	VS1003_XDCS = 1;
	VS1003_SoftReset();
}

//д���ݣ���������
void VS1003_WriteDAT(unsigned char dat)
{
	VS1003_XDCS = 0;
	SPIPutChar(dat);
	VS1003_XDCS = 1;
	VS1003_XCS = 1;
}

void VS1003_Fill2048Zero()
{
	unsigned char i,j;

	for(i=0;i<64;i++)
	{
		if(VS1003_DREQ)
		{
			VS1003_XDCS = 0;
			for(j=0;j<32;j++)
				VS1003_WriteDAT(0x00);
			VS1003_XDCS = 1;
		}
	}
}

unsigned char VS1003_Play_Temp[32] = {0};
void VS1003_Play() 
{
	unsigned char i = 0;
	while(VS1003_DREQ)
	{		
		//P11 = ~ P11;
		if(PlayQueueOut(VS1003_Play_Temp, 32) == 1)
			return;
		for(i = 0; i < 32; i++)
			VS1003_WriteDAT(VS1003_Play_Temp[i]);
	}
}
//ͨ��vs1003�ɼ�����
unsigned char db[256] = {0};
void VS1003_Record()
{
	unsigned int wwwww = 0, idx = 0, i;
	VS1003_SetVolume(0x1414); /* Recording monitor volume */
	VS1003_WriteRegister(SPI_BASS, 0); /* Bass/treble disabled */
	VS1003_WriteRegister(SPI_CLOCKF, 0x4430); /* 2.0x 12.288MHz */
	Delay(100);
	VS1003_WriteRegister(SPI_AICTRL0, 12); /* Div -> 12=8kHz 8=12kHz 6=16kHz */
	Delay(100);
	VS1003_WriteRegister(SPI_AICTRL1, 0); /* Auto gain */
	Delay(100);
	
	//VS1003_WriteRegister(SPI_MODE, 0x5c04); //linein
	VS1003_WriteRegister(SPI_MODE, 0x1c04); 	//mic
	/* Record loop */
	while (!P37) {
		while(1)
		{
			wwwww = VS1003_ReadRegister(SPI_HDAT1);	
			if(wwwww < 256)P10 = 0;
			else if(wwwww >= 896)P11 = 0;
			else break;	
		} /* Delay until 512 bytes available */
		P10 = 1;
		P11 = 1;	
		
		for(i = 0; i < 128; i++)
		{
			wwwww = VS1003_ReadRegister(SPI_HDAT0);
			db[i * 2] = wwwww >> 8;
			db[i * 2 + 1] = wwwww & 0xFF;			
		}
		RecordQueueIn(db, 256);
		for(i = 0; i < 128; i++)
		{
			wwwww = VS1003_ReadRegister(SPI_HDAT0);
			db[i * 2] = wwwww >> 8;
			db[i * 2 + 1] = wwwww & 0xFF;			
		}
		RecordQueueIn(db, 256);
		
	}
	VS1003_Reset();
}
