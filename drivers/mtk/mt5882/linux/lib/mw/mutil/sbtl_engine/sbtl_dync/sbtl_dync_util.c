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
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_util.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_reg.h"

#include "mutil/mm_util_input.h"



#define SBTL_VERIFY_MIN_SIZE (1024)

typedef struct
{
    CHAR*          ps_ext;
    MM_SBTL_TYPE_T e_sbtl_type;
}MM_SBTL_FAVOR_TYPE_T;

MM_SBTL_FAVOR_TYPE_T t_favor_ext[] = 
{
    {"xml",   MM_SBTL_TYPE_XML},
    {"ipcc",  MM_SBTL_TYPE_XML_IPCC},
    {"m4t",   MM_SBTL_TYPE_XML_IPCC},
    {"smi",   MM_SBTL_TYPE_TEXT_SAMI},
    {"sami",  MM_SBTL_TYPE_TEXT_SAMI},
    {"srt",   MM_SBTL_TYPE_TEXT_SRT}
};

#define IS_ALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_')

//OK
UINT8* get_words(UINT8* pui1_s, UINT32 ui4_len, UINT32* pui4_w_len)
{
    UINT32 ui4_i = 0;
    BOOL   b_s   = FALSE;
    
    if (pui1_s == NULL ||
        ui4_len == 0 ||
        pui4_w_len == NULL)
    {
        return NULL;
    }
    while (ui4_i < ui4_len)
    {
        if (b_s)
        {
            if (IS_ALPHA(pui1_s[ui4_i]) || pui1_s[ui4_i] == ':')
            {
                (*pui4_w_len)++;
            }
            else
            {
                return pui1_s + (ui4_i - *pui4_w_len);
            }
        }
        else
        {
            if (IS_ALPHA(pui1_s[ui4_i]))
            {
                b_s = TRUE;
                *pui4_w_len = 1;
            }
        }
        ui4_i++;
    }
    return NULL;
}

//OK
UINT32 get_png_start(UINT8* pui1_buf, UINT32 ui4_len)
{
    UINT32 ui4_i;
    for (ui4_i = 0; ui4_i < ui4_len; ui4_i++)
    {
        if (pui1_buf[ui4_i] == 0x89)
        {
            break;
        }
    }
    return ui4_i;
}

//OK

