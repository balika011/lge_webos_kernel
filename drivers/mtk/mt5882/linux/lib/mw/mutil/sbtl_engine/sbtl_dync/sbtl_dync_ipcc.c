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


#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_reg.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_util.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_ipcc.h"
#include "mutil/sbtl_engine/sbtl_xml_parser.h"
#include "mutil/sbtl_engine/sbtl_xml/sbtl_xml_page_inst_mngr.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_ipcc_util.h"
#include "x_sbtl_ipcc.h"
#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "u_img.h"
#include "x_img.h"
#include "x_gl.h"
#include "u_wgl.h"
#include "x_wgl.h"

typedef struct
{
    UINT32   ui4_x;
    UINT32   ui4_y;
    UINT32   ui4_w;
    UINT32   ui4_h;
    HANDLE_T h_region;
    HANDLE_T h_style;
    UINT16   ui2_sel_img;
    CHAR*    ps_img_id;
    BOOL     b_force_disp;
}IPCC_IMG_ATTR_FLT_T;

//support show one image at one time

typedef struct
{
    HANDLE_T h_plane;
    HANDLE_T h_screen;
    HANDLE_T h_view;
    HANDLE_T h_surf;
    UINT32   ui4_viewport;
    BOOL     b_cc_created;
    HANDLE_T h_canvas;
    WGL_CANVAS_INFO_T t_keep_info;
    HANDLE_T h_img_surf;
    UINT32   ui4_surf_w;
    UINT32   ui4_surf_h;
    GL_RECT_T t_surf_pos;
}IPCC_IMG_RES_T;


#define IPCC_MAX_CMDS_CNT (10)

#define IPCC_IMG_SURF_INIT_WIDTH  (900)
#define IPCC_IMG_SURF_INIT_HEIGHT (300)

#define IPCC_SUPPORT_TWO_IMG_BUFFER

static INT32 _sbtl_free_one_p(SBTL_TTML_ELEMENT_P_T*  pt_p_list)
{
    if (pt_p_list)
    {
        SBTL_TTML_ELEMENT_SPAN_T*    pt_span_list;
        SBTL_TTML_ATTR_T*            pt_attr_list;
        
        pt_span_list = pt_p_list->t_element_span_list.pt_head;
        
        while(pt_span_list  != NULL) 
        {                                
            SBTL_TTML_ELEMENT_SPAN_T*       pt_span_list_cur = NULL;
            pt_span_list_cur = pt_span_list;
            pt_attr_list = (pt_span_list->t_attr_list).pt_head;
            while(pt_attr_list != NULL)
            {                         
                SBTL_TTML_ATTR_T*        pt_attr_list_cur = pt_attr_list;
                SLIST_REMOVE(pt_attr_list,t_attr_link);                       
                x_mem_free(pt_attr_list_cur);         
                pt_attr_list = (pt_span_list->t_attr_list).pt_head;                      
            }

            SLIST_REMOVE(pt_span_list,t_element_span_link);   
            pt_span_list = pt_p_list->t_element_span_list.pt_head;                
            x_mem_free(pt_span_list_cur);   
        }
        
        SLIST_REMOVE(pt_p_list, t_element_p_link);                  
        x_mem_free(pt_p_list);
    }
    return MMSBTLR_OK;
}

static INT32 _sbtl_cc_show(
    SBTL_IPCC_PARSER_T*       pt_parser,
    MM_SBTL_M4T_IDX_INFO_T*   pt_idx,
    BOOL                      b_reset_head,
    SBTL_SHOW_TYPE_T          e_show_mode)
{
    INT32                      i4_ret = MMSBTLR_OK;
    SBTL_TTML_ELEMENT_P_T*     pt_p_list = NULL;        
    SBTL_TTML_ELEMENT_SPAN_T*  pt_span_list = NULL;
    SBTL_TTML_ATTR_T*          pt_attr_list = NULL;
    UINT8                      ui1_p_num = 0;
    BOOL                       b_skip = FALSE;
        

    if (b_reset_head)
    {
        BOOL    b_sample_end = TRUE;
        i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_CRNT_SAMPLE_END,
                                 (VOID *)&b_sample_end ,
                                 sizeof(BOOL));   
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_ERROR(("====send data fail %d====\n", i4_ret));
        }
        
        i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_GLOBAL_STYLE,
                             (VOID *)&pt_idx->t_common_id_attr_list,
                             sizeof(SBTL_TTML_ATTR_ID_LIST_T));
        if(MMSBTLR_OK != i4_ret)
        {
            DBG_ERROR(("x_sbtl_ipcc_set_fct() SBTL_IPCC_SET_TYPE_GLOBAL_STYLE FAIL! ret:%d\n", i4_ret));
        }  
    }
 
    pt_p_list = SLIST_FIRST(&pt_parser->t_element_p_list);
    while (pt_p_list)
    {   
        DBG_INFO(("============Load %d <p>============\n",ui1_p_num));
        b_skip = FALSE;
        SLIST_FOR_EACH(pt_attr_list,&(pt_p_list->t_common_attr_list),t_attr_link)
        {
            sbtl_xml_dump_attrs(pt_attr_list); 
            if (pt_attr_list->e_attr == SBTL_XML_IPCC_ATTR_DISP_MODE)
            {
                if (e_show_mode != SBTL_SHOW_TYPE_FORCE &&
                    x_strcasecmp(pt_attr_list->u.ps_value, "TRUE") == 0)
                {
                    b_skip = TRUE;
                }
            }
        }
        if (b_skip)
        {
            SBTL_TTML_ELEMENT_P_T*     pt_p_next = pt_p_list->t_element_p_link.pt_next;
            _sbtl_free_one_p(pt_p_list);
            pt_p_list = pt_p_next;
            DBG_INFO(("Free none forced\n"));
            continue;
        }
        SLIST_FOR_EACH(pt_span_list,&(pt_p_list->t_element_span_list),t_element_span_link)
        {               
            DBG_INFO(("Content:%s\n<span attrs>\n",pt_span_list->ps_content));             
            SLIST_FOR_EACH(pt_attr_list,&(pt_span_list->t_attr_list),t_attr_link)
            {                   
                sbtl_xml_dump_attrs(pt_attr_list);
            }                           
        } 
        pt_p_list = pt_p_list->t_element_p_link.pt_next;
        ui1_p_num++;
    }
    if (SLIST_IS_EMPTY(&pt_parser->t_element_p_list))
    {
        return MMSBTLR_NOT_ALLOW;
    }
    DBG_INFO(("===begin to send data to ipcc!====\n"));
    i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_DATA,
                                (VOID *)&pt_parser->t_element_p_list, 
                                 sizeof(SBTL_TTML_DATA_T));
    if (MMSBTLR_OK != i4_ret)
    {                    
        DBG_INFO(("===Send data fail return (%d)====\n",i4_ret));
    }
    return i4_ret;
}



 static INT32  _sbtl_cc_hide(SBTL_IPCC_PARSER_T*   pt_parser)
{
    SBTL_TTML_ELEMENT_P_T*          pt_p_list = NULL;
    SBTL_TTML_ATTR_T*               pt_com_attr_list;
    INT32                           i4_ret = 0;
    BOOL                            b_is_remove = FALSE;

    if (SLIST_IS_EMPTY(&pt_parser->t_element_p_list))
    {
        return MMSBTLR_OK;
    }
    
    pt_p_list = pt_parser->t_element_p_list.pt_head;     
    while(pt_p_list != NULL)
    {    
        
        b_is_remove = FALSE;
        pt_com_attr_list = (pt_p_list->t_common_attr_list).pt_head;
        while(pt_com_attr_list != NULL)
        {                         
            SBTL_TTML_ATTR_T*        pt_attr_list_cur = pt_com_attr_list;
            if(pt_parser->b_showing  &&
                b_is_remove == FALSE && 
                pt_attr_list_cur->e_attr == SBTL_XML_IPCC_ATTR_REGION)
            {                        
                i4_ret = x_sbtl_ipcc_remove(pt_attr_list_cur->u.ps_value);
                DBG_INFO(("Remove region %s ret %d\n",pt_attr_list_cur->u.ps_value,i4_ret));
                b_is_remove = TRUE;
            }
                
            SLIST_REMOVE(pt_com_attr_list,t_attr_link);                          
            x_mem_free(pt_attr_list_cur);         
            pt_com_attr_list = (pt_p_list->t_common_attr_list).pt_head;                       
        }
        
        if (pt_parser->b_showing && b_is_remove == FALSE)
        {
            i4_ret = x_sbtl_ipcc_remove("undefine");
            DBG_INFO(("Remove default region ret %d\n",i4_ret));
        }
        _sbtl_free_one_p(pt_p_list);
        pt_p_list = pt_parser->t_element_p_list.pt_head;     
    }
    
    pt_parser->b_showing = FALSE;

    return i4_ret;
}



