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
 * $RCSfile: venc_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_mw.c
 *  This file defines the interfaces for middleware to access VENC driver
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "drv_comp_id.h"
#include "mtvenc.h"
#include "tvsrc_mw.h"
#include "mtvdecex_mwif.h"

#define DEFINE_IS_LOG    VENC_IsLog
//#include "x_debug.h"
#include "mwif2_debug.h"
#include "x_assert.h"
#include "x_vid_enc.h"


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct _VENC_MW_INFO_T
{
    BOOL           fgIsRunning;
} VENC_MW_INFO_T;



//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define VENC_SET_VALIDATE(TYPE)         \
do                                      \
{                                       \
    if (NULL == pv_set_info)            \
    {                                   \
        return RMR_DRV_INV_SET_INFO;    \
    }                                   \
                                        \
    if (sizeof(TYPE) > z_set_info_len)  \
    {                                   \
        return RMR_DRV_INV_SET_INFO;    \
    }                                   \
} while (0)


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------
EXTERN MTVDECEX_SRC_T VdoSrcMap2Mtal(UINT8 u1Src);

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static VENC_MW_INFO_T _arVEncMwInfo[VENC_COMP_NS];



//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static VENC_MW_INFO_T *_VEncMwGetInfoInst(UINT16 u2CompId)
{
    ASSERT((UINT16)VENC_COMP_NS > u2CompId);
    return ((UINT16)VENC_COMP_NS > u2CompId)? &_arVEncMwInfo[u2CompId] : &_arVEncMwInfo[0];
}


/* component functions */
static INT32 _VEncMwConnect(
                            DRV_COMP_ID_T*   pt_comp_id,
                            DRV_CONN_TYPE_T  e_conn_type,
                            const VOID*      pv_conn_info,
                            SIZE_T           z_conn_info_len,
                            VOID*            pv_tag,
                            x_rm_nfy_fct     pf_nfy)
{
    DRV_COMP_ID_T* prCompReg;
    MT_RESULT_T eRet;
    UINT16 u2CompId;
    UINT16 u2SrcCompId;

    LOG(1, "%s: begin\n", __FUNCTION__);

    // Check component type and id
    if (NULL == pt_comp_id)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    if (DRVT_VID_ENC != pt_comp_id->e_type)
    {
       return RMR_DRV_INV_CONN_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)VENC_COMP_NS)
    {
        LOG(1, "%s: invalid u2CompId[%d]\n", __FUNCTION__, u2CompId);
        return RMR_DRV_INV_CONN_INFO;
    }

    if (pt_comp_id->b_sel_out_port)
    {
       return RMR_DRV_INV_CONN_INFO;
    }

    if (pt_comp_id->u.ui1_inp_port > 1)
    {
       return RMR_DRV_INV_CONN_INFO;
    }

    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
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
    if ((DRVT_VID_PLANE != prCompReg->e_type)
    && (DRVT_VID_DEC != prCompReg->e_type))
    {
        LOG(1, "%s: unexpected comp type [%d]\n", __FUNCTION__, prCompReg->e_type);
        return RMR_DRV_INV_CONN_INFO;
    }

    u2SrcCompId = prCompReg->ui2_id;

    eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_CONNECT_SRC, (VOID *)(UINT32)u2SrcCompId, 0);

    LOG(1, "video encoder(%d) connect to %d, %d\n", u2CompId, u2SrcCompId, eRet);

    UNUSED(pv_tag);
    UNUSED(pf_nfy);

    return (MTR_OK != eRet)? RMR_DRV_INV_CONN_INFO : RMR_OK;
}


