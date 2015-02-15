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
 * $RCSfile: img_slideshow.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: Photo slideshow library
 *---------------------------------------------------------------------------*/

#include "x_img_slideshow.h"


img_slidshow_nfy_fct pf_slideshow_nfy = NULL;
img_slideshow_nfy_for_fade_fct pf_fade_nfy = NULL;
HANDLE_T    h_binary_sema = NULL_HANDLE;
BOOL        b_stop = FALSE;

HANDLE_T    h_slideshow_queue = NULL_HANDLE;
HANDLE_T    h_slideshow_thread = NULL_HANDLE;

static UINT32       gui4_frame_record = 0;

UINT32      aui4_effect_frame[SLIDESHOW_MAX] = 
    {
         0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
         4,  4,  4,  4,  4,  4,  4,  4,  8,  8, 
         8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
         8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
         8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
         8,  8,  6,  6,  8,  8,  8,  8,  8,  8,
         4,  8,  8
    };

#define HW_FADE

INT32   x_img_stop_slideshow(VOID);
INT32   x_img_sync_slideshow(
    IMG_SLIDE_SHOW_EFFECT_T     effect, 
    UCHAR                       *src_Y,
    UCHAR                       *src_C, //422 YYYY+CbCrCbCr
    UCHAR                       *cur_Y,
    UCHAR                       *cur_C,
    UCHAR                       *dst_Y,
    UCHAR                       *dst_C,
    UINT32                      ui4_Y_buf_size,
    UINT32                      ui4_C_buf_size,
    UINT32                      ui4_width,
    UINT32                      ui4_height,
    UINT32                      ui4_total_time);

VOID slideshow_process_thread_main (VOID *pv_arg)
{
    INT32           i4_ret;
    SIZE_T          z_msg_size;
    UINT16          ui2_index;
    SLIDESHOW_MSG_T t_msg;

    while(1)
    {
        z_msg_size = sizeof(SLIDESHOW_MSG_T);

        i4_ret = x_msg_q_receive(&ui2_index,
                                &t_msg,
                                &z_msg_size,
                                (HANDLE_T*)pv_arg,
                                (UINT16)1,
                                X_MSGQ_OPTION_WAIT);
        if (i4_ret == OSR_OK)
        {
            switch(t_msg.e_type)
            {
                case SLIDESHOW_MSG_START:
                    x_img_sync_slideshow(t_msg.effect,
                                        t_msg.src_Y,
                                        t_msg.src_C,
                                        t_msg.cur_Y,
                                        t_msg.cur_C,
                                        t_msg.dst_Y,
                                        t_msg.dst_C,
                                        t_msg.ui4_Y_buf_size,
                                        t_msg.ui4_C_buf_size,
                                        t_msg.ui4_width,
                                        t_msg.ui4_height,
                                        t_msg.ui4_total_time);
                    break;
                case SLIDESHOW_MSG_EXIT:
                    goto EXIT_THREAD;
                default:
                    goto EXIT_THREAD;
            }
        }
    }
EXIT_THREAD:
    return;
}

INT32   x_img_init_slideshow(img_slidshow_nfy_fct pf_nfy)
{
    INT32   i4_ret = 0;
    
    if (pf_nfy == NULL)
        return SLIDESHOW_INVALID_ARG;
    
    pf_slideshow_nfy = pf_nfy;

    if (h_slideshow_queue == NULL_HANDLE)
    {
        i4_ret = x_msg_q_create(&h_slideshow_queue,
                                "SLIDESHOW_QUEUE",
                                sizeof(SLIDESHOW_MSG_T),
                                20);
        if (i4_ret != OSR_OK)
        {
            return SLIDESHOW_INITIAL_ERROR;
        }
    }

    if (h_slideshow_thread == NULL_HANDLE)
    {
        i4_ret = x_thread_create(   &h_slideshow_thread,
                                    "SLIDESHOW_THREAD",
                                    4096,
                                    128,
                                    slideshow_process_thread_main,
                                    sizeof(HANDLE_T),
                                    &h_slideshow_queue);
        if (i4_ret != OSR_OK)
        {
            return SLIDESHOW_INITIAL_ERROR;
        }
    }
    if (h_binary_sema == NULL_HANDLE)
    {
        i4_ret = x_sema_create( &h_binary_sema, 
                                X_SEMA_TYPE_BINARY, 
                                X_SEMA_STATE_UNLOCK);
        if (i4_ret != OSR_OK)
        {
            
            return SLIDESHOW_SEMA_ERROR;
        }
    }

    b_stop = FALSE;
    
    return SLIDESHOW_OK;
}

INT32 x_img_slideshow_init_for_fade(img_slideshow_nfy_for_fade_fct pf_nfy)
{
    if (pf_nfy == NULL)
    {
        return SLIDESHOW_INVALID_ARG;
    }

    pf_fade_nfy = pf_nfy;

    return SLIDESHOW_OK;
}

INT32   slideshow_fade_in(  
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buf_size,
    UINT32  ui4_C_buf_size,
    UINT32  ui4_total_time)
{
#ifdef HW_FADE
    SLIDESHOW_FADE_T t_fade;
#else   
    INT32   i = 0;
#endif
    UINT32  ui4_frame_number, ui4_slot_time;
    UINT32  ui4_start_time, ui4_end_time;
    INT32   i4_delay_time;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_FADE_IN];
    if (gui4_frame_record >= ui4_frame_number)
    {   
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }
    
    ui4_slot_time = ui4_total_time / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();
