/*----------------------------------------------------------------------------
 * No Warranty :  Except  as  may  be  otherwise  agreed  to  in  writing, no *
 * warranties  of  any  kind,  whether  express or  implied, are given by MTK *
 * with  respect  to  any  Confidential  Information  or any use thereof, and *
 * the  Confidential  Information  is  provided  on  an  "AS IS"  basis.  MTK *
 * hereby  expressly  disclaims  all  such  warranties, including any implied *
 * warranties   of  merchantability ,  non-infringement  and  fitness  for  a *
 * particular purpose and any warranties arising out of course of performance *
 * course  of dealing or usage of trade.  Parties further acknowledge that in *
 * connection  with  the Purpose, Company may, either presently and/or in the *
 * future,   instruct   MTK   to   assist  it  in  the  development  and  the *
 * implementation,  in accordance with Company's designs, of certain software *
 * relating  to  Company's  product(s)  (the  "Services").   Except as may be *
 * otherwise agreed to in writing, no warranties of any kind, whether express *
 * or  implied,  are  given by MTK with respect to the Services provided, and *
 * the  Services  are  provided  on  an  "AS  IS"  basis.   Company   further *
 * acknowledges  that  the  Services  may  contain  errors,  which testing is *
 * important  and  it  is  solely  responsible for fully testing the Services *
 * and/or   derivatives   thereof   before  they  are  used,  sublicensed  or *
 * distributed.   Should  there  be any third party action brought again MTK, *
 * arising  out  of  or  relating  to  the  Services,  Company agree to fully *
 * indemnify and hold MTK harmless.                                           *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2004, CrystalMedia Technology, Inc.
 * All rights reserved.
 * 
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.  
 *-----------------------------------------------------------------------------
 * $RCSfile: mp4_midxbuld_mem.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/3 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 0a0e5908fac0e73885ce39efe82bcd61 $ Asa
 *
 * Description: 
 *         This file contains all the media index builder public APIs  
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
#include "x_mm_common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "u_midxbuld.h"
#include "mutil/midxbuld/midxbuld.h"
#include "mutil/mm_util_input.h"
#include "mutil/mm_util_memory.h"
#include "x_dbg.h"
#include "u_dbg.h"
#include "mutil/mm_util.h"

#ifndef __NO_FM__
#include "x_fm.h"
#endif

#include "x_lnk_list.h"
#include "mp4_midxbuld.h"

#else

#include "x_os.h"
#include "x_common.h"
#include "x_mm_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "../u_midxbuld.h"
#include "../midxbuld.h"
#include "../../mm_util_input.h"
#include "../../mm_util_memory.h"
#include "x_dbg.h"
#include "u_dbg.h"

#include "../../mm_util.h"

#include "x_lnk_list.h"
#include "mp4_midxbuld.h"

#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

#define MP4_KEY_LEN_OF_SAMPLE_NUM   100
#define FM_ROOT_HANDLE              NULL_HANDLE   /**< Root handle used in FM       */
#define FM_READ_ONLY                ((UINT32) 0000)    /**<read only        */

#define PIFF_KEY_NUM                512
#define PIFF_INET_BUFF_SIZE         (15*1024)
#define PIFF_INET_MOOF_MAX_COUNT    20

#define ENABLE_CTTS                 1

#define dwBigFourCC(a,b,c,d)        ((((UINT32)a)<<24)|((b)<<16)|((c)<<8)|(d))
#define MP4_BOX_MOOF                dwBigFourCC('m', 'o', 'o', 'f')
#define MP4_BOX_MFHD                dwBigFourCC('m', 'f', 'h', 'd')
#define MP4_BOX_TRAF                dwBigFourCC('t', 'r', 'a', 'f')
#define MP4_BOX_TFHD                dwBigFourCC('t', 'f', 'h', 'd')
#define MP4_BOX_TRUN                dwBigFourCC('t', 'r', 'u', 'n')
#define MP4_BOX_MFRO                dwBigFourCC('m', 'f', 'r', 'o')
#define MP4_BOX_SENC                dwBigFourCC('s', 'e', 'n', 'c')

#define INFO_HEADER "MP4_MIDXBULD: "


INT32 ui4_get_cur_pts(UINT32 ui4_ctts_value,
                      UINT32 ui4_cur_dur_sum,
                      UINT32 ui4_timescale)
{
    INT64   i8_ctts_ofst    = 0;
    INT64   i8_cur_dur_sum  = 0;
    UINT64  ui8_time        = 0;
    UINT32  ui4_curr_pts    = 0;

    i8_ctts_ofst = (INT64)((INT32)ui4_ctts_value);
    i8_cur_dur_sum = (INT64)ui4_cur_dur_sum;
    if((i8_ctts_ofst + i8_cur_dur_sum) < 0)
    {
        ui8_time = (UINT64)i8_cur_dur_sum;
    }
    else
    {
        ui8_time = (UINT64)(i8_ctts_ofst + i8_cur_dur_sum);
    }
        
    ui8_time = ui8_time * MIDXBULD_SYSTEM_CLOCK_FREQUENCY;
        
    ui4_curr_pts = (UINT32)_mm_div64(ui8_time,
                                     ui4_timescale, 
                                     NULL); 
    return ui4_curr_pts;
}
                    
/*-----------------------------------------------------------------------------
 * Name:  piff_get_key_from_mfra
 *
 * Description: 
 *
 * Inputs:  -
 *          
 * Outputs: -
 *
 * Returns: MIDXBULDR_OK                 The API is successful. 
 *          MIDXBULDR_INTERNAL_ERR       The library initialize failed.
 ----------------------------------------------------------------------------*/                            
