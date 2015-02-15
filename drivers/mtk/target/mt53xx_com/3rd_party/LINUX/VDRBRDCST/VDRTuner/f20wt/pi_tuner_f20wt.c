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
 * $RCSfile: pi_tuner_f20wt.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_tuner_f20wt.c
 *  Tuner control for TCL F20WT(sawless tuner template)
 */


#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "i2c_api.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "eq_script_f20wt.h"

// *** Release Note ***
// Ver     Date         Description
// 1.00    2009.03.25   file established
#define C_F20WT_VERSION				"F20WT 2010-01-01\n 2012-01-10\n v1.3"

#define C_F20WT_PLL_TIMEOUT_CNT     10
 
#define C_F20WT_LO_ADDRRESS           ((UINT16)  0xC2)
#define C_F20WT_IF_FREQUENCY_ANA      ((UINT16)  38000)  /* kHz */
#define C_F20WT_IF_FREQUENCY_L1       ((UINT16)  33875)  /* kHz */
#define C_F20WT_LO_DIVIDER_STEP_ANA   ((UINT16)  62500) //50000)  /* Hz */


static UINT8* pF20wtEqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pF20wtEqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pF20wtEqStrongChroma[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};

//Tuner releated ATD register
#if defined(CC_MT5396)  || defined(CC_MT5368) || defined(CC_MT5389)
static ATD_TUNER_REL_REG_T arrAtdRelReg[]=
{
    {0x7d4, 0x02},
    {0x7d5, 0xb3},
    {0x7d6, 0xb3},
    {0x7d7, 0xef},
    {0x7d8, 0xa3},
    {0x7d9, 0xa3},
    {0x7da, 0x7f},
    {0x7db, 0x7f},
    {0x7dc, 0x80},
    {0x7dd, 0xf0},
    {0x7de, 0xf0},
    {0x7ca, 0x77},
    {0x7cb, 0x03},
    {0x7cc, 0x14},
    {0x0,0x0}//end flag
 };
#else
static ATD_TUNER_REL_REG_T arrAtdRelReg[]=
{
    {0x0,0x0}//end flag
};
#endif

//


///////////////////////////////////
// ALPS F20WT driver
///////////////////////////////////

CHAR* F20WT_GetSwVer(void)
{
	return ((CHAR*)C_F20WT_VERSION);
}


VOID F20WT_TunerInit(ITUNER_CTX_T *pTCtx)
{
    ITUNER_CTX_T *psTunerCtx = pTCtx;
    SPECIFIC_MEMBER_EU_CTX *pEUCtx = &(psTunerCtx->specific_member.eu_ctx);

    psTunerCtx->I2cAddress = C_F20WT_LO_ADDRRESS;
    psTunerCtx->u2IF_Freq_A = C_F20WT_IF_FREQUENCY_ANA;
    psTunerCtx->u4RF_Freq = 0;
    psTunerCtx->u1AtdPatchSwitch = 0x09;
    psTunerCtx->fgRFAGC = TRUE;//need external RF AGC control
	 psTunerCtx->fgRFTuner = TRUE;//sawless tuner
    psTunerCtx->u2RetryTVDTm = 100;

    // iven 20110706, add for polarity if setting for NA reference
    psTunerCtx->bSpecPolarity = 0;
    // iven 20110706, add for legacy tuner as default fixed PGA gain index. Don care for Sawless
    psTunerCtx->uSLD_DefaultPGAIndex = 0;		
    	
	psTunerCtx->u2LO_Step=C_F20WT_LO_DIVIDER_STEP_ANA/10;//PLL stepsize(unit is 10Hz) 
    pEUCtx->u2LO_Step=psTunerCtx->u2LO_Step;//for single-system
    
    // init EQ script for mtk board
    #if ((defined CC_MT5365) || (defined CC_MT5395)|| (defined CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389))
    pF20wtEqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQF20wtBG;
    pF20wtEqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQF20wtDK;
    pF20wtEqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]   = EQF20wtI;
    pF20wtEqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN] = EQF20wtL;
    pF20wtEqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQF20wtL1;
    pF20wtEqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQF20wtM;
    pF20wtEqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQF20wtBG;
    pF20wtEqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQF20wtDK;
    pF20wtEqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]   = EQF20wtI;
    pF20wtEqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQF20wtM;
    pF20wtEqStrongChroma[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQF20wtBG;
    #else
    pF20wtEqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQF20wtBG;
    pF20wtEqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQF20wtDK;
    pF20wtEqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]   = EQF20wtI;
    pF20wtEqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN] = EQF20wtL;
    pF20wtEqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQF20wtL;
    pF20wtEqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQF20wtM;
    pF20wtEqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQF20wtBGWeak;
    pF20wtEqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQF20wtDKWeak;
    pF20wtEqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]   = EQF20wtIWeak;
    pF20wtEqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQF20wtMWeak;
    pF20wtEqStrongChroma[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQF20wtBGStrongChroma;
    #endif
    
    #if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)
    pF20wtEqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQF20wtBGWeak;
    pF20wtEqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQF20wtDKWeak;
    pF20wtEqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]   = EQF20wtIWeak;
    pF20wtEqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQF20wtMWeak;
    pF20wtEqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN] = EQF20wtLWeak;
    pF20wtEqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN] = EQF20wtL1Weak;
    #endif
    
    mcDBG_MSG_ERR(("Tuner F20WT for mtk board init done!\n"));
}

