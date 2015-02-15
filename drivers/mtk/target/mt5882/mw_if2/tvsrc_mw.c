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
 * $RCSfile: tvsrc_mw.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X/DTV_X_HQ_int/DTV_X_ATSC/14 $
 * $SWAuthor: Clear Case Administrator $
 * $MD5HEX: a872122d22ab4b982b54b29e95d0ff55 $
 *
 * Description:
 *         AV Source middleware driver API.
 *---------------------------------------------------------------------------*/ 
/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_assert.h"
#include "source_table.h"
#include "x_avc.h"
#include "x_dbg.h"

#include "vdo_if.h"
#include "video_def.h"
#include "drv_video.h"
#include "drv_scart.h"
#include "sv_const.h"
#include "drv_adcset.h"
#include "mw_comp_excl_reg.h"
#include "d_drv_cust.h"
#include "d_drv_cust_util.h"
#include "tvsrc_mw.h"
#include "tve_if.h"
#include "vdp_if.h"

#if 0 //#ifdef LINUX_TURNKEY_SOLUTION
#include "cb_data_rm.h"
#include "cb_param.h"
#define x_dbg_stmt
#endif

#include "vdec_mw.h"
#include "mtscart.h"
#include "mtvdecex_mwif.h"
#include "u_dbg.h"




/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#if 0
#define AVC_DBG_STMT( n )     { if ( b_avc_dbg ) { UNUSED(x_dbg_stmt n); } }
#endif

#define AVC_COMP_LIST_NUM_MAX      ((SIZE_T) 48)
#define SRC_COMP_NS                (SV_VS_MAX+1)

#define NPTV_VID_SRC_MAPPING_TBL_END             { (UINT32)SV_VS_MAX, DRVT_UNKNOWN, 0 }
#define IS_NPTV_VID_SRC_MAPPING_TBL_END(pt_elm)   \
(                                                 \
    ((pt_elm)->e_vid_src_idx == (UINT32)SV_VS_MAX) &&     \
    ((pt_elm)->e_type == DRVT_UNKNOWN) &&         \
    ((pt_elm)->ui2_comp_id == 0)                  \
)

#define AVC_SCART_CONNECTED_TO_TUNER_TBL_SIZE      8

#define AVC_IS_TUNER_SRC( e_type )   ( (e_type == DRVT_TUNER_SAT_ANA) ||         \
                                       (e_type == DRVT_TUNER_CAB_ANA) ||         \
                                       (e_type == DRVT_TUNER_TER_ANA) )
                                     
extern void _MW_VDP_SCART_PIN_8_CB(UCHAR ucVdpId, DRV_CUSTOM_SCART_PIN_8_T t_scart_pin_8);
UINT8 MWGetScartPin8Status(UINT8 u1Path);

typedef struct _NPTV_VID_SRC_MAPPING_TBL_T
{
    UINT32             e_vid_src_idx;
    DRV_TYPE_T         e_type;
    UINT16             ui2_comp_id;
}   NPTV_VID_SRC_MAPPING_TBL_T;

typedef struct _AVC_COMBI_MW_NFY_INFO_T
{
    avc_combi_nfy_fct   pf_combi_nfy;
    VOID*               pv_tag;
}   AVC_COMBI_MW_NFY_INFO_T;

typedef struct _AVC_SCART_CONNECTED_TO_TUNER_T
{
    DRV_TYPE_T         e_type;
    UINT16             ui2_comp_id;
    DRV_TYPE_T         e_src_type;
    UINT16             ui2_src_comp_id;
}   AVC_SCART_CONNECTED_TO_TUNER_T;


/*-----------------------------------------------------------------------------
                    data declaraions
 ----------------------------------------------------------------------------*/
#if 0
static BOOL                 b_avc_dbg = FALSE;
#endif

static AVC_SCART_INFO_T     t_scart_info = {FALSE, 0, 0, 0, 0, AVC_SCART_BYPASS_TRUE};

static AVC_SCART_CONNECTED_TO_TUNER_T   a_avc_scart_conn2tuner_tbl[AVC_SCART_CONNECTED_TO_TUNER_TBL_SIZE] = 
{
    { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }, { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }, 
    { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }, { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }, 
    { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }, { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }, 
    { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }, { DRVT_UNKNOWN, 0, DRVT_UNKNOWN, 0 }
};

/* NPTV defined video source mapping table */
static NPTV_VID_SRC_MAPPING_TBL_T at_vid_src_mapping_tbl[] =
{
    { (UINT32)SV_VS_TUNER1, DRVT_TUNER_CAB_ANA,  0 },
    { (UINT32)SV_VS_TUNER1, DRVT_TUNER_TER_ANA,  0 },
    { (UINT32)SV_VS_CVBS1,  DRVT_AVC_COMP_VIDEO, 0 },
    { (UINT32)SV_VS_CVBS2,  DRVT_AVC_COMP_VIDEO, 1 },
    { (UINT32)SV_VS_CVBS3,  DRVT_AVC_COMP_VIDEO, 2 },
    { (UINT32)SV_VS_CVBS4,  DRVT_AVC_COMP_VIDEO, 3 },
    { (UINT32)SV_VS_S1,     DRVT_AVC_S_VIDEO,    0 },
    { (UINT32)SV_VS_S2,     DRVT_AVC_S_VIDEO,    1 },
    { (UINT32)SV_VS_S3,     DRVT_AVC_S_VIDEO,    2 },
    { (UINT32)SV_VS_YPbPr1, DRVT_AVC_Y_PB_PR,    0 },
    { (UINT32)SV_VS_YPbPr2, DRVT_AVC_Y_PB_PR,    1 },
    { (UINT32)SV_VS_YPbPr3, DRVT_AVC_Y_PB_PR,    2 },
    { (UINT32)SV_VS_YPbPr4, DRVT_AVC_Y_PB_PR,    3 },
    { (UINT32)SV_VS_VGACOMP1,DRVT_AVC_Y_PB_PR,   4 },
    { (UINT32)SV_VS_HDMI1,  DRVT_AVC_HDMI,       0 },
    { (UINT32)SV_VS_HDMI2,  DRVT_AVC_HDMI,       1 },
    { (UINT32)SV_VS_HDMI3,  DRVT_AVC_HDMI,       2 },
    { (UINT32)SV_VS_HDMI4,  DRVT_AVC_HDMI,       3 },
    { (UINT32)SV_VS_HDMI5,  DRVT_AVC_HDMI,       4 },
    { (UINT32)SV_VS_VGA,    DRVT_AVC_VGA,        0 },
    { (UINT32)SV_VS_DVI,    DRVT_AVC_DVI,        0 },
    { (UINT32)SV_VS_SCART1, DRVT_AVC_SCART,      (AVC_SCART_TYPE_SOURCE + 0) },
    { (UINT32)SV_VS_SCART1, DRVT_AVC_SCART,      (AVC_SCART_TYPE_SINK + 0) },
    { (UINT32)SV_VS_SCART1, DRVT_AVC_SCART,      (AVC_SCART_TYPE_SOURCE_SINK + 0) },    
    { (UINT32)SV_VS_SCART1, DRVT_AVC_SCART,      (AVC_SCART_TYPE_MONITOR + 0) },
    { (UINT32)SV_VS_SCART2, DRVT_AVC_SCART,      (AVC_SCART_TYPE_SOURCE + 1) },
    { (UINT32)SV_VS_SCART2, DRVT_AVC_SCART,      (AVC_SCART_TYPE_SINK + 1) },   
    { (UINT32)SV_VS_SCART2, DRVT_AVC_SCART,      (AVC_SCART_TYPE_SOURCE_SINK + 1) },        
    { (UINT32)SV_VS_SCART2, DRVT_AVC_SCART,      (AVC_SCART_TYPE_MONITOR + 1) },    

    /* DO NOT delete this */
    NPTV_VID_SRC_MAPPING_TBL_END
};

