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
 * $RCSfile: sbtl_dync_mngr.c,v $
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
#include "mutil/sbtl_engine/sbtl_widget.h"

#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_wdgt.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_mngr.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_reg.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_util.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_ipcc.h"
#include "mutil/sbtl_engine/sbtl_dbg.h"



typedef enum
{
     DYNC_STATE_NONE,
     DYNC_STATE_DATA_RDY,
     DYNC_STATE_DISPLAY,
     DYNC_STATE_END
} SBTL_DYNC_STATE_T;


#define PTS_FREQ         ((UINT64)(90000))
#define SBTL_DYNC_PERIOD (50)

#define MM_SBTL_FETCH_NEXT_SAMPLE_PERIOD (3*1000)


typedef struct
{
    HANDLE_T              h_sbtl_wgl;
    HANDLE_T              h_timer;
    UINT32                ui4_start_pts;
    UINT32                ui4_end_pts;
    SBTL_DYNC_STATE_T     e_cur_state;
    MM_SBTL_FILE_STACK_T* pt_stack;
    VOID*                 pv_data;
    VOID*                 pv_private;
    BOOL                  b_video_ready;
    SBTL_COMM_TYPE_T      e_using_type;
}SBTL_DYNC_MNGR_T;

static SBTL_PAGE_INST_MNGR_T* _dync_page_mngr_constructor (SBTL_ENGINE_T*    pt_engine);
static VOID _dync_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*   pt_mngr);
static BOOL _dync_page_mngr_verify(SBTL_ENGINE_T*     pt_engine,
                                    UINT8*             pt_type);


static SBTL_MNGR_FCT_TBL_T t_dync_fnct_tbl = 
{
    TRUE,
    _dync_page_mngr_constructor,
    _dync_page_mngr_destructor,
    _dync_page_mngr_verify
};


static INT32 _sbtl_wait_create_wdgt(
    SBTL_ENGINE_T*     pt_engine,
    SBTL_DYNC_MNGR_T*  pt_dync);


static VOID convert_file_info(
    SBTL_BUFFER_FILE_INFO_T*   pt_buf_file,
    MM_SBTL_LOAD_FILE_INFO_T*  pt_file);

static VOID dump_url(SBTL_DYNC_MNGR_T*         pt_dync)
{
    if (pt_dync)
    {
         MM_SBTL_FILE_STACK_T* pt_cur;
         pt_cur = pt_dync->pt_stack;
         while (pt_cur)
         {
            MM_SBTL_LOAD_FILE_INFO_T* pt_file = &(pt_cur->t_info);
            DBG_INFO(("Url:.......................... \n"
                      "  path:%s   (%d Bytes) [%d-%d]"
                      "  handle:%x idxnum:%d  of (%x)\n"
                      "  loaded:%d curidx:%d  sbtl type:%d\n"
                      "  wdgt:%x   private:%x funcs:%x\n",
            pt_file->ps_file_path?pt_file->ps_file_path:"null",
            pt_file->ui4_file_size,
            pt_file->ui4_start_pts,
            pt_file->ui4_end_pts,
           
            pt_file->t_feeder.h_feeder,
            pt_file->ui4_sample_num,
            pt_file->pv_samle_index,
            
            pt_file->b_has_loaded,
            pt_file->ui4_cur_sample,
            pt_file->e_sbtl_type,
            
            pt_file->h_widget,
            pt_file->pv_private,
            pt_file->pt_reg_func));
            
            pt_cur = pt_cur->pt_next;
            (VOID)pt_file;
         }
    }
}


