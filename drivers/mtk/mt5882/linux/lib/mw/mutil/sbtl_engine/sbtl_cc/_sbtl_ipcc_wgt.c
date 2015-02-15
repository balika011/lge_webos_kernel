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
                    define, data, enum, struct
-----------------------------------------------------------------------------*/
#define SBTL_IPCC_SRVC_BLK_DATA_SIZE      60
#define SBTL_IPCC_SRVC_WGT_DATA_SIZE      32



static BOOL                            gb_sbtl_cc_dbg_z_order  = FALSE;
static SM_CCH_WGT_ORDER_T              ge_ipcc_wgt_order       = SM_CCH_WGT_ORDER_TOPMOST;
static SM_CC_ASPECT_RATIO_T            ge_aspect_ratio         = SM_CC_ASPECT_RATIO_UNKNOWN;

static WGL_CC_INIT_INFO_T              gt_ipcc_info_initor;
static UINT32                          gui4_org_canvas_w       = 0;
static UINT32                          gui4_org_canvas_h       = 0;



/* Default colors */
static const GL_COLOR_T        t_black              = CC_COLOR_BLACK;
static const GL_COLOR_T        t_white              = CC_COLOR_WHITE;
static const GL_COLOR_T        t_transparent        = CC_COLOR_TRANSPARENT;

/* User customized attributes */
static SM_CCH_DTVCC_ATTRIBS_INFO_T t_sbtl_ipcc_attr = 
{
    SM_CCH_DTVCC_ATT_FLAG_ALL_AS_BRDCST,
    SM_CCH_DTVCC_FONT_STYLE_MONO_SP_WITHOUT_SERIFS,
    SM_CCH_DTVCC_FONT_SIZE_MEDIUM,
    { 0, 0, 0, SM_CCH_DTVCC_OPACITY_SOLID },
    { 0, 0, 0, SM_CCH_DTVCC_OPACITY_SOLID },
    { 0, 0, 0, SM_CCH_DTVCC_OPACITY_SOLID },
    SM_CCH_DTVCC_EDGE_TYPE_NONE,
    { 0, 0, 0, SM_CCH_DTVCC_OPACITY_SOLID },
    { 0, 0, 0, SM_CCH_DTVCC_OPACITY_SOLID },
    SM_CCH_DTVCC_EDGE_TYPE_NONE,
    SM_CCH_DTVCC_JUSTIFY_LEFT
};


static const CHAR*             as_font_style_name[IPCC_MAX_PEN_STL_NUM] =
{
    SM_CC_FONT_STYLE_0,   /* default */
    SM_CC_FONT_STYLE_1,
    SM_CC_FONT_STYLE_2,
    SM_CC_FONT_STYLE_3,
    SM_CC_FONT_STYLE_4,
    SM_CC_FONT_STYLE_5,
    SM_CC_FONT_STYLE_6,
    SM_CC_FONT_STYLE_7   
};



static const WGL_CC_WIN_ATTR_T at_wnd_style[IPCC_MAX_PEN_STL_NUM] = 
{
    { /* style 0 */
        WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
        WGL_CC_JUSTIFY_LEFT,
        WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
        WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    },
    { /* style 1 */
        WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
        WGL_CC_JUSTIFY_LEFT,
        WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
        WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    },
    { /* style 2 */
        WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
        WGL_CC_JUSTIFY_LEFT,
        WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
        WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    },
    { /* style 3 */
        WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL, 
        WGL_CC_JUSTIFY_CENTER,
        WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
        WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    },
    { /* style 4 */
        WGL_CC_WIN_ATTR_WORD_WRAP | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
        WGL_CC_JUSTIFY_LEFT,
        WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
        WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    },
    { /* style 5 */
        WGL_CC_WIN_ATTR_WORD_WRAP | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
        WGL_CC_JUSTIFY_LEFT,
        WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
        WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    },
    { /* style 6 */
        WGL_CC_WIN_ATTR_WORD_WRAP | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
        WGL_CC_JUSTIFY_CENTER,
        WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
        WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    },
    { /* style 7 */
        WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
        WGL_CC_JUSTIFY_LEFT,
        WGL_CC_PRINT_DIR_TOP_TO_BOTTOM,
        WGL_CC_SCROLL_DIR_RIGHT_TO_LEFT,
        WGL_CC_DISPLAY_EFFECT_SNAP,
        0,
        0,
        CC_COLOR_TRANSPARENT,
        FALSE,
        WGL_CC_BORDER_TYPE_NONE,
        CC_COLOR_BLACK
    }
};





static const WGL_CC_PEN_FONT_T at_pen_att[IPCC_MAX_PEN_STL_NUM] = 
{
    { /* style 0 */
        {
            SM_CC_FONT_STYLE_0,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_NONE
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    },
    { /* style 1 */
        {
            SM_CC_FONT_STYLE_0,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_NONE
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    },
    { /* style 2 */
        {
            SM_CC_FONT_STYLE_1,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_NONE
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    },
    { /* style 3 */
        {
            SM_CC_FONT_STYLE_2,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_NONE
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    },
    { /* style 4 */
        {
            SM_CC_FONT_STYLE_3,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_NONE
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    },
    { /* style 5 */
        {
            SM_CC_FONT_STYLE_4,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_NONE
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    },
    { /* style 6 */
        {
            SM_CC_FONT_STYLE_3,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_UNIFORM
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    },
    { /* style 7 */
        {
            SM_CC_FONT_STYLE_4,
            FE_FNT_SIZE_MEDIUM, 
            WGL_CC_FNT_STYLE_REGULAR,
            FE_CMAP_ENC_UNICODE,
            WGL_CC_EDGE_TYPE_UNIFORM
        },
        WGL_CC_TEXT_OFFSET_NORMAL
    }      
};

static const WGL_CC_PEN_COLOR_T at_pen_color[IPCC_MAX_PEN_STL_NUM] = 
{
    { /* style 0 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_BLACK,               /* background color */
        CC_COLOR_TRANSPARENT,         /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    },
    { /* style 1 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_BLACK,               /* background color */
        CC_COLOR_TRANSPARENT,         /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    },
    { /* style 2 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_BLACK,               /* background color */
        CC_COLOR_TRANSPARENT,         /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    },
    { /* style 3 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_BLACK,               /* background color */
        CC_COLOR_TRANSPARENT,         /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    },
    { /* style 4 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_BLACK,               /* background color */
        CC_COLOR_TRANSPARENT,         /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    },
    { /* style 5 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_BLACK,               /* background color */
        CC_COLOR_TRANSPARENT,         /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    },
    { /* style 6 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_TRANSPARENT,         /* background color */
        CC_COLOR_BLACK,               /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    },
    { /* style 7 */
        CC_COLOR_WHITE,               /* foreground color */
        CC_COLOR_TRANSPARENT,         /* background color */
        CC_COLOR_BLACK,               /* edge color */
        FALSE,                        /* foreground is flash */
        FALSE                         /* background is flash */
    }
};



typedef struct _SBTL_IPCC_SCROLL_T
{
    WGL_CC_TEXT_PARAG_LOCATION_T   t_parag_loc;
    BOOL                           b_cr;
} SBTL_IPCC_SCROLL_T;


static SBTL_IPCC_SCROLL_T t_scroll[IPCC_MAX_PEN_STL_NUM] = 
{
    {
        {0xFFFF, 0xFFFF},
        FALSE
    },
    {
        {0xFFFF, 0xFFFF},
        FALSE
    },
    {
        {0xFFFF, 0xFFFF},
        FALSE
    },
    {
        {0xFFFF, 0xFFFF},
        FALSE
    },
    {
        {0xFFFF, 0xFFFF},
        FALSE
    },
    {
        {0xFFFF, 0xFFFF},
        FALSE
    },
    {
        {0xFFFF, 0xFFFF},
        FALSE
    },
    {
        {0xFFFF, 0xFFFF},
        FALSE
    }
};

#ifndef ________________________________________SPLIT_LINE_____________
#endif

/*-----------------------------------------------------------------------------
                    function declaration
-----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_pen_loc
( 
    IN  SBTL_IPCC_WGT_T*                pt_ipcc_wgt,
    IN  WGL_CC_TEXT_PARAG_LOCATION_T*   pt_pen_loc,
    IN  UINT8                           ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_pen_color
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_PEN_COLOR_T*    pt_pen_color,
    IN  UINT8                  ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_pen_font
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_PEN_FONT_T*     pt_pen_font,
    IN  UINT8                  ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_pen_style
(
    IN  SBTL_IPCC_WGT_T* pt_ipcc_wgt,
    IN  UINT8            ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_win_anchor
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_WIN_ANCHOR_T*   pt_win_anchor,
    IN  UINT8                  ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_win_size
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_WIN_SIZE_T*     pt_win_size,
    IN  UINT8                  ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_win_attr
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_WIN_ATTR_T*     pt_win_attr,
    IN  UINT8                  ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_wnd_style
(
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt,
    IN  UINT8               ui1_wnd_num
);

static INT32 _sbtl_ipcc_wgt_move_pen_according_to_scroll_n_print_dir
(
 IN SBTL_IPCC_WGT_T*  pt_ipcc_wgt,
 IN UINT8             ui1_wnd_num 
);

static INT32 _sbtl_ipcc_wgt_set_text_tag
(
    SBTL_IPCC_WGT_T * pt_ipcc_wgt,
    UINT8 * pui1_text_tag,
    UINT8 ui1_wnd_num
);

static INT32 _sbtl_ipcc_wgt_text
(
    IN  SBTL_IPCC_WGT_T* pt_ipcc_wgt,
    IN  CHAR*            ps_text
);

static INT32 _sbtl_ipcc_wgt_create_wnd
( 
    IN  HANDLE_T                   h_frame,
    IN  const WGL_CC_INIT_INFO_T*  pt_cc_info,
    IN  HANDLE_T*                  ph_cc_wnd 
);

static INT32 _sbtl_ipcc_wgt_display_wnd
(
    IN  UINT32              ui4_crnt_wnd, 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
);

static INT32 _sbtl_ipcc_wgt_hide_wnd
(
    IN  UINT32              ui4_crnt_wnd, 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
);

static INT32 _sbtl_ipcc_wgt_clear_wnd
(
    IN  UINT32              ui4_crnt_wnd, 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
);

static INT32 _sbtl_ipcc_wgt_define_wnd
(
    IN  UINT32              ui4_crnt_wnd,  
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
);

static INT32 _sbtl_ipcc_wgt_destroy_wnd( IN HANDLE_T h_cc_wnd );

static VOID _sbtl_ipcc_wgt_reset_cc_info_initor
( 
    IN  UINT32  ui4_org_canvas_w,
    IN  UINT32  ui4_org_canvas_h 
);

static INT32 _sbtl_ipcc_wgt_chg_z_order
( 
    SBTL_IPCC_WGT_T*                pt_ipcc_wgt,
    UINT8                          ui1_wnd_num,
    UINT8                          ui1_hdend_z_order 
);

static VOID _sbtl_ipcc_wgt_set_globle_org_canvas
(
    IN    UINT32    ui4_canvas_w,
    IN    UINT32    ui4_canvas_h 
);

static INT32 _sbtl_ipcc_wgt_flush_text
( 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt,
    IN  UTF16_T*            pwc_text,
    IN  SIZE_T              z_text_len 
);

static FE_FNT_SIZE _sbtl_ipcc_wgt_conv_font_size( IN UINT8 ui1_size );

static UINT8 _sbtl_ipcc_wgt_conv_font_edge_type( IN UINT8 ui1_edge_type );

static VOID _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color
( 
    IN  const SM_CCH_DTVCC_COLOR_T*      pt_cch_color,
    OUT GL_COLOR_T*                      pt_gl_color,
    OUT BOOL*                            pb_is_flash 
);

static UINT8 _sbtl_ipcc_wgt_conv_hdlr_justify_2_cc_wgt_justify
(
    IN SM_CCH_DTVCC_JUSTIFY_T e_cch_justify 
);

INT32 _sbtl_ipcc_wgt_set_order( VOID );

static SM_CC_ASPECT_RATIO_T _sbtl_ipcc_wgt_get_asp_ratio( VOID );

static VOID _sbtl_ipcc_wgt_update_attr(IN OUT SBTL_IPCC_WGT_T*    pt_ipcc_wgt);


#ifndef _______________________________________SPLIT_LINE_____________
#endif



/*-----------------------------------------------------------------------------
                    function define
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_pen_loc
 *
 * Description: This API is used to set pen location.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pt_pen_loc           - Pen location
 *          ui1_wnd_num          - Number of window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_pen_loc
( 
    IN  SBTL_IPCC_WGT_T*                pt_ipcc_wgt,
    IN  WGL_CC_TEXT_PARAG_LOCATION_T*   pt_pen_loc,
    IN  UINT8                           ui1_wnd_num 
)
{
    INT32                          i4_ret = SMR_OK;
    WGL_CC_TEXT_PARAG_LOCATION_T   t_parag_loc;

    if ( ui1_wnd_num >= (UINT8) IPCC_MAX_WGT_NUM )
    {
        /* current window undefine, ignore */
        return IPCCR_OK;   
    }

    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR((IPCC_PREFIX"ERR: pt_ipcc_wgt is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG; 
    }
    
    t_parag_loc.ui2_idx_parag = pt_pen_loc->ui2_idx_parag;
    t_parag_loc.ui2_col = pt_pen_loc->ui2_col;
    
    if(t_scroll[ui1_wnd_num].t_parag_loc.ui2_idx_parag == t_parag_loc.ui2_idx_parag &&
       //t_scroll[ui1_wnd_num].t_parag_loc.ui2_col == t_parag_loc.ui2_col && 
       t_scroll[ui1_wnd_num].b_cr)
    {
        i4_ret = IPCCR_OK;
        i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui1_wnd_num],
                               WGL_CMD_CC_MOVE_PEN_TO_COL, 
                               (VOID*)(UINT32)t_parag_loc.ui2_col,
                               (VOID*)WGL_CC_MOVE_MODE_FILL_NULL_SPACE );
        
        //t_scroll[ui1_wnd_num].b_cr = FALSE;
    }
    else
    {
        i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui1_wnd_num],
                           WGL_CMD_CC_SET_PEN_VISIBLE_PARAG_LOCATION, 
                           (VOID*)&t_parag_loc,
                           (VOID*)WGL_CC_MOVE_MODE_FILL_NULL_SPACE );
        t_scroll[ui1_wnd_num].t_parag_loc.ui2_idx_parag = t_parag_loc.ui2_idx_parag;
        t_scroll[ui1_wnd_num].t_parag_loc.ui2_col = t_parag_loc.ui2_col; 
        t_scroll[ui1_wnd_num].b_cr = FALSE;
       
    }
    if ( i4_ret < WGLR_OK )
    {
        DBG_ERROR(( 
            IPCC_PREFIX"x_wgl_do_cmd(%d,WGL_CMD_CC_SET_PEN_VISIBLE_PARAG_LOCATION) ERROR, return %d\r\n",
            ui1_wnd_num, i4_ret ));
        return IPCCR_INTERNAL_ERROR;
    }

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_pen_color
 *
 * Description: This API is used to set pen color
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pt_pen_color         - Pen color
 *          ui1_wnd_num          - Number of window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_pen_color
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_PEN_COLOR_T*    pt_pen_color,
    IN  UINT8                  ui1_wnd_num 
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_pen_color,IPCCR_INV_ARG);  
    INT32 i4_ret = IPCCR_OK;

    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.b_is_flash_bk = pt_pen_color->b_is_flash_bk;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.b_is_flash_fg = pt_pen_color->b_is_flash_fg;
    
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.a = pt_pen_color->t_color_bk.a;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.u1.r = pt_pen_color->t_color_bk.u1.r;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.u2.g = pt_pen_color->t_color_bk.u2.g;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.u3.b = pt_pen_color->t_color_bk.u3.b;

    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.a = pt_pen_color->t_color_edge.a;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.u1.r = pt_pen_color->t_color_edge.u1.r;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.u2.g = pt_pen_color->t_color_edge.u2.g;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.u3.b = pt_pen_color->t_color_edge.u3.b;    

    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.a = pt_pen_color->t_color_text.a;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.u1.r = pt_pen_color->t_color_text.u1.r;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.u2.g = pt_pen_color->t_color_text.u2.g;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.u3.b = pt_pen_color->t_color_text.u3.b; 

    i4_ret = x_wgl_do_cmd( pt_sbtl_ipcc_wgt->ah_cc_wnd[ui1_wnd_num],
                           WGL_CMD_CC_SET_PEN_COLOR, 
                           (VOID*)&pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color,
                           (VOID*)NULL );
    
    if ( i4_ret < WGLR_OK )
    {
        DBG_ERROR(( "[SM] x_wgl_do_cmd(WGL_CMD_CC_SET_PEN_COLOR) ERROR, return %d\r\n",i4_ret ));
        return IPCCR_INTERNAL_ERROR;
    }

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_pen_font
 *
 * Description: This API is used to set pen attributes.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pt_pen_font          - Pen font
 *          ui1_wnd_num          - Number of window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_pen_font
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_PEN_FONT_T*     pt_pen_font,
    IN  UINT8                  ui1_wnd_num 
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_pen_font,IPCCR_INV_ARG);  

    INT32 i4_ret = IPCCR_OK;

    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.ui1_offset             = pt_pen_font->ui1_offset;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.e_cmap          = pt_pen_font->t_font.e_cmap;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.e_size          = pt_pen_font->t_font.e_size;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.e_style         = pt_pen_font->t_font.e_style;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.ui1_edge_type   = pt_pen_font->t_font.ui1_edge_type;
    x_strcpy(
                (CHAR*)&pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.s_font_name, 
                (const CHAR*)pt_pen_font->t_font.s_font_name
            );

    i4_ret = x_wgl_do_cmd( pt_sbtl_ipcc_wgt->ah_cc_wnd[ui1_wnd_num],
                           WGL_CMD_CC_SET_PEN_FONT, 
                           (VOID*)&pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font,
                           (VOID*)NULL );   

    if ( i4_ret < WGLR_OK )
    {
        DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_SET_PEN_FONT) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
        return IPCCR_INTERNAL_ERROR;
    }

    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_pen_style
 *
 * Description: <function description>
 *
 * Inputs:  pt_dtv_wgt    References.
 *          ui1_wnd_num   References.
 *
 * Outputs: - 
 *
 * Returns: SMR_INV_ARG  Success.
 *          SMR_OK       Fail.
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_pen_style
(
    IN  SBTL_IPCC_WGT_T* pt_ipcc_wgt,
    IN  UINT8            ui1_wnd_num 
)
{
    if ( ui1_wnd_num >= (UINT8) IPCC_MAX_WGT_NUM || !pt_ipcc_wgt)
    {
        return IPCCR_INV_ARG;   
    }  
    
    //pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color;
    //pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font  = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_font;

    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.b_is_flash_bk = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.b_is_flash_bk;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.b_is_flash_fg = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.b_is_flash_fg;
    
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.a = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_bk.a;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.u1.r = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_bk.u1.r;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.u2.g = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_bk.u2.g;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_bk.u3.b = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_bk.u3.b;

    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.a = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_edge.a;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.u1.r = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_edge.u1.r;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.u2.g = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_edge.u2.g;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_edge.u3.b = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_edge.u3.b;

    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.a = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_text.a;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.u1.r = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_text.u1.r;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.u2.g = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_text.u2.g;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_color.t_color_text.u3.b = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_color.t_color_text.u3.b;


    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.ui1_offset = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_font.ui1_offset;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.e_cmap = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_font.t_font.e_cmap;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.e_size = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_font.t_font.e_size;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.e_style = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_font.t_font.e_style;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.ui1_edge_type = gat_wgt_wnd_attr[ui1_wnd_num].t_pen_font.t_font.ui1_edge_type;
    x_strcpy((CHAR*)pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_pen_font.t_font.s_font_name, (const CHAR*)gat_wgt_wnd_attr[ui1_wnd_num].t_pen_font.t_font.s_font_name);
        
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_win_anchor
 *
 * Description: This API is used to set window anchor
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pt_win_anchor        - Window anchor
 *          ui1_wnd_num          - Number of window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_win_anchor
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_WIN_ANCHOR_T*   pt_win_anchor,
    IN  UINT8                  ui1_wnd_num 
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_win_anchor,IPCCR_INV_ARG);  
    
    
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].ui1_anchor    = pt_win_anchor->ui1_anchor;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_anchor_h  = pt_win_anchor->ui2_anchor_h;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_anchor_v  = pt_win_anchor->ui2_anchor_v;
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_win_size
 *
 * Description: This API is used to set window size
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pt_win_size          - Window size
 *          ui1_wnd_num          - Number of window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_win_size
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_WIN_SIZE_T*     pt_win_size,
    IN  UINT8                  ui1_wnd_num 
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt, IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_win_size, IPCCR_INV_ARG); 
   
    
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_row_count = pt_win_size->ui2_row_count;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_col_count = pt_win_size->ui2_col_count;
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_win_attr
 *
 * Description: This API is used to set window attributes.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pt_win_attr          - Window attributes
 *          ui1_wnd_num          - Number of window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_win_attr
