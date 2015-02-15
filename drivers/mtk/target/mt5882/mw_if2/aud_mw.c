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
                    Include files
 ----------------------------------------------------------------------------*/
#include "x_util.h"         //53xx_com_driver/inc/
#include "mtal.h"
#include "aud_if.h"
#include "x_aud_dec.h"
#ifdef LINUX_TURNKEY_SOLUTION
#include "u_rm_dev_types.h" 
#else
#include "x_rm_dev_types.h" 
#endif

#include "x_aud_enc.h"

#ifdef CC_AUD_4_DECODER_SUPPORT
#define AUD_DEC_PIP_SUB         4   //decoder id "4" means audio in PIP sub mode
#elif defined (CC_MT5391_AUD_3_DECODER)
#define AUD_DEC_PIP_SUB         3   //decoder id "3" means audio in PIP sub mode
#else
#define AUD_DEC_PIP_SUB         2   //decoder id "2" means audio in PIP sub mode
#endif

#ifdef CC_53XX_SWDMX_V2
EXTERN VOID _MW_DMX_MM_AudMwNfyFunc(void *pvNfyTag, UINT8 u1DecId,
    UINT32 u4NfyCond, UINT32 u4Data1, UINT32 u4Data2, UINT32 u4Data3);
EXTERN VOID _MW_DMX_MM_AudSetCtrl(UINT8 u1SwdmxSrcId, UINT8 u1DecId, UINT32 u4AudCtrl);
EXTERN UINT8 _MW_DMX_MM_GetSwdmxSrcId(UINT16 u2CompId);
#else
EXTERN VOID _MW_DMX_MM_AudMwNfyFunc(void *pvNfyTag, UINT8 u1DecId,
    UINT32 u4NfyCond, UINT32 u4Data1, UINT32 u4Data2);
EXTERN VOID _MW_DMX_MM_AudSetCtrl(UINT8 u1DecId, UINT32 u4AudCtrl);
#endif 

static UINT8 _AudConvertDecId(UINT16 u2_comp_id, UINT8* u1DecId)
{
    if (u2_comp_id > (AUD_DEC_NUM - 1))
    {
        *u1DecId = AUD_DEC_MAIN;
        if (u2_comp_id == AUD_DEC_PIP_SUB) //pseduo decoder id for audio in PIP sub mode
        {
            ////AUD_DrvSetPIPVideoPath(AUD_DEC_MAIN, 1); //PIP sub mode : 1 //PIP information is directly set by MW.
            return 0;
        }
        else
        {
            return 1;   //middleware set wrong decoder id
        }
    }
    ////AUD_DrvSetPIPVideoPath(AUD_DEC_MAIN, 0); //PIP main mode : 0    //PIP information is directly set by MW by AUD_DEC_SET_TYPE_PIPVIDEO_PATH_INFO.
    *u1DecId = (UINT8)u2_comp_id;
    return 0;
}

