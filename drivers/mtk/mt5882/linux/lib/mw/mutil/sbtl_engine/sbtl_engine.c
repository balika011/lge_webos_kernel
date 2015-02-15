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
 * $RCSfile: sbtl_engine.c,v $
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
#include <stdio.h>
#ifdef _WIN32_
#include "x_os.h"
#else
#include "x_os.h"
#endif
#include "x_common.h"
#include "inc/common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"

#ifndef __NO_FM__
#include "u_fm.h"
#include "x_fm.h"
#endif

#ifdef INET_SUPPORT
#ifdef USE_SMART_FILE
#include "x_smart_file.h"
#else
#include "x_httpc_api.h"
#endif
#endif
#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_widget.h"

#include "mutil/sbtl_engine/mm_sbtl_cli.h"
#include "x_mm_sbtl_engine.h"

#else /*__KERNEL__*/
#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"

#include "sbtl_dbg.h"
#include "sbtl_struct.h"
#include "sbtl_utils.h"
#include "sbtl_register.h"
#include "sbtl_state.h"
#include "sbtl_widget.h"

#include "mm_sbtl_cli.h"
#include "x_mm_sbtl_engine.h"

#endif /*__KERNEL__*/


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define SM_MM_SBTL_STRM_HDLR_NAME      "sbtl_mm_strm_hdlr"
#ifdef MW_IPCC_SUPPORT
#define SM_MM_SBTL_STRM_PARSER_HDLR_NAME     "sbtl_mm_strm_parser_hdlr"
#endif

#define MAX_MM_SBTL_REQUEST         30
#define MM_SBTL_THREAD_PRIORITY     ((UINT8)   100)
#define MM_SBTL_THREAD_STACK_SIZE   ((SIZE_T) 6144)

/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/
static BOOL             b_inited       = FALSE;
static HANDLE_T         h_sbtl_thread = NULL_HANDLE;
static HANDLE_T         h_msg_que = NULL_HANDLE;
#ifdef MW_IPCC_SUPPORT
static HANDLE_T         h_sbtl_parser_thread = NULL_HANDLE;
static HANDLE_T         h_msg_parser_que = NULL_HANDLE;
#endif

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
static VOID _mm_sbtl_thread(VOID*   pv_data);
#ifdef MW_IPCC_SUPPORT
static VOID _mm_sbtl_parser_thread(VOID*   pv_data);
#endif


