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
 * $RCSfile: sbtl_dync_wdgt.c,v $
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


#include "x_common.h"
#include "inc/common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_mm_common.h"
#include "x_uc_str.h"
#include "x_lnk_list.h"

#include "mutil/mm_util.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_widget.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_wdgt.h"


typedef struct
{
    SBTL_COMM_TYPE_T       e_comm_type;
    HANDLE_T               h_sema;
    SBTL_ARGB_INFO_LIST_T* pt_img_list;
    UINT64                 ui8_pts;
}SBTL_ARGB_WDGT_PRIVATE_T;

static INT32 _sbtl_argb_lock_pib_fct(HANDLE_T   h_page_inst,
                                     VOID*      pv_tag)
{
    SBTL_ARGB_WDGT_PRIVATE_T*  pt_private;
    if (NULL == pv_tag)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_ARGB_WDGT_PRIVATE_T*)pv_tag;
    if (NULL == pt_private || pt_private->h_sema == NULL_HANDLE)
    {
        return MMSBTLR_NOT_FOUND;
    }
    x_sema_lock(pt_private->h_sema, X_SEMA_OPTION_WAIT); 
    return MMSBTLR_OK;
}


static INT32 _sbtl_argb_unlock_pib_fct(HANDLE_T     h_page_inst,
                                         VOID*        pv_tag)
{
    SBTL_ARGB_WDGT_PRIVATE_T*  pt_private;
    if (NULL == pv_tag)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_ARGB_WDGT_PRIVATE_T*)pv_tag;
    if (NULL == pt_private || pt_private->h_sema == NULL_HANDLE)
    {
        return MMSBTLR_NOT_FOUND;
    }
    x_sema_unlock(pt_private->h_sema); 
    return MMSBTLR_OK;
}


static INT32 _sbtl_argb_get_pib_rgn_fct(
    HANDLE_T                  h_page_inst,
    VOID*                     pv_tag,
    UINT16                    ui2_rgn_idx,
    SBTL_WGL_SBTL_REGION_T*   pt_rgn)
{
    SBTL_ARGB_WDGT_PRIVATE_T*  pt_private;
    SBTL_ARGB_INFO_LIST_T*     pt_cur;
    UINT16                     ui2_cur_idx = 0;
    if (NULL == pv_tag || pt_rgn == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_ARGB_WDGT_PRIVATE_T*) pv_tag;
    if (pt_private->pt_img_list == NULL)
    {
        return MMSBTLR_NOT_FOUND;
    }
    pt_cur = pt_private->pt_img_list;
    while (pt_cur)
    {
        if (ui2_cur_idx == ui2_rgn_idx)
        {
            x_memset(pt_rgn, 0, sizeof(SBTL_WGL_SBTL_REGION_T));
            pt_rgn->at_clut = (GL_COLOR_T*)pt_cur->pui1_pal_data;
            pt_rgn->b_dirty = TRUE;
            pt_rgn->e_clr_mode = SBTL_WGL_SBTL_COLOR_MODE_CLUT_8_BIT;

            pt_rgn->i4_left    = (INT32)pt_cur->ui4_x;
            pt_rgn->i4_top     = (INT32)pt_cur->ui4_y;
            pt_rgn->ui4_width  = pt_cur->ui4_w;
            pt_rgn->ui4_height = pt_cur->ui4_h;

            pt_rgn->t_rc_dirty.i4_top  = 0;
            pt_rgn->t_rc_dirty.i4_left = 0;
            pt_rgn->t_rc_dirty.i4_right = (INT32)pt_cur->ui4_w;
            pt_rgn->t_rc_dirty.i4_bottom = (INT32)pt_cur->ui4_h;
            pt_rgn->pv_bitmap = pt_cur->pui1_img_data;
            
            DBG_LOG_PRINT(("[Dummy] L:%d T:%d W:%d H:%d\n", 
                pt_rgn->i4_left, pt_rgn->i4_top, pt_rgn->ui4_width, pt_rgn->ui4_height));
        
            return MMSBTLR_OK;
        }
        ui2_cur_idx++;
        pt_cur = pt_cur->pt_next;
    }
    return MMSBTLR_NOT_FOUND;
}


