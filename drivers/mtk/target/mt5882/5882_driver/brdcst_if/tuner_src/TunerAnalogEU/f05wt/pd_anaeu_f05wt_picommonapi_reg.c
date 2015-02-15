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
 * $Author: p4admin $
 *
 *---------------------------------------------------------------------------*/
/** @file pd_glue.c
 *  This C file implements 5151 platform dependent glue layer.
 */
//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
//#include "pd_def_5133.h"
#include "pd_anaeu_picommonapi.h"
#include "pd_tuner_type.h"
#include "x_printf.h"
//#include "pi_tuner.h"
#include "PD_Def.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------
//#define fcFpgaMode  1 //asic 0

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
//static x_break_fct     	MT5151isBreak;


//---------------------------------------------------------------------------
// global variables
//---------------------------------------------------------------------------
//DEMOD_CTX_T				MT5151Ctx;

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------
//EXTERN S16 MTTX_SetFreq(GEN_TUNER_CTX_T *pTunerCtx, U32 Freq);
//extern void F05WT_Initialize(GEN_TUNER_CTX_T  *pTunerCtx);
//extern S16 F05WT_SetFreq(GEN_TUNER_CTX_T *pTunerCtx, U32 Freq, U8 Mode, U16 AutoSearch);
//extern void F05WT_SetSawBw(GEN_TUNER_CTX_T *pTunerCtx, U8 sawbw);
//extern S16   F05WT_SetIFDemod(GEN_TUNER_CTX_T* pTunerCtx, U8 SubSysIdx);
extern BOOL  F05WT_AnaTunerInit(ATV_PI_CTX_T *psAtvPiCtx);
extern VOID  F05WT_AnaTunerClose(ATV_PI_CTX_T *psAtvPiCtx);
extern BOOL  F05WT_AnaTunerSetFreq(ATV_PI_CTX_T *psAtvPiCtx, UINT32 wFreqInKHz, UINT8 u1Mode);
extern UINT8 F05WT_TunerGetSystem(ATV_PI_CTX_T *psAtvPiCtx);
extern BOOL  F05WT_TunerSetSystem(ATV_PI_CTX_T *psAtvPiCtx, UINT8 u1SubSysIdx);
extern UINT8 F05WT_bDrvTunerCheckVIFLock(ATV_PI_CTX_T *psAtvPiCtx);
extern UINT8 F05WT_AnaTunerSearchNearbyFreq(ATV_PI_CTX_T *psAtvPiCtx, UINT32 *wFreqInKHz, x_break_fct _BreakFct, VOID *pvArg, UINT32 u4FreqBDKHz);
extern UINT8 F05WT_MonitorAnaTuner(ATV_PI_CTX_T *psAtvPiCtx, UINT32 *wFreqInKHz, x_break_fct _BreakFct, VOID *pvArg);
extern UINT8 *F05WT_AnaTunerGetVer(void);
//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// public functions
//---------------------------------------------------------------------------

#if (fcTUNER_TYPE != cDVBT_TUNER_ALL)
void Tuner_Anaeu_PiCommonAPI_Register_Fct(ANAEU_PICOMMONAPI_FCT_TBL_T *pF05wt_Fct)
#else
void F05wt_Anaeu_PiCommonAPI_Register_Fct(ANAEU_PICOMMONAPI_FCT_TBL_T *pF05wt_Fct, UINT8 tunerType)
#endif
{

	mcSHOW_USER_MSG(("anaeu pi API registration: F05wt\r\n"));
    pF05wt_Fct->pf_AnalogPIEU_TunerInit             = F05WT_AnaTunerInit;
    pF05wt_Fct->pf_AnalogPIEU_TunerClose            = F05WT_AnaTunerClose;
    pF05wt_Fct->pf_AnalogPIEU_TunerSetFreq          = F05WT_AnaTunerSetFreq;
    pF05wt_Fct->pf_AnalogPIEU_TunerGetSystem        = F05WT_TunerGetSystem;
    pF05wt_Fct->pf_AnalogPIEU_TunerSetSystem        = F05WT_TunerSetSystem;
    pF05wt_Fct->pf_AnalogPIEU_TunerCheckVIFLock     = F05WT_bDrvTunerCheckVIFLock;
    pF05wt_Fct->pf_AnalogPIEU_TunerSearchNearbyFreq = F05WT_AnaTunerSearchNearbyFreq;
    pF05wt_Fct->pf_AnalogPIEU_TunerMonitor          = F05WT_MonitorAnaTuner;
    pF05wt_Fct->pf_AnalogPIEU_TunerGetVer          =  F05WT_AnaTunerGetVer;

}
