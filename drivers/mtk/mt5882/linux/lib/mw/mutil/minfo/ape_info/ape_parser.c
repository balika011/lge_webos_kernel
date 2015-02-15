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
#include "ape_parser.h"
#include "u_dbg.h"
#include "mutil/mm_util.h"

#else

#include "x_os.h"
#include "u_common.h"
#include "x_mm_common.h"
#include "../minfo.h"
#include "x_minfo.h"
#include "x_uc_str.h"
#include "ape_parser.h"
#include "u_dbg.h"
#include "../../mm_util.h"
#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define ape_dwBigFourCC(a,b,c,d)    ((((UINT32)a)<<24)|(((UINT32)b)<<16)|(((UINT32)c)<<8)|((UINT32)d))
#define NEW_VERSION                 (3980)

#define MAC_FORMAT_FLAG_8_BIT                 1    // is 8-bit [OBSOLETE]
#define MAC_FORMAT_FLAG_CRC                   2    // uses the new CRC32 error detection [OBSOLETE]
#define MAC_FORMAT_FLAG_HAS_PEAK_LEVEL        4    // uint32 nPeakLevel after the header [OBSOLETE]
#define MAC_FORMAT_FLAG_24_BIT                8    // is 24-bit [OBSOLETE]
#define MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS    16    // has the number of seek elements after the peak level
#define MAC_FORMAT_FLAG_CREATE_WAV_HEADER    32    // create the wave header on decompression (not stored)

#define COMPRESSION_LEVEL_EXTRA_HIGH    4000

/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
static INT32 _ape_info_set_pos(MINFO_OBJ_T *pt_minfo_obj, 
                                 UINT64 ui8_file_offset, 
                                 UINT8 ui1_whence);

static INT32 _ape_info_get_pos(MINFO_OBJ_T *pt_minfo_obj, 
                                 UINT64 *pui8_cur_pos);

static INT32 _ape_info_copy_bytes(MINFO_OBJ_T *pt_minfo_obj, 
                                    void* pv_buf, 
                                    SIZE_T z_buf_leng, 
                                    SIZE_T z_copy_size, 
                                    UINT32* pui1_size);

static INT32 _ape_info_parse_current_version(MINFO_OBJ_T* pt_minfo_obj);

static INT32 _ape_info_parse_old_version(MINFO_OBJ_T* pt_minfo_obj);

/*-----------------------------------------------------------------------------
                    functions begin
 ----------------------------------------------------------------------------*/



static INT32 _ape_info_set_pos(MINFO_OBJ_T *pt_minfo_obj, 
                                 UINT64 ui8_file_offset, 
                                 UINT8 ui1_whence)
{
    setpos_fct      pf_set_pos  = pt_minfo_obj->t_input_fct_tbl.pf_set_pos; 
    MEDIA_FEEDER_T  t_feeder;
    /*feeder*/
    if(pt_minfo_obj->h_src_feeder != NULL_HANDLE)
    {
        t_feeder.h_feeder = pt_minfo_obj->h_src_feeder;
        return pf_set_pos(&t_feeder, (INT64)ui8_file_offset, ui1_whence);
    }
    /*file*/
    else
    {
        return pf_set_pos(pt_minfo_obj, (INT64)ui8_file_offset, ui1_whence);
    }
}

static INT32 _ape_info_get_pos(MINFO_OBJ_T *pt_minfo_obj, 
                                 UINT64 *pui8_cur_pos)
{
    getpos_fct      pf_get_pos  = pt_minfo_obj->t_input_fct_tbl.pf_get_pos; 
    MEDIA_FEEDER_T  t_feeder;
    /*feeder*/
    if(pt_minfo_obj->h_src_feeder != NULL_HANDLE)
    {
        t_feeder.h_feeder = pt_minfo_obj->h_src_feeder;
        return pf_get_pos(&t_feeder, pui8_cur_pos);
    }
    /*file*/
    else
    {
        return pf_get_pos(pt_minfo_obj, pui8_cur_pos);
    }
}

static INT32 _ape_info_copy_bytes(MINFO_OBJ_T *pt_minfo_obj, 
                                    void* pv_buf, 
                                    SIZE_T z_buf_leng, 
                                    SIZE_T z_copy_size, 
                                    UINT32* pui1_size)
{
    copybytes_fct      pf_copybyte  = pt_minfo_obj->t_input_fct_tbl.pf_copybytes; 
    MEDIA_FEEDER_T     t_feeder;
    
    if (pt_minfo_obj->h_src_feeder != NULL_HANDLE)
    {
        t_feeder.h_feeder = pt_minfo_obj->h_src_feeder;
        return pf_copybyte(&t_feeder, pv_buf, z_buf_leng, z_copy_size, pui1_size);
    }
   
    else
    {
        return pf_copybyte(pt_minfo_obj, pv_buf, z_buf_leng, z_copy_size, pui1_size);
    }
}

