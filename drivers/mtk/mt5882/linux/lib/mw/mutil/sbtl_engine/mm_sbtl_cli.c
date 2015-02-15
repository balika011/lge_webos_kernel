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
 * $RCSfile: mm_sbtl_cli.c,v $
 * $Revision:
 * $Date:
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_ATSC/4 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 37ebbb2d76fad2e8a90655e5f2bf2c3a $
 *
 * Description:
 *         This file implements CLI APIs for MTP.
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#ifndef __KERNEL__

#ifdef _WIN32_
#include "x_os.h"
#else
#include "x_os.h"
#endif
#include "x_common.h"
#include "x_uc_str.h"
#include "x_cli.h"
#include "x_dbg.h"

#include "u_wgl_sbtl.h"
#include "x_wgl.h"
#include "u_wgl_tools.h"
#include "u_wgl_obj_type.h"
#include "x_gl.h"

#include "x_lnk_list.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_divx/sbtl_divx_page_inst_mngr.h"
#include "u_mm_sbtl_engine.h"
#include "x_mm_sbtl_engine.h"

#include "x_mm_common.h"
#include "mutil/mm_util_input.h"

#include "u_fm.h"
#include "x_fm.h"

#include "x_handle.h"
#include "u_sys_name.h"
#ifdef MW_IPCC_SUPPORT
#include "xml/xml_debug.h"
#include "xml/xml.h" 
#include "sbtl_engine/sbtl_xml_parser.h"

#include <stdio.h>
#endif
#include "u_img.h"
#include "u_wgl_image.h"
#include "x_wgl_image.h"
#include "x_img.h"
#include "x_gl.h"
#ifndef CLI_LVL_ALL
#define CLI_LVL_ALL
#endif

#ifdef CLI_LVL_ALL

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#ifdef MW_IPCC_SUPPORT

#define XML_PARSE_TEST_FILE         "/mnt/usb/sda1/xml_test.bin"
#define XML_BUFF_MAX_LEN            30000
#define XML_VALUE_MAX_LEN           2048
#define UPDATER_XML_VALUE(pt_xml)   ((pt_xml != NULL) ? XML_Value(pt_xml) : "")


typedef enum _SBTL_MMS_ELEM_T
{
    SBTL_XML_MMS_ELM_TT,
    SBTL_XML_MMS_ELM_HEAD,
    SBTL_XML_MMS_ELM_METADATA,
    SBTL_XML_MMS_ELM_TITLE,
    SBTL_XML_MMS_ELM_DESC,
    SBTL_XML_MMS_ELM_COPYRIGHT,
    SBTL_XML_MMS_ELM_STYLING,
    SBTL_XML_MMS_ELM_STYLE,
    SBTL_XML_MMS_ELM_LAYOUT,
    SBTL_XML_MMS_ELM_REGION,
    SBTL_XML_MMS_ELM_BODY,
    SBTL_XML_MMS_ELM_DIV,
    SBTL_XML_MMS_ELM_PARA,
    SBTL_XML_MMS_ELM_SPAN
}SBTL_MMS_ELEM_T;

typedef enum _SBTL_MMS_ATTR_T
{
    SBTL_XML_MMS_ATTR_ID,
    SBTL_XML_MMS_ATTR_FONTFAMILY,
    SBTL_XML_MMS_ATTR_FONTSIZE,
    SBTL_XML_MMS_ATTR_TEXTALIGN,
    SBTL_XML_MMS_ATTR_ORIGIN,
    SBTL_XML_MMS_ATTR_EXTENT,
    SBTL_XML_MMS_ATTR_BGCOLOR,
    SBTL_XML_MMS_ATTR_DISPALIGN,

    SBTL_XML_MMS_ATTR_STYLE,
    SBTL_XML_MMS_ATTR_COLOR,
    SBTL_XML_MMS_ATTR_ZINDEX,

    SBTL_XML_MMS_ATTR_REGION,
    SBTL_XML_MMS_ATTR_BEGIN,
    SBTL_XML_MMS_ATTR_END,
    
}SBTL_MMS_ATTR_T;

SBTL_XML_HASH_INFO_T t_hash_attr[] =
{
    {"id",             (INT32)SBTL_XML_MMS_ATTR_ID},
    {"fontfamily",     (INT32)SBTL_XML_MMS_ATTR_FONTFAMILY},
    {"fontsize",       (INT32)SBTL_XML_MMS_ATTR_FONTSIZE},
    {"textalign",      (INT32)SBTL_XML_MMS_ATTR_TEXTALIGN},
    {"origin",         (INT32)SBTL_XML_MMS_ATTR_ORIGIN},
    {"extent",         (INT32)SBTL_XML_MMS_ATTR_EXTENT},
    {"backgroundcolor",(INT32)SBTL_XML_MMS_ATTR_BGCOLOR},
    {"dispalign",      (INT32)SBTL_XML_MMS_ATTR_DISPALIGN},
    {"style",          (INT32)SBTL_XML_MMS_ATTR_STYLE},
    {"color",          (INT32)SBTL_XML_MMS_ATTR_COLOR},
    {"zindex",         (INT32)SBTL_XML_MMS_ATTR_ZINDEX},
    {"region",         (INT32)SBTL_XML_MMS_ATTR_REGION},
    {"begin",          (INT32)SBTL_XML_MMS_ATTR_BEGIN},
    {"end",            (INT32)SBTL_XML_MMS_ATTR_END}, 
};

SBTL_XML_HASH_INFO_T t_hash_ele[] =
{
    {"tt",         (INT32)SBTL_XML_MMS_ELM_TT},
    {"head",       (INT32)SBTL_XML_MMS_ELM_HEAD},
    {"metadata",   (INT32)SBTL_XML_MMS_ELM_METADATA},
    {"title",      (INT32)SBTL_XML_MMS_ELM_TITLE},
    {"desc",       (INT32)SBTL_XML_MMS_ELM_DESC},
    {"copyright",  (INT32)SBTL_XML_MMS_ELM_COPYRIGHT},
    {"styling",    (INT32)SBTL_XML_MMS_ELM_STYLING},
    {"style",      (INT32)SBTL_XML_MMS_ELM_STYLE},
    {"layout",     (INT32)SBTL_XML_MMS_ELM_LAYOUT},
    {"region",     (INT32)SBTL_XML_MMS_ELM_REGION},
    {"div",        (INT32)SBTL_XML_MMS_ELM_DIV},
    {"body",       (INT32)SBTL_XML_MMS_ELM_BODY},
    {"p",          (INT32)SBTL_XML_MMS_ELM_PARA},
    {"span",       (INT32)SBTL_XML_MMS_ELM_SPAN}, 
};
#endif
/*-----------------------------------------------------------------------------
                    functions declarations
-----------------------------------------------------------------------------*/
static INT32 _sbtl_cli_get_dbg_level(INT32              i4_argc, 
                                     const CHAR**       pps_argv);
                                    
static INT32 _sbtl_cli_set_dbg_level(INT32              i4_argc, 
                                     const CHAR**       pps_argv);  
#ifdef MW_IPCC_SUPPORT
static INT32 _sbtl_cli_test_xml_parse(INT32              i4_argc, 
                                     const CHAR**             pps_argv);
static INT32 _sbtl_cli_test_xml_on(INT32              i4_argc, 
                                     const CHAR**             pps_argv);

#endif
extern SBTL_ENGINE_T* mm_sbtl_get_using_engine(VOID);

static INT32 _sbtl_cli_ctrl_engine(INT32              i4_argc, 
                                     const CHAR**        pps_argv);
static INT32 _sbtl_cli_get_engine(INT32              i4_argc, 
                                     const CHAR**        pps_argv);
static INT32 _sbtl_cli_set_engine(INT32              i4_argc, 
                                     const CHAR**        pps_argv);

static INT32 _sbtl_cli_test_png_perform(INT32              i4_argc, 
                                     const CHAR**       pps_argv);

static INT32 _sbtl_cli_init_res(VOID);
static VOID _sbtl_cli_deinit_res(VOID);

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

typedef struct
{
	UINT32* pui4_idx_pts;
	UINT32* pui4_idx_dur;
	UINT32* pui4_idx_len;
	UINT32  ui4_cnt;
	INT32   i4_idx_stat;/*build idx ok:1 or fail:-1*/
}SBTL_CLI_SIMU_IDX_T;

#define SBTL_CLI_MAX_IDX_CNT (500)


static HANDLE_T             h_screen = NULL_HANDLE;
static HANDLE_T             h_plane = NULL_HANDLE;
static HANDLE_T             h_canvas = NULL_HANDLE;
static HANDLE_T             h_subtitle_file = NULL_HANDLE;
static HANDLE_T             h_index_file = NULL_HANDLE;
static MEDIA_FEEDER_T       t_feeder;
static MEDIA_FEEDER_T       t_feeder_idx;
static BOOL                 b_network   = FALSE;
static BOOL                 b_simulator = FALSE;
static SBTL_CLI_SIMU_IDX_T  t_simu_idx = {0};

#ifdef MW_IPCC_SUPPORT
extern BOOL g_b_debug_on;
#endif

typedef enum
{
	MM_SBTL_CLI_SET_NONE = 0,
	MM_SBTL_CLI_SET_FONT,
	MM_SBTL_CLI_SET_COLOR_FG,
	MM_SBTL_CLI_SET_COLOR_BG,
	MM_SBTL_CLI_SET_COLOR_EDGE,
	MM_SBTL_CLI_SET_ENCODE,
	MM_SBTL_CLI_SET_SHOW,
	MM_SBTL_CLI_SET_COLOR_HF,
	MM_SBTL_CLI_SET_COLOR_HB,
	MM_SBTL_CLI_SET_LINEGAP,
	MM_SBTL_CLI_SET_TIME_OFFSET,
	MM_SBTL_CLI_SET_PAGE_INFO,
	MM_SBTL_CLI_SET_BDR_TYPE,
	MM_SBTL_CLI_SET_RECT,
    MM_SBTL_CLI_SET_POS,
    MM_SBTL_CLI_SET_BUFFER_FILE,

	MM_SBTL_CLI_GET_NONE,
	MM_SBTL_CLI_GET_FONT,
	MM_SBTL_CLI_GET_COLOR_FG,
	MM_SBTL_CLI_GET_COLOR_BG,
	MM_SBTL_CLI_GET_COLOR_EDGE,
	MM_SBTL_CLI_GET_ENCODE,
	MM_SBTL_CLI_GET_SHOW,

    MM_SBTL_CLI_CTRL_NONE,
	MM_SBTL_CLI_CTRL_OPEN,
	MM_SBTL_CLI_CTRL_PLAY,
	MM_SBTL_CLI_CTRL_STOP,
	MM_SBTL_CLI_CTRL_CLOSE
}MM_SBTL_CLI_CMD;