static INT32 _VEncMwDisconnect(
                            DRV_COMP_ID_T*   pt_comp_id,
                            DRV_DISC_TYPE_T  e_disc_type,
                            const VOID*      pv_disc_info,
                            SIZE_T           z_disc_info_len)
{
    DRV_COMP_ID_T* prCompReg;
    MT_RESULT_T eRet;
    UINT16 u2CompId;
    UINT16 u2SrcCompId;

    LOG(1, "%s: begin\n", __FUNCTION__);

    // Check component type and id
    if (pt_comp_id == NULL)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->e_type != DRVT_VID_ENC)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->ui2_id >= (UINT16)VENC_COMP_NS)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->b_sel_out_port)
    {
       return RMR_DRV_INV_DISC_INFO;
    }

    if (pt_comp_id->u.ui1_inp_port > 1)
    {
       return RMR_DRV_INV_DISC_INFO;
    }

    if (e_disc_type != RM_CONN_TYPE_COMP_ID)
    {
       return RMR_DRV_INV_DISC_INFO;
    }

    if (pv_disc_info == NULL)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    if (sizeof(DRV_COMP_ID_T) != z_disc_info_len)
    {
        return RMR_DRV_INV_DISC_INFO;
    }

    prCompReg = (DRV_COMP_ID_T *)pv_disc_info;
    if ((prCompReg->e_type != DRVT_VID_PLANE)
    && (prCompReg->e_type != DRVT_VID_DEC))
    {
        LOG(1, "%s: unexpected comp type [%d]\n", __FUNCTION__, prCompReg->e_type);
        return RMR_DRV_INV_DISC_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    u2SrcCompId = prCompReg->ui2_id;

    eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_DISCONNECT_SRC, (VOID *)(UINT32)u2SrcCompId, 0);

    LOG(1, "video encoder(%d) disconnect to %d, %d\n", u2CompId, u2SrcCompId, eRet);

    return (MTR_OK != eRet)? RMR_DRV_INV_DISC_INFO : RMR_OK;
}


static INT32 _VEncMwGet(
    DRV_COMP_ID_T*        pt_comp_id,
    DRV_GET_TYPE_T        e_get_type,
    VOID*                pv_get_info,
    SIZE_T*                pz_get_info_len)
{
    UINT16 u2CompId;
    MT_RESULT_T eRet;

    // Check component type and id
    if (NULL == pt_comp_id)
    {
        LOG(1, "%s: pt_comp_id is NULL\n");
        return RMR_DRV_INV_GET_INFO;
    }

    if (DRVT_VID_ENC != pt_comp_id->e_type)
    {
        LOG(1, "%s: error component type or id type [%d]\n",
            __FUNCTION__, pt_comp_id->e_type);
        return RMR_DRV_INV_GET_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)VENC_COMP_NS)
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

    switch (e_get_type)
    {
    default:
        LOG(1, "%s: Invalid get_type - %d\n", __FUNCTION__, e_get_type);
        return RMR_DRV_INV_GET_INFO;

    case VID_ENC_GET_TYPE_CTRL:
        eRet = MTVENC_Get(u2CompId, MTVENC_GET_TYPE_CTRL, pv_get_info, (UINT32 *)pz_get_info_len);
        break;
    }

    return (eRet != MTR_OK)? RMR_DRV_INV_GET_INFO : RMR_OK;
}


static MTVENC_AP_TYPE_T _VEncMw2MtalApTypeMap(VID_AP_TYPE_T eMwType)
{
    MTVENC_AP_TYPE_T eMtalType = MTVENC_AP_TYPE_UNKNOWN;

    switch (eMwType)
    {
    default:
        LOG(1, "%s: Invalid eMwType - %d\n", __FUNCTION__, eMwType);
        break;
    case VID_ENC_AP_TYPE_SKYPE:
        eMtalType = MTVENC_AP_TYPE_SKYPE;
        break;
    case VID_ENC_AP_TYPE_ATVPVR:
        eMtalType = MTVENC_AP_TYPE_ATVPVR;
        break;
    case VID_ENC_AP_TYPE_MSI:
        eMtalType = MTVENC_AP_TYPE_MSI;
        break;
    case VID_ENC_AP_TYPE_BGPVR:
        eMtalType = MTVENC_AP_TYPE_BGPVR;
        break;
    }

    return eMtalType;
}


static MTVENC_FMT_TYPE_T _VEncMw2MtalFmtTypeMap(VID_ENC_FMT_TYPE_T eMwType)
{
    MTVENC_FMT_TYPE_T eMtalType = MTVENC_FMT_UNKNOWN;

    switch (eMwType)
    {
    default:
        LOG(1, "%s: Invalid eMwType - %d\n", __FUNCTION__, eMwType);
        break;
    case VID_ENC_FMT_H264:
        eMtalType = MTVENC_FMT_H264;
        break;
    }

    return eMtalType;
}


static MTVENC_SRC_TYPE_T _VEncMw2MtalSrcTypeMap(VID_SRC_TYPE_T eMwType)
{
    MTVENC_SRC_TYPE_T eMtalType = MTVENC_SRC_TYPE_UNKNOWN;

    switch (eMwType)
    {
    default:
        LOG(1, "%s: Invalid eMwType - %d\n", __FUNCTION__, eMwType);
        break;
    case VID_ENC_SRC_TYPE_VDEC:
        eMtalType = MTVENC_SRC_TYPE_VDEC;
        break;
    case VID_ENC_SRC_TYPE_VDP:
        eMtalType = MTVENC_SRC_TYPE_VDP;
        break;
    }

    return eMtalType;
}




