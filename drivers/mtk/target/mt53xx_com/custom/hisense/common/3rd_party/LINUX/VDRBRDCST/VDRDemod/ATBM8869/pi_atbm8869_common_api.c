/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pd_DTMB_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file pi_atbm_common_api.c
 *  DTMB pi layer
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------


#include "pi_atbm8869_common_def.h"

#include "fe_tuner_common_if.h"
#include "tuner_if.h" //DRVAPI_TUNER_OK
#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "x_gpio.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "TDIS_header.h"
#include "drvcust_if.h" //Get Tuner/Demod I2C bus id
#include "x_stl_lib.h"//for StrToInt function
#include "sif_if.h"
#include "pi_atbm8869_dvbc_api.h"
#include "pi_demod_atbm8869_dvbc.h"

#define I2C_TIMING 135
//static DEMOD_I2C_DATA_T InitTbl[]={NULL};

static UINT8 ATBM_ChipID;
//static pDVBC_Demod_Ctx psDemodCtx;


//chipID==0x30
static DEMOD_I2C_DATA_T TSOutFMTTbl[]=
{
 {0x0301,0x01},//2 bits bit 0, 0:rising edge 1:fallling edge;bit1,0:Enable TS output 1: Disable TS output
 {0x0306,0x01},//Clock mode 1bit 0:gated 1:continuous 
 {0x0311,0x00},
 {0x0305,0x01},//1:serial TS mode, 0: parallel TS mode
 {0x031f,0x00},//1bit 1: TS output from TSDATA[7], bit 0: TS output from TSDATA[0]
};

//chipID==0x40
static DEMOD_I2C_DATA_T TSOutFMTTbl2[]=
{
 {0x0301,0x01},//2 bits bit 0, 0:rising edge 1:fallling edge;bit1,0:Enable TS output 1: Disable TS output
 {0x0306,0x01},//Clock mode 1bit 0:gated 1:continuous 
 {0x0311,0x00},
 {0x0312,0x01},//886x special
 {0x0b24,0x01},
 {0x0305,0x01},//1:serial TS mode, 0: parallel TS mode
 {0x031f,0x00},//1bit 1: TS output from TSDATA[7], bit 0: TS output from TSDATA[0]
};
static DEMOD_I2C_DATA_T ui8AATBM886xCommonReg[]=
{
 {0x0245, 0x33},
 {0x024a, 0x96},
 {0x02c6, 0x00},
 {0x02c7, 0x01},
};

