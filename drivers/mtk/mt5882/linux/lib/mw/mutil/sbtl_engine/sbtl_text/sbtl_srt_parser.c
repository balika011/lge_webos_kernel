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
 * $RCSfile: sbtl_srt_parser.c,v $
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

#include "mutil/mm_util.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_text/sbtl_text_page_inst_mngr.h"
#include "mutil/sbtl_engine/sbtl_text/sbtl_srt_parser.h"



#else /*__KERNEL__*/
#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "x_dbg.h"
#include "x_mm_common.h"
#include "x_uc_str.h"

#include "../../mm_util.h"

#include "../sbtl_dbg.h"
#include "../sbtl_struct.h"
#include "../sbtl_utils.h"
#include "../sbtl_register.h"
#include "../sbtl_state.h"

#include "sbtl_text_page_inst_mngr.h"
#include "sbtl_srt_parser.h"
#endif /*__KERNEL__*/

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define PTS_FREQ        ((UINT64)(90000))
#define TIMER_PERIOD    500

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
INT32 _srt_check_time_format(SBTL_ENGINE_T*   pt_engine,
    MM_SBTL_FEEDER_TYPE_T           e_type,
    BOOL                            b_found_total,
    UINT16*                         pui2_count,
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*  pt_private,
    BOOL                            b_UTF16);

INT32 _srt_time_stamp_sort(
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*         pt_private,
    UINT16                              ui2_order_start,
    UINT16                              ui2_order_end);
