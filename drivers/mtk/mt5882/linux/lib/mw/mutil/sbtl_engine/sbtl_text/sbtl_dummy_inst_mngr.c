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
 * $RCSfile: sbtl_text_page_inst_mngr.c,v $
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
#include "mutil/mm_util.h"



#else /*__KERNEL__*/
#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"
#include "x_uc_str.h"
#include "../../mm_util.h"


#include "../sbtl_dbg.h"
#include "../sbtl_struct.h"
#include "../sbtl_utils.h"
#include "../sbtl_register.h"
#include "../sbtl_state.h"
#include "../sbtl_widget.h"
#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define SBTL_DUMMY_PERIOD (300)
#define SBTL_DUMMY_IMG_CLUT4 (UINT32)(1)
#define SBTL_DUMMY_IMG_CLUT8 (UINT32)(2)

#define SBTL_DUMMY_STYLE_ITALIC    ((UINT16)1)
#define SBTL_DUMMY_STYLE_UNDERLINE ((UINT16)2)
#define SBTL_DUMMY_STYLE_BGCOLOR   ((UINT16)4)
#define SBTL_DUMMY_STYLE_FGCOLOR   ((UINT16)8)

typedef enum
{
     DUMMY_STATE_NONE,
     DUMMY_STATE_DATA_RDY,
     DUMMY_STATE_DISPLAY,
     DUMMY_STATE_END
} DUMMY_PAGE_STATE_T;

typedef struct
{
    UINT32 ui4_start_time;
    UINT32 ui4_end_time;
    UINT32 ui4_offset;
}SBTL_INDEX_INFO_T;

typedef struct
{
    CHAR               s_magic[4];
    UINT32             ui4_size;        
    UINT32             ui4_count;
}SBTL_DUMMY_HEAD_T;

typedef struct
{
    UINT8 ui1_a;
    UINT8 ui1_r;
    UINT8 ui1_g;
    UINT8 ui1_b;
}SBTL_DUMMY_COLOR_T;

typedef struct
{
   UINT8                   ui1_size;
   UINT16                  ui2_style;
   SBTL_DUMMY_COLOR_T      t_fg;
   SBTL_DUMMY_COLOR_T      t_bg;
}SBTL_DUMMY_TXTDATA_T;

typedef struct
{
   UINT32          ui4_clr_mode;
   UINT16          ui2_pal_len;
   UINT16          ui2_pal_idx;
   UINT16          ui2_vid_width;
   UINT16          ui2_vid_height;
   UINT32          ui4_left;
   UINT32          ui4_top;
   UINT32          ui4_width;
   UINT32          ui4_height;     
}SBTL_DUMMY_IMGDATA_T;

typedef struct
{
    union
    {
        SBTL_DUMMY_IMGDATA_T t_img;
        SBTL_DUMMY_TXTDATA_T t_txt; 
    }u;
}SBTL_DUMMY_DATA_T;

typedef struct
{
    SBTL_DUMMY_HEAD_T*  pt_idx;
    SBTL_DUMMY_HEAD_T*  pt_data;
    SBTL_DUMMY_TYPE_T   e_type;
    DUMMY_PAGE_STATE_T  e_state;
    HANDLE_T            h_sbtl_wgl;
    HANDLE_T            h_timer;
    HANDLE_T            h_sem;
    UINT32              ui4_sel;
    BOOL                b_started;
    PTS_T               t_pre_pts;
    UTF16_T*            w2s_str;
    VOID*               pv_phy;
    VOID*               pv_vir;
    UINT32              ui4_size;
}SBTL_DUMMY_PAGE_MNGR_PRIVATE_T;

static SBTL_PAGE_INST_MNGR_T* _dummy_page_mngr_constructor (SBTL_ENGINE_T*    pt_engine);
static VOID _dummy_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*   pt_mngr);

static SBTL_MNGR_FCT_TBL_T t_dummy_fnct_tbl = 
{
    FALSE,
    _dummy_page_mngr_constructor,
    _dummy_page_mngr_destructor,
    NULL
};

static VOID* _sbtl_dummy_get_vir_addr(SBTL_DUMMY_PAGE_MNGR_PRIVATE_T* pt_private,
                                             VOID* pv_phy)
{
    if (pt_private && pt_private->pv_phy && pv_phy)
    {
        UINT8* pui1_vir = (UINT8*)pt_private->pv_vir;
        if (pv_phy >= pt_private->pv_phy &&
            (UINT8*)pv_phy < (UINT8*)pt_private->pv_phy+pt_private->ui4_size)
        {
            pui1_vir += (UINT8*)pv_phy - (UINT8*)pt_private->pv_phy;
            return (VOID*)pui1_vir;
        }
    }
    return NULL;
}

static INT32 _sbtl_dummy_new_page(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    /*leave empty body*/
    return MMSBTLR_OK;
}
#define PTS_FREQ        ((UINT64)(90000))
static INT32 _sbtl_dummy_adjust_pts(PTS_T* pt_pts, SBTL_PAGE_INST_MNGR_T* pt_mngr, BOOL* pb_negative)
{
    INT64                           i8_pts_tmp;
    UINT64                          ui8_tmp;
    INT64                           i8_adjust;
    
    if((pt_pts == NULL) || (pb_negative == NULL) || (pt_mngr == NULL))
    {
        return MMSBTLR_INV_ARG;
    }

    i8_pts_tmp = (INT64)(*pt_pts);
    i8_adjust = pt_mngr->pt_engine->t_cfg.t_time_ofst.i8_time_ofst;

    if(i8_adjust >= 0)
    {
        ui8_tmp = (UINT64)(i8_adjust);
        ui8_tmp     = (UINT64)_mm_div64(PTS_FREQ*ui8_tmp, 1000, NULL);
        i8_pts_tmp += ui8_tmp;
    }
    else
    {
        ui8_tmp = (UINT64)(0 - i8_adjust);
        ui8_tmp     = (UINT64)_mm_div64(PTS_FREQ*ui8_tmp, 1000, NULL);
        i8_pts_tmp -= ui8_tmp;
    }          

    if(i8_pts_tmp < 0)
    {
        *pt_pts = 0;
        *pb_negative = TRUE;
    }
    else
    {
        *pt_pts = (PTS_T)i8_pts_tmp;
        *pb_negative = FALSE;
    }

    return MMSBTLR_OK;
}


