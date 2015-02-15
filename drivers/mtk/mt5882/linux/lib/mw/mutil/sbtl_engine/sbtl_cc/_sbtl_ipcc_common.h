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
 
/*----------------------------------------------------------------------------*/
/*! @addtogroup groupMW_STRM_MNGR_MM_SBTL_HDLR
*  @{ 
*/
/*----------------------------------------------------------------------------*/
#ifndef _SBTL_IPCC_COMMON_H_
#define _SBTL_IPCC_COMMON_H_

#ifndef __KERNEL__

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
//#include "u_sm_cc_hdlr.h"
//#include "x_dbg.h"
//#include "u_dbg.h"
//#include "x_wgl.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_xml/sbtl_xml_page_inst_mngr.h"
#include "u_sbtl_ipcc.h"
#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "u_sm_cc_hdlr.h"
#include "x_dbg.h"
#include "u_dbg.h"
#include "x_wgl.h"
#include "x_fe.h"

/*-----------------------------------------------------------------------------
                    macros, defines, enums
-----------------------------------------------------------------------------*/
#define IPCC_MEM_FREE(pt_mem) {x_mem_free(pt_mem); pt_mem = NULL;}
#define IN
#define OUT

#ifdef DEBUG
#define IPCC_CHECK_VALID(_expr, _expect_ret)                           \
    {                                                                       \
        if (_expr != _expect_ret)                                           \
        {                                                                   \
            DBG_ERROR(("{%s} ERR: RET_CHECK_VALID()=%d,  <Line:%d, Func:%s, File:%s> \n", (_expr), __LINE__, __func__, __FILE__));\
            return (_expr);                                                  \
        }                                                                   \
    }                                                                        


#define IPCC_CHECK_NULL(_param, _ret)                                       \
    {                                                                       \
        if (NULL == (_param))                                               \
        {                                                                   \
            DBG_ERROR(("{IPCC} ERR: CHECK_NULL()=%d,  <Line:%d, Func:%s, File:%s> \n", (_ret), __LINE__, __func__, __FILE__));\
            return (_ret);                                                  \
        }                                                                   \
    }                                                                       
#else
#define IPCC_CHECK_VALID(_expr, _expect_ret)                                \
    {                                                                       \
        if (_expr != _expect_ret)                                            \
        {                                                                   \
            return (_expr);                                                 \
        }                                                                   \
    }


#define IPCC_CHECK_NULL(_param, _ret)                                       \
    {                                                                       \
        if (NULL == (_param))                                               \
        {                                                                   \
            return (_ret);                                                  \
        }                                                                   \
    }
#endif

#define IPCC_PREFIX             "{IPCC}: "
#define IPCC_SUFFIX             ("\t<%d, %s, %s>\n", __LINE__, __func__, __FILE__)


#define IPCC_MAX_WGT_NUM        ((UINT32) 8)
#define IPCC_MAX_PEN_STL_NUM    ((UINT32) 8)

#define IPCC_DEFAULT_WINDOW_COLUMN  (UINT32)32
#define IPCC_DEFAULT_WINDOW_ROW     (UINT32)15
#define IPCC_TTS_ORIGIN_LENGTH       30
#define IPCC_LENGTH_UNIT_COUNT       3
#define IPCC_CAPTION_MODE_COUNT      3
#define IPCC_MAX_REGION_NUM          8

#define IPCC_INVALID_VALUE          "ipcc_invalid_value"
#define IPCC_UNDEFINE               "undefine"

#define IPCC_CAPTION_MODE_ROLL      "rollup"
#define IPCC_CAPTION_MODE_POP       "pop"
#define IPCC_CAPTION_MODE_paint     "paint"

#define IPCC_ASPECT_ARTIO_4_3       "4:3"
#define IPCC_ASPECT_ARTIO_16_9      "16:9"
#define IPCC_ASPECT_ARTIO_OTHER     "other"


/* For conversion rule of 80% */
#define IPCC_CONVERT_POINT_SENVU             ((FLOAT)5.333)
#define IPCC_BASE_LINE_HEIGHT_SENVU          ((FLOAT)5.0)
#define IPCC_CONVERT_POINT_CRACKLE           ((FLOAT)80.0)
#define IPCC_CONVERT_CRACKLE_WIDTH_OFFSET    ((FLOAT)10.0)

#define IPCC_SCALE_UP_RATIO_HD_X   ((FLOAT)1.0667)
#define IPCC_SCALE_UP_RATIO_HD_Y   ((FLOAT)1.0667)
#define IPCC_SCALE_UP_RATIO_SD_X   ((FLOAT)1.283)
#define IPCC_SCALE_UP_RATIO_SD_Y   ((FLOAT)1.0667)

#define IPCC_SCALE_UP_OFFSET_HD_A  10
#define IPCC_SCALE_UP_OFFSET_HD_B  10
#define IPCC_SCALE_UP_OFFSET_SD_A  5
#define IPCC_SCALE_UP_OFFSET_SD_B  10

