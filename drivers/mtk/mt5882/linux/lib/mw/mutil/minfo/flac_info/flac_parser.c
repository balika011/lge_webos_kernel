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
 * $RCSfile: mkv_parser.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/4 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 9ce950eb85bf90ea84aa0781ae2e804f $ Asa
 *
 * Description: 
 *         This file contains the implementation of the mkv parser function
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

#include "u_common.h"
#include "x_mm_common.h"
#include "mutil/minfo/minfo.h"
#include "x_minfo.h"
#include "x_uc_str.h"
#include "flac_parser.h"
#include "u_dbg.h"
#include "mutil/mm_util.h"

#else

#include "x_os.h"
#include "u_common.h"
#include "x_mm_common.h"
#include "../minfo.h"
#include "x_minfo.h"
#include "x_uc_str.h"
#include "flac_parser.h"
#include "u_dbg.h"
#include "../../mm_util.h"
#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define flac_dwBigFourCC(a,b,c,d)    ((((UINT32)a)<<24)|(((UINT32)b)<<16)|(((UINT32)c)<<8)|((UINT32)d))

#define FLAC_BOX_HEAD       flac_dwBigFourCC(0x66, 0x4c, 0x61, 0x43)

#define FLAC_STREAN_INFO         0
#define FLAC_PADDING             1
#define FLAC_APPLICATION         2
#define FLAC_SEEKTABLE           3
#define FLAC_VORBIS_COMMENT      4
#define FLAC_CUESHEET            5
#define FLAC_PICTURE             6
#define FLAC_INVALID			 127
#define FLAC_FRAME_HEADER        0x3FFE
#define FLAC_MD5SIGN_LEN         16
#define FLAC_ID3_HEAD       	 "ID3"
#define FLAC_MAX_BUFFER_LEN      1024*1024



/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
INT32 _flac_parse_stream_info(MINFO_OBJ_T*    pt_minfo_obj)
{
    copybytes_fct               pf_copybytes;
    INT32                       i4_ret; 
    //UINT64                      ui8_parse_pos = 0;
    //UINT64                      ui8_sub_element_size;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT8                       ac_temp[128];
    //UINT64                      ui8_file_pos;
    UINT32                      ui4_read_count;
    UINT32                      ui4_block_size = 0;
    //UINT32                      ui4_cur_buf_pos;
    UINT8*                      pui1_md5_signature;
    
    DBG_API(("[FLAC_parse]_flac_parse_stream_info\n"));
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    if(pt_flac_info_obj->ui1_cur_block_id != FLAC_STREAN_INFO)
    {
        return MINFOR_INTERNAL_ERR;
    }
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);
    ui4_block_size = flac_dwBigFourCC(0,ac_temp[0],ac_temp[1],ac_temp[2]);
    
    if(ui4_block_size == 0 || ui4_read_count == 0)
    {
        return MINFOR_INTERNAL_ERR;
    }
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          ui4_block_size,
                          ui4_block_size,
                          &ui4_read_count);
    
    pt_flac_info_obj->ui2_min_block_size = flac_dwBigFourCC(0,0,ac_temp[0],ac_temp[1]);
    
    pt_flac_info_obj->ui2_max_block_size = flac_dwBigFourCC(0,0,ac_temp[2],ac_temp[3]);
    
    pt_flac_info_obj->ui4_min_frame_size = flac_dwBigFourCC(0,ac_temp[4],ac_temp[5],ac_temp[6]);
    
    pt_flac_info_obj->ui4_max_frame_size = flac_dwBigFourCC(0,ac_temp[7],ac_temp[8],ac_temp[9]);
    
    pt_flac_info_obj->ui4_sample_rate = (flac_dwBigFourCC(0,ac_temp[10],ac_temp[11],ac_temp[12]&0xF0))>>4;
    
    pt_flac_info_obj->ui1_num_channels = ((ac_temp[12]&0x0E)>>1) + 1;
    
    pt_flac_info_obj->ui1_bits_per_sample = (flac_dwBigFourCC(0,0,ac_temp[12]&0x01,ac_temp[13]&0xF0)>>4) + 1;
    
    pt_flac_info_obj->ui8_total_sample = (((UINT64)flac_dwBigFourCC(ac_temp[13]&0x0F,ac_temp[14],ac_temp[15],ac_temp[16]))<<8)|ac_temp[17];

    if (pt_flac_info_obj->ui4_min_frame_size == pt_flac_info_obj->ui4_max_frame_size 
        && pt_flac_info_obj->ui4_min_frame_size != 0)   /*CBR, 0 means it is unknown*/
    {
        /*use overall bit rate firstly. If has another spec, to enhance this.*/
        pt_flac_info_obj->ui4_bitrate = 
                (UINT32)_mm_div64(pt_flac_info_obj->ui8_file_size * 8,_mm_div64(pt_flac_info_obj->ui8_total_sample, pt_flac_info_obj->ui4_sample_rate, NULL), NULL);
    }
    else        /*VBR*/
    {
        pt_flac_info_obj->ui4_bitrate = 
                (UINT32)_mm_div64(pt_flac_info_obj->ui8_file_size * 8,_mm_div64(pt_flac_info_obj->ui8_total_sample, pt_flac_info_obj->ui4_sample_rate, NULL), NULL);
    }
    
    pui1_md5_signature = (UINT8*)x_mem_alloc(FLAC_MD5SIGN_LEN);
    if(NULL == pui1_md5_signature)
    {
        return MINFOR_OUT_OF_MEMORY;
    }
    x_memcpy(pui1_md5_signature, &ac_temp[18], FLAC_MD5SIGN_LEN);
    pt_flac_info_obj->pt_md5_signature = pui1_md5_signature;
    return i4_ret;
}

