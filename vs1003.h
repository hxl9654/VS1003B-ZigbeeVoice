/* Include processor definition */
#include "stc15.h"

//vs1003��غ궨�� 
#define VS_WRITE_COMMAND 0x02
#define VS_READ_COMMAND 0x03
#define SCI_MODE        0x0   
#define SCI_STATUS      0x1   
#define SCI_BASS        0x2   
#define SCI_CLOCKF      0x3   
#define SCI_DECODE_TIME 0x4   
#define SCI_AUDATA      0x5   
#define SCI_WRAM        0x6   
#define SCI_WRAMADDR    0x7   
#define SCI_HDAT0       0x8   
#define SCI_HDAT1       0x9   
#define SCI_AIADDR      0xa   
#define SCI_VOL         0xb   
#define SCI_AICTRL0     0xc   
#define SCI_AICTRL1     0xd   
#define SCI_AICTRL2     0xe   
#define SCI_AICTRL3     0xf   
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


#define VS1003_SetVolume(leftchannel_rightchannel){VS1003_WriteRegister(11,(leftchannel_rightchannel));}//�������� 
		
void  VS1003_Record();
void  VS1003_Play() ;
void  VS1003_SoftReset(void);//vs1003��������λ	
void  VS1003_Reset(void);//vs1003��Ӳ����λ
void  VS1003_WriteRegister(unsigned char addressbyte,unsigned int databyte);//дvs1003�Ĵ���
unsigned int VS1003_ReadRegister(unsigned char addressbyte);//��vs1003�Ĵ���
void VS1003_Fill2048Zero();
void VS1003_OutOfWAV();
void VS1003_Init();




