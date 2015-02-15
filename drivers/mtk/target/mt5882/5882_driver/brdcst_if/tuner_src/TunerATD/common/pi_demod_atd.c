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
 * $RCSfile: pi_tuner_teqe3.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "atd_feature.h"
#include "pi_demod_atd.h"
#include "pi_def_atd.h"
#include "drvcust_if.h"
#include "drv_tvd.h"
#include "x_ckgen.h"
#include "drv_pwm.h"
#include "PD_Def_dbg.h"  /*need include RISC IF macro, if re-ware RISC IF , remove this*/
#include "ctrl_bus.h"
#include "x_hal_5381.h"
#include "source_select.h"
#include "sv_const.h"
#include "pd_atd_if.h"

//For sound carrier shift and OVM
#include "aud_if.h"
//for u4GetAutoSearchType() used when ATD_SetFreq
#include "api_srch_if.h"


/***********************************************************************/
/*                  Defines                                            */
/***********************************************************************/
STATIC PARAM_RISC_T sATDRiscCtx;
volatile UINT32 *puATD_Reg = (volatile UINT32 *) cRISC_ATD_BASE;
#if defined(CC_SUPPORT_EU_US_COMPATIABLE) // iven temp mark
extern UINT8 DynamicSwitch_DTD_DVBT_Mode;/*BRDCST_IF= DTDDVBT_ALL_P_analog for SLT, DTD: DynamicSwitchMode=0,DVBT: DynamicSwitchMode=1(default)*/
#endif
//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

/***********************************************************************/
/*              Private (static) Function Prototypes                   */
/***********************************************************************/

/***********************************************************************/
/*                  Private Types                                      */
/***********************************************************************/

/***********************************************************************/
/*                  Global variables                                   */
/***********************************************************************/
static UINT32 TestItem = 0;
static UINT32 QLabCondition[2] = {700,5};
#ifndef CC_MT5880_CODE_SIZE_CTRL
static UINT8 SmallAGC=0;
#endif

#ifdef CC_POWER_MERGE_TEST
static BOOL  fgTunerAGCThreadRunning=FALSE;
static UINT16 g_u2PowerMerge_Delay = 30;
#endif

UINT8 u1NullRegion = 0x37;
UINT8 Weak_Sinal_Indicator=0, weak_count = 0;	
UINT8 Strong_Chroma_Indicator=0;
UINT8 CCI_Indicator=0;
UINT8 NoOVMCounter0 = cNO_OVM_THRESHOLD;
UINT8 PNCounter=5;	
UINT8 u1CRPatchLoopCnt = 0;
UINT8 u1CrAutoConfig = 0;
UINT8 u1CrBandwidth = 6;

UINT8 CCI_Tone_BW3[4][3] = {{0x0F,0xFE,0x00},{0x0F,0xFD,0xFD},{0x0F,0xFD,0xF9},{0x0F,0xFD,0xCF}};
UINT8 CCI_Tone_BW2[4][3] = {{0x0F,0xFE,0x00},{0x0F,0xFB,0xFD},{0x0F,0xFB,0xF9},{0x0F,0xFB,0xCF}};

BOOL fg_Acq_flag = FALSE;

#if ATD_SUPPORT_NCR  //d20120116_hongji:NCR Glober Var
BOOL fg_Enable_BN = TRUE;
BOOL fg_Enable_CR_BW_Auto = TRUE;
#if ATD_SUPPORT_HW_AUTO_BW
BOOL fg_CR_BW_Ctrl_Switch = TRUE;  //d20120417_hongji:1:HW  0:SW
BOOL fg_cci_disable_hbw = FALSE;
#endif
BOOL fgNCN_FLAG = TRUE;
BOOL fgLCN_STATUS = FALSE;
#if defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
BOOL fg_jump_weighting_status = FALSE;
#endif
UINT8 su1KpUpCnt = 0;
//define the threshold for VIF;d111024;LW
#define ToneDetectThreshold_PAL 0x0130 //PAL
#define ToneDetectThreshold_NTSC 0x00C0 //NTSC
#define ToneDetectThreshold_SECAM 0x0230 //SECAM
BOOL fg_NCR_Sel = TRUE;  //d20120116_hongji: NCR is 1,OCR is 0. Default is NCR
#endif

#if (ATD_SUPPORT_VSI|ATD_SUPPORT_H_SYNDROME)
BOOL fgVSI_On=TRUE;
#if ATD_SUPPORT_H_SYNDROME
//d20121109_hongji:define threshold for H-Syndrome
#define TONE_DETECTION_THRESHOLD 0x0100  //PAL & NTSC    
#define TONE_DETECTION_THRESHOLD_L 0x0010  //SECAM
#define LINE_RATE_REF  864
#endif
#endif
#ifdef CC_SUPPORT_STR
EXTERN ATD_CTX_T*    psAtdeuDemodCtx;
EXTERN ATD_CTX_T*    psAtdnaDemodCtx;
#endif

#ifdef CC_SUPPORT_ATV_DESCRAMBLE
static UINT32 u4aChinaATVChannels[4][2] = {{49750,3},{77250,2},{112250,44},{471250,50}};
#define CC_CN_ATV_FREQ_OFFSET 200 //200KHz for CN ATV offset from channel table
#endif

/***********************************************************************/
#define fgCRMOdify 1//d110510_Haibo:Enable new CR patch for MT5368 IC verification
#define cCR_FIFO_DELAY_LENGTH_Ext 410
#define cCR_FIFO_DELAY_LENGTH_Org 192

//#if (defined CC_MT5389)
#if (defined(CC_MT5389) || defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882))  // Bypass the OVM Mode for MT5396
#define fgOVMBypass 1
#else
#define fgOVMBypass 0
#endif

#define cNO_OVM_CNT_THRHLD            5
#define cNO_PN_CNT_THRHLD             5
#define cCR_PATCH_NOISE_THRHLD_HIGH    0x19
#define cCR_PATCH_NOISE_THRHLD_LOW     0x15
#define cCR_PATCH_CMTRX_PN_THRHLD      0x20
#define cDRO_CVBS_UP_BUD_0_Hihg        0x3F
#define cDRO_CVBS_UP_BUD_0_Low        0x28

#define NCR_PSBOUND  179   //for Remy ATD slt fail,Mustang follow Remy

#if fgCRMOdify
static UINT8 u1CrPatchNoPnCntr = cNO_PN_CNT_THRHLD;
static UINT8 u1CrPatchNoOvmCntr = cNO_OVM_CNT_THRHLD;
UINT8 ucCrPatchState = cCR_PATCH_STATE_NORM_PN_FN;
#endif
ATD_CTX_T *ATD_DemodCtxCreate(void)
{
    ATD_CTX_T *p = NULL;

	CREATE_OBJECT(p, ATD_CTX_T);

    // error handling
    if (!p)
       return NULL;

   	// zero memory
	x_memset(p, 0, sizeof(ATD_CTX_T));

	return p;
}

void ATD_DemodCtxDestroy(ATD_CTX_T *p)
{
    DESTROY_OBJECT(p);
}




/***********************************************************************/
/*                      Private (static) Function                      */
/***********************************************************************/
/***********************************************************************/
UINT8 ATD_SetReg(ATD_CTX_T *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{
#if ATD_SUPPORT_BATCH_RW//d110623:ATD support batch write
    if (ICtrlBus_RiscDemWrite(&sATDRiscCtx, (u2RegAddr+ATD_REG_OFFSET), pu1Buffer, u2ByteCount))
    {
        mcSHOW_HW_MSG((" _@ICtrlBus Write %d byte Error: Reg[%03X] = 0x%02X!\n",
                       u2ByteCount, u2RegAddr, *pu1Buffer));
		return 1;
    }
#else
    UINT16 u2ByteCntIdx=0;
    mcSHOW_DBG_MSG4(("Non-batch write\n"));
	for(;u2ByteCntIdx<u2ByteCount;u2ByteCntIdx++)
	{
        if (ICtrlBus_RiscDemWrite(&sATDRiscCtx, (u2RegAddr+ATD_REG_OFFSET+u2ByteCntIdx), pu1Buffer+u2ByteCntIdx,1))
        {
            mcSHOW_HW_MSG((" _@ICtrlBus Write  byte Error: Reg[%03X] = 0x%02X!\n",u2RegAddr+ATD_REG_OFFSET+u2ByteCntIdx, *(pu1Buffer+u2ByteCntIdx)));
        	return 1;
        }
	}
#endif
	return 0;
}

/************************************************************************/
/* Read consecutive registers */
UINT8 ATD_GetReg(ATD_CTX_T *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{
#if ATD_SUPPORT_BATCH_RW//d110623:ATD support batch write
    if (ICtrlBus_RiscDemRead(&sATDRiscCtx, (u2RegAddr+ATD_REG_OFFSET), pu1Buffer, u2ByteCount))
    {
        mcSHOW_HW_MSG((" _@ICtrlBus Read %d byte Error: Reg[%03X] = 0x%02X!\n",
                       u2ByteCount, u2RegAddr, *pu1Buffer));
        return 1;
    }
#else
    UINT16 u2ByteCntIdx=0;
	mcSHOW_DBG_MSG4(("Non-batch read\n"));
	
	for(;u2ByteCntIdx<u2ByteCount;u2ByteCntIdx++)
	{
        if (ICtrlBus_RiscDemRead(&sATDRiscCtx, (u2RegAddr+ATD_REG_OFFSET+u2ByteCntIdx), pu1Buffer+u2ByteCntIdx,1))
        {
            mcSHOW_HW_MSG((" _@ICtrlBus Read  Error: Reg[%03X] = 0x%02X!\n",u2RegAddr+ATD_REG_OFFSET+u2ByteCntIdx, *(pu1Buffer+u2ByteCntIdx)));
            return 1;
        }
    }
#endif
    return 0;
}



/***********************************************************************/
/*                  Public Functions (not static)                      */
/***********************************************************************/

UINT8 Demod_SetReg(ATD_CTX_T *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{
    if (ICtrlBus_RiscDemWrite(&sATDRiscCtx, u2RegAddr, pu1Buffer, u2ByteCount))
    {
        mcSHOW_HW_MSG((" _@ICtrlBus Write %d byte Error: Reg[%03X] = 0x%02X!\n",
                       u2ByteCount, u2RegAddr, *pu1Buffer));
	    return 1;
    }
    return 0;
}

/************************************************************************/
/* Read consecutive registers */
UINT8 Demod_GetReg(ATD_CTX_T *psDemodCtx, UINT16 u2RegAddr, UINT8 *pu1Buffer, UINT16 u2ByteCount)
{
    if (ICtrlBus_RiscDemRead(&sATDRiscCtx, u2RegAddr, pu1Buffer, u2ByteCount))
    {
        mcSHOW_HW_MSG((" _@ICtrlBus Read %d byte Error: Reg[%03X] = 0x%02X!\n",
                       u2ByteCount, u2RegAddr, *pu1Buffer));
            return 1;
    }
    return 0;
}




/************************************************************************
*  ATD_TunerAtdCommand
*  Analog tuner ATD command.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  i4AtdArgc : ATD command parameter number.
*  @param  aszAtdArgv : ATD command parameter.
*  @retval  void.
************************************************************************/
/*move this API to correct location*/
static INT32 TunerNullBreak(VOID* pArg)
{
    UNUSED(pArg);
	return (FALSE);
}

static UINT16 u2AtdDroTarLvlInitial =0;

extern UINT16 g_CPR_delay;
extern UINT16 g_CPR_cnt;
extern UINT16 g_pra_cnt;
extern UINT16 g_CR_BW_AutoConfig_Delay_cnt;


/**********************************************************************/
/*                  Initialization APIs                               */
/**********************************************************************/

/**********************************************************************/
/*                  Configuration APIs                                */
/**********************************************************************/


/**********************************************************************/
INT32 ATD_Initialize(ATD_CTX_T *psDemodCtx, UINT8 ucI2cAddress, BOOL fgLoadCode)
{
    if (!psDemodCtx)
    {
        return DRVAPI_TUNER_ERROR;
    }
    psDemodCtx->I2cAddress = ucI2cAddress;
    psDemodCtx->fgDemodReset = FALSE;
    psDemodCtx->u1SubSysIdx = MOD_PAL_BG;  // PAL_BG
    psDemodCtx->fgPRALock = FALSE;
    psDemodCtx->fgCRLock = FALSE;
    psDemodCtx->fgAutoSearch = FALSE;
    psDemodCtx->fgAFT = TRUE;
    psDemodCtx->u1PF = 0x01;
    psDemodCtx->u2VOPCpo = 0;
    psDemodCtx->s4Cfo = 0;
    psDemodCtx->u1VIFLock = cATD_VIF_UNLOCK;
    psDemodCtx->u1UpChkCnt = 0;
    psDemodCtx->u1UpNakCnt = 0;
    psDemodCtx->u1UpMisCnt = 0;
    psDemodCtx->u4Frequency = 181250; //default frequency

    psDemodCtx->s1IFmin = 0x80;
    psDemodCtx->s1RFmin = 0x80;

    //For new RISC IF       
    sATDRiscCtx.pu4RegBase = puATD_Reg;    
#if defined(CC_SUPPORT_EU_US_COMPATIABLE)	
	if(DynamicSwitch_DTD_DVBT_Mode == 0)		
	{		
	sATDRiscCtx.pu4RegBase = (UINT32 *) 0xf002b000;			
	}	
	else		
	{		
	sATDRiscCtx.pu4RegBase = (UINT32 *) 0xf002a000;			
	}
#endif
	#if 0 //lei130823 mark unused words;
	// create semaphore for HostCmd Interface
    if (mcSEMA_CREATE(&psDemodCtx->hHostCmdLock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_HW_MSG(("TunerOpen: mcSEMA_CREATE(hHostCmdLock) Fail!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK(psDemodCtx->hHostCmdLock);
	
    if (mcSEMA_CREATE(&psDemodCtx->hLogLock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_HW_MSG(("TunerOpen: mcSEMA_CREATE(hLogLock) Fail!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK(psDemodCtx->hLogLock);

	if (mcSEMA_CREATE(&psDemodCtx->hRegLock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_HW_MSG(("TunerOpen: mcSEMA_CREATE(hRegLock) Fail!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK(psDemodCtx->hRegLock);

	if (!psDemodCtx->fgDemodInitFlag)
    {
        psDemodCtx->fgDemodInitFlag = TRUE;
        if (mcSEMA_CREATE(&psDemodCtx->hDemodAcq, X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcSHOW_HW_MSG(("TunerOpen: mcSEMA_CREATE(hDemodAcq) Fail!\n"));
            return (DRVAPI_TUNER_ERROR);
        }
        mcSEMA_UNLOCK(psDemodCtx->hDemodAcq);
    }
	#endif

    ATD_ChipInit(psDemodCtx);

    ITuner_OP(ITunerGetCtx(), itSetTunerInit, (UINT8)TRUE, NULL); //Xiaohong_20140811
    // According to MW/Albert, Extinguish operation after opening.
    //lei130823 mark unused words;
    //mcBLOCK_I2C(TRUE);

    return (DRVAPI_TUNER_OK);
}

#ifndef CC_MT5880_CODE_SIZE_CTRL
VOID ATD_SetAgc(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;
    ATD_AGC_CTX* psAtdAgcPara = &(psDemodCtx->pTCtx->sAtdAgcPara);
    UINT8 u1Ifmin=0;
    // AGC
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_RF_BIAS,&(psAtdAgcPara->AgcRfBias),1);  //RF BIAS
    ucData=0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_IF_BIAS,&ucData,1);  //IF BIAS
    ucData=0x80;
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_RF_SLOPE,&ucData,1);  //RF SLOPE
    ucData=0x80;
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_IF_SLOPE,&ucData,1);  //IF SLOPE
    ucData=0x7F;
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_RF_MAX,&ucData,1);  //RF MAX
    ucData=0x7F;
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_IF_MAX,&ucData,1);  //IF MAX
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_RF_MIN,&(psAtdAgcPara->AgcRfMin),1);  //RF MIN

    //depart  the tuner-dependent part to [ITUNER] layer , Jack Mei 2009/11/3    
    ucData = psDemodCtx->u1SubSysIdx;
    if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx, itSetIfMin,ucData, &u1Ifmin))
    {
        ATD_SetReg(psDemodCtx,REG_ATD_PRA_IF_MIN,&u1Ifmin,1);
    }
    else
    {
        ATD_SetReg(psDemodCtx,REG_ATD_PRA_IF_MIN,&(psAtdAgcPara->AgcIfMin),1);  //for other tuner IF MIN
        u1Ifmin=psAtdAgcPara->AgcIfMin;
    }
   
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_DR_SEL,&(psAtdAgcPara->AgcDrSel),1);  //PRA Dynamic Range
    ucData = 0x83;
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_LP1_CFG12,&ucData,1);  //REG_ATD_PRA_LP1_CFG12, disable PRA auto-target 
    //To fix CR DTV00138738
    if((MOD_SECAM_L == psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))
    {
        ucData = 0x43;
        ATD_SetReg(psDemodCtx,REG_ATD_PRA_LP1_CFG12,&ucData,1);  //REG_ATD_PRA_LP1_CFG12, disable PRA auto-target	 
    }
    switch (psAtdAgcPara->AgcDrSel) //mapping PRA target based on dynamic range value
    {
        case 0x00: 
            ucData = 0x60; 
            break;
        case 0x01: 
            ucData = 0x70; 
            break;
        case 0x02: 
            ucData = 0x80; 
            break;
        case 0x03: 
            ucData = 0x88; 
            break;
        case 0x04: 
            ucData = 0x90; 
            break;
        case 0x05: 
            ucData = 0x98; 
            break;
        case 0x06: 
            ucData = 0xA0; 
            break;
        case 0x07: 
            ucData = 0xA8; 
            break;
        case 0x08: 
            ucData = 0xB0; 
            break;
        case 0x09: 
            ucData = 0xB8; 
            break;
        case 0x0A: 
            ucData = 0xC0; 
            break;
        case 0x0B: 
            ucData = 0xC8;
            break;
        case 0x0C: 
            ucData = 0xD0;
            break;
        case 0x0D: 
            ucData = 0xE0; 
            break;
        case 0x0E: 
            ucData = 0xF0;
            break;
        case 0x0F: 
            ucData = 0xFF; 
            break;
        default:   
            ucData = 0x80;
            break;
    }
    ATD_SetReg(psDemodCtx,REG_ATD_PRA_LP1_CFG05,&ucData,1);  //REG_ATD_PRA_LP1_CFG05  

    psDemodCtx->s1IFmin = u1Ifmin;
    psDemodCtx->s1RFmin = psAtdAgcPara->AgcRfMin;
}
#endif


// Set CR pilot filter
// u1PF= 0x00 ==> Disable pilot filter
// u1PF= 0x01 ==> ¡Ó1.05 MHz.
// u1PF= 0x10 ==> ¡Ó2.11 MHz.
// u1PF= 0x11 ==> ¡Ó6.25 MHz.
void ATD_SetPF(ATD_CTX_T *psDemodCtx, UINT8 u1PF)
{
    UINT8 ucData;

    psDemodCtx->u1PF = u1PF;
    ATD_GetReg(psDemodCtx, REG_ATD_PS_CFG, &ucData, 1);

    ucData = (ucData & 0xFC) | u1PF;

    ATD_SetReg(psDemodCtx, REG_ATD_PS_CFG, &ucData, 1);
    mcSHOW_DBG_MSG(("Pilot filter is 0x%x \n",ucData));
}

BOOL ATD_SetFreq(ATD_CTX_T *psDemodCtx, UINT32 u4FreqInKHz, UINT8 fgAutoSearch)
{
    BOOL fgReturn;
    PARAM_SETFREQ_T ATD_param;
	UINT8 ucAutoSearchType;

    mcSHOW_DBG_MSG(("Before SetFreq\n"));

	//wenming 2012-4-16: ignore fgAutoSearch parameter, and get AutoSearchType from MW
	//Two diffrences: 1. For fine tune, fgAutoSearch will be true before modification, and will be FINE_TUNE after modification(will still act true if do not modify in tuner driver).
	//2.For driver settings of fgAutoSearch, such as AFT set true, will be not available now
	UNUSED(fgAutoSearch); 
	ucAutoSearchType = (UINT8)u4GetAutoSearchType();

    psDemodCtx->u4Frequency = u4FreqInKHz;
    psDemodCtx->fgAutoSearch = ucAutoSearchType;
    
    ATD_param.Freq = u4FreqInKHz;
    ATD_param.Modulation = psDemodCtx->u1SubSysIdx;
    ATD_param.fgAutoSearch = ucAutoSearchType;
	//d20130423_wenming: To avoid PAL DK detected as MN issue, set PAL DK to tuner if auto-search & try BG
	if((ATV_CONN_MODE_CHAN_SCAN == ucAutoSearchType) && (MOD_PAL_BG == psDemodCtx->u1SubSysIdx))
	{
		ATD_param.Modulation = MOD_PAL_DK;
		mcSHOW_DBG_MSG(("Set PAL_DK to tuner when try BG at scanning>>>>>>\n"));
	}
		
    if(FALSE == fg_Acq_flag)  //d20120720_hongji:Don't do bypass if it is called by acq
    {
        DigTunerBypassI2C (TRUE); //EC_code_d9826 for eIntATD_ExtDTD
    }
    fgReturn = ITuner_SetFreq(psDemodCtx->pTCtx, &ATD_param);  
    if(FALSE == fg_Acq_flag)
    {
        DigTunerBypassI2C (FALSE);
    }

    mcSHOW_DBG_MSG(("After SetFreq\n"));
    
    if (fgReturn)
    {
        fgReturn = FALSE;
    }
    else
    {
        fgReturn = TRUE;
    }
    return fgReturn;
}
#if ATD_SUPPORT_VSI
void ATD_SetVSIScanMode(ATD_CTX_T *psDemodCtx)
{
	UINT8 u1SpecNum=0;
	UINT8 u1LinevarPer=0x20;//default is 0.125
	//624 line.PAL
	u1SpecNum=0x70;
	ATD_SetReg(psDemodCtx,REG_ATD_VIF_CFG01,&u1SpecNum,1);
	ATD_GetReg(psDemodCtx,REG_ATD_VIF_CFG02,&u1SpecNum,1);
	u1SpecNum&=0x3F;
	u1SpecNum|=0x80;
	ATD_SetReg(psDemodCtx,REG_ATD_VIF_CFG02,&u1SpecNum,1);
	//scan mode var percent is 0.25(624*(1+-0.25)
	u1LinevarPer=0x40;
	ATD_SetReg(psDemodCtx,REG_ATD_VIF_CFG03,&u1LinevarPer,1);	
	
	
	mcSHOW_DBG_MSG(("Set VSI scan mode\n"));
}
#endif
void ATD_SetScanMode(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;
#if ATD_SUPPORT_NCR
    if(!fg_NCR_Sel)  //d20120116_hongji:Select NCR Setting
    {
    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
    ucData = ucData | 0xC0;
    ATD_SetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
    mcSHOW_DBG_MSG(("Enable CR auto Re-acq config for ATD scan mode, 0x997 = 0x%X\n", ucData)); 
    }
#else  //d20120115_hongji:OCR Setting
    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
    ucData = ucData | 0xC0;
    ATD_SetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
    mcSHOW_DBG_MSG(("Enable CR auto Re-acq config for ATD scan mode, 0x997 = 0x%X\n", ucData)); 
#endif

    switch (psDemodCtx->u1SubSysIdx)
    {
        case MOD_SECAM_L:  // to speed up PRA 
        case MOD_SECAM_L1:
            mcSHOW_DBG_MSG(("Set scan mode parameter for SECAM L/L'\n"));
            
            //ZY, 100126, SECAM L scan loss issue. PRA speed-up at channel scan
            ucData = 0x84; 
			ATD_SetReg(psDemodCtx, REG_ATD_PRA_LP1_CFG02, &ucData, 1); //0x42->0x84, fast loop speed
            ATD_GetReg(psDemodCtx, REG_ATD_PRA_LP1_CFG07, &ucData, 1);
            ucData = ucData & 0xEF;   //REG_ATD_PRA_LP1_CFG07[4]=0, middle-term
            ATD_SetReg(psDemodCtx, REG_ATD_PRA_LP1_CFG07, &ucData, 1); 
            break;

        case MOD_PAL_BG:  // to enlarge audio filter in scan mode for TV spec detection
        case MOD_PAL_DK:
        case MOD_PAL_I:
            ucData = 0x8C;
            ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1);
            ucData = 0x20;
            ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1);
            ucData = 0x81;
            ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);
            ucData = 0xD0;
            ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);
			#if ATD_SUPPORT_CHF
            ATD_SetCHF(psDemodCtx,8);  //d20120113_hongji:8M CHF Setting for BG Scan mode
            ATD_SetIF(psDemodCtx);  //d2012020_hongji:IF Setting for BG Scan mode
			#endif
            break;

        default:
            break;
    }
    
	if((psDemodCtx->u1SubSysIdx>=MOD_ANA_TYPE_BEGIN) &&(psDemodCtx->u1SubSysIdx<MOD_NTSC_M))//Only need reset AVS when sys_sub_index is PAL and SECAM
    {
        mcSHOW_DBG_MSG(("Reset AVS\n"));
        ATD_GetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
        ucData &= 0xFB;
        ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
        ucData |= 0x04;
        ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    }
}


/**********************************************************************/
#ifndef CC_MT5880_CODE_SIZE_CTRL
INT32 ATD_DemodReset(ATD_CTX_T *psDemodCtx)  // need further implementation
{

	return (DRVAPI_TUNER_OK);
}
#endif

STATIC BOOL TVD_CCI_Indicator(VOID)
{		
	mcSHOW_DBG_MSG2(("TVD CCI_Index 0x%X\n",(IO_READ16(VIDEO_IN0_BASE, 0x60)) & 0x001));

    if(((IO_READ16(VIDEO_IN0_BASE, 0x60)) & 0x001) > 0) // CCI Present
    {
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}
#if defined(CC_MT8223)||defined(CC_MT5365)||defined(CC_MT5395)||defined(CC_MT5389) 
STATIC UINT16 ATD_CheckToneMag(ATD_CTX_T *psDemodCtx,UINT8 CCIidx)
{
    UINT8 ucValue;
    UINT16 ToneMag = 0;
    //Load Tone Detection Script
    //1. Set Tone Detection BW3
    ATD_GetReg(psDemodCtx, REG_ATD_TD_CFG, &ucValue, 1);
    ucValue &= 0xF0;
    ucValue |= 0x0D;
    ATD_SetReg(psDemodCtx, REG_ATD_TD_CFG, &ucValue, 1);
    //2. Load Tone Script
    ATD_SetReg(psDemodCtx, REG_ATD_TD_BPF_MUL_COEF_0, &(CCI_Tone_BW3[CCIidx][0]), 1);
    ATD_SetReg(psDemodCtx, REG_ATD_TD_BPF_MUL_COEF_1, &(CCI_Tone_BW3[CCIidx][1]), 1);
    ATD_SetReg(psDemodCtx, REG_ATD_TD_BPF_MUL_COEF_2, &(CCI_Tone_BW3[CCIidx][2]), 1);
    //3. Get Tone Amp
    mcDELAY_MS(10);
    ATD_GetReg(psDemodCtx, REG_ATD_TD_MAX_0, &ucValue, 1);
    ToneMag += ucValue << 8;
    ATD_GetReg(psDemodCtx, REG_ATD_TD_MAX_1, &ucValue, 1);
    ToneMag += ucValue;
    mcSHOW_DBG_MSG2(("TVD CCI Tone Mag 0x%X, %d\n",ToneMag, CCIidx));
    return (UINT16)ToneMag;
}
#endif


void ATD_SetAutoConfig(ATD_CTX_T *psDemodCtx, UINT8 u1CrAcfMsk)
{
    UINT8 ucValue = 0;

    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucValue, 1);             
    ucValue &= 0xCF;
    ucValue |= u1CrAcfMsk;
    ATD_SetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucValue, 1);  //turn-off PN & FN auto-config            
    mcSHOW_DBG_MSG2(("CR Auto Config = 0x%02X \n", u1CrAcfMsk ));

}
void ATD_SetCrFIFO(ATD_CTX_T *psDemodCtx, UINT8 u1coef_a,UINT8 u1coef_b)
{    
	UINT8 ucCrFifoDelay = 0;
	UINT16 u2CrFifoDelayTmp=(UINT16)(13 + mcBIT(u1coef_a) + mcBIT(u1coef_b));
	
#if ATD_SUPPORT_CR_ENLARGED_FIFO
    UINT8 ucCRFIFOSel;
    ATD_GetReg(psDemodCtx, REG_ATD_CR_FIFO_SELECTION, &ucCRFIFOSel, 1);    //using bit7 to identify the CR FIFO length
	if ((IC_VER_5396_AA!=BSP_GetIcVersion()) && (ucCRFIFOSel &0x80))    // the MAX CR FIFO length is 410 symbols
	{
	    mcSHOW_DBG_MSG(("The CR FIFO length is in the Extension mode!!\n")); 
	    mcSHOW_DBG_MSG2(("The CR FIFO length is betseen 0 ~ 410 symbols!!\n"));
        if (u2CrFifoDelayTmp > cCR_FIFO_DELAY_LENGTH_Ext) 
		{
            ucCrFifoDelay = cCR_FIFO_DELAY_LENGTH_Ext/2;
        }
        else 
		{
            ucCrFifoDelay = (UINT8)(u2CrFifoDelayTmp/2);
        }
	}
	else  // the MAX CR FIFO length is 192 symbols
	{
	    mcSHOW_DBG_MSG(("The CR FIFO length is in the Origional mode!!\n"));
	    mcSHOW_DBG_MSG2(("The CR FIFO length is betseen 0 ~ 192 symbols!!\n"));
	    if (u2CrFifoDelayTmp > cCR_FIFO_DELAY_LENGTH_Org) 
		{
            ucCrFifoDelay = cCR_FIFO_DELAY_LENGTH_Org;
        }
        else 
		{
            ucCrFifoDelay = (UINT8)(u2CrFifoDelayTmp);
        }		
	}
#else
    mcSHOW_DBG_MSG(("The CR FIFO length is in the Origional mode!!\n"));
	mcSHOW_DBG_MSG2(("The CR FIFO length is betseen 0 ~ 192 symbols!!\n"));
	if (u2CrFifoDelayTmp > cCR_FIFO_DELAY_LENGTH_Org) 
	{
        ucCrFifoDelay = cCR_FIFO_DELAY_LENGTH_Org;
    }
    else 
	{
        ucCrFifoDelay = (UINT8)(u2CrFifoDelayTmp);
    }	
#endif
    ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG1, &ucCrFifoDelay, 1);
    return;
}
void ATD_SetCrBw(ATD_CTX_T *psDemodCtx, UINT8 u1CrBw)
{
#if fgCRMOdify
    UINT8 ucPnFnCfg, ucPnFnDect, ucCrBwSet, u1CrBwLpf, ucFnCrBw, u1CrBwTmp;
    UINT8 coef_a, coef_b;	
    ucPnFnCfg = 0;
    ucPnFnDect = 0;
	ucFnCrBw = 0;
    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucPnFnCfg, 1); //PN-FN auto config
    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucPnFnDect, 1); //PN-FN Detection
	ucPnFnCfg &=0x30;
    ucPnFnDect &=0x0c;
	
	if (((0x30 == ucPnFnCfg)&&(0x04 == ucPnFnDect)) || ((0x10 == ucPnFnCfg)&&(ucPnFnDect >=0x04)))
	{
	    mcSHOW_DBG_MSG2(("FN only case, modify the FIFO delay!! \n"));
		ATD_GetReg(psDemodCtx, REG_ATD_ACR_FN_CFG1, &u1CrBw, 1);
		u1CrBw &= 0x0F;
	}
    mcSHOW_DBG_MSG2(("Set CR BW = 0x%02X \n", u1CrBw));
    if ((u1CrBw&0xF0)>0) //setting the CR BW when the u1CrBwLpf ~= u1CrBw
    {
        u1CrBwLpf = (u1CrBw&0x0f);
		u1CrBwTmp = (u1CrBw&0xf0);
		u1CrBw = (u1CrBwTmp >> 4);
    }
	else
	{
        u1CrBwLpf = u1CrBw;
	}
    ucCrBwSet = (0x30|u1CrBwLpf);  
	ATD_SetReg(psDemodCtx, REG_ATD_TRAN_STG, &ucCrBwSet, 1); 
    ucCrBwSet = (0x40|u1CrBw);     
	ATD_SetReg(psDemodCtx, REG_ATD_PRR_TRAN, &ucCrBwSet, 1);
    ucCrBwSet = (0x30|u1CrBw);     
	ATD_SetReg(psDemodCtx, REG_ATD_PRR_PRFK, &ucCrBwSet, 1);
    ucCrBwSet = (0x40|u1CrBwLpf);  
	ATD_SetReg(psDemodCtx, REG_ATD_PRR_DAMRK,&ucCrBwSet, 1);//0x1b3
    if (cCR_PATCH_STATE_FN_ONLY == ucCrPatchState) 
    {
        ATD_GetReg(psDemodCtx, REG_ATD_ACR_FN_CFG1, &ucFnCrBw, 1); 
        coef_a = mcGET_FIELD(ucFnCrBw, 0xF0, 4);
        coef_b = mcGET_FIELD(ucFnCrBw, 0x0F, 0);
    }
    else 
    {
        coef_a = u1CrBw;
        coef_b = u1CrBwLpf;
    }

	ATD_SetCrFIFO(psDemodCtx,coef_a,coef_b);
#else
    UINT8 ucCrBwSet;

    ucCrBwSet = (0x30|u1CrBw);  
	ATD_SetReg(psDemodCtx, REG_ATD_TRAN_STG, &ucCrBwSet, 1); 
    ucCrBwSet = (0x40|u1CrBw);  
	ATD_SetReg(psDemodCtx, REG_ATD_PRR_TRAN, &ucCrBwSet, 1);
    ucCrBwSet = (0x30|u1CrBw);  
	ATD_SetReg(psDemodCtx, REG_ATD_PRR_PRFK, &ucCrBwSet, 1);
    ucCrBwSet = (0x40|u1CrBw);  
	ATD_SetReg(psDemodCtx, REG_ATD_PRR_DAMRK,&ucCrBwSet, 1);
    mcSHOW_DBG_MSG2(("Set CR BW = 0x%02X \n", u1CrBw));
#endif
    return;
}


