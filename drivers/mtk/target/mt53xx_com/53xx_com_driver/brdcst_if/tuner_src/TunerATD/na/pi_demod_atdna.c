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
#include "PD_Def_dbg.h"  /*need include RISC IF macro, if re-ware RISC IF , remove this*/
#include "drvcust_if.h"
#include "drv_tvd.h"
#include "pi_demod_atdna.h"
#include "tuner_interface_if.h"

/***********************************************************************/
/*                  Defines                                            */
/***********************************************************************/
/***********************************************************************/
/*                  Private Types                                      */
/***********************************************************************/

/***********************************************************************/
EXTERN BOOL fgEnableATP;
/***********************************************************************/

#if defined(ATD_SNOWSCREEN_MODE)
STATIC UINT32 wPreLockFreq = 900000;
STATIC UINT32 u4FreqTemp = 900000;
#endif

UINT8 ATDNA_SearchNearbyFreq(ATD_CTX_T *psDemodCtx, UINT32 *wFreqInKHz, x_break_fct _BreakFct,
                                 void *pvArg, UINT32 u4FreqBDKHz)
{
    UINT32  wFRTmp, s4NextFreqStep;
	UINT32  u2TickStart_ATDLayer;
    UINT8   bScanState = 0;
    UINT8   ii,iii;
    UINT8   bATVStatus = 0;
    BOOL fgFirstCheck = FALSE;  
    UINT32 TimingOffset;
    UINT16  u2LOStep;

    u2LOStep=psDemodCtx->pTCtx->specific_member.us_ctx.u2LO_Step;
	if(u2LOStep<5000)
    {   
		u2LOStep= 5000;
    }
    psDemodCtx->isBreak = _BreakFct;
    
    wFRTmp = *wFreqInKHz;
    
    mcSHOW_DBG_MSG(("[ATuner] SearchNearBy: Freq = %d kHz LO size=%d\n", wFRTmp,u2LOStep));

    u2TickStart_ATDLayer = mcGET_SYS_TICK();
    TimingOffset = (mcGET_SYS_TICK() - u2TickStart_ATDLayer) * mcGET_TICK_PERIOD();
   
    mcDELAY_MS(10);
    
    ATD_GetVIFLock(psDemodCtx,TRUE);   

    mcSHOW_HW_MSG(("[ATuner] VIF Lock : %d\n",psDemodCtx->u1VIFLock));
    
    // Initial check to see if curent freq need full range AFT or not
    if ((psDemodCtx->u1VIFLock >= 2))
    {
        if (fgGetTVDSts(psDemodCtx,CHN_LOCK_CHECK_TIME)) //For MT5387 TVD export fucntion        
        {
            TimingOffset = (mcGET_SYS_TICK() - u2TickStart_ATDLayer) * mcGET_TICK_PERIOD() - TimingOffset;
            mcSHOW_DBG_MSG(("[ATuner] End of SearchNearby = %d ms\n",TimingOffset));
            mcMSG_IN_SEARCHNEARBY_AUTO_TEST(u2LOStep)
            return 3;
        }       
        else
        {
            mcSHOW_DBG_MSG(("VIF lock, but TVD unlock!\n"));
            fgFirstCheck = FALSE;            
        }       
    }    	
    else if ((1 == psDemodCtx->u1VIFLock))
    {
        fgFirstCheck = TRUE;        
        psDemodCtx->u1PF = 0x00;
        psDemodCtx->fgCheckCR = FALSE;
        s4NextFreqStep = (psDemodCtx->s4Cfo*1000)/(u2LOStep*10);
        s4NextFreqStep *= ((u2LOStep*10)/1000);
		
        mcSHOW_DBG_MSG(("s4CFO=%d KHz,NextStep=%d KHz\n",psDemodCtx->s4Cfo,s4NextFreqStep));        
        
		wFRTmp += s4NextFreqStep;
        bScanState = TUNER_PLL_LOCK_STATE;
    }
    else
    {        
        fgFirstCheck = FALSE;        
    }

    for(ii=0;ii<3;ii++)  //max 3 trial per searchnearby
    {
        if((ii==0) && (!fgFirstCheck))
        {
            ii++;
        }

        if (1 == ii)
        {
            psDemodCtx->u1PF = 0x01;
            psDemodCtx->fgCheckCR = TRUE;
            wFRTmp = *wFreqInKHz - 1800;
            bScanState = TUNER_PLL_LOCK_STATE;
        }
        else if (2 == ii)
        {
            psDemodCtx->u1PF = 0x01;
            psDemodCtx->fgCheckCR = TRUE;
            wFRTmp = *wFreqInKHz + 1800;
            bScanState = TUNER_PLL_LOCK_STATE;
        }     
        mcDELAY_MS(1);
        for(iii=0;iii<8;iii++)  //max 4 iteration per trial
        {
        if (psDemodCtx->isBreak(pvArg))
        {
            mcSHOW_DBG_MSG(("[ATuner] SearchNearby.BreakFct\n"));
            mcSET_BIT(bATVStatus, cpANA_LOCK_STS_BREAK);
            return bATVStatus;
        }
            if((0 == ii) && (0 == iii))
            {
                iii = 2;
            }
            switch(bScanState)
            {
                case TUNER_PLL_LOCK_STATE:
                    // Check if current freq is outside TV_FINE_TUNE_THRESHOLD window
                    mcSHOW_DBG_MSG(("[ATuner] Set Freq %d Bnad %d Itra %d \n",wFRTmp,ii,iii));
                    if ((*wFreqInKHz == u4FreqBDKHz) && ((wFRTmp > (*wFreqInKHz + TV_FINE_TUNE_THRESHOLD_KHZ/2 + 200) ) || (wFRTmp < (*wFreqInKHz - TV_FINE_TUNE_THRESHOLD_KHZ/2 - 200))))
                    {
                        //Hit upper bound or lower bound
                        if(ii<=1)
                        {
                            mcSHOW_DBG_MSG(("[ATuner] Hit upper bound or lower bound at 1st or 2nd trial\n"));
                            iii=7;
                            break;
                        }
                        else
                        {                
                            mcSHOW_DBG_MSG(("[ATuner] Hit upper bound or lower bound at 3rd trial\n"));
                            return (FALSE);
                        }       
                    }       
                    ATD_SetFreq(psDemodCtx, wFRTmp, TRUE);
                    ATD_StartAcq(psDemodCtx,TRUE);
                    bScanState = TUNER_VIF_LOCK_STATE;
                    break;

                case TUNER_VIF_LOCK_STATE:
                    mcDELAY_MS(10);
                    ATD_GetVIFLock(psDemodCtx,TRUE);
                    switch(psDemodCtx->u1VIFLock)
                    {
                        case 0:            
                            iii = 7;
                            break;
                              
                        case 1:
                            psDemodCtx->u1PF = 0x00;
                            psDemodCtx->fgCheckCR = FALSE;
                            s4NextFreqStep = (psDemodCtx->s4Cfo*1000)/(u2LOStep*10);
                            s4NextFreqStep *= ((u2LOStep*10)/1000);
                            mcSHOW_DBG_MSG(("s4CFO=%d KHz,NextStep=%d KHz\n",psDemodCtx->s4Cfo,s4NextFreqStep)); 
                            wFRTmp += s4NextFreqStep;
                            bScanState = TUNER_PLL_LOCK_STATE;
                            break;
                                
                        case 2:  //VIF lock
                            if((73250 == *wFreqInKHz) && (wFRTmp > 75000))
                            {
                                mcSHOW_DBG_MSG(("Got false CH1!\n"));
                                return (FALSE);
                            }

                            if((79250 == *wFreqInKHz) && (wFRTmp > 81000))
                            {
                                mcSHOW_DBG_MSG(("Got false CH5!\n"));
                                return (FALSE);
                            }

                            if (fgGetTVDSts(psDemodCtx,CHN_LOCK_CHECK_TIME)) //For MT5387 TVD export fucntion
                            {
                                TimingOffset = (mcGET_SYS_TICK() - u2TickStart_ATDLayer) * mcGET_TICK_PERIOD() - TimingOffset;
                                mcSHOW_DBG_MSG(("[ATuner] End SearchNearby with Frequency Offset = %d ms\n",TimingOffset));    
                                mcMSG_IN_SEARCHNEARBY_AUTO_TEST(u2LOStep)
                                return 3;
                            }
                            else
                            {
                                mcSHOW_DBG_MSG(("VIF lock, but TVD unlock!\n"));
                                iii = 7;
                            }     
                            break;  
                            
                        default:
                            break;
                    }
                    break;

                default:
                    break;
            } //end of state machine
        } //end of for loop iii
    } //end of for loop ii
    return (FALSE);
}