typedef struct
{
	MM_SBTL_CLI_CMD     e_cmd;
	CHAR*               s_name;
	CHAR*               s_desc;
}MM_SBTL_CLI_CMD_TBL;
  

static MM_SBTL_CLI_CMD_TBL t_set_tbl[] =
{
	{MM_SBTL_CLI_SET_FONT,       "F",   "font type [BMSC] size:[d] style:[d] name:[s]"},
	{MM_SBTL_CLI_SET_COLOR_FG,   "TF",  "text fgcolor   argb[dddd]"},
	{MM_SBTL_CLI_SET_COLOR_BG,   "TB",  "text fgcolor   argb[dddd]"},
	{MM_SBTL_CLI_SET_COLOR_EDGE, "TE",  "text edgecolor argb[dddd]"},
	{MM_SBTL_CLI_SET_COLOR_HF,   "HF",  "highlight fgcolor argb[dddd]"},
	{MM_SBTL_CLI_SET_COLOR_HB,   "HB",  "highlight bgcolor argb[dddd]"},
	{MM_SBTL_CLI_SET_ENCODE,     "E",   "encoding [d]"},
	{MM_SBTL_CLI_SET_SHOW,       "S",   "Show or hide[d]"},
	{MM_SBTL_CLI_SET_LINEGAP,    "LG",  "Line gap [d]"},
	{MM_SBTL_CLI_SET_TIME_OFFSET,"OFST","Timeoffset [d]"},
	{MM_SBTL_CLI_SET_PAGE_INFO,  "PGIF","Pageinfo [SF] lines[d] intvl[d] hlt[d] gap[d]"},
	{MM_SBTL_CLI_SET_BDR_TYPE,   "BDR", "width[d] height[d]"},
    {MM_SBTL_CLI_SET_RECT,       "RCT", "x[d] y[d] w[d] h[d]"},
    {MM_SBTL_CLI_SET_POS,        "POS", "x[d] y[d]"},
    {MM_SBTL_CLI_SET_BUFFER_FILE,"FILE", "name:[s] start:[d] end:[d]"},
};

static MM_SBTL_CLI_CMD_TBL t_get_tbl[] =
{
	{MM_SBTL_CLI_GET_FONT,       "F",  "get font info"},
	{MM_SBTL_CLI_GET_COLOR_FG,   "TF", "get text fg color"},
	{MM_SBTL_CLI_GET_COLOR_BG,   "TB", "get text bg color"},
	{MM_SBTL_CLI_GET_COLOR_EDGE, "TE", "get text edge color"},
	{MM_SBTL_CLI_GET_ENCODE,     "E",  "get encoding"},
	{MM_SBTL_CLI_GET_SHOW,       "S",  "get show"}
};

static MM_SBTL_CLI_CMD_TBL t_ctrl_tbl[] =
{
	{MM_SBTL_CLI_CTRL_OPEN,      "O",  "open sbtl: sbtl:[s] idx:[s] as:[d?] sel:[d]"},
	{MM_SBTL_CLI_CTRL_PLAY,      "P",  "play sbtl"},
	{MM_SBTL_CLI_CTRL_STOP,      "S",  "stop sbtl"},
	{MM_SBTL_CLI_CTRL_CLOSE,     "C",  "close sbtl"}
};



/* minfo command table */
static CLI_EXEC_T at_mm_sbtl_cmd_tbl[] =
{
    {
        CLI_GET_DBG_LVL_STR,   
        NULL,       
        _sbtl_cli_get_dbg_level,     
        NULL,       
        CLI_GET_DBG_LVL_HELP_STR,                  
        CLI_SUPERVISOR
    },
    {
        CLI_SET_DBG_LVL_STR,   
        NULL,       
        _sbtl_cli_set_dbg_level,     
        NULL,       
        CLI_SET_DBG_LVL_HELP_STR,                  
        CLI_SUPERVISOR
    },
    {
        "get",   
        NULL,       
        _sbtl_cli_get_engine,     
        NULL,       
        "get sbtl attrs",                  
        CLI_SUPERVISOR
    },
    {
        "set",   
        NULL,       
        _sbtl_cli_set_engine,     
        NULL,       
        "set sbtl attrs",                  
        CLI_SUPERVISOR
    },
    {
        "ctrl",   
        NULL,       
        _sbtl_cli_ctrl_engine,     
        NULL,       
        "control sbtl",                  
        CLI_SUPERVISOR
    },
#ifdef MW_IPCC_SUPPORT
    {
        "xton",   
        NULL,       
        _sbtl_cli_test_xml_on,     
        NULL,       
        "Test XML parse on",                  
        CLI_SUPERVISOR
    },
    {
        "xmltest",   
        NULL,       
        _sbtl_cli_test_xml_parse,     
        NULL,       
        "Test XML parse capbility",                  
        CLI_SUPERVISOR
    },
#endif
    {
        "pngper",   
        NULL,       
        _sbtl_cli_test_png_perform,     
        NULL,       
        "Test PNG load performance:dynamic sleeptime testcount loadimg",                  
        CLI_GUEST
    },
    END_OF_CLI_CMD_TBL
};

/* minfo root command table */
static CLI_EXEC_T at_mm_sbtl_root_cmd_tbl[] =
{
    {
        "mmsbtl",     
        NULL,        
        NULL,       
        at_mm_sbtl_cmd_tbl,      
        "multimedia subtitle commands",   
        CLI_SUPERVISOR
    },
    END_OF_CLI_CMD_TBL
};

/*-----------------------------------------------------------------------------
                    Internal functions implementations
 ----------------------------------------------------------------------------*/ 
/*-----------------------------------------------------------------------------
 * Name: _mtp_cli_get_dbg_level
 *
 * Description: This API gets the current setting of mtp debug level.
 *
 * Inputs:  i4_argc         Contains the argument count.
 *          pps_argv        Contains the arguments.
 *
 * Outputs: -
 *
 * Returns: CLIR_OK                 Routine successful.
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_cli_get_dbg_level(INT32              i4_argc, 
                                     const CHAR**       pps_argv)
{
    INT32       i4_return;

    i4_return = x_cli_show_dbg_level(mm_sbtl_get_dbg_level());

    return i4_return;
}


/*-----------------------------------------------------------------------------
 * Name: _mtp_cli_set_dbg_level
 *
 * Description: This API sets the debug level of mtp.
 *
 * Inputs:  i4_argc         Contains the argument count.
 *          pps_argv        Contains the arguments.
 *
 * Outputs: -
 *
 * Returns: CLIR_OK                 Routine successful.
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_cli_set_dbg_level(INT32              i4_argc, 
                                     const CHAR**       pps_argv)
{
    INT32       i4_return;
    UINT16      ui2_dbg_level;

    i4_return = x_cli_parse_dbg_level(i4_argc, pps_argv, &ui2_dbg_level);

    if (i4_return == CLIR_OK)
    {
        mm_sbtl_set_dbg_level(ui2_dbg_level);
        i4_return = CLIR_OK;
    }
    else
    {
        i4_return = CLIR_CMD_EXEC_ERROR;
    }

    x_cli_show_dbg_level(mm_sbtl_get_dbg_level());
    DBG_INFO(("[mm sbtl] set debug level %s\n\r", (i4_return == CLIR_OK) ? "successful" : "failed"));
    return i4_return;
}

static INT32 _sbtl_cli_init_res(VOID)
{
     INT32 i4_ret;
     if (NULL_HANDLE == h_plane)
     {
         /*Open the plane */
         i4_ret = x_gl_screen_open(SN_PRES_MAIN_DISPLAY,                                   
                                    NULL,                                   
                                    NULL,                                   
                                    & h_screen); 
          if (GL_FAILED(i4_ret))
          {
             DBG_LOG_PRINT(("Can't open scr\n"));
             x_handle_free(h_screen);
             return MMSBTLR_WGL_FAIL;
          }
         
         i4_ret = x_gl_plane_open (h_screen,
                                   0,
                                   &h_plane);
         if (GL_FAILED(i4_ret))
         {
             DBG_LOG_PRINT(("Can't open plane\n"));
             x_handle_free(h_screen);
             return MMSBTLR_WGL_FAIL;
         }
         
         /*Link the plane*/
         i4_ret = x_gl_plane_link (h_plane, h_screen, TRUE);
         if (GL_FAILED(i4_ret))
         {
             DBG_LOG_PRINT(("Can't link plane\n"));
             x_handle_free(h_screen);
             x_handle_free(h_plane);
             return MMSBTLR_WGL_FAIL;
         }
     }

     if (NULL_HANDLE == h_canvas)
     {
         x_wgl_get_canvas(h_plane, &h_canvas);
     }
     return MMSBTLR_OK;
}


static INT32 _sbtl_cli_free_idx(VOID)
{
    if (t_simu_idx.pui4_idx_pts)
    {
        x_mem_free(t_simu_idx.pui4_idx_pts);
    }
    if (t_simu_idx.pui4_idx_dur)
    {
        x_mem_free(t_simu_idx.pui4_idx_dur);
    }
    if (t_simu_idx.pui4_idx_len)
    {
        x_mem_free(t_simu_idx.pui4_idx_len);
    }
    x_memset(&t_simu_idx, 0, sizeof(SBTL_CLI_SIMU_IDX_T));
    return MMSBTLR_OK;
}


static VOID _sbtl_cli_deinit_res(VOID)
{
    if (h_screen != NULL_HANDLE)
    {
        x_handle_free(h_screen);
        h_screen = NULL_HANDLE;
    }
    
    if (NULL_HANDLE != h_plane)
    {
        x_handle_free(h_plane);
        h_plane = NULL_HANDLE;
    }
    
    h_canvas = NULL_HANDLE;

    if (h_subtitle_file != NULL_HANDLE)
    {
        sbtl_util_close_file(h_subtitle_file, b_network);
        h_subtitle_file = NULL_HANDLE;
    }

    if (h_index_file != NULL_HANDLE)
    {
        sbtl_util_close_file(h_index_file, b_network);
        h_index_file = NULL_HANDLE;
    }
    _sbtl_cli_free_idx();

    if (b_simulator)
    {
        sbtl_util_set_simu_mode(FALSE);
        sbtl_util_set_simu_cb(NULL);
        b_simulator = FALSE;
    }
    
} 