static BOOL _sbtl_comm_check_attr(SBTL_COMM_ATTR_LIST* pt_attr)
{
    if (pt_attr->pv_attr_value)
    {
        switch (pt_attr->e_comm_attr)
        {
            case SBTL_COMM_FONT_SIZE:
            case SBTL_COMM_TEXT_ALIGN:
            case SBTL_COMM_FONT_STYLE:
                return (pt_attr->z_attr_size == sizeof(UINT8));
                
            case SBTL_COMM_BK_FG_COLOR:
            case SBTL_COMM_BK_BG_COLOR:
            case SBTL_COMM_FONT_FG_COLOR:
            case SBTL_COMM_FONT_BG_COLOR:
            case SBTL_COMM_FONT_EDGE_COLOR:
                return (pt_attr->z_attr_size == sizeof(GL_COLOR_T));
                
            case SBTL_COMM_FONT_INFO:
                return (pt_attr->z_attr_size == sizeof(WGL_FONT_INFO_T));
            case SBTL_COMM_TEXT_CONT:
            case SBTL_COMM_ARGB_DATA:
                return TRUE;
                
            default:
                break;
        }
    }
    return FALSE;
}

INT32 sbtl_comm_set_attr(HANDLE_T h_widget, SBTL_COMM_ATTR_LIST* pt_list)
{
    INT32 i4_ret = MMSBTLR_OK;
    if (h_widget == NULL_HANDLE || pt_list == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    while (pt_list->pt_next_attr)
    {
        if (_sbtl_comm_check_attr(pt_list))
        {
            switch (pt_list->e_comm_attr)
            {
                case SBTL_COMM_FONT_INFO:
                    i4_ret = sbtl_wgl_do_cmd (h_widget,
                               SBTL_WGL_CMD_GL_SET_FONT,
                               pt_list->pv_attr_value,
                               NULL);
                    
                    break;
                case SBTL_COMM_FONT_ALIGN:
                    i4_ret = sbtl_wgl_do_cmd (h_widget,
                               SBTL_WGL_CMD_TEXT_SET_ALIGN,
                               pt_list->pv_attr_value,
                               NULL);
                     break;
                case SBTL_COMM_BK_FG_COLOR:
                    i4_ret = sbtl_wgl_do_cmd (h_widget,
                               SBTL_WGL_CMD_GL_SET_COLOR,
                               (VOID*)SBTL_WGL_CLR_BK,
                               pt_list->pv_attr_value);
                    break;
                    
                case SBTL_COMM_BK_BG_COLOR:
                    i4_ret = sbtl_wgl_do_cmd (h_widget,
                               SBTL_WGL_CMD_GL_SET_COLOR,
                               (VOID*)SBTL_WGL_CLR_BK, /* background color */
                               pt_list->pv_attr_value);
                    break;
                    
                case SBTL_COMM_FONT_EDGE_COLOR:
                     i4_ret = sbtl_wgl_do_cmd (h_widget,
                               SBTL_WGL_CMD_GL_SET_COLOR,
                               (VOID*)SBTL_WGL_CLR_TEXT, /* text color */
                               pt_list->pv_attr_value);  
                     break;

               case SBTL_COMM_TEXT_RECT:
                i4_ret = sbtl_wgl_move(h_widget, 
                               (const GL_RECT_T*)pt_list->pv_attr_value,
                               SBTL_WGL_SYNC_AUTO_REPAINT);
                break;
                
              case SBTL_COMM_ARGB_DATA:
                    break;
              default:
                 break;
            }
        }
        pt_list = pt_list->pt_next_attr;
    }
    return i4_ret;
}

INT32 sbtl_comm_get_attr(HANDLE_T h_widget, SBTL_COMM_ATTR_LIST* pt_list)
{
    return MMSBTLR_OK;
}



INT32 sbtl_comm_create_widget(
    SBTL_COMM_WDGT_INIT_PARAM_T* pt_param,
    HANDLE_T*                    ph_widget)
{
    INT32      i4_ret = MMSBTLR_OK;
    UINT32     ui4_canvas_w = 0;
    UINT32     ui4_canvas_h = 0;
    
    if (pt_param == NULL ||
        pt_param->h_plane == NULL_HANDLE ||
        ph_widget == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    i4_ret = sbtl_wgl_get_canvas_size(pt_param->h_plane, &ui4_canvas_w, &ui4_canvas_h);
    if (i4_ret != MMSBTLR_OK)
    {
        DBG_ERROR(("get canv sz ret:%d\n", i4_ret));
        return MMSBTLR_INTERNAL_ERROR;
    }

    SBTL_ARGB_WDGT_PRIVATE_T* pt_argb_pri = 
              (SBTL_ARGB_WDGT_PRIVATE_T*)x_mem_alloc(sizeof(SBTL_ARGB_WDGT_PRIVATE_T));
    if (pt_argb_pri == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    x_memset(pt_argb_pri, 0, sizeof(SBTL_ARGB_WDGT_PRIVATE_T));
    pt_argb_pri->e_comm_type = pt_param->e_comm_type;

   

    if (pt_param->e_comm_type == SBTL_COMM_TEXT)
    {
        if (pt_param->b_default)
        {
            pt_param->t_rect.i4_left   = (INT32)ui4_canvas_w / 6;
            pt_param->t_rect.i4_top    = (INT32)ui4_canvas_h / 10;
            pt_param->t_rect.i4_right  = (INT32)ui4_canvas_w - pt_param->t_rect.i4_left;
            pt_param->t_rect.i4_bottom = (INT32)ui4_canvas_h -pt_param->t_rect.i4_top;
        }
        i4_ret = sbtl_wgl_create_widget(
                    pt_param->h_plane,
                    SBTL_WGL_WIDGET_TEXT,
                    SBTL_WGL_BORDER_NULL,
                    &pt_param->t_rect,
                    255,
                    (VOID*) (SBTL_WGL_STL_TEXT_MAX_DIS_40_LINE
                            |SBTL_WGL_STL_TEXT_SMART_CUT
                            |SBTL_WGL_STL_TEXT_SMART_CUT_OVER_MAX
                            |SBTL_WGL_STL_TEXT_MULTILINE 
                            |SBTL_WGL_STL_GL_NO_IMG_UI
                            |SBTL_WGL_STL_TEXT_MAX_2048),
                    NULL,
                    ph_widget);
    }
    else if (pt_param->e_comm_type == SBTL_COMM_IMG_RGBA)
    {
        SBTL_WGL_SBTL_INIT_T   t_sbtl_init = {0};
        
        i4_ret = x_sema_create(&pt_argb_pri->h_sema, 
                           X_SEMA_TYPE_BINARY, 
                           X_SEMA_STATE_UNLOCK);
        if (i4_ret != OSR_OK)
        {
            x_mem_free(pt_argb_pri);
            return i4_ret;
        }
        if (pt_param->b_default)
        {
            pt_param->t_rect.i4_left = 0;
            pt_param->t_rect.i4_top = 0;
            pt_param->t_rect.i4_bottom = (INT32)ui4_canvas_h;
            pt_param->t_rect.i4_right = (INT32)ui4_canvas_w;
            t_sbtl_init.ui4_page_width       = 1920;
            t_sbtl_init.ui4_page_height      = 1080;
        }
        else
        {
        }
        t_sbtl_init.pf_lock_pib          = _sbtl_argb_lock_pib_fct;
        t_sbtl_init.pv_tag_lock_pib      = (VOID*) pt_argb_pri;
        t_sbtl_init.pf_unlock_pib        = _sbtl_argb_unlock_pib_fct;
        t_sbtl_init.pv_tag_unlock_pib    = (VOID*) pt_argb_pri;
        t_sbtl_init.pf_get_page_inst_rgn = _sbtl_argb_get_pib_rgn_fct;
        t_sbtl_init.pv_tag_get_rgn       = (VOID*) pt_argb_pri;
        i4_ret = sbtl_wgl_create_widget(pt_param->h_plane,
                                        SBTL_WGL_WIDGET_SBTL,
                                        SBTL_WGL_BORDER_NULL,
                                        &pt_param->t_rect,
                                        255,
                                        &t_sbtl_init,
                                        NULL,
                                        ph_widget);
    }
    if (*ph_widget != NULL_HANDLE)
    {
        x_handle_set_tag(*ph_widget, (VOID*)pt_argb_pri);
    }
    else
    {
        i4_ret = MMSBTLR_INV_ARG;
    }
    if (i4_ret != 0)
    {
        x_mem_free(pt_argb_pri);
    }
    return i4_ret;
}


INT32 sbtl_comm_show( HANDLE_T h_widget)
{
    VOID* pv_tag = NULL;
    SBTL_ARGB_WDGT_PRIVATE_T* pt_argb_pri;
    static INT32              ui4_show_cnt = 0;
    INT32   i4_ret = MMSBTLR_OK;
    if (h_widget == NULL_HANDLE)
    {
        return MMSBTLR_INV_ARG;
    }
    x_handle_get_tag (h_widget, &pv_tag);
    if (pv_tag == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_argb_pri = (SBTL_ARGB_WDGT_PRIVATE_T*) pv_tag;
    switch (pt_argb_pri->e_comm_type)
    {
        case SBTL_COMM_TEXT:
            i4_ret = sbtl_wgl_show(h_widget, TRUE); 
            break;
        case SBTL_COMM_IMG_RGBA:
            pt_argb_pri->ui8_pts = ui4_show_cnt;
            i4_ret = sbtl_wgl_do_cmd(h_widget,
                                     SBTL_WGL_CMD_SBTL_FLIP,
                                     (VOID*)pt_argb_pri,
                                     (VOID*)&pt_argb_pri->ui8_pts);
            ui4_show_cnt++;
            break;
        default:
            i4_ret = MMSBTLR_NOT_SUPPORT;
            break;
    }
    
    return i4_ret;
}
INT32 sbtl_comm_hide(HANDLE_T h_widget)
{
    VOID*                     pv_tag = NULL;
    SBTL_ARGB_WDGT_PRIVATE_T* pt_argb_pri;
    INT32                     i4_ret = MMSBTLR_OK;
    if (h_widget == NULL_HANDLE)
    {
        return MMSBTLR_INV_ARG;
    }
    x_handle_get_tag (h_widget, &pv_tag);
    if (pv_tag == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_argb_pri = (SBTL_ARGB_WDGT_PRIVATE_T*) pv_tag;
    switch (pt_argb_pri->e_comm_type)
    {
        case SBTL_COMM_TEXT:
            i4_ret = sbtl_wgl_show(h_widget, FALSE); 
            break;
        case SBTL_COMM_IMG_RGBA:
            i4_ret = sbtl_wgl_do_cmd(
                        h_widget,
                        SBTL_WGL_CMD_SBTL_CLEAR_ALL,
                        (VOID*)&pt_argb_pri->ui8_pts,
                        NULL);
            break;
        default:
            i4_ret = MMSBTLR_NOT_SUPPORT;
            break;
    }
    return i4_ret;
}

INT32 sbtl_comm_destroy_widget(HANDLE_T h_widget)
{
    VOID*                     pv_tag = NULL;
    SBTL_ARGB_WDGT_PRIVATE_T* pt_argb_pri;
    INT32                     i4_ret = MMSBTLR_OK;
    if (h_widget == NULL_HANDLE)
    {
        return MMSBTLR_INV_ARG;
    }
    x_handle_get_tag (h_widget, &pv_tag);
    if (pv_tag == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_argb_pri = (SBTL_ARGB_WDGT_PRIVATE_T*) pv_tag;
    switch (pt_argb_pri->e_comm_type)
    {
        case SBTL_COMM_TEXT:
        case SBTL_COMM_IMG_RGBA:
            i4_ret = sbtl_wgl_destroy_widget(h_widget);
            break;
        default:
            i4_ret = MMSBTLR_NOT_SUPPORT;
            break;
    }
    x_mem_free(pt_argb_pri);
    return i4_ret;
}


INT32 sbtl_comm_calc_text_rect(
    SBTL_ENGINE_T*            pt_engine,
    GL_RECT_T*                pt_rect)
{
    UINT32       ui4_canvas_w = 0;
    UINT32       ui4_canvas_h = 0;
    INT32        i4_ret = MMSBTLR_OK;
    GL_POINT_T*  pt_offset;
    
    if (pt_engine == NULL || pt_rect == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    i4_ret = sbtl_wgl_get_canvas_size(
              pt_engine->t_cfg.h_gl_plane, 
              &ui4_canvas_w, 
              &ui4_canvas_h);
    if (i4_ret != MMSBTLR_OK)
    {
        return MMSBTLR_INTERNAL_ERROR;
    }
    if (pt_engine->t_cfg.b_rect_osd)
    {
        *pt_rect = pt_engine->t_cfg.t_rect_osd;
    }          
    else
    { 
       pt_rect->i4_left   = (INT32)(ui4_canvas_w/6);
       pt_rect->i4_top    = (INT32)(ui4_canvas_h/10);
       pt_rect->i4_right  = (INT32)ui4_canvas_w - pt_rect->i4_left;
       pt_rect->i4_bottom = (INT32)ui4_canvas_h - pt_rect->i4_top;
    } 
    
    pt_offset = &(pt_engine->t_cfg.t_pos_adjust);
    pt_rect->i4_left    += pt_offset->i4_x;
    pt_rect->i4_top     += pt_offset->i4_y;
    pt_rect->i4_right   += pt_offset->i4_x;
    pt_rect->i4_bottom  += pt_offset->i4_y;
    
    if (pt_rect->i4_left < 0)
    {
        pt_rect->i4_right -= pt_rect->i4_left;
        pt_rect->i4_left = 0;
    }

    if (pt_rect->i4_top  < 0)
    {
        pt_rect->i4_bottom -= pt_rect->i4_top ;
        pt_rect->i4_top  = 0;
    }
    
    if (pt_rect->i4_right > (INT32)ui4_canvas_w)
    {
        pt_rect->i4_left -= (pt_rect->i4_right - (INT32)ui4_canvas_w);
        pt_rect->i4_right = (INT32)ui4_canvas_w;
    }

    if (pt_rect->i4_bottom > (INT32)ui4_canvas_h)
    {
        pt_rect->i4_top -= (pt_rect->i4_bottom - (INT32)ui4_canvas_h);
        pt_rect->i4_bottom = (INT32)ui4_canvas_h;
    }
    return MMSBTLR_OK;
}

INT32 sbtl_comm_create_text_wdgt(
    SBTL_ENGINE_T*            pt_engine,
    HANDLE_T*                 ph_widget)
{

    SBTL_COMM_ATTR_LIST         t_attr[10];
    SBTL_COMM_WDGT_INIT_PARAM_T t_init_param;

    HANDLE_T                    h_widget = NULL_HANDLE;
    UINT32                      ui4_i;
    INT32                       i4_ret = MMSBTLR_OK;

    if (pt_engine == NULL || ph_widget == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    x_memset(t_attr, 0, sizeof(t_attr));
    t_init_param.h_plane = pt_engine->t_cfg.h_gl_plane;
    
    t_init_param.e_comm_type = SBTL_COMM_TEXT;
    t_init_param.h_plane = pt_engine->t_cfg.h_gl_plane;
    i4_ret = sbtl_comm_calc_text_rect(pt_engine, &t_init_param.t_rect);
    i4_ret = sbtl_comm_create_widget(&t_init_param, &h_widget);
    if (i4_ret != MMSBTLR_OK || h_widget == NULL_HANDLE)
    {
        return MMSBTLR_WGL_FAIL;
    }
    for (ui4_i = 0; ui4_i < 9; ui4_i++)
    {
        t_attr[ui4_i].pt_next_attr = &t_attr[ui4_i+1];
    }
    
    ui4_i = 0;
    t_attr[ui4_i].e_comm_attr   = SBTL_COMM_FONT_INFO;
    t_attr[ui4_i].pv_attr_value = &(pt_engine->t_cfg.t_font_info);
    t_attr[ui4_i].z_attr_size   = sizeof(WGL_FONT_INFO_T);
    ui4_i++;

    t_attr[ui4_i].e_comm_attr   = SBTL_COMM_TEXT_ALIGN;
    t_attr[ui4_i].pv_attr_value = &(pt_engine->t_cfg.ui1_text_alignment);
    t_attr[ui4_i].z_attr_size   = sizeof(UINT8);
    ui4_i++;
    
    t_attr[ui4_i].e_comm_attr   = SBTL_COMM_BK_BG_COLOR;
    t_attr[ui4_i].pv_attr_value = &(pt_engine->t_cfg.t_bg_color_info);
    t_attr[ui4_i].z_attr_size   = sizeof(GL_COLOR_T);
    ui4_i++;

    t_attr[ui4_i].e_comm_attr   = SBTL_COMM_BK_FG_COLOR;
    t_attr[ui4_i].pv_attr_value = &(pt_engine->t_cfg.t_fg_color_info);
    t_attr[ui4_i].z_attr_size   = sizeof(GL_COLOR_T);
    ui4_i++;

    t_attr[ui4_i].e_comm_attr   = SBTL_COMM_FONT_BG_COLOR;
    t_attr[ui4_i].pv_attr_value = &(pt_engine->t_cfg.t_font_bg_color);
    t_attr[ui4_i].z_attr_size   = sizeof(GL_COLOR_T);
    ui4_i++;

    t_attr[ui4_i].e_comm_attr   = SBTL_COMM_FONT_EDGE_COLOR;
    t_attr[ui4_i].pv_attr_value = &(pt_engine->t_cfg.t_edge_color_info);
    t_attr[ui4_i].z_attr_size   = sizeof(GL_COLOR_T);
    t_attr[ui4_i].pt_next_attr = NULL;
    i4_ret = sbtl_comm_set_attr(h_widget, t_attr);
    if (i4_ret != MMSBTLR_OK)
    {
        DBG_ERROR(("Set text attr ret:%d\n", i4_ret));
    }
    return MMSBTLR_OK;
}

#endif /*__KERNEL__*/


