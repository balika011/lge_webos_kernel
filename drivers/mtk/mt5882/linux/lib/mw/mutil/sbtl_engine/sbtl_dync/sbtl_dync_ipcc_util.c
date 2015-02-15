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
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_util.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_ipcc_util.h"
#include "mutil/sbtl_engine/sbtl_xml_parser.h"
#include "mutil/sbtl_engine/sbtl_xml/sbtl_xml_page_inst_mngr.h"
#include "u_xml.h"
#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/mm_util_input.h"
#include "u_img.h"
#include "x_img.h"


/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/
 /* big endian */


static  SBTL_XML_HASH_INFO_T t_elm_hash[] =
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
    {"body",       (INT32)SBTL_XML_ELM_BODY},    
    {"div",        (INT32)SBTL_XML_ELM_DIV},
    {"p",          (INT32)SBTL_XML_ELM_PARA},
    {"span",       (INT32)SBTL_XML_ELM_SPAN},    
    {"br",         (INT32)SBTL_XML_ELM_BR},
    {"information",(INT32)SBTL_XML_ELM_INFO},
    {"image",      (INT32)SBTL_XML_ELM_IMAGE}
};


static  SBTL_XML_HASH_INFO_T t_attr_hash[] =
{       
    {"id",              (INT32)SBTL_XML_IPCC_ATTR_ID},
    {"region",          (INT32)SBTL_XML_IPCC_ATTR_REGION},    
    {"style",           (INT32)SBTL_XML_IPCC_ATTR_STYLE},
    
    {"begin",           (INT32)SBTL_XML_IPCC_ATTR_BEGIN},
    {"end",             (INT32)SBTL_XML_IPCC_ATTR_END}, 
    {"dur",             (INT32)SBTL_XML_IPCC_ATTR_DUR}, 
    {"fontfamily",      (INT32)SBTL_XML_IPCC_ATTR_FONTFAMILY},
    {"fontsize",        (INT32)SBTL_XML_IPCC_ATTR_FONTSIZE},
    {"textalign",       (INT32)SBTL_XML_IPCC_ATTR_TEXTALIGN},
    {"origin",          (INT32)SBTL_XML_IPCC_ATTR_ORIGIN},
    {"fontweight",      (INT32)SBTL_XML_IPCC_ATTR_FONT_WEIGHT},
    {"visibility",      (INT32)SBTL_XML_IPCC_ATTR_VISIBILY},
    {"backgroundcolor", (INT32)SBTL_XML_IPCC_ATTR_BGCOLOR},
    {"encoding",        (INT32)SBTL_XML_IPCC_ATTR_ECODING},
    {"color",           (INT32)SBTL_XML_IPCC_ATTR_COLOR},
    {"fontstyle",       (INT32)SBTL_XML_IPCC_ATTR_FONT_STYLE},  
    {"extent",          (INT32)SBTL_XML_IPCC_ATTR_FONT_EXTENT},
    {"cellresolution",  (INT32)SBTL_XML_IPCC_ATTR_CELL_RESOLUTION},    
    {"textdecoration",  (INT32)SBTL_XML_IPCC_ATTR_TEXT_DECORATION},
    
    {"opacity",         (INT32)SBTL_XML_IPCC_ATTR_WINDOW_OPACITY},    
    {"textoutline",     (INT32)SBTL_XML_IPCC_ATTR_TEXT_OUTLINE},
    {"warpoption",      (INT32)SBTL_XML_IPCC_ATTR_WRAP_OPTION},    
    {"space",           (INT32)SBTL_XML_IPCC_ATTR_WHITESPACE},
    {"lineheight",      (INT32)SBTL_XML_IPCC_ATTR_LINEHEIGHT},
    {"role",            (INT32)SBTL_XML_IPCC_ATTR_ROLE},
    {"mode",            (INT32)SBTL_XML_IPCC_ATTR_MODE},
    {"fakespan",        (INT32)SBTL_XML_IPCC_ATTR_FAKESPAN},
    {"backgroundimage", (INT32)SBTL_XML_IPCC_ATTR_BGIMG},
    {"forcedisplaymode",(INT32)SBTL_XML_IPCC_ATTR_DISP_MODE}, 
    {"imagetype",       (INT32)SBTL_XML_IPCC_ATTR_IMAGE_TYPE}
};



typedef struct
{
    BOOL                b_found;
    BOOL                b_white_space;
    SBTL_IPCC_ATTR_T    e_attr;
    CHAR*               ps_value;
    HANDLE_T            h_style;
    HANDLE_T            h_region;
}SBTL_XML_ATTR_SEARCH_T;


SBTL_XML_PARSER_INFO_T t_parse_info = {NULL_HANDLE, NULL_HANDLE};

static SLIST_T(_SBTL_TTML_ATTR_T)  t_attr_cur = {NULL};//need to be change
static BOOL              b_is_whitspace_valid = FALSE;//need to be change

#define _CHECK_OVER_RET(off, siz) if (off >= siz) return MMSBTLR_OK;


static INT32 parse_all_image(
    MM_SBTL_M4T_IDX_INFO_T* pt_idx,
    UINT8*                  pui1_buf, 
    UINT32                  ui4_len);

//ok
static INT32 sbtl_ipcc_init_parse_info(VOID)
{
    if (t_parse_info.h_ele_hash == NULL_HANDLE)
    {
         x_sbtl_xml_create_hash(t_elm_hash,
                                sizeof(t_elm_hash)/sizeof(SBTL_XML_HASH_INFO_T),
                                &t_parse_info.h_ele_hash);
    }
    if (t_parse_info.h_attr_hash == NULL_HANDLE)
    {
        x_sbtl_xml_create_hash(t_attr_hash,
                               sizeof(t_attr_hash)/sizeof(SBTL_XML_HASH_INFO_T), 
                               &t_parse_info.h_attr_hash);
    }
    if (t_parse_info.h_ele_hash == NULL_HANDLE ||
        t_parse_info.h_attr_hash == NULL_HANDLE)
    {
         return MMSBTLR_NOT_INIT;
    } 
    return MMSBTLR_OK;
}


//ok
static CHAR* _sbtl_ipcc_get_attr_name(SBTL_IPCC_ATTR_T e_attr)
{
    UINT32 ui4_i;
    for (ui4_i = 0; ui4_i < sizeof(t_attr_hash)/sizeof(SBTL_XML_HASH_INFO_T); ui4_i++)
    {
        if (t_attr_hash[ui4_i].i4_value == (INT32)e_attr)
        {
            return t_attr_hash[ui4_i].ps_key;
        }
    }
    return NULL;
}

//OK
BOOL sbtl_m4t_matched(
   UINT8*                    pui1_buf,
   UINT32                    ui4_len)
{

    UINT32          ui4_box_size = 0;
    UINT32          ui4_box_type = 0;

    BOOL            b_ok         = FALSE;
    if (pui1_buf && ui4_len)
    {
        UINT32 ui4_pos = 0;
        while (ui4_pos+8 < ui4_len)
        {
            LOADB_DWRD(pui1_buf+ui4_pos,   ui4_box_size);
            LOADB_DWRD(pui1_buf+ui4_pos+4, ui4_box_type);
            DBG_INFO(("type:%x\n", *(UINT32*)(pui1_buf+ui4_pos+4)));
            switch (ui4_box_type)
            {
            case MP4_BOX_MOOV:
            case MP4_BOX_MVEX:
            case MP4_BOX_TRAF:   
            case MP4_BOX_MVHD:
            case MP4_BOX_MOOF:
            case MP4_BOX_MFHD:          
            case MP4_BOX_TRUN:
            case MP4_BOX_FTYP: 
            case MP4_BOX_TRAK:
            case MP4_BOX_MEHD:
            case MP4_BOX_TREX:
            case MP4_BOX_MDAT:
            case MP4_BOX_MFRA:
            case MP4_BOX_TFHD:
                b_ok = TRUE;
                break;
            default:
                    break;
             }
            if (b_ok)
            {
                return TRUE;
            } 
            if (ui4_box_size < 8)
            {
                ui4_box_size = 8;
            }
            ui4_pos += ui4_box_size;
        }
    }
    return FALSE;
}

//OK

BOOL sbtl_xml_matched(
   UINT8*                    pui1_buf,
   UINT32                    ui4_len)
{
    if (pui1_buf && ui4_len > 40)
    {
        UINT8* pui1_cur = pui1_buf;
        pui1_buf += 25;
        while (pui1_cur < pui1_buf)
        {
            if (x_memcmp(pui1_cur, "<?xml", 5) == 0)
            {
                return TRUE;
            }
            pui1_cur++;
        }
    }
    return FALSE;
}

//OK
 VOID sbtl_xml_dump_attrs(SBTL_TTML_ATTR_T* pt_attr)
{
    CHAR* ps_name = NULL;
    if (pt_attr)
    {
         if (pt_attr->e_attr == SBTL_XML_IPCC_ATTR_REGION_BGCOLOR)
         {
            ps_name = "regionbg";
         }
         else
         {
             ps_name = _sbtl_ipcc_get_attr_name(pt_attr->e_attr);
             if (ps_name == NULL)
             {
                DBG_INFO(("Unknown attr %d\n",pt_attr->e_attr)); 
                return;
             }
         }
         switch(pt_attr->e_attr)
         {
             case SBTL_XML_IPCC_ATTR_BEGIN:
             case SBTL_XML_IPCC_ATTR_END:                
             case SBTL_XML_IPCC_ATTR_DUR: 
             case SBTL_XML_IPCC_ATTR_FONTFAMILY: 
             case SBTL_XML_IPCC_ATTR_FONT_STYLE: 
             case SBTL_XML_IPCC_ATTR_VISIBILY:  
             case SBTL_XML_IPCC_ATTR_FONT_WEIGHT:
             case SBTL_XML_IPCC_ATTR_TEXT_DECORATION: 
             case SBTL_XML_IPCC_ATTR_TEXTALIGN:
             case SBTL_XML_IPCC_ATTR_WHITESPACE:
                 DBG_INFO(("%s[%d]=%d  \n",ps_name,pt_attr->e_attr,(INT32)pt_attr->u.e_pts_tm)); 
                 break;
             case SBTL_XML_IPCC_ATTR_ID:
             case SBTL_XML_IPCC_ATTR_REGION:
             case SBTL_XML_IPCC_ATTR_STYLE:
             case SBTL_XML_IPCC_ATTR_COLOR:
             case SBTL_XML_IPCC_ATTR_BGCOLOR:
             case SBTL_XML_IPCC_ATTR_REGION_BGCOLOR:
             case SBTL_XML_IPCC_ATTR_ROLE:
                 DBG_INFO(("%s[%d]=%s  \n",ps_name,pt_attr->e_attr,(INT32)pt_attr->u.ps_value)); 
                 break;
            default:
                break;
         }
    }
    
}

