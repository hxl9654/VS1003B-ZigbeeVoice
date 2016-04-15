#include "vs1003.h"
#include "UART.h"
#include "queue.h"

#include <intrins.h>


sbit  MP3_XRESET  = P3^2;
sbit MP3_XCS = P3^3;
sbit MP3_XDCS  = P3^4;
sbit MP3_DREQ = P3^5;
sbit c_SPI_SI = P1^3;
sbit c_SPI_SO = P1^4;
sbit c_SPI_CLK = P1^5;

void wait(unsigned int ms)		//@22.1184MHz
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
void InitPortVS1003(void)
{
	c_SPI_SO = 1;
	MP3_DREQ = 1;		
	MP3_XRESET = 1;
	MP3_XCS = 1;
	MP3_XDCS = 1;
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
void Mp3WriteRegister(unsigned char addressbyte, unsigned int databyte)
{
	unsigned char lowbyte, highbyte;
	lowbyte = databyte & 0xff;
	highbyte = databyte >> 8;
	
	MP3_XCS = 1;
	MP3_XCS = 0;
	SPIPutChar(VS_WRITE_COMMAND); //����д�Ĵ�������
	SPIPutChar(addressbyte);      //���ͼĴ����ĵ�ַ
	SPIPutChar(highbyte);         //���ʹ�д���ݵĸ�8λ
	SPIPutChar(lowbyte);          //���ʹ�д���ݵĵ�8λ
	MP3_XCS = 1;
}
/*************************************************************/
/*  �������� :  Mp3ReadRegister                              */
/*  �������� �� ��vs1003�Ĵ���                               */
/*  ����     :  �Ĵ�����ַ				     */
/*  ����ֵ   :  vs1003��16λ�Ĵ�����ֵ                       */
/*-----------------------------------------------------------*/
unsigned int Mp3ReadRegister(unsigned char addressbyte)
{
	unsigned int resultvalue = 0;
	unsigned char ucReadValue;

	MP3_XCS = 1;
	MP3_XCS = 0;
	SPIPutChar(VS_READ_COMMAND); //���Ͷ��Ĵ�������
	SPIPutChar(addressbyte);	 //���ͼĴ����ĵ�ַ

	ucReadValue = SPI_RecByte();
	resultvalue = ucReadValue<<8;

	ucReadValue = SPI_RecByte();
	resultvalue |= ucReadValue;
	//SBUF = resultvalue >> 8 ;
	MP3_XCS = 1;
	//resultvalue = 0x3456;
	return resultvalue;                 //����16λ�Ĵ�����ֵ
}
/**********************************************************/
/*  �������� :  Mp3SoftReset                              */
/*  �������� �� vs1003�����λ                            */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void Mp3SoftReset(void)
{
	Mp3WriteRegister (SPI_MODE, 0x0c04); //�����λ

	wait(1); //��ʱ1ms
	while (MP3_DREQ == 0); //�ȴ������λ����
	Mp3WriteRegister(SPI_CLOCKF, 0x4430);
	Mp3SetVolume(0x1414);//��������
    wait(1); //��ʱ1ms
    	
    //��vs1003����4���ֽ���Ч���ݣ���������SPI����
   	MP3_XDCS = 0;
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	MP3_XDCS = 1;

}
/**********************************************************/
/*  �������� :  Mp3Reset                                  */
/*  �������� �� vs1003Ӳ����λ                            */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void Mp3Reset(void)
{	
	MP3_XRESET = 0;// ��λvs1003      
	InitSPI();
	wait(100);//��ʱ100ms
	SPIPutChar(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	MP3_XCS = 1;   
	MP3_XDCS = 1;    
	MP3_XRESET =1; 
	wait(100);            //��ʱ100ms
	while (MP3_DREQ == 0);//�ȴ�DREQΪ��
    wait(100);            //��ʱ100ms
    Mp3SoftReset();//vs1003��λ
}
/***********************************************************/
/*  �������� :  VsSineTest                                 */
/*  �������� �� vs1003���Ҳ��ԣ����ú�������whileѭ���У�  */
/*              ����ܳ�������һ��һ�͵�������֤������ͨ�� */                            
/*  ����     :  ��                                         */
/*  ����ֵ   :  ��                                         */
/*---------------------------------------------------------*/
void VsSineTest(void)
{
	MP3_XRESET = 0;  //xReset = 0   ��λvs1003
	wait(100);        //��ʱ100ms        
	SPIPutChar(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	MP3_XCS = 1;
	MP3_XDCS = 1;     
	MP3_XRESET =1; 
	wait(100);	               
	Mp3SetVolume(0x1414);  

 	Mp3WriteRegister(SPI_MODE,0x0c20);//����vs1003�Ĳ���ģʽ
	while (MP3_DREQ == 0);     //�ȴ�DREQΪ��
 	MP3_XDCS = 0;      //ѡ��vs1003�����ݽӿ�
 	
 	//��vs1003�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨vs1003�����������Ҳ���Ƶ��ֵ��������㷽����vs1003��datasheet
   	SPIPutChar(0x53);      
	SPIPutChar(0xef);      
	SPIPutChar(0x6e);      
	SPIPutChar(0x24);      
	SPIPutChar(0x00);      
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	wait(250);
	wait(250);
	MP3_XDCS = 1;//����ִ�е������Ӧ���ܴӶ�������һ����һƵ�ʵ�����
  
        //�˳����Ҳ���
	MP3_XDCS = 0;
	SPIPutChar(0x45);
	SPIPutChar(0x78);
	SPIPutChar(0x69);
	SPIPutChar(0x74);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	wait(250);
	wait(250);

	MP3_XDCS = 1;

    //�ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ
    MP3_XDCS = 0;
	SPIPutChar(0x53);      
	SPIPutChar(0xef);      
	SPIPutChar(0x6e);      
	SPIPutChar(0x44);      
	SPIPutChar(0x00);      
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	wait(250);
	wait(250);

	MP3_XDCS = 1; 

	//�˳����Ҳ���
	MP3_XDCS = 0;
	SPIPutChar(0x45);
	SPIPutChar(0x78);
	SPIPutChar(0x69);
	SPIPutChar(0x74);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	wait(250);
	wait(250);

	MP3_XDCS = 1;
 }

//д���ݣ���������
void VS1003B_WriteDAT(unsigned char dat)
{
	MP3_XDCS = 0;
	SPIPutChar(dat);
	MP3_XDCS = 1;
	MP3_XCS = 1;
}

void VS1003B_Fill2048Zero()
{
	unsigned char i,j;

	for(i=0;i<64;i++)
	{
		if(MP3_DREQ)
		{
			MP3_XDCS = 0;

			for(j=0;j<32;j++)
			{

				VS1003B_WriteDAT(0x00);
			}
			MP3_XDCS = 1;
		}
	}
}

unsigned char temp[32] = {0};
void VS1003_Play() 
{
	unsigned char i = 0;
	while(MP3_DREQ)
	{		
		//P11 = ~ P11;
		if(QueueOut(temp, 32) == 1)
			return;
		for(i = 0; i < 32; i++)
			VS1003B_WriteDAT(temp[i]);
	}
}

xdata unsigned char db[550] = {0};
void VS1003BRecord()
{
	unsigned int wwwww = 0, idx = 0;
	Mp3SetVolume(0x1414); /* Recording monitor volume */
	Mp3WriteRegister(SPI_BASS, 0); /* Bass/treble disabled */
	Mp3WriteRegister(SPI_CLOCKF, 0x4430); /* 2.0x 12.288MHz */
	wait(100);
	Mp3WriteRegister(SPI_AICTRL0, 12); /* Div -> 12=8kHz 8=12kHz 6=16kHz */
	wait(100);
	Mp3WriteRegister(SPI_AICTRL1, 0); /* Auto gain */
	wait(100);
	//if (line_in) {
	//Mp3WriteRegister(SPI_MODE, 0x5c04); /* Normal SW reset + other bits */
	//} else {
	Mp3WriteRegister(SPI_MODE, 0x1c04); /* Normal SW reset + other bits */
	//}
	/* Record loop */
	while (!P37) {
		while(1)
		{
			wwwww = Mp3ReadRegister(SPI_HDAT1);	
			if(wwwww < 256)P10 = 0;
			else if(wwwww >= 896)P11 = 0;
			else break;	
		} /* wait until 512 bytes available */
		P10 = 1;
		P11 = 1;		
		while (idx < 512) {
			wwwww = Mp3ReadRegister(SPI_HDAT0);
			db[idx++] = wwwww>>8;
			db[idx++] = wwwww&0xFF;
		}
		idx = 0;
		UART_SendString(db, 512);
	}
	Mp3Reset();
}
