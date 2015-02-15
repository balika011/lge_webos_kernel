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
 * $RCSfile: mfmtrecg.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/3 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 0a0e5908fac0e73885ce39efe82bcd61 $ Asa
 *
 * Description: 
 *         This file contains all the media format recognizer public APIs  
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
#ifndef __NO_FM__
#include "x_fm.h"
#endif
#include "u_mfmtrecg.h"
#include "mutil/mfmtrecg/mfmtrecg.h"
#include "mutil/mfmtrecg/mfmtrecg_cli.h"
#include "mutil/mm_util.h"
#include "x_dbg.h"
/*#include "libc/ctype.h"*/
#include "x_lnk_list.h"

#else

#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "u_mfmtrecg.h"
#include "common.h"
#include "mfmtrecg.h"
#include "mfmtrecg_cli.h"
#include "x_dbg.h"
#include "x_lnk_list.h"
#include "../mm_util.h"

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
SLIST_T(_MFMTRECG_HANDLER_REG_T) gt_mfmtrecg_handler_cntnr_reg_list = {NULL};
SLIST_T(_MFMTRECG_HANDLER_REG_T) gt_mfmtrecg_handler_es_reg_list = {NULL};

static BOOL     gb_init = FALSE;
static HANDLE_T                       gh_notify_async_exe_thread = NULL_HANDLE;
static HANDLE_T                       gh_notify_async_exe_msgq   = NULL_HANDLE;
HANDLE_T                              gh_mfmtrecg_obj_lock = NULL_HANDLE;

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
#ifdef __KERNEL__
CHAR* x_str_toupper (CHAR*  ps_str)
{
    if (ps_str != NULL)
    {
        CHAR*  ps_cursor;
        CHAR   c_char;
        
        ps_cursor = ps_str;
        
        while ((c_char = *ps_cursor) != '\0')
        {
            if ((c_char >= 'a') && (c_char <= 'z'))
            {
                *ps_cursor = c_char - 'a' + 'A';
            }
            
            ps_cursor++;
        }
    }
    
    return ps_str;
}
#endif

