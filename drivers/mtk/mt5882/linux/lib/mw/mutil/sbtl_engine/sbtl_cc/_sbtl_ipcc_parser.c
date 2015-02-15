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

#include "_sbtl_ipcc_parser.h"
#include "x_sbtl_ipcc_util.h"
#include "_sbtl_ipcc_wgt.h"
#include "_sbtl_ipcc_common.h"
#include "_sbtl_ipcc_process.h"


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
extern SBTL_IPCC_WGT_T*             pt_sbtl_ipcc_wgt;
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
                    Local varible
-----------------------------------------------------------------------------*/
static UINT32 ui4_scrn_ht_cust_768[7][3] = {
                                {31, 36, 39},//{29, 32, 36},
                                {29, 32, 34},//{29, 32, 36},
                                {31, 35, 39},//{29, 32, 36},
                                {31, 34, 39},//{29, 32, 36},
                                {30, 34, 38},//{29, 32, 36},
                                {28, 32, 35}, //{29, 32, 36},
                                {33, 37, 42},//{29, 32, 36}
                                };


static UINT32 ui4_scrn_ht_cust_1080[7][3] = {
                                {28, 31, 34},
                                {28, 31, 34},
                                {28, 31, 34},
                                {28, 31, 34},
                                {28, 31, 34},
                                {28, 31, 34},
                                {28, 31, 34}
                                };

static UINT32 ui4_scrn_ht_cust_720[7][3] = {
                                {19, 34, 38},
                                {14, 26, 29},
                                {19, 34, 38},
                                {14, 26, 29},
                                {19, 34, 38},
                                {14, 26, 29},
                                {14, 26, 29}
                                };