/* Notify info */
static AVC_NFY_INFO_T at_avc_nfy_info[AVC_COMP_LIST_NUM_MAX];

/* COMBI Nofity info */
static AVC_COMBI_MW_NFY_INFO_T at_avc_combi_mw_nfy_info[AVC_COMP_LIST_NUM_MAX];

/* Connection notification tags */
static VOID* apf_rm_conn_tag[AVC_COMP_LIST_NUM_MAX];

static DRV_ID_T at_avc_comp_list[AVC_COMP_LIST_NUM_MAX] =
{
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END, DRV_ID_END,
    DRV_ID_END, DRV_ID_END, DRV_ID_END
};

static BOOL ab_avc_comp_no_disc_list[AVC_COMP_LIST_NUM_MAX] =
{
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE
};

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
extern BOOL fgHDMISupportAudio(void);

/*-----------------------------------------------------------------------------
                    Internal functions implementations
 ----------------------------------------------------------------------------*/
static UINT8 _alloc_avc_scart_conn2tuner_tbl_free_blk( 
    DRV_TYPE_T e_type, UINT16 ui2_comp_id,
    DRV_TYPE_T e_src_type, UINT16 ui2_src_comp_id )
{
    UINT8 ui1_idx;
    
    for(ui1_idx = 0; ui1_idx < AVC_SCART_CONNECTED_TO_TUNER_TBL_SIZE; ui1_idx++)
    {
        if ( (a_avc_scart_conn2tuner_tbl[ui1_idx].e_type == DRVT_UNKNOWN &&
              a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_comp_id == 0) ||
             (a_avc_scart_conn2tuner_tbl[ui1_idx].e_type == e_type &&
              a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_comp_id == ui2_comp_id)  )
        {
            a_avc_scart_conn2tuner_tbl[ui1_idx].e_type = e_type;
            a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_comp_id = ui2_comp_id;
            a_avc_scart_conn2tuner_tbl[ui1_idx].e_src_type = e_src_type;
            a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_src_comp_id = ui2_src_comp_id;
            return ui1_idx;
        }
    }
    
    return 0xFF;
}

static VOID _free_avc_scart_conn2tuner_tbl_free_blk( DRV_TYPE_T e_type, UINT16 ui2_comp_id )
{
    UINT8 ui1_idx;
    
    for(ui1_idx = 0; ui1_idx < AVC_SCART_CONNECTED_TO_TUNER_TBL_SIZE; ui1_idx++)
    {
        if ( a_avc_scart_conn2tuner_tbl[ui1_idx].e_type == e_type &&
             a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_comp_id == ui2_comp_id )
        {
            a_avc_scart_conn2tuner_tbl[ui1_idx].e_type = DRVT_UNKNOWN;
            a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_comp_id = 0;
            a_avc_scart_conn2tuner_tbl[ui1_idx].e_src_type = DRVT_UNKNOWN;
            a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_src_comp_id = 0;
            return;
        }
    }
}

extern BOOL avc_scart_is_from_tuner_src( 
    DRV_TYPE_T e_type, UINT16 ui2_comp_id,
    DRV_TYPE_T* pe_src_type, UINT16* pui2_src_comp_id )
{
    UINT8 ui1_idx;
    
    for(ui1_idx = 0; ui1_idx < AVC_SCART_CONNECTED_TO_TUNER_TBL_SIZE; ui1_idx++)
    {
        if ( a_avc_scart_conn2tuner_tbl[ui1_idx].e_type == e_type &&
             a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_comp_id == ui2_comp_id )
        {
            if ( pe_src_type )
            {
                *pe_src_type = a_avc_scart_conn2tuner_tbl[ui1_idx].e_src_type;
            }
            if ( pui2_src_comp_id )
            {
                *pui2_src_comp_id = a_avc_scart_conn2tuner_tbl[ui1_idx].ui2_src_comp_id;
            }
            return TRUE;
        }
    }
    return FALSE;
}

#if 0 //#ifdef LINUX_TURNKEY_SOLUTION
static TV_SRC_AVC_NFY_PARAM tv_src_avc_param;

static x_avc_nfy_fct tv_src_avc_nfy_fct_tbl[AVC_COMP_LIST_NUM_MAX];

static VOID set_tv_src_avc_nfy(UINT32 ui4_id)
{
    tv_src_avc_param.pf_nfy = tv_src_avc_nfy_fct_tbl[ui4_id];
}

static VOID tv_src_avc_nfy_fct(VOID*            pv_nfy_tag,
                               TV_DEC_COND_T    e_nfy_cond,
                               UINT32           ui4_data_1,
                               UINT32           ui4_data_2)
{
    tv_src_avc_param.pv_nfy_tag = pv_nfy_tag;
    tv_src_avc_param.e_nfy_cond = e_nfy_cond;
    tv_src_avc_param.ui4_data_1 = ui4_data_1;
    tv_src_avc_param.ui4_data_2 = ui4_data_2;
    _CB_PutEvent_Adpt(CB_DRV_TV_SRC_AVC_NFY, sizeof(TV_SRC_AVC_NFY_PARAM), &tv_src_avc_param);
}
#endif

/*-----------------------------------------------------------------------------
 * Name: _get_avc_comp_list_idx
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/ 
static UINT8 _get_avc_comp_list_idx( DRV_TYPE_T e_type, UINT16 ui2_comp_id )
{
    UINT8 ui1_idx;
    
    for(ui1_idx = 0;
        ui1_idx < AVC_COMP_LIST_NUM_MAX && !IS_DRV_ID_END((&at_avc_comp_list[ui1_idx])); 
        ui1_idx++)
    {
        if( (at_avc_comp_list[ui1_idx].e_type == e_type) && 
            (at_avc_comp_list[ui1_idx].ui2_id == ui2_comp_id) )
        {
            return (ui1_idx);
        }
    }
    
    return (0xFF);
}


/*-----------------------------------------------------------------------------
 * Name: _add_avc_comp_list
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/ 
static BOOL _add_avc_comp_list( DRV_TYPE_T e_type, UINT16 ui2_comp_id )
{
    UINT8 ui1_idx;
    
    for(ui1_idx = 0; 
        ui1_idx < AVC_COMP_LIST_NUM_MAX && !IS_DRV_ID_END((&at_avc_comp_list[ui1_idx])) ; 
        ui1_idx++)
    {
        if( (at_avc_comp_list[ui1_idx].e_type == e_type) && 
            (at_avc_comp_list[ui1_idx].ui2_id == ui2_comp_id) )
        {
            return FALSE;
        }
    }
    
    if ( ui1_idx >= AVC_COMP_LIST_NUM_MAX )
    {
        return FALSE;
    }
    
    at_avc_comp_list[ui1_idx].e_type = e_type;
    at_avc_comp_list[ui1_idx].ui2_id = ui2_comp_id;
    
    return TRUE;
}


/*-----------------------------------------------------------------------------
 * Name: _SrcMwConnect
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
//lint -e818
static INT32 _SrcMwConnect(
    DRV_COMP_ID_T*   pt_comp_id,
    DRV_CONN_TYPE_T  e_conn_type,
    const VOID*      pv_conn_info,
    SIZE_T           z_conn_info_len,
    VOID*            pv_tag,
    x_rm_nfy_fct     pf_nfy )
{
    UINT8           ui1_avc_comp_list_idx;
    DRV_COMP_ID_T*  prVSrcCompReg;    

    UNUSED(pf_nfy);
    UNUSED(e_conn_type);
    UNUSED(z_conn_info_len);

    if (pt_comp_id == NULL)
    {
        return RMR_DRV_INV_CONN_INFO;
    }
    
    ui1_avc_comp_list_idx = _get_avc_comp_list_idx( 
        pt_comp_id->e_type, pt_comp_id->ui2_id );
    if ( ui1_avc_comp_list_idx >= AVC_COMP_LIST_NUM_MAX)
    {
        return RMR_DRV_INV_CONN_INFO;
    }
    
    /* Should we separate video/audio here? */
    apf_rm_conn_tag[ui1_avc_comp_list_idx] = pv_tag;
    
    /* Clear the notify function */
    /* If the driver type is SCART, its notify function record won't be cleared.
       This is for SCART pin-8 detection purpose */
    if (pt_comp_id->e_type != DRVT_AVC_SCART)
    {        
        at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy = NULL;
        at_avc_nfy_info[ui1_avc_comp_list_idx].pv_tag = NULL;
    }
            
    ab_avc_comp_no_disc_list[ui1_avc_comp_list_idx] = FALSE;
    
    /* Store SCART related info */