BOOL mfmrecg_is_except_file(CHAR* ps_buf, UINT32 ui4_len)
{
    const CHAR s_jpg[]  = {10,0xFF,0xD8,0xFF,'*','*','*',0x4A,0x46,0x49,0x46};
    const CHAR s_exif[] = {10,0xFF,0xD8,0xFF,'*','*','*',0x45,0x78,0x69,0x66};
    const CHAR s_bmp[]  = {10,'B','M','*','*','*','*',0,0,0,0};
    const CHAR s_gif[]  = {3,'G','I','F'};
    const CHAR s_png[]  = {8 ,0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
    const CHAR* pps_except_fmt[] = //{len, char series * represent any}
    {
        s_jpg,
        s_exif,
        s_bmp,
        s_gif,
        s_png
    };
    UINT32 ui4_i = 0;
    UINT32 ui4_j = 0;
    for (ui4_i = 0; ui4_i < sizeof(pps_except_fmt)/sizeof(CHAR*);ui4_i++)
    {
        CHAR* ps_except = (CHAR*)pps_except_fmt[ui4_i];
        
        if (ui4_len < ps_except[0])
        {
            continue;
        }
        for (ui4_j = 0; ui4_j < (UINT32)ps_except[0]; ui4_j++)
        {
            if (ps_except[ui4_j+1] == '*')
            {
                continue;
            }
            if (ps_except[ui4_j+1] != ps_buf[ui4_j])
            {
                ui4_j = 0;
                break;
            }
        }
        if (ui4_j != 0)
        {
            DBG_INFO(("[Fmt recg] quit for except file:%d\n", ui4_i));
            return TRUE;
        }
    }
    return FALSE;
    
}

VOID _mfmtrecg_data_to_format(MEDIA_FORMAT_T*  pt_format,
                                CHAR*           ps_buf, 
                                UINT32          ui4_len)
{
    const char s_asf_object_header[16] =
{0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};    
    const char s_mkv_object_header[4] = {0x1A, 0x45, 0xDF, 0xA3};
    const char ps_header[4] = {0x00, 0x00, 0x01, 0xBA};
    const char amr_header[6] = {0x23, 0x21, 0x41, 0x4D, 0x52, 0x0A};
    const char awb_header[9] = {0x23, 0x21, 0x41, 0x4D, 0x52, 0x2D, 0x57, 0x42, 0x0A};
    const char aac_header1[4] = {0x41, 0x44, 0x49, 0x46};
    const char aac_header2[2] = {0xFF, 0xF0};
    const char ac3_header[2] = {0x0b, 0x77};
    const char spec_ac3_header[2] = {0x77, 0x0b};
    const char dts_header[4] = {0x7f, 0xfe, 0x80, 0x01};
    const char spec_dts_header[4] = {0xff, 0x1f, 0x00, 0xe8};
    const char flac_header[4] = {0x66, 0x4c, 0x61, 0x43};
    const char vid_es_header[3] = {0x00, 0x00, 0x01};
    UINT16     ui2_mp3_header = 0; 
    
    ui2_mp3_header = ((UINT16)ps_buf[0] << 8) | (UINT16)ps_buf[1];
    if (x_memcmp(ps_buf, "OggS", 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_CONTAINER;
        pt_format->t_media_subtype.u.e_contnr_subtype = MEDIA_CONTNR_SUBTYPE_OGM;
    }
    else if (x_memcmp(ps_buf, ".RMF", 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_CONTAINER;
        pt_format->t_media_subtype.u.e_contnr_subtype = MEDIA_CONTNR_SUBTYPE_RM;
    }
    else if (x_memcmp(ps_buf, s_mkv_object_header, 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_CONTAINER;
        pt_format->t_media_subtype.u.e_contnr_subtype = MEDIA_CONTNR_SUBTYPE_MATROSKA;
    }
    else if (x_memcmp(ps_buf + 4, "ftyp", 4) == 0   ||
             x_memcmp(ps_buf + 4, "moov", 4) == 0   ||
             x_memcmp(ps_buf + 4, "flsz", 4) == 0   ||
             x_memcmp(ps_buf + 4, "mdat", 4) == 0   ||
             x_memcmp(ps_buf + 4, "pnot", 4) == 0   ||
             x_memcmp(ps_buf + 4, "PICT", 4) == 0   ||
             x_memcmp(ps_buf + 4, "free", 4) == 0   ||
             x_memcmp(ps_buf + 4, "skip", 4) == 0   ||
             x_memcmp(ps_buf + 4, "wide", 4) == 0 )
    {
        pt_format->e_media_type = MEDIA_TYPE_CONTAINER;
        pt_format->t_media_subtype.u.e_contnr_subtype = MEDIA_CONTNR_SUBTYPE_MP4;
    }
    else if (x_memcmp(ps_buf, "RIFF", 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_CONTAINER;
        pt_format->t_media_subtype.u.e_contnr_subtype = MEDIA_CONTNR_SUBTYPE_AVI;
    }
    else if (x_memcmp(ps_buf, "FLV", 3) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_CONTAINER;
        pt_format->t_media_subtype.u.e_contnr_subtype = MEDIA_CONTNR_SUBTYPE_FLV;
    }
    else if (x_memcmp(ps_buf, "AIFF", 4) == 0   ||
            x_memcmp(ps_buf, "AIFC", 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_AIF;
    }
    else if (x_memcmp(ps_buf, s_asf_object_header, 16) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_CONTAINER;
        pt_format->t_media_subtype.u.e_contnr_subtype = MEDIA_CONTNR_SUBTYPE_ASF;
    }
    else if (x_memcmp(ps_buf, "MAC ", 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_APE;
    }
    else if (x_memcmp(ps_buf, ps_header, 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_PS;
    }
    else if (x_memcmp(ps_buf, amr_header, 6) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_AMR;
    }    
    else if (x_memcmp(ps_buf, awb_header, 9) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_AWB;
    }
    else if (x_memcmp(ps_buf, aac_header1, 4) == 0 ||
             x_memcmp(ps_buf, aac_header2, 2) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_AAC;
    }
    else if (x_memcmp(ps_buf, ac3_header, 2) == 0 ||
             x_memcmp(ps_buf, spec_ac3_header, 2) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_AC3;
    }
    else if (x_memcmp(ps_buf, dts_header, 4) == 0 ||
             x_memcmp(ps_buf, spec_dts_header, 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_DTS;
    }
    else if (x_memcmp(ps_buf, flac_header, 4) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_FLAC;
    }
    else if ((ui2_mp3_header & 0xffe0) == 0xffe0)
    {
        pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
        pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_MP3;
    }
    else if (ps_buf[0] == 0x47 || ps_buf[4] == 0x47)
    {
        pt_format->e_media_type = MEDIA_TYPE_TS;
    }
    else if (x_memcmp(ps_buf, vid_es_header, 3) == 0)
    {
        pt_format->e_media_type = MEDIA_TYPE_VIDEO_ES;
    }
}

/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_get_file_fmt
 *
 * Description: This API retrieves the format of the media file.
 *
 * Inputs:  ps_name         specify the path name of the media file which want to retrieve the format and it is not case sensitive.
 *
 * Outputs: pt_format       retrieve the format of the current media file.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mfmtrecg_get_file_fmt (
                                const CHAR*       ps_name,       
                                MEDIA_FORMAT_T*   pt_format)  
{
    
#ifndef __NO_FM__

    CHAR*                       ps_extension = NULL; 
    CHAR                        ac_extension[MAX_EXTENSION_NAME_LEN]; 
    INT32                       i4_ret = MFMTRECGR_INV_ARG;
    HANDLE_T                    h_file;
    BOOL                        b_fav_recg_only;
        
    DBG_API(("[mfmtrecg] x_mfmtrecg_get_file_fmt: ps_name:%s \t pt_format:%p\n\r"
        , ps_name, pt_format));  
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((ps_name == NULL)
        || (pt_format == NULL))
    {
        return MFMTRECGR_INV_ARG;
    }

    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    /* Search backward. */
    ps_extension = x_strrchr( ps_name, '.' );/* ".mp3" */

    /************************************************************************/
    /*  check all handler                                                    */
    /************************************************************************/
    if (ps_extension)
    {
        b_fav_recg_only = pt_format->b_fav_recg_only;
        
        ps_extension++; /* cut off'.'=>"mp3" */ 
        //x_memset(pt_format, 0, sizeof(MEDIA_FORMAT_T));   //mark this code, otherwise, pt_format info which set from mm_svctx will be clean
        x_memset(ac_extension, 0 , MAX_EXTENSION_NAME_LEN*sizeof(CHAR));
        x_memcpy(ac_extension, ps_extension, x_strlen(ps_extension));
        MUTIL_DBG_LOG_MW_0(("x_mfmtrecg : begin open file!\r\n"));
        /*MUTIL_DBG_LOG_MW_0(("x_mfmtrecg : file name : %s !\r\n",ps_name));*/
        i4_ret = x_fm_open(FM_ROOT_HANDLE, ps_name, 
                           FM_READ_ONLY, 0777, 
                           FALSE, &h_file);
        MUTIL_DBG_LOG_MW_0(("x_mfmtrecg : end open file, ret = %d!\r\n",i4_ret));
        if (i4_ret == FMR_OK)
        {
            UINT64 ui8_cur_pos;
            UINT32 ui4_read;
            UINT32 ui4_code;
            i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
            if (i4_ret == FMR_OK)
            {
                i4_ret = x_fm_read(h_file, &ui4_code, 4, &ui4_read);
                if (i4_ret != FMR_OK)
                {
                    x_fm_close(h_file);
                    return MFMTRECGR_FILE_ERR;
                }
            }
            else
            {
                x_fm_close(h_file);
                return MFMTRECGR_FILE_ERR;
            }
            i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
            
            /* firstly, match favor string */
            i4_ret = mfmtrecg_ask_handler_in_favor_by_file(h_file, 
                                                           ac_extension, 
                                                           pt_format);
            if (i4_ret == MFMTRECGR_OK ||
               i4_ret == MFMTRECGR_FILE_ERR)
            {
                /*do nothing*/
            }
            /* secondly, ask all handler */
            else if (i4_ret != MFMTRECGR_OK && (!b_fav_recg_only))
            {
                i4_ret = mfmtrecg_ask_all_handler_by_file(h_file, ac_extension, pt_format);
            }
        }      
        x_fm_close(h_file);
    }   
    return i4_ret;
#else
    return MFMTRECGR_INV_ARG;
#endif
}

/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_get_file_fmt_by_handle
 *
 * Description: This API retrieves the format of the media file.
 *
 * Inputs:  h_file          specify the media file handle which want to retrieve the format and it is not case sensitive.
 *
 * Outputs: pt_format       retrieve the format of the current media file.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mfmtrecg_get_file_fmt_by_handle (
                                const HANDLE_T    h_file,       
                                MEDIA_FORMAT_T*   pt_format)  
{
    
#ifndef __NO_FM__

    CHAR                        ac_extension[MAX_EXTENSION_NAME_LEN]; 
    INT32                       i4_ret = MFMTRECGR_INV_ARG;
    BOOL                        b_fav_recg_only;
        
    DBG_API(("[mfmtrecg] x_mfmtrecg_get_file_fmt_by_handle: h_file:%d \t pt_format:%p\n\r"
        , h_file, pt_format));  
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((h_file == NULL_HANDLE)
        || (pt_format == NULL))
    {
        return MFMTRECGR_INV_ARG;
    }

    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    b_fav_recg_only = pt_format->b_fav_recg_only;
    
    //x_memset(pt_format, 0, sizeof(MEDIA_FORMAT_T));   //mark this code, otherwise, pt_format info which set from mm_svctx will be clean
    x_memset(ac_extension, 0 , MAX_EXTENSION_NAME_LEN*sizeof(CHAR));

    /* firstly, match favor string */
    i4_ret = mfmtrecg_ask_handler_in_favor_by_file(h_file, 
                                                   ac_extension, 
                                                   pt_format);
    if (i4_ret == MFMTRECGR_OK ||
       i4_ret == MFMTRECGR_FILE_ERR)
    {
        /*do nothing*/
    }
    /* secondly, ask all handler */
    else if (i4_ret != MFMTRECGR_OK && (!b_fav_recg_only))
    {
        i4_ret = mfmtrecg_ask_all_handler_by_file(h_file, ac_extension, pt_format);
    }
    return i4_ret;
#else
    return MFMTRECGR_INV_ARG;
#endif
}


/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_get_mem_fmt
 *
 * Description: This API retrieves the format of the media memory buffer.
 *
 * Inputs:  pv_mem_data     specify the address of memory buffer which want to retrieve the format.
 *          z_mem_leng      specify the length of memory buffer which want to retrieve the format.
 *
 * Outputs: pt_format       retrieve the format of the media memory buffer.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mfmtrecg_get_mem_fmt ( 
                        const VOID*       pv_mem_data, 
                        SIZE_T            z_mem_leng,   
                        MEDIA_FORMAT_T*   pt_format)   
{

    INT32 i4_ret;

    DBG_API(("[mfmtrecg] x_mfmtrecg_get_mem_fmt:pv_mem_data:%p \t z_mem_leng:%u \t pt_format:%p\n\r"
        , pv_mem_data, z_mem_leng, pt_format)); 
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_mem_data == NULL)
        || (pt_format == NULL))
    {
        return MFMTRECGR_INV_ARG;
    }

    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/

    i4_ret = mfmtrecg_ask_handler_in_favor_by_mem_with_fmt(pv_mem_data, z_mem_leng, pt_format);
    if (MFMTRECGR_OK != i4_ret)
    {
        if(MFMTRECGR_FILE_ERR == i4_ret)
        {
            return i4_ret;
        }
        return mfmtrecg_ask_all_handler_by_mem(pv_mem_data, z_mem_leng, pt_format);
    }
    else
    {
        return MFMTRECGR_OK;
    }
    
    
}


/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_ask_handler_in_favor_by_file
 *
 * Description: this function will ask favor handler which match extension and favor
 *
 * Inputs:  h_file         specify the handle of the file
 *          ps_extension   specify extension of the file
 *
 * Outputs: pt_format      retrieve the format of the file
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mfmtrecg_ask_handler_in_favor_by_file(
                            HANDLE_T                    h_file,        
                            const CHAR*                 ps_extension,  
                            MEDIA_FORMAT_T*             pt_format)     
{
#ifndef __NO_FM__
    MFMTRECG_HANDLER_REG_T*     pt_reg = NULL; 
    handler_file_confirm_fct    pf_fconfirm = NULL;
    MFMREC_FILE_EXT_T*          pt_file_ext = NULL;
    INT32                       i4_ret = 0;
    CHAR                        s_buf[MAX_CNTNR_HEADER_BUFFER_LEN];
    UINT64                      ui8_cur_pos = 0;
    UINT32                      ui4_read = 0;
    UINT32                      ui4_code = 0;
    UINT32                      ui4_start_tick;
    UINT32                      ui4_id3_size = 0;

    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ( (h_file == NULL_HANDLE) 
        || (ps_extension == NULL) 
        || (pt_format == NULL) )
    {
        return MFMTRECGR_INV_ARG;
    }    
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    /* check engines */
    
    i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
    if (i4_ret == FMR_OK)
    {
        i4_ret = x_fm_read(h_file, s_buf, 16, &ui4_read);
        if (i4_ret != FMR_OK)
        {
            return MFMTRECGR_FILE_ERR;
        }
    }
    else
    {
        return MFMTRECGR_FILE_ERR;
    }

    if (x_memcmp(s_buf, "ID3", 3) == 0)
    {
        ui4_id3_size = (s_buf[6] & 0x7F) * 0x200000 + (s_buf[7] & 0x7F) * 0x4000
                       + (s_buf[8] & 0x7F) * 0x80 + (s_buf[9] & 0x7F);

        ui4_id3_size += 10;  /* ID3 tag v2 header size*/
        
        i4_ret = x_fm_lseek(h_file, ui4_id3_size, FM_SEEK_BGN, &ui8_cur_pos);
        if (i4_ret == FMR_OK)
        {
            i4_ret = x_fm_read(h_file, s_buf, 16, &ui4_read);
            if (i4_ret != FMR_OK)
            {
                return MFMTRECGR_FILE_ERR;
            }
        }
    }

    i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
    if (i4_ret != FMR_OK)
    {
        return MFMTRECGR_FILE_ERR;
    }
    
    _mfmtrecg_data_to_format(pt_format, s_buf, MAX_CNTNR_HEADER_BUFFER_LEN);

    SLIST_FOR_EACH(pt_reg, &gt_mfmtrecg_handler_cntnr_reg_list, t_link)
    {
        if (pt_reg->t_format.e_media_type == pt_format->e_media_type &&
            pt_reg->t_format.t_media_subtype.u.e_contnr_subtype == pt_format->t_media_subtype.u.e_contnr_subtype)
        {
            pf_fconfirm = pt_reg->t_handler_fct_tbl.pf_fconfirm;
            if (pf_fconfirm != NULL)
            {
                ui4_start_tick = x_os_get_sys_tick();
                i4_ret = pf_fconfirm(h_file, pt_format);
                DBG_INFO(("[Fmt recg] check for favor %d:%d using:%dms ret %d\n",
                              (UINT32)pt_reg->t_format.e_media_type,
                            (UINT32)pt_reg->t_format.t_media_subtype.u.e_contnr_subtype,
                            (x_os_get_sys_tick()-ui4_start_tick)*x_os_get_sys_tick_period(),
                            i4_ret));
                if(i4_ret == MFMTRECGR_OK)
                {
                    return i4_ret;
                }
            }
        }
    }
    
    SLIST_FOR_EACH(pt_reg, &gt_mfmtrecg_handler_es_reg_list, t_link)
    {
        SLIST_FOR_EACH(pt_file_ext, &(pt_reg->t_favor_list), t_file_ext_list)
        {
           if ((pt_reg->t_format.e_media_type == pt_format->e_media_type &&
                pt_reg->t_format.e_media_type == MEDIA_TYPE_AUDIO_ES &&
                pt_reg->t_format.t_media_subtype.u.e_aud_subtype == pt_format->t_media_subtype.u.e_aud_subtype) ||
                (pt_reg->t_format.e_media_type == pt_format->e_media_type &&
                (pt_reg->t_format.e_media_type == MEDIA_TYPE_PS ||
                pt_reg->t_format.e_media_type == MEDIA_TYPE_TS ||
                pt_reg->t_format.e_media_type == MEDIA_TYPE_VIDEO_ES)))
           {
               pf_fconfirm = pt_reg->t_handler_fct_tbl.pf_fconfirm;
               
               if (pf_fconfirm)
               {
                    //x_memcpy(pt_format, &pt_reg->t_format, sizeof(MEDIA_FORMAT_T)); //mark this code, otherwise, pt_format info which set from mm_svctx will be clean
                 ui4_start_tick = x_os_get_sys_tick();
                 i4_ret = pf_fconfirm(h_file, pt_format);
                 DBG_INFO(("[Fmt recg]check for favor %d:%d using:%dms ret %d\n",
                              pt_reg->t_format.e_media_type,
                            (UINT32)pt_reg->t_format.t_media_subtype.u.e_vid_subtype,
                            (x_os_get_sys_tick()-ui4_start_tick)*x_os_get_sys_tick_period(),
                            i4_ret));
                 if (i4_ret == MFMTRECGR_OK)
                 {
                    return i4_ret;
                 }
                 else if(i4_ret == MFMTRECGR_FILE_ERR)
                 {
                     i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
                     if (i4_ret == FMR_OK)
                     {
                         i4_ret = x_fm_read(h_file, &ui4_code, 4, &ui4_read);
                         if (i4_ret != FMR_OK)
                         {
                             return MFMTRECGR_FILE_ERR;
                         }
                     }
                     else
                     {
                         return MFMTRECGR_FILE_ERR;
                     }
                     i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
                 }
              }
           }          
        }
    }
    UNUSED(ui4_start_tick);

    return MFMTRECGR_INTERNAL_ERR;
#else
    return MFMTRECGR_HANDLER_NOT_FOUND;
#endif
}
                            
/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_ask_all_handler_by_file
 *
 * Description: this function will ask all handler
 *
 * Inputs:  h_file         specify the handle of the file
 *          ps_extension   specify extension of the file
 *
 * Outputs: pt_format      retrieve the format of the file
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mfmtrecg_ask_all_handler_by_file(
                            HANDLE_T                    h_file,        
                            const CHAR*                 ps_extension,  
                            MEDIA_FORMAT_T*             pt_format)     
{
#ifndef __NO_FM__
    MFMTRECG_HANDLER_REG_T*     pt_reg; 
    handler_mem_confirm_fct     pf_mconfirm = NULL;
    INT32 i4_ret;
    UINT32                      ui4_start_tick;
    MEDIA_FORMAT_T              t_favor_fmt;
    UINT8*                      pui1_buf = NULL;
    UINT64                      ui8_cur_pos = 0;    
    UINT32                      ui4_read = 0;
    UINT32                      ui4_len = 0;
    UINT8                       ui1_loop_idx = 0;   /*0: from header; 1: from middle*/
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ( (h_file == NULL_HANDLE) 
        || (ps_extension == NULL) 
        || (pt_format == NULL) )
    {
        return MFMTRECGR_INV_ARG;
    }
    x_memcpy(&t_favor_fmt, pt_format, sizeof(MEDIA_FORMAT_T));
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    /* check engines */
    pui1_buf = x_mem_alloc(MAX_MEM_RECOGNIZE_LENGTH(pt_format->b_is_network));
    if (pui1_buf == NULL)
    {
        return MFMTRECGR_OUT_OF_MEMORY;
    }

    while (ui1_loop_idx <= 1)
    {
        if (ui1_loop_idx == 0)
        {
            ui4_len = 0;
        }
        else if (ui1_loop_idx == 1)
        {
            i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_END, &ui8_cur_pos);
            if (i4_ret != FMR_OK)
            {
                if (pui1_buf != NULL)
                {
                    x_mem_free(pui1_buf);
                    pui1_buf = NULL;
                }
                return MFMTRECGR_FILE_ERR;
            }
            ui4_len = ui8_cur_pos / 2;
        }

        i4_ret = x_fm_lseek(h_file, ui4_len, FM_SEEK_BGN, &ui8_cur_pos);
        if (i4_ret != FMR_OK)
        {
            if (pui1_buf != NULL)
            {
                x_mem_free(pui1_buf);
                pui1_buf = NULL;
            }
            return MFMTRECGR_FILE_ERR;
        }

        i4_ret = x_fm_read(h_file, pui1_buf, MAX_MEM_RECOGNIZE_LENGTH(pt_format->b_is_network), &ui4_read);
        if (i4_ret != FMR_OK)
        {
            if (pui1_buf != NULL)
            {
                x_mem_free(pui1_buf);
                pui1_buf = NULL;
            }
            return MFMTRECGR_FILE_ERR;
        }

        i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
        if (i4_ret != FMR_OK)
        {
            if (pui1_buf != NULL)
            {
                x_mem_free(pui1_buf);
                pui1_buf = NULL;
            }
            return MFMTRECGR_FILE_ERR;
        }
        
        SLIST_FOR_EACH(pt_reg, &gt_mfmtrecg_handler_es_reg_list, t_link)
        {
            if (t_favor_fmt.e_media_type == pt_reg->t_format.e_media_type &&
                (x_memcmp(&t_favor_fmt.t_media_subtype, &pt_reg->t_format.t_media_subtype, sizeof(MEDIA_SUBTYPE_T)) == 0))
            {
                DBG_INFO(("[Fmt recg]skip favor %d:%d\n",
                           pt_reg->t_format.e_media_type,
                           (UINT32)pt_reg->t_format.t_media_subtype.u.e_contnr_subtype));
                continue;
            }
            pf_mconfirm = pt_reg->t_handler_fct_tbl.pf_mconfirm;
            if (pf_mconfirm)
            {
                ui4_start_tick = x_os_get_sys_tick();
                i4_ret = pf_mconfirm(pui1_buf, ui4_read, pt_format);
                DBG_INFO(("[Fmt recg]check for %d:%d using:%dms ret %d\n",
                              pt_reg->t_format.e_media_type,
                            (UINT32)pt_reg->t_format.t_media_subtype.u.e_vid_subtype,
                            (x_os_get_sys_tick()-ui4_start_tick)*x_os_get_sys_tick_period(),
                            i4_ret));
                if (i4_ret == MFMTRECGR_OK)
                {
                    if (pui1_buf != NULL)
                    {
                        x_mem_free(pui1_buf);
                        pui1_buf = NULL;
                    }
                    return i4_ret;
                }
            }
        }
        ui1_loop_idx ++;
    }
    
    UNUSED(ui4_start_tick);
    if (pui1_buf != NULL)
    {
        x_mem_free(pui1_buf);
        pui1_buf = NULL;
    }
#endif
    return MFMTRECGR_HANDLER_NOT_FOUND;
}

/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_ask_handler_in_favor_by_mem_with_fmt
 *
 * Description: this function will ask favor handler which match given format
 *
 * Inputs:  pv_mem_data     specify the address of memory buffer which want to retrieve the format.
 *            z_mem_leng        specify the length of memory buffer which want to retrieve the format.
 *
 * Outputs: pt_format       retrieve the format of the media memory buffer.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/

INT32 mfmtrecg_ask_handler_in_favor_by_mem_with_fmt(
                        const VOID*       pv_mem_data, 
                        SIZE_T            z_mem_leng,   
                        MEDIA_FORMAT_T*   pt_format) 

{
    INT32                       i4_ret = 0;
    MFMTRECG_HANDLER_REG_T*     pt_reg = NULL; 
    handler_mem_confirm_fct    pf_mconfirm = NULL;
    MEDIA_FEEDER_T*             pt_src_feeder = NULL;
    CHAR                        s_buf[MAX_CNTNR_HEADER_BUFFER_LEN];
    UINT32                      ui4_len = 0;
    UINT32                      ui4_start_tick;
    UINT32                      ui4_id3_size = 0;

    if (pv_mem_data == NULL || pt_format == NULL)
    {
        return MFMTRECGR_INV_ARG;
    }

    if (z_mem_leng == 0)
    {
        pt_src_feeder = (MEDIA_FEEDER_T*)pv_mem_data;

        /*try to read begin 16 bytes*/
        i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos(
                                &pt_src_feeder->h_feeder,
                                0,
                                MEDIA_SEEK_BGN);
        if (i4_ret != MFMTRECGR_OK)
        {
            return MFMTRECGR_FILE_ERR;
        }

        i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_copybytes(
                                        &pt_src_feeder->h_feeder, 
                                        s_buf,
                                        MAX_CNTNR_HEADER_BUFFER_LEN,
                                        MAX_CNTNR_HEADER_BUFFER_LEN,
                                        &ui4_len);
        if (i4_ret != MFMTRECGR_OK || ui4_len < MAX_CNTNR_HEADER_BUFFER_LEN)
        {
            return MFMTRECGR_FILE_ERR;
        }

        if (x_memcmp(s_buf, "ID3", 3) == 0)
        {
            ui4_id3_size = (s_buf[6] & 0x7F) * 0x200000 + (s_buf[7] & 0x7F) * 0x4000
                           + (s_buf[8] & 0x7F) * 0x80 + (s_buf[9] & 0x7F);

            ui4_id3_size += 10;  /* ID3 tag v2 header size*/
            i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos(
                                &pt_src_feeder->h_feeder,
                                ui4_id3_size,
                                MEDIA_SEEK_BGN);
            if (i4_ret != MFMTRECGR_OK)
            {
                return MFMTRECGR_FILE_ERR;
            }

            i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_copybytes(
                                        &pt_src_feeder->h_feeder, 
                                        s_buf,
                                        MAX_CNTNR_HEADER_BUFFER_LEN,
                                        MAX_CNTNR_HEADER_BUFFER_LEN,
                                        &ui4_len);
            if (i4_ret != MFMTRECGR_OK || ui4_len < MAX_CNTNR_HEADER_BUFFER_LEN)
            {
                return MFMTRECGR_FILE_ERR;
            }
        }
        
        i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos(
                                                &pt_src_feeder->h_feeder,
                                                0,
                                                MEDIA_SEEK_BGN);

    }
    else
    {
        x_memcpy(s_buf, pv_mem_data, MAX_CNTNR_HEADER_BUFFER_LEN);
    }
    
    _mfmtrecg_data_to_format(pt_format, s_buf, MAX_CNTNR_HEADER_BUFFER_LEN);
    
    if (pt_format->e_media_type == MEDIA_TYPE_CONTAINER)
    {
        SLIST_FOR_EACH(pt_reg, &gt_mfmtrecg_handler_cntnr_reg_list, t_link)
        {
            if (pt_reg->t_format.e_media_type == pt_format->e_media_type &&
                pt_reg->t_format.t_media_subtype.u.e_contnr_subtype == pt_format->t_media_subtype.u.e_contnr_subtype)
            {
                pf_mconfirm = pt_reg->t_handler_fct_tbl.pf_mconfirm;
                if (pf_mconfirm != NULL)
                {
                    ui4_start_tick = x_os_get_sys_tick();
                    i4_ret = pf_mconfirm(pv_mem_data, z_mem_leng, pt_format);
                    DBG_INFO(("[Fmt recg]mem check for favor %d:%d using:%d ms ret %d\n",
                                  pt_reg->t_format.e_media_type,
                                (UINT32)pt_reg->t_format.t_media_subtype.u.e_contnr_subtype,
                                (x_os_get_sys_tick()-ui4_start_tick)*x_os_get_sys_tick_period(),
                                i4_ret));
                    if(i4_ret == MFMTRECGR_OK)
                    {
                        return i4_ret;
                    }
                }
            }
        }
    }
    else if (pt_format->e_media_type != MEDIA_TYPE_UNKNOWN)
    {
        /* check engines */
        SLIST_FOR_EACH(pt_reg, &gt_mfmtrecg_handler_es_reg_list, t_link)
        {
            if(z_mem_leng == 0 && pt_src_feeder->b_canceled)
            {
#ifndef __KERNEL__
                DBG_LOG_PRINT(("!!!mfmtrecg_ask_handler_in_favor_by_mem_with_fmt: canceled = true \n"));
#endif
                return MFMTRECGR_FILE_ERR;
            }
            
            if ((pt_reg->t_format.e_media_type == pt_format->e_media_type &&
                pt_format->e_media_type == MEDIA_TYPE_TS) ||
                (pt_reg->t_format.e_media_type == pt_format->e_media_type &&
                pt_format->e_media_type == MEDIA_TYPE_PS) ||
                (pt_reg->t_format.e_media_type == pt_format->e_media_type &&
                pt_format->e_media_type == MEDIA_TYPE_AUDIO_ES &&
                pt_reg->t_format.t_media_subtype.u.e_aud_subtype == pt_format->t_media_subtype.u.e_aud_subtype) ||
                (pt_reg->t_format.e_media_type == pt_format->e_media_type &&
                pt_format->e_media_type == MEDIA_TYPE_VIDEO_ES))
            {
                pf_mconfirm = pt_reg->t_handler_fct_tbl.pf_mconfirm;
                if (pf_mconfirm != NULL)
                {
                    ui4_start_tick = x_os_get_sys_tick();
                    i4_ret = pf_mconfirm(pv_mem_data, z_mem_leng, pt_format);
                    DBG_INFO(("[Fmt recg]mem check for favor %d:%d using:%d ms ret %d\n",
                                  pt_reg->t_format.e_media_type,
                                (UINT32)pt_reg->t_format.t_media_subtype.u.e_vid_subtype,
                                (x_os_get_sys_tick()-ui4_start_tick)*x_os_get_sys_tick_period(),
                                i4_ret));
                    if(i4_ret == MFMTRECGR_OK)
                    {
                        return i4_ret;
                    }
                }
            }
        }
    }
    
    if (z_mem_leng == 0)
    {
        /*restore*/                            
        i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos(
                                                        &pt_src_feeder->h_feeder,
                                                        0,                                                    
                                                        MEDIA_SEEK_BGN);
    }
    UNUSED(ui4_start_tick);
    
    return MFMTRECGR_INTERNAL_ERR;
}

/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_ask_all_handler_by_mem
 *
 * Description: This API retrieves the format of the media memory buffer.
 *
 * Inputs:  pv_mem_data     specify the address of memory buffer which want to retrieve the format.
 *          z_mem_leng      specify the length of memory buffer which want to retrieve the format.
 *
 * Outputs: pt_format       retrieve the format of the media memory buffer.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mfmtrecg_ask_all_handler_by_mem(
                        const VOID*       pv_mem_data, 
                        SIZE_T            z_mem_leng,   
                        MEDIA_FORMAT_T*   pt_format) 
{
    INT32                       i4_ret   = 0;
    MFMTRECG_HANDLER_REG_T*     pt_reg = NULL; 
    handler_mem_confirm_fct     pf_mconfirm = NULL;
    MEDIA_FEEDER_NW_T*          pt_feeder = NULL;
    MEDIA_FEEDER_T*             pt_src_feeder = NULL;
    UINT32                      ui4_start_tick;
    MEDIA_FORMAT_T              t_favor_fmt;
    UINT8*                      pui1_buf = NULL;
    UINT32                      ui4_len = 0;
    UINT8                       ui1_loop_idx = 0;   /*0: from header; 1: from middle*/
                
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_mem_data == NULL)
        || (pt_format == NULL))
    {
        return MFMTRECGR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    x_memcpy(&t_favor_fmt, pt_format, sizeof(MEDIA_FORMAT_T));
    
    if (z_mem_leng == 0)
    {
        pui1_buf = x_mem_alloc(MAX_MEM_RECOGNIZE_LENGTH(pt_format->b_is_network));
        if (pui1_buf == NULL)
        {
            return MFMTRECGR_OUT_OF_MEMORY;
        }
        
        pt_src_feeder = (MEDIA_FEEDER_T*)pv_mem_data;
        pt_feeder = (MEDIA_FEEDER_NW_T*)pt_src_feeder->h_feeder;
    }
    else
    {
        ui1_loop_idx = 1;   /*when z_mem_leng !=0, one loop only*/
    }

    while (ui1_loop_idx <= 1)
    {
        if (z_mem_leng == 0)
        {
            if (ui1_loop_idx == 0)
            {
                /*try to read 200KB bytes from header*/
                ui4_len = 0;
            }
            else if (ui1_loop_idx == 1)
            {
                /* set pt_feeder->b_seekable_check value */
                i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos(
                                        &pt_src_feeder->h_feeder,
                                        0,
                                        MEDIA_SEEK_BGN | FM_SEEK_CHK);
                if (i4_ret != MFMTRECGR_OK)
                {
                    if (pui1_buf != NULL)
                    {
                        x_mem_free(pui1_buf);
                        pui1_buf = NULL;
                    }
                    return MFMTRECGR_FILE_ERR;
                }

                if (pt_feeder->ui8_file_size != 0 && pt_feeder->b_seekable_checked)
                {
                    /*try to read 200KB bytes from middle*/
                    ui4_len = pt_feeder->ui8_file_size / 2;
                }
            }

            i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos(
                                    &pt_src_feeder->h_feeder,
                                    ui4_len,
                                    MEDIA_SEEK_BGN);
            if (i4_ret != MFMTRECGR_OK)
            {
                if (pui1_buf != NULL)
                {
                    x_mem_free(pui1_buf);
                    pui1_buf = NULL;
                }
                return MFMTRECGR_FILE_ERR;
            }

            i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_copybytes(
                                            pt_src_feeder, 
                                            pui1_buf,
                                            MAX_MEM_RECOGNIZE_LENGTH(pt_format->b_is_network),
                                            MAX_MEM_RECOGNIZE_LENGTH(pt_format->b_is_network),
                                            &ui4_len);
            if (i4_ret != MFMTRECGR_OK || ui4_len == 0)
            {
                if (pui1_buf != NULL)
                {
                    x_mem_free(pui1_buf);
                    pui1_buf = NULL;
                }
                return MFMTRECGR_FILE_ERR;
            }
        }

        /* check engines */
        SLIST_FOR_EACH(pt_reg, &gt_mfmtrecg_handler_es_reg_list, t_link)
        {
            if(z_mem_leng == 0 && pt_src_feeder->b_canceled)
            {
#ifndef __KERNEL__
                DBG_LOG_PRINT(("!!!mfmtrecg_ask_all_handler_by_mem: canceled = true \n"));
#endif            
                if (pui1_buf != NULL)
                {
                    x_mem_free(pui1_buf);
                    pui1_buf = NULL;
                }
                return MFMTRECGR_FILE_ERR;
            }
            if (t_favor_fmt.e_media_type == pt_reg->t_format.e_media_type &&
                (x_memcmp(&t_favor_fmt.t_media_subtype, &pt_reg->t_format.t_media_subtype, sizeof(MEDIA_SUBTYPE_T)) == 0))
            {
                DBG_INFO(("[Fmt recg] mem skip favor %d:%d\n",
                           (UINT32)pt_reg->t_format.e_media_type,
                           (UINT32)pt_reg->t_format.t_media_subtype.u.e_vid_subtype));
                continue;
            }
            pf_mconfirm = pt_reg->t_handler_fct_tbl.pf_mconfirm;
            if (pf_mconfirm)
            {
                ui4_start_tick = x_os_get_sys_tick();
                if ( pui1_buf != NULL )
                {
                    i4_ret = pf_mconfirm(pui1_buf, ui4_len, pt_format);
                }
                else
                {
                    i4_ret = pf_mconfirm(pv_mem_data, z_mem_leng, pt_format);
                }
                DBG_INFO(("[Fmt recg]check for %d:%d using:%dms ret %d\n",
                              (UINT32)pt_reg->t_format.e_media_type,
                            (UINT32)pt_reg->t_format.t_media_subtype.u.e_vid_subtype,
                            (x_os_get_sys_tick()-ui4_start_tick)*x_os_get_sys_tick_period(),
                            i4_ret));  
                if (i4_ret == MFMTRECGR_OK)
                {
                    if (pui1_buf != NULL)
                    {
                        x_mem_free(pui1_buf);
                        pui1_buf = NULL;
                    }
                    return i4_ret;
                }   

                if (z_mem_leng == 0)
                {
                    i4_ret = pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos(
                                            &pt_src_feeder->h_feeder,
                                            0,
                                            MEDIA_SEEK_BGN);
                }
            }
        }
        ui1_loop_idx ++;
    }
    
    UNUSED(ui4_start_tick);
    if (pui1_buf != NULL)
    {
        x_mem_free(pui1_buf);
        pui1_buf = NULL;
    }
    
    return MFMTRECGR_HANDLER_NOT_FOUND;

}
/*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_register_handler
 *
 * Description: This API register the handler.
 *
 * Inputs:  ps_name    specifies sixteen characters of a handler name, and the character is case sensitive.
 *          ps_favor        specify the favor file extension string list, for example "mp3", the list must end with FILE_EXTENSION_END
 *          pt_fct_tbl       specify the function table of the engine.
 *
 * Outputs: NONE.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/

INT32 mfmtrecg_register_handler (  
                            const CHAR*                             ps_name,    
                            const CHAR**                            ps_favor,
                            MEDIA_FORMAT_T                          t_media_format,
                            const MFMTRECG_HANDLER_FCT_TBL_T*       pt_fct_tbl)
{
    MFMTRECG_HANDLER_REG_T* pt_handler_reg = NULL;
    
    UINT8                   ui1_i = 0;
    CHAR*                   ps_ext;
    MFMREC_FILE_EXT_T*      pt_node_favor;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ( (ps_name == NULL) 
        || (ps_favor == NULL) 
        || (pt_fct_tbl == NULL) )
    {
        return MFMTRECGR_INV_ARG;
    }
    
    /************************************************************************/
    /* Perform register handler                                              */
    /************************************************************************/

    /* check if the engine is exist?*/
    if (t_media_format.e_media_type == MEDIA_TYPE_CONTAINER)
    {
        SLIST_FOR_EACH(pt_handler_reg, &gt_mfmtrecg_handler_cntnr_reg_list, t_link)
        {
            if (x_strncmp(pt_handler_reg->s_handler_name, ps_name, MAX_HANDLER_NAME_LEN) == 0)
            {
                return MFMTRECGR_NAME_EXIST;
            }
        }
    }
    else
    {
        SLIST_FOR_EACH(pt_handler_reg, &gt_mfmtrecg_handler_es_reg_list, t_link)
        {
            if (x_strncmp(pt_handler_reg->s_handler_name, ps_name, MAX_HANDLER_NAME_LEN) == 0)
            {
                return MFMTRECGR_NAME_EXIST;
            }
        }
    }

    /* malloc node */
    pt_handler_reg = (MFMTRECG_HANDLER_REG_T*) _mm_util_mem_alloc(sizeof(MFMTRECG_HANDLER_REG_T));
    if (pt_handler_reg == NULL)
    {
        return MFMTRECGR_OUT_OF_MEMORY;
    }
    x_memset(pt_handler_reg, 0, sizeof(MFMTRECG_HANDLER_REG_T));

    /*extension*/
    if (t_media_format.e_media_type != MEDIA_TYPE_CONTAINER)
    {
        while(x_strncmp(ps_favor[ui1_i] ,FILE_EXTENSION_END_CODE,MAX_EXTENSION_NAME_LEN)!=0)
        {
            pt_node_favor = (MFMREC_FILE_EXT_T*) _mm_util_mem_alloc(sizeof(MFMREC_FILE_EXT_T));
            if(pt_node_favor == NULL)
            {
                if (pt_handler_reg != NULL)
                {
                    _mm_util_mem_free(pt_handler_reg);
                    pt_handler_reg = NULL;
                }
                return MFMTRECGR_OUT_OF_MEMORY;
            }
            x_memset(pt_node_favor, 0, sizeof(MFMREC_FILE_EXT_T));
            
            ps_ext = (CHAR*) _mm_util_mem_alloc(x_strlen(ps_favor[ui1_i]) + 1/*NULL terminator*/); 
            if(ps_ext  == NULL)
            {
                if (pt_handler_reg != NULL)
                {
                    _mm_util_mem_free(pt_handler_reg);
                    pt_handler_reg = NULL;
                }
                if (pt_node_favor != NULL)
                {
                    _mm_util_mem_free(pt_node_favor);
                    pt_node_favor = NULL;
                }
                return MFMTRECGR_OUT_OF_MEMORY;
            }
            x_memset(ps_ext, 0, (x_strlen(ps_favor[ui1_i]) + 1));
            pt_node_favor->ps_file_ext = ps_ext;
            x_strncpy(pt_node_favor->ps_file_ext, ps_favor[ui1_i], x_strlen(ps_favor[ui1_i]));
            x_str_toupper(pt_node_favor->ps_file_ext);
            SLIST_INSERT_HEAD(pt_node_favor,&(pt_handler_reg->t_favor_list), t_file_ext_list);
            ui1_i++;
        }
    }
    
    /*name*/
    x_strncpy(pt_handler_reg->s_handler_name, ps_name, MAX_HANDLER_NAME_LEN);
    
    pt_handler_reg->t_handler_fct_tbl.pf_fconfirm = pt_fct_tbl->pf_fconfirm;
    pt_handler_reg->t_handler_fct_tbl.pf_mconfirm = pt_fct_tbl->pf_mconfirm;
    pt_handler_reg->t_handler_fct_tbl.pf_mfmtrecg_cli_init
                                            = pt_fct_tbl->pf_mfmtrecg_cli_init;
    
    pt_handler_reg->t_format = t_media_format;
    
    /*add new node: 
    */
    if (t_media_format.e_media_type == MEDIA_TYPE_CONTAINER)
    {
        SLIST_INSERT_HEAD(pt_handler_reg, &gt_mfmtrecg_handler_cntnr_reg_list, t_link);
    }
    else
    {
        SLIST_INSERT_HEAD(pt_handler_reg, &gt_mfmtrecg_handler_es_reg_list, t_link);
    }
    return MFMTRECGR_OK;
}


 /*-----------------------------------------------------------------------------
  * Name:  mfmtrecg_send_notify_msg
  *
  * Description: 
  *
  * Inputs:  
  *          
  * Outputs: -
  *
  * Returns: 
  ----------------------------------------------------------------------------*/                            
 INT32 mfmtrecg_send_notify_msg (MFMTRECG_ASYNC_MSG_T* pt_msg)
 {
     INT32 i4_ret;
 
     /************************************************************************/
     /* Check parameter                                                      */
     /************************************************************************/
     if (pt_msg == NULL) 
     {
         return MINFOR_INV_ARG;
     }
     /************************************************************************/
     /* Do send message                                                      */
     /************************************************************************/
 
 
     i4_ret = x_msg_q_send( gh_notify_async_exe_msgq,
                        pt_msg,
                        sizeof( MFMTRECG_ASYNC_MSG_T ),
                        10 );
     if ( i4_ret != OSR_OK )
     {
         DBG_ERROR(("[mfmtrecg] gh_notify_async_exe_msgq full\n"));
         return MFMTRECGR_MSG_Q_ERR;
     }
 
     return i4_ret;
 }


 /*-----------------------------------------------------------------------------
  * Name: mfmtrecg_notify_async_exec_thread_main
  *
  * Description: 
  *
  * Inputs:  pv_arg References.
  *
  * Outputs: - 
  *
  * Returns: -
  *
  ----------------------------------------------------------------------------*/
 static VOID mfmtrecg_notify_async_exec_thread_main( VOID*  pv_arg )
 {
     INT32                              i4_ret;
     SIZE_T                             z_msg_size;  
     UINT16                             ui2_index;
     MFMTRECG_ASYNC_MSG_T               t_msg;
     HANDLE_TYPE_T                      e_handle_type;
     VOID*                              pv_obj = NULL;
     MFMTRECG_OBJ_T*                    pt_obj;
     MEDIA_FORMAT_T*                    pt_format;
     x_mfmtrecg_nfy_fct                 pf_nfy;
     MFMTRECG_NFY_EVENT_TYPE_T          t_nfy_type;
     VOID*                              pv_tag;
     INT32                              i4_error_data = MFMTRECGR_INITIAL_ERR;
 
     while(1)
     {
         z_msg_size = sizeof(MFMTRECG_ASYNC_MSG_T);
         
         i4_ret = x_msg_q_receive( &ui2_index,
                                   &t_msg,
                                   &z_msg_size,
                                   &gh_notify_async_exe_msgq,
                                   (UINT16)1,
                                   X_MSGQ_OPTION_WAIT );
         if ( i4_ret == OSR_OK )
         {
            x_sema_lock(gh_mfmtrecg_obj_lock, X_SEMA_OPTION_WAIT);
            
            i4_ret = handle_get_type_obj(t_msg.h_handle, &e_handle_type, &pv_obj);
            if ((i4_ret != HR_OK) 
                || ((e_handle_type != HT_MFMTRECG_FILE_TYPE) && (e_handle_type != HT_MFMTRECG_MEM_TYPE))
                || (pv_obj == NULL))
            {
                x_sema_unlock(gh_mfmtrecg_obj_lock);
                continue;
            }
            pt_obj = (MFMTRECG_OBJ_T*)pv_obj;
            
            if (t_msg.e_msg_type == MFMTRECG_MSG_TYPE_FILE)
            {
                if (t_msg.u.t_fmtrecog_file_msg.ps_name != NULL)
                {
                    pt_format = pt_obj->pt_format;
#ifndef __NO_FM__
                    i4_ret = x_fm_open(FM_ROOT_HANDLE, t_msg.u.t_fmtrecog_file_msg.ps_name, 
                                       FM_READ_ONLY, 0777, 
                                       TRUE, /*no cache*/
                                       &pt_obj->h_file);
                    if (i4_ret != FMR_OK)
                    {                        
                        DBG_INFO(("x_fm_open(%s) error, ret = %d!\n", 
                                t_msg.u.t_fmtrecog_file_msg.ps_name, i4_ret));                        
                        t_nfy_type = MFMTRECG_NFY_EVENT_RECGNIZE_ERROR;                    
                    }
                    else
                    {
                        i4_ret = mfmtrecg_get_file_fmt_by_handle(pt_obj->h_file, pt_format);
                        if (i4_ret == MFMTRECGR_OK)
                        {
                            t_nfy_type = MFMTRECG_NFY_EVENT_RECGNIZE_DONE;
                            DBG_INFO(("File recognize done! ok! \r\n"));
                        }
                        else
                        {
                            t_nfy_type = MFMTRECG_NFY_EVENT_RECGNIZE_ERROR;
                            DBG_LOG_PRINT(("File recognize done! fail(%d)! \r\n",i4_ret));
                            i4_error_data = i4_ret;
                        }

                        if (pt_obj->h_file)
                        {
                            x_fm_close(pt_obj->h_file);
                            pt_obj->h_file = NULL_HANDLE;
                        }
                    }
#else
                    t_nfy_type = MFMTRECG_NFY_EVENT_RECGNIZE_ERROR;
                    DBG_LOG_PRINT(("FM not support! \r\n"));
#endif
                    pf_nfy = pt_obj->pf_nfy;
                    pv_tag = pt_obj->pv_tag;

                    if (pf_nfy != NULL)
                    {
                        pf_nfy(t_nfy_type, (VOID*)&i4_error_data, NULL, pv_tag);
                    }
                }
            }
            else if (t_msg.e_msg_type == MFMTRECG_MSG_TYPE_MEM)
            {
                if (t_msg.u.t_fmtrecog_mem_msg.pv_data != NULL)
                {
                    pt_format = pt_obj->pt_format;
                    i4_ret = mfmtrecg_get_mem_fmt(t_msg.u.t_fmtrecog_mem_msg.pv_data,
                                                  t_msg.u.t_fmtrecog_mem_msg.z_size,
                                                  pt_format);
                    if (i4_ret == MFMTRECGR_OK)
                    {
                        t_nfy_type = MFMTRECG_NFY_EVENT_RECGNIZE_DONE;
                        MUTIL_DBG_LOG_MW_0(("Mem recognize done! ok! \r\n"));
                    }
                    else
                    {
                        t_nfy_type = MFMTRECG_NFY_EVENT_RECGNIZE_ERROR;
                        DBG_LOG_PRINT(("Mem recognize done! fail(%d)! \r\n",i4_ret));
                        i4_error_data = i4_ret;
                    }
                    
                    pf_nfy = pt_obj->pf_nfy;
                    pv_tag = pt_obj->pv_tag;

                    if (pf_nfy != NULL)
                    {
                        pf_nfy(t_nfy_type, (VOID*)&i4_error_data, NULL, pv_tag);
                    }
                }
            }
            else if (t_msg.e_msg_type == MFMTRECG_MSG_TYPE_CLOSED)
            {
                if (pt_obj->h_file)
                {
#ifndef __NO_FM__
                    x_fm_close(pt_obj->h_file);
#endif
                    pt_obj->h_file = NULL_HANDLE;
                }
                x_sema_unlock(pt_obj->h_sync_lock);
            }
            x_sema_unlock(gh_mfmtrecg_obj_lock);
        }
    }
}


 /*-----------------------------------------------------------------------------
 * Name:  mfmtrecg_init
 *
 * Description: This function is a selector's API. This function is called by system initial procedure when system boot-up to init media format recognizer module.
 *
 * Inputs:  NONe.
 * Outputs: NONE.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 mfmtrecg_init (void)
{ 
    /*it could be called multiple times when mp3 aac ... recognizer inital*/
    INT32 i4_ret;

    if (gb_init == TRUE)
    {
        return MFMTRECGR_OK;
    }
    
    if (gh_notify_async_exe_msgq == NULL_HANDLE)
    {
        /* Create notify and async execute msg queue */
        i4_ret = x_msg_q_create( &gh_notify_async_exe_msgq,
                                 (CHAR*)MFMTRECG_NFY_ASYNC_EXEC_MSG_Q_NAME,
                                 sizeof( MFMTRECG_ASYNC_MSG_T ),
                                 MFMTRECG_NFY_ASYNC_EXEC_NUM_OF_MSGS );
        if ( i4_ret != OSR_OK )
        {
            return MFMTRECGR_INITIAL_ERR;
        }
    }

    if (gh_notify_async_exe_thread == NULL_HANDLE)
    {
        /* Start image Selector Thread */
        i4_ret = x_thread_create( &gh_notify_async_exe_thread,
                          MFMTRECG_NFY_ASYNC_EXEC_THREAD_NAME,
                          MFMTRECG_NFY_ASYNC_EXEC_THREAD_DEFAULT_STACK_SIZE,
                          MFMTRECG_NFY_ASYNC_EXEC_THREAD_DEFAULT_PRIORITY,
                          &mfmtrecg_notify_async_exec_thread_main,
                          0,
                          NULL );
        if ( i4_ret != OSR_OK )
        {
            return MFMTRECGR_INITIAL_ERR;
        }
    }

    if (gh_mfmtrecg_obj_lock == NULL_HANDLE)
    {
        i4_ret = x_sema_create( &gh_mfmtrecg_obj_lock,
                                 X_SEMA_TYPE_MUTEX,
                                 X_SEMA_STATE_UNLOCK );

        if ( i4_ret != OSR_OK || gh_mfmtrecg_obj_lock == NULL_HANDLE )
        {
            return MFMTRECGR_INITIAL_ERR;
        }
    }
    
    gb_init = TRUE;
    return MFMTRECGR_OK;
}




