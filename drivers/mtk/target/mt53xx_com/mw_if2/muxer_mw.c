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

//#include "x_rm_dev_types.h"
#include "drv_comp_id.h"
#include "mtmx.h"
#include "x_mux.h"
#include "x_assert.h"


#ifdef LOG
#undef LOG
#endif

#define MPV_LOG_LVL 3
#define LOG(x, y...) \
    if(x <= MPV_LOG_LVL) \
        printf(y); \



//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct _MUXER_MW_INT_DATA_T
{
    BOOL fgEnabled;
    UCHAR ucMwMxId;
    UINT16 u2VStrmId;
    UINT16 u2AStrmId;
    UINT16 u2VbiStrmId;
    VOID *pv_mw_mx_nfy_tag;
    x_mux_nfy_fct pf_mw_mx_nfy_fct;
} MUXER_MW_INT_DATA_T;


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define MX_SET_VALIDATE(TYPE)                      \
do {                                               \
    if (NULL == pv_set_info || z_set_info_len != sizeof(TYPE)) \
    {                                              \
        return RMR_DRV_INV_SET_INFO;               \
    }                                              \
} while(0)


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static MUXER_MW_INT_DATA_T  _arMuxerMwData[MUXER_COMP_NS];


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static MUXER_MW_INT_DATA_T * get_mw_mx_ins(UCHAR ucMxId)
{
    ASSERT(MUXER_COMP_NS > ucMxId);
    return (MUXER_COMP_NS > ucMxId)? &_arMuxerMwData[ucMxId] : &_arMuxerMwData[0];
}


static BOOL _muxer_nfy_fct( VOID*            pvTag,
                               MTMX_NFY_COND_T  eNfyCond,
                               UINT32           u4Data )
{
    MUXER_MW_INT_DATA_T *prMxMwData;
    MTMX_CB_T * ptMxCbT;
    UINT32 u4Nfy;

    if (!pvTag)
    {
        LOG(1, "%s: pvTag is NULL\n", __FUNCTION__);
        return FALSE;
    }

    if (!u4Data)
    {
        LOG(1, "%s: no cb data\n", __FUNCTION__);
        return FALSE;
    }

    prMxMwData = (MUXER_MW_INT_DATA_T *)pvTag;
    ptMxCbT = (MTMX_CB_T *)u4Data;

    if (!prMxMwData->pf_mw_mx_nfy_fct)
    {
        LOG(1, "%s: nfy is not registered\n", __FUNCTION__);
        return FALSE;
    }

    switch (eNfyCond)
    {
    default:
        LOG(1, "%s: unknown eNfyCond(%d)\n", __FUNCTION__, eNfyCond);
        return FALSE;

    case MTMX_COND_DATA:
        {
            MTMX_BUF_DATA_T *ptBufData = &ptMxCbT->u.tBufData;
            MUX_BUF_DATA_T rData;
            rData.pui1_buf_phy = ptBufData->pui1BufPhy;
            rData.t_size = ptBufData->u4Size;
            rData.t_pts = ptBufData->u8Pts;
            rData.ui4_flag = ptBufData->u4Flag;
            prMxMwData->pf_mw_mx_nfy_fct(prMxMwData->pv_mw_mx_nfy_tag, MUX_COND_DATA, (UINT32)&rData);
        }
        break;

    case MTMX_COND_OVERFLOW:
        switch (ptMxCbT->u.eOverflowReason)
        {
        default:
            LOG(1, "%s: unknown eOverflowReason(%d)\n", __FUNCTION__, ptMxCbT->u.eOverflowReason);
            return FALSE;

        case MTMX_OVERFLOW_REASON_RING_BUF_FULL:
            u4Nfy = MUX_OVERFLOW_REASON_RING_BUF_FULL;
            break;

        case MTMX_OVERFLOW_REASON_METADATA_BUF_FULL:
            u4Nfy = MUX_OVERFLOW_REASON_METADATA_BUF_FULL;
            break;
        }
        prMxMwData->pf_mw_mx_nfy_fct(prMxMwData->pv_mw_mx_nfy_tag, MUX_COND_OVERFLOW, u4Nfy);
        break;

    case MTMX_COND_ERROR:
        switch (ptMxCbT->u.eErrorReason)
        {
        default:
            LOG(1, "%s: unknown eErrorReason(%d)\n", __FUNCTION__, ptMxCbT->u.eErrorReason);
            return FALSE;

        case MTMX_ERROR_REASON_INTERNAL:
            u4Nfy = MUX_ERROR_REASON_INTERNAL;
            break;
        }
        prMxMwData->pf_mw_mx_nfy_fct(prMxMwData->pv_mw_mx_nfy_tag, MUX_COND_ERROR, u4Nfy);
        break;
    }

    return TRUE;
}