UINT8 ATD_GetOVMIdx(ATD_CTX_T *psDemodCtx)
{
#if ATD_SUPPORT_DRO_SYNC_MODE//get ovm idx from DRO
    UINT8 ucData, ucGainJitter, ucCRstate0;
	UINT8 ucOVMIdx = 0;
	UINT8 ucOVMOffset = 1; //to consist with the previous methof that the OVM index gat frpm CR_State0
	UINT32 u4Fref2 = 0;
	UINT32 u4Fref2Step = 0;
	UINT32 u4SyncSwing = 0;
	UINT32 u4OVMTarget;//, u4SyncHighTmp;

    mcSHOW_DBG_MSG2(("OVM index: The New OVM index method!!\n"));
    // Step1. Set the MUX
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1);
    ucData &= 0xD0;
	ucData |= 0x21;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1);
    // Step2. Get the Fref2 and Swing, High
    //Swing
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MAX1, &ucData, 1);
	mcSHOW_DBG_MSG2(("OVM index: The REG_ATD_DRO_LSYNC_MAX1 is 0x%02X!!\n", ucData));
    u4SyncSwing = ucData;
	ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MAX2, &ucData, 1);
	mcSHOW_DBG_MSG2(("OVM index: The REG_ATD_DRO_LSYNC_MAX2 is 0x%02X!!\n", ucData));
	u4SyncSwing = (u4SyncSwing<<8);
	u4SyncSwing &= 0xff00;
	u4SyncSwing |= ucData;
	u4SyncSwing = (u4SyncSwing<<4);//resolution = 1/16
	u4SyncSwing &= 0xfffffff0;
	//Fref2
	ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData, 1);
	mcSHOW_DBG_MSG2(("OVM index: The REG_ATD_DRO_RPOBE_1 is 0x%02X!!\n", ucData));
    u4Fref2 = ucData;
	ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData, 1);
	mcSHOW_DBG_MSG2(("OVM index: The REG_ATD_DRO_PROBE_2 is 0x%02X!!\n", ucData));
	u4Fref2 = (u4Fref2<<8);
	u4Fref2 &= 0xff00;
	u4Fref2 |=ucData;
	u4Fref2Step = u4Fref2;    //resolution = 1/16
	u4Fref2 = (u4Fref2<<4);
	u4Fref2 &= 0xfffffff0;	

    u4OVMTarget = u4SyncSwing;

    // Step4. compute the OVM index
    if(u4OVMTarget >= 0x80000000) //to avoid the negative value
    {
        mcSHOW_DBG_MSG2(("OVM index: the swing is negative!!\n"));
		ucOVMIdx +=ucOVMOffset;
		mcSHOW_DBG_MSG2(("OVM index: The index is 0x%02X!!\n", ucOVMIdx + ucOVMOffset));
	    return ucOVMIdx ;		
    }

    if (u4OVMTarget < u4Fref2)
    {
        mcSHOW_DBG_MSG2(("OVM index: The index is less than 100% !!\n"));
    	ucOVMIdx +=ucOVMOffset;
		mcSHOW_DBG_MSG2(("OVM index: The index is 0x%02X!!\n", ucOVMIdx + ucOVMOffset));
        return ucOVMIdx;
    } 

	while (ucOVMIdx < 6)
	{	    	
		ucOVMIdx +=1;
		u4Fref2 += u4Fref2Step;
		if (u4OVMTarget < u4Fref2)
        {
			break;
        }
		mcSHOW_DBG_MSG2(("OVM index: Searching the OVM index !!\n"));
	}
    // Step4. return the OVM index
	mcSHOW_DBG_MSG2(("OVM index: The index is 0x%02X!!\n", ucOVMIdx + ucOVMOffset));
	ucOVMIdx +=ucOVMOffset;

	ucData = 0x2E;
	ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1); // DRO probe sel: gain_jitter	 
	ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucGainJitter, 1);
	mcSHOW_DBG_MSG(("Gain jitter=0x%02X\n",ucGainJitter));
	if(ucGainJitter>0x0a)    //Avoid the AM-FV case
	{
	    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucCRstate0, 1); 
		ucOVMIdx = ((ucCRstate0 & 0x70)>>4);
	    ucOVMIdx &=0x07;	
		mcSHOW_DBG_MSG(("Get the OVM index from 0x1A0\n"));
	}

	return ucOVMIdx;
    
#elif ATD_SUPPORT_OVM_HW_MAX_HOLD//d110627_Haibo:MT5389 get the OVM MAX Hold directly(no need to set OVM_THD more)
    UINT8 u1Data;
	UINT8 u1OVMIdx=1;
	if(IC_VER_5396_AA==BSP_GetIcVersion())//MT5396 before ECO
    {
        mcSHOW_DBG_MSG2(("IC Ver=53960000 OVM index: The OVM index form the CR model!!\n"));
        while(u1OVMIdx<8)
        {
            u1Data=(u1OVMIdx<<5);
            ATD_SetReg(psDemodCtx, REG_ATD_OVM_THD, &u1Data, 1);//Set OVM THD
            ATD_GetReg(psDemodCtx, REG_ATD_CR_OVMIND, &u1Data, 1);//Read OVM IND twice
            ATD_GetReg(psDemodCtx, REG_ATD_CR_OVMIND, &u1Data, 1);//Read OVM IND
            if(u1Data &0x80)
            {
                mcSHOW_DBG_MSG(("OVMIDX=%d\n",u1OVMIdx));
                u1OVMIdx++;
            }
            else
            {
                break;
            }
        }
        return (u1OVMIdx-1);	
    }
	else
    {
        ATD_GetReg(psDemodCtx, REG_ATD_CR_OVMIND, &u1Data, 1);//Read OVM IND
        mcSHOW_DBG_MSG2(("OVM index: Get the Max hold %d!!\n",((u1Data>>5)&0x07)));
#if ATD_SUPPORT_OVM_ACCURATE_RANK	
        mcSHOW_DBG_MSG(("Normalized OVM index: %d\n",(u1Data>>1)&0x0F));
#endif
        return ((u1Data>>5)&0x07);
	}
#else  //for MT5396/68 ES
	UINT8 u1OVMIdx=1;
	UINT8 u1Data,u2Data;
	mcSHOW_DBG_MSG2(("OVM index: The OVM index form the CR model!!\n"));
    ATD_GetReg(psDemodCtx, REG_ATD_OVM_THD, &u2Data, 1);
    u2Data &= 0x1F;
	while(u1OVMIdx<8)
	{
        u1Data=(u1OVMIdx<<5);
        u1Data |= u2Data;
        ATD_SetReg(psDemodCtx, REG_ATD_OVM_THD, &u1Data, 1);//Set OVM THD
        ATD_GetReg(psDemodCtx, REG_ATD_CR_OVMIND, &u1Data, 1);//Read OVM IND twice
        ATD_GetReg(psDemodCtx, REG_ATD_CR_OVMIND, &u1Data, 1);//Read OVM IND
        if(u1Data &0x80)
        {
           mcSHOW_DBG_MSG(("OVMIDX=%d\n",u1OVMIdx));
           u1OVMIdx++;
        }
        else
        {
           break;
        }
	}
	return (u1OVMIdx-1);	
#endif
}

#if defined(CC_MT8223)||defined(CC_MT5365)||defined(CC_MT5395)||defined(CC_MT5389) 
STATIC void ATD_LoadCCIScript(ATD_CTX_T *psDemodCtx,UINT8 CCIidx)
{
    UINT8 ucValue;
    // for Q-lab CCI case, disable CVBS scaling, fine-tune CR loop BW    
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_0, &ucValue, 1);
    ucValue = ucValue & 0x7F;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_SCAL_0, &ucValue, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_DCR_EXT1, &ucValue, 1);
    ucValue = ucValue & 0x7F;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_DCR_EXT1, &ucValue, 1);
    ucValue = 0x37;
    ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucValue, 1);
}
#endif

STATIC void ATD_Patch_CCI(ATD_CTX_T *psDemodCtx)
{    	
#if defined(CC_MT8223)||defined(CC_MT5365)||defined(CC_MT5395)||defined(CC_MT5389) 
    UINT8 ii;
#endif
    UINT8 ucValue;

    if(fgIsSrcAtv(SV_VP_MAIN)||fgIsSrcAtv(SV_VP_PIP))
    {
        if(TVD_CCI_Indicator()) // CCI Present
        {       
        	mcSHOW_DBG_MSG(("TVD Enter CCI\n"));
#if defined(CC_MT8223)||defined(CC_MT5365)||defined(CC_MT5395)||defined(CC_MT5389)             
            for(ii=1;ii<4;ii++)
            {
                if( ATD_CheckToneMag(psDemodCtx,ii) > 0x0200)
                {
                    CCI_Indicator = 6;
                    mcSHOW_DBG_MSG(("Load CCI %d\n", ii));
                    ATD_LoadCCIScript(psDemodCtx, ii);
                	break;
                }      	
            }
            if( 4 == ii)
         	{
                mcSHOW_DBG_MSG(("Can't Findout any tone\n"));
         	}	              
#else  //d20121205_hongji:for 96/68/98/5880/5860/5881 and future IC version, no TD_BPF module
            CCI_Indicator = 6;
            mcSHOW_DBG_MSG(("Don't support TD_BPF function\n"));
#endif
        }       
        else
        {	            	
        	if(CCI_Indicator > 1)
            {
                CCI_Indicator--;
            }
            else if(1 == CCI_Indicator)		
            {	
                mcSHOW_DBG_MSG(("TVD Exit CCI\n"));
                ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_0, &ucValue, 1);
                ucValue = ucValue | 0x80;
                ATD_SetReg(psDemodCtx, REG_ATD_DRO_SCAL_0, &ucValue, 1);
                ATD_GetReg(psDemodCtx, REG_ATD_DRO_DCR_EXT1, &ucValue, 1);
                ucValue = ucValue | 0x80;
                ATD_SetReg(psDemodCtx, REG_ATD_DRO_DCR_EXT1, &ucValue, 1);
                CCI_Indicator = 0;
            }	
        }	

        if (CCI_Indicator != 0) 
        {
#if ATD_SUPPORT_NCR   //d20120205_hongji:CCI Patch for NCR
            if(TRUE == fg_NCR_Sel)
            {
                if((MOD_SECAM_L != psDemodCtx->u1SubSysIdx) && (MOD_SECAM_L1 != psDemodCtx->u1SubSysIdx))
                {                                    
#if ATD_SUPPORT_HW_AUTO_BW
                    if((FALSE==IS_IC_5880())&&(TRUE==fg_CR_BW_Ctrl_Switch))
                    {
                        ATD_GetReg(psDemodCtx,REG_ATD_NCR_HBW_0,&ucValue,1);
                        ucValue &= 0x7F;
                        ATD_SetReg(psDemodCtx,REG_ATD_NCR_HBW_0,&ucValue,1);
                        fg_cci_disable_hbw = TRUE;
                    }
                    else
                    {
                        fg_Enable_CR_BW_Auto = FALSE;
                    }
#else
                    fg_Enable_CR_BW_Auto = FALSE;
#endif
                    ATD_GetReg(psDemodCtx,REG_ATD_NCR_FSM_12,&ucValue,1);
                    ucValue &= 0x0F;
                    ucValue |= 0x90;
                    ATD_SetReg(psDemodCtx,REG_ATD_NCR_FSM_12,&ucValue,1);
                    mcSHOW_DBG_MSG(("Set NCR Kp as 0x09 and disable BW auto config for CCI Case\n")); 
#if defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
                    ATD_GetReg(psDemodCtx,REG_ATD_NCR_PLL_4,&ucValue,1);
                    if(fg_jump_weighting_status)
                    {
                        ucValue &= 0xF9;  //d20120616_hongji:reset jump weighting to *1
                        ATD_SetReg(psDemodCtx,REG_ATD_NCR_PLL_4,&ucValue,1);
                        mcSHOW_DBG_MSG2(("0x2a8 = 0x%X\n", ucValue));
                        fg_jump_weighting_status = FALSE;
                        mcSHOW_DBG_MSG2(("Kp exit range [5,8], Reset jump weighting to *1 for CCI case \n"));
                    }
#endif                    
                }    
            }
            else
            {
                u1CrAutoConfig = 0;
                u1CrBandwidth = 7;
            }
#else
            u1CrAutoConfig = 0;
            u1CrBandwidth = 7;
#endif        
        }
#if ATD_SUPPORT_NCR    
        else
        {
            if(TRUE == fg_NCR_Sel)
            {                              
#if ATD_SUPPORT_HW_AUTO_BW
                if((FALSE==IS_IC_5880())&&(TRUE==fg_CR_BW_Ctrl_Switch))
                {
                    if(TRUE == fg_cci_disable_hbw)
                    {
                        ATD_GetReg(psDemodCtx,REG_ATD_NCR_HBW_0,&ucValue,1);
                        ucValue |= 0xA0;
                        ATD_SetReg(psDemodCtx,REG_ATD_NCR_HBW_0,&ucValue,1);
                        fg_cci_disable_hbw = FALSE;
                    }
                }
                else
                {
                    fg_Enable_CR_BW_Auto = TRUE; 
                }
#else
                fg_Enable_CR_BW_Auto = TRUE; 
#endif
            }
        }
#endif    
    }
    else
    {
        CCI_Indicator = 0x00;
    }
}


STATIC void ATD_Patch_Sensitivity(ATD_CTX_T *psDemodCtx)
{	
    UINT8 ucTVDstate;            
    UINT8 ucNoiselevel, ucThreshold1, ucThreshold2; 
    
    if((MOD_SECAM_L == psDemodCtx->u1SubSysIdx) || (MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))
    {
    	ucThreshold1 = 0x60;
    	ucThreshold2 = 0x40;
    }
    else
    {
    	ucThreshold1 = 0x60;
    	ucThreshold2 = 0x50;
    }
    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucNoiselevel, 1);
    mcSHOW_DBG_MSG2(("ATD noise level = 0x%02x\n", ucNoiselevel));    
    
    ucTVDstate = IO_READ8(VIDEO_IN0_BASE, 0x9D);    
    mcSHOW_DBG_MSG2(("TVD color catch = 0x%02x\n", (ucTVDstate & 0x01)));
    mcSHOW_DBG_MSG2(("weak signal indicator = %d\n", Weak_Sinal_Indicator));
    mcSHOW_DBG_MSG2(("weak signal counter = %d\n", weak_count));
    
    if( (ucNoiselevel > ucThreshold1) && (weak_count < 5) )
    {
    	weak_count++;
        if((5 == weak_count) && (0 == Weak_Sinal_Indicator) && ((ucTVDstate & 0x01)==0x00) ) 
        {
    	    ATD_SetEQ(psDemodCtx, itGetEqScriptWeak);
    	    Weak_Sinal_Indicator = 1;
    	    mcSHOW_DBG_MSG(("Enter weak signal mode!\n"));
        }
    }
    else
    {    	
    	if((ucNoiselevel < ucThreshold2) && (weak_count > 0))    	
    	{
    	    weak_count--;
    	}
    	
    	if((0 == weak_count) && (1 == Weak_Sinal_Indicator)) 
    	{
    	    ATD_SetEQ(psDemodCtx, itGetEqScriptNormal);
    	    Weak_Sinal_Indicator = 0;
    	    mcSHOW_DBG_MSG(("Escape weak signal mode!\n"));
        }
    }    
}

STATIC void ATD_OCR_Patch(ATD_CTX_T *psDemodCtx)
{	
#if fgCRMOdify
	UINT8 ucData = 0;	 
	UINT8 ucCRstate, ucNoiselevel = 0, ucCmetrix = 0, ucOvmIdx, ucPNIndex, ucFNIndex;
	UINT8 ucNullRegion, ucCfoTraStep, fgAutoReconfig;
#if fgEnable_CR_AVG_Mode
    UINT8 fgPrrAvgEn;
#endif
	ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucData, 1);	   
	ucCRstate = mcGET_FIELD(ucData, MSK_ATD_CR_STATE0_CR_STATE, 0);
    ucOvmIdx=ATD_GetOVMIdx(psDemodCtx);		
	
	if (ucCRstate != cCR_STATE_TRA) 
    {
		mcSHOW_DBG_MSG2(("CR not in TRA, state = 0x%02X\n", ucCRstate));
		return;
	}
	ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucNoiselevel, 1);
	ATD_GetReg(psDemodCtx, REG_ATD_CMETRIC0, &ucCmetrix, 1);	
	ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucData, 1);	  
	ucPNIndex = mcGET_FIELD(ucData, MSK_ATD_CR_STATE1_PNIND, 3);
	ucFNIndex = mcGET_FIELD(ucData, MSK_ATD_CR_STATE1_FNIND, 2);
	mcSHOW_DBG_MSG2(("CMetrix	= 0x%02X\n", ucCmetrix));
	mcSHOW_DBG_MSG2(("NoiseLvl	= 0x%02X\n", ucNoiselevel));
	mcSHOW_DBG_MSG2(("PN/FN/OVM = %d/%d/%d\n", ucPNIndex, ucFNIndex, ucOvmIdx));
	
	if (fgOVMBypass)	
	{
		ucOvmIdx=0;    //Test for the OVM case
		mcSHOW_DBG_MSG2(("Bypass OVM mode\n"));
	}
	
	if(ucNoiselevel < cCR_PATCH_NOISE_THRHLD_LOW) // normal power
    {   
        if ((0 == ucPNIndex) && (0 == ucFNIndex)) 
        {
            if(ucOvmIdx >= 2)//d110702_Haibo:OVM mode
            {
                u1CrPatchNoOvmCntr=0;
                ucCrPatchState = cCR_PATCH_STATE_OVM;
            }
            else
            {
                if(u1CrPatchNoOvmCntr<cNO_OVM_CNT_THRHLD)//d110702_Haibo:OVM transient mode
                {
                    u1CrPatchNoOvmCntr++;
                    ucCrPatchState = cCR_PATCH_STATE_OVM_WAIT;
                }
                else
                {
                    ucCrPatchState =cCR_PATCH_STATE_NORM; 
                }
            }
        }
        else 
        {
            if((0 == ucPNIndex) && (ucFNIndex != 0)) 
            {
            	ucCrPatchState = cCR_PATCH_STATE_FN_ONLY;
            }
            else 
            {
            	ucCrPatchState = cCR_PATCH_STATE_NORM_PN_FN;
            }
        }
    }
    else if (ucNoiselevel > cCR_PATCH_NOISE_THRHLD_HIGH) // weak power
    { 
        if (ucCmetrix < cCR_PATCH_CMTRX_PN_THRHLD)
        {
        	if (u1CrPatchNoPnCntr < cNO_PN_CNT_THRHLD) 
            {
        		u1CrPatchNoPnCntr++;
        		ucCrPatchState = cCR_PATCH_STATE_WEAK_SIG_WAIT;
        	}
        	else 
            {
        		ucCrPatchState = cCR_PATCH_STATE_WEAK_SIG;
        	}
        }
        else 
        {
            u1CrPatchNoPnCntr=0;
        	ucCrPatchState = cCR_PATCH_STATE_WEAK_SIG_PN;
        }
    }
	mcSHOW_DBG_MSG(("CR State=%d\n",ucCrPatchState));
	switch (ucCrPatchState) 
    {
        case cCR_PATCH_STATE_WEAK_SIG:
#if fgEnable_CR_AVG_Mode
            u1CrBandwidth = 6;
            ucNullRegion = 1;
            ucCfoTraStep = 4;
            fgPrrAvgEn = 0;
#else
            u1CrBandwidth = 7;//d110903_Haibo:weak signal CR BW is 7
            ucNullRegion = 1;
            ucCfoTraStep = 4;
#endif
            fgAutoReconfig = 0;
            mcSHOW_DBG_MSG2(("CR state: cCR_PATCH_STATE_WEAK_SIG!\n"));
            break;
            
        case cCR_PATCH_STATE_WEAK_SIG_WAIT:
#if fgEnable_CR_AVG_Mode 
            u1CrBandwidth = 5;
            ucNullRegion = 1;
            ucCfoTraStep = 0;
            fgPrrAvgEn = 0;
#else
            u1CrBandwidth = 5;
            ucNullRegion = 2;
            ucCfoTraStep = 0;
#endif
            fgAutoReconfig = 0;
            mcSHOW_DBG_MSG2(("CR state: cCR_PATCH_STATE_WEAK_SIG_WAIT!\n"));
            break;
            
        case cCR_PATCH_STATE_WEAK_SIG_PN:
#if fgEnable_CR_AVG_Mode 
            u1CrBandwidth = 5;
            ucNullRegion = 1;
            ucCfoTraStep = 0;
            fgPrrAvgEn = 0;
#else
            u1CrBandwidth = 5;
            ucNullRegion = 2;
            ucCfoTraStep = 0;
#endif
            fgAutoReconfig = 0;
            mcSHOW_DBG_MSG2(("CR state: cCR_PATCH_STATE_WEAK_SIG_PN!\n"));
            break;
            
        case cCR_PATCH_STATE_STRONG_OVM:
#if fgEnable_CR_AVG_Mode 
            u1CrBandwidth = 6;
            ucNullRegion = 1;
            ucCfoTraStep = 4;
            if ((0 == ucPNIndex) && (0 == ucFNIndex)) 
            {
                fgPrrAvgEn =1;
            }
            else 
            {
                fgPrrAvgEn = 0;
            }
#else
            u1CrBandwidth = 7;
            ucNullRegion = 2;
            ucCfoTraStep = 4;
#endif
            fgAutoReconfig = 0;
            mcSHOW_DBG_MSG2(("CR state: cCR_PATCH_STATE_STRONG_OVM!\n"));
            break;
        case cCR_PATCH_STATE_OVM:
#if fgEnable_CR_AVG_Mode 
            u1CrBandwidth = 6;
            ucNullRegion = 1;
            ucCfoTraStep = 4;
            if ((0 == ucPNIndex) && (0 == ucFNIndex)) 
            {
                fgPrrAvgEn = 1;
            }
            else 
            {
                fgPrrAvgEn = 0;
            }
#else
            u1CrBandwidth = 7;
            ucNullRegion = 2;
            ucCfoTraStep = 4;
#endif
            fgAutoReconfig = 1;
            mcSHOW_DBG_MSG2(("CR state: cCR_PATCH_STATE_OVM!\n"));
            break;
        case cCR_PATCH_STATE_OVM_WAIT:
        case cCR_PATCH_STATE_NORM_PN_FN:
        case cCR_PATCH_STATE_FN_ONLY:
        case cCR_PATCH_STATE_NORM:
#if fgEnable_CR_AVG_Mode
            u1CrBandwidth = 6;
            ucNullRegion = 1;
            ucCfoTraStep = 4;
            if ((0 == ucPNIndex) && (0 == ucFNIndex)) 
            {
                fgPrrAvgEn = 1;
            }
            else 
            {
                fgPrrAvgEn = 0;
            }
#else
            u1CrBandwidth = 6;
            ucNullRegion = 2;
            ucCfoTraStep = 4;
#endif
            fgAutoReconfig = 1;
            mcSHOW_DBG_MSG2(("CR state: cCR_PATCH_STATE_NORM_PN_FN!\n"));
            break;
        default:
#if fgEnable_CR_AVG_Mode 
            u1CrBandwidth = 6;
            ucNullRegion = 1;
            ucCfoTraStep = 4;
            if ((0 == ucPNIndex) && (0 == ucFNIndex)) 
            {
                fgPrrAvgEn =fgEnable_CR_AVG_Mode;
            }
            else 
            {
                fgPrrAvgEn = 0;
            }
#else
            u1CrBandwidth = 6;
            ucNullRegion = 2;
            ucCfoTraStep = 4;
#endif
            fgAutoReconfig = 1;
            mcSHOW_DBG_MSG2(("CR state: error state!\n"));
            break;
    }
    ATD_SetNullRegion(psDemodCtx,ucNullRegion);
	ATD_GetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);
	mcSET_FIELD(ucData, ucCfoTraStep, MSK_ATD_PE_CFOK_CFO_K_TRK, 0);
	ATD_SetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);
	ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
	if (fgAutoReconfig) 
    {
		mcSET_MASK(u1CrAutoConfig, (MSK_ATD_ACR_CFG_PN_ACFG_EN|MSK_ATD_ACR_CFG_FN_ACFG_EN));
	}
	else 
    {
		mcCLR_MASK(u1CrAutoConfig, (MSK_ATD_ACR_CFG_PN_ACFG_EN|MSK_ATD_ACR_CFG_FN_ACFG_EN));
	}
#if fgEnable_CR_AVG_Mode 
    ATD_GetReg(psDemodCtx, REG_ATD_CR_AVGPE1, &ucData, 1);
    if (fgPrrAvgEn) // no PN->enable hw auto->PN->HW AVG disable->maually disable
    {
		mcSET_MASK(ucData, MSK_ATD_CR_AVGPE1_AVG_PRR_EN);//0x08 b3:1
    }
    else //manually disable
    {
	     mcCLR_MASK(ucData, MSK_ATD_CR_AVGPE1_AVG_PRR_EN);//b3:0
    }
    ATD_SetReg(psDemodCtx, REG_ATD_CR_AVGPE1, &ucData, 1);	
#endif
#else    //origion method;d110328
    UINT8 ucData, ucCRstate0, ucCRstate1, ucNoiselevel, ucCmetrix, ucOVMIdx;	
	UINT8 ucPRFDelay, ucLPFDelay,ucPRFShl, ucLPFShl;
	UINT8 ucPRFDelayIni=0x01;
	UINT8 ucLPFDelayIni=0x01;
             
    if(u1CRPatchLoopCnt < cCR_PATCH_CHANGE_PN_THRESHOLD) 
    {
        u1CRPatchLoopCnt++;
    }
    else 
    {
        ucData = 0x1C;            
        ATD_SetReg(psDemodCtx, REG_ATD_CR_PNDEC_CFG, &ucData, 1);    
    }	
    
    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucCRstate0, 1);         	
 	
    if( ((ucCRstate0 & 0x03) == 0x03))   //CR in tracking state
    {
        ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucNoiselevel, 1);    
        if(ucNoiselevel > 0x19)  //signal < 25dBuV, weak power
        {		                
            u1CrAutoConfig = 0;
            NoOVMCounter0 = cNO_OVM_THRESHOLD;    
            ATD_GetReg(psDemodCtx, REG_ATD_CMETRIC0, &ucCmetrix, 1);    
            mcSHOW_DBG_MSG2(("CMetrix = 0x%02X\n", ucCmetrix));
            if (ucCmetrix < 0x20)  // CMetrix <= REG_ATD_DRO_OBS20, weak PN
            {
                if (PNCounter < cPN_CNTR_THRESHOLD) 
                {
                    PNCounter++;
                    mcSHOW_DBG_MSG2(("CR entering weak signal mode...waiting\n"));
                }
                else
                {
                    u1CrBandwidth = 7;                  
                    ucData = 0x17;  
					ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucData, 1);  //lower null-region for CR weak signal mode
                    ucData = 0x7C;  
					ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG1, &ucData, 1);  //FIFO delay                                                                                           
                    //GA FT issue, DISABLE pilot quality mask of pilot stablity                                                                 
                    ATD_GetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);    
                    ucData |= 0x04; 
                    ATD_SetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);
                    mcSHOW_DBG_MSG2(("CR enter weak signal mode!\n"));
                }
            }
            else
            {
                PNCounter = 0;
                u1CrBandwidth = 5; //a little wider BW
                ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG0, &u1NullRegion, 1);  //higher null-region for CR weak signal+PN mode
                ucData = 0x4D;	
				ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG1, &ucData, 1);  //FIFO delay     
                
                //GA FT issue, ENABLE pilot quality mask of pilot stablity
                ATD_GetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);   
                if((ucData & 0x04) == 0x04)  
                {
                    ucData = ucData & 0x80; 
                    ATD_SetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);
                }     
                mcSHOW_DBG_MSG2(("CR enter weak signal+PN mode!\n"));                
            }				
        }
        else if(ucNoiselevel < 0x15)  // signal > 25dBuV, normal power
        {
            ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucData, 1);
            if ((ucData & 0x20) >> 5 ) // if pilot stability == 1
            {  
                u1CrAutoConfig = cFN_ACFG_EN|cPN_ACFG_EN;               
            }
            else 
            {
                u1CrAutoConfig = cFN_ACFG_EN|cPN_ACFG_EN;             
                mcSHOW_DBG_MSG2(("Pilot Stability = 0 !!\n"));
            }                    
            PNCounter = cPN_CNTR_THRESHOLD;   	    
            ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG0, &u1NullRegion, 1);  //higher null-region for CR strong signal mode    	    
            ATD_GetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1); //GA FT issue    
            if((ucData & 0x04) != 0x04)  
            {
                ucData = ucData | 0x04; 
                ATD_SetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);
            } 
            ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucData, 1);    
            ucCRstate1 = ucData;    	    
            mcSHOW_DBG_MSG2(("PN/FN = 0x%02X\n", (ucCRstate1&0x0C)>>2 ));
			ucOVMIdx=ATD_GetOVMIdx(psDemodCtx);
			mcSHOW_DBG_MSG(("OVM IDX=%d\n",ucOVMIdx));
            if(ucOVMIdx>=4)  
            {
                u1CrBandwidth = 7; //narrow BW
                NoOVMCounter0 = 0;
                mcSHOW_DBG_MSG2(("CR enter strong OVM mode!\n"));
            }
            else if ((ucCRstate1 & 0x0C) == 0x00)  // no PN/FN		
            {
                ucData = 0x7C;  
				ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG1, &ucData, 1);  //FIFO delay       
                
                if(ucOVMIdx>=2)
                {
                    u1CrBandwidth = 7; //narrow BW
                    NoOVMCounter0 = 0;
                    mcSHOW_DBG_MSG2(("CR enter OVM mode!\n"));
                }
                else
                {
                    if (NoOVMCounter0 < cNO_OVM_THRESHOLD) 
                    {
                        NoOVMCounter0++;
                        mcSHOW_DBG_MSG2(("CR entering normal mode, waiting...\n"));		
                    }                        
                    else 
                    {
                        u1CrBandwidth = 6; 
                        mcSHOW_DBG_MSG2(("CR enter normal mode!\n"));			
                    }
                }
            }
            else  // PN/FN exists
            {              
                if((ucCRstate1 & 0x0C) == 0x04)  //only FN exists
                {
                    ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG1, &ucData, 1);             
                    if(ucData != 0x0C)
                    {            
                        ucData = 0x0C;
                        ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG1, &ucData, 1);  //FIFO delay       
    	            }    	
    	        } 
				else	//auto-compuate the fifo delay when PN exists;LiWei;d100818
				{
    				ATD_GetReg(psDemodCtx, REG_ATD_ACR_PN_CFG1, &ucData, 1); 
                    ucPRFShl = ((ucData >> 4) & 0x0f);
    				ucLPFShl = (ucData & 0x0f);
    				ucPRFDelay = (ucPRFDelayIni << ucPRFShl);
    				ucLPFDelay = (ucLPFDelayIni << ucLPFShl);
    				ucData = 0x0d + ucPRFDelay + ucLPFDelay;
    				ATD_SetReg(psDemodCtx, REG_ATD_ACR_PN_CFG0, &ucData, 1);
				}                      	
                u1CrBandwidth = 6; 
                NoOVMCounter0 = cNO_OVM_THRESHOLD;
                mcSHOW_DBG_MSG2(("CR in PN/FN mode!\n"));
    	    }            
    	}
    }
#endif
}

#if ATD_SUPPORT_NCR
void ATD_NCR_BN_Patch(ATD_CTX_T *psDemodCtx)
{
	UINT8   ucData, ucBN_detect_thd;
	UINT16  fDRE, fData;   //d20120116_hongji:change Float type to INT type
	UINT8 	ucBN_scal = 3;
	UINT16   ucBN_upper_bound = 0x7F8;  //255*8 = 2040
	UINT16   ucBN_lower_bound = 0x388;  //113*8 = 904

	ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_6,&ucData, 1);
	fDRE = (UINT16)ucData * 16;  //d20120116_hongji:change Float type to INT type
	mcSHOW_DBG_MSG3(("Current DRE = 0x%x\n", fDRE));
    if(fDRE < 0x220)  //d20120920_hongji: Set DRE thd to 0x22 for unplug/plug awgn detect fail issue
    {
        fDRE = 0x220;
    }

	fData = fDRE * ucBN_scal;  //bn_detect_thd
	
	if((MOD_SECAM_L != psDemodCtx->u1SubSysIdx)&&(MOD_SECAM_L1 != psDemodCtx->u1SubSysIdx))	
    {   
        if(fData > ucBN_upper_bound) //upper bound: 225*8=2040
    	{
    		ucBN_detect_thd= ucBN_upper_bound/8;
    		mcSHOW_DBG_MSG3(("upper bound: bn_detect_thd = 0x%x\n", ucBN_upper_bound));
     	}
    	else if(fData < ucBN_lower_bound) // lower bound: 113*8=904
    	{
    		ucBN_detect_thd = ucBN_lower_bound/8;
    		mcSHOW_DBG_MSG3(("lower bound: bn_detect_thd = 0x%x\n", ucBN_lower_bound));
    	}
    	else
    	{
    		ucBN_detect_thd= (UINT8)(fData/8);
    		mcSHOW_DBG_MSG3(("bn_detect_thd = 0x%x\n", fData));
    	}
    }
    else
    {
        ucBN_detect_thd = 0xC4;  //d20120301_hongji:SECAM(positive modulation) use a fixed value
        mcSHOW_DBG_MSG3(("SECAM L/L1:Fix bn_detect_thd = 0x%x\n",(ucBN_detect_thd*8)));
    }
	
	ATD_SetReg(psDemodCtx,REG_ATD_NCR_PLL_3, &ucBN_detect_thd, 1);
    return;
}
#endif
#if ATD_SUPPORT_NCR
void ATD_NCR_NegBW_Patch(ATD_CTX_T *psDemodCtx)
{
    UINT8   ucData, ucKP,ucAWGN,ucPn_indicator, jj;
    UINT16  u2DRE, u2Pd_switch_thd;    
    UINT32  u4Awgn_pn_ratio;
    UINT32  u4Avg_ratio=0x00; 
    UINT32  u4Avg_pn_indicator=0x00;
    UINT32  u4Avg_awgn=0x00; 
	UINT32 u4MaxAvg_ratio = 0x00; //d20130327_wenming:Sync A2 CL:1162737
	UINT8 ucMaxPNInd = 0x00;
    UINT8 ucAWGNArr[MEASURE_SAMPLE_CNT]={0};
	UINT32  u4Avg_CVBS_noise=0x00;	
#if !USE_NCR_AWGN 
    UINT8   ucCVBS_noise;
    UINT32  u4Mapping_awgn;
#endif
    BOOL    fgBW_Protection = FALSE;   


#if USE_NCR_AWGN
    mcSHOW_DBG_MSG2(("Negative Modulation use NCR awgn !\n"));
#else    
    mcSHOW_DBG_MSG2(("Negative Modulation use DRO noise level mapping AWGN !\n")); 
#endif

    // Normal CN setting When enable "ENABLE LCN CONFIGRUATION"
    if(fgNCN_FLAG)
    {
        //set pd_switch_thd 
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_6,&ucData, 1);        
        u2DRE = ucData * 16;
        mcSHOW_DBG_MSG2(("DRE : 0x%x\n", u2DRE));            
        u2Pd_switch_thd = u2DRE * DRE_RATIO;  //set 138%OVM=>160%OVM pd_switch_thd
        mcSHOW_DBG_MSG2(("Normal CN Setting:Set pd_switch_threshold to %d/100 \n",(DRE_RATIO*10)));
        ucData = (u2Pd_switch_thd / 10 / 8) + 0x01;
        ATD_SetReg(psDemodCtx,REG_ATD_NCR_ADV_3, &ucData,1);            

#if ATD_SUPPORT_OVM_SMASH  //OVM Smash function
        if(IC_VER_5398_AA!=BSP_GetIcVersion()) //d2012_hongji:MT5398ES Version don't support this function
        {
            // disable "dis_smash" to overcome overcome weak + OVM
            ATD_GetReg(psDemodCtx,REG_ATD_NCR_ADV_28,&ucData, 1);
            ucData = 0x04 | ucData; //0x2DF[2]=1
            ATD_SetReg(psDemodCtx,REG_ATD_NCR_ADV_28, &ucData,1);       
        }
#endif        
        fgNCN_FLAG = FALSE;
    }
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)    
    //d20130222_hongji:phase_rev_indicator update
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV32,&ucData, 1);
    mcSHOW_DBG_MSG(("ph_rev_ind = %01d \n",(ucData&0x04)>>2));   //0x1f1[2]
#endif    
    //d20120419_hongji:m_index update
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_16,&ucData, 1);    	
    mcSHOW_DBG_MSG(("m_index = %04d /100 \n", ucData*100/128));

    // get average awgn/map_awgn and ratio/map_ratio
    for(jj = 1; jj <= MEASURE_SAMPLE_CNT; jj++)
    {        
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_22,&ucData, 1); // pn_indicator
        ucPn_indicator = ucData;
		if(ucPn_indicator>ucMaxPNInd)//get the maximum
		{
		   ucMaxPNInd=ucPn_indicator;
		}
        u4Avg_pn_indicator += ucData;
		ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0,&ucData, 1); // cvbs noise
        u4Avg_CVBS_noise += ucData;
#if USE_NCR_AWGN
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_21,&ucData, 1); //awgn
        ucAWGN = ucData;
        u4Avg_awgn += ucData;
#else
        ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0,&ucData, 1); // cvbs noise
		ucCVBS_noise = ucData;
        //"awgn" mapping "CVBS noise"
        if(CVBS_NOISE_LOWER_BOUND >= ucCVBS_noise) //good signal
        {
            u4Mapping_awgn = MIN_MAPPED_AWGN;
        }
        else if (CVBS_NOISE_UPPER_BOUND < ucCVBS_noise)
        { 
            u4Mapping_awgn = MAX_MAPPED_AWGN;
        }
        else 
        {
            u4Mapping_awgn = SCAL*ucCVBS_noise + OFFSET;
        }   
        
        //Round AWGN to the nearest integer
        if(u4Mapping_awgn%100000 > 5)
        {
            ucAWGN = u4Mapping_awgn/100000 + 1;
        }
        else
        {
            ucAWGN = u4Mapping_awgn/100000;
        }
        u4Avg_awgn += ucAWGN;
