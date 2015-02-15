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
 * $RCSfile: muxer_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file muxer_mw.c
 *  This file defines the interfaces for middleware to access Muxer driver
 */


/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/

#include "u_cli.h"

#include "x_rm_dev_types.h"
#include "x_dbg.h"
#include "x_cli.h"
#include "drv_comp_id.h"
#include "x_mux.h"


#include "muxer_if.h"
#include "muxer_drvif.h"

#ifdef LINUX_TURNKEY_SOLUTION
#include "cb_data_rm.h"
#include "cb_param.h"
#endif

#include "aud_drv.h"

#ifdef DEFINE_IS_LOG
#undef DEFINE_IS_LOG
#endif
#define DEFINE_IS_LOG    MUXER_IsLog
#include "x_debug.h"


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/


#define MX_SET_VALIDATE(TYPE)                                  \
do {                                                           \
    if (NULL == pv_set_info || z_set_info_len != sizeof(TYPE)) \
    {                                                          \
        return RMR_DRV_INV_SET_INFO;                           \
    }                                                          \
} while(0)


typedef struct _MUXER_MW_INT_DATA_T
{
    BOOL fgEnabled;
    UCHAR ucMxId;
    UINT16 u2VStrmId;
    UINT16 u2AStrmId;
    UINT16 u2VbiStrmId;
    UINT32 u4VStrmNo;
    UINT32 u4AStrmNo;
    UINT32 u4VbiStrmNo;
    VOID *pv_mw_mx_nfy_tag;
    x_mux_nfy_fct pf_mw_mx_nfy_fct;
} MUXER_MW_INT_DATA_T;


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

extern VOID MW_VENC_SetMxId(UINT16  u2Id, UCHAR ucMxId);
#ifdef LINUX_TURNKEY_SOLUTION
extern INT32 _CB_PutEvent_Adpt(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);
#endif


/*----------------------------------------------------------------------------
 * Static variables
 *---------------------------------------------------------------------------*/

static MUXER_MW_INT_DATA_T  _arMuxerMwData[MUXER_COMP_NS];


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static MUXER_MW_INT_DATA_T * get_mw_mx_ins(UCHAR ucMxId)
{
    ASSERT(MUXER_COMP_NS > ucMxId);
    return (MUXER_COMP_NS > ucMxId)? &_arMuxerMwData[ucMxId] : &_arMuxerMwData[0];
}


static VOID _muxer_data_nfy(UCHAR ucMxId, MX_CB_DATA_T *prCbData)
{
    MUXER_MW_INT_DATA_T *prMxMwData = get_mw_mx_ins(ucMxId);
#ifdef LINUX_TURNKEY_SOLUTION
    MUXER_NFY_PARAM muxer_param;
    MUX_BUF_DATA_T *prData = &muxer_param.rData;
    prData->pui1_buf_phy = (UINT8 *)PHYSICAL(prCbData->u4Addr);
    prData->t_size = prCbData->u4Len;
    prData->t_pts = (UINT64)prCbData->u4Pts;
    muxer_param.pf_nfy = prMxMwData->pf_mw_mx_nfy_fct;
    muxer_param.pv_nfy_tag = prMxMwData->pv_mw_mx_nfy_tag;
    muxer_param.e_nfy_cond = MUX_COND_DATA;

    _CB_PutEvent_Adpt(CB_DRV_MUXER_NFY, sizeof(MUXER_NFY_PARAM), &muxer_param);
#else
    if (prMxMwData->pf_mw_mx_nfy_fct)
    {
        MUX_BUF_DATA_T rData;
        rData.pui1_buf_phy = (UINT8 *)PHYSICAL(prCbData->u4Addr);
        rData.t_size = prCbData->u4Len;
        rData.t_pts = prCbData->u4Pts;
        prMxMwData->pf_mw_mx_nfy_fct(prMxMwData->pv_mw_mx_nfy_tag, MUX_COND_DATA, (UINT32)&rData);
    }
#endif
}