INT32 piff_get_key_from_mfra(MIDXBULD_OBJ_T*  pt_midxbuld_obj, MIDXBULD_KEY_INFO_T* pt_key_info)
{
    UINT64                          ui8_time_len            = 0;
    UINT64                          ui8_ofs_len             = 0; 
    UINT32                          ui4_i                   = 0;
    UINT32                          ui4_ver                 = 0;
    UINT32                          ui4_read_size           = 0;
    UINT32                          ui4_temp                = 0;
    UINT32                          ui4_tmp                 = 0;
    UINT32                          ui4_high_word           = 0;
    UINT32                          ui4_low_word            = 0;
    UINT32                          ui4_box_size            = 0;
    UINT32                          ui4_entry_num           = 0;
    INT32                           i4_ret                  = 0;   
    UINT8*                          pui1_tfra_buf           = NULL;
    UINT8*                          pui1_pstart_buf         = NULL;

    setpos_fct                      pf_set_pos              = NULL;    
    copybytes_fct                   pf_copybytes            = NULL;     
    
    MP4_IDXBULD_OBJ_T*              pt_mp4_handler          = NULL;
    MP4_IDXBULD_KEY_EXT_TBL_T*      pt_key_ext_tbl          = NULL;

    pt_mp4_handler          = pt_midxbuld_obj->pv_handler_obj;
    pf_set_pos              = pt_midxbuld_obj->t_input_fct_tbl.pf_set_pos;
    pf_copybytes            = pt_midxbuld_obj->t_input_fct_tbl.pf_copybytes;

    if((pt_mp4_handler == NULL) || (pt_mp4_handler->piff_inet_moof_info == NULL))
    {
        return MIDXBULDR_INTERNAL_ERR;
    }
    
    if( pt_mp4_handler->piff_inet_moof_info->b_is_piff_file         &&
        (!pt_mp4_handler->piff_inet_moof_info->b_is_mp4_plus_seg_file   ||
            (pt_mp4_handler->t_mln_moov_info.b_is_moov_empty &&
                pt_mp4_handler->piff_inet_moof_info->b_is_mp4_plus_seg_file)))
    {
        if (pt_mp4_handler->pt_key_ext_tbl != NULL)
        {
            _mm_util_mem_free_with_tag(pt_mp4_handler->pt_key_ext_tbl, MM_MEM_MIDX_TAG);
            pt_mp4_handler->pt_key_ext_tbl = NULL;
        }
        pt_mp4_handler->pt_key_ext_tbl = (MP4_IDXBULD_KEY_EXT_TBL_T*)
                    _mm_util_mem_alloc_with_tag(sizeof(MP4_IDXBULD_KEY_EXT_TBL_T), 
                                                MM_MEM_MIDX_TAG);
        if (pt_mp4_handler->pt_key_ext_tbl == NULL)
        {
            return MIDXBULDR_OUT_OF_MEMORY;
        }
        x_memset(pt_mp4_handler->pt_key_ext_tbl, 
                 0, 
                 sizeof(MP4_IDXBULD_KEY_EXT_TBL_T));
    }
    
    pt_key_ext_tbl = pt_mp4_handler->pt_key_ext_tbl;

    i4_ret = pf_set_pos(pt_midxbuld_obj, 
                        pt_mp4_handler->piff_inet_moof_info->ui8_mfra_box_start_pos,
                        FM_SEEK_BGN);/*to mfra box*/
    if (i4_ret != INPUTR_OK)
    {
        return MIDXBULDR_INTERNAL_ERR;
    }

    ui4_box_size = pt_mp4_handler->piff_inet_moof_info->ui4_mfra_box_size;
    pui1_tfra_buf = (UINT8*)_mm_util_mem_alloc_with_tag(sizeof(UINT8)*(ui4_box_size - 8), 
                                                        MM_MEM_MIDX_TAG); 
    if(pui1_tfra_buf == NULL)
    {
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pui1_tfra_buf, 0, (ui4_box_size - 8));

    /*copy mfra box data to buf*/
    i4_ret = pf_copybytes(pt_midxbuld_obj, 
                          pui1_tfra_buf,  
                          (ui4_box_size - 8), 
                          (ui4_box_size - 8), 
                          &ui4_read_size);
    if (i4_ret != INPUTR_OK)
    {
        return MIDXBULDR_INTERNAL_ERR;
    }

    pui1_pstart_buf  = pui1_tfra_buf;        
    pui1_tfra_buf   += 8;   /* ver 1 flag 3 id 4 bit 4*/
    ui4_ver          = *(pui1_tfra_buf);    
    LOADB_DWRD(pui1_tfra_buf + 8, ui4_temp);  
    ui4_tmp = (((ui4_temp&0x30)>>4)+1) + (((ui4_temp&0x0C)>>2)+1) + ((ui4_temp&0x03)+1);

    LOADB_DWRD(pui1_tfra_buf + 12, ui4_entry_num);
    pui1_tfra_buf += 16; /* to key elem */  

    if( pt_mp4_handler->piff_inet_moof_info->b_is_piff_file         &&
        (!pt_mp4_handler->piff_inet_moof_info->b_is_mp4_plus_seg_file   ||
            (pt_mp4_handler->t_mln_moov_info.b_is_moov_empty &&
                pt_mp4_handler->piff_inet_moof_info->b_is_mp4_plus_seg_file)))
    {
        pt_key_ext_tbl->pt_key_entry = (MP4_MIDXBULD_ELMT_STRM_ENTRY_T*)
            _mm_util_mem_alloc_with_tag(sizeof(MP4_MIDXBULD_ELMT_STRM_ENTRY_T) * ui4_entry_num, 
                                        MM_MEM_MIDX_TAG);
        if(pt_key_ext_tbl->pt_key_entry == NULL)
        {
            return MIDXBULDR_OUT_OF_MEMORY;
        }
        x_memset(pt_key_ext_tbl->pt_key_entry, 
                 0, 
                 sizeof(MP4_MIDXBULD_ELMT_STRM_ENTRY_T)* ui4_entry_num); 
        
        pt_key_ext_tbl->pt_key_ext = (MP4_IDXBULD_KEY_EXT_T*)
            _mm_util_mem_alloc_with_tag(sizeof(MP4_IDXBULD_KEY_EXT_T) * ui4_entry_num, 
                                        MM_MEM_MIDX_TAG);
        if (pt_key_ext_tbl->pt_key_ext == NULL)
        {
            return MIDXBULDR_OUT_OF_MEMORY;
        }
        x_memset(pt_key_ext_tbl->pt_key_ext, 
                 0, 
                 sizeof(MP4_IDXBULD_KEY_EXT_T)* ui4_entry_num);
        pt_key_ext_tbl->ui4_key_num = 0;
        ui4_i = 0;
    }
    else
    {
        pt_key_ext_tbl->ui4_key_num = pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_key_cnt;
        ui4_i = pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_key_cnt;
    }   
        
    for(; ui4_i < ui4_entry_num + pt_key_ext_tbl->ui4_key_num; ui4_i++)
    {
        if(ui4_ver == 1)
        {
            LOADB_DWRD(pui1_tfra_buf, ui4_high_word);             
            LOADB_DWRD(pui1_tfra_buf + 4, ui4_low_word);
            ui8_time_len = ((UINT64)ui4_high_word << 32) | ui4_low_word;
            
            LOADB_DWRD(pui1_tfra_buf + 8, ui4_high_word); 
            LOADB_DWRD(pui1_tfra_buf + 12, ui4_low_word);
            ui8_ofs_len = ((UINT64)ui4_high_word << 32) | ui4_low_word;                
        }
        else if(ui4_ver == 0)
        {
            LOADB_DWRD(pui1_tfra_buf, ui8_time_len);
            LOADB_DWRD(pui1_tfra_buf + 4, ui4_low_word);  
            ui8_ofs_len = ui4_low_word;
        }

        pt_key_ext_tbl->pt_key_entry[ui4_i].ui4_pts =  ui4_get_cur_pts(0,
                                                                       (UINT32)ui8_time_len,
                                                                       pt_key_info->ui4_scale);
        pt_key_ext_tbl->pt_key_entry[ui4_i].ui8_relative_offset     = ui8_ofs_len;
        pt_key_ext_tbl->pt_key_ext[ui4_i].ui8_key_moof_ofst         = ui8_ofs_len;
        pt_key_ext_tbl->pt_key_ext[ui4_i].ui8_key_ext_duration_sum  = ui8_time_len;
        
        if(ui4_ver == 1)
        {
             pui1_tfra_buf += 16;   
        }
        else if(ui4_ver == 0)
        {
             pui1_tfra_buf += 8; 
        }
       
        pui1_tfra_buf += ui4_tmp;
    }  
    pt_key_ext_tbl->ui4_key_num += ui4_entry_num;
    pui1_tfra_buf = pui1_pstart_buf;
    if(pui1_tfra_buf != NULL)
    {
        _mm_util_mem_free_with_tag(pui1_tfra_buf, MM_MEM_MIDX_TAG);     
        /*_mp4_midxbuld_mem_free(pt_midxbuld_obj, pui1_tfra_buf);*/
        pui1_tfra_buf = NULL;
        pui1_pstart_buf = NULL;
    }

    return MIDXBULDR_OK;    
}


/*-----------------------------------------------------------------------------
 * Name:  mp4_midxbuld_key_frame_table_alloc
 *
 * Description: This function initialize key frame table obj for indexing
 *
 * Inputs:  
 *         pt_key_info:     specifies key frame table
 *         pv_obj:          specify the media info object  
 *
 * Outputs: NULL 
 *
 * Returns: MIDXBULDR_OK    Success. The routine was successful and 
 *                                   the operation has been completed.
 *          Non-zero values: Fail
 ----------------------------------------------------------------------------*/