#ifdef HW_FADE
    t_fade.ui4_src1_Y_addr = (UINT32)cur_Y;
    t_fade.ui4_src1_C_addr = (UINT32)cur_C;
    t_fade.ui4_src2_Y_addr = (UINT32)src_Y;
    t_fade.ui4_src2_C_addr = (UINT32)src_C;
    t_fade.ui4_dest_Y_addr = (UINT32)dst_Y;
    t_fade.ui4_dest_C_addr = (UINT32)dst_C;
    t_fade.ui4_width = ui4_width;
    t_fade.ui4_height = ui4_height;
    t_fade.ui4_pitch = ((ui4_width + 15) >> 4) << 4; //this should be 16 alignment

    t_fade.ui4_fade_level = (gui4_frame_record+1) * 128 / ui4_frame_number;
    
    pf_fade_nfy(t_fade);

#else
    for (i = 0; i < ui4_Y_buf_size; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }

        dst_Y[i] = (src_Y[i] * (gui4_frame_record + 1) + cur_Y[i] * (ui4_frame_number - (gui4_frame_record + 1))) / ui4_frame_number;

        dst_C[i] = (src_C[i] * (gui4_frame_record + 1) + cur_C[i] * (ui4_frame_number - (gui4_frame_record + 1))) / ui4_frame_number;
    }
    
#endif

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time-ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }
    
    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32   slideshow_hor_block_tl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_TOPLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number*ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number*ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (j >= (ui4_h_step-ui4_h_inc))
            ui4_w_range = ui4_w_step;
        else
            ui4_w_range = ui4_width;

        x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
        x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
    }

    gui4_frame_record++;
    
    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_hor_block_tr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_TOPRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (j >= (ui4_h_step-ui4_h_inc))
            ui4_w_range = ui4_w_step;
        else
            ui4_w_range = ui4_width;

        x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
        x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_hor_block_bl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_BOTTOMLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (j <= (ui4_height-ui4_h_step+ui4_h_inc))
            ui4_w_range = ui4_w_step;
        else
            ui4_w_range = ui4_width;

        x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
        x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_hor_block_br(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_BOTTOMRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (j <= (ui4_height-ui4_h_step+ui4_h_inc))
            ui4_w_range = ui4_w_step;
        else
            ui4_w_range = ui4_width;
        
        x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
        x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_vert_block_tl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_TOPLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = 0; j < ui4_h_step; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
        
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_step);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if (j < ui4_h_step)
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_w_step-ui4_w_inc;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32   slideshow_vert_block_tr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_TOPRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = 0; j < ui4_h_step; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_step, src_Y+tmp+ui4_width-ui4_w_step, ui4_w_step);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_step, src_C+tmp+ui4_width-ui4_w_step, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if (j < ui4_h_step)
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_w_step-ui4_w_inc;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_vert_block_bl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_BOTTOMLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_step);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if (j < (ui4_height - ui4_h_step))
                ui4_w_range = ui4_w_step-ui4_w_inc;
            else
                ui4_w_range = ui4_w_step;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_vert_block_br(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_BOTTOMRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_step, src_Y+tmp+ui4_width-ui4_w_step, ui4_w_step);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_step, src_C+tmp+ui4_width-ui4_w_step, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if (j < ui4_h_step)
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_w_step-ui4_w_inc;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_hor_zigzag_block_tr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_TOPLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (((ui4_h_step / ui4_h_inc)%2) == 1)
        {
            if (j >= (ui4_h_step-ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
        }
        else
        {
            if (j >= (ui4_h_step-ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32   slideshow_hor_zigzag_block_tl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_TOPRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (((ui4_h_step/ui4_h_inc)%2) == 1)
        {
            if (j >= (ui4_h_step-ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
        else
        {
            if (j >= (ui4_h_step-ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32   slideshow_hor_zigzag_block_br(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_BOTTOMLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (((ui4_h_step/ui4_h_inc)%2) == 1)
        {
            if (j <= (ui4_height-ui4_h_step+ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
        }
        else
        {
            if (j <= (ui4_height-ui4_h_step+ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32   slideshow_hor_zigzag_block_bl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_BOTTOMRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        if (((ui4_h_step/ui4_h_inc)%2) == 0)
        {
            if (j <= (ui4_height-ui4_h_step+ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
        }
        else
        {
            if (j <= (ui4_height-ui4_h_step+ui4_h_inc))
                ui4_w_range = ui4_w_step;
            else
                ui4_w_range = ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_vert_zigzag_block_tl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_TOPLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = 0; j < ui4_h_step; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_step);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if ((ui4_w_step/ui4_w_inc)%2 == 1)
            {
                if (j < ui4_h_step)
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
                x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
            }
            else
            {
                if (j >= (ui4_height - ui4_h_step))
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
                x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
            }
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32   slideshow_vert_zigzag_block_tr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_TOPRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = 0; j < ui4_h_step; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_step, src_Y+tmp+ui4_width-ui4_w_step, ui4_w_step);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_step, src_C+tmp+ui4_width-ui4_w_step, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if ((ui4_w_step / ui4_w_inc)%2 == 1)
            {
                if (j < ui4_h_step)
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
                x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
            }
            else
            {
                if (j >= (ui4_height - ui4_h_step))
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
                x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
            }
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_vert_zigzag_block_bl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_BOTTOMLEFT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_step);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if ((ui4_w_step / ui4_w_inc)%2 == 1)
            {
                if (j > (ui4_height - ui4_h_step))
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
                x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
            }
            else
            {
                if (j < ui4_h_step)
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
                x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
            }
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_vert_zigzag_block_br(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_h_inc, ui4_h_step, ui4_w_range;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_BOTTOMRIGHT];
    
    if (gui4_frame_record >= (ui4_frame_number * ui4_frame_number))
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_w_step = ui4_w_inc * ((gui4_frame_record/(UINT32)(ui4_frame_number))+1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(UINT32)(ui4_frame_number))+1);
    ui4_slot_time = ui4_total_time / (ui4_frame_number * ui4_frame_number);

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen); 

    if (ui4_w_step == ui4_w_inc)
    {
        for (j = (ui4_height-ui4_h_step); j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_width-ui4_w_step, src_Y+tmp+ui4_width-ui4_w_step, ui4_w_step);
            x_memcpy(dst_C+tmp+ui4_width-ui4_w_step, src_C+tmp+ui4_width-ui4_w_step, ui4_w_step);
        }
    }
    else
    {
        for (j = 0; j < ui4_height; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j * ui4_width;
            if ((ui4_w_step / ui4_w_inc)%2 == 0)
            {
                if (j < ui4_h_step)
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
                x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
            }
            else
            {
                if (j >= (ui4_height-ui4_h_step))
                    ui4_w_range = ui4_w_step;
                else
                    ui4_w_range = ui4_w_step-ui4_w_inc;
                
                x_memcpy(dst_Y+tmp+ui4_width-ui4_w_range, src_Y+tmp+ui4_width-ui4_w_range, ui4_w_range);
                x_memcpy(dst_C+tmp+ui4_width-ui4_w_range, src_C+tmp+ui4_width-ui4_w_range, ui4_w_range);
            }
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32   slideshow_barwipe_lr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARWIPE_LEFT2RIGHT];
    
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_slot_time = ui4_total_time / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j*ui4_width;

        x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_step);
        x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32   slideshow_barwipe_rl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARWIPE_RIGHT2LEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_w_inc = ui4_width / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    
    ui4_w_step = (gui4_frame_record+1) * ui4_w_inc;
    ui4_offset = ui4_width - ui4_w_step;

    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j*ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32   slideshow_barwipe_tb(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step;
    INT32   j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARWIPE_TOP2BOTTOM];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;

    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j*ui4_width;
        
        x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_width);
        x_memcpy(dst_C+tmp, src_C+tmp, ui4_width);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32   slideshow_barwipe_bt(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARWIPE_BOTTOM2TOP];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width;

    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j*ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_width);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_width);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32 slideshow_barndoorwipe_vert_open(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_VERTICAL_OPEN];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }
    
    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_offset = ((ui4_width>>1) - ui4_w_step);

    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j*ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step*2);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step*2);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

INT32 slideshow_barndoorwipe_vert_close(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   i, j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_VERTICAL_CLOSE];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;

    
    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_offset = ui4_width - ui4_w_step;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        for (i = 0; i < 2; i++)
        {
            tmp = j*ui4_width+i*ui4_offset;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_step);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_step);
        }
    }

    gui4_frame_record++;
    
    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

INT32 slideshow_barndoorwipe_hor_open(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_HORIZONTAL_OPEN];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;
    ui4_offset = ((ui4_height>>1) - ui4_h_step) * ui4_width;

    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    for (j = 0; j < ui4_h_step*2; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j*ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_width);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_width);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

INT32 slideshow_barndoorwipe_hor_close(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_offset;
    INT32   i, j;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_HORIZONTAL_CLOSE];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width;

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < ui4_h_step; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = j*ui4_width+i*ui4_offset;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_width);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_width);
        }
    }

    gui4_frame_record++;
    
    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