static VOID _sbtl_dummy_hide(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*   pt_private;
    INT32                             i4_ret;
    
    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if ( pt_private->e_state != DUMMY_STATE_DISPLAY)
    {
        return;
    }
    
    if (pt_private->e_type == SBTL_DUMMY_TEXT)
    {   
        i4_ret = sbtl_wgl_show(pt_private->h_sbtl_wgl, FALSE);
        if (MMSBTLR_OK != i4_ret)
        {
            ASSERT(0, ("clear widget fail!!/n"));
        }
         SBTL_SET_FONT_STYLE_FLAG(pt_mngr->pt_engine, FALSE);
         SBTL_GET_FONT_STYLE(pt_mngr->pt_engine) =  (FE_FNT_STYLE)0;
    }
    if (pt_private->e_type == SBTL_DUMMY_IMG)
    {
       i4_ret = sbtl_wgl_do_cmd(pt_private->h_sbtl_wgl,
                                 SBTL_WGL_CMD_SBTL_CLEAR_ALL,
                                 (VOID*)&pt_private->t_pre_pts,
                                 NULL);
        if (MMSBTLR_OK != i4_ret)
        {
            ASSERT(0, ("clear widget fail!!/n"));
        }
    }
    pt_private->e_state = DUMMY_STATE_NONE;
    DBG_LOG_PRINT(("[Dummy] Hide Dummy!\n"));
    return;
}

static INT32 _sbtl_dummy_show(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*  pt_private;
    INT32                           i4_ret = MMSBTLR_OK;
    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    if (pt_private->e_type == SBTL_DUMMY_TEXT)
    {
        if (pt_private->pt_data && 
            pt_private->pt_data->ui4_count > 0 &&
            pt_private->w2s_str)
        {
            SBTL_DUMMY_DATA_T* pt_data = (SBTL_DUMMY_DATA_T*)((UINT8*)pt_private->pt_data+sizeof(SBTL_DUMMY_HEAD_T));
            FE_FNT_STYLE       e_new_style;
            WGL_FONT_INFO_T    t_font_inf;
            WGL_COLOR_INFO_T   t_color_inf;
            CHAR*              ps_str = (CHAR*)((UINT8*)pt_private->pt_data+sizeof(SBTL_DUMMY_HEAD_T)+
                                          pt_private->pt_data->ui4_count*sizeof(SBTL_DUMMY_DATA_T));
            
            e_new_style = SBTL_GET_BASE_FONT_STYLE(pt_mngr->pt_engine);
            SBTL_GET_FONT_STYLE(pt_mngr->pt_engine) = e_new_style;
            x_uc_ps_to_w2s(ps_str, pt_private->w2s_str,512);
            
            if (pt_data->u.t_txt.ui2_style != 0 ||
                pt_data->u.t_txt.ui1_size != 0)
            {
                x_memcpy(&t_font_inf, &pt_mngr->pt_engine->t_cfg.t_font_info, sizeof(WGL_FONT_INFO_T));
                #ifndef __KERNEL__
                i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                          WGL_CMD_GL_GET_FONT,
                                          (VOID*)&t_font_inf,
                                          NULL);
                #endif
                if (pt_data->u.t_txt.ui2_style & SBTL_DUMMY_STYLE_UNDERLINE)
                {
                    e_new_style |= FE_FNT_STYLE_UNDERLINE;
                }
                if (pt_data->u.t_txt.ui2_style & SBTL_DUMMY_STYLE_ITALIC)
                {
                    e_new_style |= FE_FNT_STYLE_ITALIC;
                }
                if ((pt_data->u.t_txt.ui1_size > 0 && 
                    ((t_font_inf.e_font_size != FE_FNT_SIZE_CUSTOM) ||
                     (t_font_inf.ui1_custom_size != (UINT8)pt_data->u.t_txt.ui1_size))) ||
                     (e_new_style != SBTL_GET_PREV_FONT_STYLE(pt_mngr->pt_engine))
                    )
                {
                    if (pt_data->u.t_txt.ui1_size > 0)
                    {
                        t_font_inf.e_font_size = FE_FNT_SIZE_CUSTOM;
                        t_font_inf.ui1_custom_size = (UINT8)pt_data->u.t_txt.ui1_size;
                    }
                    t_font_inf.e_font_style = e_new_style;
                    i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                              SBTL_WGL_CMD_GL_SET_FONT,
                                              (VOID*)&t_font_inf,
                                              NULL);
                    if (MMSBTLR_OK != i4_ret)
                    {
                        return (MMSBTLR_WGL_FAIL);
                    }
                    SBTL_GET_PREV_FONT_STYLE(pt_mngr->pt_engine) = e_new_style;
                    
                }
                x_memset(&t_color_inf,0, sizeof(WGL_COLOR_INFO_T));
                if ((pt_data->u.t_txt.ui2_style & SBTL_DUMMY_STYLE_FGCOLOR))
                {
                    #ifndef __KERNEL__
                    i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                              WGL_CMD_GL_GET_COLOR,
                                              (VOID*)SBTL_WGL_CLR_TEXT,
                                              (VOID*)(&t_color_inf)); 
                    #endif
                    x_memcpy(&t_color_inf.u_color_data.t_basic.t_enable, &pt_data->u.t_txt.t_fg, sizeof(SBTL_DUMMY_COLOR_T));
                    
                    i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                              SBTL_WGL_CMD_GL_SET_COLOR,
                                              (VOID*)SBTL_WGL_CLR_TEXT,
                                              (VOID*)(&t_color_inf));            
                }
                if ((pt_data->u.t_txt.ui2_style & SBTL_DUMMY_STYLE_BGCOLOR))
                {
                    #ifndef __KERNEL__
                    i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                              WGL_CMD_GL_GET_COLOR,
                                              (VOID*)SBTL_WGL_CLR_TEXT_BG,
                                              (VOID*)(&t_color_inf)); 
                    #endif
                    
                    x_memcpy(&t_color_inf.u_color_data.t_basic.t_enable, &pt_data->u.t_txt.t_bg, sizeof(SBTL_DUMMY_COLOR_T));
                    
                    i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                              SBTL_WGL_CMD_GL_SET_COLOR,
                                              (VOID*)SBTL_WGL_CLR_TEXT_BG,
                                              (VOID*)(&t_color_inf));            
                }
            }
            i4_ret = sbtl_wgl_show(pt_private->h_sbtl_wgl, TRUE); 
        }
      
    }
    else if (pt_private->e_type == SBTL_DUMMY_IMG)
    {
        if(pt_private->pt_data && pt_private->pt_data->ui4_count > 0)
        {
            DBG_LOG_PRINT(("[Dummy]Show now count:%d\n",pt_private->pt_data->ui4_count));
            i4_ret = sbtl_wgl_do_cmd(pt_private->h_sbtl_wgl,
                                  SBTL_WGL_CMD_SBTL_FLIP,
                                  (VOID*)pt_mngr,
                                  (VOID*)&pt_private->t_pre_pts);
            
            if (i4_ret != MMSBTLR_OK)
            {
                ASSERT(0, ("wgl flip error!!\n"));
            }
        }  
    } 
    return i4_ret;
}