INT16 F20WT_TunerSetFreq(ITUNER_CTX_T *pTCtx, PARAM_SETFREQ_T* param)
{
    UCHAR TunerS[4];
    UINT16   Ndivider;
    UCHAR CB1, BB; 
    UCHAR ucPllValue;
    UINT32 Lo;
    INT8 s1PllPollCnt;
    INT8 jj;

    UINT32 Freq = param->Freq; // transfer to video freq 
    //UINT16 AutoSearch = param->fgAutoSearch;
    UINT8  Mode = param->Modulation;
	
    pTCtx->u4RF_Freq = Freq;

    if( Mode == MOD_SECAM_L1 )
    {
        Lo = Freq + C_F20WT_IF_FREQUENCY_L1; // kHz
        Ndivider = (UINT16) ( (Lo*1000+C_F20WT_LO_DIVIDER_STEP_ANA/2) / C_F20WT_LO_DIVIDER_STEP_ANA);
        mcDBG_MSG_L2(("Freq = %dkHz, Divider ratio = %04X, PLL set freq = %d kHz\n",
                        pTCtx->u4RF_Freq, 
                        Ndivider,
                        (Ndivider*C_F20WT_LO_DIVIDER_STEP_ANA/1000-C_F20WT_IF_FREQUENCY_L1)));
    }
    else
    {
        Lo = Freq + C_F20WT_IF_FREQUENCY_ANA; // kHz
        Ndivider = (UINT16) ( (Lo*1000+C_F20WT_LO_DIVIDER_STEP_ANA/2) / C_F20WT_LO_DIVIDER_STEP_ANA);
        mcDBG_MSG_L2(("Freq = %dkHz, Divider ratio = %04X, PLL set freq = %d kHz\n",
                        pTCtx->u4RF_Freq,
                        Ndivider,
                        (Ndivider*C_F20WT_LO_DIVIDER_STEP_ANA/1000-C_F20WT_IF_FREQUENCY_ANA)));
    }	        
    pTCtx->u2IF_Freq_A = Lo - Freq;
    
    Freq = Freq/1000;
 
    CB1 = 0x8E; 
        
    if (Freq <= 150) {
        BB = 0x01;	//8M		
    }
    else if (Freq  <= 423) {        
        BB = 0x02; 			
    }		
    else {
        BB = 0x08; 
    }
	    
    // LO settings
    TunerS[0] = (UCHAR) 0x7F & (Ndivider >> 8);
    TunerS[1] = (UCHAR) 0xFF & Ndivider;
    TunerS[2] = (UCHAR) CB1;  
    TunerS[3] = (UCHAR) BB;
		
    for (jj=0; jj<2; jj++)
    {
        if (ucI2cWriteOnly(pTCtx->I2cAddress, TunerS, 4))
        {
            mcDBG_MSG_ERR(("TunerWrite PLL failed!\n"));
            return (+1);
        }
        else
        {
            mcDBG_MSG_L2(("Tuner F20WT Write PLL success!\n"));
        }
	
    		for (s1PllPollCnt=0; s1PllPollCnt<C_F20WT_PLL_TIMEOUT_CNT; s1PllPollCnt++)
    		{
            x_thread_delay((UINT32)10);
            
            if (ucI2cReadOnly(pTCtx->I2cAddress, &ucPllValue, 1))
            {
                mcDBG_MSG_ERR(("Tuner read failed!\n"));
                return (+1);
            }			
            //mcDELAY_MS(10);			
            // check if PLL lock
            if (ucPllValue&0x40) {
                break;
            }											
    		}

        if(s1PllPollCnt < C_F20WT_PLL_TIMEOUT_CNT)
        {
            mcDBG_MSG_L2(("Freq = %d MHz, the %d-th time setting PLL success, cnt = %d\n" , Freq, jj+1, s1PllPollCnt));	
            break;
        }
        else
        {
            mcDBG_MSG_L2(("Freq = %d MHz, the %d-th time setting PLL fail\n" , Freq, jj+1));		
        }
    }
    
    return (0);
}


INT16 F20WT_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal){
         
    switch(eOperation)
    {
    case itGetVer:
        {
            CHAR ** RetStr = (CHAR **)pInOutVal;
            *RetStr = F20WT_GetSwVer();
        }
        break;
    case itGetEqScriptNormal: 
        *(UINT8**)pInOutVal = pF20wtEqNormal[SetVal];
        break;
    case itGetEqScriptWeak: 
        *(UINT8**)pInOutVal = pF20wtEqWeak[SetVal];
        break;
    case itGetEqScriptStrongChroma: 
        *(UINT8**)pInOutVal = pF20wtEqStrongChroma[SetVal];
        break;  
    case itGetTunerRelReg://Get ATD Tuner Rel register setting
       {
            *(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg;
        }
        break;

    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}