#endif
        if(0x00 == ucAWGN)
        {
            ucAWGN++;
        }          
		ucAWGNArr[jj-1]=ucAWGN;
        //check if pn_indicator & awgn is valid
        if (INDEX_PROTECT  < ucPn_indicator || INDEX_PROTECT  < ucAWGN)
        {
            fgBW_Protection = TRUE;
            mcSHOW_DBG_MSG2(("invalid pn_indicator: %d   or awgn: %d\n", ucPn_indicator, ucAWGN));
            break;
        }
        else
        {
            fgBW_Protection = FALSE;
        }
        u4Awgn_pn_ratio =  ucPn_indicator*10 / ucAWGN; // awgn_pn_ratio
        u4Avg_ratio += u4Awgn_pn_ratio;
    }
    mcSHOW_DBG_MSG(("ucMaxPn_indicator=0x%02X\n",ucMaxPNInd));
    // don't update BW and LCN setting with invaliad pn_indicator of awgn
    if(fgBW_Protection)
    {
        mcSHOW_DBG_MSG2(("contain before status!!!\n"));        
    }
    else // do  update BW and LCN setting with valid pn_indicator of awgn
    {        
        for(jj=0;jj<MEASURE_SAMPLE_CNT;jj++)
		{
		   u4MaxAvg_ratio+=(ucMaxPNInd*10)/ucAWGNArr[jj];
		}
		u4MaxAvg_ratio/=MEASURE_SAMPLE_CNT;
		u4Avg_awgn = u4Avg_awgn*10 / MEASURE_SAMPLE_CNT;
		u4Avg_pn_indicator = u4Avg_pn_indicator/MEASURE_SAMPLE_CNT;                         
	    u4Avg_ratio = u4Avg_ratio / MEASURE_SAMPLE_CNT;
		u4Avg_CVBS_noise = u4Avg_CVBS_noise*10 / MEASURE_SAMPLE_CNT;

        mcSHOW_DBG_MSG3(("avg_pn_indicator = %d, avg_awgn(Kp) = %d/10, avg_cvbs_noise(LCN) = %d/10, avg_ratio = %d/10, u4MaxAvg_ratio = %d/10 \n",u4Avg_pn_indicator,u4Avg_awgn,u4Avg_CVBS_noise,u4Avg_ratio,u4MaxAvg_ratio));    
        //enable ENABLE "LCN CONFIGRUATION" for weak signal?
        //if(LCN_LOWER*10 <= u4Avg_awgn) //weak signal?
		if(u4Avg_CVBS_noise > cCR_PATCH_NOISE_THRHLD_HIGH * 10) //d201401218_wenming: Align with LGE A2 method, also to avoid avg_awgn over mapping the CVBS noise at strong signal
        {
            //if (LCN_UPPER*10 <= u4Avg_awgn) // weak signal!!
            {

#if ATD_SUPPORT_OVM_SMASH
                if(IC_VER_5398_AA!=BSP_GetIcVersion()) //d2012_hongji:MT5398ES Version don't support this function
                {
                    // enable "dis_smash" to overcome overcome weak + OVM
                    ATD_GetReg(psDemodCtx,REG_ATD_NCR_ADV_28,&ucData, 1);
                    ucData = 0xFB & ucData; //0x2DF[2]=0
                    ATD_SetReg(psDemodCtx,REG_ATD_NCR_ADV_28, &ucData,1);
                }
#endif                
                // set pd_switch_thd for weak signal
                ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_6,&ucData, 1);
                u2DRE = ucData * 16;
                mcSHOW_DBG_MSG2(("DRE : 0x%x\n", u2DRE));
                // set pd_switch_phd = 0x05
                ucData = 0x05;
                ATD_SetReg(psDemodCtx,REG_ATD_NCR_ADV_3, &ucData,1);
                mcSHOW_DBG_MSG2(("Low CN Setting:Set pd_switch_threshold to 0x%x \n",ucData));                                

                fgLCN_STATUS = TRUE;        
            }
    
        }
		else if(fgLCN_STATUS && (u4Avg_CVBS_noise < cCR_PATCH_NOISE_THRHLD_LOW * 10))
        //else if( fgLCN_STATUS && (LCN_LOWER*10 > u4Avg_awgn )) // from weak to normal, reset pd_switch_thd setting
        {
            fgLCN_STATUS = FALSE;  // change CN status
            fgNCN_FLAG = TRUE;     
        }
    
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12,&ucData, 1); //kp
        ucKP = ucData >> 4;
        mcSHOW_DBG_MSG2(("Last Kp = 0x%x\n", ucKP));
        if(TRUE == fg_Enable_CR_BW_Auto)
        {            
            mcSHOW_DBG_MSG2(("Enter Negative Modulation NCR BW SW auto config.\n"));
            if((ucKP < BW_LOWER_BOUND) || (ucKP > BW_UPPER_BOUND))  //d20120221_hongji:Avoid other function modify it.Initial it if beyond range(0x03~0x09)
            {
                mcSHOW_DBG_MSG3(("Current Kp value is beyond range(0x03 to 0x09),need do Kp initial setting \n"));
                if(MOD_PAL_BG == (psDemodCtx->u1SubSysIdx))
                {
                    ucKP = 0x09;
                }
                else
                {
                    ucKP = 0x07;
                }
            }
            
			if((RATIO_UPPER_BOUND < u4MaxAvg_ratio) && (BW_LOWER_BOUND < ucKP)) // need to up CR bandwidth?
            {
                mcSHOW_DBG_MSG2(("Down kp ==> \n"));
                ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12,&ucData, 1);
            
                ucKP--; // up bandwidth
                mcSHOW_DBG_MSG2(("Current Kp = 0x%x \n", ucKP));
                ucData = (ucKP << 4) | (ucData & 0x0F);        
                ATD_SetReg(psDemodCtx,REG_ATD_NCR_FSM_12, &ucData,1);
            }
            else if((RATIO_LOWER_BOUND > u4MaxAvg_ratio) && (BW_UPPER_BOUND > ucKP)) 
            {
			    su1KpUpCnt++;
				if(su1KpUpCnt<cKP_UP_CHECK_CNT)
				{
					mcSHOW_DBG_MSG(("Prepare to up kp,wait...\n"));
					return;
				}
                mcSHOW_DBG_MSG2(("Up kp ==> \n"));
                ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12,&ucData, 1);
            
                ucKP++; // down bandwidth
                mcSHOW_DBG_MSG2(("Current Kp = 0x%x \n", ucKP));
                ucData = (ucKP << 4) | (ucData & 0x0F);                    
                ATD_SetReg(psDemodCtx,REG_ATD_NCR_FSM_12, &ucData,1);            
            }
            else 
            {   
                mcSHOW_DBG_MSG2(("KP stable,Keep last value !!!\n"));
            }
			su1KpUpCnt=0;
#if defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
            ATD_GetReg(psDemodCtx,REG_ATD_NCR_PLL_4,&ucData,1);
            mcSHOW_DBG_MSG2(("current 0x2a8 = 0x%X\n", ucData));
            if((ucKP < 0x09) && (ucKP > 0x04))  //d20120616_hongji: adjust jump weighting when Kp=[5,8]
            {
                if(!fg_jump_weighting_status)
                {
                    ucData |= 0x04; 
                    ucData &= 0xFD;  //d20120616_hongji:reset jump weighting to *8
                    ATD_SetReg(psDemodCtx,REG_ATD_NCR_PLL_4,&ucData,1);
                    mcSHOW_DBG_MSG2(("0x2a8 = 0x%X\n", ucData));
                    fg_jump_weighting_status = TRUE;
                    mcSHOW_DBG_MSG2(("Kp enter range [5,8], Reset jump weighting to *8\n"));
                }
            }
            else
            {
                if(fg_jump_weighting_status)
                {
                    ucData &= 0xF9;  //d20120616_hongji:reset jump weighting to *1
                    ATD_SetReg(psDemodCtx,REG_ATD_NCR_PLL_4,&ucData,1);
                    mcSHOW_DBG_MSG2(("0x2a8 = 0x%X\n", ucData));
                    fg_jump_weighting_status = FALSE;
                    mcSHOW_DBG_MSG2(("Kp exit range [5,8], Reset jump weighting to *1\n"));
                }
            }
#endif            
            
        }
        else
        {
            mcSHOW_DBG_MSG3(("Disable CR BW Auto Config,Keep last value !!!\n"));
        }
    }        
}
#endif
#if ATD_SUPPORT_NCR
void ATD_NCR_PosBW_Patch(ATD_CTX_T *psDemodCtx)
{
    UINT8   ucData, ucKP, ucPn_indicator, jj;
    UINT8   ucCVBS_noise; // SNR from DRO output: 0x242
    UINT32  u4Avg_map_awgn,u4Avg_Pn_indicator,u4Mapping_awgn,u4Mapping_ratio,u4Avg_map_ratio,u4Avg_CVBS_noise;
    BOOL    fgBW_Protection = FALSE;
   
    mcSHOW_DBG_MSG2(("Positive Modulation use CVBS noise mapping AWGN !\n"));

    // get average awgn and ratio of pn_indiactor and awgn 
    u4Avg_map_awgn = 0;
    u4Avg_CVBS_noise = 0;
    u4Avg_Pn_indicator = 0;
    u4Avg_map_ratio = 0;
    for(jj = 1; jj <= MEASURE_SAMPLE_CNT; jj++)
    {
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_22,&ucData, 1); // pn_indicator
        ucPn_indicator = ucData;
        u4Avg_Pn_indicator += ucData;

        //check if pn_indicator & awgn is valid
        if (INDEX_PROTECT < ucPn_indicator)
        {
            fgBW_Protection = TRUE;
            mcSHOW_DBG_MSG2(("invalid pn_indicator: %d \n", ucPn_indicator));
            break;
        }
        else
        {
            fgBW_Protection = FALSE;
        }

        ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0,&ucData, 1); // cvbs noise
        ucCVBS_noise = ucData;
        u4Avg_CVBS_noise += ucData;
        
        //"awgn" mapping "CVBS noise"
        if(CVBS_NOISE_LOWER_BOUND >= ucCVBS_noise) //good signal
        {
            u4Mapping_awgn = MIN_MAPPED_AWGN;
        }
        else if (CVBS_NOISE_UPPER_BOUND < ucCVBS_noise)
        { 
            u4Mapping_awgn = MAX_MAPPED_AWGN;
        }
        else 
        {
            u4Mapping_awgn = SCAL*ucCVBS_noise + OFFSET;
        }       
        u4Avg_map_awgn += u4Mapping_awgn/10000;
        
		u4Mapping_ratio =  (ucPn_indicator*1000000 / u4Mapping_awgn); //mapping_ratio
        u4Avg_map_ratio += u4Mapping_ratio;        
    }

    //don't update BW and LCN setting with invaliad pn_indicator of awgn
    if(fgBW_Protection)
    {
        mcSHOW_DBG_MSG2(("contain before status!\n"));        
    }
    else //update BW and LCN setting with valid pn_indicator and awgn
    {
		u4Avg_map_awgn = u4Avg_map_awgn/MEASURE_SAMPLE_CNT;
        u4Avg_CVBS_noise = u4Avg_CVBS_noise/MEASURE_SAMPLE_CNT;
        u4Avg_Pn_indicator = u4Avg_Pn_indicator/MEASURE_SAMPLE_CNT;
		u4Avg_map_ratio = u4Avg_map_ratio/MEASURE_SAMPLE_CNT;        

        //positive use mapped awgn
		mcSHOW_DBG_MSG2(("avg_cvbs_noise = %d,avg_map_awgn = %d/10,avg_pn_indicator = %d,avg_map_ratio = %d/10 \n",u4Avg_CVBS_noise,u4Avg_map_awgn,u4Avg_Pn_indicator,u4Avg_map_ratio));
 
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12,&ucData, 1); //kp
        ucKP = ucData >> 4;        
        mcSHOW_DBG_MSG3(("Last Kp = 0x%x\n", ucKP));        
        if(TRUE == fg_Enable_CR_BW_Auto)
        {        
            mcSHOW_DBG_MSG2(("Enter Positive Modulation NCR BW SW auto config.\n"));
            if((RATIO_UPPER_BOUND < u4Avg_map_ratio) && (BW_LOWER_BOUND < ucKP)) // need to up CR bandwidth?
            {
                mcSHOW_DBG_MSG2(("Down Kp ==> \n"));
                ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12,&ucData, 1);
            
                ucKP--; //up bandwidth            
                mcSHOW_DBG_MSG2(("Current Kp = 0x%x \n", ucKP));
                ucData = (ucKP << 4) | (ucData & 0x0F);                    
                ATD_SetReg(psDemodCtx,REG_ATD_NCR_FSM_12, &ucData,1);           
            }
            else if((RATIO_LOWER_BOUND > u4Avg_map_ratio ) && (BW_UPPER_BOUND > ucKP)) 
            {
                mcSHOW_DBG_MSG2(("Up Kp ==> \n"));
                ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12,&ucData, 1);
            
                ucKP++; //down bandwidth
                mcSHOW_DBG_MSG2(("Current Kp = 0x%x \n", ucKP));
                ucData = (ucKP << 4) | (ucData & 0x0F);        
                ATD_SetReg(psDemodCtx,REG_ATD_NCR_FSM_12, &ucData,1);            
            }
            else 
            {    
                mcSHOW_DBG_MSG2(("KP stable,Keep last value !!!\n"));
            }
        }
        else
        {
            mcSHOW_DBG_MSG3(("Disable CR BW Auto Config,Keep last value !!!\n"));
        }
    }
}
#endif
#if ATD_SUPPORT_NCR
STATIC void ATD_NCR_Patch(ATD_CTX_T *psDemodCtx)
{
    UINT8 u1Data;
#if defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    UINT8 u1Data1;
#endif
#if !ATD_SUPPORT_NCR_FN_IND
    UINT8 u1CurrentKp,u1UperKp = 5, u1LowerKp = 8;
#endif
    if(fg_Enable_BN) //Enable NCR_BN Patch
    {
        mcSHOW_DBG_MSG2(("Enter NCR BN Patch.\n"));
        ATD_NCR_BN_Patch(psDemodCtx);        
    }
    else
    {
        mcSHOW_DBG_MSG2(("Disable NCR BN Patch.\n"));
    }

#if !ATD_SUPPORT_NCR_FN_IND  //d20120227_hongji:Use OCR FN indicator as FN indicator when NCR don't support FN indicator
    ATD_GetReg(psDemodCtx,REG_ATD_NCR_FSM_12, &u1CurrentKp, 1);
    u1CurrentKp = u1CurrentKp >> 4;
    if(u1UperKp >= u1CurrentKp)// adjust ki = E
    {
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &u1Data, 1);
        u1Data &= 0x0F;
        u1Data |= 0xE0;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &u1Data, 1);
        mcSHOW_DBG_MSG(("FN case:Adjust Ki co-efficient to 0x%x \n",u1Data));
    }
    else if (u1LowerKp <= u1CurrentKp)// adjust ki = F
    {
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &u1Data, 1);
        u1Data &= 0x0F;
        u1Data |= 0xF0;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &u1Data, 1);
        mcSHOW_DBG_MSG(("Not FN case:Adjust Ki co-efficient to 0x%x \n",u1Data));
    }
#else  //d20121101_hongji:Use NCR fn indicator
        //d20121101_hongji:Access pll_fn_level for pll ki adjustment and show total_fn_level   
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN1,&u1Data, 1);
        u1Data |= 0x0F;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_FN1,&u1Data, 1);  //d20121101_hongji:99KHz step size
        mcDELAY_MS(10);   //d20121124_hongji:10ms delay for FN indicator stable
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN2,&u1Data1, 1);
        mcSHOW_DBG_MSG(("Current Total  FN level is: %dKHz to %dKHz\n",((u1Data1>>4)&0x07)*((u1Data&0x0F)*66)/20,(((u1Data1>>4)&0x07)+1)*((u1Data&0x0F)*66)/20));
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN1,&u1Data, 1);
        u1Data &= 0xF0;
        u1Data |= 0x02;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_FN1,&u1Data, 1);  //d20121101_hongji:13.2KHz step size
        mcDELAY_MS(10);  //d20121124_hongji:10ms delay for FN indicator stable
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN2,&u1Data1, 1);
        mcSHOW_DBG_MSG(("Current PLL  FN level is: %dKHz to %dKHz\n",(u1Data1&0x07)*((u1Data&0x0F)*66)/20,((u1Data1&0x07)+1)*((u1Data&0x0F)*66)/20));
#endif    
    
#if !ATD_SUPPORT_HW_AUTO_BW  //d20120114_hongji:SW Auto config patch for MT5398 ES Version    
    if((MOD_SECAM_L == psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx)) // Positive modulation
    {
        ATD_NCR_PosBW_Patch(psDemodCtx);                                      
    }
    else  //Negative modulation
    {
        ATD_NCR_NegBW_Patch(psDemodCtx);                                      
    } 
#else
    if(TRUE==IS_IC_5880() || (FALSE==fg_CR_BW_Ctrl_Switch))  //d20120417_hongji:MT580 don't support CR BW auto config
    {
        if((MOD_SECAM_L == psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx)) // Positive modulation
        {
            ATD_NCR_PosBW_Patch(psDemodCtx);                                      
        }
        else  //Negative modulation
        {
            ATD_NCR_NegBW_Patch(psDemodCtx);                                      
        } 
    }
    else  //d20120417_hongji:MT5880 Later version will support HW BW Auto control function
    {
        //d20120511_hongji:enable NCR HW BW auto control
        if(FALSE==fg_cci_disable_hbw)
        {
            ATD_GetReg(psDemodCtx,REG_ATD_NCR_HBW_0,&u1Data,1);
            u1Data |= 0x80;
            ATD_SetReg(psDemodCtx,REG_ATD_NCR_HBW_0,&u1Data,1);  
            mcSHOW_DBG_MSG(("CR Patch Enable CR BW HW Auto config \n"));
        }
        //Show releated log message for NCR auto control flow
        //1.lcn status:0x2e1[3]
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_HBW_1,&u1Data,1);
        mcSHOW_DBG_MSG(("lcn status = 0x%x \n", ((u1Data>>3)&0x01)));
        //2.ncn status:0x2e0[5]
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_HBW_0,&u1Data,1);
        mcSHOW_DBG_MSG(("ncn status = 0x%x \n", ((u1Data>>5)&0x01)));
        //3.DRE
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_ADV_6,&u1Data,1);
        mcSHOW_DBG_MSG(("DRE = 0x%x \n", (u1Data*16)));
        //4.pd_switch_threshold:0x2c1
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_ADV_3,&u1Data,1);
        mcSHOW_DBG_MSG(("pd_switch_threshold = 0x%x \n", u1Data));
        //5.avg awgn:0x2e7
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_HBW_7,&u1Data,1);
        u1Data /= 2;
        mcSHOW_DBG_MSG(("avg awgn = 0x%x \n", u1Data));
        //6.pn_indicator:
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_ADV_22,&u1Data,1);
        mcSHOW_DBG_MSG(("Current pn_indicator = 0x%x \n", u1Data));
        //7.avg ratio:0x2e8
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_HBW_8,&u1Data,1);
        u1Data = (u1Data*10)/4;
        mcSHOW_DBG_MSG(("avg ratio = %d/10 \n", u1Data));
        //8.kp:0x2b4
        ATD_GetReg(psDemodCtx,REG_ATD_NCR_FSM_12,&u1Data,1);
        u1Data = ((u1Data&0xF0)>>4);
        mcSHOW_DBG_MSG(("Current Kp = 0x%x \n", u1Data));
        if((psDemodCtx->u1SubSysIdx!=MOD_SECAM_L)&&(psDemodCtx->u1SubSysIdx!=MOD_SECAM_L1))
        {
            //8.m_index:0x2d3
            ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_16,&u1Data, 1);    	
            mcSHOW_DBG_MSG(("m_index = %04d /100 \n", u1Data*100/128));
        }
    }   
#endif      
    return;
}
#endif

STATIC void ATD_Patch_CR(ATD_CTX_T *psDemodCtx)
{
#if ATD_SUPPORT_NCR  //d20120114_hongji:NCR Patch
    if (fg_NCR_Sel == FALSE)    // Old CR
	{
	    ATD_OCR_Patch(psDemodCtx);
	}
	else  // NCR patch
    {
        ATD_NCR_Patch(psDemodCtx);
    }
#else  //OCR Patch
    ATD_OCR_Patch(psDemodCtx);
#endif
    return;
}

STATIC void ATD_Patch_StrongChroma(ATD_CTX_T *psDemodCtx)
{       
    UINT32 u4Temp;
    UINT16 u2ChromaGain;
     
    u4Temp = mcRISC_Reg(cRISC_TVD_BASE, 0x084);
    u2ChromaGain =(UINT16)((u4Temp & 0xFFF00000) >> 20);
    mcSHOW_DBG_MSG2(("TVD chroma gain = 0x%04X\n", u2ChromaGain));
    //only patch PAL-BG
    if(psDemodCtx->u1SubSysIdx != MOD_PAL_BG)
    {
        return;
    }
    if((u2ChromaGain <= 0x0030) && (0 == Strong_Chroma_Indicator))
    {
        ATD_SetEQ(psDemodCtx, itGetEqScriptStrongChroma);
        Strong_Chroma_Indicator = 1;
        mcSHOW_DBG_MSG(("Enter strong chroma mode!\n"));
    }
    else if((u2ChromaGain >= 0x00A0) && (1 == Strong_Chroma_Indicator)) //0x7A>0xA0 for India FT issue
    {
        ATD_SetEQ(psDemodCtx, itGetEqScriptNormal);
        Strong_Chroma_Indicator = 0;
        mcSHOW_DBG_MSG(("Leave strong chroma mode!\n"));
    }    
    
    if(Strong_Chroma_Indicator)
    {
        mcSHOW_DBG_MSG2(("In strong chroma mode!\n"));
    }
    else
    {
    	mcSHOW_DBG_MSG2(("In normal chroma mode!\n"));
    }	
        
}

#if ATD_SUPPORT_AGCSPEED   //d20120114_hongji:Support AGC Speed function
void ATD_Patch_DRO_AGC(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucNoiselevel,u1GainJitter,ucData;
    static UINT8 u1IsAGCMode=2;
    ucData = 0;
    u1GainJitter = 0;
    //Get  CVBS SNR
    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucData, 1);   
    ucNoiselevel = ucData;
    
    mcSHOW_DBG_MSG(("SNR=0x%02X\n",ucNoiselevel));
    
    if(ucNoiselevel>0x06)//CVBS SNR >0x06
    {
        u1IsAGCMode=0;//normal-mode
    }
    else if(ucNoiselevel<=0x05)
    {
        //Get gain jitter
        ucData = 0x2E;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1); // DRO probe sel: gain_jitter    
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &u1GainJitter, 1);
        mcSHOW_DBG_MSG(("Gain jitter=0x%02X\n",u1GainJitter));
        if(u1GainJitter>0x0a)
        {
            u1IsAGCMode=1;           
        }
        if(u1GainJitter<0x08)
        {
            u1IsAGCMode=0;       
        }
    }
    if ((MOD_SECAM_L==psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1==psDemodCtx->u1SubSysIdx))
    {
        u1IsAGCMode=0;
        mcSHOW_DBG_MSG(("The TV system is SECAM.\n"));
		return;
    }
    mcSHOW_DBG_MSG(("Is AGC mode?%d\n",u1IsAGCMode));
    
    if(IC_VER_5398_AA==BSP_GetIcVersion())  //d20120417_hongji:MT5398ES version don't support AGC Speed
    {
        if(1 == u1IsAGCMode)//AGC mode;disable LP3
        { 
            ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);           
            ucData &= 0xFE;    //Disable LP3
            ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);           
        }   
        if(0 == u1IsAGCMode)//normal mode; enable LP3
        {
            ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);           
            ucData |= 0x01;    //Enable LP3
            ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);
        }
    }
    else
    {
    if(u1IsAGCMode==1)//AGC mode;Enable Time-Tracking,,wadc time = 0,w_adc = 2^-10
    { 
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);   //Enable Time-Tracking      
        ucData |= 0x40;    
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);      //wadc time = 0
        ucData &=0x80;
        ucData |=0x00;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);    //w_adc = 2^-10
        ucData &= 0xF8;   
        ucData |= 0x02;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);        
    }
    
    if(u1IsAGCMode==0)//normal mode; Disable Time-Tracking,wadc time = 2,w_adc = 2^-14
    {
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);   //Disable Time-Tracking     
        ucData &= 0xBF;    
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
        
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);      //wadc time = 2
        ucData &=0x80;
        ucData |=0x02;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);    //w_adc = 2^-14
        ucData &= 0xF8;   
        ucData |= 0x04;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);
        }
    }
    return;
}

#else
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)
void ATD_Patch_DRO_AGC(ATD_CTX_T *psDemodCtx)
{
	UINT8 ucNoiselevel,u1GainJitter,ucData;
	UINT8 u1IsAGCMode=2;
	ucData = 0;
	u1GainJitter = 0;
	//Get  CVBS SNR
	ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucData, 1);	  
	ucNoiselevel = ucData;
	mcSHOW_DBG_MSG(("SNR=0x%02X\n",ucNoiselevel));
	if(ucNoiselevel>0x06)//CVBS SNR >0x06
	{
		u1IsAGCMode=0;//normal-mode
	}
	else if(ucNoiselevel<=0x05)
	{
        //Get gain jitter
        ucData = 0x2E;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1); // DRO probe sel: gain_jitter	 
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &u1GainJitter, 1);
        mcSHOW_DBG_MSG(("Gain jitter=0x%02X\n",u1GainJitter));
        if(u1GainJitter>0x0a)
        {
            u1IsAGCMode=1;			  
        }
        if(u1GainJitter<0x08)
        {
            u1IsAGCMode=0;		  
        }
	}
	if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))
    {
        u1IsAGCMode=0;
		mcSHOW_DBG_MSG(("The TV system is SECAM.\n"));
	}
	mcSHOW_DBG_MSG(("Is AGC mode?%d\n",u1IsAGCMode));	
	if(1 == u1IsAGCMode)//AGC mode;disable LP3
	{ 
	    ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);			
		ucData &= 0xFE;    //Disable LP3
		ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);			
	}	
	if(0 == u1IsAGCMode)//normal mode; enable LP3
	{
		ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);			
		ucData |= 0x01;    //Enable LP3
		ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4, &ucData, 1);
	}
}

#elif defined(CC_MT5365) || defined(CC_MT5395)
STATIC UINT8 fgAsyncMode;
STATIC UINT8 fgSyncmode;
void ATD_Patch_DRO_AGC(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucNoiselevel,u1GainJitter,ucData;
    UINT8 u1IssyncMode=2;
//Get  CVBS SNR
    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucData, 1);    
    ucNoiselevel = ucData;
	
	mcSHOW_DBG_MSG(("SNR=0x%02X\n",ucNoiselevel));
	
	if(ucNoiselevel>0x06)//CVBS SNR >0x06
	{
	    u1IssyncMode=0;//Async-mode
	}
	else if(ucNoiselevel<=0x05)
	{
        //Get gain jitter
        ucData = 0x2E;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1); // DRO probe sel: gain_jitter    
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &u1GainJitter, 1);
        mcSHOW_DBG_MSG(("Gain jitter=0x%02X\n",u1GainJitter));
        if(u1GainJitter>0x0a)
        {
            u1IssyncMode=1;      		  
        }
        if(u1GainJitter<0x08)
        {
            u1IssyncMode=0;  		  
        }
	}
	mcSHOW_DBG_MSG(("Is Sync mode?%d\n",u1IssyncMode));
	
	if(1 == u1IssyncMode)
	{
	    ucData=0x19;
		ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);
		ucData=0x20;
		ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG05, &ucData, 1);//FIFO delay
		ucData=0xc0;
		ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1);	
		if(0 == fgSyncmode)
        {
            ucData = 0xEF;
            ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
            ucData = 0xFF;
            ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
            fgSyncmode = 1;
        }
        fgAsyncMode=0;
	}
    if(0 == u1IssyncMode)//async mode
    {
        ucData=0x14;
		ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);
		ucData=0x87;
		ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG05, &ucData, 1);//FIFO delay 	
		if(0 == fgAsyncMode)
        {
            ucData = 0xEF;
            ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
            ucData = 0xFF;
            ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
            fgAsyncMode = 1;
        }
            fgSyncmode=0;
    }
}
#else
void ATD_Patch_DRO_AGC(ATD_CTX_T *psDemodCtx)
{	
}
#endif

#endif
VOID ATD_DroTargetLevelModifySyncH(ATD_CTX_T *psDemodCtx, BOOL fgSel, UINT8 ucACC_MonCntSel, BOOL fgRegAuto, UINT32 u4TarSyncH, UINT16 u2DroTarLvlInitial)
{
    if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))
    {
        //only for the SECAM L
        UINT8 ucData1, ucData2, ucWGain, ucWGainTmp;
        UINT16 u2Fref2, u2SyncH_Wagc, u2DroTargetLevel, u2Counter, u2CounterMax, u2DroRatio;
        INT16 i2DroTargetLevel, i2DroWadc, i2TarBlk, i2ACC_MON, i2SyncH_Wagc; 
        INT32 i4WGainInver, i4BlkWagc, i4SyncWagc, i4SyncH_WagcTmp,i4ACC_MONTmp; 
        UINT32 u4CalHeigh, u4DroTargetLevelTmp, u4SyncH_Wagc, u4DroTarLevNxt, u4WGainTmp1, u4WGainTmp2;
        UINT16 u2DroTagetLvlMax, u2DroTagetLvlMin;
        UINT16 u2DRO_CVBS_UP_BUD_NEXT, u2DRO_CVBS_UP_BUD_NEXT_TMP, u2DroTargetLevelTMP , u2DroTargetLevelTMP2;
        u2DroTagetLvlMax = u2DroTarLvlInitial;
        u2DroTagetLvlMin = u2DroTarLvlInitial;
        ucData1 = 0;

        mcSHOW_DBG_MSG(("The System is  L/L1\n"));

        if (u2DroTarLvlInitial >= 0x4000)
        {
            mcSHOW_DBG_MSG(("The DRO target level is negative!!\n"));
            return ;
        }

        if (u2DroTagetLvlMax <= 0x03FF)
        {
            u2DroTagetLvlMax = (u2DroTagetLvlMax << 3);
        }
        else
        {
            u2DroTagetLvlMax = 0x3FFF;
        }

        if (u2DroTagetLvlMin >= 0x0008)
        {
        	u2DroTagetLvlMin = (u2DroTagetLvlMin >> 3);
        }
        else
        {
        	u2DroTagetLvlMin = 0x0001;
        }
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_0, &ucData1, 1);
        ucData1 &= 0x80;
        if (0x00 == ucData1)
        {
            mcSHOW_DBG_MSG(("Scaling bypass==>no adjust\n"));
        	ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);
            ucData1 &= 0x80;
        	ucData2 = (UINT8)((u2DroTarLvlInitial >> 8) & 0x7F);
        	ucData1 = ucData1 | ucData2;
        	ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);
        	ucData1 = (UINT8)(u2DroTarLvlInitial & 0xFF);		
            ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData1, 1); 
            return ;
        }
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_ASYNC_CFG0, &ucData1, 1);
        ucData1 &= 0xE0;
        if (ucData1 != 0xC0)
        {
            mcSHOW_DBG_MSG(("The Ferf2 is not sync Low==>no adjust\n"));
            return ;
        }
        //Step 0: initialize the parameters	
        //u2CalHeigh (U(11,-3), according to the DRO target level)
        //u4CalHeigh = 3142; //Vpp = 1.9
        //u4CalHeigh = 3407; //Vpp = 1.97
        //u4CalHeigh = 1653; //Vpp = 1.09
        u4CalHeigh = u4TarSyncH;
        // step 1 Get the parameters
        // DRO target Level
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData2, 1);
        ucData1 &= 0x7F;
        if (ucData1 >= 0x40) //	value < 0 (S(+10,_4))
        {
            i2DroTargetLevel = (ucData1*256 + ucData2) - 32768;
        }
        else
        {
            i2DroTargetLevel = (ucData1*256 + ucData2);
        }
        // W_ADC
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_1, &ucData1, 1);	
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_2, &ucData2, 1);	
        if (ucData1 >=0x80)    //	value < 0 (S(+7,-8))
        {
            i2DroWadc = (ucData1*256 + ucData2) - 65536;
        }
        else
        {
            i2DroWadc = (ucData1*256 + ucData2);
        }
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData1, 1);
        ucData1 &=0xF0;
        ucData1 |=0x01;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData1, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData1, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData2, 1);
        u2Fref2 = ucData1*256 + ucData2;
        mcSHOW_DBG_MSG2(("Fref2	= 0x%04X\n", u2Fref2));

        //Target Blank
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_3, &ucData1, 1);
        if (ucData1 >0x80) //value < 0 (S(+11,+4))
        {
            i2TarBlk = -(256 - ucData1);
        }
        else
        {
            i2TarBlk = ucData1;
        }

        //W_Gain (S(1,-4)) or U(1,-4)
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_0, &ucWGain, 1);
        ucWGain &=0x3F;
#if ATD_SUPPORT_ENLARGED_SCALING_GAIN
        if ((0 == ucWGain))
        {
            mcSHOW_DBG_MSG2(("Error I2CGain"));
            return;
        }
#else
        if ((0 == ucWGain)||(0x37 == ucWGain))
        {
            mcSHOW_DBG_MSG2(("Error I2CGain"));
            return;
        }
#endif
        //Step 2. To find the Target_blank(S(11,4))/WGain(S,1,-4), output(S(11,-3))
#if ATD_SUPPORT_ENLARGED_SCALING_GAIN
        ucWGainTmp = ucWGain;
#else
        if (ucWGain >= 0x20) //find the ABS of ucWGain
        {
            ucWGainTmp = 64 - ucWGain;
        }
        else
        {
            ucWGainTmp = ucWGain;
        }
#endif
        if (ucWGainTmp <= 1)
        {
            i4WGainInver= 32767;
        }
        else
        {
            i4WGainInver = 0;
            u4WGainTmp1 = 0x8000;
            u4WGainTmp2 = (UINT32)ucWGainTmp;
            u4WGainTmp2 = (u4WGainTmp2 << 16);
            for(u2Counter=0;u2Counter<16;u2Counter++) 
            {
                if (u4WGainTmp1 >= u4WGainTmp2)
                {
                    u4WGainTmp1 = u4WGainTmp1 - u4WGainTmp2;
            		i4WGainInver = i4WGainInver + 1;
                }
            	i4WGainInver = (i4WGainInver << 1);
            	u4WGainTmp2 = (u4WGainTmp2 >> 1);
            }
        }
#if ATD_SUPPORT_ENLARGED_SCALING_GAIN
        i4WGainInver = i4WGainInver;
#else
        if (ucWGain >= 0x20) 
        {
            i4WGainInver = -i4WGainInver;
        }    