static INT32 _VEncMwSet(
                    DRV_COMP_ID_T*        pt_comp_id,
                    DRV_SET_TYPE_T         e_set_type,
                    const VOID*            pv_set_info,
                    SIZE_T                z_set_info_len)
{
    UINT16 u2CompId;
    MT_RESULT_T eRet;
    VENC_MW_INFO_T *prVEncMwInfo;

    // Check component type and id
    if (NULL == pt_comp_id)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    if (DRVT_VID_ENC != pt_comp_id->e_type)
    {
        LOG(1, "%s: error component type or id type [%d]\n",
            __FUNCTION__, pt_comp_id->e_type);
        return RMR_DRV_INV_SET_INFO;
    }

    u2CompId = pt_comp_id->ui2_id;
    if (u2CompId >= (UINT16)VENC_COMP_NS)
    {
        LOG(1, "%s: error component id [%d]\n", __FUNCTION__, u2CompId);
        return RMR_DRV_INV_SET_INFO;
    }

    prVEncMwInfo = _VEncMwGetInfoInst(u2CompId);
    switch (e_set_type)
    {
    default:
        LOG(1, "%s: Invalid set_type - %d\n", __FUNCTION__, e_set_type);
        return RMR_DRV_INV_SET_INFO;

    case VID_ENC_SET_TYPE_CTRL:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_CTRL, pv_set_info, 0);
        if (MTR_OK == eRet)
        {
            prVEncMwInfo->fgIsRunning = pv_set_info? TRUE : FALSE;
        }
        break;

    case VID_ENC_SET_TYPE_RESTART:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_RESTART, 0, 0);
        break;

    case VID_ENC_SET_TYPE_APP_TYPE:
        eRet = MTVENC_Set(
            u2CompId,
            MTVENC_SET_TYPE_APP_TYPE,
            (VOID *)(UINT32)_VEncMw2MtalApTypeMap((VID_AP_TYPE_T)pv_set_info),
            0);
        break;

    case VID_ENC_SET_TYPE_SETTINGS:
        if (prVEncMwInfo->fgIsRunning)
        {
            LOG(1, "%s(%d): venc is running\n", __FUNCTION__, __LINE__);
            return RMR_DRV_SET_FAILED;
        }
        VENC_SET_VALIDATE(VID_ENC_SETTINGS_T);
        {
            VID_ENC_SETTINGS_T *prSettings = (VID_ENC_SETTINGS_T *)pv_set_info;
            VID_ENC_COMMON_INFO_T  *pt_vid_enc_info = &prSettings->u_fmt.t_fmt_h264.t_vid_enc_info;
            MTVENC_SETTINGS_T rSettings;
            rSettings.eEncFmt = _VEncMw2MtalFmtTypeMap(prSettings->e_enc_fmt);
            rSettings.eSrcType = _VEncMw2MtalSrcTypeMap(prSettings->e_src_type);
            rSettings.uFmt.tFmtH264.tCmmInfo.tResolution.u4Height = pt_vid_enc_info->t_resolution.ui4_height;
            rSettings.uFmt.tFmtH264.tCmmInfo.tResolution.u4Width = pt_vid_enc_info->t_resolution.ui4_width;
            rSettings.uFmt.tFmtH264.tCmmInfo.u4Bps = pt_vid_enc_info->ui4_bitrate;
            rSettings.uFmt.tFmtH264.tCmmInfo.u4Fps = pt_vid_enc_info->ui4_fps;
            eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_SETTINGS, &rSettings, sizeof(rSettings));
        }
        break;

    //maybe used in runtime
    case VID_ENC_SET_TYPE_FRAME_RESOLUTION:
        VENC_SET_VALIDATE(VID_FRAME_RESOLUTION_T);
        {
            VID_FRAME_RESOLUTION_T *pt_resolution = (VID_FRAME_RESOLUTION_T *)pv_set_info;
            MTVENC_FRAME_RESOLUTION_T tResolution;
            tResolution.u4Height = pt_resolution->ui4_height;
            tResolution.u4Width = pt_resolution->ui4_width;
            eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_FRAME_RESOLUTION, &tResolution, sizeof(tResolution));
        }
        break;

    case VID_ENC_SET_TYPE_FRAME_RATE:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_FRAME_RATE, pv_set_info, 0);
        break;

    case VID_ENC_SET_TYPE_BITRATE:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_BITRATE, pv_set_info, 0);
        break;

    case VID_ENC_SET_TYPE_INSERT_KEYFRAME:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_INSERT_KEYFRAME, 0, 0);
        break;

    #if 0   //wait mw to add VID_ENC_SET_TYPE_ENCODE_INPUT 
    case VID_ENC_SET_TYPE_ENCODE_INPUT:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_ENC_INPUT, pv_set_info, 0);
        break;

    //wait mw to add VID_ENC_SET_TYPE_ENCODE_MODULE    
    case VID_ENC_SET_TYPE_ENCODE_MODULE:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_ENC_MODULE, pv_set_info, 0);
        break;
    #endif

    case VID_ENC_SET_TYPE_ENCODE_OSD:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_ENCODE_OSD, pv_set_info, 0);
        break;

    case VID_ENC_SET_TYPE_ENABLE_OSTG:
        eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_MSI_OSTG, pv_set_info, 0);
        break;

    case VID_ENC_SET_TYPE_ENCODE_FIELD:
        {
            VID_ENC_FLD_TYPE_T      eEncFldType = (VID_ENC_FLD_TYPE_T)pv_set_info;  
            MTVENC_ENC_FLD_TYPE_T   eEncMode;
            switch (eEncFldType)
            {
                case VID_ENC_TOP_FLD:
                    eEncMode = MTVENC_ENC_TOP_FLD;
                    break;
                case VID_ENC_BOTTOM_FLD:
                    eEncMode = MTVENC_ENC_BOTTOM_FLD;
                    break;
                default:
                    eEncMode = MTVENC_ENC_BOTH_FLD;
                    break;
            }
            eRet = MTVENC_Set(u2CompId, MTVENC_SET_TYPE_ENC_FLD, (const VOID*)eEncMode, 0);
        }    
        break;
        
    case VID_ENC_SET_TYPE_INPUT_SOURCE:
        {
            UINT8  u1MonSrc= SV_VS_MAX;
            MTVDECEX_SRC_T eSrc = MTVDECEX_SRC_MAX;
            VID_ENC_INPUT_SOURCE_T *prInpSrc = (VID_ENC_INPUT_SOURCE_T *)pv_set_info;

            u1MonSrc = rGetVSrcMap(prInpSrc->e_type, prInpSrc->ui2_comp_id);
            eSrc     = VdoSrcMap2Mtal(u1MonSrc);
            eRet     = MTVDECEX_SetVideoSrc(MTVDO_SUB, eSrc);
        }
        break;
    }

    return (eRet != MTR_OK)? RMR_DRV_INV_SET_INFO : RMR_OK;
}


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