static INT32 _sbtl_cli_build_simu_idx(SBTL_ENGINE_T*  pt_engine)
{
    INT32    i4_ret   = MMSBTLR_OK;
    UINT32   ui4_idx  = 0;
    UINT32   ui4_pos  = 0;
    UINT32   ui4_data = 0;
    
    if (t_simu_idx.i4_idx_stat == 0)
    {
        do
        {
            t_simu_idx.pui4_idx_pts = (UINT32*)x_mem_alloc(sizeof(UINT32)*SBTL_CLI_MAX_IDX_CNT);
            if (t_simu_idx.pui4_idx_pts == NULL)
            {
                i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
                break;
            }
            
            t_simu_idx.pui4_idx_len = (UINT32*)x_mem_alloc(sizeof(UINT32)*SBTL_CLI_MAX_IDX_CNT);
            if (t_simu_idx.pui4_idx_len == NULL)
            {
                i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
                break;
            }

            t_simu_idx.pui4_idx_dur = (UINT32*)x_mem_alloc(sizeof(UINT32)*SBTL_CLI_MAX_IDX_CNT);
            if (t_simu_idx.pui4_idx_dur == NULL)
            {
                i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
                break;
            }
            
            sbtl_feeder_init(pt_engine, MM_SBTL_FEEDER_SUBTITLE);

            while (1)
            {
                if (ui4_idx >= SBTL_CLI_MAX_IDX_CNT)
                {
                    break;
                }
                i4_ret = sbtl_feeder_inp4byte(pt_engine, &ui4_data, MM_SBTL_FEEDER_SUBTITLE);
                if (i4_ret != MMSBTLR_OK) break;
                t_simu_idx.pui4_idx_pts[ui4_idx] = ui4_data;

                i4_ret = sbtl_feeder_inp4byte(pt_engine, &ui4_data, MM_SBTL_FEEDER_SUBTITLE);
                if (i4_ret != MMSBTLR_OK) break;
                t_simu_idx.pui4_idx_dur[ui4_idx] = ui4_data;
                
                i4_ret = sbtl_feeder_inp4byte(pt_engine, &ui4_data, MM_SBTL_FEEDER_SUBTITLE);
                if (i4_ret != MMSBTLR_OK) break;
                t_simu_idx.pui4_idx_len[ui4_idx] = ui4_data;
                
                ui4_idx++;
                t_simu_idx.ui4_cnt = ui4_idx;

                ui4_pos += sizeof(UINT32)*3 + ui4_data;
                 
                i4_ret = sbtl_feeder_setpos(pt_engine,
                                   (INT64)ui4_pos,
                                   MEDIA_SEEK_BGN,
                                   MM_SBTL_FEEDER_SUBTITLE);
                if (i4_ret != MMSBTLR_OK)
                {
                    break;
                }
                
            }   
        }while(0);
        
        if (i4_ret != MMSBTLR_OK && i4_ret != MMSBTLR_EOT)
        {
            _sbtl_cli_free_idx();
            t_simu_idx.i4_idx_stat = -1;
            return i4_ret;
        }
        t_simu_idx.i4_idx_stat = 1;
    }
    return MMSBTLR_OK;
}

/*
Simu data file content format:
first     pts:[int] duration:[int]  content length:[int]  real content
second pts:[int] duration:[int]  content length:[int]  real content
*/

static INT32 _sbtl_cli_simu_data_func(
                                       SBTL_SIMU_DATA_ACT e_act,
                                       PTS_T              t_cur,
                                       MKV_PACK_T*        pt_pack,
                                       VOID*              pv_data)
{
    SBTL_ENGINE_T*  pt_engine = mm_sbtl_get_using_engine();
    INT32           i4_ret    = MMSBTLR_OK;
    UINT32          ui4_idx   = 0;
    
    if (h_subtitle_file == NULL_HANDLE || pt_engine == NULL)
    {
        return MMSBTLR_NOT_INIT;
    }
    switch (e_act)
    {
        case SBTL_SIMU_DATA_ASK:
        {
            i4_ret = _sbtl_cli_build_simu_idx(pt_engine);
            if (i4_ret != MMSBTLR_OK)
            {
                return i4_ret;
            }
            if (pt_pack == NULL || pv_data == NULL)
            {
                return MMSBTLR_INV_ARG;
            }
            if (t_simu_idx.i4_idx_stat == 1)//idx build done
            {
                if (t_simu_idx.pui4_idx_pts == NULL || 
                    t_simu_idx.pui4_idx_dur == NULL || 
                    t_simu_idx.pui4_idx_len == NULL)
                {
                    return MMSBTLR_NOT_INIT;
                }
                *(BOOL*)pv_data = FALSE;
                for (ui4_idx = 0; ui4_idx < t_simu_idx.ui4_cnt; ui4_idx++)
                {
                    if (t_cur >= (PTS_T)t_simu_idx.pui4_idx_pts[ui4_idx] && 
                        t_cur <  (PTS_T)t_simu_idx.pui4_idx_pts[ui4_idx]+t_simu_idx.pui4_idx_dur[ui4_idx])
                    {
                        *(BOOL*)pv_data = TRUE;
                        pt_pack->ui4_data_1   = 0x111;
                        pt_pack->ui4_data_2   = t_simu_idx.pui4_idx_len[ui4_idx];
                        pt_pack->ui4_data_3   = 0x100000+ui4_idx;
                        pt_pack->ui4_pts      = t_simu_idx.pui4_idx_pts[ui4_idx];
                        pt_pack->ui4_duration = t_simu_idx.pui4_idx_dur[ui4_idx];
                        break;
                        
                    }
                }

                return MMSBTLR_OK;
            }
            return MMSBTLR_NOT_SUPPORT;
        }
        //break;

        case SBTL_SIMU_DATA_CPY:
        {
            UINT32 ui4_cpy_idx; 
            UINT32 ui4_pos = 0;
            UINT32 ui4_readed = 0;
            if (pt_pack == NULL || pv_data == NULL)
            {
                return MMSBTLR_INV_ARG;
            }
            ui4_cpy_idx = pt_pack->ui4_data_3-0x100000;
            
            if (ui4_cpy_idx >= t_simu_idx.ui4_cnt ||
                t_simu_idx.pui4_idx_pts == NULL || 
                t_simu_idx.pui4_idx_dur == NULL || 
                t_simu_idx.pui4_idx_len == NULL ||
                t_simu_idx.pui4_idx_len[ui4_cpy_idx] < pt_pack->ui4_data_2)
            {
                return MMSBTLR_NOT_INIT;
            }
            
            for (ui4_idx = 0; ui4_idx < ui4_cpy_idx; ui4_idx++)
            {
                ui4_pos += 3*sizeof(UINT32)+ t_simu_idx.pui4_idx_len[ui4_idx];
            }
            
            i4_ret = sbtl_feeder_setpos(pt_engine,
                                   (INT64)ui4_pos+3*sizeof(UINT32),
                                   MEDIA_SEEK_BGN,
                                   MM_SBTL_FEEDER_SUBTITLE);
            if (i4_ret != MMSBTLR_OK) break;

            i4_ret = sbtl_feeder_inpnbyte(pt_engine,
                                        pt_pack->ui4_data_2,
                                        (UINT8*)pv_data,
                                        &ui4_readed,
                                        MM_SBTL_FEEDER_SUBTITLE);
            if (i4_ret != MMSBTLR_OK) break;
            
            //return MMSBTLR_OK;
        }
        break;
            
        default:
        break;
    }
    return i4_ret;
}
/*
sbtl open command:
sbtl: sbtl path, or simulation internal subtitle path
idx:  idx file path
as:  as which subtitle
sel:  one file has multi subtitle select which
sim: simulation mode can open subtitle independent
*/

