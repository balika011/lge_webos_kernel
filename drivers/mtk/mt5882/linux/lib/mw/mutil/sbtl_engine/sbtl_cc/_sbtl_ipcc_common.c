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
#include "_sbtl_ipcc_common.h"

/*-----------------------------------------------------------------------------
                            Global varible
-----------------------------------------------------------------------------*/
/*---------------------- Common global varible ------------------------------*/
UINT32                  gui4_screen_width           = 0;
UINT32                  gui4_screen_height          = 0;
UINT8                   gui1_width_percent          = 100;
UINT8                   gui1_height_percent         = 100;
FLOAT                   gf_xml_display_ratio        = IPCC_DISPLAY_RATIO_75;
FLOAT                   gf_after_cnvt_display_ratio = IPCC_DISPLAY_RATIO_90;
BOOL                    gb_is_extent_exist          = FALSE; //This flag is used to indicate if the current file contains tts:extent
BOOL                    gb_ipcc_visible             = TRUE;


SM_CC_ASPECT_RATIO_T    gt_aspect_ratio             = SM_CC_ASPECT_RATIO_UNKNOWN;
SBTL_IPCC_VIDEO_T       gt_video_info               = {SBTL_IPCC_VIDEO_TYPE_HD, 
                                                        0, 
                                                        0, 
                                                        SBTL_IPCC_FILE_TYPE_CRACKLE, 
                                                        SBTL_IPCC_FILE_MODE_PRESERVED};

BOOL                        gb_apply_convert_rule               = FALSE; // This flag indicate if the convertion rule need to apply.
BOOL                        gb_is_global_style_set              = FALSE;
UINT8                       gaui1_crnt_wnd[IPCC_MAX_WGT_NUM]    = {8,8,8,8,8,8,8,8};
SBTL_IPCC_WGT_T*            pt_sbtl_ipcc_wgt                    = NULL;
SBTL_TTML_ATTR_ID_LIST_T*   gpt_sbtl_ttml_record_global_style   = NULL;
SBTL_IPCC_CAPTION_MODE      ge_crnt_caption_mode = SBTL_IPCC_CAPTION_MODE_POP_ON;
SBTL_IPCC_CAPTION_MODE      ge_last_caption_mode = SBTL_IPCC_CAPTION_MODE_POP_ON;
BOOL gb_is_crnt_wnd_defined[IPCC_MAX_PEN_STL_NUM]= {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
SBTL_IPCC_REGION_ATTR gt_region_attr[IPCC_MAX_REGION_NUM] = 
{
    {
        " ",
        " ",
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        FE_FNT_SIZE_SMALL,
        "black", 
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    },
    {
        " ",
        " ",    
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW,  
        FE_FNT_SIZE_SMALL,
        "black",  
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    },
    {
        " ",
        " ",    
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW, 
        FE_FNT_SIZE_SMALL,
        "black", 
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    },
    {
        " ",
        " ",
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        FE_FNT_SIZE_SMALL,
        "black",
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    },
    {
        " ",
        " ",
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW, 
        FE_FNT_SIZE_SMALL,
        "black",
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    },
    {
        " ",
        " ",
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW,  
        FE_FNT_SIZE_SMALL,
        "black", 
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    },
    {
        " ",
        " ",
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        FE_FNT_SIZE_SMALL,
        "black", 
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    },
    {
        " ",
        " ",
        0,
        TTML_LENGTH_UNIT_TYPE_UNKNOW, 
        FE_FNT_SIZE_SMALL,
        "black", 
        "0.0",
        "white",
        "black",
        "transparent",
        " ",
        0,
        SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT,
        SM_CC_FONT_STYLE_0,
        SBTL_TTML_ATTR_FONT_STYLE_NORMAL,
        SBTL_TTML_ATTR_FONT_WEGHT_NORMAL,
        SBTL_TTML_ATTR_TEXT_DECORATION_NONE,
        SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT,
        {0, 0, 0},
        {0, 0},
        TTML_LENGTH_UNIT_TYPE_UNKNOW,
        {0, 0},
        "noWrap",
        FALSE,
        "dialog"
    } 
};

SBTL_IPCC_WGT_WND_ATTR gat_wgt_wnd_attr[IPCC_MAX_REGION_NUM] = 
{
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW| WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    },
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW| WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    },
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW| WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    },
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    },
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    },
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    },
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    },
    {
        " ", // region id
        " ", // style id
        { /* pen color */
            CC_COLOR_WHITE,               /* foreground color */
            CC_COLOR_BLACK,               /* background color */
            CC_COLOR_TRANSPARENT,         /* edge color */
            FALSE,                        /* foreground is flash */
            FALSE                         /* background is flash */
        },
        { // pen font
            {
                SM_CC_FONT_STYLE_0,
                FE_FNT_SIZE_MEDIUM, 
                WGL_CC_FNT_STYLE_REGULAR,
                FE_CMAP_ENC_UNICODE,
                WGL_CC_EDGE_TYPE_NONE
            },
            WGL_CC_TEXT_OFFSET_NORMAL
        },
        { // win attr
            WGL_CC_WIN_ATTR_LIMIT_ROW | WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
            WGL_CC_JUSTIFY_LEFT,
            WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
            WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
            WGL_CC_DISPLAY_EFFECT_SNAP,
            0,
            0,
            CC_COLOR_BLACK,
            FALSE,
            WGL_CC_BORDER_TYPE_NONE,
            CC_COLOR_TRANSPARENT
        },
        {0, 0, 0}, // win anchor
        {0, 0}, // win size
        0
    }
};