static VOID _sbtl_timer_cb_fct(HANDLE_T     pt_tm_handle,
                               VOID*        pv_tag)
{
    MM_SBTL_HDLR_MSG_T      t_msg;
    if (NULL == pv_tag)
    {
        return;
    }
    mm_sbtl_hdlr_send_timer_msg(t_msg, pv_tag);
}

static INT32 _sbtl_dummy_lock_pib_fct(HANDLE_T   h_page_inst,
                                     VOID*      pv_tag)
{
    SBTL_PAGE_INST_MNGR_T*           pt_mngr;
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*  pt_private;
    if (NULL == pv_tag)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_mngr = (SBTL_PAGE_INST_MNGR_T*)pv_tag;
    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if (NULL == pt_private)
    {
        return MMSBTLR_NOT_FOUND;
    }
    x_sema_lock(pt_private->h_sem, X_SEMA_OPTION_WAIT); 
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
static INT32 _sbtl_dummy_unlock_pib_fct(HANDLE_T     h_page_inst,
                                         VOID*        pv_tag)
{
    SBTL_PAGE_INST_MNGR_T*           pt_mngr;
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*  pt_private;
    if (NULL == pv_tag)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_mngr = (SBTL_PAGE_INST_MNGR_T*)pv_tag;
    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if (NULL == pt_private)
    {
        return MMSBTLR_NOT_FOUND;
    }
    x_sema_unlock(pt_private->h_sem); 
    return MMSBTLR_OK;
}


static INT32 _sbtl_dummy_get_pib_rgn_fct(HANDLE_T                  h_page_inst,
                                          VOID*                     pv_tag,
                                          UINT16                    ui2_rgn_idx,
                                          SBTL_WGL_SBTL_REGION_T*   pt_rgn)
{
    SBTL_PAGE_INST_MNGR_T*           pt_mngr;
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*  pt_private;
    if (NULL == pv_tag)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_mngr = (SBTL_PAGE_INST_MNGR_T*)pv_tag;
    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if (NULL == pt_private)
    {
        return MMSBTLR_NOT_FOUND;
    }

    if (NULL == pt_private ||
        pt_private->pt_data == NULL ||
        ui2_rgn_idx > 0 ||
        pt_rgn == NULL)
    {
        return MMSBTLR_NOT_FOUND;
    }
    
    if (ui2_rgn_idx < pt_private->pt_data->ui4_count)
    {
        UINT16             ui2_i = 0;
        VOID*              pv_addr = pt_private->pt_data;
        SBTL_DUMMY_DATA_T* pt_data;
        VOID*              pv_pal;
        VOID*              pv_color;
        UINT32             ui4_img_size = 0;
        
        //head
        pv_addr = (UINT8*)pv_addr + sizeof(SBTL_DUMMY_HEAD_T);
        for (ui2_i = 0; ui2_i < ui2_rgn_idx; ui2_i++)
        {
            pt_data = (SBTL_DUMMY_DATA_T*) pv_addr;
            ui4_img_size += pt_data->u.t_img.ui4_width*pt_data->u.t_img.ui4_height*sizeof(UINT8);
            pt_data++;
        }
        pt_data =  (SBTL_DUMMY_DATA_T*) ((UINT8*)pv_addr + sizeof(SBTL_DUMMY_DATA_T)*ui2_rgn_idx);
        //+data desc
        pv_pal =  (UINT8*)pv_addr+ (pt_private->pt_data->ui4_count)*sizeof(SBTL_DUMMY_DATA_T);
        //+pal length
        pv_color = (UINT8*)pv_pal + pt_data->u.t_img.ui2_pal_len;
        //+pre imag size
        pv_color = (UINT8*)pv_color + ui4_img_size;
        
        x_memset(pt_rgn, 0, sizeof(SBTL_WGL_SBTL_REGION_T));
        
        pt_rgn->at_clut = (GL_COLOR_T*)pv_pal;
        pt_rgn->b_dirty = TRUE;
        if (pt_data->u.t_img.ui4_clr_mode == SBTL_DUMMY_IMG_CLUT8)
        {
            pt_rgn->e_clr_mode = SBTL_WGL_SBTL_COLOR_MODE_CLUT_8_BIT;
        }
        else
        {
            pt_rgn->e_clr_mode = SBTL_WGL_SBTL_COLOR_MODE_CLUT_4_BIT;
        }
        
        pt_rgn->i4_top     = (INT32)pt_data->u.t_img.ui4_top;
        pt_rgn->i4_left    = (INT32)pt_data->u.t_img.ui4_left;
        pt_rgn->ui4_width  = pt_data->u.t_img.ui4_width;
        pt_rgn->ui4_height = pt_data->u.t_img.ui4_height;
        pt_rgn->t_rc_dirty.i4_bottom = (INT32)pt_data->u.t_img.ui4_height;
        pt_rgn->t_rc_dirty.i4_top = 0;
        pt_rgn->t_rc_dirty.i4_left = 0;
        pt_rgn->t_rc_dirty.i4_right = (INT32)pt_data->u.t_img.ui4_width;
        pt_rgn->pv_bitmap = pv_color;
        
        DBG_LOG_PRINT(("[Dummy] L:%d T:%d W:%d H:%d clut:%x img:%x\n", 
            pt_rgn->i4_left, pt_rgn->i4_top, pt_rgn->ui4_width, pt_rgn->ui4_height, (int)pv_pal, (int)pv_color));
        
        return MMSBTLR_OK;
    }
    
    return MMSBTLR_NOT_FOUND;
}


static INT32 _sbtl_dummy_start(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    INT32                               i4_ret = MMSBTLR_OK;
    GL_RECT_T                           t_sbtl_rc_scrn;
    UINT32                              ui4_canvas_w = 0;
    UINT32                              ui4_canvas_h = 0;
    HANDLE_T                            h_canvas;
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*     pt_private;
    GL_POINT_T                          t_osd_ofst;                      

    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;

    if (pt_private->h_sbtl_wgl)
    {
        return MMSBTLR_OK;
    }
    h_canvas = pt_mngr->pt_engine->t_cfg.h_gl_plane;
    
    i4_ret = sbtl_wgl_get_canvas_size(h_canvas, &ui4_canvas_w, &ui4_canvas_h);
    if (i4_ret != MMSBTLR_OK)
    {
        return MMSBTLR_INTERNAL_ERROR;
    }

    if (pt_private->e_type == SBTL_DUMMY_TEXT)
    {
        if (pt_mngr->pt_engine->t_cfg.b_rect_osd)
        {
            t_sbtl_rc_scrn = pt_mngr->pt_engine->t_cfg.t_rect_osd;
        }
        else
        {
            
            t_sbtl_rc_scrn.i4_left = (INT32)(ui4_canvas_w/6);
            t_sbtl_rc_scrn.i4_top = (INT32)(ui4_canvas_h/10);
            t_sbtl_rc_scrn.i4_right = (INT32)ui4_canvas_w-(INT32)(ui4_canvas_w/6);
            t_sbtl_rc_scrn.i4_bottom = (INT32)(ui4_canvas_h)-(INT32)(ui4_canvas_h/10);
        }    
        
        t_osd_ofst = pt_mngr->pt_engine->t_cfg.t_pos_adjust;
        
        if(t_osd_ofst.i4_x !=0 || t_osd_ofst.i4_y !=0)
        {            
            t_sbtl_rc_scrn.i4_left += t_osd_ofst.i4_x;
            t_sbtl_rc_scrn.i4_top += t_osd_ofst.i4_y;
            t_sbtl_rc_scrn.i4_right += t_osd_ofst.i4_x;
            t_sbtl_rc_scrn.i4_bottom += t_osd_ofst.i4_y;

            if(t_sbtl_rc_scrn.i4_left < 0)
            {
                t_sbtl_rc_scrn.i4_right -= t_sbtl_rc_scrn.i4_left;
                t_sbtl_rc_scrn.i4_left = 0;
            }

            if(t_sbtl_rc_scrn.i4_top < 0)
            {
                t_sbtl_rc_scrn.i4_bottom -= t_sbtl_rc_scrn.i4_top;
                t_sbtl_rc_scrn.i4_top = 0;
            }
            
            if(t_sbtl_rc_scrn.i4_right > (INT32)ui4_canvas_w)
            {
                t_sbtl_rc_scrn.i4_left -= (t_sbtl_rc_scrn.i4_right - (INT32)ui4_canvas_w);
                t_sbtl_rc_scrn.i4_right = (INT32)ui4_canvas_w;
            }

            if(t_sbtl_rc_scrn.i4_bottom > (INT32)ui4_canvas_h)
            {
                t_sbtl_rc_scrn.i4_top -= (t_sbtl_rc_scrn.i4_bottom - (INT32)ui4_canvas_h);
                t_sbtl_rc_scrn.i4_bottom = (INT32)ui4_canvas_h;
            }
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
                    &(pt_private->h_sbtl_wgl));
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }
        i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                           SBTL_WGL_CMD_GL_SET_FONT,
                           (VOID*)&pt_mngr->pt_engine->t_cfg.t_font_info,
                           NULL);
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }

        i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                           SBTL_WGL_CMD_TEXT_SET_ALIGN,
                           (VOID*)((UINT32)pt_mngr->pt_engine->t_cfg.ui1_text_alignment),
                           NULL);
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }

        i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                               SBTL_WGL_CMD_GL_SET_COLOR,
                               (VOID*)SBTL_WGL_CLR_BK, /* background color */
                               (VOID*)&pt_mngr->pt_engine->t_cfg.t_bg_color_info);
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }

        i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                               SBTL_WGL_CMD_GL_SET_COLOR,
                               (VOID*)SBTL_WGL_CLR_TEXT, /* text color */
                               (VOID*)&pt_mngr->pt_engine->t_cfg.t_fg_color_info);    
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }

        i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                               SBTL_WGL_CMD_GL_SET_COLOR,
                               (VOID*)SBTL_WGL_CLR_TEXT_BG, /* text color */
                               (VOID*)&pt_mngr->pt_engine->t_cfg.t_font_bg_color);    
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }
    	
        if(pt_mngr->pt_engine->t_cfg.ui4_cmd_mask | MM_SBTL_CMD_MSK_EDGE_COLOR)
        {
    	    i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
    	                           SBTL_WGL_CMD_GL_SET_COLOR,
    	                                       (VOID*)SBTL_WGL_CLR_EDGE, /* font edge color */
    	                                       (VOID*)&(pt_mngr->pt_engine->t_cfg.t_edge_color_info));
    	                
    	    if (MMSBTLR_OK != i4_ret)
    	    {
    	       return (MMSBTLR_WGL_FAIL);
       	    }
        }    
    	
        i4_ret = sbtl_wgl_show(pt_private->h_sbtl_wgl, FALSE);

        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }

        SBTL_GET_BASE_FONT_STYLE(pt_mngr->pt_engine) = pt_mngr->pt_engine->t_cfg.t_font_info.e_font_style;
        SBTL_GET_PREV_FONT_STYLE(pt_mngr->pt_engine) = pt_mngr->pt_engine->t_cfg.t_font_info.e_font_style;
        SBTL_SET_FONT_STYLE_FLAG(pt_mngr->pt_engine, FALSE);
        SBTL_GET_FONT_STYLE(pt_mngr->pt_engine) =  (FE_FNT_STYLE)0;
    }
    else if (pt_private->e_type == SBTL_DUMMY_IMG)
    {
        UINT32                 ui4_logical_w = 1920;
        UINT32                 ui4_logical_h = 1080;
        SBTL_WGL_SBTL_INIT_T   t_sbtl_init;
        if (pt_private->pt_data && pt_private->pt_data->ui4_count > 0)
        {
            SBTL_DUMMY_DATA_T* pt_data = (SBTL_DUMMY_DATA_T*)((UINT8*)pt_private->pt_data+sizeof(SBTL_DUMMY_HEAD_T));
            ui4_logical_w = pt_data->u.t_img.ui2_vid_width;
            ui4_logical_h = pt_data->u.t_img.ui2_vid_height;
        }    
        t_sbtl_rc_scrn.i4_left = 0;
        t_sbtl_rc_scrn.i4_top = 0;
        t_sbtl_rc_scrn.i4_bottom = (INT32)ui4_canvas_h;
        t_sbtl_rc_scrn.i4_right = (INT32)ui4_canvas_w;
        
        if(ui4_logical_w == 1440 && ui4_logical_h == 1080)
        {
            ui4_logical_w = 1920;
        }
        
        t_sbtl_init.ui4_page_width       = ui4_logical_w;
        t_sbtl_init.ui4_page_height      = ui4_logical_h;
        t_sbtl_init.pf_lock_pib          = _sbtl_dummy_lock_pib_fct;
        t_sbtl_init.pv_tag_lock_pib      = (VOID*) pt_mngr;
        t_sbtl_init.pf_unlock_pib        = _sbtl_dummy_unlock_pib_fct;
        t_sbtl_init.pv_tag_unlock_pib    = (VOID*) pt_mngr;
        t_sbtl_init.pf_get_page_inst_rgn = _sbtl_dummy_get_pib_rgn_fct;
        t_sbtl_init.pv_tag_get_rgn       = (VOID*) pt_mngr;
        i4_ret = sbtl_wgl_create_widget(h_canvas,
                                        SBTL_WGL_WIDGET_SBTL,
                                        SBTL_WGL_BORDER_NULL,
                                        &t_sbtl_rc_scrn,
                                        255,
                                        &t_sbtl_init,
                                        NULL,
                                        &(pt_private->h_sbtl_wgl));
        if (MMSBTLR_OK != i4_ret)
        {
           DBG_LOG_PRINT(("[Dummy] Create widget Fail!\n"));
           return (MMSBTLR_WGL_FAIL);
        }
        
        i4_ret = sbtl_wgl_show(pt_private->h_sbtl_wgl, TRUE);
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }
        if (pt_private->h_sem == NULL_HANDLE)
        {
            i4_ret = x_sema_create(&pt_private->h_sem, 
                           X_SEMA_TYPE_BINARY, 
                           X_SEMA_STATE_UNLOCK);
            if (i4_ret != OSR_OK)
            {
                return MMSBTLR_CORE;
            }
        }      
    }
    else
    {
        return MMSBTLR_NOT_ALLOW;
    }
    
    if (pt_private->h_timer == NULL_HANDLE)
    {
        i4_ret = x_timer_create(&pt_private->h_timer);
        if (i4_ret != OSR_OK || pt_private->h_timer == NULL_HANDLE)
        {
            return MMSBTLR_CORE;
        }
    }

    i4_ret = x_timer_start(pt_private->h_timer,
                           SBTL_DUMMY_PERIOD,
                           X_TIMER_FLAG_REPEAT,
                           _sbtl_timer_cb_fct,
                           pt_mngr);
                    
    if (i4_ret != OSR_OK)
    {
        return MMSBTLR_CORE;
    }

    pt_mngr->b_started = TRUE;
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
static INT32 _sbtl_dummy_stop(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*      pt_private;

    if ((NULL==pt_mngr) || (NULL==pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }
	DBG_LOG_PRINT(("[Dummy] stop start\n"));

    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    if (pt_private->h_timer)
    {
        x_timer_delete(pt_private->h_timer);
        pt_private->h_timer = NULL_HANDLE;
    }
     if (pt_private->h_sem)
    {
        x_sema_delete(pt_private->h_sem);
        pt_private->h_sem = NULL_HANDLE;
    }
    _sbtl_dummy_hide(pt_mngr);
    
    if (pt_private->h_sbtl_wgl)
    {
        sbtl_wgl_destroy_widget(pt_private->h_sbtl_wgl);
        pt_private->h_sbtl_wgl = NULL_HANDLE;
    }

    pt_mngr->b_started = FALSE;
    pt_private->e_state = DUMMY_STATE_NONE;   
	DBG_LOG_PRINT(("[Dummy] stop end\n"));
    return MMSBTLR_OK;
}


static INT32 _sbtl_dummy_handle_timing(SBTL_PAGE_INST_MNGR_T*     pt_mngr,
                                      PTS_T                      t_pts)
{
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*   pt_private;
    BOOL                              b_negative;
    SBTL_DUMMY_FNCTBL_T*              pt_dummy; 
    SBTL_INDEX_INFO_T*                pt_idx;
    UINT32                            ui4_idx;
    DUMMY_PAGE_STATE_T                e_new_state = DUMMY_STATE_NONE;

    if ((NULL == pt_mngr) || 
        (NULL == pt_mngr->pv_private) ||
        (NULL == pt_mngr->pt_engine))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    pt_dummy = &pt_mngr->pt_engine->t_cfg.t_dummy_func;

    if (pt_private->pt_idx == NULL)
    {
        return MMSBTLR_NOT_ALLOW;
    }

    _sbtl_dummy_adjust_pts(&t_pts, pt_mngr, &b_negative);
    
    pt_idx = (SBTL_INDEX_INFO_T*)((UINT8*)pt_private->pt_idx+sizeof(SBTL_DUMMY_HEAD_T));
    
    if (pt_private->pt_data && pt_private->ui4_sel < pt_private->pt_idx->ui4_count)
    {
        if (t_pts < (PTS_T)(pt_idx[pt_private->ui4_sel].ui4_end_time))
        {
            if (t_pts >= (PTS_T)(pt_idx[pt_private->ui4_sel].ui4_start_time))
            {
                e_new_state = DUMMY_STATE_DISPLAY;
            }
            else
            {
                e_new_state = DUMMY_STATE_DATA_RDY;
            }
        }
    }
    if (pt_private->e_state == DUMMY_STATE_DISPLAY)
    {
        if (e_new_state != DUMMY_STATE_DISPLAY)
        {
            _sbtl_dummy_hide(pt_mngr);
        }
        else if (e_new_state != DUMMY_STATE_NONE)
        {
            return MMSBTLR_OK;
        }
    }
       
    if (e_new_state == DUMMY_STATE_NONE)
    {
        for (ui4_idx = 0; ui4_idx < pt_private->pt_idx->ui4_count; ui4_idx++)
        {
            if (t_pts < (PTS_T)(pt_idx[ui4_idx].ui4_end_time))
            {
                break;
            }
        }
        
        if (ui4_idx < pt_private->pt_idx->ui4_count)
        {
            VOID* pv_phy;
            DBG_LOG_PRINT(("[Dummy] Match idx:%d at [%d-%d]\n",ui4_idx, 
                        pt_idx[ui4_idx].ui4_start_time/90, 
                        pt_idx[ui4_idx].ui4_end_time/90));
            if (pt_private->e_type == SBTL_DUMMY_IMG)
            {
               x_sema_lock(pt_private->h_sem, X_SEMA_OPTION_WAIT); 
            }
            pv_phy = pt_dummy->pf_get_data(pt_dummy->pv_tag, ui4_idx);
            if (pv_phy)
            {
                pt_private->pt_data = _sbtl_dummy_get_vir_addr(pt_private, pv_phy);
            }
            else
            {
                DBG_LOG_PRINT(("[Dummy] Get Data Fail\n"));
            }
            
            pt_private->ui4_sel = ui4_idx;
            if (pt_private->e_type == SBTL_DUMMY_IMG)
            {
               x_sema_unlock(pt_private->h_sem); 
            }
            
            if (pt_private->pt_data && 
                (x_strncmp(pt_private->pt_data->s_magic,"DUMY",4) == 0) &&
                pt_private->pt_data->ui4_count > 0)
            {
                if (t_pts >= (PTS_T)(pt_idx[pt_private->ui4_sel].ui4_start_time))
                {
                    e_new_state = DUMMY_STATE_DISPLAY;
                }
                else
                {
                    e_new_state = DUMMY_STATE_DATA_RDY;
                }
                DBG_LOG_PRINT(("[Dummy] New state is %d\n",e_new_state));
            } 
        }
    }

    if (e_new_state == DUMMY_STATE_DISPLAY)
    {
        pt_private->t_pre_pts = t_pts;
        _sbtl_dummy_show(pt_mngr);
    }
    pt_private->e_state = e_new_state;
    return MMSBTLR_OK;
}

static INT32 _sbtl_dummy_config(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                                  VOID*                    pv_cfg_info,
                               UINT32                   ui4_set_type)
{
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*  pt_private;
    INT32                           i4_ret = MMSBTLR_OK;
    GL_RECT_T                       t_sbtl_rc_scrn;
    UINT32                          ui4_canvas_w;
    UINT32                          ui4_canvas_h;
    HANDLE_T                        h_canvas;
    GL_POINT_T*                     pt_osd_ofst;
    GL_RECT_T*                      pt_osd_rect;
    BOOL                            b_check_show = FALSE;
    
    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    switch (ui4_set_type)
    {
    case EVN_MM_SBTL_SET_OSD_FONT_INFO:
        if (pt_private->e_type == SBTL_DUMMY_TEXT)
        {
            i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                  SBTL_WGL_CMD_GL_SET_FONT,
                                  (VOID*)&pt_mngr->pt_engine->t_cfg.t_font_info,
                                  NULL);
             if (MMSBTLR_OK != i4_ret)
             {
                DBG_ERROR(("[Dummy]Set Font fail\n"));
             }
             else if (pt_private->e_state == DUMMY_STATE_DISPLAY)
             {
                i4_ret = sbtl_wgl_show(pt_private->h_sbtl_wgl, TRUE);
             }            
        }
                  
        break;
               
    case EVN_MM_SBTL_SET_SHOW_HIDE: 
        if((*(BOOL*)pv_cfg_info) == TRUE)
        {
            pt_mngr->pt_engine->t_cfg.b_hide = FALSE;
        }
        else
        {
            pt_mngr->pt_engine->t_cfg.b_hide = TRUE;             
        }
        b_check_show = TRUE;
        break;
        
    case EVN_MM_SBTL_SET_ADJUST_TIME:
        i4_ret = _sbtl_dummy_new_page(pt_mngr);
        b_check_show = TRUE;
        break;
    
    case EVN_MM_SBTL_SET_OSD_FG_COLOR:
    case EVN_MM_SBTL_SET_OSD_FONT_EDGE_COLOR:
    case EVN_MM_SBTL_SET_FONT_BG_COLOR:
    case EVN_MM_SBTL_SET_OSD_BG_COLOR:
        
    if (pt_private->e_type == SBTL_DUMMY_TEXT)
    {
        WGL_COLOR_INFO_T* pt_color_info = (WGL_COLOR_INFO_T*)pv_cfg_info;
        UINT32            ui4_type = SBTL_WGL_CLR_TEXT;
        
        if (pt_color_info->e_type != WGL_COLOR_SET_STANDARD &&
            pt_color_info->e_type != WGL_COLOR_SET_BASIC)
        {
            return (MMSBTLR_WGL_FAIL);
        }
        if (ui4_set_type == EVN_MM_SBTL_SET_OSD_FONT_EDGE_COLOR)
        {
            ui4_type = SBTL_WGL_CLR_EDGE;
        }
        else if (ui4_set_type == EVN_MM_SBTL_SET_FONT_BG_COLOR)
        {
            ui4_type = SBTL_WGL_CLR_TEXT_BG;
        }
        else if (ui4_set_type == EVN_MM_SBTL_SET_OSD_BG_COLOR)
        {
            ui4_type = SBTL_WGL_CLR_BK;
        }
        i4_ret = sbtl_wgl_do_cmd (pt_private->h_sbtl_wgl,
                                  SBTL_WGL_CMD_GL_SET_COLOR,
                                  (VOID*)ui4_type,
                                  (VOID*)(pt_color_info));            
        if (MMSBTLR_OK != i4_ret)
        {
           return (MMSBTLR_WGL_FAIL);
        }
        b_check_show = TRUE;
    }
    break;

    case EVN_MM_SBTL_SET_OSD_OFFSET:
        pt_osd_ofst = (GL_POINT_T*)pv_cfg_info;
        h_canvas = pt_mngr->pt_engine->t_cfg.h_gl_plane;

        i4_ret = sbtl_wgl_get_canvas_size(h_canvas, &ui4_canvas_w, &ui4_canvas_h);

        if (MMSBTLR_OK != i4_ret)
        {
            return (MMSBTLR_WGL_FAIL);
        } 
    
        if (pt_mngr->pt_engine->t_cfg.b_rect_osd)
        {
            t_sbtl_rc_scrn = pt_mngr->pt_engine->t_cfg.t_rect_osd;
        }
        else
        {                   
            t_sbtl_rc_scrn.i4_left = (INT32)(ui4_canvas_w/6);
            t_sbtl_rc_scrn.i4_top = (INT32)(ui4_canvas_h/10);
            t_sbtl_rc_scrn.i4_right = (INT32)ui4_canvas_w-(INT32)(ui4_canvas_w/6);
            t_sbtl_rc_scrn.i4_bottom = (INT32)(ui4_canvas_h)-(INT32)(ui4_canvas_h/10);
        }
        
        t_sbtl_rc_scrn.i4_left += pt_osd_ofst->i4_x;
        t_sbtl_rc_scrn.i4_top += pt_osd_ofst->i4_y;
        t_sbtl_rc_scrn.i4_right += pt_osd_ofst->i4_x;
        t_sbtl_rc_scrn.i4_bottom += pt_osd_ofst->i4_y;

        if(t_sbtl_rc_scrn.i4_left < 0)
        {
            t_sbtl_rc_scrn.i4_right -= t_sbtl_rc_scrn.i4_left;
            t_sbtl_rc_scrn.i4_left = 0;
        }

        if(t_sbtl_rc_scrn.i4_top < 0)
        {
            t_sbtl_rc_scrn.i4_bottom -= t_sbtl_rc_scrn.i4_top;
            t_sbtl_rc_scrn.i4_top = 0;
        }
        
        if(t_sbtl_rc_scrn.i4_right > (INT32)ui4_canvas_w)
        {
            t_sbtl_rc_scrn.i4_left -= (t_sbtl_rc_scrn.i4_right - (INT32)ui4_canvas_w);
            t_sbtl_rc_scrn.i4_right = (INT32)ui4_canvas_w;
        }

        if(t_sbtl_rc_scrn.i4_bottom > (INT32)ui4_canvas_h)
        {
            t_sbtl_rc_scrn.i4_top -= (t_sbtl_rc_scrn.i4_bottom - (INT32)ui4_canvas_h);
            t_sbtl_rc_scrn.i4_bottom = (INT32)ui4_canvas_h;
        }
        i4_ret = sbtl_wgl_move(pt_private->h_sbtl_wgl,&t_sbtl_rc_scrn,SBTL_WGL_SYNC_AUTO_REPAINT);
        
        if (MMSBTLR_OK != i4_ret)
        {
           break;
        } 
        b_check_show = TRUE;

        break;

    case EVN_MM_SBTL_SET_OSD_RECT:
        pt_osd_rect = (GL_RECT_T *)pv_cfg_info;
    
        h_canvas = pt_mngr->pt_engine->t_cfg.h_gl_plane;
        i4_ret = sbtl_wgl_get_canvas_size(h_canvas, &ui4_canvas_w, &ui4_canvas_h);
        if (MMSBTLR_OK != i4_ret)
        {
            break;
        }
        if( (pt_osd_rect->i4_left >= pt_osd_rect->i4_right) ||
            (pt_osd_rect->i4_top  >= pt_osd_rect->i4_bottom)||
            (pt_osd_rect->i4_right > ui4_canvas_w) ||
            (pt_osd_rect->i4_top  > ui4_canvas_h))
        {
           break;
        }
        i4_ret = sbtl_wgl_move(pt_private->h_sbtl_wgl,pt_osd_rect, SBTL_WGL_SYNC_AUTO_REPAINT);
        if (MMSBTLR_OK != i4_ret)
        {
           DBG_ERROR(("[Dummy] Move fail!"));
           break;
        }
        b_check_show = TRUE;
        break;
               
    default:
        break;
    }

    if (i4_ret == MMSBTLR_OK && b_check_show)
    {
        b_check_show = pt_mngr->pt_engine->t_cfg.b_hide? FALSE:TRUE;  
        i4_ret = sbtl_wgl_show(pt_private->h_sbtl_wgl, b_check_show); 
    }
    return i4_ret;
}