static INT32 _sbtl_cli_parse_open_cmds(
            INT32              i4_argc, 
            const CHAR**       pps_argv,
            SBTL_CFG_T*        pt_cfg)
{
    if (i4_argc > 0 && 
        pt_cfg != NULL &&
        pps_argv != NULL)
    {
        INT32 i4_i = 0;
        INT32 i4_ret;
        BOOL  b_net = FALSE;
        static GL_COLOR_T           t_bg_en = MM_SBTL_DFT_BG_EN_COLOR;
        static GL_COLOR_T           t_bg_dis = MM_SBTL_DFT_BG_DIS_COLOR;
        static GL_COLOR_T           t_bg_hlt = MM_SBTL_DFT_BG_HLT_COLOR;
        static GL_COLOR_T           t_fg_en = MM_SBTL_DFT_FG_EN_COLOR;
        static GL_COLOR_T           t_fg_dis = MM_SBTL_DFT_FG_DIS_COLOR;
        static GL_COLOR_T           t_fg_hlt = MM_SBTL_DFT_FG_HLT_COLOR;
        static GL_COLOR_T           t_font_bg_en = MM_SBTL_DFT_FONT_BG_EN_COLOR;
        static GL_COLOR_T           t_font_bg_dis = MM_SBTL_DFT_FONT_BG_DIS_COLOR;
        static GL_COLOR_T           t_font_bg_hlt = MM_SBTL_DFT_FONT_BG_HLT_COLOR;
        #ifdef MW_IPCC_SUPPORT
        extern VOID x_sbtl_dync_init(VOID);
        x_sbtl_dync_init();    
        #endif
        pt_cfg->e_sbtl_type = MM_SBTL_TYPE_TEXT;
        while (i4_i < i4_argc)
        {
            if (x_strncmp(pps_argv[i4_i], "as:", 3) == 0)
            {
                pt_cfg->e_sbtl_type = (MM_SBTL_TYPE_T)x_strtoll(
                                        pps_argv[i4_i]+3,
                                        NULL,
                                        10);
            }
            else
            if (x_strncmp(pps_argv[i4_i], "sbtl:", 5) == 0)
            {
                if (x_strncmp(pps_argv[i4_i]+5, "http", 4) == 0)
                {
                    b_net = TRUE;
                }
                i4_ret = sbtl_util_open_file(pps_argv[i4_i]+5, b_net, &h_subtitle_file);
                if (i4_ret != FMR_OK) return i4_ret;
                b_network = b_net;      
            }
            else
            if (x_strncmp(pps_argv[i4_i], "idx:", 4) == 0)
            {
                 i4_ret = sbtl_util_open_file(pps_argv[i4_i]+4, b_net, &h_index_file);
                 if (i4_ret != FMR_OK) return i4_ret;
                 
                 pt_cfg->e_sbtl_type = MM_SBTL_TYPE_VOBSUB;
            }
            else
            if (x_strncmp(pps_argv[i4_i], "sel:", 4) == 0)
            {
                pt_cfg->b_has_strm_id = TRUE;
                pt_cfg->ui4_stream_id = (UINT32)x_strtoll(
                                        pps_argv[i4_i]+4,
                                        NULL,
                                        10);
            }
            else
            if (x_strncmp(pps_argv[i4_i], "sim:", 4) == 0)
            {
                b_simulator = TRUE;
                sbtl_util_set_simu_mode(TRUE);
            }
            else
            if (x_strncmp(pps_argv[i4_i], "start:", 6) == 0)
            {
                pt_cfg->t_first_info.ui4_start_time = 
                    (UINT32)x_strtoll(pps_argv[i4_i]+6,
                                      NULL,
                                      10);
            }
            else
            if (x_strncmp(pps_argv[i4_i], "end:", 4) == 0)
            {
                pt_cfg->t_first_info.ui4_end_time = 
                    (UINT32)x_strtoll(pps_argv[i4_i]+4,
                                      NULL,
                                      10);
            }
            else
            if (x_strncmp(pps_argv[i4_i], "dync:", 5) == 0)
            {
                pt_cfg->e_sbtl_type = MM_SBTL_TYPE_DYNC;
                pt_cfg->t_first_info.e_show_mode = SBTL_SHOW_TYPE_NORMAL;
                pt_cfg->t_first_info.e_file_type  = SBTL_BUFF_FILE_TYPE_PATH;
                pt_cfg->t_first_info.u_info.t_path_info.ps_path = x_strdup(pps_argv[i4_i]+5);
            }
           
            i4_i++;
        }
        if (h_subtitle_file == NULL_HANDLE && pt_cfg->e_sbtl_type != MM_SBTL_TYPE_DYNC)
        {
            return MMSBTLR_NOT_FOUND;
        }
            
        if (b_net)
        {
            #ifdef INET_SUPPORT
            t_feeder.t_feeder_be_fct_tbl.pf_copybytes = sbtl_util_http_fcopybytes_fct;
            t_feeder.t_feeder_be_fct_tbl.pf_input4 = NULL;
            t_feeder.t_feeder_be_fct_tbl.pf_input3 = NULL;
            t_feeder.t_feeder_be_fct_tbl.pf_input2 = NULL;
            t_feeder.t_feeder_be_fct_tbl.pf_input1 = sbtl_util_http_finput1bytes_fct;
            t_feeder.t_feeder_be_fct_tbl.pf_set_pos = sbtl_util_http_fsetpos_fct;
            t_feeder.t_feeder_be_fct_tbl.pf_get_pos = sbtl_util_http_fgetpos_fct;
            #else
            return MMSBTLR_NOT_SUPPORT;
            #endif
        }
        else
        {
            t_feeder.t_feeder_be_fct_tbl.pf_copybytes = sbtl_util_fcopybytes_fct;
            t_feeder.t_feeder_be_fct_tbl.pf_input4 = NULL;
            t_feeder.t_feeder_be_fct_tbl.pf_input3 = NULL;
            t_feeder.t_feeder_be_fct_tbl.pf_input2 = NULL;
            t_feeder.t_feeder_be_fct_tbl.pf_input1 = sbtl_util_finput1bytes_fct;
            t_feeder.t_feeder_be_fct_tbl.pf_set_pos = sbtl_util_fsetpos_fct;
            t_feeder.t_feeder_be_fct_tbl.pf_get_pos = sbtl_util_fgetpos_fct;
        }
        x_memcpy(&t_feeder.t_feeder_le_fct_tbl,
                 &t_feeder.t_feeder_be_fct_tbl, 
                 sizeof(FEEDER_SRC_INPUT_FCT_TBL_T));
        x_memcpy(&t_feeder_idx, &t_feeder, sizeof(MEDIA_FEEDER_T));
        
        t_feeder.h_feeder     = h_subtitle_file;
        t_feeder_idx.h_feeder = h_index_file;
        
        pt_cfg->pt_feeder[MM_SBTL_FEEDER_SUBTITLE] = &t_feeder;
        pt_cfg->pt_feeder[MM_SBTL_FEEDER_INDEX]    = &t_feeder_idx;
        
    
        x_memset(&pt_cfg->t_font_info, 0, sizeof(WGL_FONT_INFO_T));
        x_strcpy(pt_cfg->t_font_info.a_c_font_name, MM_SBTL_DFT_FONT_NAME);
        pt_cfg->t_font_info.e_font_size  = MM_SBTL_DFT_FONT_SIZE;
        pt_cfg->t_font_info.e_font_style = MM_SBTL_DFT_FONT_STYLE;
        pt_cfg->t_font_info.e_font_cmap  = MM_SBTL_DFT_FONT_CMAP;
        
        pt_cfg->t_disp_page.e_page_mode = MM_SBTL_DFT_PAGE_MODE;
        pt_cfg->t_disp_page.ui2_page_line_num = MM_SBTL_DFT_PAGE_LINE;
        pt_cfg->t_disp_page.ui2_hlt_fix_line = MM_SBTL_DFT_PAGE_HLT_FIX_LINE;
        pt_cfg->t_disp_page.i4_line_gap = MM_SBTL_DFT_PAGE_LINE_GAP;
        pt_cfg->t_disp_page.ui2_interval_line = MM_SBTL_DFT_PAGE_INTERVAL_LINE;
        x_mmsbtl_get_def_value(MM_SBTL_DFT_FONT_SIZE_TYPE, 
                              &pt_cfg->t_font_info.ui1_custom_size);
        x_mmsbtl_get_def_value(MM_SBTL_DFT_FONT_ENC_TYPE,  
                              &pt_cfg->e_encoding);
        pt_cfg->b_hide = FALSE;

        pt_cfg->t_bg_color_info.e_type = MM_SBTL_DFT_BG_TYPE;
        x_memcpy (&pt_cfg->t_bg_color_info.u_color_data.t_standard.t_enable, 
                  &t_bg_en, 
                  sizeof(GL_COLOR_T));

        x_memcpy (&pt_cfg->t_bg_color_info.u_color_data.t_standard.t_disable, 
                  &t_bg_dis, 
                  sizeof(GL_COLOR_T));

        x_memcpy (&pt_cfg->t_bg_color_info.u_color_data.t_standard.t_highlight, 
                  &t_bg_hlt, 
                  sizeof(GL_COLOR_T));

        pt_cfg->t_fg_color_info.e_type = MM_SBTL_DFT_FG_TYPE;
        x_memcpy (&pt_cfg->t_fg_color_info.u_color_data.t_standard.t_enable, 
                  &t_fg_en, 
                  sizeof(GL_COLOR_T));

        x_memcpy (&pt_cfg->t_fg_color_info.u_color_data.t_standard.t_disable, 
                  &t_fg_dis, 
                  sizeof(GL_COLOR_T));

        x_memcpy (&pt_cfg->t_fg_color_info.u_color_data.t_standard.t_highlight, 
                  &t_fg_hlt, 
                  sizeof(GL_COLOR_T));
        
        pt_cfg->t_font_bg_color.e_type = MM_SBTL_DFT_FONT_BG_TYPE;
        x_memcpy (&pt_cfg->t_font_bg_color.u_color_data.t_standard.t_enable, 
                  &t_font_bg_en,  
                  sizeof(GL_COLOR_T));

        x_memcpy (&pt_cfg->t_font_bg_color.u_color_data.t_standard.t_disable, 
                  &t_font_bg_dis, 
                  sizeof(GL_COLOR_T));

        x_memcpy (&pt_cfg->t_font_bg_color.u_color_data.t_standard.t_highlight, 
                  &t_font_bg_hlt, 
                  sizeof(GL_COLOR_T));
        
        x_strncpy(pt_cfg->s_dest_grp_name, SN_PRES_MAIN_DISPLAY, RM_NAME_LEN);

        pt_cfg->ui1_text_alignment = MM_SBTL_DFT_TEXT_ALIGN;
        pt_cfg->h_gl_plane = h_canvas;
        pt_cfg->i4_speed = 100;

        return MMSBTLR_OK;
    }
    return MMSBTLR_INV_ARG;
}

static VOID _sbtl_cli_state_notify(VOID*        pv_tag,
                                SBTL_COND_T  e_cond,
                                UINT32       e_event,
                                UINT32       ui4_data)
{
    SBTL_ENGINE_T*  pt_engine = mm_sbtl_get_using_engine();
    if (e_cond == SBTL_COND_CLOSED)
    {
        _sbtl_cli_deinit_res();
        DBG_LOG_PRINT(("[sbtl cli] deinit all resource\n"));
    }
    if (e_cond == SBTL_COND_OPENED)
    {
        if (b_simulator && 
            pt_engine &&
            pt_engine->pt_page_mngr &&
            pt_engine->pt_page_mngr->pf_need_dmx)
        {
            BOOL   b_need_dmx = FALSE;
            pt_engine->pt_page_mngr->pf_need_dmx(pt_engine->pt_page_mngr, &b_need_dmx);
            if (b_need_dmx)
            {
                SBTL_DISPLAY_INFO_CMD_T t_dsp_cmd;
                x_memset(&t_dsp_cmd, 0, sizeof(SBTL_DISPLAY_INFO_CMD_T));
                t_dsp_cmd.b_display_info = TRUE;
                t_dsp_cmd.t_rect_refer.i4_left  = 0;
                t_dsp_cmd.t_rect_refer.i4_right = 1920;
                t_dsp_cmd.t_rect_refer.i4_top = 0;
                t_dsp_cmd.t_rect_refer.i4_bottom = 1080;
                t_dsp_cmd.ui4_scrn_width = 1920;
                t_dsp_cmd.ui4_scrn_height = 1080;
                t_dsp_cmd.ui4_src_width =  1920;
                t_dsp_cmd.ui4_src_height = 1080;
                t_dsp_cmd.ui1_b_mgn_percent = 5;
                t_dsp_cmd.ui1_font_max_grade = 18;
                t_dsp_cmd.ui1_font_min_grade = 26;
                t_dsp_cmd.ui1_h_percent = 20;
                t_dsp_cmd.ui1_l_mgn_percent = 5;
                t_dsp_cmd.ui1_r_mgn_percent = 5;
        
                sbtl_util_set_simu_cb(_sbtl_cli_simu_data_func);
                DBG_LOG_PRINT(("[sbtl cli] enable simu cb\n"));
                x_sbtl_engine_set(pt_engine,
                                  EVN_MM_SBTL_SET_TYPE_DISPLAY_INFO,
                                  &t_dsp_cmd,
                                  sizeof(SBTL_DISPLAY_INFO_CMD_T));
            }
        }
    }
    DBG_LOG_PRINT(("[sbtl cli] cond:%d event:%d\n", e_cond, e_event));
}


