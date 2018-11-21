//TAS1020A USB����
//By Rush,����֮��
//================================================== 
// Texas Instruments Strictly Private 
// Copyright 1999, Texas Instruments Inc. 
//================================================== 
/*================================================== 
Softpll.c: ʱ�����첽����
//================================================*/
#include <Reg52.h>
#include "..\rom\types.h"
#include "..\rom\Reg_stc1.h"
#include "..\rom\usbdfu.h"
#include "devDesc.h"
#include "device.h"
#include "Softpll.h"

word EngAcgCap1, EngAcgCap2;

/*====================================================================
softPllInit(): DACʱ�ӳ�ʼ��

=====================================================================*/
void softPllInit()   
{
	EngAcgCap1 = 0;
	EngAcgCap2 = 0;


	//ʹ���ⲿ11.2896MHZ����
	ACGDCTL   = 0x00;//����Ƶ
	ACGCTL  = 0x4d;//ʹ�ܷ�Ƶ��,MCLK 1 2ѡ���ⲿMCLKIN����


}

/*====================================================================
softPll(): SOF�жϳ��򣬼����첽������

=====================================================================*/
#include "..\ROM\Mmap.h"
#include "Devmap.h"
#include "..\ROM\hwMacro.h"


void softPll()
{ 
    unsigned int BufSize; 
		
	BufSize = DMABCNT0L | (DMABCNT0H << 8);//FIFO�������������
	
	if (BufSize > (OUTEP1_RSIZE /4 ) ) 
	{
			INEP2_X[0]=0x33;	INEP2_X[1]=0x03;	INEP2_X[2]=0x0b;//�Ա��˳ŵ�
	}
	else if (BufSize > (OUTEP1_RSIZE /8 ) ) 
	{
			INEP2_X[0]=0x66;	INEP2_X[1]=0x06;	INEP2_X[2]=0x0b;//44.100	
	}
	else
	{
			INEP2_X[0]=0x99;	INEP2_X[1]=0x09;	INEP2_X[2]=0x0b;//ι���
	}
//�޸��Ϸ������ݿ��Ը�������Ҫ������PC�ṩ�����������仺��

	IEPDCNTX2 = 3;IEPDCNTY2 = 3; 	// let's send it to PC.	


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

/*
���������£����³��򼴿��ȶ����У�

	if (BufSize > (OUTEP1_RSIZE /4 ) ) 
	{
			INEP2_X[0]=0x33;	INEP2_X[1]=0x03;	INEP2_X[2]=0x0b;//�Ա��˳ŵ�
	}
	else
	{
			INEP2_X[0]=0x66;	INEP2_X[1]=0x06;	INEP2_X[2]=0x0b;//44.100	
	}

Rushע�͡�

*/