(
    IN  SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN  WGL_CC_WIN_ATTR_T*     pt_win_attr,
    IN  UINT8                  ui1_wnd_num 
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pt_win_attr,IPCCR_INV_ARG);  

    INT32 i4_ret = IPCCR_OK;

    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.b_is_flash_bk                  = pt_win_attr->b_is_flash_bk;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_border_type                = pt_win_attr->ui1_border_type;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_display_effect             = pt_win_attr->ui1_display_effect;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_display_effect_dir         = pt_win_attr->ui1_display_effect_dir;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_justify                    = pt_win_attr->ui1_justify;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_print_dir                  = pt_win_attr->ui1_print_dir;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_scroll_dir                 = pt_win_attr->ui1_scroll_dir;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui2_win_attr                   = pt_win_attr->ui2_win_attr;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui4_display_effect_duration    = pt_win_attr->ui4_display_effect_duration;

    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.a   = pt_win_attr->t_color_bk.a;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.u1  = pt_win_attr->t_color_bk.u1; 
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.u2  = pt_win_attr->t_color_bk.u2;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.u3  = pt_win_attr->t_color_bk.u3;

    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.a   = pt_win_attr->t_color_border.a;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.u1  = pt_win_attr->t_color_border.u1;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.u2  = pt_win_attr->t_color_border.u2;
    pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.u3  = pt_win_attr->t_color_border.u3;

    i4_ret = x_wgl_do_cmd( pt_sbtl_ipcc_wgt->ah_cc_wnd[ui1_wnd_num],
                           WGL_CMD_CC_SET_WIN_ATTR, 
                           (VOID*)&pt_sbtl_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr,
                           (VOID*)TRUE );
    
    if ( i4_ret < WGLR_OK )
    {
        DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_SET_WIN_ATTR) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
        return IPCCR_INTERNAL_ERROR;
    }

    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_set_wnd_style
 *
 * Description: <function description>
 *
 * Inputs:  pt_dtv_wgt    References.
 *          ui1_wnd_num   References.
 *          b_new_wnd     References
 *          ui1_wnd_style_old  References.
 *          ui1_wnd_style_new  References.
 *
 * Outputs: - 
 *
 * Returns: SMR_INV_ARG  Success.
 *          SMR_OK       Fail.
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_set_wnd_style
(
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt,
    IN  UINT8               ui1_wnd_num
)
{
    if ( ui1_wnd_num >= (UINT8) IPCC_MAX_WGT_NUM || !pt_ipcc_wgt)
    {
        return IPCCR_INV_ARG;   
    }

    //pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.b_is_flash_bk 
                        = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.b_is_flash_bk;

    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.a = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_bk.a;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.u1.r = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_bk.u1.r;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.u2.g = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_bk.u2.g;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_bk.u3.b = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_bk.u3.b;
    
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.a = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_border.a;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.u1.r = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_border.u1.r;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.u2.g = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_border.u2.g;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.t_color_border.u3.b = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.t_color_border.u3.b;

    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_border_type = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui1_border_type;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_display_effect = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui1_display_effect;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_display_effect_dir = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui1_display_effect_dir;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_justify = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui1_justify;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_print_dir = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui1_print_dir;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui1_scroll_dir = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui1_scroll_dir;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui2_win_attr = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui2_win_attr;
    pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr.ui4_display_effect_duration = gat_wgt_wnd_attr[ui1_wnd_num].t_win_attr.ui4_display_effect_duration;
    
    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
* Name: _sbtl_ipcc_wgt_move_pen_according_to_scroll_n_print_dir
*
* Description: This API is used to check the current character is white space or not
*
* Inputs:  c_char            - Current character
*
* Outputs: pt_tmp_color      - Color
*
* Returns: IPCCR_OK              - Success
*          IPCCR_INV_ARG         - Invalid para
*
----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_move_pen_according_to_scroll_n_print_dir
(
 IN SBTL_IPCC_WGT_T*  pt_ipcc_wgt,
 IN UINT8             ui1_wnd_num 
)
{
    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR((IPCC_PREFIX"ERR: pt_ipcc_wgt is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG; 
    }
    
    WGL_CC_TEXT_PARAG_LOCATION_T  t_pen_loc;
    WGL_CC_WIN_ATTR_T*     pt_wgt_wnd_att = &pt_ipcc_wgt->at_cc_info[ui1_wnd_num].t_win_attr;
    
    if ( pt_wgt_wnd_att->ui2_win_attr & WGL_CC_WIN_ATTR_AUTO_SCROLL )
    {
        if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP &&
             pt_wgt_wnd_att->ui1_print_dir == WGL_CC_PRINT_DIR_LEFT_TO_RIGHT )
        {
            t_pen_loc.ui2_idx_parag= pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_row_count - 1;
            t_pen_loc.ui2_col= 0;
        }
        else if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP &&
                  pt_wgt_wnd_att->ui1_print_dir == WGL_CC_PRINT_DIR_RIGHT_TO_LEFT )
        {
            t_pen_loc.ui2_idx_parag = pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_row_count - 1;
            t_pen_loc.ui2_col = pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_col_count - 1;
        }
        else if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_TOP_TO_BOTTOM &&
                  pt_wgt_wnd_att->ui1_print_dir == WGL_CC_PRINT_DIR_LEFT_TO_RIGHT )
        {
            t_pen_loc.ui2_idx_parag = 0;
            t_pen_loc.ui2_col = 0;
        }
        else if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_TOP_TO_BOTTOM &&
                  pt_wgt_wnd_att->ui1_print_dir == WGL_CC_PRINT_DIR_RIGHT_TO_LEFT )
        {
            t_pen_loc.ui2_idx_parag = 0;
            t_pen_loc.ui2_col = pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_col_count - 1;
        }
        else if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_LEFT_TO_RIGHT &&
                  pt_wgt_wnd_att->ui1_print_dir == WGL_CC_PRINT_DIR_TOP_TO_BOTTOM )
        {
            t_pen_loc.ui2_idx_parag = 0;
            t_pen_loc.ui2_col = 0;
        }
        else if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_LEFT_TO_RIGHT &&
                  pt_wgt_wnd_att->ui1_print_dir == WGL_CC_PRINT_DIR_BOTTOM_TO_TOP )
        {
            t_pen_loc.ui2_idx_parag = pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_row_count - 1;
            t_pen_loc.ui2_col = 0;
        }
        else if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_RIGHT_TO_LEFT &&
                  pt_wgt_wnd_att->ui1_print_dir == WGL_CC_SCROLL_DIR_TOP_TO_BOTTOM )
        {
            t_pen_loc.ui2_idx_parag = 0;
            t_pen_loc.ui2_col = pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_col_count - 1;
        }
        else if ( pt_wgt_wnd_att->ui1_scroll_dir == WGL_CC_SCROLL_DIR_RIGHT_TO_LEFT &&
                  pt_wgt_wnd_att->ui1_print_dir == WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP )
        {
            t_pen_loc.ui2_idx_parag = pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_row_count - 1;
            t_pen_loc.ui2_col = pt_ipcc_wgt->at_cc_info[ui1_wnd_num].ui2_col_count - 1;
        }
        else
        {
            t_pen_loc.ui2_idx_parag = 0;
            t_pen_loc.ui2_col = 0;
        }
    }
    else
    {
        t_pen_loc.ui2_idx_parag = 0;
        t_pen_loc.ui2_col = 0;
    }
    
    _sbtl_ipcc_wgt_set_pen_loc( pt_ipcc_wgt, &t_pen_loc, ui1_wnd_num );       
    return IPCCR_OK;   
}