static INT32 _muxer_connect(
    DRV_COMP_ID_T*   pt_comp_id,
    DRV_CONN_TYPE_T  e_conn_type,
    const VOID*      pv_conn_info,
    SIZE_T           z_conn_info_len,
    VOID*            pv_tag,
    x_rm_nfy_fct     pf_nfy)
{
    DRV_COMP_ID_T*   prCompReg;
    MUXER_MW_INT_DATA_T *prMxMwData;
    UCHAR ucMxId;
    UINT16 u2CompId;
    UINT16 u2SrcCompId;

    LOG(1, "%s: begin\n", __FUNCTION__);

    /* Check component type and id */
    if (NULL == pt_comp_id)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if (DRVT_MUXER != pt_comp_id->e_type)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId > (MUXER_COMP_NS - 1))
    {
        LOG(1, "%s: invalid u2CompId[%d]\n", __FUNCTION__, u2CompId);
        return RMR_DRV_INV_CONN_INFO;
    }

    if (pt_comp_id->b_sel_out_port)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if (pt_comp_id->u.ui1_inp_port > 3)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if (NULL == pv_conn_info)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if (sizeof(DRV_COMP_ID_T) != z_conn_info_len)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
    u2SrcCompId = prCompReg->ui2_id;
    prMxMwData = get_mw_mx_ins(u2CompId);
    ucMxId = prMxMwData->ucMxId;

    if (prMxMwData->fgEnabled)
    {
        // disable
        VERIFY(MUXER_Stop(ucMxId));
        prMxMwData->fgEnabled = FALSE;
    }

    switch (prCompReg->e_type)
    {
    default:
        return RMR_DRV_INV_CONN_INFO;

    case DRVT_VID_ENC:
        if (MUXER_INVALID_STRM_ID != prMxMwData->u4VStrmNo)
        {
            VERIFY(MUXER_DelStrm(ucMxId, prMxMwData->u4VStrmNo));
            prMxMwData->u4VStrmNo = MUXER_INVALID_STRM_ID;
        }
        VERIFY(MUXER_AddStrm(ucMxId, eMUXER_STRMTYPE_VIDEO, &prMxMwData->u4VStrmNo));
        prMxMwData->u2VStrmId = u2SrcCompId;
        LOG(1, "connect video(%d) to muxer(%d) port(%d) successfully!\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port);
        break;

    case DRVT_AUD_ENC:
    case DRVT_AUD_DEC:
        if (MUXER_INVALID_STRM_ID != prMxMwData->u4AStrmNo)
        {
            VERIFY(MUXER_DelStrm(ucMxId, prMxMwData->u4AStrmNo));
            prMxMwData->u4AStrmNo = MUXER_INVALID_STRM_ID;
        }
        VERIFY(MUXER_AddStrm(ucMxId, eMUXER_STRMTYPE_AUDIO, &prMxMwData->u4AStrmNo));
        prMxMwData->u2AStrmId = u2SrcCompId;
        LOG(1, "connect audio(%d) to muxer(%d) port(%d) successfully!\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port);
        break;

    case DRVT_VBI_ENC:
        if (MUXER_INVALID_STRM_ID != prMxMwData->u4VbiStrmNo)
        {
            VERIFY(MUXER_DelStrm(ucMxId, prMxMwData->u4VbiStrmNo));
            prMxMwData->u4VbiStrmNo = MUXER_INVALID_STRM_ID;
        }
        VERIFY(MUXER_AddStrm(ucMxId, eMUXER_STRMTYPE_VBI, &prMxMwData->u4VbiStrmNo));
        prMxMwData->u2VbiStrmId = u2SrcCompId;
        LOG(1, "connect vbi(%d) to muxer(%d) port(%d) successfully!\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port);
        break;
    }
    
    return RMR_OK;
}


static INT32 _muxer_disconn(
    DRV_COMP_ID_T*   pt_comp_id,
    DRV_DISC_TYPE_T  e_disc_type,
    const VOID*      pv_disc_info,
    SIZE_T           z_disc_info_len)
{
    DRV_COMP_ID_T*   prCompReg;
    MUXER_MW_INT_DATA_T *prMxMwData;
    UCHAR ucMxId;
    UINT16 u2CompId;
    UINT16 u2SrcCompId;

    LOG(1, "%s: begin\n", __FUNCTION__);

    /* Check component type and id */
    if (NULL == pt_comp_id)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (DRVT_MUXER != pt_comp_id->e_type)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId > (MUXER_COMP_NS - 1))
    {
        LOG(1, "%s: invalid u2CompId[%d]\n", __FUNCTION__, u2CompId);
        return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->b_sel_out_port)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->u.ui1_inp_port > 3)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (NULL == pv_disc_info)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if (sizeof(DRV_COMP_ID_T) != z_disc_info_len)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    prCompReg = (DRV_COMP_ID_T*)pv_disc_info;
    u2SrcCompId = prCompReg->ui2_id;
    prMxMwData = get_mw_mx_ins(u2CompId);
    ucMxId = prMxMwData->ucMxId;

    if (prMxMwData->fgEnabled)
    {
        // disable
        VERIFY(MUXER_Stop(ucMxId));
        prMxMwData->fgEnabled = FALSE;
    }

    switch (prCompReg->e_type)
    {
    default:
        return RMR_DRV_INV_CONN_INFO;

    case DRVT_VID_ENC:
        ASSERT(MUXER_INVALID_STRM_ID != prMxMwData->u4VStrmNo);
        VERIFY(MUXER_DelStrm(ucMxId, prMxMwData->u4VStrmNo));
        prMxMwData->u4VStrmNo = MUXER_INVALID_STRM_ID;
        ASSERT(u2SrcCompId == prMxMwData->u2VStrmId);
        prMxMwData->u2VStrmId = MAX_COMP_ID;
        LOG(1, "disconnect video(%d) to muxer(%d) port(%d) successfully!\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port);
        break;

    case DRVT_AUD_DEC:
    case DRVT_AUD_ENC:
        ASSERT(MUXER_INVALID_STRM_ID != prMxMwData->u4AStrmNo);
        VERIFY(MUXER_DelStrm(ucMxId, prMxMwData->u4AStrmNo));
        prMxMwData->u4AStrmNo = MUXER_INVALID_STRM_ID;
        ASSERT(u2SrcCompId == prMxMwData->u2AStrmId);
        prMxMwData->u2AStrmId = MAX_COMP_ID;
        LOG(1, "disconnect audio(%d) to muxer(%d) port(%d) successfully!\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port);
        break;

    case DRVT_VBI_ENC:
        ASSERT(MUXER_INVALID_STRM_ID != prMxMwData->u4VbiStrmNo);
        VERIFY(MUXER_DelStrm(ucMxId, prMxMwData->u4VbiStrmNo));
        prMxMwData->u4VbiStrmNo= MUXER_INVALID_STRM_ID;
        ASSERT(u2SrcCompId == prMxMwData->u2VbiStrmId);
        prMxMwData->u2VbiStrmId = MAX_COMP_ID;
        LOG(1, "disconnect vbi(%d) to muxer(%d) port(%d) successfully!\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port);
        break;
    }

    LOG(1, "%s: disconnect successfully!\n", __FUNCTION__);

    return RMR_OK;
}


static INT32 _muxer_get(
    DRV_COMP_ID_T*  pt_comp_id,
    DRV_GET_TYPE_T  e_get_type,
    VOID*           pv_get_info,
    SIZE_T*         pz_get_info_len)
{
    INT32           i4_ret = RMR_OK;
    MUXER_MW_INT_DATA_T *prMxMwData;
    UINT16 u2CompId;

    /* Check component ID */
    if (NULL == pt_comp_id)
    {
        LOG(1, "%s: pt_comp_id is NULL\n");
        return RMR_DRV_INV_GET_INFO;
    }

    if (DRVT_MUXER != pt_comp_id->e_type)
    {
        LOG(1, "%s: error component type or id type [%d]\n",
            __FUNCTION__, pt_comp_id->e_type);
        return RMR_DRV_INV_GET_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)MUXER_COMP_NS)
    {
        LOG(1, "%s: error component id [%d]\n", __FUNCTION__, u2CompId);
        return RMR_DRV_INV_GET_INFO;
    }

    if (NULL == pv_get_info)
    {
        LOG(1, "%s: pv_get_info is NULL\n", __FUNCTION__);
        return RMR_DRV_INV_GET_INFO;
    }

    if (NULL == pz_get_info_len)
    {
        LOG(1, "%s: pz_get_info_len is NULL\n", __FUNCTION__);
        return RMR_DRV_INV_GET_INFO;
    }

    prMxMwData = get_mw_mx_ins(u2CompId);

    /* handle get type */
    switch (e_get_type)
    {
    default:
        i4_ret = RMR_DRV_INV_GET_INFO;
        break;

    case MUX_GET_TYPE_CTRL:
        if (pv_get_info == NULL)
        {
            i4_ret = RMR_DRV_INV_GET_INFO;
        }
        else
        {
            *(BOOL *)pv_get_info = prMxMwData->fgEnabled;
            i4_ret = RMR_OK;
        }
        break;
    }

    return i4_ret;
}


static INT32 _muxer_set(
    DRV_COMP_ID_T*  pt_comp_id,
    DRV_SET_TYPE_T  e_set_type,
    const VOID*     pv_set_info,
    SIZE_T          z_set_info_len)
{
    UCHAR ucMxId;
    UINT16 u2CompId;
    INT32           i4_ret = RMR_OK;
    MUXER_MW_INT_DATA_T *prMxMwData;

    /* Check component ID */
    if (NULL == pt_comp_id)
    {
        LOG(1, "%s: pt_comp_id is NULL\n");
        return RMR_DRV_INV_SET_INFO;
    }

    if (DRVT_MUXER != pt_comp_id->e_type)
    {
        LOG(1, "%s: error component type or id type [%d]\n",
            __FUNCTION__, pt_comp_id->e_type);
        return RMR_DRV_INV_SET_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)MUXER_COMP_NS)
    {
        LOG(1, "%s: error component id [%d]\n", __FUNCTION__, u2CompId);
        return RMR_DRV_INV_SET_INFO;
    }

    LOG(5, "%s(%p, %d, %p, %d): u2CompId=%d\n", __FUNCTION__,
        pt_comp_id, e_set_type, pv_set_info, z_set_info_len, u2CompId);

    prMxMwData = get_mw_mx_ins(u2CompId);
    ucMxId = prMxMwData->ucMxId;

    /* handle set type here */
    switch (e_set_type)
    {
    default:
        LOG(1, "%s: Invalid set_type - %d\n", __FUNCTION__, e_set_type);
        return RMR_DRV_INV_SET_INFO;

    case MUX_SET_TYPE_CTRL:
        if (pv_set_info)
        {
            // enable
            if (MUXER_INVALID_STRM_ID != prMxMwData->u4VStrmNo)
            {
                MW_VENC_SetMxId(prMxMwData->u2VStrmId, ucMxId);
                MUXER_SetVencFmt(ucMxId, ENUM_MUXER_VENC_H264); // TODO: remove this hardcode
            }

            if (MUXER_INVALID_STRM_ID != prMxMwData->u4AStrmNo)
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
                MUXER_SetAencFmt(ucMxId, ENUM_MUXER_AENC_MP3); // TODO: remove this hardcode
            }
            VERIFY(MUXER_Start(ucMxId));
            prMxMwData->fgEnabled = TRUE;
        }
        else
        {
            // disable
            VERIFY(MUXER_Stop(ucMxId));
            prMxMwData->fgEnabled = FALSE;
        }
        i4_ret = RMR_OK;
        break;

    case MUX_SET_TYPE_NFY_FCT:
        MX_SET_VALIDATE(MUX_NFY_INFO_T);
        {
            MUX_NFY_INFO_T *prNfyInfo = (MUX_NFY_INFO_T *)pv_set_info;
            ASSERT(prNfyInfo->pv_tag);
            ASSERT(prNfyInfo->pf_nfy);
            prMxMwData->pv_mw_mx_nfy_tag = prNfyInfo->pv_tag;
            prMxMwData->pf_mw_mx_nfy_fct = prNfyInfo->pf_nfy;
        }
        i4_ret = RMR_OK;
        break;

    case MUX_SET_TYPE_BUFFER_SETTING:
        MX_SET_VALIDATE(MUX_BUF_SETTING_T);
        {
            MUX_BUF_SETTING_T *prBufSetting = (MUX_BUF_SETTING_T *)pv_set_info;

            switch (prBufSetting->t_type)
            {
            default:
                return RMR_DRV_INV_SET_INFO;

            case MUX_DATA_TYPE_MTK:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(prBufSetting->u.t_buf_mtk.pv_buf_phy)), prBufSetting->u.t_buf_mtk.z_buf_size, _muxer_data_nfy, prBufSetting->u.t_buf_mtk.ui4_tick_period));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_CMPB));
                break;

            case MUX_DATA_TYPE_TS:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(prBufSetting->u.t_buf_ts.pv_buf_phy)), prBufSetting->u.t_buf_ts.z_buf_size, _muxer_data_nfy, prBufSetting->u.t_buf_ts.ui4_tick_period));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_MPEG2_TS_192));
                break;

            case MUX_DATA_TYPE_VIDEO_ES:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(prBufSetting->u.t_buf_vid.pv_buf_phy)), prBufSetting->u.t_buf_vid.z_buf_size, _muxer_data_nfy, prBufSetting->u.t_buf_vid.ui4_tick_period));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_RAW));
                break;

            case MUX_DATA_TYPE_AUDIO_ES:
                VERIFY(MUXER_SetBuffer(ucMxId, VIRTUAL((UINT32)(prBufSetting->u.t_buf_aud.pv_buf_phy)), prBufSetting->u.t_buf_aud.z_buf_size, _muxer_data_nfy, prBufSetting->u.t_buf_aud.ui4_tick_period));
                VERIFY(MUXER_SetMuxFmt(ucMxId, MUXER_FMT_RAW));
                break;
            }
        }
        i4_ret = RMR_OK;
        break;

    case MUX_SET_TYPE_UNLOCK_BUFFER:
        MX_SET_VALIDATE(MUX_UNLOCK_BUFFER_INFO_T);
        {
            MUX_UNLOCK_BUFFER_INFO_T *prUnlockBuf = (MUX_UNLOCK_BUFFER_INFO_T *)pv_set_info;
            VERIFY(MUXER_UpdateRp(ucMxId, VIRTUAL((UINT32)(prUnlockBuf->pui1_data_addr) + prUnlockBuf->z_data_len)));
        }
        i4_ret = RMR_OK;
        break;

    case MUX_SET_TYPE_FLUSH_BUFFER:
        if (MUXER_Flush(ucMxId))
        {
            i4_ret = RMR_OK;
        }
        else
        {
            i4_ret = RMR_DRV_ERROR;
        }
        break;

    case MUX_SET_TYPE_ENCRYPT_KEY:
        MX_SET_VALIDATE(MM_CRYPT_INFO_T);
        {
            MM_CRYPT_INFO_T *prCryptInfo = (MM_CRYPT_INFO_T *)pv_set_info;
            VERIFY(MUXER_SetScrambleKey(ucMxId, &prCryptInfo->u.t_pvr_crypt));
        }
        i4_ret = RMR_OK;
        break;

    case MUX_SET_TYPE_ENCRYPT_CTRL:
        VERIFY(MUXER_SetScramble(ucMxId, (pv_set_info? TRUE : FALSE)));
        break;
    }
    return i4_ret;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

