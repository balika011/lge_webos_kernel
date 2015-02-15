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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_wmvutil.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_wmvutil.h
 *  This header file declares exported APIs of RealVideo.
 */

#ifndef VDEC_RVUTIL_H
#define VDEC_RVUTIL_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------




//vdec_rvutil.h
#include "x_lint.h"
#include "vdec_drvif.h"
#include "../vdec_debug.h"
LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//#define CC_RV_EMULATION

//#define WAIT_SRAM_STABLE

#define MC_PP_MODE  1

#define MAX_NUM_RPR_SIZES RV_MAX_NUM_RPR_SIZES

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef enum
{
    RV_INTRAPIC,        /* 0 (00) */
    RV_FORCED_INTRAPIC, /* 1 (01) */
    RV_INTERPIC,        /* 2 (10) */
    RV_TRUEBPIC         /* 3 (11) */
} EnumRVPicCodType;

typedef enum
{
  RPR_RAC_NOTHING,
  RPR_RAC_SCP2_DTCED,
  RPR_RAC_SCP1_PENDING,
  RPR_RAC_SCP1_DONE
} EnumRprRacStt;

typedef struct
{
  BOOL   fgRV9;  // TRUE -> RV9  FALSE -> RV8
  BOOL   fgBwdIsI;
  EnumRVPicCodType ePtype;
  UINT32 u4OrgWidth;
  UINT32 u4OrgHeight;
  UINT32 u4Width;
  UINT32 u4Height;
  UINT32 u4LineSize;
  UINT32 u4PctszSize;
  UINT32 u4Pctsz;
  UINT32 u4Pquant;
  UINT32 u4Oquant;
  UINT32 u4DFP;
  UINT32 u4Tr;
  UINT32 u4MbaSize;
  UINT32 u4Mba;
  UINT32 u4Rtype;
  UINT32 u4Iratio;
  UINT32 u4HdrSkip;
  UINT32 u4NumSlice;
  UINT32 au4SliceSize[128];
  UINT32 u4BstLength;
} RvPicAttr;

// Dram Related
typedef struct
{
  UINT32 u4DecBufIdx; //decode buffer index
  UINT32 u4BRefBufIdx; //reference frame1 buffer index
  UINT32 u4FRefBufIdx; //reference frame2 buffer index

  UINT32 u4ForwardRefPicType;
  UINT32 u4BackwardRefPicType;

  UINT8 *pu1VFifo; //fifo start
  UINT8 *pu1FifoEnd; // fifo end
  UINT8 *pu1Pic0Y; // Video Output Pic 0, Ref0
  UINT8 *pu1Pic0C; // Video Output Pic 0, Ref0
  UINT8 *pu1Pic1Y; // Video Output Pic 1, Ref1
  UINT8 *pu1Pic1C; // Video Output Pic 1, Ref1
  UINT8 *pu1Pic2Y; // Video Output Pic 2, B
  UINT8 *pu1Pic2C; // Video Output Pic 2, B
  UINT8 *pu1Pic3Y; // Video Output Pic 3, B
  UINT8 *pu1Pic3C; // Video Output Pic 3, B

  // working buffers
  UINT8 *pu1VldPred;
  UINT8 *pu1MvDirect;
} RVDram;

typedef struct
{
  UINT32 dwPtrY;
  UINT32 dwPtrC;
} TRVVSyncPrm;

// Other
typedef struct
{
  UINT32 u4DispBufIdx;
  UINT32 u4BBufIdx;

  BOOL fgEnCalPTS;
  BOOL fgFirstDispFrame;
  UINT32 u4FrameRate;
  UINT32 u4DeltaPTS;
  UINT32 u4LastPTS;

  //for fbm
  UINT8 u1FbgId;
  UINT8 u1FbId0;
  UINT8 u1FbId1;
  UINT8 u1FbId2;
  UINT8 u1GetSeqHdr;
  UINT8 u1GetPicHdr;

  INT32 i4SetPos;

  UINT32 u4TimeOutCnt;
  UINT32 u4DecErrCnt;
  UINT32 u4LastTimeOutPicNo;
  UINT32 u4LastDecErrPicNo;

  // for emulation
  UINT32 u4CurrPicNo;
  UINT32 u4FrminfoPtr;

  TRVVSyncPrm rVSyncPrm;
} RVOther;

typedef struct
{
  VDEC_PES_INFO_T rPesInfo;

  RvPicAttr rRvPicAttr;
  BOOL fgInited;
  RVDram rDram;
  RVOther rOther;
} VDEC_RV_INFO_T;


extern VDEC_RV_INFO_T _rRVDecInfo;


#define vSetRVDecBuf(arg)       _rRVDecInfo.rDram.u4DecBufIdx = (arg)
#define vSetRVBRefBuf(arg)      _rRVDecInfo.rDram.u4BRefBufIdx = (arg)
#define vSetRVFRefBuf(arg)      _rRVDecInfo.rDram.u4FRefBufIdx = (arg)


#endif //VDEC_RVUTIL_H