static INT32 _sbtl_ipcc_wgt_set_text_tag
(
    SBTL_IPCC_WGT_T * pt_ipcc_wgt,
    UINT8 * pui1_text_tag,
    UINT8 ui1_wnd_num
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pui1_text_tag,IPCCR_INV_ARG);      

    pt_ipcc_wgt->aui1_text_tag[ui1_wnd_num] = *pui1_text_tag;

    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_text
 *
 * Description: This API is used to process text.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          ps_text              - Text
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_text
(
    IN  SBTL_IPCC_WGT_T* pt_ipcc_wgt,
    IN  CHAR*            ps_text
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(ps_text,IPCCR_INV_ARG);  


    INT32 i4_ret            = IPCCR_OK;
    UINT32 ui4_count        = 0;
    SIZE_T z_text_buff_len  = 0;
    CHAR* ps_str            = ps_text;
    SIZE_T z_str_len        = x_strlen((const CHAR*)ps_text);
    UTF16_T   awc_text_buff[SBTL_IPCC_SRVC_WGT_DATA_SIZE + 1]={0};


    if ( pt_ipcc_wgt->aui1_text_tag[pt_ipcc_wgt->ui1_curr_wnd_num] ==
         CAP_CMD_TEXT_TAG_NOT_TO_BE_DISP )
    {
        /* If text tag is "not to be display", ignore the text */
        return IPCCR_OK;
    } 

    t_scroll[pt_ipcc_wgt->ui1_curr_wnd_num].b_cr = FALSE;


    CHAR* ps_dst_str = (CHAR*)x_mem_alloc(SBTL_IPCC_SRVC_WGT_DATA_SIZE * sizeof(CHAR));
    if(NULL == ps_dst_str)
    {
        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_memset(ps_dst_str, 0, sizeof(CHAR)*SBTL_IPCC_SRVC_WGT_DATA_SIZE );

    UINT32 ui4_last_index = 0;
    UINT32 ui4_crnt_index = 0;
    
    for(ui4_crnt_index = 0; ui4_crnt_index < z_str_len; ++ui4_crnt_index)
    {
        if('\n' == *(ps_str + ui4_crnt_index))
        {      
            if((ui4_crnt_index - ui4_last_index) > 0)
            {
                z_text_buff_len = (SIZE_T)(ui4_crnt_index - ui4_last_index);
				if(SBTL_IPCC_SRVC_WGT_DATA_SIZE < z_text_buff_len)
				{
					DBG_LOG_PRINT(("\n [IPCC WGT] Text len too large %d,%d\n",z_str_len,__LINE__));
					z_text_buff_len = SBTL_IPCC_SRVC_WGT_DATA_SIZE;
				}
                x_strncpy(ps_dst_str, (const CHAR*)(ps_str + ui4_last_index), (SIZE_T)z_text_buff_len);
                i4_ret = x_uc_ps_to_w2s((const CHAR*) ps_dst_str, (UTF16_T*)awc_text_buff, (UINT32)z_text_buff_len);
                if(UCR_OK != i4_ret)
                {
                    return i4_ret;
                }
                else
                {
//                    DBG_INFO(( "{IPCC} INFO convert to utf-16 success.   <%d, %s, %s>\r\n",__LINE__, __func__, __FILE__ ));
                }
                x_memset(ps_dst_str, 0, sizeof(CHAR)*SBTL_IPCC_SRVC_WGT_DATA_SIZE );

                i4_ret = _sbtl_ipcc_wgt_flush_text(pt_ipcc_wgt, awc_text_buff, z_text_buff_len);
                if(IPCCR_OK != i4_ret)
                {
                    IPCC_MEM_FREE(ps_dst_str);
                    return i4_ret;
                }
            }  

            /* CR cmd */
            i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ pt_ipcc_wgt->ui1_curr_wnd_num ],
                                   WGL_CMD_CC_MOVE_PEN_TO_NEXT_PARAG, 
                                   (VOID*)FALSE,
                                   (VOID*)NULL );
            if ( i4_ret < WGLR_OK )
            {
                DBG_ERROR(( "{IPCC} ERR: x_wgl_do_cmd(WGL_CMD_CC_MOVE_PEN_TO_NEXT_PARAG) ERROR, return %d   <%d, %s, %s>\r\n", i4_ret,__LINE__, __func__, __FILE__ ));
                IPCC_MEM_FREE(ps_dst_str);
                return IPCCR_INTERNAL_ERROR;
            }
            else
            {
 //               DBG_INFO(( "{IPCC} INFO x_wgl_do_cmd(WGL_CMD_CC_MOVE_PEN_TO_NEXT_PARAG) success.   <%d, %s, %s>\r\n",__LINE__, __func__, __FILE__ ));
            }
                
            i4_ret = x_wgl_do_cmd( 
                                    pt_ipcc_wgt->ah_cc_wnd[ pt_ipcc_wgt->ui1_curr_wnd_num ],
                                    WGL_CMD_CC_UPDATE, 
                                    (VOID*)WGL_CC_UPDATE_MODE_AUTO_SMTH_SCRL,
                                    (VOID*)NULL );
            if ( i4_ret < WGLR_OK )
            {
                DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_UPDATE) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
                IPCC_MEM_FREE(ps_dst_str);
                return IPCCR_INTERNAL_ERROR;
            }
            ui4_last_index = ui4_crnt_index;
            ++ui4_last_index;
            ++ui4_count;
        }
    }


    /* Process the last string */
    if((ui4_crnt_index - ui4_last_index) > 0)
    {
        z_text_buff_len = (SIZE_T)(ui4_crnt_index - ui4_last_index);
		if(SBTL_IPCC_SRVC_WGT_DATA_SIZE < z_text_buff_len)
		{
			DBG_LOG_PRINT(("\n [IPCC WGT] Text len too large %d,%d\n",z_str_len,__LINE__));
			z_text_buff_len = SBTL_IPCC_SRVC_WGT_DATA_SIZE;
		}
        x_strncpy(ps_dst_str, (const CHAR*)(ps_str + ui4_last_index), (SIZE_T)z_text_buff_len);
        i4_ret = x_uc_ps_to_w2s((const CHAR*) ps_dst_str, (UTF16_T*)awc_text_buff, (UINT32)z_text_buff_len);
        if(UCR_OK != i4_ret)
        {
            return i4_ret;
        }
        else
        {
//            DBG_INFO(( "{IPCC} INFO convert to utf-16 success.   <%d, %s, %s>\r\n",__LINE__, __func__, __FILE__ ));
        }
        x_memset(ps_dst_str, 0, sizeof(CHAR)*SBTL_IPCC_SRVC_WGT_DATA_SIZE );

        i4_ret = _sbtl_ipcc_wgt_flush_text(pt_ipcc_wgt, awc_text_buff, z_text_buff_len);
        if(IPCCR_OK != i4_ret)
        {
            return i4_ret;
        }
    } 

    ++ui4_count;
    
    if(SBTL_TTML_ATTR_TEXT_ALIGIN_END == gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].e_text_align)
    {
        if(pt_sbtl_ipcc_wgt->at_cc_info[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].ui2_row_count >= (UINT16)ui4_count)
        {
            gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].t_pen_loc.ui2_idx_parag = 
                            (UINT16)(pt_sbtl_ipcc_wgt->at_cc_info[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].ui2_row_count - (UINT16)ui4_count);
            gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].t_pen_loc.ui2_col = 0;
        }
    }

    IPCC_MEM_FREE(ps_dst_str);    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_create_wnd
 *
 * Description: This API is used to create window.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pt_cc_info           - The window info
 *          ph_cc_wnd            - The handle of window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_create_wnd
( 
    IN  HANDLE_T                   h_frame,
    IN  const WGL_CC_INIT_INFO_T*  pt_cc_info,
    IN  HANDLE_T*                  ph_cc_wnd 
)
{
    INT32                   i4_ret;
    GL_RECT_T               t_rect;    

    /* ignore the rectangle, it will be override by anchor point settings */
    SET_RECT_BY_SIZE(&t_rect, 0, 0, 0, 0);
    
    i4_ret = x_wgl_create_widget(
        h_frame,
        HT_WGL_WIDGET_CC,
        WGL_CONTENT_CC_DEF,
        WGL_BORDER_NULL,
        &t_rect,
        NULL,
        255,
        (VOID*)pt_cc_info,
        NULL,
        ph_cc_wnd );
    if ( i4_ret < WGLR_OK )
    {
        return IPCCR_INTERNAL_ERROR;
    }
    
    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_display_wnd
 *
 * Description: This API is used to display window.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          ui4_crnt_wnd         - The number of current window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_display_wnd
(
    IN  UINT32              ui4_crnt_wnd, 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
)
{
    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR(("\n{IPCC} ERR: pt_ipcc_wgt is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG; 
    }
    
    INT32 i4_ret = IPCCR_OK;;

    i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_crnt_wnd],
                           WGL_CMD_CC_SHOW, 
                           (VOID*)TRUE,
                           (VOID*)TRUE );
    if(i4_ret < WGLR_OK )
    {
        DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_SHOW) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
        return IPCCR_INTERNAL_ERROR;
    }

    pt_ipcc_wgt->at_cc_info[ui4_crnt_wnd].ui2_cc_info_flags |= WGL_CC_INFO_VISIBLE;

    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_hide_wnd
 *
 * Description: This API is used to hide window.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          ui4_crnt_wnd         - The number of current window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_hide_wnd
(
    IN  UINT32              ui4_crnt_wnd, 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
)
{
    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR(("\n{IPCC} ERR: pt_ipcc_wgt is NULL!   <%d, %s, %s>\n\r",  
                    __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG; 
    }

    UINT32 ui4_i = 0;
    UINT32 ui4_tmp_i = 0;
    INT32 i4_ret = IPCCR_OK;

    if(0xFF == ui4_crnt_wnd)
    {
        ui4_i = 0;

        for(; ui4_i<IPCC_MAX_WGT_NUM; ++ui4_i)
        {
            if(TRUE == gb_is_enhance_rollup)
            {
                ui4_tmp_i = ui4_i;
                ui4_i = IPCC_MAX_WGT_NUM - ui4_tmp_i - 1;
            }
            
            i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                                                   WGL_CMD_CC_SHOW, 
                                                   (VOID*)FALSE,
                                                   (VOID*)TRUE );
            if(i4_ret < WGLR_OK )
            {
                DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_SHOW) ERROR, return %d   <%d, %s, %s>\r\n",
                            i4_ret,__LINE__, __func__, __FILE__ ));
                return IPCCR_INTERNAL_ERROR;
            }
    
            pt_ipcc_wgt->at_cc_info[ui4_i].ui2_cc_info_flags &= (~WGL_CC_INFO_VISIBLE);

            if(TRUE == gb_is_enhance_rollup)
            {
                ui4_i = ui4_tmp_i;
                ui4_tmp_i = 0;
            }
        }
    }
    else
    {
        ui4_i = ui4_crnt_wnd;

        i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                                       WGL_CMD_CC_SHOW, 
                                       (VOID*)FALSE,
                                       (VOID*)TRUE );
        if(i4_ret < WGLR_OK )
        {
            DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_SHOW) ERROR, return %d   <%d, %s, %s>\r\n",
                        i4_ret,__LINE__, __func__, __FILE__ ));
            return IPCCR_INTERNAL_ERROR;
        }

        pt_ipcc_wgt->at_cc_info[ui4_i].ui2_cc_info_flags &= (~WGL_CC_INFO_VISIBLE);
    }
   
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_clear_wnd
 *
 * Description: This API is used to clear window.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          ui4_crnt_wnd         - The number of current window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_clear_wnd
(
    IN  UINT32              ui4_crnt_wnd, 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
)
{
    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR(("\n{IPCC} ERR: pt_ipcc_wgt is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG; 
    }

    UINT32 ui4_i = 0;  
    UINT32 ui4_tmp_i = 0;
    INT32 i4_ret = IPCCR_OK;

    if(0xFF == ui4_crnt_wnd)
    {
        ui4_i = 0;

        for(; ui4_i<IPCC_MAX_WGT_NUM; ++ui4_i)
        {
            if(8 != gaui1_crnt_wnd[ui4_i])
            {
                pt_ipcc_wgt->aui1_text_tag[ui4_i] = CAP_CMD_TEXT_TAG_DIALOG;

                if(TRUE == gb_is_enhance_rollup)
                {
                    ui4_tmp_i = ui4_i;
                    ui4_i = IPCC_MAX_WGT_NUM - ui4_tmp_i -1 ;
                }

                i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                                        WGL_CMD_CC_CLEAR_TEXT, 
                                       (VOID*)NULL,
                                       (VOID*)NULL );
                
                if(i4_ret < WGLR_OK )
                {
                    DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_CLEAR_TEXT) ERROR, return %d   <%d, %s, %s>\r\n",
                                i4_ret,__LINE__, __func__, __FILE__ ));
                    return IPCCR_INTERNAL_ERROR;
                }

                if(SBTL_IPCC_CAPTION_MODE_ROLL_UP != ge_crnt_caption_mode)
                {
                    WGL_CC_WIN_ATTR_T*     pt_wgt_wnd_att = &pt_ipcc_wgt->at_cc_info[ui4_i].t_win_attr;
                    pt_wgt_wnd_att->ui2_win_attr &= (~WGL_CC_WIN_ATTR_AUTO_SCROLL);        
                }

                _sbtl_ipcc_wgt_move_pen_according_to_scroll_n_print_dir(pt_ipcc_wgt, (UINT8)ui4_i );

                i4_ret = x_wgl_do_cmd(  pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                                        WGL_CMD_CC_UPDATE, 
                                        (VOID*)WGL_CC_UPDATE_MODE_DEFAULT,
                                        (VOID*)NULL );
                
                if ( i4_ret < WGLR_OK )
                {
                    DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_UPDATE) ERROR, return %d   <%d, %s, %s>\r\n",
                                i4_ret,__LINE__, __func__, __FILE__ ));
                }

                t_scroll[ui4_i].t_parag_loc.ui2_idx_parag = 0xFFFF;
                t_scroll[ui4_i].t_parag_loc.ui2_col = 0xFFFF;
                t_scroll[ui4_i].b_cr = FALSE;

                if(TRUE == gb_is_enhance_rollup)
                {
                    ui4_i = ui4_tmp_i;
                    ui4_tmp_i = 0;
                }
            }
        }
    }
    else
    {
        ui4_i = ui4_crnt_wnd;

        pt_ipcc_wgt->aui1_text_tag[ui4_i] = CAP_CMD_TEXT_TAG_DIALOG;
    
        i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                               WGL_CMD_CC_CLEAR_TEXT, 
                                       (VOID*)NULL,
                                       (VOID*)NULL );
        if(i4_ret < WGLR_OK )
        {
            DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_CLEAR_TEXT) ERROR, return %d   <%d, %s, %s>\r\n",
                        i4_ret,__LINE__, __func__, __FILE__ ));
            return IPCCR_INTERNAL_ERROR;
        }

        if(SBTL_IPCC_CAPTION_MODE_ROLL_UP != ge_crnt_caption_mode)
        {
            WGL_CC_WIN_ATTR_T*     pt_wgt_wnd_att = &pt_ipcc_wgt->at_cc_info[ui4_i].t_win_attr;
            pt_wgt_wnd_att->ui2_win_attr &= (~WGL_CC_WIN_ATTR_AUTO_SCROLL);        
        }

        _sbtl_ipcc_wgt_move_pen_according_to_scroll_n_print_dir(pt_ipcc_wgt, (UINT8)ui4_i );

        i4_ret = x_wgl_do_cmd( 
                    pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                    WGL_CMD_CC_UPDATE, 
                    (VOID*)WGL_CC_UPDATE_MODE_DEFAULT,
                    (VOID*)NULL );
        if ( i4_ret < WGLR_OK )
        {
            DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_UPDATE) ERROR, return %d   <%d, %s, %s>\r\n",
                        i4_ret,__LINE__, __func__, __FILE__ ));
        }

        t_scroll[ui4_i].t_parag_loc.ui2_idx_parag = 0xFFFF;
        t_scroll[ui4_i].t_parag_loc.ui2_col = 0xFFFF;
        t_scroll[ui4_i].b_cr = FALSE;
    }
    
    return IPCCR_OK;
}











/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_define_wnd
 *
 * Description: This API is used to define window.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          ui4_crnt_wnd         - The number of current window
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_define_wnd
(
    IN  UINT32              ui4_crnt_wnd,  
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);

//    DBG_INFO(("{IPCC} INFO: Crnt wnd is %u!   <%d, %s, %s>\n\r", 
//                ui4_crnt_wnd, __LINE__, __func__, __FILE__));

    INT32 i4_ret = IPCCR_OK;
    UINT8 ui1_crnt_wnd = (UINT8)ui4_crnt_wnd;
    UINT8 ui1_hdend_z_order = ui1_crnt_wnd;

    /* reset text tag */
    pt_ipcc_wgt->aui1_text_tag[ui4_crnt_wnd] = gat_wgt_wnd_attr[ui1_crnt_wnd].aui1_text_tag;//CAP_CMD_TEXT_TAG_DIALOG;
    
    //t_cc_info_old = pt_ipcc_wgt->at_cc_info[ui4_crnt_wnd];
    
    i4_ret = _sbtl_ipcc_wgt_set_pen_style( pt_ipcc_wgt,
                                           ui1_crnt_wnd);
    if ( i4_ret != IPCCR_OK )    
    {
        return i4_ret;   
    }
    
    i4_ret = _sbtl_ipcc_wgt_set_wnd_style( pt_ipcc_wgt,
                                           ui1_crnt_wnd);
    if ( i4_ret != IPCCR_OK )    
    {
        return i4_ret;   
    }
    
    i4_ret = _sbtl_ipcc_wgt_chg_z_order( pt_ipcc_wgt,
                                         ui1_crnt_wnd,
                                         ui1_hdend_z_order);
    if ( i4_ret != IPCCR_OK )    
    {
        return i4_ret;   
    }
    
   pt_ipcc_wgt->at_cc_info[ui4_crnt_wnd].ui2_cc_info_flags &= (~WGL_CC_INFO_COL_LOCK);
   pt_ipcc_wgt->at_cc_info[ui4_crnt_wnd].ui2_cc_info_flags &= (~WGL_CC_INFO_ROW_LOCK);

   //pt_ipcc_wgt->at_cc_info[ui4_crnt_wnd].ui2_cc_info_flags |= WGL_CC_INFO_COL_LOCK;
   //pt_ipcc_wgt->at_cc_info[ui4_crnt_wnd].ui2_cc_info_flags |= WGL_CC_INFO_ROW_LOCK;
   pt_ipcc_wgt->at_cc_info[ui4_crnt_wnd].ui2_cc_info_flags &= (~WGL_CC_INFO_VISIBLE);


    /* set ANCHOR point and its position */
    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui1_anchor = gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_anchor.ui1_anchor;
   
    /* y position */
    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_anchor_v = gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_anchor.ui2_anchor_v;
//    DBG_ERROR(("{IPCC} INFO: Crnt wnd is %u, anchor_v is %u!   <%d, %s, %s>\n\r", 
//                ui4_crnt_wnd, gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_anchor.ui2_anchor_v,__LINE__, __func__, __FILE__));

    /* x position */
    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_anchor_h = gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_anchor.ui2_anchor_h;
