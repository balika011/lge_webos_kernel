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
 * $RCSfile: muxer_if.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_if.c
 *  Software program stream demux driver - public interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "muxer_if.h"
#include "muxer_drvif.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** MUXER_Init
 *  Initialize muxer
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
VOID MUXER_Init(VOID)
{
    _MUXER_Init();
}


//-----------------------------------------------------------------------------
/** MUXER_GetInst
 *  Get a Muxer instance
 *
 *  @retval MX_RET
 */
//-----------------------------------------------------------------------------
MX_RET MUXER_GetInst(UCHAR *pucMxId)
{
    ASSERT(pucMxId);
    return _MUXER_GetInst(pucMxId);
}


//-----------------------------------------------------------------------------
/** MUXER_ReleaseInst
 *  Release a Muxer instance
 *
 *  @retval MX_RET
 */
//-----------------------------------------------------------------------------
MX_RET MUXER_ReleaseInst(UCHAR ucMxId)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_ReleaseInst(ucMxId);
}


//-----------------------------------------------------------------------------
/** MUXER_Start
 *  Start to run muxer
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL MUXER_Start(UCHAR ucMxId)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_Start(ucMxId);
}


//-----------------------------------------------------------------------------
/** MUXER_Pause
 *  Pause muxer
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL MUXER_Pause(UCHAR ucMxId)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_Pause(ucMxId);
}


//-----------------------------------------------------------------------------
/** SWDMX_Stop
 *  Stop muxer
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL MUXER_Stop(UCHAR ucMxId)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_Stop(ucMxId);
}


//-----------------------------------------------------------------------------
/** SWDMX_Flush
 *  Flush A/V data
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL MUXER_Flush(UCHAR ucMxId)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_Flush(ucMxId);
}


//-----------------------------------------------------------------------------
/** MUXER_SendVideoPes
 *  Send Video Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL MUXER_SendVideoPes(UCHAR ucMxId, VENC_PES_INFO_T *prVEncPes)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SendVideoPes(ucMxId, prVEncPes);
}


//-----------------------------------------------------------------------------
/** MUXER_SendAudioPes
 *  Send Audio Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL MUXER_SendAudioPes(UCHAR ucMxId, AENC_PES_INFO_T *prAEncPes)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SendAudioPes(ucMxId, prAEncPes);
}


//-----------------------------------------------------------------------------
/** MUXER_SendVBIPes
 *  Send VBI Pes to Muxer
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL MUXER_SendVBIPes(UCHAR ucMxId, VBIENC_PES_INFO_T *prVBIEncPes)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SendVBIPes(ucMxId, prVBIEncPes);
}


//-----------------------------------------------------------------------------
/** MUXER_SetMuxFmt
 *  Set multiplexing format
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL MUXER_SetMuxFmt(UCHAR ucMxId, ENUM_MUXER_FMT_T eMuxFmt)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SetMuxFmt(ucMxId, eMuxFmt);
}


//-----------------------------------------------------------------------------
/** MUXER_SetVencFmt
 *  Set venc format
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL MUXER_SetVencFmt(UCHAR ucMxId, ENUM_MUXER_VENC_T eVidFmt)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SetVencFmt(ucMxId, eVidFmt);
}


//-----------------------------------------------------------------------------
/** MUXER_SetAencFmt
 *  Set aenc format
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL MUXER_SetAencFmt(UCHAR ucMxId, ENUM_MUXER_AENC_T eAudFmt)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SetAencFmt(ucMxId, eAudFmt);
}


BOOL MUXER_SetBuffer(UCHAR ucMxId, UINT32 u4BufSa, UINT32 u4BufSz, MX_FCT_NFY pfNotify, UINT32 u4NfyPeriod)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SetBuffer(ucMxId, u4BufSa, u4BufSz, pfNotify, u4NfyPeriod);
}


BOOL MUXER_UpdateRp(UCHAR ucMxId, UINT32 u4Rp)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_UpdateRp(ucMxId, u4Rp);
}


BOOL MUXER_AddStrm(UCHAR ucMxId, ENUM_MUXER_STRMTYPE_T eStrm, UINT32 *pu4StrmNo)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_AddStrm(ucMxId, eStrm, pu4StrmNo);
}


BOOL MUXER_DelStrm(UCHAR ucMxId, UINT32 u4StrmNo)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_DelStrm(ucMxId, u4StrmNo);
}


BOOL MUXER_SetScramble(UCHAR ucMxId, BOOL fgScramble)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SetScramble(ucMxId, fgScramble);
}


BOOL MUXER_SetScrambleKey(UCHAR ucMxId, MM_PVR_CRYPT_INFO_T *ptPvrCrypt)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_SetScrambleKey(ucMxId, ptPvrCrypt);
}


BOOL MUXER_Query(UCHAR ucMxId)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_Query(ucMxId);
}

BOOL MUXER_ConfigPid(UCHAR ucMxId, ENUM_MUXER_PID_TYPE_T ePidType, UINT16 u2Pid)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_ConfigPid(ucMxId, ePidType, u2Pid);
}

BOOL MUXER_AddSection(UCHAR ucMxId, MUXER_ADD_SECTION_INFO_T *ptAddSection)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_AddSection(ucMxId, ptAddSection);
}

BOOL MUXER_DelSection(UCHAR ucMxId, INT32 i4SectionId)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_DelSection(ucMxId, i4SectionId);
}

BOOL MUXER_UseTrustZone(UCHAR ucMxId, BOOL fgTrustZone)
{
    ASSERT(ucMxId < MUXER_MAX_INST_NS);
    return _MUXER_UseTrustZone(ucMxId, fgTrustZone);
}