BOOL ATBMWrite(UINT8 u1I2cAddr,UINT16 u2RegAddr, UINT8 data)
{
    UINT8 u1RegAddr[2];
	UINT32 u4DemodBusId=SIF_BUS_TUNER_I2C;//default
	u1RegAddr[0]=(UINT8)((u2RegAddr>>8) & 0xFF);//function address
	u1RegAddr[1]=(UINT8)(u2RegAddr & 0xFF);//offset address
	
	#if 0
	//Write Address
	if(ICtrlBus_I2cTunerWrite(I2C_TIMING,u1I2cAddr,u1RegAddr,2))
	{
	 mcDBG_MSG_INFO(("Fail to write 0x%02X:0x%02X-0x%02X\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1]));
	 return FALSE;
	}
	//Write data
	if(ICtrlBus_I2cTunerWrite(I2C_TIMING,u1I2cAddr,&data,1))
	{
	 mcDBG_MSG_INFO(("Fail to write 0x%02X:0x%02X-0x%02X-0x%02X\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1],data));
	 return FALSE;
	}
	#else
	 
	  
    DRVCUST_OptQuery(eDemodI2cBusID,&u4DemodBusId);
	
    if (SIF_X_Write_SpecialDemod((UINT8)(u4DemodBusId), I2C_TIMING, u1I2cAddr, 2, (UINT32)u2RegAddr, &data, 1) == 0)
	{
	  mcDBG_MSG_INFO(("Fail to write 0x%02X:0x%02X-0x%02X-0x%02X\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1],data));
	 return FALSE;
	}
	#endif
	mcDBG_MSG_L4((" Write 0x%02X:0x%02X-0x%02X-0x%02X\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1],data));
	return TRUE;	
}

BOOL ATBMRead(UINT8 u1I2cAddr,UINT16 u2RegAddr, UINT8 * data)
{
    UINT8 u1RegAddr[2];
	UINT32 u4DemodBusId=SIF_BUS_TUNER_I2C;
	
	u1RegAddr[0]=(UINT8)((u2RegAddr>>8) & 0xFF);//function address
	u1RegAddr[1]=(UINT8)(u2RegAddr & 0xFF);//offset address
	
	#if 0
	//Write Address
	if(ICtrlBus_I2cTunerWrite(I2C_TIMING,u1I2cAddr,u1RegAddr,2))
	{
	 mcDBG_MSG_INFO(("Fail to write 0x%02X:0x%02X-0x%02X\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1]));
	 return FALSE;
	}
	//Read  data
	if(ICtrlBus_I2cTunerRead(I2C_TIMING,u1I2cAddr,data,1))
	{
	 mcDBG_MSG_INFO(("Fail to read 0x%02X:0x%02X-0x%02X\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1]));
	 return FALSE;
	}
	#else
	
	  DRVCUST_OptQuery(eDemodI2cBusID,&u4DemodBusId);
	
    if (SIF_X_Read((UINT8)(u4DemodBusId), I2C_TIMING, u1I2cAddr, 2, (UINT32)u2RegAddr,data, 1) == 0)
	{
	  mcDBG_MSG_INFO(("Fail to read 0x%02X:0x%02X-0x%02X\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1]));
	 return FALSE;
	}
	#endif
	mcDBG_MSG_L4((" Read 0x%02X:0x%02X-0x%02X-0x%02X %d\n",u1I2cAddr,u1RegAddr[0],u1RegAddr[1],*data,I2C_TIMING));
	
	return TRUE;	
}


static UINT8 ATBMChipID(UINT8 u1I2cAddr) // This function checks ATBM884x chip ID,  The chip ID of ATBM884x is 0x20.   
{  
    BOOL fgRet=TRUE;
	UINT8 chipID;
	fgRet&= ATBMRead(u1I2cAddr,0x0000,&chipID);
//	printf("Chip ID= 0x%x, chipID ATBM8846:0x20 ATBM8869:0x30\n",chipID);
	return chipID;
}

BOOL ATBMWriteDemodData(UINT8 u1I2cAddr,pDEMOD_I2C_DATA pData,UINT8 u1Len)
{
    UINT8 ii=0;
	
	while(ii<u1Len)
	{
     if(!ATBMWrite(u1I2cAddr,pData->u2RegAddr,pData->u1RegVal))
    {
     return FALSE;
     }
	 pData++;
	 ii++;
	}
	return TRUE;
}

BOOL ATBMSetTsOutFMT(UINT8 u1I2cAddr)
{
 BOOL fgRet=TRUE;
 ATBM_ChipID= ATBMChipID(u1I2cAddr);
 if(ATBM_ChipID==0x30)
  { 
  fgRet&=ATBMWriteDemodData(u1I2cAddr,TSOutFMTTbl,sizeof(TSOutFMTTbl)/sizeof(TSOutFMTTbl[0]));
  }
 if(ATBM_ChipID==0x40)
  {
  fgRet&=ATBMWriteDemodData(u1I2cAddr,TSOutFMTTbl2,sizeof(TSOutFMTTbl2)/sizeof(TSOutFMTTbl2[0]));
  }
 return fgRet;
}

BOOL ATBMGetSync(UINT8 u1I2cAddr,BOOL * fgLock)
{
   UINT8  u1Data2,u1Mode;
   BOOL fgRet=TRUE;
   
   fgRet&=ATBMRead(u1I2cAddr,0x0013,&u1Mode);//0:DTMB;1:DVBC  
   fgRet&=ATBMRead(u1I2cAddr,0x0b08,&u1Data2);//decode state
   
   *fgLock=(((u1Data2&0x01)==0x01) &&(u1Mode==0x00));//If DTMB,decode state can be also referenced
   	 
    return fgRet;   
}

VOID ATBM_SwitchIfAGC(UINT8 u1I2cAddr,BOOL fgAnalog)
{
 //BOOL fgSts=TRUE;
 //ATBM_ChipID= ATBMChipID(u1I2cAddr);
 UINT32 u4SwitchPin;
 UINT8 SwitchValue=0;
 if(DRVCUST_OptQuery(eTunerRFAGCSwitchGpio,&u4SwitchPin)==0)
 {
 	if(fgAnalog)//ATV
	{
	SwitchValue=1;
	}
	else
	{
	SwitchValue=0;
	}
}
GPIO_SetOut(u4SwitchPin,SwitchValue);
mcDBG_MSG_L2(("Switch IF_AGC to %s,GPIO[%d] = %d\n",(fgAnalog)?"ATV":"DTV",u4SwitchPin,SwitchValue));

}
 #ifdef CC_TUNER_ADAPT_PCB_5301D
static VOID ATBM_SwitchDIF(BOOL fgAnalog)
{
UINT8 u1GpioPort=47;
UINT8 u1GpioSts=0;
 if(fgAnalog)
 {
  u1GpioSts=1;
 }
 else
 {
  u1GpioSts=0;
 }
 GPIO_SetOut((UINT32)u1GpioPort,u1GpioSts);
 mcDBG_MSG_L2(("Switch DIF to %s,Gpio %d=%d\n",(fgAnalog?"ATV":"DTV"),u1GpioPort,u1GpioSts));
}
#endif



BOOL ATBM_Standby(UINT8 u1I2cAddr)
{
 UINT8 u1Data;
 BOOL fgRet=TRUE;
 ATBM_ChipID= ATBMChipID(u1I2cAddr);
  #ifdef CC_TUNER_ADAPT_PCB_5301D
    //Switch DIF to ATV
   ATBM_SwitchDIF(TRUE);
  #endif 
 //Swtich IF_AGC to ATV
 //fgRet&=SetDemodIO(u1I2cAddr,TESTIO11_1);
 
 ATBM_SwitchIfAGC(u1I2cAddr,TRUE);
  
 fgRet&=ATBMWrite(u1I2cAddr,0x0005,0x01);//Set bit0 to 1 to turnoff DSP
 
 fgRet&= ATBMRead(u1I2cAddr,0x060d,&u1Data);
 u1Data|=0x08;  
 fgRet&=ATBMWrite(u1I2cAddr,0x060d, u1Data);
	
 
 //Turn off ADC reference clock
 fgRet&=ATBMRead(u1I2cAddr,0x0600,&u1Data);
 u1Data|=0x02;//bit1 set to 1 to tuen off ADC reference voltage
 fgRet&=ATBMWrite(u1I2cAddr,0x0600,u1Data);
 
 //Turn off ADC
 fgRet&=ATBMRead(u1I2cAddr,0x0602,&u1Data);
 u1Data|=0x01;//bit0 set to 1 to tuen off ADC 
 fgRet&=ATBMWrite(u1I2cAddr,0x0602,u1Data);
 
 //Turnoff DVBC clock
 fgRet&=ATBMWrite(u1I2cAddr,0x1500,0x00);//Set bit0 to 0 to turnoff DVBC
 
 //disable TS output
 fgRet&=ATBMRead(u1I2cAddr,0x0301,&u1Data);
 u1Data|=0x02;//bit1 set to 1 to set TS port Hign-Z
 fgRet&=ATBMWrite(u1I2cAddr,0x0301,u1Data); 

 fgRet&= ATBMRead(u1I2cAddr,0x10f7,&u1Data); 
 u1Data|=0x01; 
 fgRet&=ATBMWrite(u1I2cAddr,0x10f7, u1Data); 
 
 fgRet&=ATBMWrite(u1I2cAddr,0x0019, 0x00); 
 	
 return fgRet;
}
BOOL ATBM_WakeUp(UINT8 u1I2cAddr)
{
  UINT8 u1Data;
  BOOL  fgRet=TRUE;
 
  //fgRet&=ATBMWrite(u1I2cAddr,0x0005,0x00);//Set bit0 to 0 to turn on DSP
 
 //Turn on ADC reference clock
 fgRet&=ATBMRead(u1I2cAddr,0x0600,&u1Data);
 u1Data&=0xFD;//bit1 set to 0 to tuen on ADC reference voltage
 fgRet&=ATBMWrite(u1I2cAddr,0x0600,u1Data);
 
 //Turn on ADC
 fgRet&=ATBMRead(u1I2cAddr,0x0602,&u1Data);
 u1Data&=0xFE;//bit0 set to 0 to tuen off ADC 
 fgRet&=ATBMWrite(u1I2cAddr,0x0602,u1Data);
 
 //Turn on  DVBC clock
 fgRet&=ATBMWrite(u1I2cAddr,0x1500,0x01);//Set bit0 to 1 to turn on DVBC
 
 //enable TS output
 fgRet&=ATBMRead(u1I2cAddr,0x0301,&u1Data);
 u1Data&=0xFD;
 fgRet&=ATBMWrite(u1I2cAddr,0x0301,u1Data);//all TS output PINs will be in normal mode     
 
 fgRet&= ATBMRead(u1I2cAddr,0x10f7,&u1Data);	
 u1Data&=0xfe; 
 fgRet&=ATBMWrite(u1I2cAddr,0x10f7, u1Data); 
 
 fgRet&= ATBMRead(u1I2cAddr,0x060d,&u1Data); 
 u1Data&=0xf7; 
 fgRet&=ATBMWrite(u1I2cAddr,0x060d, u1Data); 
 
 fgRet&=ATBMWrite(u1I2cAddr,0x0019, 0x01); 
 fgRet&=ATBMWrite(u1I2cAddr,0x0005,0x00);

 
 return fgRet;	
}
BOOL ATBM_BypassI2C(UINT8 u1I2cAddr,BOOL fgBypass)
{
//fgBypass=true,bypass ON,otherwise bypass OFF
  // return ATBMWrite(u1I2cAddr,0x0103,fgBypass&0x01);
  return TRUE;
}

static BOOL ATBM_SetIF(UINT8 u1I2cAddr)
{
  UINT16 u2TunerIfFreq=ITunerGetCtx()->u2IF_Freq/100;//0.1MHz,
  UINT32 ui32CLKFreq= C_SAMPLE_CLK_FREQ/100;//0.1MHz origin is MHz
  UINT32 u4Freq_lif=0x4A1350;
  UINT8 u1Data;
  BOOL fgRet=TRUE;
  UINT8 ui8BandWidth=8; //uint: MHz
  UINT32 i32RateRatioTmp;
  UINT16 ui16ParamsFSADC, ui16ParamsIFFreq;
  UINT32 ui32ParamsFSADCInverse;
  
  ATBM_ChipID= ATBMChipID(u1I2cAddr);

  fgRet&=ATBMWrite(u1I2cAddr,0x0c01,0x01);

  if(u2TunerIfFreq>ui32CLKFreq)
  {
   u4Freq_lif=(UINT32)(26353589*(u2TunerIfFreq-ui32CLKFreq)/ui32CLKFreq);
  }
  else//Low if
  {
   u4Freq_lif=(UINT32)(26353589*u2TunerIfFreq/ui32CLKFreq);

  }
  u1Data=(UINT8)(u4Freq_lif&0xFF);
  fgRet&=ATBMWrite(u1I2cAddr,0x0A00,u1Data);
  
  u1Data=(UINT8)((u4Freq_lif>>8)&0xFF);
  fgRet&=ATBMWrite(u1I2cAddr,0x0A01,u1Data);
  
  u1Data=(UINT8)((u4Freq_lif>>16)&0xFF);
  fgRet&=ATBMWrite(u1I2cAddr,0x0A02,u1Data);
  
  mcDBG_MSG_L2(("Set IF 0x%08X IF_Freq=%d (*0.1 MHz)\n",u4Freq_lif,u2TunerIfFreq));

  //Set ADC  rate ratio
  i32RateRatioTmp = (4194304*ui32CLKFreq/(ui8BandWidth*152));

  u1Data=(UINT8)(i32RateRatioTmp&0xff);
  fgRet&=ATBMWrite(u1I2cAddr,0x0A03,u1Data);

  u1Data=(UINT8)((i32RateRatioTmp>>8)&0xff);
  fgRet&=ATBMWrite(u1I2cAddr,0x0A04,u1Data);

  u1Data=(UINT8)((i32RateRatioTmp>>16)&0x1f);
  fgRet&=ATBMWrite(u1I2cAddr,0x0A05,u1Data);
 // end set ADC rate ratio
  
  if(ui32CLKFreq==240)//24MHz crystal, register should set here.
  {
   
   fgRet&=ATBMRead(u1I2cAddr,0x060e,&u1Data);
   u1Data|=0x04;
   fgRet&=ATBMWrite(u1I2cAddr,0x060e,u1Data);
  
   if(ATBM_ChipID==0x30){
	fgRet&=ATBMWrite(u1I2cAddr,0x0afb,0x02);
   	}
    if(ATBM_ChipID==0x40){
	fgRet&=ATBMWrite(u1I2cAddr,0x0afb,0x01);
   	}
   
  }
  if(ATBM_ChipID==0x40){
  ui16ParamsFSADC = (UINT16)(ui32CLKFreq * 2048/10);
  u1Data=(UINT8)(ui16ParamsFSADC/256);
  fgRet&=ATBMWrite(u1I2cAddr,0x0234,u1Data);
  u1Data=(UINT8)(ui16ParamsFSADC&0xff);
  fgRet&=ATBMWrite(u1I2cAddr,0x0233,u1Data);

  ui32ParamsFSADCInverse = 20971520/ui32CLKFreq;
  u1Data=(UINT8)(ui32ParamsFSADCInverse&0xff);
  fgRet&=ATBMWrite(u1I2cAddr,0x029d,u1Data);
  u1Data=(UINT8)((ui32ParamsFSADCInverse>>8)&0xff);
  fgRet&=ATBMWrite(u1I2cAddr,0x029e,u1Data);
   u1Data=(UINT8)((ui32ParamsFSADCInverse>>16)&0x01);
   if(ui32CLKFreq == 240) //24MHz crystal, register should set here.
   	{
   	 u1Data|=0x02;
	 fgRet&=ATBMWrite(u1I2cAddr,0x029f,u1Data);
   	}
   if(ui32CLKFreq == 304) //30.4MHz crystal, register should set here.
   	{
   fgRet&=ATBMWrite(u1I2cAddr,0x029f,u1Data);
   	}
   if(u2TunerIfFreq > ui32CLKFreq) 
		{
			ui16ParamsIFFreq = (u2TunerIfFreq-ui32CLKFreq)*2048/10;//to mhz
		}
	else //low IF
		{
			ui16ParamsIFFreq = u2TunerIfFreq*2048/10;
		}
	u1Data=(UINT8)(ui16ParamsIFFreq/256); 
	fgRet&=ATBMWrite(u1I2cAddr,0x0232,u1Data);
	u1Data=(UINT8)(ui16ParamsIFFreq&0xff); 
	fgRet&=ATBMWrite(u1I2cAddr,0x0231,u1Data);
	
	if(ui8BandWidth == 8)
		{	
			fgRet&=ATBMWrite(u1I2cAddr,0x0251, 0x00);
		}
	else if(ui8BandWidth == 7)
		{
			fgRet&=ATBMWrite(u1I2cAddr,0x0251, 0x01);
		}
	else if(ui8BandWidth == 6)
		{
			fgRet&=ATBMWrite(u1I2cAddr,0x0844, 0x00); 
			fgRet&=ATBMWrite(u1I2cAddr,0x148a, 0x0f);
			fgRet&=ATBMWrite(u1I2cAddr,0x0251, 0x02);
		}else
		{
			fgRet&=ATBMWrite(u1I2cAddr,0x0251, 0x00);
		}
	if(ui32CLKFreq == 240) 
		{
			fgRet&=ATBMWrite(u1I2cAddr,0x0290, 0xd0); 
			fgRet&=ATBMWrite(u1I2cAddr,0x0291, 0x93); 
			fgRet&=ATBMWrite(u1I2cAddr,0x0292, 0x01); 
		}
	if(ui32CLKFreq == 304) 
	{
		fgRet&=ATBMWrite(u1I2cAddr,0x0290, 0x00); 
		fgRet&=ATBMWrite(u1I2cAddr,0x0291, 0xe0); 
		fgRet&=ATBMWrite(u1I2cAddr,0x0292, 0x01); 
	}
  }
    
  return fgRet;
}


BOOL ATBM_Init(UINT8 u1I2cAddr)
{ 
  BOOL fgRet=TRUE;
  ATBM_ChipID= ATBMChipID(u1I2cAddr); 
    mcDELAY_MS(5);
  fgRet&=ATBMWrite(u1I2cAddr,0x010c, 0x01);//i2c clock using PLL, 1:PLL, 0:Crystal.
  fgRet&=ATBMWrite(u1I2cAddr,0x0004, 0x00);// bit: 1:Software complete the register setting 0:register setting  is not completed

  fgRet&=ATBMWrite(u1I2cAddr,0x0003,0x01);//SW reset Need  10 clock to finish reset, try to debug unlock issue

  fgRet&=ATBMWrite(u1I2cAddr,0x10f7, 0xe8);
  fgRet&=ATBMWrite(u1I2cAddr,0x10fb, 0x07);
  if(ATBM_ChipID == 0x40)
	  {
	  fgRet&=ATBMWriteDemodData(u1I2cAddr,ui8AATBM886xCommonReg,sizeof(ui8AATBM886xCommonReg)/sizeof(ui8AATBM886xCommonReg[0])); 	
	  }

  //Function:    ATBMConfig
  fgRet&=ATBM_SetIF(u1I2cAddr); 
  fgRet&=ATBMSetTsOutFMT(u1I2cAddr); /*Configures  MPEG TS output  mode*/		
  fgRet&=ATBMWrite(u1I2cAddr,0x0102,0x01);	//I2C related	
  fgRet&=ATBMWrite(u1I2cAddr,0x020c,0x0A);	//ATBM SoftwareVersion
  //End of ATBMConfig

  ATBM_SwitchIfAGC(u1I2cAddr,FALSE);

	
   return fgRet;
}

static BOOL ATBM_GetIfAGCCtrl(UINT8 u1I2cAddr,UINT16 * u2IfAGCCtrl)
{
  BOOL fgRet=TRUE;
  UINT8 u1InstanReadFlag=1;
  UINT8 u1Data;
  
  fgRet&=ATBMWrite(u1I2cAddr,0x084d,u1InstanReadFlag);//1bit the register is used to make sure that several  registers for the same variable are latched and read at the same instant
  //0x1029[1:0],0x1028[7:0]PWN counter value
  fgRet&=ATBMRead(u1I2cAddr,0x1028,&u1Data);
  * u2IfAGCCtrl=u1Data;
  fgRet&=ATBMRead(u1I2cAddr,0x1029,&u1Data);
  * u2IfAGCCtrl|=((u1Data&0x03)<<8);
  
  u1InstanReadFlag=0;
  fgRet&=ATBMWrite(u1I2cAddr,0x084d,u1InstanReadFlag);//1bit the register is used to make sure that several  registers for the same variable are latched and read at the same instant
  return fgRet;
}
BOOL ATMB_SetFreq(UINT8 u1I2cAddr,PARAM_SETFREQ_T * pPara)
{
 BOOL fgRet=TRUE;
 
 fgRet&=ATBM_BypassI2C(u1I2cAddr,TRUE);
 
 if(ITuner_SetFreq(ITunerGetCtx(),pPara)!=0)
 {
  mcDBG_MSG_INFO(("Fail to set freq\n"));
  fgRet=FALSE;
 }
  fgRet&=ATBM_BypassI2C(u1I2cAddr,FALSE);
  return fgRet;
}
/*UINT16 log2(UINT32 u4Data)
{
  UINT16 jj=1;

  if(u4Data<=1)return 0;

  while(1)
  {
   u4Data>>=1;
   if(u4Data<2)break;
   jj++;
  }
return jj;
}
*/
static UINT16 SNR_log_table_int[]=
{
	0, 100,
		2, 104,
		4, 109,
		6, 114,
		8, 120,
		10, 125,
		12, 131,
		14, 138,
		16, 144,
		18, 151,
		20, 158,
		22, 166,
		24, 173,
		26, 182,
		28, 190,
		30, 199,
		32, 208	
};

/****************************************************************************
Function:    Left_Table
Parameters:  InValue
Return:      OutValue  

  Description:
  This is a sub function called by Log10_Convert.
******************************************************************************/
static UINT16 Left_Table(UINT16 InValue)
{
	UINT16 OutValue = 0;
	UINT16 TableSize = sizeof(SNR_log_table_int)/sizeof(UINT16);
	UINT16 i;	
	for(i=0; i<TableSize; i+=2)
	{	
		if(InValue<=SNR_log_table_int[i+1])
		{		
			OutValue = SNR_log_table_int[i];	
		//	mcDBG_MSG_L1(("InValue = %d, OutValue = %d\n", InValue, OutValue));
			return OutValue;
		}	
	}
	OutValue = 32;
	return OutValue;
}

/****************************************************************************
Function:    Log10_Convert
Parameters:  InValue
Return:      log_ret  

  Description:
  Calculate log value
******************************************************************************/
UINT32 Log10_Convert(UINT32 InValue)
{
	UINT32		u32_temp = 1;
	UINT8		indx = 0;
	UINT32             InValue_left;
	UINT32 log_ret;    

	do {	
		u32_temp = u32_temp << 1;
		if (InValue < u32_temp)
			break;		
	}while(++indx < 32);
	
	log_ret = (301*indx+50)/100;

	InValue_left = InValue*100/(1<<indx);
	
	log_ret = log_ret + Left_Table(InValue_left)/10;
	
	//mcDBG_MSG_L1((" InValue = %d  log ret = %d\n", InValue,  log_ret));
	return log_ret;	
}

BOOL ATBM_TimeOut(UINT8 u1I2cAddr,BOOL* fgTimeOut)
	{
	  BOOL fgRet=TRUE;
	  UINT8 u1Data2;
	  UINT8 u1Mode=0;//DTMB or DVBC mode
	 //fgRet&=ATBMRead(u1I2cAddr,0x0d04,&u1Data1);
	  //Not in GB modulation type
	  fgRet&=ATBMRead(u1I2cAddr,0x082B,&u1Data2);
	  
	   fgRet&=ATBMRead(u1I2cAddr,0x0013,&u1Mode);//Get Current work mode.0:DTMB 1:DVBC
	
	   * fgTimeOut=FALSE;
	   
	   if(u1Mode==0)//DTMB
	   {
		//if((u1Data2!=6)||(u1Data1<3)||(u1Data1>24))
		if(u1Data2!=6)
		{
		 * fgTimeOut=TRUE;
		} 
	   
	   }
	   else//DVBC
	   {
		* fgTimeOut=TRUE;
	   }
	 
	  return fgRet;
	}

BOOL ATBMGetTsFMT(UINT8 u1I2cAddr,TS_FMT_T * pTsFmt)
{
   BOOL fgRet=TRUE;
   UINT8 u1Data;
   
    pTsFmt->ClockPolarity=POL_HIGH_ACTIVE;
	 pTsFmt->fgTristate=BOOL_TRUE;
	 pTsFmt->SerOrPar=SP_PARALLEL;
	 pTsFmt->fgGatedClock=BOOL_TRUE;
	 
	 pTsFmt->u4TsClockInKHz=7600;//7.6M parallel
	 
   fgRet&=ATBMRead(u1I2cAddr,0x0301,&u1Data);
   if(u1Data&0x01)
   {
   //0x0301 bit0 0:rising edge 1:falling edge
   pTsFmt->ClockPolarity=POL_LOW_ACTIVE;
   }
    if(u1Data&0x02)
   {
   //0x0301 bit1 0:enable ts output 1:disable ts output
   pTsFmt->fgTristate=BOOL_FALSE;
   }
    fgRet&=ATBMRead(u1I2cAddr,0x0305,&u1Data);
   if(u1Data&0x01)
   {
   //0x0305 bit0 0:SPI 1:SSI
   pTsFmt->SerOrPar=SP_SERIAL;
   pTsFmt->u4TsClockInKHz=60800;
   }
   
       fgRet&=ATBMRead(u1I2cAddr,0x0306,&u1Data);
   if(u1Data&0x01)
   {
   //0x0306  bit0 0:Gated 1:non-gated
   pTsFmt->fgGatedClock=BOOL_FALSE;
   }
   pTsFmt->fgExternalDemod=BOOL_TRUE;
   return fgRet;
}
BOOL ATBMGetSigLvl(UINT8 u1I2cAddr,MOD_TYPE_T mode,UINT8 * pu1SigLvl)
{
 BOOL fgRet=TRUE;
 UINT16 u2IfagcCtrl;
 fgRet&=ATBM_GetIfAGCCtrl(u1I2cAddr,&u2IfagcCtrl);
 mcDBG_MSG_L2(("IF_AGC_Ctrl=0x%04X(%d)\n",u2IfagcCtrl,u2IfagcCtrl));
 ATBM_BypassI2C(u1I2cAddr,1);
  ITuner_OP(ITunerGetCtx(),itGetSignalLevel,mode,&u2IfagcCtrl);
 ATBM_BypassI2C(u1I2cAddr,0);
 * pu1SigLvl=(UINT8)u2IfagcCtrl;
 
 return fgRet;
}
static UINT8 PLL_Lock_Flag(UINT8 u1I2cAddr)
 {
	 int i = 0;
	 BOOL fgRet=TRUE;
	 UINT8 u1Data;
	 while(i<20)
	 { fgRet&=ATBMRead(u1I2cAddr,0x0611,&u1Data);
		 
		 if((u1Data&0x01)!= 0x01)
		 {
			 return 0;
		 }
		 i++;
	 }
	 return 1;
 }

//ATBM_Check_PLL_Status:When demodulator PLL locked and stable, the returned value is 1.

static UINT8 ATBM_Check_PLL_Status(UINT8 u1I2cAddr)
 {
	 UINT8 pll_state,u1Data;	
	 BOOL fgRet=TRUE;
	 
	 pll_state = PLL_Lock_Flag(u1I2cAddr); 
	 if(pll_state == 1)
	 {
		 fgRet&=ATBMRead(u1I2cAddr,0x0214,&u1Data);//oscillator status
		 if((u1Data&0x02) != 0x02)
		 {		
			fgRet&=ATBMWrite(u1I2cAddr,0x0606, 0x02); //enable the oscillartor clock output
		 }
		 else
		 {
			 fgRet&=ATBMWrite(u1I2cAddr,0x0606, 0x00); //disable the oscillartor clock output
		 }		 
	 }
	 else
	 {
		 return 0;
	 }	
	 
	 x_thread_delay((UINT32)(10));
	 
	 pll_state = PLL_Lock_Flag(u1I2cAddr); 
	 if(pll_state == 1)
	 	
	 {			 
		 return 1;		 
	 }

	 return 0;
 } 

UINT8 ATBMIsDemodOk(UINT8 u1I2cAddr)
{
 BOOL fgRet=TRUE;
 UINT8 u1Data,PLL_state;
 UINT8 ii=0;
 UINT8 ui8Step = 0x20,ui8Analog; 
 ATBM_ChipID= ATBMChipID(u1I2cAddr); 
 fgRet&=ATBMRead(u1I2cAddr,0x0607,&u1Data);//HW status
 
 if((u1Data!=0x05) || (!fgRet))
 {  
    return 0;//need HW reset
 }
 if(ATBM_ChipID==0x20)
 {  
  return 1;
 }
  
 	PLL_state=ATBM_Check_PLL_Status(u1I2cAddr);
 	if(PLL_state==1)
 	{ 	
	 return 1;
 	}
	
 	fgRet&=ATBMRead(u1I2cAddr,0x0214,&u1Data);
	while(ii<5)
	{if((u1Data&0x02)==0x02)
	   {fgRet&=ATBMWrite(u1I2cAddr,0x0606, 0x02); }
	 else 
 	   {fgRet&=ATBMWrite(u1I2cAddr,0x0606, 0x00); }
	 
	 ui8Analog=ui8Step*ii;
 	if(ii==4)
 	   {ui8Analog=0x60;
	 	fgRet&=ATBMWrite(u1I2cAddr,0x0605, 0x00);
 	   }
	fgRet&=ATBMWrite(u1I2cAddr,0x060d, ui8Analog);
               
 	x_thread_delay((UINT32)(10));

	PLL_state=ATBM_Check_PLL_Status(u1I2cAddr);
	if(PLL_state==1)
 	{ 	 
	 return 1;
 	}
	ii++;
	}
	if(!fgRet)
	{
	  mcDBG_MSG_L1(("Demod in abnormal mode\n"));	  
	}
 return 0;
}

VOID ATBMDoHWReset(VOID)
{
  UINT32 u4ResetPin;
  
   if(DRVCUST_OptQuery(eDemodResetGpio,&u4ResetPin)==0)
  {
  mcDBG_MSG_INFO(("Reset Demod GPIO %d 0->1 50 ms\n",u4ResetPin));
  GPIO_SetOut(u4ResetPin,0);
  x_thread_delay((UINT32)(50));//50ms low
  GPIO_SetOut(u4ResetPin,1);  
  x_thread_delay((UINT32)(30));//30ms to wait i2c ok
   }
   else
   {
	 mcDBG_MSG_L1(("No demod reset pin defined\n"));	  
   }

}
BOOL ATBMCheckDemodSts(UINT8 u1I2cAddr,BOOL * fgNeedReset)
{ UINT8 u1Data;
  BOOL fgRet=TRUE;;
  mcDBG_MSG_L4(("start ATBMCheckDemodSts\n"));	  
	fgRet&=ATBMRead(u1I2cAddr,0x0607,&u1Data);//HW status
	
	if((u1Data!=0x05) || (!fgRet))
		{
		 * fgNeedReset=TRUE; //need HW reset	  
		 mcDBG_MSG_L2(("ATBMCheckDemodSts is Abnormal!\n"));	  
		}
	else
		{
		* fgNeedReset=FALSE; //Don't need do  HW reset
		}
	return fgRet;

}

UINT8 ATBMReset(UINT8 u1I2cAddr)//return 1: success, return 0:failed
{
UINT8 ii;
UINT8 fgResetSts=0;
mcDBG_MSG_L4(("start ATBMReset\n"));	  

//Software checking, when power on failed, it will return 0 because I2C error or PLL error
fgResetSts=ATBMIsDemodOk(u1I2cAddr);

if(fgResetSts==1)
	{ 
	mcDBG_MSG_L2(("ATBMIsDemodOk return TRUE!\n"));	
	return 1;
	}

//Hardware checking, usually used after demodulator resume from suspend mode
for(ii=0;ii<2;ii++)
	{
	if(fgResetSts!=1)
		{
		ATBMDoHWReset();
		mcDELAY_MS(50);
		mcDBG_MSG_L2(("need check once again\n"));	
		ATBMIsDemodOk(u1I2cAddr);
		
		}
	else{		
		return 1;
		}
		
	}
return 0;

}
BOOL ATBMPowerOnInit(UINT8 u1I2cAddr)
{ BOOL fgRet=TRUE;
  UINT8 ui8pll;
  UINT8 fgCheckSts=0;
  ATBM_ChipID= ATBMChipID(u1I2cAddr); 
	if(ATBM_ChipID==0x40)
		{
		fgRet&=ATBMWrite(u1I2cAddr,0x010c, 0x00);
		fgRet&=ATBMWrite(u1I2cAddr,0x0606, 0x00);
		fgRet&=ATBMWrite(u1I2cAddr,0x0103, 0x00); 
		fgRet&=ATBMRead(u1I2cAddr,0x060e, &ui8pll);
		fgRet&=ATBMWrite(u1I2cAddr,0x0604, 0x01); 
		ui8pll |= 0x01;
		fgRet&=ATBMWrite(u1I2cAddr,0x060e,ui8pll);	
		ui8pll &= 0xfe;
		fgRet&=ATBMWrite(u1I2cAddr,0x060e,ui8pll);
		fgRet&=ATBMWrite(u1I2cAddr,0x0604, 0x00);
		}
	else if(ATBM_ChipID==0x30)
		{
		fgCheckSts=ATBMReset(u1I2cAddr);
		if(fgCheckSts==0)
			{
			mcDBG_MSG_ERR(("please check Crystal or oscillator setting\n"));
			}
		fgRet&=ATBMWrite(u1I2cAddr,0x0606, 0x00);
		fgRet&=ATBMWrite(u1I2cAddr,0x0103, 0x00);
		}
	else
		{
		mcDBG_MSG_ERR(("power on init failed, chip not correctly detected\n"));
		}
return fgRet;
}