#if 1
    DBG_INFO(("\n{IPCC} INFO: (Wnd_%u: anchor_h=%u, anchor_v=%u)!   <%d, %s, %s>\n\r", 
                    ui4_crnt_wnd, gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_anchor.ui2_anchor_h,
                    gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_anchor.ui2_anchor_v,
                    __LINE__, __func__, __FILE__));
#endif

    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_row_count = gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_size.ui2_row_count;
    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_col_count = gat_wgt_wnd_attr[ui4_crnt_wnd].t_win_size.ui2_col_count;

    DBG_INFO(("\n{IPCC} INFO: (Wnd_%u: r_c=%u, c_c=%u)!   <%d, %s, %s>\n\r", 
                    ui4_crnt_wnd, pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_row_count,
                    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_col_count,
                    __LINE__, __func__, __FILE__));

    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_cc_info_flags |= WGL_CC_INFO_IS_VALID_WIN_ATTR;
    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_cc_info_flags |= WGL_CC_INFO_IS_VALID_PEN_FONT;
    pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd].ui2_cc_info_flags |= WGL_CC_INFO_IS_VALID_PEN_COLOR;
    

    i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui1_crnt_wnd],
                           WGL_CMD_CC_UPDATE_ATTR, 
                           (VOID*)&pt_ipcc_wgt->at_cc_info[ui1_crnt_wnd],
                           (VOID*)TRUE );
    if ( i4_ret < WGLR_OK )
    {
        DBG_ERROR(( 
            "{IPCC} ERR:  x_wgl_do_cmd(%d,WGL_CMD_CC_UPDATE_ATTR) ERROR, return %d\r\n",
            ui1_crnt_wnd, i4_ret ));
        return IPCCR_INTERNAL_ERROR;
    }

    UINT16 ui2_row = 0; 
    UINT16 ui2_col = 0;

    if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
    {
        ui2_row = gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].t_win_size.ui2_row_count - 1;
    }
    
    WGL_CC_TEXT_PARAG_LOCATION_T  t_pen_loc = {ui2_row, ui2_col};
        //{gt_region_attr[ui4_crnt_wnd].t_pen_loc.ui2_idx_parag, gt_region_attr[ui4_crnt_wnd].t_pen_loc.ui2_col}; 
    i4_ret = _sbtl_ipcc_wgt_set_pen_loc( 
                            pt_ipcc_wgt,
                            &t_pen_loc,
                            ui1_crnt_wnd );
    if ( i4_ret != IPCCR_OK )
    {
        return IPCCR_INTERNAL_ERROR;
    }
   
    return IPCCR_OK;    
    
}


/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_destroy_wnd
 *
 * Description: This API is used to destory windows which were created at open phase
 *
 * Inputs:  h_cc_wnd          - The handle of windows
 *
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_destroy_wnd( IN HANDLE_T h_cc_wnd )
{
    INT32    i4_ret;

    i4_ret = x_wgl_do_cmd( h_cc_wnd,
                           WGL_CMD_CC_DESTROY, 
                           (VOID*)NULL,
                           (VOID*)NULL );
    if ( i4_ret < WGLR_OK )
    {
        DBG_INFO(( "[SM] x_wgl_do_cmd(WGL_CMD_CC_DESTROY) ERROR, return %d\r\n",i4_ret ));
        return IPCCR_INTERNAL_ERROR;
    }
    
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_reset_cc_info_initor
 *
 * Description: This API is used to reset cc info.
 *
 * Inputs:  ui4_org_canvas_w         - The width of canvas
 *          ui4_org_canvas_h         - The height of canvas
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static VOID _sbtl_ipcc_wgt_reset_cc_info_initor
( 
    IN  UINT32  ui4_org_canvas_w,
    IN  UINT32  ui4_org_canvas_h 
)
{    
    /* Prepare font ID*/
    gt_ipcc_info_initor.ui4_font_tbl_id = 
        WGL_CC_MAKE_FONT_TBL_ID( ui4_org_canvas_w, ui4_org_canvas_h );
    
    /* Prepare initial CC info */
    gt_ipcc_info_initor.t_override_attr.ui2_flag = 0;
    gt_ipcc_info_initor.ui2_max_width = (UINT16) (ui4_org_canvas_w * gui1_width_percent / 100);
    gt_ipcc_info_initor.ui2_max_height = (UINT16) (ui4_org_canvas_h * gui1_height_percent / 100);
    gt_ipcc_info_initor.ui4_flashing_interval = 0;
    gt_ipcc_info_initor.ui4_smooth_scroll_time_bound = 433;
    gt_ipcc_info_initor.ui4_typewriter_effect_time_bound = 0;
    gt_ipcc_info_initor.ui2_tab_size = 4;
    gt_ipcc_info_initor.t_cc_info.ui2_cc_info_flags = 
        WGL_CC_INFO_DTVCC_MODE               |
        WGL_CC_INFO_AUTO_DEL_INVISIBLE_PARAG |
        WGL_CC_INFO_IS_VALID_WIN_ATTR        | 
        WGL_CC_INFO_IS_VALID_PEN_FONT        | 
        WGL_CC_INFO_IS_VALID_PEN_COLOR;
    gt_ipcc_info_initor.t_cc_info.ui1_anchor = WGL_CC_ANCHOR_POINT_0;

    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui2_win_attr = 
        WGL_CC_WIN_ATTR_WORD_WRAP        | 
        WGL_CC_WIN_ATTR_LIMIT_ROW        |
        WGL_CC_WIN_ATTR_AUTO_SCROLL      | 
        WGL_CC_WIN_ATTR_SMOOTH_SCROLL;
    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui1_justify = WGL_CC_JUSTIFY_LEFT;
    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui1_print_dir = WGL_CC_PRINT_DIR_LEFT_TO_RIGHT;
    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui1_scroll_dir = WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP;
    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui1_display_effect = WGL_CC_DISPLAY_EFFECT_SNAP;
    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui1_display_effect_dir = WGL_CC_DISPLAY_EFFECT_DIR_LEFT_TO_RIGHT;
    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui4_display_effect_duration = 0;
    x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_win_attr.t_color_bk, &t_transparent, sizeof(GL_COLOR_T) );
    
    gt_ipcc_info_initor.t_cc_info.t_win_attr.b_is_flash_bk = FALSE;
    gt_ipcc_info_initor.t_cc_info.t_win_attr.ui1_border_type = WGL_CC_BORDER_TYPE_NONE;
    x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_win_attr.t_color_border, &t_black, sizeof(GL_COLOR_T) );
    
    /* Set Font Style */
    x_memcpy(&gt_ipcc_info_initor.t_cc_info.t_pen_font.t_font, 
             &at_pen_att[0].t_font, sizeof(WGL_CC_FONT_INFO_T));

    gt_ipcc_info_initor.t_cc_info.t_pen_font.ui1_offset = WGL_CC_TEXT_OFFSET_NORMAL;
    
    if ( t_sbtl_ipcc_attr.ui4_att_flag == SM_CCH_DTVCC_ATT_FLAG_ALL_AS_BRDCST )
    {
        x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_pen_color.t_color_bk, &t_black, sizeof(GL_COLOR_T) );
        x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_pen_color.t_color_text, &t_white, sizeof(GL_COLOR_T) );
        x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_pen_color.t_color_edge, &t_white, sizeof(GL_COLOR_T) );
        gt_ipcc_info_initor.t_cc_info.t_pen_color.b_is_flash_bk = FALSE;
        gt_ipcc_info_initor.t_cc_info.t_pen_color.b_is_flash_fg = FALSE;
    }
    else
    {
        x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_pen_color.t_color_bk, &t_black, sizeof(GL_COLOR_T) );
        x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_pen_color.t_color_text, &t_white, sizeof(GL_COLOR_T) );
        x_memcpy( &gt_ipcc_info_initor.t_cc_info.t_pen_color.t_color_edge, &t_white, sizeof(GL_COLOR_T) );
        gt_ipcc_info_initor.t_cc_info.t_pen_color.b_is_flash_bk = FALSE;
        gt_ipcc_info_initor.t_cc_info.t_pen_color.b_is_flash_fg = FALSE;
    }
    
    gt_ipcc_info_initor.t_cc_info.ui2_anchor_h = 0;
    gt_ipcc_info_initor.t_cc_info.ui2_anchor_v = (UINT16) (ui4_org_canvas_h * gui1_height_percent / 100);
    gt_ipcc_info_initor.t_cc_info.ui2_col_count = 44;
    gt_ipcc_info_initor.t_cc_info.ui2_row_count = 1;   
}




static INT32 _sbtl_ipcc_wgt_chg_z_order
( 
    SBTL_IPCC_WGT_T*                pt_ipcc_wgt,
    UINT8                          ui1_wnd_num,
    UINT8                          ui1_hdend_z_order 
)
{
    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR((IPCC_PREFIX"ERR: pt_ipcc_wgt is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG; 
    }
    
    INT32  i4_ret;
    UINT8  ui1_tgt_wnd_num = 0;
    UINT8  ui1_org_z_order = pt_ipcc_wgt->aui1_z_order[ui1_wnd_num];
    UINT8  ui1_wnd_num_iter = 0;
    UINT8  ui1_z_order = 0;
    
    if ( ui1_hdend_z_order >= (UINT8) IPCC_MAX_WGT_NUM ||
         ui1_wnd_num >= (UINT8) IPCC_MAX_WGT_NUM )
    {
        return IPCCR_INV_ARG;   
    }
    
    if ( gb_sbtl_cc_dbg_z_order )
    {
        UINT32 ui4_i = 0;

        DBG_INFO(( "Before Change W%d to order %d\r\n", ui1_wnd_num, ui1_hdend_z_order ));
        DBG_INFO(( "HEADEND odr = { " ));
        for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ui4_i++ )
        {
            DBG_INFO(( "W%d(%d) ", ui4_i, pt_ipcc_wgt->aui1_hdend_z_order[ui4_i] ));
        }

        DBG_INFO(( "}\r\n" ));
        DBG_INFO(( "real odr = { " ));
        for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ui4_i++ )
        {
            DBG_INFO(( "W%d(%d) ", ui4_i, pt_ipcc_wgt->aui1_z_order[ui4_i] ));
        }
        DBG_INFO(( "}\r\n" ));
    }
   /*/ 
    ui1_tgt_wnd_num = _sbtl_ipcc_wgt_get_target_wnd_num_by_hdend_z_order(
        pt_ipcc_wgt, ui1_wnd_num, ui1_hdend_z_order );
    if (ui1_tgt_wnd_num >= (UINT8) IPCC_MAX_WGT_NUM)
    {
        return IPCCR_INTERNAL_ERROR; 
    }
    */
    pt_ipcc_wgt->aui1_hdend_z_order[ui1_wnd_num] = ui1_hdend_z_order;
    ui1_z_order = pt_ipcc_wgt->aui1_z_order[ui1_tgt_wnd_num];

    if ( ui1_org_z_order == ui1_z_order )
    {
        return IPCCR_OK;
    }
  /*  
    ui1_tgt_wnd_num = _sm_cc_dtv_wgt_get_wnd_num_by_z_order( pt_dtv_wgt, ui1_z_order);
    if (ui1_tgt_wnd_num >= (UINT8) IPCC_MAX_WGT_NUM)
    {
        return IPCCR_INTERNAL_ERROR; 
    }*/
    if ( ui1_org_z_order > ui1_z_order )
    {
        i4_ret = x_wgl_insert(
            pt_ipcc_wgt->ah_cc_wnd[ui1_wnd_num],
            pt_ipcc_wgt->ah_cc_wnd[ui1_tgt_wnd_num],
            WGL_INSERT_BEFORE,
            TRUE );
        if ( i4_ret != WGLR_OK )
        {
            return IPCCR_INTERNAL_ERROR;   
        }
        
        /* move z-order up */
        for( ui1_wnd_num_iter = 0; ui1_wnd_num_iter < (UINT8) IPCC_MAX_WGT_NUM; ++ui1_wnd_num_iter )
        {
            if ( pt_ipcc_wgt->aui1_z_order[ui1_wnd_num_iter] >= ui1_z_order &&
                 pt_ipcc_wgt->aui1_z_order[ui1_wnd_num_iter] < ui1_org_z_order )
            {
                ++(pt_ipcc_wgt->aui1_z_order[ui1_wnd_num_iter]);
            }
        }
        
        pt_ipcc_wgt->aui1_z_order[ui1_wnd_num] = ui1_z_order;
    }
    else /* ui1_org_z_order < ui1_z_order */
    {
        i4_ret = x_wgl_insert(
            pt_ipcc_wgt->ah_cc_wnd[ui1_wnd_num],
            pt_ipcc_wgt->ah_cc_wnd[ui1_tgt_wnd_num],
            WGL_INSERT_AFTER,
            TRUE );
        if ( i4_ret != WGLR_OK )
        {
            return IPCCR_INTERNAL_ERROR;   
        }

        /* move z-order down */
        for( ui1_wnd_num_iter = 0; ui1_wnd_num_iter < (UINT8) IPCC_MAX_WGT_NUM; ++ui1_wnd_num_iter )
        {
            if ( pt_ipcc_wgt->aui1_z_order[ui1_wnd_num_iter] <= ui1_z_order &&
                 pt_ipcc_wgt->aui1_z_order[ui1_wnd_num_iter] > ui1_org_z_order )
            {
                --(pt_ipcc_wgt->aui1_z_order[ui1_wnd_num_iter]);
            }
        }
        
        pt_ipcc_wgt->aui1_z_order[ui1_wnd_num] = ui1_z_order;
    }

    if ( gb_sbtl_cc_dbg_z_order )
    {
        UINT32 ui4_i = 0;

        DBG_INFO(( "After Change W%d to order %d\r\n", ui1_wnd_num, ui1_hdend_z_order ));
        DBG_INFO(( "HEADEND odr = { " ));
        for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ui4_i++ )
        {
            DBG_INFO(( "W%d(%d) ", ui4_i, pt_ipcc_wgt->aui1_hdend_z_order[ui4_i] ));
        }
        DBG_INFO(( "}\r\n" ));
        DBG_INFO(( "real odr = { " ));
        for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ui4_i++ )
        {
            DBG_INFO(( "W%d(%d) ", ui4_i, pt_ipcc_wgt->aui1_z_order[ui4_i] ));
        }
        DBG_INFO(( "}\r\n\r\n" ));
    }
    
    return IPCCR_OK;
}    


static VOID _sbtl_ipcc_wgt_set_globle_org_canvas
(
    IN    UINT32    ui4_canvas_w,
    IN    UINT32    ui4_canvas_h 
)
{
    CRIT_STATE_T   t_cs;
    
    t_cs = x_crit_start();
    
    gui4_org_canvas_w = ui4_canvas_w;
    gui4_org_canvas_h = ui4_canvas_h;

#if 1
    if(gui4_screen_height == gui4_org_canvas_h)
    {
        DBG_INFO(("{IPCC}: gui4_screen_height == gui4_org_canvas_h\n\r"));
    }
    else
    {
        DBG_INFO(("{IPCC}: gui4_screen_height != gui4_org_canvas_h\n\r"));
    }

    if(gui4_screen_width == gui4_org_canvas_w)
    {
        DBG_INFO(("{IPCC}: gui4_screen_width == gui4_org_canvas_w\n\r"));
    }
    else
    {
        DBG_INFO(("{IPCC}: gui4_screen_width != gui4_org_canvas_w\n\r"));
    }
#endif
    
    x_crit_end( t_cs );
}