static INT32 _sbtl_cli_ctrl_engine(INT32              i4_argc, 
                                     const CHAR**        pps_argv)
{
    if (i4_argc < 2)
    {
        return CLIR_UNKNOWN_CMD;
    }
    SBTL_ENGINE_T*  pt_engine = NULL;
    MM_SBTL_CLI_CMD e_cmd = MM_SBTL_CLI_CTRL_NONE;
    UINT16          ui2_i;
    INT32           i4_ret = MMSBTLR_OK;
    for (ui2_i = 0; ui2_i < sizeof(t_ctrl_tbl)/sizeof(MM_SBTL_CLI_CMD_TBL); ui2_i++)
    {
        if (x_strcasecmp(pps_argv[1], t_ctrl_tbl[ui2_i].s_name) == 0)
        {
            e_cmd = t_ctrl_tbl[ui2_i].e_cmd;
        }
    }
    if (e_cmd == MM_SBTL_CLI_CTRL_NONE)
    {
        DBG_LOG_PRINT(("Ctrl Command List cmd :: args\n"));
        for (ui2_i = 0; ui2_i < sizeof(t_ctrl_tbl)/sizeof(MM_SBTL_CLI_CMD_TBL); ui2_i++)
        {
            DBG_LOG_PRINT(("    %s :: %s\n", t_ctrl_tbl[ui2_i].s_name,  t_ctrl_tbl[ui2_i].s_desc));
        }
        return CLIR_OK;
    }
    pt_engine = mm_sbtl_get_using_engine();
    if (e_cmd != MM_SBTL_CLI_CTRL_OPEN)
    {
        if (pt_engine == NULL)
        {
            DBG_LOG_PRINT(("[mm sbtl] cli engine not OK yet!\n"));
            return 0;
        }
        switch (e_cmd)
        {
            case MM_SBTL_CLI_CTRL_PLAY:
                i4_ret = x_sbtl_engine_set(pt_engine,
                            EVN_MM_SBTL_APP_PLAY_REQ,
                            NULL,
                            0);
                break;
                
            case MM_SBTL_CLI_CTRL_STOP:
                i4_ret = x_sbtl_engine_set(pt_engine,
                    EVN_MM_SBTL_APP_STOP_REQ,
                    NULL,
                    0);
                break;
                
            case MM_SBTL_CLI_CTRL_CLOSE:
                 i4_ret = x_sbtl_engine_close(pt_engine);
                 
            default: 
                break;        
        }
    }
    else
    {
        SBTL_CFG_T  t_sbtl_cfg;
        VOID*       pv_cur_engine;
        
        if (pt_engine != NULL)
        {
            DBG_LOG_PRINT(("sbtl engine already runs!\n"));
            return CLIR_ALREADY_INIT;
        }
        
        x_memset(&t_sbtl_cfg, 0, sizeof(SBTL_CFG_T));
        do
        {
            i4_ret = _sbtl_cli_init_res();
            if (i4_ret != MMSBTLR_OK)break;
            
            i4_ret = _sbtl_cli_parse_open_cmds(i4_argc-1, pps_argv+1, &t_sbtl_cfg);
            if (i4_ret != MMSBTLR_OK)break;
        
            i4_ret = x_sbtl_engine_open(
                     &t_sbtl_cfg,
                     TRUE,
                     _sbtl_cli_state_notify,
                     NULL,
                     &pv_cur_engine);
            if (i4_ret != MMSBTLR_OK)break;
            
            pt_engine = (SBTL_ENGINE_T*)pv_cur_engine;
            {
                SBTL_DISPLAY_INFO_CMD_T t_video_disp_info;
                SIZE_T                  z_info_len;
                //fake to set disp rect
                x_memset(&t_video_disp_info, 0, sizeof(SBTL_DISPLAY_INFO_CMD_T));
                t_video_disp_info.b_display_info = TRUE;
                t_video_disp_info.t_rect_refer.i4_left  = 0;
                t_video_disp_info.t_rect_refer.i4_right = 1920;
                t_video_disp_info.t_rect_refer.i4_top = 0;
                t_video_disp_info.t_rect_refer.i4_bottom = 1080;
                t_video_disp_info.ui4_scrn_width = 1920;
                t_video_disp_info.ui4_scrn_height = 1080;
                t_video_disp_info.ui4_src_width =  1920;
                t_video_disp_info.ui4_src_height = 1080;
                t_video_disp_info.ui1_b_mgn_percent = 5;
                t_video_disp_info.ui1_font_max_grade = 18;
                t_video_disp_info.ui1_font_min_grade = 26;
                t_video_disp_info.ui1_h_percent = 20;
                t_video_disp_info.ui1_l_mgn_percent = 5;
                t_video_disp_info.ui1_r_mgn_percent = 5;
                z_info_len = sizeof(SBTL_DISPLAY_INFO_CMD_T);
                x_sbtl_engine_set(pv_cur_engine,
                             EVN_MM_SBTL_SET_TYPE_DISPLAY_INFO,
                             &t_video_disp_info,
                             z_info_len);
            }
            
            return i4_ret;
        }while(0);
        
        _sbtl_cli_deinit_res();
        
    }
    return i4_ret;
}

static INT32 _sbtl_cli_get_engine(INT32              i4_argc, 
                                     const CHAR**        pps_argv)
{
    SBTL_ENGINE_T*  pt_engine = NULL;
    UINT16          ui2_i = 0;
    INT32           i4_ret = MMSBTLR_OK;
    pt_engine = mm_sbtl_get_using_engine();
    if (pt_engine == NULL)
    {
        DBG_LOG_PRINT(("[mm sbtl] cli engine not OK yet!\n"));
        return 0;
    }
    if (i4_argc == 2)
    {
        MM_SBTL_CLI_CMD e_cmd = MM_SBTL_CLI_GET_NONE;
        
        for (ui2_i = 0; ui2_i < sizeof(t_get_tbl)/sizeof(MM_SBTL_CLI_CMD_TBL); ui2_i++)
        {
            if (x_strcasecmp(pps_argv[1], t_get_tbl[ui2_i].s_name) == 0)
            {
                e_cmd = t_get_tbl[ui2_i].e_cmd;
            }
        }
        
        if (e_cmd == MM_SBTL_CLI_GET_NONE)
        {
            DBG_LOG_PRINT(("Get Command List cmd :: args\n"));
            for (ui2_i = 0; ui2_i < sizeof(t_get_tbl)/sizeof(MM_SBTL_CLI_CMD_TBL); ui2_i++)
            {
                DBG_LOG_PRINT(("    %s :: %s\n", t_get_tbl[ui2_i].s_name,  t_get_tbl[ui2_i].s_desc));
            }
            return CLIR_OK;
        }
        
        switch(e_cmd)
        {
            case MM_SBTL_CLI_GET_FONT:
            {
                WGL_FONT_INFO_T t_font;
                x_memset(&t_font, 0, sizeof(WGL_FONT_INFO_T));
                i4_ret = x_sbtl_engine_get(pt_engine,
                                  EVN_MM_SBTL_GET_OSD_FONT_INFO,
                                  &t_font,
                                  sizeof(WGL_FONT_INFO_T));
                if (i4_ret != MMSBTLR_OK) break;
                DBG_LOG_PRINT(("current using font name:%s sizetype:%d cmap:%x style:%d width:%d size:%d\n", 
                           t_font.a_c_font_name,
                           t_font.e_font_size,
                           t_font.e_font_cmap,
                           t_font.e_font_style,
                           t_font.i2_width,
                           t_font.ui1_custom_size));
            }
            break;
            
            case MM_SBTL_CLI_GET_COLOR_FG:
            case MM_SBTL_CLI_GET_COLOR_BG:
            case MM_SBTL_CLI_GET_COLOR_EDGE:
            {
                MM_EVN_TYPE_T    e_type = EVN_MM_SBTL_GET_OSD_FG_COLOR;
                WGL_COLOR_INFO_T t_color;
                CHAR*            ps_type = "fg";
                x_memset(&t_color, 0, sizeof(WGL_COLOR_INFO_T));
                if (e_cmd == MM_SBTL_CLI_GET_COLOR_BG)
                {
                    e_type = EVN_MM_SBTL_GET_OSD_BG_COLOR;
                    ps_type = "bg";
                }
                else if (e_cmd == MM_SBTL_CLI_GET_COLOR_EDGE)
                {
                    e_type = EVN_MM_SBTL_GET_OSD_FONT_EDGE_COLOR;
                    ps_type = "edge";
                }
                i4_ret = x_sbtl_engine_get(pt_engine,
                                  e_type,
                                  &t_color,
                                  sizeof(WGL_COLOR_INFO_T));
                if (i4_ret != MMSBTLR_OK) break;
                DBG_LOG_PRINT(("Current text %scolor is: type:%d argb=[%d,%d,%d,%d]\n", 
                           ps_type,
                           t_color.e_type,
                           t_color.u_color_data.t_basic.t_enable.a,
                           t_color.u_color_data.t_basic.t_enable.u1.r,
                           t_color.u_color_data.t_basic.t_enable.u2.g,
                           t_color.u_color_data.t_basic.t_enable.u3.b));
            }
            break;
            
            case MM_SBTL_CLI_GET_ENCODE:
            {
                MM_SBTL_ENCODING_T e_encode = MM_SBTL_ENCODE_AUTO;
                i4_ret = x_sbtl_engine_get(pt_engine,
                                  EVN_MM_SBTL_GET_ENCODING,
                                  &e_encode,
                                  sizeof(MM_SBTL_ENCODING_T));
                if (i4_ret != MMSBTLR_OK) break;
                DBG_LOG_PRINT(("Current encoding is: %d\n", e_encode));               
            }
            break;
            
            case MM_SBTL_CLI_GET_SHOW:
            {
                BOOL b_show = TRUE;
                i4_ret = x_sbtl_engine_get(pt_engine,
                                  EVN_MM_SBTL_GET_SHOW_HIDE,
                                  &b_show,
                                  sizeof(BOOL));
                if (i4_ret != MMSBTLR_OK) break;
                DBG_LOG_PRINT(("Current show is: %d\n", b_show));                 
            }
            break;
            
            default:
            break;
        }
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_LOG_PRINT(("[sbtl cli] get ret %d\n", i4_ret));
        }
        
        return MMSBTLR_OK;
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
}


