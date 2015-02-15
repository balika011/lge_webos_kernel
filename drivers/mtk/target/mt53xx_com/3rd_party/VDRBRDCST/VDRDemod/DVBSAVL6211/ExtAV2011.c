/*****************************************************************************
* Tuner sample code
*
* History:
* Date Athor Version Reason
* ============ ============= ========= =================
* 1.Apr.29.2010 Version1.0
*****************************************************************************/
#include "ExtAV2011.h"
#include "IBSP.h"
#include "IBase.h" 
#include "II2C.h"
#include "tunerDebug.h"


AVL_DVBSx_ErrorCode AV2011_I2C_write(AVL_uchar reg_start,AVL_uchar* buff,AVL_uchar len,struct AVL_Tuner * pTuner)
{
	AVL_DVBSx_ErrorCode r=0;
	AVL_uint16 uiTimeOut = 0;
	AVL_uchar ucTemp[50];
	int i;
	//AVL_DVBSx_IBSP_Delay(5); //comment by wanghua
	ucTemp[0] = reg_start;
	for(i=1;i<len+1;i++)
	{
		ucTemp[i]=*(buff+i-1);
	}
	
	r = AVL_DVBSx_II2CRepeater_GetOPStatus( pTuner->m_pAVLChip );
	while( r != AVL_DVBSx_EC_OK)
	{
	#ifdef AV2011_ENABLE_TIMEOUT
		if( uiTimeOut++>AV2011_TIMEOUT )
			return(AVL_DVBSx_EC_TimeOut);
	#endif
		AVL_DVBSx_IBSP_Delay(1);
		r = AVL_DVBSx_II2CRepeater_GetOPStatus( pTuner->m_pAVLChip );
	}
		
	r = AVL_DVBSx_II2CRepeater_SendData((AVL_uchar)(pTuner->m_uiSlaveAddress),ucTemp, len+1, pTuner->m_pAVLChip );
	if(r != AVL_DVBSx_EC_OK){
		return(r);
	}

	//AVL_DVBSx_IBSP_Delay(5);
	return(r);
}