INT32 slideshow_iriswipe_outer( 
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_IRISWIPE_OUTER];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = ((ui4_height>>1) - ui4_h_step)*ui4_width + (ui4_width/2-ui4_w_step);
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (i = 0; i < ui4_h_step*2; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i*ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step*2);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step*2);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32 slideshow_iriswipe_center(    
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_IRISWIPE_CENTER];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, src_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, src_C, ui4_C_buflen);

    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = cur_Y;
    tmp_src_C = cur_C;
    ui4_offset = ui4_h_step*ui4_width + ui4_w_step;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (i = 0; i < (ui4_height - ui4_h_step*2); i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, (ui4_width-ui4_w_step*2));
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, (ui4_width-ui4_w_step*2));
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_tl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step ,ui4_w_inc, ui4_w_step;
    INT32   i;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_TOPLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_step);
        x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;
    
    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_tr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_TOPRIGHT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = ui4_width - ui4_w_step;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;
    
    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_bl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;
    
    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
        
    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_br(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_dst_Y, *tmp_dst_C, *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMRIGHT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = (ui4_height - ui4_h_step)*ui4_width + (ui4_width - ui4_w_step);
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;
    
    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
        
    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_tc(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_TOPCENTER];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = (ui4_width>>1)-ui4_w_step;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step*2);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step*2);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
        
    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_rc(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_RIGHTCENTER];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = ((ui4_height>>1)-ui4_h_step)*ui4_width + (ui4_width - ui4_w_step);
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (i = 0; i < ui4_h_step*2; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
        
    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_bc(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMCENTER];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = (ui4_height-ui4_h_step)*ui4_width + ((ui4_width>>1) - ui4_w_step);
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step*2);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step*2);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }
    
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
            
    return SLIDESHOW_OK;
}