#if 0
    if (pv_conn_info == NULL)
    {
        return RMR_DRV_INV_CONN_INFO;
    }
    prVSrcCompReg = (DRV_COMP_ID_T*)pv_conn_info;    
    t_scart_info.e_tuner_type = prVSrcCompReg->e_type;
    t_scart_info.ui2_tuner_id = prVSrcCompReg->ui2_id;    
    t_scart_info.e_scart_type = pt_comp_id->e_type;
    t_scart_info.ui2_scart_id = pt_comp_id->ui2_id;     
    if ((pt_comp_id->ui2_id & AVC_SCART_TYPE_SOURCE_SINK) == AVC_SCART_TYPE_SOURCE_SINK)
    {    
        t_scart_info.b_canal_plus_support = TRUE;
    }
    else
    {
        t_scart_info.b_canal_plus_support = FALSE;
    }        
#else
    if (pv_conn_info == NULL)
    {
        //t_scart_info.b_canal_plus_support = FALSE;
        return RMR_OK;
    }
    else
    {
        prVSrcCompReg = (DRV_COMP_ID_T*)pv_conn_info;    

        if ((pt_comp_id->ui2_id & AVC_SCART_TYPE_SOURCE_SINK) == AVC_SCART_TYPE_SOURCE_SINK && pt_comp_id->e_type == DRVT_AVC_SCART )
        {
            t_scart_info.e_tuner_type = prVSrcCompReg->e_type;
            t_scart_info.ui2_tuner_id = prVSrcCompReg->ui2_id;    
            t_scart_info.e_scart_type = pt_comp_id->e_type;
            t_scart_info.ui2_scart_id = pt_comp_id->ui2_id;     
            t_scart_info.b_canal_plus_support = TRUE;
        }
        else if(pt_comp_id->e_type == DRVT_AVC_SCART)
        {
            t_scart_info.b_canal_plus_support = FALSE;
        }
        else /* other than scart type, ignore the tvsrc connection */
        {
            return RMR_OK;
        }
    }