static INT32 ipcc_destroy_img_res(IPCC_IMG_RES_T*  pt_res)
{
    if (pt_res)
    {
        INT32 i4_ret = MMSBTLR_OK;
        if (pt_res->h_view != NULL_HANDLE)
        {
             i4_ret = x_gl_view_delete_viewport(pt_res->h_view, pt_res->ui4_viewport);
             
             i4_ret = x_gl_obj_free(pt_res->h_view);
        }
        if (pt_res->h_surf != NULL_HANDLE)
        {
            i4_ret = x_gl_obj_free(pt_res->h_surf);
        }
        
#ifdef IPCC_SUPPORT_TWO_IMG_BUFFER
        if (pt_res->h_img_surf != NULL_HANDLE)
        {
            i4_ret = x_gl_obj_free(pt_res->h_img_surf);
        }
#endif
        
        if (pt_res->h_screen != NULL_HANDLE)
        {
            x_handle_free(pt_res->h_screen);
            pt_res->h_screen = NULL_HANDLE;
        }
        {
            WGL_CANVAS_CFG_T    t_cfg     = {0};

            t_cfg.ui4_flag     = WGL_CANVAS_CFG_AUTOSCALE | WGL_CANVAS_CFG_COLORMODE | WGL_CANVAS_CFG_WIDTH | WGL_CANVAS_CFG_HEIGHT;
            t_cfg.b_auto_scale = TRUE;
            /*restore setting*/
            t_cfg.ui4_width    = pt_res->t_keep_info.ui4_width;
            t_cfg.ui4_height   = pt_res->t_keep_info.ui4_height;
            t_cfg.e_clr_mode   = pt_res->t_keep_info.e_clr_mode;
    
            x_gl_plane_enable(pt_res->h_plane, FALSE);
            x_gl_plane_change_resolution_cm(pt_res->h_plane, t_cfg.ui4_width, t_cfg.ui4_height, t_cfg.e_clr_mode);
            x_wgl_canvas_reconfig(pt_res->h_canvas, pt_res->h_plane, &t_cfg);
            x_gl_plane_enable(pt_res->h_plane, TRUE);
        }
        
        if (NULL_HANDLE != pt_res->h_plane)
        {
            x_handle_free(pt_res->h_plane);
            pt_res->h_plane = NULL_HANDLE;
        }
        
        return MMSBTLR_OK;
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
    
}

static INT32 ipcc_create_img_res(IPCC_IMG_RES_T* pt_res)
{
    INT32                i4_ret = MMSBTLR_OK;
    GL_SURFACE_DESC_T    t_surf_descr = {0};
    UINT32               ui4_w = 0, ui4_h = 0;
    GL_DISPLAY_OPT_T     t_dsp_opt = {0};
    GL_POINT_T           t_screen_offset = {0};
    
    if (pt_res->h_surf != NULL_HANDLE && pt_res->h_view != NULL_HANDLE)
    {
        return MMSBTLR_OK;
    }

    do
    {
        GL_COLOR_T t_color = {0, {0}, {0}, {0}};
        i4_ret = x_gl_screen_get_panel_size(NULL_HANDLE, &ui4_w, &ui4_h);
        if (ui4_w == 0 || ui4_h == 0)
        {
            DBG_ERROR(("get panel size w:%d h:%d\n", ui4_w, ui4_h));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
        }

        i4_ret = x_gl_screen_open(SN_PRES_MAIN_DISPLAY,                                   
                           NULL,                                   
                           NULL,                                   
                           &pt_res->h_screen); 
         if (GL_FAILED(i4_ret))
         {
            DBG_ERROR(("open screen ret:%d\n", i4_ret));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
         } 
             
         i4_ret = x_gl_plane_open (pt_res->h_screen, 0, &pt_res->h_plane);
         if (GL_FAILED(i4_ret))
         {
            DBG_ERROR(("open plane ret:%d\n", i4_ret));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
         }
         
        i4_ret = x_gl_plane_link (pt_res->h_plane, pt_res->h_screen, TRUE);
        if (GL_FAILED(i4_ret))
        {
            DBG_ERROR(("link plane ret:%d\n", i4_ret));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
        }
        
        t_surf_descr.e_clrmode  = GL_COLORMODE_ARGB_D8888;
        t_surf_descr.ui4_width  = ui4_w;
        t_surf_descr.ui4_height = ui4_h; 
        //first create surface
        DBG_LOG_PRINT(("begine create surf w:%d h:%d\n", ui4_w, ui4_h));
        
        i4_ret = x_gl_surface_create(&t_surf_descr, &pt_res->h_surf); 
        if (NULL_HANDLE == pt_res->h_surf)
        {
            DBG_ERROR(("create surf ret:%d\n", i4_ret));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
        }
        x_gl_surface_fill(pt_res->h_surf, &t_color, GL_SYNC);
        
#ifdef IPCC_SUPPORT_TWO_IMG_BUFFER
        //create double buffer for load image
        t_surf_descr.ui4_width  = IPCC_IMG_SURF_INIT_WIDTH;
        t_surf_descr.ui4_height = IPCC_IMG_SURF_INIT_HEIGHT;
        
        i4_ret = x_gl_surface_create(&t_surf_descr, &pt_res->h_img_surf); 
        if (NULL_HANDLE == pt_res->h_img_surf)
        {
            DBG_ERROR(("create img surf ret:%d\n", i4_ret));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
        }
        x_gl_surface_fill(pt_res->h_img_surf, &t_color, GL_SYNC);
        pt_res->t_surf_pos.i4_left = 0;
        pt_res->t_surf_pos.i4_top  = 0;
        pt_res->t_surf_pos.i4_right  = IPCC_IMG_SURF_INIT_WIDTH;
        pt_res->t_surf_pos.i4_bottom = IPCC_IMG_SURF_INIT_HEIGHT;
        pt_res->ui4_surf_w = IPCC_IMG_SURF_INIT_WIDTH;
        pt_res->ui4_surf_h = IPCC_IMG_SURF_INIT_HEIGHT;
#endif     
            

        // i4_ret = x_gl_plane_reorder(h_plane, GL_REORDER_ABSOLUTE, 0);
        t_dsp_opt.e_flag            = GL_DISP_OPT_POSITION | GL_DISP_OPT_BLEND_OPTION | GL_DISP_OPT_ALPHA;
        t_dsp_opt.e_blend_option    = GL_BLEND_OPT_PIXEL;
        t_dsp_opt.ui1_alpha         = 255;
        t_dsp_opt.t_pos             = t_screen_offset;

        i4_ret = x_gl_view_create(ui4_w, ui4_h, TRUE, &pt_res->h_view);
        if (NULL_HANDLE == pt_res->h_view)
        {
            DBG_ERROR(("create view ret:%d\n", i4_ret));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
        }

        i4_ret = x_gl_view_create_viewport(
                        pt_res->h_view,
                        pt_res->h_surf,
                        &t_dsp_opt, 
                        &pt_res->ui4_viewport);
        if(GLR_OK != i4_ret)
        {
            DBG_ERROR(("create viewport ret:%d\n", i4_ret));
            i4_ret = MMSBTLR_WGL_FAIL;
            break;
        }
        //to flip
        #ifdef IPCC_SUPPORT_TWO_IMG_BUFFER
        x_gl_plane_flip(pt_res->h_plane, pt_res->h_view, TRUE);
        #endif
        x_wgl_get_canvas_info(pt_res->h_canvas, &pt_res->t_keep_info);
        
    }while(0);

    return i4_ret; 
}

static INT32 sbtl_ipcc_img_show(IPCC_IMG_RES_T* pt_res, BOOL b_show)
{
    if (pt_res &&
        pt_res->h_plane     != NULL_HANDLE &&
        pt_res->h_view      != NULL_HANDLE &&
        pt_res->h_surf      != NULL_HANDLE 
        #ifdef IPCC_SUPPORT_TWO_IMG_BUFFER
        &&  pt_res->h_img_surf  != NULL_HANDLE
        #endif
    )
    {
        INT32 i4_ret = MMSBTLR_OK;
        if (b_show)
        {
            #ifdef IPCC_SUPPORT_TWO_IMG_BUFFER
            if (pt_res->t_surf_pos.i4_right  > pt_res->t_surf_pos.i4_left &&
                pt_res->t_surf_pos.i4_bottom > pt_res->t_surf_pos.i4_top)
            {
                i4_ret = x_gl_surface_blt(
                              pt_res->h_surf,
                              pt_res->h_img_surf,
                              pt_res->t_surf_pos.i4_left,
                              pt_res->t_surf_pos.i4_top,
                              0,
                              0,
                              (pt_res->t_surf_pos.i4_right - pt_res->t_surf_pos.i4_left),
                              (pt_res->t_surf_pos.i4_bottom- pt_res->t_surf_pos.i4_top),
                              255,
                              GL_SYNC);
                //i4_ret = x_gl_plane_flip(pt_res->h_plane, pt_res->h_view, TRUE);
            }
            #else
            i4_ret = x_gl_plane_flip(pt_res->h_plane, pt_res->h_view, TRUE);
            #endif
        }
        else
        {
            #ifdef IPCC_SUPPORT_TWO_IMG_BUFFER
            GL_COLOR_T t_color = {0, {0}, {0}, {0}};
            x_gl_surface_fill(pt_res->h_surf, &t_color, GL_SYNC);
            //x_gl_surface_fill(pt_res->h_img_surf, &t_color, GL_SYNC);
            #else
            i4_ret = x_gl_plane_flip(pt_res->h_plane, NULL_HANDLE, TRUE);
            #endif
        }
        return i4_ret;
    }
    return MMSBTLR_INV_ARG;
}

static INT32 _sbtl_ipcc_iter_img_attr(
    XML_ATTRIBUTE_T*   pt_attr,
    VOID*              pv_arg,
    INT32              i4_depth)
{
    IPCC_IMG_ATTR_FLT_T* pt_flt =(IPCC_IMG_ATTR_FLT_T*)pv_arg;
    CHAR*                ps_str = NULL;
    UINT16               ui2_start[2];
    UINT16               ui2_end[2];
    MEM_T                t_mem;
    
    t_mem.start    = ui2_start;
    t_mem.end      = ui2_end;
    t_mem.len      = 2; 
    
    if (pt_attr && pt_flt)
    {
        if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_ORIGIN)
        {
            ps_str = pt_attr->ps_value;
        }
        if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_FONT_EXTENT)
        {
            ps_str = pt_attr->ps_value;
        }
        if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_BGIMG)
        {
            ps_str = pt_attr->ps_value;
            if (match_reg_exp("subtitleimageindex: *(\\d+)", ps_str, &t_mem, NULL))
            {
                pt_flt->ui2_sel_img = (UINT16)x_strtoll(ps_str + t_mem.start[0], NULL, 10);
            }
            else
            {
                //skip leadind #
                pt_flt->ps_img_id = (pt_attr->ps_value[0] == '#')? pt_attr->ps_value+1: pt_attr->ps_value;
            }
            return MMSBTLR_OK;
        }
        if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_DISP_MODE)
        {
            //handle disp mode settings
            pt_flt->b_force_disp = (x_strcasecmp(pt_attr->ps_value, "TRUE") == 0);
            return MMSBTLR_OK;
        }
        
       // "1920px 1080px"
        if (ps_str)
        {
            if (match_reg_exp("^ *(\\d+)px *(\\d+)px", ps_str, &t_mem, NULL))
            {
                if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_ORIGIN)
                {
                    pt_flt->ui4_x  = (INT32)x_strtoll(ps_str + t_mem.start[0], NULL, 10);
                    pt_flt->ui4_y  = (INT32)x_strtoll(ps_str + t_mem.start[1], NULL, 10);
                }
                else
                {
                    pt_flt->ui4_w   = (UINT32)x_strtoll(ps_str + t_mem.start[0], NULL, 10);
                    pt_flt->ui4_h   = (UINT32)x_strtoll(ps_str + t_mem.start[1], NULL, 10);
                }
            }
            else
            {
                DBG_ERROR(("Invalid attr %s\n", ps_str));
            }
        }
            

        if ( pt_attr->i4_type == SBTL_XML_IPCC_ATTR_STYLE ||
             pt_attr->i4_type ==  SBTL_XML_IPCC_ATTR_REGION)
        {
            XML_T*        pt_cur   = NULL;
            HANDLE_T      h_handle = pt_flt->h_style;
            INT32         i4_ret;
            if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_REGION)
            {
                h_handle = pt_flt->h_region;
            }
            if (h_handle != NULL_HANDLE)
            {
                i4_ret = hash_get_obj(h_handle,
                                     (INT8*)pt_attr->ps_value,
                                     x_strlen(pt_attr->ps_value),
                                     (VOID**)&pt_cur);
                if(i4_ret == 0 && pt_cur && i4_depth < 4)
                {
                     x_sbtl_xml_iterate_all_attr(pt_cur,
                                     _sbtl_ipcc_iter_img_attr,
                                     pv_arg,
                                     i4_depth + 1); 
                }
            }  
        }     
    }    
    return MMSBTLR_OK; 
}