#ifndef ______________________________________________SPLIT_LINE_______
#endif
/*-----------------------------------------------------------------------------
                    function declaration
-----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_color
(
    IN CHAR* ps_ttml_color, 
    OUT GL_COLOR_T* pt_tmp_color
);

static INT32 _sbtl_ipcc_parse_font_family
(
    IN SBTL_TTML_ATTR_FONT_FAMILY* pe_ttml_font_family, 
    OUT CHAR* ps_font_name
);

static INT32 _sbtl_ipcc_parse_font_style
(
    IN SBTL_TTML_ATTR_FONT_STYLE* pe_ttml_font_style, 
    OUT WGL_CC_FNT_STYLE_T* pt_wgl_font_style
);

static INT32 _sbtl_ipcc_parse_font_weight
(
    IN SBTL_TTML_ATTR_FONT_WEIGHT* pe_ttml_font_weight, 
    OUT WGL_CC_FNT_STYLE_T* pt_wgl_font_style
);

static INT32 _sbtl_ipcc_parse_text_decoration
(
    IN SBTL_TTML_ATTR_TEXT_DECORATION* pe_ttml_text_decoration, 
    OUT WGL_CC_FNT_STYLE_T* pt_wgl_font_style
);

static INT32 _sbtl_ipcc_parse_text_align
(
    IN SBTL_TTML_ATTR_TEXT_ALIGIN* pe_ttml_text_align, 
    OUT UINT8* pui1_wgl_text_align
);

static INT32 _sbtl_ipcc_parse_font_size
(
    IN SBTL_TTML_ATTR_FONT_FAMILY* pe_font_family_name, 
    IN UINT32* pui4_font_size, 
    IN TTML_LENGTH_UNIT_TYPE* pe_font_size_unit, 
    OUT FE_FNT_SIZE* pe_font_size
);

static INT32 _sbtl_ipcc_parse_to_wgt_font_size
(
    IN UINT32 *pui4_font, 
    IN UINT32* pui4_font_size, 
    IN FE_FNT_SIZE* pe_font_size, 
    IN TTML_LENGTH_UNIT_TYPE* pe_font_size_unit
);

static INT32 _sbtl_ipcc_parse_window_size_for_integer
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN UINT32* pui4_origin_width,
    IN UINT32* pui4_origin_height,
    OUT UINT16* pui2_window_row_count, 
    OUT UINT16* pui2_window_col_count
);


static INT32 _sbtl_ipcc_parse_window_size_for_float
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN FLOAT* pf_origin_width,
    IN FLOAT* pf_origin_height,
    OUT UINT16* pui4_window_row_count,
    OUT UINT16* pui4_window_col_count
);


static INT32 _sbtl_ipcc_parse_pen_edge_type
(
    IN CHAR* ps_pen_edge_type, 
    IN OUT UINT8* pui1_edge_type
);

static INT32 _sbtl_ipcc_parse_handle_global_style
(
    IN SBTL_TTML_ELEMENT_P_T* pt_elem_p, 
    IN UINT8* pui1_crnt_wnd
);

static INT32 _sbtl_ipcc_parse_set_wnd_attr(IN UINT8* pui1_crnt_wnd);

static INT32 _sbtl_ipcc_parse_handle_global_attr
(
    IN SBTL_TTML_ATTR_T *pt_attr, 
    IN UINT8 ui1_crnt_wnd
);

static INT32 _sbtl_ipcc_parse_text_tag
(
    IN CHAR* ps_role, 
    OUT UINT8* pui1_text_tag
);

static INT32 _sbtl_ipcc_parse_recalculate_window_anchor
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN FLOAT* pf_origin_width,
    IN FLOAT* pf_origin_height,
    OUT UINT16* pui2_window_anchor_v, 
    OUT UINT16* pui2_window_anchor_h
);

static INT32 _sbtl_ipcc_parse_window_anchor
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN FLOAT* pf_origin_width,
    IN FLOAT* pf_origin_height,
    OUT UINT16* pui2_window_anchor_v, 
    OUT UINT16* pui2_window_anchor_h
);

static INT32 _sbtl_ipcc_parse_detect_length_unit
(
    IN CHAR* ps_str, 
    IN OUT TTML_LENGTH_UNIT_TYPE* pe_origin_type
);

static BOOL _sbtl_ipcc_parse_is_space (IN CHAR  c_char);

static INT32 _sbtl_ipcc_parse_detect_caption_mode
(
    IN  CHAR* ps_src_str, 
    OUT SBTL_IPCC_CAPTION_MODE* pe_caption_mode
);




#ifndef ______________________________________________SPLIT_LINE_______
#endif


/*-----------------------------------------------------------------------------
                    function define
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_color
 *
 * Description: This API is used to parse color of tts:color and tts:backgroundColor
 *
 * Inputs:  ps_ttml_color     - tts:color or tts:backgroundColor
 *
 * Outputs: pt_tmp_color      - Color
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_color
(
    IN CHAR* ps_ttml_color, 
    OUT GL_COLOR_T* pt_tmp_color
)

{
    /* Check arguments */
    IPCC_CHECK_NULL(ps_ttml_color,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_tmp_color,IPCCR_INV_ARG);
    
    if((x_strcmp("transparent", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("00000000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("000000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#00000000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("000000", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0x00;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("black", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("000000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#000000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#000000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("000000", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("white", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ffffffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ffffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#ffffffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#ffffff", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0xFF;
        pt_tmp_color->u2.g  = (UINT8)0xFF;
        pt_tmp_color->u3.b  = (UINT8)0xFF;
    }
    else if((x_strcmp("red", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ff0000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ff0000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#ff0000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#ff0000", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0xFF;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("green", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("008000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("008000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#008000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#008000", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0x80;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("yellow", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ffff00ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ffff00", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#ffff00ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#ffff00", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0xFF;
        pt_tmp_color->u2.g  = (UINT8)0xFF;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("blue", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("0000ffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("0000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#0000ffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#0000ff", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0xFF;
    }
    else if((x_strcmp("cyan", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("aqua", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("00ffffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("00ffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#00ffffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#00ffff", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0xFF;
        pt_tmp_color->u3.b  = (UINT8)0xFF;
    }
    else if((x_strcmp("magenta", (const CHAR *)ps_ttml_color) == 0)
        || (x_strcmp("fuchsia", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ff00ffff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("ff00ff", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0xFF;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0xFF;
    }
    else if((x_strcmp("silver", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("c0c0c0ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("c0c0c0", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#c0c0c0ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#c0c0c0", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0xc0;
        pt_tmp_color->u2.g  = (UINT8)0xc0;
        pt_tmp_color->u3.b  = (UINT8)0xc0;
    }
    else if((x_strcmp("gray", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("808080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("808080", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#808080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#808080", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x80;
        pt_tmp_color->u2.g  = (UINT8)0x80;
        pt_tmp_color->u3.b  = (UINT8)0x80;
    }
    else if((x_strcmp("maroon", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("800000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("800000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#800000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#800000", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x80;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("purple", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("800080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("800080", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#800080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#800080", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x80;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0x80;
    }
    else if((x_strcmp("lime", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("00ff00ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("00ff00", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#00ff00ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#00ff00", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0xFF;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("olive", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("808000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("808000", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#808000ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#808000", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x80;
        pt_tmp_color->u2.g  = (UINT8)0x80;
        pt_tmp_color->u3.b  = (UINT8)0x00;
    }
    else if((x_strcmp("navy", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("000080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("000080", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#000080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#000080", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0x00;
        pt_tmp_color->u3.b  = (UINT8)0x80;
    }
    else if((x_strcmp("teal", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("008080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("008080", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#008080ff", (const CHAR *)ps_ttml_color) == 0)
       ||(x_strcmp("#008080", (const CHAR *)ps_ttml_color) == 0))
    {
        pt_tmp_color->a     = (UINT8)0xFF;
        pt_tmp_color->u1.r  = (UINT8)0x00;
        pt_tmp_color->u2.g  = (UINT8)0x80;
        pt_tmp_color->u3.b  = (UINT8)0x80;
    }
    else
    {
        //CHAR* pps_str[4] = {"", "", "", ""};
        CHAR* ps_a = (CHAR *)x_mem_alloc(sizeof(CHAR)* 2);
        if(NULL == ps_a)
        {
            DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        
        CHAR* ps_r = (CHAR *)x_mem_alloc(sizeof(CHAR)* 2);
        if(NULL == ps_r)
        {
            DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            IPCC_MEM_FREE(ps_a);
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        
        CHAR* ps_g = (CHAR *)x_mem_alloc(sizeof(CHAR)* 2);
        if(NULL == ps_g)
        {
            DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            IPCC_MEM_FREE(ps_a);
            IPCC_MEM_FREE(ps_r);
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        
        CHAR* ps_b = (CHAR *)x_mem_alloc(sizeof(CHAR)* 2);
        if(NULL == ps_b)
        {
            DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            IPCC_MEM_FREE(ps_a);
            IPCC_MEM_FREE(ps_r);
            IPCC_MEM_FREE(ps_g);
            return MMSBTLR_INSUFFICIENT_SIZE;
        }
        
        if(6 == x_strlen((const CHAR *) ps_ttml_color))
        {
            x_strncpy(ps_r, (const CHAR *) ps_ttml_color, (SIZE_T)2);
            x_strncpy(ps_g, (const CHAR *) (ps_ttml_color+2), (SIZE_T)2);
            x_strncpy(ps_b, (const CHAR *) (ps_ttml_color+4), (SIZE_T)2);
            
            pt_tmp_color->a     = (UINT8)0xFF;
            pt_tmp_color->u1.r  = (UINT8)x_strtoull((const CHAR*)ps_r, NULL, 16);
            pt_tmp_color->u2.g  = (UINT8)x_strtoull((const CHAR*)ps_g, NULL, 16);
            pt_tmp_color->u3.b  = (UINT8)x_strtoull((const CHAR*)ps_b, NULL, 16);
        }
        else if(7 == x_strlen((const CHAR *) ps_ttml_color))
        {
            x_strncpy(ps_r, (const CHAR *) (ps_ttml_color+1), (SIZE_T)2);
            x_strncpy(ps_g, (const CHAR *) (ps_ttml_color+3), (SIZE_T)2);
            x_strncpy(ps_b, (const CHAR *) (ps_ttml_color+5), (SIZE_T)2);
            
            pt_tmp_color->a     = (UINT8)0xFF;
            pt_tmp_color->u1.r  = (UINT8)x_strtoull((const CHAR*)ps_r, NULL, 16);
            pt_tmp_color->u2.g  = (UINT8)x_strtoull((const CHAR*)ps_g, NULL, 16);
            pt_tmp_color->u3.b  = (UINT8)x_strtoull((const CHAR*)ps_b, NULL, 16);
        }
        else if(8 == x_strlen((const CHAR *) ps_ttml_color))
        {
            x_strncpy(ps_a, (const CHAR *) (ps_ttml_color), (SIZE_T)2);
            x_strncpy(ps_r, (const CHAR *) (ps_ttml_color+2), (SIZE_T)2);
            x_strncpy(ps_g, (const CHAR *) (ps_ttml_color+4), (SIZE_T)2);
            x_strncpy(ps_b, (const CHAR *) (ps_ttml_color+6), (SIZE_T)2);
            
            pt_tmp_color->a     = (UINT8)x_strtoull((const CHAR*)ps_a, NULL, 16);
            pt_tmp_color->u1.r  = (UINT8)x_strtoull((const CHAR*)ps_r, NULL, 16);
            pt_tmp_color->u2.g  = (UINT8)x_strtoull((const CHAR*)ps_g, NULL, 16);
            pt_tmp_color->u3.b  = (UINT8)x_strtoull((const CHAR*)ps_b, NULL, 16);
        }
        else if(9 == x_strlen((const CHAR *) ps_ttml_color))
        {
            x_strncpy(ps_a, (const CHAR *) (ps_ttml_color+1), (SIZE_T)2);
            x_strncpy(ps_r, (const CHAR *) (ps_ttml_color+3), (SIZE_T)2);
            x_strncpy(ps_g, (const CHAR *) (ps_ttml_color+5), (SIZE_T)2);
            x_strncpy(ps_b, (const CHAR *) (ps_ttml_color+7), (SIZE_T)2);
            
            pt_tmp_color->a     = (UINT8)x_strtoull((const CHAR*)ps_a, NULL, 16);
            pt_tmp_color->u1.r  = (UINT8)x_strtoull((const CHAR*)ps_r, NULL, 16);
            pt_tmp_color->u2.g  = (UINT8)x_strtoull((const CHAR*)ps_g, NULL, 16);
            pt_tmp_color->u3.b  = (UINT8)x_strtoull((const CHAR*)ps_b, NULL, 16);
        }
        else
        {
            DBG_INFO(("{IPCC} INFO: Not support this color -- %s!   <%d, %s, %s>\n\r", ps_ttml_color, __LINE__, __func__, __FILE__));
            
            pt_tmp_color->a     = (UINT8)0x00;
            pt_tmp_color->u1.r  = (UINT8)0x00;
            pt_tmp_color->u2.g  = (UINT8)0x00;
            pt_tmp_color->u3.b  = (UINT8)0x00;
        }

        IPCC_MEM_FREE(ps_a);
        IPCC_MEM_FREE(ps_r);
        IPCC_MEM_FREE(ps_g);
        IPCC_MEM_FREE(ps_b);        
    }    

    DBG_INFO(("{IPCC} INFO: Color is %s, a=0x%x, r=0x%x, g=0x%x, b=0x%x!   <%d, %s, %s>\n\r", 
                    ps_ttml_color, pt_tmp_color->a, pt_tmp_color->u1.r, pt_tmp_color->u2.g, pt_tmp_color->u3.b,
                    __LINE__, __func__, __FILE__));

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_font_family
 *
 * Description: This API is used to parse font family of tts:fontFamily
 *
 * Inputs:  pe_ttml_font_family     - tts:fontFamily
 *
 * Outputs: ps_font_name            - Font family name
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_font_family
(
    IN SBTL_TTML_ATTR_FONT_FAMILY* pe_ttml_font_family, 
    OUT CHAR* ps_font_name
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_ttml_font_family,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(ps_font_name,IPCCR_INV_ARG);     

    INT32 i4_ret = IPCCR_OK;
    
    switch(*pe_ttml_font_family)
    {
        case SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_0);
            break;

        case SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_1);
            break;

        case SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_2);
            break;

        case SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_3);
            break;

        case SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_4);
            break;   

        case SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_5);
            break;

        case SBTL_TTML_ATTR_FONT_FAMILY_CURSIVE_TBD:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_6);
            break;

        case SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD:
            x_strcpy(ps_font_name, SM_CC_FONT_STYLE_7);
            break;  

        default:
            i4_ret = IPCCR_NOT_SUPPORT;
            DBG_INFO((IPCC_PREFIX" INFO: Not support!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            break;
    }

    return i4_ret;
}     


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_font_style
 *
 * Description: This API is used to parse font style of tts:fontStyle
 *
 * Inputs:  pe_ttml_font_style     - tts:fontStyle
 *
 * Outputs: ps_font_name            - Font family name
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_font_style
(
    IN SBTL_TTML_ATTR_FONT_STYLE* pe_ttml_font_style, 
    OUT WGL_CC_FNT_STYLE_T* pt_wgl_font_style
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_ttml_font_style,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_wgl_font_style,IPCCR_INV_ARG);  

    INT32 i4_ret = IPCCR_OK;
    
    switch(*pe_ttml_font_style)
    {
        case SBTL_TTML_ATTR_FONT_STYLE_NORMAL:
            *pt_wgl_font_style &= (~WGL_CC_FNT_STYLE_ITALIC);
            break;

        case SBTL_TTML_ATTR_FONT_STYLE_ITALIC:
            *pt_wgl_font_style |= WGL_CC_FNT_STYLE_ITALIC;
            break;

        default:
            i4_ret = IPCCR_NOT_SUPPORT;
            break;
    }

    return i4_ret;

}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_font_weight
 *
 * Description: This API is used to parse font weight of tts:fontWeight
 *
 * Inputs:  pe_ttml_font_weight     - tts:fontWeight
 *
 * Outputs: pt_wgl_font_style       - Font style
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_NOT_SUPPORT     - Not support
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_font_weight
(
    IN SBTL_TTML_ATTR_FONT_WEIGHT* pe_ttml_font_weight, 
    OUT WGL_CC_FNT_STYLE_T* pt_wgl_font_style
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_ttml_font_weight,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_wgl_font_style,IPCCR_INV_ARG);  

    INT32 i4_ret = IPCCR_OK;

    switch(*pe_ttml_font_weight)
    {
        case SBTL_TTML_ATTR_FONT_WEGHT_NORMAL:
            *pt_wgl_font_style &= (~WGL_CC_FNT_STYLE_BOLD);
            break;

        case SBTL_TTML_ATTR_FONT_WEGHT_BOLD:
            *pt_wgl_font_style |= WGL_CC_FNT_STYLE_BOLD;
            break;

        default:
            i4_ret = IPCCR_NOT_SUPPORT;
            break;
    }

    return i4_ret;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_text_decoration
 *
 * Description: This API is used to parse text decoration of tts:textDecoration
 *
 * Inputs:  pe_ttml_text_decoration     - tts:textDecoration
 *
 * Outputs: pt_wgl_font_style           - Font style
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_NOT_SUPPORT     - Not support
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_text_decoration
(
    IN SBTL_TTML_ATTR_TEXT_DECORATION* pe_ttml_text_decoration, 
    OUT WGL_CC_FNT_STYLE_T* pt_wgl_font_style
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_ttml_text_decoration,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_wgl_font_style,IPCCR_INV_ARG);  

    INT32 i4_ret = IPCCR_OK;

    switch(*pe_ttml_text_decoration)
    {
        case SBTL_TTML_ATTR_TEXT_DECORATION_NONE:
            *pt_wgl_font_style &= (~WGL_CC_FNT_STYLE_UNDERLINE);
            break;

        case SBTL_TTML_ATTR_TEXT_DECORATION_UNDERLINE:
            *pt_wgl_font_style |= WGL_CC_FNT_STYLE_UNDERLINE;
            break;

        default:
            i4_ret = IPCCR_NOT_SUPPORT;
            break;
    }

    return i4_ret;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_text_align
 *
 * Description: This API is used to parse text align of tts:textAlign
 *
 * Inputs:  pe_ttml_text_align     - tts:textAlign
 *
 * Outputs: pui1_wgl_text_align    - Text align
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_text_align(
                IN SBTL_TTML_ATTR_TEXT_ALIGIN* pe_ttml_text_align, 
                OUT UINT8* pui1_wgl_text_align)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_ttml_text_align,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui1_wgl_text_align,IPCCR_INV_ARG);  
    
    switch(*pe_ttml_text_align)
    {
        case SBTL_TTML_ATTR_TEXT_ALIGIN_START:
            *pui1_wgl_text_align = WGL_CC_JUSTIFY_LEFT;
             break;
            
        case SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT:
            *pui1_wgl_text_align = WGL_CC_JUSTIFY_LEFT;
            break;

        case SBTL_TTML_ATTR_TEXT_ALIGIN_END:
            *pui1_wgl_text_align = WGL_CC_JUSTIFY_RIGHT;
            break;
            
        case SBTL_TTML_ATTR_TEXT_ALIGIN_RIGHT:
            *pui1_wgl_text_align = WGL_CC_JUSTIFY_RIGHT;
            break;

        case SBTL_TTML_ATTR_TEXT_ALIGIN_CENTER:
            *pui1_wgl_text_align = WGL_CC_JUSTIFY_CENTER;
            break;

        default:
            *pui1_wgl_text_align = WGL_CC_JUSTIFY_LEFT;
            break;
    }
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_font_size
 *
 * Description: This API is used to parse font size of tts:fontSize
 *
 * Inputs:  pe_font_family_name     - tts:fontFamily
 *          pui4_font_size          - tts:fontSize
 *          pe_font_size_unit       - The unit of font size
 *
 * Outputs: pe_font_size            - Font size after parse
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_font_size
(
    IN SBTL_TTML_ATTR_FONT_FAMILY* pe_font_family_name, 
    IN UINT32* pui4_font_size, 
    IN TTML_LENGTH_UNIT_TYPE* pe_font_size_unit, 
    OUT FE_FNT_SIZE* pe_font_size
)
{   
    /* Check arguments */
    IPCC_CHECK_NULL(pe_font_family_name,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui4_font_size,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_font_size,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_font_size_unit,IPCCR_INV_ARG);    

    INT32 i4_ret = IPCCR_OK;
     
    /* Get the screen height */
    UINT32 ui4_screen_height = gui4_screen_height;

    /* Compute the font size accroding to font family and font size */
    switch(ui4_screen_height)
    {
        case ((UINT32)768):
            if(SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT == *pe_font_family_name
                || SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_768[2][0], pui4_font_size, pe_font_size, pe_font_size_unit);
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_768[0][0], pui4_font_size, pe_font_size, pe_font_size_unit);
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_768[1][0], pui4_font_size, pe_font_size, pe_font_size_unit);
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_768[3][0], pui4_font_size, pe_font_size, pe_font_size_unit);
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_768[4][0], pui4_font_size, pe_font_size, pe_font_size_unit);
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_CURSIVE_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_768[5][0], pui4_font_size, pe_font_size, pe_font_size_unit);
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_768[6][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }

            if(IPCCR_OK != i4_ret)
            {
                return i4_ret;
            }
            break;

            
    case ((UINT32)1080):
            if(SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT == *pe_font_family_name
                || SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_1080[2][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_1080[0][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_1080[1][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_1080[3][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_1080[4][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_CURSIVE_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_1080[5][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_1080[6][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }

            if(IPCCR_OK != i4_ret)
            {
                return i4_ret;
            }

            break; 

    case ((UINT32)720):
            if(SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT == *pe_font_family_name
                || SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_720[2][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_720[0][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_720[1][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_720[3][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_720[4][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_CURSIVE_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_720[5][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }
            else if(SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD == *pe_font_family_name)
            {
                i4_ret = _sbtl_ipcc_parse_to_wgt_font_size(&ui4_scrn_ht_cust_720[6][0], pui4_font_size, pe_font_size, pe_font_size_unit);   
            }

            if(IPCCR_OK != i4_ret)
            {
                return i4_ret;
            }

            break;         
            
    }
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_to_wgt_font_size
 *
 * Description: This API is used to parse font size of tts:fontSize of WGT
 *
 * Inputs:  pui4_font               - The standard font size in DTV-CC
 *          pui4_font_size          - tts:fontSize
 *          pe_font_size_unit       - The unit of font size
 *
 * Outputs: pe_font_size            - Font size after parse
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_to_wgt_font_size
(
    IN UINT32 *pui4_font, 
    IN UINT32* pui4_font_size, 
    IN FE_FNT_SIZE* pe_font_size, 
    IN TTML_LENGTH_UNIT_TYPE* pe_font_size_unit
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pui4_font,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui4_font_size,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_font_size,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_font_size_unit,IPCCR_INV_ARG);    

    if(TTML_LENGTH_UNIT_TYPE_C != *pe_font_size_unit)
    {
        FLOAT f_delta_m = ((FLOAT)(*(pui4_font+1) - *pui4_font) / 2);
        FLOAT f_delta_l = ((FLOAT)(*(pui4_font+2) - *pui4_font) / 2 + f_delta_m);
        FLOAT f_delta_font = 0.0;
        
        if(TTML_LENGTH_UNIT_TYPE_PX == *pe_font_size_unit)
        {
            f_delta_font = (FLOAT)(*pui4_font_size - *pui4_font);
        }
        else if(TTML_LENGTH_UNIT_TYPE_PRCNT == *pe_font_size_unit)
        {
            f_delta_font = ((FLOAT)((*pui4_font_size) * (*(pui4_font+1))) / 100 - (*pui4_font));
        }

        if( f_delta_font < f_delta_m)
        {
            *pe_font_size = FE_FNT_SIZE_SMALL;
        }
        else if(f_delta_font >= f_delta_m && f_delta_font < f_delta_l)
        {
            *pe_font_size = FE_FNT_SIZE_MEDIUM;
        }
        else if(f_delta_font >= f_delta_l)
        {
            *pe_font_size = FE_FNT_SIZE_LARGE;
        }
    }
    else if(TTML_LENGTH_UNIT_TYPE_C == *pe_font_size_unit)
    {
        if(0 == *pui4_font_size)
        {
            *pe_font_size = FE_FNT_SIZE_SMALL;
        }
        else if(1 == *pui4_font_size)
        {
            *pe_font_size = FE_FNT_SIZE_MEDIUM;
        }
        else if(2 == *pui4_font_size)
        {
            *pe_font_size = FE_FNT_SIZE_LARGE;
        }
    }
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_abstract_value
 *
 * Description: This API is used to abstract value from string
 *
 * Inputs:  ps_string           - Source string
 *          pe_length_unit      - Length unit
 *          
 * Outputs: ps_row                  - row
 *          ps_col                  - col
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_abstract_value
(
    IN CHAR* ps_string, 
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit, 
    OUT CHAR* ps_row, 
    OUT CHAR* ps_col
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(ps_string,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_length_unit,IPCCR_INV_ARG);  
    IPCC_CHECK_NULL(ps_row,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(ps_col,IPCCR_INV_ARG);

    INT32 i4_ret = IPCCR_OK;
    i4_ret = _sbtl_ipcc_parse_detect_length_unit(ps_string, pe_length_unit);
    if(IPCCR_OK != i4_ret)
    {
        return i4_ret;
    }

    CHAR* ps_origin = (CHAR*)x_mem_alloc(sizeof(CHAR) * IPCC_TTS_ORIGIN_LENGTH);
    if(NULL == ps_origin)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_strcpy(ps_origin, ps_string);

    CHAR* ps_whitespace =" \t\f\r\v\n";
    CHAR* ps_px = "px";
    CHAR* ps_percentage = "%";
    CHAR* ps_rslt[2];
    UINT8 ui1_i = 0;
    
    for(ui1_i = 0; ui1_i < 2; ++ui1_i)
    {
        ps_rslt[ui1_i] = (CHAR*)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
        if(NULL == ps_rslt[ui1_i])
        {
            DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            return IPCCR_INSUFFICIENT_SIZE;
        }
    }

    /* Parse tts:origin by white space, and save the result  */
    i4_ret = x_sbtl_ipcc_str_split(ps_whitespace, ps_origin, ps_rslt, 2, NULL);
    if(IPCCR_OK != i4_ret)
    {
        DBG_ERROR(("\n{IPCC} ERR: i4_ret=%d! <%d, %s, %s>\n\r", 
                            i4_ret, __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    /* Parse each result */
    switch(*pe_length_unit)
    {
        case TTML_LENGTH_UNIT_TYPE_PX:
            i4_ret = x_sbtl_ipcc_str_split(ps_px, ps_rslt[0], &ps_col, 1, NULL);
            if(IPCCR_OK == i4_ret)
            {
                i4_ret = x_sbtl_ipcc_str_split(ps_px, ps_rslt[1], &ps_row, 1, NULL);
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                    return i4_ret;
                }
            }
            else
            {
                DBG_ERROR((IPCC_PREFIX" ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                return i4_ret;
            }

            break;

        case TTML_LENGTH_UNIT_TYPE_PRCNT:
            i4_ret = x_sbtl_ipcc_str_split(ps_percentage, ps_rslt[0], &ps_col, 1, NULL);
            if(IPCCR_OK == i4_ret)
            {
                i4_ret = x_sbtl_ipcc_str_split(ps_percentage, ps_rslt[1], &ps_row, 1, NULL);
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                    return i4_ret;
                }
            }
            else
            {
                DBG_ERROR((IPCC_PREFIX" ERR: Invalid arg or Not matched, i4_ret=%d!   <%d, %s, %s>\n\r",  i4_ret, __LINE__, __func__, __FILE__));
                return i4_ret;
            }

            break;


        default:
            DBG_INFO((IPCC_PREFIX" INFO: Not support this length unit!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
            break;
    }

    IPCC_MEM_FREE(ps_origin);
    for(ui1_i = 0; ui1_i < 2; ++ui1_i)
    {
        IPCC_MEM_FREE(ps_rslt[ui1_i]);
    }

    return i4_ret;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_window_size_for_float
 *
 * Description: This API is used to parse window size.
 *
 * Inputs:  pe_length_unit          - Length unit
 *          pui4_origin_width       - the width of tts:origin
 *          pui4_origin_height      - the height of tts:origin   
 *
 * Outputs: pui4_window_row_count    - 
 *          pui4_window_col_count    -
 *
 * Returns: IPCCR_OK          -Success
 *          IPCCR_INV_ARG     - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_window_size_for_float
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN FLOAT* pf_origin_width,
    IN FLOAT* pf_origin_height,
    OUT UINT16* pui4_window_row_count,
    OUT UINT16* pui4_window_col_count
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_length_unit,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pf_origin_width,IPCCR_INV_ARG);  
    IPCC_CHECK_NULL(pf_origin_height,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui4_window_row_count,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui4_window_col_count,IPCCR_INV_ARG);

    switch(*pe_length_unit)
    {
        case TTML_LENGTH_UNIT_TYPE_PX:
            {
            }
            break;

        case TTML_LENGTH_UNIT_TYPE_PRCNT:
            {
                FLOAT f_tmp_height = *pf_origin_height;
                FLOAT f_tmp_width = *pf_origin_width;

                FLOAT f_row_unit = 0.0;
                FLOAT f_col_unit = 0.0;

                if(TRUE == gb_apply_convert_rule)
                {
                    if(SM_CC_ASPECT_RATIO_16_9 == gt_aspect_ratio)
                    {
                        f_tmp_width = f_tmp_width * IPCC_SCALE_UP_RATIO_HD_X;
                        f_tmp_height = f_tmp_height * IPCC_SCALE_UP_RATIO_HD_Y; 
                        f_col_unit = IPCC_COL_UNIT_90_HD;

                    }
                    else if(SM_CC_ASPECT_RATIO_4_3 == gt_aspect_ratio)
                    {
                        f_tmp_width = f_tmp_width * IPCC_SCALE_UP_RATIO_SD_X;
                        f_tmp_height = f_tmp_height * IPCC_SCALE_UP_RATIO_SD_Y;
                        f_col_unit = IPCC_COL_UNIT_90_SD;
                    }

                    f_row_unit = IPCC_ROW_UNIT_90;
                }  
                else
                {
                    f_row_unit = IPCC_ROW_UNIT_90;
                    f_col_unit = IPCC_COL_UNIT_90_HD;
                }
            
                FLOAT f_row = (FLOAT)(f_tmp_height / f_row_unit);
                FLOAT f_col = (FLOAT)(f_tmp_width / f_col_unit);

                UINT32 ui4_row = (UINT32)f_row;
                UINT32 ui4_col = (UINT32)f_col;

                if(((f_col - (FLOAT)ui4_col) > 0.0) && (ui4_col < IPCC_DEFAULT_WINDOW_COLUMN))
                {
                    ++ui4_col;
                }

                if(((f_row - (FLOAT)ui4_row) > 0.0) && (ui4_row < IPCC_DEFAULT_WINDOW_ROW))
                {
                    ++ui4_row;
                }
                
                *pui4_window_row_count = ui4_row;
                *pui4_window_col_count = ui4_col; 

                if(*pui4_window_row_count > IPCC_DEFAULT_WINDOW_ROW)
                {
                    *pui4_window_row_count = IPCC_DEFAULT_WINDOW_ROW;
                }

                if(*pui4_window_col_count > IPCC_DEFAULT_WINDOW_COLUMN)
                {
                    *pui4_window_col_count = IPCC_DEFAULT_WINDOW_COLUMN;
                }

                /* In Crackle case, if width of tts:extent is less than 90%, expand to 90%. */
                if(SBTL_IPCC_FILE_TYPE_CRACKLE == gt_video_info.e_file_type)
                {
                    *pui4_window_col_count = IPCC_DEFAULT_WINDOW_COLUMN;
                }
            }
            
            break;

        default:
            break;
    }
    
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_window_size_for_integer
 *
 * Description: This API is used to parse window size.
 *
 * Inputs:  pe_length_unit          - Length unit
 *          pui4_origin_width       - the width of tts:origin
 *          pui4_origin_height      - the height of tts:origin   
 *
 * Outputs: pui4_window_row_count    - 
 *          pui4_window_col_count    -
 *
 * Returns: IPCCR_OK          -Success
 *          IPCCR_INV_ARG     - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_window_size_for_integer
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN UINT32* pui4_origin_width,
    IN UINT32* pui4_origin_height,
    OUT UINT16* pui2_window_row_count, 
    OUT UINT16* pui2_window_col_count
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_length_unit,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui4_origin_width,IPCCR_INV_ARG);  
    IPCC_CHECK_NULL(pui4_origin_height,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui2_window_row_count,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui2_window_col_count,IPCCR_INV_ARG);

    switch(*pe_length_unit)
    {
        case TTML_LENGTH_UNIT_TYPE_PX:
            {
                FLOAT f_width_ratio = *pui4_origin_height / pt_sbtl_ipcc_wgt->ui4_canvas_h;
                FLOAT f_height_ratio = *pui4_origin_width / pt_sbtl_ipcc_wgt->ui4_canvas_w;
                UINT16 ui2_row = 0;
                UINT16 ui2_col = 0;

                ui2_row = (UINT16)(f_height_ratio / IPCC_ROW_UNIT_90);
                ui2_col = (UINT16)(f_width_ratio / IPCC_COL_UNIT_90_HD);

                FLOAT f_tmp_row = (FLOAT)((f_height_ratio / IPCC_ROW_UNIT_90) - (FLOAT)ui2_row);
                FLOAT f_tmp_col = (FLOAT)((f_width_ratio / IPCC_COL_UNIT_90_HD) - (FLOAT)ui2_col);

                if(f_tmp_row > 0.0)
                {
                    if(ui2_row < IPCC_DEFAULT_WINDOW_ROW)
                    {
                        ++ui2_row;
                    }                    
                }

                if(f_tmp_col > 0.0)
                {
                    if(ui2_col < IPCC_DEFAULT_WINDOW_COLUMN)
                    {
                        ++ui2_col;
                    }                    
                }

                *pui2_window_col_count = ui2_col;
                *pui2_window_row_count = ui2_row;

                 /* In Crackle case, if width of tts:extent is less than 90%, expand to 90%. */
                if(SBTL_IPCC_FILE_TYPE_CRACKLE == gt_video_info.e_file_type)
                {
                    *pui2_window_col_count = IPCC_DEFAULT_WINDOW_COLUMN;
                }
            }

            break;

        case TTML_LENGTH_UNIT_TYPE_PRCNT:
            {
                UINT32 ui4_tmp_height = *pui4_origin_height;
                UINT32 ui4_tmp_width = *pui4_origin_width;

                FLOAT f_row_unit = 0.0;
                FLOAT f_col_unit = 0.0;

                FLOAT f_scale_up_width = 0.0;
                FLOAT f_scale_up_height = 0.0;
                
                if(TRUE == gb_apply_convert_rule)
                {
                    if((SM_CC_ASPECT_RATIO_16_9 == gt_aspect_ratio))
                    {
                        f_scale_up_width = (FLOAT)ui4_tmp_width * IPCC_SCALE_UP_RATIO_HD_X;
                        f_scale_up_height = (FLOAT)ui4_tmp_height * IPCC_SCALE_UP_RATIO_HD_Y; 
                        f_col_unit = IPCC_COL_UNIT_90_HD;
                    }
                    else if(SM_CC_ASPECT_RATIO_4_3 == gt_aspect_ratio)
                    {
                        f_scale_up_width = (FLOAT)ui4_tmp_width * IPCC_SCALE_UP_RATIO_SD_X;
                        f_scale_up_height = (FLOAT)ui4_tmp_height * IPCC_SCALE_UP_RATIO_SD_Y;  
                        f_col_unit = IPCC_COL_UNIT_90_SD;
                    }

                    f_row_unit = IPCC_ROW_UNIT_90;
                }
                else
                {
                    f_row_unit = IPCC_ROW_UNIT_90;
                    f_col_unit = IPCC_COL_UNIT_90_HD;

                    f_scale_up_width = (FLOAT)ui4_tmp_width;
                    f_scale_up_height = (FLOAT)ui4_tmp_height;
                }
                
                *pui2_window_row_count = (UINT16)(f_scale_up_height / f_row_unit);
                *pui2_window_col_count = (UINT16)(f_scale_up_width / f_col_unit); 

                FLOAT f_row = (FLOAT)f_scale_up_height - (((FLOAT)*pui2_window_row_count)*f_row_unit);
                FLOAT f_col = (FLOAT)f_scale_up_width - (((FLOAT)*pui2_window_col_count)*f_col_unit);
                
                if((f_row > 0.0) && (*pui2_window_row_count < IPCC_DEFAULT_WINDOW_ROW))
                {
                    (*pui2_window_row_count)++;
                }

                if((f_col > 0.0) && (*pui2_window_col_count < IPCC_DEFAULT_WINDOW_COLUMN))
                {
                    (*pui2_window_col_count)++;
                }

                if(*pui2_window_row_count > IPCC_DEFAULT_WINDOW_ROW)
                {
                    *pui2_window_row_count = IPCC_DEFAULT_WINDOW_ROW;
                }

                if(*pui2_window_col_count > IPCC_DEFAULT_WINDOW_COLUMN)
                {
                    *pui2_window_col_count = IPCC_DEFAULT_WINDOW_COLUMN;
                }

                /* In Crackle case, if width of tts:extent is less than 90%, expand to 90%. */
                if(SBTL_IPCC_FILE_TYPE_CRACKLE == gt_video_info.e_file_type)
                {
                    *pui2_window_col_count = IPCC_DEFAULT_WINDOW_COLUMN;
                }
            }
            
            break;

        default:
            break;
    }
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_pen_edge_type
 *
 * Description: This API is used to parse the pen edge type
 *
 * Inputs:  ps_pen_edge_type           - The string which contain pen edge type value
 *          pui1_edge_type             - The data which parsed to wgt pen edge type
 *
 * Outputs: pui1_edge_type              - The data which parsed to wgt pen edge type
 *
 * Returns: IPCCR_OK                 - Success
 *          IPCCR_INV_ARG            - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_pen_edge_type
(
    IN CHAR* ps_pen_edge_type, 
    IN OUT UINT8* pui1_edge_type
)
{
    IPCC_CHECK_NULL(ps_pen_edge_type,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui1_edge_type,IPCCR_INV_ARG);

    if(0 == x_strstr("%", (const CHAR*)ps_pen_edge_type))
    {
        *pui1_edge_type = WGL_CC_EDGE_TYPE_NONE;
    }
    else
    {
        SIZE_T z_strlen = x_strlen((const CHAR *) ps_pen_edge_type);
        
        if(2 < z_strlen && z_strlen < 6)
        {
            if(0 == x_strstr("10", (const CHAR*)ps_pen_edge_type))
            {
                *pui1_edge_type = WGL_CC_EDGE_TYPE_RAISED;
            }
            else if(0 == x_strstr("20", (const CHAR*)ps_pen_edge_type))
            {
                *pui1_edge_type = WGL_CC_EDGE_TYPE_UNIFORM;
            }
            else
            {
                *pui1_edge_type = WGL_CC_EDGE_TYPE_NONE;
            }
        }
        else if(z_strlen > 6)
        {
            if(0 == x_strstr("10% 10%", (const CHAR*)ps_pen_edge_type))
            {
                *pui1_edge_type = WGL_CC_EDGE_TYPE_DEPRESSED;
            }
            else if(0 == x_strstr("10% 20%", (const CHAR*)ps_pen_edge_type))
            {
                *pui1_edge_type = WGL_CC_EDGE_TYPE_LEFT_DROP_SHADOW;
            }
            else if(0 == x_strstr("20% 10%", (const CHAR*)ps_pen_edge_type))
            {
                *pui1_edge_type = WGL_CC_EDGE_TYPE_RIGHT_DROP_SHADOW;
            }
            else
            {
                *pui1_edge_type = WGL_CC_EDGE_TYPE_NONE;
            }
        }
        else
        {
            DBG_INFO((IPCC_PREFIX" INFO: Not support this pen edge type, %s!   <%d, %s, %s>\n\r", ps_pen_edge_type, __LINE__, __func__, __FILE__));
            *pui1_edge_type = WGL_CC_EDGE_TYPE_NONE;
        }
    }

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_handle_global_style
 *
 * Description: This API is used to copy global style
 *
 * Inputs:  pt_elem_p           - The global style
 *          pui1_crnt_wnd       - The current window id
 *
 * Outputs: 
 *
 * Returns: IPCCR_OK                 - Success
 *          IPCCR_INV_ARG            - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_handle_global_style
(
    IN SBTL_TTML_ELEMENT_P_T* pt_elem_p, 
    IN UINT8* pui1_crnt_wnd
)
{
    IPCC_CHECK_NULL(pt_elem_p,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui1_crnt_wnd,IPCCR_INV_ARG);

    INT32 i4_ret = IPCCR_OK;;

    CHAR* ps_region_id = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH));
    if(NULL == ps_region_id)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_memset(ps_region_id, 0, (SIZE_T)(IPCC_TTS_ORIGIN_LENGTH));

    CHAR* ps_style_id = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH));
    if(NULL == ps_style_id)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        IPCC_MEM_FREE(ps_region_id);
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_memset(ps_style_id, 0, (SIZE_T)(IPCC_TTS_ORIGIN_LENGTH));
    
    /* region and style attr */
    SBTL_TTML_ATTR_T* pt_elem_p_attr = SLIST_FIRST(&pt_elem_p->t_common_attr_list);

    while(pt_elem_p_attr)
    {                
        switch(pt_elem_p_attr->e_attr)
        {
            case SBTL_XML_IPCC_ATTR_STYLE:
                x_strcpy(ps_style_id, (const CHAR*)pt_elem_p_attr->u.ps_value); 
                break;

            case SBTL_XML_IPCC_ATTR_REGION:
                x_strcpy(ps_region_id, (const CHAR*)pt_elem_p_attr->u.ps_value);  
                DBG_INFO((IPCC_PREFIX" INFO: region_id=%s!   <%d, %s, %s>\n\r", 
                            ps_region_id, __LINE__, __func__, __FILE__));
                break;
                
            default:
                break;
        }
        pt_elem_p_attr = SLIST_NEXT(pt_elem_p_attr, t_attr_link);
    }

    DBG_INFO((IPCC_PREFIX" INFO: region_id=%s!   <%d, %s, %s>\n\r", 
                            ps_region_id, __LINE__, __func__, __FILE__));
    i4_ret = _sbtl_ipcc_parse_detect_caption_mode(ps_region_id, &ge_crnt_caption_mode);
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(ps_region_id);
        IPCC_MEM_FREE(ps_style_id);
        
        return i4_ret;
    }

#if 1
    

    /* To check if current caption mode is equal to last caption mode */
    if(SBTL_IPCC_CAPTION_MODE_ROLL_UP != ge_crnt_caption_mode &&
       SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_last_caption_mode)
    {
        DBG_INFO((IPCC_PREFIX" INFO: gps_roll_up_last_id=%s!   <%d, %s, %s>\n\r", 
                            gps_roll_up_last_id, __LINE__, __func__, __FILE__));
        if(0 != x_strcmp((const CHAR*)gps_roll_up_last_id, (const CHAR*)ps_region_id))
        {
            i4_ret = sbtl_ipcc_ttml_remove(gps_roll_up_last_id);             
            IPCC_CHECK_VALID(IPCCR_OK,i4_ret);

             x_strcpy(gps_roll_up_last_id, "rollup");
        }
    }
    #if 0
    else if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode &&
            SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
    {
        i4_ret = sbtl_ipcc_ttml_remove(gps_roll_up_last_id);
        IPCC_CHECK_VALID(IPCCR_OK,i4_ret);

        x_strcpy(gps_roll_up_last_id, (const CHAR*)ps_region_id);
    }
    #endif
#endif

    UINT8 ui1_i = 0;
    UINT8 ui1_crnt_wnd = IPCC_MAX_REGION_NUM;

    /* After 1st time, search the proper window. */
    if(!(SBTL_IPCC_FILE_MODE_ENHANCED == gt_video_info.e_file_mode
        && (0 == x_strcmp((const CHAR*)ps_region_id, IPCC_CAPTION_MODE_ROLL))))
    {
        for(ui1_i = 0; ui1_i < IPCC_MAX_REGION_NUM; ++ui1_i)
        {
            if((NULL != ps_region_id) && (NULL != ps_style_id))
            {
                if((0 == x_strcmp((const CHAR*)gt_region_attr[ui1_i].ps_region_id, (const CHAR*)ps_region_id))
                    && (0 == x_strcmp((const CHAR*)gt_region_attr[ui1_i].ps_style_id, (const CHAR*)ps_style_id)))
                {
                    ui1_crnt_wnd = ui1_i;
                    DBG_INFO((IPCC_PREFIX" INFO: ui1_crnt_wnd=%u!   <%d, %s, %s>\n\r", 
                            ui1_crnt_wnd, __LINE__, __func__, __FILE__));
                    break;
                }
            }
            else if((NULL == ps_region_id) && (NULL != ps_style_id))
            {
                if(0 == x_strcmp((const CHAR*)gt_region_attr[ui1_i].ps_style_id, (const CHAR*)ps_style_id))
                {
                    ui1_crnt_wnd = ui1_i;
                    break;
                }
            }
            else if((NULL != ps_region_id) && (NULL == ps_style_id))
            {
                if(0 == x_strcmp((const CHAR*)gt_region_attr[ui1_i].ps_region_id, (const CHAR*)ps_region_id))
                {
                    ui1_crnt_wnd = ui1_i;
                    break;
                }
            }
            else if((NULL == ps_region_id) && (NULL == ps_style_id))
            {
                
            }
            
        }
    }
    else
    {
        ui1_crnt_wnd = IPCC_MAX_REGION_NUM;

        if(FALSE == gb_is_enhance_rollup)
        {
            gb_is_enhance_rollup = TRUE;
        }
    }

    if(ui1_crnt_wnd < IPCC_MAX_REGION_NUM)
    {
        *pui1_crnt_wnd = ui1_crnt_wnd;
        DBG_INFO((IPCC_PREFIX" INFO: *pui1_crnt_wnd=%u!   <%d, %s, %s>\n\r", 
                            *pui1_crnt_wnd, __LINE__, __func__, __FILE__));        
    }
    /* 1st time, need to select unused window to record attributes. */
    else if(IPCC_MAX_REGION_NUM == ui1_crnt_wnd)
    {
        for(ui1_i = 0; ui1_i < IPCC_MAX_REGION_NUM; ++ui1_i)
        {
            if((0 == x_strcmp((const CHAR*)gt_region_attr[ui1_i].ps_region_id, (const CHAR*)" "))
                && (0 == x_strcmp((const CHAR*)gt_region_attr[ui1_i].ps_style_id, (const CHAR*)" ")))
            {
                ui1_crnt_wnd = ui1_i;
                break;
            }
        }

        if(ui1_crnt_wnd < IPCC_MAX_REGION_NUM)
        {
            *pui1_crnt_wnd = ui1_crnt_wnd;

            if((NULL != ps_region_id) && (NULL != ps_style_id))
            {
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_region_id, (const CHAR*)ps_region_id);
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_style_id, (const CHAR*)ps_style_id);
            }
            else if((NULL == ps_region_id) && (NULL != ps_style_id))
            {
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_style_id, (const CHAR*)ps_style_id);
            }
            else if((NULL != ps_region_id) && (NULL == ps_style_id))
            {
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_region_id, (const CHAR*)ps_region_id);
            }
            else if((NULL == ps_region_id) && (NULL == ps_style_id))
            {

            }
            
        }

        /* If not found unused window, then select 1st window */
        else
        {
            DBG_ERROR((IPCC_PREFIX" ERR: Not found correct region!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            IPCC_MEM_FREE(ps_region_id);
            IPCC_MEM_FREE(ps_style_id);
            return IPCCR_INV_ARG;

            /* Need to add flow for the case of not found unused window */
        }
        

        if(!gpt_sbtl_ttml_record_global_style)
        {
            DBG_ERROR((IPCC_PREFIX" ERR: gpt_sbtl_ttml_record_global_style is invalid!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            IPCC_MEM_FREE(ps_region_id);
            IPCC_MEM_FREE(ps_style_id);
            return IPCCR_INV_ARG;
        }

           /* If <p> element does not include "style", 
           then search "style" in common attribute list. */           
        if(0 == x_strcmp("", (const CHAR*)ps_style_id) && 
            0 < x_strlen((const CHAR*)ps_region_id))
        {
            if(!SLIST_IS_EMPTY(&gpt_sbtl_ttml_record_global_style->t_attr_id_list))
            {
                SBTL_TTML_ATTR_ID_T* pt_ttml_attr = SLIST_FIRST(&gpt_sbtl_ttml_record_global_style->t_attr_id_list);

                while(pt_ttml_attr)
                {
                    DBG_INFO(("{IPCC} Info: pt_ttml_attr->e_attr_type is %d, pt_ttml_attr->ps_attr_id_value is %s, ps_region_id is %s!   <%d, %s, %s>\n\r", 
                                    pt_ttml_attr->e_attr_type, pt_ttml_attr->ps_attr_id_value, ps_region_id,
                                    __LINE__, __func__, __FILE__));
                    if( SBTL_XML_IPCC_ATTR_REGION == pt_ttml_attr->e_attr_type &&
                        0 == x_strcmp((const CHAR*)ps_region_id, (const CHAR*)pt_ttml_attr->ps_attr_id_value))    
                    {
                        if(!SLIST_IS_EMPTY(&pt_ttml_attr->t_attr_list))
                        {
                            SBTL_TTML_ATTR_T* pt_attr = SLIST_FIRST(&pt_ttml_attr->t_attr_list);
                            while(pt_attr)
                            {
                                if(SBTL_XML_IPCC_ATTR_STYLE == pt_attr->e_attr)
                                {
                                    x_strcpy(ps_style_id, (const CHAR*)pt_attr->u.ps_value);
                                    DBG_INFO(("{IPCC} Info: pt_attr->u.ps_value is %s, ps_style_id is %s!   <%d, %s, %s>\n\r", 
                                        pt_attr->u.ps_value, ps_style_id,
                                        __LINE__, __func__, __FILE__));
                                    break;
                                }

                                pt_attr = SLIST_NEXT(pt_attr,t_attr_link);
                            }
                        }
                    }
                    pt_ttml_attr = SLIST_NEXT(pt_ttml_attr,t_attr_id_link);
                }       
            }
        }

        if(!SLIST_IS_EMPTY(&gpt_sbtl_ttml_record_global_style->t_attr_id_list))
        {
            SBTL_TTML_ATTR_ID_T* pt_ttml_attr = SLIST_FIRST(&gpt_sbtl_ttml_record_global_style->t_attr_id_list);

            while(pt_ttml_attr)
            {
                if((0 == x_strcmp((const CHAR*)pt_ttml_attr->ps_attr_id_value, (const CHAR*)ps_style_id))
                    || (0 == x_strcmp((const CHAR*)pt_ttml_attr->ps_attr_id_value, (const CHAR*)ps_region_id)))
                {
                    if(!SLIST_IS_EMPTY(&pt_ttml_attr->t_attr_list))
                    {
                        SBTL_TTML_ATTR_T* pt_attr = SLIST_FIRST(&pt_ttml_attr->t_attr_list);
                        
                        DBG_INFO((IPCC_PREFIX" <GLB_ATTR>:   "));
                        
                        while(pt_attr)
                        {
                            /* Process global style of <region> and <style> */
                            if(SBTL_XML_IPCC_ATTR_BEGIN != pt_attr->e_attr
                                && SBTL_XML_IPCC_ATTR_END != pt_attr->e_attr)
                            {
                                i4_ret = _sbtl_ipcc_parse_handle_global_attr(pt_attr, ui1_crnt_wnd);
                                if(IPCCR_OK != i4_ret)
                                {
                                    DBG_ERROR((IPCC_PREFIX" ERR: Handle each global attr failed, i4_ret=%d!   <%d, %s, %s>\n\r", 
                                                    i4_ret, __LINE__, __func__, __FILE__));
                                    IPCC_MEM_FREE(ps_region_id);
                                    IPCC_MEM_FREE(ps_style_id);
                                    return i4_ret;
                                }

                                if(FALSE == gb_is_global_style_set)
                                {
                                    gb_is_global_style_set = TRUE;
                                }
                            }                            
                            
                            pt_attr = SLIST_NEXT(pt_attr,t_attr_link);
                         }
                          
                        DBG_INFO(("<%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                    }
                }

                pt_ttml_attr = SLIST_NEXT(pt_ttml_attr,t_attr_id_link);
            }
        }
    }
    else
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Not found correct region!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        IPCC_MEM_FREE(ps_region_id);
        IPCC_MEM_FREE(ps_style_id);
        return IPCCR_INV_ARG;
    }

#if 1
    if(!SLIST_IS_EMPTY(&pt_elem_p->t_common_attr_list))
    {
        SBTL_TTML_ATTR_T* pt_attr = SLIST_FIRST(&pt_elem_p->t_common_attr_list);

        DBG_INFO((IPCC_PREFIX" <GLB_ATTR>:   "));
        while(pt_attr)
        {
            /* Process the rest global style of <p>. */
             if(SBTL_XML_IPCC_ATTR_BEGIN != pt_attr->e_attr
                 && SBTL_XML_IPCC_ATTR_END != pt_attr->e_attr)
             {
                i4_ret = _sbtl_ipcc_parse_handle_global_attr(pt_attr, ui1_crnt_wnd);
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: Handle each global attr failed, i4_ret=%d!   <%d, %s, %s>\n\r", 
                                    i4_ret, __LINE__, __func__, __FILE__));
                    IPCC_MEM_FREE(ps_region_id);
                    IPCC_MEM_FREE(ps_style_id);
                    return i4_ret;
                }

                if(FALSE == gb_is_global_style_set)
                {
                    gb_is_global_style_set = TRUE;
                }
             }           
            
            pt_attr = SLIST_NEXT(pt_attr,t_attr_link);
        }
        DBG_INFO(("<%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        
    }
#endif  

    i4_ret = _sbtl_ipcc_parse_font_size(&(gt_region_attr[*pui1_crnt_wnd].e_font_family),
                                    &(gt_region_attr[*pui1_crnt_wnd].ui4_font_size),
                                    &(gt_region_attr[*pui1_crnt_wnd].e_font_size_unit),
                                    &(gt_region_attr[*pui1_crnt_wnd].e_font_size));
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(ps_region_id);
        IPCC_MEM_FREE(ps_style_id);
        return i4_ret;
    }

    i4_ret = _sbtl_ipcc_parse_pen_edge_type(gt_region_attr[*pui1_crnt_wnd].ps_pen_edge_type,
                                            &(gt_region_attr[*pui1_crnt_wnd].ui1_pen_edge_type));
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(ps_region_id);
        IPCC_MEM_FREE(ps_style_id);
        return i4_ret;
    }    

    i4_ret = _sbtl_ipcc_parse_set_wnd_attr(pui1_crnt_wnd);
    if(IPCCR_OK != i4_ret)
    {
        IPCC_MEM_FREE(ps_region_id);
        IPCC_MEM_FREE(ps_style_id);
        return i4_ret;
    }

    IPCC_MEM_FREE(ps_region_id);
    IPCC_MEM_FREE(ps_style_id);
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_set_wnd_attr
 *
 * Description: This API is used to copy global style
 *
 * Inputs:  pui1_crnt_wnd       - The current window id
 *
 * Outputs: 
 *
 * Returns: IPCCR_OK                 - Success
 *          IPCCR_INV_ARG            - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_set_wnd_attr(IN UINT8* pui1_crnt_wnd)
{
    if(!pui1_crnt_wnd || (*pui1_crnt_wnd > 7))
    {
        DBG_ERROR((IPCC_PREFIX" ERR: pui1_crnt_wnd is invalid!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    INT32 i4_ret = IPCCR_OK;
    UINT8 ui1_crnt_wnd = *pui1_crnt_wnd;

    x_strcpy(gat_wgt_wnd_attr[ui1_crnt_wnd].ps_region_id, (const CHAR*)gt_region_attr[ui1_crnt_wnd].ps_region_id);

    /* t_pen_color */
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_color.b_is_flash_bk = FALSE;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_color.b_is_flash_fg = FALSE;
    i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_wnd].ps_pen_bg_color,
                                    &gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_color.t_color_bk);
    if(IPCCR_OK != i4_ret)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Parse pen bg color failed!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return i4_ret;
    }
    i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_wnd].ps_pen_fg_color,
                                    &gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_color.t_color_text);
    if(IPCCR_OK != i4_ret)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Parse pen fg color failed!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return i4_ret;
    }
    i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_wnd].ps_pen_edge_color,
                                    &gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_color.t_color_edge);
    if(IPCCR_OK != i4_ret)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Parse pen edge color failed!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return i4_ret;
    }


    /* t_pen_font */
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_font.ui1_offset = WGL_CC_TEXT_OFFSET_NORMAL;
    x_strcpy(gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_font.t_font.s_font_name, 
             (const CHAR*)gt_region_attr[ui1_crnt_wnd].ps_font_name);
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_font.t_font.e_cmap = FE_CMAP_ENC_UNICODE;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_font.t_font.e_size = gt_region_attr[ui1_crnt_wnd].e_font_size;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_font.t_font.e_style = gt_region_attr[ui1_crnt_wnd].e_font_style;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_pen_font.t_font.ui1_edge_type = gt_region_attr[ui1_crnt_wnd].ui1_pen_edge_type;


    /* t_win_anchor */
    if(0 == gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_h
        && 0 == gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_v)
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_anchor.ui1_anchor = 0;
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_h = 0;
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_v = 0;
    }
    else
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_anchor = gt_region_attr[ui1_crnt_wnd].t_win_anchor;
    }
    

    /* t_win_size */
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_size = gt_region_attr[ui1_crnt_wnd].t_win_size;
    DBG_INFO(("\n{IPCC} INFO: (Wnd_%u: r_c=%u, c_c=%u)!   <%d, %s, %s>\n\r", 
                    ui1_crnt_wnd, gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_size.ui2_row_count,
                    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_size.ui2_col_count,
                    __LINE__, __func__, __FILE__));


    /* t_win_attr */
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.b_is_flash_bk = FALSE;
    i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_wnd].ps_win_bg_color,
                                    &gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_bk);
    if(IPCCR_OK != i4_ret)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Parse win bg color failed!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return i4_ret;
    }

    if(SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui1_display_effect = WGL_CC_DISPLAY_EFFECT_SNAP;
    }
    else if(SBTL_IPCC_CAPTION_MODE_PAINT_ON == ge_crnt_caption_mode
            || SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui1_display_effect = WGL_CC_DISPLAY_EFFECT_WIPE;
    }

    /* It's not necessary to support the following two attributes which were defined in 
     * page 8 of Spec SMPTE rp 2052-11:201x 
     */
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui1_border_type = WGL_CC_BORDER_TYPE_NONE;
    //gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_border = CC_COLOR_TRANSPARENT;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_border.a = 0x00;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_border.u1.r = 0x00;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_border.u2.g = 0x00;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_border.u3.b = 0x00;
    
    
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui1_display_effect_dir = WGL_CC_DISPLAY_EFFECT_DIR_LEFT_TO_RIGHT;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui1_print_dir = WGL_CC_PRINT_DIR_LEFT_TO_RIGHT;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui1_scroll_dir = WGL_CC_SCROLL_DIR_LEFT_TO_RIGHT;
    gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui4_display_effect_duration = (UINT32)(2 * 500);

    i4_ret = _sbtl_ipcc_parse_text_align(&gt_region_attr[ui1_crnt_wnd].e_text_align,
                                         &gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui1_justify);
    
    if(IPCCR_OK != i4_ret)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Parse text align failed!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return i4_ret;
    }

    if(0 == x_strcmp("noWrap", gt_region_attr[ui1_crnt_wnd].ps_win_word_wrap))
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui2_win_attr &= (~WGL_CC_WIN_ATTR_WORD_WRAP);
    }
    else if(0 == x_strcmp("wrap", gt_region_attr[ui1_crnt_wnd].ps_win_word_wrap))
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.ui2_win_attr |= WGL_CC_WIN_ATTR_WORD_WRAP;
    }

    if(NULL != x_strstr("1.0", (const CHAR*)gt_region_attr[ui1_crnt_wnd].ps_win_opacity))
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_bk.a = (UINT8)0xFF;
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.b_is_flash_bk = FALSE;
    }
    else if(NULL != x_strstr("0.5", (const CHAR*)gt_region_attr[ui1_crnt_wnd].ps_win_opacity))
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_bk.a = (UINT8)0x7F;
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.b_is_flash_bk = FALSE;
    }
    else if(NULL != x_strstr("0.0", (const CHAR*)gt_region_attr[ui1_crnt_wnd].ps_win_opacity))
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_bk.a = (UINT8)0x00;
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.b_is_flash_bk = FALSE;
    }
    else//default value of opacity
    {
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.t_color_bk.a = (UINT8)0x00;
        gat_wgt_wnd_attr[ui1_crnt_wnd].t_win_attr.b_is_flash_bk = FALSE;
    }


    /*Text tag*/
    i4_ret = _sbtl_ipcc_parse_text_tag(gt_region_attr[ui1_crnt_wnd].ps_text_tag,
                                       &gat_wgt_wnd_attr[ui1_crnt_wnd].aui1_text_tag);
    if(IPCCR_OK != i4_ret)
    {
        return i4_ret;
    }

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_handle_global_attr
 *
 * Description: This API is used to convert global attr.
 *
 * Inputs:  pui1_crnt_wnd       - The current window id
 *          pt_attr             - The global attr
 *
 * Outputs: 
 *
 * Returns: IPCCR_OK                 - Success
 *          IPCCR_INV_ARG            - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_handle_global_attr
(
    IN SBTL_TTML_ATTR_T *pt_attr, 
    IN UINT8 ui1_crnt_wnd
)
{
    if(!pt_attr)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: pt_attr is invalid!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    if(ui1_crnt_wnd > 7)
    {
        DBG_ERROR((IPCC_PREFIX" ERR: ui1_crnt_wnd is %u, greater than 7!   <%d, %s, %s>\n\r", 
                    ui1_crnt_wnd, __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG;
    }

    INT32 i4_ret = IPCCR_OK;

    switch(pt_attr->e_attr)
    {
        /* Set pen color */
        case SBTL_XML_IPCC_ATTR_COLOR:
            x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_pen_fg_color, (const CHAR*)pt_attr->u.ps_value);
            DBG_INFO(("((%u)GLB_PEN_CLR: %s), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].ps_pen_fg_color));
            break;

        /* Set pen attr */
        case SBTL_XML_IPCC_ATTR_FONTFAMILY: 
            gt_region_attr[ui1_crnt_wnd].e_font_family = pt_attr->u.e_font_family;
            DBG_INFO(("((%u)GLB_FONT_FAM: %d), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].e_font_family));
            break;

        case SBTL_XML_IPCC_ATTR_FONT_STYLE:
            {
                gt_region_attr[ui1_crnt_wnd].e_font_style = pt_attr->u.e_font_style;
                DBG_INFO(("((%u)GLB_FONT_STYLE: %d), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].e_font_style));
            }
            break;


        case SBTL_XML_IPCC_ATTR_FONT_WEIGHT:
            {
                gt_region_attr[ui1_crnt_wnd].e_font_weight = pt_attr->u.e_font_weight;
                DBG_INFO(("((%u)GLB_FONT_WGHT: %d), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].e_font_weight));
            }
            break;                            

        case SBTL_XML_IPCC_ATTR_TEXT_DECORATION:
            {
                gt_region_attr[ui1_crnt_wnd].e_text_decoration = pt_attr->u.e_text_decoration;
                DBG_INFO(("((%u)GLB_TXT_DEC: %d), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].e_text_decoration));                
            }
            break;

        case SBTL_XML_IPCC_ATTR_FONTSIZE:
            {
                CHAR* ps_font_size = (CHAR *)x_mem_alloc(sizeof(CHAR)* (1 + x_strlen((const CHAR*)pt_attr->u.ps_value)));
                if(NULL == ps_font_size)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                    return IPCCR_INSUFFICIENT_SIZE;
                }

                x_strcpy(ps_font_size, (const CHAR*)pt_attr->u.ps_value);
                
                CHAR* ps_rslt = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                if(NULL == ps_rslt)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                    IPCC_MEM_FREE(ps_font_size);
                    return IPCCR_INSUFFICIENT_SIZE;
                }

                TTML_LENGTH_UNIT_TYPE e_font_size_unit = TTML_LENGTH_UNIT_TYPE_UNKNOW;
                i4_ret = _sbtl_ipcc_parse_detect_length_unit(ps_font_size, &e_font_size_unit);
                if(IPCCR_OK != i4_ret)
                {
                    IPCC_MEM_FREE(ps_font_size);
                    IPCC_MEM_FREE(ps_rslt);
                    return i4_ret;
                }

                if(TTML_LENGTH_UNIT_TYPE_PX == e_font_size_unit)
                {
                    i4_ret = x_sbtl_ipcc_str_split("px", ps_font_size, &ps_rslt, 1, NULL);
                }
                else if(TTML_LENGTH_UNIT_TYPE_PRCNT == e_font_size_unit)
                {
                    i4_ret = x_sbtl_ipcc_str_split("%", ps_font_size, &ps_rslt, 1, NULL); 
                }
                else if(TTML_LENGTH_UNIT_TYPE_C == e_font_size_unit)
                {
                    i4_ret = x_sbtl_ipcc_str_split("c", ps_font_size, &ps_rslt, 1, NULL);
                }
                
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR(("\n{IPCC} ERR: i4_ret=%d! src_str=%s, dst_str=%s  <%d, %s, %s>\n\r", 
                            i4_ret, ps_font_size, ps_rslt,
                            __LINE__, __func__, __FILE__));
                    IPCC_MEM_FREE(ps_font_size);
                    IPCC_MEM_FREE(ps_rslt);
                    return i4_ret;
                }
                   
                UINT32 ui4_crnt_font_size = (UINT32)x_strtoull((const CHAR*)ps_rslt, NULL, 10);
                gt_region_attr[ui1_crnt_wnd].e_font_size_unit = e_font_size_unit;
                gt_region_attr[ui1_crnt_wnd].ui4_font_size = ui4_crnt_font_size;
                IPCC_MEM_FREE(ps_font_size);
                IPCC_MEM_FREE(ps_rslt);

                DBG_INFO(("((%u)GLB_FONT_SIZE: %u), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].ui4_font_size));
                DBG_INFO(("((%u)GLB_FONT_SIZE_U: %d), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].e_font_size_unit));

            }
            break;


        /* Set window attr */
        case SBTL_XML_IPCC_ATTR_TEXTALIGN:
            {
                gt_region_attr[ui1_crnt_wnd].e_text_align = pt_attr->u.e_text_align;
                DBG_INFO(("((%u)GLB_TXT_ALIGN: %d), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].e_text_align));
            }
            break;

                        
        /* Set win anchor */
        case SBTL_XML_IPCC_ATTR_ORIGIN:
            DBG_INFO((IPCC_PREFIX" INFO: ge_crnt_caption_mode=%d, gb_roll_up_1st_set =%d!   <%d, %s, %s>\n\r", 
                             ge_crnt_caption_mode, 
                             gb_roll_up_1st_set, 
                             __LINE__, __func__, __FILE__));
            if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode
                && TRUE == gb_roll_up_1st_set)
            {
                if(0 != x_strcmp((const CHAR*)gps_roll_up_base_row, (const CHAR*)pt_attr->u.ps_value))
                {
                    gb_roll_up_ignore_crnt_p = TRUE;
                }
                
                break;
            }
            else
            {
                TTML_LENGTH_UNIT_TYPE e_length_unit = TTML_LENGTH_UNIT_TYPE_UNKNOW;
                CHAR* ps_origin_x = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                if(NULL == ps_origin_x)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                    return IPCCR_INSUFFICIENT_SIZE;
                }
                
                CHAR* ps_origin_y = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                if(NULL == ps_origin_y)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                    IPCC_MEM_FREE(ps_origin_x);
                    return IPCCR_INSUFFICIENT_SIZE;
                }

                if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode
                    && FALSE == gb_roll_up_1st_set)
                {
                    x_memset(gps_roll_up_base_row, 0, (SIZE_T)IPCC_TTS_ORIGIN_LENGTH);
                    x_strcpy(gps_roll_up_base_row, (const CHAR*)pt_attr->u.ps_value);
                }
                
                i4_ret = _sbtl_ipcc_parse_abstract_value(pt_attr->u.ps_value, &e_length_unit, ps_origin_y, ps_origin_x);
                if(IPCCR_OK != i4_ret)
                {
                    IPCC_MEM_FREE(ps_origin_x);
                    IPCC_MEM_FREE(ps_origin_y);
                    return i4_ret;        
                }

                SBTL_IPCC_ARG_TYPE e_arg_type_x = SBTL_IPCC_ARG_TYPE_INT;
                SBTL_IPCC_ARG_TYPE e_arg_type_y = SBTL_IPCC_ARG_TYPE_INT;
                FLOAT f_row = 0.0;
                FLOAT f_col = 0.0;

                if(NULL != x_strstr((const CHAR*)ps_origin_x, "."))
                {
                    e_arg_type_x = SBTL_IPCC_ARG_TYPE_FLOAT;
                }

                if(NULL != x_strstr((const CHAR*)ps_origin_y, "."))
                {
                    e_arg_type_y = SBTL_IPCC_ARG_TYPE_FLOAT;
                }   

                
                if(SBTL_IPCC_ARG_TYPE_INT == e_arg_type_x && 
                   SBTL_IPCC_ARG_TYPE_INT == e_arg_type_y)
                {
                    /* The tts:origin may map to the anchor of window in DTV-CC */
                    UINT32 ui4_row = (UINT32)x_strtoull((const CHAR*)ps_origin_y, NULL, 10);
                    UINT32 ui4_col = (UINT32)x_strtoull((const CHAR*)ps_origin_x, NULL, 10);

                    f_row = (FLOAT)ui4_row;
                    f_col = (FLOAT)ui4_col;
                }  
                else if(SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_x &&
                        SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_y)
                {
                    /* The tts:origin may map to the anchor of window in DTV-CC */
                    f_row = 0.0;
                    f_col = 0.0;
                    
                    i4_ret = x_sbtl_ipcc_str_to_float(ps_origin_y, &f_row);
                    if(IPCCR_OK != i4_ret)
                    {
                        IPCC_MEM_FREE(ps_origin_x);
                        IPCC_MEM_FREE(ps_origin_y);
                        return i4_ret;
                    }

                    i4_ret = x_sbtl_ipcc_str_to_float(ps_origin_x, &f_col);
                    if(IPCCR_OK != i4_ret)
                    {
                        IPCC_MEM_FREE(ps_origin_x);
                        IPCC_MEM_FREE(ps_origin_y);
                        return i4_ret;
                    }
                }
                else 
                {                    
                    if(SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_y)
                    {
                        f_col = (FLOAT)((UINT32)x_strtoull((const CHAR*)ps_origin_x, NULL, 10));
                        i4_ret = x_sbtl_ipcc_str_to_float(ps_origin_y, &f_row);
                        if(IPCCR_OK != i4_ret)
                        {
                            IPCC_MEM_FREE(ps_origin_y);
                            return i4_ret;
                        }
                    }

                    if(SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_x)
                    {
                        f_row = (FLOAT)((UINT32)x_strtoull((const CHAR*)ps_origin_y, NULL, 10));
                        i4_ret = x_sbtl_ipcc_str_to_float(ps_origin_x, &f_col);
                        if(IPCCR_OK != i4_ret)
                        {
                            IPCC_MEM_FREE(ps_origin_x);
                            return i4_ret;
                        }
                    }
                   
                }

                if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
                {
                    gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui1_anchor = 6;
                }
                else
                {
                    gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui1_anchor = 0;
                }

                i4_ret = _sbtl_ipcc_parse_recalculate_window_anchor(&e_length_unit, &f_col, &f_row, 
                                        &(gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_v), 
                                        &(gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_h));

                DBG_INFO(("((%u)GLB_WND_ANC: x=%s, y=%s; v=%u, h=%u) ",  
                            ui1_crnt_wnd, ps_origin_x, ps_origin_y, 
                            gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_v,
                            gt_region_attr[ui1_crnt_wnd].t_win_anchor.ui2_anchor_h));



                if(IPCCR_OK != i4_ret)
                {
                    IPCC_MEM_FREE(ps_origin_x);
                    IPCC_MEM_FREE(ps_origin_y);
                    return i4_ret;
                }

                IPCC_MEM_FREE(ps_origin_x);
                IPCC_MEM_FREE(ps_origin_y);

                /* To define window size for roll-up */
                if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
                {
                    CHAR* ps_match[3] = {"rollup2", "rollup3", "rollup4"};
                    INT32 i4_i = 0;        

                    for(i4_i = 0; i4_i < 3; ++i4_i)
                    {
                        if(0 == x_strcmp((const CHAR*)gt_region_attr[ui1_crnt_wnd].ps_region_id, (const CHAR*)ps_match[i4_i]))
                        {
                            break;
                        }
                    }

                    i4_i = i4_i + 2;
                    gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_row_count = i4_i;
                    gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_col_count = 32;
                    DBG_INFO((IPCC_PREFIX" INFO: region id=%s, ui2_row_count =%d!   <%d, %s, %s>\n\r", 
                             gt_region_attr[ui1_crnt_wnd].ps_region_id, 
                             gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_row_count, 
                             __LINE__, __func__, __FILE__));
                    
                }
            }     
                break;   


        /* Set win size */
        /* tts:extent  map to row count and column count in DTVCC*/
        case SBTL_XML_IPCC_ATTR_FONT_EXTENT:
            {
                if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode && 
                   TRUE == gb_roll_up_1st_set)
                {
                    break;
                }
                
                TTML_LENGTH_UNIT_TYPE e_win_size_length_unit = TTML_LENGTH_UNIT_TYPE_UNKNOW;
                CHAR* ps_win_size_row = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                if(NULL == ps_win_size_row)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                    return IPCCR_INSUFFICIENT_SIZE;
                }
                
                CHAR* ps_win_size_col = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                if(NULL == ps_win_size_col)
                {
                    DBG_ERROR((IPCC_PREFIX" ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                    IPCC_MEM_FREE(ps_win_size_row);
                    return IPCCR_INSUFFICIENT_SIZE;
                }
                
                i4_ret = _sbtl_ipcc_parse_abstract_value(pt_attr->u.ps_value, &e_win_size_length_unit, ps_win_size_row, ps_win_size_col);
                if(IPCCR_OK != i4_ret)
                {
                    IPCC_MEM_FREE(ps_win_size_row);
                    IPCC_MEM_FREE(ps_win_size_col);
                    return i4_ret;        
                }

                
                SBTL_IPCC_ARG_TYPE e_arg_type_x = SBTL_IPCC_ARG_TYPE_INT;
                SBTL_IPCC_ARG_TYPE e_arg_type_y = SBTL_IPCC_ARG_TYPE_INT;

                if(NULL != x_strstr((const CHAR*)ps_win_size_col, "."))
                {
                    e_arg_type_x = SBTL_IPCC_ARG_TYPE_FLOAT;
                }

                if(NULL != x_strstr((const CHAR*)ps_win_size_row, "."))
                {
                    e_arg_type_y = SBTL_IPCC_ARG_TYPE_FLOAT;
                }   

                
                if(SBTL_IPCC_ARG_TYPE_INT == e_arg_type_x
                    && SBTL_IPCC_ARG_TYPE_INT == e_arg_type_y)
                {
                    UINT32 ui4_row = (UINT32)x_strtoull((const CHAR*)ps_win_size_row, NULL, 10);
                    UINT32 ui4_col = (UINT32)x_strtoull((const CHAR*)ps_win_size_col, NULL, 10);

                    i4_ret = _sbtl_ipcc_parse_window_size_for_integer(&e_win_size_length_unit, &ui4_col, &ui4_row, 
                                                                    &(gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_row_count),
                                                                    &(gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_col_count));

                }  
                else if(SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_x
                        && SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_y)
                {
                    FLOAT f_row = 0.0;
                    FLOAT f_col = 0.0;
                    
                    i4_ret = x_sbtl_ipcc_str_to_float(ps_win_size_row, &f_row);
                    if(IPCCR_OK != i4_ret)
                    {
                        IPCC_MEM_FREE(ps_win_size_row);
                        IPCC_MEM_FREE(ps_win_size_col);
                        return i4_ret;
                    }

                    i4_ret = x_sbtl_ipcc_str_to_float(ps_win_size_col, &f_col);
                    if(IPCCR_OK != i4_ret)
                    {
                        IPCC_MEM_FREE(ps_win_size_row);
                        IPCC_MEM_FREE(ps_win_size_col);
                        return i4_ret;
                    }
                    
                    i4_ret = _sbtl_ipcc_parse_window_size_for_float(&e_win_size_length_unit, &f_col, &f_row, 
                                                                    &(gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_row_count),
                                                                    &(gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_col_count));
                }
                else 
                {              
                    FLOAT f_col = 0.0;
                    FLOAT f_row = 0.0;
                    
                    if(SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_y)
                    {
                        f_col = (FLOAT)((UINT32)x_strtoull((const CHAR*)ps_win_size_col, NULL, 10));
                        f_row = 0.0;
                        i4_ret = x_sbtl_ipcc_str_to_float(ps_win_size_row, &f_row);
                        if(IPCCR_OK != i4_ret)
                        {
                            IPCC_MEM_FREE(ps_win_size_row);
                            return i4_ret;
                        }
                    }

                    if(SBTL_IPCC_ARG_TYPE_FLOAT == e_arg_type_x)
                    {
                        f_row = (FLOAT)((UINT32)x_strtoull((const CHAR*)ps_win_size_row, NULL, 10));
                        f_col = 0.0;
                        i4_ret = x_sbtl_ipcc_str_to_float(ps_win_size_col, &f_col);
                        if(IPCCR_OK != i4_ret)
                        {
                            IPCC_MEM_FREE(ps_win_size_col);
                            return i4_ret;
                        }
                    }

                    i4_ret = _sbtl_ipcc_parse_window_size_for_float(&e_win_size_length_unit, &f_col, &f_row, 
                                                                    &(gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_row_count),
                                                                    &(gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_col_count));
                   
                }

                DBG_INFO(("((%u)GLB_WND_SIZE: r_s=%s, c_s=%s; r_c=%u, c_c=%u), ", 
                        ui1_crnt_wnd, 
                        ps_win_size_row,
                        ps_win_size_col,
                        gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_row_count,
                        gt_region_attr[ui1_crnt_wnd].t_win_size.ui2_col_count));
                
                if(IPCCR_OK != i4_ret)
                {
                    IPCC_MEM_FREE(ps_win_size_row);
                    IPCC_MEM_FREE(ps_win_size_col);
                    return i4_ret;
                }

                IPCC_MEM_FREE(ps_win_size_row);
                IPCC_MEM_FREE(ps_win_size_col);
            }
             
            break; 

        /* Get <p> bgcolor and set ir for window bgcolor of DTV-CC */
        case SBTL_XML_IPCC_ATTR_BGCOLOR:
            {
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_win_bg_color, (const CHAR*)pt_attr->u.ps_value);
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_pen_bg_color, (const CHAR*)pt_attr->u.ps_value);

                DBG_INFO(("((%u)GLB_WND_BG_CLR: wgc=%s, pbc=%s), ", 
                        ui1_crnt_wnd, 
                        gt_region_attr[ui1_crnt_wnd].ps_win_bg_color,
                        gt_region_attr[ui1_crnt_wnd].ps_pen_bg_color));
            }
            break;

        /* Get the crnt window opacity */
        case SBTL_XML_IPCC_ATTR_WINDOW_OPACITY:
            {
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_win_opacity, (const CHAR*)pt_attr->u.ps_value);
                DBG_INFO(("((%u)GLB_WND_OPCTY: %s), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].ps_win_opacity));
            }
            break;

        /* Get the crnt pen edge color */
        case SBTL_XML_IPCC_ATTR_TEXT_OUTLINE:
            {
                /* Step 1: Detect ps_value if it contains "%" */

                /* If there is no "%" */
                if(NULL == x_strstr((const CHAR*)pt_attr->u.ps_value, "%"))
                {
                    x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_pen_edge_color, (const CHAR*)pt_attr->u.ps_value);
                    x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_pen_edge_type, " ");
                }
                /* If there is "%" */
                else
                {
                    /* Step 2: Split pen edge color from ps_value */
                    CHAR* ps_whitespace =" ";
                    CHAR* ps_pen_edge_color = gt_region_attr[ui1_crnt_wnd].ps_pen_edge_color;
                    i4_ret = x_sbtl_ipcc_str_split( ps_whitespace, 
                                                    pt_attr->u.ps_value, 
                                                    &ps_pen_edge_color, 
                                                    1, 
                                                    gt_region_attr[ui1_crnt_wnd].ps_pen_edge_type);
                    if(IPCCR_OK != i4_ret)
                    {
                        return IPCCR_INV_ARG;
                    }

                }

                DBG_INFO(("((%u)GLB_TXT_OL: %s), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].ps_pen_edge_color));
            }
            break;

        /* Get the crnt window word wrap */
        case SBTL_XML_IPCC_ATTR_WRAP_OPTION:
            {
                x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_win_word_wrap, (const CHAR*)pt_attr->u.ps_value);
                DBG_INFO(("((%u)GLB_WW: %s), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].ps_win_word_wrap));
            }
            break;

        /* Get xml:space */
        case SBTL_XML_IPCC_ATTR_WHITESPACE:
        {            
            if(SBTL_TTML_ATTR_WHITESPACE_DEFAULT == pt_attr->u.e_whitespace)
            {
                gt_region_attr[ui1_crnt_wnd].b_whitespace_preserve = FALSE;
            }
            else if( SBTL_TTML_ATTR_WHITESPACE_PRESERVE == pt_attr->u.e_whitespace)
            {
                gt_region_attr[ui1_crnt_wnd].b_whitespace_preserve = TRUE;
            }

            DBG_INFO(("((%u)GLB_WS: %d), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].b_whitespace_preserve));
            
        }
            break;  

        /* Get ttm:role*/
        case SBTL_XML_IPCC_ATTR_ROLE:
        {
            x_strcpy(gt_region_attr[ui1_crnt_wnd].ps_text_tag, (const CHAR*)pt_attr->u.ps_value);
            DBG_INFO(("((%u)GLB_RL: %s), ", ui1_crnt_wnd, gt_region_attr[ui1_crnt_wnd].ps_text_tag));
        }
            break;

        default:
            break;
    }    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_text_tag
 *
 * Description: This API is used to parse text tag of tts:role
 *
 * Inputs:  ps_role                 - tts:role
 *
 * Outputs: pui1_text_tag           - Text tag name
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_text_tag
(
    IN CHAR* ps_role, 
    OUT UINT8* pui1_text_tag
)
{
    IPCC_CHECK_NULL(ps_role,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui1_text_tag,IPCCR_INV_ARG);

    if(0 == x_strcmp("dialog", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_DIALOG;
    }
    else if(0 == x_strcmp("source", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_SRC_SPEAKER_ID;
    }
    else if(0 == x_strcmp("reproduction", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_ELEC_REPROD_VOICE;
    }
    else if(0 == x_strcmp("x-smpte-voiceover", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_DIALOG_NOT_PRIM;
    }
    else if(0 == x_strcmp("x-smpte-voiceover", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_VOICEOVER;
    }
    else if(0 == x_strcmp("caption", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_AUDIBLE_TRANSLATE;
    }
    else if(0 == x_strcmp("transcription", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_SUBTITLE_TRANSLATE;
    }
    else if(0 == x_strcmp("quality", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_VOICE_QA_DESC;
    }
    else if(0 == x_strcmp("lyrics", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_SONG_LYRICS;
    }
    else if(0 == x_strcmp("sound", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_SND_EFF_DESC;
    }
    else if(0 == x_strcmp("x-smpte-musical-score", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_MUSICAL_SCORE_DESC;
    }
    else if(0 == x_strcmp("expletive", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_EXPLETIVE;
    }
    else if(0 == x_strcmp("suppressed", (const CHAR*)ps_role))
    {
        *pui1_text_tag = CAP_CMD_TEXT_TAG_NOT_TO_BE_DISP;
    }
    else
    {
        DBG_ERROR((IPCC_PREFIX" ERR: Not support this text tag: %s!   <%d, %s, %s>\n\r", 
            ps_role, __LINE__, __func__, __FILE__));
        return IPCCR_NOT_SUPPORT;
    }

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_recalculate_window_anchor
 *
 * Description: This API is used to parse window anchor and recalculate.
 *
 * Inputs:  pe_length_unit          - Length unit
 *          pui4_origin_width       - the width of tts:origin
 *          pui4_origin_height      - the height of tts:origin   
 *
 * Outputs: pui4_window_anchor_v    - 
 *          pui4_window_anchor_h    -
 *
 * Returns: IPCCR_OK          -Success
 *          IPCCR_INV_ARG     - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_recalculate_window_anchor
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN FLOAT* pf_origin_width,
    IN FLOAT* pf_origin_height,
    OUT UINT16* pui2_window_anchor_v, 
    OUT UINT16* pui2_window_anchor_h
)

{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_length_unit, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pf_origin_width, IPCCR_INV_ARG);  
    IPCC_CHECK_NULL(pf_origin_height, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui2_window_anchor_v, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui2_window_anchor_h, IPCCR_INV_ARG);


    /* Need to know the current video src */

    FLOAT f_recal_origin_width = 0.0; //X
    FLOAT f_recal_origin_height = 0.0;  //Y

    if(TRUE == gb_apply_convert_rule)
    {
        if(SBTL_IPCC_FILE_TYPE_SENVU == gt_video_info.e_file_type)
        {
            switch(gt_aspect_ratio)
            {
                case SM_CC_ASPECT_RATIO_4_3:
                {
                    f_recal_origin_width = (*pf_origin_width - 12.5) * IPCC_SCALE_UP_RATIO_SD_X + IPCC_SCALE_UP_OFFSET_SD_A;
                    f_recal_origin_height = (*pf_origin_height - 12.5) * IPCC_SCALE_UP_RATIO_SD_Y + IPCC_SCALE_UP_OFFSET_SD_B;
                }
                    break;

                case SM_CC_ASPECT_RATIO_16_9:
                {
                    f_recal_origin_width = (*pf_origin_width - 12.5) * IPCC_SCALE_UP_RATIO_HD_X + IPCC_SCALE_UP_OFFSET_HD_A;
                    f_recal_origin_height = (*pf_origin_height - 12.5) * IPCC_SCALE_UP_RATIO_HD_Y + IPCC_SCALE_UP_OFFSET_HD_B;
                }
                    break;

                default:
                    break;
            }
        }
        else if(SBTL_IPCC_FILE_TYPE_CRACKLE == gt_video_info.e_file_type)
        {
            f_recal_origin_width = *pf_origin_width;
            f_recal_origin_height = *pf_origin_height;
        }    
    }
    else
    {
        if(SBTL_IPCC_FILE_TYPE_SENVU == gt_video_info.e_file_type)
        {
            f_recal_origin_width = *pf_origin_width;
            f_recal_origin_height = *pf_origin_height;
        }
        else if(SBTL_IPCC_FILE_TYPE_CRACKLE == gt_video_info.e_file_type)
        {
            f_recal_origin_width = *pf_origin_width;
            f_recal_origin_height = *pf_origin_height;
        }  
    }


    INT32 i4_ret = _sbtl_ipcc_parse_window_anchor(pe_length_unit, 
                                                  &f_recal_origin_width,
                                                  &f_recal_origin_height,
                                                  pui2_window_anchor_v,
                                                  pui2_window_anchor_h);
    IPCC_CHECK_VALID(i4_ret,IPCCR_OK);

    if(FALSE  == gb_roll_up_1st_set && 
       SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
    {
        gb_roll_up_1st_set = TRUE;
    }
    else if(TRUE  == gb_roll_up_1st_set && 
            SBTL_IPCC_CAPTION_MODE_ROLL_UP != ge_crnt_caption_mode)
    {
        gb_roll_up_1st_set = FALSE;
    }
    
    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_window_anchor
 *
 * Description: This API is used to calculate the window anchor.
 *
 * Inputs:  pe_length_unit          - Length unit
 *          pui4_origin_width       - the width of tts:origin
 *          pui4_origin_height      - the height of tts:origin   
 *
 * Outputs: pui4_window_anchor_v    - 
 *          pui4_window_anchor_h    -
 *
 * Returns: IPCCR_OK          -Success
 *          IPCCR_INV_ARG     - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_window_anchor
(
    IN TTML_LENGTH_UNIT_TYPE* pe_length_unit,
    IN FLOAT* pf_origin_width,
    IN FLOAT* pf_origin_height,
    OUT UINT16* pui2_window_anchor_v, 
    OUT UINT16* pui2_window_anchor_h
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pe_length_unit, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pf_origin_width, IPCCR_INV_ARG);  
    IPCC_CHECK_NULL(pf_origin_height, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui2_window_anchor_v, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui2_window_anchor_h, IPCCR_INV_ARG);


    switch(*pe_length_unit)
    {
        case TTML_LENGTH_UNIT_TYPE_PRCNT:
        {
            FLOAT f_h = (FLOAT)gui4_screen_width * 0.9;
            FLOAT f_v = (FLOAT)gui4_screen_height* 0.9;
            
            FLOAT f_canvas_ratio_h = 
                ((FLOAT)gui4_screen_width * (*pf_origin_width-5.0) / 100.0) / f_h;
            FLOAT f_canvas_ratio_v = 
                ((FLOAT)gui4_screen_height * (*pf_origin_height-5.0) / 100.0) / f_v;

            FLOAT f_tmp_h = ((FLOAT)pt_sbtl_ipcc_wgt->ui4_canvas_w * f_canvas_ratio_h);
            FLOAT f_tmp_v = ((FLOAT)pt_sbtl_ipcc_wgt->ui4_canvas_h * f_canvas_ratio_v);

            if(f_tmp_h < 0)
            {
                f_tmp_h = 0;
            }

            if(f_tmp_v < 0)
            {
                f_tmp_v = 0;
            }
            
            *pui2_window_anchor_h = (UINT16)f_tmp_h;
            *pui2_window_anchor_v = (UINT16)f_tmp_v;

        }
            break;


        case TTML_LENGTH_UNIT_TYPE_PX:
        {
            *pui2_window_anchor_h = (UINT16)pt_sbtl_ipcc_wgt->ui4_canvas_w;
            *pui2_window_anchor_v = (UINT16)pt_sbtl_ipcc_wgt->ui4_canvas_h;
        }
            break;

        default:
            return IPCCR_INV_ARG;
            break;
    }
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_detect_length_unit
 *
 * Description: This API is used to detect the unit of length of tts:origin
 *
 * Inputs:  ps_str                  - String which will be detected
 *          pe_origin_type          - The detected origin type
 *
 * Outputs: pe_origin_type         - The detected origin type
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid para
 *          IPCCR_NOT_FOUND       - Not matched
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_detect_length_unit
(
    IN CHAR* ps_str, 
    IN OUT TTML_LENGTH_UNIT_TYPE* pe_origin_type
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(ps_str,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_origin_type,IPCCR_INV_ARG);
    
    INT32 i4_ret = IPCCR_OK;
    CHAR* ps_match[IPCC_LENGTH_UNIT_COUNT] = {"px", "%", "c"};
    UINT8 ui1_index = 0;
    BOOL b_flag = FALSE;

    for(ui1_index = 0; ui1_index < IPCC_LENGTH_UNIT_COUNT; ++ui1_index)
    {
        if(NULL != x_strstr((const CHAR*)ps_str, (const CHAR*)ps_match[ui1_index]))
        {
            *pe_origin_type = (TTML_LENGTH_UNIT_TYPE)(ui1_index + 1);
            b_flag = TRUE;
            
            break;
        }
    }

    if(FALSE == b_flag)
    {
        i4_ret = IPCCR_NOT_FOUND;
        DBG_INFO((IPCC_PREFIX" INFO: Not matched!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
    }
    
    return i4_ret;    
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_detect_caption_mode
 *
 * Description: This API is used to detect the unit of length of tts:origin
 *
 * Inputs:  ps_src_str               - String which will be detected
 *          pe_caption_mode          - The detected caption mode
 *
 * Outputs: pe_caption_mode         - The detected caption mode
 *
 * Returns: IPCCR_OK                 - Success
 *          IPCCR_INV_ARG         - Invalid para
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_parse_detect_caption_mode
(
    IN  CHAR* ps_src_str, 
    OUT SBTL_IPCC_CAPTION_MODE* pe_caption_mode
)
{
    /* Check arguments */
    IPCC_CHECK_NULL(ps_src_str,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pe_caption_mode,IPCCR_INV_ARG);
  
    CHAR* ps_match[IPCC_CAPTION_MODE_COUNT] = {"pop", "paint", "rollup"};
    UINT8 ui1_index = 0;
    BOOL  b_is_caption_mode_set = FALSE;
    SBTL_IPCC_CAPTION_MODE e_tmp_cap_mod = SBTL_IPCC_CAPTION_MODE_POP_ON;

    e_tmp_cap_mod = ge_crnt_caption_mode;
    
    
    for(ui1_index = 0; ui1_index < IPCC_CAPTION_MODE_COUNT; ++ui1_index)
    {
        if(NULL != x_strstr((const CHAR*)ps_src_str, (const CHAR*)ps_match[ui1_index]))
        {
            *pe_caption_mode = (SBTL_IPCC_CAPTION_MODE)ui1_index;
            b_is_caption_mode_set = TRUE;

            if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == (SBTL_IPCC_CAPTION_MODE)(*pe_caption_mode))
            {
                if(SBTL_IPCC_FILE_MODE_PRESERVED == gt_video_info.e_file_mode)
                {
                    *pe_caption_mode = SBTL_IPCC_CAPTION_MODE_ROLL_UP;

                #if 1
                    if(0 != x_strcmp((const CHAR*)gps_roll_up_last_id, (const CHAR*)ps_src_str))
                    {
                        x_strcpy(gps_roll_up_last_id, (const CHAR*)ps_src_str);
                    }
                #endif    
                }
                else if(SBTL_IPCC_FILE_MODE_ENHANCED == gt_video_info.e_file_mode)
                {
                    *pe_caption_mode = SBTL_IPCC_CAPTION_MODE_POP_ON;
                }                
            }

            
            ge_last_caption_mode = e_tmp_cap_mod;
            ge_crnt_caption_mode = *pe_caption_mode; 
            
        }
    }

    if(FALSE == b_is_caption_mode_set)
    {
        *pe_caption_mode = SBTL_IPCC_CAPTION_MODE_POP_ON;
        ge_last_caption_mode = e_tmp_cap_mod;
        ge_crnt_caption_mode = *pe_caption_mode;
    }

    DBG_INFO((IPCC_PREFIX" INFO: current caption mode is %d!   <%d, %s, %s>\n\r", 
        ge_crnt_caption_mode, __LINE__, __func__, __FILE__));
    
    return IPCCR_OK;
}                                           



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_parse_detect_length_unit
 *
 * Description: This API is used to detect the unit of length of tts:origin
 *
 * Inputs:  c_char                  - 
 *
 * Outputs: 
 *
 * Returns: 
 *
 ----------------------------------------------------------------------------*/
static BOOL _sbtl_ipcc_parse_is_space (IN CHAR  c_char)
{
    return ((c_char == ' ')        /* space */
            || (c_char == '\f')    /* form feed */
            || (c_char == '\n')    /* new line */
            || (c_char == '\r')    /* carriage return */
            || (c_char == '\t')    /* horizontal tab */
            || (c_char == '\v'));  /* vertical tab */
}



#ifndef _____________________________________________SPLIT_LINE________
#endif

/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_parse_record_global_style
 *
 * Description: This API is used to record global style which was defined in <style> and <region>
 *
 * Inputs:  pt_sbtl_global_style       - The data of global style
 *
 * Outputs: 
 *
 * Returns: IPCCR_OK                 - Success
 *          IPCCR_INV_ARG            - Invalid para
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_parse_record_global_style(IN SBTL_TTML_ATTR_ID_LIST_T* pt_sbtl_global_style)
{
    /* Check arguments */
    IPCC_CHECK_NULL(pt_sbtl_global_style,IPCCR_INV_ARG);
   

    if(!SLIST_IS_EMPTY(&pt_sbtl_global_style->t_attr_id_list))
    {
        gpt_sbtl_ttml_record_global_style = (SBTL_TTML_ATTR_ID_LIST_T*)x_mem_alloc(sizeof(SBTL_TTML_ATTR_ID_LIST_T));
        if(NULL == gpt_sbtl_ttml_record_global_style)
        {
            DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
            return IPCCR_INSUFFICIENT_SIZE;
        }
        x_memset(gpt_sbtl_ttml_record_global_style, 0, (SIZE_T)sizeof(SBTL_TTML_ATTR_ID_LIST_T)); 
        SLIST_INIT(&gpt_sbtl_ttml_record_global_style->t_attr_id_list);
        
        SBTL_TTML_ATTR_ID_T* pt_sbtl_ttml_style = SLIST_FIRST(&pt_sbtl_global_style->t_attr_id_list);
        
        while(pt_sbtl_ttml_style)
        {
            SBTL_TTML_ATTR_ID_T* pt_record_style = (SBTL_TTML_ATTR_ID_T*)x_mem_alloc(sizeof(SBTL_TTML_ATTR_ID_T));
            if(NULL == pt_record_style)
            {
                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                IPCC_MEM_FREE(gpt_sbtl_ttml_record_global_style);
                return IPCCR_INSUFFICIENT_SIZE;
            }
            x_memset(pt_record_style, 0, (SIZE_T)sizeof(SBTL_TTML_ATTR_ID_LIST_T));         
            x_memcpy((VOID*)pt_record_style, (const VOID*)pt_sbtl_ttml_style, sizeof(SBTL_TTML_ATTR_ID_T));
            SLIST_INIT(&pt_record_style->t_attr_list);
        
            /* Record attr */
            if(!SLIST_IS_EMPTY(&pt_sbtl_ttml_style->t_attr_list))
            {
                SBTL_TTML_ATTR_T* pt_sbtl_ttml_style_attr = SLIST_FIRST(&pt_sbtl_ttml_style->t_attr_list);

                while(pt_sbtl_ttml_style_attr)
                {
                    SBTL_TTML_ATTR_T* pt_record_style_attr = (SBTL_TTML_ATTR_T*)x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
                    if(NULL == pt_record_style_attr)
                    {
                        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                        IPCC_MEM_FREE(pt_record_style);
                        IPCC_MEM_FREE(gpt_sbtl_ttml_record_global_style);
                        return IPCCR_INSUFFICIENT_SIZE;
                    }
                    x_memset(pt_record_style_attr, 0, (SIZE_T)sizeof(SBTL_TTML_ATTR_T)); 

                    /* Record attr value */
                    x_memcpy((VOID*)pt_record_style_attr, (const VOID*)pt_sbtl_ttml_style_attr, sizeof(SBTL_TTML_ATTR_T));

                    SLIST_INSERT_HEAD(pt_record_style_attr,&(pt_record_style->t_attr_list),t_attr_link); 
                    pt_sbtl_ttml_style_attr = SLIST_NEXT(pt_sbtl_ttml_style_attr,t_attr_link);
                }
                
            }
          
            SLIST_INSERT_HEAD(pt_record_style, &(gpt_sbtl_ttml_record_global_style->t_attr_id_list), t_attr_id_link);
            pt_sbtl_ttml_style = SLIST_NEXT(pt_sbtl_ttml_style, t_attr_id_link);
        }

    }
    else
    {

    }
    
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_parse
 *
 * Description: This API is used to parse the data of sbtl engine
 *
 * Inputs:  pt_ttml_data                - The data of sbtl engine
 *          pt_wgt_ttml_data          - The data which parsed to WGT data
 *
 * Outputs: pt_wgt_ttml_data           - The data which parsed to WGT data
 *
 * Returns: IPCCR_OK                 - Success
 *          IPCCR_INV_ARG            - Invalid para
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_parse
(
    IN SBTL_TTML_DATA_T* pt_ttml_data, 
    IN OUT WGT_TTML_DATA_CMD_T* pt_wgt_ttml_data
)
{
    IPCC_CHECK_NULL(pt_wgt_ttml_data,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_ttml_data,IPCCR_INV_ARG);
    
    INT32 i4_ret = IPCCR_OK;
    
    /* Record the <region> id for window id of DTV-CC */
    BOOL b_region_id = FALSE;  

    CHAR ps_crnt_pen_edge_color[IPCC_TTS_ORIGIN_LENGTH/2] = "transparent";
    BOOL b_is_crnt_pen_edge_color_set = FALSE;

    CHAR ps_crnt_pen_edge_type[IPCC_TTS_ORIGIN_LENGTH/2] = " ";
    BOOL b_is_crnt_pen_edge_type_set = FALSE;
    
    CHAR ps_crnt_pen_fg_color[IPCC_TTS_ORIGIN_LENGTH/2] = "white";
    BOOL b_is_crnt_pen_fg_color_set = FALSE;
    
    CHAR ps_crnt_pen_bg_color[IPCC_TTS_ORIGIN_LENGTH/2] = "black";
    BOOL b_is_crnt_pen_bg_color_set = FALSE;
    
    SBTL_TTML_ATTR_FONT_FAMILY e_crnt_font_family = SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT;
    BOOL b_is_crnt_font_family_set = FALSE;

    UINT32 ui4_crnt_font_size = 0;
    BOOL b_is_crnt_font_size_set = FALSE;
    TTML_LENGTH_UNIT_TYPE e_font_size_unit = TTML_LENGTH_UNIT_TYPE_UNKNOW;
    
    SBTL_TTML_ATTR_FONT_STYLE  e_crnt_font_style = SBTL_TTML_ATTR_FONT_STYLE_NORMAL;
    BOOL b_is_crnt_font_style_set = FALSE;
    
    SBTL_TTML_ATTR_FONT_WEIGHT e_crnt_font_weight = SBTL_TTML_ATTR_FONT_WEGHT_NORMAL;
    BOOL b_is_crnt_font_weight_set = FALSE;
    
    SBTL_TTML_ATTR_TEXT_DECORATION e_crnt_text_decoration = SBTL_TTML_ATTR_TEXT_DECORATION_NONE;
    BOOL b_is_crnt_text_decoration_set = FALSE;
    
    SBTL_TTML_ATTR_TEXT_ALIGIN e_crnt_text_align = SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT;
    //BOOL b_is_crnt_text_align_set = FALSE;

    CHAR ps_crnt_win_opacity[IPCC_TTS_ORIGIN_LENGTH/2] = "1.0";
    //BOOL b_is_crnt_win_opacity_set = FALSE;

    CHAR ps_crnt_win_word_wrap[IPCC_TTS_ORIGIN_LENGTH/2] = "noWrap";
    //BOOL b_is_crnt_win_word_wrap_set = FALSE;

    BOOL b_ws_preserve = FALSE;
    BOOL b_is_crnt_ws_preserve_set = FALSE;

    UINT8 aui1_crnt_wnd[IPCC_MAX_WGT_NUM] = {8,8,8,8,8,8,8,8};
    UINT8* pui1_crnt_wnd = aui1_crnt_wnd;

    BOOL b_is_crnt_p_effective = FALSE;
    BOOL b_is_crnt_span_effective = FALSE;
    BOOL b_is_crnt_wgt_data_effective = FALSE;

    CHAR ps_crnt_text_tag[IPCC_TTS_ORIGIN_LENGTH] = "dialog";
    BOOL b_is_crnt_text_tag_set = FALSE;

    /* For roll up use*/
    BOOL b_roll_up_flag[IPCC_MAX_WGT_NUM] = {FALSE, FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
    UINT8 ui1_roll_up_index = 0;
    

    if(!SLIST_IS_EMPTY(&pt_ttml_data->t_element_p_list))
    {
        /* Get the first <p> element and parse it */
        SBTL_TTML_ELEMENT_P_T* pt_elem_p = SLIST_FIRST(&pt_ttml_data->t_element_p_list);

        if(NULL != &pt_elem_p->t_common_attr_list)//if(TRUE == gb_is_global_style_set)
        {
            
            while(pt_elem_p)
            {
                i4_ret = _sbtl_ipcc_parse_handle_global_style(pt_elem_p, pui1_crnt_wnd);
                if(IPCCR_OK != i4_ret)
                {
                    return i4_ret;
                }  

                if(TRUE == gb_roll_up_ignore_crnt_p
                    && SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
                {
                    b_roll_up_flag[ui1_roll_up_index] = TRUE;
                    gb_roll_up_ignore_crnt_p = FALSE;
                    ++ui1_roll_up_index;
                }
                
                ++pui1_crnt_wnd;
                pt_elem_p = SLIST_NEXT(pt_elem_p,t_element_p_link);               
            }
        }
        else
        {
            /* May be there is no global style of current file. */
            //*pui1_crnt_wnd = 0;

            gb_is_global_style_set = FALSE;
        }       


        pt_elem_p = SLIST_FIRST(&pt_ttml_data->t_element_p_list);
        pui1_crnt_wnd = aui1_crnt_wnd;

        UINT8 ui1_no_global_style_index = 0;// for no global style use
        ui1_roll_up_index = 0;
        
        /* Process <p> element  */
        while(pt_elem_p)
        {          
            WGT_ELEMENT_P_CMD_T* pt_wgt_element_p = (WGT_ELEMENT_P_CMD_T*)x_mem_alloc(sizeof(WGT_ELEMENT_P_CMD_T));
            if(NULL == pt_wgt_element_p)
            {
                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                return IPCCR_INSUFFICIENT_SIZE;
            }
            DLIST_INIT(&pt_wgt_element_p->t_element_span_list);

            /* Set default region id and captiom mode */
            pt_wgt_element_p->e_caption_mode = SBTL_IPCC_CAPTION_MODE_POP_ON;
            pt_wgt_element_p->ui1_region_id = (UINT8)0;

            if(FALSE == gb_is_global_style_set)
            {
                *pui1_crnt_wnd = ui1_no_global_style_index;
                //++ui1_no_global_style_index;
            }
            
            UINT8 ui1_crnt_region_id = *pui1_crnt_wnd;
            pt_wgt_element_p->ui1_region_id = ui1_crnt_region_id;

            if(!SLIST_IS_EMPTY(&pt_elem_p->t_element_span_list))
            {
                SBTL_TTML_ELEMENT_SPAN_T* pt_elem_span = SLIST_FIRST(&pt_elem_p->t_element_span_list);
                SIZE_T z_crnt_p_str_len = 0;

                /* Process <span> element */
                while(pt_elem_span)
                {
                    /* Set false when parser a new <p> element*/
                    b_region_id = FALSE;

                    
                    WGT_ELEMENT_SPAN_CMD_T* pt_wgt_element_span = (WGT_ELEMENT_SPAN_CMD_T*)x_mem_alloc(sizeof(WGT_ELEMENT_SPAN_CMD_T));
                    if(NULL == pt_wgt_element_span)
                    {
                        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                        IPCC_MEM_FREE(pt_wgt_element_p);
                        return IPCCR_INSUFFICIENT_SIZE;
                    }
                    DLIST_INIT(&pt_wgt_element_span->t_attr_list);
                    
                    if(0 < x_strlen((const CHAR*)pt_elem_span->ps_content))
                    {
                        if(!SLIST_IS_EMPTY(&pt_elem_span->t_attr_list))
                        {                            
                            SBTL_TTML_ATTR_T* pt_attr = SLIST_FIRST(&pt_elem_span->t_attr_list);

                            CHAR* ps_origin_x = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                            if(NULL == ps_origin_x)
                            {
                                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                IPCC_MEM_FREE(pt_wgt_element_p);
                                IPCC_MEM_FREE(pt_wgt_element_span);
                                return IPCCR_INSUFFICIENT_SIZE;
                            }
                            
                            CHAR* ps_origin_y = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                            if(NULL == ps_origin_y)
                            {
                                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                IPCC_MEM_FREE(pt_wgt_element_p);
                                IPCC_MEM_FREE(pt_wgt_element_span);
                                IPCC_MEM_FREE(ps_origin_x);
                                return IPCCR_INSUFFICIENT_SIZE;
                            }
                            TTML_LENGTH_UNIT_TYPE e_length_unit = TTML_LENGTH_UNIT_TYPE_UNKNOW;

                            
                            CHAR* ps_win_size_col = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                            if(NULL == ps_win_size_col)
                            {
                                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                IPCC_MEM_FREE(pt_wgt_element_p);
                                IPCC_MEM_FREE(pt_wgt_element_span);
                                IPCC_MEM_FREE(ps_origin_x);
                                IPCC_MEM_FREE(ps_origin_y);
                                return IPCCR_INSUFFICIENT_SIZE;
                            }

                            CHAR* ps_win_size_row = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                            if(NULL == ps_win_size_row)
                            {
                                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                IPCC_MEM_FREE(pt_wgt_element_p);
                                IPCC_MEM_FREE(pt_wgt_element_span);
                                IPCC_MEM_FREE(ps_origin_x);
                                IPCC_MEM_FREE(ps_origin_y);
                                IPCC_MEM_FREE(ps_win_size_col);
                                return IPCCR_INSUFFICIENT_SIZE;
                            }
                            TTML_LENGTH_UNIT_TYPE e_win_size_length_unit = TTML_LENGTH_UNIT_TYPE_UNKNOW;    


                            DBG_INFO(("{IPCC} <PRI_ATTR>:   "));
                            /* Process the attr of <sapn> element */
                            while(pt_attr)
                            {
                                SBTL_IPCC_ATTR_T e_attr = pt_attr->e_attr; 
                                
                                switch(e_attr)
                                {
                                    /* Set pen color */
                                    case SBTL_XML_IPCC_ATTR_COLOR:                                        
                                        x_strcpy(ps_crnt_pen_fg_color, (const CHAR*)pt_attr->u.ps_value);
                                        b_is_crnt_pen_fg_color_set = TRUE;
                                        DBG_INFO(("((%u)PRI_PEN_CLR: %s), ", ui1_crnt_region_id, ps_crnt_pen_fg_color));
                                        break;


                                    case SBTL_XML_IPCC_ATTR_BGCOLOR:                                        
                                        x_strcpy(ps_crnt_pen_bg_color, (const CHAR*)pt_attr->u.ps_value);
                                        b_is_crnt_pen_bg_color_set = TRUE;
                                        DBG_INFO(("((%u)PRI_PEN_BGCLR: %s), ", ui1_crnt_region_id, ps_crnt_pen_bg_color));
                                        break;

                                    /* Set pen attr */
                                    case SBTL_XML_IPCC_ATTR_FONTFAMILY: 
                                        {
                                            e_crnt_font_family = pt_attr->u.e_font_family;
                                            b_is_crnt_font_family_set = TRUE;
                                            DBG_INFO(("((%u)PRI_FNT_FMLY: %d), ", ui1_crnt_region_id, e_crnt_font_family));
                                        }
                                        
                                        break;

                                    case SBTL_XML_IPCC_ATTR_FONT_STYLE:
                                        {
                                            e_crnt_font_style = pt_attr->u.e_font_style;
                                            b_is_crnt_font_style_set = TRUE;
                                            DBG_INFO(("((%u)PRI_FNT_STYLE: %d), ", ui1_crnt_region_id, e_crnt_font_style));
                                        }
                                        break;


                                    case SBTL_XML_IPCC_ATTR_FONT_WEIGHT:
                                        {
                                            e_crnt_font_weight = pt_attr->u.e_font_weight;
                                            b_is_crnt_font_weight_set = TRUE;
                                            DBG_INFO(("((%u)PRI_FNT_WGHT: %d), ", ui1_crnt_region_id, e_crnt_font_weight));
                                        }
                                        break;                            

                                    case SBTL_XML_IPCC_ATTR_TEXT_DECORATION:
                                        {
                                            e_crnt_text_decoration = pt_attr->u.e_text_decoration;
                                            b_is_crnt_text_decoration_set = TRUE;
                                            DBG_INFO(("((%u)PRI_TXT_DECR: %d), ", ui1_crnt_region_id, e_crnt_text_decoration));
                                            
                                        }
                                        break;

                                    case SBTL_XML_IPCC_ATTR_FONTSIZE:
                                        {                                            
                                            CHAR* ps_font_size = (CHAR *)x_mem_alloc(sizeof(CHAR)* (1 + x_strlen((const CHAR*)pt_attr->u.ps_value)));
                                            if(NULL == ps_font_size)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                return IPCCR_INSUFFICIENT_SIZE;
                                            }

                                            x_strcpy(ps_font_size, (const CHAR*)pt_attr->u.ps_value);
                                            
                                            CHAR* ps_rslt = (CHAR *)x_mem_alloc(sizeof(CHAR)* (IPCC_TTS_ORIGIN_LENGTH/2));
                                            if(NULL == ps_rslt)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(ps_font_size);
                                                return IPCCR_INSUFFICIENT_SIZE;
                                            }

                                            
                                            i4_ret = _sbtl_ipcc_parse_detect_length_unit(ps_font_size, &e_font_size_unit);
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(ps_font_size);
                                                IPCC_MEM_FREE(ps_rslt);
                                                return i4_ret;
                                            }

                                            if(TTML_LENGTH_UNIT_TYPE_PX == e_font_size_unit)
                                            {
                                                i4_ret = x_sbtl_ipcc_str_split("px", ps_font_size, &ps_rslt, 1, NULL);
                                            }
                                            else if(TTML_LENGTH_UNIT_TYPE_PRCNT == e_font_size_unit)
                                            {
                                                i4_ret = x_sbtl_ipcc_str_split("%", ps_font_size, &ps_rslt, 1, NULL); 
                                            }
                                            else if(TTML_LENGTH_UNIT_TYPE_C == e_font_size_unit)
                                            {
                                                i4_ret = x_sbtl_ipcc_str_split("c", ps_font_size, &ps_rslt, 1, NULL);
                                            }
                                            
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("\n{IPCC} ERR: i4_ret=%d! src_str=%s, dst_str=%s  <%d, %s, %s>\n\r", 
                                                            i4_ret, ps_font_size, ps_rslt,
                                                            __LINE__, __func__, __FILE__));
                                                
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                 IPCC_MEM_FREE(ps_font_size);
                                                IPCC_MEM_FREE(ps_rslt); 
                                                return i4_ret;
                                            }
                                               
                                            ui4_crnt_font_size = (UINT32)x_strtoull((const CHAR*)ps_rslt, NULL, 10);
                                            IPCC_MEM_FREE(ps_rslt);
                                            IPCC_MEM_FREE(ps_font_size);

                                            b_is_crnt_font_size_set = TRUE;

                                            DBG_INFO(("((%u)PRI_FNT_SIZE: %u), ", ui1_crnt_region_id, ui4_crnt_font_size));
                                            DBG_INFO(("((%u)PRI_FNT_SIZE_UNT: %d), ", ui1_crnt_region_id, e_font_size_unit));
                                        }
                                        break;


                                    /* Set window attr */
                                    case SBTL_XML_IPCC_ATTR_TEXTALIGN:
                                        {
                                            e_crnt_text_align = pt_attr->u.e_text_align;
                                            //b_is_crnt_text_align_set = TRUE;
                                            DBG_INFO(("((%u)PRI_TXT_ALIGN: %d), ", ui1_crnt_region_id, e_crnt_text_align));
                                        }
                                        break;

                                                            
                                    /* Set win anchor */
                                    case SBTL_XML_IPCC_ATTR_ORIGIN:
                                        i4_ret = _sbtl_ipcc_parse_abstract_value(pt_attr->u.ps_value, &e_length_unit, ps_origin_y, ps_origin_x);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            return i4_ret;        
                                        }
                                        break;   


                                    /* Set win size */
                                    /* tts:extent  map to row count and column count in DTVCC*/
                                    case SBTL_XML_IPCC_ATTR_FONT_EXTENT:
                                        i4_ret = _sbtl_ipcc_parse_abstract_value(pt_attr->u.ps_value, &e_win_size_length_unit, ps_win_size_row, ps_win_size_col);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            return i4_ret;        
                                        } 
                                        break; 

                                    /* Get <region> id and set it for window id of DTV-CC */
                                    case SBTL_XML_IPCC_ATTR_REGION:
                                    {
                                        if(FALSE == b_region_id)
                                        {
                                            CHAR* ps_region_id = (CHAR *)x_mem_alloc(sizeof(CHAR)* (1 + x_strlen((const CHAR*)pt_attr->u.ps_value)));
                                            if(NULL == ps_region_id)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                return IPCCR_INSUFFICIENT_SIZE;
                                            }

                                            x_strcpy(ps_region_id, (const CHAR*)pt_attr->u.ps_value);
                                            DBG_INFO((IPCC_PREFIX" INFO: current caption mode is %d!   <%d, %s, %s>\n\r", 
                                                        ge_crnt_caption_mode, __LINE__, __func__, __FILE__));

                                            /*Step 1. detect caption mode */
                                            i4_ret = _sbtl_ipcc_parse_detect_caption_mode(ps_region_id, &pt_wgt_element_p->e_caption_mode);
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                return i4_ret;
                                            }

                                            /* Step 2. Select an unused array and save attr */
                                            UINT8 ui_i = 0;
                                            for(ui_i = 0; ui_i < IPCC_MAX_REGION_NUM; ++ui_i)
                                            {
                                                /* Step 2.1 If crnt region id is same with crnt array region id*/
                                                if(0 == x_strcmp((const CHAR*)ps_region_id, (const CHAR*)gt_region_attr[ui_i].ps_region_id))
                                                {
                                                    ui1_crnt_region_id = ui_i;
                                                    pt_wgt_element_p->ui1_region_id = ui_i;
                                                    break;
                                                }
                                                /* Step 2.2 If crnt region id is not same with crnt array region id*/
                                                else
                                                {
                                                    /* Step 2.2.1  Select an unused array */
                                                    if(0 == x_strcmp(" ", (const CHAR*)gt_region_attr[ui_i].ps_region_id))
                                                    {
                                                        ui1_crnt_region_id = ui_i;
                                                        pt_wgt_element_p->ui1_region_id = ui_i;
                                                        x_strcpy(gt_region_attr[ui_i].ps_region_id, (const CHAR*)ps_region_id);
                                                        aui1_crnt_wnd[ui_i] = 9;
                                                        break;
                                                    }
                                                }
                                            }

                                            b_region_id = TRUE; // Not parse it again
                                            
                                        }
                                    }
                                        break;

                                    /* Get <region> bgcolor and set it for window bgcolor of DTV-CC */
                                    case SBTL_XML_IPCC_ATTR_REGION_BGCOLOR:
                                    {                                        
                                        x_strcpy(ps_crnt_pen_bg_color, (const CHAR*)pt_attr->u.ps_value);
                                        b_is_crnt_pen_bg_color_set = TRUE;
                                        DBG_INFO(("((%u)PRI_WND_BGCLR: %s), ", ui1_crnt_region_id, ps_crnt_pen_bg_color));
                
                                    }
                                        break;

                                    /* Get the crnt window opacity */
                                    case SBTL_XML_IPCC_ATTR_WINDOW_OPACITY:
                                    {
                                        x_strcpy(ps_crnt_win_opacity, (const CHAR*)pt_attr->u.ps_value);
                                        //b_is_crnt_win_opacity_set = TRUE;
                                        DBG_INFO(("((%u)PRI_WND_OPCTY: %s), ", ui1_crnt_region_id, ps_crnt_win_opacity));
                                    }
                                        break;

                                    /* Get the crnt pen edge color */
                                    case SBTL_XML_IPCC_ATTR_TEXT_OUTLINE:
                                    {
                                        /* Step 1: Detect ps_value if it contains "%" */
                                        
                                        /* If there is no "%" */
                                        if(NULL == x_strstr((const CHAR*)pt_attr->u.ps_value, "%"))
                                        {
                                            x_strcpy(ps_crnt_pen_edge_color, (const CHAR*)pt_attr->u.ps_value);
                                            x_strcpy(ps_crnt_pen_edge_type, " ");
                                        }
                                        /* If there is "%" */
                                        else
                                        {
                                            /* Step 2: Split pen edge color from ps_value */
                                            CHAR* ps_whitespace =" ";
                                            CHAR* ps_pen_edge_color = ps_crnt_pen_edge_color;
                                            i4_ret = x_sbtl_ipcc_str_split( ps_whitespace, 
                                                                            pt_attr->u.ps_value, 
                                                                            &ps_pen_edge_color, 
                                                                            1, 
                                                                            ps_crnt_pen_edge_type);
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("\n{IPCC} ERR: i4_ret=%d! src_str=%s, dst_str=%s  <%d, %s, %s>\n\r", 
                                                            i4_ret, pt_attr->u.ps_value, ps_pen_edge_color,
                                                            __LINE__, __func__, __FILE__));

                                                
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                return IPCCR_INV_ARG;
                                            }

                                        }
                                        
                                        b_is_crnt_pen_edge_color_set = TRUE;
                                        b_is_crnt_pen_edge_type_set = TRUE;
                                        DBG_INFO(("((%u)PRI_TXT_OUTLINE: %s), ", ui1_crnt_region_id, ps_crnt_pen_edge_color));
                                    }
                                        break;

                                    /* Get the crnt window word wrap */
                                    case SBTL_XML_IPCC_ATTR_WRAP_OPTION:
                                    {
                                        x_strcpy(ps_crnt_win_word_wrap, (const CHAR*)pt_attr->u.ps_value);
                                        //b_is_crnt_win_word_wrap_set = TRUE;
                                        DBG_INFO(("((%u)PRI_WW: %s), ", ui1_crnt_region_id, ps_crnt_win_word_wrap));
                                    }
                                        break;


                                    /* Get xml:space */
                                    case SBTL_XML_IPCC_ATTR_WHITESPACE:
                                    {
                                        if(SBTL_TTML_ATTR_WHITESPACE_DEFAULT == pt_attr->u.e_whitespace)
                                        {
                                            b_ws_preserve = FALSE;
                                        }
                                        else if( SBTL_TTML_ATTR_WHITESPACE_PRESERVE == pt_attr->u.e_whitespace)
                                        {
                                            b_ws_preserve = TRUE;
                                        }

                                        b_is_crnt_ws_preserve_set = TRUE;
                                        
                                        DBG_INFO(("((%u)PRI_WS: %d), ", ui1_crnt_region_id, b_ws_preserve));
                                        
                                    }
                                        break;  


                                    /* Get ttm:role */
                                    case SBTL_XML_IPCC_ATTR_ROLE:
                                    {
                                        x_strcpy(ps_crnt_text_tag, (const CHAR*)pt_attr->u.ps_value);
                                        b_is_crnt_text_tag_set = TRUE;

                                        DBG_INFO(("((%u)PRI_RL: %s), ", ui1_crnt_region_id, ps_crnt_text_tag));
                                    }
                                        break;
                                    
                                    default:
                                        break;
                                }
                                pt_attr = SLIST_NEXT(pt_attr, t_attr_link);
                            }

                            DBG_INFO(("<%d, %s, %s>\n\r", __LINE__, __func__, __FILE__)); 


                            /* Add pen color element*/
                            {                                
                                BOOL b_is_new_pen_color = FALSE;
                                

                                /* Step1. Check the crnt pen fg color and bg color is same to gt_region_attr[ui1_crnt_region_id] or not */
                                if(TRUE == b_is_crnt_pen_fg_color_set)
                                {
                                   b_is_new_pen_color = TRUE;                            
                                }

                                
                                if(TRUE == b_is_crnt_pen_bg_color_set)
                                {
                                    b_is_new_pen_color = TRUE;
                                }
                                

                                 /* Step2. If they are not same, set it and save it */    
                                 if(TRUE == b_is_new_pen_color)
                                 {
                                    WGT_ATTR_CMD_T* pt_pen_color = (WGT_ATTR_CMD_T*)x_mem_alloc(sizeof(WGT_ATTR_CMD_T));
                                    if(NULL == pt_pen_color)
                                    {
                                        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                        IPCC_MEM_FREE(pt_wgt_element_p);
                                        IPCC_MEM_FREE(pt_wgt_element_span);
                                        IPCC_MEM_FREE(ps_origin_x);
                                        IPCC_MEM_FREE(ps_origin_y);
                                        IPCC_MEM_FREE(ps_win_size_col);
                                        IPCC_MEM_FREE(ps_win_size_row);
                                        return IPCCR_INSUFFICIENT_SIZE;
                                    }
                                    x_memset(pt_pen_color, 0, (SIZE_T)(sizeof(WGT_ATTR_CMD_T)));
                                    
                                    pt_pen_color->e_attr_type = WGT_ATTR_TYPE_SET_PEN_COLOR;
                                    
                                    
                                    /* Set pen fg color */
                                    if(TRUE == b_is_crnt_pen_fg_color_set)
                                    {
                                        i4_ret = _sbtl_ipcc_parse_color(ps_crnt_pen_fg_color, &(pt_pen_color->u.t_attr_pen_color.t_color_text));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen fg color error!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_color);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                        
                                        b_is_crnt_pen_fg_color_set = FALSE;
                                    }
                                    else
                                    {
                                        i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_region_id].ps_pen_fg_color, &(pt_pen_color->u.t_attr_pen_color.t_color_text));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen fg color error,!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id,__LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_color);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                       
                                    }



                                    /* Set pen bg color */
                                    if(TRUE == b_is_crnt_pen_bg_color_set)
                                    {
                                        i4_ret = _sbtl_ipcc_parse_color(ps_crnt_pen_bg_color, &(pt_pen_color->u.t_attr_pen_color.t_color_bk));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen bg color error!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_color);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                        
                                        b_is_crnt_pen_bg_color_set = FALSE;
                                    }
                                    else
                                    {
                                        i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_region_id].ps_win_bg_color, &(pt_pen_color->u.t_attr_pen_color.t_color_bk));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen bg color error!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_color);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                    }

                                    /* Set pen edge color */
                                    if(TRUE == b_is_crnt_pen_edge_color_set)
                                    {
                                        i4_ret = _sbtl_ipcc_parse_color(ps_crnt_pen_edge_color, &(pt_pen_color->u.t_attr_pen_color.t_color_edge));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse edge color error!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_color);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                        
                                        b_is_crnt_pen_edge_color_set = FALSE;
                                    }
                                    else
                                    {
                                        i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_region_id].ps_pen_edge_color, &(pt_pen_color->u.t_attr_pen_color.t_color_edge));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse edge color error!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_color);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                        
                                    }
                                    

                                    pt_pen_color->u.t_attr_pen_color.b_is_flash_bk = FALSE;
                                    pt_pen_color->u.t_attr_pen_color.b_is_flash_fg = FALSE;

                                    DLIST_INSERT_HEAD(pt_pen_color,&(pt_wgt_element_span->t_attr_list),t_attr_link);
                                 }
                                 else // The pen bg color will change if win bg color changed
                                 {
                                    WGT_ATTR_CMD_T* pt_pen_color = (WGT_ATTR_CMD_T*)x_mem_alloc(sizeof(WGT_ATTR_CMD_T));
                                    if(NULL == pt_pen_color)
                                    {
                                        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                        IPCC_MEM_FREE(pt_wgt_element_p);
                                        IPCC_MEM_FREE(pt_wgt_element_span);
                                        IPCC_MEM_FREE(ps_origin_x);
                                        IPCC_MEM_FREE(ps_origin_y);
                                        IPCC_MEM_FREE(ps_win_size_col);
                                        IPCC_MEM_FREE(ps_win_size_row);
                                        return IPCCR_INSUFFICIENT_SIZE;
                                    }
                                    x_memset(pt_pen_color, 0, (SIZE_T)(sizeof(WGT_ATTR_CMD_T)));
                                    
                                    pt_pen_color->e_attr_type = WGT_ATTR_TYPE_SET_PEN_COLOR;

                                    if(FALSE == b_is_crnt_pen_bg_color_set)
                                    {
                                        {
                                            /* Set pen bg color */
                                            i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_region_id].ps_pen_bg_color, &(pt_pen_color->u.t_attr_pen_color.t_color_bk));
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen bg color error!   <%d, %s, %s>\n\r", 
                                                            ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(pt_pen_color);
                                                return IPCCR_INTERNAL_ERROR;
                                            }

                                            /* Set pen fg color */
                                            i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_region_id].ps_pen_fg_color, &(pt_pen_color->u.t_attr_pen_color.t_color_text));
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen fg color error,!   <%d, %s, %s>\n\r", 
                                                            ui1_crnt_region_id,__LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(pt_pen_color);
                                                return IPCCR_INTERNAL_ERROR;
                                            }
                                            
                                            /* Set pen edge color */
                                            i4_ret = _sbtl_ipcc_parse_color(gt_region_attr[ui1_crnt_region_id].ps_pen_edge_color, &(pt_pen_color->u.t_attr_pen_color.t_color_edge));
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen edge color error,!   <%d, %s, %s>\n\r", 
                                                            ui1_crnt_region_id,__LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(pt_pen_color);
                                                return IPCCR_INTERNAL_ERROR;
                                            }
                                            else
                                            {

                                            }

                                            pt_pen_color->u.t_attr_pen_color.b_is_flash_bk = FALSE;
                                            pt_pen_color->u.t_attr_pen_color.b_is_flash_fg = FALSE;

                                            DLIST_INSERT_HEAD(pt_pen_color,&(pt_wgt_element_span->t_attr_list),t_attr_link);                                       
                                        }  
                                    }
                                 }
                            }



                            /* Add pen font element */
                            {
                                BOOL b_is_new_pen_attr = FALSE;
                                
                                /* Step1. Check if the crnt value is same with last one */
                                /* Check font sytle */
                                if(TRUE == b_is_crnt_font_style_set)
                                {
                                   b_is_new_pen_attr = TRUE;
                                }
                                

                                /* Check text decoration */
                                if(TRUE == b_is_crnt_text_decoration_set)
                                {
                                   b_is_new_pen_attr = TRUE;
                                }
                                

                                /* Check font weight */
                                if(TRUE == b_is_crnt_font_weight_set)
                                {
                                   b_is_new_pen_attr = TRUE;
                                }
                                


                                /* Check font family */
                                if(TRUE == b_is_crnt_font_family_set)
                                {
                                   b_is_new_pen_attr = TRUE;
                                }   


                                /* Check font size */
                                if(TRUE == b_is_crnt_font_size_set)
                                {
                                   b_is_new_pen_attr = TRUE;
                                }

                                /* Check pen edge type */
                                if(TRUE == b_is_crnt_pen_edge_type_set)
                                {
                                   b_is_new_pen_attr = TRUE;
                                }
                                

                                /* Step2. Set new pen font */
                                if(TRUE == b_is_new_pen_attr)
                                {
                                    /* Pen attr should be map to the pen attr of DTV-CC */
                                    WGT_ATTR_CMD_T* pt_pen_attr = NULL;
                                    pt_pen_attr = (WGT_ATTR_CMD_T*)x_mem_alloc(sizeof(WGT_ATTR_CMD_T));                
                                    if(NULL == pt_pen_attr)
                                    {
                                        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                        IPCC_MEM_FREE(pt_wgt_element_p);
                                        IPCC_MEM_FREE(pt_wgt_element_span);
                                        IPCC_MEM_FREE(ps_origin_x);
                                        IPCC_MEM_FREE(ps_origin_y);
                                        IPCC_MEM_FREE(ps_win_size_col);
                                        IPCC_MEM_FREE(ps_win_size_row);
                                        return IPCCR_INSUFFICIENT_SIZE;
                                    }
                                    x_memset(pt_pen_attr, 0, (SIZE_T)(sizeof(WGT_ATTR_CMD_T)));
                                    pt_pen_attr->e_attr_type = WGT_ATTR_TYPE_SET_PEN_FONT;


                                    /* Set font sytle */
                                    if(TRUE == b_is_crnt_font_style_set)
                                    {
                                        i4_ret = _sbtl_ipcc_parse_font_style(&e_crnt_font_style, &(pt_pen_attr->u.t_pen_font.t_font.e_style));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                           return i4_ret;
                                        }
                                        b_is_crnt_font_style_set = FALSE;
                                    }
                                    else
                                    {
                                        i4_ret = _sbtl_ipcc_parse_font_style(&gt_region_attr[ui1_crnt_region_id].e_font_style, &(pt_pen_attr->u.t_pen_font.t_font.e_style));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                           return i4_ret;
                                        }
                                    }


                                    /* Set text decoration */
                                    if(TRUE == b_is_crnt_text_decoration_set)
                                    {
                                        i4_ret = _sbtl_ipcc_parse_text_decoration(&e_crnt_text_decoration, &(pt_pen_attr->u.t_pen_font.t_font.e_style));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                           return i4_ret;
                                        }
                                    }
                                    else
                                    {
                                        i4_ret = _sbtl_ipcc_parse_text_decoration(&gt_region_attr[ui1_crnt_region_id].e_text_decoration, &(pt_pen_attr->u.t_pen_font.t_font.e_style));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                           return i4_ret;
                                        }
                                    }


                                    /* Set font weight */
                                    if(TRUE == b_is_crnt_font_weight_set)
                                    {
                                        i4_ret = _sbtl_ipcc_parse_font_weight(&e_crnt_font_weight, &(pt_pen_attr->u.t_pen_font.t_font.e_style));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                           return i4_ret;
                                        }
                                    }
                                    else
                                    {
                                        i4_ret = _sbtl_ipcc_parse_font_weight(&gt_region_attr[ui1_crnt_region_id].e_font_weight, &(pt_pen_attr->u.t_pen_font.t_font.e_style));
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                           return i4_ret;
                                        }
                                    }


                                    /* Set font family */
                                    if(TRUE == b_is_crnt_font_family_set) //font family changed
                                    {
                                        i4_ret = _sbtl_ipcc_parse_font_family(&e_crnt_font_family, pt_pen_attr->u.t_pen_font.t_font.s_font_name);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                            return i4_ret;
                                        }
                                        else
                                        {

                                        }
                                        

                                        if(TRUE == b_is_crnt_font_size_set) //font size changed
                                        {                                            
                                            i4_ret = _sbtl_ipcc_parse_font_size(&e_crnt_font_family, &ui4_crnt_font_size, &e_font_size_unit, &(pt_pen_attr->u.t_pen_font.t_font.e_size));
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse font size error!   <%d, %s, %s>\n\r", 
                                                            ui1_crnt_region_id,__LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(pt_pen_attr);
                                                return IPCCR_INTERNAL_ERROR;
                                            }
                                        }
                                        else //font size not changed
                                        {                                            
                                            i4_ret = _sbtl_ipcc_parse_font_size(&e_crnt_font_family, &gt_region_attr[ui1_crnt_region_id].ui4_font_size, &e_font_size_unit, &(pt_pen_attr->u.t_pen_font.t_font.e_size));
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse font size error!   <%d, %s, %s>\n\r", 
                                                            ui1_crnt_region_id,__LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(pt_pen_attr);
                                                return IPCCR_INTERNAL_ERROR;
                                            }
                                        }
                                    }
                                    else //font family not changed
                                    {
                                        i4_ret = _sbtl_ipcc_parse_font_family(&gt_region_attr[ui1_crnt_region_id].e_font_family, pt_pen_attr->u.t_pen_font.t_font.s_font_name);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                            return i4_ret;
                                        }

                                        /* Set font size */
                                        if(TRUE == b_is_crnt_font_size_set) ////font size changed
                                        {                                             
                                            i4_ret = _sbtl_ipcc_parse_font_size(&gt_region_attr[ui1_crnt_region_id].e_font_family, &ui4_crnt_font_size, &e_font_size_unit, &(pt_pen_attr->u.t_pen_font.t_font.e_size));
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse font size error!   <%d, %s, %s>\n\r", 
                                                            ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(pt_pen_attr);
                                                return IPCCR_INTERNAL_ERROR;
                                            }
                                        }
                                        else //font size not changed
                                        {                                            
                                            i4_ret = _sbtl_ipcc_parse_font_size(&gt_region_attr[ui1_crnt_region_id].e_font_family, &gt_region_attr[ui1_crnt_region_id].ui4_font_size, &e_font_size_unit, &(pt_pen_attr->u.t_pen_font.t_font.e_size));
                                            if(IPCCR_OK != i4_ret)
                                            {
                                                DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse font size error!   <%d, %s, %s>\n\r", 
                                                            ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                                IPCC_MEM_FREE(pt_wgt_element_p);
                                                IPCC_MEM_FREE(pt_wgt_element_span);
                                                IPCC_MEM_FREE(ps_origin_x);
                                                IPCC_MEM_FREE(ps_origin_y);
                                                IPCC_MEM_FREE(ps_win_size_col);
                                                IPCC_MEM_FREE(ps_win_size_row);
                                                IPCC_MEM_FREE(pt_pen_attr);
                                                return IPCCR_INTERNAL_ERROR;
                                            }
                                        }
                                    }

                                    /* Set pen edge type*/
                                    if(TRUE == b_is_crnt_pen_edge_type_set)
                                    {
                                        i4_ret = _sbtl_ipcc_parse_pen_edge_type(ps_crnt_pen_edge_type, 
                                                                                &pt_pen_attr->u.t_pen_font.t_font.ui1_edge_type);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen edge type error!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                    }
                                    else
                                    {
                                        i4_ret = _sbtl_ipcc_parse_pen_edge_type(gt_region_attr[ui1_crnt_region_id].ps_pen_edge_type, 
                                                                                &pt_pen_attr->u.t_pen_font.t_font.ui1_edge_type);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: (NO.%u Window) Parse pen edge type error!   <%d, %s, %s>\n\r", 
                                                        ui1_crnt_region_id, __LINE__, __func__, __FILE__));
                                            IPCC_MEM_FREE(pt_wgt_element_p);
                                            IPCC_MEM_FREE(pt_wgt_element_span);
                                            IPCC_MEM_FREE(ps_origin_x);
                                            IPCC_MEM_FREE(ps_origin_y);
                                            IPCC_MEM_FREE(ps_win_size_col);
                                            IPCC_MEM_FREE(ps_win_size_row);
                                            IPCC_MEM_FREE(pt_pen_attr);
                                            return IPCCR_INTERNAL_ERROR;
                                        }
                                    }

                                    pt_pen_attr->u.t_pen_font.ui1_offset = WGL_CC_TEXT_OFFSET_NORMAL;
                                    pt_pen_attr->u.t_pen_font.t_font.e_cmap = FE_CMAP_ENC_UNICODE;
                                    
                                    DLIST_INSERT_HEAD(pt_pen_attr,&(pt_wgt_element_span->t_attr_list),t_attr_link);
                                }
                                
                                b_is_crnt_text_decoration_set = FALSE;
                                b_is_crnt_font_family_set = FALSE;
                                b_is_crnt_font_size_set = FALSE;
                                b_is_crnt_font_weight_set = FALSE;
                                b_is_crnt_pen_edge_type_set = FALSE;
                                
                                b_is_new_pen_attr = FALSE;
                            }


                            /* Add text tag */
                            {
                                if(TRUE == b_is_crnt_text_tag_set)
                                {
                                    WGT_ATTR_CMD_T* pt_text_tag_attr = NULL;
                                    pt_text_tag_attr = (WGT_ATTR_CMD_T*)x_mem_alloc(sizeof(WGT_ATTR_CMD_T));                
                                    if(NULL == pt_text_tag_attr)
                                    {
                                        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                        IPCC_MEM_FREE(pt_wgt_element_p);
                                        IPCC_MEM_FREE(pt_wgt_element_span);
                                        IPCC_MEM_FREE(ps_origin_x);
                                        IPCC_MEM_FREE(ps_origin_y);
                                        IPCC_MEM_FREE(ps_win_size_col);
                                        IPCC_MEM_FREE(ps_win_size_row);
                                        return IPCCR_INSUFFICIENT_SIZE;
                                    }
                                    x_memset(pt_text_tag_attr, 0, (SIZE_T)(sizeof(WGT_ATTR_CMD_T)));
                                    pt_text_tag_attr->e_attr_type = WGT_ATTR_TYPE_SET_TEXT_TAG;

                                    i4_ret = _sbtl_ipcc_parse_text_tag(ps_crnt_text_tag, &pt_text_tag_attr->u.ui1_text_tag);
                                    if(IPCCR_OK != i4_ret)
                                    {
                                        IPCC_MEM_FREE(pt_wgt_element_p);
                                        IPCC_MEM_FREE(pt_wgt_element_span);
                                        IPCC_MEM_FREE(ps_origin_x);
                                        IPCC_MEM_FREE(ps_origin_y);
                                        IPCC_MEM_FREE(ps_win_size_col);
                                        IPCC_MEM_FREE(ps_win_size_row);
                                        IPCC_MEM_FREE(pt_text_tag_attr);
                                        return i4_ret;
                                    } 

                                    DLIST_INSERT_HEAD(pt_text_tag_attr,&(pt_wgt_element_span->t_attr_list),t_attr_link);
                                    b_is_crnt_text_tag_set = FALSE;
                                }
                            }
                        }    

                        /* Add content element */
                        {   
                            DBG_INFO((IPCC_PREFIX" INFO: Content current caption mode is %d, b_roll_up_flag[ui1_roll_up_index]=%d, content is '%s'!   <%d, %s, %s>\n\r", 
                                        ge_crnt_caption_mode, b_roll_up_flag[ui1_roll_up_index], pt_elem_span->ps_content, __LINE__, __func__, __FILE__));

                            /* Following may be for S**y requirement */
                        #if 0    
                            /* If crnt is roll up, check crnt <p> should ignore or not. */
                            if(TRUE == b_roll_up_flag[ui1_roll_up_index]
                                && SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
                            {
                                ++ui1_roll_up_index;
                                break;
                            }
                            else if(FALSE == b_roll_up_flag[ui1_roll_up_index]
                                && SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
                            {
                                ++ui1_roll_up_index;
                            }
                       #endif
                       
                            BOOL b_tmp = FALSE;
                            
                            if(TRUE == b_is_crnt_ws_preserve_set)
                            {
                                b_tmp = b_ws_preserve;
                            }
                            else
                            {
                                b_tmp = gt_region_attr[ui1_crnt_region_id].b_whitespace_preserve;
                            }
                            
                            /* xml:space="default" */
                            if(FALSE == b_tmp)
                            {
                                CHAR* ps_str = pt_elem_span->ps_content;
                                BOOL b_is_ws = FALSE;

                                while('\0' != *ps_str)
                                {
                                    b_is_ws = _sbtl_ipcc_parse_is_space(*ps_str);
                                    if(TRUE == b_is_ws)
                                    {
                                        ++ps_str;
                                    }
                                    else if(FALSE == b_is_ws)
                                    {
                                        break;
                                    } 
                                }

                                if(TRUE == b_is_ws)
                                {                                    
                                    IPCC_MEM_FREE(pt_wgt_element_span);
                                    b_is_crnt_span_effective = FALSE;
                                        

                                }
                                else if(FALSE == b_is_ws)
                                {
                                    SIZE_T z_str_len = x_strlen((const CHAR*)pt_elem_span->ps_content);
                                    
                                    CHAR* ps_text = (CHAR*)x_mem_alloc(sizeof(CHAR) * (z_str_len + 1));
                                    if(NULL == ps_text)
                                    {
                                        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                        IPCC_MEM_FREE(pt_wgt_element_p);
                                        IPCC_MEM_FREE(pt_wgt_element_span);

                                        return IPCCR_INSUFFICIENT_SIZE;
                                    }
                                    x_memset(ps_text, 0, (SIZE_T)(z_str_len + 1));  
                                    x_strcpy(ps_text, (const CHAR*)pt_elem_span->ps_content);
                                    pt_wgt_element_span->ps_content = ps_text;
                                    b_is_crnt_span_effective = TRUE;
                                    DBG_INFO((IPCC_PREFIX" INFO: Content is '%s'!   <%d, %s, %s>\n\r", 
                                                pt_wgt_element_span->ps_content, __LINE__, __func__, __FILE__));

                                    /* Check if the actual character length is equal to window column count */
                                    if(SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
                                    {
                                        z_crnt_p_str_len += z_str_len;
                                    }
                                }
                                
                            }

                             /* xml:space="preserve" */
                            else if(TRUE == b_tmp)
                            {
                                SIZE_T z_str_len = x_strlen((const CHAR*)pt_elem_span->ps_content);
                                
                                CHAR* ps_text = (CHAR*)x_mem_alloc(sizeof(CHAR) * (z_str_len + 1));
                                if(NULL == ps_text)
                                {
                                    DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
                                    IPCC_MEM_FREE(pt_wgt_element_p);
                                    IPCC_MEM_FREE(pt_wgt_element_span);

                                    return IPCCR_INSUFFICIENT_SIZE;
                                }
                                x_memset(ps_text, 0, (SIZE_T)(z_str_len + 1));  
                                x_strcpy(ps_text, (const CHAR*)pt_elem_span->ps_content);
                                pt_wgt_element_span->ps_content = ps_text;
                                b_is_crnt_span_effective = TRUE;

                                /* Check if the actual character length is equal to window column count */
                                if(SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
                                {
                                    z_crnt_p_str_len += z_str_len;
                                }

                            }

                            if(TRUE == b_is_crnt_ws_preserve_set)
                            {
                                b_is_crnt_ws_preserve_set = FALSE;
                            }

                            b_ws_preserve = FALSE;

                            /* Detect actual line number 
                             * This is for those files, which doesn't contains tts:exextent.  
                             * It may pop on and paint on mode.
                            */
                            if(FALSE == gb_is_extent_exist)
                            {
                                CHAR* ps_str = pt_elem_span->ps_content;
                                INT32 i4_line_count = 0;
                                INT32 i4_index = 0;

                                while('\0' != *ps_str)
                                {
                                    ++i4_index;
                                    
                                    if('\n' == *ps_str)
                                    {
                                        ++i4_line_count;
                                        i4_index = 0;
                                    }

                                    ++ps_str;
                                }

                                if(0 < i4_index)
                                {
                                    ++i4_line_count;
                                    i4_index = 0;
                                }

                                if(gat_wgt_wnd_attr[ui1_crnt_region_id].t_win_size.ui2_row_count != i4_line_count)
                                {
                                    gat_wgt_wnd_attr[ui1_crnt_region_id].t_win_size.ui2_row_count = i4_line_count;
                                }
                            }

                        }

                    }
                    if(NULL != pt_wgt_element_span && (TRUE == b_is_crnt_span_effective))
                    {
                        DLIST_INSERT_TAIL(pt_wgt_element_span, &(pt_wgt_element_p->t_element_span_list),t_element_span_link);
                        b_is_crnt_p_effective = TRUE;
                    }
                    
                    pt_elem_span = SLIST_NEXT(pt_elem_span, t_element_span_link);
                    b_is_crnt_span_effective = FALSE;
                }

                if(SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
                {
                    
                    if(gat_wgt_wnd_attr[ui1_crnt_region_id].t_win_size.ui2_col_count != (UINT16)z_crnt_p_str_len)
                    {
                        DBG_INFO(("{IPCC} Info: win_%u col count is %u, string length is %d!   <%d, %s, %s>\n\r", 
                                    ui1_crnt_region_id, gat_wgt_wnd_attr[ui1_crnt_region_id].t_win_size.ui2_col_count,
                                    z_crnt_p_str_len, __LINE__, __func__, __FILE__));
                        
                        gat_wgt_wnd_attr[ui1_crnt_region_id].t_win_size.ui2_col_count = (UINT16)z_crnt_p_str_len;
                        z_crnt_p_str_len = 0;
                    }
                }
            }
            if(NULL != pt_wgt_element_p && (TRUE == b_is_crnt_p_effective))
            {
                DLIST_INSERT_TAIL(pt_wgt_element_p, &(pt_wgt_ttml_data->t_element_p_list), t_element_p_link);
                ++pt_wgt_ttml_data->i4_ele_p_count; 
                b_is_crnt_wgt_data_effective = TRUE;

            }
            
            if(FALSE == gb_is_global_style_set)
            {
                gaui1_crnt_wnd[ui1_no_global_style_index] = *pui1_crnt_wnd;
                ++ui1_no_global_style_index;
            }
            
            ++pui1_crnt_wnd;
            pt_elem_p = SLIST_NEXT(pt_elem_p, t_element_p_link); 
            b_is_crnt_p_effective = FALSE;
        }
    }

    if(TRUE == b_is_crnt_wgt_data_effective)
    {
        i4_ret = sbtl_ipcc_wgt(pt_wgt_ttml_data);
        if(IPCCR_OK != i4_ret)
        {
            b_is_crnt_wgt_data_effective = FALSE;
            return i4_ret;
        }
    }
    
    b_is_crnt_wgt_data_effective = FALSE;
    return IPCCR_OK;
}
#endif

