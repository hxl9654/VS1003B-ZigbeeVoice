/* Include processor definition */
#include "stc15.h"

//vs1003相关宏定义 
#define VS_WRITE_COMMAND 0x02
#define VS_READ_COMMAND 0x03
#define SPI_MODE        0x0   
#define SPI_STATUS      0x1   
#define SPI_BASS        0x2   
#define SPI_CLOCKF      0x3   
#define SPI_DECODE_TIME 0x4   
#define SPI_AUDATA      0x5   
#define SPI_WRAM        0x6   
#define SPI_WRAMADDR    0x7   
#define SPI_HDAT0       0x8   
#define SPI_HDAT1       0x9   
#define SPI_AIADDR      0xa   
#define SPI_VOL         0xb   
#define SPI_AICTRL0     0xc   
#define SPI_AICTRL1     0xd   
#define SPI_AICTRL2     0xe   
#define SPI_AICTRL3     0xf   
#define SM_DIFF         0x01   
#define SM_JUMP         0x02   
#define SM_RESET        0x04   
#define SM_OUTOFWAV     0x08   
#define SM_PDOWN        0x10   
#define SM_TESTS        0x20   
#define SM_STREAM       0x40   
#define SM_PLUSV        0x80   
#define SM_DACT         0x100   
#define SM_SDIORD       0x200   
#define SM_SDISHARE     0x400   
#define SM_SDINEW       0x800   
#define SM_ADPCM        0x1000   
#define SM_ADPCM_HP     0x2000 


#define VS1003_SetVolume(leftchannel_rightchannel){VS1003_WriteRegister(11,(leftchannel_rightchannel));}//音量设置 
		
void  VS1003_Record();
void  VS1003_Play() ;
void  VS1003_InitPort(void);//LPC213x与vs1003的接口的初始化
void  VS1003_SoftReset(void);//vs1003的软件复位	
void  VS1003_Reset(void);//vs1003的硬件复位
void  VS1003_Beep(unsigned char f);//vs1003的正弦测试
void  VS1003_WriteRegister(unsigned char addressbyte,unsigned int databyte);//写vs1003寄存器
unsigned int VS1003_ReadRegister(unsigned char addressbyte);//读vs1003寄存器





