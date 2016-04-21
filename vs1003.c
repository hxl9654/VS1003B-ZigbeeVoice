#include <intrins.h>
#include "vs1003.h"
#include "queue.h"
#include "UART.h"
sbit VS1003_XRESET  = P2^0;
sbit VS1003_DREQ = P2^5;
sbit VS1003_XCS = P2^6;
sbit VS1003_XDCS  = P2^7;

//sbit VS1003_XRESET  = P2^4;
//sbit VS1003_XCS = P2^5;
//sbit VS1003_XDCS  = P2^6;
//sbit VS1003_DREQ = P2^7;
void Delay(unsigned int ms)		
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

//vs1003�ӿڵĳ�ʼ��
void VS1003_InitPort(void)
{
	VS1003_DREQ = 1;		
	VS1003_XRESET = 1;
	VS1003_XCS = 1;
	VS1003_XDCS = 1;
}
//��ʼ��SPI
void SPI_Init()
{
    SPDAT = 0;                  
    SPSTAT = 0xC0;       
    SPCTL = 0xDE;      
}
//ͨ��SPI����һ���ֽڵ�����
void SPI_SendByte(unsigned char SendData)
{      
	SPDAT = SendData;                
    while (!(SPSTAT & 0x80));  
    SPSTAT = 0xC0;
}
//��SPI�ӿڽ���һ���ֽ�	
unsigned char SPI_ReadByte(void)
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
	SPI_SendByte(VS_WRITE_COMMAND); //����д�Ĵ�������
	SPI_SendByte(addressbyte);      //���ͼĴ����ĵ�ַ
	SPI_SendByte(highbyte);         //���ʹ�д���ݵĸ�8λ
	SPI_SendByte(lowbyte);          //���ʹ�д���ݵĵ�8λ
	VS1003_XCS = 1;
}

//��vs1003�Ĵ���
unsigned int VS1003_ReadRegister(unsigned char addressbyte)
{
	unsigned int resultvalue = 0;
	unsigned char ucReadValue;

	VS1003_XCS = 1;
	VS1003_XCS = 0;
	SPI_SendByte(VS_READ_COMMAND); //���Ͷ��Ĵ�������
	SPI_SendByte(addressbyte);	 //���ͼĴ����ĵ�ַ

	ucReadValue = SPI_ReadByte();
	resultvalue = ucReadValue<<8;

	ucReadValue = SPI_ReadByte();
	resultvalue |= ucReadValue;

	VS1003_XCS = 1;
	return resultvalue;                 //����16λ�Ĵ�����ֵ
}

//vs1003�����λ
void VS1003_SoftReset(void)
{
	VS1003_WriteRegister (SCI_MODE, 0x0c04); //�����λ

	Delay(1); //��ʱ1ms
	while (VS1003_DREQ == 0); //�ȴ������λ����
	VS1003_WriteRegister(SCI_CLOCKF, 0x4430);
	VS1003_SetVolume(0x1414);//��������
    Delay(1); //��ʱ1ms
    	
    //��vs1003����4���ֽ���Ч���ݣ���������SPI����
   	VS1003_XDCS = 0;
	SPI_SendByte(0);
	SPI_SendByte(0);
	SPI_SendByte(0);
	SPI_SendByte(0);
	VS1003_XDCS = 1;

}
//vs1003Ӳ����λ
void VS1003_Reset(void)
{	
	VS1003_XRESET = 0;// ��λvs1003      
	SPI_Init();
	Delay(10);//��ʱ10ms
	SPI_SendByte(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	VS1003_XCS = 1;   
	VS1003_XDCS = 1;    
	VS1003_XRESET =1; 

	while (VS1003_DREQ == 0);//�ȴ�DREQΪ��
    VS1003_SoftReset();//vs1003��λ
}

//����������
void VS1003_Beep(unsigned char f)
{
	VS1003_Reset();  
	VS1003_SetVolume(0x2020);
 	VS1003_WriteRegister(SCI_MODE,0x0c24);//����vs1003�Ĳ���ģʽ
	while (VS1003_DREQ == 0);     //�ȴ�DREQΪ��
 	VS1003_XDCS = 0;      //ѡ��vs1003�����ݽӿ�
 	
 	//��vs1003�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n��vs1003�����������Ҳ���Ƶ��ֵ��������㷽����vs1003��datasheet
   	SPI_SendByte(0x53);      
	SPI_SendByte(0xef);      
	SPI_SendByte(0x6e);      
	SPI_SendByte(f);      
	SPI_SendByte(0x00);      
	SPI_SendByte(0x00);
	SPI_SendByte(0x00);
	SPI_SendByte(0x00);
	Delay(250);
	VS1003_XDCS = 1;//����ִ�е������Ӧ���ܴӶ�������һ����һƵ�ʵ�����
  
    //�˳����Ҳ���
	VS1003_XDCS = 0;
	SPI_SendByte(0x45);
	SPI_SendByte(0x78);
	SPI_SendByte(0x69);
	SPI_SendByte(0x74);
	SPI_SendByte(0x00);
	SPI_SendByte(0x00);
	SPI_SendByte(0x00);
	SPI_SendByte(0x00);

	VS1003_XDCS = 1;
	VS1003_SoftReset();
}

//д���ݣ���������
void VS1003_WriteDAT(unsigned char dat)
{
	VS1003_XDCS = 0;
	SPI_SendByte(dat);
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

data unsigned char VS1003_Play_Temp[32] = {0};
void VS1003_Play() 
{
	unsigned char i = 0;
	while(VS1003_DREQ)
	{		
		if(PlayQueue_GetStatu() < 32)
			return;
		if(PlayQueue_Out(VS1003_Play_Temp, 32) == 1)
			return;
		for(i = 0; i < 32; i++)
			VS1003_WriteDAT(VS1003_Play_Temp[i]);
	}
}
//ͨ��vs1003�ɼ�����
extern bit RecordStatu;
unsigned char db[256] = {0};
void VS1003_Record()
{
	unsigned int wwwww = 0, idx = 0, i, j;
	VS1003_SetVolume(0x1414); /* Recording monitor volume */
	VS1003_WriteRegister(SCI_BASS, 0); /* Bass/treble disabled */
	VS1003_WriteRegister(SCI_CLOCKF, 0x4430); /* 2.0x 12.288MHz */
	Delay(5);
	VS1003_WriteRegister(SCI_AICTRL0, 12); /* Div -> 12=8kHz 8=12kHz 6=16kHz */

	VS1003_WriteRegister(SCI_AICTRL1, 0); /* Auto gain */
	Delay(1);
	
	//VS1003_WriteRegister(SCI_MODE, 0x5c04); //linein
	VS1003_WriteRegister(SCI_MODE, 0x1c04); 	//mic
	/* Record loop */
	while (RecordStatu) {
		while(wwwww < 256 || wwwww > 896)
		{
			wwwww = VS1003_ReadRegister(SCI_HDAT1);	
			//if(!RecordStatu)return ;
			UART_Driver();
		} /* Delay until 512 bytes available */

		ET1 = 0;
		for(j = 0; j < 2; j++)
		{
			for(i = 0; i < 128; i++)
			{
				wwwww = VS1003_ReadRegister(SCI_HDAT0);
				db[i * 2] = wwwww >> 8;
				db[i * 2 + 1] = wwwww & 0xFF;			
			}
			RecordQueue_In(db, 256);
		}
		ET1 = 1;
	}
	VS1003_Reset();
}