static INT32 _sbtl_cli_set_engine(INT32              i4_argc, 
                                     const CHAR**             pps_argv)
{
    SBTL_ENGINE_T*  pt_engine = NULL;
    UINT16          ui2_i = 0;
    INT32           i4_ret = MMSBTLR_INV_ARG;
    pt_engine = mm_sbtl_get_using_engine();
    if (pt_engine == NULL)
    {
        DBG_LOG_PRINT(("[mm sbtl] cli engine not OK yet!\n"));
        return 0;
    }
    if (i4_argc > 0)
    {
        MM_SBTL_CLI_CMD e_set_cmd = MM_SBTL_CLI_SET_NONE;
        for (ui2_i = 0; ui2_i < sizeof(t_set_tbl)/sizeof(MM_SBTL_CLI_CMD_TBL); ui2_i++)
        {
            if (x_strcasecmp(pps_argv[1], t_set_tbl[ui2_i].s_name) == 0)
            {
                e_set_cmd = t_set_tbl[ui2_i].e_cmd;
                break;
            }
        }
        if (e_set_cmd == MM_SBTL_CLI_SET_NONE)
        {
            DBG_LOG_PRINT(("Command List: cmd :: args\n"));
            for (ui2_i = 0; ui2_i < sizeof(t_set_tbl)/sizeof(MM_SBTL_CLI_CMD_TBL); ui2_i++)
            {
                DBG_LOG_PRINT(("    %s :: %s\n", t_set_tbl[ui2_i].s_name,  t_set_tbl[ui2_i].s_desc));
            }
            return CLIR_OK;
            
        }
        switch(e_set_cmd)
        {
            case MM_SBTL_CLI_SET_FONT:
            {
                WGL_FONT_INFO_T             t_font_info;
                x_memset(&t_font_info, 0, sizeof(WGL_FONT_INFO_T));
                if (i4_argc >= 2)
                {
                    i4_ret = x_sbtl_engine_get(pt_engine,
                                  EVN_MM_SBTL_GET_OSD_FONT_INFO,
                                  &t_font_info,
                                  sizeof(WGL_FONT_INFO_T));
                    if (i4_ret != MMSBTLR_OK) break;
                    if (x_strcasecmp(pps_argv[2], "B") == 0)
                    {
                        t_font_info.e_font_size = FE_FNT_SIZE_LARGE;
                    }
                    else if (x_strcasecmp(pps_argv[2], "M") == 0)
                    {
                        t_font_info.e_font_size = FE_FNT_SIZE_MEDIUM;
                    }
                    else if (x_strcasecmp(pps_argv[2], "S") == 0)
                    {
                        t_font_info.e_font_size = FE_FNT_SIZE_SMALL;
                    }
                    else if (x_strcasecmp(pps_argv[2], "C") == 0)
                    {
                        t_font_info.e_font_size = FE_FNT_SIZE_CUSTOM;
                    }
                    for (ui2_i = 3; ui2_i < (UINT16)i4_argc; ui2_i++)
                    {
                        if (x_strncmp(pps_argv[ui2_i],"size:", 5) == 0)
                        {
                            t_font_info.ui1_custom_size = (UINT8)x_strtoll(
                                pps_argv[ui2_i]+5,
                                NULL,
                                10);
                        }
                        else if (x_strncmp(pps_argv[ui2_i],"style:", 6) == 0)
                        {
                            t_font_info.e_font_style = (FE_FNT_STYLE)x_strtoll(
                                pps_argv[ui2_i]+6,
                                NULL,
                                10);
                        }
                        else if (x_strncmp(pps_argv[ui2_i],"name:", 5) == 0)
                        {
                            if (x_strlen(pps_argv[ui2_i]+5) > 0)
                            {
                                x_strncpy(t_font_info.a_c_font_name, pps_argv[ui2_i]+5, WGL_MAX_FONT_NAME);
                            }
                        }
                    }
                    i4_ret = x_sbtl_engine_set(pt_engine,
                                  EVN_MM_SBTL_SET_OSD_FONT_INFO,
                                  &t_font_info,
                                  sizeof(WGL_FONT_INFO_T));
                }
            }
            break;

            case MM_SBTL_CLI_SET_COLOR_BG:
            case MM_SBTL_CLI_SET_COLOR_FG:
            case MM_SBTL_CLI_SET_COLOR_EDGE:
            case MM_SBTL_CLI_SET_COLOR_HF:
            case MM_SBTL_CLI_SET_COLOR_HB:
            {
                MM_EVN_TYPE_T    e_type = EVN_MM_SBTL_GET_OSD_FG_COLOR;
                WGL_COLOR_INFO_T t_color;
				GL_COLOR_T       t_rgba = {0};
                x_memset(&t_color, 0, sizeof(WGL_COLOR_INFO_T));
                if (i4_argc != 6)
                {
                    DBG_LOG_PRINT(("Set color should like TF 0 0 0 0\n"));
                    break;
                }
                if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_BG)
                {
                    e_type = EVN_MM_SBTL_GET_OSD_BG_COLOR;
                }
                else if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_EDGE)
                {
                    e_type = EVN_MM_SBTL_GET_OSD_FONT_EDGE_COLOR;
                }
                else if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_HF)
                {
                    //e_type = EVN_MM_SBTL_GET_HLT_FG_COLOR;
                     e_type = EVN_MM_SBTL_GET_OSD_FG_COLOR;
                }
                else if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_HB)
                {
                    //e_type = EVN_MM_SBTL_GET_HLT_BG_COLOR;
					e_type = EVN_MM_SBTL_GET_OSD_BG_COLOR;
                }
                i4_ret = x_sbtl_engine_get(pt_engine,
                                  e_type,
                                  &t_color,
                                  sizeof(WGL_COLOR_INFO_T));
                if (i4_ret != MMSBTLR_OK) break;
				t_rgba.a    = (UINT8)x_strtoll(pps_argv[2], NULL, 10);
				t_rgba.u1.r = (UINT8)x_strtoll(pps_argv[3], NULL, 10);
				t_rgba.u2.g = (UINT8)x_strtoll(pps_argv[4], NULL, 10);
				t_rgba.u3.b = (UINT8)x_strtoll(pps_argv[5], NULL, 10);
				
				if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_HF ||
					e_set_cmd == MM_SBTL_CLI_SET_COLOR_HB)
				{
					t_color.u_color_data.t_standard.t_highlight = t_rgba;
				}
				else
				{ 
					t_color.u_color_data.t_standard.t_enable = t_rgba;
				}               
    
                e_type = EVN_MM_SBTL_SET_OSD_FG_COLOR;
                if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_BG)
                {
                    e_type = EVN_MM_SBTL_SET_FONT_BG_COLOR;
                }
                else if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_EDGE)
                {
                    e_type = EVN_MM_SBTL_SET_OSD_FONT_EDGE_COLOR;
                }
                else if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_HF)
                {
                    e_type = EVN_MM_SBTL_SET_HLT_FG_COLOR;
                }
                else if (e_set_cmd == MM_SBTL_CLI_SET_COLOR_HB)
                {
                    e_type = EVN_MM_SBTL_SET_HLT_BG_COLOR;
                }
                i4_ret = x_sbtl_engine_set(pt_engine,
                                  e_type,
                                  &t_color,
                                  sizeof(WGL_COLOR_INFO_T));
            }
            break;

            case MM_SBTL_CLI_SET_ENCODE:
            {
                if (i4_argc == 3)
                {
                    MM_SBTL_ENCODING_T e_encode;
                    e_encode = (MM_SBTL_ENCODING_T)x_strtoll(pps_argv[2], NULL, 10);
                    i4_ret = x_sbtl_engine_set(pt_engine,
                                  EVN_MM_SBTL_SET_ENCODING,
                                  &e_encode,
                                  sizeof(MM_SBTL_ENCODING_T));
                }
            }
            break;

            case MM_SBTL_CLI_SET_SHOW:
            {
                if (i4_argc == 3)
                {
                    BOOL b_show;
                    b_show = (BOOL)x_strtoll(pps_argv[2], NULL, 10);
                    i4_ret = x_sbtl_engine_set(pt_engine,
                                  EVN_MM_SBTL_SET_SHOW_HIDE,
                                  &b_show,
                                  sizeof(BOOL));
                }
            }
            break;

            case MM_SBTL_CLI_SET_LINEGAP:
            {
                if (i4_argc == 3)
                {
                    INT32 i4_gap;
                    i4_gap = (INT32)x_strtoll(pps_argv[2], NULL, 10);
                    i4_ret = x_sbtl_engine_set(pt_engine,
                                    EVN_MM_SBTL_SET_LINE_GAP,
                                    (VOID*)&i4_gap,
                                    sizeof(INT32));  
                }
            }
            break;

            case MM_SBTL_CLI_SET_TIME_OFFSET:
            {
                if (i4_argc == 3)
                {
                    MM_SBTL_TM_OFST_INF     t_time_ofst;
                    
                    x_memset(&t_time_ofst,0,sizeof(MM_SBTL_TM_OFST_INF));
                    t_time_ofst.e_ofst_mode = MM_SBTL_TM_OFST_MODE_USER;
                    t_time_ofst.i8_time_ofst = (INT64)x_strtoll(pps_argv[2], NULL, 10);
                    
                    i4_ret = x_sbtl_engine_set(pt_engine,
                                    EVN_MM_SBTL_SET_ADJUST_TIME,
                                    (VOID*)&t_time_ofst,
                                    sizeof(MM_SBTL_TM_OFST_INF));  
                }
            }
            break;

            case MM_SBTL_CLI_SET_PAGE_INFO:
            {
                MM_SBTL_DISP_PAGE_T   t_disp_page ;
                x_memset(&t_disp_page, 0, sizeof(MM_SBTL_DISP_PAGE_T));
                if (i4_argc != 7)
                {
                    DBG_LOG_PRINT(("[sbtl cli] need mode:[F/S] lines:[d] iterv:[d] hlt:[d] gap:[d]\n"));
                    break;
                }
                if (x_strcmp(pps_argv[2], "S") == 0)
                {
                    t_disp_page.e_page_mode = MM_SBTL_PAGE_SCROLL_LINE;
                }
                else
                {
                    t_disp_page.e_page_mode = MM_SBTL_PAGE_HLT_FIXED;
                }
                t_disp_page.ui2_page_line_num = (UINT16)x_strtoll(pps_argv[3], NULL, 10);
                t_disp_page.ui2_interval_line = (UINT16)x_strtoll(pps_argv[4], NULL, 10);
                t_disp_page.ui2_hlt_fix_line  = (UINT16)x_strtoll(pps_argv[5], NULL, 10);
                t_disp_page.i4_line_gap       =  (INT32)x_strtoll(pps_argv[6], NULL, 10);

                i4_ret = x_sbtl_engine_set(pt_engine,
                                EVN_MM_SBTL_SET_PAGE_INF,
                                &t_disp_page,
                                sizeof(MM_SBTL_DISP_PAGE_T));  
            }
            break;
            case MM_SBTL_CLI_SET_BDR_TYPE:
            {
                WGL_INSET_T      t_bdr_inset;
                x_memset(&t_bdr_inset, 0, sizeof(WGL_INSET_T));
                
                if (i4_argc == 3)
                {
                    t_bdr_inset.i4_left
                        = t_bdr_inset.i4_right 
                        = t_bdr_inset.i4_top 
                        = t_bdr_inset.i4_bottom
                        = (INT32)x_strtoll(pps_argv[2], NULL, 10);
                }
                else if (i4_argc == 4)
                {
                    t_bdr_inset.i4_left = t_bdr_inset.i4_right = (INT32)x_strtoll(pps_argv[2], NULL, 10);
                    t_bdr_inset.i4_top = t_bdr_inset.i4_bottom = (INT32)x_strtoll(pps_argv[3], NULL, 10);
                }
                else
                {
                    i4_ret = MMSBTLR_INV_ARG;
                    break;
                }
                i4_ret = x_sbtl_engine_set(pt_engine,
                                EVN_MM_SBTL_SET_OSD_INSECT,
                                &t_bdr_inset,
                                sizeof(WGL_INSET_T)); 
            }
                
            break;
            case MM_SBTL_CLI_SET_RECT:
            {
                if (i4_argc == 6)
                {
                    GL_RECT_T  t_rect;
                    x_memset(&t_rect, 0, sizeof(GL_RECT_T));
                    t_rect.i4_left = (INT32)x_strtoll(pps_argv[2], NULL, 10);
                    t_rect.i4_top = (INT32)x_strtoll(pps_argv[3], NULL, 10);
                    t_rect.i4_right = t_rect.i4_left + (INT32)x_strtoll(pps_argv[4], NULL, 10);
                    t_rect.i4_bottom = t_rect.i4_top + (INT32)x_strtoll(pps_argv[5], NULL, 10);
                    i4_ret = x_sbtl_engine_set(pt_engine,
                                EVN_MM_SBTL_SET_OSD_RECT,
                                &t_rect,
                                sizeof(GL_RECT_T)); 
                }
                else
                {
                    i4_ret = MMSBTLR_INV_ARG;
                }
            }
            break;

            case MM_SBTL_CLI_SET_POS:
            {
                if (i4_argc == 4)
                {
                    GL_POINT_T t_pos;
                    t_pos.i4_x = (INT32)x_strtoll(pps_argv[2], NULL, 10);
                    t_pos.i4_y = (INT32)x_strtoll(pps_argv[3], NULL, 10);
                    i4_ret = x_sbtl_engine_set(pt_engine,
                                EVN_MM_SBTL_SET_OSD_OFFSET,
                                &t_pos,
                                sizeof(GL_POINT_T)); 
                }
                else
                {
                    i4_ret = MMSBTLR_INV_ARG;
                }
            }
            break;
            
            case MM_SBTL_CLI_SET_BUFFER_FILE:
            {
                INT32 i4_i = 0;
                SBTL_BUFFER_FILE_INFO_T t_buff_file;
                x_memset(&t_buff_file, 0, sizeof(SBTL_BUFFER_FILE_INFO_T));
                for (i4_i = 2; i4_i < i4_argc; i4_i++)
                {
                    if (x_strncmp(pps_argv[i4_i], "start:", 6) == 0)
                    {
                        t_buff_file.ui4_start_time =
                            (UINT32)x_strtoll(pps_argv[i4_i]+6,
                                              NULL,
                                              10);
                    }
                    else
                    if (x_strncmp(pps_argv[i4_i], "end:", 4) == 0)
                    {
                        t_buff_file.ui4_end_time =
                            (UINT32)x_strtoll(pps_argv[i4_i]+4,
                                              NULL,
                                              10);
                    }
                    else
                    if (x_strncmp(pps_argv[i4_i], "dync:", 5) == 0)
                    {
                        t_buff_file.e_show_mode = SBTL_SHOW_TYPE_NORMAL;
                        t_buff_file.e_file_type  = SBTL_BUFF_FILE_TYPE_PATH;
                        t_buff_file.u_info.t_path_info.ps_path = x_strdup(pps_argv[i4_i]+5);
                    }
                }
               i4_ret = x_sbtl_engine_set(pt_engine,
                                EVN_MM_SBTL_SET_TYPE_BUFF_FILE_INFO,
                                &t_buff_file,
                                sizeof(SBTL_BUFFER_FILE_INFO_T)); 
 
            }
            break;

            default:
            break;
            
        }
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_LOG_PRINT(("[sbtl cli] set fail:%d\n", i4_ret));
        }
        
    }
    return MMSBTLR_OK;
}