//OK
static INT32 push_url(
    SBTL_DYNC_MNGR_T*         pt_sync,
    MM_SBTL_LOAD_FILE_INFO_T* pt_file)
{
    MM_SBTL_FILE_STACK_T* pt_cur;

    if (pt_sync == NULL || pt_file == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_cur = alloc_one_file(pt_file);

    if (pt_cur == NULL)
    {
        return MMSBTLR_NOT_ALLOW;
    }
  
    if (pt_sync->pt_stack == NULL)
    {
        pt_sync->pt_stack = pt_cur;
    }
    else
    {
        MM_SBTL_FILE_STACK_T* pt_each = pt_sync->pt_stack;
        while (pt_each->pt_next)
        {
            pt_each = pt_each->pt_next;
        }
        pt_each->pt_next = pt_cur;
    }
    return MMSBTLR_OK;
}

static INT32 _try_to_push_url(
    SBTL_DYNC_MNGR_T*         pt_dync,
    MM_SBTL_LOAD_FILE_INFO_T* pt_file, 
    UINT32                    ui4_cur_pts)
{
    if (check_file_valid(pt_file, ui4_cur_pts) == FALSE)
    {
        return MMSBTLR_NOT_SUPPORT;
    }
    if (push_url(pt_dync, pt_file) != MMSBTLR_OK)
    {
        free_one_file(pt_file);
        return MMSBTLR_NOT_SUPPORT;
    }
    dump_url(pt_dync);
    return MMSBTLR_OK;
}

static INT32 _sbtl_dync_pop_url (SBTL_DYNC_MNGR_T* pt_dync)
{
    MM_SBTL_FILE_STACK_T* pt_stack = pt_dync->pt_stack;
    if (pt_stack)
    {
        MM_SBTL_FILE_STACK_T* pt_next = pt_stack->pt_next;
        free_one_file(&pt_stack->t_info);
        pt_dync->pt_stack = pt_next;
    }
    dump_url(pt_dync);
    return MMSBTLR_OK;
}

static INT32 flush_all_url (SBTL_DYNC_MNGR_T* pt_dync)
{
    MM_SBTL_FILE_STACK_T* pt_stack = pt_dync->pt_stack;
    while (pt_stack)
    {
        _sbtl_dync_pop_url(pt_dync);
        pt_stack = pt_dync->pt_stack;
    }
    return MMSBTLR_OK;
}

static MM_SBTL_LOAD_FILE_INFO_T* _sbtl_dync_get_first_file(SBTL_DYNC_MNGR_T* pt_dync)
{
    if (pt_dync && pt_dync->pt_stack)
    {
        return &(pt_dync->pt_stack->t_info);
    }
    return NULL;
}

static MM_SBTL_LOAD_FILE_INFO_T* _sbtl_dync_get_buffered_file(SBTL_DYNC_MNGR_T* pt_dync)
{
    if (pt_dync && 
        pt_dync->pt_stack &&
        pt_dync->pt_stack->pt_next)
    {
        return &(pt_dync->pt_stack->pt_next->t_info);
    }
    return NULL;
}
static BOOL _sbtl_dync_file_overdue(
    MM_SBTL_LOAD_FILE_INFO_T*   pt_cur_file,
    UINT32                      ui4_cur_pts)
{
    if (pt_cur_file)
    {
        if ((pt_cur_file->ui4_end_pts > 0 && ui4_cur_pts >= pt_cur_file->ui4_end_pts) ||
             pt_cur_file->pt_reg_func == NULL ||
             pt_cur_file->pt_reg_func->pf_parse == NULL ||
             pt_cur_file->pt_reg_func->pf_free == NULL)
        {
            DBG_INFO(("overdue for: %d/%d func:%x\n", ui4_cur_pts,  pt_cur_file->ui4_end_pts, pt_cur_file->pt_reg_func));
            return TRUE;
        }
    }
    return FALSE;
}

static INT32 _sbtl_get_cur_playtime(
    SBTL_ENGINE_T* pt_engine, 
    UINT32*        pui4_cur_time)
{
    PTS_T   t_pts = 0;
    INT64   i8_adjust, i8_pts_tmp;
    
    if (pt_engine == NULL )
    {
        return MMSBTLR_INV_ARG;
    }
    i8_adjust = pt_engine->t_cfg.t_time_ofst.i8_time_ofst;
    sbtl_stc_pts(pt_engine, &t_pts);
    i8_pts_tmp = (INT64)(t_pts);
    i8_pts_tmp = _mm_div64(i8_pts_tmp, (PTS_FREQ/1000), NULL);
    i8_pts_tmp += i8_adjust; 
     if (i8_pts_tmp < 0)
    {
        *pui4_cur_time = 0;
    }
    else
    {
        *pui4_cur_time = (UINT32)i8_pts_tmp;
    }
    return MMSBTLR_OK;
}

static VOID _dync_timer_cb_fct(HANDLE_T     pt_tm_handle,
                               VOID*        pv_tag)
{
    MM_SBTL_HDLR_MSG_T      t_msg;
    if (NULL == pv_tag)
    {
        return;
    }
    mm_sbtl_hdlr_send_timer_msg(t_msg, pv_tag);
}

static INT32 _sbtl_dync_new_page(SBTL_PAGE_INST_MNGR_T*  pt_mngr)
{
    /*leave empty body*/
    return MMSBTLR_OK;
}


INT32  _sbtl_dync_need_dmx(
    SBTL_PAGE_INST_MNGR_T*     pt_mngr, 
    BOOL*                      pb_need)
{
    if (pt_mngr && pb_need)
    {
        *pb_need = FALSE;
    }
    return MMSBTLR_OK;
}

static INT32 _sbtl_dync_start(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    INT32                               i4_ret = MMSBTLR_OK;
    SBTL_DYNC_MNGR_T*                   pt_dync;
    SBTL_ENGINE_T*                      pt_engine;
   
    if ( NULL == pt_mngr || 
         NULL == pt_mngr->pv_private ||
         pt_mngr->pt_engine == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    if (pt_mngr->b_started)
    {
        return MMSBTLR_OK;
    }

    pt_dync   = (SBTL_DYNC_MNGR_T*)pt_mngr->pv_private;
    pt_engine = pt_mngr->pt_engine;
    
    if (pt_dync->h_timer == NULL_HANDLE)
    {
        i4_ret = x_timer_create(&pt_dync->h_timer);
        if (pt_dync->h_timer == NULL_HANDLE)
        {
            return MMSBTLR_CORE;
        }
    }

    i4_ret = x_timer_start(pt_dync->h_timer,
                           SBTL_DYNC_PERIOD,
                           X_TIMER_FLAG_REPEAT,
                           _dync_timer_cb_fct,
                           pt_mngr);         
    if (i4_ret != OSR_OK)
    {
        return MMSBTLR_CORE;
    }
    pt_dync->e_cur_state = DYNC_STATE_NONE;
    pt_mngr->b_started = TRUE;
    
    //add this code for too much seek must free resource
    {
        MM_SBTL_LOAD_FILE_INFO_T*   pt_cur_file = NULL;
        UINT32                      ui4_cur_pts = 0;
        MM_SBTL_DYN_BASE_IDX_T*     pt_idx = NULL;
        
        pt_cur_file  =  _sbtl_dync_get_first_file(pt_dync);
        if (pt_dync->b_video_ready)
        {
            _sbtl_get_cur_playtime(pt_engine, &ui4_cur_pts);
        }
        
        if (pt_cur_file &&
            pt_cur_file->pt_reg_func &&
            pt_cur_file->ui4_cur_sample < pt_cur_file->ui4_sample_num &&
            pt_cur_file->pv_samle_index)
        {
            pt_idx = (MM_SBTL_DYN_BASE_IDX_T*)
                       pt_cur_file->pv_samle_index[pt_cur_file->ui4_cur_sample];
            if (pt_idx)
            {
                if (ui4_cur_pts < pt_idx->ui4_start_pts)
                {
                    pt_cur_file->pt_reg_func->pf_free(pt_cur_file, pt_cur_file->ui4_cur_sample);
                    pt_cur_file->ui4_cur_sample = 0;
                    DBG_INFO(("reset cur sample to 0\n"));
                }
            }
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
static INT32 _sbtl_dync_stop(SBTL_PAGE_INST_MNGR_T*     pt_mngr)
{
    SBTL_DYNC_MNGR_T*           pt_dync; 
    MM_SBTL_LOAD_FILE_INFO_T*   pt_cur_file = NULL;

    if ((NULL == pt_mngr) || (NULL == pt_mngr->pv_private))
    {
        return MMSBTLR_INV_ARG;
    }

    pt_dync = (SBTL_DYNC_MNGR_T*)pt_mngr->pv_private;
    
    if (pt_dync->h_timer != NULL_HANDLE)
    {
        x_timer_delete(pt_dync->h_timer);
        pt_dync->h_timer = NULL_HANDLE;
    }
    
    pt_cur_file  =  _sbtl_dync_get_first_file(pt_dync);
    if (pt_cur_file &&
        pt_cur_file->pt_reg_func &&
        pt_cur_file->pt_reg_func->pf_show)
    {
        pt_cur_file->pt_reg_func->pf_show(pt_cur_file, FALSE);
    }
    
    if (pt_dync->h_sbtl_wgl != NULL_HANDLE)
    {
        if (pt_dync->e_using_type == SBTL_COMM_IPCC)
        {
            ipcc_delete_widget(pt_dync->h_sbtl_wgl);
        }
        else
        {
            sbtl_comm_destroy_widget(pt_dync->h_sbtl_wgl);
        }
        pt_dync->h_sbtl_wgl = NULL_HANDLE;
    }

    pt_mngr->b_started = FALSE;
    pt_dync->e_cur_state = DYNC_STATE_NONE;   
    DBG_LOG_PRINT(("[Dync] stop end\n"));
    return MMSBTLR_OK;
}

static INT32 _sbtl_dync_new_sample(
    MM_SBTL_LOAD_FILE_INFO_T*   pt_cur_file,
    UINT32                      ui4_cur_pts)
{
    MM_SBTL_DYN_BASE_IDX_T* pt_idx = NULL;
    BOOL                    b_sample_found = FALSE;
    UINT32                  ui4_i = 0;
    INT32                   i4_ret = 0;
    
    if (pt_cur_file->pt_reg_func->pf_build_idx == NULL ||
        pt_cur_file->ui4_sample_num == 0 ||
        pt_cur_file->pv_samle_index == NULL)
    {
        DBG_INFO(("New sample do not have index\n"));
        return MMSBTLR_OK;
    }
       
    if (pt_cur_file->ui4_cur_sample >= pt_cur_file->ui4_sample_num)
    {
        pt_cur_file->ui4_cur_sample = 0;
    }
    if (ui4_cur_pts > pt_cur_file->ui4_start_pts)
    {
        ui4_cur_pts -= pt_cur_file->ui4_start_pts;
    }
    else
    {
        ui4_cur_pts = 0;
    }
    
    //this sample endtime  > curtime or next starttime > curtime
    for (ui4_i = pt_cur_file->ui4_cur_sample; ui4_i < pt_cur_file->ui4_sample_num; ui4_i++)
    {
        pt_idx = (MM_SBTL_DYN_BASE_IDX_T*) pt_cur_file->pv_samle_index[ui4_i];
        if (pt_idx == NULL)
        {
            DBG_ERROR(("This should not happen at:%d\n", __LINE__));
            continue;
        }
        if (pt_idx->ui4_end_pts)
        {
            if (pt_idx->ui4_end_pts > ui4_cur_pts)
            {
                b_sample_found = TRUE;
                break;
            }
        }
        else if (ui4_i + 1 < pt_cur_file->ui4_sample_num)
        {
            pt_idx = (MM_SBTL_DYN_BASE_IDX_T*) pt_cur_file->pv_samle_index[ui4_i+1];
            if (pt_idx &&
                pt_idx->ui4_start_pts > ui4_cur_pts)
            {
                b_sample_found = TRUE;
                break;
            }
        }
    }

    DBG_API(("sample found: %d -- %d\n", b_sample_found, ui4_i));
    
    if (b_sample_found == FALSE)
    {
        ui4_i = pt_cur_file->ui4_sample_num - 1;
    }
    if (ui4_i != pt_cur_file->ui4_cur_sample)
    {
        i4_ret = pt_cur_file->pt_reg_func->pf_free(pt_cur_file, pt_cur_file->ui4_cur_sample);
        DBG_API(("free sample:%d ret:%d\n", pt_cur_file->ui4_cur_sample, i4_ret));
    }
    DBG_API(("before parse %d at %d\n", ui4_i, sbtl_cur_time()));
    i4_ret = pt_cur_file->pt_reg_func->pf_parse(pt_cur_file, ui4_i);
    pt_cur_file->ui4_cur_sample = ui4_i;
    DBG_API(("pase sample:%d ret:%d at:%d\n", pt_cur_file->ui4_cur_sample, i4_ret,  sbtl_cur_time()));
    
    //prefetch near by samples
    for (ui4_i = pt_cur_file->ui4_cur_sample + 1; ui4_i < pt_cur_file->ui4_sample_num; ui4_i++)
    {
        pt_idx = (MM_SBTL_DYN_BASE_IDX_T*) pt_cur_file->pv_samle_index[ui4_i];
        if (pt_idx &&
            pt_idx->ui4_start_pts > ui4_cur_pts && 
            pt_idx->ui4_start_pts - ui4_cur_pts < MM_SBTL_FETCH_NEXT_SAMPLE_PERIOD)
        {
            DBG_API(("before pre-parse %d at %d\n", ui4_i, sbtl_cur_time()));
            i4_ret = pt_cur_file->pt_reg_func->pf_parse(pt_cur_file, ui4_i);
            DBG_API(("Pre-parse sample:%d [%d %d]ret %d at %d\n", 
                ui4_i, ui4_cur_pts, pt_idx->ui4_start_pts, i4_ret, sbtl_cur_time()));
        }
        else
        {
            break;
        }
    }
    return i4_ret;
}

static INT32 _sbtl_dync_parse(MM_SBTL_LOAD_FILE_INFO_T* pt_file)
{
    INT32 i4_ret = MMSBTLR_OK;
    if (pt_file)
    {
        if ( pt_file->pt_reg_func->pf_init)
        {
            i4_ret = pt_file->pt_reg_func->pf_init(pt_file);
        }
        if (pt_file->pt_reg_func->pf_build_idx == NULL)
        {
            if (pt_file->pt_reg_func->pf_parse)
            {  
               DBG_API(("Start parse at:%d\n", sbtl_cur_time()));
               i4_ret = pt_file->pt_reg_func->pf_parse(pt_file, 0);
               DBG_API(("parse total file ret:%d at:%d\n", i4_ret,  sbtl_cur_time()));
            }
        }
        else
        {
            DBG_API(("Start build idx at:%d\n", x_os_get_sys_tick()));
            pt_file->pt_reg_func->pf_build_idx(pt_file);
            DBG_API(("build index ret:%d at:%d\n", i4_ret, sbtl_cur_time()));
        }
        pt_file->b_is_loading = FALSE;
        pt_file->b_has_loaded = TRUE;
    }
    return i4_ret;
}


static VOID _sbtl_dync_parse_aync(VOID* pv_arg1, VOID* pv_arg2, VOID* pv_arg3)
{
    if (pv_arg1 != NULL)
    {
        _sbtl_dync_parse((MM_SBTL_LOAD_FILE_INFO_T* )pv_arg1);
    }
}

static INT32 _sbtl_check_parsed_as(
    MM_SBTL_LOAD_FILE_INFO_T* pt_file,
    BOOL                      b_sync)
{
    INT32 i4_ret = MMSBTLR_OK;
    
    if (pt_file == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    if (pt_file->b_has_loaded ||
        pt_file->b_is_loading)
    {
        return MMSBTLR_OK;
    }
    pt_file->b_is_loading = TRUE;
    if (b_sync)
    {
        i4_ret = _sbtl_dync_parse(pt_file);
    }
    else
    {
        MM_SBTL_PARSE_HDLR_MSG_T t_parse_msg;
        x_memset(&t_parse_msg, 0, sizeof(MM_SBTL_PARSE_HDLR_MSG_T));
        mm_sbtl_parser_send_call_msg(
                t_parse_msg,
                _sbtl_dync_parse_aync,
                (VOID*)pt_file,
                NULL,
                NULL);
    }
    return i4_ret;
         
}

//file with end time  [ file loaded               fileendtime < curtime freethisfile]
//with no index table[  file unloaded          fileloaded        cannotgetdata  freethisfile+loadnextfile]
//with index table    [  idxnum == 0           idxnum == end  or cannotgetdata]

#define DYNC_SHOW(pt_file,      b_show)  pt_file->pt_reg_func->pf_show(pt_file, b_show)
#define DYNC_PARSE(pt_file,     ui4_idx) pt_file->pt_reg_func->pf_parse(pt_file, ui4_idx)
#define DYNC_BUILD_IDX(pt_file, ui4_idx) pt_file->pt_reg_func->pf_build_idx(pt_file)

static INT32 _sbtl_dync_handle_timing(
       SBTL_PAGE_INST_MNGR_T*     pt_mngr,
       PTS_T                      t_pts)
{
    SBTL_DYNC_MNGR_T*           pt_dync;
    SBTL_DYNC_STATE_T           e_new_state = DYNC_STATE_NONE;
    UINT32                      ui4_cur_pts = 0;
    INT32                       i4_ret      = 0;
    MM_SBTL_LOAD_FILE_INFO_T*   pt_cur_file = NULL;
    MM_SBTL_LOAD_FILE_INFO_T*   pt_buffered_file = NULL;

    BOOL                        b_check_hide = FALSE;

    if ((NULL == pt_mngr) || 
        (NULL == pt_mngr->pv_private) ||
        (NULL == pt_mngr->pt_engine))
    {
        return MMSBTLR_INV_ARG;
    }
    
    DBG_API(("before handle timing at:%d\n", sbtl_cur_time()));
    
    pt_dync      = (SBTL_DYNC_MNGR_T*)pt_mngr->pv_private;
    pt_cur_file  =  _sbtl_dync_get_first_file(pt_dync);
    if (pt_cur_file == NULL)
    {
        DBG_INFO(("No file currently\n"));
        return MMSBTLR_OK;
    }
    if (pt_dync->b_video_ready)
    {
        _sbtl_get_cur_playtime(pt_mngr->pt_engine, &ui4_cur_pts);
    }
   
    //file have end time and whole file overdue
    while ( _sbtl_dync_file_overdue(pt_cur_file, ui4_cur_pts))
    {
        _sbtl_dync_pop_url(pt_dync);
        pt_cur_file =_sbtl_dync_get_first_file(pt_dync);
        if (pt_cur_file == NULL)
        {
            return MMSBTLR_OK;
        }
    }
    
    _sbtl_check_parsed_as(pt_cur_file, FALSE);
    
    //Handle show or hide states
    if (ui4_cur_pts < pt_dync->ui4_start_pts) 
    {
        if (pt_dync->e_cur_state != DYNC_STATE_NONE)
        {
            e_new_state = DYNC_STATE_DATA_RDY;
        }
    }
    else
    {
        if (ui4_cur_pts < pt_dync->ui4_end_pts)
        { 
            e_new_state = pt_dync->e_cur_state;
            if (pt_dync->e_cur_state == DYNC_STATE_DATA_RDY &&
                pt_mngr->pt_engine->t_cfg.b_hide == FALSE)
            {
                if (pt_dync->h_sbtl_wgl != NULL_HANDLE)
                {
                    DBG_API(("before show at:%d\n", sbtl_cur_time()));
                    pt_cur_file->pt_reg_func->pf_show(pt_cur_file, TRUE);
                    DBG_API(("after show at:%d\n", sbtl_cur_time()));
                    e_new_state = DYNC_STATE_DISPLAY;
                }
            }
        }
        else
        {
            if (pt_dync->e_cur_state == DYNC_STATE_DISPLAY)
            {
                //hacking not hide when show soon
                if (pt_cur_file->e_sbtl_type == MM_SBTL_TYPE_XML_IPCC)
                {
                    SBTL_IPCC_PARSER_T* pt_parser = (SBTL_IPCC_PARSER_T*)(pt_cur_file->pv_private);
                    if (pt_parser && pt_parser->b_is_img)
                    {
                        b_check_hide = TRUE;
                    }
                }
                else
                {
                    DBG_API(("before hide at:%d\n", sbtl_cur_time()));
                    pt_cur_file->pt_reg_func->pf_show(pt_cur_file, FALSE);
                    DBG_API(("after hide at:%d\n", sbtl_cur_time()));
                }
                e_new_state = DYNC_STATE_NONE;
            }
        }
    }

    DBG_API(("Toggle state to be %d at %d\n", e_new_state, ui4_cur_pts));
    
    //we need new data now
    if (e_new_state == DYNC_STATE_NONE)
    {
        SBTL_DYNC_GET_DATA_INFO_T t_data = {0};
        t_data.ui4_cur_pts = ui4_cur_pts;
        
        i4_ret = _sbtl_dync_new_sample(pt_cur_file, ui4_cur_pts);
        if (i4_ret == MMSBTLR_OK)
        {
             _sbtl_wait_create_wdgt(pt_mngr->pt_engine, pt_dync);//check data loaded ok
        }
        
        //do not load data util widget create done
        if (pt_dync->h_sbtl_wgl == NULL_HANDLE)
        {
            return MMSBTLR_OK;
        }
        
        // when get data return false means sample reach end or file reach end
        DBG_API(("before getdata at:%d\n", sbtl_cur_time()));
        i4_ret = pt_cur_file->pt_reg_func->pf_get_data(pt_cur_file, &t_data);
        DBG_API(("start to get %d data ret %d at:%d\n", pt_cur_file->ui4_cur_sample, i4_ret,  sbtl_cur_time()));

        if (b_check_hide)
        {
            if (t_data.b_found == FALSE ||
                t_data.ui4_start_pts - pt_dync->ui4_end_pts > 600)
            {
                DBG_API(("before delay hide at:%d\n", sbtl_cur_time()));
                pt_cur_file->pt_reg_func->pf_show(pt_cur_file, FALSE);
                DBG_API(("after delay hide at:%d\n", sbtl_cur_time()));
            }
        }
        
        if (t_data.b_found == FALSE)
        {
            if (pt_cur_file->b_has_loaded)//this flag means parsed done or index build done
            {
                if (pt_cur_file->ui4_cur_sample+1 < pt_cur_file->ui4_sample_num)
                {
                    i4_ret = pt_cur_file->pt_reg_func->pf_free(pt_cur_file, pt_cur_file->ui4_cur_sample);
                    DBG_API(("free sample:%d ret:%d\n", pt_cur_file->ui4_cur_sample, i4_ret));
                    pt_cur_file->ui4_cur_sample++;
                }
                else
                {
                    _sbtl_dync_pop_url(pt_dync);
                    pt_cur_file = _sbtl_dync_get_first_file(pt_dync); 
                }
            }
        }
        else
        {
            pt_dync->ui4_start_pts = t_data.ui4_start_pts;
            pt_dync->ui4_end_pts   = t_data.ui4_end_pts;
            pt_dync->pv_data       = t_data.pv_data;
            DBG_INFO(("find data %d [%d - %d]\n", ui4_cur_pts, pt_dync->ui4_start_pts, pt_dync->ui4_end_pts));

            if (pt_dync->ui4_end_pts > ui4_cur_pts)
            {
                e_new_state = DYNC_STATE_DATA_RDY;
                if (pt_dync->ui4_start_pts <= ui4_cur_pts &&
                    pt_dync->h_sbtl_wgl != NULL_HANDLE &&
                    pt_mngr->pt_engine->t_cfg.b_hide == FALSE)
                {
                    DBG_API(("before show at:%d\n", sbtl_cur_time()));
                    pt_cur_file->pt_reg_func->pf_show(pt_cur_file, TRUE);
                    DBG_API(("after show at:%d\n", sbtl_cur_time()));
                    e_new_state = DYNC_STATE_DISPLAY;
                }
            }
        }
    }

    pt_dync->e_cur_state = e_new_state;
    
    if (pt_cur_file == NULL)
    {
        return MMSBTLR_OK;
    }
    
    pt_buffered_file = _sbtl_dync_get_buffered_file(pt_dync);
    //recheck whether is loaded and prebuffer next file
    _sbtl_check_parsed_as(pt_cur_file, FALSE);
    
    if (pt_buffered_file &&
        pt_buffered_file->ui4_start_pts > ui4_cur_pts &&
        pt_buffered_file->ui4_start_pts - ui4_cur_pts < MM_SBTL_FETCH_NEXT_SAMPLE_PERIOD)
    {
       _sbtl_check_parsed_as(pt_buffered_file, FALSE);
    }
    DBG_API(("after handle timing at:%d\n", sbtl_cur_time()));
       
    return MMSBTLR_OK;
}

//To do: add video check
static INT32 _sbtl_wait_create_wdgt(
    SBTL_ENGINE_T*     pt_engine,
    SBTL_DYNC_MNGR_T*  pt_dync)
{
    INT32 i4_ret = MMSBTLR_OK;
    
    if (pt_dync && pt_dync->b_video_ready)
    {
        MM_SBTL_LOAD_FILE_INFO_T* pt_cur_file; 
        pt_cur_file  =  _sbtl_dync_get_first_file(pt_dync);
        if (pt_cur_file == NULL ||
            pt_cur_file->pt_reg_func == NULL)
        {
            return MMSBTLR_NOT_ALLOW;
        }
        
        pt_cur_file->pt_engine = pt_engine;
        if (pt_cur_file->pt_reg_func->e_comm_type != pt_dync->e_using_type ||
            pt_dync->h_sbtl_wgl == NULL_HANDLE)
        {
            pt_cur_file->h_widget = NULL_HANDLE;
            if (pt_dync->h_sbtl_wgl != NULL_HANDLE)
            {
                if (pt_dync->e_using_type == SBTL_COMM_IPCC)
                {
                    ipcc_delete_widget(pt_dync->h_sbtl_wgl);
                }
                else
                {
                    sbtl_comm_destroy_widget(pt_dync->h_sbtl_wgl);
                }
                pt_dync->h_sbtl_wgl = NULL_HANDLE;
            }
            if (pt_cur_file->pt_reg_func->e_comm_type == SBTL_COMM_IPCC)
            {
                i4_ret = ipcc_create_widget(pt_cur_file);
            }
            else
            {
                sbtl_comm_create_text_wdgt(pt_engine, &pt_cur_file->h_widget);
            }
            pt_dync->h_sbtl_wgl = pt_cur_file->h_widget;
            pt_dync->e_using_type = pt_cur_file->pt_reg_func->e_comm_type;
             DBG_INFO(("End to create widget %x\n", pt_cur_file->h_widget));
        }
        
        pt_cur_file->h_widget = pt_dync->h_sbtl_wgl;
    }
    
    return i4_ret;
}

static INT32 _sbtl_dync_config(SBTL_PAGE_INST_MNGR_T*   pt_mngr,
                                  VOID*                    pv_cfg_info,
                               UINT32                   ui4_set_type)
{
    INT32                          i4_ret = MMSBTLR_OK;
    SBTL_DYNC_MNGR_T*              pt_dync      = NULL;
    MM_SBTL_LOAD_FILE_INFO_T*      pt_cur_file = NULL;
    SBTL_ENGINE_T*                 pt_engine = NULL;
    SBTL_COMM_ATTR_LIST            t_attr;
    
    if (pt_mngr             == NULL ||
        pt_mngr->pt_engine  == NULL ||
        pt_mngr->pv_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_engine    = (SBTL_ENGINE_T* )pt_mngr->pt_engine;
    pt_dync      = (SBTL_DYNC_MNGR_T*)pt_mngr->pv_private;
    pt_cur_file  =  _sbtl_dync_get_first_file(pt_dync);
    if ((pt_cur_file == NULL ||
        pt_cur_file->pt_reg_func == NULL) &&
        ui4_set_type != EVN_MM_SBTL_SET_TYPE_BUFF_FILE_INFO)
    {
        return MMSBTLR_OK;
    }
    x_memset(&t_attr, 0, sizeof(SBTL_COMM_ATTR_LIST));
    
    switch (ui4_set_type)
    {
    case EVN_MM_SBTL_SET_TYPE_DISPLAY_INFO:
            pt_dync->b_video_ready = TRUE;
            _sbtl_wait_create_wdgt(pt_engine, pt_dync);
            break;
    case EVN_MM_SBTL_SET_OSD_FONT_INFO:
        if (pt_cur_file->pt_reg_func->e_comm_type == SBTL_COMM_TEXT)
        {
            t_attr.e_comm_attr = SBTL_COMM_FONT_INFO;
            t_attr.z_attr_size = sizeof(WGL_FONT_INFO_T);
            t_attr.pv_attr_value = &pt_mngr->pt_engine->t_cfg.t_font_info;
            sbtl_comm_set_attr(pt_dync->h_sbtl_wgl, &t_attr);       
        }
                  
        break;
               
    case EVN_MM_SBTL_SET_SHOW_HIDE:
    {
        BOOL b_show = *((BOOL*)pv_cfg_info);
        if (b_show != (!pt_mngr->pt_engine->t_cfg.b_hide))
        {
            pt_mngr->pt_engine->t_cfg.b_hide = !b_show;
            if (b_show)
            {
                _sbtl_dync_handle_timing(pt_mngr, 0);//should be pts
            }
            else
            {
                //sbtl_comm_hide(pt_dync->h_sbtl_wgl);
                if (pt_dync->e_cur_state == DYNC_STATE_DISPLAY)
                {
                    DBG_API(("before hide at:%d\n", sbtl_cur_time()));
                    pt_cur_file->pt_reg_func->pf_show(pt_cur_file, FALSE);
                    DBG_API(("after hide at:%d\n", sbtl_cur_time()));
                    pt_dync->e_cur_state = DYNC_STATE_NONE;
                }
            }
        }
    }
    break;
        
    case EVN_MM_SBTL_SET_ADJUST_TIME:
         sbtl_comm_hide(pt_dync->h_sbtl_wgl);
        _sbtl_dync_handle_timing(pt_mngr, 0);
        break;
    
    case EVN_MM_SBTL_SET_OSD_FG_COLOR:
    case EVN_MM_SBTL_SET_OSD_FONT_EDGE_COLOR:
    case EVN_MM_SBTL_SET_FONT_BG_COLOR:
    case EVN_MM_SBTL_SET_OSD_BG_COLOR:
    {
        WGL_COLOR_INFO_T* pt_color_info = (WGL_COLOR_INFO_T*)pv_cfg_info;
        
        if (pt_color_info->e_type != WGL_COLOR_SET_STANDARD &&
            pt_color_info->e_type != WGL_COLOR_SET_BASIC)
        {
            return (MMSBTLR_WGL_FAIL);
        }
        t_attr.z_attr_size = sizeof(GL_COLOR_T);
        t_attr.pv_attr_value = &pv_cfg_info;
        t_attr.e_comm_attr = SBTL_COMM_FONT_FG_COLOR;
        if (ui4_set_type == EVN_MM_SBTL_SET_OSD_FONT_EDGE_COLOR)
        {
            t_attr.e_comm_attr = SBTL_COMM_FONT_EDGE_COLOR;
        }
        else if (ui4_set_type == EVN_MM_SBTL_SET_FONT_BG_COLOR)
        {
            t_attr.e_comm_attr = SBTL_COMM_FONT_BG_COLOR;
        }
        else if (ui4_set_type == EVN_MM_SBTL_SET_OSD_BG_COLOR)
        {
            t_attr.e_comm_attr = SBTL_COMM_BK_BG_COLOR;
        }
        i4_ret = sbtl_comm_set_attr(pt_dync->h_sbtl_wgl, &t_attr); 
    }
    break;

    case EVN_MM_SBTL_SET_OSD_OFFSET:
    case EVN_MM_SBTL_SET_OSD_RECT:
    {
        GL_RECT_T t_rect = {0};
        t_attr.z_attr_size = sizeof(GL_RECT_T);
        t_attr.pv_attr_value = &t_rect;
        t_attr.e_comm_attr = SBTL_COMM_TEXT_RECT;
        if (ui4_set_type == EVN_MM_SBTL_SET_OSD_OFFSET)
        {
            pt_engine->t_cfg.t_pos_adjust = *((GL_POINT_T*)pv_cfg_info);
        }
        else
        {
            x_memset(&pt_engine->t_cfg.t_pos_adjust, 0, sizeof(GL_POINT_T));
            pt_engine->t_cfg.b_rect_osd = TRUE;
            pt_engine->t_cfg.t_rect_osd = *((GL_RECT_T*) pv_cfg_info);
        }
        i4_ret = sbtl_comm_calc_text_rect( pt_mngr->pt_engine, &t_rect);
        i4_ret = sbtl_comm_set_attr(pt_dync->h_sbtl_wgl, &t_attr); 
    }
    break;

    case EVN_MM_SBTL_SET_TYPE_BUFF_FILE_INFO:
    {
        SBTL_BUFFER_FILE_INFO_T*   pt_buff_file = (SBTL_BUFFER_FILE_INFO_T*)pv_cfg_info;
        MM_SBTL_LOAD_FILE_INFO_T   t_file;
        UINT32                     ui4_cur_t = 0;
        
        x_memset(&t_file, 0, sizeof(MM_SBTL_LOAD_FILE_INFO_T));
        t_file.e_sbtl_type = MM_SBTL_TYPE_NONE;
        convert_file_info(pt_buff_file, &t_file);
        if (pt_dync->b_video_ready)
        {
            _sbtl_get_cur_playtime(pt_mngr->pt_engine, &ui4_cur_t);
        }
        i4_ret = _try_to_push_url(pt_dync, &t_file, ui4_cur_t); 
    }
    break;
    
    case EVN_MM_SBTL_APP_CLOSE_REQ:
    {
         MM_SBTL_FILE_STACK_T* pt_cur;
         pt_cur = pt_dync->pt_stack;
         while (pt_cur)
         {
            pt_cur->t_info.b_force_exit = TRUE;
            pt_cur = pt_cur->pt_next;
         }
    }  
    break;
               
    default:
        break;
    }

    return i4_ret;
}


static VOID convert_file_info(
    SBTL_BUFFER_FILE_INFO_T*   pt_buf_file,
    MM_SBTL_LOAD_FILE_INFO_T*  pt_file)
{
    if (pt_buf_file == NULL || pt_file == NULL)
    {
        return;
    }
    pt_file->ui4_start_pts = pt_buf_file->ui4_start_time;
    pt_file->ui4_end_pts = pt_buf_file->ui4_end_time;
    pt_file->e_show_mode  = pt_buf_file->e_show_mode;
    DBG_INFO(("Buff file type is:%d path:%s\n", pt_buf_file->e_file_type, pt_buf_file->u_info.t_path_info.ps_path));
    switch (pt_buf_file->e_file_type)
    {
        case SBTL_BUFF_FILE_TYPE_PATH:
            pt_file->ps_file_path = pt_buf_file->u_info.t_path_info.ps_path;
            break;
        case SBTL_BUFF_FILE_TYPE_MEM:
            pt_file->ui4_file_size = pt_buf_file->u_info.t_mem_info.ui4_file_size;
            pt_file->pui1_file_buff = pt_buf_file->u_info.t_mem_info.pui1_file_data;
            break;
        default:
            break;
    }
    
}
static BOOL _dync_page_mngr_verify(SBTL_ENGINE_T*     pt_engine,
                                     UINT8*             pt_type)
{
    #if 0
    INT32   i4_ret;
    BOOL    b_found;
    MM_SBTL_LOAD_FILE_INFO_T   t_file;
    x_memset(&t_file, 0, sizeof(MM_SBTL_LOAD_FILE_INFO_T));
    t_file.e_sbtl_type = MM_SBTL_TYPE_NONE;
    convert_file_info(pt_engine, &t_file);
  
    i4_ret = _try_to_push_url(&t_file, 0);
    if (i4_ret == MMSBTLR_OK && 
        t_file.e_sbtl_type != MM_SBTL_TYPE_NONE)
    {
        return TRUE;
    }
    return FALSE;
    #endif
    return TRUE;
}

static SBTL_PAGE_INST_MNGR_T* _dync_page_mngr_constructor (SBTL_ENGINE_T*    pt_engine)
{
    SBTL_PAGE_INST_MNGR_T* pt_mngr    = NULL;
    SBTL_DYNC_MNGR_T*      pt_dync    = NULL;
    INT32                  i4_ret     = MMSBTLR_OK;

    if (NULL == pt_engine)
    {
        return NULL;
    }
    
    do
    {
        MM_SBTL_LOAD_FILE_INFO_T   t_file;
        pt_mngr = (SBTL_PAGE_INST_MNGR_T*)x_mem_alloc(sizeof(SBTL_PAGE_INST_MNGR_T));
        if (NULL == pt_mngr)
        {
            break;
        }
        x_memset(pt_mngr, 0, sizeof(SBTL_PAGE_INST_MNGR_T));

        pt_mngr->pf_start         = _sbtl_dync_start;
        pt_mngr->pf_stop          = _sbtl_dync_stop;
        pt_mngr->pf_cfg           = _sbtl_dync_config;
        pt_mngr->pf_handle_timing = _sbtl_dync_handle_timing;
        pt_mngr->pf_need_dmx      = _sbtl_dync_need_dmx;
        pt_mngr->pf_new_page      = _sbtl_dync_new_page;
        pt_mngr->pf_get           = NULL;
        pt_mngr->pt_engine        = pt_engine;
        
        pt_mngr->pv_private = x_mem_alloc(sizeof(SBTL_DYNC_MNGR_T));

        if (NULL == pt_mngr->pv_private)
        {
            break;
        }
        pt_dync = (SBTL_DYNC_MNGR_T*)pt_mngr->pv_private;
        x_memset(pt_mngr->pv_private, 0, sizeof(SBTL_DYNC_MNGR_T));
        
        x_memset(&t_file, 0, sizeof(MM_SBTL_LOAD_FILE_INFO_T));
        t_file.e_sbtl_type = MM_SBTL_TYPE_NONE;
        convert_file_info(&pt_engine->t_cfg.t_first_info, &t_file);
        i4_ret = _try_to_push_url(pt_dync, &t_file, 0);
        if (i4_ret != MMSBTLR_OK || 
            t_file.e_sbtl_type == MM_SBTL_TYPE_NONE)
        {
            DBG_ERROR(("Push first url fail:%d\n", i4_ret));
            break;
        }
        pt_dync->e_using_type = t_file.e_sbtl_type;
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
static VOID _dync_page_mngr_destructor (SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    SBTL_DYNC_MNGR_T*  pt_dync;
    
    if (NULL == pt_mngr ||
        pt_mngr->pt_engine == NULL ||
        pt_mngr->pv_private == NULL)
    {
        return;
    }
    pt_dync = (SBTL_DYNC_MNGR_T*) pt_mngr->pv_private;
  
    
    flush_all_url(pt_dync);
    
    x_mem_free(pt_dync);
    
    pt_mngr->pt_engine->pt_page_mngr = NULL;
    x_mem_free(pt_mngr);
    pt_mngr = NULL;
    DBG_LOG_PRINT(("[Dync] destructor end\n"));
}

/*-----------------------------------------------------------------------------
                    External functions implementations
 ----------------------------------------------------------------------------*/
VOID x_sbtl_dync_init(VOID)
{
    sbtl_register(MM_SBTL_TYPE_DYNC,
                  &t_dync_fnct_tbl);

    sbtl_ipcc_reg_dync_func();
}
#endif /*__KERNEL__*/