static INT32 sbtl_ipcc_load_img(
    IPCC_IMG_RES_T*         pt_res, 
    XML_T*                  pt_cur,
    MM_SBTL_M4T_IDX_INFO_T* pt_idx,
    SBTL_SHOW_TYPE_T        e_show_mode)
{
    INT32               i4_ret = MMSBTLR_OK;
    IPCC_IMG_ATTR_FLT_T t_img_flt;
    HANDLE_T            h_img = NULL_HANDLE;
    UINT16              ui2_i;

    if (pt_cur == NULL ||
        pt_idx == NULL ||
        pt_res == NULL)
    {
    
        return MMSBTLR_INV_ARG;
    }
    if (pt_res->h_plane == NULL_HANDLE ||
        pt_res->h_view  == NULL_HANDLE ||
        pt_res->h_surf  == NULL_HANDLE)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    x_memset(&t_img_flt, 0, sizeof(IPCC_IMG_ATTR_FLT_T));
    t_img_flt.h_region = pt_idx->h_region;
    t_img_flt.h_style = pt_idx->h_style;

    if (pt_idx->pt_image_table == NULL ||
        pt_idx->ui2_imge_num == 0)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    
    
    if (pt_cur->pt_parent)//parent is body or parent is DIV
        //pt_cur->pt_parent->i4_type == pt_cur->i4_type)
    {
        x_sbtl_xml_iterate_all_attr(pt_cur->pt_parent,
                                _sbtl_ipcc_iter_img_attr,
                                 (VOID*)&t_img_flt,
                                 0); 
    }
    x_sbtl_xml_iterate_all_attr(pt_cur,
                                _sbtl_ipcc_iter_img_attr,
                                ( VOID*)&t_img_flt,
                                 0);
    #if 0
    if (t_img_flt.b_force_disp && e_show_mode != SBTL_SHOW_TYPE_FORCE)
    {
        return MMSBTLR_OK;
    }
    #endif
    if (t_img_flt.ps_img_id != NULL)
    {
        for (ui2_i = 0; ui2_i <  pt_idx->ui2_imge_num; ui2_i++ )
        {
            if (x_strcmp(pt_idx->pt_image_table[ui2_i].ps_image_id, t_img_flt.ps_img_id) == 0)
            {
                h_img = pt_idx->pt_image_table[ui2_i].h_img;
                break;
            }
        }
    }
    else if (t_img_flt.ui2_sel_img > 0 &&
             t_img_flt.ui2_sel_img <= pt_idx->ui2_imge_num)
    {
        h_img =  pt_idx->pt_image_table[t_img_flt.ui2_sel_img - 1].h_img;
    }
    DBG_INFO(("Try to load image%d: x:%d y:%d w:%d h:%d at:%d\n", 
              t_img_flt.ui2_sel_img, t_img_flt.ui4_x,  t_img_flt.ui4_y,
              t_img_flt.ui4_w, t_img_flt.ui4_h, sbtl_cur_time()));
    //todo: should add sort and  support multi-image loading
    //here just do overwrite
   
    if (h_img != NULL_HANDLE)
    {
        UINT32 ui4_img_w = 0;
        UINT32 ui4_img_h = 0;
        i4_ret = x_img_get_dimension(
                        h_img,
                        INTRA_IMG_PRIMARY_IMG_TYPE,
                        &ui4_img_w,
                        &ui4_img_h);
        if (i4_ret != GLR_OK)
        {
            return i4_ret;
        }
        if (t_img_flt.ui4_w == 0 || t_img_flt.ui4_h == 0)
        {
            t_img_flt.ui4_w = ui4_img_w;
            t_img_flt.ui4_h = ui4_img_h; 
        }
        #ifdef IPCC_SUPPORT_TWO_IMG_BUFFER
        if (t_img_flt.ui4_w > pt_res->ui4_surf_w ||
            t_img_flt.ui4_h > pt_res->ui4_surf_h ||
             pt_res->h_img_surf == NULL_HANDLE)
        {
            DBG_ERROR(("Image size too large\n"));
            return MMSBTLR_OK;
        }
        //currently just do overwrite
        pt_res->t_surf_pos.i4_left   = (INT32)t_img_flt.ui4_x;
        pt_res->t_surf_pos.i4_top    = (INT32)t_img_flt.ui4_y;
        pt_res->t_surf_pos.i4_right  = (INT32)(t_img_flt.ui4_x + t_img_flt.ui4_w);
        pt_res->t_surf_pos.i4_bottom = (INT32)(t_img_flt.ui4_y + t_img_flt.ui4_h);
        i4_ret = x_img_sync_load(h_img,
                            INTRA_IMG_PRIMARY_IMG_TYPE,
                            NULL,
                            IMG_ROTATE_CLOCKWS_0_OP_TYPE,
                            0,
                            0,
                            ui4_img_w,
                            ui4_img_h,
                            0,//t_img_flt.ui4_x,
                            0,//t_img_flt.ui4_y,
                            t_img_flt.ui4_w,
                            t_img_flt.ui4_h,
                            pt_res->h_img_surf);
        
        #else
            
        i4_ret = x_img_sync_load(h_img,
                            INTRA_IMG_PRIMARY_IMG_TYPE,
                            NULL,
                            IMG_ROTATE_CLOCKWS_0_OP_TYPE,
                            0,
                            0,
                            ui4_img_w,
                            ui4_img_h,
                            t_img_flt.ui4_x,
                            t_img_flt.ui4_y,
                            t_img_flt.ui4_w,
                            t_img_flt.ui4_h,
                            pt_res->h_surf);
        #endif
        if (i4_ret != GLR_OK)
        {
            DBG_ERROR(("Sync load img ret:%d\n", i4_ret));
        }
    }
    
    return i4_ret;
}