static INT32 _ape_info_parse_current_version(MINFO_OBJ_T* pt_minfo_obj)
{
    MINFO_APE_FILE_INFO_T*          pt_ape_hdlr_obj = NULL;
    MINFO_APE_DESCRIPTOR_INFO_T*    pt_ape_desc = NULL;
    MINFO_APE_HEADER_INFO_T*        pt_ape_header = NULL;
    MINFO_APE_WAVEHEADER_INFO_T*    pt_wav_header = NULL;
    MINFO_APE_AUDIO_INFO_T*         pt_aud_info = NULL;
    INT32                           i4_ret = 0;
    SIZE_T                          z_buf_len = 0;
    UINT32                          ui4_read_num = 0;

    if (pt_minfo_obj == NULL)
    {
        return MINFOR_INV_ARG;
    }
    
    pt_ape_hdlr_obj = (MINFO_APE_FILE_INFO_T*)pt_minfo_obj->pv_handler_obj;

    pt_ape_hdlr_obj->b_is_old = FALSE;
    /* parse ape desc*/
    pt_ape_desc = &pt_ape_hdlr_obj->t_ape_desc_info;

    i4_ret = _ape_info_set_pos(pt_minfo_obj , pt_ape_hdlr_obj->ui8_curr_parser_offset, FM_SEEK_BGN);
    if (i4_ret != MINFOR_OK)
    {
        DBG_ERROR(("ERROR: seek to ape desc fail. \r\n"));
        return i4_ret;
    }
    
    z_buf_len = sizeof(MINFO_APE_DESCRIPTOR_INFO_T);
    i4_ret = _ape_info_copy_bytes(pt_minfo_obj,
                    (VOID*)pt_ape_desc,
                    z_buf_len,
                    z_buf_len, 
                    &ui4_read_num);
    if (i4_ret != INPUTR_OK || ui4_read_num != z_buf_len)
    {
        DBG_INFO(("APE INFO: read ape desc error!\r\n"));
        return MINFOR_INTERNAL_ERR;
    }

    pt_ape_hdlr_obj->ui8_curr_parser_offset += pt_ape_desc->ui4_descrptorbytes;
    
    /* parse ape header*/
    pt_ape_header = &pt_ape_hdlr_obj->t_ape_header_info;
    
    i4_ret = _ape_info_set_pos(pt_minfo_obj , pt_ape_hdlr_obj->ui8_curr_parser_offset, FM_SEEK_BGN);
    if (i4_ret != MINFOR_OK)
    {
        DBG_ERROR(("ERROR: seek to ape header fail. \r\n"));
        return i4_ret;
    }
    
    z_buf_len = sizeof(MINFO_APE_HEADER_INFO_T);
    i4_ret = _ape_info_copy_bytes(pt_minfo_obj,
                    (VOID*)pt_ape_header,
                    z_buf_len,
                    z_buf_len, 
                    &ui4_read_num);
    if (i4_ret != INPUTR_OK || ui4_read_num != z_buf_len)
    {
        DBG_INFO(("APE INFO: read ape header error!\r\n"));
        return MINFOR_INTERNAL_ERR;
    }

    pt_ape_hdlr_obj->ui8_curr_parser_offset += pt_ape_desc->ui4_headerbytes;
    
    /*seek table*/
    pt_ape_hdlr_obj->ui8_seektableoffset = pt_ape_hdlr_obj->ui8_curr_parser_offset;
    pt_ape_hdlr_obj->ui8_seektablebytes = pt_ape_desc->ui4_seektablebytes;
    pt_ape_hdlr_obj->ui8_curr_parser_offset += pt_ape_hdlr_obj->ui8_seektablebytes;
    
    /* parse wav header*/
    if (!(pt_ape_header->ui2_formatflags & MAC_FORMAT_FLAG_CREATE_WAV_HEADER))
    {
        pt_wav_header = &pt_ape_hdlr_obj->t_ape_wavheader_info;
        i4_ret = _ape_info_set_pos(pt_minfo_obj , pt_ape_hdlr_obj->ui8_curr_parser_offset, FM_SEEK_BGN);
        if (i4_ret != MINFOR_OK)
        {
            DBG_ERROR(("ERROR: seek to wav header fail. \r\n"));
            return i4_ret;
        }
        
        z_buf_len = sizeof(MINFO_APE_WAVEHEADER_INFO_T);
        i4_ret = _ape_info_copy_bytes(pt_minfo_obj,
                        (VOID*)pt_wav_header,
                        z_buf_len,
                        z_buf_len, 
                        &ui4_read_num);
        if (i4_ret != INPUTR_OK || ui4_read_num != z_buf_len)
        {
            DBG_INFO(("APE INFO: read wav header error!\r\n"));
            return MINFOR_INTERNAL_ERR;
        }

        pt_ape_hdlr_obj->ui8_curr_parser_offset += z_buf_len;
    }
    
    /*aud info */
    pt_aud_info = &pt_ape_hdlr_obj->t_ape_audio_info;
    pt_aud_info->b_audio_only = FALSE;
    pt_aud_info->ui2_bitsperframe = pt_ape_header->ui2_bitsperframe;
    pt_aud_info->ui2_channels = pt_ape_header->ui2_channels;
    pt_aud_info->ui2_compresslevel = pt_ape_header->ui2_compresslevel;
    pt_aud_info->ui4_blockperframe = pt_ape_header->ui4_blockperframe;
    pt_aud_info->ui4_finalframeblocks = pt_ape_header->ui4_finalframeblocks;
    pt_aud_info->ui4_samplerate = pt_ape_header->ui4_samplerate;
    pt_aud_info->ui4_totalframes = pt_ape_header->ui4_totalframes;
    pt_aud_info->ui4_version = pt_ape_desc->ui4_version;
    pt_aud_info->ui2_bitpersample = pt_aud_info->ui2_bitsperframe;
    
    return MINFOR_OK;
}