#endif
        //Step 3. Obtain the Clamp_biasS(11,-3)    
        switch (ucACC_MonCntSel)
        {
            case 0:
                u2CounterMax = 1;
        		break;
        	case 1:
                u2CounterMax = 2;
        		break;
        	case 2:
                u2CounterMax = 4;
        		break;
            default:
        		u2CounterMax = 1;
                break;
        }
        i4ACC_MONTmp = 0;

        for (u2Counter=1;u2Counter <= u2CounterMax;u2Counter++)
        {
            ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_10, &ucData1, 1);
            ucData1 &= 0x7F;
            ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_11, &ucData2, 1);    
         	if (ucData1>=0x80)
         	{
                i2ACC_MON = 32768 - (ucData1*256 + ucData2);
        	}
            else
           	{
                i2ACC_MON = (ucData1*256 + ucData2);
            }

        	i4ACC_MONTmp = (INT32)i2ACC_MON + i4ACC_MONTmp;
        	mcDELAY_MS(30);
        }
        i4ACC_MONTmp = (i4ACC_MONTmp >> ucACC_MonCntSel);
        i2ACC_MON = (INT16)(i4ACC_MONTmp & 0xFFFF);
        mcSHOW_DBG_MSG2(("ACC_MON = 0x%04X\n", i2ACC_MON));

        //Step 4. Target_blank/WGain  + Clamp_bias (output = S(11,-3))
        i4BlkWagc = (i2TarBlk*i4WGainInver);
        if (i4BlkWagc < 0)
        {
            i4BlkWagc = (i4BlkWagc >> 4);
        	i4BlkWagc |=0xf0000000;
        }
        else
        {
            i4BlkWagc = (i4BlkWagc >> 4);
        }
        i4BlkWagc = i4BlkWagc + i2ACC_MON;

        //Step 5. Select the data for the SyncWagc (S(18,-3))
        if (fgSel)
        {
            i4SyncWagc = (INT32)i2DroTargetLevel;
        }
        else
        {
            i4SyncWagc = ((INT32)u2Fref2*(INT32)i2DroWadc) + 128;
        	if (i4SyncWagc < 0)
        	{
                i4SyncWagc = (i4SyncWagc >> 8);
        		i4SyncWagc |= 0xFF000000;
        	}
        	else
        	{
                i4SyncWagc = (i4SyncWagc >> 8);
        	}
        }
        //Step 6. Estim the extra gain for the DRO target level 
        //i2SyncH_Wagc S(11,-3)
        //u2SyncH_Wagc U(11,-3)
        //u2DroTargetLevel U(16,-13)
        //u4DroTarLevNxt U(11,-18)
        i4SyncH_WagcTmp = i4BlkWagc - i4SyncWagc;
        i2SyncH_Wagc = i4SyncH_WagcTmp;
        if (i4SyncH_WagcTmp > 32767)
        {
            i2SyncH_Wagc = 32767;
        }
        else if (i4SyncH_WagcTmp < -32767)
        {
            i2SyncH_Wagc = -32767;
        }

        if (i2SyncH_Wagc < 0)
        {
            u2SyncH_Wagc = -i2SyncH_Wagc;
        }
        else
        {
            u2SyncH_Wagc = i2SyncH_Wagc;
        }

        if (i2DroTargetLevel < 0)
        {
            u2DroTargetLevel = 256 + i2DroTargetLevel;
        }
        else
        {
            u2DroTargetLevel = i2DroTargetLevel;
        }
        u4DroTargetLevelTmp = (UINT32)u2DroTargetLevel;	
        u4DroTargetLevelTmp = (u4DroTargetLevelTmp <<15);
        u4SyncH_Wagc = (UINT32) u2SyncH_Wagc;
        u4SyncH_Wagc = (u4SyncH_Wagc <<15);
        u4CalHeigh = (u4CalHeigh << 10);
        u4DroTarLevNxt = 0;
        u2DroRatio = 0;    
        for(u2Counter=1;u2Counter<16;u2Counter++) //(Ratio U(5,-10) )
        {
            if (u4CalHeigh >=u4SyncH_Wagc)
            {
                u4CalHeigh = u4CalHeigh - u4SyncH_Wagc;
            	u4DroTarLevNxt = u4DroTarLevNxt + u4DroTargetLevelTmp;
            	u2DroRatio = u2DroRatio + 1;
            }		    
            u4SyncH_Wagc = (u4SyncH_Wagc >> 1);
            u4DroTargetLevelTmp = (u4DroTargetLevelTmp >> 1);
            u2DroRatio = (u2DroRatio << 1);
        }
        mcSHOW_DBG_MSG2(("ucDroRatio = 0x%04X\n", u2DroRatio));	
        u4DroTarLevNxt = (u4DroTarLevNxt >> 10); //  U(16,-3)	
        if (u4DroTarLevNxt >= 0x3FFF)
        {
            mcSHOW_DBG_MSG2(("DRO_targetEstim larger than the 0x3FF\n"));
        	u4DroTarLevNxt = 0x3FFF;
        }
        //Step 7. Modify the DRO target level    
        if (i2DroTargetLevel < 0)
        {
            i2DroTargetLevel = -(INT16)u4DroTarLevNxt;
        }
        else
        {
            i2DroTargetLevel = (INT16)u4DroTarLevNxt;
        	if (i2DroTargetLevel >= u2DroTagetLvlMax)
        	{
                i2DroTargetLevel = (INT16)u2DroTagetLvlMax;
        		mcSHOW_DBG_MSG(("Upper boundary limitted!!\n"));
        	}
        	else if (i2DroTargetLevel <= u2DroTagetLvlMin)
        	{
                i2DroTargetLevel = (INT16)u2DroTagetLvlMin;
        		mcSHOW_DBG_MSG(("Lower boundary limitted!!\n"));
        	}
        }
        mcSHOW_DBG_MSG2(("The DroTarLevNxt = 0x%04X\n", i2DroTargetLevel));

        u2DRO_CVBS_UP_BUD_NEXT_TMP = cDRO_CVBS_UP_BUD_0_Low;
        u2DRO_CVBS_UP_BUD_NEXT_TMP = (u2DRO_CVBS_UP_BUD_NEXT_TMP<<8);
        u2DRO_CVBS_UP_BUD_NEXT_TMP = u2DRO_CVBS_UP_BUD_NEXT_TMP + 0xFF;
        if (i2DroTargetLevel > u2DroTarLvlInitial)
        {
            u4DroTargetLevelTmp = (UINT32)(u2DroTarLvlInitial + (u2DroTarLvlInitial>>1));
        	if (i2DroTargetLevel > u4DroTargetLevelTmp)
        	{
        	    u2DRO_CVBS_UP_BUD_NEXT = 0x3FFF; 		
        	}
        	else
        	{
        	    u2DroTargetLevelTMP = u2DroTarLvlInitial;
        		u2DroTargetLevelTMP2 = (UINT16)i2DroTargetLevel;
        		u2DRO_CVBS_UP_BUD_NEXT = 0;
        		for(u2Counter=1;u2Counter<16;u2Counter++)
        		{
        		    if (u2DroTargetLevelTMP2 >= u2DroTargetLevelTMP)
        		    {
        		        u2DRO_CVBS_UP_BUD_NEXT += u2DRO_CVBS_UP_BUD_NEXT_TMP;
        				u2DroTargetLevelTMP2 = u2DroTargetLevelTMP2 - u2DroTargetLevelTMP;
        		    }
                    u2DroTargetLevelTMP = (u2DroTargetLevelTMP >> 1);	
        			u2DRO_CVBS_UP_BUD_NEXT_TMP = (u2DRO_CVBS_UP_BUD_NEXT_TMP >>1);
        		}
        	}
        }
        else
        {
            u2DRO_CVBS_UP_BUD_NEXT = u2DRO_CVBS_UP_BUD_NEXT_TMP;
        }

        mcSHOW_DBG_MSG2(("The DRO_CVBS_UP_BUD_NEXT = 0x%04X\n", u2DRO_CVBS_UP_BUD_NEXT));
        if (fgRegAuto)
	    {
		    INT16 ucData11=0,Datadiff=0;
		    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData1, 1);
		    ucData11 = ucData1;
	        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);
	        ucData11 = ucData11 | ((ucData1&0x7f)<<8);
		    if(ucData11>=i2DroTargetLevel) 
		    {
		        Datadiff = (ucData11 - i2DroTargetLevel);
		        if (Datadiff > 10)
			    i2DroTargetLevel = ucData11-10;
		        mcSHOW_DBG_MSG2(("The adjust i2DroTargetLevel = %x\n",i2DroTargetLevel));
			}
	        else
		    {
		        Datadiff = (i2DroTargetLevel - ucData11);
		        if (Datadiff > 10)
		        {
			        i2DroTargetLevel = ucData11+10;
		        }
		            mcSHOW_DBG_MSG2(("The adjust i2DroTargetLevel = %x\n",i2DroTargetLevel));
			}
            mcSHOW_DBG_MSG2(("The Datadiff = %x\n",Datadiff));
            ucData1 &= 0x80;
	        ucData2 = (UINT8)((i2DroTargetLevel >> 8) & 0x7F);
	    	ucData1 = ucData1 | ucData2;
	    	ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);
	    	ucData1 = (UINT8)(i2DroTargetLevel & 0xFF);
	        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData1, 1); 
	    	ATD_GetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_0, &ucData1, 1);
	        ucData1 &= 0x80;
	    	ucData2 = (UINT8)((u2DRO_CVBS_UP_BUD_NEXT >> 8) & 0x7F);
	    	ucData1 = ucData1 | ucData2;
	    	ATD_SetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_0, &ucData1, 1);
	    	ucData1 = (UINT8)(u2DRO_CVBS_UP_BUD_NEXT & 0xFF);
	        ATD_SetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_1, &ucData1, 1); 		
	    }
        return;
    }
    else
    {
        mcSHOW_DBG_MSG2(("The System is not L/L1\n"));
    }
    return;	
}

VOID ATD_DroTargetLevelAdjust(ATD_CTX_T *psDemodCtx, UINT16 u2DroTarLvlInitial)
{
    UINT8 ucData1,u1GainJitter;
	UINT8 ucData2;
    static UINT8 ucCntBN_DroTargetLevelAdjust = 4;
	static BOOL fgBNStatus = 1;    // 1 : BN status, 0:non-BN status
	UINT8 ucBNThreshold = 8;	
	ucData1 = 0;
	u1GainJitter = 0;
	ucData2 = 0;

    if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))
    {    
        if (ATD_GetVIFLock(psDemodCtx, 0) ==cATD_VIF_UNLOCK)
        {
            ATD_GetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_0, &ucData2, 1);
        	if (ucData2 != cDRO_CVBS_UP_BUD_0_Low)
            {
                ATD_GetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_1, &ucData2, 1);
                if (ucData2 != 0xFF)
                {
                    ucData2 = cDRO_CVBS_UP_BUD_0_Low;
                    ATD_SetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_0, &ucData2, 1);
        			ucData2 = 0xFF;
                    ATD_SetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_1, &ucData2, 1);
                }
            }
        }	
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);
        mcSHOW_DBG_MSG(("ATD_DroTargetLevelAdjust:0x166=0x%02X\n",ucData1));
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData1, 1);
        mcSHOW_DBG_MSG(("ATD_DroTargetLevelAdjust:0x167=0x%02X\n",ucData1));
        //Get gain jitter
        ucData2 = 0x2E;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData2, 1); // DRO probe sel: gain_jitter	 
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &u1GainJitter, 1);
        mcSHOW_DBG_MSG(("ATD_DroTargetLevelAdjust:Gain jitter=0x%02X\n",u1GainJitter));

    	ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_4, &ucData2, 1);
    	ucData2 &= 0xC0;
    	mcSHOW_DBG_MSG(("Scalling Clampping indicztor =0x%02X\n",ucData2));
    	ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucData1, 1); 
    	mcSHOW_DBG_MSG(("CVBS SNR =0x%02X\n",ucData1));
    	if ((ucData2 >= 0x80) && (ucData1 <0x06))    //REG_ATD_DRO_SCAL_4 is valid when SNR >=45dB
    	{
    	    if (ucCntBN_DroTargetLevelAdjust < ucBNThreshold)
    	    {
    	        ucCntBN_DroTargetLevelAdjust = ucCntBN_DroTargetLevelAdjust + 2;
    	    }
            else
            {
                fgBNStatus = 1;
    			ucCntBN_DroTargetLevelAdjust = ucBNThreshold;		
            }
    		mcSHOW_DBG_MSG(("BN/AMFV condition=0x%02X\n",fgBNStatus));
    		mcSHOW_DBG_MSG(("BN/AMFV condition counter=0x%02X\n",ucCntBN_DroTargetLevelAdjust));
    	}
    	else
    	{ 	    
    		if (ucCntBN_DroTargetLevelAdjust > 0)
    	    {
    	        ucCntBN_DroTargetLevelAdjust = ucCntBN_DroTargetLevelAdjust - 1;
    	    }
            else
            {
                fgBNStatus = 0;
    			ucCntBN_DroTargetLevelAdjust = 0;
            }
    		mcSHOW_DBG_MSG(("BN condition=0x%02X\n",fgBNStatus));
    		mcSHOW_DBG_MSG(("BN condition counter=0x%02X\n",ucCntBN_DroTargetLevelAdjust));
    	}	
    	if ((u1GainJitter < 0x02))	
    	{	    
    	    if (0 == fgBNStatus)
    	    {
                ATD_DroTargetLevelModifySyncH(psDemodCtx, 1, 1, 1, 3200, u2DroTarLvlInitial);	
        		mcSHOW_DBG_MSG2(("Adjust the DRO Target Level adaptivly\n"));  
    	    }
    		else
    		{
                ucData2 = (UINT8)(u2DroTarLvlInitial & 0xFF);
                ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData2, 1);
		    	ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);
	            ucData1 &= 0x80;
    		    ucData2 = (UINT8)((u2DroTarLvlInitial >> 8) & 0x7F);
		        ucData1 = ucData1 | ucData2;
		        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1);		
	    		mcSHOW_DBG_MSG(("Setting the DRO target level to default!!\n"));  
				ATD_GetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_0, &ucData1, 1);
        	    ucData1 &= 0x80;
		        ucData2 =cDRO_CVBS_UP_BUD_0_Low ;
		        ucData1 = ucData1 | ucData2;
		        ATD_SetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_0, &ucData1, 1);
		        ucData1 = 0xFF;
	            ATD_SetReg(psDemodCtx, REG_ATD_DRO_CVBS_UP_BUD_1, &ucData1, 1); 
    		}
    	}
        return;
    }
    else
    {
    	mcSHOW_DBG_MSG(("DRO Adjust: not SECAM system\n"));
    	return;
    }
	
}
#ifndef CC_MT5880_CODE_SIZE_CTRL
void ATD_Patch_AGC(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;
    S8    s1RFgain, s1IFgain;
    
    ATD_GetReg(psDemodCtx, REG_ATD_PRA_RF_CTL_1, &ucData, 1);
    s1RFgain = (S8)ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_PRA_IF_CTL_1, &ucData, 1);
    s1IFgain = (S8)ucData;
    if (SmallAGC) // if now in SmallAGC mode
    { 
        if (s1IFgain > psDemodCtx->s1IFmin) 
        {
            ucData = 0x00;
            ATD_SetReg(psDemodCtx, REG_ATD_PRA_IF_BIAS, &ucData, 1);

            ucData = psDemodCtx->s1IFmin;
            ATD_SetReg(psDemodCtx, REG_ATD_PRA_IF_MIN, &ucData, 1);        

            ATD_GetReg(psDemodCtx, REG_ATD_PRA_CFG_1, &ucData, 1);
            ucData = ucData | 0x08;
            ATD_SetReg(psDemodCtx, REG_ATD_PRA_CFG_1, &ucData, 1);

            SmallAGC = 0;
            mcSHOW_DBG_MSG(("Leave small AGC mode!\n"));
        }
    }
    else // if now NOT in SmallAGC mode
    {
        if ((s1RFgain <= psDemodCtx->s1RFmin) && (s1IFgain <= psDemodCtx->s1IFmin)) 
        {
            ATD_GetReg(psDemodCtx, REG_ATD_PRA_CFG_1, &ucData, 1);
            ucData = ucData & 0xF7;
            ATD_SetReg(psDemodCtx, REG_ATD_PRA_CFG_1, &ucData, 1);

            ucData = 0xAB;
            ATD_SetReg(psDemodCtx, REG_ATD_PRA_IF_MIN, &ucData, 1);

            ucData = 0x26;
            ATD_SetReg(psDemodCtx, REG_ATD_PRA_IF_BIAS, &ucData, 1);

            SmallAGC = 1;
            mcSHOW_DBG_MSG(("Enter small AGC mode!\n"));
        }
    }    
}
#endif

STATIC void ATD_Patch_SoundBeat(ATD_CTX_T *psDemodCtx)
{

    INT32 s4AudCFO, s4AudOVM;
    UINT8 ucData1,ucData2,ucData3;
    S16 s2Data1;    

    s4AudCFO = AUD_MTS_CFO_IDX();    
    s4AudOVM = AudCmdAtvHdevDetQuery();
   
    mcSHOW_DBG_MSG2(("Audio CFO = %d, audio OVM = %d\n", s4AudCFO, s4AudOVM));
    
    if((999 == s4AudCFO) || (999 == s4AudOVM))
    {
        mcSHOW_DBG_MSG2(("Audio DSP status error!!\n"));
        return;
    }    
    else if((s4AudCFO > 20) || (s4AudCFO<-20))
    {
        mcSHOW_DBG_MSG2(("Audio CFO too large\n"));
        return;
    }
    
    if((1 == s4AudOVM) && (s4AudCFO >= -5) && (s4AudCFO <= 5))  //sound OVM case
    {
        switch(psDemodCtx->u1SubSysIdx)
        {
            case MOD_PAL_BG:
                ucData1=0xE1; 
                ucData2=0xF8; 
                ucData3=0x73;
                break;
            case MOD_PAL_DK:
                ucData1=0xE2; 
                ucData2=0x38; 
                ucData3=0x5A;
                break;
            case MOD_PAL_I:
                ucData1=0xE7; 
                ucData2=0x38; 
                ucData3=0xC4;
                break;    	    
            case MOD_NTSC_M:
                ucData1=0xF4; 
                ucData2=0xF8; 
                ucData3=0x70;
                break;
            default:
                ucData1=0xE1; 
                ucData2=0xF8; 
                ucData3=0x73;
                break;            
        }
        mcSHOW_DBG_MSG2(("Audio OVM mode, fine-tune AVS setting!\n"));
    }    	
    else  //sound no-OVM, need consider sound carrier shift case
    {
        switch(psDemodCtx->u1SubSysIdx)
        {
            case MOD_PAL_BG:
                ucData1=0xE1; 
                ucData2=0xF8; 
                ucData3=0x73;
                break;
            case MOD_PAL_DK:
                ucData1=0xDB; 
                ucData2=0x78; 
                ucData3=0x25;
                break;
            case MOD_PAL_I:
                ucData1=0xD8; 
                ucData2=0x78; 
                ucData3=0x70;
                break;
            case MOD_NTSC_M:
                ucData1=0xF4; 
                ucData2=0xF8; 
                ucData3=0x70;
                break;
            default:
                ucData1=0xE1; 
                ucData2=0xF8; 
                ucData3=0x73;
                break;            
        }
    	    
        if(s4AudCFO > 0)
        {
            if(psDemodCtx->u1SubSysIdx >= MOD_NTSC_M)
            {
            	if(s4AudCFO >= 3)
            	{
            	    ucData1=0xF7; 
                    ucData2=0x38; 
                    ucData3=0x27;
                }
            }
            else
            {                
                s2Data1 = (S16)s4AudCFO/3;
                ucData1 = (UINT8)ucData1 + s2Data1;
                ucData2 = (UINT8)ucData2 - 0x40;        
                ucData3 = (UINT8)ucData3 - (s2Data1*4-1)*38/5;
            }
            mcSHOW_DBG_MSG2(("Audio negative shift, fine-tune AVS setting!\n"));
        }
        else if(s4AudCFO < 0)
        {
            if(psDemodCtx->u1SubSysIdx >= MOD_NTSC_M)
            {
            	if(s4AudCFO <= -3)
            	{
            	    ucData1=0xF1; 
                    ucData2=0x78; 
                    ucData3=0xD4;    
                }            	
            }
            else if(MOD_PAL_DK == psDemodCtx->u1SubSysIdx)
            {
                mcSHOW_DBG_MSG2(("D/K not shift AVS for positive audio CFO!\n"));
            }    
            else
            {
                s4AudCFO = (INT32)(-1) * s4AudCFO;
                s2Data1 = (S16)s4AudCFO/3;        
                ucData1 = (UINT8)ucData1 - s2Data1;
                if(MOD_PAL_BG == psDemodCtx->u1SubSysIdx)
                {
                    ucData1 = ucData1 + 0x01;
                }    
                ucData2 = (UINT8)ucData2 + 0x40;        
                ucData3 = (UINT8)ucData3 + (s2Data1*4-1)*38/5;
            }
            mcSHOW_DBG_MSG2(("Audio positive shift, fine-tune AVS setting!\n"));
        }
        else
        {
                mcSHOW_DBG_MSG2(("Audio no shift, set default AVS setting!\n"));
        }
    }
        
    ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA3, &ucData1, 1);                
    ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucData2, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_VOP_FWCPOIL, &ucData3, 1);   
    return;      
}
#if 0
#if ATD_SUPPORT_AGCSPEED  //d20120302_hongji:Support time tracking mode
STATIC void ATD_Patch_DRO(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucNoiselevel,u1GainJitter,ucData;
    static UINT8 u1IsAGCMode=2;
    ucData = 0;
    u1GainJitter = 0;
    //Get  CVBS SNR
    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucData, 1);   
    ucNoiselevel = ucData;
    
    mcSHOW_DBG_MSG(("SNR=0x%02X\n",ucNoiselevel));
    
    if(ucNoiselevel>0x06)//CVBS SNR >0x06
    {
        u1IsAGCMode=0;//normal-mode
    }
    else if(ucNoiselevel<=0x05)
    {
        //Get gain jitter
        ucData = 0x2E;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1); // DRO probe sel: gain_jitter    
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &u1GainJitter, 1);
        mcSHOW_DBG_MSG(("Gain jitter=0x%02X\n",u1GainJitter));
        if(u1GainJitter>0x0a)
        {
            u1IsAGCMode=1;           
        }
        if(u1GainJitter<0x08)
        {
            u1IsAGCMode=0;       
        }
    }
    
    if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx) || (MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))         
    {
        u1IsAGCMode=0;
        mcSHOW_DBG_MSG(("The TV system is SECAM.\n"));
    }
    mcSHOW_DBG_MSG(("Is AGC mode?%d\n",u1IsAGCMode));
    
    if(u1IsAGCMode==1)//AGC mode;Enable Time-Tracking,,wadc time = 0,w_adc = 2^-10
    { 
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);   //Enable Time-Tracking      
        ucData |= 0x40;    
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);      //wadc time = 0
        ucData &=0x80;
        ucData |=0x00;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);    //w_adc = 2^-10
        ucData &= 0xF8;   
        ucData |= 0x02;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);        
    }
    
    if(u1IsAGCMode==0)//normal mode; Disable Time-Tracking,wadc time = 2,w_adc = 2^-14
    {
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);   //Disable Time-Tracking     
        ucData &= 0xBF;    
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);      //wadc time = 2
        ucData &=0x80;
        ucData |=0x02;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_WADC_5, &ucData, 1);

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);    //w_adc = 2^-14
        ucData &= 0xF8;   
        ucData |= 0x04;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1);
    }
    return;
}
#else  //Don't support time tracking mode
#if ATD_SUPPORT_DRO_SYNC_MODE
STATIC void ATD_Patch_DRO(ATD_CTX_T *psDemodCtx)
{
    UINT8  ucData, ucData1, ucData2, ucIFgain, ucNoiselevel;
    UINT16 u2GainJitter;//, u2SigSwing, u2SyncHeight;
                
    //get IF gain
    ATD_GetReg(psDemodCtx, REG_ATD_PRA_IF_CTL_1, &ucData, 1);    
    ucIFgain = ucData;        	
        
    //get DRO gain jitter
    ucData = 0x2E;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1); // DRO probe sel: gain_jitter    
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData1, 1); 
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData2, 1); 
    u2GainJitter = (UINT16)(ucData1 << 8) | ucData2;        
        
    //get signal swing
    //ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MAX1, &ucData1, 1); 
    //ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MAX2, &ucData2, 1); 
    //u2SigSwing = (UINT16)(ucData1 << 8) | ucData2;        
                                                
    //get sync height                        
    //ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MIN1, &ucData1, 1); 
    //ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MIN2, &ucData2, 1); 
    //u2SyncHeight = (UINT16)(ucData1 << 8) | ucData2;     
    
    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucData, 1);    
    ucNoiselevel = ucData;
    
    switch(psDemodCtx->u1SubSysIdx)
    {
        case MOD_NTSC_M:
            if ( u2GainJitter > 0x1500 )
            {
                ucData = 0x03;  
                ATD_SetReg(psDemodCtx, REG_ATD_PRA_TRA_CTL1, &ucData, 1);  //disable level-switch patch
            }  
            else if ( u2GainJitter < 0x1000 )
            {
                ucData = 0x83;  
                ATD_SetReg(psDemodCtx, REG_ATD_PRA_TRA_CTL1, &ucData, 1);  //enable level-switch patch
            }
                    
            if (ucNoiselevel < 0x04 )  // signal > 45dBuV
            {	                    
                if ( u2GainJitter > 0x2500 )  // larger gain_jitter: AM-Fv case
                {
                    //default 0xDC, GA FT issue 0xAC
                    ucData = 0xAC;  
                    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1); //DRO sync. mode, fast W_ADC                
                    ucData = 0xC0;  
                    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1); //fast ref_0,ref_1 leaky              
                    mcSHOW_DBG_MSG2(("DRO in AM-Fv mode!\n"));
                }
                else if ( u2GainJitter < 0x1400 ) // non AM-Fv case
                {
                    //default 0xD4, GA FT issue 0xA4
    	            ucData = 0xA4;  
                    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1); //DRO sync. mode
                    ucData = 0xD2;  
                    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1);
    	            mcSHOW_DBG_MSG2(("DRO in normal mode!\n"));       	            
                }       					
            }
            else if ( ucNoiselevel > 0x07 ) //signal < 39dBuV
            {
                ucData = 0xD6;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1); //DRO async. mode, slow W_ADC
                ucData = 0xC9;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1);
                mcSHOW_DBG_MSG2(("DRO in weak signal mode!\n"));               					
            }
            else
            {
            
            }    
        	break;
    	case MOD_PAL_BG:
    	case MOD_PAL_DK:
    	case MOD_PAL_I:   		
        	if ( u2GainJitter > 0x1500 )
            {
                ucData = 0x03;  
				ATD_SetReg(psDemodCtx, REG_ATD_PRA_TRA_CTL1, &ucData, 1);  //disable level-switch patch
            }  
            else if ( u2GainJitter < 0x1000 )
            {
                ucData = 0x83;  
				ATD_SetReg(psDemodCtx, REG_ATD_PRA_TRA_CTL1, &ucData, 1);  //enable level-switch patch
            }
        	
        	if( (ucNoiselevel > 0x0D)) // weak signal priority is higher than CCI
            {
        	    ucData = 0xD6;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1); //DRO weak signal mode    
                ucData = 0xED;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1);
        	    ucData = 0xA6;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_CCI_CFG3, &ucData, 1); 
                ucData = 0x2D;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_CCI_CFG4, &ucData, 1);
        	    mcSHOW_DBG_MSG2(("DRO in weak signal mode!\n"));      	    				
        	}  
        	else if (ucNoiselevel < 0x04 )  // signal > 45dBuV
            {	                                    				
                if ( u2GainJitter > 0x2500 )  // larger gain_jitter: AM-Fv case
                {
                    //default 0xDC, GA FT issue 0xAC 
                    ucData = 0xAC;  
					ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1); //DRO sync. mode, fast W_ADC   
                    ucData = 0xC0;  
					ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1); //fast ref_0,ref_1 leaky
                    mcSHOW_DBG_MSG2(("DRO in AM-Fv mode!\n"));                                
                }
                else if ( u2GainJitter < 0x1400 ) // non AM-Fv case
                {
                    //default 0xD4, GA FT issue 0xA4
    	            ucData = 0xA4;  
					ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG01, &ucData, 1); //DRO async. mode, slow W_ADC
                    ucData = 0xD2;  
					ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1);
    	            mcSHOW_DBG_MSG2(("DRO in normal mode!\n"));    	                	            
                }
                ucData = 0xA5;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_CCI_CFG3, &ucData, 1); 
                ucData = 0x12;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_CCI_CFG4, &ucData, 1);
            }                                                       		
        	break;
    	case MOD_SECAM_L:
    	case MOD_SECAM_L1:    		        
            if((ucIFgain >= 0x20) & (ucIFgain <= 0x7F)) // Weak Signal        
            {
        	    ucData = 0x2D;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1);
        	    ucData = 0x2D;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_CCI_CFG4, &ucData, 1);
        	    mcSHOW_DBG_MSG2(("DRO in weak signal mode!\n"));       	    
            }
        	else if((ucIFgain <= 0x10) | (ucIFgain >= 0x80))
            {
        	    ucData = 0x1B;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF, &ucData, 1);
        	    ucData = 0x1B;  
				ATD_SetReg(psDemodCtx, REG_ATD_DRO_CCI_CFG4, &ucData, 1);
        	    mcSHOW_DBG_MSG2(("DRO in normal mode!\n"));    	        	    
        	}    	
        	break;
    }
}
#else//d110927_Haibo:If not support sync mode,can not apply the patch
STATIC void ATD_Patch_DRO(ATD_CTX_T *psDemodCtx)
{

}
#endif
#endif
#endif

/*********************************************************************************
 ATD_Patch_LNA(ATD_CTX_T *) is implemented for  ATD monitor LNA , and it replace 
 the method of reading  tuner register by reading IF AGC and RF AGC to judge signal 
 power.implement by Jack Mei ,2008/10/22
 *********************************************************************************/
#ifdef   LNA_MONITOR   
STATIC void ATD_Patch_LNA(ATD_CTX_T *psDemodCtx)
{
    UINT16 ucDataRf,ucDataIf;
    BOOL Lna;
    UINT8 ucData1,ucData2;
    	
    ucDataRf = ATD_GetRFAGC(psDemodCtx);
    ucDataIf = ATDPI_GetIFAGC(psDemodCtx);
    ATD_GetReg(psDemodCtx, (REG_ATD_PRA_IF_MIN + 0x800), &ucData1, 1);
    ATD_GetReg(psDemodCtx, (REG_ATD_PRA_RF_MAX + 0x800), &ucData2, 1);
    UINT16 IFAgcMIN = (UINT16)(ucData1<<8);
    UINT16 RFAgcMAX = (UINT16)(ucData2<<8);
    
    DigTunerBypassI2C (TRUE); 
    if (( psDemodCtx->u1SubSysIdx >= MOD_PAL_BG )&&(psDemodCtx->u1SubSysIdx <= MOD_PAL_I))
    {
        ITuner_OP(psDemodCtx->pTCtx, itGetLNA, NULL, &Lna);
        if(Lna)
        {
            mcSHOW_DBG_MSG(("PAL:The LNA status is ON!\n "));
        }
        else
        {
            mcSHOW_DBG_MSG(("PAL:The LNA status is OFF!\n "));
        }

        if((ucDataRf == RFAgcMAX)&&((ucDataIf < IFAgcMIN )||(ucDataIf >= 0xe9ff))&& (!Lna)) // make sure Signal level <=50dBv LNA on
        {
            ATDPI_SetLNA(psDemodCtx, TRUE);
        }
        else if((ucDataIf == IFAgcMIN)&&((ucDataRf <= 0x16ff)||(ucDataRf > RFAgcMAX))&&(Lna))  //make sure Signal level > 70dBv LNA off
        {
            ATDPI_SetLNA(psDemodCtx, FALSE);
        }
    }

    if (( psDemodCtx->u1SubSysIdx >= MOD_SECAM_L )&&(psDemodCtx->u1SubSysIdx <= MOD_SECAM_L1))
    {
        ITuner_OP(psDemodCtx->pTCtx, itGetLNA, NULL, &Lna);
        if(Lna)
        {
            mcSHOW_DBG_MSG(("SECAM:The LNA status is ON!\n "));
        }
        else
        {
            mcSHOW_DBG_MSG(("SECAM:The LNA status is OFF!\n "));
        }

        if((ucDataRf == RFAgcMAX)&&((ucDataIf < IFAgcMIN )||(ucDataIf > 0xebff))&& (! Lna))  // Signal level <=50dBv
        {
            ATDPI_SetLNA(psDemodCtx, TRUE);    
        }
        else if((ucDataIf == IFAgcMIN)&&((ucDataRf <= 0x0aff)||(ucDataRf > RFAgcMAX))&& (Lna)) //Signal level > 70dBv
        {
            ATDPI_SetLNA(psDemodCtx, FALSE);
        }
     }
     DigTunerBypassI2C (FALSE);
}

#endif
void ATD_Patch(ATD_CTX_T *psDemodCtx)
{
    UINT8 PatchSwitch = psDemodCtx->pTCtx->u1AtdPatchSwitch;
    Qlab_PrintSelectedIndicator(psDemodCtx);

    if(mcCHK_MASK(PatchSwitch, MASK_PATCH_CR))   ATD_Patch_CR(psDemodCtx);
    if(mcCHK_MASK(PatchSwitch, MASK_PATCH_DRO))   ATD_Patch_DRO_AGC(psDemodCtx);    
    //if(mcCHK_MASK(PatchSwitch, MASK_PATCH_DRO))  ATD_Patch_DRO(psDemodCtx);
    if(mcCHK_MASK(PatchSwitch, MASK_PATCH_CCI))  ATD_Patch_CCI(psDemodCtx);
    if(mcCHK_MASK(PatchSwitch, MASK_PATCH_SENS)) ATD_Patch_Sensitivity(psDemodCtx);
    if(mcCHK_MASK(PatchSwitch, MASK_PATCH_STCHR)) ATD_Patch_StrongChroma(psDemodCtx);
    if(mcCHK_MASK(PatchSwitch, MASK_PATCH_SBEAT)) ATD_Patch_SoundBeat(psDemodCtx);
#if ATD_SUPPORT_NCR
    if(FALSE == fg_NCR_Sel)
    {
        ATD_SetAutoConfig(psDemodCtx, u1CrAutoConfig);
        ATD_SetCrBw(psDemodCtx, u1CrBandwidth);
    }
#else
    ATD_SetAutoConfig(psDemodCtx, u1CrAutoConfig);
    ATD_SetCrBw(psDemodCtx, u1CrBandwidth);
#endif    
    //Add LNA to  monitor process			
#ifdef   LNA_MONITOR
    ATD_Patch_LNA(psDemodCtx);
#endif
    //ATD_Patch_DRO_AGC(psDemodCtx);
    return;
}
    

BOOL Qlab_SetMonIndicator(UINT32 TestItems,UINT32 ucPar1,UINT32 ucPar2)
{      
	if (TestItems > 0)
	{
		Printf("START Monitor Indicator\n");
	}
	else if (0 == TestItems)
	{
		Printf("STOP Monitor Indicator\n");
	}
	
	QLabCondition[0] = ucPar1; // H Sync Loss Threshold : 700
	QLabCondition[1] = ucPar2; // V Sync Loss Threshold : 5
	TestItem = TestItems;

    Printf("Qlab_SetMonIndicator 0x%x %d %d \n",TestItems,QLabCondition[0],QLabCondition[1]);

	return TRUE;
}


void Qlab_PrintSelectedIndicator(ATD_CTX_T *psDemodCtx)
{
	UINT32 CheckItem = TestItem;
	UINT32 MaskItem = 0x1;
	UINT8  Temp = 0;	
	UINT16 Value[7] = {0};

	while (CheckItem > 0)
	{		
		switch(CheckItem & MaskItem)
		{
			case Q_Lab_Sync_Loss:			
				if(0x10 == (IO_READ8(VIDEO_IN0_BASE, 0x81) & 0x10))
                {               
					Printf("##SyncCatch");
                }
				else
                { 
					Printf("##SyncKill");
                }	
				break;
			
			case Q_Lab_Color_Loss:
				if(IO_READ8(0x20022000, 0x9D) & 0x01)
                {               
					Printf("##ColorCatch");
                }
				else
                {               
					Printf("##ColorKill");
                }
				break;

			case Q_Lab_CC:
                
			    //VBIIndicator();
			    
				break;				

			case Q_Lab_Sync_Indicator:
                
				Qlab_Indicator(psDemodCtx,Value);
                
				break;
				
			case Q_Lab_HSync_Loss:
				if(Value[1] < QLabCondition[0])
                {            
					Printf("##HLock");
                }
				else
				{
					Printf("##HShark");
                }
				break;
								
			case Q_Lab_VSync_Loss:
				if(Value[0] < QLabCondition[1])
                {               
					Printf("##VLock");
                }
				else
                {               
					Printf("##VShark");
                }
				break;

			case Q_Lab_HVSync_Loss:
				if((Value[0] < QLabCondition[1]) && (Value[1] < QLabCondition[0]))
                {            
					Printf("##HVLock");
                }
				else	
                {            
					Printf("##HVShark");
                }
    			break;
				
			case Q_Lab_HSync_Toggle:
				if(Value[4] < 1)
                {            
					Printf("##HStable");
                }
				else
                {					
                    Printf("##HToggle");
                }
				break;				
				
			case Q_Lab_VSync_Toggle:
				if(Value[3] < 1)
                {            
					Printf("##VStable");
                }
				else
                {					
                    Printf("##VToggle");
                }
				break;		
                
            default:
                Printf("##Invalid Item");
                break;
		}

		CheckItem &= ~MaskItem;
		MaskItem = (MaskItem << 1)  + 1;
		Temp = 1;
	}

	if(1 == Temp)
	{
		Printf("\n");
	}
}