static INT32 ipcc_create_cc_widget( 
    SBTL_ENGINE_T*            pt_engine,
    MM_SBTL_M4T_IDX_INFO_T*   pt_idx)
{
    XML_NODE_T*               pt_node_p = NULL;
    XML_T*                    pt_first_p = NULL;
    XML_T*                    pt_head   = NULL;
    XML_T*                    pt_info   = NULL;
    SBTL_XML_COMMAND_T        t_cmds[IPCC_MAX_CMDS_CNT];
    UINT8                     ui1_idx = 0;
    UINT8                     ui1_lines = 0;
    INT32                     i4_ret = MMSBTLR_OK;
    if (pt_engine == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
   
    x_memset(t_cmds, 0, sizeof(t_cmds));
    
    pt_head = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_HEAD, pt_idx->pt_xml_tree);
    if (pt_idx->ui2_imge_num > 0)
    {
        pt_node_p = pt_idx->pt_div_nodes;
    }
    else
    {
        pt_node_p = pt_idx->pt_p_nodes;
    }
    
    if (pt_node_p)
    {
        pt_first_p = x_sbtl_xml_get_element_byidx(pt_node_p,  0);
    }
    
    t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_ASPECT_RATIO;
    t_cmds[ui1_idx++].u.ps_text = "16:9";
    if (pt_first_p)
    {
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_FIRST_P_EXTENT;
        t_cmds[ui1_idx++].u.ps_text = sbtl_xml_get_attr_extent(
                                       pt_idx,
                                       pt_first_p,
                                       &ui1_lines);
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_FIRST_P_LINES;
        t_cmds[ui1_idx++].u.ui1_data = ui1_lines;
    }

    if (pt_engine->t_cfg.t_display_info.b_display_info)
    {
        GL_RECT_T* pt_rect    = &pt_engine->t_cfg.t_display_info.t_rect_refer;
        UINT16     ui2_width  = (UINT16)(pt_rect->i4_right-pt_rect->i4_left);
        UINT16     ui2_height = (UINT16)(pt_rect->i4_bottom-pt_rect->i4_top);
        
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_RESOLUTION;
        t_cmds[ui1_idx++].u.ui4_data = ((UINT32)ui2_width<<16)+ui2_height;
       
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_TYPE;
        if (pt_engine->t_cfg.t_display_info.ui4_src_height >= 720)
        {
            t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_VIDEO_TYPE_HD;
        }
        else
        {
            t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_VIDEO_TYPE_SD;
        }
        DBG_LOG_PRINT(("Get width %d height %d vid height %d ",
            ui2_width, ui2_height, pt_engine->t_cfg.t_display_info.ui4_src_height));
    }
    else
    {
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_RESOLUTION;
        t_cmds[ui1_idx++].u.ui4_data = (1920<<16) + 1080;
        t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_VIDEO_TYPE;
        t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_VIDEO_TYPE_HD;
    }
     t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_SET_FILE_TYPE;
    if (pt_idx->b_is_m4t)
    {
        t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_FILE_TYPE_SENVU;
    }
    else
    {
        t_cmds[ui1_idx++].u.ui4_data = (UINT32)SBTL_XML_FILE_TYPE_CRACKLE;
    }
    
    t_cmds[ui1_idx].e_code     = SBTL_XML_CMD_CODE_SET_FILE_MODE;
    t_cmds[ui1_idx].u.ui4_data = (UINT32)SBTL_XML_FILE_MODE_PRESERVED;

    pt_info = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_INFO, pt_head);
    if (pt_info)
    {
        XML_ATTRIBUTE_T*     pt_attr = NULL;
        pt_attr = x_sbtl_xml_get_first_attr(pt_info);
        while(pt_attr)
        {
            if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_MODE &&
                x_strcasecmp(pt_attr->ps_value, "enhanced") == 0)
            {
                t_cmds[ui1_idx].u.ui4_data = (UINT32)SBTL_XML_FILE_MODE_ENHANCED;
            }
            pt_attr = x_sbtl_xml_get_next_attr(pt_attr);
        }
    }
    
    DBG_LOG_PRINT(("mode %d\n",t_cmds[ui1_idx].u.ui4_data));
    ui1_idx++;
    t_cmds[ui1_idx].e_code = SBTL_XML_CMD_CODE_END;

    i4_ret = x_sbtl_ipcc_open_fct(pt_engine->t_cfg.h_gl_plane,  (VOID*)&t_cmds);
    DBG_LOG_PRINT(("===x_sbtl_ipcc_open_fct return %d===\n", i4_ret)); 
    if (i4_ret != MMSBTLR_OK)
    {
        x_sbtl_ipcc_close_fct();
    }
    else
    {
         i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_GLOBAL_STYLE,
                             (VOID *)&pt_idx->t_common_id_attr_list,
                             sizeof(SBTL_TTML_ATTR_ID_LIST_T));
        if(MMSBTLR_OK != i4_ret)
        {
            DBG_ERROR(("x_sbtl_ipcc_set_fct() SBTL_IPCC_SET_TYPE_GLOBAL_STYLE FAIL! ret:%d\n", i4_ret));
        } 
    }
    return i4_ret;
}


