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
/*  函数名称 :  InitPortVS1003                            */
/*  函数功能 ： MCU与vs1003接口的初始化                   */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void VS1003_InitPort(void)
{
	VS1003_DREQ = 1;		
	VS1003_XRESET = 1;
	VS1003_XCS = 1;
	VS1003_XDCS = 1;
}
//初始化SPI
void InitSPI()
{
    SPDAT = 0;                  
    SPSTAT = 0xC0;       
    SPCTL = 0xDD;      
}
//通过SPI发送一个字节的数据
void SPIPutChar(unsigned char SendData)
{      
	SPDAT = SendData;                
    while (!(SPSTAT & 0x80));  
    SPSTAT = 0xC0;
}
//从SPI接口接收一个字节	
unsigned char SPI_RecByte(void)
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
	SPIPutChar(VS_WRITE_COMMAND); //发送写寄存器命令
	SPIPutChar(addressbyte);      //发送寄存器的地址
	SPIPutChar(highbyte);         //发送待写数据的高8位
	SPIPutChar(lowbyte);          //发送待写数据的低8位
	VS1003_XCS = 1;
}

//读vs1003寄存器
unsigned int VS1003_ReadRegister(unsigned char addressbyte)
{
	unsigned int resultvalue = 0;
	unsigned char ucReadValue;

	VS1003_XCS = 1;
	VS1003_XCS = 0;
	SPIPutChar(VS_READ_COMMAND); //发送读寄存器命令
	SPIPutChar(addressbyte);	 //发送寄存器的地址

	ucReadValue = SPI_RecByte();
	resultvalue = ucReadValue<<8;

	ucReadValue = SPI_RecByte();
	resultvalue |= ucReadValue;

	VS1003_XCS = 1;
	return resultvalue;                 //返回16位寄存器的值
}

//vs1003软件复位
void VS1003_SoftReset(void)
{
	VS1003_WriteRegister (SPI_MODE, 0x0c04); //软件复位

	Delay(1); //延时1ms
	while (VS1003_DREQ == 0); //等待软件复位结束
	VS1003_WriteRegister(SPI_CLOCKF, 0x4430);
	VS1003_SetVolume(0x1414);//设置音量
    Delay(1); //延时1ms
    	
    //向vs1003发送4个字节无效数据，用以启动SPI发送
   	VS1003_XDCS = 0;
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	VS1003_XDCS = 1;

}
//vs1003硬件复位
void VS1003_Reset(void)
{	
	VS1003_XRESET = 0;// 复位vs1003      
	InitSPI();
	Delay(100);//延时100ms
	SPIPutChar(0xff);//发送一个字节的无效数据，启动SPI传输
	VS1003_XCS = 1;   
	VS1003_XDCS = 1;    
	VS1003_XRESET =1; 
	Delay(100);            //延时100ms
	while (VS1003_DREQ == 0);//等待DREQ为高
    VS1003_SoftReset();//vs1003软复位
}

//发出正弦音
void VS1003_Beep(unsigned char f)
{
	VS1003_Reset();  
 	VS1003_WriteRegister(SPI_MODE,0x0c24);//进入vs1003的测试模式
	while (VS1003_DREQ == 0);     //等待DREQ为高
 	VS1003_XDCS = 0;      //选择vs1003的数据接口
 	
 	//向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n是vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
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
	VS1003_XDCS = 1;//程序执行到这里后应该能从耳机听到一个单一频率的声音
  
    //退出正弦测试
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

//写数据，音乐数据
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
//通过vs1003采集声音
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