static INT32 _AudConnect(DRV_COMP_ID_T*  pt_comp_id,
                        DRV_CONN_TYPE_T  e_conn_type,
                        const VOID*      pv_conn_info,
                        SIZE_T           z_conn_info_len,
                        VOID*            pv_tag,
                        x_rm_nfy_fct     pf_nfy)
{
    DRV_COMP_ID_T* prCompReg;
    INT32 eResult= RMR_OK;
    UINT8 u1SwdmxSrcId = 0xff;
    UINT8 u1DecId = AUD_DEC_MAIN;

    // Check component type and id

    if (pt_comp_id == NULL)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->e_type != DRVT_AUD_DEC)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->b_sel_out_port != FALSE)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (pt_comp_id->u.ui1_inp_port > 1)
    {
        return (RMR_DRV_INV_CONN_INFO);
    }

    if (e_conn_type != RM_CONN_TYPE_COMP_ID)
    {
       return (RMR_DRV_INV_CONN_INFO);
    }

    //Convert Decoder ID
    if (_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId))
    {
        return (RMR_DRV_INV_SET_INFO);
    }    
    
    prCompReg = (DRV_COMP_ID_T*)pv_conn_info;

    if ((prCompReg->e_type != DRVT_DEMUX_TS_PES_PACKET) &&
        (prCompReg->e_type != DRVT_DEMUX_CONTAINER_ES) &&
        (prCompReg->e_type != DRVT_AVC_HDMI) &&
        (prCompReg->e_type != DRVT_AVC_AUDIO_INP) &&
        (prCompReg->e_type != DRVT_AVC_SPDIF) &&
        (prCompReg->e_type != DRVT_TUNER_SAT_ANA) &&
        (prCompReg->e_type != DRVT_TUNER_CAB_ANA) &&
        (prCompReg->e_type != DRVT_TUNER_TER_ANA) &&
        (prCompReg->e_type != DRVT_AVC_SCART))
    {
        return (RMR_DRV_INV_CONN_INFO);
    }    

    switch (prCompReg->e_type)
    {
    case DRVT_DEMUX_CONTAINER_ES:
#ifdef ENABLE_MULTIMEDIA
        u1SwdmxSrcId = _MW_DMX_MM_GetSwdmxSrcId(prCompReg->ui2_id);
#endif
        break;
    default:
        break;
    }

    MTAUDDEC_AudConnect(pt_comp_id, e_conn_type, pv_conn_info, u1SwdmxSrcId, &eResult);
    
    return eResult;
}

static INT32 _AudDisconnect(DRV_COMP_ID_T* pt_comp_id,
                        DRV_DISC_TYPE_T  e_disc_type,
                        const VOID*      pv_disc_info,
                        SIZE_T           z_disc_info_len)
{
    INT32 eResult = RMR_OK;
    DRV_COMP_ID_T* prCompReg;
    UINT8 u1DecId = AUD_DEC_MAIN;

    // Check component type and id
    if (pt_comp_id == NULL)
        return (RMR_DRV_INV_DISC_INFO);

    if (pt_comp_id->e_type != DRVT_AUD_DEC)
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    //Convert Decoder ID
    if (_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId))
    {
        return (RMR_DRV_INV_SET_INFO);
    }

    if (pt_comp_id->b_sel_out_port != FALSE)
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    if (pt_comp_id->u.ui1_inp_port > 1)
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    if (e_disc_type != RM_CONN_TYPE_COMP_ID)
    {
        return (RMR_DRV_INV_DISC_INFO);
    }

    prCompReg = (DRV_COMP_ID_T*)pv_disc_info;

    if ((prCompReg->e_type != DRVT_DEMUX_TS_PES_PACKET) &&
        (prCompReg->e_type != DRVT_DEMUX_CONTAINER_ES) &&
        (prCompReg->e_type != DRVT_AVC_COMP_VIDEO) &&
        (prCompReg->e_type != DRVT_AVC_HDMI) &&
        (prCompReg->e_type != DRVT_AVC_AUDIO_INP) &&
        (prCompReg->e_type != DRVT_AVC_SPDIF) &&
        (prCompReg->e_type != DRVT_TUNER_SAT_ANA) &&
        (prCompReg->e_type != DRVT_TUNER_CAB_ANA) &&
        (prCompReg->e_type != DRVT_TUNER_TER_ANA) &&
        (prCompReg->e_type != DRVT_AVC_SCART))
    {
        return (RMR_DRV_INV_DISC_INFO);
    }
    
    MTAUDDEC_AudDisconnect(pt_comp_id, e_disc_type, pv_disc_info, &eResult);
    
    return eResult;
}

static INT32 _AudGet(DRV_COMP_ID_T* pt_comp_id,
                    DRV_GET_TYPE_T  e_get_type,
                    VOID*           pv_get_info,
                    SIZE_T*         pz_get_info_len)
{
    INT32 eResult = 0;
    UINT8 u1DecId = AUD_DEC_MAIN;

    // Check component type and id
    if (pt_comp_id == NULL)
        return (RMR_DRV_INV_GET_INFO);

    if (pt_comp_id->e_type != DRVT_AUD_DEC)
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    //Convert Decoder ID
    if (_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId))
    {
        return (RMR_DRV_INV_SET_INFO);
    }
    
    MTAUDDEC_AudGet(pt_comp_id, e_get_type, pv_get_info, pz_get_info_len, &eResult);
    
    return eResult;
}