static INT32 _muxer_connect(
    DRV_COMP_ID_T*   pt_comp_id,
    DRV_CONN_TYPE_T  e_conn_type,
    const VOID*      pv_conn_info,
    SIZE_T           z_conn_info_len,
    VOID*            pv_tag,
    x_rm_nfy_fct     pf_nfy)
{
    MTMX_SRC_INFO_T rSrcInfo;
    DRV_COMP_ID_T*   prCompReg;
    MUXER_MW_INT_DATA_T *prMxMwData;
    MT_RESULT_T eRet = MTR_OK;
    UINT16 u2CompId;
    UINT16 u2SrcCompId;

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
    if (u2CompId >= (UINT16)MUXER_COMP_NS)
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

    if (RM_CONN_TYPE_COMP_ID != e_conn_type)
    {
        LOG(1, "%s: unexpected conn type [%d]\n", __FUNCTION__, e_conn_type);
        return RMR_DRV_INV_CONN_INFO;
    }

    prCompReg = (DRV_COMP_ID_T*)pv_conn_info;
    u2SrcCompId = prCompReg->ui2_id;
    prMxMwData = get_mw_mx_ins(u2CompId);

    switch (prCompReg->e_type)
    {
    default:
        return RMR_DRV_INV_CONN_INFO;

    case DRVT_VID_ENC:
        rSrcInfo.eSrcType = MTMX_SRC_TYPE_VENC;
        rSrcInfo.u2CompId = u2SrcCompId;
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_CONNECT_SRC, &rSrcInfo, sizeof(rSrcInfo));
        prMxMwData->u2VStrmId = u2SrcCompId;
        LOG(1, "connect video(%d) to muxer(%d) port(%d), %d\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port, eRet);
        break;

    case DRVT_AUD_ENC:
    case DRVT_AUD_DEC:
        rSrcInfo.eSrcType = MTMX_SRC_TYPE_AENC;
        rSrcInfo.u2CompId = u2SrcCompId;
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_CONNECT_SRC, &rSrcInfo, sizeof(rSrcInfo));
        prMxMwData->u2AStrmId = u2SrcCompId;
        LOG(1, "connect audio(%d) to muxer(%d) port(%d), %d\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port, eRet);
        break;

    case DRVT_VBI_ENC:
        rSrcInfo.eSrcType = MTMX_SRC_TYPE_VBI;
        rSrcInfo.u2CompId = u2SrcCompId;
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_CONNECT_SRC, &rSrcInfo, sizeof(rSrcInfo));
        prMxMwData->u2VbiStrmId = u2SrcCompId;
        LOG(1, "connect vbi(%d) to muxer(%d) port(%d), %d\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port, eRet);
        break;
    }

    return (MTR_OK != eRet)? RMR_DRV_INV_CONN_INFO : RMR_OK;
}