#endif

    if ((TRUE == t_scart_info.b_canal_plus_support) &&
        (pt_comp_id->ui2_id & AVC_SCART_TYPE_SOURCE_SINK) == AVC_SCART_TYPE_SOURCE_SINK && pt_comp_id->e_type == DRVT_AVC_SCART )
    {
        UINT8 ui1_idx = _alloc_avc_scart_conn2tuner_tbl_free_blk( 
            pt_comp_id->e_type, pt_comp_id->ui2_id,
            prVSrcCompReg->e_type, prVSrcCompReg->ui2_id );
        if ( ui1_idx == 0xFF )
        {
            return RMR_DRV_INV_CONN_INFO;
        }
    }
    
    /* We don't check the valid of source combination here, 
       just a pseudo connection */

    return RMR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _SrcMwDisconnect
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static INT32 _SrcMwDisconnect(DRV_COMP_ID_T*   pt_comp_id,
                        DRV_DISC_TYPE_T  e_disc_type,
                        const VOID*      pv_disc_info,
                        SIZE_T           z_disc_info_len)
{
    UINT8           ui1_avc_comp_list_idx;

    UNUSED(e_disc_type);
    UNUSED(pv_disc_info);
    UNUSED(z_disc_info_len);

    if (pt_comp_id == NULL)
    {
        return RMR_DRV_INV_CONN_INFO;
    }
    
    ui1_avc_comp_list_idx = _get_avc_comp_list_idx( 
        pt_comp_id->e_type, pt_comp_id->ui2_id );
    if ( ui1_avc_comp_list_idx >= AVC_COMP_LIST_NUM_MAX)
    {
        return RMR_DRV_INV_CONN_INFO;
    }

    /* Clear the notify function */
    /* If the driver type is SCART, its notify function record won't be cleared.
       This is for SCART pin-8 detection purpose */    
    if (pt_comp_id->e_type != DRVT_AVC_SCART)
    {          
        at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy = NULL;
        at_avc_nfy_info[ui1_avc_comp_list_idx].pv_tag = NULL;
    }
            
    if ( apf_rm_conn_tag[ui1_avc_comp_list_idx] )
    {
        apf_rm_conn_tag[ui1_avc_comp_list_idx] = NULL;
    }

    if ( pt_comp_id->e_type == DRVT_AVC_SCART )
    {
        _free_avc_scart_conn2tuner_tbl_free_blk( pt_comp_id->e_type, pt_comp_id->ui2_id );
    }

    return RMR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: _SrcMwGet
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static INT32 _SrcMwGet(DRV_COMP_ID_T*  pt_comp_id,
                    DRV_GET_TYPE_T  e_get_type,
                    VOID*           pv_get_info,
                    SIZE_T*         pz_get_info_len)
{
    MTVDO_PATH_T       u1Path = 0xff;
    MTVDECEX_SIG_STATUS       u1SigStatus = MTVDECEX_VDO_UNKNOWN;
    UINT8       ui1_avc_comp_list_idx;
    MTSCART_PIN_8_T       ePin8Status = MTSCART_PIN_8_UNKNOWN;
    UINT16       u2Pin16Status;
    MTSCART_INPUT_MODE_T       eScartInputMode = MTSCART_AUTO;
    
    /* Check component type and id */
    if (pt_comp_id == NULL)
    {
        return (RMR_DRV_INV_GET_INFO);
    }
    
    ui1_avc_comp_list_idx = _get_avc_comp_list_idx( 
        pt_comp_id->e_type, pt_comp_id->ui2_id);
    if ( ui1_avc_comp_list_idx >= AVC_COMP_LIST_NUM_MAX )
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    if ( (pt_comp_id->e_type != DRVT_AVC_AUDIO_INP) &&
         (pt_comp_id->e_type != DRVT_AVC_SCART) &&
         (pt_comp_id->e_type != DRVT_AVC_SPDIF) )
    {
        if( (_rMChannelCompInfo.u2_e_type == pt_comp_id->e_type) &&
            (_rMChannelCompInfo.u2_e_ui2_id == pt_comp_id->ui2_id) )
        {
            u1Path = MTVDO_MAIN;
        }
        else if( (_rPChannelCompInfo.u2_e_type == pt_comp_id->e_type) &&
                 (_rPChannelCompInfo.u2_e_ui2_id == pt_comp_id->ui2_id) )
        {
            u1Path = MTVDO_SUB;
        }
        else
        {
            return RMR_DRV_INV_GET_INFO; 
        }
    }
      
    /* Perform get opertion */
    switch (e_get_type)
    {
        case AVC_GET_TYPE_NO_CONN_DISC_NFY:
            if ( (NULL == pv_get_info) || (NULL == pz_get_info_len) || (*pz_get_info_len < sizeof(BOOL)) )
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            *(BOOL*)pv_get_info = ab_avc_comp_no_disc_list[ui1_avc_comp_list_idx];
            break;
                    
        case AVC_GET_TYPE_SIGNAL_STATUS:
            if ( (NULL == pv_get_info) || (NULL == pz_get_info_len) || (*pz_get_info_len < sizeof(UINT32)) )
            {
                return (RMR_DRV_NOT_ENOUGH_SPACE);
            }
            if ( (pt_comp_id->e_type == DRVT_AVC_AUDIO_INP) ||
                 (pt_comp_id->e_type == DRVT_AVC_SPDIF) )
            {
                (*(UINT32 *)pv_get_info) = AVC_STATE_AUDIO_SIGNAL;
            }
            else /* Is Video source */
            {
                /* Get Signal Status */
                 MTVDECEX_GetVideoStatus(u1Path, &u1SigStatus); 
                if( u1SigStatus == (UINT8)MTVDECEX_VDO_STABLE )
                {
                    (*(UINT32 *)pv_get_info) = AVC_STATE_TV_SIGNAL;
                }
                else if ( u1SigStatus == (UINT8)MTVDECEX_VDO_UNKNOWN )
                {
                    (*(UINT32 *)pv_get_info) = AVC_STATE_TV_SIGNAL_UNKNOWN;
                }
                else /* u1SigStatus == SV_VDO_NOSIGNAL or SV_VDO_NOSUPPORT */
                {
                    (*(UINT32 *)pv_get_info) = AVC_STATE_NONE;
                }
                
                if ( pt_comp_id->e_type == DRVT_AVC_HDMI )
                {
                    UINT8 bHDMISupportAudio = FALSE;
                    MTVDECEX_HDMI_IsSupportAudio(&bHDMISupportAudio);
                    if ( bHDMISupportAudio)
                    {
                        (*(UINT32 *)pv_get_info) |= AVC_STATE_AUDIO_SIGNAL;
                    }
                    
                     DBG_LOG_PRINT(( "[AVC] GET HDMI%d has%s Audio\r\n",
                        pt_comp_id->ui2_id, 
                        (((*(UINT32 *)pv_get_info) & AVC_STATE_AUDIO_SIGNAL)?"":" NO") ));
                }
            }
            break;
    
        case AVC_GET_TYPE_SCART_PIN_8_STATUS:
            if (pt_comp_id->e_type == DRVT_AVC_SCART) //chack source whether SCART is
            {   
                if ( (pv_get_info) && (pz_get_info_len) && (*pz_get_info_len >= sizeof(AVC_SCART_PIN_8_T)) )
                {
                    MTSCART_ID_T eScartIdx = MTSCART_ID_MAX;
                    UINT8 u1SrcIdx = SV_VS_MAX;
                    //Gene Chang For the SCART driver 2006.10.31
                    //UNUSED(bDrvGetScartFSStatus(rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id)));

                    u1SrcIdx = rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id);
                    if(u1SrcIdx == SV_VS_SCART1)
                        eScartIdx = MTSCART_1;
                    else if(u1SrcIdx == SV_VS_SCART2)
                        eScartIdx = MTSCART_2;
                    MTVDECEX_SCART_GetPin8Status(eScartIdx, &ePin8Status);
                    
                    switch(ePin8Status)
                    {
                        case MTSCART_PIN_8_ASPECT_4_3:
                            *(AVC_SCART_PIN_8_T*)pv_get_info = AVC_SCART_PIN_8_ASPECT_4_3;
                            break;
                            
                        case MTSCART_PIN_8_ASPECT_16_9 :
                            *(AVC_SCART_PIN_8_T*)pv_get_info = AVC_SCART_PIN_8_ASPECT_16_9;
                            break;
                            
                        case MTSCART_PIN_8_NO_SIGNAL:
                            *(AVC_SCART_PIN_8_T*)pv_get_info = AVC_SCART_PIN_8_NO_SIGNAL;
                            break;
                            
                        default :
                            *(AVC_SCART_PIN_8_T*)pv_get_info = AVC_SCART_PIN_8_UNKNOWN;
                            break;
                    }
                    
                    *pz_get_info_len = sizeof(AVC_SCART_PIN_8_T);
                }
                else
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
            }
            else
            {
                return RMR_DRV_INV_GET_INFO;
            }
            break;
    
        case AVC_GET_TYPE_SCART_PIN_16_STATUS:
            if (pt_comp_id->e_type == DRVT_AVC_SCART) //chack source whether SCART is
            {   
                if ( (pv_get_info) && (pz_get_info_len) && (*pz_get_info_len >= sizeof(AVC_SCART_PIN_16_T)) )
                {
                    //Gene Chang For the SCART driver 2006.10.31
                    u2Pin16Status = 0;
                    MTVDECEX_GetScartFBStatus(&u2Pin16Status);  //read register
                    if (u2Pin16Status == 0x0F)
                    {                       
                        *(AVC_SCART_PIN_16_T*)pv_get_info = AVC_SCART_PIN_16_COMPOSITE;
                    }                       
                    else if (u2Pin16Status == 0xF0)
                    {                       
                        *(AVC_SCART_PIN_16_T*)pv_get_info = AVC_SCART_PIN_16_RGB;
                    }                       
                    else if (u2Pin16Status == 0xFF)
                    {                       
                        *(AVC_SCART_PIN_16_T*)pv_get_info = AVC_SCART_PIN_16_TOGGLE;
                    }                       
                    else
                    {                       
                        *(AVC_SCART_PIN_16_T*)pv_get_info = AVC_SCART_PIN_16_UNKNOWN;
                    }                       
                    
                    *pz_get_info_len = sizeof(AVC_SCART_PIN_16_T);
                }
                else
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
            }           
            else
            {
                return RMR_DRV_INV_GET_INFO;
            }
            break;
            
        case AVC_GET_TYPE_SCART_INPUT_MODE:
            if (pt_comp_id->e_type == DRVT_AVC_SCART) //chack source whether SCART is
            {   
                if ( (pv_get_info) && (pz_get_info_len) && (*pz_get_info_len >= sizeof(AVC_SCART_INPUT_MODE_T)) )
                {
                    MTVDECEX_SCART_GetInputMode(&eScartInputMode);
                    switch (eScartInputMode)
                    {
                    case MTSCART_CVBS_RGB:
                        *(AVC_SCART_INPUT_MODE_T*)pv_get_info = AVC_SCART_INPUT_MODE_MIX;
                        break;
                        
                    case MTSCART_RGB:
                        *(AVC_SCART_INPUT_MODE_T*)pv_get_info = AVC_SCART_INPUT_MODE_RGB;
                        break;
                        
                    case MTSCART_CVBS:
                        *(AVC_SCART_INPUT_MODE_T*)pv_get_info = AVC_SCART_INPUT_MODE_CVBS;
                        break;
                        
                    case MTSCART_SV:
                        *(AVC_SCART_INPUT_MODE_T*)pv_get_info = AVC_SCART_INPUT_MODE_SV;
                        break;

                    default:
                        *(AVC_SCART_INPUT_MODE_T*)pv_get_info = AVC_SCART_INPUT_MODE_UNKNOWN;
                        break;
                    }

                    *pz_get_info_len = sizeof(AVC_SCART_INPUT_MODE_T);
                }
                else
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
            }           
            else
            {
                return RMR_DRV_INV_GET_INFO;
            }
            break;
            
        case AVC_GET_TYPE_SCART_INP_CAP:
            if (pt_comp_id->e_type == DRVT_AVC_SCART) //chack source whether SCART is
            {
                if ( (pv_get_info) && (pz_get_info_len) && (*pz_get_info_len >= sizeof(UINT32)) )
                {
                    //Gene Chang For the SCART driver 2006.10.31
                    *(UINT32*)pv_get_info = (AVC_SCART_INP_CAP_RGB | AVC_SCART_INP_CAP_COMPOSITE | AVC_SCART_INP_CAP_S_VIDEO); /* return SCART support input type - Full SCART */
                    *pz_get_info_len = sizeof(UINT32);
                }
                else
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
            }
            else
            {
                return RMR_DRV_INV_GET_INFO;
            }
            break;
        
        case AVC_GET_TYPE_SCART_BYPASS:
            /* Not support... */
            break;
        
        case AVC_GET_TYPE_SCART_OUTPUT:
            /* Not support... */            
            break;
                    
        default:
            return RMR_DRV_INV_GET_INFO;
    }
    
    return RMR_OK;
}
//lint -restore