static INT32 _AudSet(DRV_COMP_ID_T* pt_comp_id,
                    DRV_SET_TYPE_T  e_set_type,
                    const VOID*     pv_set_info,
                    SIZE_T          z_set_info_len)
{
    INT32 eResult = 0;
    UINT8 u1DecId = AUD_DEC_MAIN;

    // Check component type and id
    if (pt_comp_id == NULL)
    {
        return (RMR_DRV_INV_SET_INFO);    
    }

    if (pt_comp_id->e_type != DRVT_AUD_DEC)
    {
        return (RMR_DRV_INV_SET_INFO);
    }

    //Convert Decoder ID
    if (_AudConvertDecId(pt_comp_id->ui2_id, &u1DecId))
    {
        return (RMR_DRV_INV_SET_INFO);
    }
    
    // Perform set opertion
    switch (e_set_type)
    {
    case AUD_DEC_SET_TYPE_NFY_FCT:
        MTAUDDEC_AUD_MW_RegCbFunc(MTAUDDEC_AUD_MW_CB_FUNC_NFY, (UINT32)(((AUD_DEC_NFY_INFO_T *)pv_set_info)->pf_aud_dec_nfy), u1DecId);
        MTAUDDEC_AUD_MW_RegCbFunc(MTAUDDEC_AUD_MW_CB_FUNC_DMX_NFY, 0, 0);
        break;

    case AUD_DEC_SET_TYPE_HEADPHONE_DETECT_NFY:
        MTAUDDEC_AUD_MW_RegCbFunc(MTAUDDEC_AUD_MW_CB_FUNC_HEADPHONE_DETECT_NFY, (UINT32)(((AUD_DEC_HDPH_DETECT_NFT_INFO_T *)pv_set_info)->pf_hdph_detect_nfy), u1DecId);
        break;

    case AUD_DEC_SET_TYPE_UPLOAD_DATA_NFY:
        MTAUDDEC_AUD_MW_RegCbFunc(MTAUDDEC_AUD_MW_CB_FUNC_UPLOAD_DATA_NFY, (UINT32)(((AUD_DEC_UPLOAD_DATA_NFT_INFO_T *)pv_set_info)->pf_upload_data_nfy), u1DecId);
        break;

#ifdef CC_AUD_SUPPORT_ENC
    case AUD_ENC_SET_TYPE_CTRL:
        if ((UINT32)pv_set_info == AUD_DEC_UPLOAD_OFF)
        {
            MTAUDENC_Enable(0);
        }
        else
        {
            MTAUDENC_Enable(1);
        }
        return 0;

    case AUD_ENC_SET_TYPE_SETTINGS:
        
    	if(AUD_ENC_FMT_TYPE_MP3 == ((AUD_ENC_SETTINGS_T *)pv_set_info)->e_enc_fmt)
    	{
    		MTAUDENC_Set((UINT8)((AUD_ENC_SETTINGS_T *)pv_set_info)->e_src_type, MTAUDENC_ENCODE_MP3);
    	}
        else if (AUD_ENC_FMT_TYPE_AAC == ((AUD_ENC_SETTINGS_T *)pv_set_info)->e_enc_fmt)
        {
    		MTAUDENC_Set((UINT8)((AUD_ENC_SETTINGS_T *)pv_set_info)->e_src_type, MTAUDENC_ENCODE_AAC);
        }
        else if (AUD_ENC_FMT_TYPE_PCM == ((AUD_ENC_SETTINGS_T *)pv_set_info)->e_enc_fmt)
        {
            MTAUDENC_Set((UINT8)((AUD_ENC_SETTINGS_T *)pv_set_info)->e_src_type, MTAUDENC_ENCODE_PCM);
        }
        else
        {
            return RMR_DRV_INV_SET_INFO;
        }
        
        return 0;
#endif

    default:
        break;
    }

    MTAUDDEC_AudSet(pt_comp_id, e_set_type, pv_set_info, z_set_info_len, &eResult);
    
    return eResult;    
}