static INT32 _muxer_disconn(
    DRV_COMP_ID_T*   pt_comp_id,
    DRV_DISC_TYPE_T  e_disc_type,
    const VOID*      pv_disc_info,
    SIZE_T           z_disc_info_len)
{
    MTMX_SRC_INFO_T rSrcInfo;
    DRV_COMP_ID_T*   prCompReg;
    MUXER_MW_INT_DATA_T *prMxMwData;
    MT_RESULT_T eRet = MTR_OK;
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
    if (u2CompId >= (UINT16)MUXER_COMP_NS)
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
        return RMR_DRV_INV_DISC_INFO;
    }

    if (sizeof(DRV_COMP_ID_T) != z_disc_info_len)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    prCompReg = (DRV_COMP_ID_T*)pv_disc_info;
    u2SrcCompId = prCompReg->ui2_id;
    prMxMwData = get_mw_mx_ins(u2CompId);

    switch (prCompReg->e_type)
    {
    default:
        return RMR_DRV_INV_DISC_INFO;

    case DRVT_VID_ENC:
        rSrcInfo.eSrcType = MTMX_SRC_TYPE_VENC;
        rSrcInfo.u2CompId = u2SrcCompId;
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_DISCONNECT_SRC, &rSrcInfo, sizeof(rSrcInfo));
        prMxMwData->u2VStrmId = MAX_COMP_ID;
        LOG(1, "disconnect video(%d) to muxer(%d) port(%d), %d\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port, eRet);
        break;

    case DRVT_AUD_DEC:
    case DRVT_AUD_ENC:
        rSrcInfo.eSrcType = MTMX_SRC_TYPE_AENC;
        rSrcInfo.u2CompId = u2SrcCompId;
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_DISCONNECT_SRC, &rSrcInfo, sizeof(rSrcInfo));
        prMxMwData->u2AStrmId = MAX_COMP_ID;
        LOG(1, "disconnect audio(%d) to muxer(%d) port(%d), %d\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port, eRet);
        break;

    case DRVT_VBI_ENC:
        rSrcInfo.eSrcType = MTMX_SRC_TYPE_VBI;
        rSrcInfo.u2CompId = u2SrcCompId;
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_DISCONNECT_SRC, &rSrcInfo, sizeof(rSrcInfo));
        prMxMwData->u2VbiStrmId = MAX_COMP_ID;
        LOG(1, "disconnect vbi(%d) to muxer(%d) port(%d), %d\n", u2SrcCompId, u2CompId, pt_comp_id->u.ui1_inp_port, eRet);
        break;
    }

    return (MTR_OK != eRet)? RMR_DRV_INV_DISC_INFO : RMR_OK;
}