/*-----------------------------------------------------------------------------
 * Name: _SrcMwSet
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
//lint -e818
static INT32 _SrcMwSet(
    DRV_COMP_ID_T*  pt_comp_id,
    DRV_SET_TYPE_T  e_set_type,
    const VOID*     pv_set_info,
    SIZE_T          z_set_info_len )
{
    UINT8         ui1_avc_comp_list_idx;
    
    /* Check component type and id */
    if (pt_comp_id == NULL)
    {
        return (RMR_DRV_INV_GET_INFO);
    }
    
    ui1_avc_comp_list_idx = _get_avc_comp_list_idx( 
        pt_comp_id->e_type, pt_comp_id->ui2_id);
    if ( ui1_avc_comp_list_idx >= AVC_COMP_LIST_NUM_MAX )
    {
        return (RMR_DRV_INV_GET_INFO);
    }

    /* Perform set opertion */
    switch (e_set_type)
    {
        case AVC_SET_TYPE_NFY_FCT:
            if ( (NULL == pv_set_info) || (z_set_info_len < sizeof(AVC_NFY_INFO_T)) )
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            at_avc_nfy_info[ui1_avc_comp_list_idx].pv_tag = ((AVC_NFY_INFO_T*)pv_set_info)->pv_tag;
        #if 1 //#ifndef LINUX_TURNKEY_SOLUTION
            at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy = ((AVC_NFY_INFO_T*)pv_set_info)->pf_avc_nfy;
        #else
            tv_src_avc_nfy_fct_tbl[ui1_avc_comp_list_idx] = ((AVC_NFY_INFO_T*)pv_set_info)->pf_avc_nfy;
            at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy = tv_src_avc_nfy_fct;
        #endif
            break;

        case AVC_SET_TYPE_NO_CONN_DISC_NFY:
            if ( z_set_info_len < sizeof(BOOL) )
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            ab_avc_comp_no_disc_list[ui1_avc_comp_list_idx] = (BOOL)(UINT32)pv_set_info;
            break;
        
        //Gene Chang For the SCART driver 2006.10.31    
        case AVC_SET_TYPE_SCART_BYPASS:
            if ( (NULL == pv_set_info) || (z_set_info_len < sizeof(AVC_SCART_BYPASS_T)) )
            {
                return RMR_DRV_INV_SET_INFO;
            }
            
            t_scart_info.e_scart_bypass = *(AVC_SCART_BYPASS_T*)pv_set_info;
            
            if (t_scart_info.e_scart_bypass == AVC_SCART_BYPASS_TRUE)
            {
                UINT8 u1SrcType = SV_VS_MAX;
                MTVDECEX_GetVideoSrcType(MTVDO_MAIN, &u1SrcType);
                if ((VSS_MAJOR(u1SrcType) == VSS_ATV) ||
                    (VSS_MAJOR(u1SrcType) == VSS_ATD) ||
                    (VSS_MAJOR(u1SrcType) == VSS_SCART))
                {
                    //UNUSED(bApiVideoSetVideoSrc(SV_VP_MAIN, SV_VS_TUNER1));
                    _MW_VDP_SCART_PIN_8_CB(VDP_1, DRV_CUSTOM_SCART_PIN_8_UNKNOWN);

                }
                else
                {
                    //UNUSED(bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_TUNER1));
                    _MW_VDP_SCART_PIN_8_CB(VDP_2, DRV_CUSTOM_SCART_PIN_8_UNKNOWN);
                }
            }                   
            else
            {
                UINT8 u1SrcType = SV_VS_MAX;
                MTVDECEX_GetVideoSrcType(MTVDO_MAIN, &u1SrcType);
                if ((VSS_MAJOR(u1SrcType) == VSS_ATV) ||
                    (VSS_MAJOR(u1SrcType) == VSS_ATD) ||
                    (VSS_MAJOR(u1SrcType) == VSS_SCART))
                {
                    //UNUSED(bApiVideoSetVideoSrc(SV_VP_MAIN, SV_VS_SCART1));
                    _MW_VDP_SCART_PIN_8_CB(SV_VP_MAIN, (DRV_CUSTOM_SCART_PIN_8_T)MWGetScartPin8Status(VDP_1));
                }
                else
                {
                    //UNUSED(bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_SCART1));
                    _MW_VDP_SCART_PIN_8_CB(SV_VP_PIP, (DRV_CUSTOM_SCART_PIN_8_T)MWGetScartPin8Status(VDP_2));
                }
                //force to mix mode for canel+ 
                MTVDECEX_SCART_SetInputMode(MTSCART_CVBS_RGB);
            }                   
            break;
        
        case AVC_SET_TYPE_SCART_OUTPUT:
            if (z_set_info_len < sizeof(AVC_SCART_OUTPUT_T))
            {
                return RMR_DRV_INV_SET_INFO;
            }
#if 0   // No more used.
		//Only block scart2  out (todo: scart1 need to control GPIO in PCB) 
		if(rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id) == SV_VS_SCART2)
		{
			switch(((AVC_SCART_OUTPUT_T*)pv_set_info)->t_dev_type)
			{
				case DRVT_AVC_COMP_VIDEO:
				case DRVT_TUNER_SAT_ANA:
				case DRVT_TUNER_CAB_ANA:
				case DRVT_TUNER_TER_ANA:
					vApiCvbsOutCtrl(SV_VS_CVBS1);
					break;
				case DRVT_AVC_S_VIDEO:
					vApiCvbsOutCtrl(SV_VS_S1);
					break;
				case DRVT_AVC_SCART:
					vApiCvbsOutCtrl(SV_VS_SCART2);
					break;
				case DRVT_UNKNOWN:
					//vApiCvbsOutCtrl(SV_VS_MAX); //scart2 out off
				default:
					vApiCvbsOutCtrl(SV_VS_MAX);
                                        /* Power Down DAC2 */
//                                        TVE_DACPower(TVE_1, /*DAC_2*/1, SV_FALSE); 
					break;
			}
		}
		else if(rGetVSrcMap(pt_comp_id->e_type, pt_comp_id->ui2_id) == SV_VS_SCART1)
                {
                
		    if(((AVC_SCART_OUTPUT_T*)pv_set_info)->t_dev_type == DRVT_UNKNOWN)
                    {
                        /* power down DAC_1 */
//                        TVE_DACPower(TVE_1, /*DAC_1*/0, SV_FALSE); 
                    }
                }
            #if 0
            if (*(AVC_SCART_OUTPUT_T*)pv_set_info == AVC_SCART_OUTPUT_ON)
            {                   
                /* To be implemented: Please support auto-configuration (composite/S-Video) for SCART2 */
/*                              
                vDrvAvByp();
                vDrvSVdoByp();
*/                
            }
            else
            {
                vDrvBypPD();
            }
            #endif
#endif
            break;          
        
        default:
            return RMR_DRV_INV_SET_INFO;
    }    

    return RMR_OK;
}
//lint -restore