/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_flush_text
 *
 * Description: This API is used to flush text.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *          pwc_text             - The buffer of text
 *          z_text_len           - The length of text
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_ipcc_wgt_flush_text
( 
    IN  SBTL_IPCC_WGT_T*    pt_ipcc_wgt,
    IN  UTF16_T*            pwc_text,
    IN  SIZE_T              z_text_len 
)
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);
    IPCC_CHECK_NULL(pwc_text,IPCCR_INV_ARG);  
    

    INT32 i4_ret = IPCCR_OK;
    UINT32    ui4_i = 0;

    //gt_last_txt_len = 0;

    if ( pt_ipcc_wgt->ui1_curr_wnd_num  >= (UINT8) IPCC_MAX_WGT_NUM )
    {
        return IPCCR_INTERNAL_ERROR;   
    }

    DBG_INFO(( "{IPCC} (Wnd_%u)Caption: ", pt_ipcc_wgt->ui1_curr_wnd_num));
    for( ui4_i = 0; ui4_i < z_text_len; ++ui4_i )
    {
        DBG_INFO(( "%c", (CHAR)(pwc_text[ui4_i]) ));
    }
    DBG_INFO(( "(Wnd_%u) <END_%d>   <%d, %s, %s>\n\r", 
        pt_ipcc_wgt->ui1_curr_wnd_num, z_text_len, __LINE__, __func__, __FILE__));

    if(SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
    {
        i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ pt_ipcc_wgt->ui1_curr_wnd_num ],
                                   WGL_CMD_CC_WRITE_TEXT, 
                                   (VOID*)pwc_text,
                                   (VOID*)z_text_len );
        if ( i4_ret < WGLR_OK )
        {
            DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_WRITE_TEXT_UPDATE) ERROR, return %d   <%d, %s, %s>\r\n",
                        i4_ret,__LINE__, __func__, __FILE__ ));
            return IPCCR_INTERNAL_ERROR;
        }
    }
    else if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode
            || SBTL_IPCC_CAPTION_MODE_PAINT_ON == ge_crnt_caption_mode)
    {   
        UINT32 ui4_text_len = (UINT32)z_text_len * 3 / 4;
        UINT32 ui4_inc_len = ui4_text_len;

        i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ pt_ipcc_wgt->ui1_curr_wnd_num ],
                               WGL_CMD_CC_WRITE_TEXT_UPDATE, 
                               (VOID*)pwc_text,
                               (VOID*)ui4_inc_len );
        if ( i4_ret < WGLR_OK )
        {
            DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_WRITE_TEXT_UPDATE) ERROR, return %d   <%d, %s, %s>\r\n",
                        i4_ret,__LINE__, __func__, __FILE__ ));
            return IPCCR_INTERNAL_ERROR;
        }
        
        i4_ret = _sbtl_ipcc_wgt_display_wnd(pt_ipcc_wgt->ui1_curr_wnd_num, pt_ipcc_wgt);
        if(IPCCR_OK != i4_ret)
        {
            DBG_ERROR(("{IPCC} ERR: Display window failed, return %d!   <%d, %s, %s>\n\r", 
                        i4_ret, __LINE__, __func__, __FILE__));
        }
        
        
        ui4_inc_len = 1;
        for(; ui4_text_len < (UINT32)z_text_len; ++ui4_text_len)
        {
            i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ pt_ipcc_wgt->ui1_curr_wnd_num ],
                               WGL_CMD_CC_WRITE_TEXT_UPDATE, 
                               (VOID*)(pwc_text+ui4_text_len),
                               (VOID*)ui4_inc_len );
            if ( i4_ret < WGLR_OK )
            {
                DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_WRITE_TEXT_UPDATE) ERROR, return %d   <%d, %s, %s>\r\n",
                            i4_ret,__LINE__, __func__, __FILE__ ));
                return IPCCR_INTERNAL_ERROR;
            }

            
            i4_ret = _sbtl_ipcc_wgt_display_wnd(pt_ipcc_wgt->ui1_curr_wnd_num, pt_ipcc_wgt);
            if(IPCCR_OK != i4_ret)
            {
                DBG_ERROR(("{IPCC} ERR: Display window failed, return %d!   <%d, %s, %s>\n\r", 
                            i4_ret, __LINE__, __func__, __FILE__));
            }
        }    
    }
        
    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_conv_font_size
 *
 * Description: <function description>
 *
 * Inputs:  ui1_size References.
 *
 * Outputs: - 
 *
 * Returns: FE_FNT_SIZE_SMALL   Success.
 *          FE_FNT_SIZE_LARGE   Fail.
 *          FE_FNT_SIZE_MEDIUM  Fail.
 *
 ----------------------------------------------------------------------------*/
static FE_FNT_SIZE _sbtl_ipcc_wgt_conv_font_size( IN UINT8 ui1_size )
{
    if ( ui1_size == 0 )
    {
        return FE_FNT_SIZE_SMALL;
    }
    else if ( ui1_size == 2 )
    {
        return FE_FNT_SIZE_LARGE;
    }

    return FE_FNT_SIZE_MEDIUM;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_conv_font_edge_type
 *
 * Description: <function description>
 *
 * Inputs:  ui1_edge_type References.
 *
 * Outputs: - 
 *
 * Returns: WGL_CC_EDGE_TYPE_RAISED             Success.
 *          WGL_CC_EDGE_TYPE_DEPRESSED          Fail.
 *          WGL_CC_EDGE_TYPE_UNIFORM            Fail.
 *          WGL_CC_EDGE_TYPE_LEFT_DROP_SHADOW   Fail.
 *          WGL_CC_EDGE_TYPE_RIGHT_DROP_SHADOW  Fail.
 *          WGL_CC_EDGE_TYPE_NONE               Fail.
 *
 ----------------------------------------------------------------------------*/
static UINT8 _sbtl_ipcc_wgt_conv_font_edge_type( IN UINT8 ui1_edge_type )
{
    if ( ui1_edge_type == 1 )
    {
        return WGL_CC_EDGE_TYPE_RAISED;
    }
    else if ( ui1_edge_type == 2 )
    {
        return WGL_CC_EDGE_TYPE_DEPRESSED;
    }
    else if ( ui1_edge_type == 3 )
    {
        return WGL_CC_EDGE_TYPE_UNIFORM;
    }
    else if ( ui1_edge_type == 4 )
    {
        return WGL_CC_EDGE_TYPE_LEFT_DROP_SHADOW;
    }
    else if ( ui1_edge_type == 5 )
    {
        return WGL_CC_EDGE_TYPE_RIGHT_DROP_SHADOW;
    }

    return WGL_CC_EDGE_TYPE_NONE;
}



/*-----------------------------------------------------------------------------
 * Name: _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color
 *
 * Description: <function description>
 *
 * Inputs:  pt_cch_color References.
 *          pt_gl_color  References.
 *          pb_is_flash  References.
 *
 * Outputs: - 
 *
 * Returns: -
 *
 ----------------------------------------------------------------------------*/
static VOID _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color
( 
    IN  const SM_CCH_DTVCC_COLOR_T*      pt_cch_color,
    OUT GL_COLOR_T*                      pt_gl_color,
    OUT BOOL*                            pb_is_flash 
)
{
    pt_gl_color->u1.r = pt_cch_color->ui1_red;
    pt_gl_color->u2.g = pt_cch_color->ui1_green;
    pt_gl_color->u3.b = pt_cch_color->ui1_blue;

    if ( pb_is_flash )
    {
        *pb_is_flash = FALSE;
    }
    
    switch( pt_cch_color->e_opacity )
    {
    case SM_CCH_DTVCC_OPACITY_TRANSLUCENT:
        pt_gl_color->a = 128;
        break;
        
    case SM_CCH_DTVCC_OPACITY_TRANSPARENT:
        pt_gl_color->a = 0;
        break;
        
    case SM_CCH_DTVCC_OPACITY_FLASH:
        if ( pb_is_flash )
        {
            *pb_is_flash = TRUE;
        }
        /* intend to leave no "break" here */
        
    case SM_CCH_DTVCC_OPACITY_SOLID:
    default:
        pt_gl_color->a = 255;
    }
}



static UINT8 _sbtl_ipcc_wgt_conv_hdlr_justify_2_cc_wgt_justify
(
    IN SM_CCH_DTVCC_JUSTIFY_T e_cch_justify 
)
{
    switch( e_cch_justify )
    {
    case SM_CCH_DTVCC_JUSTIFY_RIGHT:
        return WGL_CC_JUSTIFY_RIGHT;
        
    case SM_CCH_DTVCC_JUSTIFY_CENTER:
        return WGL_CC_JUSTIFY_CENTER;
        
    case SM_CCH_DTVCC_JUSTIFY_FULL:
        return WGL_CC_JUSTIFY_FULL;
        
    case SM_CCH_DTVCC_JUSTIFY_LEFT:
    default:
        return WGL_CC_JUSTIFY_LEFT;
    }
}



INT32 _sbtl_ipcc_wgt_set_order( VOID )
{
    INT32                   i4_ret;
    HANDLE_T                h_thread = NULL_HANDLE;
    
    i4_ret = x_thread_self( &h_thread );
    if ( i4_ret != OSR_OK )
    {
        return IPCCR_INTERNAL_ERROR;
    }
    
    i4_ret = x_wgl_set_app_order( 
            h_thread, 
            ((ge_ipcc_wgt_order == SM_CCH_WGT_ORDER_BOTTOMMOST)?
             WGL_APP_BOTTOMMOST: WGL_APP_TOPMOST) );
    if ( i4_ret != WGLR_OK )
    {
        return IPCCR_INTERNAL_ERROR;
    }
    
    return IPCCR_OK;
}



static SM_CC_ASPECT_RATIO_T _sbtl_ipcc_wgt_get_asp_ratio( VOID )
{
    UINT32 ui4_osd_height = gui4_screen_height;
    UINT32 ui4_osd_width = gui4_screen_width;

    DBG_INFO(("\n{IPCC}: ui4_osd_height=%u, ui4_osd_width=%u.   <%d, %s, %s>\n",
                ui4_osd_height, ui4_osd_width, __LINE__, __func__, __FILE__));

    SM_CC_ASPECT_RATIO_T e_asp_ratio = SM_CC_ASPECT_RATIO_UNKNOWN;
    UINT32   ui4_asp_ratio = 0;;

    if( (0 != ui4_osd_height) && (0 != ui4_osd_width))
    {
        ui4_asp_ratio = ui4_osd_width * 100 / ui4_osd_height;
        
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
            e_asp_ratio = SM_CC_ASPECT_RATIO_OTHER;            
        }
    }

    ge_aspect_ratio = e_asp_ratio;

    return ge_aspect_ratio;
}




