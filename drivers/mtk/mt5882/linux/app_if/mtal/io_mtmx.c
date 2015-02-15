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
 * $RCSfile: io_mtmx.c,v $
 * $Revision: #1 $
 * $Author: p4admin $
 * $SWAuthor: wen han $
 * $MD5HEX:  $
 *---------------------------------------------------------------------------*/

/** @file io_mtmx.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtmx.h"
#include "mtutil.h"
#include <linux/mtal_ioctl.h>

#ifdef CC_SUPPORT_MUXER
#include "muxer_if.h"
#include "muxer_drvif.h"
#include "aud_drv.h"
#include "x_assert.h"
#include "x_rm.h"
#include "fbm_drvif.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

UINT32 g_muxer_buffer = 0;
UINT32 g_muxer_buffer_size = 0x00;

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct _MTMX_INT_DATA_T
{
    BOOL fgEnabled;
    UCHAR ucMxId;
    UINT16 u2VStrmId;
    UINT16 u2AStrmId;
    UINT16 u2VbiStrmId;
    UINT32 u4VStrmNo;
    UINT32 u4AStrmNo;
    UINT32 u4VbiStrmNo;
} MTMX_INT_DATA_T;


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

#ifdef CC_SUPPORT_VENC
extern VOID MTVENC_SetMxId(UINT16  u2Id, UCHAR ucMxId);
#endif
extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static MTMX_INT_DATA_T  _arMtmxData[MTMX_COMP_NS];


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static MTMX_INT_DATA_T * get_mtmx_ins(UCHAR ucMxId)
{
    ASSERT(MTMX_COMP_NS > ucMxId);
    return (MTMX_COMP_NS > ucMxId)? &_arMtmxData[ucMxId] : &_arMtmxData[0];
}


static VOID _mtmx_data_nfy(UCHAR ucMxId, MX_CB_DATA_T *prCbData)
{
    MTMX_CB_T tMxCb;
    MTMX_BUF_DATA_T *ptBufData = &tMxCb.u.tBufData;
    tMxCb.eCond           = MTMX_COND_DATA;
    tMxCb.u4Id            = ucMxId;
    ptBufData->pui1BufPhy = (UINT8*)PHYSICAL(prCbData->u4Addr);
    ptBufData->u4Offset   = (UINT8*)PHYSICAL(prCbData->u4Addr) - (UINT8*)g_muxer_buffer;
    ptBufData->u4Size     = prCbData->u4Len;
    ptBufData->u8Pts      = (UINT64)prCbData->u4Pts;
    ptBufData->u4Flag     = prCbData->u4Flag;
    _CB_PutEvent(CB_MTAL_MX_DATA_NFY, sizeof(tMxCb), &tMxCb);
}


static VOID _mtmx_overflow_nfy(UCHAR ucMxId, MX_OVERFLOW_REASON_T eType)
{
    MTMX_CB_T tMxCb;
    MTMX_OVERFLOW_REASON_T eOverflowReason = MTMX_OVERFLOW_REASON_UNKNOWN;

    switch (eType)
    {
    default:
        MTMX_PRINT("unknown type %d\n", eType);
        break;

    case MX_OVERFLOW_REASON_RING_BUF_FULL:
        eOverflowReason = MTMX_OVERFLOW_REASON_RING_BUF_FULL;
        break;

    case MX_OVERFLOW_REASON_METADATA_BUF_FULL:
        eOverflowReason = MTMX_OVERFLOW_REASON_METADATA_BUF_FULL;
        break;
    }

    tMxCb.eCond = MTMX_COND_OVERFLOW;
    tMxCb.u4Id = ucMxId;
    tMxCb.u.eOverflowReason = eOverflowReason;
    _CB_PutEvent(CB_MTAL_MX_DATA_NFY, sizeof(tMxCb), &tMxCb);
}


static VOID _mtmx_error_nfy(UCHAR ucMxId, MX_ERROR_REASON_T eType)
{
    MTMX_CB_T tMxCb;
    MTMX_ERROR_REASON_T eErrorReason;

    switch (eType)
    {
    default:
        MTMX_PRINT("unknown type %d\n", eType);
        break;

    case MX_ERROR_REASON_INTERNAL:
        eErrorReason = MTMX_ERROR_REASON_INTERNAL;
        break;
    }

    tMxCb.eCond = MTMX_COND_ERROR;
    tMxCb.u4Id = ucMxId;
    tMxCb.u.eErrorReason = eErrorReason;
    _CB_PutEvent(CB_MTAL_MX_DATA_NFY, sizeof(tMxCb), &tMxCb);
}


static VOID _mtmx_nfy(UCHAR ucMxId, MX_NFY_COND_T eType, VOID *pvData)
{
    switch (eType)
    {
    default:
        MTMX_PRINT("unknown type %d\n", eType);
        break;

    case MX_COND_DATA:
        _mtmx_data_nfy(ucMxId, (MX_CB_DATA_T *)pvData);
        break;

    case MX_COND_OVERFLOW:
        _mtmx_overflow_nfy(ucMxId, (MX_OVERFLOW_REASON_T)pvData);
        break;

    case MX_COND_ERROR:
        _mtmx_error_nfy(ucMxId, (MX_ERROR_REASON_T)pvData);
        break;
    }
}


static MT_RESULT_T _mtmx_connect(
    MTMX_INT_DATA_T *prMtmxData,
    MTMX_SRC_TYPE_T  eSrcType,
    UINT16           u2CompId)
{
    UCHAR ucMxId = prMtmxData->ucMxId;

    if (prMtmxData->fgEnabled)
    {
        // disable
        VERIFY(MUXER_Stop(ucMxId));
        prMtmxData->fgEnabled = FALSE;
    }

    switch (eSrcType)
    {
    default:
        MTMX_PRINT("unknown src type %d\n", eSrcType);
        return MTR_NOT_OK;

    case MTMX_SRC_TYPE_VENC:
        if (MUXER_INVALID_STRM_ID != prMtmxData->u4VStrmNo)
        {
            VERIFY(MUXER_DelStrm(ucMxId, prMtmxData->u4VStrmNo));
            prMtmxData->u4VStrmNo = MUXER_INVALID_STRM_ID;
        }
        VERIFY(MUXER_AddStrm(ucMxId, eMUXER_STRMTYPE_VIDEO, &prMtmxData->u4VStrmNo));
        prMtmxData->u2VStrmId = u2CompId;
        MTMX_PRINT("video id is %d\n", u2CompId);
        break;

    case MTMX_SRC_TYPE_AENC:
        if (MUXER_INVALID_STRM_ID != prMtmxData->u4AStrmNo)
        {
            VERIFY(MUXER_DelStrm(ucMxId, prMtmxData->u4AStrmNo));
            prMtmxData->u4AStrmNo = MUXER_INVALID_STRM_ID;
        }
        VERIFY(MUXER_AddStrm(ucMxId, eMUXER_STRMTYPE_AUDIO, &prMtmxData->u4AStrmNo));
        prMtmxData->u2AStrmId = u2CompId;
        MTMX_PRINT("audio id is %d\n", u2CompId);
        break;

    case MTMX_SRC_TYPE_VBI:
        if (MUXER_INVALID_STRM_ID != prMtmxData->u4VbiStrmNo)
        {
            VERIFY(MUXER_DelStrm(ucMxId, prMtmxData->u4VbiStrmNo));
            prMtmxData->u4VbiStrmNo = MUXER_INVALID_STRM_ID;
        }
        VERIFY(MUXER_AddStrm(ucMxId, eMUXER_STRMTYPE_VBI, &prMtmxData->u4VbiStrmNo));
        prMtmxData->u2VbiStrmId = u2CompId;
        MTMX_PRINT("vbi id is %d\n", u2CompId);
        break;
    }

    MTMX_PRINT("connect (%d, %d) successfully! \n", eSrcType, u2CompId);

    return MTR_OK;
}


static MT_RESULT_T _mtmx_disconn(
    MTMX_INT_DATA_T *prMtmxData,
    MTMX_SRC_TYPE_T  eSrcType,
    UINT16           u2CompId)
{
    UCHAR ucMxId = prMtmxData->ucMxId;

    if (prMtmxData->fgEnabled)
    {
        // disable
        VERIFY(MUXER_Stop(ucMxId));
        prMtmxData->fgEnabled = FALSE;
    }

    switch (eSrcType)
    {
    default:
        MTMX_PRINT("unknown src type %d\n", eSrcType);
        return MTR_NOT_OK;

    case MTMX_SRC_TYPE_VENC:
        ASSERT(MUXER_INVALID_STRM_ID != prMtmxData->u4VStrmNo);
        VERIFY(MUXER_DelStrm(ucMxId, prMtmxData->u4VStrmNo));
        prMtmxData->u4VStrmNo = MUXER_INVALID_STRM_ID;
        ASSERT(u2CompId == prMtmxData->u2VStrmId);
        prMtmxData->u2VStrmId = MAX_COMP_ID;
        break;

    case MTMX_SRC_TYPE_AENC:
        ASSERT(MUXER_INVALID_STRM_ID != prMtmxData->u4AStrmNo);
        VERIFY(MUXER_DelStrm(ucMxId, prMtmxData->u4AStrmNo));
        prMtmxData->u4AStrmNo = MUXER_INVALID_STRM_ID;
        ASSERT(u2CompId == prMtmxData->u2AStrmId);
        prMtmxData->u2AStrmId = MAX_COMP_ID;
        break;

    case MTMX_SRC_TYPE_VBI:
        ASSERT(MUXER_INVALID_STRM_ID != prMtmxData->u4VbiStrmNo);
        VERIFY(MUXER_DelStrm(ucMxId, prMtmxData->u4VbiStrmNo));
        prMtmxData->u4VbiStrmNo= MUXER_INVALID_STRM_ID;
        ASSERT(u2CompId == prMtmxData->u2VbiStrmId);
        prMtmxData->u2VbiStrmId = MAX_COMP_ID;
        break;
    }

    MTMX_PRINT("disconnect (%d, %d) successfully! \n", eSrcType, u2CompId);

    return MTR_OK;
}


static MT_RESULT_T _MTMX_Init(void)
{
    UINT16 u2CompId;
    MTMX_PRINT(" - void\n");

    for (u2CompId = 0; u2CompId < MTMX_COMP_NS; u2CompId++)
    {
        MTMX_INT_DATA_T *prMtmxData = get_mtmx_ins(u2CompId);
        prMtmxData->ucMxId = u2CompId;
        prMtmxData->u2VStrmId = MAX_COMP_ID;
        prMtmxData->u2AStrmId = MAX_COMP_ID;
        prMtmxData->u2VbiStrmId = MAX_COMP_ID;
        prMtmxData->u4VStrmNo = MUXER_INVALID_STRM_ID;
        prMtmxData->u4AStrmNo = MUXER_INVALID_STRM_ID;
        prMtmxData->u4VbiStrmNo = MUXER_INVALID_STRM_ID;
    }

    MUXER_Init();

    return MTR_OK;
}


static MT_RESULT_T _MTMX_Termint(void)
{
    MTMX_PRINT(" - void\n");
    //add driver implementation code here
    return MTR_OK;
}


static MT_RESULT_T _MTMX_Reset(unsigned long arg)
{
    UINT16 u2CompId;
    MTMX_INT_DATA_T *prMtmxData;
    u2CompId = (UINT16)arg;
    MTMX_PRINT(" - u2CompId = %d\n",(int)u2CompId);

    prMtmxData = get_mtmx_ins(u2CompId);

    if (prMtmxData->fgEnabled)
    {
        // disable
        VERIFY(MUXER_Stop(prMtmxData->ucMxId));
        prMtmxData->fgEnabled = FALSE;
    }

    prMtmxData->ucMxId = u2CompId;
    prMtmxData->u2VStrmId = MAX_COMP_ID;
    prMtmxData->u2AStrmId = MAX_COMP_ID;
    prMtmxData->u2VbiStrmId = MAX_COMP_ID;
    prMtmxData->u4VStrmNo = MUXER_INVALID_STRM_ID;
    prMtmxData->u4AStrmNo = MUXER_INVALID_STRM_ID;
    prMtmxData->u4VbiStrmNo = MUXER_INVALID_STRM_ID;

    return MTR_OK;
}


static MT_RESULT_T _MTMX_Set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT16             u2CompId;
    MTMX_SET_TYPE_T  eSetType;
    const VOID*        pvData;
    UINT32             u4Size;
    MTMX_INT_DATA_T *prMtmxData;
    MT_RESULT_T eRet = MTR_NOT_OK;
    UCHAR ucMxId;
    INT32 i4SectionId;
    BOOL fgTrustZone = FALSE;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u2CompId = (UINT16)rArg.ai4Arg[0];
    eSetType = (MTMX_SET_TYPE_T)rArg.ai4Arg[1];
    pvData = (VOID*)rArg.ai4Arg[2];
    u4Size = (UINT32)rArg.ai4Arg[3];

    MTMX_PRINT(" - u2CompId = %d, eSetType = %d\n",
        (int)u2CompId, (int)eSetType);

    prMtmxData = get_mtmx_ins(u2CompId);
    ucMxId = prMtmxData->ucMxId;

    switch (eSetType)
    {
    default:
        MTMX_PRINT("unknown set type %d\n", eSetType);
        eRet = MTR_NOT_OK;
        break;

    case MTMX_SET_TYPE_CONNECT_SRC:
        MTMX_SET_VALIDATE(MTMX_SRC_INFO_T);
        {
            MTMX_SRC_INFO_T rSrcInfo;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MTMX_SRC_INFO_T);
            COPY_FROM_USER_ARG(pvData, rSrcInfo, MTMX_SRC_INFO_T);
            eRet = _mtmx_connect(prMtmxData, rSrcInfo.eSrcType, rSrcInfo.u2CompId);
        }
        break;
    case MTMX_SET_TYPE_DISCONNECT_SRC:
        MTMX_SET_VALIDATE(MTMX_SRC_INFO_T);
        {
            MTMX_SRC_INFO_T rSrcInfo;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MTMX_SRC_INFO_T);
            COPY_FROM_USER_ARG(pvData, rSrcInfo, MTMX_SRC_INFO_T);
            eRet = _mtmx_disconn(prMtmxData, rSrcInfo.eSrcType, rSrcInfo.u2CompId);
        }
        break;
    case MTMX_SET_TYPE_CTRL:
        if ((UINT32)pvData)
        {
            // enable
            if (MUXER_INVALID_STRM_ID != prMtmxData->u4VStrmNo)
            {
#ifdef CC_SUPPORT_VENC
                MTVENC_SetMxId(prMtmxData->u2VStrmId, ucMxId);
#endif
                MUXER_SetVencFmt(ucMxId, ENUM_MUXER_VENC_H264); // TODO: remove this hardcode
            }

            if (MUXER_INVALID_STRM_ID != prMtmxData->u4AStrmNo)
            {
#if 0
                /* if want to remove hard code, please use below code,
                    but it seems that audio codec is not ready when mx ctrl, need further debug */
                AUD_FMT_T eDecType;
                UINT8 u1DecId=2; // 96 => 2, 98 => 3
                eDecType = _AudGetStrFormat(u1DecId);
                LOG(0, "u1DecId=%d, eDecType=%d\n", u1DecId, eDecType);
                //if (eDecType == AUD_FMT_MP3ENC)