#ifdef MW_IPCC_SUPPORT
static INT32 _sbtl_cli_test_xml_on(INT32              i4_argc, 
                                     const CHAR**             pps_argv)
{
    UINT8      ui1_option;
    ui1_option = (UINT8)x_strtoll(pps_argv[1],NULL,10);
    if(ui1_option == 0)
    {
        g_b_debug_on = FALSE;
    }
    else
    {
        g_b_debug_on = TRUE;
    }
    return CLIR_OK;
}
static INT32 _sbtl_cli_test_xml_parse(INT32              i4_argc, 
                                     const CHAR**             pps_argv)
{
    INT32                   i4_ret = CLIR_OK;
    CHAR*                   ps_buff = NULL;
    SIZE_T                  z_buff_len = 0;
    UINT64                  ui8_pos = 0;
    HANDLE_T                h_xml_file = NULL_HANDLE;
    UINT32                  ui4_read = 0;
    XML_T*                  pt_tmp_xml = NULL;
    SBTL_XML_PARSER_INFO_T  t_parser_info;
    HANDLE_T                h_ele_hash;
    HANDLE_T                h_attr_hash;

    x_memset(&t_parser_info,0,sizeof(SBTL_XML_PARSER_INFO_T));
    
    do
    {
        //open file
        i4_ret = x_fm_open(FM_ROOT_HANDLE, 
                      XML_PARSE_TEST_FILE,
                      FM_READ_ONLY, 
                      0777, 
                      FALSE, 
                      &h_xml_file);
        if (FMR_OK != i4_ret)
        {
            DBG_LOG_PRINT(("Call x_fm_open fail\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }

        //get file size
        i4_ret = x_fm_lseek(h_xml_file, 0, FM_SEEK_END, &ui8_pos);
        DBG_LOG_PRINT(("Get file size is %lld!\n",ui8_pos));
        if (FMR_OK != i4_ret)
        {
            DBG_ERROR(("Get file size error!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        z_buff_len = (SIZE_T)ui8_pos;
        
        //allocate file size memory for xml parser    
        ps_buff = (CHAR*)x_mem_alloc(z_buff_len);
        if(ps_buff == NULL)
        {
            DBG_ERROR(("Memory is not enough!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        x_memset((VOID*)ps_buff, 0, z_buff_len);
        
        //reset the file position
        i4_ret = x_fm_lseek(h_xml_file, 0, FM_SEEK_BGN, &ui8_pos);
        if (FMR_OK != i4_ret)
        {
            DBG_ERROR(("Reset the file position error!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        //read file to allocated buffer
        i4_ret = x_fm_read(h_xml_file, (VOID*)ps_buff, (UINT32)z_buff_len, &ui4_read);
        if ((FMR_OK != i4_ret) && (FMR_EOF != i4_ret))
        {
            DBG_ERROR(("Call x_fm_read fail\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        //create ele hash
        i4_ret = x_sbtl_xml_create_hash(t_hash_ele,sizeof(t_hash_ele)/sizeof(SBTL_XML_HASH_INFO_T),&h_ele_hash);
        if(SBTL_XMLR_SUCCESS != i4_ret)
        {
            DBG_ERROR(("Call x_sbtl_xml_create_hash for ele failed!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        //create attribute hash
        i4_ret = x_sbtl_xml_create_hash(t_hash_attr,sizeof(t_hash_attr)/sizeof(SBTL_XML_HASH_INFO_T),&h_attr_hash);
        if(SBTL_XMLR_SUCCESS != i4_ret)
        {
            DBG_ERROR(("Call x_sbtl_xml_create_hash for attr failed!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }

        t_parser_info.h_attr_hash = h_attr_hash;
        t_parser_info.h_ele_hash = h_ele_hash;
        //begin xml parser
        i4_ret = x_sbtl_xml_parse_by_memory((VOID*)ps_buff,ui4_read,(VOID**)&pt_tmp_xml,&t_parser_info);
       
        if(SBTL_XMLR_SUCCESS != i4_ret)
        {
            DBG_ERROR(("Call x_xml_parse_buffer failed!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        //print xml parser result
        sbtl_xml_print_func(pt_tmp_xml,0);

        //free resource
        i4_ret = x_sbtl_xml_free_hash(h_ele_hash);
        if(i4_ret != SBTL_XMLR_SUCCESS)
        {
            DBG_ERROR(("Call x_sbtl_xml_free_hash failed!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        x_sbtl_xml_free_hash(h_attr_hash);
        if(i4_ret != SBTL_XMLR_SUCCESS)
        {
            DBG_ERROR(("Call x_sbtl_xml_free_hash failed!\n"));
            i4_ret = CLIR_CMD_EXEC_ERROR;
            break;
        }
        
    }while(0);
    
    if (h_xml_file)
    {
        x_fm_close(h_xml_file);
        h_xml_file = NULL_HANDLE;
    }
    if(pt_tmp_xml)
    {
       x_sbtl_xml_parse_free(pt_tmp_xml);
    }
    if(ps_buff != NULL)
    {
        x_mem_free(ps_buff);
    }
    
    return i4_ret;
}

#endif


static INT32 _sbtl_cli_read_png(CHAR* s_path, VOID** pv_data, UINT32* pui4_len)
{
     //open file
     INT32    i4_ret = MMSBTLR_OK;
     UINT64   ui8_pos = 0;
     HANDLE_T h_png_file = NULL_HANDLE;
     UINT32   ui4_size = 0;
     if (s_path == NULL ||
         pv_data == NULL ||
         pui4_len == NULL)
     {
        return MMSBTLR_INV_ARG;
     }
     i4_ret = x_fm_open(FM_ROOT_HANDLE, 
                  s_path,
                  FM_READ_ONLY, 
                  0777, 
                  FALSE, 
                  &h_png_file);
    if (FMR_OK != i4_ret)
    {
        return -1;
    }

        //get file size
    i4_ret = x_fm_lseek(h_png_file, 0, FM_SEEK_END, &ui8_pos);
    if (ui8_pos > 600*1024 || ui8_pos == 0)
    {
        return -2;
    }
    ui4_size = (UINT32)ui8_pos;
    *pv_data = x_mem_alloc(ui4_size);
    
    if (*pv_data)
    {
        i4_ret = x_fm_lseek(h_png_file, 0, FM_SEEK_BGN, &ui8_pos);
        i4_ret = x_fm_read(h_png_file, *pv_data, ui4_size, (UINT32*)&ui8_pos);
        
    }
    else
    {
        i4_ret = -2;
    }
    x_fm_close(h_png_file);
    if ((UINT32)ui8_pos != ui4_size || i4_ret != FMR_OK)
    {
        x_mem_free(*pv_data);
        *pv_data  = NULL;
        return -1;
    }
    *pui4_len = ui4_size;
    return 0;     
}

typedef struct
{
    VOID*          pv_buf;
    UINT32         ui4_len;
    HANDLE_T       h_img;//
    WGL_IMG_T      t_img_info;//this var should have the same life dur as h_img
    WGL_HIMG_TPL_T h_tpl;
}SBTL_CLI_IMG_RES_T;


static INT32 _sbtl_cli_create_viewport(
    GL_HSURFACE_T* ph_surf,
    GL_HVIEW_T*    ph_view,
    UINT32*        pui4_viewport,
    BOOL           b_create)
{
    INT32 i4_ret = 0;
    
    if (ph_surf && ph_view && pui4_viewport)
    {
        if (b_create)
        {
            GL_SURFACE_DESC_T       t_surf_descr;
            UINT32                  ui4_w = 0, ui4_h = 0;
            GL_DISPLAY_OPT_T        t_dsp_opt;
            GL_POINT_T              t_screen_offset = {0};
            
            x_memset(&t_surf_descr, 0, sizeof(GL_SURFACE_DESC_T));
            x_memset(&t_dsp_opt,    0, sizeof(GL_DISPLAY_OPT_T));
            
            i4_ret = x_gl_screen_get_panel_size(h_screen, &ui4_w, &ui4_h);
            if (ui4_w == 0 || ui4_h == 0)
            {
                return -1;
            }
            do
            {
                t_surf_descr.e_clrmode  = GL_COLORMODE_ARGB_D8888;
                t_surf_descr.ui4_width  = ui4_w;
                t_surf_descr.ui4_height = ui4_h; 
                //first create surface
                i4_ret = x_gl_surface_create(&t_surf_descr, ph_surf);

                if (*ph_surf == NULL_HANDLE)
                {
                    DBG_LOG_PRINT(("Create surf ret:(%d)\n", i4_ret));
                    break;
                }
                
                t_dsp_opt.e_flag            = GL_DISP_OPT_POSITION | GL_DISP_OPT_BLEND_OPTION | GL_DISP_OPT_ALPHA;
                t_dsp_opt.e_blend_option    = GL_BLEND_OPT_PIXEL;
                t_dsp_opt.ui1_alpha         = 255;
                t_dsp_opt.t_pos             = t_screen_offset;

                i4_ret = x_gl_view_create(ui4_w,
                                          ui4_h,
                                          TRUE,
                                          ph_view);
                if (*ph_view == NULL_HANDLE)
                {
                    DBG_LOG_PRINT(("Create view ret:(%d)\n", i4_ret));
                    break;
                }
                i4_ret = x_gl_view_create_viewport(
                                          *ph_view,
                                          *ph_surf, 
                                          &t_dsp_opt, 
                                           pui4_viewport);
                return 0;
            }while(0);
        }
        
        if (*ph_view != NULL_HANDLE)
        {
             i4_ret = x_gl_view_delete_viewport(*ph_view, *pui4_viewport);
             
             i4_ret = x_gl_obj_free(*ph_view);
        }
        if (*ph_surf != NULL_HANDLE)
        {
            i4_ret = x_gl_obj_free(*ph_surf);
        }
        if (b_create)
        {
            return -4;
        }
        return 0;
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
}

static INT32 _sbtl_cli_test_png_perform(INT32              i4_argc, 
                                     const CHAR**       pps_argv)
{
    INT32              i4_ret = MMSBTLR_OK;
    CHAR               s_file_list[20] = {"/mnt/usb/sda4/0.png"};
    SBTL_CLI_IMG_RES_T t_img_res[10];
    UINT32             ui4_i;
    UINT8              ui1_img_cnt = 0;
    CHAR*              s_ext = x_strrchr(s_file_list, '/');
    INT32              i4_flag = 0;
    UINT32             ui4_sleep = 0;
    UINT32             ui4_lim_cnt = 300;
    
    GL_RECT_T          t_rect;
    
    t_rect.i4_left  = 0;
    t_rect.i4_top = 0;
    t_rect.i4_right = 100;
    t_rect.i4_bottom = 100;
    
    x_memset(&t_img_res, 0, sizeof(t_img_res));
    
    if (i4_argc > 1)
    {
         i4_flag = (INT32) x_strtoll(pps_argv[1], NULL, 10);
         if (i4_argc > 2)
         {
            ui4_sleep = (INT32) x_strtoll(pps_argv[2], NULL, 10);
         }
         if (i4_argc > 3)
         {
            ui4_lim_cnt = (UINT32) x_strtoll(pps_argv[3], NULL, 10);
         }
         if (i4_argc > 4)
         {
		    x_strncpy(s_file_list, pps_argv[4], 19);
			s_file_list[19] = 0;
         }
         DBG_LOG_PRINT(("Set Preload flag:%d sleep every time: (%d) limittobe:%d path:%s\n", 
		                i4_flag, ui4_sleep, ui4_lim_cnt, s_file_list));
    }
    _sbtl_cli_init_res();
    // open image resource
    for (ui4_i = 0; ui4_i < 10; ui4_i++)
    {
        i4_ret = _sbtl_cli_read_png(
                    s_file_list, 
                   &t_img_res[ui1_img_cnt].pv_buf,
                   &t_img_res[ui1_img_cnt].ui4_len);
        
        if (i4_ret == 0)
        {
            if (i4_flag == 1)
            {
                i4_ret = x_img_mopen(t_img_res[ui1_img_cnt].pv_buf,
                                     t_img_res[ui1_img_cnt].ui4_len, 
                                     "png", 
                                     NULL, 
                                     NULL, 
                                     &t_img_res[ui1_img_cnt].h_img);
                 if (i4_ret != IMGR_OK || t_img_res[ui1_img_cnt].h_img == NULL_HANDLE)
                 {
                    DBG_LOG_PRINT(("image open %d failed. (%d)\n", ui4_i, i4_ret));
                 }
            }
            ui1_img_cnt++;
        }
        else
        {
            DBG_LOG_PRINT(("Can not open file %d ret:(%d)\n", ui4_i, i4_ret));
        }
        if (s_ext)
        {
            s_ext[1]++;
        }
    }
   

    {
        //test for surface show
        GL_HSURFACE_T           h_surf       = NULL_HANDLE;
        GL_HVIEW_T              h_view       = NULL_HANDLE;
        UINT32                  ui4_viewport = 0;
        UINT32                  ui4_test_num = ui4_lim_cnt;
        HANDLE_T                h_img        = NULL_HANDLE;

        i4_ret = _sbtl_cli_create_viewport(&h_surf, &h_view, &ui4_viewport, TRUE);
        if (i4_ret != 0)
        {
            DBG_LOG_PRINT(("create view port failed: %d\n", i4_ret));
            ui4_test_num = 0;
        }
        DBG_LOG_PRINT(("start to test surf to show 300 times: %d\n", x_os_get_sys_tick()));
       
         for (ui4_i = 0; ui4_i < ui4_test_num; ui4_i++)
        {
            UINT32 ui4_cur_idx = ui4_i % ui1_img_cnt;
            UINT32 ui4_img_w = 0;
            UINT32 ui4_img_h = 0;
            if (i4_flag == 0)
            {
                i4_ret = x_img_mopen(t_img_res[ui4_cur_idx].pv_buf,
                                     t_img_res[ui4_cur_idx].ui4_len, 
                                     "png", 
                                     NULL, 
                                     NULL, 
                                     &h_img);
                 if (i4_ret != IMGR_OK)
                 {
                    DBG_LOG_PRINT(("image open %d failed. (%d)\n", ui4_i, i4_ret));
                 }
            }
            else
            {
                h_img = t_img_res[ui4_cur_idx].h_img;
            }
            if (h_img == NULL_HANDLE)
            {
                 DBG_LOG_PRINT(("Skip test flip surf test: %d times\n", i4_ret));
                 continue;
            }
            i4_ret = x_img_get_dimension(
                        h_img,
                        INTRA_IMG_PRIMARY_IMG_TYPE,
                        &ui4_img_w,
                        &ui4_img_h);
            
            i4_ret = x_img_sync_load(h_img,
                                INTRA_IMG_PRIMARY_IMG_TYPE,
                                NULL,
                                IMG_ROTATE_CLOCKWS_0_OP_TYPE,
                                0,
                                0,
                                ui4_img_w,
                                ui4_img_h,
                                t_rect.i4_left,
                                t_rect.i4_top,
                                ui4_img_w,
                                ui4_img_h,
                                h_surf);
            
            if (i4_ret < IMGR_OK)
            {
                DBG_LOG_PRINT(("Fail to load image ret: %d\n", i4_ret));
            }
            //i4_ret = x_gl_plane_flip(h_plane, NULL_HANDLE, TRUE);
            i4_ret = x_gl_plane_flip(h_plane, h_view, TRUE);
            if (i4_flag == 0)
            {
                x_img_close(h_img);
            }
            if (ui4_sleep > 0)
            {
                x_thread_delay(ui4_sleep);
            }
        }
        DBG_LOG_PRINT(("End to test surf to show 300 times: %d\n", x_os_get_sys_tick()));
        _sbtl_cli_create_viewport(&h_surf, &h_view, &ui4_viewport, FALSE);
    }

    //free image resource
    for (ui4_i = 0; ui4_i < ui1_img_cnt; ui4_i++)
    {
        if (t_img_res[ui4_i].h_img != NULL_HANDLE)
        {
            x_img_close(t_img_res[ui4_i].h_img);
            t_img_res[ui4_i].h_img = NULL_HANDLE;
        }
    
        if (t_img_res[ui4_i].pv_buf)
        {
            x_mem_free(t_img_res[ui4_i].pv_buf);
            t_img_res[ui4_i].pv_buf = NULL;
        }
    }
    _sbtl_cli_deinit_res();
    return i4_ret;
}
#endif

/*-----------------------------------------------------------------------------
                    External functions implementations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: mm_sbtl_cli_init
 *
 * Description: This API initializes multimedia subtitle CLI component.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: 
 ---------------------------------------------------------------------------- */ 

extern INT32 mm_sbtl_cli_init()
{
#ifdef CLI_LVL_ALL
    INT32       i4_return;
    
    i4_return = x_cli_attach_cmd_tbl(at_mm_sbtl_root_cmd_tbl, CLI_CAT_MW, CLI_GRP_NONE);

    if (i4_return != CLIR_OK)
    {
        return MMSBTLR_CLI_ERROR;
    }

    return MMSBTLR_OK;
#else
    return MMSBTLR_OK;
#endif
}

#endif /*__KERNEL__*/