void Qlab_Indicator(ATD_CTX_T *psDemodCtx,UINT16* Values)
{
	UINT32 RegTemp = 0;
	UINT32 dRegTemp = 0;	
            		
	//1 1.Enable Catch Sel
	RegTemp = IO_READ32(VIDEO_IN0_BASE, 0x5EC);
	RegTemp |= 0x00008000; //Bit 15
	IO_WRITE32(VIDEO_IN0_BASE, 0x5EC, RegTemp);

	//1 2.Select Probe Signal
	RegTemp = IO_READ32(VIDEO_IN0_BASE, 0x61C);
	RegTemp &= 0x0FFFFFFF; //Bit 31 ~28
	RegTemp |= 0xB0000000; //Bit 31 ~28
	IO_WRITE32(VIDEO_IN0_BASE, 0x61C, RegTemp);
	mcDELAY_MS(100);

	//1 3. Get Indicator
	dRegTemp = IO_READ32(VIDEO_IN0_BASE, 0x0A8);
	//mcSHOW_USER_MSG(("Reg 0x%x\n",dRegTemp));
	//mcSHOW_USER_MSG(("V Sync Loss sum during 64 fields %d\n", dRegTemp & 0x3F));
	Values[0] = dRegTemp & 0x3F;
	//mcSHOW_USER_MSG(("H Sync Loss sum during 64 fields %d\n", (dRegTemp >> 6) & 0x3FFF));
	Values[1] = (dRegTemp >> 6) & 0x3FFF;
	//mcSHOW_USER_MSG(("VCR status toggle during 64 fields %d\n", (dRegTemp >> 20) & 0x1));
	Values[2] = (dRegTemp >> 21) & 0x1;
	//mcSHOW_USER_MSG(("VPRES status toggle during 64 fields %d\n", (dRegTemp >> 21) & 0x1));            		
	IO_WRITE32(VIDEO_IN0_BASE, 0x0A8, 0x0);
			
	RegTemp = IO_READ32(VIDEO_IN0_BASE, 0x61C);
	RegTemp &= 0x0FFFFFFF; //Bit 31 ~28			
	RegTemp |= 0xA0000000; //Bit 31 ~28
	IO_WRITE32(VIDEO_IN0_BASE, 0x61C, RegTemp);
	//mcSHOW_USER_MSG(("Setting Reg 0x%x\n",RegTemp));	
			
	dRegTemp = IO_READ32(VIDEO_IN0_BASE, 0x0A8);
	//mcSHOW_USER_MSG(("Reg 0x%x\n",dRegTemp)); 
	//mcSHOW_USER_MSG(("V Lock Status toggle during 64 fields %d\n", dRegTemp & 0x3F));
	Values[3] = dRegTemp & 0x3F;
	//mcSHOW_USER_MSG(("H Lock Status toggle during 64 fields %d\n", (dRegTemp >> 6) & 0x3F));
	Values[4] = (dRegTemp >> 6) & 0x3F;
	//mcSHOW_USER_MSG(("B Lock Status toggle during 64 fields %d\n", (dRegTemp >> 12) & 0x3F));
	Values[5] = (dRegTemp >> 12) & 0x3F;
	
	IO_WRITE32(VIDEO_IN0_BASE, 0x0A8, 0x0);			
}

void ATD_LoadEQCoef(ATD_CTX_T *psDemodCtx, UINT8* pucAtdEQScript)
{
    UINT8  ucData, ii;

    ucData= 0x80;  // Set tap index auto-inc when write
    ATD_SetReg(psDemodCtx, REG_ATD_EQ_CFG, &ucData, 1);
    ucData= 0x00;  // Set tap index to 0
    ATD_SetReg(psDemodCtx, REG_ATD_EQ_WIDX, &ucData, 1);
    for (ii=0; ii<103; ii++)
    {
        ATD_SetReg(psDemodCtx, REG_ATD_EQ_WMSB, (pucAtdEQScript+ii*2), 1);
        ATD_SetReg(psDemodCtx, REG_ATD_EQ_WLSB, (pucAtdEQScript+ii*2+1), 1);
    }
    mcSHOW_DBG_MSG(("Finish EQ coefficient dump!!\n"));
#if ATD_SUPPORT_EQ_3TAP_AUTO
    ucData= 0x83;  //d20120326_hongji:eanble 96tap and 3tap EQ
    ATD_SetReg(psDemodCtx, REG_ATD_EQ_CFG, &ucData, 1);
#else
    ucData= 0x81;  //96tap EQ enable
    ATD_SetReg(psDemodCtx, REG_ATD_EQ_CFG, &ucData, 1);
#endif  
}
void ATD_GetEQ(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData,ucDataArr[2] = {0,0},ii;

    ATD_GetReg(psDemodCtx, REG_ATD_EQ_CFG, &ucData, 1);
    ucData|=0x40;//auto increase weight index for reading weight
    ATD_SetReg(psDemodCtx, REG_ATD_EQ_CFG, &ucData, 1);
    ucData=0x00;//initalize weight index 
    ATD_SetReg(psDemodCtx, REG_ATD_EQ_WIDX, &ucData, 1);

    for (ii=0; ii<103; ii++)
    {
        ATD_GetReg(psDemodCtx, REG_ATD_EQ_WMSB, ucDataArr, 2);
        mcSHOW_USER_MSG(("nim.atd w 0x%03X 0x%02X\n",REG_ATD_EQ_WMSB,ucDataArr[0]));
        mcSHOW_USER_MSG(("nim.atd w 0x%03X 0x%02X\n",REG_ATD_EQ_WLSB,ucDataArr[1]));
    } 
}
void ATD_SetEQ(ATD_CTX_T *psDemodCtx, ITUNEROP_T EqScriptType)
{
    UINT8* pucAtdEQScript = NULL;
#if ATD_SUPPORT_EQ_3TAP_AUTO
    ATD_3TapEQ_REL_REG_T* p3TapEQRelReg = NULL;
#endif
    UINT8 u1EqScriptSysIdx = psDemodCtx->u1SubSysIdx;
    
     //get 3Tap EQ script
#if ATD_SUPPORT_EQ_3TAP_AUTO  //d20120326_hongji:3Tap EQ Setting
     if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx, itGet3TapEQRelReg, psDemodCtx->u1SubSysIdx, &p3TapEQRelReg))
     {
         if(p3TapEQRelReg)
         {
             while(p3TapEQRelReg->u2RegAddr!=0)
             {
                 mcSHOW_DBG_MSG(("RegWrite 0x%04X 0x%02X\n",p3TapEQRelReg->u2RegAddr,p3TapEQRelReg->u1RegVal));
                 ATD_SetReg(psDemodCtx,p3TapEQRelReg->u2RegAddr, &p3TapEQRelReg->u1RegVal,1);
                 p3TapEQRelReg++;
             }
         }
     }
#endif
     // get EQ script
    ITuner_OP(psDemodCtx->pTCtx, EqScriptType, u1EqScriptSysIdx-MOD_ANA_TYPE_BEGIN, &pucAtdEQScript);

    if (pucAtdEQScript != NULL) 
    {
        ATD_LoadEQCoef(psDemodCtx, (pucAtdEQScript));
    }    
    
}
void ATD_SetTVDReChkCfg(ATD_CTX_T *psDemodCtx)
{
    UINT8 u1PnFnSts=0x00;
    UINT8 u1FnBw,coef_a,coef_b;
    UINT8 u1CrAutoCfg=0;
    UINT8 u1Data;

    u1PnFnSts=ATD_GetPnFn(psDemodCtx);
#if ATD_SUPPORT_NCR
    if(TRUE == fg_NCR_Sel)  
    {
	    if(MOD_PAL_BG == (psDemodCtx->u1SubSysIdx))
        {//d20131205_steven:PAL_BG,default kp=9, strong pn will be tvd unlock;so when detect strong pn,set kp=6 in channel scan
         if(0x08 == ((u1PnFnSts)&(MSK_ATD_CR_STATE1_PNIND)))
           {
                ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12, &u1Data, 1);
                u1Data &= 0x0F;
                u1Data |= 0x60;
                ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_12, &u1Data, 1);
                mcSHOW_DBG_MSG(("PN case:Adjust Kp coefficient to 0x%x \n",u1Data));
           }
        }
        if(0x04 == (u1PnFnSts&MSK_ATD_CR_STATE1_FNIND))
        {//d20120221_hongji:FN case for NCR,Current setting is < 400k FN case
            ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &u1Data, 1);
            u1Data &= 0x0F;
            u1Data |= 0xD0;
            ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &u1Data, 1);
            mcSHOW_DBG_MSG(("FN case:Adjust Ki coefficient to 0x%x \n",u1Data));
        }
    }
    else
    {
    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &u1CrAutoCfg, 1); //PN-FN auto config
    ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG0,&u1Data,1);

    mcSHOW_DBG_MSG(("Null region is 0x%02X\n ",u1Data));

    if((u1PnFnSts&MSK_ATD_CR_STATE1_PNIND) && (u1CrAutoCfg&MSK_ATD_ACR_CFG_PN_ACFG_EN))//d111107_Haibo:PN indicator ON but TVD unlock.Disable PN auto config
    {
        mcSHOW_DBG_MSG(("PN IND ON.Disable Auto Config and reset null region to 2\n"));
        ATD_SetAutoConfig(psDemodCtx,0x10);//disable PN auto config
        ATD_SetNullRegion(psDemodCtx,2);//d111108_Haibo:reset null region to avoid tvd unlock caused by ovm 
    }
    else if((u1PnFnSts&MSK_ATD_CR_STATE1_FNIND) && (u1CrAutoCfg & MSK_ATD_ACR_CFG_FN_ACFG_EN))//d111107_Haibo:FN only case.Set normal FIFO as FN FIFO
    {
        mcSHOW_DBG_MSG(("FN only case.Correct the FIFO\n"));
        ATD_GetReg(psDemodCtx, REG_ATD_ACR_FN_CFG1, &u1FnBw, 1); 
        coef_a = mcGET_FIELD(u1FnBw, 0xF0, 4);
        coef_b = mcGET_FIELD(u1FnBw, 0x0F, 0);
        ATD_SetCrFIFO(psDemodCtx,coef_a,coef_b);
        }
    }
#else
    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &u1CrAutoCfg, 1); //PN-FN auto config
    ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG0,&u1Data,1);

    mcSHOW_DBG_MSG(("Null region is 0x%02X\n ",u1Data));

    if((u1PnFnSts&MSK_ATD_CR_STATE1_PNIND) && (u1CrAutoCfg&MSK_ATD_ACR_CFG_PN_ACFG_EN))//d111107_Haibo:PN indicator ON but TVD unlock.Disable PN auto config
    {
        mcSHOW_DBG_MSG(("PN IND ON.Disable Auto Config and reset null region to 2\n"));
        ATD_SetAutoConfig(psDemodCtx,0x10);//disable PN auto config
        ATD_SetNullRegion(psDemodCtx,2);//d111108_Haibo:reset null region to avoid tvd unlock caused by ovm 
    }
    else if((u1PnFnSts&MSK_ATD_CR_STATE1_FNIND) && (u1CrAutoCfg & MSK_ATD_ACR_CFG_FN_ACFG_EN))//d111107_Haibo:FN only case.Set normal FIFO as FN FIFO
    {
        mcSHOW_DBG_MSG(("FN only case.Correct the FIFO\n"));
        ATD_GetReg(psDemodCtx, REG_ATD_ACR_FN_CFG1, &u1FnBw, 1); 
        coef_a = mcGET_FIELD(u1FnBw, 0xF0, 4);
        coef_b = mcGET_FIELD(u1FnBw, 0x0F, 0);
        ATD_SetCrFIFO(psDemodCtx,coef_a,coef_b);
    }
#endif    
}

/**********************************************************************/
/*                  Query APIs                                        */
/**********************************************************************/
VOID ATD_GetInfo(ATD_CTX_T *psDemodCtx)
{
    S32     s2Data;
	UINT8 ucData1 = 0,ucData2 = 0;
	UINT16 u2Data;
#if ATD_SUPPORT_NCR    //d20120202_hongji:for NCR
    UINT8 ucTmp[4];
    INT32 CFO_Fix;
#endif    
    //==================================PRA========================================        
    mcSHOW_USER_MSG(("RF gain = 0x%04X\n", ATD_GetRFAGC(psDemodCtx)));
    mcSHOW_USER_MSG(("IF gain = 0x%04X\n", ATDPI_GetIFAGC(psDemodCtx)));
	ATD_GetReg(psDemodCtx, REG_ATD_PRA_ST, &ucData1, 1);
    mcSHOW_USER_MSG(("0x15E = 0x%02X\n", ucData1));
    //==================================CR=======================================
    s2Data = ATD_GetCFOKHz(psDemodCtx, FALSE);
    mcSHOW_USER_MSG(("CFO = %d KHz\n", s2Data));
#if ATD_SUPPORT_NCR  
    if(FALSE == fg_NCR_Sel) //for OCR Message
    {
    ATD_GetReg(psDemodCtx, REG_ATD_CR_CPO0, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CR_CPO1, &ucData2, 1);
    s2Data = (S32)mcSIGN_2BYTE(ucData1, ucData2, 5, 8);
    s2Data = (S32)s2Data * 439 / 100000;
    mcSHOW_USER_MSG(("CPO = %d degree\n", s2Data));
    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucData1, 1);                
    mcSHOW_USER_MSG(("OVM idx = %d\n", (ucData1 & 0x70) >> 4 ));
	mcSHOW_USER_MSG(("OVM IND=%d\n",ATD_GetOVMIdx(psDemodCtx)));
    mcSHOW_USER_MSG(("0x%03X = 0x%02X\n", REG_ATD_CR_STATE0,ucData1));
    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucData1, 1);
    mcSHOW_USER_MSG(("0x%03X = 0x%02X\n", REG_ATD_CR_STATE1,ucData1));
    mcSHOW_USER_MSG(("PN indicator = %d\n", (ucData1 & 0x08) >> 3 ));
    mcSHOW_USER_MSG(("FN indicator = %d\n", (ucData1 & 0x04) >> 2 ));
    ATD_GetReg(psDemodCtx, REG_ATD_CMETRIC0, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CMETRIC1, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("CR CMetrix = 0x%04X\n", u2Data));
    }
    else  //for NCR Message
    {
        //d20120202_hongji:FLL CFO
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_CFO_1,ucTmp, 1);  
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_CFO_2,ucTmp+1, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_CFO_3,ucTmp+2, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_CFO_4,ucTmp+3, 1);    
        CFO_Fix = ((ucTmp[0] & 0x3) << 24) + (ucTmp[1] << 16) + (ucTmp[2] << 8) + ucTmp[3];
        if(CFO_Fix > (1 << 25))
        {
            CFO_Fix = CFO_Fix - (1 << 26);
        }
        mcSHOW_USER_MSG(("FLL CFO = %04d\n", (CFO_Fix/4971)));
        
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_6,ucTmp, 1);       
        mcSHOW_USER_MSG(("DRE = 0x%03x \n", (ucTmp[0] * 16)));

    //Pilot Strength
    	ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_11,ucTmp, 1);
        mcSHOW_USER_MSG(("Pilot Strength = %03d/100 \n", ucTmp[0]*100/256));
        
    //In Channel Strength
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_12,ucTmp, 1);
        mcSHOW_USER_MSG(("In Channel Strength = %03d/100 \n", ucTmp[0]*100/256));

        //auto_pd_switch_thd
    	ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_3,ucTmp, 1);    	
        mcSHOW_USER_MSG(("pd_switch_thd = 0x%04d \n", (ucTmp[0]*8)));

        //max_m_index
    	ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_16,ucTmp, 1);    	
        mcSHOW_USER_MSG(("m_index = %04d /100 \n", ucTmp[0]*100/128));

        //Kp and Ki
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_11,ucTmp, 1);   
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12,ucTmp+1, 1);
        mcSHOW_USER_MSG(("Kp = %d  Ki = %d \n", (ucTmp[1]&0xF0)>>4,(ucTmp[0]&0xF0)>>4));
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_22,ucTmp, 1); // pn_indicator
    	mcSHOW_DBG_MSG2(("ucPn_indicator = %d  \n",ucTmp[0]));        
        ucTmp[1] = ucTmp[0]; 
#if !ATD_SUPPORT_NCR_FN_IND
        ucTmp[0]=ATD_GetPnFn(psDemodCtx);
        mcSHOW_DBG_MSG2(("OCR FN_indicator =%d \n",((ucTmp[0]&MSK_ATD_CR_STATE1_FNIND)>>2)));
#else  //d20121101_hongji:Use NCR fn indicator
        //d20121101_hongji:Access pll_fn_level for pll ki adjustment and show total_fn_level   
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN1,ucTmp, 1);
        ucTmp[0] |= 0x0F;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_FN1,ucTmp, 1);  //d20121101_hongji:99KHz step size
        mcDELAY_MS(10);  //d20121124_hongji:10ms delay for FN indicator stable
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN2,ucTmp+1, 1);
        mcSHOW_DBG_MSG(("Current Total  FN level is: %dKHz to %dKHz\n",((ucTmp[1]>>4)&0x07)*((ucTmp[0]&0x0F)*66)/20,(((ucTmp[1]>>4)&0x07)+1)*((ucTmp[0]&0x0F)*66)/20));
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN1,ucTmp, 1);
        ucTmp[0] &= 0xF0;
        ucTmp[0] |= 0x02;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_FN1,ucTmp, 1);  //d20121101_hongji:13.2KHz step size
        mcDELAY_MS(10);  //d20121124_hongji:10ms delay for FN indicator stable
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FN2,ucTmp+1, 1);
        mcSHOW_DBG_MSG(("Current PLL  FN level is: %dKHz to %dKHz\n",(ucTmp[1]%0x07)*((ucTmp[0]&0x0F)*66)/20,((ucTmp[1]&0x07)+1)*((ucTmp[0]&0x0F)*66)/20));
#endif    
    	ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_21,ucTmp, 1); //awgn
    	ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0,ucTmp+2, 1); //CVBS Noise Level from DRO
    	mcSHOW_DBG_MSG2(("awgn = %d   noise level = 0x%x \n",ucTmp[0],ucTmp[2]));               
        if(0x00 == ucTmp[0])
        {
            ucTmp[0]++;
        }
    	mcSHOW_DBG_MSG2(("pn indicator/awgn ratio = %d/100  \n",ucTmp[1]*100/ucTmp[0]));            
    }
#else
    ATD_GetReg(psDemodCtx, REG_ATD_CR_CPO0, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CR_CPO1, &ucData2, 1);
    s2Data = (S32)mcSIGN_2BYTE(ucData1, ucData2, 5, 8);
    s2Data = (S32)s2Data * 439 / 100000;
    mcSHOW_USER_MSG(("CPO = %d degree\n", s2Data));
    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucData1, 1);                
    mcSHOW_USER_MSG(("OVM idx = %d\n", (ucData1 & 0x70) >> 4 ));
    mcSHOW_USER_MSG(("OVM IND=%d\n",ATD_GetOVMIdx(psDemodCtx)));
    mcSHOW_USER_MSG(("0x%03X = 0x%02X\n", REG_ATD_CR_STATE0,ucData1));
    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucData1, 1);
    mcSHOW_USER_MSG(("0x%03X = 0x%02X\n", REG_ATD_CR_STATE1,ucData1));
    mcSHOW_USER_MSG(("PN indicator = %d\n", (ucData1 & 0x08) >> 3 ));
    mcSHOW_USER_MSG(("FN indicator = %d\n", (ucData1 & 0x04) >> 2 ));
    ATD_GetReg(psDemodCtx, REG_ATD_CMETRIC0, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CMETRIC1, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("CR CMetrix = 0x%04X\n", u2Data));
#endif    
    //==================================DRO=======================================
#if ATD_SUPPORT_EQ_3TAP_AUTO
    ucData1=0x0D;
    ATD_SetReg(psDemodCtx,REG_ATD_DRO_PROBE_CFG,&ucData1,1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("3Tap EQ Alpha Value = 0x%04X\n", u2Data));
#endif
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_2, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("DRO gain = 0x%04X\n", u2Data));
	ucData1 = 0x2E;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("DRO gain jitter = 0x%04X\n", u2Data));
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MAX1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MAX2, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("DRO signal swing = 0x%04X\n", u2Data));
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MIN1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LSYNC_MIN2, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("DRO sync height = 0x%04X\n", u2Data));                                
    ucData1 = 0x2C;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("DRO sync height jitter = 0x%04X\n", u2Data));
    ucData1 = 0x2D;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData2, 1);
    u2Data = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_USER_MSG(("DRO blank jitter = 0x%04X\n", u2Data));
    //==================================Noise level========================================       				
    ucData1 = IO_READ8(VIDEO_IN0_BASE, 0x80);
    mcSHOW_USER_MSG(("TVD noise level = %d\n", ucData1));
    mcSHOW_USER_MSG(("ATD noise level = 0x%08X\n", ATDPI_GetNoiseLevel(psDemodCtx)));
}
#if ATD_SUPPORT_NCR  //d20120115_hongji:NCR CFO Function
INT32 ATD_NCRGetCFOKHz(ATD_CTX_T *psDemodCtx, BOOL fgLogMessage)
{    
    UCHAR   ucTmp[4];
    INT32	CFO_Fix; //before IQmc offset
    INT32	CFO; //after IQmc offset  //d20120116_hongji:DOUBLE-->INT32 Type change
    //read CFO
    ATD_GetReg(psDemodCtx, REG_ATD_FW_CFO_1,ucTmp, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_FW_CFO_2,ucTmp+1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_FW_CFO_3,ucTmp+2, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_FW_CFO_4,ucTmp+3, 1);        
    CFO_Fix = ((ucTmp[0] & 0x3) << 24) + (ucTmp[1] << 16) + (ucTmp[2] << 8) + ucTmp[3];
    if(CFO_Fix > (1 << 25))
    {
        CFO_Fix = CFO_Fix - (1 << 26);
    }
    //CFO = (double)CFO_Fix/67108864*13500;
    //CFO = CFO_Fix/67108864*13500;
    CFO = CFO_Fix/4971;
    psDemodCtx->s4Cfo = CFO;
    if(fgLogMessage)
    {
        mcSHOW_DBG_MSG(("CFO = %d Khz\n", psDemodCtx->s4Cfo));
    }
    return 0;
}
#endif
INT32 ATD_OCRGetCFOKHz(ATD_CTX_T *psDemodCtx, BOOL fgLogMessage)
{
    UINT8 ucData[2];
    S32  CFO_Tmp;

    ATD_GetReg(psDemodCtx, REG_ATD_CR_CFO0, &ucData[0], 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CR_CFO1, &ucData[1], 1);
    CFO_Tmp = (S32)mcSIGN_2BYTE(ucData[0], ucData[1], 7, 8);
    CFO_Tmp = (S32)(CFO_Tmp * 45776 / 100000);
    psDemodCtx->s4Cfo = CFO_Tmp;
    if(fgLogMessage)
    {
        mcSHOW_DBG_MSG(("CFO = %d Khz\n", psDemodCtx->s4Cfo));
    }
    return 0;
}

INT32 ATD_GetCFOKHz(ATD_CTX_T *psDemodCtx, BOOL fgLogMessage)
{
 S32 s4TunerAFC=0;
#if ATD_SUPPORT_NCR
    if(TRUE == fg_NCR_Sel)
    {
        ATD_NCRGetCFOKHz(psDemodCtx,fgLogMessage);
    }
    else
    {
        ATD_OCRGetCFOKHz(psDemodCtx,fgLogMessage);         
    }
#else
    ATD_OCRGetCFOKHz(psDemodCtx,fgLogMessage);
#endif
    if(FALSE == fg_Acq_flag)  //d20120720_hongji:Don't do bypass if it is called by acq
    {
        DigTunerBypassI2C (TRUE); //EC_code_d9826 for eIntATD_ExtDTD
    }
	if (ITuner_OP(psDemodCtx->pTCtx, itGetTunerAFCKhz, 1, &s4TunerAFC) == ITUNER_OK)
	{
    	psDemodCtx->s4Cfo = psDemodCtx->s4Cfo + s4TunerAFC;
    	mcSHOW_DBG_MSG2(("ATD+Tuner CFO= %d,Tuner CFO = %d\n",psDemodCtx->s4Cfo,s4TunerAFC));
	}
    if(FALSE == fg_Acq_flag)
    {
        DigTunerBypassI2C (FALSE); 
    }
    return (psDemodCtx->s4Cfo);
}
UINT16 ATD_GetVOPCpo(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;//, ucData_pre=255, ii, jj=0;
    ATD_GetReg(psDemodCtx, REG_ATD_VOP_RESERVE0, &ucData, 1);
    
    psDemodCtx->u2VOPCpo = (UINT16)(ucData * 360 / 256);
    mcSHOW_DBG_MSG(("ATD VOP CPO = %d degree, 0x%02x\n", psDemodCtx->u2VOPCpo, ucData));
    return (psDemodCtx->u2VOPCpo);
}

UINT8 ATD_GetPRALock(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;

    ATD_GetReg(psDemodCtx, REG_ATD_PRA_ST, &ucData, 1);
    if( (ucData & 0x40) != 0x00 )
    {
        psDemodCtx->fgPRALock = TRUE;
    }
    else
    {
    	psDemodCtx->fgPRALock = FALSE;
    }
    return (psDemodCtx->fgPRALock);
}

BOOL ATD_ReSetDAGC(ATD_CTX_T *psDemodCtx,UINT8 ucPOAGain)
{
    UINT8 ucData;
	UINT8 ucS_Qind, ucPLT_STR;
    if(ucPOAGain <= 0x02)
    {
        mcSHOW_DBG_MSG(("Channel Scan: Enter the selectivity mode!!\n"));   
        mcSHOW_DBG_MSG(("Channel Scan: Adjust the DAGC to 0x7f and 0x6f!!\n"));
        ucData = 0x7f;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_DAGC_04, &ucData, 1);
        ucData = 0x6f;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_DAGC_14, &ucData, 1);
        
        ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucData, 1);
        mcSHOW_DBG_MSG(("Channel Scan: Adjust the null region to 1 !!\n"));
        ucData &= 0x8f;
        ucData |= 0x10;   //it is better for selectivity issue
        ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucData, 1);      

        ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucPLT_STR, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucS_Qind, 1);
        if(((ucPLT_STR & 0x0C) != 0x00) || ((ucS_Qind & 0x30) != 0x00))
        {
            psDemodCtx->fgCRLock = TRUE;
        }
        else
        {
            psDemodCtx->fgCRLock = FALSE;
        }
        mcSHOW_DBG_MSG(("Channel Scan: Leave the selectivity mode!!\n"));   
        mcSHOW_DBG_MSG(("Channel Scan: Adjust the DAGC to 0x48 and 0x38 !!\n"));
        ucData = 0x48;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_DAGC_04, &ucData, 1);
        ucData = 0x38;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_DAGC_14, &ucData, 1);
        
        ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucData, 1);
        mcSHOW_DBG_MSG(("Channel Scan: Adjust the null region to 3 !!\n"));
        ucData |= 0x30;   
        ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucData, 1);  
    }
    else
    {
        mcSHOW_DBG_MSG(("Bad pilot quality!\n"));
        psDemodCtx->fgCRLock = FALSE;
    }
    
    return psDemodCtx->fgCRLock;
}
#if ATD_SUPPORT_NCR  //d20120115_hongji:NCR Lock function
UINT8 ATD_NCRGetCRLock(ATD_CTX_T *psDemodCtx, BOOL fgLogMessage)  //NCR ATD_GetCRLock function
{
    //float Pilot_Strength, Inchannel_Strength;
    UINT8 Pilot_Strength;
    UINT8 ii;
    //UCHAR ucTmp[1];
    UINT8 ucData;
    //FLOAT ISbound = (float)0.75;  //unused
    //FLOAT PSACIbound = (float)0.055;  //unused
    //FLOAT PSbound = (float)0.7;     //8227 & MT5398
    UINT8 PSbound = 179;  //d20120116_hongji:threshold: 0.7*256=179.2
    //FLOAT Audio_IS = (float)0.6;
//    UINT8 Audio_IS = 154;  //d20120116_hongji:threshold: 0.6*256=153.6
    UINT8 b5 = 5;
//    UINT8 ucNCR_LockTime = 10;
    UINT8 ucFLL_LockTime = 4;	
    UINT8 ucCdcSts, ucAudFilter;	// Used to check the SSIF CDC status
    psDemodCtx->fgCRLock = FALSE;

    for(ii =0; ii <= ucFLL_LockTime; ii++)
    {
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_FLL_6, &ucData, 1);
        if(mcTEST_BIT(ucData, b5))  // FLL Lock ==> NCR Lock
        {
            mcSHOW_DBG_MSG2(("FLL Lock!\n"));
            mcDELAY(14); //IS & PS converged time

            ATD_GetReg(psDemodCtx, REG_ATD_POA_PROBE_GWRDH, &ucData, 1);
            ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_11,&ucData, 1);
            Pilot_Strength = ucData;
            //Pilot_Strength = Pilot_Strength/256;  //d20120116_hongji:marked(change float algorithm)
            if(Pilot_Strength > PSbound) //has signal
            {
                mcSHOW_DBG_MSG2(("Pilot Strength : %d / 100 ==> has Signal \n", Pilot_Strength*100/256));
                psDemodCtx->fgCRLock = TRUE;
            }
            else
            {
#if 0          
                //In Channel Strength
                ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_12,&ucData, 1);
                Inchannel_Strength = ucData;
                //Inchannel_Strength = Inchannel_Strength/256; //d20120116_hongji:marked(change float algorithm)
                if(Inchannel_Strength > Audio_IS)
                {
                    mcSHOW_DBG_MSG2(("Pilot Strength : %d / 256 & In channel Strength = %d / 256 => NO Signal \n", Pilot_Strength, Inchannel_Strength));
                    psDemodCtx->fgCRLock = FALSE;
                }
                else
                {
                    mcSHOW_DBG_MSG2(("Pilot Strength : %d / 256 & In channel Strength = %d /256 => Stong Audio Signal \n", Pilot_Strength, Inchannel_Strength));
                    psDemodCtx->fgCRLock = TRUE;
                }
#endif                    
            }
            break;
        }
        else //FLL unLock
        {
            psDemodCtx->fgCRLock = FALSE;
            mcSHOW_DBG_MSG2(("FLL unLock!\n"));
        }
        mcDELAY(5); // delay 5ms
    }

	
    if(psDemodCtx->fgCRLock)
    { //CR Lock, check CDC status, if CDC status is unsync, toggle the audio filter bypass
        ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG3, &ucCdcSts, 1);
        if((ucCdcSts & 0x03) != 0x03)
        {  // Audio filter path filtering reset
            ATD_GetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucAudFilter, 1);
            ucAudFilter &= 0xF7;
            ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucAudFilter, 1);
            mcDELAY_MS(1);   // delay 1ms
            ucAudFilter |= 0x08;
            ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucAudFilter, 1);
            mcSHOW_DBG_MSG(("SSIF CDC unsync, reset the audio filter!\n"));
        }
    }
    return (psDemodCtx->fgCRLock);
}
#endif
UINT8 ATD_OCRGetCRLock(ATD_CTX_T *psDemodCtx, BOOL fgLogMessage)
{
    UINT8 ii,ucData, ucData3[3],OvmIdx,ucPOAGain;
    UINT32 u4PilotPower, u4SignalPower, u2DroGain;
    UINT8 u1Fref2;
	UINT8 ucS_Qind, ucPLT_STR;	
    UINT8 ucCdcSts, ucAudFilter;	// Used to check the SSIF CDC status
	BOOL fgACI;    
    UINT16 u2ADCValue;
	
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1);
	ucData &= 0xf0;
    ucData |= 0x01;
	ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1);
	ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &u1Fref2, 1);
    mcSHOW_DBG_MSG(("Fref2_MSB = 0x%02x\n", u1Fref2));

	if (u1Fref2<=0x08) 
	{
	    fgACI=1;
        ATD_SetNullRegion(psDemodCtx,1);//d111108_Haibo:set null region
        OvmIdx=ATD_GetOVMIdx(psDemodCtx);
        if(7 == OvmIdx)
        {
		    mcSHOW_DBG_MSG(("OvmIdx=%d\n",OvmIdx));
		    ATD_SetCrBwAutoCfg(psDemodCtx,TRUE);//d111108_Haibo:Enable Pilot filter BW auto config when strong ACI case
            if(psDemodCtx->pTCtx->fgRFTuner)
            {
                u2ADCValue = ATD_GetADCValue(psDemodCtx);
                if(u2ADCValue >= 0x0050)
                {
                    for(ii=0x00;ii<g_CR_BW_AutoConfig_Delay_cnt;ii++)
                    {
                        u4PilotPower=ATD_GetPilotPower(psDemodCtx);     
				        mcDELAY_MS(10);
                    } 
                }
            } 
            psDemodCtx->pTCtx->u2RetryTVDTm=100;			
        }
        else
        {
            ATD_SetCrBwAutoCfg(psDemodCtx,FALSE);
		}
	}
	else
	{
	    fgACI=0;
        ATD_SetCrBwAutoCfg(psDemodCtx,FALSE);
		ATD_SetNullRegion(psDemodCtx,2);
	}
	if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))
    {
         fgACI=1;    //SECAM L can not judgment due to the DAGC bypassing
	}

    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucPLT_STR, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucS_Qind, 1);

    u4PilotPower=ATD_GetPilotPower(psDemodCtx);  

    ucData = (0x07 << 3); // Select ADVPS_PROB as SignalPower by setting 0x9BB[b5:b3] 0x07
    ATD_SetReg(psDemodCtx, REG_ATD_ADVPS_CFG2 , &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_ADVPS_PRB0, ucData3, 3);            
    u4SignalPower = mcUNSIGN_2BYTE(ucData3[0], ucData3[1], 8, 8);
    u4SignalPower = (u4SignalPower << 8) + ucData3[2];

    ATD_GetReg(psDemodCtx, REG_ATD_DRO_WADC_1, ucData3, 2);    
    u2DroGain = mcUNSIGN_2BYTE(ucData3[0], ucData3[1], 8, 8);
    ATD_GetReg(psDemodCtx, REG_ATD_POA_PROBE_GWRDH, &ucData, 1);
    ucPOAGain = ucData;
               
    if(fgLogMessage)
    {
        mcSHOW_DBG_MSG(("Pilot Strength = 0x%02x\n", (ucPLT_STR & 0x0C) >> 2  ));
        mcSHOW_DBG_MSG(("Pilot Stability = 0x%02x\n", (ucS_Qind & 0x20) >> 5 ));
        mcSHOW_DBG_MSG(("Pilot Quality = 0x%02x\n", (ucS_Qind & 0x10) >> 4 ));
        mcSHOW_DBG_MSG(("Pilot Power = %08d\n", u4PilotPower));
        mcSHOW_DBG_MSG(("Signal Power = %08d\n", u4SignalPower));
        mcSHOW_DBG_MSG(("DRO Gain = %08d\n", u2DroGain));
		mcSHOW_DBG_MSG(("Fref2=0x%02X\n",u1Fref2));
        mcSHOW_DBG_MSG(("POA Gain = 0x%02x\n", ucPOAGain ));
    }