static INT32 _muxer_get(
    DRV_COMP_ID_T*  pt_comp_id,
    DRV_GET_TYPE_T  e_get_type,
    VOID*           pv_get_info,
    SIZE_T*         pz_get_info_len)
{
    MT_RESULT_T eRet = MTR_OK;
    UINT16 u2CompId;

    /* Check component ID */
    if (NULL == pt_comp_id)
    {
        LOG(1, "%s: pt_comp_id is NULL\n", __FUNCTION__);
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

    /* handle get type */
    switch (e_get_type)
    {
    default:
        LOG(1, "%s: Invalid get_type - %d\n", __FUNCTION__, e_get_type);
        return RMR_DRV_INV_GET_INFO;

    case MUX_GET_TYPE_CTRL:
        eRet = MTMX_Get(u2CompId, MTMX_GET_TYPE_CTRL, pv_get_info, (UINT32 *)pz_get_info_len);
        break;
    }

    return (MTR_OK != eRet)? RMR_DRV_INV_GET_INFO : RMR_OK;
}


static INT32 _muxer_set(
    DRV_COMP_ID_T*  pt_comp_id,
    DRV_SET_TYPE_T  e_set_type,
    const VOID*     pv_set_info,
    SIZE_T          z_set_info_len)
{
    UINT16 u2CompId;
    MT_RESULT_T eRet = MTR_OK;
    MUXER_MW_INT_DATA_T *prMxMwData;

    /* Check component ID */
    if (NULL == pt_comp_id)
    {
        LOG(1, "%s: pt_comp_id is NULL\n", __FUNCTION__);
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

    prMxMwData = get_mw_mx_ins(u2CompId);

    /* handle set type here */
    switch (e_set_type)
    {
    default:
        LOG(1, "%s: Invalid set_type - %d\n", __FUNCTION__, e_set_type);
        return RMR_DRV_INV_SET_INFO;

    case MUX_SET_TYPE_CODEC_INFO:
    {
        MUX_SET_CODEC_INFO_T *ptSetCodec;
        MTMX_VENC_T     eVenc = MTMX_VENC_UNKNOWN;
        MTMX_AENC_T     eAenc = MTMX_AENC_UNKNOWN;
        MTMX_SRC_TYPE_T eSrcType = MTMX_SRC_TYPE_AENC;

        if(pv_set_info == NULL)
        {
            LOG(0, "pv_set_info is NULL\n");   
            return RMR_DRV_INV_SET_INFO;    
        }

        ptSetCodec = (MUX_SET_CODEC_INFO_T *)pv_set_info;
        
        LOG(0, "MTMX_SetCodec(): eCodecType(%d),eVenc(%d), eAenc(%d)\n", ptSetCodec->eCodecType, ptSetCodec->eVenc, ptSetCodec->eAenc);

        switch(ptSetCodec->eCodecType)
        {
            case MUX_SET_CODEC_TYPE_VENC:
                eSrcType = MTMX_SRC_TYPE_VENC;
                break;
            case MUX_SET_CODEC_TYPE_AENC:
                eSrcType = MTMX_SRC_TYPE_AENC;
                break;
            case MUX_SET_CODEC_TYPE_AV:
                eSrcType = MTMX_SRC_TYPE_AV;
                break;
            case MUX_SET_CODEC_TYPE_VBI:
                eSrcType = MTMX_SRC_TYPE_VBI;
                break;
            default:
                LOG(0, "mw set video codec not support\n");    
            break; 
        }
        
        switch(ptSetCodec->eVenc)
        {
            case VID_ENC_FMT_H264:
            {
                eVenc = MTMX_VENC_H264;
            }
            break;

            default:
                LOG(0, "mw set video codec not support\n");    
            break; 
        }
        
        switch(ptSetCodec->eAenc)
        {
            case AUD_ENC_FMT_TYPE_MP3:
            eAenc = MTMX_AENC_MP3;
            break;
            case AUD_ENC_FMT_TYPE_AAC:
            eAenc = MTMX_AENC_AAC;
            break;
            case AUD_ENC_FMT_TYPE_PCM:
            eAenc = MTMX_AENC_PCM;
            break;
            default:
               LOG(0, "mw set audio codec not support\n");    
            break; 
        }
        MTMX_SetCodec(u2CompId, eSrcType, eVenc, eAenc);
    }
        break;
    
    case MUX_SET_TYPE_CTRL:
    {
        LOG(0, "MUX_SET_TYPE_CTRL\n");  
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_CTRL, pv_set_info, 0);
    }
        break;
    
    case MUX_SET_TYPE_NFY_FCT:
        MX_SET_VALIDATE(MUX_NFY_INFO_T);
        {
            MUX_NFY_INFO_T *prNfyInfo = (MUX_NFY_INFO_T *)pv_set_info;
            MTMX_NFY_INFO_T rNfyInfo;
            ASSERT(prNfyInfo->pv_tag);
            ASSERT(prNfyInfo->pf_nfy);
            prMxMwData->pv_mw_mx_nfy_tag = prNfyInfo->pv_tag;
            prMxMwData->pf_mw_mx_nfy_fct = prNfyInfo->pf_nfy;

            rNfyInfo.pfNfy = _muxer_nfy_fct;
            rNfyInfo.pvTag = prMxMwData;
            MTMX_Set(u2CompId, MTMX_SET_TYPE_NFY_FCT, &rNfyInfo, sizeof(rNfyInfo));
        }
        break;

    case MUX_SET_TYPE_BUFFER_SETTING:
        MX_SET_VALIDATE(MUX_BUF_SETTING_T);
        {
            MUX_BUF_SETTING_T *prBufSetting = (MUX_BUF_SETTING_T *)pv_set_info;
            MTMX_BUF_SETTING_T rBufSetting;

            switch (prBufSetting->t_type)
            {
            default:
                return RMR_DRV_INV_SET_INFO;

            case MUX_DATA_TYPE_MTK:
                rBufSetting.eType = MTMX_DATA_TYPE_MTK;
                rBufSetting.u.tBufMtk.pvBufPhy     = prBufSetting->u.t_buf_mtk.pv_buf_phy;
                rBufSetting.u.tBufMtk.u4BufSize    = prBufSetting->u.t_buf_mtk.z_buf_size;
                rBufSetting.u.tBufMtk.u4TickPeriod = prBufSetting->u.t_buf_mtk.ui4_tick_period;
                break;

            case MUX_DATA_TYPE_TS:
                rBufSetting.eType = MTMX_DATA_TYPE_TS;
                rBufSetting.u.tBufTs.pvBufPhy     = prBufSetting->u.t_buf_ts.pv_buf_phy;
                rBufSetting.u.tBufTs.u4BufSize    = prBufSetting->u.t_buf_ts.z_buf_size;
                rBufSetting.u.tBufTs.u4TickPeriod = prBufSetting->u.t_buf_ts.ui4_tick_period;
                break;

            case MUX_DATA_TYPE_TS_192:
                rBufSetting.eType = MTMX_DATA_TYPE_TS_192;
                rBufSetting.u.tBufTs.pvBufPhy     = prBufSetting->u.t_buf_ts.pv_buf_phy;
                rBufSetting.u.tBufTs.u4BufSize    = prBufSetting->u.t_buf_ts.z_buf_size;
                rBufSetting.u.tBufTs.u4TickPeriod = prBufSetting->u.t_buf_ts.ui4_tick_period;
                break;

            case MUX_DATA_TYPE_VIDEO_ES:
                rBufSetting.eType = MTMX_DATA_TYPE_VIDEO_ES;
                rBufSetting.u.tBufVid.pvBufPhy     = prBufSetting->u.t_buf_vid.pv_buf_phy;
                rBufSetting.u.tBufVid.u4BufSize    = prBufSetting->u.t_buf_vid.z_buf_size;
                rBufSetting.u.tBufVid.u4TickPeriod = prBufSetting->u.t_buf_vid.ui4_tick_period;
                break;

            case MUX_DATA_TYPE_AUDIO_ES:
                rBufSetting.eType = MTMX_DATA_TYPE_AUDIO_ES;
                rBufSetting.u.tBufAud.pvBufPhy     = prBufSetting->u.t_buf_aud.pv_buf_phy;
                rBufSetting.u.tBufAud.u4BufSize    = prBufSetting->u.t_buf_aud.z_buf_size;
                rBufSetting.u.tBufAud.u4TickPeriod = prBufSetting->u.t_buf_aud.ui4_tick_period;
                break;
            }

            eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_BUFFER_SETTING, &rBufSetting, sizeof(rBufSetting));
        }
        break;

    case MUX_SET_TYPE_UNLOCK_BUFFER:
        MX_SET_VALIDATE(MUX_UNLOCK_BUFFER_INFO_T);
        {
            MUX_UNLOCK_BUFFER_INFO_T *prUnlockBuf = (MUX_UNLOCK_BUFFER_INFO_T *)pv_set_info;
            MTMX_UNLOCK_BUFFER_INFO_T rUnlockBuf;
            rUnlockBuf.pui1DataAddr = prUnlockBuf->pui1_data_addr;
            rUnlockBuf.u4DataLen = prUnlockBuf->z_data_len;

            eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_UNLOCK_BUFFER, &rUnlockBuf, sizeof(rUnlockBuf));
        }
        break;

    case MUX_SET_TYPE_FLUSH_BUFFER:
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_FLUSH_BUFFER, 0, 0);
        break;

    case MUX_SET_TYPE_ENCRYPT_KEY:
        MX_SET_VALIDATE(MM_CRYPT_INFO_T);
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_ENCRYPT_KEY, pv_set_info, z_set_info_len);
        break;

    case MUX_SET_TYPE_ENCRYPT_CTRL:
        eRet = MTMX_Set(u2CompId, MTMX_SET_TYPE_ENCRYPT_CTRL, pv_set_info, 0);
        break;
    }

    return (MTR_OK != eRet)? RMR_DRV_INV_SET_INFO : RMR_OK;
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
        x_memset(prMxMwData, 0, sizeof(MUXER_MW_INT_DATA_T));
        prMxMwData->ucMwMxId = i;
        prMxMwData->u2VStrmId = MAX_COMP_ID;
        prMxMwData->u2AStrmId = MAX_COMP_ID;
        prMxMwData->u2VbiStrmId = MAX_COMP_ID;
    }

    /* init MTMX here */
    MTMX_Init();

    /* Register component to Resource Manager database */
    i4_ret = x_rm_reg_comp(&t_comp_reg, 3, 1, "", 0, &t_comp_fct, NULL, 0);

    return i4_ret;
}