static VOID _sbtl_ipcc_wgt_update_attr(IN OUT SBTL_IPCC_WGT_T*    pt_ipcc_wgt)
{
   
    if(!pt_ipcc_wgt)
    {
        DBG_ERROR(("{IPCC} ERR: pt_ipcc_wgt is invalid!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return;
    }

    INT32 i4_ret = IPCCR_OK;
    i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[pt_ipcc_wgt->ui1_curr_wnd_num],
                           WGL_CMD_CC_UPDATE_ATTR, 
                           (VOID*)&pt_ipcc_wgt->at_cc_info[pt_ipcc_wgt->ui1_curr_wnd_num],
                           (VOID*)TRUE );
    if ( i4_ret < WGLR_OK )
    {
        DBG_ERROR(( 
            "{IPCC} ERR: x_wgl_do_cmd(%d,WGL_CMD_CC_UPDATE_ATTR) ERROR, return %d.  <%d, %s, %s>\r\n",
            pt_ipcc_wgt->ui1_curr_wnd_num, i4_ret, __LINE__, __func__, __FILE__));
    }
}








#ifndef _______________________________________SPLIT_LINE_____________
#endif


/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_wgt_set_wnd_attr
 *
 * Description: This API is used to set window attributes.
 *
 * Inputs:  pt_ipcc_wgt          - 
 *          pt_ipcc_att          - Window attributes
 *
 * Outputs:
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
VOID sbtl_ipcc_wgt_set_wnd_attr
(
    IN SBTL_IPCC_WGT_T*       pt_ipcc_wgt,
    IN const SM_CCH_DTVCC_ATTRIBS_INFO_T*  pt_ipcc_att
)
{
    INT32                    i4_ret;
    UINT32                   ui4_i = 0;
    UINT16                   ui2_undo_flag = 0;
    UINT32                   ui4_undo_flag_ex = 0;
    WGL_CC_OVERRIDE_ATTR_T   t_cc_over_attr;

    if ( pt_ipcc_att )
    {
        t_sbtl_ipcc_attr = *pt_ipcc_att;
    }
    else
    {
        t_sbtl_ipcc_attr.ui4_att_flag = SM_CCH_DTVCC_ATT_FLAG_ALL_AS_BRDCST;
    }
    
    if ( !pt_ipcc_wgt )
    {
        return;
    }
    
    t_cc_over_attr.ui2_flag = 0;
    t_cc_over_attr.ui4_flag_ex = 0;
    
    if ( !pt_ipcc_att ||
         pt_ipcc_att->ui4_att_flag == SM_CCH_DTVCC_ATT_FLAG_ALL_AS_BRDCST )
    {
        ui2_undo_flag = (UINT16) WGL_CC_OVERRIDE_ALL_FLAGS;
        ui4_undo_flag_ex = WGL_CC_OVERRIDE_ALL_EX_FLAGS;
        
        for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ++ui4_i )
        {
            i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                                   WGL_CMD_CC_UNDO_OVERRIDE_ATTR, 
                                   (VOID*)(UINT32)ui2_undo_flag,
                                   (VOID*)(UINT32)ui4_undo_flag_ex );
            if ( i4_ret < WGLR_OK )
            {
                DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_UNDO_OVERRIDE_ATTR) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
            }
        }
        return;
    }
    
    DBG_INFO(( "[SM] CCS: Overrride attributs:\r\n" ));
    
    /* Font style */
    DBG_INFO(( "\tFont style: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_FONT_STYLE_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_FONT_NAME;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_FONT_NAME;
        
        if ( ! x_fe_has_font( as_font_style_name[(UINT8)pt_ipcc_att->e_font_style],
                              (FE_FNT_STYLE)WGL_CC_FNT_STYLE_REGULAR ) )
        {
            /* Use default font SM_CCH_DTVCC_FONT_STYLE_MONO_SP_WITHOUT_SERIFS */
            x_strncpy( t_cc_over_attr.t_font_info.s_font_name,
                       as_font_style_name[(UINT8)SM_CCH_DTVCC_FONT_STYLE_MONO_SP_WITHOUT_SERIFS],
                       sizeof(t_cc_over_attr.t_font_info.s_font_name) );
        }
        else
        {
            x_strncpy( t_cc_over_attr.t_font_info.s_font_name,
                       as_font_style_name[(UINT8)pt_ipcc_att->e_font_style],
                       sizeof(t_cc_over_attr.t_font_info.s_font_name) );
        }
        
        t_cc_over_attr.t_font_info.e_style = WGL_CC_FNT_STYLE_REGULAR;
        t_cc_over_attr.t_font_info.e_cmap = FE_CMAP_ENC_UNICODE;
        
        DBG_INFO(( "\t%s\r\n", t_cc_over_attr.t_font_info.s_font_name ));
    }
        
    /* Font size */
    DBG_INFO(( "\tFont size: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_FONT_SIZE_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_FONT_SIZE;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_FONT_SIZE;
        
        t_cc_over_attr.t_font_info.e_size = 
            _sbtl_ipcc_wgt_conv_font_size( (UINT8)pt_ipcc_att->e_font_size );
        
        DBG_INFO(( "\t%d \r\n", (UINT8)pt_ipcc_att->e_font_size ));
    }
   
    /* Font edge type */
    DBG_INFO(( "\tFont edge type: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_EDGE_TYPE_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_FONT_EDGE_TYPE;
        DBG_INFO(( "\tAs Broadcast\r\n" ));    
    }
    else
    {   
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_FONT_EDGE_TYPE;
        
        t_cc_over_attr.t_font_info.ui1_edge_type = 
            _sbtl_ipcc_wgt_conv_font_edge_type( (UINT8)pt_ipcc_att->e_font_edge_type );

        DBG_INFO(( "\t%s%s%s%s%s%s\r\n", 
            ((pt_ipcc_att->e_font_edge_type==SM_CCH_DTVCC_EDGE_TYPE_NONE)?"none":""),
            ((pt_ipcc_att->e_font_edge_type==SM_CCH_DTVCC_EDGE_TYPE_RAISED)?"raised":""),
            ((pt_ipcc_att->e_font_edge_type==SM_CCH_DTVCC_EDGE_TYPE_DEPRESSED)?"depressed":""),
            ((pt_ipcc_att->e_font_edge_type==SM_CCH_DTVCC_EDGE_TYPE_UNIFORM)?"uniform":""),
            ((pt_ipcc_att->e_font_edge_type==SM_CCH_DTVCC_EDGE_TYPE_LEFT_DROP_SHADOW)?"left drop shadow":""),
            ((pt_ipcc_att->e_font_edge_type==SM_CCH_DTVCC_EDGE_TYPE_RIGHT_DROP_SHADOW)?"right drop shadow":"")));
    }

    /* FG color RGB */
    DBG_INFO(( "\tFont color: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_FG_COLOR_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_RGB;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_IS_FLASH_TEXT_FG;
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_RGB;
        
        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_font_fg_color,
            &t_cc_over_attr.t_color_text,
            &t_cc_over_attr.b_is_flash_text_fg );
        
        DBG_INFO(( "\tR(%d), G(%d), B(%d)\r\n", 
            (UINT8)pt_ipcc_att->t_font_fg_color.ui1_red,
            (UINT8)pt_ipcc_att->t_font_fg_color.ui1_green,
            (UINT8)pt_ipcc_att->t_font_fg_color.ui1_blue));
    }

    /* FG color opacity */
    DBG_INFO(( "\tFont opacity: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_FG_COLOR_OPC_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_ALPHA;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_IS_FLASH_TEXT_FG;
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_ALPHA;
        
        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_font_fg_color,
            &t_cc_over_attr.t_color_text,
            &t_cc_over_attr.b_is_flash_text_fg );
        
        DBG_INFO(( "\t%s%s%s%s\r\n", 
            ((pt_ipcc_att->t_font_fg_color.e_opacity==SM_CCH_DTVCC_OPACITY_SOLID)?"solid":""),
            ((pt_ipcc_att->t_font_fg_color.e_opacity==SM_CCH_DTVCC_OPACITY_FLASH)?"flash":""),
            ((pt_ipcc_att->t_font_fg_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSLUCENT)?"translucent":""),
            ((pt_ipcc_att->t_font_fg_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSPARENT)?"transparent":"")));
    }
    
    /* BG color RGB */
    DBG_INFO(( "\tBG color: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_BG_COLOR_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_BK_RGB;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_IS_FLASH_TEXT_BK;
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_BK_RGB;
        
        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_font_bg_color,
            &t_cc_over_attr.t_color_text_bk,
            &t_cc_over_attr.b_is_flash_text_bk );

        DBG_INFO(( "\tR(%d), G(%d), B(%d)\r\n", 
            (UINT8)pt_ipcc_att->t_font_bg_color.ui1_red,
            (UINT8)pt_ipcc_att->t_font_bg_color.ui1_green,
            (UINT8)pt_ipcc_att->t_font_bg_color.ui1_blue ));
    }

    /* BG color opacity */
    DBG_INFO(( "\tBG opacity: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_BG_COLOR_OPC_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_BK_ALPHA;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_IS_FLASH_TEXT_BK;
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_BK_ALPHA;
        
        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_font_bg_color,
            &t_cc_over_attr.t_color_text_bk,
            &t_cc_over_attr.b_is_flash_text_bk );
        
        DBG_INFO(( "\t%s%s%s%s\r\n", 
            ((pt_ipcc_att->t_font_bg_color.e_opacity==SM_CCH_DTVCC_OPACITY_SOLID)?"solid":""),
            ((pt_ipcc_att->t_font_bg_color.e_opacity==SM_CCH_DTVCC_OPACITY_FLASH)?"flash":""),
            ((pt_ipcc_att->t_font_bg_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSLUCENT)?"translucent":""),
            ((pt_ipcc_att->t_font_bg_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSPARENT)?"transparent":"") ));
    }
    
    /* EDGE color RGB */
    DBG_INFO(( "\tEDGE color: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_EDGE_COLOR_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_EDGE_RGB;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_EDGE_RGB;
        
        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_font_edge_color,
            &t_cc_over_attr.t_color_text_edge,
            NULL );

        DBG_INFO(( "\tR(%d), G(%d), B(%d)\r\n", 
            (UINT8)pt_ipcc_att->t_font_edge_color.ui1_red,
            (UINT8)pt_ipcc_att->t_font_edge_color.ui1_green,
            (UINT8)pt_ipcc_att->t_font_edge_color.ui1_blue ));
    }

    /* EDGE color opacity */
    DBG_INFO(( "\tEDGE opacity: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_EDGE_COLOR_OPC_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_EDGE_ALPHA;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_TEXT_EDGE_ALPHA;

        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_font_edge_color,
            &t_cc_over_attr.t_color_text_edge,
            NULL );

        DBG_INFO(( "\t%s%s%s%s\r\n", 
            ((pt_ipcc_att->t_font_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_SOLID)?"solid":""),
            ((pt_ipcc_att->t_font_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_FLASH)?"flash":""),
            ((pt_ipcc_att->t_font_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSLUCENT)?"translucent":""),
            ((pt_ipcc_att->t_font_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSPARENT)?"transparent":"") ));
    }
    
    /* Window color RGB */
    DBG_INFO(( "\tWindow color: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_WND_COLOR_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_RGB;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_IS_FLASH_WIN;
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_RGB;

        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_wnd_color,
            &t_cc_over_attr.t_color_win,
            &t_cc_over_attr.b_is_flash_win );

        DBG_INFO(( "\tR(%d), G(%d), B(%d)\r\n", 
            (UINT8)pt_ipcc_att->t_wnd_color.ui1_red,
            (UINT8)pt_ipcc_att->t_wnd_color.ui1_green,
            (UINT8)pt_ipcc_att->t_wnd_color.ui1_blue ));
    }

    /* Window color opacity */
    DBG_INFO(( "\tWindow opacity: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_WND_COLOR_OPC_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_ALPHA;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_IS_FLASH_WIN;
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_ALPHA;

        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_wnd_color,
            &t_cc_over_attr.t_color_win,
            &t_cc_over_attr.b_is_flash_win );

        DBG_INFO(( "\t%s%s%s%s\r\n", 
            ((pt_ipcc_att->t_wnd_color.e_opacity==SM_CCH_DTVCC_OPACITY_SOLID)?"solid":""),
            ((pt_ipcc_att->t_wnd_color.e_opacity==SM_CCH_DTVCC_OPACITY_FLASH)?"flash":""),
            ((pt_ipcc_att->t_wnd_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSLUCENT)?"translucent":""),
            ((pt_ipcc_att->t_wnd_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSPARENT)?"transparent":"") ));
    }
    
    /* Window edge color RGB */
    DBG_INFO(( "\tWindow edge color: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_WND_EDGE_COLOR_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_BORDER_RGB;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_BORDER_RGB;

        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_wnd_edge_color,
            &t_cc_over_attr.t_color_win_border,
            NULL );

        DBG_INFO(( "\tR(%d), G(%d), B(%d)\r\n", 
            (UINT8)pt_ipcc_att->t_wnd_edge_color.ui1_red,
            (UINT8)pt_ipcc_att->t_wnd_edge_color.ui1_green,
            (UINT8)pt_ipcc_att->t_wnd_edge_color.ui1_blue ));
    }

    /* Window edge color opacity */
    DBG_INFO(( "\tWindow edge opacity: " ));
    if ( pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_WND_EDGE_COLOR_OPC_AS_BRDCST )
    {
        ui4_undo_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_BORDER_ALPHA;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui4_flag_ex |= WGL_CC_OVERRIDE_FLAG_EX_COLOR_WIN_BORDER_ALPHA;

        _sbtl_ipcc_wgt_conv_hdlr_color_2_gl_color(
            &pt_ipcc_att->t_wnd_edge_color,
            &t_cc_over_attr.t_color_win_border,
            NULL );

        DBG_INFO(( "\t%s%s%s%s\r\n", 
            ((pt_ipcc_att->t_wnd_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_SOLID)?"solid":""),
            ((pt_ipcc_att->t_wnd_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_FLASH)?"flash":""),
            ((pt_ipcc_att->t_wnd_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSLUCENT)?"translucent":""),
            ((pt_ipcc_att->t_wnd_edge_color.e_opacity==SM_CCH_DTVCC_OPACITY_TRANSPARENT)?"transparent":"") ));
    }
    
    /* Window edge type */
    DBG_INFO(( "\tWindow edge type: " ));
    if ( (pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_WND_EDGE_TYPE_AS_BRDCST ) )
    {
        ui2_undo_flag |= WGL_CC_OVERRIDE_FLAG_BORDER_TYPE_WIN;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_BORDER_TYPE_WIN;

        t_cc_over_attr.ui1_border_type_win = (UINT8)pt_ipcc_att->e_wnd_edge_type;

        DBG_INFO(( "\t%s%s%s%s%s%s\r\n", 
            ((pt_ipcc_att->e_wnd_edge_type==SM_CCH_DTVCC_EDGE_TYPE_NONE)?"none":""),
            ((pt_ipcc_att->e_wnd_edge_type==SM_CCH_DTVCC_EDGE_TYPE_RAISED)?"raised":""),
            ((pt_ipcc_att->e_wnd_edge_type==SM_CCH_DTVCC_EDGE_TYPE_DEPRESSED)?"depressed":""),
            ((pt_ipcc_att->e_wnd_edge_type==SM_CCH_DTVCC_EDGE_TYPE_UNIFORM)?"uniform":""),
            ((pt_ipcc_att->e_wnd_edge_type==SM_CCH_DTVCC_EDGE_TYPE_LEFT_DROP_SHADOW)?"left drop shadow":""),
            ((pt_ipcc_att->e_wnd_edge_type==SM_CCH_DTVCC_EDGE_TYPE_RIGHT_DROP_SHADOW)?"right drop shadow":"") ));
    }

    /* Window justify */
    DBG_INFO(( "\tWindow justify type: " ));
    if ( (pt_ipcc_att->ui4_att_flag & SM_CCH_DTVCC_ATT_FLAG_WND_JUSTIFY_AS_BRDCST ) )
    {
        ui2_undo_flag |= WGL_CC_OVERRIDE_FLAG_JUSTIFY_WIN;
        DBG_INFO(( "\tAs Broadcast\r\n" ));
    }
    else
    {   
        t_cc_over_attr.ui2_flag |= WGL_CC_OVERRIDE_FLAG_JUSTIFY_WIN;

        t_cc_over_attr.ui1_justify = 
            _sbtl_ipcc_wgt_conv_hdlr_justify_2_cc_wgt_justify(
                pt_ipcc_att->e_wnd_jusify );

        DBG_INFO(( "\t%s%s%s%s%s%s\r\n", 
            ((pt_ipcc_att->e_wnd_jusify==SM_CCH_DTVCC_JUSTIFY_LEFT)?"justify_left":""),
            ((pt_ipcc_att->e_wnd_jusify==SM_CCH_DTVCC_JUSTIFY_RIGHT)?"justify_right":""),
            ((pt_ipcc_att->e_wnd_jusify==SM_CCH_DTVCC_JUSTIFY_CENTER)?"justify_center":""),
            ((pt_ipcc_att->e_wnd_jusify==SM_CCH_DTVCC_JUSTIFY_FULL)?"justify_full":"") ));
    }
    
    DBG_INFO(( "\tOverride flag: (0x%.4x), ex(0x%.8x)\r\n",
         t_cc_over_attr.ui2_flag, t_cc_over_attr.ui4_flag_ex ));
    DBG_INFO(( "\tUndo flag: (0x%.4x), ex(0x%.8x)\r\n",
         ui2_undo_flag, ui4_undo_flag_ex ));
    DBG_INFO(( "\tApply to CC wgt #: " ));
    for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ++ui4_i )
    {
        DBG_INFO(( "%d(", ui4_i ));
        if ( t_cc_over_attr.ui2_flag != 0 || 
             t_cc_over_attr.ui4_flag_ex != 0 )
        {
            i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                                   WGL_CMD_CC_OVERRIDE_ATTR, 
                                   (VOID*)&t_cc_over_attr,
                                   (VOID*)NULL );
            if ( i4_ret < WGLR_OK )
            {
                DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_OVERRIDE_ATTR) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
            }
            DBG_INFO(( "O" ));
        }
        
        if ( ui2_undo_flag != 0 )
        {
            i4_ret = x_wgl_do_cmd( pt_ipcc_wgt->ah_cc_wnd[ui4_i],
                                   WGL_CMD_CC_UNDO_OVERRIDE_ATTR, 
                                   (VOID*)(UINT32)ui2_undo_flag,
                                   (VOID*)(UINT32)ui4_undo_flag_ex );
            if ( i4_ret < WGLR_OK )
            {
                DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_UNDO_OVERRIDE_ATTR) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
            }
            DBG_INFO(( ",U" ));
        }
        DBG_INFO(( "), " ));
    }   
    DBG_INFO(( "\r\n" ));

}



/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_wgt_chg_cc_visible
 *
 * Description: This API is used to change cc visible.
 *
 * Inputs:  pt_ipcc_wgt          - 
 *          b_is_visible         - Visible flag
 *
 * Outputs:
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_wgt_chg_cc_visible
( 
    IN SBTL_IPCC_WGT_T*  pt_ipcc_wgt,
    IN BOOL              b_is_visible
)
{
    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR(("\n{IPCC} ERR: pt_ipcc_wgt is NULL!   <%d, %s, %s>\n\r",  __LINE__, __func__, __FILE__));
        return IPCCR_INV_ARG; 
    }
    
    if (NULL_HANDLE != pt_ipcc_wgt->h_frame)
    {
        if (TRUE == gb_ipcc_visible)
        {
            x_wgl_set_visibility(pt_ipcc_wgt->h_frame, WGL_SW_NORMAL);
        }
        else
        {
            x_wgl_set_visibility(pt_ipcc_wgt->h_frame, WGL_SW_HIDE);
        }
        x_wgl_repaint(pt_ipcc_wgt->h_frame, NULL, TRUE);
    }
    return IPCCR_OK;
}