/*-----------------------------------------------------------------------------
                    Global Function definitions
 ----------------------------------------------------------------------------*/
extern VOID* _pfnAudMwDmxNfy;
extern VOID* _pfnAudMwDmxAudSetCtrl;

INT32 MW_AUD_Init(void)
{
    DRV_COMP_REG_T tAudCompReg;
    DRV_COMP_FCT_TBL_T tAudFctTbl;
    UINT32 u4Flag;

    MTAUDDEC_MW_AUD_Init();
#ifdef ENABLE_MULTIMEDIA
    _pfnAudMwDmxNfy = &_MW_DMX_MM_AudMwNfyFunc;
    _pfnAudMwDmxAudSetCtrl = &_MW_DMX_MM_AudSetCtrl;
#endif
    MTAUDDEC_AUD_MW_RegCbFunc(MTAUDDEC_AUD_MW_CB_FUNC_DMX_AUDSETCTRL, 0, 0);
    
    /* Setup component ID */
    tAudCompReg.e_type = DRVT_AUD_DEC;
    tAudCompReg.e_id_type = ID_TYPE_RANGE;
    tAudCompReg.u.t_range.ui2_first_id = 0; //AUD_DEC_MAIN;
    tAudCompReg.u.t_range.ui2_delta_id = 1;
    tAudCompReg.u.t_range.ui2_num_of_ids = 5; //AUD_DEC_NUM + 1;  /*there are 3 decoders 0 for MAIN , 1 for AD , 2 for SUB*/
    tAudCompReg.u.t_range.pv_first_tag = NULL;                             /* 4 decoders for MW under 3-decoder architect */
    tAudCompReg.u.t_range.pv_delta_tag = NULL;                            /* 0 for MAIN, 1 for SCART, 2 for AD, 3 for SUB */
    tAudCompReg.u.t_range.ui1_port = 1;

    /* Setup component function table */
    tAudFctTbl.pf_rm_connect = _AudConnect;
    tAudFctTbl.pf_rm_disconnect = _AudDisconnect;
    tAudFctTbl.pf_rm_get = _AudGet;
    tAudFctTbl.pf_rm_set = _AudSet;

    u4Flag = DRV_FLAG_SINGLE_CONN_ON_OUTPUT;

    /* Register component with the Resource Manager database */
    return x_rm_reg_comp(&tAudCompReg,
                         1,
                         1,
                         NULL,           //"aud decoder",
                         u4Flag,
                         &tAudFctTbl,
                         NULL,
                         0);
}

INT32 d_custom_aud_set (
    DRV_CUSTOM_AUD_TYPE_T     e_aud_type,
    VOID*                     pv_set_type,
    SIZE_T                    z_size,
    BOOL                      b_store
)
{
    INT32 eResult = 0;
    
    MTAUDDEC_d_custom_aud_set(e_aud_type, pv_set_type, z_size, &eResult);
    
    return eResult;
}

INT32 d_custom_aud_get (
    DRV_CUSTOM_AUD_TYPE_T     e_aud_type,
    VOID*                     pv_get_type,
    SIZE_T*                   pz_size
)
{
    INT32 eResult = 0;
    
    MTAUDDEC_d_custom_aud_get(e_aud_type, pv_get_type, pz_size, &eResult);
    
    return eResult;
}

INT32 d_custom_aud_get_min_max (
    DRV_CUSTOM_AUD_TYPE_T     e_aud_type,
    INT16*                    pui2_min,
    INT16*                    pui2_max
)
{
    // Check input parameters
    if((pui2_min == NULL) || (pui2_max == NULL))
    {
        return RMR_DRV_INV_GET_INFO;
    }

    MTAUDDEC_d_custom_aud_get_min_max(e_aud_type, pui2_min, pui2_max);

    return RMR_OK;
}