INT32  _sbtl_dummy_need_dmx(SBTL_PAGE_INST_MNGR_T*     pt_mngr, 
                                     BOOL*                      pb_need)
{
    if (pt_mngr && pb_need)
    {
        *pb_need = FALSE;
    }
    return MMSBTLR_OK;
}
#define SBTL_DUMMY_TXT_BUFFER_SIZE (100*1024)
#define SBTL_DUMMY_IMG_BUFFER_SIZE (2048*1024)
static SBTL_PAGE_INST_MNGR_T* _dummy_page_mngr_constructor (SBTL_ENGINE_T*    pt_engine)
{
    SBTL_PAGE_INST_MNGR_T*           pt_mngr    = NULL;
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*  pt_private = NULL;
    VOID*                            pv_phy = NULL;
    VOID*                            pv_vir = NULL;
    SIZE_T                           z_size = 0;

    if (NULL == pt_engine)
    {
        return NULL;
    }

    do
    {
        SBTL_DUMMY_FNCTBL_T* pt_dummy_func;
		VOID*                pv_idx = NULL;
        pt_dummy_func = &pt_engine->t_cfg.t_dummy_func;
        if ((pt_dummy_func->e_type == SBTL_DUMMY_TEXT || pt_dummy_func->e_type == SBTL_DUMMY_IMG) &&
            pt_dummy_func->pf_build_idx &&
            pt_dummy_func->pf_get_data &&
            pt_dummy_func->pf_grab     &&
            pt_dummy_func->pv_tag)
        {
            //dummy information all right
        }
        else
        {
            break;
        }
        pt_mngr = (SBTL_PAGE_INST_MNGR_T*)x_mem_alloc(sizeof(SBTL_PAGE_INST_MNGR_T));
        if (NULL == pt_mngr)
        {
            break;
        }
        x_memset(pt_mngr, 0, sizeof(SBTL_PAGE_INST_MNGR_T));

        pt_mngr->pf_start = _sbtl_dummy_start;
        pt_mngr->pf_stop = _sbtl_dummy_stop;
        pt_mngr->pf_cfg = _sbtl_dummy_config;
        pt_mngr->pf_handle_timing = _sbtl_dummy_handle_timing;
        pt_mngr->pf_need_dmx = _sbtl_dummy_need_dmx;
        pt_mngr->pf_new_page = _sbtl_dummy_new_page;
        pt_mngr->pf_get = NULL;
        pt_mngr->pt_engine = pt_engine;
        if (pt_dummy_func->e_type == SBTL_DUMMY_TEXT)
        {
            z_size = SBTL_DUMMY_TXT_BUFFER_SIZE;
        }
        else
        {
            z_size = SBTL_DUMMY_IMG_BUFFER_SIZE;
        }
        
        pt_mngr->pv_private = x_mem_alloc(sizeof(SBTL_DUMMY_PAGE_MNGR_PRIVATE_T));

        if (NULL == pt_mngr->pv_private)
        {
            break;
        }
        #ifndef __KERNEL__
        pv_phy = x_share_mem_alloc(z_size);
        #endif
        if (pv_phy == NULL)
        {
            break;
        }
        #ifndef __KERNEL__
        pv_vir = x_mmap_share_mem_rmmgr(pv_phy, z_size);
        #endif
        if (pv_vir == NULL)
        {
            break;
        }
        x_memset(pt_mngr->pv_private, 0, sizeof(SBTL_DUMMY_PAGE_MNGR_PRIVATE_T));
        pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
        
        pt_private->pv_phy = pv_phy;
        pt_private->pv_vir = pv_vir;
        pt_private->ui4_size = (UINT32)z_size;
        if (pt_dummy_func->pf_grab(pt_dummy_func->pv_tag, pv_phy, z_size) != 0)
        {
			DBG_LOG_PRINT(("[Dummy] grab fail\n"));
            break;
        }
        
        pv_idx = pt_dummy_func->pf_build_idx(pt_dummy_func->pv_tag);
        pt_private->pt_idx = _sbtl_dummy_get_vir_addr(pt_private, pv_idx);
		
		DBG_LOG_PRINT(("[Dummy] get idx total:%d\n", pt_private->pt_idx == NULL?0:pt_private->pt_idx->ui4_count));
        
        /*do unmap at rpc side*/
        if (pt_private->pt_idx && 
            (x_strncmp(pt_private->pt_idx->s_magic,"DUMY",4) == 0) &&
            pt_private->pt_idx->ui4_count > 0)
        {
            /*we try to get first packet of data*/
            VOID* pv_data = NULL;
			UINT32 ui4_i;
			for (ui4_i = 0 ; ui4_i < pt_private->pt_idx->ui4_count && ui4_i <10; ui4_i++)
			{ 	
				pv_data = pt_dummy_func->pf_get_data(pt_dummy_func->pv_tag, ui4_i);
            	pt_private->pt_data = _sbtl_dummy_get_vir_addr(pt_private, pv_data);
				if (pt_private->pt_data != NULL)
				{
					break;
				}
			}
			DBG_LOG_PRINT(("[Dummy] get first data:%x\n", (int)pt_private->pt_data));
            if (pt_private->pt_data)
            {
                if (pt_dummy_func->e_type == SBTL_DUMMY_TEXT)
                {
                    pt_private->w2s_str = x_mem_alloc(512*sizeof(UTF16_T));
                    if (pt_private->w2s_str == NULL)
                    {
                        break;
                    }
                }
                pt_private->ui4_sel = ui4_i;
                DBG_LOG_PRINT(("[Dummy]init OK for type:%d\n",pt_dummy_func->e_type));
                pt_private->e_type = pt_dummy_func->e_type;
                return pt_mngr;
            }   
        }
        
    }while(0);
    
    if (pv_phy)
    {
        if (pv_vir)
        {
            #ifndef __KERNEL__
            x_munmap_share_mem(pv_vir, z_size);
            #endif
        }
        #ifndef __KERNEL__
        x_share_mem_free(pv_phy, z_size);
        #endif
    }

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
static VOID _dummy_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*  pt_private;
    SBTL_DUMMY_FNCTBL_T* pt_dummy_func;
    

    if (NULL == pt_mngr || pt_mngr->pt_engine == NULL)
    {
        return;
    }
	DBG_LOG_PRINT(("[Dummy] destructor start\n"));
    pt_dummy_func = &pt_mngr->pt_engine->t_cfg.t_dummy_func;
    pt_private = (SBTL_DUMMY_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if (NULL == pt_private)
    {
        return;
    }
    
    if (pt_private->pv_phy)
    {
        if (pt_dummy_func->pf_release)
        {
            pt_dummy_func->pf_release(pt_dummy_func->pv_tag, 0);
        }
        if (pt_private->pv_vir)
        {
             #ifndef __KERNEL__
             x_munmap_share_mem(pt_private->pv_vir, pt_private->ui4_size);
             #endif
        }
        #ifndef __KERNEL__
        x_share_mem_free(pt_private->pv_phy, pt_private->ui4_size);
        #endif
    }
    
    if (pt_private->w2s_str)
    {
        x_mem_free(pt_private->w2s_str);
    }
    
    x_mem_free(pt_private);
    pt_private = NULL;
    pt_mngr->pt_engine->pt_page_mngr = NULL;
    x_mem_free(pt_mngr);
    pt_mngr = NULL;
	DBG_LOG_PRINT(("[Dummy] destructor end\n"));
}



/*-----------------------------------------------------------------------------
                    External functions implementations
 ----------------------------------------------------------------------------*/
VOID x_sbtl_dummy_init(VOID)
{
    sbtl_register(MM_SBTL_TYPE_DUMMY,
                  &t_dummy_fnct_tbl);
}