INT32 MW_Muxer_Init(void)
{
    INT32 i4_ret;
    UINT32 i;

    /* Component ID */
    DRV_COMP_REG_T t_comp_reg;

    /* Function table */
    DRV_COMP_FCT_TBL_T t_comp_fct;

    /* Setup component ID */
    t_comp_reg.e_type = DRVT_MUXER;
    t_comp_reg.e_id_type = ID_TYPE_RANGE;
    t_comp_reg.u.t_range.ui2_first_id = MUXER_COMP_1;
    t_comp_reg.u.t_range.ui2_delta_id = 1;
    t_comp_reg.u.t_range.ui2_num_of_ids = MUXER_COMP_NS;
    t_comp_reg.u.t_range.pv_first_tag = NULL;
    t_comp_reg.u.t_range.pv_delta_tag = NULL;
    t_comp_reg.u.t_range.ui1_port = 1;

    /* Setup component function table */
    t_comp_fct.pf_rm_connect = _muxer_connect;
    t_comp_fct.pf_rm_disconnect = _muxer_disconn;
    t_comp_fct.pf_rm_get = _muxer_get;
    t_comp_fct.pf_rm_set = _muxer_set;

    for (i = 0; i < MUXER_COMP_NS; i++)
    {
        MUXER_MW_INT_DATA_T *prMxMwData = get_mw_mx_ins(i);
        prMxMwData->fgEnabled = FALSE;
        prMxMwData->ucMxId = i;
        prMxMwData->u2VStrmId = MAX_COMP_ID;
        prMxMwData->u2AStrmId = MAX_COMP_ID;
        prMxMwData->u2VbiStrmId = MAX_COMP_ID;
        prMxMwData->u4VStrmNo = MUXER_INVALID_STRM_ID;
        prMxMwData->u4AStrmNo = MUXER_INVALID_STRM_ID;
        prMxMwData->u4VbiStrmNo = MUXER_INVALID_STRM_ID;
        prMxMwData->pv_mw_mx_nfy_tag = NULL;
        prMxMwData->pf_mw_mx_nfy_fct = NULL;
    }

    /* init muxer driver here */
    MUXER_Init();

    /* Register component to Resource Manager database */
    i4_ret = x_rm_reg_comp(&t_comp_reg, 3, 1, "", 0, &t_comp_fct, NULL, 0);

    return i4_ret;
}