#if ((defined CC_MT5365) || (defined CC_MT5395))
    if((ucS_Qind & 0x30) != 0) // pilot stability or pilot quality
#elif ((defined CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389))
    if( ((ucPLT_STR & 0x0C) != 0x00) || (0x30 == (ucS_Qind & 0x30)))
#else
    if(((ucPLT_STR & 0x0C) != 0x00) || (0x30 == (ucS_Qind & 0x30)))// || (u4PilotPower >= 0x00000100) ) //&& ((ucData2 & 0x33) == 0x03) )        
#endif           
    {
        psDemodCtx->fgCRLock = TRUE;
    }
	else if((fgACI)&&(0x20 == (ucS_Qind&0x20))) //for ACI case
	{
		psDemodCtx->fgCRLock = TRUE;
		mcSHOW_DBG_MSG2(("VIF lock modified by ACI case!\n"));
	}
#if (defined NA_PLATFORM)    
    else if(_fgAutoSearch)
    {
        mcSHOW_DBG_MSG(("channel scan is  = %08d\n", _fgAutoSearch));
        psDemodCtx->fgCRLock = ATD_ReSetDAGC(psDemodCtx,ucPOAGain);        
    }
#endif    
    else if(ucPOAGain <= 0x02) //for audio carrier > video carrier case
    {
        psDemodCtx->fgCRLock = TRUE;
    }
    else
    {
        mcSHOW_DBG_MSG(("Bad pilot quality!\n"));
        psDemodCtx->fgCRLock = FALSE;
    }

    
    if(psDemodCtx->fgCRLock)
    { //CR Lock, check CDC status, if CDC status is unsync, toggle the audio filter bypass
        ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG3, &ucCdcSts, 1);
        if((ucCdcSts & 0x03) != 0x03)
        {  // Audio filter path filtering reset
            ATD_GetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucAudFilter, 1);
            ucAudFilter &= 0xF7;
            ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucAudFilter, 1);
            mcDELAY_MS(1);   // delay 1ms
            ucAudFilter |= 0x08;
            ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucAudFilter, 1);
            mcSHOW_DBG_MSG(("SSIF CDC unsync, reset the audio filter!\n"));
        }
    }
	
    return (psDemodCtx->fgCRLock);
}

UINT8 ATD_GetCRLock(ATD_CTX_T *psDemodCtx, BOOL fgLogMessage)
{
#if ATD_SUPPORT_NCR    
    if(TRUE == fg_NCR_Sel)
    {
        ATD_NCRGetCRLock(psDemodCtx,fgLogMessage);
    }
    else
    {
        ATD_OCRGetCRLock(psDemodCtx,fgLogMessage);
    }
#else
    ATD_OCRGetCRLock(psDemodCtx,fgLogMessage);
#endif
    return (psDemodCtx->fgCRLock);
}
#if ATD_SUPPORT_VSI
BOOL ATD_GetVSILock(ATD_CTX_T *psDemodCtx)
{
    //Step 1. To start channel scan, assume the system spec (NTSC/PAL/SECAM) of the received signal 
    //Step 2. Set all parameters for channel scan and reset
    //Step 3. Check if CR lock by indicator fll_converged. If CR unlock ? try another system
    //Step 4. Repeat step 1 to 3 until CR lock (read ¡§fll_converged¡¨ == 1)
    //Step 5. Enable VIF indicator: write ¡§vif_enable¡¨ = 1
    //Step 6. Read out VIF indicator ¡§vif1_ind¡¨ for channel scan reference
    //Step 7. Result: and(CR pilot strength > threshold , VIF indicator) Enable the VIF function 
    UINT8 ucData; 
//    UINT16 u2ToneDetect;
    BOOL fgVIF_Lock=FALSE;
    
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData, 1); //bypass LP3
    ucData &= 0xFE;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData, 1);
    
    ATD_GetReg(psDemodCtx, REG_ATD_CTL_RST2,&ucData, 1);// reset the VIF
    ucData &=0xF7;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2,&ucData, 1);
    ucData |=0x08;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2,&ucData, 1);
    
    ATD_GetReg(psDemodCtx, REG_ATD_VIF_CFG04,&ucData, 1);
    ucData &=0x03;
    ucData +=4;
    ucData = ucData*20;
    mcSHOW_DBG_MSG2(("VSI: Delay = %d!\n", ucData));
    mcDELAY_MS(ucData);
    ATD_GetReg(psDemodCtx, REG_ATD_VIF_CFG05,&ucData, 1);
    ucData &=0x01;
    if (ucData == 0x00)
    {
        mcSHOW_DBG_MSG2(("VSI: No signal!\n"));
        fgVIF_Lock = 0;
    }
    else
    {
        mcSHOW_DBG_MSG2(("VSI: Have signal!\n"));
        fgVIF_Lock = 1;
    }
#if 0
    // tone detection; //20111024
    if (fgVIF_Lock==1)
    {               
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_TONE_DETEC01,&ucData, 1);
        ucData &= 0x7F;
        u2ToneDetect = (UINT16)ucData;
        u2ToneDetect = (u2ToneDetect << 8);
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_TONE_DETEC02,&ucData, 1);
        u2ToneDetect |= ucData;
        mcSHOW_DBG_MSG2(("Tone Dection  = 0x%04x!\n", u2ToneDetect));                   
        if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx) || (MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))  //Positive modulation   
        {
            if (u2ToneDetect>=ToneDetectThreshold_SECAM)
            {
                fgVIF_Lock = 1;
            }
            else
            {
                fgVIF_Lock = 0;
            }
        }
        else        //Negative modulation
        {
            if((MOD_PAL_BG == psDemodCtx->u1SubSysIdx)||(MOD_PAL_DK== psDemodCtx->u1SubSysIdx)||(MOD_PAL_I == psDemodCtx->u1SubSysIdx))
            {
                if (u2ToneDetect>=ToneDetectThreshold_PAL)
                {
                    fgVIF_Lock = 1;
                }
                else
                {
                    fgVIF_Lock = 0;
                }
            }
            else if(MOD_NTSC_M == psDemodCtx->u1SubSysIdx)
            {
                if (u2ToneDetect>=ToneDetectThreshold_NTSC)
                {
                    fgVIF_Lock = 1;
                }
                else
                {
                    fgVIF_Lock = 0;
                }
            }
        }   
    }
#endif	
    //Enable scaling for VIF Lock
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_0,&ucData, 1);//d120204_Haibo:why need to enable scaling.Who disable it?
    ucData |= 0x80;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_SCAL_0,&ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData, 1); //Enable LP3
    ucData |= 0x01;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData, 1);
    return fgVIF_Lock;   
}
#endif

#if ATD_SUPPORT_H_SYNDROME
//d20121109_hongji:only to avoid strong tone false alarm
BOOL ATD_Tone_DeviationChannel(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData1,ucData2;
    UINT16 u2ToneDetect;    
    BOOL fgTone_Deviation_Lock;
#if 0    //d20121122_hongji:Enable LPF3 for tone detection access according ChingTing's suggestion
    //d20121109_hongji:bypass LP3 for tone detection access
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData1, 1); //bypass LP3
    ucData1 &= 0xFE;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData1, 1);
#endif    

    ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_TONE_DETEC01,&ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_TONE_DETEC02,&ucData2, 1);
	u2ToneDetect = mcUNSIGN_2BYTE(ucData1,ucData2,7,8);
    mcSHOW_DBG_MSG3(("Tone Dection  = 0x%04x!\n", u2ToneDetect));					
	if ((MOD_SECAM_L == (psDemodCtx->u1SubSysIdx))||(MOD_SECAM_L1 == (psDemodCtx->u1SubSysIdx)))	//Positive modulation	
    {
        if (u2ToneDetect>=TONE_DETECTION_THRESHOLD_L)
        {
            fgTone_Deviation_Lock = 1;
        }
    	else
	    {
		    fgTone_Deviation_Lock = 0;
    	}
	}
    else        //Negative modulation
    {
        if (u2ToneDetect>=TONE_DETECTION_THRESHOLD)
        {
            fgTone_Deviation_Lock = 1;
        }
    	else
        {
            fgTone_Deviation_Lock = 0;
    	}
    }
    //Enable scaling for VIF Lock
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_0,&ucData1, 1);
    ucData1 |= 0x80;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_SCAL_0,&ucData1, 1);  //d20121109_hongji:who disable scaling module ?
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData1, 1); //Enable LP3
    ucData1 |= 0x01;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_VSI_CFG4,&ucData1, 1);
    return fgTone_Deviation_Lock;
}
#endif

#if ATD_SUPPORT_H_SYNDROME
BOOL ATD_HsyncChannel(ATD_CTX_T *psDemodCtx)
{   
    //[Remy]FPGA: check by HW
    UINT8 ucData, ucData1;
    UINT16 u2HsynDrome_cnt;
    BOOL fgHsync_Lock;
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF0_1,&ucData, 1);
    ucData |= 0x80;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF0_1,&ucData, 1);  //re-trigger  h-syndrome cnt
    ucData = 0x0A;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG,&ucData, 1);  //H-Syndrome cnt output

    while (1)
    {
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1,&ucData, 1); 
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2,&ucData1, 1);
        u2HsynDrome_cnt = mcUNSIGN_2BYTE(ucData,ucData1,2,8);
        mcSHOW_DBG_MSG3(("H-Syndrome cnt is  = 0x%x \n", u2HsynDrome_cnt));  //H-Syndrome cnt

        ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF0_1,&ucData, 1); //reset H-syndrom detection 
        if ((ucData&0x80) == 0x80)  //H-Syndrome detect period is finished
        {
            break;      
        }
    }
    
    if ((ucData&0x40) == 0x40) //H-Syndrome  lock flag
    {
        fgHsync_Lock = 1;
    }
    else
    {
        fgHsync_Lock = 0;
    }    
    return fgHsync_Lock;
}
#endif
#if ATD_SUPPORT_H_SYNDROME
UINT16 ATD_GetLRE(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData,ucData1;  //ucInitial_FREF2_MID_LEN,ucInitial_StepSize; // 0x21B
    UINT16 u2LRE;
    UINT8 ucInital_DRO_REF_COEF,ucInitial_DRO_MASK_CFG2,ucInitial_NCR_PJ_2,ucInitial_DRO_LP_CFG03;

    //ATD_GetReg(psDemodCtx, REG_ATD_TD_NF_MUL_COEF_2, &ucData, 1); //middle term length delta
    //ucInitial_FREF2_MID_LEN = ucData;

    //ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);  //timing tracking index step size
    //ucInitial_StepSize = ucData;

    //d20121130_hongji:Back up related setting
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1);
    ucInital_DRO_REF_COEF = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2,&ucData, 1);
    ucInitial_DRO_MASK_CFG2 = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_PJ_2,&ucData, 1);
    ucInitial_NCR_PJ_2 = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03,&ucData, 1);
    ucInitial_DRO_LP_CFG03 = ucData;

    ucData = 0x07;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_IMP_TIMER_RST, &ucData, 1);  //?
    //[Remy] FPGA Validation : Channel scan setting
    //Added by hongji:Set timing tracking threshold as FPGA setting
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1); 
    ucData &= 0xF0;
    ucData |= 0x01;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1);

    //Set StepSize of Scan = 6;
    //ucData = ((ucData >> 4 ) << 4) + 0x06;  //d20121109_hongji: why clean bit [7:5] ? marked ,change to read-write,
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);
    ucData &= 0xE0;
    ucData |= 0x0A;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);  //timing tracking index step size

    //[Remy] FPGA: Do LRE & H syndrome flow
    mcSHOW_DBG_MSG4(("ACQ = 2^-3\n"));
    mcSHOW_DBG_MSG4(("Trk = 2^-5\n"));
    mcSHOW_DBG_MSG4(("Vbound = 7x4 =28 \n"));

    //enable LRE, don't need enable timing tracking mode, only lpf3 path
    mcSHOW_DBG_MSG4(("1. Enable LRE\n"));
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
    ucData = ucData | 0x10; // enable LRE
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
    mcDELAY(10); //delay 10ms to wait LRE stable

    //get LRE
    ucData = 0x0B; 
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData1, 1);
    u2LRE = mcUNSIGN_2BYTE(ucData,ucData1,8,8);
    mcSHOW_DBG_MSG4(("2. LRE = %d\n", u2LRE));
    
    //d20121130_hongji:recover    related initial setting
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucInital_DRO_REF_COEF, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2,&ucInitial_DRO_MASK_CFG2, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_PJ_2,&ucInitial_NCR_PJ_2, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03,&ucInitial_DRO_LP_CFG03, 1);
    return u2LRE;
}
#endif

#if ATD_SUPPORT_H_SYNDROME
/********************************************************
Return:
0. Not ATV signal
1. PAL/NTSC signal
2. SECAM signal
*********************************************************/
UINT8 ATD_GetHSydLock(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData,ucATD_H_Lock;  //ucInitial_FREF2_MID_LEN,ucInitial_StepSize; // 0x21B
    BOOL fgHsync_Lock,fgTone_Deviation_Lock;
    UINT8 ucInital_DRO_REF_COEF,ucInitial_DRO_MASK_CFG2,ucInitial_NCR_PJ_2,ucInitial_DRO_LP_CFG03;

    ucATD_H_Lock = 0;

    //ATD_GetReg(psDemodCtx, REG_ATD_TD_NF_MUL_COEF_2, &ucData, 1); //middle term length delta
    //ucInitial_FREF2_MID_LEN = ucData;

    //ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);  //timing tracking index step size
    //ucInitial_StepSize = ucData;

    //d20121130_hongji:Back up related setting
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1);
    ucInital_DRO_REF_COEF = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2,&ucData, 1);
    ucInitial_DRO_MASK_CFG2 = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_PJ_2,&ucData, 1);
    ucInitial_NCR_PJ_2 = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03,&ucData, 1);
    ucInitial_DRO_LP_CFG03 = ucData;

    ucData = 0x07;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_IMP_TIMER_RST, &ucData, 1);  //?
    //[Remy] FPGA Validation : Channel scan setting
    //Added by hongji:Set timing tracking threshold as FPGA setting
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1); 
    ucData &= 0xF0;
    ucData |= 0x01;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1);
    
    //Set StepSize of Scan = 6;
    //ucData = ((ucData >> 4 ) << 4) + 0x06;  //d20121109_hongji: why clean bit [7:5] ? marked ,change to read-write,
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);
    ucData &= 0xE0;
    ucData |= 0x0A;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);  //timing tracking index step size

    //[Remy] FPGA: Do LRE & H syndrome flow
    mcSHOW_DBG_MSG4(("ACQ = 2^-3\n"));
    mcSHOW_DBG_MSG4(("Trk = 2^-5\n"));
    mcSHOW_DBG_MSG4(("Vbound = 7x4 =28 \n"));
    
    //d201201109_hongji:enable timing tracking mode according to current mid-term-len-delta for H-syndrome detect
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
    ucData |= 0x40;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
    mcDELAY(5); //[Remy] FPGA: delay 5ms to wait for Timimg tracking converged    
    //Step 2: ebable H syndrome
    mcSHOW_DBG_MSG4(("5. Enable H-Syndrome\n"));
    fgHsync_Lock = ATD_HsyncChannel(psDemodCtx);
    
    //d201201109_hongji:disable timing tracking mode    for tone detection
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
    ucData &= 0xBF;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
    fgTone_Deviation_Lock = ATD_Tone_DeviationChannel(psDemodCtx);
    
    // NCR & H & Tone
    if (fgHsync_Lock && fgTone_Deviation_Lock)
    {
        ucATD_H_Lock = 0x01;
        mcSHOW_DBG_MSG3(("Current signal perhaps is PAL/NTSC ATV signal \n"));
    }
    else
    {        
        mcSHOW_DBG_MSG4(("Not PAL/NTSC signal, Try SECAM \n"));
        //SECAM CR/DRO        related setting
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_PJ_2, &ucData, 1);
        ucData = ucData & 0xFE;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_PJ_2, &ucData, 1);
        ucData = 0x01;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData, 1);
        ucData = 0x00;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData, 1);  //?              
        
        //d201201109_hongji:enable timing tracking mode according to current mid-term-len-delta for H-syndrome detect
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
        ucData |= 0x40;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
        mcDELAY(5); //[Remy] FPGA: delay 5ms to wait for Timimg tracking converged
        mcSHOW_DBG_MSG4(("Enable H-Syndrome\n"));
        fgHsync_Lock = ATD_HsyncChannel(psDemodCtx);        
        
        //d201201109_hongji:disable timing tracking mode     for tone detection
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
        ucData &= 0xBF;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);      
        fgTone_Deviation_Lock = ATD_Tone_DeviationChannel(psDemodCtx);
        
        if (fgHsync_Lock && fgTone_Deviation_Lock)
        {
            ucATD_H_Lock = 0x02;
            mcSHOW_DBG_MSG3(("Current signal perhaps is SECAM ATV signal \n"));
        }
        else
        {
            ucATD_H_Lock = 0x00;
            mcSHOW_DBG_MSG3(("Current signal is not ATV signal \n"));            
        }
    }  
    //d20121130_hongji:recover    related initial setting
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucInital_DRO_REF_COEF, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2,&ucInitial_DRO_MASK_CFG2, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_PJ_2,&ucInitial_NCR_PJ_2, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03,&ucInitial_DRO_LP_CFG03, 1);
    return ucATD_H_Lock;
}
#endif

#if ATD_SUPPORT_H_SYNDROME
BOOL ATD_GetHSydromeLock(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData,ucData1;  //ucInitial_FREF2_MID_LEN,ucInitial_StepSize; // 0x21B
    UINT16 u2LRE;
    INT16 s2Estimated_FREF2_MID_LEN,s2Set_Estimated_FREF2_MID_LEN;
    BOOL fgATD_H_Lock,fgHsync_Lock;
    UINT8 ucInital_DRO_REF_COEF,ucInitial_DRO_MASK_CFG2,ucInitial_NCR_PJ_2,ucInitial_DRO_LP_CFG03;

    fgATD_H_Lock = 0;

    //ATD_GetReg(psDemodCtx, REG_ATD_TD_NF_MUL_COEF_2, &ucData, 1); //middle term length delta
    //ucInitial_FREF2_MID_LEN = ucData;

    //ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);  //timing tracking index step size
    //ucInitial_StepSize = ucData;

    //d20121130_hongji:Back up related setting
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1);
    ucInital_DRO_REF_COEF = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2,&ucData, 1);
    ucInitial_DRO_MASK_CFG2 = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_PJ_2,&ucData, 1);
    ucInitial_NCR_PJ_2 = ucData;
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03,&ucData, 1);
    ucInitial_DRO_LP_CFG03 = ucData;
    
    ucData = 0x07;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_IMP_TIMER_RST, &ucData, 1);  //?
    //[Remy] FPGA Validation : Channel scan setting
    //Added by hongji:Set timing tracking threshold as FPGA setting
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1); 
    ucData &= 0xF0;
    ucData |= 0x01;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucData, 1);
    
    //Set StepSize of Scan = 6;
    //ucData = ((ucData >> 4 ) << 4) + 0x06;  //d20121109_hongji: why clean bit [7:5] ? marked ,change to read-write,
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);
    ucData &= 0xE0;
    ucData |= 0x0A;  //d20121126_hongji:modify timing tracking stepsize len to 10 according to remy verification test data
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2, &ucData, 1);  //timing tracking index step size

    //[Remy] FPGA: Do LRE & H syndrome flow
    mcSHOW_DBG_MSG4(("ACQ = 2^-3\n"));
    mcSHOW_DBG_MSG4(("Trk = 2^-5\n"));
    mcSHOW_DBG_MSG4(("Vbound = 7x4 =28 \n"));

    //Step 1:enable LRE, don't need enable timing tracking mode, only lpf3 path
    mcSHOW_DBG_MSG4(("1. Enable LRE\n"));
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
    ucData = ucData | 0x10; // enable LRE
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
    mcDELAY(10); //delay 10ms to wait LRE stable
    
    //get LRE
    ucData = 0x0B; 
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData1, 1);
    u2LRE = mcUNSIGN_2BYTE(ucData,ucData1,8,8);
    mcSHOW_DBG_MSG4(("2. LRE = %d\n", u2LRE));
    
    s2Estimated_FREF2_MID_LEN = u2LRE - LINE_RATE_REF;
    mcSHOW_DBG_MSG4(("3. s2Estimated_FREF2_MID_LEN = %d\n", s2Estimated_FREF2_MID_LEN));
    if (s2Estimated_FREF2_MID_LEN >= 127)
    {
        s2Set_Estimated_FREF2_MID_LEN = 127;
    }
    else if (s2Estimated_FREF2_MID_LEN <= -127)
    {
        s2Set_Estimated_FREF2_MID_LEN = -127;
    }  
    else
    {
        s2Set_Estimated_FREF2_MID_LEN = s2Estimated_FREF2_MID_LEN;
    }
    ucData = s2Set_Estimated_FREF2_MID_LEN;
    mcSHOW_DBG_MSG4(("4. s2Set_Estimated_FREF2_MID_LEN = %d\n", s2Set_Estimated_FREF2_MID_LEN));
    ATD_SetReg(psDemodCtx, REG_ATD_TD_NF_MUL_COEF_2, &ucData, 1);  //set middle term length delta
    
    //d201201109_hongji:enable timing tracking mode according to current mid-term-len-delta for H-syndrome detect
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
    ucData |= 0x40;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
    mcDELAY(5); //[Remy] FPGA: delay 5ms to wait for Timimg tracking converged    
    //Step 2: ebable H syndrome
    mcSHOW_DBG_MSG4(("5. Enable H-Syndrome\n"));
    fgHsync_Lock = ATD_HsyncChannel(psDemodCtx);
    
    //d201201109_hongji:disable timing tracking mode    for tone detection
    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
    ucData &= 0xBF;
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
    
    // d20121212_hongji:only NCR & H 
    if (fgHsync_Lock)
    {
        fgATD_H_Lock = TRUE;
        mcSHOW_DBG_MSG3(("Current signal perhaps is PAL/NTSC ATV signal \n"));
    }
    else
    {        
        mcSHOW_DBG_MSG4(("Not PAL/NTSC signal, Try SECAM \n"));
        //SECAM CR/DRO        related setting
        ATD_GetReg(psDemodCtx, REG_ATD_NCR_PJ_2, &ucData, 1);
        ucData = ucData & 0xFE;
        ATD_SetReg(psDemodCtx, REG_ATD_NCR_PJ_2, &ucData, 1);
        ucData = 0x01;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData, 1);
        ucData = 0x00;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData, 1);  //?
        
        //Step 1:
        mcSHOW_DBG_MSG4(("1. Enable LRE\n"));
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
        ucData = ucData | 0x10; // enable LRE
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
        mcDELAY(10); //delay 10ms to wait LRE stable
        //get LRE
        ucData = 0x0B; 
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_PROBE_CFG, &ucData, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_RPOBE_1, &ucData, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_PROBE_2, &ucData1, 1);
        u2LRE = mcUNSIGN_2BYTE(ucData,ucData1,8,8);
        mcSHOW_DBG_MSG4(("2. LRE = %d\n", u2LRE));
        
        s2Estimated_FREF2_MID_LEN = u2LRE - LINE_RATE_REF;
        mcSHOW_DBG_MSG4(("3. s2Estimated_FREF2_MID_LEN = %d\n", s2Estimated_FREF2_MID_LEN));
        if (s2Estimated_FREF2_MID_LEN >= 127)
        {
            s2Set_Estimated_FREF2_MID_LEN = 127;
        }
        else if (s2Estimated_FREF2_MID_LEN <= -127)
        {
            s2Set_Estimated_FREF2_MID_LEN = -127;
        }  
        else
        {
            s2Set_Estimated_FREF2_MID_LEN = s2Estimated_FREF2_MID_LEN;
        }
        ucData = s2Set_Estimated_FREF2_MID_LEN;
        mcSHOW_DBG_MSG4(("4. s2Set_Estimated_FREF2_MID_LEN = %d\n", s2Set_Estimated_FREF2_MID_LEN));
        ATD_SetReg(psDemodCtx, REG_ATD_TD_NF_MUL_COEF_2, &ucData, 1);
        
        //d201201109_hongji:enable timing tracking mode according to current mid-term-len-delta for H-syndrome detect
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
        ucData |= 0x40;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);
        mcDELAY(5); //[Remy] FPGA: delay 5ms to wait for Timimg tracking converged
        mcSHOW_DBG_MSG4(("5. Enable H-Syndrome\n"));
        fgHsync_Lock = ATD_HsyncChannel(psDemodCtx);        
        
        //d201201109_hongji:disable timing tracking mode     for tone detection
        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);    
        ucData &= 0xBF;
        ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG02, &ucData, 1);      
        // d20121212_hongji:only NCR & H 
        if (fgHsync_Lock)
        {
            fgATD_H_Lock = TRUE;
            mcSHOW_DBG_MSG3(("Current signal perhaps is SECAM ATV signal \n"));
        }
        else
        {
            fgATD_H_Lock = FALSE;
            mcSHOW_DBG_MSG3(("Current signal is not ATV signal \n"));            
        }
    }  
     //d20121130_hongji:recover    related initial setting
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_REF_COEF,&ucInital_DRO_REF_COEF, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_MASK_CFG2,&ucInitial_DRO_MASK_CFG2, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_PJ_2,&ucInitial_NCR_PJ_2, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03,&ucInitial_DRO_LP_CFG03, 1);
    
    return fgATD_H_Lock;
}
#endif

UINT8 ATD_GetVIFLock(ATD_CTX_T *psDemodCtx, BOOL fgLogMessage)
{
    INT16 s2FullVIFLockRange;
	UINT16  u2LOStep;
    INT16  S2PartialLockCFORange;
#if (ATD_SUPPORT_VSI|ATD_SUPPORT_H_SYNDROME)
    BOOL fgATDLock;
#endif
#if defined(NA_PLATFORM)
    u2LOStep=psDemodCtx->pTCtx->specific_member.us_ctx.u2LO_Step;
#else
    u2LOStep=psDemodCtx->pTCtx->u2LO_Step;	
#endif
	s2FullVIFLockRange=(INT16)(u2LOStep/100);
	if(s2FullVIFLockRange<cATD_VIF_FULL_LOCK_MIN_RANGE)
    {   
		s2FullVIFLockRange=cATD_VIF_FULL_LOCK_MIN_RANGE;
    }
#if ATD_SUPPORT_NCR    
    if(TRUE == fg_NCR_Sel)
    {
        S2PartialLockCFORange = cATD_NCR_VIF_PARTIAL_LOCK_RANGE;
    }
    else
    {
        S2PartialLockCFORange = cATD_OCR_VIF_PARTIAL_LOCK_RANGE;
    }
#else
    S2PartialLockCFORange = cATD_VIF_PARTIAL_LOCK_RANGE;
#endif
#ifdef CC_SUPPORT_ATV_DESCRAMBLE
    if(psDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn) //descramble on
    {
        mcDELAY_MS(20); //delay 20ms before check VIF lock
    }
#endif
#if (ATD_SUPPORT_VSI|ATD_SUPPORT_H_SYNDROME)
#ifdef CC_SUPPORT_ATV_DESCRAMBLE
    if(psDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn) //descramble on
    {
        fgVSI_On = FALSE; //Turn off VSI or H-Syndrome at descramble on, for sync may be abnormal.
        mcSHOW_DBG_MSG(("Turn off VSI/H-Syndrome at descramble on, for sync may be abnormal!\n"));
    }
    else
    {
        fgVSI_On = TRUE;
        mcSHOW_DBG_MSG(("Turn on VSI/H-Syndrome at descramble off!\n"));
    }
#endif
#endif
    if (fgLogMessage)
    {
        ATD_GetRFAGC(psDemodCtx);
        ATDPI_GetIFAGC(psDemodCtx);
    }
    ATD_GetCRLock(psDemodCtx, fgLogMessage);
    ATD_GetCFOKHz(psDemodCtx, fgLogMessage);

    if( (FALSE == psDemodCtx->fgCRLock) && (TRUE == psDemodCtx->fgCheckCR) )
    {
        psDemodCtx->u1VIFLock = cATD_VIF_UNLOCK;
    }
    else
    {
    	if( (psDemodCtx->s4Cfo > (-s2FullVIFLockRange)) && 
            (psDemodCtx->s4Cfo <   s2FullVIFLockRange)   )
    	{
    	    psDemodCtx->u1VIFLock = cATD_VIF_FULL_LOCK;
#if (ATD_SUPPORT_VSI|ATD_SUPPORT_H_SYNDROME)  //d20120115_hongji:VSI Function for MT5398
            if((TRUE == fgVSI_On)  && (TRUE==_fgAutoSearch))  //d20121109_hongji:only for channel scan
            {
#if ATD_SUPPORT_VSI            
                fgATDLock = ATD_GetVSILock(psDemodCtx);
#else
                fgATDLock = ATD_GetHSydromeLock(psDemodCtx);
#endif
            }
            else
            {
                fgATDLock = 1;
            }
            if(!fgATDLock)
            {
                psDemodCtx->u1VIFLock = cATD_VIF_UNLOCK;
            }
            else
            {
#endif
#if !TVD_SUPPORT_SNOW_MODE
            mcSHOW_DBG_MSG3(("Snow Mode Off: VIF full lock\n"));
            ATD_SetSnowMode(psDemodCtx, FALSE);  // turn off snow mode when VIF lock
#endif
#if (ATD_SUPPORT_VSI|ATD_SUPPORT_H_SYNDROME)
            }
#endif            
    	}
    	else if( (psDemodCtx->s4Cfo >= (-S2PartialLockCFORange)) && 
                 (psDemodCtx->s4Cfo <=   S2PartialLockCFORange)   )
    	{
    	    psDemodCtx->u1VIFLock = cATD_VIF_PARTIAL_LOCK;
        }
        else
        {
            psDemodCtx->u1VIFLock = cATD_VIF_UNLOCK;
        }
    }
    return (psDemodCtx->u1VIFLock);
}
#ifndef CC_MT5880_CODE_SIZE_CTRL
UINT8 ATD_GetSignalLevel(ATD_CTX_T *psDemodCtx)
{
	return 0;
}
#endif

UINT16 ATD_GetRFAGC(ATD_CTX_T *psDemodCtx)
{  	
    UINT8 ucData1 = 0, ucData2 = 0;
    UINT16 u2RFAGC;
    ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_20, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_21, &ucData2, 1);
    u2RFAGC = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    mcSHOW_DBG_MSG(("RF gain = 0x%04X\n", u2RFAGC));	
    return u2RFAGC;
}

UINT32 ATDPI_GetNoiseLevel(ATD_CTX_T *psDemodCtx)
{	
    UINT8 ucData1, ucData2, ucData3;
    UINT32 u4NoiseLevel;

    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_0, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_1, &ucData2, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_CVBS_SNR_2, &ucData3, 1);

    u4NoiseLevel = (UINT32) (ucData1 << 11);
    u4NoiseLevel += (UINT32) (ucData2 << 3);
    u4NoiseLevel += (UINT32) (ucData3 >> 5);
    	
    mcSHOW_DBG_MSG(("Get ATD noise level = 0x%08X\n", u4NoiseLevel));
    	
    return u4NoiseLevel;
}
UINT32 ATDPI_GetTVDNoiseLevel(ATD_CTX_T *psDemodCtx)
{	
	UINT8 ucData1;
	UINT32 u4NoiseLevel;
	ucData1= IO_READ8(VIDEO_IN0_BASE, 0x80);
	u4NoiseLevel = (UINT32)ucData1;
    mcSHOW_USER_MSG(("TVD noise level = %d\n", u4NoiseLevel));
    	
    return u4NoiseLevel;
}
UINT16 ATDPI_GetIFAGC(ATD_CTX_T *psDemodCtx)
{
    	
    UINT8 ucData1, ucData2;
    UINT16 u2IFAGC;
    if(psDemodCtx->pTCtx->fgRFTuner)
    {
        ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_30, &ucData1, 1);
        ucData2=0x00;	  
    }
    else
    {
        ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_22, &ucData1, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_23, &ucData2, 1);
    }
    u2IFAGC = mcUNSIGN_2BYTE(ucData1, ucData2, 8, 8);
    	
    mcSHOW_DBG_MSG(("IF gain = 0x%04X\n", u2IFAGC));
    	
    return u2IFAGC;
}
UINT8 ATD_GetPnFn(ATD_CTX_T *psDemodCtx)
{
    UINT8 u1CRSts1;
	
    ATD_GetReg(psDemodCtx,REG_ATD_CR_STATE1, &u1CRSts1, 1);
	return (u1CRSts1 &(MSK_ATD_CR_STATE1_PNIND|MSK_ATD_CR_STATE1_FNIND));  
}
UINT32 ATD_GetPilotPower(ATD_CTX_T * psDemodCtx)
{
    UINT32 u4PilotPower;
    UINT8 ucData,ucData3[3];

    ucData = (0x06 << 3); // Select ADVPS_PROB as PilotPower by setting 0x9BB[b5:b3] to 0x06
    ATD_SetReg(psDemodCtx, REG_ATD_ADVPS_CFG2 , &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_ADVPS_PRB0, ucData3, 3);    
    u4PilotPower = mcUNSIGN_2BYTE(ucData3[0], ucData3[1], 8, 8);
    u4PilotPower = (u4PilotPower << 8) + ucData3[2];
    mcSHOW_DBG_MSG(("Pilot Power=0x%06X\n",u4PilotPower));
    return u4PilotPower;
}    
UINT16 ATD_GetADCValue(ATD_CTX_T *psDemodCtx)
{
    UINT8  ucPRAProbe_Sel,ucADCValue[2];
    UINT16 u2ADCValue;
    ATD_GetReg(psDemodCtx,REG_ATD_SLD_PRA_35,&ucPRAProbe_Sel,1);
    ucPRAProbe_Sel &= 0xE0;
    ucPRAProbe_Sel |= 0x03;//d111230_hongji:Select PRA PROBE SELECT as ADC Value by setting 0xFE5[4:0] to 0x03
    ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_35,&ucPRAProbe_Sel,1);
    ATD_GetReg(psDemodCtx,REG_ATD_SLD_PRA_36,ucADCValue,2);
    u2ADCValue = mcUNSIGN_2BYTE(ucADCValue[0],ucADCValue[1],8,8);
    mcSHOW_DBG_MSG(("Current ADC Value is 0x%x \n",u2ADCValue));
    return u2ADCValue;
}
void ATD_SetNullRegion(ATD_CTX_T *psDemodCtx, UINT8 u1NR)
{
    UINT8 ucData;

    ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucData, 1);
    ucData&=0x8F;
    ucData|=(u1NR<<4);
    ATD_SetReg(psDemodCtx, REG_ATD_PE_CFG0, &ucData, 1);	 
    return;
}
void ATD_SetCrBwAutoCfg(ATD_CTX_T *psDemodCtx,BOOL fgEnable)
{
    UINT8 ucData;

    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
    ucData&=0xF7;
    ucData|=((FALSE == fgEnable)<<3);
    ATD_SetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
    return;
}
BOOL ATDPI_SetLNA(ATD_CTX_T *psDemodCtx, BOOL fgLNA)
{
    BOOL fgReturn;
    UINT8 ucData1;

    mcSHOW_DBG_MSG(("Set LNA = %d\n", fgLNA));

    fgReturn = ITuner_OP(psDemodCtx->pTCtx, itSetLNA, fgLNA, &ucData1);
    return fgReturn;
}

BOOL ATDPI_SetAntIn(ATD_CTX_T *psDemodCtx, UINT8 fgAntIn)
{
    BOOL fgReturn;
    UINT8 ucData1;

    mcSHOW_DBG_MSG(("Set LNA = %d\n", fgAntIn));

    fgReturn = ITuner_OP(psDemodCtx->pTCtx, itSetAntIn, fgAntIn, &ucData1);
    return fgReturn;
    
}