INT32   slideshow_boxwipe_lc(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXWIPE_LEFTCENTER];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record+1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = ((ui4_height>>1)-ui4_h_step)*ui4_width;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (i = 0; i < ui4_h_step*2; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
            
    return SLIDESHOW_OK;
}

INT32   slideshow_diagonal_tl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_w_range;
    INT32   i;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / (ui4_frame_number>>1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);
    ui4_w_inc = ui4_width / (ui4_frame_number>>1);
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    if (gui4_frame_record < (ui4_frame_number/2))
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = (ui4_h_step - i) * ui4_w_step / ui4_h_step;
            tmp = i*ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }
    else
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = i * ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_width);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_width);
        }
        for (i = ui4_h_step; i < ui4_height; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = ui4_width - (i - ui4_h_step)*(ui4_width-ui4_w_step) /(ui4_height-ui4_h_step);
            tmp = i * ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_diagonal_tr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step;
    UINT32  ui4_w_range;
    INT32   i;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / (ui4_frame_number>>1);
    
    ui4_w_inc = ui4_width / (ui4_frame_number>>1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);

    if (gui4_frame_record < ui4_frame_number/2)
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = ui4_width - (ui4_h_step - i)* ui4_w_step / ui4_h_step ;
            tmp = i * ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_w_range, src_Y+tmp+ui4_w_range, ui4_width-ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_w_range, src_C+tmp+ui4_w_range, ui4_width-ui4_w_range);
        }
    }
    else
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = i * ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_width);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_width);
        }
        
        for (i = ui4_h_step; i < ui4_height; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = (i-ui4_h_step) * (ui4_width - ui4_w_step) / (ui4_height - ui4_h_step);
            tmp = i * ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_w_range, src_Y+tmp+ui4_w_range, ui4_width-ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_w_range, src_C+tmp+ui4_w_range, ui4_width-ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
            
    return SLIDESHOW_OK;
}

INT32   slideshow_diagonal_bl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step;
    UINT32  ui4_offset,ui4_w_range;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / (ui4_frame_number>>1);
    
    ui4_w_inc = ui4_width / (ui4_frame_number>>1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    if (gui4_frame_record < ui4_frame_number/2)
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = i* ui4_w_step / ui4_h_step ;
            tmp = i * ui4_width;
            
            x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_range);
            x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_range);
        }
    }
    else
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = i * ui4_width;
            
            x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_width);
            x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_width);
        }
        
        for (i = 0; i < (ui4_height-ui4_h_step); i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = ui4_width - (ui4_height-ui4_h_step-i) * (ui4_width - ui4_w_step) / (ui4_height - ui4_h_step);
            tmp = i * ui4_width;
            
            x_memcpy(dst_Y+tmp, src_Y+tmp, ui4_w_range);
            x_memcpy(dst_C+tmp, src_C+tmp, ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
            
    return SLIDESHOW_OK;
}

INT32   slideshow_diagonal_br(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step;
    UINT32  ui4_offset,ui4_w_range;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / (ui4_frame_number>>1);
    
    ui4_w_inc = ui4_width / (ui4_frame_number>>1);
    ui4_h_step = ui4_h_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);
    ui4_w_step = ui4_w_inc * ((gui4_frame_record%(ui4_frame_number/2))+1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = (ui4_height - ui4_h_step)*ui4_width;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    if (gui4_frame_record < ui4_frame_number/2)
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = ui4_width - i * ui4_w_step/ ui4_h_step ;
            tmp = i * ui4_width;
            
            x_memcpy(tmp_dst_Y+tmp+ui4_w_range, tmp_src_Y+tmp+ui4_w_range, ui4_width-ui4_w_range);
            x_memcpy(tmp_dst_C+tmp+ui4_w_range, tmp_src_C+tmp+ui4_w_range, ui4_width-ui4_w_range);
        }
    }
    else
    {
        for (i = 0; i < ui4_h_step; i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp = i * ui4_width;
            
            x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_width);
            x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_width);
        }
        
        for (i = 0; i < (ui4_height-ui4_h_step); i++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            ui4_w_range = (ui4_height - ui4_h_step - i) * (ui4_width - ui4_w_step) / (ui4_height - ui4_h_step);
            tmp = i * ui4_width;
            
            x_memcpy(dst_Y+tmp+ui4_w_range, src_Y+tmp+ui4_w_range, ui4_width-ui4_w_range);
            x_memcpy(dst_C+tmp+ui4_w_range, src_C+tmp+ui4_w_range, ui4_width-ui4_w_range);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
                
    return SLIDESHOW_OK;
}

INT32   slideshow_barslide_lr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARSLIDE_LEFT2RIGHT];
    
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_slot_time = ui4_total_time / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = ui4_width - ui4_w_step;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        
        x_memcpy(dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32   slideshow_barslide_rl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARSLIDE_RIGHT2LEFT];
    
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_slot_time = ui4_total_time / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = ui4_width - ui4_w_step;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_barslide_tb(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARSLIDE_TOP2BOTTOM];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;    

    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        
        x_memcpy(dst_Y+tmp, tmp_src_Y+tmp, ui4_width);
        x_memcpy(dst_C+tmp, tmp_src_C+tmp, ui4_width);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}