/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_wgt_create
 *
 * Description: This API is used to create ipcc widget.
 *
 * Inputs:  h_canvas          - The handle of canvas
 *          ps_aspect_ratio   - The aspect ratio
 *
 * Outputs: 
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_wgt_create
(
    IN HANDLE_T h_canvas, 
    IN CHAR*    ps_aspect_ratio
)
{
    INT32       i4_ret  = IPCCR_OK;
    UINT32      ui4_i = 0;
    GL_RECT_T   t_rect;
    UINT32      ui4_canvas_x = 0;
    UINT32      ui4_canvas_y = 0;
    UINT32      ui4_org_canvas_w = 0;
    UINT32      ui4_org_canvas_h = 0;
    SM_CC_ASPECT_RATIO_T  t_aspect_ratio = SM_CC_ASPECT_RATIO_OTHER;
    SM_CC_ASPECT_RATIO_T  t_user_aspect_ratio = _sbtl_ipcc_wgt_get_asp_ratio();

    if ( !x_handle_valid( h_canvas ) )
    {
        DBG_ERROR(("{IPCC} ERR: Invalid h_canvas!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_OUT_OF_HANDLE;
    }

    SBTL_IPCC_WGT_T*  pt_ipcc_wgt = (SBTL_IPCC_WGT_T*)x_mem_alloc( sizeof(SBTL_IPCC_WGT_T) );
    if(NULL == pt_ipcc_wgt)
    {
        DBG_ERROR(("{IPCC} ERR: No enough memory!   <%d, %s, %s>\n\r", __LINE__, __func__, __FILE__));
        return IPCCR_INSUFFICIENT_SIZE;
    }
    x_memset( pt_ipcc_wgt, 0, sizeof(SBTL_IPCC_WGT_T) );

    pt_ipcc_wgt->h_canvas = h_canvas;
    pt_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0xFF;
    pt_ipcc_wgt->ui1_cc_wnd_alive_mask = 0;

    if ( t_user_aspect_ratio != SM_CC_ASPECT_RATIO_UNKNOWN )
    {
        t_aspect_ratio = t_user_aspect_ratio;
    }
    else // Support m708:aspectRatio    
    {
        if(0 == x_strcmp("4:3", (const CHAR*)ps_aspect_ratio))
        {
            t_aspect_ratio = SM_CC_ASPECT_RATIO_4_3;
        }
        else if(0 == x_strcmp("16:9", (const CHAR*)ps_aspect_ratio))
        {
            t_aspect_ratio = SM_CC_ASPECT_RATIO_16_9;
        }
        else
        {
            t_aspect_ratio = SM_CC_ASPECT_RATIO_OTHER;
        }   
    }

    if ( t_aspect_ratio == SM_CC_ASPECT_RATIO_4_3 )
    {
        pt_ipcc_wgt->ui1_max_anchor_v = 75;
        pt_ipcc_wgt->ui1_max_anchor_h = 160;
    }
    if ( t_aspect_ratio == SM_CC_ASPECT_RATIO_16_9 )
    {
        pt_ipcc_wgt->ui1_max_anchor_v = 75;
        pt_ipcc_wgt->ui1_max_anchor_h = 210;
    }
    if ( t_aspect_ratio == SM_CC_ASPECT_RATIO_OTHER )
    {
        pt_ipcc_wgt->ui1_max_anchor_v = 100;
        pt_ipcc_wgt->ui1_max_anchor_h = 100;
    }

    i4_ret = _sbtl_ipcc_wgt_set_order();
    if ( IPCCR_OK != i4_ret )
    {
        x_mem_free(pt_ipcc_wgt);
        pt_ipcc_wgt = NULL;
        return i4_ret;
    }


    i4_ret = x_wgl_get_canvas_size( 
        pt_ipcc_wgt->h_canvas, &ui4_org_canvas_w, &ui4_org_canvas_h);
    if ( i4_ret != WGLR_OK )
    {
        x_mem_free( pt_ipcc_wgt );
        pt_ipcc_wgt = NULL;
        return IPCCR_INTERNAL_ERROR;
    }

    _sbtl_ipcc_wgt_set_globle_org_canvas( ui4_org_canvas_w, ui4_org_canvas_h );

    pt_ipcc_wgt->ui4_canvas_w = ui4_org_canvas_w * gui1_width_percent / 100;
    pt_ipcc_wgt->ui4_canvas_h = ui4_org_canvas_h * gui1_height_percent / 100; 

    /* For conversion rule from 75% to 90% */
    if(TRUE == gb_apply_convert_rule)
    {
        if(SBTL_IPCC_FILE_TYPE_CRACKLE == gt_video_info.e_file_type)
        {
            if((gui1_width_percent < 100) && (gui1_height_percent < 100))
            {
                ui4_canvas_x = (ui4_org_canvas_w * (100 - gui1_width_percent) / 200);
                ui4_canvas_y = (ui4_org_canvas_h * (100 - gui1_height_percent) / 200);
            }
            else
            {
                ui4_canvas_x = (ui4_org_canvas_w * (100 - 90) / 200);
                ui4_canvas_y = (ui4_org_canvas_h * (100 - 90) / 200);
            }
        }
        else if(SBTL_IPCC_FILE_TYPE_SENVU == gt_video_info.e_file_type)
        {
            if(SM_CC_ASPECT_RATIO_4_3 == gt_aspect_ratio)
            {
                if(ui4_org_canvas_w > gt_video_info.ui2_video_width )
                {
                    ui4_canvas_x = (UINT32)((ui4_org_canvas_w - gt_video_info.ui2_video_width) / 2);
                }
                else
                {
                    ui4_canvas_x = 0;
                }            

                if(gui1_height_percent < 100)
                {
                    ui4_canvas_y = (ui4_org_canvas_h * (100 - gui1_height_percent) / 200);
                }
                else
                {
                    ui4_canvas_y = (ui4_org_canvas_h * (100 - 90) / 200);
                }
            }
            else if(SM_CC_ASPECT_RATIO_16_9 == gt_aspect_ratio)
            {
                if((gui1_width_percent < 100) && (gui1_height_percent < 100))
                {
                    ui4_canvas_x = (ui4_org_canvas_w * (100 - gui1_width_percent) / 200);
                    ui4_canvas_y = (ui4_org_canvas_h * (100 - gui1_height_percent) / 200);
                }
                else 
                {
                    ui4_canvas_x = (ui4_org_canvas_w * (100 - 90) / 200);
                    ui4_canvas_y = (ui4_org_canvas_h * (100 - 90) / 200);
                }
            }
        }
        
    }
    else
    {
        if((gui1_width_percent < 100) && (gui1_height_percent < 100))
        {
            ui4_canvas_x = (ui4_org_canvas_w * (100 - gui1_width_percent) / 200);
            ui4_canvas_y = (ui4_org_canvas_h * (100 - gui1_height_percent) / 200);
        }
        else
        {
            ui4_canvas_x = (ui4_org_canvas_w * (100 - 90) / 200);
            ui4_canvas_y = (ui4_org_canvas_h * (100 - 90) / 200);
        }
        
    }
    

    DBG_INFO(("{IPCC} INFO: ui4_canvas_w = %u, ui4_canvas_h = %u; x=%u, y=%u    <%d, %s, %s>",
                pt_ipcc_wgt->ui4_canvas_w, pt_ipcc_wgt->ui4_canvas_h, 
                ui4_canvas_x, ui4_canvas_y, __LINE__, __func__, __FILE__));


    /* create main frame */
    SET_RECT_BY_SIZE( &t_rect, 
                      (INT32) ui4_canvas_x, // the start position of main frame 
                      (INT32) ui4_canvas_y, 
                      (INT32) pt_ipcc_wgt->ui4_canvas_w, 
                      (INT32) pt_ipcc_wgt->ui4_canvas_h );

    i4_ret = x_wgl_create_widget(
        pt_ipcc_wgt->h_canvas,
        HT_WGL_WIDGET_FRAME,
        WGL_CONTENT_FRAME_DEF,
        WGL_BORDER_NULL,
        &t_rect,
        NULL, //_sbtl_ipcc_dtv_wgt_frame_msg_proc,
        0,
        (VOID*)(WGL_STL_GL_NO_IMG_UI | WGL_STL_GL_NO_BK),
        (VOID*)(pt_ipcc_wgt),
        &(pt_ipcc_wgt->h_frame) );
    
    if( WGLR_OK < i4_ret)
    {
        x_mem_free( pt_ipcc_wgt );
        pt_ipcc_wgt = NULL;
        return IPCCR_INTERNAL_ERROR;
    }

     /* Warning, better don't use WGL_SW_RECURSIVE. */
    if (TRUE == gb_ipcc_visible)
    {
        x_wgl_show( pt_ipcc_wgt->h_frame, WGL_SW_NORMAL );
    }
    else
    {
        x_wgl_show( pt_ipcc_wgt->h_frame, WGL_SW_HIDE );
    }
    x_wgl_repaint(pt_ipcc_wgt->h_frame, NULL, TRUE);
    
    x_memset( pt_ipcc_wgt->ah_cc_wnd, 0, sizeof(HANDLE_T)*IPCC_MAX_WGT_NUM );
    x_memset( pt_ipcc_wgt->at_cc_info, 0, sizeof(WGL_CC_INFO_T)*IPCC_MAX_WGT_NUM );
    x_memset( pt_ipcc_wgt->aui1_z_order, 0, sizeof(UINT8)*IPCC_MAX_WGT_NUM );
    x_memset( pt_ipcc_wgt->aui1_hdend_z_order, 0, sizeof(UINT8)*IPCC_MAX_WGT_NUM );

    /* init CC info structure */
    _sbtl_ipcc_wgt_reset_cc_info_initor( ui4_org_canvas_w, ui4_org_canvas_h );
    
    for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ui4_i++ )
    {
        i4_ret = _sbtl_ipcc_wgt_create_wnd( pt_ipcc_wgt->h_frame,
                                            &gt_ipcc_info_initor,
                                            &(pt_ipcc_wgt->ah_cc_wnd[ui4_i]) );
        if ( i4_ret != IPCCR_OK )
        {
            sbtl_ipcc_wgt_destory( pt_ipcc_wgt );
            pt_ipcc_wgt = NULL;
            return IPCCR_INTERNAL_ERROR;
        }
        
        pt_ipcc_wgt->at_cc_info[ui4_i] = gt_ipcc_info_initor.t_cc_info;
        pt_ipcc_wgt->aui1_z_order[ui4_i] = (UINT8) ui4_i;
        pt_ipcc_wgt->aui1_hdend_z_order[ui4_i] = (UINT8) ui4_i;
        pt_ipcc_wgt->aui1_text_tag[ui4_i] = CAP_CMD_TEXT_TAG_DIALOG;
        
        i4_ret = _sbtl_ipcc_wgt_chg_z_order( pt_ipcc_wgt, 
                                             (UINT8) ui4_i,
                                             pt_ipcc_wgt->aui1_z_order[ui4_i] );
        if ( i4_ret != IPCCR_OK )
        {
            sbtl_ipcc_wgt_destory( pt_ipcc_wgt );
            pt_ipcc_wgt = NULL;
            return IPCCR_INTERNAL_ERROR;
        }

        _sbtl_ipcc_wgt_move_pen_according_to_scroll_n_print_dir( 
            pt_ipcc_wgt, (UINT8)ui4_i );
    }

    sbtl_ipcc_wgt_set_wnd_attr( pt_ipcc_wgt, &t_sbtl_ipcc_attr );

    pt_sbtl_ipcc_wgt = pt_ipcc_wgt;

    return IPCCR_OK;   
}



/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_wgt_destory
 *
 * Description: This API is used to destory ipcc widget.
 *
 * Inputs:  pt_ipcc_wgt          - The pointer of ipcc widget
 *
 * Outputs:
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_wgt_destory( IN SBTL_IPCC_WGT_T*    pt_ipcc_wgt )
{
    IPCC_CHECK_NULL(pt_ipcc_wgt,IPCCR_INV_ARG);

    UINT32 ui4_i = 0;
    for ( ui4_i = 0; ui4_i < IPCC_MAX_WGT_NUM; ui4_i++ )
    { 
        if ( pt_ipcc_wgt->ah_cc_wnd[ui4_i]!=NULL_HANDLE )
        {
            _sbtl_ipcc_wgt_destroy_wnd( pt_ipcc_wgt->ah_cc_wnd[ui4_i] );
        }

        gb_is_crnt_wnd_defined[ui4_i] = FALSE;
    }
    
    if ( pt_ipcc_wgt->h_frame!=NULL_HANDLE )
    {
        x_wgl_destroy_widget( pt_ipcc_wgt->h_frame );
        pt_ipcc_wgt->h_frame = NULL_HANDLE;
    }

    x_mem_free( pt_ipcc_wgt );
    
    return IPCCR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_wgt_remove
 *
 * Description: This API is used to remove window.
 *
 * Inputs:  ui1_wnd_num          - Number of window
 *
 * Outputs:
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_wgt_remove(IN UINT32 ui4_crnt_wnd)
{    
    INT32 i4_ret = IPCCR_OK;

    if(SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode
        || SBTL_IPCC_CAPTION_MODE_PAINT_ON == ge_crnt_caption_mode
        || (SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode
            && 0xFF == ui4_crnt_wnd))
    {
        i4_ret = _sbtl_ipcc_wgt_clear_wnd(ui4_crnt_wnd, pt_sbtl_ipcc_wgt);
        if(IPCCR_OK != i4_ret)
        {
            return i4_ret;
        }
            
        i4_ret = _sbtl_ipcc_wgt_hide_wnd(ui4_crnt_wnd, pt_sbtl_ipcc_wgt);
        if(IPCCR_OK != i4_ret)
        {
            return i4_ret;
        }
    }   
    else if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
    {
        /* CR cmd */
        i4_ret = x_wgl_do_cmd( pt_sbtl_ipcc_wgt->ah_cc_wnd[ pt_sbtl_ipcc_wgt->ui1_curr_wnd_num ],
                              WGL_CMD_CC_MOVE_PEN_TO_NEXT_PARAG, 
                              (VOID*)FALSE,
                              (VOID*)NULL );
        if ( i4_ret < WGLR_OK )
        {
           DBG_ERROR(( "{IPCC} ERR: x_wgl_do_cmd(WGL_CMD_CC_MOVE_PEN_TO_NEXT_PARAG) ERROR, return %d   <%d, %s, %s>\r\n", i4_ret,__LINE__, __func__, __FILE__ ));
           return IPCCR_INTERNAL_ERROR;
        }        
           
        i4_ret = x_wgl_do_cmd( 
                               pt_sbtl_ipcc_wgt->ah_cc_wnd[ pt_sbtl_ipcc_wgt->ui1_curr_wnd_num ],
                               WGL_CMD_CC_UPDATE, 
                               (VOID*)WGL_CC_UPDATE_MODE_AUTO_SMTH_SCRL,
                               (VOID*)NULL );
        if ( i4_ret < WGLR_OK )
        {
           DBG_ERROR(( "{IPCC} x_wgl_do_cmd(WGL_CMD_CC_UPDATE) ERROR, return %d   <%d, %s, %s>\r\n",i4_ret,__LINE__, __func__, __FILE__ ));
           return IPCCR_INTERNAL_ERROR;
        }
   
    }
    
    return IPCCR_OK;
}




/*-----------------------------------------------------------------------------
 * Name: sbtl_ipcc_wgt
 *
 * Description: This API is used to process new attributes.
 *
 * Inputs:  pt_ttml_data          - The pointer of attribute list
 *
 * Outputs:
 *
 * Returns: IPCCR_OK              - Success
 *          IPCCR_INV_ARG         - Invalid argument
 *          IPCCR_INTERNAL_ERROR  - Internal error
 *
 ----------------------------------------------------------------------------*/
INT32 sbtl_ipcc_wgt(IN WGT_TTML_DATA_CMD_T* pt_ttml_data)
{
    IPCC_CHECK_NULL(pt_ttml_data,IPCCR_INV_ARG);   
    
    INT32 i4_ret                            = IPCCR_OK;
    INT32 i4_p_count                        = pt_ttml_data->i4_ele_p_count;
    UINT8 aui1_wnd_num[IPCC_MAX_WGT_NUM]    = {8,8,8,8,8,8,8,8};
    UINT8 ui1_wnd_array_index               = 0;

    /* Get <p> element */
    if(!DLIST_IS_EMPTY(&pt_ttml_data->t_element_p_list))
    {
        WGT_ELEMENT_P_CMD_T* pt_wgt_element_p = DLIST_HEAD(&pt_ttml_data->t_element_p_list);        
        
        /* Process <p> list */
        while(pt_wgt_element_p)
        {
            /* Set widget window number */
            pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = pt_wgt_element_p->ui1_region_id;

        
            /* Check if crnt wnd is defined or not */
            if(SBTL_IPCC_CAPTION_MODE_POP_ON != ge_crnt_caption_mode)
            {
                if(FALSE == gb_is_crnt_wnd_defined[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num]
                    || ge_crnt_caption_mode != ge_last_caption_mode)
                {
                    UINT32 ui4_crnt_wnd_num = (UINT32)pt_sbtl_ipcc_wgt->ui1_curr_wnd_num;
                    i4_ret = _sbtl_ipcc_wgt_define_wnd(ui4_crnt_wnd_num, pt_sbtl_ipcc_wgt);
                    if(IPCCR_OK != i4_ret)
                    {
                        DBG_ERROR(("{IPCC} ERR: Update wnd failed, crnt wnd is %u!   <%d, %s, %s>\n\r", ui4_crnt_wnd_num, __LINE__, __func__, __FILE__));
                        return IPCCR_INV_ARG; 
                    }
                    else
                    {
                        //DBG_INFO(("{IPCC} INFO: Update wnd success, crnt wnd is %u!   <%d, %s, %s>\n\r", ui4_crnt_wnd_num, __LINE__, __func__, __FILE__));
                        gb_is_crnt_wnd_defined[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num] = TRUE;
                    }
                }     
            }
            else
            {
                UINT32 ui4_crnt_wnd_num = (UINT32)pt_sbtl_ipcc_wgt->ui1_curr_wnd_num;
                i4_ret = _sbtl_ipcc_wgt_define_wnd(ui4_crnt_wnd_num, pt_sbtl_ipcc_wgt);
                if(IPCCR_OK != i4_ret)
                {
                    DBG_ERROR(("{IPCC} ERR: Update wnd failed, crnt wnd is %u!   <%d, %s, %s>\n\r", ui4_crnt_wnd_num, __LINE__, __func__, __FILE__));
                    return IPCCR_INV_ARG; 
                }
                else
                {
                    //DBG_INFO(("{IPCC} INFO: Update wnd success, crnt wnd is %u!   <%d, %s, %s>\n\r", ui4_crnt_wnd_num, __LINE__, __func__, __FILE__));
                    gb_is_crnt_wnd_defined[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num] = TRUE;
                }
            }
            
            if(!DLIST_IS_EMPTY(&pt_wgt_element_p->t_element_span_list))
            {
                WGT_ELEMENT_SPAN_CMD_T* pt_wgt_element_span = DLIST_HEAD(&pt_wgt_element_p->t_element_span_list);

                /* Process <span> list */
                while(pt_wgt_element_span)
                {
                    WGT_ATTR_CMD_T* pt_wgt_attr_cmd = DLIST_HEAD(&pt_wgt_element_span->t_attr_list);

                    /* Process attr list of <span>*/
                    while(pt_wgt_attr_cmd)
                    {
                        switch(pt_wgt_attr_cmd->e_attr_type)
                        {
                            /* Set pen color */
                            case WGT_ATTR_TYPE_SET_PEN_COLOR:
                                {
                                 
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num == (UINT8)0xFF)
                                    {
                                        pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0;
                                    }
                                                                    
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM)
                                    {
                                        i4_ret = _sbtl_ipcc_wgt_set_pen_color(pt_sbtl_ipcc_wgt, 
                                                                                &(pt_wgt_attr_cmd->u.t_attr_pen_color), 
                                                                                pt_sbtl_ipcc_wgt->ui1_curr_wnd_num);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: Set pen color failed, return %d!   <%d, %s, %s>\n\r", 
                                                        i4_ret, __LINE__, __func__, __FILE__));
                                        }
                                        else
                                        {
                                            pt_sbtl_ipcc_wgt->at_cc_info[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].ui2_cc_info_flags |=
                                                                                            WGL_CC_INFO_IS_VALID_PEN_COLOR;
                                            //b_is_update_cc_attr = TRUE;
  
                                            /* Update win attr */
                                            _sbtl_ipcc_wgt_update_attr(pt_sbtl_ipcc_wgt);
                                              
                                        }
                                    }
                                    else
                                    {
                                        DBG_ERROR(("{IPCC} ERR: Set pen color failed, current window is %u!   <%d, %s, %s>\n\r", 
                                                    pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                                    }
                                                                    
                                    x_mem_free(pt_wgt_attr_cmd);
                                    DLIST_REMOVE(pt_wgt_attr_cmd, &pt_wgt_element_span->t_attr_list, t_attr_link);
                                }
                            
                                break;

                            /* Set win anchor */
                            case WGT_ATTR_TYPE_SET_WIN_ANCHOR: 
                                {       
                                    
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num == (UINT8)0xFF)
                                    {
                                        pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0;
                                    }

                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM)
                                    {
                                        i4_ret = _sbtl_ipcc_wgt_set_win_anchor(pt_sbtl_ipcc_wgt,
                                                                               &(pt_wgt_attr_cmd->u.t_win_anchor),
                                                                               pt_sbtl_ipcc_wgt->ui1_curr_wnd_num);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: Set window anchor failed, return %d!   <%d, %s, %s>\n\r", 
                                                        i4_ret, __LINE__, __func__, __FILE__));
                                        }                                      
                                    }
                                    else
                                    {
                                        DBG_ERROR(("{IPCC} ERR: Set window anchor failed, current window is %u!   <%d, %s, %s>\n\r", 
                                                    pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                                    }
                                    

                                    x_mem_free(pt_wgt_attr_cmd);
                                    DLIST_REMOVE(pt_wgt_attr_cmd, &pt_wgt_element_span->t_attr_list, t_attr_link);
                                }
                                
                                break;

                            /* Set win size */
                            case WGT_ATTR_TYPE_SET_WIN_SIZE:
                                {        
                                    
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num == (UINT8)0xFF)
                                    {
                                        pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0;

                                    }
                                     
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM)
                                    {
                                        i4_ret = _sbtl_ipcc_wgt_set_win_size(pt_sbtl_ipcc_wgt,
                                                                             &(pt_wgt_attr_cmd->u.t_win_size),
                                                                             pt_sbtl_ipcc_wgt->ui1_curr_wnd_num);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: Set window size failed, return %d!   <%d, %s, %s>\n\r", 
                                                        i4_ret, __LINE__, __func__, __FILE__));
                                        }
                                        else
                                        {                                                                                         
                                           /* Update win attr */
                                            _sbtl_ipcc_wgt_update_attr(pt_sbtl_ipcc_wgt); 
                                        }
                                    }
                                    else
                                    {
                                        DBG_ERROR(("{IPCC} ERR: Set window size failed, current window is %u!   <%d, %s, %s>\n\r", 
                                                    pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                                    }
                                    
                                    x_mem_free(pt_wgt_attr_cmd);
                                    DLIST_REMOVE(pt_wgt_attr_cmd, &pt_wgt_element_span->t_attr_list, t_attr_link);
                                }
                                    
                                break;

                                
                            /* Set pen font */
                            case WGT_ATTR_TYPE_SET_PEN_FONT:
                                {         
                                    
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num == (UINT8)0xFF)
                                    {
                                        pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0;

                                    }
                                    
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM)
                                    {
                                        i4_ret = _sbtl_ipcc_wgt_set_pen_font(pt_sbtl_ipcc_wgt,
                                                                             &(pt_wgt_attr_cmd->u.t_pen_font),
                                                                             pt_sbtl_ipcc_wgt->ui1_curr_wnd_num);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: Set pen attr failed, return %d!   <%d, %s, %s>\n\r", 
                                                        i4_ret, __LINE__, __func__, __FILE__));
                                        }
                                        else
                                        {
                                             pt_sbtl_ipcc_wgt->at_cc_info[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].ui2_cc_info_flags |=
                                                                                            WGL_CC_INFO_IS_VALID_PEN_FONT;
                                             
                                            /* Update win attr */
                                            _sbtl_ipcc_wgt_update_attr(pt_sbtl_ipcc_wgt);
                                        }
                                    }
                                    else
                                    {
                                        DBG_ERROR(("{IPCC} ERR: Set pen attributes failed, current window is %u!   <%d, %s, %s>\n\r", 
                                                    pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                                    }
                                    
                                    x_mem_free(pt_wgt_attr_cmd); 
                                    DLIST_REMOVE(pt_wgt_attr_cmd, &pt_wgt_element_span->t_attr_list, t_attr_link);
                                }
                                                              
                                break;


                            /* Set text tag*/
                            case WGT_ATTR_TYPE_SET_TEXT_TAG:
                                {
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num == (UINT8)0xFF)
                                    {
                                        pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0;

                                    }
                                    
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM)
                                    {
                                        i4_ret = _sbtl_ipcc_wgt_set_text_tag(pt_sbtl_ipcc_wgt,
                                                                            &(pt_wgt_attr_cmd->u.ui1_text_tag),
                                                                            pt_sbtl_ipcc_wgt->ui1_curr_wnd_num);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: Set text tag failed, return %d!   <%d, %s, %s>\n\r", 
                                                        i4_ret, __LINE__, __func__, __FILE__));
                                        }                                        
                                    }
                                    else
                                    {
                                        DBG_ERROR(("{IPCC} ERR: Set text tag failed, current window is %u!   <%d, %s, %s>\n\r", 
                                                    pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                                    }
                                    
                                    x_mem_free(pt_wgt_attr_cmd); 
                                    DLIST_REMOVE(pt_wgt_attr_cmd, &pt_wgt_element_span->t_attr_list, t_attr_link);
                                }
                                break;

                            /* Set win attr */
                            case WGT_ATTR_TYPE_SET_WIN_ATTR:
                                {     
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num == (UINT8)0xFF)
                                    {
                                        pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0;
                                    }
                                   
                                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM)
                                    {
                                        i4_ret = _sbtl_ipcc_wgt_set_win_attr(pt_sbtl_ipcc_wgt,
                                                                             &(pt_wgt_attr_cmd->u.t_attr_win),
                                                                             pt_sbtl_ipcc_wgt->ui1_curr_wnd_num);
                                        if(IPCCR_OK != i4_ret)
                                        {
                                            DBG_ERROR(("{IPCC} ERR: Set window attr failed, return %d!   <%d, %s, %s>\n\r", 
                                                        i4_ret, __LINE__, __func__, __FILE__));
                                        }
                                        else
                                        {
                                             pt_sbtl_ipcc_wgt->at_cc_info[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].ui2_cc_info_flags |=
                                                                                            WGL_CC_INFO_IS_VALID_WIN_ATTR;

                                             
                                            /* Update win attr */
                                            _sbtl_ipcc_wgt_update_attr(pt_sbtl_ipcc_wgt);  
                                           
                                        }
                                    }
                                    else
                                    {
                                        DBG_ERROR(("{IPCC} ERR: Set window attributes failed, current window is %u!   <%d, %s, %s>\n\r", 
                                                    pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                                    }
                                                                        
                                    x_mem_free(pt_wgt_attr_cmd);
                                    DLIST_REMOVE(pt_wgt_attr_cmd, &pt_wgt_element_span->t_attr_list, t_attr_link);
                                }
                                
                                break;


                           case WGT_ATTR_TYPE_SET_FONT_NAME:
                           default:
                                break;

                        }
                        
                        pt_wgt_attr_cmd = DLIST_HEAD(&pt_wgt_element_span->t_attr_list);
                    }
                
                    /* Process text of caption */
                    i4_ret = _sbtl_ipcc_wgt_text(pt_sbtl_ipcc_wgt, pt_wgt_element_span->ps_content);
                    if(IPCCR_OK != i4_ret)
                    {
                        DBG_ERROR(("{IPCC} ERR: Process text of caption failed, return %d!   <%d, %s, %s>\n\r", 
                                                        i4_ret, __LINE__, __func__, __FILE__));
                        
                    }                   

                    x_mem_free(pt_wgt_element_span->ps_content);
                    
                    x_mem_free(pt_wgt_element_span);
                    DLIST_REMOVE(pt_wgt_element_span, &pt_wgt_element_p->t_element_span_list, t_element_span_link);
                    
                    pt_wgt_element_span = DLIST_HEAD(&pt_wgt_element_p->t_element_span_list);
                }
            } 
        
            
            if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num == (UINT8)0xFF)
            {
                pt_sbtl_ipcc_wgt->ui1_curr_wnd_num = (UINT8)0;
            }
            
            /* Set pen location */
            UINT16 ui2_idx_parag = 0;
            UINT16 ui2_col = 0;
            WGL_CC_TEXT_PARAG_LOCATION_T t_pen_loc = {0, 0};

            if(SBTL_IPCC_CAPTION_MODE_ROLL_UP == ge_crnt_caption_mode)
            {
                if(SBTL_TTML_ATTR_TEXT_ALIGIN_END == gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].e_text_align)
                {
                    ui2_col = gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].t_pen_loc.ui2_col;
                }
                ui2_idx_parag = gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].t_win_size.ui2_row_count - 1;
            }
            else if(SBTL_IPCC_CAPTION_MODE_PAINT_ON == ge_crnt_caption_mode
                    || SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
            {
                /* If tts:textAlian is end */
                if(SBTL_TTML_ATTR_TEXT_ALIGIN_END == gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].e_text_align)
                {
                    ui2_idx_parag = gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].t_pen_loc.ui2_idx_parag;
                    ui2_col = gt_region_attr[pt_sbtl_ipcc_wgt->ui1_curr_wnd_num].t_pen_loc.ui2_col;
                }
            }
            
            t_pen_loc.ui2_col = ui2_col;
            t_pen_loc.ui2_idx_parag = ui2_idx_parag;
                
            i4_ret = _sbtl_ipcc_wgt_set_pen_loc(pt_sbtl_ipcc_wgt, 
                                                &t_pen_loc, 
                                                pt_sbtl_ipcc_wgt->ui1_curr_wnd_num);
            if(IPCCR_OK != i4_ret)
            {
                DBG_ERROR(("{IPCC} ERR: Set pen location failed, return %d!   <%d, %s, %s>\n\r", 
                            i4_ret, __LINE__, __func__, __FILE__));
            }             
            
            /* Display window */
            if(SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
            {
                if(1 == i4_p_count)
                {
                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM)
                    {
                        i4_ret = _sbtl_ipcc_wgt_display_wnd(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, pt_sbtl_ipcc_wgt);
                        if(IPCCR_OK != i4_ret)
                        {
                            DBG_ERROR(("{IPCC} ERR: Display window failed, return %d!   <%d, %s, %s>\n\r", 
                                        i4_ret, __LINE__, __func__, __FILE__));
                        }                        
                    }
                    else
                    {
                        DBG_ERROR(("{IPCC} ERR: Display window failed, current window is %u!   <%d, %s, %s>\n\r", 
                            pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                    }
                }
                else if(i4_p_count > 1)
                {
                    if(pt_sbtl_ipcc_wgt->ui1_curr_wnd_num < (UINT8)IPCC_MAX_WGT_NUM
                        && ui1_wnd_array_index < IPCC_MAX_WGT_NUM)
                    {
                        aui1_wnd_num[ui1_wnd_array_index] = pt_sbtl_ipcc_wgt->ui1_curr_wnd_num;
                        ++ui1_wnd_array_index;
                    }
                    else
                    {
                        DBG_ERROR(("{IPCC} ERR: Save window failed, current window is %u!   <%d, %s, %s>\n\r", 
                            pt_sbtl_ipcc_wgt->ui1_curr_wnd_num, __LINE__, __func__, __FILE__));
                    }
                }

            }           
            
             
            x_mem_free(pt_wgt_element_p);
            DLIST_REMOVE(pt_wgt_element_p, &pt_ttml_data->t_element_p_list, t_element_p_link);
            pt_wgt_element_p = DLIST_HEAD(&pt_ttml_data->t_element_p_list);
        }
        
        if(i4_p_count > 1 && SBTL_IPCC_CAPTION_MODE_POP_ON == ge_crnt_caption_mode)
        {
            x_wgl_show( pt_sbtl_ipcc_wgt->h_frame, WGL_SW_HIDE );
            
            INT32 i4_tmp_count = i4_p_count;
            for(ui1_wnd_array_index = 0; ui1_wnd_array_index < IPCC_MAX_WGT_NUM; ++ui1_wnd_array_index)
            {
                if(8 != aui1_wnd_num[ui1_wnd_array_index] && i4_tmp_count > 0)
                {
                    i4_ret = _sbtl_ipcc_wgt_display_wnd(aui1_wnd_num[ui1_wnd_array_index], pt_sbtl_ipcc_wgt);
                    if(IPCCR_OK != i4_ret)
                    {
                        DBG_ERROR(("{IPCC} ERR: Display window failed, return %d!   <%d, %s, %s>\n\r", 
                                    i4_ret, __LINE__, __func__, __FILE__));
                    }                    
                    --i4_tmp_count;
                }

                if(0 == i4_tmp_count)
                {
                    break;
                }
            }
            
            x_wgl_show( pt_sbtl_ipcc_wgt->h_frame, WGL_SW_NORMAL);
        }
        x_mem_free(pt_ttml_data);
    }
    
    return IPCCR_OK;
}







#endif

