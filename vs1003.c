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

//vs1003接口的初始化
void VS1003_InitPort(void)
{
	VS1003_DREQ = 1;		
	VS1003_XRESET = 1;
	VS1003_XCS = 1;
	VS1003_XDCS = 1;
}
//初始化SPI
void SPI_Init()
{
    SPDAT = 0;                  
    SPSTAT = 0xC0;       
    SPCTL = 0xDE;      
}
//通过SPI发送一个字节的数据
void SPI_SendByte(unsigned char SendData)
{      
	SPDAT = SendData;                
    while (!(SPSTAT & 0x80));  
    SPSTAT = 0xC0;
}
//从SPI接口接收一个字节	
unsigned char SPI_ReadByte(void)
{
	SPDAT = 0xFF;                
    while (!(SPSTAT & 0x80));  
    SPSTAT = 0xC0;
	return SPDAT;
}
//函数功能 ： 写vs1003寄存器
void VS1003_WriteRegister(unsigned char addressbyte, unsigned int databyte)
{
	unsigned char lowbyte, highbyte;
	lowbyte = databyte & 0xff;
	highbyte = databyte >> 8;
	
	VS1003_XCS = 1;
	VS1003_XCS = 0;
	SPI_SendByte(VS_WRITE_COMMAND); //发送写寄存器命令
	SPI_SendByte(addressbyte);      //发送寄存器的地址
	SPI_SendByte(highbyte);         //发送待写数据的高8位
	SPI_SendByte(lowbyte);          //发送待写数据的低8位
	VS1003_XCS = 1;
}

//读vs1003寄存器
unsigned int VS1003_ReadRegister(unsigned char addressbyte)
{
	unsigned int resultvalue = 0;
	unsigned char ucReadValue;

	VS1003_XCS = 1;
	VS1003_XCS = 0;
	SPI_SendByte(VS_READ_COMMAND); //发送读寄存器命令
	SPI_SendByte(addressbyte);	 //发送寄存器的地址

	ucReadValue = SPI_ReadByte();
	resultvalue = ucReadValue<<8;

	ucReadValue = SPI_ReadByte();
	resultvalue |= ucReadValue;

	VS1003_XCS = 1;
	return resultvalue;                 //返回16位寄存器的值
}

//vs1003软件复位
void VS1003_SoftReset(void)
{
	VS1003_WriteRegister (SCI_MODE, 0x0c04); //软件复位

	Delay(1); //延时1ms
	while (VS1003_DREQ == 0); //等待软件复位结束
	VS1003_WriteRegister(SCI_CLOCKF, 0x4430);
	VS1003_SetVolume(0x1414);//设置音量
    Delay(1); //延时1ms
    	
    //向vs1003发送4个字节无效数据，用以启动SPI发送
   	VS1003_XDCS = 0;
	SPI_SendByte(0);
	SPI_SendByte(0);
	SPI_SendByte(0);
	SPI_SendByte(0);
	VS1003_XDCS = 1;

}
//vs1003硬件复位
void VS1003_Reset(void)
{	
	VS1003_XRESET = 0;// 复位vs1003      
	SPI_Init();
	Delay(10);//延时10ms
	SPI_SendByte(0xff);//发送一个字节的无效数据，启动SPI传输
	VS1003_XCS = 1;   
	VS1003_XDCS = 1;    
	VS1003_XRESET =1; 

	while (VS1003_DREQ == 0);//等待DREQ为高
    VS1003_SoftReset();//vs1003软复位
}

//发出正弦音
void VS1003_Beep(unsigned char f)
{
	VS1003_Reset();  
	VS1003_SetVolume(0x2020);
 	VS1003_WriteRegister(SCI_MODE,0x0c24);//进入vs1003的测试模式
	while (VS1003_DREQ == 0);     //等待DREQ为高
 	VS1003_XDCS = 0;      //选择vs1003的数据接口
 	
 	//向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n是vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
   	SPI_SendByte(0x53);      
	SPI_SendByte(0xef);      
	SPI_SendByte(0x6e);      
	SPI_SendByte(f);      
	SPI_SendByte(0x00);      
	SPI_SendByte(0x00);
	SPI_SendByte(0x00);
	SPI_SendByte(0x00);
	Delay(250);
	VS1003_XDCS = 1;//程序执行到这里后应该能从耳机听到一个单一频率的声音
  
    //退出正弦测试
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

//写数据，音乐数据
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
//通过vs1003采集声音
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