AVL_DVBSx_ErrorCode ExtAV2011_RegInit(struct AVL_Tuner * pTuner)
{
	AVL_uchar reg[50];
	AVL_DVBSx_ErrorCode r;

		//Tuner Initail registers R0~R41
		reg[0]=0x38;
		reg[1]=0x00;
		reg[2]=0x00;
		reg[3]=0x50;
		reg[4]=0x1f;
		reg[5]=0xa3;
		reg[6]=0xfd;
		reg[7]=0x58;
		//reg[8]=0x3f;
		
		reg[8] = (0x7<<3) | 0x04 | 0x03;		
		//printf("BB GAIN is %.1fdB     ",(float)(((reg[8]&0x78)>>3)*15)/10);
		//printf("BB driver Current is %.1fmA     REG[8] = 0x%x\n",(float)(((reg[8]&0x03)+1)*5)/10, reg[8]);

		reg[9]=0x82;
		reg[10]=0x88;
		reg[11]=0xb4;
		reg[12]=0xd6; //RFLP=ON at Power on initial
		reg[13]=0x40;
#ifdef AV2011_Tuner
		reg[14]=0x94;
		reg[15]=0x4a;
#else
		reg[14]=0x5b;
		reg[15]=0x6a;
#endif
		reg[16]=0x66;
		reg[17]=0x40;
		reg[18]=0x80;
		reg[19]=0x2b;
		reg[20]=0x6a;
		reg[21]=0x50;
		reg[22]=0x91;
		reg[23]=0x27;
		reg[24]=0x8f;
		reg[25]=0xcc;
		reg[26]=0x21;
		reg[27]=0x10;
		reg[28]=0x80;
		reg[29]=0x02;
		reg[30]=0xf5;
		reg[31]=0x7f;
		reg[32]=0x4a;
		reg[33]=0x9b;
		reg[34]=0xe0;
		reg[35]=0xe0;
		reg[36]=0x36;
		reg[37]=0x00; //Disble FT-function at Power on initial
		reg[38]=0xab;
		reg[39]=0x97;
		reg[40]=0xc5;
		reg[41]=0xa8;
	
		// Sequence 1, Send Reg0 ->Reg11
		r =  AV2011_I2C_write(0,reg,12,pTuner);
		AVL_DVBSx_IBSP_Delay(1);
		// Sequence 2, Send Reg13 ->Reg24
		r = AV2011_I2C_write(13,reg+13,12,pTuner);
		// Send Reg25 ->Reg35
		r = AV2011_I2C_write(25,reg+25,11,pTuner);
		// Send Reg36 ->Reg41
		r = AV2011_I2C_write(36,reg+36,6,pTuner);
		AVL_DVBSx_IBSP_Delay(1);
		// Sequence 3, send reg12
		r = AV2011_I2C_write(12,reg+12,1,pTuner);
		//monsen 20081125, Wait 100 ms
		AVL_DVBSx_IBSP_Delay(10);
		//monsen 20081030, Reinitial again
		{
			// Sequence 1, Send Reg0 ->Reg11
			r = AV2011_I2C_write(0,reg,12,pTuner);
			AVL_DVBSx_IBSP_Delay(1);
			// Sequence 2, Send Reg13 ->Reg24		
			r = AV2011_I2C_write(13,reg+13,12,pTuner);
			// Send Reg25 ->Reg35
			r = AV2011_I2C_write(25,reg+25,11,pTuner);
			// Send Reg36 ->Reg41
			r = AV2011_I2C_write(36,reg+36,6,pTuner);
			AVL_DVBSx_IBSP_Delay(1);
			// Sequence 3, send reg12
			r = AV2011_I2C_write(12,reg+12,1,pTuner);
			AVL_DVBSx_IBSP_Delay(5);
			return(r);
	    }
}

