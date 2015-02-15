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
 * $RCSfile: mkv_minfo.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/3 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 0a0e5908fac0e73885ce39efe82bcd61 $ Asa
 *
 * Description: 
 *         This file contains all the media info public APIs  
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
#include "u_minfo.h"
#include "mutil/minfo/minfo.h"
#include "x_dbg.h"
#ifndef __NO_FM__
#include "x_fm.h"
#endif
#include "x_lnk_list.h"
#include "mkv_parser.h"
#include "mutil/mm_util.h"
#include "mkv_parser.h"
#include "x_uc_str.h"
//#include "x_divx_drm.h"

#else

#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "../u_minfo.h"
#include "../minfo.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "mkv_parser.h"
#include "x_util.h"
#include "../../mm_util.h"
#include "mkv_parser.h"
#include "x_uc_str.h"
//#include "../../../drm/divx_drm/x_divx_drm.h"

#endif
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name:  mkv_minfo_is_handler
 *
 * Description: This function confirm media format could be handled or not.
 *
 * Inputs:  ps_path_name specify the path name of the file
 *          pv_obj       specify the media info object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mkv_minfo_is_handler(VOID*    pv_obj, MEDIA_FORMAT_T* pt_format)
{
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL || 
        pt_format == NULL)
    {
        return MINFOR_INV_ARG;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/    
    if((pt_format->e_media_type == MEDIA_TYPE_CONTAINER) && 
     (pt_format->t_media_subtype.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MATROSKA))
    {
       return MINFOR_OK;
    }
    else
    {
       return MINFOR_FMT_NOT_SUPPORT;
    }
}