#endif
                AUD_SetMuxId(ucMxId);
                //MUXER_SetAencFmt(ucMxId, ENUM_MUXER_AENC_MP3); // TODO: remove this hardcode
            }
            VERIFY(MUXER_Start(ucMxId));
            prMtmxData->fgEnabled = TRUE;
        }
        else
        {
            // disable
            VERIFY(MUXER_Stop(ucMxId));
            prMtmxData->fgEnabled = FALSE;
        }
        eRet = MTR_OK;
        break;
#if 0 // MTMX_SET_TYPE_NFY_FCT is handled at user space
    case MTMX_SET_TYPE_NFY_FCT:
        MTMX_SET_VALIDATE(MTMX_NFY_INFO_T);
        {
            MTMX_NFY_INFO_T *prNfyInfo = (MTMX_NFY_INFO_T *)pvData;
            ASSERT(prNfyInfo->pvTag);
            ASSERT(prNfyInfo->pfNfy);
            prMtmxData->pv_mw_mx_nfy_tag = prNfyInfo->pvTag;
            prMtmxData->pf_mw_mx_nfy_fct = prNfyInfo->pfNfy;
        }
        eRet = MTR_OK;
        break;
#endif
    case MTMX_SET_TYPE_BUFFER_SETTING:
        MTMX_SET_VALIDATE(MTMX_BUF_SETTING_T);
        {
            MTMX_BUF_SETTING_T rBufSetting;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MTMX_BUF_SETTING_T);
            COPY_FROM_USER_ARG(pvData, rBufSetting, MTMX_BUF_SETTING_T);

            switch (rBufSetting.eType)
            {
            default:
                return MTR_NOT_OK;

            case MTMX_DATA_TYPE_MTK:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufMtk.pvBufPhy)), rBufSetting.u.tBufMtk.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufMtk.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_CMPB));
                break;

            case MTMX_DATA_TYPE_TS:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufTs.pvBufPhy)), rBufSetting.u.tBufTs.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufTs.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_MPEG2_TS));
                break;

            case MTMX_DATA_TYPE_TS_192:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufTs.pvBufPhy)), rBufSetting.u.tBufTs.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufTs.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_MPEG2_TS_192));
                break;

            case MTMX_DATA_TYPE_VIDEO_ES:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufVid.pvBufPhy)), rBufSetting.u.tBufVid.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufVid.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_RAW));
                break;

            case MTMX_DATA_TYPE_AUDIO_ES:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufAud.pvBufPhy)), rBufSetting.u.tBufAud.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufAud.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_RAW));
                break;
            }
        }
        eRet = MTR_OK;
        break;

    case MTMX_SET_TYPE_BUFFER_ALLOC:
        MTMX_SET_VALIDATE(MTMX_BUF_SETTING_T);
        {
            MTMX_BUF_SETTING_T rBufSetting;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MTMX_BUF_SETTING_T);
            COPY_FROM_USER_ARG(pvData, rBufSetting, MTMX_BUF_SETTING_T);
    
            // alloc buffer
            if (g_muxer_buffer == 0)
            {
				FBM_POOL_T  *prFbmPool = NULL;
				prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_PVR);

			    if (!prFbmPool || !prFbmPool->u4Addr || !prFbmPool->u4Size)
                {
			        Printf("ERROR: Get PVR Pool fail.\n");
			        return MTR_NOT_OK;
                }

				g_muxer_buffer = prFbmPool->u4Addr;
				g_muxer_buffer_size = prFbmPool->u4Size;
            }
            
            rBufSetting.u.tBufMtk.pvBufPhy  = (VOID *)g_muxer_buffer;
            rBufSetting.u.tBufMtk.u4BufSize = g_muxer_buffer_size;
            switch (rBufSetting.eType)
            {
            default:
                return MTR_NOT_OK;
    
            case MTMX_DATA_TYPE_MTK:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufMtk.pvBufPhy)), rBufSetting.u.tBufMtk.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufMtk.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_CMPB));
                break;
    
            case MTMX_DATA_TYPE_TS:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufTs.pvBufPhy)), rBufSetting.u.tBufTs.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufTs.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_MPEG2_TS));
                break;
    
            case MTMX_DATA_TYPE_TS_192:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufTs.pvBufPhy)), rBufSetting.u.tBufTs.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufTs.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_MPEG2_TS_192));
                break;
    
            case MTMX_DATA_TYPE_VIDEO_ES:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufVid.pvBufPhy)), rBufSetting.u.tBufVid.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufVid.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_RAW));
                break;
    
            case MTMX_DATA_TYPE_AUDIO_ES:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(rBufSetting.u.tBufAud.pvBufPhy)), rBufSetting.u.tBufAud.u4BufSize, _mtmx_nfy, rBufSetting.u.tBufAud.u4TickPeriod));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_RAW));
                break;
            }
        }
        eRet = MTR_OK;
        break;

    case MTMX_SET_TYPE_UNLOCK_BUFFER:
        MTMX_SET_VALIDATE(MTMX_UNLOCK_BUFFER_INFO_T);
        {
            MTMX_UNLOCK_BUFFER_INFO_T rUnlockBuf;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MTMX_UNLOCK_BUFFER_INFO_T);
            COPY_FROM_USER_ARG(pvData, rUnlockBuf, MTMX_UNLOCK_BUFFER_INFO_T);

            MTMX_PRINT("MTMX_SET_TYPE_UNLOCK_BUFFER, u4Offset(0x%08x), rUnlockBuf.u4DataLen(%d)\n", rUnlockBuf.u4Offset, rUnlockBuf.u4DataLen);
            if(rUnlockBuf.u4DataLen > 0)
            {
                if(g_muxer_buffer != 0)
                {
                    rUnlockBuf.pui1DataAddr = (UINT8*)g_muxer_buffer + rUnlockBuf.u4Offset;
                }
                VERIFY(MUXER_UpdateRp(ucMxId, VIRTUAL((UINT32)(rUnlockBuf.pui1DataAddr + rUnlockBuf.u4DataLen - 1))));
            }
        }
        eRet = MTR_OK;
        break;

    case MTMX_SET_TYPE_FLUSH_BUFFER:
        if (MUXER_Flush(ucMxId))
        {
            eRet = MTR_OK;
        }
        else
        {
            eRet = MTR_NOT_OK;
        }
        break;

    case MTMX_SET_TYPE_ENCRYPT_KEY:
        MTMX_SET_VALIDATE(MM_CRYPT_INFO_T);
        {
            MM_CRYPT_INFO_T rCryptInfo;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MM_CRYPT_INFO_T);
            COPY_FROM_USER_ARG(pvData, rCryptInfo, MM_CRYPT_INFO_T);
            VERIFY(MUXER_SetScrambleKey(ucMxId, &rCryptInfo.u.t_pvr_crypt));
        }
        eRet = MTR_OK;
        break;

    case MTMX_SET_TYPE_ENCRYPT_CTRL:
        VERIFY(MUXER_SetScramble(ucMxId, (((UINT32)pvData)? TRUE : FALSE)));
        eRet = MTR_OK;
        break;

    case MTMX_SET_TYPE_ADD_SECTION:
        MTMX_SET_VALIDATE(MUXER_ADD_SECTION_INFO_T);
        {
            MUXER_ADD_SECTION_INFO_T rAddSection;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MUXER_ADD_SECTION_INFO_T);
            COPY_FROM_USER_ARG(pvData, rAddSection, MUXER_ADD_SECTION_INFO_T);
            VERIFY(MUXER_AddSection(ucMxId, &rAddSection));
        }
        eRet = MTR_OK;
        break;
        
    case MTMX_SET_TYPE_DELETE_SECTION:
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, INT32);
            COPY_FROM_USER_ARG(pvData, i4SectionId, INT32);
            VERIFY(MUXER_DelSection(ucMxId, i4SectionId));
        }
        eRet = MTR_OK;
        break;

    case MTMX_SET_TYPE_USE_TRUSTZONE:
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, BOOL);
            COPY_FROM_USER_ARG(pvData, fgTrustZone, BOOL);
            VERIFY(MUXER_UseTrustZone(ucMxId, fgTrustZone));
        }
        eRet = MTR_OK;
        break;

    case MTMX_SET_TYPE_CONFIG_PID:    
        MTMX_SET_VALIDATE(ENUM_MUXER_PID_TYPE_T);
        {
            ENUM_MUXER_PID_TYPE_T ePidType;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData, ENUM_MUXER_PID_TYPE_T);
            COPY_FROM_USER_ARG(pvData, ePidType, ENUM_MUXER_PID_TYPE_T);
            VERIFY(MUXER_ConfigPid(ucMxId, ePidType, (UINT16)u4Size));
        }
		eRet = MTR_OK;
        break;
    }

    return eRet;
}