INT32   slideshow_barslide_bt(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;
    
    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARSLIDE_BOTTOM2TOP];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;    

    for (j = 0; j < ui4_h_step; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, src_Y+tmp, ui4_width);
        x_memcpy(tmp_dst_C+tmp, src_C+tmp, ui4_width);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;
}


INT32 slideshow_boxslide_tl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step,ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_TOPLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record + 1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record + 1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width + (ui4_width - ui4_w_step);
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }       

    return SLIDESHOW_OK;
}

INT32 slideshow_boxslide_tr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_src_offset, ui4_dst_offset;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_TOPRIGHT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record + 1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record + 1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_src_offset = (ui4_height - ui4_h_step) * ui4_width ;
    ui4_dst_offset = ui4_width - ui4_w_step;
    tmp_src_Y += ui4_src_offset;
    tmp_src_C += ui4_src_offset;
    tmp_dst_Y += ui4_dst_offset;
    tmp_dst_C += ui4_dst_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);

    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
        
    return SLIDESHOW_OK;
}

INT32 slideshow_boxslide_bl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_src_offset, ui4_dst_offset;
    INT32   i;
    UCHAR   *tmp_src_Y, *tmp_src_C, *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_BOTTOMLEFT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record + 1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record + 1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_dst_offset = (ui4_height - ui4_h_step) * ui4_width ;
    ui4_src_offset = ui4_width - ui4_w_step;
    tmp_src_Y += ui4_src_offset;
    tmp_src_C += ui4_src_offset;
    tmp_dst_Y += ui4_dst_offset;
    tmp_dst_C += ui4_dst_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, tmp_src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }   

    return SLIDESHOW_OK;
}

INT32 slideshow_boxslide_br(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_w_inc, ui4_w_step, ui4_offset;
    INT32   i;
    UCHAR   *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_BOTTOMRIGHT];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = ui4_height / ui4_frame_number;
    ui4_h_step = ui4_h_inc * (gui4_frame_record + 1);
    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = ui4_w_inc * (gui4_frame_record + 1);

    ui4_start_time = x_os_get_sys_tick();
    
    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = (ui4_height - ui4_h_step) * ui4_width  + (ui4_width - ui4_w_step);
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (i = 0; i < ui4_h_step; i++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = i * ui4_width;
        
        x_memcpy(tmp_dst_Y+tmp, src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, src_C+tmp, ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }   

    return SLIDESHOW_OK;
}

INT32   slideshow_barndoorslide_vert_open(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   i, j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp1, tmp2;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_VERTICAL_OPEN];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_offset = ((ui4_width>>1) - ui4_w_step);

    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        for (i = 0; i < 2; i++)
        {
            tmp1 = j*ui4_width+i*ui4_w_step;
            tmp2 = j*ui4_width+i*(ui4_width-ui4_w_step);
            
            x_memcpy(tmp_dst_Y+tmp1, src_Y+tmp2, ui4_w_step);
            x_memcpy(tmp_dst_C+tmp1, src_C+tmp2, ui4_w_step);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);   
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

INT32 slideshow_barndoorslide_vert_close(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_src_offset;
    INT32   i, j;
    UCHAR   *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp1, tmp2;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_VERTICAL_CLOSE];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_w_inc = (ui4_width>>1) / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_src_offset = (ui4_width>>1) - ui4_w_step;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_src_Y += ui4_src_offset;
    tmp_src_C += ui4_src_offset;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        for (i = 0; i < 2; i++)
        {
            tmp1 = j*ui4_width+i*(ui4_width-ui4_w_step);
            tmp2 = j*ui4_width+i*ui4_w_step;
            
            x_memcpy(dst_Y+tmp1, tmp_src_Y+tmp2, ui4_w_step);
            x_memcpy(dst_C+tmp1, tmp_src_C+tmp2, ui4_w_step);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

INT32 slideshow_barndoorslide_hor_open(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_dst_offset;
    INT32   i, j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp1, tmp2;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_HORIZONTAL_OPEN];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;
    ui4_dst_offset = ((ui4_height>>1) - ui4_h_step) * ui4_width;

    tmp_dst_Y += ui4_dst_offset;
    tmp_dst_C += ui4_dst_offset;

    for (i = 0 ;i < 2; i++)
    {
        for (j = 0; j < ui4_h_step; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp1 = j*ui4_width+i*ui4_h_step*ui4_width;
            tmp2 = j*ui4_width+i*(ui4_height-ui4_h_step)*ui4_width;
            
            x_memcpy(tmp_dst_Y+tmp1, src_Y+tmp2, ui4_width);
            x_memcpy(tmp_dst_C+tmp1, src_C+tmp2, ui4_width);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

INT32 slideshow_barndoorslide_hor_close(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_h_inc=0, ui4_h_step, ui4_offset;
    INT32   i, j;
    UCHAR   *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp1, tmp2;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_HORIZONTAL_CLOSE];
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_slot_time = ui4_total_time / ui4_frame_number;
    ui4_h_inc = (ui4_height>>1) / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    
    ui4_h_step = (gui4_frame_record +1) * ui4_h_inc;
    ui4_offset = ((ui4_height>>1) - ui4_h_step) * ui4_width;
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < ui4_h_step; j++)
        {
            if (b_stop)
            {
                x_sema_unlock(h_binary_sema);
                return SLIDESHOW_STOP;
            }
            
            tmp1 = j*ui4_width+i*(ui4_height-ui4_h_step)*ui4_width;
            tmp2 = j*ui4_width+i*ui4_h_step*ui4_width;
            
            x_memcpy(dst_Y+tmp1, tmp_src_Y+tmp2, ui4_width);
            x_memcpy(dst_C+tmp1, tmp_src_C+tmp2, ui4_width);
        }
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }

    return SLIDESHOW_OK;    
}