/*-----------------------------------------------------------------------------
 * Name: avc_drv_cust_parse
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
static DRVP_TYPE_T avc_drv_cust_parse(
    const DRV_CUSTOM_T*     pt_drv_cust,
    VOID*                   pv_tag )
{
    INT32       i4_ret;
    
    UNUSED(pv_tag);

    if ( !pt_drv_cust )
    {
        return DRVP_ERROR;
    }
    
    switch( pt_drv_cust->e_custom_type )
    {
        case DRV_CUSTOM_TYPE_COMMON_REG:
            {
                UINT32     ui4_flag;
                DRV_REG_T* pt_drv_reg = (DRV_REG_T*)pt_drv_cust->pv_arg;
                
                /* Do mapping of RM component id to true HARDWARE ID,
                 * pt_drv_reg->t_drv_id <=======> pt_drv_reg->ui4_hw_addr
                 */
                
                if ( (pt_drv_reg->t_drv_id.e_type != DRVT_AVC_AUDIO_INP) &&
                     (pt_drv_reg->t_drv_id.e_type != DRVT_AVC_SPDIF) &&
                     (rGetVSrcMap( 
                        pt_drv_reg->t_drv_id.e_type, 
                        pt_drv_reg->t_drv_id.ui2_id ) == SV_VS_MAX) )
                {
                     DBG_LOG_PRINT(( "[AVC] avc_drv_cust_parse(): invalid driver type=%d, id=%u\n", 
                         _rMChannelCompInfo.u2_e_type, _rMChannelCompInfo.u2_e_ui2_id ));
                    return DRVP_ERROR;
                }
                
                /* Do custom registration, this function will register component
                   and conenction */
                ui4_flag = DRV_FLAG_ASYNC_CONN_OR_DISC;
                if ( (pt_drv_reg->t_drv_id.e_type != DRVT_AVC_AUDIO_INP) &&
                     (pt_drv_reg->t_drv_id.e_type != DRVT_AVC_SPDIF) )
                {
                    ui4_flag |= ( AVC_FLAG_DEC_REQ_FOR_TV_SIG_DETECTION |
                                  AVC_FLAG_DEC_REQ_FOR_COLOR_SYS_DETECTION );
                }
                
                /* Decide which flag a SCART driver should register */
                if (pt_drv_reg->t_drv_id.e_type == DRVT_AVC_SCART)
                { 
                    if ((pt_drv_reg->t_drv_id.ui2_id & AVC_SCART_TYPE_SOURCE_SINK) == AVC_SCART_TYPE_SOURCE_SINK)   
                    {
                        ui4_flag |= AVC_FLAG_SCART_SOURCE | AVC_FLAG_SCART_SINK;
                    }                    
                    else if ((pt_drv_reg->t_drv_id.ui2_id & AVC_SCART_TYPE_SOURCE) == AVC_SCART_TYPE_SOURCE)
                    {
                        ui4_flag |= AVC_FLAG_SCART_SOURCE;
                    }
                    else if ((pt_drv_reg->t_drv_id.ui2_id & AVC_SCART_TYPE_SINK) == AVC_SCART_TYPE_SINK)   
                    {
                        ui4_flag |= AVC_FLAG_SCART_SINK;
                    }
                    else if ((pt_drv_reg->t_drv_id.ui2_id & AVC_SCART_TYPE_MONITOR) == AVC_SCART_TYPE_MONITOR)                       
                    {
                        ui4_flag |= AVC_FLAG_SCART_MONITOR;
                    }            
                    else
                    {
                        return DRVP_ERROR;
                    }                        
                }        
                         
                if ((ui4_flag & (AVC_FLAG_SCART_SOURCE | AVC_FLAG_SCART_SINK | AVC_FLAG_SCART_MONITOR))
                    == AVC_FLAG_SCART_SINK)
                {
                    i4_ret = d_drv_cust_reg_ex( 
                        pt_drv_reg,
                        _SrcMwConnect,
                        _SrcMwDisconnect,
                        _SrcMwGet,
                        _SrcMwSet,
                        ui4_flag,
                        CONN_DIR_INP_TO_OUT);
                }
                else
                {
                    i4_ret = d_drv_cust_reg( 
                        pt_drv_reg,
                        _SrcMwConnect,
                        _SrcMwDisconnect,
                        _SrcMwGet,
                        _SrcMwSet,
                        ui4_flag );
                }
                if ( i4_ret != 0 )
                {
                    return DRVP_ERROR;
                }
                
                if ( !_add_avc_comp_list( pt_drv_reg->t_drv_id.e_type, 
                                          pt_drv_reg->t_drv_id.ui2_id ) )
                {
                    return DRVP_ERROR;
                }
            }
            break;
        
        default:
            /* Ignore unknown types */
            break;
    }
    
    return DRVP_NEXT;
}

/*-----------------------------------------------------------------------------
                    External functions implementations
 ----------------------------------------------------------------------------*/
VOID vDrvNotifyComp_Sig_Chg( 
	UINT8	ucVdpId,	
    UINT32      ui4_drv_status )
{

    COMP_INFO_FOR_CHANNEL_T *rChnPtr;
    DRV_TYPE_T  e_avc_type;
    UINT16      ui2_avc_id;


    rChnPtr = (ucVdpId == VDP_1)?(&_rMChannelCompInfo):(&_rPChannelCompInfo) ;

    e_avc_type =  rChnPtr->t_mon_id.e_type;
    ui2_avc_id =  rChnPtr->t_mon_id.ui2_id;

    //Printf( "[AVC0] vNotifyComp_Sig_Chg(): driver type=%d, id=%u , stable =%d\r\n", e_avc_type,  ui2_avc_id, ui4_drv_status );

	vNotifyComp_Sig_Chg(e_avc_type, ui2_avc_id, ui4_drv_status);
}

/*-----------------------------------------------------------------------------
 * Name: vNotifyComp_Sig_Chg
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
VOID vNotifyComp_Sig_Chg(
    DRV_TYPE_T  e_avc_type,
    UINT16      ui2_avc_id,
    UINT32      ui4_drv_status )
{
    UINT8         ui1_avc_comp_list_idx;
    UINT32        ui4_mw_sig_status;


    //Printf( "[AVC1] vNotifyComp_Sig_Chg(): driver type=%d, id=%u stable=%d \r\n", e_avc_type, ui2_avc_id, ui4_drv_status );
    ui1_avc_comp_list_idx = _get_avc_comp_list_idx( e_avc_type, ui2_avc_id );
    if ( ui1_avc_comp_list_idx >= AVC_COMP_LIST_NUM_MAX )
    {
         DBG_LOG_PRINT(( "[AVC] vNotifyComp_Sig_Chg(): invalid driver type=%d, id=%u\r\n", 
             e_avc_type, ui2_avc_id ));
        return;
    }
    
    if( ui4_drv_status == (UINT32)SV_VDO_STABLE )
    {
        ui4_mw_sig_status = AVC_STATE_TV_SIGNAL;
    }
    else if( ui4_drv_status == (UINT32)SV_VDO_NOSIGNAL )
    {
        ui4_mw_sig_status = AVC_STATE_NONE;
    }
    else
    {
        /* Ignore other states */
         DBG_LOG_PRINT(( "[AVC] vNotifyComp_Sig_Chg(): invalid driver signal statustype=%d\r\n", 
             ui4_drv_status ));
        return;
    }
    
    if ( (e_avc_type == DRVT_AVC_HDMI) &&
         (ui4_drv_status == (UINT32)SV_VDO_STABLE) )
    {
        UINT8 bHDMISupportAudio = FALSE;
        MTVDECEX_HDMI_IsSupportAudio(&bHDMISupportAudio);
        if ( bHDMISupportAudio)
        {
            ui4_mw_sig_status |= AVC_STATE_AUDIO_SIGNAL;
        }
         DBG_LOG_PRINT(( "[AVC] Notify HDMI%d has%s Audio\r\n",
            ui2_avc_id, 
            ((ui4_mw_sig_status & AVC_STATE_AUDIO_SIGNAL)?"":" NO") ));
    }
    
    if( at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy )
    {
    #if 0 //#ifdef LINUX_TURNKEY_SOLUTION
        set_tv_src_avc_nfy(ui1_avc_comp_list_idx);
    #endif
        at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy(
            at_avc_nfy_info[ui1_avc_comp_list_idx].pv_tag, 
            AVC_COND_SIGNAL_STATUS, 
            ui4_mw_sig_status, 
            0 );
    }
    
    /* Notify COMBI, if required. */
    if ( at_avc_combi_mw_nfy_info[ui1_avc_comp_list_idx].pf_combi_nfy )
    {
        at_avc_combi_mw_nfy_info[ui1_avc_comp_list_idx].pf_combi_nfy(
            at_avc_comp_list[ui1_avc_comp_list_idx].e_type,
            at_avc_comp_list[ui1_avc_comp_list_idx].ui2_id,
            at_avc_combi_mw_nfy_info[ui1_avc_comp_list_idx].pv_tag,
            ui4_mw_sig_status,
            0 );
    }
}