static INT32 _ape_info_parse_old_version(MINFO_OBJ_T* pt_minfo_obj)
{
    MINFO_APE_FILE_INFO_T*          pt_ape_hdlr_obj = NULL;
    MINFO_APE_HEADER_OLD_INFO_T*    pt_ape_head_old = NULL;
    MINFO_APE_WAVEHEADER_INFO_T*    pt_wav_header = NULL;
    MINFO_APE_AUDIO_INFO_T*         pt_aud_info = NULL;
    INT32                           i4_ret = 0;
    SIZE_T                          z_buf_len = 0;
    UINT32                          ui4_read_num = 0;
    UINT32                          ui4_seektable_bytes = 0;

    if (pt_minfo_obj == NULL)
    {
        return MINFOR_INV_ARG;
    }

    /*parse ape header*/
    pt_ape_hdlr_obj = (MINFO_APE_FILE_INFO_T*)pt_minfo_obj->pv_handler_obj;
    pt_ape_hdlr_obj->b_is_old = TRUE;

    pt_ape_head_old = &pt_ape_hdlr_obj->t_ape_header_old_info;
    i4_ret = _ape_info_set_pos(pt_minfo_obj , pt_ape_hdlr_obj->ui8_curr_parser_offset, FM_SEEK_BGN);
    if (i4_ret != MINFOR_OK)
    {
        DBG_ERROR(("ERROR: seek to ape desc fail. \r\n"));
        return i4_ret;
    }
    
    z_buf_len = sizeof(MINFO_APE_HEADER_OLD_INFO_T);
    i4_ret = _ape_info_copy_bytes(pt_minfo_obj,
                    (VOID*)pt_ape_head_old,
                    z_buf_len,
                    z_buf_len, 
                    &ui4_read_num);
    if (i4_ret != INPUTR_OK || ui4_read_num != z_buf_len)
    {
        DBG_INFO(("APE INFO: read ape header old error!\r\n"));
        return MINFOR_INTERNAL_ERR;
    }

    if (pt_ape_head_old->ui4_totalframes == 0)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    pt_ape_hdlr_obj->ui8_curr_parser_offset += z_buf_len;

    if (pt_ape_head_old->ui2_formatflags & MAC_FORMAT_FLAG_HAS_PEAK_LEVEL)
    {
        pt_ape_hdlr_obj->ui8_curr_parser_offset += 4;
    }

    if (pt_ape_head_old->ui2_formatflags & MAC_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
    {
        i4_ret = _ape_info_set_pos(pt_minfo_obj , pt_ape_hdlr_obj->ui8_curr_parser_offset, FM_SEEK_BGN);
        if (i4_ret != MINFOR_OK)
        {
            DBG_ERROR(("ERROR: seek to seektable fail. \r\n"));
            return i4_ret;
        }
        
        z_buf_len = sizeof(UINT32);
        i4_ret = _ape_info_copy_bytes(pt_minfo_obj,
                        (VOID*)&ui4_seektable_bytes,
                        z_buf_len,
                        z_buf_len, 
                        &ui4_read_num);
        if (i4_ret != INPUTR_OK || ui4_read_num != z_buf_len)
        {
            DBG_INFO(("APE INFO: read ape header old error!\r\n"));
            return MINFOR_INTERNAL_ERR;
        }

        ui4_seektable_bytes *= 4;
        pt_ape_hdlr_obj->ui8_seektablebytes = ui4_seektable_bytes;
        pt_ape_hdlr_obj->ui8_curr_parser_offset += 4;
    }
    else
    {
        pt_ape_hdlr_obj->ui8_seektablebytes = pt_ape_head_old->ui4_totalframes * 4;
    }
    
    /* parse wav header*/
    if (!(pt_ape_head_old->ui2_formatflags & MAC_FORMAT_FLAG_CREATE_WAV_HEADER))
    {
        pt_wav_header = &pt_ape_hdlr_obj->t_ape_wavheader_info;
        i4_ret = _ape_info_set_pos(pt_minfo_obj , pt_ape_hdlr_obj->ui8_curr_parser_offset, FM_SEEK_BGN);
        if (i4_ret != MINFOR_OK)
        {
            DBG_ERROR(("ERROR: seek to wav header fail. \r\n"));
            return i4_ret;
        }
        
        z_buf_len = sizeof(MINFO_APE_WAVEHEADER_INFO_T);
        i4_ret = _ape_info_copy_bytes(pt_minfo_obj,
                        (VOID*)pt_wav_header,
                        z_buf_len,
                        z_buf_len, 
                        &ui4_read_num);
        if (i4_ret != INPUTR_OK || ui4_read_num != z_buf_len)
        {
            DBG_INFO(("APE INFO: read wav header error!\r\n"));
            return MINFOR_INTERNAL_ERR;
        }

        pt_ape_hdlr_obj->ui8_curr_parser_offset += sizeof(MINFO_APE_WAVEHEADER_INFO_T);
    }

    pt_ape_hdlr_obj->ui8_seektableoffset = pt_ape_hdlr_obj->ui8_curr_parser_offset;
    pt_ape_hdlr_obj->ui8_curr_parser_offset += pt_ape_head_old->ui4_totalframes * 4;

     /*aud info */
    pt_aud_info = &pt_ape_hdlr_obj->t_ape_audio_info;
    pt_aud_info->b_audio_only = FALSE;
    pt_aud_info->ui2_channels = pt_ape_head_old->ui2_channels;
    pt_aud_info->ui2_compresslevel = pt_ape_head_old->ui2_compresslevel;
    pt_aud_info->ui4_finalframeblocks = pt_ape_head_old->ui4_finalframeblocks;
    pt_aud_info->ui4_samplerate = pt_ape_head_old->ui4_samplerate;
    pt_aud_info->ui4_totalframes = pt_ape_head_old->ui4_totalframes;
    pt_aud_info->ui4_version = pt_ape_head_old->ui4_version;
    pt_aud_info->ui2_bitpersample = (pt_ape_head_old->ui2_formatflags & MAC_FORMAT_FLAG_8_BIT) ? 8 : ((pt_ape_head_old->ui2_formatflags & MAC_FORMAT_FLAG_24_BIT) ? 24 : 16);

    return MINFOR_OK;
}

INT32 ape_parse_elements(MINFO_OBJ_T* pt_minfo_obj)
{
    MINFO_APE_FILE_INFO_T*          pt_ape_hdlr_obj = NULL;
    MINFO_APE_HEADER_OLD_INFO_T*    pt_ape_header_old = NULL;
    MINFO_APE_COMMON_HEAD_T         t_ape_common_head;  
    INT32                           i4_ret = 0;
    SIZE_T                          z_buf_len = 0;
    UINT32                          ui4_read_num = 0;
    UINT32                          ui4_blockperframe = 0;
    UINT32                          ui4_totalblocks = 0;
    UINT32                          ui4_finalframeblocks = 0;
    UINT32                          ui4_totalframes = 0;
    UINT32                          ui4_samplerate = 0;

    DBG_INFO(("ape parse begin!\n"));
    
    if ((pt_minfo_obj == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_input4 == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_copybytes == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_set_pos == NULL)
        || (pt_minfo_obj->t_input_fct_tbl.pf_get_pos == NULL))
    {
        DBG_ERROR(("ERROR: ape_file_minfo_parse fail.\n"));
        return MINFOR_INTERNAL_ERR;
    }

    pt_ape_hdlr_obj = (MINFO_APE_FILE_INFO_T*)pt_minfo_obj->pv_handler_obj;

    i4_ret = _ape_info_set_pos(pt_minfo_obj , 0, FM_SEEK_END);
    if (i4_ret != MINFOR_OK)
    {
        DBG_ERROR(("ERROR: seek to file end fail. \r\n"));
        return i4_ret;
    }
    _ape_info_get_pos(pt_minfo_obj, &(pt_ape_hdlr_obj->ui8_filesize));
    
    if (pt_ape_hdlr_obj->ui8_filesize == (UINT32)(-1))
    {
        pt_minfo_obj->b_filesize_non_gettable = TRUE;
    }
    
    /*seek to beginning*/
    i4_ret = _ape_info_set_pos(pt_minfo_obj , pt_minfo_obj->pt_media_format->ui8_format_magic, FM_SEEK_BGN);
    
    pt_ape_hdlr_obj->ui8_curr_parser_offset = pt_minfo_obj->pt_media_format->ui8_format_magic;

    x_memset(&t_ape_common_head, 0, sizeof(MINFO_APE_COMMON_HEAD_T));
    z_buf_len = sizeof(t_ape_common_head);
    i4_ret = _ape_info_copy_bytes(pt_minfo_obj, 
                    (VOID*)&t_ape_common_head, 
                    z_buf_len, 
                    z_buf_len, 
                    &ui4_read_num);
    if (i4_ret != INPUTR_OK || ui4_read_num != z_buf_len)
    {
        DBG_INFO(("APE INFO: read buffer error!\r\n"));
        return MINFOR_INTERNAL_ERR;
    }

    if (t_ape_common_head.ui1_id[0] != 'M'
       || t_ape_common_head.ui1_id[1] != 'A' 
       || t_ape_common_head.ui1_id[2] != 'C' 
       || t_ape_common_head.ui1_id[3] != ' ')
    {
        DBG_INFO(("file header wrong!\n"));
        return MINFOR_INTERNAL_ERR;
    }
    
    if (t_ape_common_head.ui4_version >= NEW_VERSION)
    {
        i4_ret = _ape_info_parse_current_version(pt_minfo_obj);
    }
    else
    {
        i4_ret = _ape_info_parse_old_version(pt_minfo_obj);
    }

    if (i4_ret != MINFOR_OK)
    {
        return MINFOR_INITIAL_ERR;
    }
    
    pt_ape_hdlr_obj->t_ape_audio_info.e_enc = MINFO_INFO_AUD_ENC_MONKEY;

    if (pt_ape_hdlr_obj->b_is_old)
    {
        pt_ape_header_old = &pt_ape_hdlr_obj->t_ape_header_old_info;
        ui4_finalframeblocks = pt_ape_hdlr_obj->t_ape_header_old_info.ui4_finalframeblocks;
        ui4_blockperframe = ((pt_ape_header_old->ui4_version >= 3900) || ((pt_ape_header_old->ui4_version >= 3800) && (pt_ape_header_old->ui2_compresslevel == COMPRESSION_LEVEL_EXTRA_HIGH)))? 73728 : 9216;
        if (pt_ape_header_old->ui4_version >= 3950)
        {
            ui4_blockperframe = 73728 * 4;
        }
        ui4_totalframes = pt_ape_header_old->ui4_totalframes;
        ui4_samplerate = pt_ape_header_old->ui4_samplerate;
        pt_ape_hdlr_obj->t_ape_audio_info.ui4_blockperframe = ui4_blockperframe;
    }
    else
    {
        ui4_finalframeblocks = pt_ape_hdlr_obj->t_ape_header_info.ui4_finalframeblocks;
        ui4_blockperframe = pt_ape_hdlr_obj->t_ape_header_info.ui4_blockperframe;
        ui4_totalframes = pt_ape_hdlr_obj->t_ape_header_info.ui4_totalframes;
        ui4_samplerate = pt_ape_hdlr_obj->t_ape_header_info.ui4_samplerate;
    }

    ui4_totalblocks = (ui4_totalframes == 0) ? 0 : ((ui4_totalframes -  1) * ui4_blockperframe) + ui4_finalframeblocks;

    if (ui4_samplerate != 0)
    {
        pt_ape_hdlr_obj->ui8_duration = _mm_div64((UINT64)ui4_totalblocks * 90000 , (UINT64)ui4_samplerate, NULL);
    }
    
    DBG_INFO(("ape info parse end, duration = %llu\n", pt_ape_hdlr_obj->ui8_duration));
    
    return MINFOR_OK;
}