INT32 parse_png(MM_SBTL_IMG_INFO_T* pt_img, UINT8* pui1_buf, UINT32 ui4_len)
{
    const UINT8  aui1_stamp[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    UINT8*       pui1_c = pui1_buf;
    UINT32       ui4_pos = 0;
    UINT32       ui4_ck_type;
    UINT32       ui4_ck_len;
    UINT32       ui4_width = 0;
    UINT32       ui4_height = 0;

    if (pui1_buf == NULL || ui4_len < 100)
    {
        return MMSBTLR_NOT_ALLOW;
    }
  
    if (x_memcmp(pui1_buf, aui1_stamp, sizeof(aui1_stamp)-1) != 0)
    {
        return MMSBTLR_NOT_FOUND;
    }
  
    ui4_pos = sizeof(aui1_stamp);
  
    while (ui4_pos < ui4_len)
    {
        pui1_c = pui1_buf + ui4_pos;
        LOADB_DWRD(pui1_c, ui4_ck_len);
        LOADB_DWRD(pui1_c + 4, ui4_ck_type);

        if (ui4_ck_type == dwBigFourCC('I', 'H', 'D', 'R'))
        {
            LOADB_DWRD(pui1_c + 8,  ui4_width);
            LOADB_DWRD(pui1_c + 12, ui4_height);
            
        }
        
        ui4_pos += ui4_ck_len;
        ui4_pos += 12;
        
        if (ui4_pos > ui4_len)
        {
            break;
        }
        if (ui4_ck_type == dwBigFourCC('I', 'E', 'N', 'D'))
        {
            pt_img->pui1_img_data = pui1_buf;
            pt_img->ui4_img_size = ui4_pos;
            pt_img->ui4_height = ui4_height;
            pt_img->ui4_width = ui4_width;
            pt_img->e_img_type = MM_SBTL_IMG_TYPE_PNG;
            return MMSBTLR_OK;
        }
    }
    
    return MMSBTLR_NOT_FOUND;
}


//OK
INT32  sbtl_get_timecode(CHAR* ps_str,UINT32 *pui4_time)
{
  /*00:00:00:00*/
    UINT32 ui4_time = 0;
    UINT16 ui2_m    = 0;
    UINT16 ui2_cnt  = 0;
    UINT16 ui2_d    = 1;
    
    if (ps_str == NULL || pui4_time == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    while(*ps_str)
    {
        if (*ps_str >= '0' && *ps_str <= '9')
        {
            ui2_m = ui2_m*10 + (*ps_str -'0');
            if (ui2_cnt == 3)
            {
                ui2_d *= 10;
                if (ui2_d >= 1000)break;
            }
        }
        if (*ps_str == ':' ||*ps_str == '.' )
        {
            ui4_time=ui4_time*60+ui2_m;
            ui2_m = 0;
            ui2_d = 1;
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
        ui2_m = ui2_m*(1000/ui2_d);
        ui4_time = ui4_time*1000+ui2_m;/*to ms*/
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
    
    *pui4_time =  ui4_time;
    return MMSBTLR_OK;
}

//OK
BOOL valid_string(UINT8* ps_str)
{
    if (ps_str)
    {
        while (*ps_str)
        {
            if (x_strchr(" \t\r\n", *ps_str) == NULL)
            {
                return TRUE;
            }
            ps_str++;
        }
    }
    return FALSE;
}


//OK
static INT32 _mm_sbtl_get_feeder_by_path(
    CHAR*           ps_path, 
    MEDIA_FEEDER_T* pt_feeder)
{
    BOOL     b_net_file = FALSE;
    HANDLE_T h_file_handle = NULL_HANDLE;
    INT32    i4_ret = MMSBTLR_OK;
    
    if (ps_path == NULL || pt_feeder == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    x_memset(pt_feeder, 0, sizeof(MEDIA_FEEDER_T));
   
    if ((x_strncasecmp(ps_path, "http:", 5) == 0) ||
        (x_strncasecmp(ps_path, "https:", 6) == 0))
    {
        b_net_file = TRUE;
    } 
    else if (x_strncasecmp(ps_path, "file://", 7) == 0)
    {
        ps_path += 7;
    }
    
    i4_ret = sbtl_util_open_file(ps_path, b_net_file, &h_file_handle);
    DBG_ERROR(("open file:%s ret:%d\n", ps_path, i4_ret));
    if (h_file_handle == NULL_HANDLE)
    {
        DBG_ERROR(("Invalid file path:%s\n", ps_path));
        return MMSBTLR_NOT_FOUND;
    }
    
    pt_feeder->h_feeder = h_file_handle;
 
    if (b_net_file)
    {
#ifdef INET_SUPPORT
        pt_feeder->t_feeder_be_fct_tbl.pf_copybytes = sbtl_util_http_fcopybytes_fct;
        pt_feeder->t_feeder_be_fct_tbl.pf_input4 = NULL;
        pt_feeder->t_feeder_be_fct_tbl.pf_input3 = NULL;
        pt_feeder->t_feeder_be_fct_tbl.pf_input2 = NULL;
        pt_feeder->t_feeder_be_fct_tbl.pf_input1 = sbtl_util_http_finput1bytes_fct;
        pt_feeder->t_feeder_be_fct_tbl.pf_set_pos = sbtl_util_http_fsetpos_fct;
        pt_feeder->t_feeder_be_fct_tbl.pf_get_pos = sbtl_util_http_fgetpos_fct;
#else
        i4_ret = MMSBTLR_NOT_SUPPORT;
#endif
    }
    else
    {
        pt_feeder->t_feeder_be_fct_tbl.pf_copybytes = sbtl_util_fcopybytes_fct;
        pt_feeder->t_feeder_be_fct_tbl.pf_input4 = NULL;
        pt_feeder->t_feeder_be_fct_tbl.pf_input3 = NULL;
        pt_feeder->t_feeder_be_fct_tbl.pf_input2 = NULL;
        pt_feeder->t_feeder_be_fct_tbl.pf_input1 = sbtl_util_finput1bytes_fct;
        pt_feeder->t_feeder_be_fct_tbl.pf_set_pos = sbtl_util_fsetpos_fct;
        pt_feeder->t_feeder_be_fct_tbl.pf_get_pos = sbtl_util_fgetpos_fct;
    }
    return MMSBTLR_OK;
}


//OK
static MM_SBTL_TYPE_T _get_sbtl_type_by_ext(CHAR*  ps_ext)
{
    UINT16 ui2_i = sizeof(t_favor_ext)/sizeof(MM_SBTL_FAVOR_TYPE_T);
    while (ui2_i > 0)
    {
        ui2_i--;
        if (x_strcasecmp(ps_ext, t_favor_ext[ui2_i].ps_ext) == 0)
        {
            return t_favor_ext[ui2_i].e_sbtl_type;
        }
    }
    return MM_SBTL_TYPE_NONE;
}

//OK
BOOL check_file_valid(MM_SBTL_LOAD_FILE_INFO_T* pt_file, UINT32 ui4_cur_pts)
{
    MM_SBTL_TYPE_T     e_sbtl_type = MM_SBTL_TYPE_NONE;
    UINT8*             pui1_buf  = NULL;
    UINT32             ui4_len = 0;
    CHAR*              ps_ext = NULL;
    MEDIA_FEEDER_T*    pt_feeder;
    setpos_fct         pf_set_pos;
    getpos_fct         pf_get_pos;
    copybytes_fct      pf_copybytes = NULL;
    UINT64             ui8_pos_bgn = 0;
    UINT64             ui8_pos_end = 0;
    INT32              i4_ret      = MMSBTLR_OK;

    if ((pt_file == NULL) ||
        (pt_file->ps_file_path == NULL && pt_file->pui1_file_buff == NULL))
    {
        return FALSE;
    }

    do
    {
        if (pt_file->ui4_end_pts &&
            pt_file->ui4_end_pts < ui4_cur_pts)
        {
            break;
        }
        if (pt_file->pui1_file_buff)
        {
            pui1_buf = pt_file->pui1_file_buff;
            ui4_len  = pt_file->ui4_file_size;
        }
        else
        {
            UINT32 ui4_real_read = 0;
            ps_ext = x_strrchr(pt_file->ps_file_path, '.');
            if (ps_ext)
            {
                pt_file->e_sbtl_type = _get_sbtl_type_by_ext(ps_ext + 1);
            }
            i4_ret = _mm_sbtl_get_feeder_by_path(pt_file->ps_file_path,
                                                 &pt_file->t_feeder);
            if (i4_ret != MMSBTLR_OK)
            {
                break;
            }
            
            pt_feeder = &pt_file->t_feeder;
            pf_get_pos = pt_feeder->t_feeder_be_fct_tbl.pf_get_pos;
            pf_set_pos = pt_feeder->t_feeder_be_fct_tbl.pf_set_pos;
            pf_copybytes = pt_feeder->t_feeder_be_fct_tbl.pf_copybytes;
            if (pf_get_pos == NULL || 
                pf_set_pos == NULL ||
                pf_copybytes == NULL)
            {
                break;
            }
            
            pf_set_pos(pt_feeder, 0, MEDIA_SEEK_END);
            pf_get_pos(pt_feeder, &ui8_pos_end);
            pf_set_pos(pt_feeder, 0, MEDIA_SEEK_BGN);
            pf_get_pos(pt_feeder, &ui8_pos_bgn);
            pt_file->ui4_file_size = (UINT32)(ui8_pos_end - ui8_pos_bgn);
            if (pt_file->ui4_file_size  == 0)
            {
                break;
            }
            if (pt_file->ui4_file_size <= SBTL_VERIFY_MIN_SIZE)
            {
                ui4_len = pt_file->ui4_file_size;
            }
            else
            {
                ui4_len = 1024;
            }
            pui1_buf = (UINT8*)x_mem_alloc(ui4_len);
            if (pui1_buf == NULL)
            {
                break;
            }
            i4_ret = pf_copybytes(pt_feeder, 
                                  pui1_buf, 
                                  ui4_len, 
                                  ui4_len,
                                 &ui4_real_read);
            if (ui4_real_read != ui4_len)
            {
                DBG_ERROR(("Read size:%d ret:%d\n", ui4_len, ui4_real_read));
                break;
            }
            if (pt_file->ui4_file_size <= SBTL_VERIFY_MIN_SIZE)
            {
                pt_file->pui1_file_buff = pui1_buf;
            }
        }
        
        if (pui1_buf == NULL || ui4_len == 0)
        {
            break;
        }
        pt_file->pt_reg_func = sbtl_dync_mem_match_type(&e_sbtl_type, pui1_buf, ui4_len);
        if (pt_file->pt_reg_func == NULL)
        {
            break;
        }
        if (pt_file->pui1_file_buff != pui1_buf)
        {
            x_mem_free(pui1_buf);
        }
        pt_file->e_sbtl_type = e_sbtl_type;
        return TRUE;
    } while (0);
    //clear up func
    if (pui1_buf)
    {
        x_mem_free(pui1_buf);
    }
    
    if (pt_file->t_feeder.h_feeder != NULL_HANDLE)
    {
        BOOL b_net = (pt_file->t_feeder.t_feeder_be_fct_tbl.pf_copybytes != sbtl_util_fcopybytes_fct);
        sbtl_util_close_file(pt_file->t_feeder.h_feeder, b_net);
        pt_file->t_feeder.h_feeder = NULL_HANDLE;
    }
    return FALSE;
    
}

//OK
MM_SBTL_FILE_STACK_T* alloc_one_file(
    MM_SBTL_LOAD_FILE_INFO_T* pt_file)
{
    MM_SBTL_FILE_STACK_T* pt_cur = NULL;
    INT32                 i4_ret = MMSBTLR_OK;

    if (pt_file == NULL)
    {
        return NULL;
    }
    do
    {
        pt_cur = x_mem_alloc(sizeof(MM_SBTL_FILE_STACK_T));
        if (pt_cur == NULL)
        {
            return NULL;
        }
        x_memset(pt_cur, 0, sizeof(MM_SBTL_FILE_STACK_T));
        
        if (pt_file->pui1_file_buff)
        {
            if (pt_file->ui4_file_size == 0)
            {
                break;
            }
            pt_cur->t_info.pui1_file_buff = pt_file->pui1_file_buff;
            pt_cur->t_info.ui4_start_pts = pt_file->ui4_start_pts;
            pt_cur->t_info.ui4_end_pts = pt_file->ui4_end_pts;
        }
        else if (pt_file->ps_file_path)
        {
            CHAR* ps_path = x_strdup(pt_file->ps_file_path);
            if (ps_path == NULL)
            {
                break;
            }
            pt_cur->t_info.ui4_start_pts = pt_file->ui4_start_pts;
            pt_cur->t_info.ui4_end_pts   = pt_file->ui4_end_pts;
            pt_cur->t_info.ps_file_path  = ps_path;
            pt_cur->t_info.t_feeder      = pt_file->t_feeder;
        }
        pt_cur->t_info.ui4_file_size = pt_file->ui4_file_size;
        pt_cur->t_info.e_sbtl_type = pt_file->e_sbtl_type;
        pt_cur->t_info.pt_reg_func = pt_file->pt_reg_func;
        i4_ret = x_sema_create(&(pt_cur->t_info.h_sema), 
                           X_SEMA_TYPE_BINARY, 
                           X_SEMA_STATE_UNLOCK);
        if (i4_ret != OSR_OK ||
            pt_cur->t_info.h_sema == NULL_HANDLE)
        {
            break;
        }
        return pt_cur;
    
    }while (0);
    if (pt_cur)
    {
        if (pt_cur->t_info.ps_file_path)
        {
            x_mem_free(pt_cur->t_info.ps_file_path);
            pt_cur->t_info.ps_file_path = NULL;
        }
        x_mem_free(pt_cur);
    }
    return NULL;
}

//OK
INT32 free_one_file(MM_SBTL_LOAD_FILE_INFO_T* pt_file)
{
    UINT32 ui4_i = 0;
    if (pt_file)
    {
        //waiting untill parse done
        pt_file->b_force_exit = TRUE;
        if (pt_file->h_sema != NULL_HANDLE)
        {
            x_sema_lock(pt_file->h_sema, X_SEMA_OPTION_WAIT);
            x_sema_unlock(pt_file->h_sema);
        }
        
        //free index table
        if (pt_file->pt_reg_func)
        {
            if (pt_file->pt_reg_func->pf_show)
            {
                pt_file->pt_reg_func->pf_show(pt_file, FALSE);
            }
            if (pt_file->pv_samle_index)
            {
                for (ui4_i = 0; ui4_i < pt_file->ui4_sample_num; ui4_i++)
                {
                    if (pt_file->pt_reg_func->pf_free)
                    {
                        pt_file->pt_reg_func->pf_free(pt_file, ui4_i);//add free index
                    }
                    pt_file->pv_samle_index[ui4_i] = NULL;
                }
                x_mem_free(pt_file->pv_samle_index);
                pt_file->pv_samle_index = NULL;
            }
            if (pt_file->pt_reg_func->pf_deinit)
            {
                pt_file->pt_reg_func->pf_deinit(pt_file);
            }
        }
        
        //free file handle mem
        if (pt_file->t_feeder.h_feeder != NULL_HANDLE)
        {
            BOOL b_net = (pt_file->t_feeder.t_feeder_be_fct_tbl.pf_copybytes != sbtl_util_fcopybytes_fct);
            sbtl_util_close_file(pt_file->t_feeder.h_feeder, b_net);
            pt_file->t_feeder.h_feeder = NULL_HANDLE;
        }
        if (pt_file->pui1_file_buff)
        {
            x_mem_free(pt_file->pui1_file_buff);
            pt_file->pui1_file_buff = NULL;
        }
        if (pt_file->ps_file_path)
        {
            x_mem_free(pt_file->ps_file_path);
            pt_file->ps_file_path = NULL;
        }
        if (pt_file->h_sema != NULL_HANDLE)
        {
            x_sema_delete(pt_file->h_sema);
            pt_file->h_sema = NULL_HANDLE;
        }
    }
    return MMSBTLR_OK;
}
/*
f: encode data
t: encoded data
l: encode data len
return OK/FAIL
*/

INT32 sbtl_base64_encode(CHAR* f, CHAR* t, INT32 l)
{
    INT32   i = 0; 
    INT32   e = l/3;
    CHAR    enc[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    CHAR*   p = f;
    INT32   m = 0;
    if (f == NULL || t == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    for (i = 0; i < e; i++)
    {
        t[m++] = enc[(p[0] >> 2)];
        t[m++] = enc[((p[0] & 0x3) << 4) + (p[1] >> 4)];
        t[m++] = enc[((p[1] & 0xF) << 2) + (p[2] >> 6)];
        t[m++] = enc[(p[2] & 0x3F)];
        p += 3;
    }

    if (p < f+l)
    {
        t[m++] = enc[(p[0] >> 2)];
        if (p + 1 < f + l)
        {
            t[m++] = enc[((p[0] & 0x3) << 4) + (p[1] >> 4)];
            t[m++] = enc[((p[1] & 0xF) << 2)];
        }
        else
        {
            t[m++] = enc[((p[0] & 0x3) << 4)];
            t[m++] = '=';
        }
        t[m++] = '=';
    }
    return MMSBTLR_OK;
}


/*
f: to be decoded data
t: decoded data
l:  to be decoded data len
return Fail or decoded data length
*/

INT32 sbtl_base64_decode(CHAR* f, CHAR* t, INT32 l)
{
    UINT8 *p = (UINT8*)f;
    const CHAR  dec[256] = {
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
      52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, 0, -1, -1,
      -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
      -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
      41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
      0,  0,  0};
    INT32  i;
    INT32  m = 0;
    if (f == NULL || t == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    for (i = 0; i < l/4; i++)
    {
        CHAR c1, c2, c3, c4;
        c1 = dec[*(p++)];
        c2 = dec[*(p++)];
        c3 = dec[*(p++)];
        c4 = dec[*(p++)];
        t[m++] = (c1 << 2) + (c2 >> 4);
        t[m++] = ((c2 & 0xF) << 4) + (c3 >> 2);
        t[m++] = ((c3 & 0x3) << 6) + c4;
    }
    p = (UINT8*)f+l-1;
    while (*p == '=')
    {
        p--;
        m--;
    }
    return m;
}

/*
remove letters which are not base64 compatiable
*/

VOID sbtl_base64_chomp(CHAR* p)
{
    CHAR  *s = p;
    INT32  n = 0;
    while (*p)
    {
        if ((*p >= 'A' && *p <= 'Z') ||
            (*p >= 'a' && *p <= 'z') ||
            (*p >= '0' && *p <= '9') ||
            (*p == '+' || *p == '/' || *p == '=')
          )
        {
        }
        else
        {
            n++;
            p++;
            continue;
        }
        if (n > 0)
        {
            *s = *p;
        }
        s++;
        p++;
    }
    if (n > 0)
    *s = 0;
}
#include <sys/time.h>
UINT32 sbtl_cur_time(VOID)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec & 0xFFFF)*1000 + t.tv_usec/1000;
}


#endif /*__KERNEL__*/