static MT_RESULT_T _MTMX_Get(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    UINT16              u2CompId;
    MTMX_GET_TYPE_T   eGetType;
    VOID*               pvData;
    UINT32*             pu4Size;
    MTMX_INT_DATA_T *prMtmxData;
    MT_RESULT_T eRet;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u2CompId = (UINT16)rArg.ai4Arg[0];
    eGetType = (MTMX_GET_TYPE_T)rArg.ai4Arg[1];
    pvData = (VOID*)rArg.ai4Arg[2];
    pu4Size = (UINT32 *)rArg.ai4Arg[3];

    MTMX_PRINT("_MTMX_Get - u2CompId = %d, eGetType = %d\n",
        (int)u2CompId, (int)eGetType);

    prMtmxData = get_mtmx_ins(u2CompId);
    switch (eGetType)
    {
    default:
        eRet = MTR_NOT_OK;
        break;

    case MTMX_GET_TYPE_CTRL:
        if (pvData == NULL)
        {
            eRet = MTR_NOT_OK;
        }
        else
        {
            BOOL fgCtrl = prMtmxData->fgEnabled;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,fgCtrl);
            COPY_TO_USER_ARG(pvData,fgCtrl,fgCtrl);
            eRet = MTR_OK;
        }
        break;

    case MTMX_GET_TYPE_BUFFER:
    	{
    		UINT32 u4PoolAddr;
    		UINT32 u4PoolSize;
    		UINT32* pu4Addr;
    		UINT32* pu4Size;
    		FBM_POOL_T *prFbmPool;

    		pu4Addr = (UINT32*)rArg.ai4Arg[2];
    		pu4Size = (UINT32*)rArg.ai4Arg[3];


    		if (pu4Addr == NULL || pu4Size == NULL )
            {
                eRet = MTR_NOT_OK;
    			MTMX_PRINT("_MTMX_Get -  pu4Addr = 0x%08x, pu4Addr = 0x%08x\n", pu4Addr, pu4Size);
            }
    		else
    		{
    			prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_PVR);
    			ASSERT(prFbmPool != (FBM_POOL_T *)NULL);
    			ASSERT(prFbmPool->u4Addr != 0);
    			
    			u4PoolAddr = prFbmPool->u4Addr;
    			u4PoolSize = prFbmPool->u4Size;

    			MTMX_PRINT("_MTMX_Get - u4PoolAddr = %d, u4PoolSize = %d\n",
    				(int)(prFbmPool->u4Addr), (int)(prFbmPool->u4Size));

    			USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr,UINT32);
    		    COPY_TO_USER_ARG(pu4Addr,u4PoolAddr,UINT32);
    		    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size,UINT32);
    		    COPY_TO_USER_ARG(pu4Size,u4PoolSize,UINT32);

    			eRet = MTR_OK;
    		}
    	}
        break;
    }

    return eRet;
}

