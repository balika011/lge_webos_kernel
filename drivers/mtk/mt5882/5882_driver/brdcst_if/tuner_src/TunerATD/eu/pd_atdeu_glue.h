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
 * $RCSfile: pd_atdeu_glue.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pd_anaeu_glue.h
 *  Header file of analog EU pd gule layer.
 */

#ifndef PD_ANAEU_GLUE_H
#define PD_ANAEU_GLUE_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "PI_Def.h"
#include "fe_tuner_common_if.h"
#include "pd_atd_glue.h"
#include "atd_feature.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#define fcTIMING_MEASURE        1

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//jackson need refine this. refer sv_const.h
#define SV_ON              1
#define SV_OFF             0

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#ifdef CC_FE_FAST_CHANNEL_CHANGE //To improve channel change timing, default is not defined, turn it on in drv_opt if needed
#define cCHECK_VIF_DELAY        1           // in ms
#else
#define cCHECK_VIF_DELAY        2           // in ms
#endif

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/
EXTERN BOOL _fgAutoSearch;//Yulia 2005.10.24
#if fcTIMING_MEASURE
EXTERN  UINT32  u2TickStart_eu;
#endif

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
UINT32 AtdEU_GetNoiseLevel(void);
UINT8 AtdEU_GetTVSystem(void);
UINT16 AtdEU_GetIFAGC(void);
BOOL AtdEU_SetLNA(BOOL fgLNA);
BOOL AtdEU_SetAntIn(UINT8 fgAntIn);
BOOL AtdEU_SetDeScrambleBox(BOOL fgScramble, UINT16 DelayForDescramble );
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
void AtdEU_AudSetSystem(UINT8 u1SubSysIdx, UINT8 u1ChMode);
#endif

//#if !ATD_SUPPORT_DRO_SYNC_MODE && !ATD_SUPPORT_H_BLANK_MODE
VOID ATD_DroTargetLevelModifySyncH(ATD_CTX_T *psDemodCtx, BOOL fgSel, UINT8 ucACC_MonCntSel, BOOL fgRegAuto, UINT32 u4TarSyncH, UINT16 u2DroTarLvlInitial);
VOID ATD_DroTargetLevelAdjust(ATD_CTX_T *psDemodCtx, UINT16 u2DroTarLvlInitial);
//#endif


#endif /* PD_ANAEU_GLUE_H */

