/* Include processor definition */
#include "stc15.h"

//vs1003��غ궨�� 
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


#define Mp3SetVolume(leftchannel_rightchannel){Mp3WriteRegister(11,(leftchannel_rightchannel));}//�������� 
		
		 
void  MSPI_Init(void);  //SPI��ʼ��
void  InitPortVS1003(void);//LPC213x��vs1003�Ľӿڵĳ�ʼ��
void  SPIPutChar(unsigned char c);//ͨ��SPI����һ���ֽڵ�����
void  Mp3SoftReset(void);//vs1003�������λ	
void  Mp3Reset(void);//vs1003��Ӳ����λ
void  VsSineTest(void);//vs1003�����Ҳ���
void  Mp3WriteRegister(unsigned char addressbyte,unsigned int databyte);//дvs1003�Ĵ���
unsigned int Mp3ReadRegister(unsigned char addressbyte);//��vs1003�Ĵ���





