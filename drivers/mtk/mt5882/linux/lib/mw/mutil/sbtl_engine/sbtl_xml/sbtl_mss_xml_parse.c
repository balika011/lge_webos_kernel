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
 * $RCSfile: sbtl_mkv_text_page_inst_mngr.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision: $
 * $SWAuthor: $
 * $MD5HEX: $
 *
 * Description:
 *
 * History:
 *
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
#include "os/inc/os.h"
#include "x_common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"
#include "x_uc_str.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_widget.h"
#include "mutil/sbtl_engine/sbtl_xml_parser.h"
#include "x_sbtl_mss.h"
#include "hash/hash_api.h"



/*==========================UTILITY FUNCTIONS==============================*/

#define SET_RGBA_COLOR(pt_color,sr,sg,sb,sa) \
do {                                         \
    pt_color->a=sa;                          \
    pt_color->u1.r=sr;                       \
    pt_color->u2.g=sg;                       \
    pt_color->u3.b=sb;                       \
}while(0)

INT32 mss_get_color(CHAR* ps_str, GL_COLOR_T* pt_color)
{
    if (ps_str && pt_color)
    {
        switch(ps_str[0])
        {
             case 'b':
                if (x_strcmp(ps_str,"black")==0)
                {
                    SET_RGBA_COLOR(pt_color,0,0,0,255);
                }
                else if (x_strcmp(ps_str,"blue")==0)
                {
                    SET_RGBA_COLOR(pt_color,0,0,255,255);
                }
               break;
               
            case 'l':
                if (x_strcmp(ps_str,"lightgreen")==0)
                {
                    SET_RGBA_COLOR(pt_color,255,128,255,128);
                }
                break;
                
            case 'r':
                if (x_strcmp(ps_str,"red")==0)
                {
                    SET_RGBA_COLOR(pt_color,255,0,0,255);
                }
                else if (x_strncmp(ps_str,"rgba",4)==0)
                {
                    UINT8 rgba[4] ={0,0,0,0};
                    INT8  i1_i   = -1;
                    BOOL  b_s = TRUE;
                    ps_str+=4;
                    while(*ps_str)
                    {
                        if (*ps_str >= '0' && *ps_str <= '9')
                        {
                            if (b_s)
                            {
                                if (i1_i<3)i1_i++;
                                b_s = FALSE;
                            }
                            rgba[i1_i] = rgba[i1_i]*10+(*ps_str-'0');
                        }
                        else
                        {
                            b_s = TRUE;
                            if (i1_i >= 3)break;
                        }
                        ps_str++;
                    }
                    SET_RGBA_COLOR(pt_color,rgba[0],rgba[1],rgba[2],rgba[3]);
                }
                break;
                
            case 't':
                if (x_strncmp(ps_str,"trans",4)==0)
                {
                    SET_RGBA_COLOR(pt_color,0,0,0,0);
                }
            break;
            
           case 'v':
                if (x_strcmp(ps_str,"voilet")==0)
                {
                    SET_RGBA_COLOR(pt_color,255,128,0,255);
                }
                break;
                
            case 'w':
                if (x_strcmp(ps_str,"white")==0)
                {
                    SET_RGBA_COLOR(pt_color,255,255,255,255);
                }
                break;
                
            case 'y':
                if (x_strcmp(ps_str,"yellow")==0)
                {
                    SET_RGBA_COLOR(pt_color,255,255,0,255);
                }
                break;
                
            default:
                break;
        }
        return MMSBTLR_OK;
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
}

CHAR* word_cmp(CHAR* ps_a, const CHAR* ps_b)
{
    while(*ps_a)
    {
        if (*ps_a>='a' && *ps_a <='z')
        {
            break;
        }
        ps_a++;
    }
    while(*ps_b)
    {
        if (*ps_a == *ps_b)
        {
            ps_a++;
            ps_b++;
        }
        else
        {
            break;
        }
    }
    if (*ps_b == 0)
    {
        return ps_a;
    }
    return NULL;
}
INT32 mss_parse_align(CHAR* ps_str,UINT8* pui1_align)
{
     if (ps_str && pui1_align)
     {
        UINT8 ui1_align[2] = {1,1};
        CHAR* ps_now = NULL;
        do
        {
            ps_now = word_cmp(ps_str,"left");
            if (ps_now != NULL)
            {
                ui1_align[0]=0;
                break;
            }
            ps_now = word_cmp(ps_str,"center");
            if (ps_now != NULL)
            {
                ui1_align[0]=1;
                break;
            }
            ps_now = word_cmp(ps_str,"right");
            if (ps_now != NULL)
            {
                ui1_align[0]=2;
                break;
            }
        }while(0);
        if (ps_now)
        {
            ps_str = ps_now;
            do
            {
                ps_now = word_cmp(ps_str,"top");
                if (ps_now != NULL)
                {
                    ui1_align[1]=0;
                    break;
                }
                ps_now = word_cmp(ps_str,"center");
                if (ps_now != NULL)
                {
                    ui1_align[1]=1;
                    break;
                }
                ps_now = word_cmp(ps_str,"bottom");
                if (ps_now != NULL)
                {
                    ui1_align[1]=2;
                    break;
                }
            }while(0);
        }
        
        switch(ui1_align[0])
        {
            case 0:
              switch(ui1_align[1])
              {
                case 0:*pui1_align = WGL_AS_LEFT_TOP;break;
                case 1:*pui1_align = WGL_AS_LEFT_CENTER;break;
                case 2:*pui1_align = WGL_AS_LEFT_BOTTOM;break;
              }
              break;
           case 1:
              switch(ui1_align[1])
              {
                case 0:*pui1_align = WGL_AS_CENTER_TOP;break;
                case 1:*pui1_align = WGL_AS_CENTER_CENTER;break;
                case 2:*pui1_align = WGL_AS_CENTER_BOTTOM;break;
              }
              break;
          case 2:
              switch(ui1_align[1])
              {
                case 0:*pui1_align = WGL_AS_RIGHT_TOP;break;
                case 1:*pui1_align = WGL_AS_RIGHT_CENTER;break;
                case 2:*pui1_align = WGL_AS_RIGHT_BOTTOM;break;
              }
              break;
        }
        return MMSBTLR_OK;
     }
     else
     {
        return MMSBTLR_INV_ARG;
     }
}

INT32 mss_get_font_size(CHAR* ps_str,UINT8* pui1_size)
{
    if (ps_str && pui1_size)
    {
        BOOL b_dig = FALSE;
        UINT8 ui1_m = 0;
        *pui1_size = 0;
        while(*ps_str)
        {
            if (*ps_str >= '0' && *ps_str <= '9')
            {
                ui1_m = ui1_m*10+(*ps_str-'0');
                b_dig = TRUE;
            }
            else if (b_dig)
            {
                break;
            }
            ps_str++;
        }
        *pui1_size = ui1_m;
        return MMSBTLR_OK;
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
}

static INT32  get_timecode(CHAR* ps_str,UINT32 *pui4_time)
{
    /*00:00:00:00*/
    UINT32 ui4_time = 0;
    UINT16 ui2_m = 0;
    UINT16 ui2_cnt = 0; 
    
    if (ps_str == NULL || pui4_time == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    while(*ps_str)
    {
        if (*ps_str >= '0' && *ps_str <= '9')
        {
            ui2_m = ui2_m*10+(*ps_str -'0');
        }
        if (*ps_str == ':')
        {
            ui4_time=ui4_time*60+ui2_m;
            ui2_m = 0;
            ui2_cnt++;
        }
        ps_str++;
    }
    if (ui2_cnt == 2)
    {
        ui4_time = (ui4_time*60+ui2_m)*1000;/*to ms*/
        
    }
    else if (ui2_cnt == 3)
    {
        ui4_time = (ui4_time*1000+ui2_m*10);/*to ms*/
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
    *pui4_time = ui4_time;
    return MMSBTLR_OK;
}

BOOL valid_str(CHAR* ps_str)
{
    if (ps_str)
    {
        while(ps_str)
        {
            if (*ps_str == ' ' ||
                *ps_str == '\n'||
                *ps_str == '\t')
            {
            }
            else
            {
                return TRUE;
            }
            ps_str++;
        }
    }
    return FALSE;
}

/*=================================UTILITY FUNCTION END==============================================*/

#define TEXT_BUF_LEN_MAX    256
#define PTS_FREQ        ((UINT64)(90000))
#define TIMER_PERIOD    500

typedef enum _XML_MSS_PAGE_STATE_T
{
    XML_MSS_STATE_NONE,
    XML_MSS_STATE_DATA_RDY,
    XML_MSS_STATE_DISPLAY,
    XML_MSS_STATE_END
} XML_MSS_PAGE_STATE_T;

typedef INT32 (*sbtl_seek_time_stamp_fnc)(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                                          PTS_T                     t_pts_current,
                                          PTS_T*                    pt_pts_start,
                                          PTS_T*                    pt_pts_end,
                                          BOOL*                     pb_found);

typedef INT32 (*sbtl_load_string_fnc)(SBTL_PAGE_INST_MNGR_T*    pt_mngr);

typedef UINT32 (*sbtl_timer_period_fnc)(SBTL_PAGE_INST_MNGR_T*  pt_mngr);

typedef INT32 (*sbtl_init_fnc)(SBTL_PAGE_INST_MNGR_T*   pt_mngr);

typedef INT32 (*sbtl_deinit_fnc)(SBTL_PAGE_INST_MNGR_T* pt_mngr);

typedef BOOL (*text_parser_verify_fnc)(SBTL_ENGINE_T*   pt_engine);

typedef VOID (*text_parser_start)(SBTL_PAGE_INST_MNGR_T*    pt_mngr);

typedef VOID (*text_parser_stop)(SBTL_PAGE_INST_MNGR_T*     pt_mngr);

typedef struct _SBTL_XML_MSS_FNC_TBL_T
{
    MM_SBTL_TYPE_T              e_sbtl_text_type;
    sbtl_seek_time_stamp_fnc    pf_seek_time;
    sbtl_load_string_fnc        pf_load_string;
    sbtl_timer_period_fnc       pf_timer_period;
    sbtl_init_fnc               pf_init;
    sbtl_deinit_fnc             pf_deinit;
    text_parser_start           pf_start;
    text_parser_stop            pf_stop;
    text_parser_verify_fnc      pf_verify;
}SBTL_XML_MSS_FNC_TBL_T;

#define XML_MSS_MAX_BUF_LEN (8*1024)
typedef struct _MSS_ONE_PARSE
{
    XML_T*     pt_top;
    XML_T*     pt_paras;
    XML_T*     pt_cur;
    HANDLE_T   h_style;
    HANDLE_T   h_region;
    
    UINT32     ui4_start;
    CHAR*      ps_fontfamily;
    CHAR*      ps_fontsize;
    CHAR*      ps_color;
    CHAR*      ps_bgcolor;
    CHAR*      ps_align;
    CHAR*      ps_show;
    
    UINT8      ui1_fontsize;
    CHAR       s_fontfamily[40];
    GL_COLOR_T t_color;
    GL_COLOR_T t_bgcolor;
    UINT8      ui1_align;
    HANDLE_T   h_sbtl_wgl;
}MSS_ONE_PARSE_T;

typedef struct _SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T
{
    XML_MSS_PAGE_STATE_T                e_state;
    BOOL                                b_page_exist;
    UTF16_T                             w2s_text[256];
    UINT8                               aui1_buf[XML_MSS_MAX_BUF_LEN];
   
    PTS_T                               t_pts_s;
    PTS_T                               t_pts_e;

    MSS_ONE_PARSE_T                     t_parse;
    SBTL_XML_MSS_FNC_TBL_T*             pt_fnc_table;
    
    HANDLE_T                            h_timer;
    BOOL                                b_need_dmx;
    UINT32                              ui4_timer;/*work around delay to start*/
    VOID*                               pv_data;
}SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T;

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
static UINT32 _sbtl_xml_mms_timer_period(SBTL_PAGE_INST_MNGR_T*     pt_mngr);
static INT32 _sbtl_xml_mms_timer_start(SBTL_PAGE_INST_MNGR_T*       pt_mngr);
static VOID _sbtl_xml_mms_timer_stop(SBTL_PAGE_INST_MNGR_T*         pt_mngr);


static INT32 _sbtl_xml_mms_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*  pt_mngr,
                                            PTS_T                   t_pts_current,
                                            PTS_T*                  pt_pts_start,
                                            PTS_T*                  pt_pts_end,
                                            BOOL*                   pb_found);

static INT32 _sbtl_xml_mms_load_string(SBTL_PAGE_INST_MNGR_T*   pt_mngr);

static INT32 _sbtl_xml_mms_start(SBTL_PAGE_INST_MNGR_T*    pt_mngr);

static INT32 _sbtl_xml_mms_stop(SBTL_PAGE_INST_MNGR_T*     pt_mngr);

static INT32 _sbtl_xml_mms_need_dmx(SBTL_PAGE_INST_MNGR_T*     pt_mngr, 
                                     BOOL*                      pb_need);

static INT32 _sbtl_xml_mms_handle_timing(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                                          PTS_T                     t_pts);

static INT32 _sbtl_xml_mms_config(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                                        VOID*                    pv_get_info,
                                   UINT32                   ui4_set_type);

static VOID _sbtl_xml_mms_remove(SBTL_PAGE_INST_MNGR_T*        pt_mngr);
static INT32 _sbtl_xml_mms_new_page(SBTL_PAGE_INST_MNGR_T*     pt_mngr);
BOOL _xml_mss_verify(SBTL_ENGINE_T*   pt_engine, UINT8*  pt_type);
static SBTL_PAGE_INST_MNGR_T* _text_page_mngr_constructor (SBTL_ENGINE_T*   pt_engine);
static VOID _text_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*  pt_mngr);
INT32 xml_mss_load_string(SBTL_PAGE_INST_MNGR_T*    pt_mngr);
UINT32 xml_mss_timer_period(SBTL_PAGE_INST_MNGR_T*      pt_mngr);
INT32 xml_mss_init(SBTL_PAGE_INST_MNGR_T*   pt_mngr);
INT32 xml_mss_deinit(SBTL_PAGE_INST_MNGR_T* pt_mngr);
VOID xml_mss_start(SBTL_PAGE_INST_MNGR_T*   pt_mngr);
VOID xml_mss_stop(SBTL_PAGE_INST_MNGR_T*    pt_mngr);

/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/
static SBTL_MNGR_FCT_TBL_T t_text_fnct_tbl = 
{
    TRUE,/*false later*/
    _text_page_mngr_constructor,
    _text_page_mngr_destructor,
    _xml_mss_verify
};
INT32 xml_mss_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                              PTS_T                     t_pts_current,
                              PTS_T*                    pt_pts_start,
                              PTS_T*                    pt_pts_end,
                              BOOL*                     pb_found);

static SBTL_XML_MSS_FNC_TBL_T  at_fnc_tbl_list[] = 
{
    {
        MM_SBTL_TYPE_XML_MSS,
        xml_mss_seek_time_stamp,
        xml_mss_load_string,
        xml_mss_timer_period,
        xml_mss_init,
        xml_mss_deinit,
        xml_mss_start,
        xml_mss_stop,
        NULL
    },

    {
        MM_SBTL_TYPE_XML_MSS_EXT,
        xml_mss_seek_time_stamp,
        xml_mss_load_string,
        xml_mss_timer_period,
        xml_mss_init,
        xml_mss_deinit,
        xml_mss_start,
        xml_mss_stop,
        NULL
    }
};

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
    SBTL_XML_MMS_ATTR_END
    
}SBTL_MMS_ATTR_T;

static SBTL_XML_HASH_INFO_T t_hash_attr[] =
{
    {"id",             (INT32)SBTL_XML_MMS_ATTR_ID},
    {"fontfamily",     (INT32)SBTL_XML_MMS_ATTR_FONTFAMILY},
    {"fontsize",       (INT32)SBTL_XML_MMS_ATTR_FONTSIZE},
    {"textalign",      (INT32)SBTL_XML_MMS_ATTR_TEXTALIGN},
    //{"origin",         (INT32)SBTL_XML_MMS_ATTR_ORIGIN},
    //{"extent",         (INT32)SBTL_XML_MMS_ATTR_EXTENT},
    {"backgroundcolor",(INT32)SBTL_XML_MMS_ATTR_BGCOLOR},
    {"dispalign",      (INT32)SBTL_XML_MMS_ATTR_DISPALIGN},
    {"style",          (INT32)SBTL_XML_MMS_ATTR_STYLE},
    {"color",          (INT32)SBTL_XML_MMS_ATTR_COLOR},
    //{"zindex",         (INT32)SBTL_XML_MMS_ATTR_ZINDEX},
    {"region",         (INT32)SBTL_XML_MMS_ATTR_REGION},
    {"begin",          (INT32)SBTL_XML_MMS_ATTR_BEGIN},
    {"end",            (INT32)SBTL_XML_MMS_ATTR_END}, 
};

static SBTL_XML_HASH_INFO_T t_hash_elm[] =
{
    {"tt",         (INT32)SBTL_XML_ELM_TT},
    {"head",       (INT32)SBTL_XML_ELM_HEAD},
    {"metadata",   (INT32)SBTL_XML_ELM_METADATA},
    {"title",      (INT32)SBTL_XML_ELM_TITLE},
    {"desc",       (INT32)SBTL_XML_ELM_DESC},
    {"copyright",  (INT32)SBTL_XML_ELM_COPYRIGHT},
    {"styling",    (INT32)SBTL_XML_ELM_STYLING},
    {"style",      (INT32)SBTL_XML_ELM_STYLE},
    {"layout",     (INT32)SBTL_XML_ELM_LAYOUT},
    {"region",     (INT32)SBTL_XML_ELM_REGION},
    {"div",        (INT32)SBTL_XML_ELM_DIV},
    {"body",       (INT32)SBTL_XML_ELM_BODY},
    {"p",          (INT32)SBTL_XML_ELM_PARA},
    {"span",       (INT32)SBTL_XML_ELM_SPAN},
    {"br",         (INT32)SBTL_XML_ELM_BR},
};

static HANDLE_T  h_elm_hash  = NULL_HANDLE;
static HANDLE_T  h_attr_hash = NULL_HANDLE;
static UINT32    ui4_ref_cnt = 0;

/*====================================Functions================================*/

BOOL _xml_mss_verify(SBTL_ENGINE_T*   pt_engine,
                          UINT8*           pt_type)
{
    CHAR    s_data[101] = {0};
    CHAR*   ps_str = s_data;
    UINT32  ui4_size = 0;
    ASSERT(pt_engine, ("pt_engine=NULL\n"));
    
    if (*pt_type == MM_SBTL_TYPE_XML_MSS)
    {
        return TRUE;
    }
    sbtl_feeder_init(pt_engine, MM_SBTL_FEEDER_SUBTITLE);
    sbtl_feeder_setpos(pt_engine, 0, MEDIA_SEEK_BGN, MM_SBTL_FEEDER_SUBTITLE);
    sbtl_feeder_inpnbyte(pt_engine, 99, (UINT8*)s_data, &ui4_size, MM_SBTL_FEEDER_SUBTITLE);
    s_data[ui4_size]= 0;
    sbtl_feeder_setpos(pt_engine, 0, MEDIA_SEEK_BGN, MM_SBTL_FEEDER_SUBTITLE);
    
    while(*ps_str)
    {
        if (x_strncmp(ps_str,"<?xml",5) == 0)
        {
            return TRUE;
        }
        ps_str++;
    }
    return FALSE;
}


static INT32 mss_iterate_attr(XML_ATTRIBUTE_T*   pt_attr,
                              VOID*              pv_arg,
                              INT32              i4_depth)
{
    if (i4_depth > 3)
    {
        return MMSBTLR_OK;
    }
    if (pt_attr)
    {
        MSS_ONE_PARSE_T* pt_parse = (MSS_ONE_PARSE_T*)pv_arg;
        INT32 i4_ret;
        XML_T* pt_cur;
        
        switch(pt_attr->i4_type)
        {
             case SBTL_XML_MMS_ATTR_STYLE:
                 if (pt_attr->ps_value && pt_parse->h_style)
                 {
                    pt_cur = NULL;
                    i4_ret = hash_get_obj(pt_parse->h_style,
                                         (INT8*)pt_attr->ps_value,
                                         x_strlen(pt_attr->ps_value),
                                         (VOID**)&pt_cur);
                     if (i4_ret == 0 &&
                         pt_cur != NULL)
                     {
                        x_sbtl_xml_iterate_all_attr(pt_cur,
                                         mss_iterate_attr,
                                         pv_arg,
                                         i4_depth+1);
                     }
                 }
                 break;
             case SBTL_XML_MMS_ATTR_REGION:
                 if (pt_attr->ps_value && pt_parse->h_region)
                 {
                    pt_cur = NULL;
                    i4_ret = hash_get_obj(pt_parse->h_region,
                                         (INT8*)pt_attr->ps_value,
                                         x_strlen(pt_attr->ps_value),
                                         (VOID**)&pt_cur);
                     if (i4_ret == 0 &&
                         pt_cur != NULL)
                     {
                        x_sbtl_xml_iterate_all_attr(pt_cur,
                                         mss_iterate_attr,
                                         pv_arg,
                                         i4_depth+1);
                     }
                 }
                 break;
             case SBTL_XML_MMS_ATTR_COLOR:
                 pt_parse->ps_color = pt_attr->ps_value;
                 break;
             /*save to your structure*/
             case SBTL_XML_MMS_ATTR_BGCOLOR:
                 pt_parse->ps_bgcolor = pt_attr->ps_value;
                 break;
             case SBTL_XML_MMS_ATTR_FONTFAMILY:
                 pt_parse->ps_fontfamily= pt_attr->ps_value;
                 break;
             case SBTL_XML_MMS_ATTR_FONTSIZE:
                 pt_parse->ps_fontsize = pt_attr->ps_value;
                 break;
             case SBTL_XML_MMS_ATTR_TEXTALIGN:
                 pt_parse->ps_align = pt_attr->ps_value;
                 break;
             default:
                break;
        }
        return MMSBTLR_OK;
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
}


static VOID reset_all_attr(MSS_ONE_PARSE_T* pt_parse)
{
    pt_parse->ps_fontfamily = NULL;
    pt_parse->ps_fontsize = NULL;
    pt_parse->ps_color = NULL;
    pt_parse->ps_bgcolor= NULL;
    pt_parse->ps_align = NULL; 
}

static VOID set_all_sttr(MSS_ONE_PARSE_T* pt_parse)
{
    UINT8      ui1_fontsize;
    GL_COLOR_T t_color;
    UINT8      ui1_align;
    INT32      i4_ret;
    WGL_COLOR_INFO_T t_clr_info;

    if (valid_str(pt_parse->ps_show) == FALSE)
    {
        return;
    }
    x_memset(&t_color, 0, sizeof(GL_COLOR_T));
    x_memset(&t_clr_info, 0, sizeof(WGL_COLOR_INFO_T));
    if (x_strcmp(pt_parse->ps_fontfamily,pt_parse->s_fontfamily) != 0)
    {
        //x_strncpy(pt_parse->s_fontfamily,pt_parse->ps_fontfamily,20);/*not used now*/
    }

    if (pt_parse->ps_fontsize)
    {
        i4_ret = mss_get_font_size(pt_parse->ps_fontsize,&ui1_fontsize);
        if (ui1_fontsize <  32)ui1_fontsize=32;
        if (i4_ret == MMSBTLR_OK &&
            ui1_fontsize != pt_parse->ui1_fontsize)
        {
            WGL_FONT_INFO_T     t_fnt_info;    
            i4_ret = sbtl_wgl_do_cmd(pt_parse->h_sbtl_wgl,
                                    WGL_CMD_GL_GET_FONT,
                                    WGL_PACK(&t_fnt_info),
                                    NULL);
            t_fnt_info.e_font_size = FE_FNT_SIZE_CUSTOM;
            t_fnt_info.ui1_custom_size = ui1_fontsize;
            
            i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                                   SBTL_WGL_CMD_GL_SET_FONT,
                                   (VOID*)&t_fnt_info,
                                   NULL);
            pt_parse->ui1_fontsize = ui1_fontsize;
            DBG_INFO(("[MSS] Change font size %d\n",ui1_fontsize));
        }
        
    }
    if (pt_parse->ps_color)
    {
        i4_ret =  mss_get_color(pt_parse->ps_color, &t_color);
        if (i4_ret == MMSBTLR_OK &&
            (x_memcmp(&pt_parse->t_color,&t_color,sizeof(GL_COLOR_T)) != 0))
        {
            
            i4_ret = sbtl_wgl_do_cmd(pt_parse->h_sbtl_wgl,
                             WGL_CMD_GL_GET_COLOR,
                             WGL_PACK(WGL_CLR_TEXT),
                             WGL_PACK(&t_clr_info));

            x_memcpy(&t_clr_info.u_color_data,
                     &t_color,
                     sizeof(GL_COLOR_T));
            
            i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_COLOR,
                           (VOID*)SBTL_WGL_CLR_TEXT, /* text color */
                           (VOID*)&t_clr_info);
            pt_parse->t_color = t_color;
            DBG_INFO(("[MSS]Change text color rgba(%d,%d,%d,%d)\n",t_color.u1.r,t_color.u2.g,t_color.u3.b,t_color.a));
        }
    }
    if (pt_parse->ps_bgcolor)
    {
        i4_ret =  mss_get_color(pt_parse->ps_bgcolor, &t_color);
        if (i4_ret == MMSBTLR_OK &&
            (x_memcmp(&pt_parse->t_bgcolor,&t_color,sizeof(GL_COLOR_T)) != 0))
        {
            i4_ret = sbtl_wgl_do_cmd(pt_parse->h_sbtl_wgl,
                             WGL_CMD_GL_GET_COLOR,
                             WGL_PACK(SBTL_WGL_CLR_BK),
                             WGL_PACK(&t_clr_info));

            x_memcpy(&t_clr_info.u_color_data,
                     &t_color,
                     sizeof(GL_COLOR_T));
            
            i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_COLOR,
                           (VOID*)SBTL_WGL_CLR_BK, /* text color */
                           (VOID*)&t_clr_info);
            pt_parse->t_bgcolor = t_color;
            DBG_INFO(("[MSS]Change text bgcolor rgba(%d,%d,%d,%d)\n",t_color.u1.r,t_color.u2.g,t_color.u3.b,t_color.a));
        }
    }
    if (pt_parse->ps_align)
    {
        i4_ret = mss_parse_align(pt_parse->ps_align,&ui1_align);
        if (i4_ret == MMSBTLR_OK && 
            pt_parse->ui1_align != ui1_align)
        {
             i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_TEXT_SET_ALIGN,
                           (VOID*)((UINT32)ui1_align),
                           NULL);
             pt_parse->ui1_align = ui1_align;
             DBG_INFO(("[MSS] Change font align %d\n",ui1_align));
        }
    }
    
}
 
static INT32 free_one_parse(MSS_ONE_PARSE_T* pt_parse)
{
    if (pt_parse)
    {
        if (pt_parse->pt_top)
        {
            x_sbtl_xml_parse_free(pt_parse->pt_top);
        }
        if (pt_parse->h_style)
        {
            x_sbtl_xml_free_hash(pt_parse->h_style);
        }
        if (pt_parse->h_region)
        {
            x_sbtl_xml_free_hash(pt_parse->h_region);
        }
        pt_parse->pt_top = NULL;
        pt_parse->pt_paras = NULL;
        pt_parse->pt_cur = NULL;
        pt_parse->h_style = NULL_HANDLE;
        pt_parse->h_region = NULL_HANDLE;
        reset_all_attr(pt_parse);
        return MMSBTLR_OK;
    }
    return MMSBTLR_INV_ARG;
}
static BOOL do_single_parse(MSS_ONE_PARSE_T* pt_parse,
                                UINT32 ui4_current,
                                UINT32* pui4_begin,
                                UINT32 *pui4_end)
{
    XML_T* pt_span = NULL;
    CHAR*  ps_value = NULL;
    INT32  i4_ret = MMSBTLR_OK;
    UINT32 ui4_begin = 0;
    UINT32 ui4_end = 0;
    while(pt_parse->pt_cur)
    {
        do
        {
            ps_value = x_sbtl_xml_get_attr_value_bytype((INT32)SBTL_XML_MMS_ATTR_BEGIN,pt_parse->pt_cur);
            i4_ret = get_timecode(ps_value,&ui4_begin);
            if (i4_ret != MMSBTLR_OK)break;
            
            ps_value = x_sbtl_xml_get_attr_value_bytype((INT32)SBTL_XML_MMS_ATTR_END,pt_parse->pt_cur);
            i4_ret = get_timecode(ps_value,&ui4_end);
            if (i4_ret != MMSBTLR_OK)break;
            /*find all right one*/
            if (ui4_end > ui4_begin && 
                (ui4_end+pt_parse->ui4_start) > ui4_current &&
                (ui4_end+pt_parse->ui4_start - ui4_current) <= 90*1000)
            {
                *pui4_begin = (ui4_begin+pt_parse->ui4_start);
                *pui4_end   = (ui4_end  +pt_parse->ui4_start);
                pt_span = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_SPAN,pt_parse->pt_cur);
                if (pt_span == NULL)break;
                reset_all_attr(pt_parse);
                pt_parse->ps_show = pt_span->ps_value;
                x_sbtl_xml_iterate_all_attr(pt_parse->pt_cur,
                            mss_iterate_attr,
                            (VOID*)pt_parse,
                            0);
                DBG_INFO(("[MSS]Find(%d,%d) %s\n",ui4_begin,ui4_end,pt_parse->ps_show));
                set_all_sttr(pt_parse);
                pt_parse->pt_cur = x_sbtl_xml_get_next_element((INT32)SBTL_XML_ELM_PARA,pt_parse->pt_cur); 
                return TRUE;
            }
        }while(0);
        pt_parse->pt_cur = x_sbtl_xml_get_next_element((INT32)SBTL_XML_ELM_PARA,pt_parse->pt_cur); 
    }
                
    return FALSE;
}
static INT32 start_one_parse(MSS_ONE_PARSE_T* pt_parse,XML_T* pt_top)
{
    if (pt_parse && pt_top)
    {
        XML_T* pt_parent = NULL;
        XML_T* pt_cur = NULL;
        if (pt_parse->pt_top)free_one_parse(pt_parse);
        pt_parse->pt_top = pt_top;
        pt_parent = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_HEAD,pt_top);
        pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_STYLING,pt_parent);
        if (pt_cur)
        {
             x_sbtl_xml_create_ref_hash(pt_cur,
                                      (INT32)SBTL_XML_ELM_STYLE,
                                      (INT32)SBTL_XML_MMS_ATTR_ID,
                                      &pt_parse->h_style);
        }
        pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_LAYOUT,pt_parent);
        if (pt_cur)
        {
            x_sbtl_xml_create_ref_hash(pt_cur,
                                      (INT32)SBTL_XML_ELM_REGION,
                                      (INT32)SBTL_XML_MMS_ATTR_ID,
                                      &pt_parse->h_region);
        }
        pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_BODY,pt_top);
        pt_parse->pt_paras = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_DIV,pt_cur);
        pt_parse->pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_PARA,pt_parse->pt_paras);
    }
    return MMSBTLR_OK;
}

INT32 xml_mss_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                              PTS_T                     t_pts_current,
                              PTS_T*                    pt_pts_start,
                              PTS_T*                    pt_pts_end,
                              BOOL*                     pb_found)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private;
    MSS_ONE_PARSE_T*                   pt_parse;
    INT32                              i4_ret;
    BOOL                               b_found = FALSE;
    UINT32                             ui4_begin;
    UINT32                             ui4_end;
    UINT32                             ui4_current;
    CHAR*                              ps_value = NULL;
    XML_T*                             pt_top = NULL;
    XML_T*                             pt_cur = NULL;
    UINT32                             ui4_size;
    SBTL_XML_PARSER_INFO_T             t_info;
    
    if (pt_mngr == NULL || pt_mngr->pv_private == NULL)
    {
        
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_parse= &pt_private->t_parse;
    ui4_current = (UINT32)((t_pts_current)*1000/PTS_FREQ);
    *pb_found   = FALSE;

    if (pt_parse->pt_top != NULL)
    {
        /*load next until found*/
        b_found = do_single_parse(pt_parse,ui4_current,&ui4_begin,&ui4_end);
        if (b_found)
        {
            *pt_pts_start = ui4_begin*PTS_FREQ/1000;
            *pt_pts_end   = ui4_end  *PTS_FREQ/1000;
            *pb_found     = TRUE;
            return MMSBTLR_OK;
        }  
    }
    
    if (pt_private->b_need_dmx == FALSE)
    {
       return MMSBTLR_NOT_FOUND;
        
    }
    free_one_parse(pt_parse);
    /*fetch new data from dmx buffer*/
    while (1)
    {
        UINT32          ui4_data_1;
        UINT32          ui4_data_2;
        UINT32          ui4_data_3;
        UINT32          ui4_pts;
        UINT32          ui4_duration;
        PTS_T           t_stc;
        b_found = sbtl_dmx_data_que_query(pt_mngr->pt_engine,
                                          &ui4_data_1,
                                          &ui4_data_2,
                                          &ui4_data_3,
                                          &ui4_pts,
                                          &ui4_duration);
        if (!b_found)
        {
            return MMSBTLR_OK;
        }
        ui4_size = ui4_data_2;
        if (ui4_size < 100 || ui4_size > XML_MSS_MAX_BUF_LEN)
        {
            DBG_ERROR(("[MSS] Dmx send too little or too long data size %d\n",ui4_size));
            if (ui4_data_2 > XML_MSS_MAX_BUF_LEN)
            {
                ui4_size = XML_MSS_MAX_BUF_LEN;
            }
            else
            {
                return MMSBTLR_INV_ARG;
            } 
        }
        /*after retrieve drop directly*/
        sbtl_dmx_retrive_data(pt_mngr->pt_engine,
                              ui4_data_1,
                              ui4_size,
                              ui4_data_3,
                              pt_private->aui1_buf);

        if (mm_sbtl_get_dbg_level() == 7)
        {
            UINT32 ui4_m = 0;
            DBG_LOG_PRINT(("=======================START==================\n"));
            for (ui4_m=0;ui4_m<ui4_size;ui4_m++)
            {
                DBG_LOG_PRINT(("%c",pt_private->aui1_buf[ui4_m]));
            }
            DBG_LOG_PRINT(("=======================END==================\n"));
        }
        sbtl_dmx_data_que_retrieve(pt_mngr->pt_engine,
                                  &ui4_data_1,
                                  &ui4_data_2,
                                  &ui4_data_3,
                                  &ui4_pts,
                                  &ui4_duration,
                                  &t_stc);
        sbtl_dmx_release_data(pt_mngr->pt_engine,
                                 ui4_data_1,
                                 ui4_data_2,
                                 ui4_data_3);
        
        if (h_elm_hash == NULL_HANDLE ||
            h_attr_hash == NULL_HANDLE)
        {
            return MMSBTLR_NOT_INIT;
        }
        t_info.h_ele_hash = h_elm_hash;
        t_info.h_attr_hash = h_attr_hash;
        
        i4_ret = x_sbtl_xml_parse_by_memory((VOID*)pt_private->aui1_buf, 
                                            ui4_size, 
                                            (VOID**)&pt_top,
                                            &t_info);
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_ERROR(("[MSS] Call parse mem fail at %d\n",__LINE__));
            break;
        }
        
        i4_ret = start_one_parse(pt_parse,pt_top);
        pt_cur = x_sbtl_xml_get_tail_element((INT32)SBTL_XML_ELM_PARA,pt_parse->pt_paras);
        ps_value = x_sbtl_xml_get_attr_value_bytype((INT32)SBTL_XML_MMS_ATTR_END,pt_cur);
        i4_ret = get_timecode(ps_value,&ui4_end);
        
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_ERROR(("[MSS] Bad attr last end time!\n"));
        }
        pt_parse->ui4_start = ui4_pts/PTS_FREQ*1000;
        DBG_INFO(("[MSS]Get packet start time %d\n",pt_parse->ui4_start));
        if ((ui4_end+pt_parse->ui4_start) > ui4_current)
        {
            /*try to get start and begin time if not overdue*/
            b_found = do_single_parse(pt_parse,ui4_current,&ui4_begin,&ui4_end);
            
            if (b_found)
            {
                *pt_pts_start = ui4_begin*PTS_FREQ/1000;
                *pt_pts_end   = ui4_end  *PTS_FREQ/1000;
                *pb_found     = TRUE;
                return MMSBTLR_OK;
            }
        }
        DBG_INFO(("[MSS]Discard directly overdue start:%d current %d!!\n",pt_parse->ui4_start,ui4_current));
        free_one_parse(pt_parse);
    }
    
    *pb_found = FALSE;
     return MMSBTLR_NOT_FOUND;
    
}

INT32 xml_mss_load_string(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private;
    MSS_ONE_PARSE_T*                   pt_parse;
    
    if (pt_mngr == NULL || pt_mngr->pv_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_parse = &pt_private->t_parse;
    DBG_INFO(("[MSS] Start Load string %s\n",pt_parse->ps_show));
    
    if (pt_parse->ps_show &&
        valid_str(pt_parse->ps_show))
    {
        x_uc_ps_to_w2s((const CHAR *)pt_parse->ps_show,
                       pt_private->w2s_text,
                       256);
    }
    else
    {
        pt_private->w2s_text[0] = 0;
        pt_private->w2s_text[1] = 0;
    }
    return MMSBTLR_OK;
}

UINT32 xml_mss_timer_period(SBTL_PAGE_INST_MNGR_T*      pt_mngr)
{
    ASSERT(pt_mngr->pt_engine->t_cfg.i4_speed > 0, ("pt_mngr->pt_engine.t_cfg.i4_speed=0\n"));

    return (UINT32)((TIMER_PERIOD * 100)/pt_mngr->pt_engine->t_cfg.i4_speed);
}

INT32 xml_mss_init(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private = NULL;
    MSS_ONE_PARSE_T*                   pt_parse = NULL;
    INT32                              i4_ret = MMSBTLR_OK;
    
    if (pt_mngr == NULL || pt_mngr->pv_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_parse = &pt_private->t_parse;
    
    if (h_elm_hash == NULL_HANDLE)
    {
         x_sbtl_xml_create_hash(t_hash_elm,
                                sizeof(t_hash_elm)/sizeof(SBTL_XML_HASH_INFO_T),
                                &h_elm_hash);
    }
    if (h_attr_hash == NULL_HANDLE)
    {
        x_sbtl_xml_create_hash(t_hash_attr,
                               sizeof(t_hash_attr)/sizeof(SBTL_XML_HASH_INFO_T), 
                               &h_attr_hash);
    }
    
    if (pt_private->b_need_dmx == FALSE)
    {
        /*get file content and start to parse*/
        CHAR*                  s_buf = NULL;
        UINT32                 ui4_size = 0;
        UINT32                 ui4_read = 0; 
        XML_T*                 pt_top = NULL;
        SBTL_XML_PARSER_INFO_T t_info;
        
        sbtl_feeder_init(pt_mngr->pt_engine, MM_SBTL_FEEDER_SUBTITLE);
        ui4_size  = (UINT32)pt_mngr->pt_engine->aui8_file_size[MM_SBTL_FEEDER_SUBTITLE];
        
        if (ui4_size > 100 && ui4_size < 200*1024)
        {
            s_buf = (CHAR*)x_mem_alloc(ui4_size);
            sbtl_feeder_setpos(pt_mngr->pt_engine, 0, MEDIA_SEEK_BGN, MM_SBTL_FEEDER_SUBTITLE);
            sbtl_feeder_inpnbyte(pt_mngr->pt_engine, ui4_size, (UINT8*)s_buf, &ui4_read, MM_SBTL_FEEDER_SUBTITLE);
            if (ui4_read > 0)
            {
               
                if (h_elm_hash  != NULL_HANDLE &&
                    h_attr_hash != NULL_HANDLE)
                {
                    t_info.h_ele_hash = h_elm_hash;
                    t_info.h_attr_hash = h_attr_hash;
                     
                    i4_ret = x_sbtl_xml_parse_by_memory((VOID*)s_buf, 
                                                         ui4_read, 
                                                         (VOID**)&pt_top,
                                                         &t_info);
                    i4_ret = start_one_parse(pt_parse,pt_top);
                    pt_parse->ui4_start = 0;
                }  
            }
            x_mem_free(s_buf);
        } 
        else
        {
            DBG_WARN(("Too small or big file size %d\n", ui4_size));
        }
        if (i4_ret != MMSBTLR_OK)
        {
            return i4_ret;
        }
        
    }
    ui4_ref_cnt++;
    return MMSBTLR_OK;
}

INT32 xml_mss_deinit(SBTL_PAGE_INST_MNGR_T* pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private = NULL;
    MSS_ONE_PARSE_T*                   pt_parse = NULL;
        
    if (pt_mngr == NULL ||pt_mngr->pv_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_parse = &pt_private->t_parse;
    free_one_parse(pt_parse);
    ui4_ref_cnt--; 
    DBG_INFO(("[MSS] Deinit Left client count is %d\n",ui4_ref_cnt));
    if (ui4_ref_cnt == 0)
    {
        if (h_elm_hash != NULL_HANDLE)
        {
            x_sbtl_xml_free_hash(h_elm_hash);
            h_elm_hash = NULL_HANDLE;
        }
        if (h_attr_hash != NULL_HANDLE)
        {
            x_sbtl_xml_free_hash(h_attr_hash);
            h_attr_hash = NULL_HANDLE;
        } 
    }
    return MMSBTLR_OK;
}

VOID xml_mss_start(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private = NULL;
    MSS_ONE_PARSE_T*                   pt_parse = NULL;
    
    if (pt_mngr != NULL && pt_mngr->pv_private != NULL)
    {
        pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
        pt_parse = &pt_private->t_parse;
        if (pt_private->b_need_dmx == FALSE)
        {
            pt_parse->pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_PARA,pt_parse->pt_paras);
        }
        else
        {
            free_one_parse(pt_parse);
            pt_parse->ui4_start = 0;/*reset dmx start time*/
        }
    } 
}

VOID xml_mss_stop(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private = NULL;
    MSS_ONE_PARSE_T*                   pt_parse = NULL;
    
    if (pt_mngr != NULL && pt_mngr->pv_private != NULL)
    {
        pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
        pt_parse = &pt_private->t_parse;
        if (pt_private->b_need_dmx)
        {
           free_one_parse(pt_parse);
        }
        return;
    }
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*  pt_mngr,
                                            PTS_T                   t_pts_current,
                                            PTS_T*                  pt_pts_start,
                                            PTS_T*                  pt_pts_end,
                                            BOOL*                   pb_found)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private;
    INT32                              i4_ret;
    SBTL_XML_MSS_FNC_TBL_T*            pt_fnc_table;

    if (pt_mngr != NULL && pt_mngr->pv_private!= NULL)
    {
        pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
        pt_fnc_table = pt_private->pt_fnc_table;
        if (pt_fnc_table != NULL && pt_fnc_table->pf_seek_time != NULL)
        {
            i4_ret = pt_fnc_table->pf_seek_time(pt_mngr,
                                                t_pts_current,
                                                pt_pts_start,
                                                pt_pts_end,
                                                pb_found);
            return i4_ret;
        }
    }
    return MMSBTLR_INV_ARG;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_load_string(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private;
    INT32                              i4_ret;
    SBTL_XML_MSS_FNC_TBL_T*            pt_fnc_table;

    if (pt_mngr != NULL && pt_mngr->pv_private!= NULL)
    {
        pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
        pt_fnc_table = pt_private->pt_fnc_table;
        if (pt_fnc_table != NULL && pt_fnc_table->pf_load_string != NULL)
        {
            i4_ret = pt_fnc_table->pf_load_string(pt_mngr);
            return i4_ret;
        }
    }
    
    return MMSBTLR_INV_ARG;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static UINT32 _sbtl_xml_mms_timer_period(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private;
    SBTL_XML_MSS_FNC_TBL_T*            pt_fnc_table;
    
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    pt_fnc_table = pt_private->pt_fnc_table;
    
    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    ASSERT(pt_fnc_table->pf_timer_period, ("pt_fnc_table->pf_timer_period=NULL\n"));
    if ((NULL==pt_fnc_table) || (NULL==pt_fnc_table->pf_timer_period))
    {
        return TIMER_PERIOD;
    }

    return pt_fnc_table->pf_timer_period(pt_mngr);
}


/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static VOID _sbtl_timer_cb_fct(HANDLE_T     pt_tm_handle,
                                   VOID*        pv_tag)
{
    SBTL_PAGE_INST_MNGR_T*  pt_mngr;
    MM_SBTL_HDLR_MSG_T      t_msg;
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*   pt_private;
    ASSERT(pv_tag, ("pv_tag=NULL\n"));
    if (NULL==pv_tag)
    {
        return;
    }
    pt_mngr = (SBTL_PAGE_INST_MNGR_T*)pv_tag;
    if (NULL==pt_mngr->pv_private)
    {
        return;
    }
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_private->ui4_timer++;
    mm_sbtl_hdlr_send_timer_msg(t_msg, pt_mngr);
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_timer_start(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*   pt_private;
    INT32                                i4_ret;

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    ASSERT(pt_mngr->pv_private, ("pt_mngr->pv_private=NULL\n"));
    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;

    if (pt_private->h_timer)
    {
        x_timer_stop(pt_private->h_timer);
        pt_private->h_timer = NULL_HANDLE;
    }
    else
    {
        i4_ret = x_timer_create(&pt_private->h_timer);
        if (i4_ret != OSR_OK)
        {
            return MMSBTLR_CORE;
        }
    }

    i4_ret = x_timer_start(pt_private->h_timer,
                           _sbtl_xml_mms_timer_period(pt_mngr),
                           X_TIMER_FLAG_REPEAT,
                           _sbtl_timer_cb_fct,
                           pt_mngr);
    pt_private->ui4_timer = 0;
                    
    if (i4_ret != OSR_OK)
    {
        return MMSBTLR_CORE;
    }
    return MMSBTLR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static VOID _sbtl_xml_mms_timer_stop(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*   pt_private;

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    ASSERT(pt_mngr->pv_private, ("pt_mngr->pv_private=NULL\n"));
    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return;
    }
    
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if (pt_private->h_timer)
    {
        x_timer_delete(pt_private->h_timer);
        pt_private->h_timer = NULL_HANDLE;
    }
}

static INT32 _sbtl_mss_get_wgl_rect(
                       SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                       GL_RECT_T*                pt_sbtl_rect)
{
   INT32                               i4_ret;
   UINT32                              ui4_canvas_w = 0;
   UINT32                              ui4_canvas_h = 0;
   HANDLE_T                            h_canvas;
   GL_POINT_T                          t_osd_ofst;

   if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
   {
       return MMSBTLR_INV_ARG;
   }
   h_canvas = pt_mngr->pt_engine->t_cfg.h_gl_plane;
   if (pt_mngr->pt_engine->t_cfg.b_rect_osd)
   {
       *pt_sbtl_rect = pt_mngr->pt_engine->t_cfg.t_rect_osd;
   }
   else
   {
       i4_ret = sbtl_wgl_get_canvas_size(h_canvas, &ui4_canvas_w, &ui4_canvas_h);
       if (MMSBTLR_OK != i4_ret)
       {
          return (MMSBTLR_WGL_FAIL);
       }
       
       pt_sbtl_rect->i4_left = (INT32)(ui4_canvas_w/6);
       pt_sbtl_rect->i4_top = (INT32)(ui4_canvas_h/10*8);
       pt_sbtl_rect->i4_right = (INT32)ui4_canvas_w-(INT32)(ui4_canvas_w/6);
       pt_sbtl_rect->i4_bottom = (INT32)(ui4_canvas_h)-(INT32)(ui4_canvas_h/10);
   }    
   
   t_osd_ofst = pt_mngr->pt_engine->t_cfg.t_pos_adjust;
   
   if(t_osd_ofst.i4_x !=0 || t_osd_ofst.i4_y !=0)
   {            
       pt_sbtl_rect->i4_left += t_osd_ofst.i4_x;
       pt_sbtl_rect->i4_top += t_osd_ofst.i4_y;
       pt_sbtl_rect->i4_right += t_osd_ofst.i4_x;
       pt_sbtl_rect->i4_bottom += t_osd_ofst.i4_y;

       if(pt_sbtl_rect->i4_left < 0)
       {
           pt_sbtl_rect->i4_right -= pt_sbtl_rect->i4_left;
           pt_sbtl_rect->i4_left = 0;
       }

       if(pt_sbtl_rect->i4_top < 0)
       {
           pt_sbtl_rect->i4_bottom -= pt_sbtl_rect->i4_top;
           pt_sbtl_rect->i4_top = 0;
       }
       
       if(pt_sbtl_rect->i4_right > (INT32)ui4_canvas_w)
       {
           pt_sbtl_rect->i4_left -= (pt_sbtl_rect->i4_right - (INT32)ui4_canvas_w);
           pt_sbtl_rect->i4_right = (INT32)ui4_canvas_w;
       }

       if(pt_sbtl_rect->i4_bottom > (INT32)ui4_canvas_h)
       {
           pt_sbtl_rect->i4_top -= (pt_sbtl_rect->i4_bottom - (INT32)ui4_canvas_h);
           pt_sbtl_rect->i4_bottom = (INT32)ui4_canvas_h;
       }
   }
   return MMSBTLR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_start(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    INT32                                   i4_ret;
    GL_RECT_T                               t_sbtl_rc_scrn = {0,0,960,540};
    HANDLE_T                                h_canvas;
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*       pt_private;
    MSS_ONE_PARSE_T*                        pt_parse;
    SBTL_XML_MSS_FNC_TBL_T*                 pt_fnc_table;
    

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    ASSERT(pt_mngr->pv_private, ("pt_mngr->pv_private=NULL\n"));
    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_fnc_table = pt_private->pt_fnc_table;
    pt_parse = &pt_private->t_parse;

    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    ASSERT(pt_fnc_table->pf_start, ("pt_fnc_table->pf_start=NULL\n"));
    if ((NULL==pt_fnc_table) || (NULL==pt_fnc_table->pf_start))
    {
        return MMSBTLR_INV_ARG;
    }

    if (pt_parse->h_sbtl_wgl)
        return MMSBTLR_EXIST;
    h_canvas = pt_mngr->pt_engine->t_cfg.h_gl_plane;
    i4_ret = _sbtl_mss_get_wgl_rect(pt_mngr,&t_sbtl_rc_scrn);
    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }

    i4_ret = sbtl_wgl_create_widget(h_canvas,
                                    SBTL_WGL_WIDGET_TEXT,
                                    SBTL_WGL_BORDER_NULL,
                                    &t_sbtl_rc_scrn,
                                    255,
                                    (VOID*) (SBTL_WGL_STL_TEXT_MAX_DIS_40_LINE
                                            |SBTL_WGL_STL_TEXT_SMART_CUT
                                            |SBTL_WGL_STL_TEXT_SMART_CUT_OVER_MAX
                                            |SBTL_WGL_STL_TEXT_MULTILINE 
                                            |SBTL_WGL_STL_GL_NO_IMG_UI
                                            |SBTL_WGL_STL_TEXT_MAX_2048),
                                    NULL,
                                    &(pt_parse->h_sbtl_wgl));
    
    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }

    i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_FONT,
                           (VOID*)&pt_mngr->pt_engine->t_cfg.t_font_info,
                           NULL);
    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }

    i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_TEXT_SET_ALIGN,
                           (VOID*)((UINT32)pt_mngr->pt_engine->t_cfg.ui1_text_alignment),
                           NULL);
    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }
    pt_parse->ui1_fontsize = 20;/*set default*/
    pt_parse->ui1_align    = pt_mngr->pt_engine->t_cfg.ui1_text_alignment;

    i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_COLOR,
                           (VOID*)SBTL_WGL_CLR_BK, /* background color */
                           (VOID*)&pt_mngr->pt_engine->t_cfg.t_bg_color_info);
    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }

    i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_COLOR,
                           (VOID*)SBTL_WGL_CLR_TEXT, /* text color */
                           (VOID*)&pt_mngr->pt_engine->t_cfg.t_fg_color_info);

    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }
    x_memcpy(&pt_parse->t_bgcolor,
             &pt_mngr->pt_engine->t_cfg.t_bg_color_info.u_color_data,
              sizeof(GL_COLOR_T));
    x_memcpy(&pt_parse->t_color,
             &pt_mngr->pt_engine->t_cfg.t_fg_color_info.u_color_data,
              sizeof(GL_COLOR_T));

    /*i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_COLOR,
                           (VOID*)SBTL_WGL_CLR_TEXT_BG, 
                           (VOID*)&pt_mngr->pt_engine->t_cfg.t_font_bg_color);    
    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }*/
	
    /*if(pt_mngr->pt_engine->t_cfg.ui4_cmd_mask | MM_SBTL_CMD_MSK_EDGE_COLOR)
    {
	    i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
	                             SBTL_WGL_CMD_GL_SET_COLOR,
                                 (VOID*)SBTL_WGL_CLR_EDGE, 
                                 (VOID*)&(pt_mngr->pt_engine->t_cfg.t_edge_color_info));
	                
	    if (MMSBTLR_OK != i4_ret)
	    {
	       return (MMSBTLR_WGL_FAIL);
   	    }
    }*/

    i4_ret = sbtl_wgl_show(pt_parse->h_sbtl_wgl, FALSE);

    if (MMSBTLR_OK != i4_ret)
    {
       return (MMSBTLR_WGL_FAIL);
    }

    SBTL_SET_BASE_FONT_STYLE(pt_mngr->pt_engine,pt_mngr->pt_engine->t_cfg.t_font_info.e_font_style);
    
    pt_mngr->b_started = TRUE;
  
    _sbtl_xml_mms_timer_start(pt_mngr);
    pt_fnc_table->pf_start(pt_mngr);
    DBG_INFO(("[MSS] started !!\n"));
    return MMSBTLR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_stop(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*      pt_private;
    SBTL_XML_MSS_FNC_TBL_T*                pt_fnc_table;

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    ASSERT(pt_mngr->pv_private, ("pt_mngr->pv_private=NULL\n"));
    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    MSS_ONE_PARSE_T* pt_parse = &pt_private->t_parse;
    
    pt_fnc_table = pt_private->pt_fnc_table;

    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    ASSERT(pt_fnc_table->pf_stop, ("pt_fnc_table->pf_stop=NULL\n"));
    if ((NULL==pt_fnc_table) || (NULL==pt_fnc_table->pf_stop))
    {
        return MMSBTLR_INV_ARG;
    }
    
    _sbtl_xml_mms_timer_stop(pt_mngr);

    pt_fnc_table->pf_stop(pt_mngr);

    _sbtl_xml_mms_remove(pt_mngr);

    if (pt_parse->h_sbtl_wgl)
    {
        sbtl_wgl_destroy_widget(pt_parse->h_sbtl_wgl);
        pt_parse->h_sbtl_wgl = NULL_HANDLE;
    }

    pt_mngr->b_started = FALSE;
    DBG_INFO(("[MSS] stopped !!\n"));
    return MMSBTLR_OK;
}

static INT32 _sbtl_xml_mms_need_dmx(SBTL_PAGE_INST_MNGR_T*      pt_mngr, 
                                     BOOL*                       pb_need)
{
    *pb_need = TRUE;
    return MMSBTLR_OK;
}

static INT32 _sbtl_xml_mms_ext_need_dmx(SBTL_PAGE_INST_MNGR_T*      pt_mngr, 
                                     BOOL*                       pb_need)
{
    *pb_need = FALSE;
    return MMSBTLR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_handle_timing(SBTL_PAGE_INST_MNGR_T*     pt_mngr,
                                          PTS_T                      t_pts)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*      pt_private;
    INT32                               i4_ret;
    MSS_ONE_PARSE_T* pt_parse;

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    ASSERT(pt_mngr->pv_private, ("pt_mngr->pv_private=NULL\n"));
    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_parse = &pt_private->t_parse;

    if (pt_private->b_page_exist)
    {
        if (t_pts < pt_private->t_pts_s)
        {
            pt_private->e_state = XML_MSS_STATE_DATA_RDY;
            return MMSBTLR_OK;
        }
        else if (t_pts >= pt_private->t_pts_s && t_pts <= pt_private->t_pts_e)
        {
            if (pt_private->e_state != XML_MSS_STATE_DISPLAY)
            {
                pt_private->e_state = XML_MSS_STATE_DISPLAY;
                /*display*/
                if(pt_mngr->pt_engine->t_cfg.b_hide == FALSE)
                {
                    i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                                              SBTL_WGL_CMD_TEXT_SET_TEXT,
                                              (VOID*)pt_private->w2s_text,
                                              (VOID*)x_uc_w2s_strlen (pt_private->w2s_text));
                    if (i4_ret != MMSBTLR_OK)
                    {
                        ASSERT(0, ("set text error!!\n"));
                    }
                    
                    i4_ret = sbtl_wgl_show(pt_parse->h_sbtl_wgl, TRUE);
                    
                    if (MMSBTLR_OK != i4_ret)
                    {
                       ASSERT(0, ("show text error!!\n"));
                    }
                }
            }
            return MMSBTLR_OK;
        }
        else
        {
            _sbtl_xml_mms_remove(pt_mngr);
            _sbtl_xml_mms_new_page(pt_mngr);
        }
    }
    else
    {
        _sbtl_xml_mms_new_page(pt_mngr);
    }
    return MMSBTLR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_config(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                                        VOID*                    pv_get_info,
                                   UINT32                   ui4_set_type)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*   pt_private;
    INT32                                i4_ret = MMSBTLR_OK;
    BOOL                                 b_flag;
    PTS_T                                t_pts_current;

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    ASSERT(pt_mngr->pv_private, ("pt_mngr->pv_private=NULL\n"));
    
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    MSS_ONE_PARSE_T* pt_parse = &pt_private->t_parse;

    switch (ui4_set_type)
    {
    case EVN_MM_SBTL_SET_SHOW_HIDE:
        
        b_flag = *(BOOL*)pv_get_info;
        
        i4_ret = sbtl_stc_pts(pt_mngr->pt_engine,
                               &t_pts_current);

        if (i4_ret != MMSBTLR_OK)
        {
            ASSERT(0, ("Get STC Error!!\n"));
            break;
        }
                
        if(b_flag == TRUE)
        {
            pt_mngr->pt_engine->t_cfg.b_hide = FALSE;         
        }
        else
        {
            pt_mngr->pt_engine->t_cfg.b_hide = TRUE;
            if (pt_private->e_state == XML_MSS_STATE_DISPLAY)
            {        
                i4_ret = sbtl_wgl_show(pt_parse->h_sbtl_wgl, FALSE);
            
                if (MMSBTLR_OK != i4_ret)
                {
                    return (MMSBTLR_WGL_FAIL);
                }            
            }               
        }
        
        break;

    default:
        break;
    }

   
    return MMSBTLR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static VOID _sbtl_xml_mms_remove(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*    pt_private;
    INT32                                i4_ret;
    
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    MSS_ONE_PARSE_T* pt_parse = &pt_private->t_parse;

    if (!pt_private->b_page_exist)
    {
        return;
    }
    
    if (pt_private->e_state == XML_MSS_STATE_DISPLAY)
    {
        i4_ret = sbtl_wgl_show(pt_parse->h_sbtl_wgl, FALSE);
            
        if (MMSBTLR_OK != i4_ret)
        {
            ASSERT(0, ("clear widget fail!!\n"));
        }
    }

    pt_private->e_state = XML_MSS_STATE_NONE;
    pt_private->b_page_exist = FALSE;
    
    return;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static INT32 _sbtl_xml_mms_new_page(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*   pt_private;
    BOOL                                b_found = FALSE;
    PTS_T                               t_pts_current = 0;
    INT32                               i4_ret;
    PTS_T                               t_pts_start;
    PTS_T                               t_pts_end;
    MSS_ONE_PARSE_T*                    pt_parse;

    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    ASSERT(pt_mngr->pv_private, ("pt_mngr->pv_private=NULL\n"));
    ASSERT(pt_mngr->pt_engine, ("pt_mngr->pt_engine=NULL\n"));
    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private) || (NULL==pt_mngr->pt_engine))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_parse = &pt_private->t_parse;

    if (!pt_mngr->b_started)
    {
        return MMSBTLR_OK;
    }
    
    if (pt_private->ui4_timer <= 1)/*workaround waiting*/
    {
        return MMSBTLR_OK;
    }

    if (pt_private->b_page_exist)
    {
        return MMSBTLR_OK;
    }
    i4_ret = sbtl_stc_pts(pt_mngr->pt_engine, &t_pts_current);

    if (i4_ret != MMSBTLR_OK)
    {
	    DBG_ERROR(("Get STC Error!!\n"));
    }
    {
        UINT32 ui4_cur = t_pts_current*1000/PTS_FREQ;
        DBG_INFO(("[MSS] Searching time %d\n",ui4_cur));
        UNUSED(ui4_cur);
    }
    _sbtl_xml_mms_seek_time_stamp(pt_mngr,
                                  t_pts_current,
                                  &t_pts_start,
                                  &t_pts_end,
                                  &b_found);

    if (b_found)
    {
        pt_private->t_pts_s = t_pts_start;
        pt_private->t_pts_e = t_pts_end;

        /*Clear the tmp font style information.*/
        SBTL_SET_FONT_STYLE_FLAG(pt_mngr->pt_engine, FALSE);
        SBTL_SET_FONT_STYLE(pt_mngr->pt_engine, 0);

        if (t_pts_current < t_pts_start)
        {
            /*load str string*/
            i4_ret = _sbtl_xml_mms_load_string(pt_mngr);
            if (i4_ret == MMSBTLR_OK)
            {
                if (x_uc_w2s_strlen (pt_private->w2s_text))
                {
                    pt_private->e_state = XML_MSS_STATE_DATA_RDY;
                    pt_private->b_page_exist = TRUE;
                }
            }
        }
        else if (t_pts_current >= t_pts_start && t_pts_current <= t_pts_end)
        {
            /*display*/
            if (XML_MSS_STATE_DATA_RDY == pt_private->e_state)
            {
            }
            else
            {
                i4_ret = _sbtl_xml_mms_load_string(pt_mngr);
                if (i4_ret != MMSBTLR_OK)
                {
                    return MMSBTLR_OK;
                }
            }

            if (0 == x_uc_w2s_strlen (pt_private->w2s_text))
            {
                pt_private->e_state = XML_MSS_STATE_NONE;
                pt_private->b_page_exist = FALSE;
            }

            pt_private->e_state = XML_MSS_STATE_DISPLAY;
            i4_ret = sbtl_wgl_do_cmd (pt_parse->h_sbtl_wgl,
                                      SBTL_WGL_CMD_TEXT_SET_TEXT,
                                      (VOID*)pt_private->w2s_text,
                                      (VOID*)x_uc_w2s_strlen (pt_private->w2s_text));

            if (i4_ret != MMSBTLR_OK)
            {
                ASSERT(0, ("set text error!!\n"));
            }
            
            if(pt_mngr->pt_engine->t_cfg.b_hide == FALSE)
            {
                i4_ret = sbtl_wgl_show(pt_parse->h_sbtl_wgl, TRUE);
                
                if (MMSBTLR_OK != i4_ret)
                {
                   ASSERT(0, ("show text error!!\n"));
                }
            }

            pt_private->b_page_exist = TRUE;
        }
        else
        {
            pt_private->e_state = XML_MSS_STATE_NONE;
            return MMSBTLR_OK;
        }
    }

    return MMSBTLR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
SBTL_PAGE_INST_MNGR_T* _text_page_mngr_constructor (SBTL_ENGINE_T*    pt_engine)
{
    SBTL_PAGE_INST_MNGR_T*                  pt_mngr;
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*       pt_private;
    UINT8                                   ui1_idx;
    BOOL                                    b_found;
    INT32                                   i4_ret;

    ASSERT(pt_engine, ("_text_page_mngr_constructor pt_engine=NULL\n"));
    if (NULL==pt_engine)
    {
        return NULL;
    }

    do
    {
        pt_mngr = (SBTL_PAGE_INST_MNGR_T*)x_mem_alloc(sizeof(SBTL_PAGE_INST_MNGR_T));
        if (NULL == pt_mngr)
        {
            break;
        }
        x_memset(pt_mngr, 0, sizeof(SBTL_PAGE_INST_MNGR_T));

        pt_mngr->pf_start = _sbtl_xml_mms_start;
        pt_mngr->pf_stop = _sbtl_xml_mms_stop;
        pt_mngr->pf_cfg = _sbtl_xml_mms_config;
        pt_mngr->pf_handle_timing = _sbtl_xml_mms_handle_timing;
        pt_mngr->pf_new_page = _sbtl_xml_mms_new_page;
        
        pt_mngr->pt_engine = pt_engine;
        
        pt_mngr->pv_private = x_mem_alloc(sizeof(SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T));

        if (NULL == pt_mngr->pv_private)
        {
            break;
        }
        x_memset(pt_mngr->pv_private, 0, sizeof(SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T));

        pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
        
        b_found = FALSE;
        for(ui1_idx=0;ui1_idx < (UINT8)(sizeof(at_fnc_tbl_list)/sizeof(at_fnc_tbl_list[0])) ; ui1_idx++)
        {
            if(at_fnc_tbl_list[ui1_idx].e_sbtl_text_type == pt_engine->t_cfg.e_sbtl_type)
            {
                if (pt_engine->t_cfg.e_sbtl_type == MM_SBTL_TYPE_XML_MSS_EXT)
                {
                    pt_private->b_need_dmx = FALSE;
                    pt_mngr->pf_need_dmx = _sbtl_xml_mms_ext_need_dmx;
                }
                else
                {
                    pt_private->b_need_dmx = TRUE;
                    pt_mngr->pf_need_dmx = _sbtl_xml_mms_need_dmx;
                }
                pt_private->pt_fnc_table = &at_fnc_tbl_list[ui1_idx];
                b_found = TRUE;
            }
        }
        
        if (!b_found)
        {
            break;
        }
        pt_engine->t_cfg.e_encoding = MM_SBTL_ENCODE_UTF8;
        
        i4_ret = pt_private->pt_fnc_table->pf_init(pt_mngr);
        DBG_INFO(("[MSS] Call init function ret %d\n",i4_ret));
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
        
        return pt_mngr;
    }while(0);

    if (pt_mngr)
    {
        if (pt_mngr->pv_private)
        {
            x_mem_free(pt_mngr->pv_private);
            pt_mngr->pv_private = NULL;
        }
        x_mem_free(pt_mngr);
        pt_mngr = NULL;
    }
    return NULL;
}