/*-----------------------------------------------------------------------------
 * Name:  mkv_minfo_fopen
 *
 * Description: This function open a handler for a file
 *
 * Inputs:  ps_path_name specify the path name of the file
 *          pv_obj       specify the media info object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mkv_minfo_fopen(
                        CHAR*              ps_path_name, 
                        VOID*              pv_obj)
{
    MINFO_OBJ_T*    pt_minfo_obj;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_obj == NULL)
        || (ps_path_name == NULL))
    {
        return MINFOR_INV_ARG;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
    if (pt_minfo_obj->pv_handler_obj)
    {
        _mm_util_mem_free(pt_minfo_obj->pv_handler_obj);
        pt_minfo_obj->pv_handler_obj = NULL;
    }
    pt_minfo_obj->pv_handler_obj = _mm_util_mem_alloc(sizeof(MINFO_MKV_INF_T));
    if (pt_minfo_obj->pv_handler_obj == NULL)
    {
        return MINFOR_OUT_OF_MEMORY;
    }
    x_memset(pt_minfo_obj->pv_handler_obj, 0, sizeof(MINFO_MKV_INF_T));
    /*Willy : temp use frame buffer dram to store parse data , should be verified later. */
    ((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->pui1_header_buf 
        = _mm_util_mem_alloc(sizeof(UINT8) * MKV_HEADER_BUF_LENGTH);
    ((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->ui4_buf_length = MKV_HEADER_BUF_LENGTH;
    if (((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->pui1_header_buf  == NULL)
    {
         return MINFOR_OUT_OF_MEMORY;
    }
    x_memset(((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->pui1_header_buf , 
             0, 
             sizeof(UINT8) * MKV_HEADER_BUF_LENGTH);
    
    return MINFOR_OK;
}

/*-----------------------------------------------------------------------------
 * Name:  mkv_minfo_mopen
 *
 * Description: This function open a handler for a memory buffer
 *
 * Inputs:  pv_mem_data  specify the address of the memory buffer
 *          z_mem_leng   specify the length of the buffer
 *          pv_obj       specify the media info object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mkv_minfo_mopen(
                        VOID*              pv_mem_data, 
                        SIZE_T             z_mem_leng, 
                        VOID*              pv_obj)
{
    MINFO_OBJ_T*    pt_minfo_obj;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_obj == NULL)
        || (pv_mem_data == NULL))
    {
        return MINFOR_INV_ARG;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
    if (pt_minfo_obj->pv_handler_obj)
    {
        _mm_util_mem_free(pt_minfo_obj->pv_handler_obj);
        pt_minfo_obj->pv_handler_obj = NULL;
    }
    pt_minfo_obj->pv_handler_obj = _mm_util_mem_alloc(sizeof(MINFO_MKV_INF_T));
    if (pt_minfo_obj->pv_handler_obj == NULL)
    {
        return MINFOR_OUT_OF_MEMORY;
    }
    x_memset(pt_minfo_obj->pv_handler_obj, 0, sizeof(MINFO_MKV_INF_T));
    /*Willy : temp use frame buffer dram to store parse data , should be verified later. */
    ((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->pui1_header_buf = 
        _mm_util_mem_alloc(sizeof(UINT8) * MKV_HEADER_BUF_LENGTH);
    ((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->ui4_buf_length = MKV_HEADER_BUF_LENGTH;
    if (((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->pui1_header_buf  == NULL)
    {
         return MINFOR_OUT_OF_MEMORY;
    }
    x_memset(((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj))->pui1_header_buf , 
             0, 
             sizeof(UINT8) * MKV_HEADER_BUF_LENGTH);    
    return MINFOR_OK;
}    

/*-----------------------------------------------------------------------------
 * Name:  mkv_minfo_close
 *
 * Description: This function close a handler
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mkv_minfo_close( 
                        VOID*              pv_obj)
{
    MINFO_OBJ_T*                    pt_minfo_obj = NULL;
    MINFO_MKV_INF_T*                pt_mkv_info = NULL;
    MINFO_MKV_SEEK_ENTRY_T*         pt_seek_head = NULL;
    MINFO_MKV_VIDEO_STRM_INFO_T*    pt_vid_strm_head = NULL;
    MINFO_MKV_AUDIO_STRM_INFO_T*    pt_aud_strm_head = NULL;
    MINFO_MKV_SUBTITLE_STRM_INFO_T* pt_sp_strm_head = NULL;
    UINT8*                          pui1_drm_info_head = NULL;
    MINFO_TAG_INFO_T*               pt_tag_info = NULL;    
    MINFO_SIMPLE_TAG_INFO_T*        pt_simple_tag = NULL;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)  
    {
        return MINFOR_INV_ARG;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
    pt_mkv_info = ((MINFO_MKV_INF_T*)(pt_minfo_obj->pv_handler_obj));
    
    if (pt_mkv_info->pui1_header_buf != NULL)
    {
        _mm_util_mem_free(pt_mkv_info->pui1_header_buf);
        pt_mkv_info->pui1_header_buf  = NULL;
    }
    
    do
    {
        /*free segment info*/
        if (pt_mkv_info->t_segment_info.pws_segment_file_name != NULL)
        {
            _mm_util_mem_free(pt_mkv_info->t_segment_info.pws_segment_file_name);
            pt_mkv_info->t_segment_info.pws_segment_file_name = NULL;
        }
        if (pt_mkv_info->t_segment_info.pws_title != NULL)
        {
            _mm_util_mem_free(pt_mkv_info->t_segment_info.pws_title);
            pt_mkv_info->t_segment_info.pws_title = NULL;
        }
        
        /*free seek head info*/
        pt_seek_head = pt_mkv_info->t_all_seekhead_info.pt_seek_entry_head;
        while (pt_seek_head != NULL)
        {
            MINFO_MKV_SEEK_ENTRY_T* pt_seek_tmp = pt_seek_head->pt_next_seek_entry;
            _mm_util_mem_free(pt_seek_head);
            pt_seek_head = pt_seek_tmp;
        }

        /*free attachment info*/
        /*pt_attach_head = pt_mkv_info->t_all_attach.pt_attach_file_head;
        while (pt_attach_head != NULL)
        {
            MINFO_MKV_ATTACH_FILE_T* pt_attach_tmp = pt_attach_head->pt_next_attach_file;
            if (pt_attach_head->psz_file_description != NULL)
            {
                _mm_util_mem_free(pt_attach_head->psz_file_description);
                pt_attach_head->psz_file_description = NULL;
            }
            if (pt_attach_head->psz_file_mimetype != NULL)
            {
                _mm_util_mem_free(pt_attach_head->psz_file_mimetype);
                pt_attach_head->psz_file_mimetype = NULL;
            }
            if (pt_attach_head->psz_file_name != NULL)
            {
                _mm_util_mem_free(pt_attach_head->psz_file_name);
                pt_attach_head->psz_file_name = NULL;
            }
            _mm_util_mem_free(pt_attach_head);
            pt_attach_head = pt_attach_tmp;
        }*/
        
        /*free tracks info*/
        pui1_drm_info_head = pt_mkv_info->t_minfo_drm_info.u.t_minfo_divx_drm_obj.pui1_strd_chunk;
        if (pui1_drm_info_head != NULL)
        {
            _mm_util_mem_free(pui1_drm_info_head);
            pui1_drm_info_head = NULL;
        }
        pt_vid_strm_head = pt_mkv_info->t_all_strm_info.pt_video_strm_info_head;
        while (pt_vid_strm_head != NULL)
        {
            MINFO_MKV_VIDEO_STRM_INFO_T* pt_vid_tmp = pt_vid_strm_head->pt_next_video_strm;
            if (pt_vid_strm_head->t_video_info.pui1_codec_info != NULL)
            {
                _mm_util_mem_free(pt_vid_strm_head->t_video_info.pui1_codec_info);
                pt_vid_strm_head->t_video_info.pui1_codec_info = NULL;
            }
            if (pt_vid_strm_head->pui1_track_name != NULL)        
            {            
                _mm_util_mem_free(pt_vid_strm_head->pui1_track_name);
                pt_vid_strm_head->pui1_track_name = NULL;        
            }
            while(pt_vid_strm_head->t_video_info.pt_content_encoding)
            {
                MINFO_MKV_CONTENT_ENCODING_T* pt_cont_tmp = pt_vid_strm_head->t_video_info.pt_content_encoding->pt_next;
                if(pt_vid_strm_head->t_video_info.pt_content_encoding->pui1_comp_settings_data)
                {
                    _mm_util_mem_free(pt_vid_strm_head->t_video_info.pt_content_encoding->pui1_comp_settings_data);
                    pt_vid_strm_head->t_video_info.pt_content_encoding->pui1_comp_settings_data = NULL;
                }
                _mm_util_mem_free(pt_vid_strm_head->t_video_info.pt_content_encoding);
                pt_vid_strm_head->t_video_info.pt_content_encoding = pt_cont_tmp;
            }
            _mm_util_mem_free(pt_vid_strm_head);
            pt_vid_strm_head = pt_vid_tmp;
        }
        pt_aud_strm_head = pt_mkv_info->t_all_strm_info.pt_audio_strm_info_head;
        while (pt_aud_strm_head != NULL)
        {
            MINFO_MKV_AUDIO_STRM_INFO_T* pt_aud_tmp = pt_aud_strm_head->pt_next_audio_strm;
            if (pt_aud_strm_head->t_audio_info.pui1_codec_info != NULL)
            {
                _mm_util_mem_free(pt_aud_strm_head->t_audio_info.pui1_codec_info);
                pt_aud_strm_head->t_audio_info.pui1_codec_info = NULL;
            }
            if (pt_aud_strm_head->pui1_track_name != NULL)        
            {            
                _mm_util_mem_free(pt_aud_strm_head->pui1_track_name);
                pt_aud_strm_head->pui1_track_name = NULL;
            }
            while(pt_aud_strm_head->t_audio_info.pt_content_encoding)
            {
                MINFO_MKV_CONTENT_ENCODING_T* pt_cont_tmp = pt_aud_strm_head->t_audio_info.pt_content_encoding->pt_next;
                if(pt_aud_strm_head->t_audio_info.pt_content_encoding->pui1_comp_settings_data)
                {
                    _mm_util_mem_free(pt_aud_strm_head->t_audio_info.pt_content_encoding->pui1_comp_settings_data);
                    pt_aud_strm_head->t_audio_info.pt_content_encoding->pui1_comp_settings_data = NULL;
                }
                _mm_util_mem_free(pt_aud_strm_head->t_audio_info.pt_content_encoding);
                pt_aud_strm_head->t_audio_info.pt_content_encoding = pt_cont_tmp;
            }
            if(pt_aud_strm_head->t_audio_info.pui1_codec_private)
            {
                _mm_util_mem_free(pt_aud_strm_head->t_audio_info.pui1_codec_private);
                pt_aud_strm_head->t_audio_info.pui1_codec_private = NULL;
            }
            if(pt_aud_strm_head->t_audio_info.pt_md5_signature)
            {
                _mm_util_mem_free(pt_aud_strm_head->t_audio_info.pt_md5_signature);
                pt_aud_strm_head->t_audio_info.pt_md5_signature = NULL;
            }
            _mm_util_mem_free(pt_aud_strm_head);
            pt_aud_strm_head = pt_aud_tmp;
        }
        pt_sp_strm_head = pt_mkv_info->t_all_strm_info.pt_subtitle_strm_info_head;
        while (pt_sp_strm_head != NULL)
        {
            MINFO_MKV_SUBTITLE_STRM_INFO_T* pt_sp_tmp = pt_sp_strm_head->pt_next_subtitle_strm;
            if (pt_sp_strm_head->t_subtitle_info.pui1_codec_info != NULL)
            {
                _mm_util_mem_free(pt_sp_strm_head->t_subtitle_info.pui1_codec_info);
                pt_sp_strm_head->t_subtitle_info.pui1_codec_info = NULL;
            }
            if (pt_sp_strm_head->t_subtitle_info.pui1_content_encodings != NULL)
            {
                _mm_util_mem_free(pt_sp_strm_head->t_subtitle_info.pui1_content_encodings);
                pt_sp_strm_head->t_subtitle_info.pui1_content_encodings = NULL;
            }
            if (pt_sp_strm_head->pui1_track_name != NULL)        
            {            
                _mm_util_mem_free(pt_sp_strm_head->pui1_track_name);
                pt_sp_strm_head->pui1_track_name = NULL;        
            }
            while(pt_sp_strm_head->t_subtitle_info.pt_content_encoding)
            {
                MINFO_MKV_CONTENT_ENCODING_T* pt_cont_tmp = pt_sp_strm_head->t_subtitle_info.pt_content_encoding->pt_next;
                if(pt_sp_strm_head->t_subtitle_info.pt_content_encoding->pui1_comp_settings_data)
                {
                    _mm_util_mem_free(pt_sp_strm_head->t_subtitle_info.pt_content_encoding->pui1_comp_settings_data);
                }
                _mm_util_mem_free(pt_sp_strm_head->t_subtitle_info.pt_content_encoding);
                pt_sp_strm_head->t_subtitle_info.pt_content_encoding = pt_cont_tmp;
            }
            _mm_util_mem_free(pt_sp_strm_head);
            pt_sp_strm_head = pt_sp_tmp;
        }

        /*free meta info*/
        if (pt_mkv_info->t_meta_info.pws_copyright != NULL)
        {
            _mm_util_mem_free(pt_mkv_info->t_meta_info.pws_copyright);
            pt_mkv_info->t_meta_info.pws_copyright = NULL;
        }
        if (pt_mkv_info->t_meta_info.pws_director != NULL)
        {
            _mm_util_mem_free(pt_mkv_info->t_meta_info.pws_director);
            pt_mkv_info->t_meta_info.pws_director = NULL;
        }
        if (pt_mkv_info->t_meta_info.pws_genre != NULL)
        {
            _mm_util_mem_free(pt_mkv_info->t_meta_info.pws_genre);
            pt_mkv_info->t_meta_info.pws_genre = NULL;
        }
        
        if (pt_mkv_info->t_meta_info.pws_rating != NULL)
        {
            _mm_util_mem_free(pt_mkv_info->t_meta_info.pws_rating);
            pt_mkv_info->t_meta_info.pws_rating = NULL;
        }
        
        pt_minfo_obj->t_minfo_drm_info.t_minfo_drm_type = DRM_TYPE_NONE;
        if (pt_minfo_obj->t_minfo_drm_info.u.t_minfo_divx_drm_obj.pui1_strd_chunk != NULL)
        {
            _mm_util_mem_free(pt_minfo_obj->t_minfo_drm_info.u.t_minfo_divx_drm_obj.pui1_strd_chunk);
            pt_minfo_obj->t_minfo_drm_info.u.t_minfo_divx_drm_obj.pui1_strd_chunk = NULL;
        }
        while (pt_mkv_info->pt_tag_info != NULL)    
        {        
            pt_tag_info = pt_mkv_info->pt_tag_info;
            if (pt_tag_info->pt_tag_targets != NULL)
            {            
                _mm_util_mem_free(pt_tag_info->pt_tag_targets);
                pt_tag_info->pt_tag_targets = NULL;        
            }                
            while(pt_tag_info->pt_simple_tag != NULL)
            {            
                pt_simple_tag = pt_tag_info->pt_simple_tag;
                if (pt_simple_tag->ps_tagname != NULL)
                {                
                    _mm_util_mem_free(pt_simple_tag->ps_tagname);
                    pt_simple_tag->ps_tagname = NULL;
                }            
                if (pt_simple_tag->pws_tagstring != NULL)
                {                
                    _mm_util_mem_free(pt_simple_tag->pws_tagstring);
                    pt_simple_tag->pws_tagstring = NULL;
                }            
                if (pt_simple_tag->ps_taglang != NULL) 
                {                
                    _mm_util_mem_free(pt_simple_tag->ps_taglang);
                    pt_simple_tag->ps_taglang = NULL;            
                }            
                pt_tag_info->pt_simple_tag = pt_tag_info->pt_simple_tag->pt_next;
                _mm_util_mem_free(pt_simple_tag);
                pt_simple_tag = NULL;        
            }                
            pt_mkv_info->pt_tag_info = pt_mkv_info->pt_tag_info->pt_next;
            _mm_util_mem_free(pt_tag_info);        
            pt_tag_info = NULL;    
        }
#ifdef DIVX_PLUS_CER
        /*free chapter info*/
        while(pt_mkv_info->t_chapters_info.pt_edition)
        {
            while(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom)
            {
                while(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display)
                {
                    if(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display->ps_country)
                    {
                        _mm_util_mem_free(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display->ps_country);
                    }
                    if(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display->ps_language)
                    {
                        _mm_util_mem_free(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display->ps_language);
                    }
                    if(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display->ps_string)
                    {
                        _mm_util_mem_free(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display->ps_string);
                    }
                    pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_cur_display = 
                        pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display;
                    pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display = 
                        pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_display->pt_next;
                    _mm_util_mem_free(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_cur_display);
                }
                while(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_track)
                {
                    pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_cur_track= 
                        pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_track;
                    pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_track= 
                        pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_track->pt_next;
                    _mm_util_mem_free(pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_cur_track);
                }
                pt_mkv_info->t_chapters_info.pt_edition->pt_curr_chap = 
                    pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom;
                pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom = 
                    pt_mkv_info->t_chapters_info.pt_edition->pt_chap_atom->pt_next;
                _mm_util_mem_free(pt_mkv_info->t_chapters_info.pt_edition->pt_curr_chap);
                
            }
            pt_mkv_info->t_chapters_info.pt_curr_parse_entry = 
                pt_mkv_info->t_chapters_info.pt_edition;
            pt_mkv_info->t_chapters_info.pt_edition = 
                pt_mkv_info->t_chapters_info.pt_edition->pt_next;
            _mm_util_mem_free(pt_mkv_info->t_chapters_info.pt_curr_parse_entry);
        }
#endif

        pt_mkv_info = pt_mkv_info->pt_next;
        _mm_util_mem_free(pt_minfo_obj->pv_handler_obj);
        pt_minfo_obj->pv_handler_obj = pt_mkv_info;
    }while(pt_mkv_info);

    if (pt_minfo_obj->pui1_drm_info != NULL)
    {
        x_mem_free(pt_minfo_obj->pui1_drm_info);
        pt_minfo_obj->pui1_drm_info = NULL;
    }
    return MINFOR_OK;
}    

/*-----------------------------------------------------------------------------
 * Name:  mkv_minfo_parse
 *
 * Description: This function start parsing the infomation
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mkv_minfo_parse( 
                        VOID*              pv_obj)
{
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("mkv_minfo_parse:pv_obj is NULL\r\n"));
        return MINFOR_INITIAL_ERR;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    /* parse info */
    return _mkv_parse_elements((MINFO_OBJ_T*)pv_obj);

} 

INT32   _mkv_minfo_get_disp_info(MINFO_OBJ_T*           pt_minfo_obj,
                                         MINFO_DISP_INFO_T*     pt_disp_info)
{
    MINFO_MKV_INF_T*                pt_mkv_handler;
#ifdef DIVX_PLUS_CER
    MINFO_MKV_EDITION_T*            pt_edition              = NULL;
    MINFO_MKV_CHAP_ATOM_T*          pt_chap                 = NULL;
    UINT32                          ui4_title_idx;
    UINT32                          ui4_pl_idx;
    UINT32                          ui4_ch_idx;
    UINT32                          ui4_title_hide          = 0;
#endif
    MINFO_MKV_AUDIO_STRM_INFO_T*    pt_audio_track_info     = NULL;
    MINFO_MKV_SUBTITLE_STRM_INFO_T* pt_subtitle_track_info  = NULL;
    INT32                           i4_ret                  = MINFOR_OK;
    MINFO_TAG_INFO_T*               pt_tag_info             = NULL;
    MINFO_SIMPLE_TAG_INFO_T*        pt_simple_tag           = NULL;
    UINT32                          ui4_track_idx;
    CHAR                            ps_name[128];
    UINT32                          ui4_len;
    UINT32*                         pui4_len                = &ui4_len;

    if(pt_disp_info == NULL)
    {
        return MINFOR_INV_ARG;
    }

#ifdef DIVX_PLUS_CER
    ui4_title_idx  = pt_disp_info->ui4_title_idx;
    ui4_pl_idx     = pt_disp_info->ui4_playlist_idx;
    ui4_ch_idx     = pt_disp_info->ui4_chap_idx;
#endif
    ui4_track_idx  = pt_disp_info->ui4_track_idx;
    ui4_len        = pt_disp_info->ui4_utf_len;
#ifdef DIVX_PLUS_CER
    DBG_LOG_PRINT(("[MKV MINFO]get disp index(%d,%d,%d,%d),type = %d\n", ui4_title_idx, ui4_pl_idx, ui4_ch_idx, ui4_track_idx, pt_disp_info->e_type));
#endif
    x_memset(ps_name, 0, 128);
    pt_mkv_handler = pt_minfo_obj->pv_handler_obj;
#ifdef DIVX_PLUS_CER
    if (ui4_title_idx > 0)
    {
        while(ui4_title_idx--)
        {
            if(pt_mkv_handler->pt_next)
            {
                if(pt_mkv_handler->b_title_hide)
                {
                    ui4_title_hide ++;
                }
                pt_mkv_handler = pt_mkv_handler->pt_next;
            }
            else
            {
                return MINFOR_INFO_NOT_FOUND;
            }
        }
    }
#endif
    if(pt_mkv_handler == NULL)
    {
        return MINFOR_INFO_NOT_FOUND;
    }
    
#ifdef DIVX_PLUS_CER    
    if(pt_mkv_handler->b_title_hide)
    {
        return MINFOR_INFO_NOT_FOUND;
    }
    
    pt_edition = pt_mkv_handler->t_chapters_info.pt_edition;
    if(pt_edition == NULL && ui4_pl_idx != 0)
    {
        DBG_ERROR(("[MKV_MINFO](pt_edition == NULL && ui4_pl_idx != 0)\n"));
        return MINFOR_INFO_NOT_FOUND;
    }
    if (ui4_pl_idx > 0)
    {
        while(ui4_pl_idx--)
        {
            if(pt_edition->pt_next)
            {
                pt_edition = pt_edition->pt_next;
            }
            else
            {
                DBG_ERROR(("[MKV_MINFO]not found the edition\n"));
                return MINFOR_INFO_NOT_FOUND;
            }
        }
    }
    if(pt_edition != NULL)
    {
        DBG_LOG_PRINT(("[MKV MINFO]pt_edition id = %lld\n",pt_edition->ui8_uid));
        pt_chap = pt_edition->pt_chap_atom;
        if(pt_chap == NULL && ui4_ch_idx > 0)
        {
            DBG_ERROR(("[MKV_MINFO](pt_chap == NULL && ui4_ch_idx > 0)\n"));
            return MINFOR_INFO_NOT_FOUND;
        }
        if (ui4_ch_idx > 0)
        {
            while(ui4_ch_idx--)
            {
                if(pt_chap->pt_next)
                {
                    pt_chap = pt_chap->pt_next;
                }
                else
                {
                    DBG_ERROR(("[MKV_MINFO]not found the chapter\n"));
                    return MINFOR_INFO_NOT_FOUND;
                }
            }
        }
        
        if(pt_chap)
        {
            DBG_LOG_PRINT(("[MKV MINFO]pt_chap id = %lld\n",pt_chap->ui8_uid));
        }
    }
    else
    {
        pt_chap = NULL;
    }
#endif

    pt_tag_info = pt_mkv_handler->pt_tag_info;

    switch(pt_disp_info->e_type)
    {
#ifdef DIVX_PLUS_CER
        case MINFO_INFO_DISP_TYPE_TITLE_NAME:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                return MINFOR_INV_ARG;
            }
            
            while(pt_tag_info)
            {
                if (pt_tag_info->pt_tag_targets != NULL)
                {
                    if (pt_tag_info->pt_tag_targets->ui8_track_uid == 0 &&
                        pt_tag_info->pt_tag_targets->ui8_edition_uid == 0 &&
                        pt_tag_info->pt_tag_targets->ui8_chapter_uid == 0)
                    {
                        pt_simple_tag = pt_tag_info->pt_simple_tag;
                        while (pt_simple_tag != NULL)
                        {
                            if (pt_simple_tag->ps_tagname != NULL &&
                                pt_simple_tag->pws_tagstring != NULL &&
                                x_memcmp((VOID*)"TITLE", pt_simple_tag->ps_tagname, 5) == 0)
                            {                                
                                if(pt_simple_tag->ps_taglang != NULL)
                                {
                                    DBG_LOG_PRINT(("[MKV MINFO]ps_taglang = %s, e_language = %s\n", pt_simple_tag->ps_taglang, pt_minfo_obj->e_language));
                                    
                                    if(x_memcmp(pt_simple_tag->ps_taglang, pt_minfo_obj->e_language, 3) == 0)
                                    {
                                        COPY_DATA(  pt_simple_tag->pws_tagstring, 
                                                    x_uc_w2s_strlen(pt_simple_tag->pws_tagstring), 
                                                    pt_disp_info->ps_utf_name, 
                                                    &(pt_disp_info->ui4_utf_len));

                                        DBG_LOG_PRINT(("[MKV_MINFO]Get TITLE_NAME from tag %s\n", pt_simple_tag->pws_tagstring));
                                        
                                        return MINFOR_OK;
                                    }
                                }
                            }
                            pt_simple_tag = pt_simple_tag->pt_next;
                        }
                    }
                }
                
                pt_tag_info = pt_tag_info->pt_next;
            }
            
            if (pt_mkv_handler->t_segment_info.pws_title != NULL)
            {
                COPY_DATA(  pt_mkv_handler->t_segment_info.pws_title, 
                            x_uc_w2s_strlen(pt_mkv_handler->t_segment_info.pws_title), 
                            pt_disp_info->ps_utf_name, 
                            &(pt_disp_info->ui4_utf_len));
                return MINFOR_OK;
            }
            
            if (pt_disp_info->ui4_title_idx >= 0)
            {
                x_sprintf(ps_name, "This is title %ld ", pt_disp_info->ui4_title_idx + 1 - ui4_title_hide);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                return MINFOR_OK;
            }

            return MINFOR_INFO_NOT_FOUND;
        }       
        case MINFO_INFO_DISP_TYPE_TITLE_LAW_RATING:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                return MINFOR_INV_ARG;
            }
            while(pt_tag_info)
            {
                if (pt_tag_info->pt_tag_targets != NULL)
                {
                    if (pt_tag_info->pt_tag_targets->ui8_track_uid == 0 &&
                        pt_tag_info->pt_tag_targets->ui8_edition_uid == 0 &&
                        pt_tag_info->pt_tag_targets->ui8_chapter_uid == 0)
                    {  
                        pt_simple_tag = pt_tag_info->pt_simple_tag;
                        while(pt_simple_tag != NULL)
                        {
                            if (pt_simple_tag->ps_tagname != NULL &&
                                pt_simple_tag->pws_tagstring != NULL &&
                                x_memcmp((VOID*)"LAW_RATING", pt_simple_tag->ps_tagname, 10) == 0)
                                {
                                    COPY_DATA(  pt_simple_tag->pws_tagstring, 
                                            x_uc_w2s_strlen(pt_simple_tag->pws_tagstring), 
                                            pt_disp_info->ps_utf_name, 
                                            &(pt_disp_info->ui4_utf_len));
                                    return MINFOR_OK;
                                }
                                pt_simple_tag = pt_simple_tag->pt_next;
                            }
                        }
                }
                pt_tag_info = pt_tag_info->pt_next;  
            }
            return MINFOR_INFO_NOT_FOUND;
        }
        case MINFO_INFO_DISP_TYPE_EDITION_NAME:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                return MINFOR_INV_ARG;
            }

            while(pt_tag_info && pt_edition)
            {
                if (pt_tag_info->pt_tag_targets != NULL)
                {
                    if (pt_tag_info->pt_tag_targets->ui8_edition_uid == 
                        pt_edition->ui8_uid)
                    { 
                        pt_simple_tag = pt_tag_info->pt_simple_tag;
                        while (pt_simple_tag != NULL)
                        {
                            if (pt_simple_tag->ps_tagname != NULL &&
                                pt_simple_tag->pws_tagstring != NULL &&
                                x_memcmp((VOID*)"TITLE", pt_simple_tag->ps_tagname, 5) == 0)
                            {                                
                                if(pt_simple_tag->ps_taglang != NULL)
                                {
                                    DBG_LOG_PRINT(("[MKV MINFO]ps_taglang = %s, e_language = %s\n", pt_simple_tag->ps_taglang, pt_minfo_obj->e_language));
                                    
                                    if(x_memcmp(pt_simple_tag->ps_taglang, pt_minfo_obj->e_language, 3) == 0)
                                    {
                                        COPY_DATA(  pt_simple_tag->pws_tagstring, 
                                                    x_uc_w2s_strlen(pt_simple_tag->pws_tagstring), 
                                                    pt_disp_info->ps_utf_name, 
                                                    &(pt_disp_info->ui4_utf_len));

                                        DBG_LOG_PRINT(("[MKV_MINFO]Get EDITION_NAME from tag %s\n", pt_simple_tag->pws_tagstring));
                                        
                                        return MINFOR_OK;
                                    }
                                }
                            }
                            pt_simple_tag = pt_simple_tag->pt_next;
                        }
                    }
                }
                pt_tag_info = pt_tag_info->pt_next;
            }

            if (pt_disp_info->ui4_playlist_idx >= 0)
            {
                x_sprintf(ps_name, "This is edition %ld ", pt_disp_info->ui4_playlist_idx + 1);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                return MINFOR_OK;
            }
            
            return MINFOR_INFO_NOT_FOUND;
        }
        
        case MINFO_INFO_DISP_TYPE_EDITION_LAW_RATING:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                return MINFOR_INV_ARG;
            }
            while(pt_tag_info && pt_edition)
            {
                if (pt_tag_info->pt_tag_targets != NULL)
                {
                    if (pt_tag_info->pt_tag_targets->ui8_edition_uid == 
                        pt_edition->ui8_uid)
                    {
                        pt_simple_tag = pt_tag_info->pt_simple_tag;
                        while (pt_simple_tag != NULL)
                        {
                            if (pt_simple_tag->ps_tagname != NULL &&
                                pt_simple_tag->pws_tagstring != NULL &&
                                x_memcmp((VOID*)"LAW_RATING", pt_simple_tag->ps_tagname, 10) == 0)
                            {
                                COPY_DATA(  pt_simple_tag->pws_tagstring, 
                                            x_uc_w2s_strlen(pt_simple_tag->pws_tagstring), 
                                            pt_disp_info->ps_utf_name, 
                                            &(pt_disp_info->ui4_utf_len));
                                return MINFOR_OK;
                            }
                            pt_simple_tag = pt_simple_tag->pt_next;
                        }
                    }
                }
                pt_tag_info = pt_tag_info->pt_next;
            }
            if (pt_tag_info == NULL || pt_edition == NULL)
            {
                return MINFOR_INFO_NOT_FOUND;
            }
        }
        break;
        case MINFO_INFO_DISP_TYPE_CHAPTER_NAME:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                return MINFOR_INV_ARG;
            }

            if(pt_chap == NULL)
            {
                return MINFOR_INFO_NOT_FOUND;
            }
            
            while(pt_tag_info && pt_chap)
            {
                if (pt_tag_info->pt_tag_targets != NULL)
                {
                    if (pt_tag_info->pt_tag_targets->ui8_chapter_uid == 
                        pt_chap->ui8_uid)
                    {  
                        pt_simple_tag = pt_tag_info->pt_simple_tag;
                        while (pt_simple_tag != NULL)
                        {
                            if (pt_simple_tag->ps_tagname != NULL &&
                                pt_simple_tag->pws_tagstring != NULL &&
                                x_memcmp((VOID*)"TITLE", pt_simple_tag->ps_tagname, 5) == 0)
                            {
                                if(pt_simple_tag->ps_taglang != NULL)
                                {
                                    DBG_LOG_PRINT(("[MKV MINFO]ps_taglang = %s, e_language = %s\n", pt_simple_tag->ps_taglang, pt_minfo_obj->e_language));
                                    
                                    if(x_memcmp(pt_simple_tag->ps_taglang, pt_minfo_obj->e_language, 3) == 0)
                                    {
                                        
                                        COPY_DATA(  pt_simple_tag->pws_tagstring, 
                                                    x_uc_w2s_strlen(pt_simple_tag->pws_tagstring), 
                                                    pt_disp_info->ps_utf_name, 
                                                    &(pt_disp_info->ui4_utf_len));
                                        DBG_LOG_PRINT(("[MKV_MINFO]Get CHAPTER_NAME from tag %s\n", pt_simple_tag->pws_tagstring));
                                        return MINFOR_OK;
                                    }
                                }
                            }
                            pt_simple_tag = pt_simple_tag->pt_next;
                        }
                    }
                }
                pt_tag_info = pt_tag_info->pt_next;
                
            }
            
            if (pt_chap->pt_display != NULL && pt_chap->pt_display->ps_string != NULL)
            {
                COPY_CDATA( pt_chap->pt_display->ps_string, 
                            x_strlen(pt_chap->pt_display->ps_string), 
                            ps_name, 
                            pui4_len);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get CHAPTER_NAME from chapter info %s\n", pt_chap->pt_display->ps_string));
                return MINFOR_OK;
            }

            if (pt_disp_info->ui4_chap_idx >= 0)
            {
                x_sprintf(ps_name, "This is chapter %ld ", pt_disp_info->ui4_chap_idx + 1);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get CHAPTER_NAME from default name %s\n", ps_name));
                return MINFOR_OK;            
            }

            return MINFOR_INFO_NOT_FOUND;
        }
#endif
        case MINFO_INFO_DISP_TYPE_AUDIO_TRACK_FORMAT:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                return MINFOR_INV_ARG;
            }
            pt_audio_track_info = pt_mkv_handler->t_all_strm_info.pt_audio_strm_info_head;
            if (ui4_track_idx > 0)
            {
                while(ui4_track_idx --)
                {
                    if (pt_audio_track_info != NULL)
                    {
                        pt_audio_track_info = pt_audio_track_info->pt_next_audio_strm;
                    }
                }
            }
            if (pt_audio_track_info == NULL)
            {
                return MINFOR_INFO_NOT_FOUND;
            }
            switch (pt_audio_track_info->t_audio_info.e_enc)
            {
                case MINFO_INFO_AUD_ENC_AAC:
                    x_sprintf(ps_name, "AAC");
                    x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                    pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                    break;
                case MINFO_INFO_AUD_ENC_AC3:
                    x_sprintf(ps_name, "AC3");
                    x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                    pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                    break;
                case MINFO_INFO_AUD_ENC_DTS:
                    x_sprintf(ps_name, "DTS");
                    x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                    pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                    break;
                case MINFO_INFO_AUD_ENC_MPEG_1:
                    if(pt_audio_track_info->t_audio_info.e_layer == 3)
                    {
                        x_sprintf(ps_name, "MP3");
                        x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                        pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                    }
                    break;
                case MINFO_INFO_AUD_ENC_PCM:
                case MINFO_INFO_AUD_ENC_ADPCM:
                case MINFO_INFO_AUD_ENC_LPCM:
                    x_sprintf(ps_name, "PCM");
                    x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                    pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                    break;
                case MINFO_INFO_AUD_ENC_COOK:
                    x_sprintf(ps_name, "COOK");
                    x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                    pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                    break;
                case MINFO_INFO_AUD_ENC_EC3:
                    x_sprintf(ps_name, "E-AC3");
                    x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                    pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                    break;
                default:
                    DBG_LOG_PRINT(("[MKV_MINFO]t_audio_info.e_enc = %d\n", pt_audio_track_info->t_audio_info.e_enc));
                    break;
            }
        }
        break;
        case MINFO_INFO_DISP_TYPE_AUDIO_TRACK_CHANNEL_CONF:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                return MINFOR_INV_ARG;
            }
            pt_audio_track_info = pt_mkv_handler->t_all_strm_info.pt_audio_strm_info_head;
            if (ui4_track_idx > 0)
            {
                while(ui4_track_idx --)
                {
                    if (pt_audio_track_info != NULL)
                    {
                        pt_audio_track_info = pt_audio_track_info->pt_next_audio_strm;
                    }
                }
            }
            if (pt_audio_track_info == NULL)
            {
                return MINFOR_INFO_NOT_FOUND;
            }
            if (pt_audio_track_info->t_audio_info.i2_channels == 1)
            {
                x_sprintf(ps_name, "%1d Channel", pt_audio_track_info->t_audio_info.i2_channels);
            }
            else
            {
                x_sprintf(ps_name, "%1d Channels", pt_audio_track_info->t_audio_info.i2_channels);
            }
            
            x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
            pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
            return MINFOR_OK;
        }
        case MINFO_INFO_DISP_TYPE_AUDIO_TRACK_LANGUAGE:
        {
            if(pt_disp_info->ps_char_name == NULL || pt_disp_info->ui4_char_len < 4)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_LANGUAGE MINFOR_INV_ARG\n"));
                return MINFOR_INV_ARG;
            }
            pt_audio_track_info = pt_mkv_handler->t_all_strm_info.pt_audio_strm_info_head;
            if (ui4_track_idx > 0)
            {
                while(ui4_track_idx --)
                {
                    if (pt_audio_track_info != NULL)
                    {
                        pt_audio_track_info = pt_audio_track_info->pt_next_audio_strm;
                    }
                }
            }
            if (pt_audio_track_info == NULL)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_LANGUAGE pt_audio_track_info == NULL\n"));
                return MINFOR_INFO_NOT_FOUND;
            }
            
            if (pt_audio_track_info->b_has_language)
            {
                x_memcpy( pt_disp_info->ps_char_name, pt_audio_track_info->ac_language, ISO_639_LANG_LEN);
                pt_disp_info->ui4_char_len = x_strlen((char*)pt_disp_info->ps_char_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_LANGUAGE ac_language = %s\n", pt_audio_track_info->ac_language));
            }
            else
            {       
                return MINFOR_INFO_NOT_FOUND;
            }
        }
        break;
        case MINFO_INFO_DISP_TYPE_AUDIO_TRACK_NAME:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len == 0)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_NAME MINFOR_INV_ARG\n"));
                return MINFOR_INV_ARG;
            }
            pt_audio_track_info = pt_mkv_handler->t_all_strm_info.pt_audio_strm_info_head;
            if (ui4_track_idx > 0)
            {
                while(ui4_track_idx --)
                {
                    if (pt_audio_track_info != NULL)
                    {
                        pt_audio_track_info = pt_audio_track_info->pt_next_audio_strm;
                    }
                }
            }
            if (pt_audio_track_info == NULL)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_NAME pt_audio_track_info == NULL\n"));
                return MINFOR_INFO_NOT_FOUND;
            }
    
            while(pt_tag_info)
            {
                if (pt_tag_info->pt_tag_targets != NULL)
                {
                    if (pt_tag_info->pt_tag_targets->ui8_track_uid == 
                        pt_audio_track_info->ui8_track_uid)
                    {
                        pt_simple_tag = pt_tag_info->pt_simple_tag;
                        while (pt_simple_tag != NULL)
                        {
                            if (pt_simple_tag->ps_tagname != NULL &&
                                pt_simple_tag->pws_tagstring != NULL &&
                                pt_simple_tag->ps_taglang != NULL &&
                                x_memcmp((VOID*)"TITLE", pt_simple_tag->ps_tagname, 5) == 0 &&
                                x_memcmp(pt_minfo_obj->e_language, pt_simple_tag->ps_taglang, 3) == 0)
                            {
                                DBG_LOG_PRINT(("[MKV MINFO]ps_taglang = %s, e_language = %s\n", pt_simple_tag->ps_taglang, pt_minfo_obj->e_language));
                                COPY_DATA(  pt_simple_tag->pws_tagstring, 
                                            x_uc_w2s_strlen(pt_simple_tag->pws_tagstring),
                                            pt_disp_info->ps_utf_name, 
                                            &(pt_disp_info->ui4_utf_len));
                                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_NAME from tag %s\n", pt_simple_tag->pws_tagstring));
                                return MINFOR_OK;
                            }
                            pt_simple_tag = pt_simple_tag->pt_next;
                        }
                    }
                    
                }
                pt_tag_info = pt_tag_info->pt_next;
            }
                      
            if (pt_audio_track_info->pui1_track_name != NULL)
            {
                COPY_CDATA( pt_audio_track_info->pui1_track_name, 
                            pt_audio_track_info->ui4_track_name_len, 
                            ps_name, 
                            pui4_len);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_NAME from track name %s\n", pt_audio_track_info->pui1_track_name));
                return MINFOR_OK;
            }

            if (pt_disp_info->ui4_track_idx >= 0)
            {
                x_sprintf(ps_name, "This is audio track %ld ", pt_disp_info->ui4_track_idx + 1);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_NAME from default name %s\n", ps_name));
                return MINFOR_OK;
            }
            DBG_LOG_PRINT(("[MKV_MINFO]Get AUDIO_TRACK_NAME fail\n"));
            return MINFOR_INFO_NOT_FOUND;
        }
            
        case MINFO_INFO_DISP_TYPE_SUBTITLE_TRACK_LANGUAGE:
        {
            if(pt_disp_info->ps_char_name == NULL || pt_disp_info->ui4_char_len < ISO_639_LANG_LEN)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_LANGUAGE MINFOR_INV_ARG\n"));
                return MINFOR_INV_ARG;
            }
            
            pt_subtitle_track_info = pt_mkv_handler->t_all_strm_info.pt_subtitle_strm_info_head;
            if (ui4_track_idx > 0)
            {
                while(ui4_track_idx --)
                {
                    if (pt_subtitle_track_info != NULL)
                    {
                        pt_subtitle_track_info = pt_subtitle_track_info->pt_next_subtitle_strm;
                    }
                }
            }
            if (pt_subtitle_track_info == NULL)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_LANGUAGE pt_subtitle_track_info == NULL\n"));
                return MINFOR_INFO_NOT_FOUND;
            }

            if (pt_subtitle_track_info->t_subtitle_info.b_has_language)
            {
                x_memcpy(pt_disp_info->ps_char_name, pt_subtitle_track_info->t_subtitle_info.s_lang, ISO_639_LANG_LEN);
                pt_disp_info->ui4_char_len = x_strlen((char*)pt_disp_info->ps_char_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_LANGUAGE ac_language = %s\n", pt_subtitle_track_info->t_subtitle_info.s_lang));
                return MINFOR_OK;
            }
            
            return MINFOR_INFO_NOT_FOUND;
        }
        case MINFO_INFO_DISP_TYPE_SUBTITLE_TRACK_NAME:
        {
            if(pt_disp_info->ps_utf_name == NULL || pt_disp_info->ui4_utf_len < 1)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_NAME MINFOR_INV_ARG\n"));
                return MINFOR_INV_ARG;
            }
            
            pt_subtitle_track_info = pt_mkv_handler->t_all_strm_info.pt_subtitle_strm_info_head;
            if (ui4_track_idx > 0)
            {
                while(ui4_track_idx --)
                {
                    if (pt_subtitle_track_info != NULL)
                    {
                        pt_subtitle_track_info = pt_subtitle_track_info->pt_next_subtitle_strm;
                    }
                }
            }
            if (pt_subtitle_track_info == NULL)
            {
                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_NAME pt_subtitle_track_info == NULL\n"));
                return MINFOR_INFO_NOT_FOUND;
            }
    
            while(pt_tag_info)
            {
                if (pt_tag_info->pt_tag_targets != NULL)
                {
                    if (pt_tag_info->pt_tag_targets->ui8_track_uid == 
                        pt_subtitle_track_info->ui8_track_uid)
                    {
                        pt_simple_tag = pt_tag_info->pt_simple_tag;
                        while (pt_simple_tag != NULL)
                        {
                            if (pt_simple_tag->ps_tagname != NULL &&
                                pt_simple_tag->pws_tagstring != NULL &&
                                pt_simple_tag->ps_taglang != NULL &&
                                x_memcmp((VOID*)"TITLE", pt_simple_tag->ps_tagname, 5) == 0 && 
                                x_memcmp(pt_minfo_obj->e_language, pt_simple_tag->ps_taglang, 3) == 0)
                            {
                                DBG_LOG_PRINT(("[MKV MINFO]ps_taglang = %s, e_language = %s\n", pt_simple_tag->ps_taglang, pt_minfo_obj->e_language));
                                COPY_DATA(  pt_simple_tag->pws_tagstring, 
                                            x_uc_w2s_strlen(pt_simple_tag->pws_tagstring), 
                                            pt_disp_info->ps_utf_name, 
                                            &(pt_disp_info->ui4_utf_len));
                                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_NAME from tag %s\n", pt_simple_tag->pws_tagstring));
                                return MINFOR_OK;
                            }
                            pt_simple_tag = pt_simple_tag->pt_next;
                        }
                    }
                }
                pt_tag_info = pt_tag_info->pt_next;
            }
            
            if (pt_subtitle_track_info->pui1_track_name != NULL)
            {
                COPY_CDATA( pt_subtitle_track_info->pui1_track_name, 
                            pt_subtitle_track_info->ui4_track_name_len, 
                            ps_name, 
                            pui4_len);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_NAME from track name %s\n", pt_subtitle_track_info->pui1_track_name));
                return MINFOR_OK;
            }
            
            if (pt_disp_info->ui4_track_idx >= 0)
            {
                x_sprintf(ps_name, "This is subtitle track %ld ", pt_disp_info->ui4_track_idx + 1);
                x_uc_ps_to_w2s (ps_name, pt_disp_info->ps_utf_name, pt_disp_info->ui4_utf_len);
                pt_disp_info->ui4_utf_len = x_uc_w2s_strlen(pt_disp_info->ps_utf_name);
                DBG_LOG_PRINT(("[MKV_MINFO]Get SUBTITLE_TRACK_NAME from default name %s\n", ps_name));
                return MINFOR_OK;
            
            }
            
            return MINFOR_INFO_NOT_FOUND;
        }
        default:
            break;
    }
    
    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name:  mkv_minfo_get_num_recs
 *
 * Description:This function get information from a handler
 *
 * Inputs:  pv_obj                    specify the media info object  
 *             
 *
 * Outputs: pui2_num_recs       
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mkv_minfo_get_num_recs(
                        VOID*                pv_obj, 
                        MINFO_INFO_TYPE_T    e_info_type, 
                        UINT16*                pui2_num_recs)
{
     MINFO_OBJ_T*        pt_minfo_obj;
     MINFO_MKV_INF_T*    pt_mkv_info;
    
     /************************************************************************/
     /* Check parameter                                                      */
     /************************************************************************/
     if ((pv_obj == NULL)
         || (pui2_num_recs == NULL))
     {
         return MINFOR_INITIAL_ERR;
     }
     /************************************************************************/
     /* do                                                                   */
     /************************************************************************/
     pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
     pt_mkv_info = pt_minfo_obj->pv_handler_obj;
     
#ifdef DIVX_PLUS_CER
         if(e_info_type < MINFO_INFO_TYPE_TITLE_NUM)
         {
             UINT32 idx = pt_minfo_obj->ui4_cur_title;
             while(idx--)
             {
                 pt_mkv_info = pt_mkv_info->pt_next;
             }
         }
#endif 
    
     if (pt_mkv_info)
     {
         if (pt_minfo_obj->b_parsing_successful == FALSE)
         {
             return MINFOR_INFO_NOT_READY;    
         }
         
         switch(e_info_type)
         {
             case MINFO_INFO_TYPE_GENERIC:
                /*ignore ui2_next_info_num*/
                *pui2_num_recs = 1;
                break;                
             case MINFO_INFO_TYPE_VIDEO_ATTR:
                /*QQQQQ*/
                *pui2_num_recs = pt_mkv_info->t_all_strm_info.ui1_video_strm_num;
                break;
             case MINFO_INFO_TYPE_AUDIO_ATTR:
                *pui2_num_recs =  pt_mkv_info->t_all_strm_info.ui1_audio_strm_num;
                 break;
             case MINFO_INFO_TYPE_SP_ATTR:
                 *pui2_num_recs = pt_mkv_info->t_all_strm_info.ui1_subtitle_strm_num;
                 break;
             case MINFO_INFO_TYPE_IDX_INFO:
                 *pui2_num_recs = 1;
                 break;
             case MINFO_INFO_TYPE_META_DATA:
                 *pui2_num_recs = 1;
                 break;
 #if SUPPORT_MULTI_INFO
             case MINFO_INFO_TYPE_SCALE_INFO:
                 if(pt_mkv_info->pt_next)
                 {
                     *pui2_num_recs = 1;
                 }
                 else
                 {
                     *pui2_num_recs = 0;
                 }
                 break;
 #endif
             default:
                 return MINFOR_INFO_NOT_FOUND;
         }
         return MINFOR_OK;
     }
     return MINFOR_FMT_NOT_SUPPORT;
}
                        