/* The unit of following marcos is "%" */
#define IPCC_COL_UNIT_75     ((FLOAT)2.3438)
#define IPCC_ROW_UNIT_75     ((FLOAT)5.0)
#define IPCC_COL_UNIT_90_HD  ((FLOAT)2.5)
#define IPCC_COL_UNIT_90_SD  ((FLOAT)3.0078125)
#define IPCC_ROW_UNIT_90     ((FLOAT)6.0)

#define IPCC_DISPLAY_RATIO_75    ((FLOAT)75.0)
#define IPCC_DISPLAY_RATIO_90    ((FLOAT)80.0)
#define IPCC_DISPLAY_RATIO_100   ((FLOAT)100.0)


/* IPCC internal used return value */
#define IPCCR_OK                              ((INT32) 0)    /**<OK*/
#define IPCCR_INV_ARG                         ((INT32) -1)   /**<Invalide argument*/
#define IPCCR_INSUFFICIENT_MEMORY             ((INT32) -2)   /**<Incufficient memory*/
#define IPCCR_OUT_OF_HANDLE                   ((INT32) -3)   /**<Out of handle*/
#define IPCCR_WGL_FAIL                        ((INT32) -4)   /**<IPCC widget fail */
#define IPCCR_INSUFFICIENT_SIZE               ((INT32) -5)   /**<Incufficient size */
#define IPCCR_NOT_FOUND                       ((INT32) -6)   /**<Not found */
#define IPCCR_NOT_SUPPORT                     ((INT32) -7)   /**<Not support */
#define IPCCR_INTERNAL_ERROR                  ((INT32) -8)   /**<Internal error */




/* Default colors                 A      R       G       B   */
#define CC_COLOR_BLACK         { 0xFF, {0x00}, {0x00}, {0x00} }
#define CC_COLOR_BLACK_TLC     { 0x7F, {0x00}, {0x00}, {0x00} }
#define CC_COLOR_WHITE         { 0xFF, {0xFF}, {0xFF}, {0xFF} }
#define CC_COLOR_WHITE_TLC     { 0x7F, {0xFF}, {0xFF}, {0xFF} }
#define CC_COLOR_GREEN         { 0xFF, {0x00}, {0xFF}, {0x00} }
#define CC_COLOR_GREEN_TLC     { 0x7F, {0x00}, {0xFF}, {0x00} }
#define CC_COLOR_BLUE          { 0xFF, {0x00}, {0x00}, {0xFF} }
#define CC_COLOR_BLUE_TLC      { 0x7F, {0x00}, {0x00}, {0xFF} }
#define CC_COLOR_RED           { 0xFF, {0xFF}, {0x00}, {0x00} }
#define CC_COLOR_RED_TLC       { 0x7F, {0xFF}, {0x00}, {0x00} }
#define CC_COLOR_CYAN          { 0xFF, {0x00}, {0xFF}, {0xFF} }
#define CC_COLOR_CYAN_TLC      { 0x7F, {0x00}, {0xFF}, {0xFF} }
#define CC_COLOR_YELLOW        { 0xFF, {0xFF}, {0xFF}, {0x00} }
#define CC_COLOR_YELLOW_TLC    { 0x7F, {0xFF}, {0xFF}, {0x00} }
#define CC_COLOR_MAGENTA       { 0xFF, {0xFF}, {0x00}, {0xFF} }
#define CC_COLOR_MAGENTA_TLC   { 0x7F, {0xFF}, {0x00}, {0xFF} }
#define CC_COLOR_GRAY          { 0xFF, {0x20}, {0x20}, {0x20} }
#define CC_COLOR_GRAY_TLC      { 0x7F, {0x20}, {0x20}, {0x20} }
#define CC_COLOR_TRANSPARENT   { 0x00, {0x00}, {0x00}, {0x00} }


/* Text Tag */
#define CAP_CMD_TEXT_TAG_DIALOG             ((UINT8) 0 )
#define CAP_CMD_TEXT_TAG_SRC_SPEAKER_ID     ((UINT8) 1 )
#define CAP_CMD_TEXT_TAG_ELEC_REPROD_VOICE  ((UINT8) 2 )
#define CAP_CMD_TEXT_TAG_DIALOG_NOT_PRIM    ((UINT8) 3 )
#define CAP_CMD_TEXT_TAG_VOICEOVER          ((UINT8) 4 )
#define CAP_CMD_TEXT_TAG_AUDIBLE_TRANSLATE  ((UINT8) 5 )
#define CAP_CMD_TEXT_TAG_SUBTITLE_TRANSLATE ((UINT8) 6 )
#define CAP_CMD_TEXT_TAG_VOICE_QA_DESC      ((UINT8) 7 )
#define CAP_CMD_TEXT_TAG_SONG_LYRICS        ((UINT8) 8 )
#define CAP_CMD_TEXT_TAG_SND_EFF_DESC       ((UINT8) 9 )
#define CAP_CMD_TEXT_TAG_MUSICAL_SCORE_DESC ((UINT8) 10 )
#define CAP_CMD_TEXT_TAG_EXPLETIVE          ((UINT8) 11 )
#define CAP_CMD_TEXT_TAG_NOT_TO_BE_DISP     ((UINT8) 15 )