/*-----------------------------------------------------------------------------
 * Name: 
 *
 * Description: 
 *
 * Inputs:  pt_this     Pointer points to the 
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static VOID _text_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*  pt_private;

    ASSERT(pt_mngr, ("_text_page_mngr_destructor pt_mngr=NULL\n"));
    if (NULL==pt_mngr)
    {
        return;
    }
    
    pt_private = (SBTL_XML_MSS_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ASSERT(pt_private, ("_text_page_mngr_destructor pt_private=NULL\n"));
    if (NULL==pt_private)
    {
        return;
    }

    if (pt_private->pt_fnc_table)
    {
        if (pt_private->pt_fnc_table->pf_stop)
        {
            pt_private->pt_fnc_table->pf_stop(pt_mngr);
        }
        if (pt_private->pt_fnc_table->pf_deinit)
        {
            pt_private->pt_fnc_table->pf_deinit(pt_mngr);
        }
    }
    
    x_mem_free(pt_private);
    pt_private = NULL;
    pt_mngr->pt_engine->pt_page_mngr = NULL;
    x_mem_free(pt_mngr);
    pt_mngr = NULL;
}

/*-----------------------------------------------------------------------------
                    External functions implementations
 ----------------------------------------------------------------------------*/
extern VOID x_sbtl_xml_mss_init(VOID)
{
    sbtl_register(MM_SBTL_TYPE_XML_MSS,
                  &t_text_fnct_tbl);
    sbtl_register(MM_SBTL_TYPE_XML_MSS_EXT,
                  &t_text_fnct_tbl);
}
#endif /*__KERNEL__*/