INT32 piff_midxbuld_key_frame_table_alloc(MIDXBULD_KEY_INFO_T* pt_key_info, 
                                          VOID*                pv_obj)
{
    INT32                           i4_ret                  = 0; 
    MIDXBULD_ELMT_TBL_DESCRIB_T     t_describ;    
    MIDXBULD_OBJ_T*                 pt_midxbuld_obj         = NULL;    
    MP4_IDXBULD_OBJ_T*              pt_mp4_handler          = NULL;
    MP4_IDXBULD_RANGE_OBJ_T*        pt_range_obj            = NULL;
    MP4_IDXBULD_ELMT_TBL_OBJ_T*     pt_tbl_obj              = NULL;    
    MIDXBULD_ELMT_STRM_ENTRY_T*     pt_idx_entry            = NULL;
    
    pt_midxbuld_obj     = (MIDXBULD_OBJ_T*) pv_obj;  
    pt_mp4_handler      = pt_midxbuld_obj->pv_handler_obj;
   
    if(pt_mp4_handler->piff_inet_moof_info->b_is_mfra_key_exist)
    {   
        i4_ret = piff_get_key_from_mfra(pt_midxbuld_obj, pt_key_info);
    }
    if(i4_ret != MIDXBULDR_OK)
    {
        return i4_ret;
    }
    
    pt_key_info->ui4_range_id   = MIDXBULD_NULL_ID;
    pt_key_info->ui4_tbl_id     = MIDXBULD_NULL_ID;
    /* allocate elmt tbl range obj memory */
    pt_range_obj = (MP4_IDXBULD_RANGE_OBJ_T*) 
            _mm_util_mem_alloc_with_tag(sizeof(MP4_IDXBULD_RANGE_OBJ_T), 
                                        MM_MEM_MIDX_TAG);
    if (pt_range_obj == NULL)
    {
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    if (TRUE != _mp4_midxbuld_alloc_range_id(pt_midxbuld_obj, 
                                            &pt_key_info->ui4_range_id))
    {
        _mm_util_mem_free_with_tag(pt_range_obj, MM_MEM_MIDX_TAG);
        return MIDXBULDR_OUT_OF_HANDLE;
    }
    pt_mp4_handler->pt_key_range_obj = pt_range_obj;    
    x_memset(pt_range_obj, 0, sizeof(MP4_IDXBULD_RANGE_OBJ_T));
    
    pt_range_obj->e_tbl_type                    = MIDXBULD_TBL_TYPE_ELMT_STRM;
    pt_range_obj->u_range.t_es.ui4_start_pts    = 0;
    pt_range_obj->u_range.t_es.ui4_end_pts      = MIDXBULD_IDX_RANGE_FULL;
    pt_range_obj->ui4_range_id                  = pt_key_info->ui4_range_id;
    pt_range_obj->b_is_key_frame                = TRUE;
    pt_range_obj->b_index_is_valid              = FALSE;
    pt_range_obj->ui1_tbl_total                 = 0;

    t_describ.u_info.t_es.e_es_type             = MIDXBULD_ELEM_STRM_TYPE_KEY;
    t_describ.u_info.t_es.ui4_strm_id           = pt_key_info->ui4_strm_id;
    /* here scale and rate are used to caculate frame rate*/
    t_describ.u_info.t_es.ui4_scale             = pt_key_info->ui4_scale;
    t_describ.u_info.t_es.ui4_rate              = pt_key_info->ui4_rate;
    t_describ.u_info.t_es.ui4_sample_sz         = pt_key_info->ui4_sample_sz;
    t_describ.u_info.t_es.ui4_avg_bytes_per_sec = pt_key_info->ui4_avg_bytes_per_sec;

    /* allocate elmt tbl obj memory */
    pt_tbl_obj = (MP4_IDXBULD_ELMT_TBL_OBJ_T*) 
                _mm_util_mem_alloc_with_tag(sizeof(MP4_IDXBULD_ELMT_TBL_OBJ_T), 
                                            MM_MEM_MIDX_TAG);
    if (pt_tbl_obj == NULL)
    {
        _mp4_midxbuld_free_range_obj(pt_midxbuld_obj, pt_key_info->ui4_range_id);
        pt_key_info->ui4_range_id = MIDXBULD_NULL_ID;
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    if (TRUE != _mp4_midxbuld_alloc_table_id(pt_range_obj, &pt_key_info->ui4_tbl_id))
    {
        _mm_util_mem_free_with_tag(pt_tbl_obj, MM_MEM_MIDX_TAG);
        _mp4_midxbuld_free_range_obj(pt_midxbuld_obj, pt_key_info->ui4_range_id);
        pt_key_info->ui4_range_id = MIDXBULD_NULL_ID;
        return MIDXBULDR_OUT_OF_HANDLE;
    }
    pt_range_obj->pt_tbl_obj = pt_tbl_obj;    
    x_memset(pt_tbl_obj, 0, sizeof(MP4_IDXBULD_ELMT_TBL_OBJ_T));
    pt_tbl_obj->t_elmt_tbl.ui8_base_offset      = 0;
    pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry  = 0;
    pt_tbl_obj->t_elmt_tbl.e_tbl_type           = MIDXBULD_TBL_TYPE_ELMT_STRM;

    //refer to state, just to estimate the key tbl size
    if(pt_mp4_handler->piff_inet_moof_info->b_is_mfra_key_exist)
    {
        pt_tbl_obj->ui4_tbl_size = pt_mp4_handler->pt_key_ext_tbl->ui4_key_num; 
    }
    else
    {
        pt_tbl_obj->ui4_tbl_size = 0; 
    }   
    pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info = t_describ.u_info.t_es;
    if(pt_tbl_obj->ui4_tbl_size == 0)/*no key table exist*/
    {
        pt_tbl_obj->ui1_is_key_exist = 0;
        pt_idx_entry = NULL;
    }
    else
    {  
        pt_tbl_obj->ui1_is_key_exist = 1;
        pt_idx_entry = (MIDXBULD_ELMT_STRM_ENTRY_T*) 
                _mm_util_mem_alloc_with_tag(sizeof(MIDXBULD_ELMT_STRM_ENTRY_T)
                        * pt_tbl_obj->ui4_tbl_size, 
                        MM_MEM_MIDX_TAG);
        if (pt_idx_entry == NULL)
        {
            pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry = NULL;
            _mp4_midxbuld_free_range_obj(pt_midxbuld_obj, pt_key_info->ui4_range_id);
            pt_key_info->ui4_range_id = MIDXBULD_NULL_ID;
            pt_key_info->ui4_tbl_id = MIDXBULD_NULL_ID;
            return MIDXBULDR_OUT_OF_MEMORY;
        }
        x_memset(pt_idx_entry,
                0, 
                sizeof(MIDXBULD_ELMT_STRM_ENTRY_T) * pt_tbl_obj->ui4_tbl_size);
    }    
    pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry = pt_idx_entry;
    pt_tbl_obj->ui4_tbl_id = pt_key_info->ui4_tbl_id;
    pt_tbl_obj->pt_next = NULL;    
    pt_mp4_handler->pt_curr_range_obj = pt_range_obj;

    return MIDXBULDR_OK;
}


/*-----------------------------------------------------------------------------
 * Name:  mp4_midxbuld_key_ext_table_build
 *
 * Description: This function builds key ext table
 *
 * Inputs:  pt_midxbuld_obj     specify the media index builder object  
 *          pt_key_info ,       specifies key frame info
 *
 * Outputs: NULL
 *
 * Returns: MINFOR_OK    Success. Build the key ext table successful
 *          Non-zero values: Fail
 *
 ----------------------------------------------------------------------------*/
/*INT32 piff_midxbuld_parse_inet_buff(MIDXBULD_OBJ_T* pt_midxbuld_obj, UINT8* pui1_buff, 
                                        UINT8* pui1_head_buff)
{
}*/

/*-----------------------------------------------------------------------------
 * Name:  mp4_midxbuld_key_ext_table_build
 *
 * Description: This function builds key ext table
 *
 * Inputs:  pt_midxbuld_obj     specify the media index builder object  
 *          pt_key_info ,       specifies key frame info
 *
 * Outputs: NULL
 *
 * Returns: MINFOR_OK    Success. Build the key ext table successful
 *          Non-zero values: Fail
 *
 ----------------------------------------------------------------------------*/
INT32 piff_midxbuld_key_ext_table_build(
                            MIDXBULD_OBJ_T*         pt_midxbuld_obj,
                            MIDXBULD_KEY_INFO_T*    pt_key_info)
{
    UINT32                          ui4_key_num     = 0; 
    MP4_IDXBULD_OBJ_T*              pt_mp4_handler  = NULL;
    MP4_IDXBULD_KEY_EXT_TBL_T*      pt_key_ext_tbl  = NULL;/*TBD: No use??? */
    MIDXBULD_ELMT_STRM_ENTRY_T*     pt_idx_entry    = NULL;
   
    pt_mp4_handler  = (MP4_IDXBULD_OBJ_T* )pt_midxbuld_obj->pv_handler_obj;   
    ui4_key_num     = pt_mp4_handler->pt_key_range_obj->pt_tbl_obj->ui4_tbl_size;
    if(ui4_key_num == 0)
    {
        return MIDXBULDR_OK;
    }
    
    pt_key_ext_tbl  = (MP4_IDXBULD_KEY_EXT_TBL_T*)
                _mm_util_mem_alloc_with_tag(sizeof(MP4_IDXBULD_KEY_EXT_TBL_T), 
                                            MM_MEM_MIDX_TAG);
    if(pt_key_ext_tbl == NULL)
    {
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pt_key_ext_tbl, 0, sizeof(MP4_IDXBULD_KEY_EXT_TBL_T));
    pt_key_ext_tbl->ui4_track_id    = pt_key_info->ui4_strm_id;
    pt_key_ext_tbl->ui8_base_offset = 0;
    pt_key_ext_tbl->pt_next         = NULL;    
    pt_key_ext_tbl->ui4_key_num     = ui4_key_num;
    pt_key_ext_tbl->pt_key_entry    = (MP4_MIDXBULD_ELMT_STRM_ENTRY_T*)
                 _mm_util_mem_alloc_with_tag(sizeof(MP4_MIDXBULD_ELMT_STRM_ENTRY_T)
                                            *ui4_key_num, 
                                            MM_MEM_MIDX_TAG);
    if (pt_key_ext_tbl->pt_key_entry == NULL) 
    {
        _mm_util_mem_free_with_tag(pt_key_ext_tbl, MM_MEM_MIDX_TAG);
        pt_key_ext_tbl = NULL;
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    pt_key_ext_tbl->pt_key_ext      = (MP4_IDXBULD_KEY_EXT_T*)
                _mm_util_mem_alloc_with_tag(sizeof(MP4_IDXBULD_KEY_EXT_T)
                                            *ui4_key_num, 
                                            MM_MEM_MIDX_TAG);
    if (pt_key_ext_tbl->pt_key_ext == NULL) 
    {
        _mm_util_mem_free_with_tag(pt_key_ext_tbl->pt_key_entry, MM_MEM_MIDX_TAG);
        pt_key_ext_tbl->pt_key_entry = NULL;
        _mm_util_mem_free_with_tag(pt_key_ext_tbl, MM_MEM_MIDX_TAG);
        pt_key_ext_tbl = NULL;
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pt_key_ext_tbl->pt_key_entry, 
             0, 
             sizeof(MP4_MIDXBULD_ELMT_STRM_ENTRY_T) * ui4_key_num);
    x_memset(pt_key_ext_tbl->pt_key_ext, 
             0, 
             sizeof(MP4_IDXBULD_KEY_EXT_T) * ui4_key_num); 
    /*pt_key_ext_tbl = pt_mp4_handler->pt_key_ext_tbl; *//* ???????????  */
    pt_mp4_handler->pt_key_ext_tbl = pt_key_ext_tbl;/* ???????????  */
    /*pt_key_ext_tbl->pt_key_ext = pt_mp4_handler->pt_key_ext_tbl->pt_key_ext;
    pt_key_ext_tbl->pt_key_entry = pt_mp4_handler->pt_key_ext_tbl->pt_key_entry;*/
    pt_idx_entry = (MIDXBULD_ELMT_STRM_ENTRY_T*) _mm_util_mem_alloc_with_tag(
                    sizeof(MIDXBULD_ELMT_STRM_ENTRY_T) * ui4_key_num, 
                    MM_MEM_MIDX_TAG);
    if (pt_idx_entry == NULL)
    {
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pt_idx_entry, 0, sizeof(MIDXBULD_ELMT_STRM_ENTRY_T) * ui4_key_num);
    pt_mp4_handler->pt_key_range_obj->pt_tbl_obj->t_elmt_tbl
                                    .u_fmt.t_es.pt_idx_entry = pt_idx_entry;

    return MIDXBULDR_OK;
}


/*-----------------------------------------------------------------------------
 * Name:  piff_midxbuld_elmt_tbl_size_estimate
 *
 * Description: This function builds the index table
 *
 * Inputs:  pt_midxbuld_obj     specify the media index builder object  
 *
 * Outputs: NULL
 *
 * Returns: MINFOR_OK    Success. Build the index table successful
 *          Non-zero values: Fail
 *
 ----------------------------------------------------------------------------*/
INT32 piff_midxbuld_elmt_tbl_size_estimate(MIDXBULD_OBJ_T* pt_midxbuld_obj, 
                                     MP4_IDXBULD_RANGE_OBJ_T*       pt_range_obj, 
                                     MP4_IDXBULD_ELMT_TBL_OBJ_T*    pt_curr_tbl_obj, 
                                     MIDXBULD_ELMT_TBL_DESCRIB_T*   pt_describ)
{   
    MP4_IDXBULD_OBJ_T*          pt_mp4_handler                  = NULL;

    pt_mp4_handler = (MP4_IDXBULD_OBJ_T*) pt_midxbuld_obj->pv_handler_obj;

    if (pt_range_obj->u_range.t_es.ui4_start_pts
                    >= pt_range_obj->u_range.t_es.ui4_end_pts)
    {
        return 0;
    }

    pt_curr_tbl_obj->ui4_start_pts = pt_range_obj->u_range.t_es.ui4_start_pts;  
    pt_curr_tbl_obj->ui4_end_sample= pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_sample_cnt + 1;

    if(pt_describ->u_info.t_es.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
    {
        return 1500;
    }
    else// if(pt_describ->u_info.t_es.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
    {
        return 1500;
    }    
}

/* Check whether current table enough or not to hold the full moof, if first enter and 
   not enough, need to reallocate buffer*/
BOOL piff_midxbuld_check_tbl_enough(MIDXBULD_OBJ_T* pt_midxbuld_obj,
                                    UINT8*          pui1_buff, 
                                    UINT32          ui4_buf_len)
{
    BOOL                        b_a_sd_changed  = FALSE;
    BOOL                        b_v_sd_changed  = FALSE;
    UINT32                      ui4_a_cnt       = 0;
    UINT32                      ui4_v_cnt       = 0;
    UINT32                      ui4_s_cnt       = 0;
    MIDXBULD_ELMT_STRM_TYPE_T   e_cur_strm_type = MIDXBULD_ELEM_STRM_TYPE_VIDEO;
    UINT8*                      pui1_start_buff = pui1_buff;
    MP4_IDXBULD_OBJ_T*          pt_mp4_handler  = NULL;
    MP4_IDXBULD_ELMT_TBL_OBJ_T* pt_tbl_obj      = NULL;
    
    pt_mp4_handler  = pt_midxbuld_obj->pv_handler_obj;
    pt_tbl_obj      = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj;
    
    while(pui1_buff < pui1_start_buff + ui4_buf_len)
    {
        UINT32 ui4_box_size = 0;
        UINT32 ui4_box_type = 0;
        
        LOADB_DWRD(pui1_buff,       ui4_box_size);
        LOADB_DWRD(pui1_buff + 4,   ui4_box_type);
        
        switch(ui4_box_type)
        {
        case MP4_BOX_MFHD:
            pui1_buff += 16;
            break;
        case MP4_BOX_TRAF:
            pui1_buff += 8;
            break;                
        case MP4_BOX_TFHD:
        {
            UINT8   ui1_total_strm_num  = 0;
            UINT8   ui1_idx             = 0;
            UINT32  ui4_track_id        = 0;
            UINT32  ui4_flag            = 0;
            UINT32  ui4_sd_idx          = 1;/*default index is 1 */
            
            LOADB_DWRD(pui1_buff +  8, ui4_flag);
            LOADB_DWRD(pui1_buff + 12, ui4_track_id);
            
            ui4_flag = ui4_flag & 0xFFFFFF;            
            if(ui4_flag & 0x01)/*base_data_offset_present*/
            {
                if(ui4_flag & 0x02)
                {
                    LOADB_DWRD(pui1_buff + 24, ui4_sd_idx);
                }
            }
            else if(ui4_flag & 0x02)/*sample_description_index_present*/
            {   
                LOADB_DWRD(pui1_buff + 16, ui4_sd_idx);
            }  
            
            pt_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj; 
            ui1_total_strm_num = pt_midxbuld_obj->t_src_info.ui1_total_strm_num;
            for (ui1_idx = 0; ui1_idx < ui1_total_strm_num; ui1_idx++)
            {
                if(ui1_idx >= MIDXBULD_ELMT_TBL_MAX)
                {
                    break;
                }
                if (pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_strm_id == ui4_track_id)
                {
                    if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
                    {
                        if(pt_mp4_handler->t_mln_last_range_info.ui1_v_sd_idx != (UINT8)ui4_sd_idx)
                        {
                            b_v_sd_changed = TRUE;
                        }
                        e_cur_strm_type = MIDXBULD_ELEM_STRM_TYPE_VIDEO;
                    }
                    else if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
                    {
                        if(1 != (UINT8)ui4_sd_idx)
                        {
                            b_a_sd_changed = TRUE;
                        }
                        e_cur_strm_type = MIDXBULD_ELEM_STRM_TYPE_AUDIO;
                    }
                    else if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_SUBTITLE)
                    {
                        e_cur_strm_type = MIDXBULD_ELEM_STRM_TYPE_SUBTITLE;
                    }
                    break;
                }
                pt_tbl_obj = pt_tbl_obj->pt_next;
                if(pt_tbl_obj == NULL)
                {
                    break;
                }
            }
            pui1_buff += ui4_box_size;
        }
        break;
        case MP4_BOX_TRUN:
        {
            UINT32 ui4_mem_cnt = 0;
            LOADB_DWRD(pui1_buff + 12, ui4_mem_cnt);
            if(e_cur_strm_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
            {
                ui4_v_cnt += ui4_mem_cnt;
            }
            else if(e_cur_strm_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
            {
                ui4_a_cnt += ui4_mem_cnt;
            } 
            else if(e_cur_strm_type == MIDXBULD_ELEM_STRM_TYPE_SUBTITLE)
            {
                ui4_s_cnt += ui4_mem_cnt;
            } 
            pui1_buff += ui4_box_size;
        }
        break; 
        default:
            pui1_buff += ui4_box_size;
            break;
        }
    }

    pt_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj; 
    if(pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry == 0)//first enter
    {
        if(ui4_v_cnt > pt_tbl_obj->ui4_tbl_size)
        {
            if (pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry != NULL)
            {
                _mm_util_mem_free_with_tag(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry, 
                                            MM_MEM_MIDX_TAG);
                pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry = NULL;
            }
            pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry = 
                (MIDXBULD_ELMT_STRM_ENTRY_T*) _mm_util_mem_alloc_with_tag(
                                sizeof(MIDXBULD_ELMT_STRM_ENTRY_T) * ui4_v_cnt, 
                                MM_MEM_MIDX_TAG);
            if (pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry == NULL)
            {
                return FALSE;
            }
            pt_tbl_obj->ui4_tbl_size = ui4_v_cnt;
            x_memset(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry, 
                     0, 
                     sizeof(MIDXBULD_ELMT_STRM_ENTRY_T) * pt_tbl_obj->ui4_tbl_size);
        }
        if((pt_tbl_obj->pt_next != NULL) && (ui4_a_cnt > pt_tbl_obj->pt_next->ui4_tbl_size))
        {
            if (pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.pt_idx_entry != NULL)
            {
                _mm_util_mem_free_with_tag(pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.pt_idx_entry, 
                                            MM_MEM_MIDX_TAG);
                pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.pt_idx_entry = NULL;
            }
            pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.pt_idx_entry = 
                (MIDXBULD_ELMT_STRM_ENTRY_T*) _mm_util_mem_alloc_with_tag(
                                sizeof(MIDXBULD_ELMT_STRM_ENTRY_T) * ui4_a_cnt, 
                                MM_MEM_MIDX_TAG);
            if (pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.pt_idx_entry == NULL)
            {
                return FALSE;
            }
            pt_tbl_obj->pt_next->ui4_tbl_size = ui4_a_cnt;
            x_memset(pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.pt_idx_entry, 
                     0, 
                     sizeof(MIDXBULD_ELMT_STRM_ENTRY_T) * ui4_a_cnt);
        }
        if(ui4_v_cnt < 60)/* Only two second or less each segment*/
        {
            pt_tbl_obj->ui4_tbl_size = 300;/* change to about 10 second to avoid cache issue*/
        }
    }
    else
    {
        if(b_v_sd_changed || b_a_sd_changed)
        {
            return FALSE;
        }
    }
    if(pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry + ui4_v_cnt > pt_tbl_obj->ui4_tbl_size)
    {
        return FALSE;
    }
    else if((pt_tbl_obj->pt_next != NULL) && 
            (pt_tbl_obj->pt_next->t_elmt_tbl.ui4_number_of_entry + ui4_a_cnt > pt_tbl_obj->pt_next->ui4_tbl_size))
    {
        return FALSE;
    }
            
    return TRUE;
}

/*-----------------------------------------------------------------------------
 * Name:  piff_midxbuld_indexing
 *
 * Description: This function builds the index table
 *
 * Inputs:  pt_midxbuld_obj     specify the media index builder object  
 *
 * Outputs: NULL
 *
 * Returns: MINFOR_OK    Success. Build the index table successful
 *          Non-zero values: Fail
 *
 ----------------------------------------------------------------------------*/
INT32 piff_midxbuld_indexing(   MIDXBULD_OBJ_T* pt_midxbuld_obj, 
                                UINT32*         pui4_range_id)
{ 
    UINT64                      ui8_curr_duration_sum_a     = 0;
    UINT64                      ui8_curr_duration_sum_v     = 0;
    UINT64                      ui8_curr_duration_sum_s     = 0;
    UINT32                      ui4_i                       = 0;
    UINT32                      ui4_range_start_pts         = 0;    
    UINT32                      ui4_key_num                 = 0; 
    UINT32                      ui4_start_key_idx           = 0;    
    UINT64                      ui8_curr_pts                = 0;
    UINT32                      ui4_read_size               = 0;  
    UINT64                      ui8_base_offset             = 0;
    UINT64                      ui8_range_base_offset       = 0;
    UINT32                      ui4_sd_idx                  = 0;
    UINT32                      ui4_sample_duration         = 0;
    UINT32                      ui4_dft_sam_size_of_moof_a  = 0;
    UINT32                      ui4_sample_size             = 0;
    UINT32                      ui4_sample_ctts             = 0;
    UINT32                      ui4_sample_size_sum         = 0;
    UINT32                      ui4_mem_cnt                 = 0;
    UINT32                      flag_tr                     = 0;
    UINT32                      ui4_tmp                     = 0;
    UINT64                      ui8_moof_offset             = 0;
    UINT32                      ui4_moof_size               = 0;
    UINT32                      ui4_box_size                = 0;
    UINT32                      ui4_box_type                = 0;
    INT32                       i4_ret                      = 0;  
    BOOL                        b_exit                      = FALSE;
    BOOL                        b_eof_got                   = FALSE;
    UINT8*                      pui1_buff                   = NULL;
    UINT8*                      ui1_pui1_start_buff         = NULL;

    copybytes_fct               pf_copybytes;
    setpos_fct                  pf_set_pos;   
    getpos_fct                  pf_get_pos;   
    MP4_IDXBULD_OBJ_T*          pt_mp4_handler              = NULL;
    MP4_IDXBULD_ELMT_TBL_OBJ_T* pt_tbl_obj                  = NULL;
    MIDXBULD_ELMT_STRM_ENTRY_T* pt_idx_entry                = NULL;    
    MP4_IDXBULD_KEY_EXT_TBL_T*  pt_key_ext_tbl              = NULL; 
#ifdef CENC_MARLIN_SUPPORT
    MP4_ENCBULD_ELMT_TBL_OBJ_T*    pt_enc_tbl_obj           = NULL;
    MENCBULD_ELMT_SAMPLE_ENTRY_T*  pt_sample_entry          = NULL;
    MP4_BOX_ENCRYPT_INFO_T*        pt_mp4_enc_info          = NULL;
#endif
    if (pt_midxbuld_obj == NULL)
    {
        return MIDXBULDR_INV_ARG;
    } 
    pf_copybytes                = pt_midxbuld_obj->t_input_fct_tbl.pf_copybytes;
    pf_set_pos                  = pt_midxbuld_obj->t_input_fct_tbl.pf_set_pos;
    pf_get_pos                  = pt_midxbuld_obj->t_input_fct_tbl.pf_get_pos;
    pt_mp4_handler              = pt_midxbuld_obj->pv_handler_obj;
#ifdef CENC_MARLIN_SUPPORT
    pt_mp4_enc_info             = pt_midxbuld_obj->t_src_info.t_mp4_extra_info.pt_mp4_encrypt_info;
#endif
    if (pt_mp4_handler  == NULL)
    {
        return MIDXBULDR_INV_ARG;
    }    
    pt_key_ext_tbl              = pt_mp4_handler->pt_key_ext_tbl; 
    
    /* initial local variable */
    *pui4_range_id = pt_mp4_handler->pt_curr_range_obj->ui4_range_id;    
    if (pt_mp4_handler->pt_curr_range_obj->b_index_is_valid == TRUE)
    {
        return MIDXBULDR_OK;
    }
    if (pt_midxbuld_obj->b_abort_flag == TRUE)
    {
        return MIDXBULDR_ABORT_INDEXING;
    } 

    if(!pt_mp4_handler->b_is_seg_no_key_mp4)
    {
        if (pt_mp4_handler->pt_key_range_obj->b_index_is_valid  == TRUE &&
            pt_mp4_handler->ui1_range_count == 0)
        {
            return MIDXBULDR_OK;
        }
        ui4_key_num = pt_key_ext_tbl->ui4_key_num; 
        /* key table indexing */
        if (pt_mp4_handler->pt_curr_range_obj->b_is_key_frame)
        {
            pt_tbl_obj = pt_mp4_handler->pt_key_range_obj->pt_tbl_obj;
            /* get related key ext table for key table */
           
            pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry = ui4_key_num; 
            pt_tbl_obj->t_elmt_tbl.ui8_base_offset     = pt_key_ext_tbl->ui8_base_offset;
            pt_tbl_obj->t_elmt_tbl.e_tbl_type          = MIDXBULD_TBL_TYPE_ELMT_STRM;
#if 0
            x_memcpy(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry, 
                     pt_key_ext_tbl->pt_key_entry, 
                     sizeof(MIDXBULD_ELMT_STRM_ENTRY_T)*ui4_key_num);
#else
            _mp4_reorder_key_tbl(pt_tbl_obj, pt_key_ext_tbl);
#endif
            return MIDXBULDR_OK;
        }
    }
    
    pt_mp4_handler->ui1_curr_buld_progress = 60;
    /* non-key table indexing */
    pt_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj;    
    //while(pt_tbl_obj != NULL)
    {
        if (pt_midxbuld_obj->b_abort_flag == TRUE)
        {
            return MIDXBULDR_ABORT_INDEXING;
        }
        if(pt_tbl_obj->ui4_tbl_size == 0)
        {
            pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry = 0;
        }    
        ui4_range_start_pts = pt_tbl_obj->ui4_start_pts;
                
        /* cal start key ext index by start pts */
        if(!pt_mp4_handler->b_is_seg_no_key_mp4)
        {
            for(ui4_i = ui4_key_num; ui4_i > 0; ui4_i--)
            {
                if (pt_key_ext_tbl->pt_key_entry[ui4_i-1].ui4_pts < ui4_range_start_pts)
                {
                    ui4_start_key_idx = ui4_i - 1;
                    /*if(ui4_start_key_idx <= pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_key_cnt)
                    {
                        ui4_start_key_idx = pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_key_cnt;
                    } Can't as first key may not in first moof box*/
                    break;
                }
                else if(pt_key_ext_tbl->pt_key_entry[ui4_i-1].ui4_pts == ui4_range_start_pts)
                {
                    ui4_start_key_idx = ui4_i;
                    break;
                }
            }
            while(pt_key_ext_tbl->pt_key_entry[ui4_start_key_idx].ui8_relative_offset == 
                        pt_key_ext_tbl->pt_key_entry[ui4_start_key_idx-1].ui8_relative_offset)
            {
                ui4_start_key_idx--;
            }
            
            /* cal pts */
            if(ui4_key_num == 0)/*no key table exist*/
            {        
               return MIDXBULDR_INTERNAL_ERR;
            }
            else
            { 
                pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_start_pts
                            = pt_key_ext_tbl->pt_key_entry[ui4_start_key_idx].ui4_pts;
                pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_start_sample
                            = pt_key_ext_tbl->pt_key_ext[ui4_start_key_idx].ui4_key_ext_sample; 
                ui8_curr_duration_sum_v 
                            = pt_key_ext_tbl->pt_key_ext[ui4_start_key_idx].ui8_key_ext_duration_sum;       
            } 
        }
        
        pt_idx_entry = pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry;   
        if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale == 0)
        {
            return MIDXBULDR_INTERNAL_ERR;
        } 
        if(((ui4_range_start_pts >= pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_dur)        &&
            (ui4_range_start_pts <= pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_dur + 9000))   || 
            ((pt_mp4_handler->t_mln_last_range_info.ui4_range_end_pts < ui4_range_start_pts) && 
           (pt_mp4_handler->t_mln_last_range_info.ui4_range_end_pts+30000 > ui4_range_start_pts)))
        {
            if(pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos == 0)
            {/*Holmes common mp4 duration < first moof key pts, has a gap*/
                pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos = 
                    pt_mp4_handler->piff_inet_moof_info->ui8_moof_start_pos;
                pt_mp4_handler->t_mln_last_range_info.ui4_range_end_pts = 
                    pt_mp4_handler->t_mln_moov_info.ui4_mln_moov_dur;
                    
                pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_v =
                    (UINT64)pt_mp4_handler->t_mln_last_range_info.ui4_range_end_pts * 
                     pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale;
                     
                pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_v = 
                    _mm_div64(pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_v,
                              MIDXBULD_SYSTEM_CLOCK_FREQUENCY, 
                              NULL);  
                if((pt_tbl_obj->pt_next != NULL) && 
                    (pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale != 0) &&
                    (pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO))
               {
                    pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_a = 
                        _mm_div64(pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_v * 
                                  pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale,
                                  pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale, 
                                  NULL); 
                }
            }
            pf_set_pos(pt_midxbuld_obj, 
                       pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos, 
                       FM_SEEK_BGN);
#ifndef __KERNEL__
            DBG_LOG_PRINT(("---------seek to moof start: 0x%llx  range_start_pts=0x%x--------\r\n",
                        pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos,
                        (unsigned int)ui4_range_start_pts));
#endif  
            if(pt_mp4_handler->t_mln_last_range_info.ui4_range_end_pts != 0)
            {
                ui8_curr_duration_sum_v = pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_v;
            }
            pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_start_sample =
                        pt_mp4_handler->t_mln_last_range_info.ui4_end_sample_idx + 1;
        }
        else if(!pt_mp4_handler->b_is_seg_no_key_mp4)
        {/* seek case*/
            pt_mp4_handler->t_mln_last_range_info.ui1_v_sd_idx = pt_mp4_handler->piff_inet_moof_info->at_dft_sam_info[0].ui4_dft_desc_idx;
            ui8_curr_duration_sum_v = pt_key_ext_tbl->pt_key_ext[ui4_start_key_idx].ui8_key_ext_duration_sum;
#ifndef __KERNEL__
            DBG_LOG_PRINT(("---------seek to moof start: 0x%llx with keyidx=%d keypts=0x%x range_start_pts=0x%x--------\r\n",
                        pt_key_ext_tbl->pt_key_ext[ui4_start_key_idx].ui8_key_moof_ofst,
                        (int)ui4_start_key_idx,
                        (unsigned int)pt_key_ext_tbl->pt_key_entry[ui4_start_key_idx].ui4_pts,
                        (unsigned int)ui4_range_start_pts));
#endif      
            pf_set_pos(pt_midxbuld_obj, 
                       (INT64)(pt_key_ext_tbl->pt_key_ext[ui4_start_key_idx].ui8_key_moof_ofst), 
                       FM_SEEK_BGN); 
        }
        
        ui8_curr_duration_sum_a = ui8_curr_duration_sum_v;
        if((pt_tbl_obj->pt_next != NULL) && 
           (pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale != 0) &&
           (pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO))
        {
            ui8_curr_duration_sum_a = _mm_div64(ui8_curr_duration_sum_v * 
                                                    pt_tbl_obj->pt_next->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale,
                                                pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale, 
                                                NULL); 
        }
        

        while (1)
        {  
            UINT8   aui1_tmp_buf[8] = {0};
            do
            {
                i4_ret = pf_copybytes(  pt_midxbuld_obj, 
                                        aui1_tmp_buf,/* moof box size*/
                                        sizeof(UINT8)*8, 
                                        sizeof(UINT8)*8, 
                                        &ui4_read_size);
                if(i4_ret != INPUTR_OK)
                {
                    if(i4_ret == INPUTR_EOF)
                    {
                        b_exit = TRUE;
                        b_eof_got = TRUE;
                        break;
                    }
                    return MIDXBULDR_FILE_ERR;
                }
                LOADB_DWRD(aui1_tmp_buf,       ui4_box_size);
                LOADB_DWRD(aui1_tmp_buf+4,     ui4_box_type);
                if(ui4_box_type != MP4_BOX_MOOF)
                {
                    pf_set_pos(pt_midxbuld_obj, 
                               ui4_box_size-8, 
                               FM_SEEK_CUR); 
                }
            }while(ui4_box_type != MP4_BOX_MOOF);            
            if(b_exit)
            {
                break;
            }
            pf_get_pos(pt_midxbuld_obj, &(pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos));
            pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos -= 8;

#ifndef __KERNEL__
            DBG_LOG_PRINT(("--------- get moof start: 0x%llx moof_size=0x%x--------\r\n",
                        pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos,
                        (unsigned int)ui4_box_size));
#endif 
            if(MIDXBULDR_OK != _mp4_midxbuld_mem_alloc(
                                             pt_midxbuld_obj,
                                             (VOID*)&pui1_buff,
                                             sizeof(UINT8)*(ui4_box_size-8)))
            {
                return MIDXBULDR_OUT_OF_MEMORY;
            }
            ui1_pui1_start_buff = pui1_buff;  
            ui4_moof_size = ui4_box_size - 8;    
            i4_ret = pf_copybytes(  pt_midxbuld_obj, 
                                    pui1_buff,
                                    sizeof(UINT8)*ui4_moof_size, 
                                    sizeof(UINT8)*ui4_moof_size, 
                                    &ui4_read_size);
            if(i4_ret != INPUTR_OK)
            {
#ifndef __KERNEL__
            DBG_LOG_PRINT(("--------- read data fail return %d --------\r\n", (int)i4_ret));
#endif 
                if(i4_ret != INPUTR_EOF)
                {
                    return MIDXBULDR_FILE_ERR;
                }
                b_exit = TRUE;
            }
            
            pt_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj;
            if(FALSE == piff_midxbuld_check_tbl_enough( pt_midxbuld_obj,
                                                        pui1_buff, 
                                                        ui4_moof_size))
            {
                b_exit = TRUE;
                break;
            }
            
            ui4_sample_size_sum = 0;
            while(pui1_buff < ui1_pui1_start_buff + ui4_moof_size)
            {              
                LOADB_DWRD(pui1_buff,       ui4_box_size);
                LOADB_DWRD(pui1_buff + 4,   ui4_box_type);

                switch(ui4_box_type)
                {
                    case MP4_BOX_MFHD:
                        pui1_buff += 16;
                        break;
                    case MP4_BOX_TRAF:
                        pui1_buff += 8;
                        break;                
                    case MP4_BOX_TFHD:
                    {
                        UINT8   ui1_total_strm_num  = 0;
                        UINT8   ui1_idx             = 0;
                        UINT32  ui4_track_id        = 0;
                        LOADB_DWRD(pui1_buff + 8, ui4_tmp); /*ver & flag */    
                        LOADB_DWRD(pui1_buff + 12, ui4_track_id);
                        pui1_buff += 16;
                        
                        pt_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj; 
                        #ifdef CENC_MARLIN_SUPPORT
                        pt_enc_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_enc_tbl_obj;
                        #endif
                        ui1_total_strm_num = pt_midxbuld_obj->t_src_info.ui1_total_strm_num;

                        for (ui1_idx = 0; ui1_idx < ui1_total_strm_num; ui1_idx++)
                        {
                            if(ui1_idx >= MIDXBULD_ELMT_TBL_MAX)
                            {
                                return MIDXBULDR_INTERNAL_ERR;
                            }
                            if (pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_strm_id == ui4_track_id)
                            {
                                UINT8 ui1_tmp = 0;
                                for (ui1_tmp = 0; ui1_tmp < ui1_total_strm_num; ui1_tmp++)
                                {
                                    if(ui4_track_id == pt_mp4_handler->piff_inet_moof_info->at_dft_sam_info[ui1_tmp].ui4_track_id)
                                    {
                                        ui4_sample_duration = pt_mp4_handler->piff_inet_moof_info->at_dft_sam_info[ui1_tmp].ui4_dft_sample_dur;
                                        ui4_sample_size = pt_mp4_handler->piff_inet_moof_info->at_dft_sam_info[ui1_tmp].ui4_dft_sample_size;
                                        break;
                                    }
                                }
                                if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
                                {
                                    ui4_dft_sam_size_of_moof_a = ui4_sample_size;
                                }
                                pt_idx_entry = pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry + 
                                                            pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry;
                                #ifdef CENC_MARLIN_SUPPORT
                                pt_sample_entry = pt_enc_tbl_obj->t_elmt_tbl.pt_sample_entry + 
                                                            pt_enc_tbl_obj->t_elmt_tbl.ui4_number_of_entry;
                                #endif
                                break;
                            }
                            pt_tbl_obj = pt_tbl_obj->pt_next;
                            #ifdef CENC_MARLIN_SUPPORT
                            pt_enc_tbl_obj = pt_enc_tbl_obj->pt_next;
                            #endif
                            if(pt_tbl_obj == NULL)
                            {
                                break;
                            }
                        }
                        
                        flag_tr = ui4_tmp & 0xFFFFFF;
                        
                        if(flag_tr & 0x01)/*base_data_offset_present*/
                        {   
                            UINT32 ui4_high_word = 0;
                            UINT32 ui4_low_word  = 0;
                            
                            LOADB_DWRD(pui1_buff, ui4_high_word);             
                            LOADB_DWRD(pui1_buff + 4, ui4_low_word);
                            
                            ui8_moof_offset = ((UINT64)ui4_high_word << 32) | ui4_low_word;
                            if(ui8_moof_offset > (UINT64)0xA0000000)//2.5G
                            {/*video first*/
                                if(pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry == 0)
                                {
                                    UINT32 ui4_tmp = 0;
                                    
                                    ui4_tmp = ui8_moof_offset&0x7FFFFFFF;/* 2G*/
                                    pt_tbl_obj->t_elmt_tbl.ui8_base_offset = ui8_moof_offset - ui4_tmp;
                                    ui8_range_base_offset = pt_tbl_obj->t_elmt_tbl.ui8_base_offset;
                                    ui8_moof_offset = ui4_tmp;//2G
                                }
                                else if((pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO) ||
                                        (pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO) ||
                                        (pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_SUBTITLE))
                                {
                                    if(ui8_range_base_offset != 0)
                                    {
                                        ui8_moof_offset = ui8_moof_offset - ui8_range_base_offset;
                                        pt_tbl_obj->t_elmt_tbl.ui8_base_offset = ui8_range_base_offset;
                                    }
                                }
                            }
                            pui1_buff += 8;
                        }
                        else
                        {
                            ui8_moof_offset = pt_mp4_handler->t_mln_last_range_info.ui8_moof_pos;
                        }
                        
                        if(flag_tr & 0x02)/*sample_description_index_present*/
                        {   
                            LOADB_DWRD(pui1_buff, ui4_sd_idx);
 
                            /* for if sd changed, stop to build range,keep each range only one codec type*/
                            if (pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
                            {
                                pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui1_sd_idx = ui4_sd_idx;
                                pt_mp4_handler->t_mln_last_range_info.ui1_v_sd_idx  = (UINT8)ui4_sd_idx;
                            }
                            pui1_buff += 4;
                        }
                        else
                        {/* Only process video */
                            if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
                            {
                                pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui1_sd_idx = 
                                    pt_mp4_handler->piff_inet_moof_info->at_dft_sam_info[0].ui4_dft_desc_idx;
                                pt_mp4_handler->t_mln_last_range_info.ui1_v_sd_idx  = 
                                    pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui1_sd_idx;
                            }
                        }
                        
                        if(flag_tr & 0x08)/*default_sample_duration_present*/
                        {
                            LOADB_DWRD(pui1_buff, ui4_sample_duration);
                            pui1_buff += 4;
                        }
                        
                        if(flag_tr & 0x10)/*default_sample_size_present*/
                        {
                            LOADB_DWRD(pui1_buff, ui4_sample_size);
                            pui1_buff += 4;
                            /*if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
                            {
                                ui4_dft_sample_size_v = ui4_sample_size;
                            }
                            else */
                            if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
                            {
                                ui4_dft_sam_size_of_moof_a = ui4_sample_size;
                            }
                        }
                        
                        if(flag_tr & 0x20)/*default_sample_flags_present*/
                        {
                            //LOADB_DWRD(pui1_buff, ui4_sample_size);
                            pui1_buff += 4;
                        }
                    }
                    break;
                    case MP4_BOX_TRUN:
                    {
                        ui4_sample_ctts     = 0; /*reset first as no default ctts*/
                        ui4_sample_size_sum = 0;
                        ui8_base_offset     = ui8_moof_offset;
                        
                        pui1_buff += 8;
                        LOADB_DWRD(pui1_buff , ui4_tmp);
                        LOADB_DWRD(pui1_buff + 4, ui4_mem_cnt);
                        pui1_buff += 8;

                        if(ui4_sample_duration != 0)
                        {
                            if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
                            {
                                ui8_curr_pts = ui4_get_cur_pts(0,
                                                               (UINT32)ui8_curr_duration_sum_v + ui4_sample_duration*ui4_mem_cnt,
                                                               pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale);   
                                if(ui8_curr_pts < ui4_range_start_pts)
                                {
                                    ui8_curr_duration_sum_v += ui4_sample_duration*ui4_mem_cnt;
                                    pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_v = ui8_curr_duration_sum_v;
                                    pui1_buff += ui4_box_size-16;
                                    break;
                                }
                            }
                            else if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
                            {
                                ui4_sample_size = ui4_dft_sam_size_of_moof_a;
                                ui8_curr_pts = ui4_get_cur_pts(0,
                                                               (UINT32)ui8_curr_duration_sum_a+ui4_sample_duration*ui4_mem_cnt,
                                                               pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale);  
                                if(ui8_curr_pts < ui4_range_start_pts)
                                {
                                    ui8_curr_duration_sum_a += ui4_sample_duration*ui4_mem_cnt;
                                    pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_a = ui8_curr_duration_sum_a;
                                    pui1_buff += ui4_box_size-16;
                                    break;
                                }
                            } 
                            else if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_SUBTITLE)
                            {
                                ui8_curr_pts = ui4_get_cur_pts(0,
                                                               (UINT32)ui8_curr_duration_sum_a+ui4_sample_duration*ui4_mem_cnt,
                                                               pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale);  
                                if(ui8_curr_pts < ui4_range_start_pts)
                                {
                                    ui8_curr_duration_sum_s += ui4_sample_duration*ui4_mem_cnt;
                                    pui1_buff += ui4_box_size-16;
                                    break;
                                }
                            } 
                        }
                        
                        flag_tr = ui4_tmp & 0xFFFFFF;
                        if(flag_tr & 0x01)
                        {
                            UINT32 ui4_data_ofst = 0;
                            /*add moof_ofs is first elem ofs in mdat,not include head8,
                            need to modify , default_data_ofs concerned*/
                            LOADB_DWRD(pui1_buff, ui4_data_ofst); 
                            pui1_buff += 4;
                            ui8_base_offset = ui8_moof_offset + ui4_data_ofst;
                        }
                        else
                        {
                            ui8_base_offset = ui8_moof_offset;
                        }
                        
                        if(flag_tr & 0x04)
                        {/* First sample flags present*/
                            pui1_buff += 4;
                        }
                        
                        if(pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry + ui4_mem_cnt > pt_tbl_obj->ui4_tbl_size)
                        {
                            b_exit = TRUE;
                            break;
                        }
                            
                        // read trun box data to get duration,size,to cal pts , ofs;
                        for(ui4_i = 0; ui4_i < ui4_mem_cnt; ui4_i++)
                        {
                            if( flag_tr & 0x100)
                            {
                                LOADB_DWRD(pui1_buff, ui4_sample_duration);
                                pui1_buff += 4; 
                            }
                            if(flag_tr & 0x200)
                            {
                                LOADB_DWRD(pui1_buff, ui4_sample_size);                
                                pui1_buff += 4;    
                            }
                            if( flag_tr & 0x400)
                            {                
                                pui1_buff += 4; 
                            }
                            if( flag_tr & 0x800)
                            {
                                LOADB_DWRD(pui1_buff, ui4_sample_ctts);                
                                pui1_buff += 4;                 
                            }
                            
                            if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
                            {
                                ui8_curr_duration_sum_v += ui4_sample_duration;
                                ui8_curr_pts = ui4_get_cur_pts(ui4_sample_ctts,
                                                               (UINT32)ui8_curr_duration_sum_v,
                                                               pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale);   
                            }
                            else if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
                            {
                                ui8_curr_duration_sum_a += ui4_sample_duration;  
                                ui8_curr_pts = ui4_get_cur_pts(ui4_sample_ctts,
                                                               (UINT32)ui8_curr_duration_sum_a,
                                                               pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale); 
                            }  
                            else if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_SUBTITLE)
                            {
                                ui8_curr_duration_sum_s += ui4_sample_duration;  
                                ui8_curr_pts = ui4_get_cur_pts(ui4_sample_ctts,
                                                               (UINT32)ui8_curr_duration_sum_s,
                                                               pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.ui4_scale); 
                            }  
                            
                            pt_idx_entry->ui4_pts = (UINT32)ui8_curr_pts; 
                            pt_idx_entry->ui4_relative_offset = ui8_base_offset + ui4_sample_size_sum; 
                            pt_idx_entry->ui4_size = ui4_sample_size;  
                            pt_idx_entry++;   
                            pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry++;
                            
                            ui4_sample_size_sum += ui4_sample_size;
                        }

                        if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_VIDEO)
                        {
                            pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_v = ui8_curr_duration_sum_v;
                        }

                        if(pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.t_info.e_es_type == MIDXBULD_ELEM_STRM_TYPE_AUDIO)
                        {
                            pt_mp4_handler->t_mln_last_range_info.ui8_last_dur_sum_a = ui8_curr_duration_sum_a;
                        }

                        if(b_exit)
                        {
                            break;
                        }
                    }
                    break; 
#ifdef CENC_MARLIN_SUPPORT
                    case MP4_BOX_SENC:
                    {
                        UINT32   ui4_j;
                        UINT32   ui4_sample_cnt;
                        UINT8    ui1_iv_len;
                        UINT16   ui2_tmp1;
                        UINT32   ui4_tmp2;

                        if(!pt_mp4_enc_info)
                        {
                            pui1_buff += ui4_box_size;
                            DBG_ERROR((INFO_HEADER"%s, line %d, encrypt mode is wrong!!!\r\n", __func__, __LINE__));
                            break;
                        }
                        
                        ui1_iv_len = pt_mp4_enc_info->ui1_iv_len;
                        DBG_INFO((INFO_HEADER"%s, line %d, key length %d.\r\n", __func__, __LINE__, ui1_iv_len));

                        if(pt_mp4_enc_info->e_encrypt_mode == MM_ENCRYPT_METHORD_DRM_CENC)
                        {
                            
                            pui1_buff += 8;
                            LOADB_DWRD(pui1_buff, flag_tr);
                            LOADB_DWRD(pui1_buff+4, ui4_sample_cnt);
                            flag_tr &= 0xFFFFFF;
                            
                            DBG_INFO((INFO_HEADER"%s, line %d, flag 0x%x, ui4_sample_cnt %d.\r\n", 
                                __func__, __LINE__, flag_tr, ui4_sample_cnt));

                            pui1_buff += 8;
                            for(ui4_i = 0; ui4_i < ui4_sample_cnt; ui4_i++)
                            {
                                pt_sample_entry->ui4_iv_len = ui1_iv_len;
                                x_memcpy(pt_sample_entry->aui1_iv, pui1_buff, ui1_iv_len);
                                pui1_buff += ui1_iv_len;
                                
                                if(flag_tr&0x00000002)
                                {
                                    LOADB_WORD(pui1_buff, pt_sample_entry->ui4_subsample_cnt);
                                    pui1_buff += 2;
                                    pt_sample_entry->pt_subsample_info = 
                                                (MENCBULD_ELMT_SUBSAMPLE_INFO_T*)_mm_util_mem_alloc_with_tag(
                                                sizeof(MENCBULD_ELMT_SUBSAMPLE_INFO_T)*pt_sample_entry->ui4_subsample_cnt, MM_MEM_MIDX_TAG);

                                    for(ui4_j = 0; ui4_j < pt_sample_entry->ui4_subsample_cnt; ui4_j++)
                                    {
                                        LOADB_WORD(pui1_buff, ui2_tmp1);
                                        LOADB_DWRD(pui1_buff+2, ui4_tmp2);
                                        pui1_buff += 6;
                                        (pt_sample_entry->pt_subsample_info+ui4_j)->ui4_bytes_clear_data = ui2_tmp1;
                                        (pt_sample_entry->pt_subsample_info+ui4_j)->ui4_bytes_encrypted_data = ui4_tmp2;
                                    }
                                }
                                pt_sample_entry++;
                                pt_enc_tbl_obj->t_elmt_tbl.ui4_number_of_entry++;
                            }
                        }
                        else
                        {
                            pui1_buff += ui4_box_size;
                            DBG_INFO((INFO_HEADER"%s, line %d, encrypt mode is wrong!!!\r\n", __func__, __LINE__));
                            break;
                        }
                    }
#endif
                    default:
                        if(ui4_box_size == 0)
                        {
                            pui1_buff += 8;
                        }
                        else
                        {
                            pui1_buff += ui4_box_size;
                        }
                        break;
                    }
                
                    if(b_exit)
                    {
                        break;
                    }
                }
                
                if(b_exit)
                {
                    break;
                }
            }

            pt_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj; 
            #ifdef CENC_MARLIN_SUPPORT
            pt_enc_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_enc_tbl_obj;
            #endif
            while(pt_tbl_obj != NULL)
            {
                pt_tbl_obj->ui4_tbl_size = pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry;
                if(pt_tbl_obj->ui4_tbl_size != 0)
                {
                    pt_tbl_obj->ui4_start_pts = pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry[0].ui4_pts;
                    pt_tbl_obj->ui4_end_pts = pt_tbl_obj->t_elmt_tbl.u_fmt.t_es
                                .pt_idx_entry[pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry-1].ui4_pts;
                    pt_tbl_obj->ui4_end_sample = pt_tbl_obj->ui4_start_sample + pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry-1;
                }
                else
                {
                    pt_tbl_obj->ui4_tbl_size = 1;
                    pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry = 1;
                }
                if(b_eof_got)
                {
                    pt_tbl_obj->ui4_end_pts = MIDXBULD_IDX_RANGE_FULL;
                }
                #ifdef CENC_MARLIN_SUPPORT
                if(pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry != pt_enc_tbl_obj->t_elmt_tbl.ui4_number_of_entry)
                {
                    DBG_ERROR(("%s, line %d, cenc table(%d) & index table(%d) mismatch!!!!!", 
                            __func__, __LINE__, pt_enc_tbl_obj->t_elmt_tbl.ui4_number_of_entry, pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry));
                }
                pt_enc_tbl_obj = pt_enc_tbl_obj->pt_next;
                #endif
                pt_tbl_obj = pt_tbl_obj->pt_next;
            }

            pt_mp4_handler->t_mln_last_range_info.ui4_range_start_pts = 
                        pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_start_pts;
            pt_mp4_handler->t_mln_last_range_info.ui4_range_end_pts = 
                        pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_end_pts;
            pt_mp4_handler->t_mln_last_range_info.ui4_end_sample_idx = 
                        pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_end_sample;
            pt_mp4_handler->pt_curr_range_obj->u_range.t_es.ui4_start_pts = 
                    pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_start_pts;
            pt_mp4_handler->pt_curr_range_obj->u_range.t_es.ui4_end_pts = 
                    pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_end_pts;
            pt_mp4_handler->pt_elmt_tbl_alloc->u_range.t_es.ui4_start_pts = 
                    pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_start_pts;
            pt_mp4_handler->pt_elmt_tbl_alloc->u_range.t_es.ui4_end_pts = 
                    pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_end_pts;
            if(b_eof_got/*pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj->ui4_end_pts + 30000 >
                    pt_midxbuld_obj->t_src_info.ui4_total_duration*/)
            {
                pt_mp4_handler->pt_curr_range_obj->u_range.t_es.ui4_end_pts = MIDXBULD_IDX_RANGE_FULL;
                pt_mp4_handler->pt_elmt_tbl_alloc->u_range.t_es.ui4_end_pts = MIDXBULD_IDX_RANGE_FULL;
            }


#ifndef __KERNEL__
        pt_tbl_obj = pt_mp4_handler->pt_curr_range_obj->pt_tbl_obj; 
        while(pt_tbl_obj != NULL)
        {
            UINT32 ui4_num = pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry;
            pt_tbl_obj->ui4_tbl_size = ui4_num;
            if(pt_tbl_obj->ui4_tbl_size > 1)
            {
                DBG_LOG_PRINT(("indexing ui4_number_of_entry = %d , baseoffset=0x%llx\r\n", 
                          (int)pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry,
                          pt_tbl_obj->t_elmt_tbl.ui8_base_offset));
                DBG_LOG_PRINT(("*****************piff range info start*********************\r\n"));
                DBG_LOG_PRINT(("start    pts: 0x%8x\r\n", (unsigned int)pt_tbl_obj->ui4_start_pts));
                DBG_LOG_PRINT(("end      pts: 0x%8x\r\n", (unsigned int)pt_tbl_obj->ui4_end_pts));
                DBG_LOG_PRINT(("start tbl pts: 0x%8x\r\n", (unsigned int)pt_tbl_obj->t_elmt_tbl.u_fmt
                            .t_es.pt_idx_entry[0].ui4_pts));
                DBG_LOG_PRINT(("end  tbl pts: 0x%8x\r\n", (unsigned int)pt_tbl_obj->t_elmt_tbl.u_fmt
                            .t_es.pt_idx_entry[pt_tbl_obj->ui4_tbl_size -1].ui4_pts));
                DBG_LOG_PRINT(("start sample: 0x%8x\r\n", (unsigned int)pt_tbl_obj->ui4_start_sample));
                DBG_LOG_PRINT(("end   sample: 0x%8x\r\n", (unsigned int)pt_tbl_obj->ui4_end_sample));
                DBG_LOG_PRINT(("start offset: 0x%8x\r\n", (unsigned int)pt_tbl_obj->t_elmt_tbl.u_fmt
                            .t_es.pt_idx_entry[0].ui4_relative_offset));
                DBG_LOG_PRINT(("end   offset: 0x%8x\r\n", (unsigned int)pt_tbl_obj->t_elmt_tbl.u_fmt
                            .t_es.pt_idx_entry[pt_tbl_obj->ui4_tbl_size -1].ui4_relative_offset));
                DBG_LOG_PRINT(("***************** range info end*********************\r\n"));
                for(ui4_num = 0; ui4_num<pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry; ui4_num++)
                {
                    if((ui4_num<5) || (ui4_num+5>pt_tbl_obj->t_elmt_tbl.ui4_number_of_entry))
                    {
                        DBG_LOG_PRINT(("---------ui4_pts: 0x%x, ofst: 0x%x, size: 0x%x  --------\r\n",
                                    (unsigned int)pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry[ui4_num].ui4_pts,
                                    (unsigned int)pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry[ui4_num].ui4_relative_offset,
                                    (unsigned int)pt_tbl_obj->t_elmt_tbl.u_fmt.t_es.pt_idx_entry[ui4_num].ui4_size));
                    }
                }
            }
            pt_tbl_obj = pt_tbl_obj->pt_next;
        }
#endif
        ui4_start_key_idx = 0;
    }
  
    pt_mp4_handler->ui1_curr_buld_progress = 100;
    pt_mp4_handler->pt_curr_range_obj->b_index_is_valid = TRUE;

    return MIDXBULDR_OK;
}