/*-----------------------------------------------------------------------------
 * Name: vNotify_Sig_Chg
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
VOID vNotify_Sig_Chg(UINT8 u1Path)
{
    DRV_TYPE_T    e_avc_type;
    UINT16        ui2_avc_id;
    UINT32        ui4_drv_status;
    MTVDECEX_SIG_STATUS       eSigStatus = MTVDECEX_VDO_UNKNOWN;

    if(u1Path == SV_VP_MAIN)
    {
        e_avc_type = _rMChannelCompInfo.u2_e_type;
        ui2_avc_id = _rMChannelCompInfo.u2_e_ui2_id;
        MTVDECEX_GetVideoStatus(MTVDO_MAIN, &eSigStatus);
    }
    else if(u1Path == SV_VP_PIP)
    {
        e_avc_type = _rPChannelCompInfo.u2_e_type;
        ui2_avc_id = _rPChannelCompInfo.u2_e_ui2_id;
        MTVDECEX_GetVideoStatus(MTVDO_SUB, &eSigStatus);
    }
    else
    {
         DBG_LOG_PRINT(( "[AVC] vNotify_Sig_Chg(): invalid u1Path=%u\n", 
             u1Path ));
        return;
    }
    if(eSigStatus==MTVDECEX_VDO_NOSIGNAL)
        ui4_drv_status = SV_VDO_NOSIGNAL;
    else if(eSigStatus==MTVDECEX_VDO_NOSUPPORT)
        ui4_drv_status = SV_VDO_NOSUPPORT;
    else if(eSigStatus==MTVDECEX_VDO_STABLE)
        ui4_drv_status = SV_VDO_STABLE;
    else
        ui4_drv_status = SV_VDO_UNKNOWN;

    vNotifyComp_Sig_Chg( e_avc_type, ui2_avc_id, ui4_drv_status );
}


//Gene Chang For the SCART driver 2006.10.31    
/*-----------------------------------------------------------------------------
 * Name: vNotifyScart_Sig_Chg
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
void vMLEvtBridge_MsgSend(UINT8 cmd, UINT32 d1, UINT32 d2, UINT32 d3);
#include "source_table.h"
VOID vNotifyScart_Sig_Chg(UINT8 u1Source, UINT8 uStatus)
{
    UINT8           ui1_status;
    DRV_TYPE_T      e_type;
    UINT16          ui2_comp_id;
    UINT8           ui1_avc_comp_list_idx;
    UINT8 u1MainSrc=0, u1SubSrc=0;
    UINT8 u1IsMainScart=0, u1IsPipScart=0;
	TVD_DBG_MSG("vNotifyScart_Sig_Chg-- u1Source==%d, uStatus == %d",u1Source,uStatus);
	
	u1Source=u1Source+SV_VS_SCART1;
    UNUSED(rGetVSrcCompFromIdx((UINT32)u1Source, AVC_SCART_TYPE_MONITOR, &e_type, &ui2_comp_id));
    ui1_avc_comp_list_idx = _get_avc_comp_list_idx(e_type, ui2_comp_id);
    if ( ui1_avc_comp_list_idx >= AVC_COMP_LIST_NUM_MAX )
    {
        return;
    }
    
    switch(uStatus)
    {
        case MTSCART_PIN_8_ASPECT_4_3 :
            ui1_status = (UINT8)AVC_SCART_PIN_8_ASPECT_4_3;
            break;
            
        case MTSCART_PIN_8_ASPECT_16_9 :
            ui1_status = (UINT8)AVC_SCART_PIN_8_ASPECT_16_9;
            break;
            
        case MTSCART_PIN_8_NO_SIGNAL :
            ui1_status = (UINT8)AVC_SCART_PIN_8_NO_SIGNAL;
            break;

        default :
            ui1_status = (UINT8)AVC_SCART_PIN_8_UNKNOWN;
	}
    
    if( at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy )
    {
    #if 0 //#ifdef LINUX_TURNKEY_SOLUTION
        set_tv_src_avc_nfy(ui1_avc_comp_list_idx);
    #endif
        at_avc_nfy_info[ui1_avc_comp_list_idx].pf_avc_nfy(
            at_avc_nfy_info[ui1_avc_comp_list_idx].pv_tag, 
            AVC_COND_SCART_PIN_8_STATUS, 
            ui1_status, 
            0 );
    }

    MTVDECEX_GetVideoSrcType(MTVDO_MAIN,  &u1MainSrc);
    MTVDECEX_GetVideoSrcType(MTVDO_SUB, &u1SubSrc);
    MTVDECEX_GetVideoDefStatus(MTVDO_IS_MAIN_SCART, &u1IsMainScart);
    MTVDECEX_GetVideoDefStatus(MTVDO_IS_MAIN_SCART, &u1IsPipScart);
    if (u1MainSrc == u1Source && u1IsMainScart)
        vMLEvtBridge_MsgSend(2,0,ui1_status,0);
    if (u1SubSrc == u1Source && u1IsPipScart)
        vMLEvtBridge_MsgSend(2,1,ui1_status,0);
}


/*-----------------------------------------------------------------------------
 * Name: MWGetScartPin8Status
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
 
//UINT8 bDrvGetScartFSStatus(UINT8 bSrcIdx);

extern UINT8 _bSCART1Status;
extern UINT8 _bSCART2Status;

extern UINT8 _bScartPin8ASR_save[2];

UINT8 MWGetScartPin8Status(UINT8 u1Path)
{
    UINT8 src = 0;
    MTSCART_PIN_8_T state = MTSCART_PIN_8_UNKNOWN;

    if(!u1Path)
    {
        MTVDECEX_GetVideoSrcType(MTVDO_MAIN, &src);
    }
    else
    {
        MTVDECEX_GetVideoSrcType(MTVDO_SUB, &src);
    }



    if(src==SV_VS_SCART1)
        MTVDECEX_SCART_GetPin8Status(MTSCART_1, &state);
    else if (src==SV_VS_SCART2)
        MTVDECEX_SCART_GetPin8Status(MTSCART_2, &state);
    else	
        return AVC_SCART_PIN_8_UNKNOWN;

    switch(state)
    {
        case MTSCART_PIN_8_ASPECT_4_3:
            return AVC_SCART_PIN_8_ASPECT_4_3;
        case MTSCART_PIN_8_ASPECT_16_9 :
            return AVC_SCART_PIN_8_ASPECT_16_9;           
        case MTSCART_PIN_8_NO_SIGNAL:
            return AVC_SCART_PIN_8_NO_SIGNAL;
        default :
            return AVC_SCART_PIN_8_UNKNOWN;
    }

}


/*-----------------------------------------------------------------------------
 * Name: MW_SrcInit
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
//lint -e818
INT32 MW_SrcInit( DRV_CUSTOM_DIR_T* pt_drv_cust_dir )
{
    INT32  i4_ret, i4_idx;

    for(i4_idx = 0; i4_idx < (INT32)AVC_COMP_LIST_NUM_MAX; i4_idx++)
    {
        at_avc_nfy_info[i4_idx].pf_avc_nfy = NULL;
        at_avc_nfy_info[i4_idx].pv_tag = NULL;
        apf_rm_conn_tag[i4_idx] = NULL;
        at_avc_comp_list[i4_idx].e_type = DRVT_UNKNOWN;
        at_avc_comp_list[i4_idx].ui2_id = 0;
        at_avc_combi_mw_nfy_info[i4_idx].pf_combi_nfy = NULL;
        at_avc_combi_mw_nfy_info[i4_idx].pv_tag = NULL;
    }
    
    if ( !pt_drv_cust_dir )
    {
        /* There is no AVC components */
        return 0;
    }

    i4_ret = d_drv_cust_parse( 
        pt_drv_cust_dir->pt_custom,
        avc_drv_cust_parse,
        NULL );
    if ( i4_ret != 0 )
    {
        return -1;
    }
    
    return 0;
}
//lint -restore