UINT8 ATDNA_Monitor(ATD_CTX_T *psDemodCtx, UINT32 *wFreqInKHz, x_break_fct _BreakFct, void *pvArg)
{
    UINT16  u2LOStep;
    INT16 s2FullVIFLockRange,s2PartiVIFLockRange;	
    UINT32 Freq=psDemodCtx->u4Frequency;
    UINT32 wFreqDR = *wFreqInKHz;
    UINT8  ucVpress;
#if defined(ATD_SNOWSCREEN_MODE)    
    UINT8 ucData;
    if(wPreLockFreq != *wFreqInKHz)
    {
    	mcSHOW_DBG_MSG(("[ATuner] 1st time Monitor\n"));
        u4FreqTemp = psDemodCtx -> u4Frequency;
    }
    wPreLockFreq = *wFreqInKHz;
#endif
	u2LOStep=psDemodCtx->pTCtx->specific_member.us_ctx.u2LO_Step;
	
	s2FullVIFLockRange=(INT16)(u2LOStep/100);
	if(s2FullVIFLockRange<cATD_VIF_FULL_LOCK_MIN_RANGE)
    {   
		s2FullVIFLockRange=cATD_VIF_FULL_LOCK_MIN_RANGE;
    }
	s2PartiVIFLockRange=2*s2FullVIFLockRange;

    //For Driver ATP Auto Test
    mcMSG_IN_MONITOR_AUTO_TEST(u2LOStep)
    psDemodCtx->isBreak = _BreakFct;
    
    if (psDemodCtx->isBreak(pvArg))
    {
        mcSHOW_DBG_MSG(("[ATuner] Monitor.BreakFct\n"));
        return (FALSE);
    }
    else if(!psDemodCtx->fgAFT)
    {
    	mcSHOW_DBG_MSG2(("Do not do AFT in monitor!\n"));
    	return TRUE;
    }
    else
    {
        psDemodCtx->fgCheckCR = TRUE;
        ucVpress = (IO_READ8(VIDEO_IN0_BASE, 0x81) & 0x10) >> 4;
         
        if(ATD_GetVIFLock(psDemodCtx, FALSE)<2)
        {
            if( (1 == psDemodCtx->u1VIFLock) && (psDemodCtx->s4Cfo > (-1*s2PartiVIFLockRange)) && (psDemodCtx->s4Cfo < s2PartiVIFLockRange) )
            {
                mcSHOW_DBG_MSG(("[ATuner] CFO > %d kHz but < %d kHz!\n",s2FullVIFLockRange,s2PartiVIFLockRange));
                return TRUE;
            }
            else if( (1 == psDemodCtx->u1VIFLock) && (0x01 == ucVpress) && (psDemodCtx->s4Cfo > (-3*s2PartiVIFLockRange)) && (psDemodCtx->s4Cfo < 3*s2PartiVIFLockRange) )
            {
                *wFreqInKHz = ((*wFreqInKHz+psDemodCtx->s4Cfo)/s2FullVIFLockRange) *s2FullVIFLockRange; // s2FullVIFLockRange = LO step in kHz
                ATD_SetFreq(psDemodCtx, *wFreqInKHz, TRUE); // directly tune to target freq according to CFO
                mcSHOW_DBG_MSG(("[ATuner] CFO > %d kHz but < %d kHz!, Tune to freq = %d kHz\n",s2PartiVIFLockRange*2,s2PartiVIFLockRange*3, *wFreqInKHz));
                return TRUE;
            }
            else if((psDemodCtx->s4Cfo > (-1*s2FullVIFLockRange)) && (psDemodCtx->s4Cfo < s2FullVIFLockRange) && (0x01 == ucVpress))
            {
                mcSHOW_DBG_MSG(("[ATuner] CFO < %d KHz, Vpress lock, poor pilot quality!\n",s2FullVIFLockRange));
                return TRUE;
            }            
            else
            {   
                mcSHOW_DBG_MSG(("[ATuner] VIF unlock!\n"));
                ATD_SetSystem(psDemodCtx, psDemodCtx->u1SubSysIdx);            	            
                ATD_SetFreq(psDemodCtx, *wFreqInKHz, TRUE);
                psDemodCtx->u1PF = 0x01;
                ATD_StartAcq(psDemodCtx, TRUE);
                psDemodCtx->fgCheckCR = TRUE;
                if(ATDNA_SearchNearbyFreq(psDemodCtx, &wFreqDR,_BreakFct, pvArg, *wFreqInKHz)==3)
                {
#if defined(ATD_SNOWSCREEN_MODE) 
                    mcSHOW_DBG_MSG(("[ATuner] New Freq = %dKhz\n", psDemodCtx -> u4Frequency));  
#endif
                    return TRUE;
                }
                else
                {
#if defined(ATD_SNOWSCREEN_MODE) 
                    if(u4FreqTemp != *wFreqInKHz)
                    {
                        ATD_SetFreq(psDemodCtx, u4FreqTemp, TRUE);  
                    }
                    else
                    {
                        ATD_SetFreq(psDemodCtx, *wFreqInKHz, TRUE);                    	    
                    }
                    psDemodCtx->u1PF = 0x00;
                    ucData = 0x24;                    
                    ATD_SetReg(psDemodCtx, REG_ATD_PE_CFOK, &ucData, 1);
                    ATD_StartAcq(psDemodCtx, FALSE);                    
                    mcSHOW_DBG_MSG(("[ATuner] AFT fail!\n"));
                    return FALSE;
#else
                    ATD_SetFreq(psDemodCtx, *wFreqInKHz, TRUE);                    	    
                    mcSHOW_DBG_MSG(("[ATuner] AFT fail!\n"));
                    return FALSE;
#endif
                }
            }
        }
        else
        {            
            #if defined(ATD_SNOWSCREEN_MODE) 
            u4FreqTemp = psDemodCtx -> u4Frequency;
            #endif
            mcSHOW_DBG_MSG2(("[ATuner] AFC in range!\n"));
            return TRUE;
        }
    }
}