/*-----------------------------------------------------------------------------
                    data declarations, extern, static, const
 ----------------------------------------------------------------------------*/
 
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
INT32 srt_seek_time_stamp(SBTL_PAGE_INST_MNGR_T*    pt_mngr,
                          PTS_T                     t_pts_current,
                          PTS_T*                    pt_pts_start,
                          PTS_T*                    pt_pts_end,
                          BOOL*                     pb_found)
{
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*   pt_private;
    UINT16                           ui2_time_idx;
    UINT16                           ui2_idx;
    UINT16                           ui2_cur_idx;
    UINT16                           ui2_order_start;
    UINT16                           ui2_order_end;
    
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));

    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    
    if((pt_private->pt_time_stamp == NULL) || 
       (pt_private->pui2_time_order == NULL))
    {
        return MMSBTLR_INV_ARG;
    }


    if (pt_private->ui2_cur_idx == SUBTITLE_IDX_NULL)
    {
        ui2_cur_idx = 0;
    }
    else if(pt_private->ui2_cur_idx >= pt_private->ui2_time_max_cnt)
    {
        ui2_cur_idx = (UINT16)(pt_private->ui2_time_max_cnt - 1);
    }
    else
    {
        ui2_cur_idx = pt_private->ui2_cur_idx;
    }
    
    ui2_time_idx = pt_private->pui2_time_order[ui2_cur_idx];

    if(t_pts_current < pt_private->pt_time_stamp[ui2_time_idx].t_pts_s)
    {
        ui2_order_start = 0;
        ui2_order_end = ui2_cur_idx;
    }
    else
    {
        ui2_order_start = ui2_cur_idx;
        ui2_order_end = (UINT16)(pt_private->ui2_time_max_cnt -1) ;
    }

    for(ui2_idx = ui2_order_start; ui2_idx <= ui2_order_end; ui2_idx ++)
    {
        ui2_time_idx = pt_private->pui2_time_order[ui2_idx];
        if(t_pts_current < pt_private->pt_time_stamp[ui2_time_idx].t_pts_e)
        {
            *pb_found = TRUE;
            break;
        }
    }

    if(*pb_found == TRUE)
    {
         pt_private->ui2_cur_idx = ui2_idx;

         ui2_time_idx = pt_private->pui2_time_order[pt_private->ui2_cur_idx];
         *pt_pts_start = pt_private->pt_time_stamp[ui2_time_idx].t_pts_s;
         *pt_pts_end= pt_private->pt_time_stamp[ui2_time_idx].t_pts_e;
         
         return MMSBTLR_OK;
        
    }
    else
    {
        return MMSBTLR_EOT;
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
INT32 srt_load_string(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
    UINT16                          ui2_idx = 0;
    UINT16                          ui2_tmp;
    INT32                           i4_ret;
    MM_SBTL_ENCODING_T              e_encoding;
    UINT16                          ui2_time_idx;
    UINT16                          ui2_cur_idx;
    
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*  pt_private;
    UINT32                          ui4_read_len;
    UINT16                          ui2_pos = 0;
    UINT16                          ui2_state = 0;
    UINT16                          ui2_rev = 0;
    BOOL                            b_empty = TRUE;
    enum
    {
        _SBTL_SRT_LINE = 0,
        _SBTL_SRT_CHAR,
        _SBTL_SRT_TOK,
        _SBTL_SRT_UNDERLINE,
        _SBTL_SRT_ITALIC,
        _SBTL_SRT_NOMEAN
    };
    
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));

    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    ASSERT(pt_private, ("pt_private=NULL\n"));
    
    e_encoding = pt_mngr->pt_engine->t_cfg.e_encoding;
    
    x_memset(pt_private->aui1_text_buf,0,sizeof(UINT8)*MM_SBTL_CACHE_MAX);
    x_memset(pt_private->w2s_text,0,sizeof(UINT16)*MM_SBTL_CACHE_MAX);

    ui2_cur_idx = pt_private->ui2_cur_idx;
    ui2_time_idx = pt_private->pui2_time_order[ui2_cur_idx];
    
    
    if (ui2_time_idx+1 < pt_private->ui2_time_max_cnt)
    {
        ui4_read_len = pt_private->pt_time_stamp[ui2_time_idx+1].ui4_offset -
                       pt_private->pt_time_stamp[ui2_time_idx].ui4_offset;
		if (e_encoding == MM_SBTL_ENCODE_UTF16)
	    {
	        ui4_read_len >>= 1;
	    }
    }
    else
    {
        ui4_read_len = MM_SBTL_CACHE_MAX;
    }
    if (ui4_read_len > MM_SBTL_CACHE_MAX)
    {
        ui4_read_len = MM_SBTL_CACHE_MAX;
    }
    
    sbtl_feeder_setpos(pt_mngr->pt_engine, 
                      (INT64)pt_private->pt_time_stamp[ui2_time_idx].ui4_offset, 
                       MEDIA_SEEK_BGN,
                       MM_SBTL_FEEDER_SUBTITLE);
    ui2_tmp = 0;
    
    for (ui2_idx = 0; ui2_idx < ui4_read_len; ui2_idx++)
    {
        if (e_encoding == MM_SBTL_ENCODE_UTF16)
        {
            i4_ret = sbtl_feeder_inp2byte(pt_mngr->pt_engine,
                                 &ui2_tmp,
                                 MM_SBTL_FEEDER_SUBTITLE);
        }
        else
        {
            i4_ret = sbtl_feeder_inp1byte(pt_mngr->pt_engine,
                                 (UINT8*)&ui2_tmp,
                                 MM_SBTL_FEEDER_SUBTITLE);
        }
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
        
        if (ui2_tmp == 0x0D0A)
        {
            ui2_tmp = 0x0A;
        }

        if(ui2_tmp == 0x00)
        {
            continue;
        }
        
        if (ui2_state & MAKE_BIT_MASK_16(_SBTL_SRT_LINE))
        {
            if (ui2_tmp != 0x0A)
            {
                ui2_state &=~MAKE_BIT_MASK_16(_SBTL_SRT_LINE);
            }
            else
            {
              if (e_encoding == MM_SBTL_ENCODE_UTF16)
                {
                    pt_private->w2s_text[ui2_pos] = 0x0A;
                }
                else
                {
                    pt_private->aui1_text_buf[ui2_pos] = 0x0A;
                }
                ui2_pos++;
            }
        }
        
        if (ui2_tmp == 0x0A)
        {
            if (ui2_state & MAKE_BIT_MASK_16(_SBTL_SRT_CHAR))
            {
                ui2_state &=~MAKE_BIT_MASK_16(_SBTL_SRT_CHAR);
                continue;/*skip*/
            }
            if (ui2_state & MAKE_BIT_MASK_16(_SBTL_SRT_LINE))
            {
                if (ui2_idx > 1 && (b_empty == FALSE)) 
                {
                    if (ui2_pos > 2) ui2_pos -= 2;
                    break;  
                }
                continue;
            }
            else if (ui2_idx > 0) break;
        }
        
        if (ui2_tmp == 0x0D)
        {
            ui2_state |= MAKE_BIT_MASK_16(_SBTL_SRT_LINE);
            continue;
        }
        
        if (ui2_tmp == '<')
        {
            ui2_state |= MAKE_BIT_MASK_16(_SBTL_SRT_TOK);
            ui2_state &= ~(MAKE_BIT_MASK_16(_SBTL_SRT_UNDERLINE)|
                           MAKE_BIT_MASK_16(_SBTL_SRT_ITALIC)|
                           MAKE_BIT_MASK_16(_SBTL_SRT_NOMEAN));
            ui2_rev = 0;
        }
        else if (ui2_state & MAKE_BIT_MASK_16(_SBTL_SRT_TOK))
        {
            ui2_rev++;
            switch (ui2_tmp)
            {
            case 'u':
                ui2_state |= MAKE_BIT_MASK_16(_SBTL_SRT_UNDERLINE);
                break;

            case 'i':
                ui2_state |= MAKE_BIT_MASK_16(_SBTL_SRT_ITALIC);
                break;
                
            case '>':
                 ui2_state &=~MAKE_BIT_MASK_16(_SBTL_SRT_TOK);
                 ui2_pos -= ui2_rev;
                 if ((ui2_state & MAKE_BIT_MASK_16(_SBTL_SRT_NOMEAN)) == 0)
                 {
                    DBG_INFO(("<ENTER> %x\n",ui2_state));
                    if (ui2_state & MAKE_BIT_MASK_16(_SBTL_SRT_UNDERLINE))
                    {
                        SBTL_SET_FONT_STYLE(pt_mngr->pt_engine,FE_FNT_STYLE_UNDERLINE);
                    }
                    if (ui2_state & MAKE_BIT_MASK_16(_SBTL_SRT_ITALIC))
                    {
                        SBTL_SET_FONT_STYLE(pt_mngr->pt_engine,FE_FNT_STYLE_ITALIC);
                    }
                    SBTL_SET_FONT_STYLE_FLAG(pt_mngr->pt_engine, TRUE);
                    
                 }
				 continue;
                 
            default:
                ui2_state |= MAKE_BIT_MASK_16(_SBTL_SRT_NOMEAN);
                break;
            }
        }
        
        if (ui2_tmp != ' ' && ui2_tmp != '\t')
        {
            ui2_state |= MAKE_BIT_MASK_16(_SBTL_SRT_CHAR);
            b_empty   = FALSE;
        }
        
        if (e_encoding == MM_SBTL_ENCODE_UTF16)
        {
            pt_private->w2s_text[ui2_pos] = ui2_tmp;
        }
        else
        {
            pt_private->aui1_text_buf[ui2_pos] = (UINT8)ui2_tmp;
        }
        ui2_pos++;
        
    }
    if (e_encoding == MM_SBTL_ENCODE_UTF16)
    {
        pt_private->w2s_text[ui2_pos] = 0;
    }
    else
    {
        pt_private->aui1_text_buf[ui2_pos] = 0;
        if (ui2_pos < MM_SBTL_CACHE_MAX-1)
        {
            pt_private->aui1_text_buf[ui2_pos+1] = 0;
        }
    }
    
    DBG_INFO(("==== Length is %d from pos %d===\r\n", ui4_read_len, pt_private->pt_time_stamp[ui2_time_idx].ui4_offset));

    if(e_encoding != MM_SBTL_ENCODE_UTF16)
    {
        sbtl_trans_to_w2s_ex(e_encoding,pt_private->aui1_text_buf,MM_SBTL_CACHE_MAX,pt_private->w2s_text);
        DBG_INFO(("%s",pt_private->aui1_text_buf));
    }
    else
    {
        UINT16 i;
        for (i=0; i < MM_SBTL_CACHE_MAX; i++)
        {
            if (pt_private->w2s_text[i]== 0)break;
            if (pt_private->w2s_text[i]<128)
            {
                DBG_INFO(("%c",pt_private->w2s_text[i]));
            }
            else
            {
                DBG_INFO((" %x",pt_private->w2s_text[i]));
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
UINT32 srt_timer_period(SBTL_PAGE_INST_MNGR_T*      pt_mngr)
{
    ASSERT(pt_mngr->pt_engine->t_cfg.i4_speed > 0, ("pt_mngr->pt_engine.t_cfg.i4_speed=0\n"));

    return (UINT32)((TIMER_PERIOD * 100)/pt_mngr->pt_engine->t_cfg.i4_speed);
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
INT32 srt_init(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
    INT32                           i4_ret;
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*  pt_private;
    UINT16                          ui2_found_cnt;
       
    ASSERT(pt_mngr, ("pt_mngr=NULL\n"));
    pt_private = (SBTL_TEXT_PAGE_MNGR_PRIVATE_T*)pt_mngr->pv_private;
    if(pt_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }
    pt_private->ui2_time_max_cnt = 0;
    i4_ret = _srt_check_time_format(pt_mngr->pt_engine, 
                                    MM_SBTL_FEEDER_SUBTITLE, 
                                    TRUE, 
                                    &ui2_found_cnt, 
                                    pt_private, 
                                    pt_private->b_UTF16);
    if (i4_ret != MMSBTLR_OK || ui2_found_cnt == 0)
    {
        return MMSBTLR_NOT_FOUND;
    }
    pt_private->ui2_time_max_cnt = ui2_found_cnt;
    _srt_time_stamp_sort(pt_private, 0, ui2_found_cnt);
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
INT32 srt_deinit(SBTL_PAGE_INST_MNGR_T* pt_mngr)
{
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
VOID srt_start(SBTL_PAGE_INST_MNGR_T*   pt_mngr)
{
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
VOID srt_stop(SBTL_PAGE_INST_MNGR_T*    pt_mngr)
{
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
/*
00:00:05,892 --> 00:00:10,730
*/
BOOL srt_parser_verify(SBTL_ENGINE_T*   pt_engine)
{
    INT32   i4_ret;
    UINT16  ui2_found_cnt = 0;
    BOOL    b_UTF16;
    UINT8   ui1_tmp;
        
    ASSERT(pt_engine, ("pt_engine=NULL\n"));

    b_UTF16 = FALSE;
    
    sbtl_feeder_setpos(pt_engine, 0, MEDIA_SEEK_BGN, MM_SBTL_FEEDER_SUBTITLE);
    sbtl_feeder_inpnbyte(pt_engine, 1, &ui1_tmp, NULL, MM_SBTL_FEEDER_SUBTITLE);
    
    if (0xFF == ui1_tmp)
    {
        sbtl_feeder_inpnbyte(pt_engine, 1, &ui1_tmp, NULL, MM_SBTL_FEEDER_SUBTITLE);
        if (0xFE == ui1_tmp)
        {
            b_UTF16 = TRUE;
        }
    }

    sbtl_feeder_setpos(pt_engine, 0, MEDIA_SEEK_BGN, MM_SBTL_FEEDER_SUBTITLE);
    
    i4_ret = _srt_check_time_format(pt_engine, MM_SBTL_FEEDER_SUBTITLE, FALSE, &ui2_found_cnt, NULL, b_UTF16);
    
    if(i4_ret == MMSBTLR_OK && ui2_found_cnt == 1)
    {
      return TRUE;
    }
    
    return FALSE;
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

INT32 _srt_check_time_format(SBTL_ENGINE_T*   pt_engine,
    MM_SBTL_FEEDER_TYPE_T           e_type,
    BOOL                            b_found_total,
    UINT16*                         pui2_count,
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*  pt_private,
    BOOL                            b_UTF16)
{
    
    UINT8      s_str[128];
    UINT16     ui2_num = 0;
    UINT16     ui2_idx = 0;
    UINT16     ui2_i;
    UINT8      ui1_state = 0;
    UINT8      ui1_sep = 0;
    UINT8      ui1_tmp;
    UINT16     ui2_tmp;
    UINT16     aui2_time[4];
    BOOL       b_found = FALSE;
    UINT32     ui4_end_time = 0;
    UINT32     ui4_start_time = 0;
    UINT32     ui4_time;
    INT32      i4_ret = MMSBTLR_OK;
    UINT32     ui4_read_byte = 0;
    SIZE_T     z_alloc;
    if(NULL == pui2_count)
    {
        return MMSBTLR_INV_ARG;
    }
    
    ASSERT(pt_engine, ("pt_engine=NULL\n"));
    
    if (b_found_total == TRUE)
    {
          z_alloc = sizeof(SBTL_TAG_INFO_T) * 1000;/*increase by 1000*/
          pt_private->pt_time_stamp = (SBTL_TAG_INFO_T*)x_mem_alloc(z_alloc);  
          if(pt_private->pt_time_stamp == NULL)
          {
              return MMSBTLR_INSUFFICIENT_MEMORY;
          }  
    }
    
    while(1)
    {
        //readline();
        //skipspace();
        //get_timecode();
        if (b_UTF16)
        {
            i4_ret = sbtl_feeder_inp2byte(pt_engine,
                                 &ui2_tmp,
                                 MM_SBTL_FEEDER_SUBTITLE);
            ui1_tmp = (ui2_tmp>>8);
            if (ui1_tmp == 0x0A)
            {
                ui2_tmp = 0x0A;
            }
            else
            {
                ui1_tmp = (UINT8)ui2_tmp;
            }
            ui4_read_byte += 2;
        }
        else
        {
            i4_ret = sbtl_feeder_inp1byte(pt_engine,
                                 (UINT8*)&ui1_tmp,
                                 MM_SBTL_FEEDER_SUBTITLE);
            ui4_read_byte += 1;
        }
        if (i4_ret != MMSBTLR_OK)
        {
            break;
        }
        
        if (ui1_tmp == 0x0A)
        {
             ui1_tmp = 0;
        }
        if (ui2_idx < 127)
        {
            s_str[ui2_idx] = ui1_tmp;
        }
        else
        {
            s_str[127] = 0;
        }
        ui2_idx++;
        
        if (ui1_tmp != 0)
        {
            continue;
        } 
        // has one empty line
        
        if (b_found)
        {
            BOOL b_empty = TRUE;
            for (ui2_i = 0; ui2_i < 128; ui2_i++)
            {
                if (s_str[ui2_i] == 0)
                {
                    break;
                }
                if (x_strchr("\r\n \t", (CHAR)s_str[ui2_i]) == NULL)
                {
                    b_empty = FALSE;
                }     
            }
            if (b_empty == FALSE)
            {
                ui2_idx = 0;
                continue;
            }
        }
        //try to get timecode
        
        ui1_state = 0;
        ui1_sep   = 0;
        b_found   = FALSE;
        ui4_time  = 0;
        x_memset(&aui2_time, 0, 4*sizeof(UINT16));
        
        for (ui2_i = 0; ui2_i < 128; ui2_i++)/*00:00:00,000 */
        {
            if (s_str[ui2_i] >= '0' && s_str[ui2_i] <= '9')
            {
                if (ui1_sep < 4)
                {
                    aui2_time[ui1_sep] *=10;
                    aui2_time[ui1_sep] +=s_str[ui2_i]-'0';
                }
            }
            else
            {
                if (s_str[ui2_i] != ':' && s_str[ui2_i] != ',')
                {
                    if (ui1_sep == 3 && ui4_time == 0)
                    {
                        ui4_time = 1000*(aui2_time[0]*3600+aui2_time[1]*60+aui2_time[2]);
                        ui4_time += aui2_time[3];
                        if (ui1_state)
                        {
                            ui4_end_time = ui4_time;
                            b_found = TRUE;
                            break;
                        }
                        else
                        {
                            ui4_start_time = ui4_time;
                        }
                    }
                    if (s_str[ui2_i] == ' ' || s_str[ui2_i] == '\t')
                    {
                    }
                    else if (s_str[ui2_i] == '-')
                    {
                        if(ui2_i >= 126)
                        {
                            break;
                        }
                        if (ui2_i+10 < ui2_idx    &&
                            s_str[ui2_i+1] == '-' &&
                            s_str[ui2_i+2] == '>')
                        {
                            ui1_state = 1;
                            ui1_sep   = 0;
                            ui4_time = 0;
                            x_memset(&aui2_time, 0, 4*sizeof(UINT16));
                            ui2_i+=2;
                        }
                        else
                        {
                            break;
                        } 
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    ui1_sep++;
                    if (ui1_sep > 3)break;
                }
                
            }
              
        }
        if (b_found)
        {
            if(b_found_total == FALSE)
            {
                *pui2_count = 1;
                return MMSBTLR_OK;
            }
            if(b_found_total == TRUE && pt_private != NULL)/*get information*/
            {
                   if (ui2_num >= 1000 && ui2_num%1000 == 0)/*increase by 1000*/
                   {
                       DBG_INFO(("[Realloc]\n"));
                       z_alloc = sizeof(SBTL_TAG_INFO_T)*(ui2_num/1000+1)*1000;
                       pt_private->pt_time_stamp = (SBTL_TAG_INFO_T*)x_mem_realloc(
                                                        pt_private->pt_time_stamp,
                                                        z_alloc);  
                       if(pt_private->pt_time_stamp == NULL)
                       {
                           return MMSBTLR_INSUFFICIENT_MEMORY;
                       }
                   }
          
                    pt_private->pt_time_stamp[ui2_num].t_pts_s= 
                        (UINT64)ui4_start_time*(PTS_FREQ/1000);
                    pt_private->pt_time_stamp[ui2_num].t_pts_e= 
                        (UINT64)ui4_end_time*(PTS_FREQ/1000);
                    sbtl_feeder_getpos(pt_engine, (UINT64*)&pt_private->pt_time_stamp[ui2_num].ui4_offset, e_type);
                    if (b_UTF16)
                    {
                        pt_private->pt_time_stamp[ui2_num].ui4_offset -= (ui2_idx-ui2_i)*2;
                    }
                    else
                    {
                        pt_private->pt_time_stamp[ui2_num].ui4_offset -= (ui2_idx-ui2_i);
                    }
                     
            }
            ui2_num++;
            if (ui2_num >= 65535U)
            {
                break;
            }
        }
        else
        {
            if (ui4_read_byte > 200 && b_found_total == FALSE)/*verify*/
            {
                *pui2_count = 0;
                return MMSBTLR_OK;
            }
        }
       
        ui2_idx = 0;
    }
    
    *pui2_count = ui2_num;
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
INT32 _srt_time_stamp_sort(
    SBTL_TEXT_PAGE_MNGR_PRIVATE_T*         pt_private,
    UINT16                              ui2_order_start,
    UINT16                              ui2_order_end)
{
    UINT16                              ui2_start;
    UINT16                              ui2_end;
    UINT16                              ui2_i,ui2_j;
    UINT16                              ui2_cur,ui2_next;
    UINT16                              ui2_tmp_end;
    SIZE_T                              z_alloc;

    if(pt_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    if ((ui2_order_end <= ui2_order_start) || (ui2_order_start > pt_private->ui2_time_max_cnt))
    {
        return MMSBTLR_INV_ARG;
    }
    z_alloc = sizeof(UINT16) * ui2_order_end;
    pt_private->pui2_time_order = (UINT16*)x_mem_alloc(z_alloc);
    if(pt_private->pui2_time_order == NULL)
    {
        return MMSBTLR_INSUFFICIENT_MEMORY;
    } 
    for (ui2_j = 0; ui2_j < ui2_order_end; ui2_j++)
    {
        pt_private->pui2_time_order[ui2_j] = ui2_j;
    }
    

    ui2_start = ui2_order_start;
    if(ui2_order_end > pt_private->ui2_time_max_cnt)
    {
        ui2_end = pt_private->ui2_time_max_cnt;
    }
    else
    {
        ui2_end = ui2_order_end;
    }
    
    for(ui2_j = 0; ui2_j < (ui2_end - ui2_start); ui2_j ++)
    {
        BOOL fg_finished = TRUE;
        ui2_tmp_end = ui2_end -ui2_j;
        ui2_tmp_end --;

        for(ui2_i = ui2_start; ui2_i < ui2_tmp_end; ui2_i ++)
        {
            ui2_cur = pt_private->pui2_time_order[ui2_i];
            ui2_next = pt_private->pui2_time_order[ui2_i+1];
            if(pt_private->pt_time_stamp[ui2_cur].t_pts_s > pt_private->pt_time_stamp[ui2_next].t_pts_s)
            {
                pt_private->pui2_time_order[ui2_i] = ui2_next;
                pt_private->pui2_time_order[ui2_i+1] = ui2_cur;
                fg_finished = FALSE;
            }
        }
        if(fg_finished)
        {
            break;
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
INT32 _srt_free_time_stamp(SBTL_TEXT_PAGE_MNGR_PRIVATE_T* pt_private)
{
    if(pt_private == NULL)
    {
        return MMSBTLR_INV_ARG;
    }

    pt_private->ui2_time_max_cnt = 0;
    if(pt_private->pt_time_stamp != NULL)
    {
        x_mem_free(pt_private->pt_time_stamp);
        pt_private->pt_time_stamp = NULL;
    }

    if(pt_private->pui2_time_order != NULL)
    {
        x_mem_free(pt_private->pui2_time_order);
        pt_private->pui2_time_order = NULL;
    }

    return MMSBTLR_OK;
}