/*-----------------------------------------------------------------------------
                    Struct
-----------------------------------------------------------------------------*/
typedef struct _SBTL_IPCC_REGION_ATTR
{
    CHAR                            ps_region_id[IPCC_TTS_ORIGIN_LENGTH];
    CHAR                            ps_style_id[IPCC_TTS_ORIGIN_LENGTH];
    UINT32                          ui4_font_size;
    TTML_LENGTH_UNIT_TYPE           e_font_size_unit;
    FE_FNT_SIZE                     e_font_size;
    CHAR                            ps_win_bg_color[IPCC_TTS_ORIGIN_LENGTH/2];
    CHAR                            ps_win_opacity[IPCC_TTS_ORIGIN_LENGTH/2];
    CHAR                            ps_pen_fg_color[IPCC_TTS_ORIGIN_LENGTH/2];
    CHAR                            ps_pen_bg_color[IPCC_TTS_ORIGIN_LENGTH/2];
    CHAR                            ps_pen_edge_color[IPCC_TTS_ORIGIN_LENGTH/2];
    CHAR                            ps_pen_edge_type[IPCC_TTS_ORIGIN_LENGTH/2];
    UINT8                           ui1_pen_edge_type;
    SBTL_TTML_ATTR_FONT_FAMILY      e_font_family;
    CHAR                            ps_font_name[IPCC_TTS_ORIGIN_LENGTH];
    SBTL_TTML_ATTR_FONT_STYLE       e_font_style;
    SBTL_TTML_ATTR_FONT_WEIGHT      e_font_weight;
    SBTL_TTML_ATTR_TEXT_DECORATION  e_text_decoration;
    SBTL_TTML_ATTR_TEXT_ALIGIN      e_text_align;
    WGL_CC_WIN_ANCHOR_T             t_win_anchor;
    WGL_CC_WIN_SIZE_T               t_win_size;
    TTML_LENGTH_UNIT_TYPE           e_win_size_unit;
    WGL_CC_TEXT_PARAG_LOCATION_T    t_pen_loc;
    CHAR                            ps_win_word_wrap[IPCC_TTS_ORIGIN_LENGTH/2];
    BOOL                            b_whitespace_preserve;
    CHAR                            ps_text_tag[IPCC_TTS_ORIGIN_LENGTH];
}SBTL_IPCC_REGION_ATTR;


typedef struct _SBTL_IPCC_WGT_WND_ATTR
{
    CHAR                            ps_region_id[IPCC_TTS_ORIGIN_LENGTH];
    CHAR                            ps_style_id[IPCC_TTS_ORIGIN_LENGTH];

    WGL_CC_PEN_COLOR_T              t_pen_color;
    WGL_CC_PEN_FONT_T               t_pen_font;

    WGL_CC_WIN_ATTR_T               t_win_attr;
    WGL_CC_WIN_ANCHOR_T             t_win_anchor;
    WGL_CC_WIN_SIZE_T               t_win_size;   

    UINT8                           aui1_text_tag;
       
}SBTL_IPCC_WGT_WND_ATTR;


typedef struct _SBTL_IPCC_WGT_T
{
    HANDLE_T                       h_app;
    HANDLE_T                       h_canvas;
    HANDLE_T                       h_frame;
    HANDLE_T                       ah_cc_wnd[IPCC_MAX_WGT_NUM];
    WGL_CC_INFO_T                  at_cc_info[IPCC_MAX_WGT_NUM];
    UINT8                          aui1_z_order[IPCC_MAX_WGT_NUM];
    UINT8                          aui1_hdend_z_order[IPCC_MAX_WGT_NUM];
    UINT8                          aui1_text_tag[IPCC_MAX_WGT_NUM];
    UINT8                          ui1_cc_wnd_alive_mask;
    UINT8                          ui1_curr_wnd_num;
    UINT8                          ui1_max_anchor_v;
    UINT8                          ui1_max_anchor_h;
    UINT32                         ui4_canvas_w;
    UINT32                         ui4_canvas_h;
} SBTL_IPCC_WGT_T;


/* Command - set pen color ***************************************************/
typedef struct _CAP_CMD_SET_PEN_LOC_T
{
    UINT8    ui1_b1;
    
#define CAP_CMD_SPL_RSRV1( pt_cmd )        ((UINT8)( pt_cmd->ui1_b1 & 0xF0 ) >> 4)
#define CAP_CMD_SPL_ROW( pt_cmd )          ((UINT8)( pt_cmd->ui1_b1 & 0x0F ))

    UINT8    ui1_b2;
    
#define CAP_CMD_SPL_RSRV2( pt_cmd )        ((UINT8)( pt_cmd->ui1_b2 & 0xC0 ) >> 6)
#define CAP_CMD_SPL_COLUMN( pt_cmd )       ((UINT8)( pt_cmd->ui1_b2 & 0x3F ))

} CAP_CMD_SET_PEN_LOC_T;



#endif /* __KERNEL__ */
#endif/* _SBTL_IPCC_COMMON_H_ */
/*----------------------------------------------------------------------------*/
/*! @} */   /* end of  groupMW_STRM_MNGR_MM_SBTL_HDLR*/
/*----------------------------------------------------------------------------*/