static MT_RESULT_T _MTMX_CopyData(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T   rArg;
    UINT16              u2CompId;
    UINT32              u4UserAddr;
    UINT32              u4PhyAddr;
    UINT32              u4Size;
    UINT32              u4StartAddr;
    UINT32              u4EndAddr;
    UINT32              u4CopySize;
    UINT32              u4VirAddr;
    INT32               i4Ret = 0;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u2CompId   = (UINT16)rArg.ai4Arg[0];
    u4UserAddr = rArg.ai4Arg[1];
    u4PhyAddr  = rArg.ai4Arg[2];
    u4Size     = rArg.ai4Arg[3];

    if (g_muxer_buffer == 0x00)
    {
        return MTR_NOT_OK;
    }
    
    u4StartAddr = PHYSICAL(g_muxer_buffer);
    u4EndAddr   = PHYSICAL(g_muxer_buffer) + 4 * 1024 * 1024;

    MTMX_PRINT(" - u2CompId = %d, u4UserAddr = 0x%08x, u4PhyAddr = 0x%08x, u4Size = %d, u4StartAddr = 0x%08x, u4EndAddr = 0x%08x\n",
        (int)u2CompId, u4UserAddr, u4PhyAddr, u4Size, u4StartAddr, u4EndAddr);

    if ((u4PhyAddr + u4Size) > u4EndAddr)
    {
        u4CopySize = u4EndAddr - u4PhyAddr;
        u4VirAddr = VIRTUAL(u4PhyAddr);
        HalInvalidateDCacheMultipleLine(u4VirAddr,u4CopySize);
        i4Ret = copy_to_user((void __user *)u4UserAddr,(void *)u4VirAddr, u4CopySize);
        if(i4Ret)
        {
           printk("%s copy_to_user error1\n", __FUNCTION__);
           return MTR_ERR_INV;
        }

        u4UserAddr += u4CopySize;
        u4CopySize = u4Size - u4CopySize;
        u4VirAddr = VIRTUAL(u4StartAddr);
        HalInvalidateDCacheMultipleLine(u4VirAddr, u4CopySize);
        i4Ret = copy_to_user((void __user *)u4UserAddr, (void *)u4VirAddr, u4CopySize);
        if (i4Ret)
        {
           printk("%s copy_to_user error2\n", __FUNCTION__);
           return MTR_ERR_INV;
        }
    }
    else
    {
        u4CopySize = u4Size;
        u4VirAddr = VIRTUAL(u4PhyAddr);
        HalInvalidateDCacheMultipleLine(u4VirAddr, u4CopySize);
        i4Ret = copy_to_user((void __user *)u4UserAddr, (void *)u4VirAddr, u4CopySize);
        if (i4Ret)
        {
            printk("%s copy_to_user error3\n", __FUNCTION__);
            return MTR_ERR_INV;
        }
    }

    return MTR_OK;
}