/********************************************************************************
* AVL_DVBSx_ErrorCode Tuner_control(AVL_uint32  channel_freq, AVL_uint32  bb_sym, struct AVL_Tuner * pTuner,struct AV2011_Setting * AV2011_Configure)
*
* Arguments:
* Parameter1: AVL_uint32  channel_freq : Channel freqency (Unit: MHz)
* Parameter2: AVL_uint32  bb_sym : Baseband Symbol Rate (Unit: KHz)
* Paramiter3: AVL_Tuner * pAVLChip : AVL_Tuner structure
* Paramiter4: struct AV2011_Setting * AV2011_Configure : Special configuration for AV2011(IQ mode, Auto_Gain, Scan mode, RF_LP)
* Return Value: INT32 : Result
*****************************************************************************/
AVL_DVBSx_ErrorCode Tuner_control(AVL_uint32  channel_freq, AVL_uint32  bb_sym, struct AVL_Tuner * pTuner,struct AV2011_Setting * AV2011_Configure)
{
	AVL_uchar reg[50];
	AVL_uint32  fracN;
	AVL_uint32  BW;
	AVL_uint32  BF;
	AVL_DVBSx_ErrorCode r;
	//when sym is 0(KHz) or 45000(KHz), means auto-scan channel.
	if (bb_sym == 0 || bb_sym == 45000)
	{
		AV2011_Configure->Auto_Scan = 1;
	}
	//AVL_DVBSx_IBSP_Delay(50);  //comment by wanghua
	fracN = (channel_freq + tuner_crystal/2)/tuner_crystal;
	if(fracN > 0xff)
	fracN = 0xff;
	reg[0]=(fracN & 0xff);
	fracN = (channel_freq<<17)/tuner_crystal;
	fracN = fracN & 0x1ffff;
	reg[1]=(fracN>>9)&0xff;
	reg[2]=(fracN>>1)&0xff;
	// reg[3]_D7 is frac<0>, D6~D0 is 0x50
#ifdef AV2011_Tuner
	//AV2011 IQ Single_end/Differential mode at bit2
	reg[3]=((fracN<<7)&0x80) | 0x50 | ((AV2011_Configure->Single_End)<<2);
#else
    //AV2020 no IQ mode
   	reg[3]=(((fracN<<7)&0x80) | 0x50);
#endif
	// Channel Filter Bandwidth Setting.
	//"sym" unit is Hz;
	if(AV2011_Configure->Auto_Scan)//’ßautoscan requested by BB
	{
		reg[5] = 0xA3; //BW=27MHz
		// Sequence 4, Send Reg0 ->Reg4
  	 	r = AV2011_I2C_write(0,reg,4,pTuner);
		AVL_DVBSx_IBSP_Delay(5);
  	 	// Sequence 5, Send Reg5
  	 	r = AV2011_I2C_write(5, reg+5, 1,pTuner);
		AVL_DVBSx_IBSP_Delay(5);

  	 	// Fine-tune Function Control, Auto-scan mode , FT_block=1, FT_EN=0, FT_hold=1
		reg[37] = 0x05;
		r = AV2011_I2C_write(37, reg+37, 1,pTuner);
		AVL_DVBSx_IBSP_Delay(4);
	}
	else
	{
		// rolloff is 35%
		BW = bb_sym*135/200;
		// monsen 20080726, BB request low IF when sym < 6.5MHz
		// add 6M when Rs<6.5M,
		if(bb_sym<6500)
		{
			BW = BW + 6000;
		}
		// add 2M for LNB frequency shifting
		BW = BW + 2000;
		// add 8% margin since fc is not very accurate
		BW = BW*108/100;
		// Bandwidth can be tuned from 4M to 40M
		if( BW< 4000)
		{
			BW = 4000;
		}
		if( BW> 40000)
		{
			BW = 40000;
		}
		BF = (BW*127 + 21100/2) / (21100); 
		reg[5] = (AVL_uchar)BF;

		// Sequence 4, Send Reg0 ->Reg4
		AVL_DVBSx_IBSP_Delay(5);
		r = AV2011_I2C_write(0,reg,4,pTuner);
		AVL_DVBSx_IBSP_Delay(5);
		r = AV2011_I2C_write(0,reg,4,pTuner);
		AVL_DVBSx_IBSP_Delay(5);
		// Sequence 5, Send Reg5
		r = AV2011_I2C_write(5, reg+5, 1,pTuner);
		AVL_DVBSx_IBSP_Delay(5);
		
		// Fine-tune Function Control, not auto-scan case. enable block function., FT_block=1, FT_EN=1, FT_hold=0
		reg[37] = 0x04 | ((AV2011_Configure->AutoGain)<<1);
		r = AV2011_I2C_write(37, reg+37, 1,pTuner);
		AVL_DVBSx_IBSP_Delay(5);
#ifdef AV2011_Tuner  //wanghua
		//AV2011 IQ Single_end/Differential mode at bit2
		reg[3]=((fracN<<7)&0x80) | 0x50 | ((AV2011_Configure->Single_End)<<2);
#else
    	//AV2020 no IQ mode
   		reg[3]=(((fracN<<7)&0x80) | 0x50);
#endif
/*		
#ifdef AV2011_Tuner
		//Differential mode
		reg[3] = 0x50 | ((AV2011_Configure->Single_End)<<2);
#else
		reg[3] = 0x50;
#endif
*/
		r = AV2011_I2C_write(3, reg+3, 1,pTuner);
		AVL_DVBSx_IBSP_Delay(5);
		//Disable RFLP at Lock Channel sequence after reg[37], RFLP=OFF at Lock Channel sequence, RFLP can be Turned OFF, only at Receving mode.
		reg[12] = 0x96 + ((AV2011_Configure->RF_Loop)<<6);
		r = AV2011_I2C_write(12, reg+12, 1,pTuner);
		AVL_DVBSx_IBSP_Delay(5);
	}
	return r;
}