/*-----------------------------------------------------------------------------
                    Internal functions implementations
 ----------------------------------------------------------------------------*/
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
static INT32 _mm_sbtl_hdlr_wgt_app_callback(
    UINT32                  ui4_msg,
    VOID*                   pv_param1,
    VOID*                   pv_param2)
{
    return 0;
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
static VOID _sbtl_engine_do_state_msg_hdlr(SBTL_ENGINE_T*      pt_engine,
                                           UINT32              ui4_event,
                                           UINT32              ui4_data)
{
    sbtl_state_on_event(&pt_engine->t_state,
                        ui4_event,
                        (VOID*) pt_engine,
                        (VOID*) ui4_data,
                        NULL);
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
VOID _sbtl_engine_dmx_msg_hdlr(SBTL_ENGINE_T*           pt_engine,
                               DEMUX_COND_T             e_nfy_cond,
                               UINT32                   ui4_data_1,
                               UINT32                   ui4_data_2,
                               UINT32                   ui4_data_3,
                               UINT32                   ui4_pts,
                               UINT32                   ui4_duration,
                               PTS_T                    t_stc)
{
    INT32   i4_ret;
    
    if (NULL == pt_engine)
    {
        ASSERT(0, ("_sbtl_engine_pts_msg_hdlr pt_engine=NULL\n"));
        return;
    }

    if (NULL == pt_engine->pt_page_mngr)
    {
        ASSERT(0, ("_sbtl_engine_pts_msg_hdlr pt_engine->pt_page_mngr=NULL\n"));
        return;
    }

    if (NULL == pt_engine->pt_page_mngr->pf_new_page)
    {
        ASSERT(0, ("_sbtl_engine_pts_msg_hdlr pt_engine->pt_page_mngr->pf_handle_timing=NULL\n"));
        return;
    }

    /* append to queue */
    i4_ret = sbtl_dmx_data_que_append(pt_engine, 
                                      ui4_data_1, 
                                      ui4_data_2, 
                                      ui4_data_3, 
                                      ui4_pts, 
                                      ui4_duration,
                                      t_stc);
    if (i4_ret != MMSBTLR_OK)
    {
        ASSERT(0, ("_sbtl_engine_dmx_msg_hdlr sbtl_dmx_data_que_append error\n"));
        return;
    }

    //pt_engine->pt_page_mngr->pf_new_page(pt_engine->pt_page_mngr);
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
VOID _sbtl_engine_pts_msg_hdlr(SBTL_ENGINE_T*           pt_engine,
                               PTS_T                    t_pts,
                               BOOL                     b_cycle)
{
    if (NULL == pt_engine)
    {
        ASSERT(0, ("_sbtl_engine_pts_msg_hdlr pt_engine=NULL\n"));
        return;
    }

    if (NULL == pt_engine->pt_page_mngr)
    {
        ASSERT(0, ("_sbtl_engine_pts_msg_hdlr pt_engine->pt_page_mngr=NULL\n"));
        return;
    }

    if (NULL == pt_engine->pt_page_mngr->pf_handle_timing)
    {
        ASSERT(0, ("_sbtl_engine_pts_msg_hdlr pt_engine->pt_page_mngr->pf_handle_timing=NULL\n"));
        return;
    }

    pt_engine->pt_page_mngr->pf_handle_timing(pt_engine->pt_page_mngr,
                                              t_pts);
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
VOID _sbtl_engine_timer_msg_hdlr(SBTL_ENGINE_T*         pt_engine)
{
    INT32   i4_ret;
    PTS_T   t_pts_current = 0;

    if (NULL == pt_engine)
    {
       /*ASSERT(0, ("_sbtl_engine_timer_msg_hdlr pt_engine=NULL\n"));*/
        return;
    }

    if (NULL == pt_engine->pt_page_mngr)
    {
       /* ASSERT(0, ("_sbtl_engine_timer_msg_hdlr pt_engine->pt_page_mngr=NULL\n"));*/
        return;
    }

    if (NULL == pt_engine->pt_page_mngr->pf_handle_timing)
    {
        /*ASSERT(0, ("_sbtl_engine_timer_msg_hdlr pt_engine->pt_page_mngr->pf_handle_timing=NULL\n"));*/
        return;
    }
    
    i4_ret = sbtl_stc_pts(pt_engine, &t_pts_current);

    if (MMSBTLR_OK == i4_ret)
    {
        pt_engine->pt_page_mngr->pf_handle_timing(pt_engine->pt_page_mngr,
                                                  t_pts_current);
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
BOOL _sbtl_engine_is_text_subtitle(SBTL_ENGINE_T* pt_engine)
{
    if (pt_engine)
    {   switch(pt_engine->t_cfg.e_sbtl_type)
        {
            case MM_SBTL_TYPE_MKV_SRT:
            case MM_SBTL_TYPE_MKV_SSA:
            case MM_SBTL_TYPE_TEXT:
            case MM_SBTL_TYPE_TEXT_SRT:
            case MM_SBTL_TYPE_TEXT_SAMI:
            case MM_SBTL_TYPE_TEXT_SVSUB:
            case MM_SBTL_TYPE_TEXT_MPLAYER2:
            case MM_SBTL_TYPE_TEXT_SSA:
            case MM_SBTL_TYPE_TEXT_MDVDSUB:
            case MM_SBTL_TYPE_TEXT_TXT:
            case MM_SBTL_TYPE_TEXT_POWERDIVX:
            case MM_SBTL_TYPE_TEXT_DVDSS:
            case MM_SBTL_TYPE_LYRIC_LRC:
            case MM_SBTL_TYPE_LYRIC_KRK_KING:
            case MM_SBTL_TYPE_LYRIC_MP3_KRK:
                return TRUE;
                //break;
            default:
                break;
        }
    }
    return FALSE;
}
static VOID _sbtl_engine_config_msg_hdlr(SBTL_ENGINE_T*            pt_engine,
                                         MM_SBTL_HDLR_MSG_T*       pt_msg)
{
    UINT32  ui4_data;
    INT32              i4_ret = MMSBTLR_OK;
    MM_SBTL_ENCODING_T e_encoding;//for auto encoding memorize
    UINT32             ui4_mem_type = MM_SBTL_INF_MSK_NONE;
    VOID*              pv_mem       = NULL;
    
    if (pt_engine == NULL ||
        pt_msg == NULL)
    {
        return;
    }
   
    if (pt_msg->u.t_msg_config_event.pv_set_info == NULL)
    {
        if (pt_msg->u.t_msg_config_event.e_set_type != EVN_MM_SBTL_SET_TYPE_PARSE_DONE)
        {
            return;
        }
    }
    
    switch (pt_msg->u.t_msg_config_event.e_set_type)
    {
    case EVN_MM_SBTL_SET_VIDEO_RECT:
        x_memcpy(&pt_engine->t_cfg.t_rect_video, 
                 pt_msg->u.t_msg_config_event.pv_set_info, 
                 sizeof(GL_RECT_T));
        break;
        
    case EVN_MM_SBTL_SET_VIDEO_OVERSCAN:
        ui4_data = *((UINT32*)pt_msg->u.t_msg_config_event.pv_set_info);
        
        pt_engine->t_cfg.ui1_ovs_left =    (UINT8)((ui4_data & 0xFF000000)>>24);
        pt_engine->t_cfg.ui1_ovs_top =     (UINT8)((ui4_data & 0x00FF0000)>>16);
        pt_engine->t_cfg.ui1_ovs_right =   (UINT8)((ui4_data & 0x0000FF00)>>8);
        pt_engine->t_cfg.ui1_ovs_bottom =  (UINT8)(ui4_data & 0x000000FF);

        break;
        
    case EVN_MM_SBTL_SET_OSD_RECT:
        x_memcpy(&pt_engine->t_cfg.t_rect_osd, 
                 pt_msg->u.t_msg_config_event.pv_set_info, 
                 sizeof(GL_RECT_T));
		pt_engine->t_cfg.b_rect_osd = TRUE;
        break;
        
    case EVN_MM_SBTL_SET_OSD_INSECT:
        x_memcpy(&pt_engine->t_cfg.t_inset, 
                 pt_msg->u.t_msg_config_event.pv_set_info, 
                 sizeof(WGL_INSET_T));
        break;
        
    case EVN_MM_SBTL_SET_OSD_OFFSET:
        ui4_mem_type = MM_SBTL_INF_MSK_OSD_OFST;
        x_memcpy(&pt_engine->t_cfg.t_pos_adjust, 
                 pt_msg->u.t_msg_config_event.pv_set_info, 
                 sizeof(GL_POINT_T));
        pv_mem = &pt_engine->t_cfg.t_pos_adjust;
        break;
        
    case EVN_MM_SBTL_SET_OSD_FG_COLOR:
          if(_sbtl_engine_is_text_subtitle(pt_engine))
          {
           ui4_mem_type = MM_SBTL_INF_MSK_FG_COLOR;
           x_memcpy(&pt_engine->t_cfg.t_fg_color_info, 
                 pt_msg->u.t_msg_config_event.pv_set_info, 
                 sizeof(WGL_COLOR_INFO_T));
           pv_mem = &pt_engine->t_cfg.t_fg_color_info;
          }
       break;
        
    case EVN_MM_SBTL_SET_OSD_BG_COLOR:
        if(_sbtl_engine_is_text_subtitle(pt_engine))
           {
           x_memcpy(&pt_engine->t_cfg.t_bg_color_info, 
                pt_msg->u.t_msg_config_event.pv_set_info, 
                sizeof(WGL_COLOR_INFO_T));
        }
       break;
        
    case EVN_MM_SBTL_SET_OSD_FONT_EDGE_COLOR:
        if (_sbtl_engine_is_text_subtitle(pt_engine))
         {
            ui4_mem_type = MM_SBTL_INF_MSK_EDGE_COLOR;
          x_memcpy(&pt_engine->t_cfg.t_edge_color_info, 
                 pt_msg->u.t_msg_config_event.pv_set_info, 
                 sizeof(WGL_COLOR_INFO_T));
          pv_mem = &pt_engine->t_cfg.t_edge_color_info;
        }
        break;
        
    case EVN_MM_SBTL_SET_OSD_FONT_INFO:
          if(_sbtl_engine_is_text_subtitle(pt_engine))
          {
                  ui4_mem_type = MM_SBTL_INF_MSK_FONT_INF;
                x_memcpy(&pt_engine->t_cfg.t_font_info, 
                         pt_msg->u.t_msg_config_event.pv_set_info, 
                         sizeof(WGL_FONT_INFO_T));
                
                /*The font libarary only support unicode format,so it will transfer unicode format before using font.*/
                if (pt_engine->t_cfg.t_font_info.e_font_cmap == FE_CMAP_ENC_GB2312)
                {
                    pt_engine->t_cfg.t_font_info.e_font_cmap = FE_CMAP_ENC_UNICODE;
                }
                else if (pt_engine->t_cfg.t_font_info.e_font_cmap == FE_CMAP_ENC_BIG5)
                {
                    pt_engine->t_cfg.t_font_info.e_font_cmap = FE_CMAP_ENC_UNICODE;
                }
                pv_mem = &pt_engine->t_cfg.t_font_info;
        }     
        break;
        
    case EVN_MM_SBTL_SET_OSD_TEXT_ALIGNMENT:
        pt_engine->t_cfg.ui1_text_alignment = *((UINT8*)pt_msg->u.t_msg_config_event.pv_set_info);
        break;
        
    case EVN_MM_SBTL_SET_DIVX_MENU_ITEM:
        pt_engine->t_cfg.ui2_menu_item = *((UINT16*)pt_msg->u.t_msg_config_event.pv_set_info);
        break;
        
    case EVN_MM_SBTL_SET_SPEED:
        pt_engine->t_cfg.i4_speed = (INT32)*((UINT32*)pt_msg->u.t_msg_config_event.pv_set_info);
        break;
        
    case EVN_MM_SBTL_SET_STREAM_INDEX:
        pt_engine->t_cfg.ui4_stream_id = (UINT32)*((UINT32*)pt_msg->u.t_msg_config_event.pv_set_info);
        break;
        
    case EVN_MM_SBTL_SET_ENCODING:
         if(_sbtl_engine_is_text_subtitle(pt_engine))
         {
             ui4_mem_type = MM_SBTL_INF_MSK_ENCODING;
            e_encoding = (MM_SBTL_ENCODING_T)*((MM_SBTL_ENCODING_T*)pt_msg->u.t_msg_config_event.pv_set_info);
            pt_engine->t_cfg.e_encoding = e_encoding;
            pv_mem = &e_encoding;
         }
        break;
        
    case EVN_MM_SBTL_SET_LANGUAGE:
        break;
        
    case EVN_MM_SBTL_SET_ADJUST_TIME:
        //pt_engine->t_cfg.t_time_ofst = *(MM_SBTL_TM_OFST_INF*)pt_msg->u.t_msg_config_event.pv_set_info;
        ui4_mem_type = MM_SBTL_INF_MSK_TM_OFST;
        x_memcpy(&pt_engine->t_cfg.t_time_ofst, 
                 pt_msg->u.t_msg_config_event.pv_set_info, 
                 sizeof(MM_SBTL_TM_OFST_INF));
        pv_mem = &pt_engine->t_cfg.t_time_ofst;
        break;

    case EVN_MM_SBTL_SET_HLT_FG_COLOR:
        break;

    case EVN_MM_SBTL_SET_HLT_BG_COLOR:
        break;

    case EVN_MM_SBTL_SET_PAGE_INF:
        break;

    case EVN_MM_SBTL_SET_LINE_GAP:
        break;
        
    case EVN_MM_SBTL_SET_SHOW_HIDE:
        break;
        
    case EVN_MM_SBTL_SET_FONT_BG_COLOR:
       if(_sbtl_engine_is_text_subtitle(pt_engine))
       {
               ui4_mem_type = MM_SBTL_INF_MSK_BG_COLOR;
             pt_engine->t_cfg.t_font_bg_color = *((WGL_COLOR_INFO_T*)pt_msg->u.t_msg_config_event.pv_set_info);
             pv_mem = &pt_engine->t_cfg.t_font_bg_color;
        }
        break;  
#ifdef MW_IPCC_SUPPORT
    case EVN_MM_SBTL_SET_CC_ATTR_INFO:         
        pt_engine->t_cfg.t_sbtl_cc_info= *((SM_CCH_DTVCC_ATTRIBS_INFO_T*)pt_msg->u.t_msg_config_event.pv_set_info);
        break;    
#endif
   case EVN_MM_SBTL_SET_TYPE_DISPLAY_INFO:
        if (pt_msg->u.t_msg_config_event.z_set_info_len == sizeof(SBTL_DISPLAY_INFO_CMD_T))
        {
            x_memcpy(&(pt_engine->t_cfg.t_display_info),
                       pt_msg->u.t_msg_config_event.pv_set_info,
                       sizeof(SBTL_DISPLAY_INFO_CMD_T));
            if (pt_engine->t_state.ui4_curr_state != SBTL_COND_STARTED)
            {
                if (pt_msg->u.t_msg_config_event.pv_set_info)
                {
                    x_mem_free(pt_msg->u.t_msg_config_event.pv_set_info);
                }
                return;
            }
        }
        break;
    case EVN_MM_SBTL_SET_TYPE_PARSE_DONE://internal parse done
    break;
	
	case EVN_MM_SBTL_SET_TYPE_BUFF_FILE_INFO:
        break;

    default:
        ASSERT(0, ("unknown type %d\n", pt_msg->u.t_msg_config_event.e_set_type));
        break;
    }

    DBG_INFO(("mmsbtl config type %d \n", pt_msg->u.t_msg_config_event.e_set_type));
    
      if (pt_engine->pt_page_mngr && pt_engine->pt_page_mngr->pf_cfg)
    {
    
        i4_ret = pt_engine->pt_page_mngr->pf_cfg(
                                    pt_engine->pt_page_mngr,
                                    pt_msg->u.t_msg_config_event.pv_set_info,
                                    pt_msg->u.t_msg_config_event.e_set_type);
    }
    
    if (i4_ret == MMSBTLR_OK)
    {
        i4_ret = sbtl_record_config(pt_engine,ui4_mem_type, pv_mem);
    }

    if (pt_msg->u.t_msg_config_event.pv_set_info)
    {
        x_mem_free(pt_msg->u.t_msg_config_event.pv_set_info);
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
static VOID _sbtl_engine_get_msg_hdlr(SBTL_ENGINE_T*            pt_engine,
                                      MM_SBTL_HDLR_MSG_T*       pt_msg)
{
    switch (pt_msg->u.t_msg_get_event.e_get_type)
    {
    case EVN_MM_SBTL_GET_STREAM_INFO:
    case EVN_MM_SBTL_GET_OSD_OFFSET:
    case EVN_MM_SBTL_GET_OSD_FG_COLOR:
    case EVN_MM_SBTL_GET_OSD_BG_COLOR:
    case EVN_MM_SBTL_GET_OSD_FONT_INFO:
    case EVN_MM_SBTL_GET_ADJUST_TIME:
        if (pt_engine->pt_page_mngr && pt_engine->pt_page_mngr->pf_get)
        {
            pt_engine->pt_page_mngr->pf_get(
                                pt_engine->pt_page_mngr,
                                pt_msg->u.t_msg_get_event.e_get_type,
                                pt_msg->u.t_msg_get_event.pv_get_info,
                                pt_msg->u.t_msg_get_event.z_get_info_len);
        }
        break;
    default:
        break;
    }        

    x_sema_unlock(pt_engine->h_get_sem);
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
 #define MAX_MM_SBTL_INST  (3)
 
 SBTL_ENGINE_T*   pt_g_mm_sbtl_engine[MAX_MM_SBTL_INST] = {NULL,NULL,NULL};

 extern SBTL_ENGINE_T* mm_sbtl_get_using_engine(VOID)
 {
 	UINT32 ui4_i = 0;
	for (ui4_i = 0; ui4_i < MAX_MM_SBTL_INST; ui4_i++)
	{
		if (pt_g_mm_sbtl_engine[ui4_i])
		{
			return pt_g_mm_sbtl_engine[ui4_i];
		}
	}
	return NULL;
 }
static VOID _mm_sbtl_thread(VOID*   pv_data)
{
    INT32                       i4_ret;
    MM_SBTL_HDLR_MSG_T          t_msg;
    SBTL_PAGE_INST_MNGR_T*      pt_mngr = NULL;
    SBTL_ENGINE_T*              pt_engine = NULL;
    SIZE_T                      z_msg_size = sizeof(MM_SBTL_HDLR_MSG_T);
    UINT16                      ui2_index;
    HANDLE_T                    h_wgl_app;
    HANDLE_T                    h_thread;
    PTS_T                       t_stc;
    UINT16                      ui2_i = 0;
    
    i4_ret = sbtl_wgl_register( _mm_sbtl_hdlr_wgt_app_callback, 
                             &h_wgl_app);
    
    if (MMSBTLR_OK != i4_ret)
    {
        x_thread_exit();
        return ;
    }

    /* Push all subtitle widget to bottom */
    i4_ret = x_thread_self( &h_thread );
    if ( i4_ret != OSR_OK )
    {
        ASSERT(0, ("_mm_sbtl_thread x_thread_self error=%d\n", i4_ret));
        x_thread_exit();
        return;

    }
    
    i4_ret = sbtl_wgl_set_app_order(h_thread, SBTL_WGL_APP_BOTTOMMOST);
    if ( i4_ret != MMSBTLR_OK )
    {
        ASSERT(0, ("_mm_sbtl_thread sbtl_wgl_set_app_order error=%d\n", i4_ret));
        x_thread_exit();
        return;
    }

    
    while (1)
    {
        x_memset( &t_msg, 0, sizeof(MM_SBTL_HDLR_MSG_T) );

        i4_ret = x_msg_q_receive( &ui2_index,
                                  &t_msg,
                                  &z_msg_size,
                                  &h_msg_que,
                                  (UINT16)1,
                                  X_MSGQ_OPTION_WAIT );
        if ( i4_ret == OSR_NO_MSG )
        {
            continue;
        }
        
        if ( i4_ret != OSR_OK )
        {
            ASSERT(0, ("_mm_sbtl_thread x_msg_q_receive error=%d\n", i4_ret));
            x_thread_exit();
            return;
        }

        if(t_msg.e_msg_type == MM_SBTL_HDLR_MSG_TIMER_EVENT)
        {
            pt_mngr = (SBTL_PAGE_INST_MNGR_T*)t_msg.pv_mngr;
            if (NULL == pt_mngr)
            {
                ASSERT(0, ("_mm_sbtl_thread pt_mngr=NULL\n"));
                continue;
            } 

            pt_engine = pt_mngr->pt_engine;
        }
        else
        {
            pt_engine = (SBTL_ENGINE_T*)t_msg.pv_sbtl_engine;
        }
        
        if (NULL == pt_engine)
        {
            ASSERT(0, ("_mm_sbtl_thread pt_engine=NULL\n"));
            continue;
        }

        if (t_msg.e_msg_type == MM_SBTL_HDLR_MSG_STATE_EVENT &&
            EVN_MM_SBTL_APP_OPEN_REQ == t_msg.u.t_msg_state_event.ui4_event)
        {
            for (ui2_i = 0; ui2_i < MAX_MM_SBTL_INST; ui2_i++)
            {
                if (pt_g_mm_sbtl_engine[ui2_i] == NULL)
                {
                    pt_g_mm_sbtl_engine[ui2_i] = pt_engine;
                    break;
                }
            }
        }
        
        for (ui2_i = 0; ui2_i < MAX_MM_SBTL_INST; ui2_i++)
        {
            if (pt_g_mm_sbtl_engine[ui2_i] == pt_engine)
            {
                break;
            }
        }
        if (ui2_i >= MAX_MM_SBTL_INST)
        {
            #ifndef __KERNEL__
            DBG_ERROR(("Discard message %d\n",t_msg.e_msg_type));
            #endif
            continue;
        }
        switch( t_msg.e_msg_type )
        {
        case MM_SBTL_HDLR_MSG_STATE_EVENT:
            _sbtl_engine_do_state_msg_hdlr(pt_engine, 
                                           t_msg.u.t_msg_state_event.ui4_event, 
                                           0);
            if (EVN_MM_SBTL_APP_CLOSE_REQ == t_msg.u.t_msg_state_event.ui4_event)
            {
                for (ui2_i = 0; ui2_i < MAX_MM_SBTL_INST; ui2_i++)
                {
                    if (pt_g_mm_sbtl_engine[ui2_i] == pt_engine)
                    {
                        pt_g_mm_sbtl_engine[ui2_i] = NULL;
                        break;
                    }
                }
            }
            break;

        case MM_SBTL_HDLR_MSG_DMX_EVENT:
            sbtl_stc_pts(pt_engine, &t_stc);
            _sbtl_engine_dmx_msg_hdlr(pt_engine, 
                                      t_msg.u.t_msg_dmx_event.e_nfy_cond /* e_nfy_cond */,
                                      t_msg.u.t_msg_dmx_event.ui4_data_1 /* data 1 */,
                                      t_msg.u.t_msg_dmx_event.ui4_data_2 /* data 2 */,
                                      t_msg.u.t_msg_dmx_event.ui4_data_3 /* data 3 */,
                                      t_msg.u.t_msg_dmx_event.ui4_pts,
                                      t_msg.u.t_msg_dmx_event.ui4_duration,
                                      t_stc);
            break;

        case MM_SBTL_HDLR_MSG_PTS_EVENT:
            _sbtl_engine_pts_msg_hdlr(pt_engine,
                                      t_msg.u.t_msg_pts_event.t_pts,
                                      t_msg.u.t_msg_pts_event.b_cycle);
            break;

        case MM_SBTL_HDLR_MSG_TIMER_EVENT:
            if (pt_engine->t_state.ui4_curr_state == SBTL_COND_STARTED)
            {
                _sbtl_engine_timer_msg_hdlr(pt_engine);
            }
            break;

        case MM_SBTL_HDLR_MSG_CONFIG_EVENT:
            _sbtl_engine_config_msg_hdlr(pt_engine, &t_msg);
            break;
            
        case MM_SBTL_HDLR_MSG_GET_EVENT:
            _sbtl_engine_get_msg_hdlr(pt_engine, &t_msg);
            break;
            
        case MM_SBTL_HDLR_MSG_IGNORE:
            break;

        default:
            DBG_ERROR((_ERROR_HEADER"t_msg.e_msg_type=%d\n", t_msg.e_msg_type));
            break;
        }
        
    }
}

#ifdef MW_IPCC_SUPPORT
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
static VOID _mm_sbtl_parser_thread(VOID*   pv_data)
{        
        MM_SBTL_PARSE_HDLR_MSG_T          t_parser_msg;        
        
        SBTL_PAGE_INST_MNGR_T*      pt_mngr = NULL;
        SIZE_T                      z_msg_size = sizeof(MM_SBTL_PARSE_HDLR_MSG_T);
        UINT16                      ui2_index;
        HANDLE_T                    h_thread;
        INT32                       i4_ret;
        
        /* Push all subtitle widget to bottom */
        i4_ret = x_thread_self( &h_thread );
        if ( i4_ret != OSR_OK )
        {
            ASSERT(0, ("_mm_sbtl_thread x_thread_self error=%d\n", i4_ret));
            x_thread_exit();
            return;
        
        }
       while(1)
        {                        
             x_memset( &t_parser_msg, 0, sizeof(MM_SBTL_PARSE_HDLR_MSG_T) );

            i4_ret = x_msg_q_receive( &ui2_index,
                                      &t_parser_msg,
                                      &z_msg_size,
                                      &h_msg_parser_que,
                                      (UINT16)1,
                                      X_MSGQ_OPTION_WAIT );
            if ( i4_ret == OSR_NO_MSG )
            {
                continue;
            }
            
            if ( i4_ret != OSR_OK )
            {
                ASSERT(0, ("_mm_sbtl_thread x_msg_q_receive error=%d\n", i4_ret));
                x_thread_exit();
                return;
            }
            
            switch ( t_parser_msg.e_msg_type )
            {   
                case MM_SBTL_PARSER_HDLR_MSG_PARSER_EVENT:
                {
                    pt_mngr   = (SBTL_PAGE_INST_MNGR_T*)(t_parser_msg.pv_mngr);       
                    if (NULL == pt_mngr || 
                        pt_mngr->pf_pre_parser == NULL ||
                        pt_mngr->pt_engine == NULL)
                    {
                        break;
                    }

                    i4_ret = pt_mngr->pf_pre_parser(pt_mngr);
                    if (MMSBTLR_OK != i4_ret)
                    {
                        DBG_INFO(("=========the xml_parser parser data fail!=======\n"));
                    }
                }
                break;

                case MM_SBTL_PARSER_HDLR_MSG_CALL_EVENT:
                {
                    MM_SBTL_PARSE_CALL_T* pt_call_arg = (MM_SBTL_PARSE_CALL_T*)&t_parser_msg.u_event.t_call_args;
                    if (pt_call_arg->pf_call)
                    {
                        pt_call_arg->pf_call(pt_call_arg->pv_arg1, pt_call_arg->pv_arg2, pt_call_arg->pv_arg3);
                    }
                }
                break;
                
                default:
                     DBG_ERROR((_ERROR_HEADER"t_msg.e_msg_type=%d\n", t_parser_msg.e_msg_type));
                      break;           
                        
            }
    }
}
#endif

/*-----------------------------------------------------------------------------
                    External functions implementations
 ----------------------------------------------------------------------------*/
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
VOID mm_sbtl_hdlr_send_msg(const MM_SBTL_HDLR_MSG_T* pt_msg)
{
    INT32               i4_ret; 
    INT32               i4_retry;

    i4_retry = 0;
    
    if (pt_msg && pt_msg->pv_sbtl_engine)
    {
        #ifdef MW_IPCC_SUPPORT
        SBTL_ENGINE_T* pt_engine = pt_msg->pv_sbtl_engine;
        if (pt_msg->e_msg_type == MM_SBTL_HDLR_MSG_STATE_EVENT &&
            pt_msg->u.t_msg_state_event.ui4_event == EVN_MM_SBTL_APP_CLOSE_REQ)
        {
            if (pt_engine &&
                pt_engine->pt_page_mngr &&
                pt_engine->pt_page_mngr->pf_cfg)
            {
                i4_ret = pt_engine->pt_page_mngr->pf_cfg(
                                pt_engine->pt_page_mngr,
                                NULL,
                                EVN_MM_SBTL_APP_CLOSE_REQ);
            }
            pt_engine->b_is_stop_req = TRUE ;
            DBG_INFO(("[mm sbtl] set stop flags in close\n"));
        }
        #endif
     }
    
    while ( i4_retry < 10 )
    {
        i4_retry++;
        i4_ret = x_msg_q_send( h_msg_que,
                               pt_msg,
                               sizeof( MM_SBTL_HDLR_MSG_T ),
                               0 );
        if ( i4_ret != OSR_OK )
        {
            /* Send message fail, wait 10 ms and retry for 10 times */
            x_thread_delay( 10 );
            continue;
        }
        
        /* success */
        return;
    }
}

#ifdef MW_IPCC_SUPPORT
VOID mm_sbtl_parser_hdlr_send_msg(const MM_SBTL_PARSE_HDLR_MSG_T* pt_parser_msg)
{
    INT32               i4_ret; 
    INT32               i4_retry;

    i4_retry = 0;
    while ( i4_retry < 10 )
    {
        i4_retry++;
        i4_ret = x_msg_q_send( h_msg_parser_que,
                               pt_parser_msg,
                               sizeof( MM_SBTL_PARSE_HDLR_MSG_T ),
                               0 );
        if ( i4_ret != OSR_OK )
        {
            /* Send message fail, wait 10 ms and retry for 10 times */
            x_thread_delay( 10 );
            continue;
        }
        
        /* success */
        return;
    }
}
#endif
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
INT32 x_sbtl_engine_init(VOID)
{
    INT32   i4_ret;
    
    if (b_inited == TRUE)
    {
        return MMSBTLR_OK;
    }

    if (NULL_HANDLE == h_msg_que)
    {
        i4_ret = x_msg_q_create( &h_msg_que,
                                 SM_MM_SBTL_STRM_HDLR_NAME,
                                 sizeof( MM_SBTL_HDLR_MSG_T ),
                                 MAX_MM_SBTL_REQUEST );
        if ( i4_ret != OSR_OK )
        {
            return MMSBTLR_CORE;
        }
    }

    if (NULL_HANDLE == h_sbtl_thread)
    {
        i4_ret = x_thread_create(&h_sbtl_thread,
                                 SM_MM_SBTL_STRM_HDLR_NAME,
                                 MM_SBTL_THREAD_STACK_SIZE,
                                 MM_SBTL_THREAD_PRIORITY,
                                 _mm_sbtl_thread,
                                 sizeof(HANDLE_T *),
                                 (VOID *)&h_msg_que);

        if (i4_ret != OSR_OK)
        {
            x_msg_q_delete(h_msg_que);
            return MMSBTLR_CORE;
        }
    }
    #ifdef MW_IPCC_SUPPORT
    if (NULL_HANDLE == h_msg_parser_que)
        {
            i4_ret = x_msg_q_create( &h_msg_parser_que,
                                     SM_MM_SBTL_STRM_PARSER_HDLR_NAME,
                                     sizeof( MM_SBTL_PARSE_HDLR_MSG_T ),
                                     MAX_MM_SBTL_REQUEST );
            if ( i4_ret != OSR_OK )
            {
                return MMSBTLR_CORE;
            }
        }
    
        if (NULL_HANDLE == h_sbtl_parser_thread)
        {
            i4_ret = x_thread_create(&h_sbtl_parser_thread,
                                     SM_MM_SBTL_STRM_PARSER_HDLR_NAME,
                                     MM_SBTL_THREAD_STACK_SIZE,
                                     MM_SBTL_THREAD_PRIORITY,
                                     _mm_sbtl_parser_thread,
                                     sizeof(HANDLE_T *),
                                     (VOID *)&h_msg_parser_que);
            
            if (i4_ret != OSR_OK)
            {
                x_msg_q_delete(h_msg_parser_que);
                return MMSBTLR_CORE;
            }
        }
#endif
    sbtl_register_init();
#ifndef __KERNEL__
    mm_sbtl_cli_init();
#endif
    b_inited = TRUE;
    
    sbtl_record_init();
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
INT32 x_sbtl_engine_open( SBTL_CFG_T*               pt_cfg,
                          BOOL                      b_auto_play,
                          sbtl_state_notify_fct     pf_state_nfy,
                          VOID*                     pv_sm_nfy_tag,
                          VOID**                    ppv_engine)
{
    INT32                           i4_ret;
    SBTL_ENGINE_T*                  pt_engine;
    SBTL_ENGINE_T**                 ppt_engine;
    MM_SBTL_HDLR_MSG_T              t_msg;
    page_inst_mngr_destructor       pf_destructor;
    UINT8                           ui1_final_type;
    
    do
    {
        ppt_engine = (SBTL_ENGINE_T**)ppv_engine;
        pt_engine = (SBTL_ENGINE_T*)x_mem_alloc(sizeof(SBTL_ENGINE_T));
        if (NULL == pt_engine)
        {
            i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
            break;
        }
        x_memset(pt_engine, 0, sizeof(SBTL_ENGINE_T));

        pt_engine->t_cfg = *pt_cfg;
        
        *ppt_engine = pt_engine;
       
        pt_engine->h_get_sem = NULL_HANDLE;
        i4_ret = x_sema_create(&pt_engine->h_get_sem, 
                               X_SEMA_TYPE_BINARY, 
                               X_SEMA_STATE_LOCK);
        if ( i4_ret != MMSBTLR_OK)
        {
            i4_ret = MMSBTLR_CORE;
            break;
        }
      
        /*create page inst mngr*/
        i4_ret = sbtl_register_match_mngr(pt_engine,
                                          pt_engine->t_cfg.e_sbtl_type,
                                          &pt_engine->pt_fct_tbl,
                                          &ui1_final_type);
        
        if ( i4_ret != MMSBTLR_OK)
        {
            i4_ret = MMSBTLR_NOT_FOUND;
            break;
        }

        pt_engine->t_cfg.e_sbtl_type = (MM_SBTL_TYPE_T)ui1_final_type;

        if (NULL==pt_engine->pt_fct_tbl)
        {
            i4_ret = MMSBTLR_NOT_FOUND;
            break;
        }
        
        
        i4_ret = mm_sbtl_state_init( pf_state_nfy,
                                     pv_sm_nfy_tag,
                                     &pt_engine->t_state);
        if ( i4_ret != MMSBTLR_OK )
        {
            i4_ret = MMSBTLR_CORE;
            break;
        }
#ifndef __KERNEL__
        DBG_LOG_PRINT(("==== Sbtl Open type %d Success!====\r\n", pt_engine->t_cfg.e_sbtl_type));
#endif
        mm_sbtl_hdlr_send_state_msg(t_msg, 
                                    pt_engine, 
                                    EVN_MM_SBTL_APP_OPEN_REQ);
        
        if (TRUE == b_auto_play)
        {
            mm_sbtl_hdlr_send_state_msg(t_msg, 
                                        pt_engine, 
                                        EVN_MM_SBTL_APP_PLAY_REQ);
        }

        return MMSBTLR_OK;
    }while(0);

    if (pt_engine)
    {
        if (pt_engine->pt_page_mngr)
        {
            if (pt_engine->pt_fct_tbl)
            {
                pf_destructor = pt_engine->pt_fct_tbl->pf_mngr_destructor;

                if (pf_destructor)
                {
                    pf_destructor(pt_engine->pt_page_mngr);
                }
            }
        }

        if (pt_engine->h_get_sem)
        {
            x_sema_delete(pt_engine->h_get_sem);
            pt_engine->h_get_sem = NULL_HANDLE;
        }
        x_mem_free(pt_engine);
    }

    return i4_ret;
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
INT32 x_sbtl_engine_close( VOID*    pv_engine)
{
    MM_SBTL_HDLR_MSG_T              t_msg;
    SBTL_ENGINE_T*                  pt_engine;

    ASSERT(pv_engine, ("x_sbtl_engine_close pv_engine=NULL\n"));
    if (NULL==pv_engine)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_engine = (SBTL_ENGINE_T*)pv_engine;
    if (NULL == pt_engine)
    {
        return MMSBTLR_INV_ARG;
    }

    mm_sbtl_hdlr_send_state_msg(t_msg, 
                                pt_engine, 
                                EVN_MM_SBTL_APP_CLOSE_REQ);
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
INT32 x_sbtl_engine_set(VOID*               pv_engine,
                        MM_EVN_TYPE_T       e_event,
                        VOID*               pv_set_info,
                        SIZE_T              z_set_info_len)
{
    INT32                   i4_ret = MMSBTLR_OK;
    MM_SBTL_HDLR_MSG_T      t_sbtl_hdlr_msg;
    SBTL_ENGINE_T*          pt_engine;
    VOID*                   pv_info;

    ASSERT(pv_engine, ("x_sbtl_engine_set pv_engine=NULL\n"));
    if (NULL==pv_engine)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_engine = (SBTL_ENGINE_T*)pv_engine;

    if ( pt_engine->t_state.ui4_curr_state != SBTL_COND_IGNORE &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_OPENING &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_OPENED &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_STARTING &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_STARTED &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_STOPPING )
    {
        return MMSBTLR_CAN_NOT_OP_IN_THIS_STATE;
    }

    switch ( e_event )
    {
        case EVN_MM_SBTL_APP_CLOSE_REQ:
            break;
        case EVN_MM_SBTL_APP_PLAY_REQ:
        case EVN_MM_SBTL_APP_STOP_REQ:
            if ( pt_engine->t_state.ui4_curr_state == SBTL_COND_STARTED &&
                 e_event == EVN_MM_SBTL_APP_PLAY_REQ )
            {
                return MMSBTLR_OK;
            }

            mm_sbtl_hdlr_send_state_msg(t_sbtl_hdlr_msg, 
                                        pt_engine, 
                                        e_event);
            break;
            
        case EVN_MM_SBTL_SET_VIDEO_RECT:
        case EVN_MM_SBTL_SET_VIDEO_OVERSCAN:
        case EVN_MM_SBTL_SET_OSD_RECT:
        case EVN_MM_SBTL_SET_OSD_INSECT:
        case EVN_MM_SBTL_SET_OSD_OFFSET:
        case EVN_MM_SBTL_SET_OSD_FG_COLOR:
        case EVN_MM_SBTL_SET_OSD_BG_COLOR:
        case EVN_MM_SBTL_SET_OSD_FONT_INFO:
        case EVN_MM_SBTL_SET_OSD_TEXT_ALIGNMENT:
        case EVN_MM_SBTL_SET_DIVX_MENU_ITEM:
        case EVN_MM_SBTL_SET_SPEED:
        case EVN_MM_SBTL_SET_STREAM_INDEX:
        case EVN_MM_SBTL_SET_LANGUAGE:
        case EVN_MM_SBTL_SET_ADJUST_TIME:
        case EVN_MM_SBTL_SET_ENCODING:
        case EVN_MM_SBTL_SET_HLT_FG_COLOR:
        case EVN_MM_SBTL_SET_HLT_BG_COLOR:
        case EVN_MM_SBTL_SET_OSD_FONT_EDGE_COLOR:
        case EVN_MM_SBTL_SET_PAGE_INF:
        case EVN_MM_SBTL_SET_LINE_GAP:
        case EVN_MM_SBTL_SET_SHOW_HIDE:
        case EVN_MM_SBTL_SET_FONT_BG_COLOR:
        case EVN_MM_SBTL_SET_TYPE_DISPLAY_INFO:
        case EVN_MM_SBTL_SET_TYPE_BUFF_FILE_INFO:
#ifdef MW_IPCC_SUPPORT
        case EVN_MM_SBTL_SET_CC_ATTR_INFO:
#endif 
        case EVN_MM_SBTL_SET_TYPE_PARSE_DONE://internal parse done
            if (z_set_info_len)
            {
                pv_info = x_mem_alloc(z_set_info_len);
                if (pv_info)
                {
                    x_memcpy(pv_info, pv_set_info, z_set_info_len);
                    mm_sbtl_hdlr_send_config_msg(t_sbtl_hdlr_msg, 
                                                 pt_engine,
                                                 e_event,
                                                 pv_info,
                                                 z_set_info_len);
                }
                else
                {
                    i4_ret = MMSBTLR_CORE;
                }
            }
            else
            {
                mm_sbtl_hdlr_send_config_msg(t_sbtl_hdlr_msg, 
                                             pt_engine,
                                             e_event,
                                             NULL,
                                             0);
            }

            break;
            
        default:
            i4_ret = MMSBTLR_INV_ARG;
    }

    return i4_ret;
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
INT32 x_sbtl_engine_get(VOID*               pv_engine,
                        MM_EVN_TYPE_T       e_event,
                        VOID*               pv_get_info,
                        SIZE_T              z_get_info_len)
{
    INT32                   i4_ret = MMSBTLR_INV_ARG;
    MM_SBTL_HDLR_MSG_T      t_sbtl_hdlr_msg;
    SBTL_ENGINE_T*          pt_engine;

    ASSERT(pv_engine, ("x_sbtl_engine_set pv_engine=NULL\n"));
    if (NULL==pv_engine || NULL == pv_get_info)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_engine = (SBTL_ENGINE_T*)pv_engine;

    if ( pt_engine->t_state.ui4_curr_state != SBTL_COND_IGNORE &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_OPENING &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_OPENED &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_STARTING &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_STARTED &&
         pt_engine->t_state.ui4_curr_state != SBTL_COND_STOPPING )
    {
        return MMSBTLR_CAN_NOT_OP_IN_THIS_STATE;
    }

    switch ( e_event )
    {
    case EVN_MM_SBTL_GET_STREAM_INFO:
    case EVN_MM_SBTL_GET_VIDEO_RECT:
    case EVN_MM_SBTL_GET_VIDEO_OVERSCAN:
    case EVN_MM_SBTL_GET_OSD_RECT:
    case EVN_MM_SBTL_GET_OSD_INSECT:
    case EVN_MM_SBTL_GET_OSD_OFFSET:
    case EVN_MM_SBTL_GET_OSD_TEXT_ALIGNMENT:
    case EVN_MM_SBTL_GET_DIVX_MENU_ITEM:
    case EVN_MM_SBTL_GET_SPEED:
    case EVN_MM_SBTL_GET_STREAM_INDEX:
    case EVN_MM_SBTL_GET_LANGUAGE:
    case EVN_MM_SBTL_GET_ADJUST_TIME:
    case EVN_MM_SBTL_GET_HLT_FG_COLOR:
    case EVN_MM_SBTL_GET_HLT_BG_COLOR:
    case EVN_MM_SBTL_GET_PAGE_INF:
    case EVN_MM_SBTL_GET_LINE_GAP:
        if(z_get_info_len > 0)
        {
            //i4_ret = x_sema_lock(pt_engine->h_get_sem, X_SEMA_OPTION_WAIT);
            mm_sbtl_hdlr_send_get_msg(t_sbtl_hdlr_msg, 
                                      pt_engine,
                                      e_event,
                                      pv_get_info,
                                      z_get_info_len);
			i4_ret = MMSBTLR_OK;
        }

        //i4_ret = x_sema_lock(pt_engine->h_get_sem, X_SEMA_OPTION_WAIT);
        break;
		
    case EVN_MM_SBTL_GET_SHOW_HIDE:
		if (z_get_info_len == sizeof(BOOL))
		{
			*((BOOL*)pv_get_info) = pt_engine->t_cfg.b_hide?FALSE:TRUE;
			i4_ret = MMSBTLR_OK;
		}
		break;
    case EVN_MM_SBTL_GET_ENCODING:
		if (z_get_info_len == sizeof(MM_SBTL_ENCODING_T))
		{
			x_memcpy(pv_get_info, &pt_engine->t_cfg.e_encoding, sizeof(MM_SBTL_ENCODING_T));
			i4_ret = MMSBTLR_OK;
		}
		break;
		
	case EVN_MM_SBTL_GET_OSD_FONT_INFO:
		if (z_get_info_len == sizeof(WGL_FONT_INFO_T))
		{
			x_memcpy(pv_get_info, &pt_engine->t_cfg.t_font_info, sizeof(WGL_FONT_INFO_T));
			i4_ret = MMSBTLR_OK;
		}
		break;
		
	case EVN_MM_SBTL_GET_OSD_FG_COLOR:
    case EVN_MM_SBTL_GET_OSD_BG_COLOR:
	case EVN_MM_SBTL_GET_OSD_FONT_EDGE_COLOR:
		if (z_get_info_len == sizeof(WGL_COLOR_INFO_T))
		{
			WGL_COLOR_INFO_T* pt_color = &pt_engine->t_cfg.t_bg_color_info;
			if (e_event == EVN_MM_SBTL_GET_OSD_FG_COLOR)
			{
				pt_color = &pt_engine->t_cfg.t_fg_color_info;
			}
			else if (e_event == EVN_MM_SBTL_GET_OSD_FONT_EDGE_COLOR)
			{
				pt_color = &pt_engine->t_cfg.t_edge_color_info;
			}
			x_memcpy(pv_get_info, pt_color, sizeof(WGL_COLOR_INFO_T));
			i4_ret = MMSBTLR_OK;
		}
		break;
        
    case EVN_MM_SBTL_GET_SBTL_TYPE:
        if(z_get_info_len == sizeof(MM_SBTL_TYPE_T))
        {
            x_memcpy(pv_get_info,&pt_engine->t_cfg.e_sbtl_type,sizeof(MM_SBTL_TYPE_T));
			i4_ret = MMSBTLR_OK;
        }
		break;
        
    default:
        i4_ret = MMSBTLR_INV_ARG;
        break;
    }

    return i4_ret;
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
INT32 x_sbtl_parse_lang(SBTL_LANG_DESC_T* t_sbtl_lang_desc)
{
#ifndef __NO_FM__
#ifndef __KERNEL__
#ifdef MW_MMP_VIDEO_SUPPORT
    CHAR*                   ps_file_extension   = NULL;
    UINT32                  ui4_path_length     = 0;
    INT32                   i4_ret              = MMSBTLR_OK;
    HANDLE_T                h_file_subtitle     = NULL_HANDLE;
    HANDLE_T                h_file_index        = NULL_HANDLE;

    ASSERT(t_sbtl_lang_desc->pc_sbtl_file_name, ("x_sbtl_parse_lang pc_sbtl_file_name=NULL\n"));
    if (NULL == t_sbtl_lang_desc->pc_sbtl_file_name || NULL == t_sbtl_lang_desc->pc_related_file_name
        || x_strlen(t_sbtl_lang_desc->pc_sbtl_file_name)==0)
    {
        return MMSBTLR_INV_ARG;
    }

    if(t_sbtl_lang_desc->b_is_http_file)
    {
#ifdef INET_SUPPORT
        x_thread_delay(1500);
#ifdef USE_SMART_FILE
        i4_ret = x_smart_file_open((char*)t_sbtl_lang_desc->pc_sbtl_file_name, (int*)&h_file_subtitle);
#else 
        i4_ret = x_httpc_fm_open_proxy_agent_timeout_cookie(
                                    t_sbtl_lang_desc->pc_sbtl_file_name, 
                                    NULL,//pt_media_desc->ps_proxy_url, 
                                    NULL,//pt_media_desc->ps_agent_url, 
                                    0,
                                    128*1024,
                                    &h_file_subtitle,
                                    NULL/*pt_media_desc->ps_cookie_url*/);
#endif
#endif
    }
    else
    {
#ifndef ANDROID
        i4_ret = x_fm_open( FM_ROOT_HANDLE, 
                            t_sbtl_lang_desc->pc_sbtl_file_name,
                            FM_READ_ONLY, 
                            0777,
                            FALSE, 
                            &h_file_subtitle);
#else
        FILE *pt_file = NULL;
        pt_file = fopen (t_sbtl_lang_desc->pc_sbtl_file_name, "r");
        if(pt_file == NULL)
        {
            t_sbtl_lang_desc->ui1_lang_count = 0;
            return MMSBTLR_INV_ARG;
        }
        h_file_subtitle = (HANDLE_T)pt_file;
#endif
    }
    if(i4_ret != FMR_OK)
    {
        t_sbtl_lang_desc->ui1_lang_count = 0;
        return i4_ret;
    }

    ps_file_extension = x_strrchr(t_sbtl_lang_desc->pc_sbtl_file_name,'.');
    ps_file_extension++;
    
    if ((x_strcmp("sub", ps_file_extension) == 0) ||
        (x_strcmp("SUB", ps_file_extension) == 0))
    {
        ui4_path_length = (UINT32)(ps_file_extension - t_sbtl_lang_desc->pc_sbtl_file_name);
        x_memcpy(t_sbtl_lang_desc->pc_related_file_name, t_sbtl_lang_desc->pc_sbtl_file_name, ui4_path_length);
        t_sbtl_lang_desc->pc_related_file_name[ui4_path_length] = 0;
        x_strcat(t_sbtl_lang_desc->pc_related_file_name, "idx");

        if(t_sbtl_lang_desc->b_is_http_file)
        {
#ifdef INET_SUPPORT
            x_thread_delay(1500);
#ifdef USE_SMART_FILE
            i4_ret = x_smart_file_open((char*)t_sbtl_lang_desc->pc_sbtl_file_name, (int*)&h_file_subtitle);
#else
            i4_ret = x_httpc_fm_open_proxy_agent_timeout_cookie(
                                        t_sbtl_lang_desc->pc_related_file_name, 
                                        NULL,//pt_media_desc->ps_proxy_url, 
                                        NULL,//pt_media_desc->ps_agent_url, 
                                        0,
                                        128*1024,
                                        &h_file_index,
                                        NULL/*pt_media_desc->ps_cookie_url*/);
#endif
#endif
        }
        else
        {
#ifndef ANDROID
            i4_ret = x_fm_open( FM_ROOT_HANDLE, 
                                t_sbtl_lang_desc->pc_related_file_name,
                                FM_READ_ONLY, 
                                0777,
                                FALSE, 
                                &h_file_index);
#else
            FILE *pt_file = NULL;
            pt_file = fopen (t_sbtl_lang_desc->pc_related_file_name, "r");
            if(pt_file == NULL)
            {
                t_sbtl_lang_desc->ui1_lang_count = 1;
                x_memset(t_sbtl_lang_desc->pc_related_file_name, 0, x_strlen(t_sbtl_lang_desc->pc_related_file_name));
                fclose((FILE*)h_file_subtitle);
                return MMSBTLR_OK;
            }
            h_file_index = (HANDLE_T)pt_file;
#endif
        }
        if(i4_ret != FMR_OK)
        {
            t_sbtl_lang_desc->ui1_lang_count = 1;
            x_memset(t_sbtl_lang_desc->pc_related_file_name, 0, x_strlen(t_sbtl_lang_desc->pc_related_file_name));
#ifndef ANDROID
            x_fm_close(h_file_subtitle);
#else
            fclose((FILE*)h_file_subtitle);
#endif
            return MMSBTLR_OK;
        }

        i4_ret = x_vobsub_parse_lang(h_file_subtitle, 
                                    h_file_index,
                                    MM_SBTL_TYPE_VOBSUB,
                                    t_sbtl_lang_desc);
     }
    else if((x_strcmp("idx", ps_file_extension) == 0) ||
        (x_strcmp("IDX", ps_file_extension) == 0))
    {    
        t_sbtl_lang_desc->ui1_lang_count = 0;
    }
    else
    {
#ifdef MW_IPCC_SUPPORT
        if(x_strcmp("xml", ps_file_extension) == 0  ||
                 x_strcmp("XML", ps_file_extension) == 0 ||
                x_strcmp("M4T", ps_file_extension) == 0    ||
                x_strcmp("m4t", ps_file_extension) == 0)
        {
            t_sbtl_lang_desc->b_is_xml_file  = TRUE;
        }      
#endif  
        t_sbtl_lang_desc->ui1_lang_count = 1;
    }

    if(t_sbtl_lang_desc->b_is_http_file)
    {
#ifdef INET_SUPPORT
        if(h_file_index != NULL_HANDLE)
        {
#ifdef USE_SMART_FILE
            x_smart_file_close(h_file_index);
#else            
            x_httpc_fm_close(h_file_index);
#endif
            h_file_index = NULL_HANDLE;
        }
        if(h_file_subtitle != NULL_HANDLE)
        {
#ifdef USE_SMART_FILE
            x_smart_file_close(h_file_subtitle);
#else             
            x_httpc_fm_close(h_file_subtitle);
#endif
            h_file_subtitle = NULL_HANDLE;
        }
#endif
    }
    else
    {
#ifndef ANDROID
        if(h_file_index != NULL_HANDLE)
        {
            x_fm_close(h_file_index);
            h_file_index = NULL_HANDLE;
        }
        if(h_file_subtitle != NULL_HANDLE)
        {
            x_fm_close(h_file_subtitle);
            h_file_subtitle = NULL_HANDLE;
        }
#else
        if(h_file_index != NULL_HANDLE)
        {
            fclose((FILE*)h_file_index);
            h_file_index = NULL_HANDLE;
        }
        if(h_file_subtitle != NULL_HANDLE)
        {
            fclose((FILE*)h_file_subtitle);
            h_file_subtitle = NULL_HANDLE;
        }
#endif
    }
    return i4_ret;
#else
    t_sbtl_lang_desc->ui1_lang_count = 0;
    return MMSBTLR_OK;
#endif

#else
    t_sbtl_lang_desc->ui1_lang_count = 0;
    return MMSBTLR_OK;
#endif
#else
    t_sbtl_lang_desc->ui1_lang_count = 0;
    return MMSBTLR_OK;
#endif
    
}


extern INT32 x_mmsbtl_get_def_value(UINT32 ui4_type, VOID* pv_data)
{
    if (pv_data != NULL)
    {
        switch (ui4_type)
        {
            case MM_SBTL_DFT_FONT_SIZE_TYPE:
            {
                UINT8* pui1_size = (UINT8*)pv_data;
                #ifdef SBTL_DFT_FONT_FOR_CHINA
                  #ifndef MM_SBTL_DFT_FONT_CSTM_SIZE
                  *pui1_size = 36;
                  #else
                  *pui1_size = MM_SBTL_DFT_FONT_CSTM_SIZE;
                  #endif
               #else
                  *pui1_size = 30;
               #endif  
            }
            break;
            
            case MM_SBTL_DFT_FONT_ENC_TYPE:
            {
                MM_SBTL_ENCODING_T* pt_enc = (MM_SBTL_ENCODING_T*)pv_data;
                
                #ifdef SBTL_DFT_FONT_FOR_CHINA
                *pt_enc = MM_SBTL_ENCODE_GB2312;
                #else
                    #ifdef SUPPORT_SET_SBTL_ENCODING_CN
                    *pt_enc = MM_SBTL_ENCODE_GB18030;
                    #else
                    *pt_enc = MM_SBTL_ENCODE_ASCI;
                    #endif
                #endif 
            }
            break;
            default:
            break;
        }
        return MMSBTLR_OK;  
    }
    return MMSBTLR_INV_ARG;
}

INT32 x_sbtl_get_ext_sbtl_info(
	 SBTL_LANG_DESC_T*      pt_sbtl_desc, 
	 MM_SBTL_BASIC_INFO_T*  pt_info)
{
	 SBTL_ENGINE_T* 			  pt_engine   = NULL;
	 SBTL_CFG_T*				  pt_cfg = NULL;
	 MEDIA_FEEDER_T*			  pt_subtitle_feeder = NULL;
	 MEDIA_FEEDER_T*			  pt_index_feeder = NULL;
	 INT32						  i4_ret = MMSBTLR_OK;
	 page_inst_mngr_constructor   pf_constructor;
	 page_inst_mngr_destructor	  pf_desturctor;
	 UINT8						  ui1_matched_type;
	 if (pt_info == NULL)
	 {
		return MMSBTLR_INV_ARG;
	 }
	 pt_info->pt_track_info = NULL;
 
	 do
	 {
		pt_engine = (SBTL_ENGINE_T*)x_mem_alloc(sizeof(SBTL_ENGINE_T));
		if (NULL == pt_engine)
		{
		 i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
		 break;
		}
		pt_subtitle_feeder = x_mem_alloc(sizeof(MEDIA_FEEDER_T));
		if (NULL == pt_subtitle_feeder)
		{
		 i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
		 break;
		}
		pt_index_feeder = x_mem_alloc(sizeof(MEDIA_FEEDER_T));
		if (NULL == pt_index_feeder)
		{
		 i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
		 break;
		}

		x_memset(pt_engine, 0, sizeof(SBTL_ENGINE_T));
		x_memset(pt_subtitle_feeder, 0, sizeof(MEDIA_FEEDER_T));
		x_memset(pt_index_feeder, 0, sizeof(MEDIA_FEEDER_T));
		pt_cfg = &pt_engine->t_cfg;

		if (pt_sbtl_desc->b_is_http_file)
		{
#ifdef INET_SUPPORT
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_copybytes = sbtl_util_http_fcopybytes_fct;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input4 = NULL;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input3 = NULL;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input2 = NULL;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input1 = sbtl_util_http_finput1bytes_fct;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_set_pos = sbtl_util_http_fsetpos_fct;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_get_pos = sbtl_util_http_fgetpos_fct;
#else
			i4_ret = MMSBTLR_NOT_SUPPORT;
			break;
#endif
		}
		else
		{
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_copybytes = sbtl_util_fcopybytes_fct;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input4 = NULL;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input3 = NULL;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input2 = NULL;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_input1 = sbtl_util_finput1bytes_fct;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_set_pos = sbtl_util_fsetpos_fct;
			pt_subtitle_feeder->t_feeder_be_fct_tbl.pf_get_pos = sbtl_util_fgetpos_fct;
		}
		
		x_memcpy(&pt_subtitle_feeder->t_feeder_le_fct_tbl,
				 &pt_subtitle_feeder->t_feeder_be_fct_tbl, 
				 sizeof(FEEDER_SRC_INPUT_FCT_TBL_T));
		x_memcpy(pt_index_feeder, pt_subtitle_feeder, sizeof(MEDIA_FEEDER_T));
	
		pt_subtitle_feeder->h_feeder = pt_sbtl_desc->h_sbtl;
		pt_index_feeder->h_feeder	 = pt_sbtl_desc->h_idx;
		
		pt_cfg->e_sbtl_type = pt_info->e_sbtl_type;
		
	
		pt_cfg->pt_feeder[MM_SBTL_FEEDER_SUBTITLE] = pt_subtitle_feeder;
		pt_cfg->pt_feeder[MM_SBTL_FEEDER_INDEX]    = pt_index_feeder;
			 
		i4_ret = sbtl_register_match_mngr(pt_engine,
										  pt_cfg->e_sbtl_type,
										  &pt_engine->pt_fct_tbl,
										  &ui1_matched_type);
		
		if ( i4_ret != MMSBTLR_OK)
		{
			pt_info->e_sbtl_type = MM_SBTL_TYPE_NONE;
			i4_ret = MMSBTLR_NOT_FOUND;
			break;
		}
		
		pt_cfg->e_sbtl_type = (MM_SBTL_TYPE_T)ui1_matched_type;
		pt_info->e_sbtl_type = pt_cfg->e_sbtl_type;
		
		  
		pf_constructor = pt_engine->pt_fct_tbl->pf_mngr_constructor;
		pf_desturctor  = pt_engine->pt_fct_tbl->pf_mngr_destructor;
		if (pf_constructor == NULL)
		{
			break;
		}
		if ((pt_cfg->e_sbtl_type == MM_SBTL_TYPE_TEXT_SAMI) ||
			(pt_cfg->e_sbtl_type == MM_SBTL_TYPE_VOBSUB))
		{
			pt_engine->pt_page_mngr = pf_constructor(pt_engine);
			if (pt_engine->pt_page_mngr == NULL)
			{
				i4_ret = MMSBTLR_INTERNAL_ERROR;
				break;
			}
			// if could get basic info else set info to be null tracks to be 1
			if (pt_engine->pt_page_mngr->pf_get)
			{
				i4_ret = pt_engine->pt_page_mngr->pf_get(
							pt_engine->pt_page_mngr,
							EVN_MM_SBTL_GET_BASIC_INFO,
							(VOID*)pt_info,
							sizeof(MM_SBTL_BASIC_INFO_T)
							);
			}
			if (pf_desturctor)
			{
				pf_desturctor(pt_engine->pt_page_mngr);
			}
		}
		//default information is NULL
		if (pt_info->pt_track_info == NULL)
		{
			pt_info->pt_track_info = (MM_SBTL_TRACK_INFO*) x_mem_alloc(sizeof(MM_SBTL_TRACK_INFO));
			if (pt_info->pt_track_info)
			{
				x_memset(pt_info->pt_track_info, 0, sizeof(MM_SBTL_TRACK_INFO));
			}
		}	 
	 }while(0);
	 if (pt_subtitle_feeder) 
	 {
		x_mem_free(pt_subtitle_feeder);
	 }
	 if (pt_index_feeder)
	 {
		x_mem_free(pt_index_feeder);
	 }
	 if (pt_engine)
	 {
		x_mem_free(pt_engine);
	 }
	 return i4_ret;
}