//hua dong te xiao
INT32 slideshow_slide_lr(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_src_Y, *tmp_src_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp, tmp1;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_SLIDE_LEFT2RIGHT];
    
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_slot_time = ui4_total_time / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_src_Y = src_Y;
    tmp_src_C = src_C;
    ui4_offset = ui4_width - ui4_w_step;
    tmp_src_Y += ui4_offset;
    tmp_src_C += ui4_offset;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        tmp1 = ui4_w_step + tmp;
        
        x_memcpy(dst_Y+tmp, tmp_src_Y+tmp, ui4_w_step);
        x_memcpy(dst_C+tmp, tmp_src_C+tmp, ui4_w_step);

        x_memcpy(dst_Y+tmp1, cur_Y+tmp, ui4_width - ui4_w_step);
        x_memcpy(dst_C+tmp1, cur_C+tmp, ui4_width - ui4_w_step);
    }

    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32 slideshow_slide_rl(
    UCHAR   *src_Y,
    UCHAR   *src_C, 
    UCHAR   *cur_Y,
    UCHAR   *cur_C,
    UCHAR   *dst_Y,
    UCHAR   *dst_C,
    UINT32  ui4_width,
    UINT32  ui4_height,
    UINT32  ui4_Y_buflen,
    UINT32  ui4_C_buflen,
    UINT32  ui4_total_time)
{
    UINT32  ui4_w_inc=0, ui4_w_step, ui4_offset;
    INT32   j;
    UCHAR   *tmp_dst_Y, *tmp_dst_C;
    UINT32  ui4_start_time, ui4_end_time, ui4_slot_time, ui4_frame_number;
    INT32   i4_delay_time;
    UINT32  tmp, tmp1;

    x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);

    ui4_frame_number = aui4_effect_frame[IMG_SLIDE_SHOW_EFFECT_SLIDE_RIGHT2LEFT];
    
    if (gui4_frame_record >= ui4_frame_number)
    {
        gui4_frame_record = 0;
        x_sema_unlock(h_binary_sema);
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_PIC);
        return SLIDESHOW_OK;
    }

    ui4_w_inc = ui4_width / ui4_frame_number;
    ui4_w_step = (gui4_frame_record +1) * ui4_w_inc;
    ui4_slot_time = ui4_total_time / ui4_frame_number;

    ui4_start_time = x_os_get_sys_tick();

    //x_memcpy(dst_Y, cur_Y, ui4_Y_buflen);
    //x_memcpy(dst_C, cur_C, ui4_C_buflen);
    tmp_dst_Y = dst_Y;
    tmp_dst_C = dst_C;
    ui4_offset = ui4_width - ui4_w_step;
    tmp_dst_Y += ui4_offset;
    tmp_dst_C += ui4_offset;

    for (j = 0; j < ui4_height; j++)
    {
        if (b_stop)
        {
            x_sema_unlock(h_binary_sema);
            return SLIDESHOW_STOP;
        }
        
        tmp = j * ui4_width;
        tmp1 = tmp + ui4_w_step;
        
        x_memcpy(tmp_dst_Y+tmp, src_Y+tmp, ui4_w_step);
        x_memcpy(tmp_dst_C+tmp, src_C+tmp, ui4_w_step);

        x_memcpy(dst_Y+tmp, cur_Y+tmp1, ui4_offset);
        x_memcpy(dst_C+tmp, cur_C+tmp1, ui4_offset);
    }
    
    gui4_frame_record++;

    ui4_end_time = x_os_get_sys_tick();
    i4_delay_time = ui4_slot_time - (ui4_end_time - ui4_start_time) * x_os_get_sys_tick_period();
    if (i4_delay_time > 0)
    {
        x_thread_delay(i4_delay_time);
    }

    x_sema_unlock(h_binary_sema);
    if (pf_slideshow_nfy)
    {
        pf_slideshow_nfy(SLIDESHOW_NFY_COMPLETE_ONE_FRAME);
    }
    
    return SLIDESHOW_OK;
}