#ifdef CC_SUPPORT_ATV_DESCRAMBLE
static BOOL CheckIsChinaATVChannel(UINT32 u4Frequency)
{
	UINT32 u4Temp;
	UINT8 i,j,ucChannelCount;
	UINT8 fgIsChinaATVChannel = FALSE;
	for(i=0; i<4; i++)
	{
		u4Temp = u4aChinaATVChannels[i][0];
		ucChannelCount = u4aChinaATVChannels[i][1];

		if(u4Frequency < (u4Temp - CC_CN_ATV_FREQ_OFFSET))
		{
			break;
		}
		if(u4Frequency > (u4Temp + (ucChannelCount - 1) * 8000 + CC_CN_ATV_FREQ_OFFSET))
		{
			continue;	
		}
		for(j=0; j<ucChannelCount; j++)
		{
			if((u4Frequency >= (u4Temp + j*8000 - CC_CN_ATV_FREQ_OFFSET)) && (u4Frequency <= (u4Temp + j*8000 + CC_CN_ATV_FREQ_OFFSET)))
			{
				mcSHOW_DBG_MSG(("Got China ATV channel at %d + Channel %d!\n", u4Temp, j));
				fgIsChinaATVChannel = TRUE;
				break;
			}
		}
		if(fgIsChinaATVChannel)
		{
			break;
		}
	}
	return fgIsChinaATVChannel;
}
#endif

BOOL fgGetTVDSts(ATD_CTX_T *psDemodCtx,UINT16 u1ChkTime)
{
    ITUNER_CTX_T * pTCtx=psDemodCtx->pTCtx;
    
#ifdef CC_SUPPORT_ATV_DESCRAMBLE
    if(psDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn) //descramble on
    {
        if(CheckIsChinaATVChannel(psDemodCtx->u4Frequency))
        {
            mcSHOW_DBG_MSG(("Descramble on, and VIF lock at channel frequency!Return true directly!\n"));
            return TRUE;
        }
    }
#endif
    if(!fgDrvTvdCheckTVDLock(u1ChkTime))//TVD unlock first time
    {
        mcSHOW_DBG_MSG(("[ATuner]TVD can not lock at 1st time\n"));
        if(0 == (pTCtx->u2RetryTVDTm))
        {
            mcSHOW_DBG_MSG(("[ATuner]not re-check TVD more(Delay time is 0)\n"));
            return FALSE;
        }

            ATD_SetTVDReChkCfg(psDemodCtx);//d111107_Haibo:Apply TVD re-check setting
  
        mcDELAY_MS(pTCtx->u2RetryTVDTm);
        mcSHOW_DBG_MSG(("[ATuner]2nd check TVD after %d ms delay\n", pTCtx->u2RetryTVDTm));	
        return fgDrvTvdCheckTVDLock(u1ChkTime);
    }
    else//First time lock
    {
        mcSHOW_DBG_MSG(("[ATuner]TVD lock at 1st time\n"));
        return TRUE;
    }   
}

void SetFMRadio(ATD_CTX_T *psDemodCtx, UINT32  i4CurFreq)
{
    UINT8 ucData1;    
    psDemodCtx->u1PF = 0x00;
    ucData1 = 0x6D;
    ATD_SetReg(psDemodCtx, REG_ATD_PJ_CFG, &ucData1, 1);
    ucData1 = 0xC9;
    ATD_SetReg(psDemodCtx, REG_ATD_PFLL_CFG, &ucData1, 1);
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) //d20120114_hongji:MT5398 New feature
    ATD_GetReg(psDemodCtx, REG_ATD_CPR_CFO_CFG, &ucData1, 1);
    ucData1 &=0xFE;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_CFO_CFG, &ucData1, 1);
	
	ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_4, &ucData1, 1);    //FLL trk off
	ucData1 &=0xF8;
	ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_4, &ucData1, 1);
	ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &ucData1, 1);   //PLL ki off
	ucData1 &=0x0F;
	ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &ucData1, 1);
	ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12, &ucData1, 1);    //PLL kp off 
	ucData1 &=0x0F;
	ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_12, &ucData1, 1);
	ATD_GetReg(psDemodCtx, REG_ATD_NCR_FLL_1, &ucData1, 1);    //FLL acq off
	ucData1 &=0xE3;
	ATD_SetReg(psDemodCtx, REG_ATD_NCR_FLL_1, &ucData1, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_23, &ucData1, 1);    //FPC off
	ucData1 &=0xFE;
	ATD_SetReg(psDemodCtx, REG_ATD_NCR_ADV_23, &ucData1, 1);
		
	ATD_GetReg(psDemodCtx, REG_ATD_NCR_PLL_1, &ucData1, 1);    //use_bn_protection disable
	ucData1 &=0xFB;                                            //for noticeable white noise on FM channel Issue
	ATD_SetReg(psDemodCtx, REG_ATD_NCR_PLL_1, &ucData1, 1);
	
#else
    ucData1 = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_CFO_CFG, &ucData1, 1);
#endif
    ucData1 = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData1, 1);
    ucData1 = 0x20;
    ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData1, 1);
    ucData1 = 0x81;
    ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData1, 1);
    ucData1 = 0xA1;
    ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData1, 1);

#if ATD_SUPPORT_CHF
    //LW_code_d100929; FM issuep;START
    ATD_GetReg(psDemodCtx, REG_ATD_SLD_DC_CFG3, &ucData1, 1);
    mcSHOW_DBG_MSG(("LW:beforeREG_ATD_ATD_DC_CFG3_ucData1      =  %d\n", ucData1));
    ucData1 += cIfCtrlWordOffset;
    mcSHOW_DBG_MSG(("LW:after REG_ATD_ATD_DC_CFG3_ucData1      =  %d\n", ucData1));
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_DC_CFG3, &ucData1, 1);
#else
    ATD_GetReg(psDemodCtx, REG_ATD_ATD_DC_CFG3, &ucData1, 1);
    mcSHOW_DBG_MSG(("LW:beforeREG_ATD_ATD_DC_CFG3_ucData1      =  %d\n", ucData1));
    ucData1 += cIfCtrlWordOffset;
    mcSHOW_DBG_MSG(("LW:after REG_ATD_ATD_DC_CFG3_ucData1      =  %d\n", ucData1));
    ATD_SetReg(psDemodCtx, REG_ATD_ATD_DC_CFG3, &ucData1, 1);
#endif
    ucData1 = 0x81;
    ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData1, 1);
    ucData1 = 0x83;
    ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData1, 1);   
    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG6, &ucData1, 1);	//NSF Enable
    ucData1 &= 0x8F;
    ucData1 |= 0x70;
    ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG6, &ucData1, 1);

    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG5, &ucData1, 1);	//ACF disable
    ucData1 &= 0x80;
    ucData1 |= 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG5, &ucData1, 1);

    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG2, &ucData1, 1);	//NSF centeral frequency ~ 5.3MHz
    ucData1 &= 0xE0;
    ucData1 |= 0x09;
    ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG2, &ucData1, 1);

    ATD_GetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData1, 1);
    ucData1 &= 0x0F;
    ucData1 |= 0x40;
    ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData1, 1);    //AVS BW
    //LW_code_d100929; FM issue; END
	ucData1 = 0x06;
	ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_02, &ucData1, 1);	

    ITuner_OP(psDemodCtx->pTCtx, itSetLNA, 0, &ucData1);
    mcSHOW_DBG_MSG((" Enable ATD CR bypass mode for FM radio\n")); 
    return;
}
#ifdef CC_SUPPORT_STR
void ATD_pm_str_suspend(void)
{
#if defined(CC_MT5398)||defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)  //d20120629_hongji:only for cobra/viper/python
    //ATD reset 0xF000D1C4[10]:0 reset,1 not reset.
    IO_WRITE32MSK(0xF000D000, 0x1c4, 0x00000000, 0x00000400); //0xD1C4[10]=0 
    mcSHOW_DBG_MSG(("Reset ATD(digital part) for power suspend \n"));
    //ATD clock 0xF000D1CC[10]:0 off,1 on.
    IO_WRITE32MSK(0xF000D000, 0x1cc, 0x00000000, 0x00000400); //0xD1CC[10]=0 
    mcSHOW_DBG_MSG(("off ATD clock(digital part) for power suspend \n"));
#if  defined(CC_MT5398)
    //d20120731_hongji:Power down adc
    //RG_ADC_PWD_DEMOD & RG_HPF_PWD_DEMOD
    IO_WRITE32MSK(0xF0061000, 0x400, 0x00000003, 0x00000003);//0xf0061400[1:0]=11'b
    //RG_IFTESTBUF_PWD
    IO_WRITE32MSK(0xF0061000, 0x504, 0x00080000, 0x00080000);//0xf0061504[19]=1'b
    //d20120731_hongji:Power down pga
    //RG_PGAVOCM_PWD & RG_PGA_PWD
    IO_WRITE32MSK(0xF0061000, 0x500, 0x00000006, 0x00000006);//0xf0061500[2:1]=11'b
#elif defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    //d20120731_hongji:Power down adc
    //RG_ADC_PWD_DEMOD & RG_HPF_PWD_DEMOD
    IO_WRITE32MSK(0xF0060000, 0x500, 0x00000003, 0x00000003);//0xf0060500[1:0]=11'b
    //RG_IFTESTBUF_PWD
    IO_WRITE32MSK(0xF0060000, 0x604, 0x00080000, 0x00080000);//0xf0060604[19]=1'b
    //d20120731_hongji:Power down pga
    //RG_PGAVOCM_PWD & RG_PGA_PWD
    IO_WRITE32MSK(0xF0060000, 0x600, 0x00000006, 0x00000006);//0xf0060600[2:1]=11'b
#else //d20121024_hongji:waiting for MT5881 setting update 
    
#endif
#endif    
    return;
}
void ATD_pm_str_resume(void)
{
#if defined(CC_MT5398)||defined(CC_MT5880)||defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)//d20120629_hongji:only for cobra/viper/python
    ATD_CTX_T *psDemodCtx;  
#if defined(DTD_INCLUDE)
#if defined(CC_SUPPORT_EU_US_COMPATIABLE)
		if(DynamicSwitch_DTD_DVBT_Mode == 0)
			{
			psDemodCtx = psAtdnaDemodCtx;
    		IO_WRITE32(0xF002A000, 0x4B4, 0x000F0000);       
    		IO_WRITE32(0xF002A000, 0xE00, 0x000010c2);//lei_code_dB110810 for CAM 10.7M clock issue.
			}
		else
			{			
			psDemodCtx = psAtdeuDemodCtx;
			}
#else
    psDemodCtx = psAtdnaDemodCtx;
    IO_WRITE32(0xF002A000, 0x4B4, 0x000F0000);       
    IO_WRITE32(0xF002A000, 0xE00, 0x000010c2);//lei_code_dB110810 for CAM 10.7M clock issue.
#endif
#else
    psDemodCtx = psAtdeuDemodCtx;
#endif
    //ATD clock 0xF000D1CC[10]:0 off,1 on.
    IO_WRITE32MSK(0xF000D000, 0x1cc, 0x00000400, 0x00000400); //0xD1CC[10]=1 
    mcSHOW_DBG_MSG(("ON ATD clock for power resume \n"));
    //ATD reset 0xF000D1C4[10]:0 reset,1 not reset.
    IO_WRITE32MSK(0xF000D000, 0x1c4, 0x00000400, 0x00000400); //0xD1C4[10]=1 
    mcSHOW_DBG_MSG(("Not reset ATD for power resume \n"));
    ATD_ChipInit(psDemodCtx);   // make sure ATD in normal mode after nim.go dvbt
    ATD_SetSystem(psDemodCtx, psDemodCtx->u1SubSysIdx);
    ATD_StartAcq(psDemodCtx, FALSE);
#endif    
    return;    
}
#endif

#ifdef CC_POWER_MERGE_TEST
static HANDLE_T t_TunerAGCThread = NULL;
static void TunerAGCPowerMergeProc(void *pvArg)
{
    UINT8 ucData,ucInitial_PRA_2F;
    ATD_CTX_T *psDemodCtx = (ATD_CTX_T*)pvArg;
    //Fix rfagc&ifagc
    ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_2F, &ucData, 1);
    ucInitial_PRA_2F = ucData; //Back up PRA AGC setting
    ucData |= 0x0c;  //Fix rfagc & ifagc setting
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_2F, &ucData, 1);

    //TunerAGC power merge thread loop
    while(fgTunerAGCThreadRunning)
    {
        mcDELAY_MS(g_u2PowerMerge_Delay);
        //rfagc to min and ifagc to max
        ucData = 0x80;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_20, &ucData, 1);
        ucData = 0x00;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_21, &ucData, 1);
        ucData = 0x7F;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_22, &ucData, 1);
        ucData = 0xF8;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_23, &ucData, 1);
        mcDELAY_MS(g_u2PowerMerge_Delay);
        //rfagc to max and ifagc to min
        ucData = 0x7F;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_20, &ucData, 1);
        ucData = 0xF8;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_21, &ucData, 1);
        ucData = 0x80;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_22, &ucData, 1);
        ucData = 0x00;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_23, &ucData, 1);
    }
        
    mcSHOW_USER_MSG(("TunerAGCPowerMergeProc exit!\n"));
    //roll back original setting since thread die
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_2F, &ucInitial_PRA_2F, 1);
    mcTHREAD_DESTROY();
    t_TunerAGCThread = (THREAD_T)NULL;

    return;
}
#endif

VOID ATD_TunerAtdCommand(ATD_CTX_T *psDemodCtx, INT32 i4AtdArgc, const CHAR** aszAtdArgv,UINT8 (*ATD_SearchNearbyFreq)(ATD_CTX_T *, UINT32 *, x_break_fct ,void *, UINT32 ))
{
    UINT32  i4CurFreq=0, i4BndFreq=0;
    UINT32  u4Freq = 0;
	S32 	s2Data;
    U32 	u2Data;
    U16 	u2RegAddr, u2Period;
	UINT16	u2State;
    UINT8   u1SubSysIdx, ii, jj, kk;
	UCHAR   ucCmdId = 0, ucPar, ucData[256];
    UINT8   ucData1 = 0;
    UINT8   ucData2 = 0;
    UINT8	u1Count;
    UINT8   i4CurTop =0;
    UINT8 temp[2];
    UINT8 RegistWord[5];
    UINT8 i1CurCabIn;
#if ATD_SUPPORT_CHF
    UINT8 u1CHFBw;
#endif	
#if ATD_SUPPORT_NCR
    UINT8 u1PilotStr,u1InchannelStr;
#endif
    INT8 i1Top;
	UINT8 u1IFAmp;
    if(FALSE == fg_Acq_flag)
    {
        DigTunerBypassI2C (TRUE);
    }
//UINT8 *SearchNearBY(ATD_CTX_T *,UINT32 *,x_break_fct, VOID *, UINT32);    
    if (i4AtdArgc > 0)
    {
        ucCmdId = *((CHAR *) aszAtdArgv[0]);

        switch (ucCmdId)
        {
			case 'b':
				{
				 if (i4AtdArgc > 1)
                {
                //    _SubSysIdx = StrToInt(aszAtdArgv[1]);
                    u1SubSysIdx = StrToInt(aszAtdArgv[1]);
					ATD_BypassSIF2Aud(psDemodCtx,u1SubSysIdx);
				}
				else
				{
				    mcSHOW_USER_MSG(("\t %d: PAL_BG  \n", MOD_PAL_BG));
                    mcSHOW_USER_MSG(("\t %d: PAL_DK  \n", MOD_PAL_DK));
                    mcSHOW_USER_MSG(("\t %d: PAL_I   \n", MOD_PAL_I));
                    mcSHOW_USER_MSG(("\t %d: SECAM_L \n", MOD_SECAM_L));
                    mcSHOW_USER_MSG(("\t %d: SECAM_L1\n", MOD_SECAM_L1));
                    mcSHOW_USER_MSG(("\t %d: NTSC_M  \n", MOD_NTSC_M));
					mcSHOW_USER_MSG(("\t %d: Unkown  \n", MOD_TYPE_UNKNOWN));
				}
				}
				break;
			
			case 'c':
                i4CurFreq = psDemodCtx->u4Frequency;
//				i4CurFreq = psAtdPdCtx->Frequency;    // 120109;ORG
                mcSHOW_USER_MSG(("Current Freq = %d kHz\n", i4CurFreq));
                break;
			

			case 'e':
			{                 
                if (2 == i4AtdArgc)
			    {                      
				    ii = 0;
				    jj = 0;
				    kk = 0;
				    while ( (((CHAR *) aszAtdArgv[1])[ii] != '\0') &&( kk < 5))
                   	{
                        while((((CHAR *) aszAtdArgv[1])[ii] != '-') && (((CHAR *) aszAtdArgv[1])[ii] != '\0')&& (jj < 2))
                        { 
                            temp[jj] = ((CHAR *) aszAtdArgv[1])[ii];
					        ii++;
						    jj++;
						   
                     	}
                        if(jj > 2)
						{
                            mcSHOW_USER_MSG(("error!One word's format is ## \n")); 
							break;
						}
					
                        if (kk >= 5)
                        {
						    mcSHOW_USER_MSG(("error! the format is ##-##-##-##-##,just 5 words \n")); 
							break;
						} 
						RegistWord[kk] = (U16)StrToHex((CHAR *)&temp,jj);
						mcSHOW_USER_MSG(("the input word is 0x%x\n",RegistWord[kk])); 
						kk++;
					    jj = 0;
						ii++;
					}
                    if ((jj <= 2) &&( kk <= 5))
                    {
				        ITuner_OP(psDemodCtx->pTCtx, itSetReg,0, &RegistWord); 
			            mcSHOW_USER_MSG(("Set Register complete!\n"));
                    }
                }				
                else if (1 == i4AtdArgc)
                {     
                   	for (ii = 0; ii < 5; ii++)
					  	RegistWord[ii] = 0;
					  	
                    ITuner_OP(psDemodCtx->pTCtx, itGetReg,0, RegistWord); 
                    
                    for (ii = 0; ii < 5; ii++)
						mcSHOW_USER_MSG(("the output word is 0x%x\n",RegistWord[ii])); 	
				}                   
			    else
			    {
			  	    mcSHOW_USER_MSG(("The CLI input formats are: nim.atd e or nim.atd e ##-##-##-##-## \n"));
			    }			
			}
                break;
			
			case 'f':
                i4CurFreq = psDemodCtx->u4Frequency;
//				i4CurFreq = psAtdPdCtx->Frequency;    //120109;ORG
                if (i4AtdArgc > 1)
                {
                    i4CurFreq = StrToInt(aszAtdArgv[1]);
                    psDemodCtx->u4Frequency = i4CurFreq;
//					psAtdPdCtx->Frequency = i4CurFreq;    //120109;ORG
                }
				if(cCTMR_TUNER!=psDemodCtx->u1TunerType)
				{ 
				ATD_ChipInit(psDemodCtx);
#if defined(CC_MT5890)
					if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx, itSetRSetting, psDemodCtx->u1SubSysIdx, &u2Data))
					{
					UINT32 RSetting;
					RSetting = ((UINT32)u2Data)<<16;
					mcSHOW_DBG_MSG(("ATD R_Setting Vaule=%d\n",u2Data));
					IO_WRITE32MSK(0xF0061000, 0x418, RSetting, 0x00070000); //RSetting:bit[18:16],RG_DMAD_GAIN=001:
					}
#endif
                 ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);
				 ATD_StartAcq(psDemodCtx, FALSE);
                //AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtdPdCtx), i4CurFreq, ATV_CONN_MODE_CHAN_CHANGE);
                mcSHOW_USER_MSG(("Set Frequency = %d KHz 2\n", i4CurFreq));
				}
				else
				{
    			 ATD_ChipInit(psDemodCtx);
#if defined(CC_MT5890) 
				if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx, itSetRSetting, psDemodCtx->u1SubSysIdx, &u2Data))
				{
				UINT32 RSetting;
				RSetting = ((UINT32)u2Data)<<16;
				mcSHOW_DBG_MSG(("ATD R_Setting Vaule=%d\n",u2Data));
				IO_WRITE32MSK(0xF0061000, 0x418, RSetting, 0x00070000); //RSetting:bit[18:16],RG_DMAD_GAIN=001:
				}
#endif
				 ATD_StartAcq(psDemodCtx, FALSE);
				}
                break;                        
			case 'g':
                if (1 == i4AtdArgc)
                {
                    u1Count = 1;
                    u2Period = 0;     	
                }
                else if (2 == i4AtdArgc)
                {
                    u1Count = StrToInt(aszAtdArgv[1]);
                    u2Period = 1000;                     
                }
                else if (3 == i4AtdArgc)
                {
                    u1Count = StrToInt(aszAtdArgv[1]);
                    u2Period = StrToInt(aszAtdArgv[2]);                       
                }
                else
                {
                    break;
                }
                
                for (ii=0; ii<u1Count; ii++)
                {
                    ATD_GetInfo(psDemodCtx);                           
                    if( ii < u1Count-1 )
                    {
                        mcDELAY_MS(u2Period);
                    } 
                }										   
                break;

			case 'h':
                jj = 0;
                kk = 0;
                i4CurFreq = psDemodCtx->u4Frequency;
//				i4CurFreq = psAtdPdCtx->Frequency;    //120109;ORG

                if (i4AtdArgc > 1)
                {
                    i4CurFreq = StrToInt(aszAtdArgv[1]);
                    i4BndFreq = StrToInt(aszAtdArgv[2]);
                    u1Count = StrToInt(aszAtdArgv[3]);
                    ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);                   
                }
                else
                {
                    i4BndFreq = i4CurFreq;
                    u1Count = 1;
                }
                ATD_StartAcq(psDemodCtx, TRUE);
                for(ii=0; ii<u1Count; ii++)
                {
                    u4Freq = i4CurFreq;
                    mcSHOW_USER_MSG(("the %d th time SearchNearFreq.\n",ii));
                    psDemodCtx->fgCheckCR = TRUE;
                    if((*ATD_SearchNearbyFreq)(psDemodCtx, &u4Freq, TunerNullBreak, NULL, i4BndFreq))
                    {
                        if (fgGetTVDSts(psDemodCtx,CHN_LOCK_CHECK_TIME)) 
                        {
                            jj++;  //VIF lock, TVD lock
                        }
                        else
                        {
                            kk++;  //VIF lock, but TVD unlock
                        }
                    }                        

                    if(ii <(u1Count -1))
                    {
                        ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);
						psDemodCtx->u1PF = 0x01;
                        ATD_StartAcq(psDemodCtx, TRUE); 
                    }     
                
                }
                mcSHOW_USER_MSG(("Search NearBy Before %d After %d KHz\n", i4CurFreq, u4Freq));
                mcSHOW_USER_MSG(("Search NearBy %d times from %d KHz to %d KHz\n", u1Count, i4CurFreq, i4BndFreq));
                mcSHOW_USER_MSG(("VIF lock times = %d, TVD lock times = %d\n", jj+kk, jj));                                
                psDemodCtx->u4Frequency = u4Freq;
//				psAtdPdCtx->Frequency = u4Freq;    //120109;ORG
                break; 
			
			case 'i': //For now, only BA/COL will use Dual-RF input tuner, so use itSetInCableMode for tuner input Cli
            {
				i4CurFreq = psDemodCtx->u4Frequency;
//				i4CurFreq = psAtdPdCtx->Frequency;    //120109;ORG
                if(i4AtdArgc > 1)
                {
    			    i1CurCabIn= (U8)StrToInt((CHAR *) aszAtdArgv[1]);
    			    
    				ITuner_OP(psDemodCtx->pTCtx, itSetInCableMode, i1CurCabIn, &ucData1); 
    				ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);
    				
    				mcSHOW_USER_MSG(("Set tuner in %d [0 for Ant, 1 for Cable] \n", i1CurCabIn));
                }
                else
                {
                    mcSHOW_USER_MSG(("Please set the tuner in [0 for Ant, 1 for Cable] \n"));
                }
			}
            break;

#if defined(CC_MT5389) 
            case 'k':			
			{					
				mcSHOW_USER_MSG(("aszAtdArgv[1] = %d\n",StrToInt(aszAtdArgv[1]) ));
				mcSHOW_USER_MSG(("aszAtdArgv[2] = %d\n",StrToInt(aszAtdArgv[2]) ));
				mcSHOW_USER_MSG(("aszAtdArgv[3] = %d\n",StrToInt(aszAtdArgv[3]) ));
				mcSHOW_USER_MSG(("aszAtdArgv[4] = %d\n",StrToInt(aszAtdArgv[4]) ));
			    if ((StrToInt(aszAtdArgv[1])<=1) || (StrToInt(aszAtdArgv[2])<= 2)||(StrToInt(aszAtdArgv[3]) <= 0)||(StrToInt(aszAtdArgv[4]) > 0))
				{   
				    mcSHOW_USER_MSG(("Adjust the DRO target level adaptivily\n")); 
			        ATD_DroTargetLevelModifySyncH(psDemodCtx, StrToInt(aszAtdArgv[1]), StrToInt(aszAtdArgv[2]), StrToInt(aszAtdArgv[3]), StrToInt(aszAtdArgv[4]), u2AtdDroTarLvlInitial);	
		    		//StrToInt(aszAtdArgv[1]:fgsel=1:DRO target; else==>Fref2
			    	//StrToInt(aszAtdArgv[2]:ACC_MON_Count=0==>1, 1==>2, 2==>4; else==>0
    				//StrToInt(aszAtdArgv[3]:Reg_autoSet=1==>set;else==>non-set				                
	    			////StrToInt(aszAtdArgv[4]: target sync height (cal value)
				    
				}
			    else
				{
                    mcSHOW_USER_MSG(("\t nim.atd k X1 X2 X3 X4!! \n"));   
    				mcSHOW_USER_MSG(("\t  X1: 1/0==>Using the DRO_target_level/ FREF2!! \n"));
	    			mcSHOW_USER_MSG(("\t  X2: ACC-avg:0/1/2==>1/2/4 times!! \n"));
		    		mcSHOW_USER_MSG(("\t  X3: Autocgahge the DRO targer level:0/1==>En/Disable!! \n"));
			    	mcSHOW_USER_MSG(("\t  X4: fine-tune the target weight-syncH(Suggest value is 3400)!! \n"));
				}
			}
			break;
#endif	

			case 'l':
                i4CurFreq = psDemodCtx->u4Frequency;
//				i4CurFreq = psAtdPdCtx->Frequency;    //120109
                
                if(1 == i4AtdArgc)                
                {                    
                    ITuner_OP(psDemodCtx->pTCtx, itGetLNA, 0, &ucData1);                    
                    
                    mcSHOW_USER_MSG(("LNA status = %d [0 for off, 1 for On]\n", ucData1));                
                }                
                else if (i4AtdArgc > 1)
                {                
                    ATDPI_SetLNA(psDemodCtx, StrToInt(aszAtdArgv[1]));
                    mcSHOW_USER_MSG(("Set LNA = %d [0 for off, 1 for on]\n", StrToInt(aszAtdArgv[1])));
                }
                break; 
			case 'o':
                    mcSHOW_USER_MSG(("Turn on the CVBS Out function\n"));
#if defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) //d20120204_hongji:for Cobra VDAC output enable
#if defined(CC_MT5399) || defined(CC_MT5882)
                    IO_WRITE32(0xF0061000,0x300, 0x6ca00000);
#elif defined(CC_MT5890)
                    IO_WRITE32(0xF0061000,0x300, 0x66500000);
#else
                    IO_WRITE32(0xF0061000,0x300, 0x3c400000);
#endif
                    IO_WRITE32(0xF002d000,0x604, 0x00000000);
                    IO_WRITE32(0xF002d000,0x634, 0x000000ff); 
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)                    
                    IO_WRITE32(0xF000d000,0x10c, 0x00605000);
#else
                    IO_WRITE32(0xF000d000,0x10c, 0x06602800);
#endif
                    IO_WRITE32(0xF002d000,0x678, 0x000d0000);
#elif defined(CC_MT5881)    
                    IO_WRITE32(0xF0060000,0x400, 0x6ca00000);
                    IO_WRITE32(0xF002d000,0x604, 0x00000000);
                    IO_WRITE32(0xF002d000,0x634, 0x000000ff);                    
                    IO_WRITE32(0xF000d000,0x10c, 0x00605000);
                    IO_WRITE32(0xF002d000,0x678, 0x000d0000);                    
                    IO_WRITE32MSK(0xF0028000, 0x914, 0x00000000, 0x00000004); //0xf0028914[2]=0:VGA bandgap power on for VDAC
#else
                    {
					UINT32 RegData;
					RegData= IO_READ32(CKGEN_BASE,0x2a8);
                    RegData &=0xFF01FFFF;//Bit17~23 =0
                    RegData |=0x00400000;//Bit22 =1
                    IO_WRITE32(CKGEN_BASE,0x2a8, RegData);
                
                    RegData= IO_READ32(TV_EOCODER_BASE,0x634); 
                    RegData &=0xFF00FF00;//Bit0~7,16~23 =0
                    RegData |=0x000000FF;//Bit0~7 FF
                    IO_WRITE32(TV_EOCODER_BASE,0x634, RegData);
                    
                    IO_WRITE32(TV_EOCODER_BASE,0x684, 0x00060600);
                    
                    IO_WRITE32(TV_EOCODER_BASE,0x604, 0x08010280);
                    
                    IO_WRITE32(TV_EOCODER_BASE,0x67c, 0x00050424);
                    
                    RegData= IO_READ32(TV_EOCODER_BASE,0x678); 
                    RegData |=0x000D0001;//Bit0,16,18,19 1
                    IO_WRITE32(TV_EOCODER_BASE,0x678, RegData);
					}
#endif
#if defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
				if (i4AtdArgc > 1)
					{ 
                    if(StrToInt(aszAtdArgv[1]))//flag for DEMOD_VDAC_CLOCK setting[54M/108M]
                    {
#if  defined(CC_MT5881) 
                    IO_WRITE32(0xf000d000,0x428, 0x00000001); //demod_vdac_ck 108M for Remy
#elif defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	                IO_WRITE32(0xf000d000,0x42C, 0x00000001); //demod_vdac_ck 108M for Remy
#endif
                    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG6, &ucData1, 1);
					ucData1 |= 0x01;
					ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG6, &ucData1, 1);//0x1e2[0]=1---CVBS DAC clock mode :108MHz
                    }
                    else
                    {                 
#if  defined(CC_MT5881) 
		            IO_WRITE32(0xf000d000,0x428, 0x00000002); //demod_vdac_ck 54M for Remy
#elif defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
		            IO_WRITE32(0xf000d000,0x42C, 0x00000002); //demod_vdac_ck 54M for mustang
#endif
                    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG6, &ucData1, 1);
					ucData1 &= 0xfe;
					ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG6, &ucData1, 1);//0x1e2[0]=0---CVBS DAC clock mode :54MHz
                    }
					}
				else
					{
					mcSHOW_USER_MSG(("0: set demod_vdac_ck  54M \n"));
					mcSHOW_USER_MSG(("1: set demod_vdac_ck 108M \n"));
					}
				    //d20140106_steven:show current VDAC clock setting[54M/108M]
				    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG6, &ucData1, 1);
					ucData1 &= 0x01;//0x1e2[0]_CVBS DAC clock mode 
					if(ucData1)
					{
					mcSHOW_USER_MSG(("current demod_vdac_ck setting %d: 108M \n",ucData1));
					}
					else
					{
                    mcSHOW_USER_MSG(("current demod_vdac_ck setting %d:  54M \n",ucData1));
					}
#endif
					break;

			case 'p':			
				if('a'==((CHAR *) aszAtdArgv[0])[1])
               {
				 if (i4AtdArgc > 1)
				 {
				   u1IFAmp=(UINT8)StrToInt(aszAtdArgv[1]);

				   if(ITUNER_OK==ITuner_OP(psDemodCtx->pTCtx, itSetIFAMP, u1IFAmp, NULL));
				   {
				      mcSHOW_DBG_MSG(("IF AMP is %d\n",u1IFAmp));
				   }
				 }
               }
                else if('d'==((CHAR *) aszAtdArgv[0])[1])
                {
                    if (i4AtdArgc > 1)
                    {
                        psDemodCtx->pTCtx->u2RetryTVDTm = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("Re-try TVD delay = %d ms\n",psDemodCtx->pTCtx->u2RetryTVDTm)); 
                }
                else if('e'==((CHAR *) aszAtdArgv[0])[1])
                {
                    if (i4AtdArgc > 1)
                    {
                        g_CPR_cnt = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("reset g_CPR_cnt = %d\n",g_CPR_cnt)); 
                }
                else if('g'==((CHAR *) aszAtdArgv[0])[1])
                {
                    if (i4AtdArgc > 1)
                    {
                        g_pra_cnt = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("reset g_pra_cnt = %d \n",g_pra_cnt)); 
                }
                else if('k'==((CHAR *) aszAtdArgv[0])[1])
                {
                    if (i4AtdArgc > 1)
                    {
                        i4CurFreq = StrToInt(aszAtdArgv[1]);
                        ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);
                        mcSHOW_USER_MSG(("Fine Tuner Mode:Set Tuner Freq to %d,Don't StartAcq \n",i4CurFreq)); 
                    }
                    else
                    {
                        SetFMRadio(psDemodCtx, i4CurFreq);
                        ucData1 = 0x00;
                        ATD_SetReg(psDemodCtx,REG_ATD_CTL_RST1,&ucData1,1);
                        ATD_SetReg(psDemodCtx,REG_ATD_CTL_RST2,&ucData1,1);
                        mcDELAY(100);
                        ucData1 = 0xFF;
                        ATD_SetReg(psDemodCtx,REG_ATD_CTL_RST1,&ucData1,1);
                        ATD_SetReg(psDemodCtx,REG_ATD_CTL_RST2,&ucData1,1);
                        mcSHOW_USER_MSG(("FM Radio Setting \n")); 
                    }
                }
				#if (ATD_SUPPORT_VSI|ATD_SUPPORT_H_SYNDROME)
                else if('q'==((CHAR *) aszAtdArgv[0])[1])
                {
                    if (i4AtdArgc > 1)
                    {
                        fgVSI_On = StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("Current fgVSI_On is = %d \n",fgVSI_On)); 
                }
				#endif
                else if('f'==((CHAR *) aszAtdArgv[0])[1])
                {
                    if (i4AtdArgc > 1)
                    {
                        g_CPR_delay = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("reset g_CPR_delay = %d\n",g_CPR_delay)); 
                }
                else if('h'==((CHAR *) aszAtdArgv[0])[1])
                {
                    if (i4AtdArgc > 1)
                    {
                        g_CR_BW_AutoConfig_Delay_cnt = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("reset g_CR_BW_AutoConfig_Delay_cnt = %d\n",g_CR_BW_AutoConfig_Delay_cnt)); 
                }
