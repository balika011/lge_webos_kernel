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
                    include files
-----------------------------------------------------------------------------*/
#ifndef __KERNEL__
        
#ifdef _WIN32_
#include "x_os.h"
#else
#include "x_os.h"
#endif

//#include "handle/x_handle.h"
//#include "handle/u_handle.h"
//#include "dbg/x_dbg.h"
//#include "dbg/u_dbg.h"

#include "u_sbtl_ipcc.h"
#include "x_sbtl_ipcc.h"
#include "_sbtl_ipcc_wgt.h"
#include "_sbtl_ipcc_process.h"
#include "_sbtl_ipcc_parser.h"
#include "_sbtl_ipcc_common.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"

#include "u_sm_cc_hdlr.h"
#include "x_handle.h"
#include "u_handle.h"
#include "x_dbg.h"
#include "u_dbg.h"


/*-----------------------------------------------------------------------------
                    Extern varible
-----------------------------------------------------------------------------*/
/*---------------------- Common global varible ------------------------------*/
extern UINT32                       gui4_screen_width;
extern UINT32                       gui4_screen_height;
extern UINT8                        gui1_width_percent;
extern UINT8                        gui1_height_percent;
extern FLOAT                        gf_xml_display_ratio;
extern FLOAT                        gf_after_cnvt_display_ratio;
extern BOOL                         gb_is_extent_exist;
extern BOOL                         gb_apply_convert_rule;
extern BOOL                         gb_is_global_style_set;
extern SM_CC_ASPECT_RATIO_T         gt_aspect_ratio;
extern SBTL_IPCC_VIDEO_T            gt_video_info;
//extern SBTL_IPCC_WGT_T*             pt_sbtl_ipcc_wgt;
extern SBTL_TTML_ATTR_ID_LIST_T*    gpt_sbtl_ttml_record_global_style;
extern BOOL                         gb_is_crnt_wnd_defined[IPCC_MAX_PEN_STL_NUM];
extern UINT8                        gaui1_crnt_wnd[IPCC_MAX_WGT_NUM];
extern SBTL_IPCC_REGION_ATTR        gt_region_attr[IPCC_MAX_REGION_NUM];
extern SBTL_IPCC_WGT_WND_ATTR       gat_wgt_wnd_attr[IPCC_MAX_REGION_NUM];
extern SBTL_IPCC_WGT_WND_ATTR       gt_wgt_wnd_attr;
extern SBTL_IPCC_CAPTION_MODE       ge_crnt_caption_mode;
extern SBTL_IPCC_CAPTION_MODE       ge_last_caption_mode;
extern SBTL_IPCC_WGT_T*             pt_sbtl_ipcc_wgt;
extern BOOL                         gb_ipcc_visible;

/*---------------------- Global varible for roll up -------------------------*/
extern BOOL gb_roll_up_1st_set;
extern BOOL gb_roll_up_ignore_crnt_p;
extern BOOL gb_is_enhance_rollup;
extern CHAR gps_roll_up_base_row[IPCC_TTS_ORIGIN_LENGTH];
extern CHAR gps_roll_up_last_id[IPCC_TTS_ORIGIN_LENGTH];