INT32   x_img_async_slideshow(
    IMG_SLIDE_SHOW_EFFECT_T     effect, 
    UCHAR                       *src_Y,
    UCHAR                       *src_C, //422 YYYY+CbCrCbCr
    UCHAR                       *cur_Y,
    UCHAR                       *cur_C,
    UCHAR                       *dst_Y,
    UCHAR                       *dst_C,
    UINT32                      ui4_Y_buf_size,
    UINT32                      ui4_C_buf_size,
    UINT32                      ui4_width,
    UINT32                      ui4_height,
    UINT32                      ui4_total_time)
{
    INT32           i4_ret;
    SLIDESHOW_MSG_T t_msg;

    t_msg.e_type = SLIDESHOW_MSG_START;
    t_msg.effect = effect;
    t_msg.src_Y = src_Y;
    t_msg.src_C = src_C;
    t_msg.cur_Y = cur_Y;
    t_msg.cur_C = cur_C;
    t_msg.dst_Y = dst_Y;
    t_msg.dst_C = dst_C;
    t_msg.ui4_Y_buf_size = ui4_Y_buf_size;
    t_msg.ui4_C_buf_size = ui4_C_buf_size;
    t_msg.ui4_width = ui4_width;
    t_msg.ui4_height = ui4_height;
    t_msg.ui4_total_time = ui4_total_time;
    
    i4_ret = x_msg_q_send(h_slideshow_queue,
                            &t_msg,
                            sizeof(SLIDESHOW_MSG_T),
                            10);
    if (i4_ret != OSR_OK)
    {
        return SLIDESHOW_MSG_ERROR;
    }

    return SLIDESHOW_OK;
}