INT32 _flac_parse_padding_info(MINFO_OBJ_T*    pt_minfo_obj)
{
    setpos_fct                  pf_set_pos;  
    //getpos_fct                  pf_get_pos;
    copybytes_fct               pf_copybytes;
    INT32                       i4_ret; 
    //UINT64                      ui8_parse_pos = 0;
    //UINT64                      ui8_sub_element_size;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT8                       ac_temp[128];
    //UINT64                      ui8_file_pos;
    UINT32                      ui4_read_count;
    UINT32                      ui4_block_size;
    //UINT32                      ui4_cur_buf_pos;
    //UINT8*                      pui1_md5_signature;
    
    DBG_API(("[FLAC_parse]_flac_parse_padding_info\n"));

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pf_set_pos      = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    //pf_get_pos      = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    if(pt_flac_info_obj->ui1_cur_block_id != FLAC_PADDING)
    {
        return MINFOR_INTERNAL_ERR;
    }
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);
    ui4_block_size = flac_dwBigFourCC(0,ac_temp[0],ac_temp[1],ac_temp[2]);
    
    i4_ret = pf_set_pos(pt_minfo_obj, ui4_block_size, FM_SEEK_CUR);
    
    return i4_ret;
}

INT32 _flac_parse_app_info(MINFO_OBJ_T*    pt_minfo_obj)
{
    setpos_fct                  pf_set_pos;  
    //getpos_fct                  pf_get_pos;
    copybytes_fct               pf_copybytes;
    INT32                       i4_ret; 
    //UINT64                      ui8_parse_pos = 0;
    //UINT64                      ui8_sub_element_size;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT8                       ac_temp[128];
    //UINT64                      ui8_file_pos;
    UINT32                      ui4_read_count;
    UINT32                      ui4_block_size;
    //UINT32                      ui4_cur_buf_pos;
    //UINT8*                      pui1_md5_signature;
    
    DBG_API(("[FLAC_parse]_flac_parse_app_info\n"));
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pf_set_pos      = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    //pf_get_pos      = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    if(pt_flac_info_obj->ui1_cur_block_id != FLAC_APPLICATION)
    {
        return MINFOR_INTERNAL_ERR;
    }
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);
    ui4_block_size = flac_dwBigFourCC(0,ac_temp[0],ac_temp[1],ac_temp[2]);
    i4_ret = pf_set_pos(pt_minfo_obj, ui4_block_size, FM_SEEK_CUR);
    
    return i4_ret;
}

INT32 _flac_parse_seek_table(MINFO_OBJ_T*    pt_minfo_obj)
{
    setpos_fct                  pf_set_pos;  
    getpos_fct                  pf_get_pos;
    copybytes_fct               pf_copybytes;
    INT32                       i4_ret; 
    //UINT64                      ui8_parse_pos = 0;
    //UINT64                      ui8_sub_element_size;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT8                       ac_temp[128];
    //UINT64                      ui8_file_pos;
    UINT32                      ui4_read_count;
    UINT32                      ui4_block_size;
    UINT64                      ui8_cur_buf_pos;
    //UINT8*                      pui1_md5_signature;
    
    DBG_API(("[FLAC_parse]_flac_parse_seek_table\n"));
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pf_set_pos      = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    pf_get_pos      = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    if(pt_flac_info_obj->ui1_cur_block_id != FLAC_SEEKTABLE)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);  
    ui4_block_size = flac_dwBigFourCC(0, ac_temp[0], ac_temp[1], ac_temp[2]);
    
    pt_flac_info_obj->ui4_index_leng = ui4_block_size;
    
    pf_get_pos(pt_minfo_obj,&ui8_cur_buf_pos);
    pt_flac_info_obj->ui8_index_start_pos = ui8_cur_buf_pos;
    
    i4_ret = pf_set_pos(pt_minfo_obj, pt_flac_info_obj->ui4_index_leng, FM_SEEK_CUR);
    
    return i4_ret;
}