#if ATD_SUPPORT_NCR                
                else if('i'==((CHAR *) aszAtdArgv[0])[1]) //d20120206_hongji:Enable CR BW SW auto config
                {
                    if (i4AtdArgc > 1)
                    {
                        fg_Enable_CR_BW_Auto = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("reset fg_Enable_CR_BW_Auto = %d\n",fg_Enable_CR_BW_Auto)); 
                }  
                else if('b'==((CHAR *) aszAtdArgv[0])[1]) //d20120206_hongji:Enable BN Patch
                {
                    if (i4AtdArgc > 1)
                    {
                        fg_Enable_BN = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    mcSHOW_USER_MSG(("reset fg_Enable_BN = %d\n",fg_Enable_BN)); 
                }                 
#endif                
				else if('m'==((CHAR *) aszAtdArgv[0])[1])
				{
				 if (i4AtdArgc > 1)
                    {
                        psDemodCtx->pTCtx->u1AtdPatchSwitch = (UINT8)StrToInt(aszAtdArgv[1]);
                    }
				else	
					{
					mcSHOW_USER_MSG(("patch for CR:0x%02X\n",MASK_PATCH_CR));
					mcSHOW_USER_MSG(("patch for DRO:0x%02X\n",MASK_PATCH_DRO));
					mcSHOW_USER_MSG(("patch for CCI:0x%02X\n",MASK_PATCH_CCI));
					mcSHOW_USER_MSG(("patch for Sensitivity:0x%02X\n",MASK_PATCH_SENS));
					mcSHOW_USER_MSG(("patch for StrongChroma:0x%02X\n",MASK_PATCH_STCHR));
					mcSHOW_USER_MSG(("patch for SoundBeat:0x%02X\n",MASK_PATCH_SBEAT));
                   	}
                    mcSHOW_USER_MSG(("Current Patch mask = 0x%02X \n",psDemodCtx->pTCtx->u1AtdPatchSwitch));  
				}
#if ATD_SUPPORT_NCR                
                else if('s'==((CHAR *) aszAtdArgv[0])[1]) //d20120203_hongji:CR Select
                {
                    if (i4AtdArgc > 1)
                    {
                        fg_NCR_Sel = (UINT16)StrToInt(aszAtdArgv[1]);
                    }
                    ATD_GetReg(psDemodCtx, REG_ATD_CR_NCR, &ucData1, 1);
                    if(TRUE == fg_NCR_Sel)  //d20120116_hongji:NCR Setting
                    {
                        ucData1 |= 0x01;
                        ATD_SetReg(psDemodCtx, REG_ATD_CR_NCR, &ucData1, 1);
                    }
                    else  //d20120116_hongji:OCR Setting
                    {
                        ucData1 &= 0xFE;
                        ATD_SetReg(psDemodCtx, REG_ATD_CR_NCR, &ucData1, 1);
                    }
                    mcSHOW_USER_MSG(("fg_NCR_Sel = %d  0:OCR   1:NCR \n",fg_NCR_Sel)); 
                }
                else if('n'==((CHAR *) aszAtdArgv[0])[1]) //d20120205_hongji:Show NCR Related message to fine tune and test
                {
                    mcSHOW_USER_MSG(("LOG NCR Message as following \n")); 
                    for(ii=0x00;ii<0xFF;ii++)
                    {
                        //Pilot Strength
                        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_11,&u1PilotStr, 1);
                        u1PilotStr = u1PilotStr*100/255;
                        //In Channel Strength
                        ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_12,&u1InchannelStr, 1);
                        u1InchannelStr = u1InchannelStr*100/255;
                        s2Data = ATD_GetCFOKHz(psDemodCtx, FALSE);
                        mcSHOW_USER_MSG(("Ps =%3d/100  Is =%3d/100  CFO =%4d KHz\n",u1PilotStr,u1InchannelStr,s2Data)); 
                        mcDELAY_US(100);  //100us sample rate for NCR information
                    }
                }
#endif                
				#if ATD_SUPPORT_CHF
				else if('c'==((CHAR *) aszAtdArgv[0])[1])//d120204_Haibo:Set channel filter bw
				{
				 if (i4AtdArgc > 1)
                    {
                        u1CHFBw = (UINT8)StrToInt(aszAtdArgv[1]);
                        mcSHOW_USER_MSG(("Current CHF Bw is %d MHz \n",u1CHFBw));
						ATD_SetCHF(psDemodCtx,u1CHFBw);		
                        ATD_SetIF(psDemodCtx);  
                    }
				}
				#endif
				else if('t'==((CHAR *) aszAtdArgv[0])[1])//zhilin_20130207:audio snow mode select
				{
                	#if AUDIO_SUPPORT_SNOW_MODE
					if (i4AtdArgc > 1)
					{
                    	ATD_SetAudioSnowMode(psDemodCtx,FALSE);
                        fgAudioSnow_Sel = (UINT16)StrToInt(aszAtdArgv[1]);					
					}
					mcSHOW_USER_MSG(("fgAudioSnow_Sel=%d,1:new;0:old\n",fgAudioSnow_Sel));
					#else
                    mcSHOW_USER_MSG(("Only Old audio snow mode ,can not switch \n"));
					#endif
				}
				else if('u'==((CHAR *) aszAtdArgv[0])[1])//zhilin_20130207:FM radio delay
				{
                    if (i4AtdArgc > 1)
                    {	

                        u1FMRadioDelay= (UINT8)StrToInt(aszAtdArgv[1]);
                    }
					mcSHOW_USER_MSG(("current FM Radio delay is %dms",u1FMRadioDelay));
				}
                else
                {
                    if (i4AtdArgc > 1)
                    {
#if ATD_SUPPORT_NCR      
                        if(FALSE == fg_NCR_Sel)
                        {
                            ucData1 = 0x00;
                            ATD_SetReg(psDemodCtx, 0x00A, &ucData1, 1);

                            ucData2 = StrToInt(aszAtdArgv[1]);
                            ATD_SetAutoConfig(psDemodCtx, 0); //turn-off PN & FN auto-config            
                            ATD_SetCrBw(psDemodCtx, (ucData2 & 0xFF));
                            mcSHOW_USER_MSG(("Set CR loop bandwidth as %d\n", ucData2)); 
                        }
                        else  //d20120205_hongji:NCR BW Control command,only for Kp setting
                        {
                            ATD_GetReg(psDemodCtx,REG_ATD_NCR_FSM_12,&ucData1,1);
                            ucData1 &= 0x0F;
                            ucData2 = StrToInt(aszAtdArgv[1]);
                            ucData1 |= ((ucData2&0x0F) << 4);
                            ATD_SetReg(psDemodCtx,REG_ATD_NCR_FSM_12,&ucData1,1);
                            mcSHOW_USER_MSG(("Set NCR Kp as 0x%x\n", ((ucData1>>4) & 0x0f))); 
                        }
#else
                        ucData1 = 0x00;
                        ATD_SetReg(psDemodCtx, 0x00A, &ucData1, 1);

                        ucData2 = StrToInt(aszAtdArgv[1]);
                        ATD_SetAutoConfig(psDemodCtx, 0); //turn-off PN & FN auto-config            
                        ATD_SetCrBw(psDemodCtx, (ucData2 & 0xFF));
                        mcSHOW_USER_MSG(("Set CR loop bandwidth as %d\n", ucData2)); 

#endif
                    }
                } 
                break;

			case 'q':
            {
            	UINT32 ucPar1 = (U32)StrToInt(aszAtdArgv[1]);	
            	UINT32 ucPar2 = (U32)StrToInt(aszAtdArgv[2]);
            	UINT32 ucPar3 = (U32)StrToInt(aszAtdArgv[3]);
            	Qlab_SetMonIndicator(ucPar1,ucPar2,ucPar3);
            }
            break;

			case 'r':	
				if (i4AtdArgc < 2)
				 	break;
				else if (i4AtdArgc == 2)
                   ucPar = 1;
				else
                	ucPar = (U8)StrToInt(aszAtdArgv[2]);
				
				u2RegAddr = (U16)StrToInt(aszAtdArgv[1]);

                if (ATD_GetReg(psDemodCtx, u2RegAddr, ucData, ucPar))
                {
                    mcSHOW_USER_MSG(("MTATD Get Register 0x%03X Fail!\n", u2RegAddr));
                    break;
                }
                for (ii=0; ii<ucPar; ii++)
                {
                    mcSHOW_USER_MSG(("Register 0x%03X = 0x%02X\n", u2RegAddr+ii, ucData[ii]));
                }
                break;


			case 's':
#if 	ATD_SUPPORT_VSI
            if('v'==((CHAR *) aszAtdArgv[0])[1])
            {
            	 ATD_SetVSIScanMode(psDemodCtx);	  
            }
            else
#elif  ATD_SUPPORT_H_SYNDROME
            if('h'==((CHAR *) aszAtdArgv[0])[1])
            {
            	 ATD_GetHSydromeLock(psDemodCtx);	  
            }            
            else if('l'==((CHAR *) aszAtdArgv[0])[1])
            {            
                UINT16 u2LRE,u2Max_LRE,u2Min_LRE;
                UINT16 u2Cnt = 0x00;
                if (i4AtdArgc > 1)
                {
                    u2Cnt = (UINT16)StrToInt(aszAtdArgv[1]);
                }
                u2LRE = ATD_GetLRE(psDemodCtx);
                u2Min_LRE = u2LRE;
                u2Max_LRE = u2LRE;
                for(ii=0x00;ii<u2Cnt;ii++)
                {
                    u2LRE = ATD_GetLRE(psDemodCtx);
                    if(u2LRE > u2Max_LRE)
                    {
                        u2Max_LRE = u2LRE;
                    }

                    if(u2LRE < u2Min_LRE)
                    {
                        u2Min_LRE = u2LRE;
                    }                    
                }
                mcSHOW_DBG_MSG1(("u2Max_LRE is %d, u2Min_LRE is %d \n",u2Max_LRE,u2Min_LRE));
            }
            else if('m'==((CHAR *) aszAtdArgv[0])[1])
            {            
                UINT8 u1HsynLock;
                UINT16 u2PAL_NTSC_Cnt= 0x00;
                UINT16 u2SECAM_Cnt = 0x00;
                UINT16 u2NoSignal_Cnt = 0x00;
                UINT16 u2Cnt = 0x00;
                if (i4AtdArgc > 1)
                {
                    u2Cnt = (UINT16)StrToInt(aszAtdArgv[1]);
                }
                for(ii=0x00;ii<u2Cnt;ii++)
                {
                    u1HsynLock = ATD_GetHSydLock(psDemodCtx);
                    switch(u1HsynLock)
                    {
                        case 0:
                        {
                            u2NoSignal_Cnt++;
                            break;
                        }
                        case 1:
                        {
                            u2PAL_NTSC_Cnt++;
                            break;
                        }
                        case 2:
                        {
                            u2SECAM_Cnt++;
                            break;
                        }
                        default:
                        {
                            mcSHOW_DBG_MSG1(("Invalidate lock status \n"));
                        }
                    }
                    mcDELAY_MS(10);   //Stress test for H-Syndrome
                }
                mcSHOW_DBG_MSG1(("u2NoSignal_Cnt is %d, u2PAL_NTSC_Cnt is %d, u2SECAM_Cnt is %d \n",u2NoSignal_Cnt,u2PAL_NTSC_Cnt,u2SECAM_Cnt));
            }
            else
#endif    
#ifdef CC_POWER_MERGE_TEST
            if('p'==((CHAR *) aszAtdArgv[0])[1])
            {
                UINT8 fg_Thread_Running = 0x00;
                if (i4AtdArgc > 1)
                {
                    fg_Thread_Running = StrToInt(aszAtdArgv[1]);
                    if(i4AtdArgc > 2)
                    {
                        g_u2PowerMerge_Delay = StrToInt(aszAtdArgv[2]);                        
                    }      

                    mcSHOW_DBG_MSG1(("g_u2PowerMerge_Delay = %d \n",g_u2PowerMerge_Delay));
                      
                    if(fg_Thread_Running)
                    {     
                        if(FALSE == fgTunerAGCThreadRunning)
                        {
                            //d20121214_hongji:power merge test function for remy
                            if (x_thread_create(&t_TunerAGCThread, "TunerAGCThread", 4096, 200, TunerAGCPowerMergeProc, sizeof(psDemodCtx), (void*)psDemodCtx) == OSR_OK) 
                            {
                                fgTunerAGCThreadRunning = TRUE;
                                mcSHOW_DBG_MSG1(("Create thread toggle RFAGC/IFAGC for PLLGP power merge test success !\n"));
                            }
                            else
                            {
                                mcSHOW_DBG_MSG1(("Create thread toggle RFAGC/IFAGC for PLLGP power merge test fail !\n"));
                            }
                        }
                    }
                    else
                    {
                        fgTunerAGCThreadRunning = FALSE;
                        mcSHOW_DBG_MSG1(("Kill TunerAGCThread and roll back original AGC setting\n"));
                    }
                }
                else
                {
                    mcSHOW_DBG_MSG1(("0: Kill TunerAGCThread \n"));
                    mcSHOW_DBG_MSG1(("1 xxx: Create TunerAGCThread and toggle AGC according xxx delay_time \n"));
                }
            }
            else
#endif
			{					 
                u1SubSysIdx = psDemodCtx->u1SubSysIdx;
//				u1SubSysIdx = psAtdPdCtx->u1SubSysIdx;    //120109;ORG
                if (i4AtdArgc > 1)
                {
                    u1SubSysIdx = StrToInt(aszAtdArgv[1]);                    
                    psDemodCtx->u1SubSysIdx = u1SubSysIdx + MOD_ANA_TYPE_BEGIN;	//modified for get the correct   s4DsmOutFreq
                    i4CurFreq = psDemodCtx->u4Frequency;
//					i4CurFreq = psAtdPdCtx->Frequency;    //120109;ORG
					ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);						
                    psDemodCtx->u1SubSysIdx = u1SubSysIdx + MOD_ANA_TYPE_BEGIN;
//					psAtdPdCtx->u1SubSysIdx = u1SubSysIdx + MOD_ANA_TYPE_BEGIN;    //120109;ORG
                    ATD_ChipInit(psDemodCtx);   // make sure ATD in normal mode after nim.go dvbt
                    ATD_SetSystem(psDemodCtx, psDemodCtx->u1SubSysIdx);
//					ATD_SetSystem(psAtdeuDemodCtx, psAtdPdCtx->u1SubSysIdx);    //120109;ORG
					if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx) ||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))// to get the initial DRO target level
//					if ((psAtdPdCtx->u1SubSysIdx==MOD_SECAM_L) ||(psAtdPdCtx->u1SubSysIdx==MOD_SECAM_L1))// to get the initial DRO target level    //120109;ORG
                    {
                        ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1); 
                        u2AtdDroTarLvlInitial = 0x0000;
                		u2AtdDroTarLvlInitial |= (UINT16)ucData1;
                    	u2AtdDroTarLvlInitial = (u2AtdDroTarLvlInitial << 8);
                	    ATD_GetReg(psDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData1, 1);
                		u2AtdDroTarLvlInitial |= (UINT16)ucData1;
                		u2AtdDroTarLvlInitial &= 0x7FFF;
                    	mcSHOW_DBG_MSG1(("The SECAML/L1 DRO target level(initial value) = 0x%04X\n", u2AtdDroTarLvlInitial));
                    }

                }
                else
                {
                    mcSHOW_USER_MSG(("\t %d: PAL_BG  \n", MOD_PAL_BG - MOD_ANA_TYPE_BEGIN));
                    mcSHOW_USER_MSG(("\t %d: PAL_DK  \n", MOD_PAL_DK - MOD_ANA_TYPE_BEGIN));
                    mcSHOW_USER_MSG(("\t %d: PAL_I   \n", MOD_PAL_I - MOD_ANA_TYPE_BEGIN));
                    mcSHOW_USER_MSG(("\t %d: SECAM_L \n", MOD_SECAM_L - MOD_ANA_TYPE_BEGIN));
                    mcSHOW_USER_MSG(("\t %d: SECAM_L1\n", MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN));
                    mcSHOW_USER_MSG(("\t %d: NTSC_M  \n", MOD_NTSC_M - MOD_ANA_TYPE_BEGIN));                    
                }
                mcSHOW_USER_MSG(("_SubSysIdx = %d\n", psDemodCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN));
//				mcSHOW_USER_MSG(("_SubSysIdx = %d\n", psAtdPdCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN));    //120109;ORG
#ifdef  EU_AUTO_ATP
               			   
                if( gATP_TEST_ITEM_BIT &0x08)
             	{
             	    UINT8 bVIFLock,bTVDLock;
		            i4CurFreq = psDemodCtx->u4Frequency;
//		            i4CurFreq = psAtdPdCtx->Frequency;    //120109;ORG
                    bVIFLock =  ATD_GetVIFLock(psDemodCtx, TRUE);
		            bTVDLock =   fgGetTVDSts(psDemodCtx,CHN_LOCK_CHECK_TIME);
		 
                    mcSHOW_DBG_MSG(("##Start\n##CurFreq : %d\n##VIFLOCK : %d\n##TVDLOCK : %d\n##SubSysIdx : %d\n",i4CurFreq,bVIFLock,bTVDLock,psDemodCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN));
//					mcSHOW_DBG_MSG(("##Start\n##CurFreq : %d\n##VIFLOCK : %d\n##TVDLOCK : %d\n##SubSysIdx : %d\n",i4CurFreq,bVIFLock,bTVDLock,psAtdPdCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN));    //120109;ORG

		            if (gATP_TEST_ITEM_BIT & 0x10)
     	            {
                        if (!gATPFailBitMsk)
         	            {
                            mcSHOW_DBG_MSG(("##ATPFAIL : 0\n##End\n"));
         	            }
	                    else
	  	                {
                            mcSHOW_DRVAPI_MSG(("##ATPFAIL : 1 , Msk =%x\n##End\n",gATPFailBitMsk));
		                    gATPFailBitMsk = 0;
	  	                }
     	            }
		            else
		    	    {
                        mcSHOW_DRVAPI_MSG(("##End\n"));
		    	    }
             	}			 			 
		        else if(gATP_TEST_ITEM_BIT & 0x01)
		        {
                    mcSHOW_DBG_MSG(("##Start\n##SubSysIdx : %d\n##End\n",psDemodCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN));
//					mcSHOW_DBG_MSG(("##Start\n##SubSysIdx : %d\n##End\n",psAtdPdCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN));    //120109
		        }		 
#endif
            }
                break;

            case 't': 
			    if (2 == i4AtdArgc)
				{
				    i4CurFreq = psDemodCtx->u4Frequency;
//					i4CurFreq = psAtdPdCtx->Frequency;    //120109;ORG
					i4CurTop= (U8)StrToInt((CHAR *) aszAtdArgv[1]);
					if(cCUST_TUNER == psDemodCtx->u1TunerType)//Customized tuner   
                    {
                        i1Top	= (INT8)StrToInt((CHAR *) aszAtdArgv[1]);	
//						if(ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetTop, psAtdPdCtx->u1SubSysIdx,&i1Top)==ITUNER_OK)     //120109;ORG
                        if(ITuner_OP(psDemodCtx->pTCtx, itSetTop, psDemodCtx->u1SubSysIdx,&i1Top)==ITUNER_OK) 
                        {
					        ATD_SetSystem(psDemodCtx, psDemodCtx->u1SubSysIdx);	
//							ATD_SetSystem(psAtdeuDemodCtx, psAtdPdCtx->u1SubSysIdx);	//120109;ORG
                        }
                    }
					else
					{
					    ITuner_OP(psDemodCtx->pTCtx, itSetTop, i4CurTop, &(psDemodCtx->u1SubSysIdx)); 
//						ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetTop, i4CurTop, &(psAtdPdCtx->u1SubSysIdx));     //120109;ORG
					    ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);
					    mcSHOW_USER_MSG(("Set Top = 0x%x\n", i4CurTop));
					}	
				}
				else
				{
				    if(ITuner_OP(psDemodCtx->pTCtx, itGetTop, psDemodCtx->u1SubSysIdx, &i1Top)==ITUNER_OK)
//					if(ITuner_OP(psAtdeuDemodCtx->pTCtx, itGetTop, psAtdPdCtx->u1SubSysIdx, &i1Top)==ITUNER_OK)    //120109;ORG
				    {
				        mcSHOW_USER_MSG(("Current Top = 0x%x sys_index=%d\n", i1Top,psDemodCtx->u1SubSysIdx));
//						mcSHOW_USER_MSG(("Current Top = 0x%x sys_index=%d\n", i1Top,psAtdPdCtx->u1SubSysIdx));    //120109;ORG
				    }
				}
				break;

            case 'v':
                i4CurFreq = psDemodCtx->u4Frequency;
//				i4CurFreq = psAtdPdCtx->Frequency;    //120109;ORG

                if (i4AtdArgc > 1)
                {
                    i4CurFreq = StrToInt(aszAtdArgv[1]);
                    psDemodCtx->u4Frequency = i4CurFreq;
//					psAtdPdCtx->Frequency = i4CurFreq;    //120109;ORG
                    ATD_ChipInit(psDemodCtx);
                    ATD_SetFreq(psDemodCtx, i4CurFreq, FALSE);
                    ATD_StartAcq(psDemodCtx, FALSE);                    
                }
                psDemodCtx->fgCheckCR=TRUE;//to avoid VIf lock indicator error
                mcSHOW_USER_MSG(("VIF Lock: %d (Freq = %dKHz)\n", ATD_GetVIFLock(psDemodCtx, TRUE), i4CurFreq));
                mcSHOW_USER_MSG(("TVD Lock: %d\n", fgGetTVDSts(psDemodCtx,CHN_LOCK_CHECK_TIME)));
#ifdef  EU_AUTO_ATP	
                UINT8 bVIFLock,bTVDLock;
                bVIFLock =  ATD_GetVIFLock(psDemodCtx, TRUE);
		        bTVDLock =   fgGetTVDSts(psDemodCtx,CHN_LOCK_CHECK_TIME);

		        if (gATP_TEST_ITEM_BIT & 0x04)
		        {   //only available in nim.atd p 4(bit 2 =1)
		            if ((2 == bVIFLock) && (1 == bTVDLock))
		            {
                        mcSHOW_DBG_MSG(("##Start\n##Video : ON\n##End\n"));  
		            }
		            else
		            {
                        mcSHOW_DBG_MSG(("##Start\n##Video : OFF\n##End\n"));  
		            }
		        } 
		        else if (gATP_TEST_ITEM_BIT & 0x01)
		        {
		            mcSHOW_DBG_MSG(("##Start\n##VIFLOCK : %d\n##TVDLOCK : %d\n##End\n", bVIFLock,  bTVDLock));	
		        }
#endif 			
                break;  

			case 'w':
				if (i4AtdArgc < 3)
					break;
			
				u2RegAddr = (U16)StrToInt(aszAtdArgv[1]);
				ucPar = (U8)StrToInt(aszAtdArgv[2]);
				if (ATD_SetReg(psDemodCtx, u2RegAddr, &ucPar, 1))
				{
				    mcSHOW_USER_MSG(("MTATD Set Register 0x%03X Fail!\n", u2RegAddr));
					break;
				}
				mcSHOW_USER_MSG(("Register 0x%03X = 0x%02X, OK!\n", u2RegAddr, ucPar));
				break;

			case 'x':
            {   
	            S32 Record[10] = {0,0,0,0,0,0,0,0,0,0};
                U16  u2Data1, u2Data2;
                UINT32 RegTemp = 0;
	    	    UINT32 dRegTemp = 0;	
                               
	    	       
	    	    //Get ATD TVD Indicator
                for (ii=0; ii<3; ii++)
                {
                    //PRA
#if (defined (CC_MT5365) || (defined CC_MT5395) || (defined CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882))
                    ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_34, &ucData1, 1);
                    mcSHOW_USER_MSG(("REG_ATD_SLD_PRA_34(0x2e4) = 0x%02X\n", ucData1));
#else
                    ATD_GetReg(psDemodCtx, REG_ATD_PRA_ST, &ucData1, 1);
                    mcSHOW_USER_MSG(("REG_ATD_PRA_ST(0x15E) = 0x%02X\n", ucData1));
#endif
                    Record[0] += (ucData1 & 0x40) >> 6;
                    
                    // CR
                    s2Data = ATD_GetCFOKHz(psDemodCtx, FALSE);
                    mcSHOW_USER_MSG(("CFO = %d KHz\n", s2Data));
                    if (s2Data < 0)
                    	s2Data = -s2Data;
                    Record[1] += s2Data;               	                    
                    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucData1, 1);
                    mcSHOW_USER_MSG(("REG_ATD_CR_STATE0(0x1a0) = 0x%02X\n", ucData1));
                    Record[2] += ucData1 & 0x3;
#if (defined (CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882))   //for Remy ATD slt fail,Mustang follow Remy
					ATD_GetReg(psDemodCtx, REG_ATD_NCR_ADV_11,&ucData1, 1);
					mcSHOW_USER_MSG(("Pilot Strength : %d / 100  \n", ucData1*100/256));
            		if(ucData1 > NCR_PSBOUND) 
            			{
       						Record[3] += 1; //NCR Pilot Strength Indicaotr = 1
            			}
					else
						{
       						Record[3] += 0; //NCR Pilot Strength Indicaotr = 0
            			}
#else

                    ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE1, &ucData1, 1);
                    mcSHOW_USER_MSG(("REG_ATD_CR_STATE1(0x1a1) = 0x%02X\n", ucData1));
                    Record[3] += (ucData1 & 0x20) >> 5;
#endif
                           
                    //TVD
                    ucData1 = IO_READ8(VIDEO_IN0_BASE, 0x80);
                    mcSHOW_USER_MSG(("TVD noise level = %d\n", ucData1));           	
                    Record[4] += ucData1;
                    ucData1 = (IO_READ8(VIDEO_IN0_BASE, 0x81) & 0x10) >> 4;
                    mcSHOW_USER_MSG(("Vpress = %d\n", ucData1)); 		
                    Record[5] += ucData1;
                    u2Data = IO_READ32(VIDEO_IN0_BASE, 0x44);
                    u2Data1= (u2Data& 0x3FF0000) >> 16; //Sync Level
                    u2Data2 = u2Data & 0x000003FF; //Blank Level
                    mcSHOW_USER_MSG(("Sync (0x%3x) and Blank (0x%3x)\n",u2Data1,u2Data2));          	
                    Record[6] += u2Data1;
                    Record[7] += u2Data2;
	                
                    {
	    	        
	    	        //UINT16 Values[6];
	    	        //1 1.Enable Catch Sel
	    	        RegTemp = IO_READ32(VIDEO_IN0_BASE, 0x5EC);
	    	        RegTemp |= 0x00008000; //Bit 15
	    	        IO_WRITE32(VIDEO_IN0_BASE, 0x5EC, RegTemp);
                    
	    	        //1 2.Select Probe Signal
	    	        RegTemp = IO_READ32(VIDEO_IN0_BASE, 0x61C);
	    	        RegTemp &= 0x0FFFFFFF; //Bit 31 ~28
	    	        RegTemp |= 0xB0000000; //Bit 31 ~28
	    	        IO_WRITE32(VIDEO_IN0_BASE, 0x61C, RegTemp);
	    	        
	    	        IO_WRITE32(VIDEO_IN0_BASE, 0x0A8,0x0);   // clear indicator, Pluto	    	        	    	        
	    	        mcDELAY_MS(600);
	                
	    	        //1 3. Get Indicator
	    	        dRegTemp = IO_READ32(VIDEO_IN0_BASE, 0x0A8);
	    	        mcSHOW_USER_MSG(("V Sync Loss sum during 64 fields %d\n", dRegTemp & 0x3F));
                                           Record[8] += dRegTemp & 0x3F;
	    	        mcSHOW_USER_MSG(("H Sync Loss sum during 64 fields %d\n", (dRegTemp >> 6) & 0x3FFF));
                                           Record[9] += ((dRegTemp >> 6) & 0x3FFF);
	                }
                } //End for(ii=0; ii<5; ii++)
        
                //Average ATD TVD Indicator                
                for(ii=0;ii<10;ii++)
                {
	                Record[ii] /= 3;
	                mcSHOW_USER_MSG(("%d ", Record[ii]));			
                }
                mcSHOW_USER_MSG(("\n"));			
        
                //Check ATD TVD Indicator
                u2State = 0;
                if (0x01 == Record[0])  // PrA should be in the tracking state
                    u2State += mcBIT(0);
                if (Record[1] < 50) // CFO < 50KHz.
                    u2State += mcBIT(1); 
                if (0x03 == Record[2]) // CR should be int the tracking state 
                	u2State += mcBIT(2); 
                if (0x01 == Record[3]) // Pilot good stability
                	u2State += mcBIT(3); 
                if (Record[4] < 0x15) // TvD noise level => Need to tune.
                	u2State += mcBIT(4); 
                if (1 == Record[5]) // TvD VPRES
                	u2State += mcBIT(5);
                if ((Record[6] > 33) || (Record[6] < 36)) // TVD sync level
                	u2State += mcBIT(6); 
                if ((Record[7] > 250) || (Record[7] < 262)) // Blank Level ~ 0x100.
                	u2State += mcBIT(7); 
                if (0 == Record[8]) // V Sync Loss sum during 64 fields?
                	u2State += mcBIT(8); 
                if ((MOD_NTSC_M == psDemodCtx->u1SubSysIdx) && (Record[9] <= 576)) // H Sync Loss sum during 64 fields, NTSC
//                if ((psAtdPdCtx->u1SubSysIdx==MOD_NTSC_M) && (Record[9] <= 576)) // H Sync Loss sum during 64 fields, NTSC    //120109;ORG
                	u2State += mcBIT(9);
                else if ((MOD_SECAM_L == psDemodCtx->u1SubSysIdx) && (Record[9] <= 480)) // H Sync Loss sum during 64 fields, SECAM
//                else if ((psAtdPdCtx->u1SubSysIdx==MOD_SECAM_L) && (Record[9] <= 480)) // H Sync Loss sum during 64 fields, SECAM    //120109;ORG
                	u2State += mcBIT(9);
                
                if(0x3FF == u2State)
                //if ((u2State&0x00ff) == 0x00ff)
                {    
                	mcSHOW_USER_MSG(("VAGood\n"));
                }
                else
                {                	
                      mcSHOW_USER_MSG(("ATD SLT video fail(0x%03X)\n", u2State));
        	          mcSHOW_USER_MSG(("ATDFAIL\n"));
                }                                
            }   
                break;
			case 'y'://only for sony tuner re227
	        {
				UINT8 isAnalog = 0;
	            if(i4AtdArgc > 1)
			    {
		            switch(aszAtdArgv[1][0]){
		            case 'd':
		            case 'D':
		                isAnalog = 0;
		                break;
		            case 'a':
		            case 'A':
		                isAnalog = 1;
		                break;
		            default:
						isAnalog = 2;
		                mcSHOW_USER_MSG(("Error: Digital/Analog ('d' or 'a') is necessary.\n"));
		                break;
		            }
			    }
				else
				{
		            mcSHOW_USER_MSG(("Error: Digital/Analog ('d' or 'a') is necessary.\n"));
					break;
				}
				if(isAnalog > 1)
				{
					break;
				}
				if(i4AtdArgc > 2)
				{
            		u4Freq = (UINT32)StrToInt(aszAtdArgv[2]);
				}
				else
				{
					mcSHOW_USER_MSG(("Error: Threshold frequency is necessary.\n"));
	                break;
				}

	            if(ITuner_OP(psDemodCtx->pTCtx, itSetTrapThresold, isAnalog, &u4Freq) == ITUNER_OK){
	                if(isAnalog){
	                    mcSHOW_USER_MSG(("Analog trap threshold : %d kHz\n", u4Freq));
	                }else{
	                    mcSHOW_USER_MSG(("Digital trap threshold : %d kHz\n", u4Freq));
	                }
	            }else{
	                mcSHOW_USER_MSG(("ITuner_OP failed.\n"));
	            }
	        }
        	break;
#ifdef EU_AUTO_ATP
            case 'z':
	
                if (i4AtdArgc > 1)
                {
                    gATP_TEST_ITEM_BIT = StrToInt(aszAtdArgv[1]);                           
		            if (gATP_TEST_ITEM_BIT & 0x10)//check ATPFAIL BITS
		     	    {
                        gATPFailBitMsk = 0;  //reset gATPFailBitMsk
		     	    }		
                }  
		break;		
#endif	
            default:
                mcSHOW_USER_MSG(("Invalid parameter!\n"));
                break;
        }
    }
	if(FALSE == fg_Acq_flag)
    {
        DigTunerBypassI2C (FALSE);
    }
}

VOID ATD_TunerAtdCommandList(INT32 i4AtdArgc, BOOL fgAtdMonitorEnable)
{
    if (i4AtdArgc < 1)
    {
        mcSHOW_USER_MSG(("\n"));
		mcSHOW_USER_MSG(("Cmd_PI: \n"));		
		mcSHOW_USER_MSG(("\t b Bypass SIF to aud\n"));  //Test ok
        mcSHOW_USER_MSG(("\t c:              Show Current Freq (kHz)\n")); //Test ok
        mcSHOW_USER_MSG(("\t e [Register (UINT8)]: Set or Get 5 Register,The CLI input formats are: nim.atd e or nim.atd e ##-##-##-##-##\n"));
		mcSHOW_USER_MSG(("\t f [Freq (kHz)]: AnaTunerSetFreq\n")); //Test OK
		mcSHOW_USER_MSG(("\t g [Count] [Period (ms)]: Get current ATD status \n")); //Test OK
		mcSHOW_USER_MSG(("\t h [Start Freq (kHz)][End Freq (kHz)][Count]: TunerSearchNearbyFreq\n"));  //Test OK
		mcSHOW_USER_MSG(("\t i Set Antenna [1/2] in Antenna 1/2 int\n")); //Test ok
		#if defined(CC_MT5389) 
		mcSHOW_USER_MSG(("\t k: Adjust the DRO target level adaptivily\n"));
		#endif
		mcSHOW_USER_MSG(("\t l [0/1]: Set LNA on/off \n")); //Test OK
		if(1 == fgAtdMonitorEnable)
        {
            mcSHOW_USER_MSG(("\t mp:             Pause monitor thread\n"));//Test OK
            mcSHOW_USER_MSG(("\t mr:             Resume monitor thread\n")); //Test OK
        }
		mcSHOW_USER_MSG(("\t o:             Turn on the CVBS out\n"));
		mcSHOW_USER_MSG(("\t p [CR loop bandwidth]: Set CR loop bandwidth \n"));//Test OK
		mcSHOW_USER_MSG(("\t pd Set TVD re-check interval \n"));//Test OK
        mcSHOW_USER_MSG(("\t pe g_CPR_cnt\n"));//Test OK
        mcSHOW_USER_MSG(("\t pg g_pra_cnt\n"));//Test OK
        mcSHOW_USER_MSG(("\t pf g_CPR_delay\n"));//Test OK
        mcSHOW_USER_MSG(("\t ph g_CR_BW_AutoConfig_delay_cnt\n"));//Test OK
		mcSHOW_USER_MSG(("\t pm patch mask setting \n"));//Test OK
		mcSHOW_USER_MSG(("\t pa [IF swing] adjust IF output swing \n"));
        mcSHOW_USER_MSG(("\t pk FM Radio Setting \n"));
#if (ATD_SUPPORT_VSI|ATD_SUPPORT_H_SYNDROME)
        mcSHOW_USER_MSG(("\t pq [VSI on/off] VSI Function on/off \n"));
#endif
#if ATD_SUPPORT_NCR 		
        mcSHOW_USER_MSG(("\t pn Log NCR Ps,Is and CFO message \n"));  //d20120205_hongji:Show NCR Log message for fine tune NCR Threshold
		mcSHOW_USER_MSG(("\t ps [CR type] CR Select 0:OCR   1:NCR \n"));  //d20120203_hongji:CR Select command
        mcSHOW_USER_MSG(("\t pi [SBW on/off] enable CR BW SW Auto config Patch. 0:disable 1:enable \n"));
        mcSHOW_USER_MSG(("\t pb [BN on/off] enable BN Patch. 0:disable 1:enable \n"));
#endif		
#if ATD_SUPPORT_CHF		
        mcSHOW_USER_MSG(("\t pc [CHF BW] set channel filter BW 6/7/8\n")); 
#endif
		
		mcSHOW_USER_MSG(("\t r [RegAddr] [Num]:   read  ATD register\n")); //Test OK
		mcSHOW_USER_MSG(("\t s [SubSysIdx]:  Color / Sound sub-system\n"));//Test OK
#if 	ATD_SUPPORT_VSI		
        mcSHOW_USER_MSG(("\t sv Set VSI scan mode\n"));//Test OK
#endif
#if ATD_SUPPORT_H_SYNDROME
        mcSHOW_USER_MSG(("\t sh Show H-Syndrome and Tone detection lock status\n"));
        mcSHOW_USER_MSG(("\t sl [Times] Static current LRE status \n"));
        mcSHOW_USER_MSG(("\t sm [Times] Static current H-Syndrome lock status \n"));
#endif
#ifdef CC_POWER_MERGE_TEST
        mcSHOW_USER_MSG(("\t sp [Thread on/off][Delay time]: Power merge test  \n"));
#endif
		mcSHOW_USER_MSG(("\t t [Top]: SetTOP\n"));          
        mcSHOW_USER_MSG(("\t v [Freq (kHz)]: Check Lock Status\n"));   //Test OK                  
        mcSHOW_USER_MSG(("\t w [RegAddr] [Value]: write ATD register\n"));  //Test OK      
        mcSHOW_USER_MSG(("\t x: SLT test    \n"));		//Test OK	        
        mcSHOW_USER_MSG(("\t y [d|a] [freqKHz]                 Change trap threshold\n")); 
    }	
}