static MT_RESULT_T _MTMX_SetCodec(unsigned long arg)
{
    UCHAR                ucMxId;
    MTAL_IOCTL_4ARG_T    rArg;
    UINT16               u2CompId;
    MTMX_SRC_TYPE_T      eSrcType;
    UINT32               u4VCodecType;
    UINT32               u4ACodecType;
    MT_RESULT_T          eRet           = FALSE;
    ENUM_MUXER_VENC_T    eVidFmt        = ENUM_MUXER_VENC_UNKNOWN;
    ENUM_MUXER_AENC_T    eAudFmt        = ENUM_MUXER_AENC_UNKNOWN;
    
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u2CompId = (UINT16)rArg.ai4Arg[0];
    eSrcType = (MTMX_SRC_TYPE_T)rArg.ai4Arg[1];
    u4VCodecType = (UINT32)rArg.ai4Arg[2];
    u4ACodecType = (UINT32)rArg.ai4Arg[3];

    ucMxId = get_mtmx_ins(u2CompId)->ucMxId;
    
    if(MTMX_SRC_TYPE_VENC == eSrcType || MTMX_SRC_TYPE_AV == eSrcType )
    {
        do
        {
            if(MTMX_VENC_H264 == (MTMX_VENC_T)u4VCodecType)
            {
                eVidFmt = ENUM_MUXER_VENC_H264;     
            }
            else
            {
                break;
            }
            eRet = MUXER_SetVencFmt(ucMxId, eVidFmt);
        }while(0);
    }

    if(MTMX_SRC_TYPE_AENC == eSrcType || MTMX_SRC_TYPE_AV == eSrcType )
    {
        do
        {
            if(MTMX_AENC_AAC == (MTMX_AENC_T)u4ACodecType)
            {
                eAudFmt = ENUM_MUXER_AENC_AAC;     
            }
            else if(MTMX_AENC_MP3 == (MTMX_AENC_T)u4ACodecType)
            {
                eAudFmt = ENUM_MUXER_AENC_MP3;     
            }
            else if(MTMX_AENC_PCM == (MTMX_AENC_T)u4ACodecType)
            {
                eAudFmt = ENUM_MUXER_AENC_PCM;     
            }
            else
            {
                break;
            }
            eRet = MUXER_SetAencFmt(ucMxId, eAudFmt);
        }while(0);
    }

    return eRet;
}
#endif // CC_SUPPORT_MUXER


int mtal_ioctl_mtmx(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;

    switch (cmd)
    {
    default:
        i4Ret = MTR_NOT_OK;
        break;

#ifdef CC_SUPPORT_MUXER
    case MTAL_IO_MX_INIT:
        i4Ret = _MTMX_Init();
        break;
    case MTAL_IO_MX_TERMINT:
        i4Ret = _MTMX_Termint();
        break;
    case MTAL_IO_MX_RESET:
        i4Ret = _MTMX_Reset(arg);
        break;
    case MTAL_IO_MX_SET:
        i4Ret = _MTMX_Set(arg);
        break;
    case MTAL_IO_MX_GET:
        i4Ret = _MTMX_Get(arg);
        break;
    case MTAL_IO_MX_SET_CODEC:
        i4Ret = _MTMX_SetCodec(arg);
        break;
    case MTAL_IO_MX_COPY_DATA:
        i4Ret = _MTMX_CopyData(arg);
        break;
        
#endif // CC_SUPPORT_MUXER
    }

    return i4Ret;
}


