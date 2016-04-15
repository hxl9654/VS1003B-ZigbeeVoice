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
/*  函数名称 :  InitPortVS1003                            */
/*  函数功能 ： MCU与vs1003接口的初始化                   */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void InitPortVS1003(void)
{
	c_SPI_SO = 1;
	MP3_DREQ = 1;		
	MP3_XRESET = 1;
	MP3_XCS = 1;
	MP3_XDCS = 1;
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
	unsigned char ucReadValue;

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
	InitSPI();
	wait(100);//延时100ms
	SPIPutChar(0xff);//发送一个字节的无效数据，启动SPI传输
	MP3_XCS = 1;   
	MP3_XDCS = 1;    
	MP3_XRESET =1; 
	wait(100);            //延时100ms
	while (MP3_DREQ == 0);//等待DREQ为高
    wait(100);            //延时100ms
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
	wait(100);        //延时100ms        
	SPIPutChar(0xff);//发送一个字节的无效数据，启动SPI传输
	MP3_XCS = 1;
	MP3_XDCS = 1;     
	MP3_XRESET =1; 
	wait(100);	               
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

//写数据，音乐数据
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
