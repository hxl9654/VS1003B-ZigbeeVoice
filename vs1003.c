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
/*  函数名称 :  InitPortVS1003                            */
/*  函数功能 ： MCU与vs1003接口的初始化                   */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
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
/*  函数名称 :  SPIPutChar                                */
/*  函数功能 ： 通过SPI发送一个字节的数据                 */
/*  参数     :  待发送的字节数据                          */
/*  返回值   :  无                                        */
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
** 函数名称: INT8U SPI_RecByte()				Name:	  INT8U SPI_RecByte()
** 功能描述: 从SPI接口接收一个字节				Function: receive a byte from SPI interface
** 输　  入: 无									Input:	  NULL
** 输 　 出: 收到的字节							Output:	  the byte that be received
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
/*  函数名称 :  Mp3WriteRegister                             */
/*  函数功能 ： 写vs1003寄存器                               */
/*  参数     :  寄存器地址，待写数据                         */
/*  返回值   :  无                                           */
/*-----------------------------------------------------------*/
void Mp3WriteRegister(unsigned char addressbyte, unsigned int databyte)
{
	unsigned char lowbyte, highbyte;
	lowbyte = databyte & 0xff;
	highbyte = databyte >> 8;
	
	MP3_XCS = 1;
	MP3_XCS = 0;
	SPIPutChar(VS_WRITE_COMMAND); //发送写寄存器命令
	SPIPutChar(addressbyte);      //发送寄存器的地址
	SPIPutChar(highbyte);         //发送待写数据的高8位
	SPIPutChar(lowbyte);          //发送待写数据的低8位
	MP3_XCS = 1;
}
/*************************************************************/
/*  函数名称 :  Mp3ReadRegister                              */
/*  函数功能 ： 读vs1003寄存器                               */
/*  参数     :  寄存器地址				     */
/*  返回值   :  vs1003的16位寄存器的值                       */
/*-----------------------------------------------------------*/
unsigned int Mp3ReadRegister(unsigned char addressbyte)
{
	unsigned int resultvalue = 0;
	uchar ucReadValue;

	MP3_XCS = 1;
	MP3_XCS = 0;
	SPIPutChar(VS_READ_COMMAND); //发送读寄存器命令
	SPIPutChar(addressbyte);	 //发送寄存器的地址

	ucReadValue = SPI_RecByte();
	resultvalue = ucReadValue<<8;

	ucReadValue = SPI_RecByte();
	resultvalue |= ucReadValue;
	//SBUF = resultvalue >> 8 ;
	MP3_XCS = 1;
	//resultvalue = 0x3456;
	return resultvalue;                 //返回16位寄存器的值
}
/**********************************************************/
/*  函数名称 :  Mp3SoftReset                              */
/*  函数功能 ： vs1003软件复位                            */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void Mp3SoftReset(void)
{
	Mp3WriteRegister (SPI_MODE, 0x0c04); //软件复位

	wait(1); //延时1ms
	while (MP3_DREQ == 0); //等待软件复位结束
	Mp3WriteRegister(SPI_CLOCKF, 0x4430);
	Mp3WriteRegister (SPI_AUDATA, 0xBB81); //采样率48k，立体声
	Mp3WriteRegister(SPI_BASS, 0x0055);//设置重音
	Mp3SetVolume(0x1414);//设置音量
    wait(1); //延时1ms
    	
    //向vs1003发送4个字节无效数据，用以启动SPI发送
   	MP3_XDCS = 0;
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	MP3_XDCS = 1;

}
/**********************************************************/
/*  函数名称 :  Mp3Reset                                  */
/*  函数功能 ： vs1003硬件复位                            */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void Mp3Reset(void)
{	
	MP3_XRESET = 0;// 复位vs1003      
	wait(200);//延时100ms
	SPIPutChar(0xff);//发送一个字节的无效数据，启动SPI传输
	MP3_XCS = 1;   
	MP3_XDCS = 1;    
	MP3_XRESET =1; 
	wait(200);            //延时100ms
	while (MP3_DREQ == 0);//等待DREQ为高

    wait(200);            //延时100ms
 	Mp3SetVolume(0x1414);  
    Mp3SoftReset();//vs1003软复位
}
/***********************************************************/
/*  函数名称 :  VsSineTest                                 */
/*  函数功能 ： vs1003正弦测试，将该函数放在while循环中，  */
/*              如果能持续听到一高一低的声音，证明测试通过 */                            
/*  参数     :  无                                         */
/*  返回值   :  无                                         */
/*---------------------------------------------------------*/
void VsSineTest(void)
{
	MP3_XRESET = 0;  //xReset = 0   复位vs1003
	wait(200);        //延时100ms        
	SPIPutChar(0xff);//发送一个字节的无效数据，启动SPI传输
	MP3_XCS = 1;
	MP3_XDCS = 1;     
	MP3_XRESET =1; 
	wait(200);	               
	Mp3SetVolume(0x1414);  

 	Mp3WriteRegister(SPI_MODE,0x0c20);//进入vs1003的测试模式
	while (MP3_DREQ == 0);     //等待DREQ为高
 	MP3_XDCS = 0;      //选择vs1003的数据接口
 	
 	//向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n = 0x24, 设定vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
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
	MP3_XDCS = 1;//程序执行到这里后应该能从耳机听到一个单一频率的声音
  
        //退出正弦测试
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

    //再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值
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

	//退出正弦测试
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
//写数据，音乐数据
void VS1003B_WriteDAT(unsigned char dat)
{
	MP3_XDCS = 0;
	SPIPutChar(dat);
	MP3_XDCS = 1;
	MP3_XCS = 1;
}

//开启环绕声
void VS1003B_SetVirtualSurroundOn(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//写时钟寄存器
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

//关闭环绕声
void VS1003B_SetVirtualSurroundOff(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//写时钟寄存器
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

//增强重音
//入口参数	1.强度0-15
//			2.频率0-15 (X10Hz)
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

	while(1)//写时钟寄存器
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

//VS1003初始化，0成功 1失败
unsigned char VS1003B_Init()
{
	unsigned char retry;
	Mp3Reset();
	retry=0;
	while(Mp3ReadRegister(0x00) != SM_SDINEW | SM_SDISHARE)//写mode寄存器
	{
		Mp3WriteRegister(0x00,SM_SDINEW | SM_SDISHARE);
		if(retry++ >10 )break;//{PORTB|=_BV(PB1);break;}
	}
	retry=0;
	while(Mp3ReadRegister(0x03) != 0x4430)//写时钟寄存器
	{
		Mp3WriteRegister(0x03,0x4430);
		if(retry++ >10 )break;
	}
	retry=0;
	while(Mp3ReadRegister(0x0b) != 0x1414)//设音量
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