INT32 _flac_parse_vorbis_comment(MINFO_OBJ_T*    pt_minfo_obj)
{
    setpos_fct                  pf_set_pos;  
    //getpos_fct                  pf_get_pos;
    copybytes_fct               pf_copybytes;
    INT32                       i4_ret; 
    //UINT64                      ui8_parse_pos = 0;
    //UINT64                      ui8_sub_element_size;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT8                       ac_temp[128];
    //UINT64                      ui8_file_pos;
    UINT32                      ui4_read_count;
    UINT32                      ui4_block_size;
    //UINT32                      ui4_cur_buf_pos;
    //UINT8*                      pui1_md5_signature;

    DBG_API(("[FLAC_parse]_flac_parse_vorbis_comment\n"));
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pf_set_pos      = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    //pf_get_pos      = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    if(pt_flac_info_obj->ui1_cur_block_id != FLAC_VORBIS_COMMENT)
    {
        return MINFOR_INTERNAL_ERR;
    }
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);
    ui4_block_size = flac_dwBigFourCC(0,ac_temp[0],ac_temp[1],ac_temp[2]);
    
    i4_ret = pf_set_pos(pt_minfo_obj, ui4_block_size, FM_SEEK_CUR);
    
    return i4_ret;
}

INT32 _flac_parse_cue_sheet(MINFO_OBJ_T*    pt_minfo_obj)
{
    setpos_fct                  pf_set_pos;  
    //getpos_fct                  pf_get_pos;
    copybytes_fct               pf_copybytes;
    INT32                       i4_ret; 
    //UINT64                      ui8_parse_pos = 0;
    //UINT64                      ui8_sub_element_size;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT8                       ac_temp[128];
    //UINT64                      ui8_file_pos;
    UINT32                      ui4_read_count;
    UINT32                      ui4_block_size;
    //UINT32                      ui4_cur_buf_pos;
    //UINT8*                      pui1_md5_signature;
    
    DBG_API(("[FLAC_parse]_flac_parse_cue_sheet\n"));

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pf_set_pos      = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    //pf_get_pos      = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    if(pt_flac_info_obj->ui1_cur_block_id != FLAC_CUESHEET)
    {
        return MINFOR_INTERNAL_ERR;
    }
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);
    ui4_block_size = flac_dwBigFourCC(0,ac_temp[0],ac_temp[1],ac_temp[2]);
    
    i4_ret = pf_set_pos(pt_minfo_obj, ui4_block_size, FM_SEEK_CUR);
    
    return i4_ret;
}

INT32 _flac_parse_picture_info(MINFO_OBJ_T*    pt_minfo_obj)
{
    setpos_fct                  pf_set_pos;  
    //getpos_fct                  pf_get_pos;
    copybytes_fct               pf_copybytes;
    INT32                       i4_ret; 
    //UINT64                      ui8_parse_pos = 0;
    //UINT64                      ui8_sub_element_size;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT8                       ac_temp[128];
    //UINT64                      ui8_file_pos;
    UINT32                      ui4_read_count;
    UINT32                      ui4_block_size;
    //UINT32                      ui4_cur_buf_pos;
    //UINT8*                      pui1_md5_signature;
    
    DBG_API(("[FLAC_parse]_flac_parse_picture_info\n"));
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pf_set_pos      = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    //pf_get_pos      = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    if(pt_flac_info_obj->ui1_cur_block_id != FLAC_PICTURE)
    {
        return MINFOR_INTERNAL_ERR;
    }
    x_memset(ac_temp, 0, 128);
    i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);
    ui4_block_size = flac_dwBigFourCC(0,ac_temp[0],ac_temp[1],ac_temp[2]);
    
    i4_ret = pf_set_pos(pt_minfo_obj, ui4_block_size, FM_SEEK_CUR);
    
    return i4_ret;
}