AVL_DVBSx_ErrorCode AVL_DVBSx_ExtAV2011_Lock(  struct AVL_Tuner * pTuner)
{
	AVL_DVBSx_ErrorCode r;
	struct AV2011_Setting AV2011_Set;
	//pTuner->m_pParameters is used store four special setting for AV2011:IQ mode, auto Gain setting, Work mode, RF loop through
	if(pTuner->m_pParameters == 0)  //if none
	{
		AV2011_Set.Auto_Scan = 0;  //Default Setting: Normal lock mode
		AV2011_Set.AutoGain = 1;   //Defualt Setting: Auto Gain control on
		AV2011_Set.Single_End = 0; //defualt setting: IQ Differential mode
		AV2011_Set.RF_Loop = 1;    //defualt setting: open RF loop through
		//(struct AV2011_Setting)(pTuner->m_pParameters) = &AV2011_Set;  //use default setting if not set it in Struct pTuner->m_pParameters.
		r = Tuner_control((AVL_uint32) ((pTuner->m_uiFrequency_100kHz)/10), (AVL_uint32) ((pTuner->m_uiLPF_100kHz)*100), pTuner, &AV2011_Set);
	}
	else  //set AV2011 tuner spectial setting accordding to pTuner->m_pParameter
	{
		r = Tuner_control((AVL_uint32) ((pTuner->m_uiFrequency_100kHz)/10), (AVL_uint32) ((pTuner->m_uiLPF_100kHz)*100), pTuner, (struct AV2011_Setting *)(pTuner->m_pParameters));
	}
	return(r);
}


AVL_DVBSx_ErrorCode AVL_DVBSx_ExtAV2011_GetLockStatus(struct AVL_Tuner * pTuner)
{
	AVL_DVBSx_ErrorCode r;
	AVL_uchar uilock;
	//Send register address
		r = AVL_DVBSx_II2CRepeater_GetOPStatus(pTuner->m_pAVLChip);
		if( AVL_DVBSx_EC_OK != r ) 
		{
			return(r);
		}
		AVL_DVBSx_IBSP_Delay(1);
		r = AVL_DVBSx_II2CRepeater_ReadData_Multi((AVL_uchar)(pTuner->m_uiSlaveAddress), &uilock, 0x0B,(AVL_uint16)(0x1), pTuner->m_pAVLChip );
		if( AVL_DVBSx_EC_OK == r ) 
		{
			if( 0 == (uilock & AV2011_R0_PLL_LOCK) ) 
			{
				r = AVL_DVBSx_EC_Running;
			}
		}
	return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_ExtAV2011_Initialize(struct AVL_Tuner * pTuner) {
	AVL_DVBSx_ErrorCode r;

	r = AVL_DVBSx_II2C_Write16(pTuner->m_pAVLChip, rc_tuner_slave_addr_addr, pTuner->m_uiSlaveAddress);
	r |= AVL_DVBSx_II2C_Write16(pTuner->m_pAVLChip, rc_tuner_use_internal_control_addr, 0);
	r |= AVL_DVBSx_II2C_Write16(pTuner->m_pAVLChip, rc_tuner_LPF_margin_100kHz_addr, 0);	//clean up the LPF margin for blind scan. for external driver, this must be zero.
	r |= AVL_DVBSx_II2C_Write16(pTuner->m_pAVLChip, rc_tuner_max_LPF_100kHz_addr, 360);	//set up the right LPF for blind scan to regulate the freq_step. This field should corresponding the flat response part of the LPF.
	r |= AVL_DVBSx_II2CRepeater_Initialize(pTuner->m_uiI2CBusClock_kHz, pTuner->m_pAVLChip);
	r |= ExtAV2011_RegInit(pTuner); //init all tuner register
	return(r);	
}