static INT32 _sbtl_m4t_build_idx(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    if (pt_info == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    if (pt_info->pui1_file_buff && pt_info->ui4_file_size > 0)
    {
        return m4t_mem_build_idx(pt_info);
    }
    else
    {
        return sbtl_m4t_file_build_idx(pt_info);
    }
}

static INT32 _sbtl_xml_build_idx(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    if (pt_info == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    if (pt_info->pui1_file_buff && pt_info->ui4_file_size > 0)
    {
        return xml_mem_build_idx(pt_info);
    }
    else
    {
        return xml_file_build_idx(pt_info);
    }
}

static INT32 _sbtl_ipcc_parse_sample(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT32                    ui4_idx)
{
    if (pt_info &&
        pt_info->pv_samle_index  &&
        ui4_idx < pt_info->ui4_sample_num)
    {
        MM_SBTL_M4T_IDX_INFO_T* pt_idx = (MM_SBTL_M4T_IDX_INFO_T*)pt_info->pv_samle_index[ui4_idx];
        SBTL_IPCC_PARSER_T*     pt_parser = (SBTL_IPCC_PARSER_T* )pt_info->pv_private;
   
        if (pt_parser == NULL   ||
            pt_idx    == NULL   ||
            pt_idx->pt_xml_tree || 
            pt_idx->b_bad_sample)
        {
            return MMSBTLR_OK;
        }
        if (pt_info->pui1_file_buff == NULL)
        {
            return sbtl_xml_file_parse ( pt_info, pt_idx );
        }
        else if (pt_info->ui4_file_size > 0)
        {
            if (pt_idx->t_base.ui4_offset + pt_idx->t_base.ui4_size >= pt_info->ui4_file_size)
            {
                return MMSBTLR_NOT_ALLOW;
            }
            return  sbtl_xml_parse_sample(pt_idx,
                              pt_info->pui1_file_buff + pt_idx->t_base.ui4_offset,
                              pt_idx->t_base.ui4_size);
        }
        pt_parser->ui4_node_p_idx_cur = 0;
    }
    return MMSBTLR_INV_ARG;
}



static INT32 _sbtl_ipcc_free_sample(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT32                    ui4_idx)
{
    if (pt_info &&
        pt_info->pv_samle_index  &&
        ui4_idx < pt_info->ui4_sample_num)
    {
        MM_SBTL_M4T_IDX_INFO_T* pt_idx;
        INT32                   i4_ret;
        pt_idx = (MM_SBTL_M4T_IDX_INFO_T*)pt_info->pv_samle_index[ui4_idx];
        i4_ret = sbtl_ipcc_free_sample(pt_info, pt_idx);
        pt_info->pv_samle_index[ui4_idx] = NULL;
        return i4_ret;
    }
    return MMSBTLR_INV_ARG;
}


static INT32 _sbtl_ipcc_init(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    INT32   i4_ret = MMSBTLR_OK;
    SBTL_IPCC_PARSER_T*       pt_parser;
    if (pt_info == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_info->pv_private = x_mem_alloc(sizeof(SBTL_IPCC_PARSER_T));
   
    if (pt_info->pv_private == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    pt_parser = (SBTL_IPCC_PARSER_T*)pt_info->pv_private;
    x_memset(pt_parser, 0, sizeof(SBTL_IPCC_PARSER_T));
    return i4_ret;
}


INT32 _sbtl_ipcc_show(MM_SBTL_LOAD_FILE_INFO_T* pt_file)
{
    SBTL_IPCC_PARSER_T*       pt_parser;
    INT32                     i4_ret = MMSBTLR_OK;
    MM_SBTL_M4T_IDX_INFO_T*   pt_idx = NULL;
    IPCC_IMG_RES_T*           pt_res = NULL;
    pt_parser = (SBTL_IPCC_PARSER_T* )pt_file->pv_private;
    
    if (pt_parser == NULL ||
        pt_file->h_widget == NULL_HANDLE)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    pt_res = (IPCC_IMG_RES_T*) pt_file->h_widget;
    if (pt_res == NULL)
    {
        return MMSBTLR_NOT_ALLOW;
    }

    if (pt_file->pv_samle_index && pt_file->ui4_sample_num &&
        pt_file->ui4_cur_sample < pt_file->ui4_sample_num)
    {     
        pt_idx = (MM_SBTL_M4T_IDX_INFO_T*) pt_file->pv_samle_index[pt_file->ui4_cur_sample];
    }
    if (pt_idx == NULL)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    if (pt_idx->ui2_imge_num > 0)
    {
        if (pt_res->b_cc_created)
        {
            i4_ret = x_sbtl_ipcc_close_fct();
            DBG_LOG_PRINT(("===x_sbtl_ipcc_close_fct ret %d===\n",i4_ret));
        }
        if (pt_res->h_surf == NULL_HANDLE)
        {
            if (pt_file->pt_engine)
            {
                x_wgl_get_canvas_info(pt_file->pt_engine->t_cfg.h_gl_plane, 
                                      &pt_res->t_keep_info);
            }
            i4_ret = ipcc_create_img_res(pt_res);
            if (i4_ret != MMSBTLR_OK)
            {
                return i4_ret;
            }
        }
        DBG_INFO(("before img-show at:%d\n", sbtl_cur_time()));
        i4_ret = sbtl_ipcc_img_show(pt_res, TRUE);
        DBG_INFO(("after img-show at:%d\n", sbtl_cur_time()));
        if (i4_ret == MMSBTLR_OK)
        {
            pt_parser->b_showing = TRUE;
            pt_parser->b_is_img  = TRUE;
        }
        return i4_ret;
     }
     else
     {
        BOOL b_reset_head = FALSE;

        i4_ret = ipcc_destroy_img_res(pt_res);
        if (pt_res->b_cc_created == FALSE)
        {
            i4_ret = ipcc_create_cc_widget(pt_file->pt_engine, pt_idx);
            if (i4_ret != MMSBTLR_OK)
            {
                return i4_ret;
            }
            pt_parser->ui4_sample_idx = pt_file->ui4_cur_sample;
        }
        b_reset_head = (pt_parser->ui4_sample_idx != pt_file->ui4_cur_sample);
        if (b_reset_head)
        {
            DBG_LOG_PRINT(("Change to another index %d\n", pt_file->ui4_cur_sample));
        }
        i4_ret = _sbtl_cc_show(pt_parser, pt_idx, b_reset_head, pt_file->e_show_mode);
        
        pt_parser->ui4_sample_idx = pt_file->ui4_cur_sample;
        if (i4_ret == MMSBTLR_OK)
        {
            pt_parser->b_showing = TRUE;
        }
     }
     
     return i4_ret;
}


static INT32 _sbtl_ipcc_deinit(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    INT32                  i4_ret = MMSBTLR_OK;
    SBTL_IPCC_PARSER_T*    pt_parser;
    if (pt_info == NULL || pt_info->pv_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_parser = ( SBTL_IPCC_PARSER_T*) pt_info->pv_private;
    if (pt_parser->b_showing)
    {
        if (pt_parser->b_is_img)
        {
            sbtl_ipcc_img_show((IPCC_IMG_RES_T*)(UINT32)pt_info->h_widget, FALSE);
            pt_parser->b_showing = FALSE;
            pt_parser->b_is_img  = FALSE;
            return MMSBTLR_OK;
        }
        else
        {
            i4_ret = _sbtl_cc_hide(pt_parser);
        }
    }
    x_mem_free(pt_info->pv_private);
    pt_info->pv_private = NULL;
    return i4_ret;
}

static INT32 _sbtl_ipcc_show_hide(MM_SBTL_LOAD_FILE_INFO_T* pt_info, BOOL b_show)
{
    SBTL_IPCC_PARSER_T*       pt_parser;
    
    if (pt_info == NULL ||
        pt_info->pv_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_parser = (SBTL_IPCC_PARSER_T*) pt_info->pv_private;
    DBG_INFO(("Set show to be:%d showing:%d img:%d\n", b_show, pt_parser->b_showing, pt_parser->b_is_img));
    if (b_show)
    {
        return _sbtl_ipcc_show(pt_info);
    }
    else
    {
        if (pt_parser->b_showing == FALSE)
        {
            return MMSBTLR_OK;
        }
        if (pt_parser->b_is_img)
        {
            sbtl_ipcc_img_show((IPCC_IMG_RES_T*)(UINT32)pt_info->h_widget, FALSE);
            pt_parser->b_showing = FALSE;
            pt_parser->b_is_img  = FALSE;
            return MMSBTLR_OK;
        }
           
        return _sbtl_cc_hide(pt_parser);
    }
}

static INT32 _sbtl_ipcc_get_data(
    MM_SBTL_LOAD_FILE_INFO_T*  pt_file,
    SBTL_DYNC_GET_DATA_INFO_T* pt_data)
{
    MM_SBTL_M4T_IDX_INFO_T* pt_idx = NULL;
    SBTL_IPCC_PARSER_T*     pt_parser;
    UINT32                  ui4_p_node_idx;
    UINT32                  ui4_find_num = 0;
    XML_NODE_T*             pt_node_p;
    UINT32                  ui4_start_pts = 0;
    UINT32                  ui4_end_pts   = 0;
    INT32                   i4_ret;
    BOOL                    b_add_next = FALSE;
    UINT32                  ui4_node_num = 0;
    
    if (pt_file == NULL ||
        pt_data == NULL ||
        pt_file->pv_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_parser = (SBTL_IPCC_PARSER_T*) pt_file->pv_private;
    if (pt_file->pv_samle_index == NULL ||
        pt_file->ui4_sample_num == 0    ||
        pt_file->ui4_cur_sample >= pt_file->ui4_sample_num)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    
    pt_idx = (MM_SBTL_M4T_IDX_INFO_T*)
              pt_file->pv_samle_index[pt_file->ui4_cur_sample];
    if (pt_idx == NULL)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    pt_node_p  = pt_idx->pt_p_nodes;
    if (pt_idx->ui2_imge_num > 0)
    {
        pt_node_p = pt_idx->pt_div_nodes;
    }
    if (pt_node_p == NULL)
    {
        pt_data->b_found = FALSE;
        return MMSBTLR_OK;
    }
    i4_ret = x_sbtl_xml_get_element_num(pt_node_p, &ui4_node_num);
    if (i4_ret != MMSBTLR_OK || ui4_node_num == 0)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    
    ui4_p_node_idx = pt_parser->ui4_node_p_idx_cur;
    
    //if find current still overdue find from begin
    if (ui4_p_node_idx < ui4_node_num)
    {
        i4_ret = sbtl_xml_get_node_time(
                               pt_node_p,
                               ui4_p_node_idx,
                               &ui4_start_pts,
                               &ui4_end_pts);
        if (i4_ret != MMSBTLR_OK ||
            ui4_start_pts + pt_file->ui4_start_pts > pt_data->ui4_cur_pts)
        {
            ui4_p_node_idx = 0;
        } 
    }
    else
    {
        ui4_p_node_idx = 0;
    }
    DBG_INFO(("========p node contain %d=========\n",(INT32)ui4_node_num));
    ui4_find_num = 0;
    pt_data->b_found = FALSE;
    
    while (ui4_p_node_idx < ui4_node_num)
    {
        i4_ret = sbtl_xml_get_node_time(
                                   pt_node_p,
                                   ui4_p_node_idx,
                                   &ui4_start_pts,
                                   &ui4_end_pts);
        ui4_start_pts += pt_file->ui4_start_pts;
        ui4_end_pts   += pt_file->ui4_start_pts;
        if (MMSBTLR_OK != i4_ret)
        {
            break;
        }
        
        if (pt_data->b_found)
        {
            if (ui4_start_pts >= pt_data->ui4_start_pts &&
                ui4_end_pts >= pt_data->ui4_end_pts &&
                ui4_end_pts - pt_data->ui4_end_pts < 30)
            {
                ui4_find_num++;
            }
            else
            {
                if (ui4_start_pts == pt_data->ui4_end_pts)
                {
                    /*load extra P for fast remove*/
                    b_add_next = TRUE;
                }
                break;
            }
        }
        else if (pt_data->ui4_cur_pts < ui4_end_pts)
        {
            pt_data->b_found = TRUE;
            pt_data->ui4_start_pts = ui4_start_pts;
            pt_data->ui4_end_pts   = ui4_end_pts;
            ui4_find_num++; 
        }   
        ui4_p_node_idx++;
        if (pt_data->b_found && pt_idx->ui2_imge_num > 0)
        {
            break;
        }
    } 
    
    if (pt_data->b_found == TRUE)
    {        
        UINT32 ui4_i = 0;

        pt_parser->ui4_node_p_idx_cur = ui4_p_node_idx;
        ui4_p_node_idx -= ui4_find_num;
        DBG_INFO(("Data  Find [cur:%d - num:%d]\n", ui4_p_node_idx, ui4_find_num));
        
        //image return directly
        if (pt_idx->ui2_imge_num > 0)
        {
            XML_T*          pt_cur = NULL;
            IPCC_IMG_RES_T* pt_res;
            pt_res = (IPCC_IMG_RES_T*)pt_file->h_widget;
            if (pt_res == NULL)
            {
                return MMSBTLR_OK;
            }
            #if 0
            UINT32 aui4_max_num[5] = {0, 1, 2, 3, 4, 5};
            if (ui4_find_num > 5)
            {
                ui4_find_num = 5;
            }
            #endif
            if (pt_res->h_img_surf != NULL_HANDLE)
            {
                GL_COLOR_T t_color = {0, {0}, {0}, {0}};
                x_gl_surface_fill(pt_res->h_img_surf, &t_color, GL_SYNC);
            }
             
             for (ui4_i = 0; ui4_i < ui4_find_num; ui4_i++)
             {
                pt_cur = x_sbtl_xml_get_element_byidx(
                                pt_idx->pt_div_nodes, 
                                ui4_p_node_idx+ui4_i);
                if (pt_cur)
                {
                   i4_ret = sbtl_ipcc_load_img(
                                pt_res,
                                pt_cur,
                                pt_idx,
                                pt_file->e_show_mode);
                   if (i4_ret != MMSBTLR_OK)
                   {
                       DBG_ERROR(("Load image data: ret %d\n", i4_ret));
                   }
                }
             }
    
            return MMSBTLR_OK;
        }

        _sbtl_cc_hide(pt_parser);
        
        SLIST_INIT(&pt_parser->t_element_p_list);    
   
        for (ui4_i = 0; ui4_i < ui4_find_num; ui4_i++)
        {
            ipcc_load_node (&pt_parser->t_element_p_list,
                            pt_node_p, 
                            ui4_p_node_idx + ui4_i,
                            FALSE);
        }
        if (b_add_next)
        {
            ipcc_load_node (&pt_parser->t_element_p_list,
                            pt_node_p, 
                            ui4_p_node_idx + ui4_i,
                            TRUE);
        }
        pt_data->pv_data = &pt_parser->t_element_p_list;
        return MMSBTLR_OK;
    }
    else
    {        
        return MMSBTLR_NOT_FOUND;
    }
}


//========================================================================
INT32 ipcc_delete_widget(HANDLE_T h_widget)
{
    INT32 i4_ret = MMSBTLR_INV_ARG;

    IPCC_IMG_RES_T*  pt_res;
    if (h_widget == NULL_HANDLE)
    {
        return i4_ret;
    }
    pt_res = (IPCC_IMG_RES_T*)h_widget;
    if (pt_res->b_cc_created)
    {
        i4_ret = x_sbtl_ipcc_close_fct();
        DBG_LOG_PRINT(("===x_sbtl_ipcc_close_fct ret %d===\n",i4_ret));
    }
    i4_ret = ipcc_destroy_img_res(pt_res);
    x_mem_free(pt_res);
    return i4_ret;
}

INT32 ipcc_create_widget( 
    MM_SBTL_LOAD_FILE_INFO_T*   pt_file)
{
    SBTL_ENGINE_T*        pt_engine = NULL;
    INT32                 i4_ret = MMSBTLR_OK;
    IPCC_IMG_RES_T*       pt_res = NULL;
    SBTL_IPCC_PARSER_T*   pt_parser = NULL;
    MM_SBTL_M4T_IDX_INFO_T*   pt_idx = NULL;
    
    if (pt_parser)
    {
        pt_parser->ui4_sample_idx = pt_file->ui4_cur_sample;
    }
            
    if (pt_file == NULL || 
        (pt_file->e_sbtl_type != MM_SBTL_TYPE_XML_IPCC && pt_file->e_sbtl_type != MM_SBTL_TYPE_XML))
    {
        return MMSBTLR_NOT_ALLOW;
    }
    pt_parser = (SBTL_IPCC_PARSER_T*)pt_file->pv_private;
    pt_engine = pt_file->pt_engine;
    
         
    if (pt_parser == NULL ||
        pt_engine == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    if (pt_file->pv_samle_index  == NULL || 
        pt_file->ui4_sample_num  == 0    ||
        pt_file->ui4_cur_sample >= pt_file->ui4_sample_num)
    {
        return MMSBTLR_NOT_ALLOW;
    }
    
    pt_idx = (MM_SBTL_M4T_IDX_INFO_T*) pt_file->pv_samle_index[pt_file->ui4_cur_sample];
    
    if (pt_idx == NULL || 
        pt_idx->pt_xml_tree == NULL)
    {
        return MMSBTLR_NOT_FOUND;
    }
    pt_res = (IPCC_IMG_RES_T*) x_mem_alloc(sizeof(IPCC_IMG_RES_T));

    if (pt_res == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    x_memset(pt_res, 0, sizeof(IPCC_IMG_RES_T));

    if (pt_file->pt_engine)
    {
        pt_res->h_canvas = pt_file->pt_engine->t_cfg.h_gl_plane;
    }

    if (pt_idx->ui2_imge_num > 0)
    {
        i4_ret = ipcc_create_img_res(pt_res);
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_ERROR(("create image res ret:%d\n", i4_ret));
        }
    }
    else
    {
        i4_ret = ipcc_create_cc_widget(pt_engine, pt_idx);
        if (i4_ret == MMSBTLR_OK)
        {
            pt_res->b_cc_created = TRUE;
        }
    }
    if (i4_ret == MMSBTLR_OK)
    {
        pt_file->h_widget = (HANDLE_T)pt_res;
        pt_parser->ui4_sample_idx = pt_file->ui4_cur_sample;
        return i4_ret;
    }
    x_mem_free(pt_res);
    
    return i4_ret;
}



SBTL_DYNC_REG_FUNC_T t_m4t_dync_func =
{
    sbtl_m4t_matched,
    _sbtl_ipcc_init,
    _sbtl_ipcc_deinit,
    _sbtl_ipcc_get_data,
    _sbtl_m4t_build_idx,
    _sbtl_ipcc_parse_sample,
    _sbtl_ipcc_free_sample,
    _sbtl_ipcc_show_hide,
    NULL,
    NULL,
    SBTL_COMM_IPCC
};

SBTL_DYNC_REG_FUNC_T t_xml_dync_func =
{
     sbtl_xml_matched,
    _sbtl_ipcc_init,
    _sbtl_ipcc_deinit,
    _sbtl_ipcc_get_data,
    _sbtl_xml_build_idx,
    _sbtl_ipcc_parse_sample,
    _sbtl_ipcc_free_sample,
    _sbtl_ipcc_show_hide,
    NULL,
    NULL,
    SBTL_COMM_IPCC
};


VOID sbtl_ipcc_reg_dync_func(VOID)
{
    sbtl_dyn_reg_parser(
                  MM_SBTL_TYPE_XML_IPCC,
                  &t_m4t_dync_func);
    sbtl_dyn_reg_parser(
                  MM_SBTL_TYPE_XML,
                  &t_xml_dync_func);
}




#if 0
static INT32 _sbtl_ipcc_config(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                              VOID*                    pv_cfg_info,
                              UINT32                   ui4_set_type)
{
    SBTL_XML_PAGE_MNGR_PRIVATE_T*  pt_private;
    SBTL_XML_FNC_TBL_T*            pt_fnc_table;
    INT32                          i4_ret;        
    SM_CCH_DTVCC_ATTRIBS_INFO_T*   pt_sbtl_cc_info;
    if (pt_mngr == NULL ||
        (pt_mngr->pv_private == NULL) ||
        (pv_cfg_info == NULL))
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private = (SBTL_XML_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_fnc_table = pt_private->pt_fnc_table;
    ASSERT(pt_fnc_table, ("pt_fnc_table=NULL\n"));
    pt_sbtl_cc_info = &pt_mngr->pt_engine->t_cfg.t_sbtl_cc_info;
    switch (ui4_set_type)
    {
        case EVN_MM_SBTL_SET_CC_ATTR_INFO:
        {
            i4_ret = x_sbtl_ipcc_set_fct(SBTL_IPCC_SET_TYPE_ATTRIBS,
                                        (VOID *) pt_sbtl_cc_info, 
                                         sizeof(SM_CCH_DTVCC_ATTRIBS_INFO_T));
            DBG_INFO(("x_sbtl_ipcc_set_fct ret %d\n",i4_ret));
            UNUSED(i4_ret);
        }
        break;
        
        default:
            DBG_INFO(("cannot find the correspound set_type !\n"));
        break;
    }
    
    return MMSBTLR_OK;
}
#endif


#endif /*__KERNEL__*/