INT32 flac_parse_elements (MINFO_OBJ_T*    pt_minfo_obj)
{
    setpos_fct                  pf_set_pos;  
    getpos_fct                  pf_get_pos;
    copybytes_fct               pf_copybytes;
    input1bytes_fct             pf_inp1byte;
    input4bytes_fct             pf_inp4byte;
    INT32                       i4_ret = MINFOR_OK; 
    UINT64                      ui8_parse_pos;
    FLAC_STREAM_INFO_T*         pt_flac_info_obj;
    UINT32                      ui4_buf_read;
    UINT8                       ui1_buf_read;
    UINT8                       ui1_block_type;
    BOOL                        b_last_metadata = FALSE;
    BOOL                        b_find_frame = FALSE;
    UINT32                      ui4_block_size;
    UINT32                      ui4_read_count;
    UINT8                       ac_temp[128];
    UINT64                      ui8_id3v2_offset;

    DBG_INFO(("mkv_parse_elements:begin to parse flac file\r\n"));
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pt_minfo_obj)
    {
        DBG_ERROR(("flac_parse_elements: pt_minfo_obj is NULL\r\n"));
        return  MINFOR_INTERNAL_ERR;
    }
    if ((pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_input1 == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_input4 == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL))
    {
        DBG_ERROR(("mkv_parse_elements:input_fct_tbl has NULL function\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    pt_flac_info_obj = (FLAC_STREAM_INFO_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_flac_info_obj)
    {
        DBG_ERROR(("_flac_parse_elements:flac_info_obj is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    
    pf_set_pos      = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    pf_get_pos      = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_inp1byte     = pt_minfo_obj->t_input_fct_tbl.pf_input1;
    pf_inp4byte     = pt_minfo_obj->t_input_fct_tbl.pf_input4;
    pf_copybytes    = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    /*get file size*/
    pf_set_pos(pt_minfo_obj , 0, FM_SEEK_END);
    pf_get_pos(pt_minfo_obj, &(pt_flac_info_obj->ui8_file_size));
    
    ui8_parse_pos = 0;
    i4_ret = MINFOR_OK;
    
    /* check ID3v2 tag is exist or NOT */
    ui8_id3v2_offset = 0;
    do
    {
        UINT32 ui4_id3_tag_offset;
        pf_set_pos(pt_minfo_obj, ui8_id3v2_offset, FM_SEEK_BGN);
        x_memset(ac_temp, 0, 128);
        pf_copybytes(pt_minfo_obj,(VOID*)ac_temp, 10, 10, &ui4_read_count);

        if (x_memcmp((VOID*)FLAC_ID3_HEAD, ac_temp, 3) == 0)
        {
            /* calculate ID3v2 tag offset */
            ui4_id3_tag_offset = (((UINT32)(ac_temp[6])) << 21)
                                 | (((UINT32)(ac_temp[7])) << 14)
                                 | (((UINT32)(ac_temp[8])) << 7)
                                 | ((UINT32)(ac_temp[9]));
                 
            ui8_id3v2_offset += ui4_id3_tag_offset;
            ui8_id3v2_offset += 10; /*header of id3 tag v2*/
        }
        else
        {
            break;
        }
    }while(1);
    
    pf_set_pos(pt_minfo_obj, ui8_id3v2_offset, FM_SEEK_BGN);
    pf_inp4byte(pt_minfo_obj, &ui4_buf_read);
    if(ui4_buf_read != FLAC_BOX_HEAD)
    {
        return MINFOR_INTERNAL_ERR;
    }
    pf_get_pos(pt_minfo_obj, &ui8_parse_pos);
    while (ui8_parse_pos < pt_flac_info_obj->ui8_file_size)
    {
        ui1_buf_read = 127;
        pf_inp1byte(pt_minfo_obj, &ui1_buf_read);
        if(ui4_buf_read == (UINT32)(-1))
        {
            break;
        }
        if(ui1_buf_read >> 7)
        {
            b_last_metadata = TRUE;
        }
            
        ui1_block_type = ui1_buf_read & 0x7F;
        pt_flac_info_obj->ui1_cur_block_id = ui1_block_type;
        
        switch(ui1_block_type)
        {
            case FLAC_STREAN_INFO:
                i4_ret = _flac_parse_stream_info(pt_minfo_obj);
                break;
            case FLAC_PADDING:
                i4_ret = _flac_parse_padding_info(pt_minfo_obj);
                break;
            case FLAC_APPLICATION:
                i4_ret = _flac_parse_app_info(pt_minfo_obj);
                break;
            case FLAC_SEEKTABLE:
                i4_ret = _flac_parse_seek_table(pt_minfo_obj);
                break;
            case FLAC_VORBIS_COMMENT:
                i4_ret = _flac_parse_vorbis_comment(pt_minfo_obj);
                break;
            case FLAC_CUESHEET:
                i4_ret = _flac_parse_cue_sheet(pt_minfo_obj);
                break;
            case FLAC_PICTURE:
                i4_ret = _flac_parse_picture_info(pt_minfo_obj);
                break;
            case FLAC_INVALID:
                break;
            default:
            {
                x_memset(ac_temp, 0, 128);
                i4_ret = pf_copybytes(pt_minfo_obj, 
                          ac_temp, 
                          3,
                          3,
                          &ui4_read_count);
                
                ui4_block_size = flac_dwBigFourCC(0,ac_temp[0],ac_temp[1],ac_temp[2]);
                i4_ret = pf_set_pos(pt_minfo_obj, ui4_block_size, FM_SEEK_CUR);
            }
            break;
        }
        
        pf_get_pos(pt_minfo_obj, &ui8_parse_pos);
        if(i4_ret != MINFOR_OK)
        {
            break;
        }
        if(b_last_metadata)
        {
            break;
        }
    }
    x_memset(ac_temp, 0, 128);
    pf_copybytes(pt_minfo_obj, 
                      ac_temp, 
                      2,
                      2,
                      &ui4_read_count);
            
    ui4_block_size = flac_dwBigFourCC(0, 0, ac_temp[0],ac_temp[1])&0X0000FFFF;
    if((ui4_block_size >>2) == FLAC_FRAME_HEADER)
    {
        pf_get_pos(pt_minfo_obj, &ui8_parse_pos);
        pt_flac_info_obj->ui8_cluster_position = ui8_parse_pos - 2;// -1;
        pt_flac_info_obj->ui8_cluster_leng = pt_flac_info_obj->ui8_file_size - pt_flac_info_obj->ui8_cluster_position;
        b_find_frame = TRUE;
    }
    else
    {
        UINT8*  ac_frame_temp = NULL;
        UINT32  ui4_try_times = 0;
        ui4_read_count = 0;
        
        DBG_INFO(("[FLAC_parse]This file need to find the sync code!\n"));
        
        ac_frame_temp = (UINT8*)x_mem_alloc(FLAC_MAX_BUFFER_LEN*(sizeof(UINT8)));
        if(NULL == ac_frame_temp)
        {
            return MINFOR_OUT_OF_MEMORY;
        }
        for(ui4_try_times = 0; ui4_try_times < 1; ui4_try_times++)
        {
            pf_get_pos(pt_minfo_obj, &ui8_parse_pos);
            pf_set_pos(pt_minfo_obj, ui8_parse_pos - 2, MEDIA_SEEK_BGN);
            x_memset(ac_frame_temp, 0, FLAC_MAX_BUFFER_LEN);
            pf_copybytes(pt_minfo_obj, 
                          ac_frame_temp, 
                          FLAC_MAX_BUFFER_LEN,
                          FLAC_MAX_BUFFER_LEN,
                          &ui4_read_count);
            
            ui4_block_size = flac_dwBigFourCC(0, 0, ac_frame_temp[0],ac_frame_temp[1])&0X0000FFFF;
            if((ui4_block_size >>2) == FLAC_FRAME_HEADER)
            {
                pf_get_pos(pt_minfo_obj, &ui8_parse_pos);
                pt_flac_info_obj->ui8_cluster_position = ui8_parse_pos - ui4_read_count;// -1;
                pt_flac_info_obj->ui8_cluster_leng = pt_flac_info_obj->ui8_file_size - pt_flac_info_obj->ui8_cluster_position;
                b_find_frame = TRUE;
            }
            else
            {
                int i;
                for(i = 0; i< ui4_read_count; i++)
                {
                    ui4_block_size = flac_dwBigFourCC(0, 0, ac_frame_temp[i],ac_frame_temp[i+1])&0X0000FFFF;
                    if((ui4_block_size >>2) == FLAC_FRAME_HEADER)
                    {
                        pf_get_pos(pt_minfo_obj, &ui8_parse_pos);
                        pt_flac_info_obj->ui8_cluster_position = ui8_parse_pos - ui4_read_count + i;
                        pt_flac_info_obj->ui8_cluster_leng = pt_flac_info_obj->ui8_file_size - pt_flac_info_obj->ui8_cluster_position;                        
                        b_find_frame = TRUE;
                        break;
                    }
                }
            }
            if(b_find_frame == TRUE)
            {
                break;
            }
        }
        x_mem_free(ac_frame_temp);
        ac_frame_temp = NULL;
    }
    if(!b_find_frame)
    {
        DBG_ERROR(("[FLAC_parse]FLAC_FRAME_HEADER NOT FOUND\n"));
    }
    
    return MINFOR_OK;
}