/*-----------------------------------------------------------------------------
 * Name:  mkv_minfo_get_info
 *
 * Description:This function get information from a handler
 *
 * Inputs:  pv_obj       specify the media info object  
 *              
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mkv_minfo_get_info( 
                        VOID*                pv_obj, 
                        MINFO_INFO_TYPE_T    e_info_type, 
                        MINFO_REC_IDX_T      ui2_index,                    
                        VOID*                pv_buf, 
                        SIZE_T*              pz_buf_leng)
{
    INT32                       i4_ret;
    MINFO_OBJ_T*                pt_minfo_obj;
    MINFO_MKV_INF_T*            pt_mkv_handler;
    MINFO_INFO_T*               pt_minfo_rec;
    MINFO_TYPE_IDX_INFO_T*      pt_idx_info;
    MINFO_TYPE_META_DATA_T*     pt_meta_data;
#if SUPPORT_MULTI_INFO
    MINFO_TYPE_SCALE_INFO_T*    pt_scale;
#endif
    UINT64                      ui8_date;
     /*video es info*/
    MINFO_MKV_VIDEO_STRM_INFO_T*    pt_video_strm;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_obj == NULL)
        || (pz_buf_leng == NULL))
    {
        return MINFOR_INITIAL_ERR;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
    pt_mkv_handler = pt_minfo_obj->pv_handler_obj;
    pt_minfo_rec = NULL;
    if(pv_buf == NULL)
    {
        /*get length only*/
        *pz_buf_leng = sizeof (MINFO_INFO_T);
        return MINFOR_OK;
    }

    if(e_info_type < MINFO_INFO_TYPE_TITLE_NUM)
    {
        pt_minfo_rec = (MINFO_INFO_T*)pv_buf;
        /*x_memset(pt_minfo_rec, 0, sizeof(MINFO_INFO_T));*/
        pt_minfo_rec->e_info_type = e_info_type;
        pt_minfo_rec->ui1_rec_idx = (UINT8)ui2_index;
        pt_minfo_rec->t_format = *pt_minfo_obj->pt_media_format;

        /* get data */
        if (*pz_buf_leng < sizeof (MINFO_INFO_T))
        {
            return MINFOR_INSUFFICIENT_BUFFER;
        }
#ifdef DIVX_PLUS_CER
        {   
            UINT32 idx = pt_minfo_obj->ui4_cur_title;

            if (e_info_type == MINFO_INFO_TYPE_DRM_INFO)
            {
                while(pt_mkv_handler!= NULL && ui2_index-- )
                {
                    pt_mkv_handler = pt_mkv_handler->pt_next;
                } 
            }
            else
            {
                while(pt_mkv_handler!= NULL && idx--)
                {
                    pt_mkv_handler = pt_mkv_handler->pt_next;
                }
            }
        }
#endif
    }
   

    if (pt_mkv_handler == NULL)
    {
        return MINFOR_INFO_NOT_FOUND;
    }
    
    if (pt_minfo_obj->b_parsing_successful == FALSE)
    {
        return MINFOR_INFO_NOT_READY;    
    }
            
    switch(e_info_type)
    {
        case MINFO_INFO_TYPE_GENERIC:
            if(ui2_index > 1)
            {
               return MINFOR_OUT_OF_RECORD;
            }
            pt_minfo_rec->u.t_general_info.ui8_pb_duration      = pt_mkv_handler->ui8_duration;
            pt_minfo_rec->u.t_general_info.ui8_1st_cluster_pts  = pt_mkv_handler->ui8_1st_cluster_pts;
            pt_minfo_rec->u.t_general_info.ui8_media_sz         = pt_mkv_handler->ui8_size;
            pt_minfo_rec->u.t_general_info.ui1_strm_ns          = pt_mkv_handler->ui1_cur_strm_num; 
            pt_minfo_rec->u.t_general_info.ui8_timecode_scale   = pt_mkv_handler->t_segment_info.ui8_timecode_scale;
            pt_minfo_rec->u.t_general_info.b_is_drm_exist       = (pt_mkv_handler->t_minfo_drm_info.t_minfo_drm_type != DRM_TYPE_NONE);
            *pz_buf_leng = sizeof(MINFO_INFO_T);
            break; 
        case MINFO_INFO_TYPE_VIDEO_ATTR:
            if(ui2_index > pt_mkv_handler->t_all_strm_info.ui1_video_strm_num)
            {
                return MINFOR_OUT_OF_RECORD;
            }
            i4_ret = _mkv_get_strm_atrbt_by_idx(&(pt_mkv_handler->t_all_strm_info), 
                                                MKV_STRM_VIDEO_TYPE,
                                                ui2_index,
                                                pt_minfo_rec);
            if (MINFOR_OK!= i4_ret)
            {
                DBG_ERROR(("mkv_minfo_get_info:error for video\r\n"));
                return MINFOR_INTERNAL_ERR;    
            }
#ifndef DIVX_PLUS_CER      
            /*divx plus not support */
            if (pt_mkv_handler->b_divx_plus)
            {
                pt_minfo_rec->u.t_stm_attribute.u.t_mkv_video_stm_attr.e_enc = MINFO_INFO_VID_ENC_UNKNOWN;
                pt_minfo_rec->u.t_stm_attribute.b_is_codec_support = FALSE;
            }
#endif           
            *pz_buf_leng = sizeof(MINFO_INFO_T);
            break;
        case MINFO_INFO_TYPE_AUDIO_ATTR:
            if(ui2_index > pt_mkv_handler->t_all_strm_info.ui1_audio_strm_num)
            {
                return MINFOR_OUT_OF_RECORD;
            }             
            i4_ret = _mkv_get_strm_atrbt_by_idx(&(pt_mkv_handler->t_all_strm_info), 
                                                MKV_STRM_AUDIO_TYPE,
                                                ui2_index,
                                                pt_minfo_rec);
            if (MINFOR_OK!= i4_ret)
            {
                DBG_ERROR(("mkv_minfo_get_info:error for audio\r\n"));
                return MINFOR_INTERNAL_ERR;    
            }
#ifndef DIVX_PLUS_CER
            /*divx plus not support */
            if (pt_mkv_handler->b_divx_plus)
            {
                pt_minfo_rec->u.t_stm_attribute.u.t_mkv_audio_stm_attr.e_enc = MINFO_INFO_AUD_ENC_UNKNOWN;
                pt_minfo_rec->u.t_stm_attribute.b_is_codec_support = FALSE;
            }
#endif
            *pz_buf_leng = sizeof(MINFO_INFO_T);
            break;
        case MINFO_INFO_TYPE_CODEC:
        {
            MINFO_INFO_T* pt_minfo_mkv_rec = (MINFO_INFO_T*)pv_buf;

            if(ui2_index > pt_mkv_handler->t_all_strm_info.ui1_video_strm_num)
            {
                return MINFOR_OUT_OF_RECORD;
            }
            i4_ret = _mkv_get_strm_atrbt_by_idx(&(pt_mkv_handler->t_all_strm_info), 
                                                MKV_STRM_VIDEO_TYPE,
                                                ui2_index,
                                                pt_minfo_mkv_rec);
            if (MINFOR_OK!= i4_ret)
            {
                DBG_ERROR(("mkv_minfo_get_info:error for video\r\n"));
                return MINFOR_INTERNAL_ERR;    
            }                
            if ((pt_minfo_mkv_rec->u.t_stm_attribute.u.t_mkv_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_DIVX_311) ||
                (pt_minfo_mkv_rec->u.t_stm_attribute.u.t_mkv_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_DIVX_4) ||
                (pt_minfo_mkv_rec->u.t_stm_attribute.u.t_mkv_video_stm_attr.e_enc == MINFO_INFO_VID_ENC_DIVX_5))
            {
                pt_minfo_mkv_rec->u.t_divx_info.t_minfo_info_codec_enc = MINFO_INFO_CODEC_VID_ENC_DIVX;
            }
#ifndef DIVX_PLUS_CER
            /*divx plus not support */
            else if (pt_mkv_handler->b_divx_plus)
            {
                pt_minfo_mkv_rec->u.t_divx_info.t_minfo_info_codec_enc = MINFO_INFO_CODEC_VID_ENC_DIVX;
            }
#endif
            else
            {
                pt_minfo_mkv_rec->u.t_divx_info.t_minfo_info_codec_enc = MINFO_INFO_CODEC_VID_ENC_UNKONWN;
            }
            *pz_buf_leng = sizeof(MINFO_INFO_T);
        }
            break;
        case MINFO_INFO_TYPE_SP_ATTR:
            if(ui2_index > pt_mkv_handler->t_all_strm_info.ui1_subtitle_strm_num)
            {
                return MINFOR_OUT_OF_RECORD;
            }      
            i4_ret = _mkv_get_strm_atrbt_by_idx(&(pt_mkv_handler->t_all_strm_info), 
                                                MKV_STRM_SUBTILE_TYPE,
                                                ui2_index,
                                                pt_minfo_rec);
            if (MINFOR_OK!= i4_ret)
            {
                DBG_ERROR(("mkv_minfo_get_info:error for audio\r\n"));
                return MINFOR_INTERNAL_ERR;    
            }
            *pz_buf_leng = sizeof(MINFO_INFO_T);
            break;

        case MINFO_INFO_TYPE_FONT_DATA:
            i4_ret = _mkv_get_font_data(pt_minfo_obj,
                       &(pt_mkv_handler->t_all_strm_info), 
                       ui2_index, pt_minfo_rec);
            break;
            
        case MINFO_INFO_TYPE_IDX_INFO:
            pt_minfo_rec->e_info_type = e_info_type;
            pt_minfo_rec->ui1_rec_idx = 1;
            
            pt_idx_info = &(pt_minfo_rec->u.t_idx_info);
            x_memset(pt_idx_info, 0, sizeof(MINFO_TYPE_IDX_INFO_T));
            x_memcpy((VOID *)&(pt_minfo_rec->u.t_idx_info),
                     (VOID *)&(pt_mkv_handler->t_idx_info),
                     sizeof(MINFO_TYPE_IDX_INFO_T));
            if(pt_idx_info->ui8_data_offset + pt_idx_info->ui8_data_sz
                > pt_mkv_handler->ui8_size)
            {
                pt_idx_info->ui8_data_sz
                    = pt_mkv_handler->ui8_size - pt_idx_info->ui8_data_offset;
            }
            *pz_buf_leng = sizeof(MINFO_INFO_T);                    
            break;
        case MINFO_INFO_TYPE_DRM_INFO:
            
            if(pt_mkv_handler->ui4_drm_size == 0)
            {
                break; 
            }
            if (pt_minfo_obj->pui1_drm_info != NULL)
            {
                x_mem_free(pt_minfo_obj->pui1_drm_info);
                pt_minfo_obj->pui1_drm_info = NULL;
            }
            pt_minfo_obj->pui1_drm_info = (UINT8*)_mm_util_mem_alloc(sizeof(UINT8) * pt_mkv_handler->ui4_drm_size);
            if (pt_minfo_obj->pui1_drm_info == NULL)
            {
                return MINFOR_INTERNAL_ERR;
            }
            
            x_memset(pt_minfo_obj->pui1_drm_info, 0, pt_mkv_handler->ui4_drm_size);
            x_memcpy(pt_minfo_obj->pui1_drm_info, pt_mkv_handler->t_minfo_drm_info.u.t_minfo_divx_drm_obj.pui1_strd_chunk, pt_mkv_handler->ui4_drm_size);
            pt_minfo_rec->u.t_drm_info.t_minfo_drm_type = 
                pt_mkv_handler->t_minfo_drm_info.t_minfo_drm_type; 
            pt_minfo_rec->u.t_drm_info.u.t_minfo_divx_drm_obj.pui1_strd_chunk =pt_minfo_obj->pui1_drm_info;
            *pz_buf_leng = sizeof(MINFO_INFO_T);
            break;
        case MINFO_INFO_TYPE_META_DATA:
            pt_meta_data = &(pt_minfo_rec->u.t_meta_data);
            /*x_memset(pt_meta_data, 0, sizeof(MINFO_TYPE_META_DATA_T));*/
            pt_minfo_rec->u.t_meta_data.ui8_pb_duration = pt_mkv_handler->ui8_duration;
            
            COPY_DATA(  pt_mkv_handler->t_segment_info.pws_title, 
                        x_uc_w2s_strlen(pt_mkv_handler->t_segment_info.pws_title), 
                        pt_meta_data->pwc_str_title, 
                        &(pt_meta_data->z_title_len));
            COPY_DATA(  pt_mkv_handler->t_meta_info.pws_director, 
                        x_uc_w2s_strlen(pt_mkv_handler->t_meta_info.pws_director), 
                        pt_meta_data->pwc_str_director, 
                        &(pt_meta_data->z_director_len));
            COPY_DATA(  pt_mkv_handler->t_meta_info.pws_copyright, 
                        x_uc_w2s_strlen(pt_mkv_handler->t_meta_info.pws_copyright), 
                        pt_meta_data->pwc_str_copyright, 
                        &(pt_meta_data->z_copyright_len));
            COPY_DATA(  pt_mkv_handler->t_meta_info.pws_genre, 
                        x_uc_w2s_strlen(pt_mkv_handler->t_meta_info.pws_genre), 
                        pt_meta_data->pwc_str_genre, 
                        &(pt_meta_data->z_genre_len));
            if (pt_mkv_handler->t_meta_info.pws_rating != NULL)
            {
                COPY_DATA(  pt_mkv_handler->t_meta_info.pws_rating,                             
                            x_uc_w2s_strlen(pt_mkv_handler->t_meta_info.pws_rating),                             
                            pt_meta_data->pwc_str_rating,                             
                            &(pt_meta_data->z_rating_len));
            }
            if (pt_mkv_handler->t_segment_info.i8_date)
            {
                pt_meta_data->b_cr_date_exist = TRUE;
                ui8_date = _mm_div64((UINT64)pt_mkv_handler->t_segment_info.i8_date,
                                      1000000 * 1000,
                                      NULL);
                _mm_gmtime((MM_TIME_T)ui8_date,
                           MM_GMT_MKV_EPOCH_YEAR_M,
                           &(pt_meta_data->t_date));
            }
            if (pt_meta_data->ui8_pb_duration != 0)
            {
                pt_meta_data->ui4_bitrate = (UINT32)
                    _mm_div64((pt_mkv_handler->t_idx_info.ui8_data_sz) * 8 * MIDXBULD_SYSTEM_CLOCK_FREQUENCY, 
                               pt_meta_data->ui8_pb_duration, 
                               NULL);
            }
            if (pt_mkv_handler->t_all_strm_info.pt_video_strm_info_head)
            {
                pt_meta_data->t_frame_rate.ui4_rate = (UINT32)
                    _mm_div64((UINT64)pt_mkv_handler->t_all_strm_info.pt_video_strm_info_head->t_video_info.ui4_rate * 1000,
                              (UINT64)pt_mkv_handler->t_all_strm_info.pt_video_strm_info_head->t_video_info.ui4_scale,
                               NULL);
                pt_meta_data->t_frame_rate.ui4_scale = 1000;
            }

            /*video es info*/
            pt_video_strm = pt_mkv_handler->t_all_strm_info.pt_video_strm_info_head;        
            if (NULL != pt_video_strm)
            {
                pt_meta_data->t_video_es_info.i4_video_inf_width = 
                            pt_video_strm->t_video_info.i4_width;
                pt_meta_data->t_video_es_info.i4_video_inf_height = 
                            pt_video_strm->t_video_info.i4_height;
            }              
            
            break;
        /*add for font info*/
        case MINFO_INFO_TYPE_FONT_INFO:
            pt_minfo_rec->u.t_font_info.pt_font_info = (FONT_INFO_T*)pt_minfo_obj->pv_attach_file; 
            break; 
#if SUPPORT_MULTI_INFO
        case MINFO_INFO_TYPE_SCALE_INFO:
            if(pt_mkv_handler->pt_next)
            {
                MINFO_MKV_VIDEO_STRM_INFO_T* pt_video_strm = NULL;
                MINFO_MKV_INF_T*             pt_mkv_handler_next = NULL;
                
                pt_mkv_handler_next = pt_mkv_handler->pt_next;
                if (!(pt_mkv_handler->t_all_strm_info.t_strm_properties.fg_enh_track_flag == 0 &&
                    pt_mkv_handler_next->t_all_strm_info.t_strm_properties.fg_enh_track_flag == 1))
                {
                    return MINFOR_FMT_NOT_SUPPORT;
                }
                pt_mkv_handler = pt_mkv_handler->pt_next;
                pt_scale = &(pt_minfo_rec->u.t_scale_info);
                x_memset(pt_scale, 0, sizeof(MINFO_TYPE_SCALE_INFO_T));
                
                pt_video_strm = pt_mkv_handler->t_all_strm_info.pt_video_strm_info_head;
                if (NULL != pt_video_strm)
                {
                    pt_scale->i4_height = pt_video_strm->t_video_info.i4_height;
                    pt_scale->i4_width = pt_video_strm->t_video_info.i4_width;
                    pt_scale->ui4_scale = pt_video_strm->t_video_info.ui4_scale;
                    pt_scale->ui4_rate = pt_video_strm->t_video_info.ui4_rate;
                    pt_scale->ui4_codec_info_size = pt_video_strm->t_video_info.ui4_codec_info_size;
                    pt_scale->pui1_codec_info = pt_video_strm->t_video_info.pui1_codec_info;
                    pt_scale->e_enc = pt_video_strm->t_video_info.e_enc;
                    pt_scale->ui4_strm_id = pt_video_strm->ui4_stream_number;
                }
                else
                {
                    return MINFOR_FMT_NOT_SUPPORT;
                }
                pt_scale->ui4_idx_type = pt_mkv_handler->t_idx_info.ui4_idx_type;
                pt_scale->ui8_data_offset = pt_mkv_handler->t_idx_info.ui8_data_offset;
                pt_scale->ui8_data_sz = pt_mkv_handler->t_idx_info.ui8_data_sz;
                pt_scale->ui8_idx_offset = pt_mkv_handler->t_idx_info.ui8_idx_offset;
                pt_scale->ui8_idx_sz =pt_mkv_handler->t_idx_info.ui8_idx_sz;
                pt_scale->ui8_media_sz = pt_mkv_handler->ui8_size;
                pt_scale->ui8_pb_duration = pt_mkv_handler->ui8_duration; 
                pt_scale->ui8_segment_data_offset = pt_mkv_handler->t_idx_info.ui8_segment_data_offset;
                pt_scale->ui8_timecode_scale = pt_mkv_handler->t_segment_info.ui8_timecode_scale;
                if(pt_scale->ui8_data_offset + pt_scale->ui8_data_sz
                    > pt_scale->ui8_media_sz)
                {
                    pt_scale->ui8_data_sz
                        = pt_scale->ui8_media_sz - pt_scale->ui8_data_offset;
                }
            }
            else
            {
                return MINFOR_FMT_NOT_SUPPORT;
            }
            break;
#endif
#ifdef DIVX_PLUS_CER
        case MINFO_INFO_TYPE_TITLE_NUM:
            {
                UINT32 ui4_title_num = 1;

                pt_mkv_handler = pt_minfo_obj->pv_handler_obj;
                while(pt_mkv_handler->pt_next)
                {
                    ui4_title_num++;
                    pt_mkv_handler = pt_mkv_handler->pt_next;
                }
                *((UINT32*)pv_buf) = ui4_title_num;
            }
            break;
        case MINFO_INFO_TYPE_TITLE_INFO:
            {
                MINFO_TITLE_INFO_T* pt_title_info = (MINFO_TITLE_INFO_T*)pv_buf;
                UINT32 ui4_idx = pt_title_info->ui4_title_idx;
                MINFO_MKV_INF_T*    pt_mkv_handler_prev = NULL;
                
                if(ui4_idx == 0)
                {
                    pt_title_info->fg_default = TRUE;
                }
                else
                {
                    pt_title_info->fg_default = FALSE;
                }
                pt_title_info->fg_enable = TRUE;
                pt_title_info->fg_hidden = FALSE;
                pt_mkv_handler = pt_minfo_obj->pv_handler_obj;

                while(ui4_idx--)
                {
                    pt_mkv_handler_prev = pt_mkv_handler;
                    if(pt_mkv_handler->pt_next)
                    {
                        pt_mkv_handler = pt_mkv_handler->pt_next;
                    }
                    else
                    {
                        return MINFOR_INFO_NOT_FOUND;
                    }
                }
                if (pt_mkv_handler_prev != NULL)
                {
                    if (pt_mkv_handler->t_all_strm_info.t_strm_properties.fg_enh_track_flag == 1 &&
                        pt_mkv_handler_prev->t_all_strm_info.t_strm_properties.fg_enh_track_flag == 0)
                    {
                        pt_title_info->fg_hidden = TRUE;
                    }
                }
                COPY_DATA(  pt_mkv_handler->t_segment_info.pws_title, 
                            x_uc_w2s_strlen(pt_mkv_handler->t_segment_info.pws_title), 
                            pt_title_info->pwc_title_name, 
                            &(pt_title_info->ui4_title_len));
                pt_title_info->ui8_title_uid_high = pt_mkv_handler->t_segment_info.ui8_seg_uid_high;
                pt_title_info->ui8_title_uid_low = pt_mkv_handler->t_segment_info.ui8_seg_uid_low;
                pt_title_info->ui4_playlist_num = pt_mkv_handler->t_chapters_info.ui4_edition_num;
            }
            break;
        case MINFO_INFO_TYPE_PLAYLIST_INFO:
            {
                MINFO_PLAYLIST_INFO_T* pt_pl_info = (MINFO_PLAYLIST_INFO_T*)pv_buf;
                MINFO_MKV_EDITION_T*    pt_edition = NULL;
                MINFO_MKV_CHAP_ATOM_T*  pt_chap_atom = NULL;
                BOOL                    b_is_total_time = FALSE;                    
                UINT32 ui4_title_idx = pt_pl_info->ui4_title_idx;
                UINT32 ui4_pl_idx = pt_pl_info->ui4_playlist_idx;
                
                while(ui4_title_idx--)
                {
                    if(pt_mkv_handler->pt_next)
                    {
                        pt_mkv_handler = pt_mkv_handler->pt_next;
                    }
                    else
                    {
                        return MINFOR_INFO_NOT_FOUND;
                    }
                }
                if(pt_mkv_handler == NULL)
                {
                    return MINFOR_INFO_NOT_FOUND;
                }
                pt_edition = pt_mkv_handler->t_chapters_info.pt_edition;
                if(pt_edition == NULL)
                {
                    return MINFOR_INFO_NOT_FOUND;
                }
                while(ui4_pl_idx--)
                {
                    if(pt_edition->pt_next)
                    {
                        pt_edition = pt_edition->pt_next;
                    }
                    else
                    {
                        return MINFOR_INFO_NOT_FOUND;
                    }
                }
                pt_chap_atom = pt_edition->pt_chap_atom;
                pt_pl_info->fg_default = pt_edition->fg_default;
                pt_pl_info->fg_enable = TRUE;
                pt_pl_info->fg_hidden = pt_edition->fg_hidden;
                pt_pl_info->ui8_playlist_uid = pt_edition->ui8_uid;
                pt_pl_info->ui4_chapter_num = pt_edition->ui4_chap_num;
                pt_pl_info->fg_ordered = pt_edition->fg_ordered;

                
                pt_pl_info->ui8_total_time = 0;
                while(pt_chap_atom != NULL)
                {
                    if (pt_chap_atom->ui8_end_time != 0)
                    {
                        pt_pl_info->ui8_total_time +=  (pt_chap_atom->ui8_end_time - pt_chap_atom->ui8_start_time);
                    }
                    else
                    {
                        pt_pl_info->ui8_total_time = pt_mkv_handler->ui8_duration;
                        b_is_total_time = FALSE;
                        break;
                    }
                    b_is_total_time = TRUE;
                    pt_chap_atom = pt_chap_atom->pt_next;
                        
                }
                if (b_is_total_time)
                {
                    pt_pl_info->ui8_total_time = (UINT64)_mm_div64(pt_pl_info->ui8_total_time, (UINT64)1000000, NULL);
                    if ((pt_pl_info->ui8_total_time * 90) > pt_mkv_handler->ui8_duration)
                    {
                        pt_pl_info->ui8_total_time = (UINT64)_mm_div64(pt_mkv_handler->ui8_duration, 90, NULL);
                    }
                }
                else
                {
                    pt_pl_info->ui8_total_time = (UINT64)_mm_div64(pt_pl_info->ui8_total_time, 90, NULL);
                }
            }
            break;
        case MINFO_INFO_TYPE_CHAP_INFO:
            {
                MINFO_CHAP_INFO_T* pt_chap_info = (MINFO_CHAP_INFO_T*)pv_buf;
                MINFO_MKV_EDITION_T* pt_edition;
                MINFO_MKV_CHAP_ATOM_T* pt_chap;
                UINT32 ui4_title_idx = pt_chap_info->ui4_title_idx;
                UINT32 ui4_pl_idx = pt_chap_info->ui4_playlist_idx;
                UINT32 ui4_ch_idx = pt_chap_info->ui4_chap_idx;
                while(ui4_title_idx--)
                {
                    if(pt_mkv_handler->pt_next)
                    {
                        pt_mkv_handler = pt_mkv_handler->pt_next;
                    }
                    else
                    {
                        return MINFOR_INFO_NOT_FOUND;
                    }
                }
                if(pt_mkv_handler == NULL)
                {
                    return MINFOR_INFO_NOT_FOUND;
                }
                pt_edition = pt_mkv_handler->t_chapters_info.pt_edition;
                if(pt_edition == NULL)
                {
                    return MINFOR_INFO_NOT_FOUND;
                }
                while(ui4_pl_idx--)
                {
                    if(pt_edition->pt_next)
                    {
                        pt_edition = pt_edition->pt_next;
                    }
                    else
                    {
                        return MINFOR_INFO_NOT_FOUND;
                    }
                }
                pt_chap = pt_edition->pt_chap_atom;
                if(pt_chap == NULL)
                {
                    return MINFOR_INFO_NOT_FOUND;
                }
                pt_chap_info->ui8_pl_time = 0;
                if(ui4_ch_idx == 0)
                {
                    pt_chap_info->fg_default = TRUE;
                }
                else
                {
                    pt_chap_info->fg_default = FALSE;
                }
                while(ui4_ch_idx--)
                {
                    if (pt_chap->ui8_end_time != 0)
                    {
                        if(pt_chap->ui8_end_time > pt_chap->ui8_start_time)
                        {
                            pt_chap_info->ui8_pl_time += pt_chap->ui8_end_time - pt_chap->ui8_start_time;
                        }
                    }
                    else if (pt_chap->pt_next != NULL)
                    {
                        if(pt_chap->pt_next->ui8_start_time > pt_chap->ui8_start_time)
                        {
                            pt_chap_info->ui8_pl_time +=  pt_chap->pt_next->ui8_start_time - 1000000000 - pt_chap->ui8_start_time;
                        }
                    }
                    else
                    {
                        return MINFOR_INTERNAL_ERR;
                    }
                    if(pt_chap->pt_next)
                    {
                        pt_chap = pt_chap->pt_next;
                    }
                    else
                    {
                        return MINFOR_INFO_NOT_FOUND;
                    }
                }
                if(pt_chap == NULL)
                {
                    return MINFOR_INFO_NOT_FOUND;
                }

                pt_chap_info->fg_enable = pt_chap->fg_enabled;
                pt_chap_info->fg_hidden = pt_chap->fg_hidden;
                pt_chap_info->ui4_disp_num = pt_chap->ui4_display_num;
                if (pt_chap->ui8_end_time == 0)
                {
                    if (pt_chap->pt_next)
                    {
                        pt_chap->ui8_end_time = pt_chap->pt_next->ui8_start_time - (UINT64)1000000000;
                    }
                    else
                    {
                        pt_chap->ui8_end_time = (UINT64)_mm_div64(pt_mkv_handler->ui8_duration * (UINT64)1000000,
                                                        (UINT64)90, 
                                                        NULL);
                    }
                }
                pt_chap_info->ui8_end_time = (UINT64)_mm_div64(pt_chap->ui8_end_time, (UINT64)1000000, NULL);
                pt_chap_info->ui8_start_time = (UINT64)_mm_div64(pt_chap->ui8_start_time, (UINT64)1000000, NULL);
                pt_chap_info->ui8_pl_time = (UINT64)_mm_div64(pt_chap_info->ui8_pl_time, (UINT64)1000000, NULL);
                pt_chap_info->ui8_chap_uid = pt_chap->ui8_uid;
            }
            break;
#endif
        case MINFO_INFO_TYPE_DISP_INFO:
            {
                MINFO_DISP_INFO_T* pt_disp_info = (MINFO_DISP_INFO_T*)pv_buf;
                
                i4_ret = _mkv_minfo_get_disp_info(pt_minfo_obj, pt_disp_info);
                
                if (MINFOR_OK!= i4_ret)
                {
                    DBG_ERROR(("_mkv_minfo_get_disp_info fail,i4_ret=%d\r\n", i4_ret));
                    return MINFOR_INTERNAL_ERR;    
                }
            }
            break;

        default:
         return MINFOR_INFO_NOT_FOUND;
    }
    return MINFOR_OK;
} 
#ifdef DIVX_PLUS_CER
INT32 mkv_minfo_set_info( 
                        VOID*                pv_obj, 
                        MINFO_INFO_TYPE_T    e_info_type, 
                        MINFO_REC_IDX_T      ui2_index,                    
                        VOID*                pv_buf, 
                        SIZE_T*              pz_buf_leng)
{
    MINFO_OBJ_T*                pt_minfo_obj;
    if ((pv_obj == NULL)
        || (pv_buf == NULL))
    {
        return MINFOR_INITIAL_ERR;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
    switch(e_info_type)
    {
        case MINFO_INFO_TYPE_SET_TITLE:
            pt_minfo_obj->ui4_cur_title = *((UINT32*)pv_buf);
            break;
        case MINFO_INFO_TYPE_SET_LANGUAGE:
            if (*pz_buf_leng != sizeof(ISO_639_LANG_T))
            {
                return MINFOR_INV_ARG;
            }
            
            x_memcpy(pt_minfo_obj->e_language, pv_buf, *pz_buf_leng);
            
            pt_minfo_obj->e_language[ISO_639_LANG_LEN -1]= '\0';
            
            DBG_LOG_PRINT(("[MKV_MINFO]set system language = %s\n", pt_minfo_obj->e_language))
            break;
        default:
            break;
    }
    return MINFOR_OK;
}
#endif


