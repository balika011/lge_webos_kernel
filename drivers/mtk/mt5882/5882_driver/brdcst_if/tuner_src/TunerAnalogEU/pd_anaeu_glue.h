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
 * $RCSfile: pd_anaeu_glue.h,v $
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
#include "pi_anaeu_if.h"
#include "pd_anaeu_if.h"
#include "dvbt_tuner_if.h"
#include "tuner_interface_if.h"
#include "pd_tuner_type.h"
//#include "ctrl_bus.h"
#include "../CtrlBus/ctrl_bus.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#define fcTIMING_MEASURE        1

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//#if 0 //need refine this
//jackson need refine this. refer sv_const.h
#define SV_ON              1
#define SV_OFF             0
//#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#if 0
typedef struct _ATV_PD_CTX_T
{
// ATD PI context
    ATV_PI_CTX_T        sAtvPiCtx;
// Hardware Configure
    UINT32      Frequency;                  /* Carrier frequency (in Hz) */
    UINT8       u1VIFLock;
    UINT8       u1TunerAcqType;
// Glue-layer variables
    SEMA_T      hHalLock;
    UINT32      u4DrvCustTunerCtrl;
    UINT32      u4AcqFrequency;
    BOOL        fgBreakMonitor;
} ATV_PD_CTX_T, *PATV_PD_CTX_T;
#endif
#if 0 //need refine this
//jackson need refine this. refer sv_const.h
/* Video Path (VP) */
enum {
	SV_VP_MAIN,
	SV_VP_PIP,
	SV_VP_NA,
	SV_VP_MAX
};
//jackson need refine this. refer sv_const.h
/* signal type */
enum
{
	SV_ST_TV,
	SV_ST_AV,
	SV_ST_SV,
	SV_ST_YP,
	SV_ST_VGA,
	SV_ST_DVI,
	SV_ST_MPEG, // add for 5371
	SV_ST_MAX
};
//jackson need refine this. refer sv_const.h
/* Video Signal Status -- Add by Cindy */
enum VIDE_SIGNAL_STATUS_LIST
{
	SV_VDO_NOSIGNAL		= 0,
	SV_VDO_NOSUPPORT,
	SV_VDO_UNKNOWN,		// Still doing mode detect
	SV_VDO_STABLE
};
#endif
#if 0 //move to pi_anaeu_if.h
enum
{
    ATV_CONN_MODE_CHAN_CHANGE = 0,
    ATV_CONN_MODE_CHAN_SCAN,
    ATV_CONN_MODE_FINE_TUNE,
    ATV_CONN_MODE_NUM
};                                      
#endif
/***********************************************************************/
/*              TUNER_TYPE                                             */
/***********************************************************************/
//Jackson: all tuner type definition move to Tuner/pd_tuner_type.h
// analog and digital now share the same fcTUNER_TYPE definition
#if 0
#define cANA_PHILIPS_FQD1236    (0 + cANA_TUNER_BASE)
#define cANA_SAMSUNG_DTVS205    (1 + cANA_TUNER_BASE)
#define cANA_ALPS_TDQU2         (2 + cANA_TUNER_BASE)
#define cANA_MT5381ATD          (3 + cANA_TUNER_BASE)
#define cANA_SAMSUNG_DTVS203EH  (4 + cANA_TUNER_BASE)
#define cANA_SAMSUNG_DTVS227CH  (5 + cANA_TUNER_BASE)
#define cANA_ALPS_TDQU8         (6 + cANA_TUNER_BASE)
#define cANA_DUMMY              (7 + cANA_TUNER_BASE)
#define cANA_TUNER_ALL          ((cANA_E_TUNER_BASE / 2) - 1 + cANA_TUNER_BASE)

#define cANA_E_TUNER1           (0 + cANA_E_TUNER_BASE + cANA_TUNER_BASE)
#define cANA_THOMSON_FE664X     (0 + cANA_E_TUNER1)
#define cANA_SAMSUNG_DTOS40A    (1 + cANA_E_TUNER1)

#define cDEV_ANA_TUNER_BEGIN    ((cANA_TUNER_BASE / 2) + cANA_TUNER_BASE)
#define cDEV_ANA_E_TUNER_BEGIN  (cANA_E_TUNER_BASE + cDEV_ANA_TUNER_BEGIN)
#endif
//jackson need refine this. move to ana pi ?

//jackson need refine this. move to ana pi ?
enum
{
    eTUNER_AUTO_SCAN,
    eTUNER_AUTO_FINE_TUNE,
    eTUNER_SEMI_SCAN,
    eTUNER_UNI_AUTO_SCAN
};

//jackson need refine this. move from tvsys_info.h to here

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
//move from PI_ATuner.h
#define fcENABLE_MONITOR        0

#define fcTIMING_MEASURE        1
#define cCHECK_VIF_DELAY        2           // in ms
//#define cpANA_LOCK_STS_VIF      0     //move to pi_anaeu_if.h
//#define cpANA_LOCK_STS_TVD      1

/***********************************************************************/

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define cATUNER_CTX(psCtx)                          (&((psCtx)->sAtvPiCtx))
#define cATUNER_PI_CTX(psCtx)                       cATUNER_CTX(psCtx)
#define cATUNER_PD_CTX(psCtx)                       (psCtx)

/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/
EXTERN BOOL _fgAutoSearch;//Yulia 2005.10.24
#if fcTIMING_MEASURE
EXTERN  UINT32  u2TickStart_eu;
EXTERN  UINT8 ucSearchNearbyLoopCnt;
#endif

EXTERN  VOID    DigTunerBypassI2C(BOOL bSwitchOn);

//add by liuqu,20080626,for jump step size test
EXTERN INT16 GetJumpSmallStep(ITUNER_CTX_T* pTunerCtx);
EXTERN INT16 GetJumpMiddleStep(ITUNER_CTX_T* pTunerCtx,UINT8 fgScanDir);
EXTERN BOOL  GetVIFLBitCheckEnable(VOID);

VOID *GetDigiDvbtTunerCtx(VOID);
/***********************************************************************/
/*              Conditional Compiling Dependent Included Files         */
/***********************************************************************/
//#include "PD_ATuner.h"


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
//Jackson: todo: refine this, check whether this api have been used.
// need move to export API ?
//INT32 TunerNullBreak(VOID* pArg); 

#endif /* PD_ANAEU_GLUE_H */

