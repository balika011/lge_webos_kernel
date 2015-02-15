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

#include "x_sbtl_ipcc_util.h"
#include "_sbtl_ipcc_process.h"
#include "_sbtl_ipcc_parser.h"
#include "_sbtl_ipcc_wgt.h"



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
extern BOOL                         gb_ipcc_visible;
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


/*---------------------- Global varible for roll up -------------------------*/
extern BOOL gb_roll_up_1st_set;
extern BOOL gb_roll_up_ignore_crnt_p;
extern BOOL gb_is_enhance_rollup;
extern CHAR gps_roll_up_base_row[IPCC_TTS_ORIGIN_LENGTH];
extern CHAR gps_roll_up_last_id[IPCC_TTS_ORIGIN_LENGTH];



/*-----------------------------------------------------------------------------
            function declaration
-----------------------------------------------------------------------------*/
INT32 _sbtl_ipcc_abstract_region_id(
                IN  CHAR* ps_crnt_region_id, 
                OUT UINT32* pui4_crnt_wnd);

VOID _sbtl_ipcc_init_global_region_attr();
VOID _sbtl_ipcc_init_global_varible();
VOID _sbtl_ipcc_init_global_wnd_attr();


#ifndef _____SPLIT_LINE________________________________________________
#endif




/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_abstract_region_id
 *
 * Description: This API is used to abstract region id
 *
 * Inputs:  ps_crnt_region_id              - The id of <region>
 * Outputs: pui4_crnt_wnd                  - The id which abstracted
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 _sbtl_ipcc_abstract_region_id(
                IN  CHAR* ps_crnt_region_id, 
                OUT UINT32* pui4_crnt_wnd)
{
    /* Check arguments */
    IPCC_CHECK_NULL(ps_crnt_region_id, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui4_crnt_wnd, IPCCR_INV_ARG);
 
    UINT8 ui1_index = 0;

    for(ui1_index = 0; ui1_index < IPCC_MAX_REGION_NUM; ++ui1_index)
    {
        if( 0 == x_strcmp((const CHAR*)ps_crnt_region_id, (const CHAR*)gt_region_attr[ui1_index].ps_region_id))
        {
            *pui4_crnt_wnd = ui1_index;
            break;
        }
    }
        
    CHAR* ps_match[3] = {"pop", "paint", "rollup"};
    SBTL_IPCC_CAPTION_MODE e_caption_mode = SBTL_IPCC_CAPTION_MODE_POP_ON;
    
    for(ui1_index = 0; ui1_index < 3; ++ui1_index)
    {
        if(NULL != x_strstr((const CHAR*)ps_crnt_region_id, (const CHAR*)ps_match[ui1_index]))
        {
            e_caption_mode = (SBTL_IPCC_CAPTION_MODE)ui1_index;
            break;
        }
    }

    if(SBTL_IPCC_CAPTION_MODE_ROLL_UP != ge_crnt_caption_mode &&
       SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_last_caption_mode)
    {

    }
    else 
    {
        ge_crnt_caption_mode = e_caption_mode;
    }
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_init_global_region_attr
 *
 * Description: This API is used to init global region attributes.
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns:  
 *
 ----------------------------------------------------------------------------*/
VOID _sbtl_ipcc_init_global_region_attr()
{
    UINT8 ui1_i = 0;
    for(ui1_i = 0; ui1_i < IPCC_MAX_REGION_NUM; ++ui1_i)
    {    
        x_strcpy(gt_region_attr[ui1_i].ps_region_id, " ");
        x_strcpy(gt_region_attr[ui1_i].ps_style_id, " ");
        x_strcpy(gt_region_attr[ui1_i].ps_win_bg_color, "black");
        x_strcpy(gt_region_attr[ui1_i].ps_win_opacity, "0.0");
        x_strcpy(gt_region_attr[ui1_i].ps_pen_fg_color, "white");
        x_strcpy(gt_region_attr[ui1_i].ps_pen_bg_color, "black");
        x_strcpy(gt_region_attr[ui1_i].ps_pen_edge_color, "transparent");
        x_strcpy(gt_region_attr[ui1_i].ps_pen_edge_type, " ");
        x_strcpy(gt_region_attr[ui1_i].ps_font_name, SM_CC_FONT_STYLE_0);
        x_strcpy(gt_region_attr[ui1_i].ps_win_word_wrap, "noWrap");
        x_strcpy(gt_region_attr[ui1_i].ps_text_tag, "dialog");        

        
        gt_region_attr[ui1_i].ui4_font_size                 = 0;
        gt_region_attr[ui1_i].e_font_size_unit              = TTML_LENGTH_UNIT_TYPE_UNKNOW;
        gt_region_attr[ui1_i].e_font_size                   = FE_FNT_SIZE_SMALL;
        
        gt_region_attr[ui1_i].ui1_pen_edge_type             = 0;
        
        gt_region_attr[ui1_i].e_font_family                 = SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT;
        gt_region_attr[ui1_i].e_font_style                  = SBTL_TTML_ATTR_FONT_STYLE_NORMAL;
        gt_region_attr[ui1_i].e_font_weight                 = SBTL_TTML_ATTR_FONT_WEGHT_NORMAL;
        
        gt_region_attr[ui1_i].e_text_decoration             = SBTL_TTML_ATTR_TEXT_DECORATION_NONE;
        gt_region_attr[ui1_i].e_text_align                  = SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT;
        
        gt_region_attr[ui1_i].t_win_anchor.ui1_anchor       = 0;
        gt_region_attr[ui1_i].t_win_anchor.ui2_anchor_h     = gui4_screen_width / 10;
        gt_region_attr[ui1_i].t_win_anchor.ui2_anchor_v     = gui4_screen_height / 10;
        
        gt_region_attr[ui1_i].t_win_size.ui2_col_count      = 32;
        gt_region_attr[ui1_i].t_win_size.ui2_row_count      = 1;
        
        gt_region_attr[ui1_i].e_win_size_unit               = TTML_LENGTH_UNIT_TYPE_UNKNOW;
        
        gt_region_attr[ui1_i].t_pen_loc.ui2_col             = 0;
        gt_region_attr[ui1_i].t_pen_loc.ui2_idx_parag       = 0;
        
        gt_region_attr[ui1_i].b_whitespace_preserve         = FALSE;

    }
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_init_global_varible
 *
 * Description: This API is used to init global varible
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns:  
 *
 ----------------------------------------------------------------------------*/
VOID _sbtl_ipcc_init_global_varible()
{

}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_init_global_wnd_attr
 *
 * Description: This API is used to init global varible
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns:  
 *
 ----------------------------------------------------------------------------*/
VOID _sbtl_ipcc_init_global_wnd_attr()
{
    UINT8 ui1_i = 0;
    for(ui1_i = 0; ui1_i < IPCC_MAX_REGION_NUM; ++ui1_i)
    {
        gat_wgt_wnd_attr[ui1_i]         = gt_wgt_wnd_attr;
        gb_is_crnt_wnd_defined[ui1_i]   = FALSE;
    }
}


#ifndef _____SPLIT_LINE________________________________________________
#endif


/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_init
 *
 * Description: This API is used to init
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_ttml_init()
{
    _sbtl_ipcc_init_global_region_attr();
    _sbtl_ipcc_init_global_wnd_attr();
    
    ge_crnt_caption_mode        = SBTL_IPCC_CAPTION_MODE_POP_ON;
    gb_roll_up_1st_set          = FALSE;
    gb_roll_up_ignore_crnt_p    = FALSE;
    
    x_memset(gps_roll_up_base_row, 0, (SIZE_T)IPCC_TTS_ORIGIN_LENGTH);
    x_strcpy(gps_roll_up_base_row, "0% 0%");
    x_memset(gps_roll_up_last_id, 0, (SIZE_T)IPCC_TTS_ORIGIN_LENGTH);
    x_strcpy(gps_roll_up_last_id, "rollup");

    gb_is_global_style_set              = FALSE;
    gpt_sbtl_ttml_record_global_style   = NULL;
    gb_is_enhance_rollup                = FALSE;
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_data
 *
 * Description: This API is the entrance of parse data
 *
 * Inputs:  pt_sbtl_ttml_data              - The data need to parse
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
 INT32 sbtl_ipcc_ttml_data(
            IN  SBTL_TTML_DATA_T* pt_sbtl_ttml_data)
 {
    /* Check arguments */
    IPCC_CHECK_NULL(pt_sbtl_ttml_data,IPCCR_INV_ARG);

    INT32 i4_ret = IPCCR_OK;

    /* If current file mode is enhance mode and region id is "rollup",
     * it will remove current window before.   
    */    
    if(TRUE == gb_is_enhance_rollup)
    {
        i4_ret = sbtl_ipcc_ttml_remove(IPCC_CAPTION_MODE_ROLL);
        IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
    } 

    
#if 0
    if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
    {
        UINT32 ui4_crnt_wnd = 0;

        if(!SLIST_IS_EMPTY(&pt_ttml_data->t_element_p_list))
        {
            SBTL_TTML_ELEMENT_P_T* pt_elem_p = SLIST_FIRST(&pt_ttml_data->t_element_p_list);

            if(NULL != &pt_elem_p->t_common_attr_list)
            {

            }
        }
        while(pt_elem_p_attr)
        {                
            switch(pt_elem_p_attr->e_attr)
            {
                case SBTL_XML_IPCC_ATTR_REGION:
                    if(0 != x_strcmp((const CHAR*)pt_elem_p_attr->u.ps_value, (const CHAR*)gps_roll_up_last_id))
                    {
                        i4_ret = sbtl_ipcc_ttml_remove(0xFF);
                        IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
                    }
                    else
                    {
                       
                        i4_ret = sbtl_ipcc_ttml_remove(pt_elem_p_attr->u.ps_value);
                        IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
                    }
                    DBG_INFO((IPCC_PREFIX" INFO: pt_elem_p_attr->u.ps_value=%s!   <%d, %s, %s>\n\r", 
                                pt_elem_p_attr->u.ps_value, __LINE__, __func__, __FILE__));
                    break;
                    
                default:
                    break;
            }
            pt_elem_p_attr = SLIST_NEXT(pt_elem_p_attr, t_attr_link);
        }
    }
#endif 

    /* Convert SBTL_TTML_DATA_T to TTML_DATA_T*/
    WGT_TTML_DATA_CMD_T* pt_wgt_ttml_data = (WGT_TTML_DATA_CMD_T*)x_mem_alloc(sizeof(WGT_TTML_DATA_CMD_T));
    IPCC_CHECK_NULL(pt_wgt_ttml_data,IPCCR_INV_ARG);
    x_memset( pt_wgt_ttml_data, 0, sizeof(WGT_TTML_DATA_CMD_T) );
    DLIST_INIT(&pt_wgt_ttml_data->t_element_p_list);

    i4_ret = sbtl_ipcc_parse(pt_sbtl_ttml_data, pt_wgt_ttml_data);
    IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
    
    return IPCCR_OK;
 }




/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_remove
 *
 * Description: This API is used to remove the specific window
 *
 * Inputs:  ps_crnt_wnd              - The id of <region>
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_ttml_remove(
                IN  CHAR* ps_crnt_wnd)
{
    /* Check arguments */
    IPCC_CHECK_NULL(ps_crnt_wnd,IPCCR_INV_ARG);

    INT32 i4_ret = IPCCR_OK;
    UINT32 ui4_crnt_wnd = 0;

    /* If there is no region id, then set "undefine" as default region id. */
    if(0 != x_strcmp(IPCC_UNDEFINE, (const CHAR*)ps_crnt_wnd))
    {
        if(TRUE == gb_is_enhance_rollup)
        {
            ui4_crnt_wnd = 0xFF;
        }
        else
        {
            i4_ret = _sbtl_ipcc_abstract_region_id(ps_crnt_wnd, &ui4_crnt_wnd);
            IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
        }
        
    }
    else
    {
        ui4_crnt_wnd = 0xFF;
    }

    //if(SBTL_IPCC_CAPTION_MODE_ROLL_UP != ge_crnt_caption_mode)
    {
        i4_ret = sbtl_ipcc_wgt_remove(ui4_crnt_wnd);
        IPCC_CHECK_VALID(IPCCR_OK,i4_ret);
    }
    

    UINT8 ui1_i = 0 ;
    for(ui1_i = 0; ui1_i < IPCC_MAX_WGT_NUM; ++ui1_i)
    {
        gaui1_crnt_wnd[ui1_i] = 8;
    }

    /* The following is used to clear all window attr for enhanced mode of roll-up. */
#if 1
    if(TRUE == gb_is_enhance_rollup)
    {
        _sbtl_ipcc_init_global_region_attr();
        _sbtl_ipcc_init_global_wnd_attr();
    }
#endif

    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_calculate_display_ratio
 *
 * Description: This API is used to calculate display ratio according to the first
 *              parsed tts:extent.
 *
 * Inputs:  ps_extent              - The first parsed tts:extent
 *          ps_text                - The first parsed text
 *          pe_file_type           - The xml file type
 *
 * Outputs: pi4_display_ratio      - The display ratio
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_ttml_calculate_display_ratio(
                            IN  const CHAR* ps_extent, 
                            IN  UINT8* pui1_row_count, 
                            IN  SBTL_IPCC_FILE_TYPE* pe_file_type,
                            OUT FLOAT* pf_display_ratio)
{
    /* Check arguments */
    IPCC_CHECK_NULL(ps_extent, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui1_row_count, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_file_type, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pf_display_ratio, IPCCR_INV_ARG);
    
    /* If there is no definition of tts:extent */
    if(0 == x_strcmp((const CHAR*)ps_extent, IPCC_INVALID_VALUE))
    {
        *pf_display_ratio               = IPCC_DISPLAY_RATIO_90;
        gf_after_cnvt_display_ratio     = *pf_display_ratio;
        gb_apply_convert_rule           = TRUE;

        gui1_width_percent = (UINT8)gf_after_cnvt_display_ratio ;
        gui1_height_percent= (UINT8)gf_after_cnvt_display_ratio ;

        DBG_INFO((IPCC_PREFIX" INFO: Need conversion!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        
        return IPCCR_OK;
    }

    INT32 i4_ret = IPCCR_OK;
        
    SIZE_T z_str_len = x_strlen(ps_extent);
    CHAR* ps_ratio = (CHAR*)x_mem_alloc(sizeof(CHAR) * (z_str_len + 1));
    IPCC_CHECK_NULL(ps_ratio, IPCCR_INSUFFICIENT_MEMORY);
    x_memset(ps_ratio, 0, (SIZE_T)(z_str_len + 1));  
    x_strcpy(ps_ratio, (const CHAR*)ps_extent);


    /* Check the length unit, "%" is valid */
    TTML_LENGTH_UNIT_TYPE e_origin_type = TTML_LENGTH_UNIT_TYPE_UNKNOW;
    i4_ret = x_sbtl_ipcc_util_detect_length_unit(ps_ratio, &e_origin_type);
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(ps_ratio);
        return IPCCR_INV_ARG;
    }
    else if(TTML_LENGTH_UNIT_TYPE_PRCNT != e_origin_type)
    {
        DBG_INFO((IPCC_PREFIX" INFO: The length unit is not percent!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        IPCC_MEM_FREE(ps_ratio);
        return IPCCR_INV_ARG;
    }

    /* Abstract the line height from tts:extent */
    CHAR* ps_row = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
    CHAR* ps_col = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
    i4_ret = x_sbtl_ipcc_util_abstract_value(ps_ratio,&e_origin_type, ps_row, ps_col);
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(ps_ratio);
        IPCC_MEM_FREE(ps_row);
        IPCC_MEM_FREE(ps_col);
        return IPCCR_INV_ARG;
    }

    /* Check the value of line height is integer or float */
    FLOAT f_ratio = 0.0;
    CHAR* ps_tmp = NULL;
    
    if(SBTL_IPCC_FILE_TYPE_CRACKLE == *pe_file_type)
    {
        ps_tmp = ps_col;
    }
    else if(SBTL_IPCC_FILE_TYPE_SENVU == *pe_file_type)
    {
        ps_tmp = ps_row;
    }
    
    if(NULL != x_strstr((const CHAR*)ps_tmp, "."))
    {
        FLOAT f_tmp = 0.0;
        i4_ret = x_sbtl_ipcc_str_to_float(ps_tmp, &f_tmp);
        if(IPCCR_OK != i4_ret)
        {
            IPCC_MEM_FREE(ps_ratio);
            IPCC_MEM_FREE(ps_row);
            IPCC_MEM_FREE(ps_col);
        }

        f_ratio = f_tmp;
    }
    else
    {
        UINT32 ui4_tmp = (UINT32)x_strtoull((const CHAR*)ps_tmp, NULL, 10);
        f_ratio = (FLOAT)ui4_tmp;        
    }

    /* Check the current file type */
    if(SBTL_IPCC_FILE_TYPE_CRACKLE == *pe_file_type)
    {
        *pf_display_ratio           = IPCC_DISPLAY_RATIO_90;
        gf_after_cnvt_display_ratio = *pf_display_ratio;
        gui1_width_percent          = (UINT8)gf_after_cnvt_display_ratio;
        gui1_height_percent         = (UINT8)gf_after_cnvt_display_ratio;

        /* Check the line height is greater or less than 90% */
        if(f_ratio < IPCC_CONVERT_POINT_CRACKLE)
        {
            gb_apply_convert_rule = TRUE;         
            DBG_INFO((IPCC_PREFIX" INFO: Need conversion!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        }
        else
        {
            gb_apply_convert_rule = FALSE;
            DBG_INFO((IPCC_PREFIX" INFO: Don't need conversion!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        }
    }
    else if(SBTL_IPCC_FILE_TYPE_SENVU == *pe_file_type)
    {
        *pf_display_ratio           = IPCC_DISPLAY_RATIO_90;
        gf_after_cnvt_display_ratio = *pf_display_ratio;
        gui1_width_percent          = (UINT8)gf_after_cnvt_display_ratio;
        gui1_height_percent         = (UINT8)gf_after_cnvt_display_ratio;
            
        /* Check the line height is greater or less than 5.926% */          
        if((f_ratio < IPCC_CONVERT_POINT_SENVU) //  <5.926%
            || (f_ratio == (2 * IPCC_BASE_LINE_HEIGHT_SENVU)) // ==10%
            || (f_ratio == (3 * IPCC_BASE_LINE_HEIGHT_SENVU)) // ==15%
            || (f_ratio == (4 * IPCC_BASE_LINE_HEIGHT_SENVU)))// ==20%
        {
            gb_apply_convert_rule = TRUE;
            DBG_INFO((IPCC_PREFIX" INFO: Need conversion!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        }
        else
        {
            gb_apply_convert_rule = FALSE;
            DBG_INFO((IPCC_PREFIX" INFO: Don't need conversion!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        }        
    }

    IPCC_MEM_FREE(ps_row);
    IPCC_MEM_FREE(ps_col);
    IPCC_MEM_FREE(ps_ratio);
    
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_record_fpi
 *
 * Description: This API is used to record the first <p> info.
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_ttml_record_fpi(
            IN      SBTL_XML_COMMAND_T* pt_cmd_sets,
            IN OUT  SBTL_IPCC_OPEN_CFG_T* pt_open_cfg )
{
    IPCC_CHECK_NULL(pt_cmd_sets, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_open_cfg, IPCCR_INV_ARG);

    INT32               i4_i        = 0;
    SBTL_XML_COMMAND_T *pt_cmd_iter = pt_cmd_sets;

    while (pt_cmd_iter->e_code != SBTL_XML_CMD_CODE_END)
    {
        switch(pt_cmd_iter->e_code)
        {
            case SBTL_XML_CMD_CODE_ASPECT_RATIO:
            {
                SIZE_T z_len = 0;
                if(NULL != pt_cmd_iter->u.ps_text)
                {
                    z_len = x_strlen((const CHAR *) pt_cmd_iter->u.ps_text);
                }
                else
                {
                    z_len = x_strlen((const CHAR *)IPCC_INVALID_VALUE);
                }
                
                CHAR* ps_str = (CHAR*)x_mem_alloc(sizeof(CHAR) * (z_len+1));
                if(NULL == ps_str)
                {
                    IPCC_MEM_FREE(pt_open_cfg);
                    return IPCCR_INSUFFICIENT_MEMORY;
                }

                if(NULL != pt_cmd_iter->u.ps_text)
                {
                    x_strcpy(ps_str, (const CHAR*)pt_cmd_iter->u.ps_text);
                }
                else
                {
                    x_strcpy(ps_str, (const CHAR*)IPCC_INVALID_VALUE);
                }

                pt_open_cfg->ps_aspect_ratio = ps_str;
                

                DBG_INFO(("(A_R: %s), ",pt_open_cfg->ps_aspect_ratio));
            }
            break;

            case SBTL_XML_CMD_CODE_FIRST_P_EXTENT:
            {
                SIZE_T z_len = 0;
                if(NULL != pt_cmd_iter->u.ps_text)
                {
                    z_len = x_strlen((const CHAR *) pt_cmd_iter->u.ps_text);
                }
                else
                {
                    z_len = x_strlen((const CHAR *)IPCC_INVALID_VALUE);
                }

                CHAR* ps_str = (CHAR*)x_mem_alloc(sizeof(CHAR) * (z_len+1));
                if(NULL == ps_str)
                {
                    IPCC_MEM_FREE(pt_open_cfg);
                    return IPCCR_INSUFFICIENT_MEMORY;
                }

                if(NULL != pt_cmd_iter->u.ps_text)
                {
                    x_strcpy(ps_str, (const CHAR*)pt_cmd_iter->u.ps_text);
                    gb_is_extent_exist = TRUE;
                }
                else
                {
                    x_strcpy(ps_str, (const CHAR*)IPCC_INVALID_VALUE);
                    gb_is_extent_exist = FALSE;
                }
                
                pt_open_cfg->ps_extent  = ps_str;

                DBG_INFO(("(EXTENT: %s), ", pt_open_cfg->ps_extent));                

            }
            break;

            case SBTL_XML_CMD_CODE_FIRST_P_LINES:
            {
                if(pt_cmd_iter->u.ui1_data > 0)
                {
                    pt_open_cfg->ui1_row_count = pt_cmd_iter->u.ui1_data;
                }
                else
                {
                    pt_open_cfg->ui1_row_count = 1;
                }                

                DBG_INFO(("(P_L: %u), ", pt_open_cfg->ui1_row_count));
            }
            break;

            case SBTL_XML_CMD_CODE_SET_VIDEO_TYPE:
            {
               pt_open_cfg->t_ipcc_video.e_video_type   = (SBTL_IPCC_VIDEO_TYPE)pt_cmd_iter->u.ui4_data;
               gt_video_info.e_video_type               = pt_open_cfg->t_ipcc_video.e_video_type;

               DBG_INFO(("(V_T: %d), ", pt_open_cfg->t_ipcc_video.e_video_type));
            }
            break;

            case SBTL_XML_CMD_CODE_SET_VIDEO_RESOLUTION:
            {
               UINT32 ui4_resolution    = pt_cmd_iter->u.ui4_data;
               UINT32 ui4_tmp           = 0;

               pt_open_cfg->t_ipcc_video.ui2_video_width = (UINT16)(ui4_resolution >> 16);
               ui4_resolution = pt_cmd_iter->u.ui4_data;
               ui4_tmp = (UINT32)pt_open_cfg->t_ipcc_video.ui2_video_width;
               pt_open_cfg->t_ipcc_video.ui2_video_height = (UINT16)(ui4_resolution - (ui4_tmp<<16));

               gt_video_info.ui2_video_height   = pt_open_cfg->t_ipcc_video.ui2_video_height;
               gt_video_info.ui2_video_width    = pt_open_cfg->t_ipcc_video.ui2_video_width;

               DBG_INFO(("(V_R: h=%u, w=%u), ", 
                    pt_open_cfg->t_ipcc_video.ui2_video_height,
                    pt_open_cfg->t_ipcc_video.ui2_video_width));

            }
            break;

            case SBTL_XML_CMD_CODE_SET_FILE_TYPE:
            {
                pt_open_cfg->t_ipcc_video.e_file_type = (SBTL_IPCC_FILE_TYPE)pt_cmd_iter->u.ui4_data;
                gt_video_info.e_file_type = pt_open_cfg->t_ipcc_video.e_file_type;

                DBG_INFO(("(F_T: %d), ", pt_open_cfg->t_ipcc_video.e_file_type));
            }
            break;

            case SBTL_XML_CMD_CODE_SET_FILE_MODE:
            {
                /* In xml/m4t file, there exist 'mode="Preserved"' or 'mode="Enhanced"' */
                pt_open_cfg->t_ipcc_video.e_file_mode = (SBTL_IPCC_FILE_MODE)pt_cmd_iter->u.ui4_data;
                gt_video_info.e_file_mode = pt_open_cfg->t_ipcc_video.e_file_mode;

                /* No matter "Preserved" or "Enhanced", just assigned to "Preserved" for V***o. */
                gt_video_info.e_file_mode = SBTL_IPCC_FILE_MODE_PRESERVED;

                DBG_INFO(("(F_M: %d), ", pt_open_cfg->t_ipcc_video.e_file_mode));
            }
            break;

            default:
            break;
        }
        ++i4_i;
        pt_cmd_iter = (SBTL_XML_COMMAND_T*)&pt_cmd_sets[i4_i];
    }

    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_recal_aspect_ratio
 *
 * Description: This API is used to recalculate aspect ratio.
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_ttml_recal_aspect_ratio(
            IN OUT  SBTL_IPCC_OPEN_CFG_T* pt_open_cfg)
{
    IPCC_CHECK_NULL(pt_open_cfg, IPCCR_INV_ARG);

    /* Recalculte the aspect ratio of current video */
    UINT32                  ui4_asp_ratio   = 0;
    SM_CC_ASPECT_RATIO_T    e_asp_ratio     = SM_CC_ASPECT_RATIO_4_3;
    
    if ((pt_open_cfg->t_ipcc_video.ui2_video_width != 0) 
            && (pt_open_cfg->t_ipcc_video.ui2_video_height != 0))
    {
        /* calcuate aspect ratio according to the ratio between height and width */
        ui4_asp_ratio = pt_open_cfg->t_ipcc_video.ui2_video_width * 100 
                        / pt_open_cfg->t_ipcc_video.ui2_video_height;

        if (ui4_asp_ratio <= 133) /* less than ASPECT_RATIO_4_3 */
        {
            e_asp_ratio = SM_CC_ASPECT_RATIO_4_3;
        }
        else if (ui4_asp_ratio >= 178) /* greater than ASPECT_RATIO_16_9 */
        {
            e_asp_ratio = SM_CC_ASPECT_RATIO_16_9;
        }
        else /* in the medium */
        {
            e_asp_ratio = (133 * 100 / ui4_asp_ratio) > (ui4_asp_ratio * 100 / 178) ?
                          SM_CC_ASPECT_RATIO_4_3 : SM_CC_ASPECT_RATIO_16_9;
        }
    }
    gt_aspect_ratio = e_asp_ratio;
    DBG_INFO((IPCC_PREFIX" Info: Video aspect ratio is %d!   <%d, %s, %s>\n\r", 
                gt_aspect_ratio, __LINE__, __func__, __FILE__));
    

    if(SM_CC_ASPECT_RATIO_4_3 == gt_aspect_ratio)
    {
        pt_open_cfg->ps_aspect_ratio = IPCC_ASPECT_ARTIO_4_3;
    }
    else if(SM_CC_ASPECT_RATIO_16_9 == gt_aspect_ratio)
    {
        pt_open_cfg->ps_aspect_ratio = IPCC_ASPECT_ARTIO_16_9;
    }
    else
    {
        pt_open_cfg->ps_aspect_ratio = IPCC_ASPECT_ARTIO_OTHER;
    }

    DBG_INFO((IPCC_PREFIX" Info: ps_aspect_ratio is %s!   <%d, %s, %s>\n\r", 
                pt_open_cfg->ps_aspect_ratio, __LINE__, __func__, __FILE__));

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_clear_global_varible
 *
 * Description: This API is used to clear global varible.
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
VOID sbtl_ipcc_ttml_clear_global_varible()
{
    gb_apply_convert_rule   = FALSE;
    gb_is_global_style_set  = FALSE;
    
    IPCC_MEM_FREE(gpt_sbtl_ttml_record_global_style);
    
    gt_video_info.e_video_type      = SBTL_IPCC_VIDEO_TYPE_HD;
    gt_video_info.ui2_video_height  = 0;
    gt_video_info.ui2_video_width   = 0;

    gui1_width_percent  = 100;
    gui1_height_percent = 100;
}


/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_init_global_varible
 *
 * Description: This API is used to init global varible.
 *
 * Inputs:  
 * Outputs: 
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
VOID sbtl_ipcc_ttml_init_global_varible()
{
    gt_video_info.e_video_type      = SBTL_IPCC_VIDEO_TYPE_HD;
    gt_video_info.ui2_video_height  = 0;
    gt_video_info.ui2_video_width   = 0;
    gt_video_info.e_file_type       = SBTL_IPCC_FILE_TYPE_CRACKLE;
    gt_video_info.e_file_mode       = SBTL_IPCC_FILE_MODE_PRESERVED; // It's better to set PRESERVED in init phase.
    gb_is_extent_exist              = FALSE;

    gf_xml_display_ratio = IPCC_DISPLAY_RATIO_90;
    gb_apply_convert_rule = FALSE;
    //pt_sbtl_ipcc_wgt = NULL;
}



/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_ttml_get_enhance_roll_up_mode
 *
 * Description: This API is used to get the value of gb_is_enhance_rollup.
 *
 * Inputs:  
 * Outputs: pb_flag
 *
 * Returns: IPCCR_OK                       
 *          IPCCR_INV_ARG  
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_ttml_get_enhance_roll_up_mode(OUT BOOL* pb_flag)
{
    IPCC_CHECK_NULL(pb_flag, IPCCR_INV_ARG);
    *pb_flag = gb_is_enhance_rollup;
    DBG_INFO((IPCC_PREFIX" Info: *pb_flag=%d, gb_is_enhance_rollup=%d!   <%d, %s, %s>\n\r", 
            *pb_flag, gb_is_enhance_rollup, __LINE__, __func__, __FILE__));

    return IPCCR_OK;
}


#endif // __KERNEL__
