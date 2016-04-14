#include "vs1003.h"
#include "MusicDataMP3.c"
#include "UART.h"

#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define bool bit
#define true 1
#define flase 0

#define uint8 unsigned char

sbit  MP3_XRESET  = P3^2;
sbit MP3_XCS = P3^3;
sbit MP3_XDCS  = P3^4;
sbit MP3_DREQ = P3^5;
sbit c_SPI_SI = P1^5;
sbit c_SPI_SO = P1^6;
sbit c_SPI_CLK = P1^7;

void wait(uchar ucDelayCount)
{
	uchar ucTempCount;
	uchar uci;

	for(ucTempCount=0; ucTempCount<ucDelayCount; ucTempCount++)
	{
		uci = 230;
		while(uci--)
		{
			_nop_();
	   	}
	}
}

/**********************************************************/
/*  �������� :  InitPortVS1003                            */
/*  �������� �� MCU��vs1003�ӿڵĳ�ʼ��                   */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void  InitPortVS1003(void)
{
	c_SPI_SO = 1;
	MP3_DREQ = 1;		
	MP3_XRESET = 1;
	MP3_XCS = 1;
	MP3_XDCS = 1;
}
/**********************************************************/
/*  �������� :  SPIPutChar                                */
/*  �������� �� ͨ��SPI����һ���ֽڵ�����                 */
/*  ����     :  �����͵��ֽ�����                          */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void  SPIPutChar(unsigned char ucSendData)
{      
	uchar ucCount;
	uchar ucMaskCode;

	ucMaskCode = 0x80;
	for(ucCount=0; ucCount<8; ucCount++)
	{
		c_SPI_CLK = 0;

		if(ucMaskCode & ucSendData)
		{
			c_SPI_SI = 1;
		}
		else
		{
			c_SPI_SI = 0;
		}

		c_SPI_CLK = 1;
		ucMaskCode >>= 1;

	}
}

/*******************************************************************************************************************
** ��������: INT8U SPI_RecByte()				Name:	  INT8U SPI_RecByte()
** ��������: ��SPI�ӿڽ���һ���ֽ�				Function: receive a byte from SPI interface
** �䡡  ��: ��									Input:	  NULL
** �� �� ��: �յ����ֽ�							Output:	  the byte that be received
********************************************************************************************************************/
static uchar SPI_RecByte(void)
{
	uchar ucReadData = 0;
	uchar ucCount;

	ucReadData = 0;
	c_SPI_SI = 1;

	for(ucCount=0; ucCount<8; ucCount++)
	{
		ucReadData <<= 1;
		c_SPI_CLK = 0;	
		if(c_SPI_SO)
		{
			ucReadData |= 0x01;
		}
		c_SPI_CLK = 1;

	}

	return(ucReadData);
}

