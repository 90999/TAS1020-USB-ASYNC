//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
/*================================================== 
Softpll.c: routines to handle ACG soft PLL
//================================================*/
#include <Reg52.h>
#include "..\rom\types.h"
#include "..\rom\Reg_stc1.h"
#include "..\rom\usbdfu.h"
#include "devDesc.h"
#include "device.h"
#include "Softpll.h"

word EngAcgCap1, EngAcgCap2;
unsigned char fbCount;

/*====================================================================
softPllInit(): USB ACG initialization
=====================================================================*/
void softPllInit()   
{
	fbCount=1;
	EngAcgCap1 = 0;
	EngAcgCap2 = 0;


	//ʹ���ⲿ11.2896MHZ����
	ACGDCTL   = 0x00;//����Ƶ
	ACGCTL  = 0x4d;//OFF��Ƶ��,MCLK 1 2ѡ���ⲿMCLKIN����


}

/*====================================================================
softPll(): ISR for Start of Frame which is used for 
the synchronous mode
=====================================================================*/


#include "..\ROM\Mmap.h"
#include "Devmap.h"
#include "..\ROM\hwMacro.h"


#define	Ch0WrPtrL  stc_sfr(0xFFBC)
#define	Ch0WrPtrH  stc_sfr(0xFFBB)
#define	Ch0RdPtrL  stc_sfr(0xFFBA)
#define	Ch0RdPtrH  stc_sfr(0xFFB9)


unsigned long MlckSum;

void softPll()
{
	unsigned char nInt;
	unsigned long nFrac;
	unsigned long fbvalue;   
    unsigned int MclkPerMs;  //��1msʱ��usb SOF����DA��ʱ��
	
	EngAcgCap2=EngAcgCap1;	//�ϴ�ʱ�Ӳ���ֵ
	LOW(EngAcgCap1)  = ACGCAPL;	  //����µ���ֵ
	HIGH(EngAcgCap1) = ACGCAPH;
	
    MclkPerMs = EngAcgCap1 - EngAcgCap2; //����ǲ�������MCLK(1ms��λ)���磺11289

//usb���߾����ᶪ֡����ʧSOF
//>22000: ���������������� >11000: ����
//���������������㣬��߾���
if (MclkPerMs < 22000 ) MclkPerMs=MclkPerMs*2;
if (fbCount==0) { //2�β���ȡƽ��ֵ

MlckSum=MlckSum+MclkPerMs;
MclkPerMs=MlckSum /4 ;//��ԭ����������

if(MclkPerMs >11293) MclkPerMs=11293;
else if (MclkPerMs < 11289)MclkPerMs=11289; 
			
	// 11289*1000/256 ~= 44100  (44.100Khz)  �ֳ���������44 С������100
	// ������10.10��ʽ=  44 << 14 + 100 << 4
	
	nInt=MclkPerMs/256;//��������	
	nFrac=(MclkPerMs - nInt*256)*1000/256;//С������	
	//fbvalue=(nInt <<14) + (nFrac<<4);	
	fbvalue= nInt;
	fbvalue= fbvalue << 14;

	nFrac=nFrac << 4;
	fbvalue= nFrac | fbvalue;
	
	INEP2_X[0]=fbvalue & 0xff;
	INEP2_X[1]=(fbvalue>>8) & 0xff;
	INEP2_X[2]=(fbvalue>>16) & 0xff;
}

else if (fbCount==1) MlckSum=MclkPerMs;//2 �β���ȡƽ��ֵ

	if (fbCount==0)
	{
		fbCount=3;  
		IEPDCNTX2 = 3;IEPDCNTY2 = 3; 	// X data count =3	
	}
	else
	{
		fbCount--;
	} 


	// Check if host drops part of a sample
    if (AppDevice.spkIf.curSetting != DEV_SPK_SETTING_0)
    {
        EngAcgCap2 = DMABCNT0L | (DMABCNT0H << 8);
        // turn on amp if there is ISO data stream out        
        if (EngAcgCap2 % DEV_NUM_BYTE_PER_SAMPLE )
    	{
    		// Alligning the UBM and DMA pointers
    		DMACTL0 = 0; // non-integral, so reset pointers
    		DMACTL0 = 0x81;
    	}

    }

}