/*-----------------------------------------------------------------------------
 * Name: MW_IsAVCExist
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
BOOL MW_IsAVCExist( 
   DRV_TYPE_T     e_type,
   UINT16         ui2_id )
{
    if ( _get_avc_comp_list_idx( e_type, ui2_id ) != 0xFF )
    {
        return TRUE;
    }

    return FALSE;
}


//mtal porting
MTVDECEX_SRC_T VdoSrcMap2Mtal(UINT8 u1Src)
{
    switch(u1Src)
    {
        case    SV_VS_ATD1:
				return MTVDECEX_SRC_ATD1;
        case    SV_VS_ATV1:
                return MTVDECEX_SRC_ATV1;                
        //case    SV_VS_DT1:
        case    SV_VS_DTV1:
                //return MTVDECEX_SRC_DT1;
                return MTVDECEX_SRC_DTV1;
        //case    SV_VS_DT2:
        case    SV_VS_DTV2:
                //return MTVDECEX_SRC_DT2;
                return MTVDECEX_SRC_DTV2;
        case    SV_VS_CVBS1:
                return MTVDECEX_SRC_CVBS1;
        case    SV_VS_CVBS2:
                return MTVDECEX_SRC_CVBS2;
        case    SV_VS_CVBS3:
                return MTVDECEX_SRC_CVBS3;
        case    SV_VS_CVBS4:
                return MTVDECEX_SRC_CVBS4;
        case    SV_VS_S1:
                return MTVDECEX_SRC_SV1;
        case    SV_VS_S2:
                return MTVDECEX_SRC_SV2;
        case    SV_VS_S3:
                return MTVDECEX_SRC_SV3;
        case    SV_VS_YPbPr1:
                return MTVDECEX_SRC_COMP1;
        case    SV_VS_YPbPr2:
                return MTVDECEX_SRC_COMP2;
        case    SV_VS_YPbPr3:
                return MTVDECEX_SRC_COMP3;
        case    SV_VS_YPbPr4:
                return MTVDECEX_SRC_COMP4;
        case    SV_VS_VGA1:
                return MTVDECEX_SRC_PC1;
        case    SV_VS_HDMI1:
                return MTVDECEX_SRC_HDMI1;
        case    SV_VS_HDMI2:
                return MTVDECEX_SRC_HDMI2;
        case    SV_VS_HDMI3:
                return MTVDECEX_SRC_HDMI3;
        case    SV_VS_HDMI4:
                return MTVDECEX_SRC_HDMI4;
        case    SV_VS_HDMI5:
                return MTVDECEX_SRC_HDMI5;
        case    SV_VS_SCART1:
                return MTVDECEX_SRC_SCART1;
        case    SV_VS_SCART2:
                return MTVDECEX_SRC_SCART2;
        case    SV_VS_SCART3:
                return MTVDECEX_SRC_SCART3;
        case    SV_VS_SCART4:
                return MTVDECEX_SRC_SCART4;	
        default:
            return MTVDECEX_SRC_MAX;
    };
};

/*-----------------------------------------------------------------------------
 * Name: rGetVSrcMap
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
UINT32 rGetVSrcMap( 
    DRV_TYPE_T e_type, 
    UINT16     ui2_comp_id )
{
    UINT8 ui1_idx;
    
    for( ui1_idx = 0; 
         ui1_idx < sizeof(at_vid_src_mapping_tbl)/sizeof(NPTV_VID_SRC_MAPPING_TBL_T)  &&
         !IS_NPTV_VID_SRC_MAPPING_TBL_END(&at_vid_src_mapping_tbl[ui1_idx]); 
         ui1_idx++)
    {
        if( (at_vid_src_mapping_tbl[ui1_idx].e_type == e_type) && 
            (at_vid_src_mapping_tbl[ui1_idx].ui2_comp_id == ui2_comp_id) )
        {
            return (at_vid_src_mapping_tbl[ui1_idx].e_vid_src_idx);
        }
    }
    
    return (UINT32)SV_VS_MAX;
}


/*-----------------------------------------------------------------------------
 * Name: rGetVSrcCompFromIdx
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
INT32 rGetVSrcCompFromIdx(
    UINT32      e_vid_src_idx,
    UINT16      ui2_flag,
    DRV_TYPE_T* pe_type, 
    UINT16*     pui2_comp_id )
{
    UINT8 ui1_idx;
    
    if ( !pe_type || !pui2_comp_id )
    {
        return -1;
    }
    
    for( ui1_idx = 0; 
         ui1_idx < sizeof(at_vid_src_mapping_tbl)/sizeof(NPTV_VID_SRC_MAPPING_TBL_T)  &&
         !IS_NPTV_VID_SRC_MAPPING_TBL_END(&at_vid_src_mapping_tbl[ui1_idx]); 
         ui1_idx++)
    {
        if( (at_vid_src_mapping_tbl[ui1_idx].e_vid_src_idx == e_vid_src_idx) &&
            ((at_vid_src_mapping_tbl[ui1_idx].ui2_comp_id & ui2_flag) == ui2_flag) )
        {
            *pe_type = at_vid_src_mapping_tbl[ui1_idx].e_type;
            *pui2_comp_id = at_vid_src_mapping_tbl[ui1_idx].ui2_comp_id;
            return 0;
        }
    }
    
    *pe_type = (DRV_TYPE_T)SV_VS_MAX;
    *pui2_comp_id = 0;
    
    return -1;
}


/*-----------------------------------------------------------------------------
 * Name: rGetVScartInfo
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
INT32 rGetVScartInfo(AVC_SCART_INFO_T* pt_scart_info) 
{
    if (NULL == pt_scart_info)
    {
        return 0;
    }

    *pt_scart_info = t_scart_info;
    
    return 0;
}


/*-----------------------------------------------------------------------------
 * Name: MW_AVCSetNotifyOfCOMBI
 *
 * Description: 
 *
 * Inputs: 
 *
 * Outputs: 
 *
 * Returns: 
 ----------------------------------------------------------------------------*/
INT32 MW_AVCSetNotifyOfCOMBI(
   DRV_TYPE_T         e_type,
   UINT16             ui2_id,
   avc_combi_nfy_fct  pf_combi_nfy,
   VOID*              pv_tag )
{
    UINT8     ui1_idx = _get_avc_comp_list_idx( e_type, ui2_id);
    
    if ( ui1_idx >= AVC_COMP_LIST_NUM_MAX )
    {
        return -1;
    }
    
    at_avc_combi_mw_nfy_info[ui1_idx].pf_combi_nfy = pf_combi_nfy;
    at_avc_combi_mw_nfy_info[ui1_idx].pv_tag = pv_tag;
    
    return 0;
}

