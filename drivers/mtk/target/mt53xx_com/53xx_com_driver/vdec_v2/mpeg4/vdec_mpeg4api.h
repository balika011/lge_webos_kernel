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
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_mpeg4api.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_mpeg4api.h
 *  This header file declares exported APIs of mpeg4.
 */

#ifndef VDEC_MPEG4API_H
#define VDEC_MPEG4API_H


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "vdec_mpeg4util.h"
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
#include "x_debug.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef enum
{
    MPEG4_DISP_IDX = 0,
    MPEG4_BS_TYPE,
    MPEG4_RRMODE,
    MPEG4_OUTPUT_DISP_ORDER,
    MPEG4_REPEAT_FRM,
    MPEG4_ECC_MODE,
    MPEG4_ECC_TYPE,
    MPEG4_DRAM_BANDWITH_SIMU,
    MPEG4_LOG_SIMU_PAT,
    MPEG4_WAIT_FB_TIME,
    MPEG4_AV_SYNC_INFO,
    MPEG4_TIME_PROFILE,
    MPEG4_TIME_PROFILE2,
    MPEG4_TIME_PROFILE3,
    MPEG4_MM_PARAM,
    MPEG4_SEQ_HDR_INFO
}   MPEG4_PARAM_T;


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

#if CC_MPEG4_UNUSED_FUNCTION
/** Get decoding handler
*@param .
 */
VDEC_HDLR _VDEC_Mpeg4GetHdlr(UCHAR ucEsId);
#endif

/** Initialize module for Mpeg4
*@param hVdec handler
 */
INT32 _VDEC_Mpeg4Init(UCHAR ucEsId);

/** MPEG4 SW reset
*@param hVdec handler
 */
void _VDEC_Mpeg4SWRst(UCHAR ucVdecId, UCHAR ucEsId);


/** MPEG4 header parsing function
*@param hVdec handler
*@param bRetValue flag for parsing finish, ready to enter decoding process
 */
INT32 _VDEC_Mpeg4PrsHdr(UCHAR ucVdecId, UCHAR ucEsId);

/** Mpeg4 decoder hardware config and decoding trigger
*@param hVdec handler
 */
INT32 _VDEC_Mpeg4DecStart(UCHAR ucVdecId, UCHAR ucEsId);

/** Mpeg4 wait decoding finish, display module setting / decoded data checking
*@param hVdec handler
 */
INT32 _VDEC_Mpeg4DecFin(UCHAR ucVdecId, UCHAR ucEsId);

/** Mpeg4 decode stop / freeze
*@param hVdec handler
 */
void _VDEC_Mpeg4Stop(UCHAR ucEsId);

/** get pes struct pointer
*@param .
 */
void _VDEC_GetMpeg4PesStruct(UCHAR ucEsId, void **ppvPesInfo);

/** get display picture addr
*@param .
 */
void _VDEC_GetMpeg4DispPic(UCHAR ucEsId, void* pvInfo);

#if CC_MPEG4_UNUSED_FUNCTION
/** Mpeg4 wait decoding finish, display module setting / decoded data checking
*@param hVdec handler
 */
void _VDEC_Mpeg4DumpCheckSum(void);
#endif

/** update dmx read ptr
*@param .
 */
void _VDEC_MPEG4UpdDmxRptr(UCHAR ucEsId);

void _VDEC_MPEG4SetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr);

#ifdef VDEC_TIME_PROFILE
void _VDEC_MPEG4GetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3);
#endif
/** set parameter
*@param .
 */
void _VDEC_Mpeg4SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3);

/** check if need skip es
*@param .
 */
BOOL _VDEC_Mpeg4EsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode);

void _VDEC_MPEG4DropFrame(UCHAR ucEsId);

VOID _VDEC_MPEG4StartPTS(UCHAR ucEsId, BOOL fgEnCalPTS, UINT32 u4Pts, UINT32 u4PtsDiff);

VOID _VDEC_MPEG4CalPts(UCHAR ucEsId, UCHAR ucFbId);

VOID _VDEC_MPEG4SetIsr(UCHAR ucEsId);

#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
BOOL _VDEC_MPEG4IsPic(UCHAR ucEsId, VDEC_PES_INFO_T *prPes);
#endif
#ifdef CC_SUPPORT_VDEC_PREPARSE
INT32 _VDEC_Mpeg4PrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId);
#endif
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


#endif //VDEC_MPEG4API_H