static INT32 _sbtl_m4t_add_sample(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT32                    ui4_start_pts,
    UINT32                    ui4_dur,
    UINT32                    ui4_offset,
    UINT32                    ui4_size)
{
    MM_SBTL_M4T_IDX_INFO_T* pt_m4t_sample = NULL;
    
    if (pt_info == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_m4t_sample = (MM_SBTL_M4T_IDX_INFO_T*)x_mem_alloc(sizeof(MM_SBTL_M4T_IDX_INFO_T));
    if (pt_m4t_sample == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    x_memset(pt_m4t_sample, 0, sizeof(MM_SBTL_M4T_IDX_INFO_T));
    pt_m4t_sample->t_base.ui4_start_pts = ui4_start_pts;
    pt_m4t_sample->t_base.ui4_offset  = ui4_offset;
    pt_m4t_sample->t_base.ui4_size    = ui4_size;
    
    if (ui4_dur > 0)
    {
        pt_m4t_sample->t_base.ui4_end_pts = ui4_start_pts + ui4_dur;
    }
    
    pt_m4t_sample->b_is_m4t  = TRUE;

    if (pt_info->ui4_sample_num % 100 == 0)
    {
        pt_info->pv_samle_index
            = (VOID**)x_mem_realloc( pt_info->pv_samle_index,
                            (pt_info->ui4_sample_num+100) * sizeof(VOID*));
        
    }
    if (pt_info->pv_samle_index == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    
    DBG_INFO(("add sample is:%d startpts:%d dur:%d offset:%d size:%d\n",
               pt_info->ui4_sample_num, pt_m4t_sample->t_base.ui4_start_pts, ui4_dur,
               pt_m4t_sample->t_base.ui4_offset, pt_m4t_sample->t_base.ui4_size));
      
    pt_info->pv_samle_index[pt_info->ui4_sample_num] = pt_m4t_sample;
    pt_info->ui4_sample_num++;

    return MMSBTLR_OK;
}


//to do
static INT32 _sbtl_m4t_parse_idx(
        MM_SBTL_LOAD_FILE_INFO_T* pt_info,
        UINT8*                    pui1_cur,
        UINT32                    ui4_len,
        UINT32                    ui4_moof_offset,
        UINT32*                   pui4_total_dur,
        UINT32                    ui4_time_scale)
{

    UINT32    ui4_sample_cnt      = 0;
    UINT32    ui4_base_offset     = ui4_moof_offset;
    UINT32    ui4_flag = 0;
    UINT32    ui4_i    = 0;
    UINT8*    pui1_end = NULL;
    INT32     i4_ret   = MMSBTLR_OK;
    
    if (pui1_cur == NULL || ui4_len < 12 || pui4_total_dur == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pui1_end = pui1_cur + ui4_len;
    
    LOADB_DWRD(pui1_cur, ui4_flag);
    LOADB_DWRD(pui1_cur + 4, ui4_sample_cnt);
    pui1_cur += 8;
    
    if (pui1_cur > pui1_end) return MMSBTLR_INTERNAL_ERROR;
    
    ui4_flag  &= 0xFFFFFF;
    
    if(ui4_flag & 0x01)
    {
        UINT32 ui4_data_ofst = 0;
        LOADB_DWRD(pui1_cur, ui4_data_ofst); 
        pui1_cur += 4;
        ui4_base_offset += ui4_data_ofst;  
    }
    
    if(ui4_flag & 0x04)
    {
        pui1_cur += 4;
    } 
    
    if (pui1_cur > pui1_end) return MMSBTLR_INTERNAL_ERROR;
        
    // read trun box data to get duration,size,to cal pts , ofst
    DBG_INFO(("try add sample count:%d\n", ui4_sample_cnt));
    for (ui4_i = 0; ui4_i < ui4_sample_cnt; ui4_i++)
    {
        UINT32                  ui4_each_dur  = 0;
        UINT32                  ui4_each_size = 0;
        if (ui4_flag & 0x100)
        {
            LOADB_DWRD(pui1_cur, ui4_each_dur);
            pui1_cur += 4; 
        }
        if (pui1_cur > pui1_end) return MMSBTLR_INTERNAL_ERROR;
        
        if (ui4_flag & 0x200)
        {
            LOADB_DWRD(pui1_cur, ui4_each_size);                
            pui1_cur += 4;    
        }
        if (pui1_cur > pui1_end) return MMSBTLR_INTERNAL_ERROR;
        
        if (ui4_flag & 0x400)
        {                
            pui1_cur += 4; 
        }
        if (pui1_cur > pui1_end) return MMSBTLR_INTERNAL_ERROR;
        if (ui4_flag & 0x800)
        {            
            pui1_cur += 4;                 
        }
        if (pui1_cur > pui1_end) return MMSBTLR_INTERNAL_ERROR;

        i4_ret = _sbtl_m4t_add_sample(
                            pt_info,
                           *pui4_total_dur,
                            0,
                            ui4_base_offset,
                            ui4_each_size);
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
    
        if (ui4_time_scale == 0)
        {
            ui4_time_scale = 24*1000;
        }
        if (ui4_time_scale >= 1000)
        {
            *pui4_total_dur += ui4_each_dur/(ui4_time_scale/1000);
        }
        else
        {
            *pui4_total_dur += (UINT32)((UINT64)ui4_each_dur*1000/ui4_time_scale);  
        }
        ui4_base_offset += ui4_each_size;
    }
    return i4_ret;
}



INT32 m4t_mem_build_idx(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    INT32           i4_ret = MMSBTLR_OK;
    UINT32          ui4_cur_offset = 0;;
    UINT32          ui4_box_size = 0;
    UINT32          ui4_box_type = 0;
    UINT8*          pui1_head_data = NULL;
    UINT32          ui4_time_scale  = 0;
    UINT32          ui4_moof_offset = 0;
    UINT32          ui4_total_dur = 0;
    UINT32          ui4_inc_size = 8;
    BOOL            b_force_exit = FALSE;

    if (pt_info == NULL ||
        pt_info->pui1_file_buff == NULL ||
        pt_info->ui4_file_size == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    
    while(ui4_cur_offset < pt_info->ui4_file_size)
    {
        i4_ret = x_sema_lock(pt_info->h_sema, X_SEMA_OPTION_WAIT);

        pui1_head_data = pt_info->pui1_file_buff + ui4_cur_offset;
        
        LOADB_DWRD(pui1_head_data,  ui4_box_size);
        LOADB_DWRD(pui1_head_data+4,ui4_box_type);

        if (ui4_cur_offset+ui4_box_size > pt_info->ui4_file_size)
        {
            i4_ret = x_sema_unlock(pt_info->h_sema);
            break;
        }
        
        switch (ui4_box_type)
        {
            case MP4_BOX_MOOV:
            case MP4_BOX_MVEX:
            case MP4_BOX_TRAF:
                ui4_inc_size = 8;
                break;
                
            case MP4_BOX_MVHD:
            {
                if (ui4_box_size < 24)
                {
                    DBG_ERROR(("MVHD too small\n"));
                }
                else
                {
                    if (ui4_time_scale == 0)
                    {
                        LOADB_DWRD(pui1_head_data + 20, ui4_time_scale);
                    }
                }
                ui4_inc_size = ui4_box_size;
           }
           break;

           case MP4_BOX_SIDX:
           {
                if (ui4_box_size > 12)
                {
                     LOADB_DWRD(pui1_head_data + 16, ui4_time_scale);
                }
                ui4_inc_size = ui4_box_size;
           }
           break;

           case MP4_BOX_MOOF:
           {
                ui4_moof_offset = ui4_cur_offset;
                ui4_inc_size = 8;
           }
           break;
            
        
        case MP4_BOX_MFHD:
            ui4_inc_size = 16;
            break;
                     
        case MP4_BOX_TRUN:
        {
            i4_ret = _sbtl_m4t_parse_idx( 
                               pt_info,
                               pui1_head_data+8,
                               ui4_box_size-8,
                               ui4_moof_offset,
                               &ui4_total_dur,
                               ui4_time_scale);
            if (i4_ret != MMSBTLR_OK)
            {
                DBG_ERROR(("mem parse idx ret:%d\n", i4_ret));
            }
            ui4_inc_size = ui4_box_size;
        }
        break; 
        
        case MP4_BOX_FTYP: 
        case MP4_BOX_TRAK:
        case MP4_BOX_MEHD:
        case MP4_BOX_TREX:
        case MP4_BOX_MDAT:
        case MP4_BOX_MFRA:
        case MP4_BOX_TFHD:
        default:
            if(ui4_box_size == 0)
            {
                ui4_inc_size = 8;
            }
            else
            {
                ui4_inc_size = ui4_box_size;
            }

            break;
        }

        
        b_force_exit = pt_info->b_force_exit;
        i4_ret = x_sema_unlock(pt_info->h_sema);
        //attention do not access any resource in struct after unlock
        if (b_force_exit)
        {
            i4_ret = MMSBTLR_CAN_NOT_OP_IN_THIS_STATE;
            break;
        }
        ui4_cur_offset += ui4_inc_size;
    }

    return i4_ret;
}


//OK
BOOL is_in_file(MM_SBTL_LOAD_FILE_INFO_T* pt_info, UINT8* pui1_buf)
{
    if (pt_info &&
        pt_info->pui1_file_buff)
    {
        if (pui1_buf >= pt_info->pui1_file_buff &&
            pui1_buf < pt_info->pui1_file_buff + pt_info->ui4_file_size)
        {
            return TRUE;
        }
    }
    return FALSE;
}


INT32 sbtl_ipcc_free_sample(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    MM_SBTL_M4T_IDX_INFO_T*   pt_idx)
{
    UINT32                  ui4_i = 0; 
    if (pt_idx == NULL || pt_info == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    if (pt_idx->pt_xml_tree == NULL &&
        pt_idx->b_bad_sample == FALSE)
    {
        return MMSBTLR_OK;
    }
    if (pt_idx->pt_p_nodes)
    {
        x_sbtl_xml_free_node(pt_idx->pt_p_nodes);
        pt_idx->pt_p_nodes = NULL;
    }
    if (pt_idx->pt_xml_tree)
    {
        x_sbtl_xml_parse_free(pt_idx->pt_xml_tree);
        pt_idx->pt_xml_tree = NULL;
    }
    
    free_head_data(pt_idx);
    
    if (pt_idx->pt_image_table)
    {
        for (ui4_i = 0; ui4_i < (UINT32)pt_idx->ui2_imge_num; ui4_i++)
        {
            UINT8* pui1_cur = pt_idx->pt_image_table[ui4_i].pui1_img_data;
            if (pui1_cur &&
                is_in_file(pt_info,pui1_cur) == FALSE &&
                !(pui1_cur >= pt_idx->pui1_sample_data &&
                  pui1_cur < pt_idx->pui1_sample_data + pt_idx->t_base.ui4_size
                 ))
            {
                x_mem_free(pui1_cur);
                pt_idx->pt_image_table[ui4_i].pui1_img_data = NULL;
            }
            if (pt_idx->pt_image_table[ui4_i].h_img != NULL_HANDLE)
            {
                 x_img_close(pt_idx->pt_image_table[ui4_i].h_img);
                 pt_idx->pt_image_table[ui4_i].h_img = NULL_HANDLE;
            }
        }
        x_mem_free(pt_idx->pt_image_table);
        pt_idx->pt_image_table = NULL;
    }
    
    if (pt_info->pui1_file_buff == NULL && pt_idx->pui1_sample_data)
    {
        x_mem_free(pt_idx->pui1_sample_data);
        pt_idx->pui1_sample_data = NULL;
    }

    return MMSBTLR_OK;
}

INT32 sbtl_xml_parse_sample(
    MM_SBTL_M4T_IDX_INFO_T* pt_idx,
    UINT8*                  pui1_buf, 
    UINT32                  ui4_len)
{
    INT32 i4_ret = MMSBTLR_OK;
    if (pt_idx == NULL ||
        pui1_buf == NULL ||
        ui4_len  == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    UINT32 ui4_i = 0;
    UINT32 ui4_tk_len = 0;
    UINT8* pui1_tk = NULL;
    BOOL   b_found = FALSE;
    UINT32 ui4_head_len = 0;
    while (ui4_i < ui4_len)
    {
        if (pui1_buf[ui4_i] == '<')
        {
            pui1_tk = get_words(pui1_buf+ui4_i+1,
                                ui4_len-ui4_i-1,
                                &ui4_tk_len);
            if (pui1_tk)
            {
                if (ui4_tk_len == 2 &&
                    x_memcmp(pui1_tk, "tt", ui4_tk_len) == 0)
                {
                    if (b_found)
                    {
                        break;
                    }
                    b_found = TRUE;
                    ui4_head_len = ui4_i;
                }
                ui4_i += ui4_tk_len;
            } 
        }
        ui4_i++;
    }
    
    if (ui4_i < ui4_len)
    {
        while (ui4_i < ui4_len)
        {
            if (pui1_buf[ui4_i] == '>')
            {
                pt_idx->ui4_xml_len = ui4_i + 1;
                break;
            }
            ui4_i++;
        } 
    }
    else
    {
        pt_idx->ui4_xml_len = ui4_len;
    }
    DBG_INFO(("Xml len is:%d, total len is:%d headlen:%d\n", pt_idx->ui4_xml_len, ui4_len, ui4_head_len));
    //start to parse xml file
    i4_ret = sbtl_xml_parse_file (pt_idx,
                                  pui1_buf+ui4_head_len,
                                  pt_idx->ui4_xml_len-ui4_head_len);
    if (i4_ret != MMSBTLR_OK)
    {
        DBG_ERROR(("Parse xml content ret:%d", i4_ret));
    }
    else if (pt_idx->ui4_xml_len < ui4_len)
    {
        //start to parse png
        i4_ret = parse_all_image(pt_idx,
                                 pui1_buf + pt_idx->ui4_xml_len,
                                 ui4_len - pt_idx->ui4_xml_len);
        if (i4_ret != MMSBTLR_OK)
        {
            DBG_ERROR(("parse image ret:%d\n", i4_ret));
        } 
    }
    if (pt_idx->ui2_imge_num == 0 ||
        pt_idx->pt_image_table == NULL)
    {
        return i4_ret;
    }
    DBG_INFO(("before mopen file:%d\n", sbtl_cur_time()));
    for (ui4_i = 0; ui4_i < (UINT32)pt_idx->ui2_imge_num; ui4_i++)
    {
        if (pt_idx->pt_image_table[ui4_i].pui1_img_data == NULL ||
            pt_idx->pt_image_table[ui4_i].ui4_img_size == 0)
        {
            continue;
        }
        if (pt_idx->pt_image_table[ui4_i].e_img_type == MM_SBTL_IMG_TYPE_PNG)
        {
            i4_ret = x_img_mopen(
                         pt_idx->pt_image_table[ui4_i].pui1_img_data,
                         pt_idx->pt_image_table[ui4_i].ui4_img_size, 
                         "png", 
                         NULL, 
                         NULL, 
                         &pt_idx->pt_image_table[ui4_i].h_img);
            DBG_INFO(("image open:%d [%x] ret %d\n", ui4_i, pt_idx->pt_image_table[ui4_i].h_img, i4_ret));
            if (i4_ret != IMGR_OK)
            {
               DBG_ERROR(("image open:%d ret %d\n", ui4_i, i4_ret));
            }
        }
    }
    DBG_INFO(("after mopen file:%d\n", sbtl_cur_time()));
    
    return i4_ret;
}


INT32 xml_mem_build_idx(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    INT32                   i4_ret = MMSBTLR_OK;
    MM_SBTL_M4T_IDX_INFO_T* pt_idx;
    if (pt_info == NULL ||
        pt_info->pui1_file_buff == NULL ||
        pt_info->ui4_file_size == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_idx = (MM_SBTL_M4T_IDX_INFO_T*) x_mem_alloc(sizeof(MM_SBTL_M4T_IDX_INFO_T));
    if (pt_idx == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    x_memset(pt_idx, 0, sizeof(MM_SBTL_M4T_IDX_INFO_T));
    pt_idx->t_base.ui4_size = pt_info->ui4_file_size;

    i4_ret = sbtl_xml_parse_sample( pt_idx,
                        pt_info->pui1_file_buff , 
                        pt_info->ui4_file_size); 
    pt_info->pv_samle_index = (VOID**)x_mem_alloc(sizeof(VOID*) * 5);
     
    if (pt_info->pv_samle_index == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    pt_info->ui4_sample_num = 1;
    pt_info->pv_samle_index[0] = pt_idx;
     
    return i4_ret;
}


INT32 sbtl_xml_file_parse(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    MM_SBTL_M4T_IDX_INFO_T*   pt_idx )
{
    INT32           i4_ret = MMSBTLR_OK;
    MEDIA_FEEDER_T* pt_feeder;
    copybytes_fct   pf_copybytes = NULL;
    setpos_fct      pf_set_pos;
    UINT8*          pui1_file_cont = NULL;
    UINT32          ui4_readed = 0;
    UINT32          ui4_to_read = 0;
    UINT32          ui4_real_read = 0;
    BOOL            b_force_exit = FALSE;
    
    if (pt_info == NULL ||
        pt_idx == NULL || 
        pt_idx->t_base.ui4_size == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    
    
    pt_feeder    =   &pt_info->t_feeder;
    pf_copybytes = pt_feeder->t_feeder_be_fct_tbl.pf_copybytes;
    pf_set_pos   = pt_feeder->t_feeder_be_fct_tbl.pf_set_pos;
    
    if (pf_copybytes == NULL ||  pf_set_pos == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    pui1_file_cont = x_mem_alloc(pt_idx->t_base.ui4_size);
    if (pui1_file_cont == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_idx->pui1_sample_data = pui1_file_cont;
    DBG_INFO(("before read file:%d\n", sbtl_cur_time()));
    
    while (ui4_readed < pt_idx->t_base.ui4_size)
    {
        i4_ret = x_sema_lock(pt_info->h_sema, X_SEMA_OPTION_WAIT);
        
        ui4_to_read = 4*1024;
        if (ui4_to_read + ui4_readed > pt_idx->t_base.ui4_size)
        {
            ui4_to_read = pt_idx->t_base.ui4_size - ui4_readed;
        }
        
        pf_set_pos(pt_feeder, pt_idx->t_base.ui4_offset + ui4_readed, MEDIA_SEEK_BGN);
        
        i4_ret = pf_copybytes(pt_feeder, 
                         pui1_file_cont+ui4_readed, 
                         ui4_to_read, 
                         ui4_to_read,
                         &ui4_real_read);
       
        
        ui4_readed += ui4_real_read;
        b_force_exit = pt_info->b_force_exit;
        i4_ret = x_sema_unlock(pt_info->h_sema);
        if (i4_ret != 0 || ui4_to_read != ui4_real_read)
        {
            DBG_ERROR(("To read:%d only readed %d ret:%d\n", ui4_to_read, ui4_readed, i4_ret));
            break;
        }
        if (b_force_exit)
        {
            i4_ret = MMSBTLR_CAN_NOT_OP_IN_THIS_STATE;
            break;
        }
    }
    DBG_INFO(("after read file:%d\n", sbtl_cur_time()));
    if (i4_ret == 0 &&
        ui4_readed == pt_idx->t_base.ui4_size)
    {
        i4_ret = sbtl_xml_parse_sample(
                          pt_idx,
                          pui1_file_cont,
                          ui4_readed);
    }
    return i4_ret;
}


INT32 xml_file_build_idx(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    INT32                   i4_ret = MMSBTLR_OK;
    MM_SBTL_M4T_IDX_INFO_T* pt_idx;
    if (pt_info == NULL ||
        pt_info->ui4_file_size == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_idx = (MM_SBTL_M4T_IDX_INFO_T*) x_mem_alloc(sizeof(MM_SBTL_M4T_IDX_INFO_T));
    if (pt_idx == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    x_memset(pt_idx, 0, sizeof(MM_SBTL_M4T_IDX_INFO_T));
    pt_idx->t_base.ui4_size = pt_info->ui4_file_size;

    i4_ret = sbtl_xml_file_parse (pt_info, pt_idx );

    pt_info->pv_samle_index = (VOID**)x_mem_alloc(sizeof(VOID*) * 5);
     
    if (pt_info->pv_samle_index == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    }
    pt_info->ui4_sample_num = 1;
    pt_info->pv_samle_index[0] = pt_idx;
    return i4_ret;
}

INT32 sbtl_xml_parse_all_image(
    MM_SBTL_M4T_IDX_INFO_T* pt_idx,
    UINT8*                  pui1_buf, 
    UINT32                  ui4_len)
{
    INT32                i4_ret = MMSBTLR_OK;
    MM_SBTL_IMG_INFO_T   t_img_info = {0};
    UINT32               ui4_cur_pos = 0;
    UINT8*               pui1_cur = pui1_buf;
    UINT32               ui4_left_size = ui4_len;
    
    if (pt_idx == NULL ||  pui1_buf == NULL || ui4_len == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    
    while (ui4_cur_pos < ui4_left_size)
    {
        ui4_left_size -= ui4_cur_pos;
        pui1_cur      += ui4_cur_pos;
        ui4_cur_pos    = get_png_start(pui1_cur, 
                                    ui4_left_size);
        if (ui4_cur_pos >= ui4_left_size)
        {
            break;
        }
        i4_ret = parse_png(&t_img_info, 
                           pui1_cur + ui4_cur_pos, 
                           ui4_left_size-ui4_cur_pos);
        if (i4_ret != MMSBTLR_OK)
        { 
            ui4_cur_pos++;
            continue;
        }
        DBG_INFO(("Got image:%d %d*%d\n", pt_idx->ui2_imge_num, t_img_info.ui4_width, t_img_info.ui4_height));
        if ((pt_idx->ui2_imge_num % 50) == 0)
        {
            pt_idx->pt_image_table = x_mem_realloc(
                    pt_idx->pt_image_table,
                    (pt_idx->ui2_imge_num + 50)*sizeof(MM_SBTL_IMG_INFO_T));
        }
        if (pt_idx->pt_image_table == NULL)
        {
            i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
            break;
        }
        pt_idx->pt_image_table[pt_idx->ui2_imge_num] = t_img_info;
        pt_idx->ui2_imge_num++;
        
        ui4_cur_pos += t_img_info.ui4_img_size;
    
    }
    
    return i4_ret; 
}

INT32 sbtl_xml_get_node_time(
    XML_NODE_T* pt_nodes,
    UINT32      ui4_node_idx,
    UINT32*     pui1_start_pts,
    UINT32*     pui1_end_pts)
{   
    CHAR         *ps_tm_dur;
    CHAR         *ps_tm_begin;
    CHAR         *ps_tm_end;
    INT32         i4_ret = 0;
    UINT32        ui4_pts_time;    
    XML_T        *pt_p = NULL;
        
    pt_p = x_sbtl_xml_get_element_byidx(pt_nodes, ui4_node_idx);
    if (pt_p == NULL)
    {
         return MMSBTLR_INV_ARG;
    }
    do
    {
        ps_tm_begin = x_sbtl_xml_get_attr_value_bytype(SBTL_XML_IPCC_ATTR_BEGIN, pt_p);
        if (NULL == ps_tm_begin)
        {
            break;
        }
        i4_ret = sbtl_get_timecode(ps_tm_begin, pui1_start_pts);
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
        ps_tm_end = x_sbtl_xml_get_attr_value_bytype(SBTL_XML_IPCC_ATTR_END, pt_p);
        if (ps_tm_end)
        {
            i4_ret = sbtl_get_timecode(ps_tm_end, &ui4_pts_time);
            if (i4_ret != MMSBTLR_OK)
            {
                break;
            }
            *pui1_end_pts = ui4_pts_time;
            return MMSBTLR_OK;
        }
        
        ps_tm_dur = x_sbtl_xml_get_attr_value_bytype(SBTL_XML_IPCC_ATTR_DUR, pt_p); 
        if (ps_tm_dur)
        {
            i4_ret = sbtl_get_timecode(ps_tm_dur, &ui4_pts_time);
            if (i4_ret != MMSBTLR_OK)
            {
                break;
            }
            *pui1_end_pts  = ui4_pts_time+ *pui1_start_pts;        
            return MMSBTLR_OK;
        }
        *pui1_end_pts = 0xFFFFFFFF;
        return MMSBTLR_OK;
    }while(0);
    return MMSBTLR_INTERNAL_ERROR;
}

INT32 free_head_data(MM_SBTL_M4T_IDX_INFO_T* pt_idx)
{
    SBTL_TTML_ATTR_ID_T* pt_com_id_attr_list = pt_idx->t_common_id_attr_list.pt_head;
    
    if (pt_idx->h_style != NULL_HANDLE)
    {
        x_sbtl_xml_free_hash(pt_idx->h_style);
    }
    if (pt_idx->h_region != NULL_HANDLE)
    {
        x_sbtl_xml_free_hash(pt_idx->h_region);
    }
    pt_idx->h_style  = NULL_HANDLE;
    pt_idx->h_region = NULL_HANDLE;

          
    while (pt_com_id_attr_list  != NULL) 
    {                                
        SBTL_TTML_ATTR_ID_T*       pt_id_list_cur = NULL;
        SBTL_TTML_ATTR_T*          pt_attr_list = NULL;
        pt_id_list_cur = pt_com_id_attr_list;
        pt_attr_list = (pt_com_id_attr_list->t_attr_list).pt_head;
        while(pt_attr_list != NULL)
        {                         
            SBTL_TTML_ATTR_T*  pt_attr_list_cur = pt_attr_list;
            
            SLIST_REMOVE(pt_attr_list,t_attr_link);                                                   
            x_mem_free(pt_attr_list_cur);         
            
            pt_attr_list = (pt_com_id_attr_list->t_attr_list).pt_head;
            if(pt_attr_list == NULL) break;                        
        }

        SLIST_REMOVE(pt_com_id_attr_list,t_attr_id_link);   
        pt_com_id_attr_list = pt_idx->t_common_id_attr_list.pt_head;;                
        x_mem_free(pt_id_list_cur);   
        if(pt_com_id_attr_list == NULL) break;
    }
    return MMSBTLR_OK;
}





static INT32 ipcc_free_attr()
{
    SBTL_TTML_ATTR_T* pt_attr_cur = NULL;
    
    pt_attr_cur = SLIST_FIRST(&t_attr_cur);
    while(pt_attr_cur)
    {
        SBTL_TTML_ATTR_T* pt_next = SLIST_NEXT(pt_attr_cur, t_attr_link);
        SLIST_REMOVE(pt_attr_cur,t_attr_link);
        x_mem_free(pt_attr_cur);
        pt_attr_cur = pt_next;
    }
    return MMSBTLR_OK;
}

static SBTL_TTML_ATTR_FONT_FAMILY   get_font_family_type_value(CHAR* ps_value)
{
    INT8   i1_idx =0;    
    CHAR  *ps_font_family[8] ={
        "default",            "monospaceSerif",        "proportionalSerif",
        "monospaceSansSerif", "proportionalSansSerif",
        "Casual<TBD>",        "Cursive<TBD>",          "Samll<TBD>"};   
    for( ;i1_idx<8;i1_idx++)
    {
        if(0  == x_strcmp(ps_value, ps_font_family[i1_idx]))
        {
            break;
        }
    }
    switch(i1_idx)
    {
        case 0:
            return SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT;
        case 1:
            return SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESERIF;
        case 2:
            return SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSERIF;
        case 3:
            return SBTL_TTML_ATTR_FONT_FAMILY_MONOSPACESANSSERIF;
        case 4:                    
            return SBTL_TTML_ATTR_FONT_FAMILY_PROPORTIONALSANSSERIF;
        case 5:              
           return SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD;
        case 6:
           return SBTL_TTML_ATTR_FONT_FAMILY_CASUAL_TBD;
       case 7:
           return  SBTL_TTML_ATTR_FONT_FAMILY_SMALL_TBD;
        default:
            return SBTL_TTML_ATTR_FONT_FAMILY_DEFAULT;
    }
}

static SBTL_TTML_ATTR_TEXT_ALIGIN   get_text_align_type_value(CHAR* ps_value)
{
    INT8    i1_idx =0;    
    CHAR    *ps_t_align[5] ={"start","end","left","right","center"};
    for( ;i1_idx<5;i1_idx++)
    {
        if(0  == x_strcmp(ps_value, ps_t_align[i1_idx]))
        {
            break;
        }
    }
    switch(i1_idx)
    {
        case 0:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_START;
        case 1:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_END;
        case 2:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_LEFT;
        case 3:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_RIGHT;
        case 4:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_CENTER;
        default:
            return SBTL_TTML_ATTR_TEXT_ALIGIN_START;
    }
}

static SBTL_TTML_ATTR_VISIBILITY   get_visibility_type_value(CHAR* ps_value)
{
    if(0  == x_strcmp(ps_value, "hidden"))
    {
            return SBTL_TTML_ATTR_VISIBILITY_HIDDEN;
    }
   return SBTL_TTML_ATTR_VISIBILITY_VISIBLE;
}

 static SBTL_TTML_ATTR_FONT_WEIGHT get_font_weight_type_value(CHAR* ps_value)
 {
     if(0  == x_strcmp(ps_value, "bold"))
     {
             return SBTL_TTML_ATTR_FONT_WEGHT_BOLD;
     }
     return SBTL_TTML_ATTR_FONT_WEGHT_NORMAL;
 }

static SBTL_TTML_ATTR_TEXT_DECORATION get_text_decoration_type_value(CHAR* ps_value)
{
     if(0  == x_strcmp(ps_value, "underline"))
     {
             return SBTL_TTML_ATTR_TEXT_DECORATION_UNDERLINE;
     }    
     return SBTL_TTML_ATTR_TEXT_DECORATION_NONE;
}

static SBTL_TTML_ATTR_FONT_STYLE get_font_style_type_value(CHAR* ps_value)
{
     if(0  == x_strcmp(ps_value, "italic"))
     {
             return SBTL_TTML_ATTR_FONT_STYLE_ITALIC;
     }    
     return SBTL_TTML_ATTR_FONT_STYLE_NORMAL;
}


static SBTL_TTML_ATTR_WHITESAPCE get_white_space_type_value(CHAR* ps_value)
{
    if(0  == x_strcmp(ps_value, "preserve"))
    {
          return SBTL_TTML_ATTR_WHITESPACE_PRESERVE;
    }    
    return SBTL_TTML_ATTR_WHITESPACE_DEFAULT;
}


//OK
static SBTL_TTML_ATTR_T* _ipcc_alloc_attr(INT32 i4_type)
{
    SBTL_TTML_ATTR_T* pt_attr_s = NULL;
    BOOL              b_found   = FALSE;
    
    if (!SLIST_IS_EMPTY(&t_attr_cur))
    {                               
        SLIST_FOR_EACH(pt_attr_s, &t_attr_cur , t_attr_link)
        {
            if(pt_attr_s != NULL &&
               i4_type == pt_attr_s->e_attr)
            {
                b_found = TRUE;
                break;
            }
        }
    }
    if (b_found == FALSE)
    {
        pt_attr_s = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
        if (pt_attr_s)
        {
            SLIST_INSERT_HEAD(pt_attr_s, &t_attr_cur, t_attr_link);
        }
    }
    if (pt_attr_s)
    {
        pt_attr_s->e_attr = (SBTL_IPCC_ATTR_T)i4_type;
    }
    return pt_attr_s;
}

//OK
static INT32 _ipcc_comm_iter_all(XML_ATTRIBUTE_T*   pt_attr,
                          VOID*              pv_arg,
                          INT32              i4_depth)
{   
    SBTL_TTML_ATTR_T* pt_attr_s = NULL;
    
    if (pt_attr           == NULL || 
        pt_attr->ps_value == NULL ||
        i4_depth > 3)
    {
        return MMSBTLR_INV_ARG;
    }
        
    if (SBTL_XML_IPCC_ATTR_BGCOLOR == pt_attr->i4_type)
    {
        if ( pv_arg != NULL )
        {
            SBTL_XML_ELEM_T* pe_elm_type = (SBTL_XML_ELEM_T*)pv_arg;
            if (*pe_elm_type  == SBTL_XML_ELM_REGION)
            {
                pt_attr->i4_type = SBTL_XML_IPCC_ATTR_REGION_BGCOLOR;  
            }
        }
    }
    
    pt_attr_s = _ipcc_alloc_attr(pt_attr->i4_type);
    if(NULL == pt_attr_s)
    {
        return MMSBTLR_OK;
    }
    
    switch(pt_attr->i4_type)
    {
       case SBTL_XML_IPCC_ATTR_ID:
       case SBTL_XML_IPCC_ATTR_STYLE:
       case SBTL_XML_IPCC_ATTR_REGION:         
       case SBTL_XML_IPCC_ATTR_FONTSIZE:
       case SBTL_XML_IPCC_ATTR_BGCOLOR:
       case SBTL_XML_IPCC_ATTR_COLOR: 
       case SBTL_XML_IPCC_ATTR_FONT_EXTENT:
       case SBTL_XML_IPCC_ATTR_CELL_RESOLUTION:
       case SBTL_XML_IPCC_ATTR_ORIGIN:
       case SBTL_XML_IPCC_ATTR_WINDOW_OPACITY:
       case SBTL_XML_IPCC_ATTR_TEXT_OUTLINE:
       case SBTL_XML_IPCC_ATTR_WRAP_OPTION:
       case SBTL_XML_IPCC_ATTR_LINEHEIGHT:
       case SBTL_XML_IPCC_ATTR_ROLE:
       case SBTL_XML_IPCC_ATTR_REGION_BGCOLOR:
       case SBTL_XML_IPCC_ATTR_BGIMG:
            pt_attr_s->u.ps_value = pt_attr->ps_value;
       break;
        
       case SBTL_XML_IPCC_ATTR_BEGIN:
       case SBTL_XML_IPCC_ATTR_END:                
       case SBTL_XML_IPCC_ATTR_DUR:
       {   
           UINT32 ui4_tm = 0;
           sbtl_get_timecode(pt_attr->ps_value, &ui4_tm);
           pt_attr_s->u.e_pts_tm = (PTS_T)ui4_tm;
       }
       break;     
       
       case SBTL_XML_IPCC_ATTR_FONTFAMILY:                             
          pt_attr_s->u.e_font_family = get_font_family_type_value(pt_attr->ps_value);       
       break;
      
       case SBTL_XML_IPCC_ATTR_TEXTALIGN:                                     
          pt_attr_s->u.e_text_align = get_text_align_type_value(pt_attr->ps_value);   
       break; 
       
       case SBTL_XML_IPCC_ATTR_VISIBILY:                                         
           pt_attr_s->u.e_visibility = get_visibility_type_value(pt_attr->ps_value);   
       break;
      
       case SBTL_XML_IPCC_ATTR_FONT_WEIGHT:                                            
           pt_attr_s->u.e_font_weight = get_font_weight_type_value(pt_attr->ps_value);
       break;   
       
       case SBTL_XML_IPCC_ATTR_TEXT_DECORATION:                                          
           pt_attr_s->u.e_text_decoration = get_text_decoration_type_value(pt_attr->ps_value);
       break;  
         
       case SBTL_XML_IPCC_ATTR_FONT_STYLE:                                          
           pt_attr_s->u.e_font_style = get_font_style_type_value(pt_attr->ps_value);
       break;
           
       case SBTL_XML_IPCC_ATTR_WHITESPACE:                                          
           pt_attr_s->u.e_whitespace = get_white_space_type_value(pt_attr->ps_value);
           if(SBTL_TTML_ATTR_WHITESPACE_PRESERVE == pt_attr_s->u.e_whitespace)
           {
               b_is_whitspace_valid = TRUE;
           }
       break;
         
       default:                
       break;
    }     
    
    return MMSBTLR_OK; 
}

static INT32 _xml_get_comm_id(
    SBTL_TTML_ATTR_LIST_T*      pt_comm_id,
    XML_NODE_T*                 pt_node,
    SBTL_XML_ELEM_T             e_ele_type)
                                                                                       
{
    SBTL_TTML_ATTR_T*       pt_iter = NULL;
    SBTL_TTML_ATTR_T*       pt_iter_attr_list = NULL;
    INT32                   i4_ret;
    UINT32                  ui4_node_num = 0;
    UINT8                   ui1_id_idx = 0;
    SBTL_TTML_ATTR_ID_T*    pt_com_id_list = NULL;
    XML_T*                  pt_idx_node = NULL;
    SBTL_IPCC_ATTR_T         e_attr_type = SBTL_XML_IPCC_ATTR_REGION;
   
    if(SBTL_XML_ELM_STYLE == e_ele_type)
    {
        e_attr_type = SBTL_XML_IPCC_ATTR_STYLE;        
    }
    else if(SBTL_XML_ELM_REGION == e_ele_type)
    {        
        e_attr_type = SBTL_XML_IPCC_ATTR_REGION;        
    }
    
    i4_ret = x_sbtl_xml_get_element_num(pt_node, &ui4_node_num);  

    for( ; ui1_id_idx < ui4_node_num; ui1_id_idx++)
    {       
       BOOL   b_id = FALSE;
       pt_idx_node = x_sbtl_xml_get_element_byidx(pt_node, ui1_id_idx);
             
       SLIST_INIT(&t_attr_cur);  
       
       x_sbtl_xml_iterate_all_attr(pt_idx_node,
                                   _ipcc_comm_iter_all,
                                   (VOID*)&e_ele_type,
                                    0);
       if(SLIST_IS_EMPTY(&t_attr_cur))
       {
          return MMSBTLR_NOT_FOUND;
       }
       pt_com_id_list = x_mem_alloc(sizeof(SBTL_TTML_ATTR_ID_T));
       if (pt_com_id_list != NULL)
       {
           pt_com_id_list->e_attr_type = e_attr_type;
           pt_com_id_list->ps_attr_id_value = NULL;
                   
           SLIST_INIT(&pt_com_id_list->t_attr_list);
           DBG_INFO(("===Load Common type:%d===\n",e_ele_type));
           SLIST_FOR_EACH(pt_iter ,&t_attr_cur, t_attr_link)
           {   
               sbtl_xml_dump_attrs(pt_iter);
               if(SBTL_XML_IPCC_ATTR_ID != pt_iter->e_attr)
               {                        
                  pt_iter_attr_list = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
                   if(NULL != pt_iter_attr_list)
                   {
                       x_memcpy(pt_iter_attr_list,pt_iter,sizeof(SBTL_TTML_ATTR_T));
                       SLIST_INSERT_HEAD(pt_iter_attr_list,&(pt_com_id_list->t_attr_list),t_attr_link);
                   }          
               }
               else
               {
                    if (b_id == FALSE)
                    {   
                        pt_com_id_list->ps_attr_id_value = pt_iter->u.ps_value;
                        SLIST_INSERT_HEAD(pt_com_id_list, pt_comm_id, t_attr_id_link);
                        b_id = TRUE;
                    }
               }
           }
       }
       ipcc_free_attr();
    }
    
    return i4_ret;
}


static INT32 _sbtl_xml_load_head_data(
    XML_T*                  pt_top, 
    MM_SBTL_M4T_IDX_INFO_T* pt_idx)
{
    XML_T*      pt_parent = NULL;
    XML_T*      pt_cur    = NULL;
    XML_NODE_T* pt_style  = NULL;
    XML_NODE_T* pt_region = NULL;
    INT32       i4_ret    = MMSBTLR_OK;
    
    pt_parent = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_HEAD, pt_top);
    if(pt_parent == NULL)
    {
        return MMSBTLR_NOT_FOUND;
    }
    
    pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_STYLING, pt_parent);
    if(NULL != pt_cur)
    {     
        i4_ret = x_sbtl_xml_create_ref_hash(pt_cur,
                          (INT32)SBTL_XML_ELM_STYLE,
                          (INT32)SBTL_XML_IPCC_ATTR_ID,
                          &pt_idx->h_style);

        i4_ret = x_sbtl_xml_get_elements_bytype(
                            pt_top,
                            &pt_style,
                            SBTL_XML_ELM_STYLE);
    }
    
    pt_cur = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_LAYOUT, pt_parent);
    if(NULL != pt_cur)
    {
        i4_ret = x_sbtl_xml_create_ref_hash(pt_cur,
                              (INT32)SBTL_XML_ELM_REGION,
                              (INT32)SBTL_XML_IPCC_ATTR_ID,
                              &pt_idx->h_region);
        
        i4_ret = x_sbtl_xml_get_elements_bytype(
                               pt_top,
                               &pt_region,
                               SBTL_XML_ELM_REGION);
    } 
    do
    {
        if (pt_style != NULL)
        {
            i4_ret = _xml_get_comm_id(&pt_idx->t_common_id_attr_list, 
                                     pt_style, 
                                     SBTL_XML_ELM_STYLE);
            if (MMSBTLR_OK != i4_ret)
            {
                pt_idx->b_bad_sample = TRUE;
                break;
            }
        }
        
        if (pt_region != NULL)
        {
            i4_ret = _xml_get_comm_id(&pt_idx->t_common_id_attr_list, 
                                     pt_region, 
                                     SBTL_XML_ELM_REGION);
            if (MMSBTLR_OK != i4_ret)
            {
                pt_idx->b_bad_sample = TRUE;
                break;
            }
        } 
    }while (0);
    if (i4_ret != MMSBTLR_OK)
    {
        free_head_data(pt_idx);
    }
    if (pt_style)
    {
        x_sbtl_xml_free_node(pt_style);
    }
    if (pt_region)
    {
        x_sbtl_xml_parse_free(pt_region);
    }
    return MMSBTLR_OK;
}


//OK
static VOID _sbtl_xml_id2reg(XML_NODE_T* pt_node, UINT16 ui2_num)
{
     UINT16   ui2_i = 0;
     XML_T*   pt_p = NULL;
     
     if (pt_node == NULL)
     {
        return;
     }
     for (ui2_i = 0; ui2_i < ui2_num; ui2_i++)
     {
        XML_ATTRIBUTE_T* pt_rep = NULL;
        XML_ATTRIBUTE_T* pt_attr = NULL;
        pt_p = x_sbtl_xml_get_element_byidx(pt_node, ui2_i);
        if (pt_p == NULL)
        {
            break;
        }    
        pt_attr = x_sbtl_xml_get_first_attr(pt_p);
        while(pt_attr)
        {
            if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_REGION)
            {
                pt_rep = pt_attr;
                break;
            }
            if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_ID && pt_rep == NULL)
            {
                pt_rep = pt_attr;
            }
            pt_attr = x_sbtl_xml_get_next_attr(pt_attr);
        }
        if (pt_rep)
        {
            pt_rep->i4_type = SBTL_XML_IPCC_ATTR_REGION;
        }
     }    
}



static INT32 _sbtl_iter_image_attr(
    XML_ATTRIBUTE_T*   pt_attr,
    VOID*              pv_arg,
    INT32              i4_depth)
{
    if (pt_attr           == NULL || 
        pt_attr->ps_value == NULL ||
        i4_depth > 3              ||
        pv_arg == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    MM_SBTL_IMG_INFO_T* pt_img_info = (MM_SBTL_IMG_INFO_T*) pv_arg;
    switch (pt_attr->i4_type)
    {
        case SBTL_XML_IPCC_ATTR_IMAGE_TYPE:
        if (x_strcasecmp(pt_attr->ps_value, "png") == 0)
        {
            pt_img_info->e_img_type = MM_SBTL_IMG_TYPE_PNG;
        }
        break;

        case SBTL_XML_IPCC_ATTR_ID:
        {
            pt_img_info->ps_image_id = pt_attr->ps_value;
        }
        break;

        case SBTL_XML_IPCC_ATTR_ECODING:
        {
            if (x_strcasecmp(pt_attr->ps_value, "Base64") == 0)
            {
                pt_img_info->e_encoding = MM_SBTL_IMG_ENC_BASE64;
            }
        }
        break;
        
        default:
        break;
            
    }
    return MMSBTLR_OK;
}

static INT32 _sbtl_xml_decode_img(
    MM_SBTL_M4T_IDX_INFO_T* pt_idx,
    XML_T*                  pt_xml)
{
    INT32 i4_ret = MMSBTLR_OK;
    
    if (pt_xml == NULL || pt_idx == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    do
    {
        XML_T* pt_cur =  x_sbtl_xml_get_first_element(SBTL_XML_ELM_HEAD, pt_xml);
        if (pt_cur == NULL)
        {
            break;
        }
        pt_cur = x_sbtl_xml_get_first_element(SBTL_XML_ELM_METADATA, pt_cur);
        if (pt_cur == NULL)
        {
            break;
        }
        pt_cur = x_sbtl_xml_get_first_element(SBTL_XML_ELM_IMAGE, pt_cur);
        while (pt_cur)
        {
            MM_SBTL_IMG_INFO_T t_image_info;
            XML_T*             pt_proc = pt_cur;
            pt_cur = x_sbtl_xml_get_next_element(SBTL_XML_ELM_IMAGE, pt_cur);

            if (pt_proc->ps_value == NULL)
            {
                continue;
            }
            
            x_memset(&t_image_info, 0, sizeof(MM_SBTL_IMG_INFO_T));
            x_sbtl_xml_iterate_all_attr(pt_proc,
                                   _sbtl_iter_image_attr,
                                   (VOID*)&t_image_info,
                                    0);
            
            if (t_image_info.e_encoding == MM_SBTL_IMG_ENC_BASE64 &&
                t_image_info.e_img_type == MM_SBTL_IMG_TYPE_PNG)
            {
                INT32  i4_len = 0;
                CHAR*  ps_png  = NULL;
                sbtl_base64_chomp((CHAR*)pt_proc->ps_value);
                i4_len = x_strlen(pt_proc->ps_value);
                if (i4_len <= 0)
                {
                    continue;
                }
                ps_png = (CHAR*)x_mem_alloc(i4_len/4*3 + 2);
                if (ps_png == NULL)
                {
                    break;
                }
                i4_len = sbtl_base64_decode((CHAR*)pt_proc->ps_value, ps_png, i4_len);
                if (i4_len <= 0)
                {
                    x_mem_free(ps_png);
                    continue;
                }
                t_image_info.pui1_img_data = (UINT8*)ps_png;
                t_image_info.ui4_img_size  = (UINT32)i4_len;
                if (i4_len > 16)
                {
                    DBG_INFO(("decoded: %x %x %x %x %x %x\n",
                        ps_png[0], ps_png[1], ps_png[2], ps_png[3], ps_png[4], ps_png[5]));
                }
                i4_ret = parse_png(&t_image_info, (UINT8*)ps_png, (UINT32)i4_len);
                if (i4_ret != MMSBTLR_OK)
                {
                    x_mem_free(ps_png);
                    continue;
                }

                DBG_INFO(("Got image:%d %d*%d\n", pt_idx->ui2_imge_num, t_image_info.ui4_width, t_image_info.ui4_height));
                if ((pt_idx->ui2_imge_num % 50) == 0)
                {
                    pt_idx->pt_image_table = x_mem_realloc(
                            pt_idx->pt_image_table,
                            (pt_idx->ui2_imge_num + 50)*sizeof(MM_SBTL_IMG_INFO_T));
                }
                if (pt_idx->pt_image_table == NULL)
                {
                    i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
                    break;
                }
                pt_idx->pt_image_table[pt_idx->ui2_imge_num] = t_image_info;
                pt_idx->ui2_imge_num++;
                
            }    
        }
        return MMSBTLR_OK;
    }while (0);
    
    return MMSBTLR_NOT_FOUND;
}


INT32 sbtl_xml_parse_file(
    MM_SBTL_M4T_IDX_INFO_T* pt_idx,
    UINT8*                  pui1_buf, 
    UINT32                  ui4_len)
{
    INT32          i4_ret     = MMSBTLR_OK;
    XML_T*         pt_xml     = NULL;
    XML_NODE_T*    pt_p_nodes = NULL;
    XML_NODE_T*    pt_div_nodes = NULL;

    if (pt_idx == NULL || pui1_buf == NULL || ui4_len == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    if (sbtl_ipcc_init_parse_info() != MMSBTLR_OK)
    {
        return MMSBTLR_NOT_INIT;
    }
    
    i4_ret = x_sbtl_xml_parse_by_memory((VOID*)pui1_buf,
                                       ui4_len,
                                       (VOID**)&pt_xml,
                                       &t_parse_info);
    if (MMSBTLR_OK != i4_ret || pt_xml == NULL)
    {   
        pt_idx->b_bad_sample = TRUE;                               
        return i4_ret;
    }
    
    pt_idx->pt_xml_tree = pt_xml;

    i4_ret = _sbtl_xml_decode_img(pt_idx, pt_xml);
    if (i4_ret != MMSBTLR_OK && i4_ret != MMSBTLR_NOT_FOUND)
    {
        DBG_ERROR(("Decode image ret:%d\n", i4_ret));
    }
        
    i4_ret = x_sbtl_xml_get_elements_bytype(pt_xml,
                                           &pt_p_nodes,
                                            SBTL_XML_ELM_PARA); 
    i4_ret = x_sbtl_xml_get_elements_bytype(pt_xml,
                                           &pt_div_nodes,
                                            SBTL_XML_ELM_DIV); 
    if (pt_p_nodes || pt_div_nodes)
    {
        UINT32 ui4_node_num = 0;
        UINT32 aui4_pts[2]   = {0,0};
        
        pt_idx->pt_p_nodes = pt_p_nodes;
        pt_idx->pt_div_nodes = pt_div_nodes;
        
        _sbtl_xml_load_head_data(pt_xml, pt_idx);
        
        i4_ret = x_sbtl_xml_get_element_num(pt_p_nodes, &ui4_node_num);
        if (ui4_node_num == 0)
        {
            i4_ret = x_sbtl_xml_get_element_num(pt_div_nodes, &ui4_node_num);
            pt_p_nodes = pt_div_nodes;
        }
        if (ui4_node_num > 0)
        {
             _sbtl_xml_id2reg(pt_p_nodes, ui4_node_num);//change <p id='a'> to <p region='a'>
            i4_ret = sbtl_xml_get_node_time(pt_p_nodes, 0, &aui4_pts[0], &aui4_pts[1]);
            if (MMSBTLR_OK == i4_ret)
            {
                DBG_INFO(("Correct pts:%d to %d\n", pt_idx->t_base.ui4_start_pts, aui4_pts[0]));
                //if (pt_idx->t_base.ui4_start_pts == 0)
                {
                    pt_idx->t_base.ui4_start_pts = aui4_pts[0];
                }
            }
        }
    }
    return MMSBTLR_OK;
}


static INT32 parse_all_image(
    MM_SBTL_M4T_IDX_INFO_T* pt_idx,
    UINT8*                  pui1_buf, 
    UINT32                  ui4_len)
{
    INT32 i4_ret = MMSBTLR_OK;
    MM_SBTL_IMG_INFO_T  t_img_info = {0};
    UINT32              ui4_cur_pos = 0;
    UINT8*              pui1_cur = pui1_buf;
    UINT32              ui4_left_size = ui4_len;
    
    if (pt_idx   == NULL ||  
        pui1_buf == NULL ||
        ui4_len  == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    
    while (ui4_cur_pos < ui4_left_size)
    {
        ui4_left_size -= ui4_cur_pos;
        pui1_cur      += ui4_cur_pos;
        ui4_cur_pos = get_png_start(pui1_cur, 
                                    ui4_left_size);
        if (ui4_cur_pos < ui4_left_size)
        {
            i4_ret = parse_png(&t_img_info, 
                               pui1_cur + ui4_cur_pos, 
                               ui4_left_size-ui4_cur_pos);
            if (i4_ret == MMSBTLR_OK)
            {
                if ((pt_idx->ui2_imge_num % 50) == 0)
                {
                    pt_idx->pt_image_table = x_mem_realloc(
                            pt_idx->pt_image_table,
                            (pt_idx->ui2_imge_num + 50)*sizeof(MM_SBTL_IMG_INFO_T));
                }
                if (pt_idx->pt_image_table == NULL)
                {
                    i4_ret = MMSBTLR_INSUFFICIENT_MEMORY;
                    break;
                }
                pt_idx->pt_image_table[pt_idx->ui2_imge_num] = t_img_info;
                pt_idx->ui2_imge_num++;
                
                ui4_cur_pos += t_img_info.ui4_img_size;
            }
            else
            {
                ui4_cur_pos++;
            }
        }
        else
        {
            break;
        }
    }
    
    return i4_ret; 
}


static INT32 _sbtl_m4t_sidx_parse(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT8*                    pui1_buf,
    UINT32                    ui4_size,
    UINT32                    ui4_offset,
    UINT32                    ui4_file_size
    )
{
    if (pt_info  == NULL || 
        pui1_buf == NULL || 
        ui4_size == 0)
    {
        return MMSBTLR_INV_ARG;
    }
    UINT32 ui4_pos         = 0;
    UINT8  ui1_ver         = pui1_buf[0];
    UINT32 ui4_base_pts    = 0;
    UINT32 ui4_base_offset = 0;
    UINT32 ui4_count       = 0;
    UINT32 ui4_i           = 0;
    UINT32 ui4_ref_info    = 0;
    UINT32 ui4_dur         = 0;
    INT32  i4_ret          = MMSBTLR_OK;
    UINT32 ui4_time_scale  = 0;
    //ver(1) & flag (3)  | refid(4) | timescale (4) 
     // offset (4|8) | pts (4|8)  
     //
    LOADB_DWRD(pui1_buf + 8, ui4_time_scale);
    if (ui4_time_scale == 0)
    {
        ui4_time_scale = 1000;
    }
    ui4_pos += 12;
   
    if (ui1_ver)
    {
        //64bit value here just read 32
        ui4_pos += 4;
        _CHECK_OVER_RET(ui4_pos, ui4_size);
        LOADB_DWRD(pui1_buf + ui4_pos, ui4_base_pts);
        ui4_pos += 4;

        ui4_pos += 4;
        _CHECK_OVER_RET(ui4_pos, ui4_size);
        LOADB_DWRD(pui1_buf + ui4_pos, ui4_base_offset);
        ui4_pos += 4;
    }
    else
    {
        _CHECK_OVER_RET(ui4_pos, ui4_size);
        LOADB_DWRD(pui1_buf + ui4_pos, ui4_base_pts);
        ui4_pos += 4;
        _CHECK_OVER_RET(ui4_pos, ui4_size);
        LOADB_DWRD(pui1_buf + ui4_pos, ui4_base_offset);
        ui4_pos += 4;
    }
    ui4_base_offset += ui4_offset;
    if (ui4_base_offset > ui4_file_size)
    {
        return MMSBTLR_NOT_SUPPORT;
    }
    //
     _CHECK_OVER_RET(ui4_pos, ui4_size);
    LOADB_DWRD(pui1_buf + ui4_pos, ui4_count);
    ui4_count &= 0xFFFF;
    ui4_pos += 4;
    for ( ui4_i = 0; ui4_i < ui4_count; ui4_i++)
    {
         _CHECK_OVER_RET(ui4_pos, ui4_size);
        LOADB_DWRD(pui1_buf + ui4_pos, ui4_ref_info);
        ui4_pos += 4;
        if (ui4_ref_info & 0x80000000)
        {
            //this is sidx ref
            ui4_pos += 8;
            continue;
        }
        ui4_ref_info &= 0x7FFFFFFF;
        
        if (ui4_base_offset >= ui4_file_size)
        {
            break;
        }
         _CHECK_OVER_RET(ui4_pos, ui4_size);
        LOADB_DWRD(pui1_buf + ui4_pos, ui4_dur);
        ui4_pos += 4;

        i4_ret = _sbtl_m4t_add_sample(
                              pt_info,
                              (UINT32)((UINT64)ui4_base_pts*1000/ui4_time_scale),
                              (UINT32)((UINT64)ui4_dur*1000/ui4_time_scale),
                              ui4_base_offset,
                              ui4_ref_info);
        ui4_base_offset += ui4_ref_info;
        ui4_base_pts    += ui4_dur;
        //extra info
        ui4_pos += 4;
    }
    
    return i4_ret;             
}

INT32 sbtl_m4t_file_build_idx(MM_SBTL_LOAD_FILE_INFO_T* pt_info)
{
    BOOL            b_end_build = FALSE;
    INT32           i4_ret = MMSBTLR_OK;
    UINT32          ui4_cur_offset = 0;
    MEDIA_FEEDER_T* pt_feeder;
    UINT8           ui1_head_data[256] = {0};
    copybytes_fct   pf_copybytes = NULL;
    setpos_fct      pf_set_pos;
    UINT32          ui4_real_read = 0;
    UINT32          ui4_box_size = 0;
    UINT32          ui4_box_type = 0;
    UINT32          ui4_time_scale  = 0;
    UINT32          ui4_moof_offset = 0;
    UINT32          ui4_total_dur = 0;
    UINT32          ui4_inc_size = 8;
    BOOL            b_force_exit = FALSE;
    BOOL            b_has_idx    = FALSE;
    
    if (pt_info == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    pt_feeder =   &pt_info->t_feeder;
    pf_copybytes = pt_feeder->t_feeder_be_fct_tbl.pf_copybytes;
    pf_set_pos = pt_feeder->t_feeder_be_fct_tbl.pf_set_pos;
    
    if (pf_copybytes == NULL ||  pf_set_pos == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
   
    while (ui4_cur_offset < pt_info->ui4_file_size)
    {
        
        i4_ret = x_sema_lock(pt_info->h_sema, X_SEMA_OPTION_WAIT);
        
        pf_set_pos(pt_feeder, ui4_cur_offset, MEDIA_SEEK_BGN);
        
        i4_ret = pf_copybytes(pt_feeder, 
                         ui1_head_data, 
                         8, 
                         8,
                         &ui4_real_read);
        if (i4_ret != MMSBTLR_OK)
        {
            x_sema_unlock(pt_info->h_sema);
            break;
        }
        
        LOADB_DWRD(ui1_head_data,   ui4_box_size);
        LOADB_DWRD(ui1_head_data+4, ui4_box_type);

        if (ui4_cur_offset+ui4_box_size > pt_info->ui4_file_size)
        {
            x_sema_unlock(pt_info->h_sema);
            break;
        }
        
        switch (ui4_box_type)
        {
            case MP4_BOX_MOOV:
            case MP4_BOX_MVEX:
            case MP4_BOX_TRAF:
                ui4_inc_size = 8;
                break;
                
            case MP4_BOX_MVHD:
            {
                i4_ret = pf_copybytes(pt_feeder, 
                         ui1_head_data, 
                         16, 
                         16,
                         &ui4_real_read);
                if (ui4_time_scale == 0)
                {
                    LOADB_DWRD(ui1_head_data + 12, ui4_time_scale);
                }
                ui4_inc_size = ui4_box_size;
           }
           break;

           case MP4_BOX_MOOF:
           {
            //a new sample come
                if (b_has_idx)
                {
                    b_end_build = TRUE;
                }
                ui4_moof_offset = ui4_cur_offset;
                ui4_inc_size = 8;
           }
           break;
            
        
        case MP4_BOX_MFHD:
            ui4_inc_size = 16;
            break;
                     
        case MP4_BOX_TRUN:
        {
            if (ui4_box_size > 256)
            {
                DBG_ERROR(("Too long trun box size\n", ui4_box_size));
                ui4_inc_size = ui4_box_size;
                break;
            }

            i4_ret = pf_copybytes(pt_feeder, 
                         ui1_head_data,
                         256, 
                         ui4_box_size - 8, 
                         &ui4_real_read);
            if (i4_ret != 0) 
            {
                b_end_build = TRUE;
                break;
            }
            
            i4_ret = _sbtl_m4t_parse_idx( 
                                   pt_info,
                                   ui1_head_data,
                                   ui4_box_size - 8,
                                   ui4_moof_offset,
                                   &ui4_total_dur,
                                   ui4_time_scale);
            if (i4_ret != MMSBTLR_OK)
            {
                DBG_ERROR(("mem parse idx ret:%d\n", i4_ret));
            }
            ui4_inc_size = ui4_box_size;
        }
        break; 

        case MP4_BOX_SIDX:
        {
            UINT8* pui1_sidx_buffer = NULL;
            ui4_real_read = 0;
            if (ui4_box_size <= 20)
            {
                ui4_inc_size = ui4_box_size;
                break;
            }
            if (ui4_box_size <= 256)
            {
                 i4_ret = pf_copybytes(pt_feeder, 
                         ui1_head_data, 
                         ui4_box_size, 
                         ui4_box_size,
                         &ui4_real_read);
                 pui1_sidx_buffer = ui1_head_data;
            }
            else
            {
                pui1_sidx_buffer = (UINT8*) x_mem_alloc(ui4_box_size);
                if (pui1_sidx_buffer)
                {
                    i4_ret = pf_copybytes(pt_feeder, 
                         pui1_sidx_buffer, 
                         ui4_box_size, 
                         ui4_box_size,
                         &ui4_real_read);
                }
            }
            i4_ret = _sbtl_m4t_sidx_parse(
                            pt_info,
                            pui1_sidx_buffer,
                            ui4_real_read,
                            ui4_cur_offset + ui4_box_size,
                            pt_info->ui4_file_size);
            if (i4_ret != MMSBTLR_OK)
            {
                DBG_ERROR(("parse sidx ret:%d\n", i4_ret));
            }
            else
            {
                b_has_idx = TRUE;
            }
            
            if (pui1_sidx_buffer && pui1_sidx_buffer != ui1_head_data)
            {
                x_mem_free(pui1_sidx_buffer);
            }
            ui4_inc_size = ui4_box_size;
        }
        break;
        
        case MP4_BOX_FTYP: 
        case MP4_BOX_TRAK:
        case MP4_BOX_MEHD:
        case MP4_BOX_TREX:
        case MP4_BOX_MDAT:
        case MP4_BOX_MFRA:
        case MP4_BOX_TFHD:
        default:
            if(ui4_box_size == 0)
            {
                ui4_inc_size = 8;
            }
            else
            {
                ui4_inc_size = ui4_box_size;
            }

            break;
        }
        
        ui4_cur_offset += ui4_inc_size;
        b_force_exit = pt_info->b_force_exit;
        x_sema_unlock(pt_info->h_sema);
        
        //after unlock do not access any  data any more

        if (b_force_exit)
        {
            i4_ret = MMSBTLR_CAN_NOT_OP_IN_THIS_STATE;
            break;
        }
        
        if (b_end_build)
        {
            break;
        }
    }
    
    return i4_ret;
}



//OK
static INT32 _sbtl_xml_attr_iter(XML_ATTRIBUTE_T*   pt_attr,
                                    VOID*              pv_arg,
                                    INT32              i4_depth)
{
    SBTL_XML_ATTR_SEARCH_T* pt_s =(SBTL_XML_ATTR_SEARCH_T*)pv_arg;
    
    if (pt_attr && pt_s)
    {
        if (pt_attr->i4_type == pt_s->e_attr)
        {
            pt_s->b_found = TRUE;
            pt_s->ps_value = pt_attr->ps_value;
        }
        if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_WHITESPACE)
        {
            if (x_strcmp(pt_attr->ps_value, "preserve") == 0)
            {
                pt_s->b_white_space = TRUE;
            }
        }

        if ( pt_attr->i4_type == SBTL_XML_IPCC_ATTR_STYLE ||
             pt_attr->i4_type ==  SBTL_XML_IPCC_ATTR_REGION)
        {
            XML_T*        pt_cur   = NULL;
            HANDLE_T      h_handle = pt_s->h_style;
            INT32         i4_ret;
            if (pt_attr->i4_type == SBTL_XML_IPCC_ATTR_REGION)
            {
                h_handle = pt_s->h_region;
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
                                     _sbtl_xml_attr_iter,
                                     pv_arg,
                                     i4_depth + 1); 
                }
            }  
        }     
    }    
    return MMSBTLR_OK; 
}



CHAR* sbtl_xml_get_attr_extent( 
    MM_SBTL_M4T_IDX_INFO_T*     pt_idx,
    XML_T*                      pt_p,
    UINT8*                      pui1_num)
{
    SBTL_XML_ATTR_SEARCH_T t_s;
    XML_T*                 pt_span;
    CHAR*                  ps_value;
    
    if (pt_p == NULL || pui1_num == NULL)
    {
        return NULL;
    }
    x_memset(&t_s, 0, sizeof(SBTL_XML_ATTR_SEARCH_T));
    t_s.e_attr = SBTL_XML_IPCC_ATTR_FONT_EXTENT;
    t_s.h_region = pt_idx->h_region;
    t_s.h_style  = pt_idx->h_region;
    x_sbtl_xml_iterate_all_attr(pt_p,
                             _sbtl_xml_attr_iter,
                             &t_s,
                             0);
    if (t_s.b_found == FALSE &&
        pt_p->pt_parent &&
        pt_p->pt_parent->i4_type == pt_p->i4_type)
    {
        DBG_LOG_PRINT(("Parent type is:%d\n",pt_p->i4_type));
        x_sbtl_xml_iterate_all_attr(
                            pt_p->pt_parent,
                            _sbtl_xml_attr_iter,
                             &t_s,
                             0);
    }
    *pui1_num = 1;
    pt_span = x_sbtl_xml_get_first_element((INT32)SBTL_XML_ELM_SPAN, pt_p);
                         
    while(NULL != pt_span)
    { 
        XML_ATTRIBUTE_T* pt_attr =  x_sbtl_xml_get_first_attr(pt_span);
        ps_value = pt_span->ps_value;
        
        if (pt_attr && pt_attr->i4_type == SBTL_XML_IPCC_ATTR_FAKESPAN)
        {
            if (t_s.b_white_space == FALSE && (valid_string((UINT8*)ps_value) == FALSE))
            {
                pt_span = x_sbtl_xml_get_next_element((INT32)SBTL_XML_ELM_SPAN, pt_span);
                continue;
            }  
        }
        
        while (ps_value)
        {
            ps_value = x_strchr(ps_value,'\n');
            if (ps_value == NULL)
            {
                break;
            }
            (*pui1_num)++;
            ps_value++;
        }
        pt_span = x_sbtl_xml_get_next_element((INT32)SBTL_XML_ELM_SPAN, pt_span);
    } 
    
    if (t_s.b_found)
    {
        DBG_INFO(("Find lines %d\n", *pui1_num));
        DBG_INFO(("Find Extent %s\n", t_s.ps_value));
        return t_s.ps_value;
    }
    return NULL; 
}


static BOOL  _span_valid(XML_T* pt_span_node)
{
     INT32 i4_len = 0;
     if (pt_span_node)
     {
        CHAR*  ps_value = pt_span_node->ps_value;
        XML_ATTRIBUTE_T* pt_attr =  x_sbtl_xml_get_first_attr(pt_span_node);
                    
        if (pt_attr && pt_attr->i4_type != SBTL_XML_IPCC_ATTR_FAKESPAN)
        {
            return TRUE;
        }
        i4_len = x_strlen(ps_value);
        if(i4_len ==  (INT32)x_strspn(ps_value,  ( CHAR*)" \n\t\r\v"))
        {
           return FALSE;
        }
        return TRUE;
     }
     return FALSE;
}


static INT32 _get_span_attrs(
    XML_T*                  pt_span_node,
    SBTL_TTML_ELEMENT_P_T*  pt_p)
{            
    SBTL_TTML_ATTR_T*               pt_iter;
    SBTL_TTML_ELEMENT_SPAN_T*       pt_span_old = NULL ;
    SBTL_TTML_ELEMENT_SPAN_T*       pt_span = NULL;
    SBTL_TTML_ELEMENT_SPAN_T*       pt_span_cur = NULL;

    
    if(b_is_whitspace_valid == FALSE &&
       (FALSE == _span_valid(pt_span_node)))
    { 
        return MMSBTLR_OK;
    }
    pt_span = x_mem_alloc(sizeof(SBTL_TTML_ELEMENT_SPAN_T));        
    if(NULL == pt_span)
    {
        DBG_INFO(("pt_span malloc fail!\n"));
        return MMSBTLR_INV_ARG;
    }
    pt_span_cur = SLIST_FIRST(&(pt_p->t_element_span_list));   
     
    pt_span->ps_content = pt_span_node->ps_value; 
    if (pt_span_cur == NULL)/*first span*/
    {
        SLIST_INSERT_HEAD(pt_span, &(pt_p->t_element_span_list),t_element_span_link); 
    }
    else
    {
        while(pt_span_cur)
        {
            pt_span_old = pt_span_cur;
            pt_span_cur = SLIST_NEXT(pt_span_old,t_element_span_link);
        }
        SLIST_INSERT_AFTER(pt_span,pt_span_old,t_element_span_link);
    }

    SLIST_INIT(&pt_span->t_attr_list); 
         
    SLIST_FOR_EACH(pt_iter ,&t_attr_cur, t_attr_link)
    {                   
        SBTL_TTML_ATTR_T* pt_new_iter = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
        if (NULL != pt_new_iter)
        {                    
            x_memcpy(pt_new_iter,pt_iter,sizeof(SBTL_TTML_ATTR_T));
            SLIST_INSERT_HEAD(pt_new_iter,&pt_span->t_attr_list,t_attr_link);
        }
    }
    return MMSBTLR_OK;
}



INT32 ipcc_load_node(
    SBTL_TTML_ELEMENT_LIST_T*        pt_elm,
    XML_NODE_T*                      pt_xml_node,
    UINT32                           ui4_idx,
    BOOL                             b_next)
{
    XML_T*                        pt_p_idx_node;    
    XML_T*                        pt_span_node;
    SBTL_TTML_ATTR_T*             pt_iter;
    SBTL_TTML_ELEMENT_P_T*        pt_p = NULL;
    SBTL_TTML_ELEMENT_P_T*        pt_p_cur = NULL;
    SBTL_TTML_ELEMENT_P_T*        pt_p_old = NULL;
    BOOL                          b_white  = FALSE;
    
    if (pt_elm == NULL || pt_xml_node == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    
    pt_p_idx_node = x_sbtl_xml_get_element_byidx(pt_xml_node, ui4_idx);
    if (pt_p_idx_node == NULL)
    {
        return MMSBTLR_NOT_FOUND;
    }

    pt_p = x_mem_alloc(sizeof(SBTL_TTML_ELEMENT_P_T));          
    if(NULL == pt_p) 
    {        
        return MMSBTLR_INV_ARG;
    }
    
    SLIST_INIT(&t_attr_cur); 

    x_sbtl_xml_iterate_all_attr(
                     pt_p_idx_node,
                     _ipcc_comm_iter_all,
                     NULL,
                     0);
    
    pt_p_cur = SLIST_FIRST(pt_elm);   
    pt_p_old = NULL ;  
    
    if (SLIST_IS_EMPTY(pt_elm))
    {/*the first p insert to list*/
        SLIST_INSERT_HEAD(pt_p, pt_elm, t_element_p_link);
    }
    else
    {
        do
        {
            pt_p_old = pt_p_cur;
            pt_p_cur = SLIST_NEXT(pt_p_old,t_element_p_link);
          
        }while(pt_p_cur);

        SLIST_INSERT_AFTER(pt_p, pt_p_old, t_element_p_link);
    }

    SLIST_INIT(&pt_p->t_common_attr_list);
   

    SLIST_FOR_EACH(pt_iter ,&t_attr_cur, t_attr_link)
    {   
        SBTL_TTML_ATTR_T*       pt_new_iter;
        if (b_next &&
            pt_iter->e_attr != SBTL_XML_IPCC_ATTR_BEGIN)
        {
            continue;
        }
        pt_new_iter = x_mem_alloc(sizeof(SBTL_TTML_ATTR_T));
        if(NULL != pt_new_iter)
        {
            x_memcpy(pt_new_iter, pt_iter,sizeof(SBTL_TTML_ATTR_T));
            SLIST_INSERT_HEAD(pt_new_iter, &(pt_p->t_common_attr_list), t_attr_link);
        } 
        if (b_next)
        {
            /*only add begin attr*/
            break;
        }
    } 
    ipcc_free_attr();
    
    SLIST_INIT(&pt_p->t_element_span_list);

    if (b_next)
    {
        return MMSBTLR_OK;
    }
    
    pt_span_node = x_sbtl_xml_get_first_element(SBTL_XML_ELM_SPAN, pt_p_idx_node);
    b_white = b_is_whitspace_valid;
    while(pt_span_node)
    {       
         SLIST_INIT(&t_attr_cur);
         b_is_whitspace_valid = FALSE;
         x_sbtl_xml_iterate_all_attr(
                     pt_span_node,
                     _ipcc_comm_iter_all,
                     NULL,
                     0);  
         if (b_is_whitspace_valid == FALSE)
         {
            b_is_whitspace_valid = b_white;
         }
         _get_span_attrs(pt_span_node, pt_p);
         ipcc_free_attr();
         pt_span_node = x_sbtl_xml_get_next_element(SBTL_XML_ELM_SPAN, pt_span_node);    
    }
    return MMSBTLR_OK;
}

#endif /*__KERNEL__*/