SBTL_IPCC_WGT_WND_ATTR gt_wgt_wnd_attr = {
                                            " ", // region id
                                            " ", // style id
                                            { /* pen color */
                                                CC_COLOR_WHITE,               /* foreground color */
                                                CC_COLOR_BLACK,               /* background color */
                                                CC_COLOR_TRANSPARENT,         /* edge color */
                                                FALSE,                        /* foreground is flash */
                                                FALSE                         /* background is flash */
                                            },
                                            { // pen font
                                                {
                                                    SM_CC_FONT_STYLE_0,
                                                    FE_FNT_SIZE_MEDIUM, 
                                                    WGL_CC_FNT_STYLE_REGULAR,
                                                    FE_CMAP_ENC_UNICODE,
                                                    WGL_CC_EDGE_TYPE_NONE
                                                },
                                                WGL_CC_TEXT_OFFSET_NORMAL
                                            },
                                            { // win attr
                                                WGL_CC_WIN_ATTR_LIMIT_ROW |WGL_CC_WIN_ATTR_LIMIT_COL | WGL_CC_WIN_ATTR_AUTO_SCROLL | WGL_CC_WIN_ATTR_SMOOTH_SCROLL,
                                                WGL_CC_JUSTIFY_LEFT,
                                                WGL_CC_PRINT_DIR_LEFT_TO_RIGHT,
                                                WGL_CC_SCROLL_DIR_BOTTOM_TO_TOP,
                                                WGL_CC_DISPLAY_EFFECT_SNAP,
                                                0,
                                                0,
                                                CC_COLOR_BLACK,
                                                FALSE,
                                                WGL_CC_BORDER_TYPE_NONE,
                                                CC_COLOR_TRANSPARENT
                                            },
                                            {0, 0, 0}, // win anchor
                                            {1, 32}, // win size
                                            0
                                        };





/*---------------------- Global varible for pop on --------------------------*/


/*---------------------- Global varible for roll up -------------------------*/
BOOL gb_roll_up_1st_set = FALSE; // This roll-up window appear 1st time.
BOOL gb_roll_up_ignore_crnt_p = FALSE;
BOOL gb_is_enhance_rollup = FALSE;
CHAR gps_roll_up_base_row[IPCC_TTS_ORIGIN_LENGTH] = "0% 0%";
CHAR gps_roll_up_last_id[IPCC_TTS_ORIGIN_LENGTH] = "rollup";


/*---------------------- Global varible for paint on -------------------------*/