INT32   x_img_sync_slideshow(
    IMG_SLIDE_SHOW_EFFECT_T     effect, 
    UCHAR                       *src_Y,
    UCHAR                       *src_C, //422 YYYY+CbCrCbCr
    UCHAR                       *cur_Y,
    UCHAR                       *cur_C,
    UCHAR                       *dst_Y,
    UCHAR                       *dst_C,
    UINT32                      ui4_Y_buf_size,
    UINT32                      ui4_C_buf_size,
    UINT32                      ui4_width,
    UINT32                      ui4_height,
    UINT32                      ui4_total_time)
{
    INT32   i4_ret = 0;
    
    if (src_Y == NULL || src_C == NULL  ||
        cur_Y == NULL || cur_C == NULL  ||
        dst_Y == NULL || dst_C == NULL  ||
        ui4_Y_buf_size == 0             ||
        ui4_C_buf_size == 0             ||
        ui4_width == 0                  ||
        ui4_height== 0                  ||
        ui4_total_time == 0)
    {
        return SLIDESHOW_INVALID_ARG;
    }
    
    switch (effect)
    {
//#if (ENABLE_PHOTO_TRANS_EFFECT_BLOCKS)
        case IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_TOPLEFT:
            i4_ret = slideshow_hor_block_tl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_TOPRIGHT:
            i4_ret = slideshow_hor_block_tr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_BOTTOMLEFT:
            i4_ret = slideshow_hor_block_bl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_BOTTOMRIGHT:
            i4_ret = slideshow_hor_block_br(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_TOPLEFT:
            i4_ret = slideshow_vert_block_tl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_TOPRIGHT:
            i4_ret = slideshow_vert_block_tr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_BOTTOMLEFT:
            i4_ret = slideshow_vert_block_bl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_BLOCKS_BOTTOMRIGHT:
            i4_ret = slideshow_vert_block_br(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_TOPLEFT:
            i4_ret = slideshow_hor_zigzag_block_tl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_TOPRIGHT:
            i4_ret = slideshow_hor_zigzag_block_tr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_BOTTOMLEFT:
            i4_ret = slideshow_hor_zigzag_block_bl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_BOTTOMRIGHT:
            i4_ret = slideshow_hor_zigzag_block_br(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_TOPLEFT:
            i4_ret = slideshow_vert_zigzag_block_tl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_TOPRIGHT:
            i4_ret = slideshow_vert_zigzag_block_tr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_BOTTOMLEFT:
            i4_ret = slideshow_vert_zigzag_block_bl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_BOTTOMRIGHT:
            i4_ret = slideshow_vert_zigzag_block_br(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
//#endif
//#if (ENABLE_PHOTO_TRANS_EFFECT_DIAGONAL_BLOCKS)
        case IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPLEFT:
            i4_ret = slideshow_diagonal_tl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPRIGHT:
            i4_ret = slideshow_diagonal_tr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_BOTTOMLEFT:
            i4_ret = slideshow_diagonal_bl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_BOTTOMRIGHT:
            i4_ret = slideshow_diagonal_br(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BLOCKS_RANDOM:
            break;
//#endif
//#if (ENABLE_PHOTO_TRANS_EFFECT_WIPE)
        case IMG_SLIDE_SHOW_EFFECT_BARWIPE_LEFT2RIGHT:
            i4_ret = slideshow_barwipe_lr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARWIPE_RIGHT2LEFT:
            i4_ret = slideshow_barwipe_rl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARWIPE_TOP2BOTTOM:
            i4_ret = slideshow_barwipe_tb(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARWIPE_BOTTOM2TOP:
            i4_ret = slideshow_barwipe_bt(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_TOPLEFT:
            i4_ret = slideshow_boxwipe_tl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_TOPRIGHT:
            i4_ret = slideshow_boxwipe_tr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMLEFT:
            i4_ret = slideshow_boxwipe_bl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMRIGHT:
            i4_ret = slideshow_boxwipe_br(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_HORIZONTAL_OPEN:
            i4_ret = slideshow_barndoorwipe_hor_open(src_Y,
                                                    src_C, 
                                                    cur_Y,
                                                    cur_C,
                                                    dst_Y,
                                                    dst_C,
                                                    ui4_width,
                                                    ui4_height,
                                                    ui4_Y_buf_size,
                                                    ui4_C_buf_size,
                                                    ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_HORIZONTAL_CLOSE:
            i4_ret = slideshow_barndoorwipe_hor_close(src_Y,
                                                    src_C, 
                                                    cur_Y,
                                                    cur_C,
                                                    dst_Y,
                                                    dst_C,
                                                    ui4_width,
                                                    ui4_height,
                                                    ui4_Y_buf_size,
                                                    ui4_C_buf_size,
                                                    ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_VERTICAL_OPEN:
            i4_ret = slideshow_barndoorwipe_vert_open(src_Y,
                                                    src_C, 
                                                    cur_Y,
                                                    cur_C,
                                                    dst_Y,
                                                    dst_C,
                                                    ui4_width,
                                                    ui4_height,
                                                    ui4_Y_buf_size,
                                                    ui4_C_buf_size,
                                                    ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORWIPE_VERTICAL_CLOSE:
            i4_ret = slideshow_barndoorwipe_vert_close(src_Y,
                                                    src_C, 
                                                    cur_Y,
                                                    cur_C,
                                                    dst_Y,
                                                    dst_C,
                                                    ui4_width,
                                                    ui4_height,
                                                    ui4_Y_buf_size,
                                                    ui4_C_buf_size,
                                                    ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_TOPCENTER:
            i4_ret = slideshow_boxwipe_tc(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_RIGHTCENTER:
            i4_ret = slideshow_boxwipe_rc(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMCENTER:
            i4_ret = slideshow_boxwipe_bc(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXWIPE_LEFTCENTER:
            i4_ret = slideshow_boxwipe_lc(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_IRISWIPE_CENTER:
            i4_ret = slideshow_iriswipe_center(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_IRISWIPE_OUTER:
            i4_ret = slideshow_iriswipe_outer(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARWIPE_HORIZONTAL_FROM_TOP:
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARWIPE_HORIZONTAL_FROM_BOTTOM:
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARWIPE_VERTICAL_FROM_LEFT:
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARWIPE_VERTICAL_FROM_RIGHT:
            break;
//#endif
//#if (ENABLE_PHOTO_TRANS_EFFECT_SLIDE)
        case IMG_SLIDE_SHOW_EFFECT_BARSLIDE_LEFT2RIGHT:
            i4_ret = slideshow_barslide_lr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARSLIDE_RIGHT2LEFT:
            i4_ret = slideshow_barslide_rl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARSLIDE_TOP2BOTTOM:
            i4_ret = slideshow_barslide_tb(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARSLIDE_BOTTOM2TOP:
            i4_ret = slideshow_barslide_bt(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_TOPLEFT:
            i4_ret = slideshow_boxslide_tl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_TOPRIGHT:
            i4_ret = slideshow_boxslide_tr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_BOTTOMLEFT:
            i4_ret = slideshow_boxslide_bl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BOXSLIDE_BOTTOMRIGHT:
            i4_ret = slideshow_boxslide_br(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_HORIZONTAL_OPEN:
            i4_ret = slideshow_barndoorslide_hor_open(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_HORIZONTAL_CLOSE:
            i4_ret = slideshow_barndoorslide_hor_close(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_VERTICAL_OPEN:
            i4_ret = slideshow_barndoorslide_vert_open(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_VERTICAL_CLOSE:
            i4_ret = slideshow_barndoorslide_vert_close(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARSLIDE_HORIZONTAL_FROM_TOP:
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARSLIDE_HORIZONTAL_FROM_BOTTOM:
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARSLIDE_VERTICAL_FROM_LEFT:
            break;
        case IMG_SLIDE_SHOW_EFFECT_4BARSLIDE_VERTICAL_FROM_RIGHT:
            break;
//#endif
//#if (ENABLE_PHOTO_TRANS_EFFECT_FADE_IN)
        case IMG_SLIDE_SHOW_EFFECT_FADE_IN:
            if (pf_fade_nfy == NULL)
            {
                //x_dbg_stmt("[MW slideshow fade] pf_fade_nfy not init.\n");
            }
            i4_ret = slideshow_fade_in( src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_SLIDE_LEFT2RIGHT:
            i4_ret = slideshow_slide_lr(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
        case IMG_SLIDE_SHOW_EFFECT_SLIDE_RIGHT2LEFT:
            i4_ret = slideshow_slide_rl(src_Y,
                                        src_C, 
                                        cur_Y,
                                        cur_C,
                                        dst_Y,
                                        dst_C,
                                        ui4_width,
                                        ui4_height,
                                        ui4_Y_buf_size,
                                        ui4_C_buf_size,
                                        ui4_total_time);
            break;
//#endif

        //case IMG_SLIDE_SHOW_EFFECT_CLOSE:
            //i4_ret = x_img_stop_slideshow();
            //break;
        default:
            break;
    }
    return i4_ret;
}


INT32   x_img_stop_slideshow(VOID)
{
    INT32   i4_ret;
    //SLIDESHOW_MSG_T   t_msg;
    
    b_stop = TRUE;
    if(h_binary_sema!=NULL_HANDLE)
    {    
        i4_ret = x_sema_lock(h_binary_sema, X_SEMA_OPTION_WAIT);
        if (i4_ret != OSR_OK)
        {
            
            return SLIDESHOW_SEMA_ERROR;
        }
        x_sema_unlock(h_binary_sema);
        x_sema_delete(h_binary_sema);
        h_binary_sema = NULL_HANDLE;
    }
    return SLIDESHOW_OK;
}