/*************************************************************/
/*  �������� :  Mp3WriteRegister                             */
/*  �������� �� дvs1003�Ĵ���                               */
/*  ����     :  �Ĵ�����ַ����д����                         */
/*  ����ֵ   :  ��                                           */
/*-----------------------------------------------------------*/
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
	uchar ucReadValue;

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
	Mp3WriteRegister (SPI_AUDATA, 0xBB81); //������48k��������
	Mp3WriteRegister(SPI_BASS, 0x0055);//��������
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
	wait(200);//��ʱ100ms
	SPIPutChar(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	MP3_XCS = 1;   
	MP3_XDCS = 1;    
	MP3_XRESET =1; 
	wait(200);            //��ʱ100ms
	while (MP3_DREQ == 0);//�ȴ�DREQΪ��

    wait(200);            //��ʱ100ms
 	Mp3SetVolume(0x1414);  
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
	wait(200);        //��ʱ100ms        
	SPIPutChar(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	MP3_XCS = 1;
	MP3_XDCS = 1;     
	MP3_XRESET =1; 
	wait(200);	               
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

void test_1003_PlayMP3File();

void TestVS1003B(void)
{
	Mp3Reset();
	VsSineTest();
	Mp3SoftReset();
	test_1003_PlayMP3File();
}
//д���ݣ���������
void VS1003B_WriteDAT(unsigned char dat)
{
	MP3_XDCS = 0;
	SPIPutChar(dat);
	MP3_XDCS = 1;
	MP3_XCS = 1;
}

//����������
void VS1003B_SetVirtualSurroundOn(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//дʱ�ӼĴ���
	{
		uiModeValue = Mp3ReadRegister(0x00);
		if(uiModeValue & 0x0001)
		{
			break;
		}
		else
		{
			uiModeValue |= 0x0001;
			Mp3WriteRegister(0,uiModeValue);
		}
		ucRepeatCount++;
		if(ucRepeatCount++ >10 )break;
	}

}

//�رջ�����
void VS1003B_SetVirtualSurroundOff(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//дʱ�ӼĴ���
	{
		uiModeValue = Mp3ReadRegister(0x00);
		if(uiModeValue & 0x0001)
		{
			break;
		}
		else
		{
			uiModeValue |= 0x0001;
			Mp3WriteRegister(0,uiModeValue);
		}
		ucRepeatCount++;
		if(ucRepeatCount++ >10 )break;
	}

}

//��ǿ����
//��ڲ���	1.ǿ��0-15
//			2.Ƶ��0-15 (X10Hz)
void VS1003B_SetBassEnhance(uchar ucValue, ucFrequencyID)
{
	uchar ucRepeatCount;
	uint uiWriteValue;
	uint uiReadValue;	

	ucRepeatCount =0;

	uiWriteValue = Mp3ReadRegister(0x02);

	uiWriteValue &= 0xFF00;
	uiWriteValue |= ucValue<<4;
	uiWriteValue &= (ucFrequencyID & 0x0F);

	while(1)//дʱ�ӼĴ���
	{

		Mp3WriteRegister(2,uiWriteValue);
		uiReadValue = Mp3ReadRegister(0x02);
		
		if(uiReadValue == uiWriteValue)
		{
			break;
		}
		ucRepeatCount++;
		if(ucRepeatCount++ >10 )break;
	}

}

//VS1003��ʼ����0�ɹ� 1ʧ��
unsigned char VS1003B_Init()
{
	unsigned char retry;
	Mp3Reset();
	retry=0;
	while(Mp3ReadRegister(0x00) != SM_SDINEW | SM_SDISHARE)//дmode�Ĵ���
	{
		Mp3WriteRegister(0x00,SM_SDINEW | SM_SDISHARE);
		if(retry++ >10 )break;//{PORTB|=_BV(PB1);break;}
	}
	retry=0;
	while(Mp3ReadRegister(0x03) != 0x4430)//дʱ�ӼĴ���
	{
		Mp3WriteRegister(0x03,0x4430);
		if(retry++ >10 )break;
	}
	retry=0;
	while(Mp3ReadRegister(0x0b) != 0x1414)//������
	{
		Mp3WriteRegister(0x0b,0x1414);
		if(retry++ >10 )break;
	}
	if(retry > 10)return 1;
	return 0;
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


void test_1003_PlayMP3File() 
{
    unsigned int data_pointer;
	unsigned char i;
	unsigned int uiCount;
	uiCount = sizeof(MusicData);
	data_pointer=0; 
	Mp3SoftReset();
    while(uiCount>0)
  	{ 
	   if(MP3_DREQ)
      	{
    		for(i=0;i<32;i++)
           	{
     			VS1003B_WriteDAT(MusicData[data_pointer]);
     			data_pointer++;
            }
			uiCount -= 32;
         }
    }
	VS1003B_Fill2048Zero();
}
sbit P10 = P1 ^ 0;
sbit P11 = P1 ^ 1;
xdata unsigned char db[1000] = {0};
sbit P37 = P3 ^ 7;
void VS1003BRecord()
{
	unsigned int wwwww = 0, idx = 0;
	unsigned char aaaaa = 0;
	VS1003B_Init();
	Mp3SetVolume(1414); /* Recording monitor volume */
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
			//aaaaa = (unsigned char)(wwwww / 256);			
			if(wwwww < 256)P10 = 0;
			else if(wwwww > 896)P11 = 0;
			else break;
			//SBUF = aaaaa;			
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