/*-----------------------------------------------------------------------------
                    function define
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_open_fct
 *
 * Description: This API is used to open ipcc
 *
 * Inputs:  h_canvas              - The handle of canvas
 *          pv_cmd_sets           - The 1st <p> info
 *
 * Outputs: 
 *
 * Returns: IPCCR_OK              - 
 *          IPCCR_WGL_FAIL        - 
 *          IPCCR_OUT_OF_HANDLE   - 
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_open_fct
(
    IN  HANDLE_T h_canvas, 
    IN  VOID* pv_cmd_sets
)
{
    if ( !x_handle_valid( h_canvas ) )
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Invalid h_canvas!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_OUT_OF_HANDLE;
    }

    sbtl_ipcc_ttml_init_global_varible();
    
    INT32                   i4_ret          = IPCCR_OK;
    SBTL_XML_COMMAND_T      *pt_cmd_sets    = (SBTL_XML_COMMAND_T*)pv_cmd_sets;
    SBTL_IPCC_OPEN_CFG_T    *pt_open_cfg    = (SBTL_IPCC_OPEN_CFG_T*)x_mem_alloc(sizeof(SBTL_IPCC_OPEN_CFG_T));
    IPCC_CHECK_NULL(pt_open_cfg, IPCCR_INSUFFICIENT_MEMORY);
    x_memset(pt_open_cfg, 0, sizeof(SBTL_IPCC_OPEN_CFG_T));

    DBG_INFO((IPCC_PREFIX" <FPI>:   "));
    i4_ret = sbtl_ipcc_ttml_record_fpi(pt_cmd_sets, pt_open_cfg);
    IPCC_CHECK_VALID(IPCCR_OK, i4_ret);    
    DBG_INFO(("<%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));

    /* 90% is the default ratio */
    FLOAT f_display_ratio   = gf_xml_display_ratio;
    UINT8 ui1_row_count     = pt_open_cfg->ui1_row_count;
    
    i4_ret = sbtl_ipcc_ttml_calculate_display_ratio((const CHAR *)pt_open_cfg->ps_extent, &ui1_row_count, &(gt_video_info.e_file_type), &f_display_ratio);
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(pt_open_cfg);
        return i4_ret;
    }    

    i4_ret = sbtl_ipcc_ttml_init();
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(pt_open_cfg);
        return IPCCR_WGL_FAIL;
    }
    
    i4_ret = sbtl_ipcc_ttml_recal_aspect_ratio(pt_open_cfg);
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(pt_open_cfg);
        return i4_ret;
    }
    
    i4_ret = sbtl_ipcc_wgt_create(h_canvas, pt_open_cfg->ps_aspect_ratio);
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(pt_open_cfg);
        return IPCCR_WGL_FAIL;
    }

    IPCC_MEM_FREE(pt_open_cfg);
    return IPCCR_OK;

}