INT32 MW_VENC_Init(void)
{
    INT32 i4Return;
    DRV_COMP_REG_T tVEncCompReg;
    DRV_COMP_FCT_TBL_T tVEncFctTbl;
    UINT32 i;

    /* init MTVENC here */
    MTVENC_Init();

    /* Init VENC MW Info */
    for (i = 0; i < VENC_COMP_NS; i++)
    {
        VENC_MW_INFO_T *prVEncMwInfo = _VEncMwGetInfoInst(i);
        x_memset((VOID *)prVEncMwInfo, 0, sizeof(VENC_MW_INFO_T));
    }

    /* register venc component */
    tVEncCompReg.e_type           = DRVT_VID_ENC;
    tVEncCompReg.e_id_type        = ID_TYPE_IND;
    tVEncCompReg.u.t_ind.ui2_id   = (UINT16)VENC_COMP_1;
    tVEncCompReg.u.t_ind.ui1_port = ALL_PORTS;
    tVEncCompReg.u.t_ind.pv_tag   = NULL;

    /* Setup component function table */
    tVEncFctTbl.pf_rm_connect     = _VEncMwConnect;
    tVEncFctTbl.pf_rm_disconnect  = _VEncMwDisconnect;
    tVEncFctTbl.pf_rm_get         = _VEncMwGet;
    tVEncFctTbl.pf_rm_set         = _VEncMwSet;

    /* Register component with the Resource Manager database */
    i4Return = x_rm_reg_comp(&tVEncCompReg, 1, 1, NULL, 0, &tVEncFctTbl, NULL, 0);

    if (i4Return != RMR_OK)
    {
        LOG(1, "%s call x_rm_reg_comp NG [%d]\n", __FUNCTION__, i4Return);
        return i4Return;
    }

    return RMR_OK;
}