/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_close_fct
 *
 * Description: This API is used to close ipcc
 *
 * Inputs:  
 *
 * Outputs: 
 *
 * Returns: IPCCR_OK              - 
 *          IPCCR_WGL_FAIL        - 
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_close_fct()
{
    INT32 i4_ret = IPCCR_OK;
    
    i4_ret = sbtl_ipcc_wgt_destory(pt_sbtl_ipcc_wgt);
    IPCC_CHECK_VALID(IPCCR_OK,i4_ret);

    sbtl_ipcc_ttml_init_global_varible();

    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_set_fct
 *
 * Description: This API is used to set attribute of ipcc
 *
 * Inputs:  e_set_type              - The handle of canvas
 *          pv_set_info             - Set info
 *          z_set_info_len          - The length of info
 * Outputs: 
 *
 * Returns: IPCCR_OK               
 *          IPCCR_WGL_FAIL         
 *          IPCCR_INV_ARG  
 *          IPCCR_INSUFFICIENT_SIZE
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_set_fct
(
    IN  SBTL_IPCC_SET_TYPE e_set_type, 
    IN  VOID* pv_set_info,
    IN  SIZE_T z_set_info_len
)
{
    IPCC_CHECK_NULL(pv_set_info,IPCCR_INV_ARG);   
    
    INT32    i4_ret = IPCCR_OK;
    switch( e_set_type )
    {
        case SBTL_IPCC_SET_TYPE_GLOBAL_VISIBLE:
            /*set global cc visbile here, when the cc engine create widget, it will reference to the
                    variable*/                   
            gb_ipcc_visible = (BOOL)((UINT32)pv_set_info);    
            i4_ret = sbtl_ipcc_wgt_chg_cc_visible(pt_sbtl_ipcc_wgt, gb_ipcc_visible);
            IPCC_CHECK_VALID(IPCCR_OK,i4_ret);           
            break;

        case SBTL_IPCC_SET_TYPE_ATTRIBS:
            sbtl_ipcc_wgt_set_wnd_attr(pt_sbtl_ipcc_wgt, (const SM_CCH_DTVCC_ATTRIBS_INFO_T*)pv_set_info); 
            break;

        case SBTL_IPCC_SET_TYPE_DATA:
            {
                SBTL_TTML_DATA_T* pt_sbtl_ttml_data = (SBTL_TTML_DATA_T*)pv_set_info;
                if(z_set_info_len != sizeof(SBTL_TTML_DATA_T))
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: z_set_info_len is invalid!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
                    return IPCCR_INSUFFICIENT_SIZE;
                }
                
                i4_ret = sbtl_ipcc_ttml_data(pt_sbtl_ttml_data);
                IPCC_CHECK_VALID(IPCCR_OK,i4_ret);

            }
            break;

       case SBTL_IPCC_SET_TYPE_GLOBAL_STYLE:
            {
                SBTL_TTML_ATTR_ID_LIST_T* pt_sbtl_global_style = (SBTL_TTML_ATTR_ID_LIST_T*)pv_set_info;
                i4_ret = sbtl_ipcc_parse_record_global_style(pt_sbtl_global_style);
                IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
            }
            break;

        case SBTL_IPCC_SET_TYPE_CRNT_SAMPLE_END:
            {                      
                if(TRUE == gb_is_enhance_rollup)
                {
                    i4_ret = sbtl_ipcc_ttml_remove(IPCC_CAPTION_MODE_ROLL);
                    IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
                }
                     
                i4_ret = sbtl_ipcc_ttml_init();
                IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
            }
            break;

       default:
            break;
            
    }
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_get_fct
 *
 * Description: This API is used to get attributes of ipcc
 *
 * Inputs:  e_get_type              - The handle of canvas
 *          z_get_info_len          - The length of info
 * Outputs: pv_get_info             - Get info
 *
 * Returns: IPCCR_OK               
 *          IPCCR_WGL_FAIL         
 *          IPCCR_INV_ARG  
 *          IPCCR_INSUFFICIENT_SIZE
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_get_fct
(
    IN  SBTL_IPCC_GET_TYPE e_get_type, 
    IN  VOID* pv_get_info,
    IN  SIZE_T z_get_info_len
)
{
    IPCC_CHECK_NULL(pv_get_info,IPCCR_INV_ARG);    

    INT32 i4_ret = IPCCR_OK;

    switch(e_get_type)
    {
        case SBTL_IPCC_GET_TYPE_ENHANCE_ROLLUP_MODE:
        {
            i4_ret = sbtl_ipcc_ttml_get_enhance_roll_up_mode((BOOL *) pv_get_info);
            if(IPCCR_OK != i4_ret)
            {
                return i4_ret;
            }
            break;
        }
            

        default:
            break;
    }
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_remove
 *
 * Description: This API is used to remove window of ipcc
 *
 * Inputs:  ps_crnt_wnd              - The id of <region>
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_remove(IN CHAR* ps_crnt_wnd)
{
    IPCC_CHECK_NULL(ps_crnt_wnd,IPCCR_INV_ARG);  
    
    INT32 i4_ret = IPCCR_OK;
    if(TRUE == gb_is_enhance_rollup)
    {
        return IPCCR_OK;
    }

    i4_ret = sbtl_ipcc_ttml_remove(ps_crnt_wnd);
    IPCC_CHECK_VALID(IPCCR_OK,i4_ret);    
    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: x_sbtl_ipcc_init
 *
 * Description: This API is used to init IPCC
 *
 * Inputs:  pt_ipcc_config      - The config of IPCC
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 x_sbtl_ipcc_init(IN   SBTL_IPCC_CONFIG_T* pt_ipcc_config)
{
    IPCC_CHECK_NULL(pt_ipcc_config,IPCCR_INV_ARG);   
    
    gui4_screen_width = pt_ipcc_config->ui4_screen_width;
    gui4_screen_height = pt_ipcc_config->ui4_screen_height;

    DBG_INFO((IPCC_PREFIX" INFO: gui4_screen_width is %u, gui4_screen_height is %u!  <%d, %s, %s>\n\r", 
            gui4_screen_width, gui4_screen_height, __LINE__, __func__, __FILE__));
    
    return IPCCR_OK;
}


#endif //__KERNEL__
