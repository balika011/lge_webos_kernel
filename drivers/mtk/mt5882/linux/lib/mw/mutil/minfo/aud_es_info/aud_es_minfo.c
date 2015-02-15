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
 * aud_es_minfo.c,v
 * 1.1.2.1 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * willyhuang
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/3 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 0a0e5908fac0e73885ce39efe82bcd61 $ Asa
 *
 * Description: 
 *         This file contains all the media info public APIs  
           Author: lianming mcn07123 
           History:
           (1)2008-3-10  lianming mcn07123  initial

 *---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


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
#include "u_minfo.h"
#include "mutil/minfo/minfo.h"
#include "mutil/minfo/minfo_dbg.h"

#include "x_dbg.h"

#ifndef __NO_FM__
#include "x_fm.h"
#endif

#include "x_lnk_list.h"
#include "u_dbg.h"
#include "mutil/mm_util_input.h"
#include "mutil/mm_util.h"

//#include "x_drm.h"
#include "x_uc_str.h"

#else

#include "x_os.h"
#include "x_common.h"
#include "x_mm_common.h"

#include "handle.h"
#include "u_handle_grp.h"
#include "../u_minfo.h"
#include "../minfo.h"
#include "x_dbg.h"

#include "../../mm_util.h"
#include "../../minfo/minfo_dbg.h"

#include "x_lnk_list.h"
#include "u_dbg.h"
#include "../../mm_util_input.h"
//#include "x_drm.h"
#include "x_uc_str.h"

#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define AUD_ES_MINFO_USE_CACHE                     (1)

#define AUD_ES_MINFO_FS_NETWORK_SIZE_THRESHOLD (30*1024)
#define AUD_ES_MINFO_FS_SIZE_THRESHOLD         (10*1024*1024)
#define AUD_ES_MINFO_INIT_BUFF_LEN             (UINT32)10000
#define AUD_ES_MINFO_FRAME_REF_CNT              (1)
#define AUD_ES_MINFO_LOOKUP_LENGTH             (1*1024*1024)

#define AUD_ES_MINFO_AC3_SYNCWORD                     (UINT16)0x0b77
#define AUD_ES_MINFO_DTS_SYNCWORD                     (UINT32)0x7ffe8001  
#define AUD_ES_MINFO_DTS_SYNCWORD_EX                  (UINT32)0xff1f00e8  
#define AUD_ES_MINFO_SPEC_AC3_SYNCWORD                (UINT16)0x770b  
#define AUD_ES_MINFO_AC3_FREQ_SAMPLE_CNT              (256*6)
#define AUD_ES_MINFO_AC3_FRM_SIZE_COD_MAX             (38)
#define AUD_ES_MINFO_AC3_FREQ_SAMPLING_MAX            (3)
#define AUD_ES_MINFO_AC3_FRAME_SIZE_OFFSET            (5)
#define AUD_ES_MINFO_DTS_FRAME_SIZE_OFFSET            (9)
#define AUD_ES_MINFO_SPEC_AC3_FRAME_SIZE_OFFSET       (6)

#define AUD_ES_MINFO_AAC_HEAD_LENGTH                  (7)
#define AUD_ES_MINFO_ADIF_ID                         (UINT32)0x41444946
#define AUD_ES_MINFO_AAC_SYNCWORD                    (UINT16)0xfff0
#define AUD_ES_MINFO_ADTS_FREQ_SAMPLING_MAX           (12)
#define AUD_ES_MINFO_ADTS_FREQ_SAMPLE_CNT            (256*4)
#define AUD_ES_MINFO_ADTS_FRAME_SIZE_OFFSET          (6)

#define FMT_MAGIC_NUM_AC3               (UINT64)(MEDIA_TYPE_AUDIO_ES << 4 | MEDIA_AUD_SUBTYPE_AC3 << 2 | 1)
#define FMT_MAGIC_NUM_SPEC_AC3          (UINT64)(MEDIA_TYPE_AUDIO_ES << 4 | MEDIA_AUD_SUBTYPE_AC3 << 2 | 2)
#define FMT_MAGIC_NUM_EC3               (UINT64)(MEDIA_TYPE_AUDIO_ES << 4 | MEDIA_AUD_SUBTYPE_AC3 << 2 | 3)
#define FMT_MAGIC_NUM_SPEC_EC3          (UINT64)(MEDIA_TYPE_AUDIO_ES << 4 | MEDIA_AUD_SUBTYPE_AC3 << 2 | 4)
#define FMT_MAGIC_NUM_DTS               (UINT64)(MEDIA_TYPE_AUDIO_ES << 4 | MEDIA_AUD_SUBTYPE_DTS << 2 | 1)
#define FMT_MAGIC_NUM_SPEC_DTS          (UINT64)(MEDIA_TYPE_AUDIO_ES << 4 | MEDIA_AUD_SUBTYPE_DTS << 2 | 2)

#define LITTLE_TO_BIG_WORD(wValue)   (  ((wValue&0x00FF)<<8)|     \
                                        ((wValue&0xFF00)>> 8))
                                        
#define ID3_TAG_HEADER_LENGTH                       (10)
#define AUD_ES_CHECK_PARSE_READY                    (10000) /*ms*/

typedef struct _MINFO_ES_ACC_ID3_T
{
    SIZE_T                  z_title_len;
    UTF16_T*                pwc_str_title;
 
    SIZE_T                  z_genre_len;
    UTF16_T*                pwc_str_genre;

    SIZE_T                  z_artist_len;
    UTF16_T*                pwc_str_artist;

    SIZE_T                  z_album_len;
    UTF16_T*                pwc_str_album;

    SIZE_T                  z_year_len;
    UTF16_T*                pwc_str_year;
    
}MINFO_ES_ACC_ID3_T;

/*store all parsered infomation */
typedef struct _MINFO_AUD_ES_HANDLER_T
{
    MINFO_TYPE_GENERIC_T              t_generic;
    MINFO_ES_AUDIO_STM_ATRBT_T        t_es_audio_stm_attr;
    MINFO_ES_ACC_ID3_T                t_es_acc_id3_attr;
    /* Current parser offset, file base */
    BOOL                               b_too_big_file;
    UINT64                             ui8_lookup_length;
    UINT64                             ui8_file_size;
    
    
    /*Header buffer size*/
    UINT64                             ui8_cache_pos;
    UINT32                             ui4_buf_length;
    /*store current parsed object buffer*/
    UINT8*                             pui1_cache_buf;

    UINT64                             ui8_cur_parser_offset;
    UINT32                             ui4_read_count;
    UINT64                             ui8_data_offset;
    HANDLE_T                           h_chk_parse_timer;
    BOOL                               b_chk_parse_timer_out;
} MINFO_AUD_ES_HANDLER_T;


/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
static UINT16 _aud_es_minfo_ac3_frame_size_table[AUD_ES_MINFO_AC3_FRM_SIZE_COD_MAX][AUD_ES_MINFO_AC3_FREQ_SAMPLING_MAX] =
{
    {64,     69,     96},
    {64,     70,     96},
    {80,     87,     120},
    {80,     88,     120},
    {96,     104,    144},
    {96,     105,    144},
    {112,    121,    168},
    {112,    122,    168},
    {128,    139,    192},
    {128,    140,    192},
    {160,    174,    240},
    {160,    175,    240},
    {192,    208,    288},
    {192,    209,    288},
    {224,    243,    336},
    {224,    244,    336},
    {256,    278,    384},
    {256,    279,    384},
    {320,    348,    480},
    {320,    349,    480},
    {384,    417,    576},
    {384,    418,    576},
    {448,    487,    672},
    {448,    488,    672},
    {512,    557,    768},
    {512,    558,    768},
    {640,    696,    960},
    {640,    697,    960},
    {768,    835,    1152},
    {768,    836,    1152},
    {896,    975,    1344},
    {896,    976,    1344},
    {1024,   1114,   1536},
    {1024,   1115,   1536},
    {1152,   1253,   1728},
    {1152,   1254,   1728},
    {1280,   1393,   1920},
    {1280,   1394,   1920}
};

static UINT32 _aud_es_minfo_ac3_bit_rate_table[] = 
{
    32000,    40000,    48000,    56000,    64000,    80000,    96000,    112000,    128000,
    160000,    192000,    224000,    256000,    320000,    384000,    448000,    512000,    576000,
    640000
};

static UINT32 _aud_es_minfo_ac3_sampling_freq[4] = {48000, 44100, 32000, 0};
static UINT32 _aud_es_minfo_adts_sampling_freq[16] = 
{
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
    16000, 12000, 11025, 8000,   0,    0,    0,     0
};

#define AMR_HEAD_LEN                    (6)
#define AWB_HEAD_LEN                    (9)

static UINT32 _aud_es_minfo_dts_bit_rate[29] = 
{
    32000,         56000,         64000,         96000,         112000,     128000,     192000,     224000,
    256000,     320000,     384000,     448000,      512000,     576000,      640000,      768000,
    960000,     1024000,     1152000,     1280000,     1344000,     1408000,     1411200,     1472000,
    1536000,     1920000,     2048000,     3072000,     3840000
};

const static UINT32 cui4_ID3  = 0x00494433;             /* "ID3"        */
const static UINT32 cui4_TPE1 = 0x54504531;             /* "TPE1" Artist*/
const static UINT32 cui4_TYER = 0x54594552;             /* "TYER" Year  */
const static UINT32 cui4_TALB = 0x54414c42;             /* "TALB" Album */
//const static UINT32 cui4_TDAT = 0x54444154;             /* "TDAT" date  */    
const static UINT32 cui4_TIT2 = 0x54495432;             /* "TIT2" title */
const static UINT32 cui4_TCON = 0x54434f4e;             /* "TCON" genre */

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/


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

static INT32 _aud_es_minfo_getpos(MINFO_OBJ_T*            pt_minfo_obj,
                                  UINT64*                 pui8_offset)
{
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pt_minfo_obj == NULL)
    {
        return INPUTR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return INPUTR_INV_ARG;
    }
    
#if (AUD_ES_MINFO_USE_CACHE == 1)
    *pui8_offset = pt_aud_es_hdlr->ui8_cur_parser_offset;
#else
    pt_minfo_obj->t_input_fct_tbl.pf_get_pos(pt_minfo_obj, pui8_offset);
#endif
    return INPUTR_OK;
}

static INT32 _aud_es_minfo_setpos(MINFO_OBJ_T*           pt_minfo_obj,
                                  INT64                  i8_offset, 
                                  UINT8                  ui1_whence)
{
#if (AUD_ES_MINFO_USE_CACHE == 1)    
    INT64                       i8_pos;
    setpos_fct                  pf_set_pos;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pt_minfo_obj == NULL)
    {
        return INPUTR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return INPUTR_INV_ARG;
    }
    
    pf_set_pos = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    switch (ui1_whence)
    {
        case MEDIA_SEEK_BGN:
            i8_pos = i8_offset;
            break;
            
        case MEDIA_SEEK_CUR: 
            i8_pos = (INT64)(pt_aud_es_hdlr->ui8_cur_parser_offset);
            i8_pos = i8_pos + i8_offset;
            break;
            
        case MEDIA_SEEK_END:
            i8_pos = (INT64)(pt_aud_es_hdlr->ui8_file_size);
            i8_pos = i8_pos + i8_offset;
            break;
            
        default:
            return INPUTR_INV_ARG;
    }

    if (i8_pos < 0)
    {
        return INPUTR_FAIL;
    }

    if (i8_pos > (INT64)pt_aud_es_hdlr->ui8_file_size)
    {
        return INPUTR_EOF;
    }

    pt_aud_es_hdlr->ui8_cur_parser_offset = (UINT64)i8_pos;

    if (pt_aud_es_hdlr->ui8_cur_parser_offset < pt_aud_es_hdlr->ui8_cache_pos
        || (pt_aud_es_hdlr->ui8_cur_parser_offset + 1 
            > pt_aud_es_hdlr->ui8_cache_pos + pt_aud_es_hdlr->ui4_read_count))
    {
        pt_aud_es_hdlr->ui4_read_count = 0;
        pf_set_pos(pt_minfo_obj, i8_pos, MEDIA_SEEK_BGN);
    }

    return INPUTR_OK;
    
#else
    setpos_fct             pf_set_pos;

    pf_set_pos = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;
    pf_set_pos(pt_minfo_obj, i8_offset, ui1_whence);
    return INPUTR_OK;
    
#endif
}

INT32 _aud_es_minfo_input1byte(MINFO_OBJ_T*            pt_minfo_obj,
                               UINT8*                  pui1_data)
{
#if (AUD_ES_MINFO_USE_CACHE == 1)
    UINT64                      ui8_pos;
    copybytes_fct               pf_copybytes;
    setpos_fct                  pf_set_pos;
    INT32                       i4_ret         = INPUTR_OK;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pt_minfo_obj == NULL) || (pui1_data == NULL))
    {
        return INPUTR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return INPUTR_INV_ARG;
    }
    
    i4_ret       = INPUTR_OK;
    pf_copybytes = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    pf_set_pos   = pt_minfo_obj->t_input_fct_tbl.pf_set_pos;

    _aud_es_minfo_getpos(pt_minfo_obj, &ui8_pos);

    if ((ui8_pos + 1 > pt_aud_es_hdlr->ui8_file_size)
         || (ui8_pos + 1 > pt_aud_es_hdlr->ui8_lookup_length))
    {   
        if (ui8_pos + 1 > pt_aud_es_hdlr->ui8_lookup_length)
        {
            pt_aud_es_hdlr->b_too_big_file = TRUE;
        }
        return INPUTR_EOF;
    }

    if ((0 == pt_aud_es_hdlr->ui4_read_count)
        || (pt_aud_es_hdlr->ui8_cur_parser_offset < pt_aud_es_hdlr->ui8_cache_pos)
        || (pt_aud_es_hdlr->ui8_cur_parser_offset + 1 
            > pt_aud_es_hdlr->ui8_cache_pos + pt_aud_es_hdlr->ui4_read_count))
    {
        i4_ret = pf_set_pos(pt_minfo_obj, 
                           (INT64)pt_aud_es_hdlr->ui8_cur_parser_offset, 
                           MEDIA_SEEK_BGN);
        if (i4_ret != INPUTR_OK)
        {
            return i4_ret;
        }
        i4_ret = pf_copybytes(pt_minfo_obj, 
                              pt_aud_es_hdlr->pui1_cache_buf, 
                              pt_aud_es_hdlr->ui4_buf_length, 
                              pt_aud_es_hdlr->ui4_buf_length, 
                              &(pt_aud_es_hdlr->ui4_read_count));
        if ((i4_ret != INPUTR_OK) && (i4_ret != INPUTR_EOF))
        {
            return i4_ret;
        }

        pt_aud_es_hdlr->ui8_cache_pos = pt_aud_es_hdlr->ui8_cur_parser_offset;
    }

    *pui1_data = pt_aud_es_hdlr->pui1_cache_buf[pt_aud_es_hdlr->ui8_cur_parser_offset 
                                                - pt_aud_es_hdlr->ui8_cache_pos];
    ++(pt_aud_es_hdlr->ui8_cur_parser_offset);

    return INPUTR_OK;

#else
    UINT64                      ui8_pos;  
    getpos_fct                  pf_get_pos;
    input1bytes_fct             pf_inp1byte;    
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pt_minfo_obj == NULL) || (pui1_data == NULL))
    {
        return INPUTR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return INPUTR_INV_ARG;
    }
    
    pf_inp1byte = pt_minfo_obj->t_input_fct_tbl.pf_input1;
    pf_get_pos  = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;
    pf_get_pos(pt_minfo_obj, &ui8_pos);

    if ((ui8_pos + 1 > pt_aud_es_hdlr->ui8_file_size)
        || (ui8_pos + 1 >= pt_aud_es_hdlr->ui8_lookup_length))
    {
        if (ui8_pos + 1 > pt_aud_es_hdlr->ui8_lookup_length)
        {
            pt_aud_es_hdlr->b_too_big_file = TRUE;
        }
        return INPUTR_EOF;
    }
    *pui1_data = pf_inp1byte(pt_minfo_obj, pui1_data);

    return INPUTR_OK;
    
#endif    
}

static INT32 _aud_es_minfo_input2byte(MINFO_OBJ_T*          pt_minfo_obj,
                                      UINT16*               pui2_data)
{
#if (AUD_ES_MINFO_USE_CACHE == 1)
    UINT8                   ui1_idx;
    UINT8                   ui1_tmp;
    INT32                   i4_ret;

    for (ui1_idx = 0; ui1_idx < 2; ui1_idx++)
    {
        i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp);
        if (i4_ret != INPUTR_OK)
        {
            return i4_ret;
        }
        
        *pui2_data = (((*pui2_data) << 8) | ui1_tmp);
    }

    return i4_ret;

#else    
    UINT8                       ui1_idx;
    UINT8                       ui1_tmp;
    UINT64                      ui8_pos;  
    getpos_fct                  pf_get_pos;
    input1bytes_fct             pf_inp1byte;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    INT32                       i4_ret;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pt_minfo_obj == NULL) || (pui2_data == NULL))
    {
        return INPUTR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return INPUTR_INV_ARG;
    }
    
    pf_inp1byte = pt_minfo_obj->t_input_fct_tbl.pf_input1;
    pf_get_pos  = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;

    pf_get_pos(pt_minfo_obj, &ui8_pos);
    
    if ((ui8_pos + 2 > pt_aud_es_hdlr->ui8_file_size)
        || (ui8_pos + 2 > pt_aud_es_hdlr->ui8_lookup_length))
    {
        if (ui8_pos + 2 > pt_aud_es_hdlr->ui8_lookup_length)
        {
            pt_aud_es_hdlr->b_too_big_file = TRUE;
        }
        return INPUTR_EOF;
    }

    *pui2_data = 0;
    for (ui1_idx = 0; ui1_idx < 2; ui1_idx++)
    {
        i4_ret = pf_inp1byte(pt_minfo_obj, &ui1_tmp);
        if (i4_ret != INPUTR_OK)
        {
            return i4_ret;
        }
        *pui2_data = (((*pui2_data) << 8) | ui1_tmp);
    }

    return INPUTR_OK;
#endif
}

/*Not be used now so remark*/
#if 0
static INT32 _aud_es_minfo_input3byte(MINFO_OBJ_T*          pt_minfo_obj,
                                      UINT32*               pui4_data)
{
#if (AUD_ES_MINFO_USE_CACHE == 1)
    UINT8               ui1_idx;
    UINT8               ui1_tmp;
    INT32               i4_ret;

    *pui4_data = 0;
    for (ui1_idx = 0; ui1_idx < 3; ui1_idx++)
    {
        i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp);
        if (i4_ret != INPUTR_OK)
        {
            return i4_ret;
        }
        
        *pui4_data = (((*pui4_data) << 8)|ui1_tmp);
    }

    return i4_ret;

#else
    UINT8                       ui1_idx;
    UINT8                       ui1_tmp;    
    UINT64                      ui8_pos;   
    getpos_fct                  pf_get_pos;
    input1bytes_fct             pf_inp1byte;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    INT32                       i4_ret;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pt_minfo_obj == NULL) || (pui4_data == NULL))
    {
        return INPUTR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return INPUTR_INV_ARG;
    }
    
    pf_inp1byte = pt_minfo_obj->t_input_fct_tbl.pf_input1;
    pf_get_pos  = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;

    pf_get_pos(pt_minfo_obj, &ui8_pos);
    
    if ((ui8_pos + 3 > pt_aud_es_hdlr->ui8_file_size)
         || (ui8_pos + 3 > pt_aud_es_hdlr->ui8_lookup_length))
    {
        if (ui8_pos + 3 > pt_aud_es_hdlr->ui8_lookup_length)
        {
            pt_aud_es_hdlr->b_too_big_file = TRUE;
        }
        return INPUTR_EOF;
    }

    *pui4_data = 0;
    for (ui1_idx = 0; ui1_idx < 3; ui1_idx++)
    {
        i4_ret = pf_inp1byte(pt_minfo_obj, &ui1_tmp);
        if (i4_ret != INPUTR_OK)
        {
            return i4_ret;
        }
        *pui4_data = (((*pui4_data) << 8) | ui1_tmp);
    }

    return INPUTR_OK;
#endif
}
#endif

static INT32 _aud_es_minfo_input4byte(MINFO_OBJ_T*          pt_minfo_obj,
                                      UINT32*               pui4_data)
{
#if (AUD_ES_MINFO_USE_CACHE == 1)
    UINT8                    ui1_idx;
    UINT8                    ui1_tmp;
    INT32                    i4_ret;

    for (ui1_idx = 0; ui1_idx < 4; ui1_idx++)
    {
        i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp);
        if (i4_ret != INPUTR_OK)
        {
            return i4_ret;
        }
        
        *pui4_data = (((*pui4_data) << 8) | ui1_tmp);
    }

    return i4_ret;

#else    
    UINT8                       ui1_idx;
    UINT8                       ui1_tmp;     
    UINT64                      ui8_pos;  
    getpos_fct                  pf_get_pos;
    input1bytes_fct             pf_inp1byte;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    INT32                       i4_ret;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pt_minfo_obj == NULL) || (pui4_data == NULL))
    {
        return INPUTR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return INPUTR_INV_ARG;
    }
    
    pf_inp1byte = pt_minfo_obj->t_input_fct_tbl.pf_input1;
    pf_get_pos = pt_minfo_obj->t_input_fct_tbl.pf_get_pos;

    pf_get_pos(pt_minfo_obj, &ui8_pos);
    
    if (ui8_pos + 4 > pt_aud_es_hdlr->ui8_file_size
        || ui8_pos + 4 > pt_aud_es_hdlr->ui8_lookup_length)
    {
        if (ui8_pos + 4 > pt_aud_es_hdlr->ui8_lookup_length)
        {
            pt_aud_es_hdlr->b_too_big_file = TRUE;
        }
        return INPUTR_EOF;
    }

    *pui4_data = 0;
    for (ui1_idx = 0; ui1_idx < 4; ui1_idx++)
    {
        i4_ret = pf_inp1byte(pt_minfo_obj, &ui1_tmp);
        if (i4_ret != INPUTR_OK)
        {
            return i4_ret;
        }
        *pui4_data = (((*pui4_data) << 8) | ui1_tmp);
    }
    
    return INPUTR_OK;
#endif
}

/*-----------------------------------------------------------------------------
 * Name:  _aud_es_adif_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *          ui8_pos      
 *  
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_adif_minfo_parse(MINFO_OBJ_T*       pt_minfo_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT8                       ui1_tmp_code;
    UINT16                      ui2_tmp_code;
    UINT32                      ui4_frame_cnt = 0; 
    UINT32                      ui4_bitrate;
    UINT32                      ui4_head;
    UINT64                      ui8_offset = 0;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pt_minfo_obj == NULL)
    {
        DBG_ERROR(("_aud_es_adif_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    i4_ret = MINFOR_OK;
    do
    {
        i4_ret = _aud_es_minfo_input4byte(pt_minfo_obj, &ui4_head);
        if ((i4_ret != INPUTR_OK) || (ui4_head != AUD_ES_MINFO_ADIF_ID))
        {
            break;
        }
    
        i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        if ((ui1_tmp_code & 0x80) >> 7)
        {
            /*copyright id 1 + 8*/
            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 8, MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
        }
        
        ui4_bitrate = ui1_tmp_code & 0x0f;
        i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        ui4_bitrate = ((ui4_bitrate << 16) | ui2_tmp_code);
        i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        ui4_bitrate = ((ui4_bitrate << 8) | ((ui1_tmp_code & 0xE0) >> 5));


        ui4_frame_cnt = 0;
        do
        {
            /*exit at any time*/
            if(pt_minfo_obj->b_abort_flag == TRUE)
            {
                return MINFOR_INTERNAL_ERR;
            }
           // DBG_INFO(("_aud_es_adif_minfo_parse: parsing all file !\r\n"));
            
            ui4_frame_cnt ++;
            i4_ret = _aud_es_minfo_getpos(pt_minfo_obj, &ui8_offset);
            if (i4_ret != INPUTR_OK)
            {
                i4_ret = INPUTR_OK;
                break;
            }
            else if (ui8_offset >= AUD_ES_MINFO_FS_SIZE_THRESHOLD)
            {
                break;
            }
            
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            if (ui1_tmp_code < 7)  /*id_syn_ele*/
            {
                i4_ret        = MINFOR_OK;
                break;
            }
            
            if (ui4_frame_cnt > 7 && pt_minfo_obj->b_filesize_non_gettable)
            {
                break;
            }
        } while (TRUE);
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        /*calculate bitrate*/
        if (ui4_bitrate != 0)
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64(pt_minfo_obj->z_buff_leng 
                                          - pt_aud_es_hdlr->ui8_cur_parser_offset, 
                                      (UINT64)ui4_bitrate, 
                                      NULL); 
            /*DBG_INFO(("_aud_es_adif_minfo_parse:"
                      "{Raw data begin position:0x%llx, "
                      "bitrate  = %u,"
                      "duration = %llu}\r\n", 
                      pt_aud_es_hdlr->ui8_cur_parser_offset, 
                      ui4_bitrate,
                      pt_aud_es_hdlr->t_generic.ui8_pb_duration));*/
            pt_aud_es_hdlr->t_generic.ui8_pb_duration *= 90000;
        }
        
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc = MINFO_INFO_AUD_ENC_AAC;
        return MINFOR_OK;
        
    } while (0);
    
    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name:  _aud_es_adts_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *          ui8_pos      
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_adts_minfo_parse(MINFO_OBJ_T*         pt_minfo_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT16                      ui2_tmp_code;
    UINT16                      ui2_frame_len;
    UINT32                      ui4_frame_cnt; 
    UINT8                       ui1_tmp_code;
    UINT32                      ui4_sample_freq = 0;
    UINT64                      ui8_offset = 0;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pt_minfo_obj == NULL)
    {
        DBG_ERROR(("_aud_es_aac_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    DBG_INFO(("_aud_es_aac_minfo_parse:begin parse adts\r\n"));
    i4_ret = MINFOR_OK;
    do
    {
        i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        ui4_frame_cnt = 0;
        /*check frames*/
        do
        {
            /*exit at any time*/
            if(pt_minfo_obj->b_abort_flag == TRUE)
            {
                return MINFOR_INTERNAL_ERR;
            }
           // DBG_INFO(("_aud_es_adts_minfo_parse: parsing all file !\r\n"));

            i4_ret = _aud_es_minfo_getpos(pt_minfo_obj, &ui8_offset);
            if (i4_ret != INPUTR_OK)
            {
                i4_ret = INPUTR_OK;
                break;
            }
            else if (ui8_offset >= pt_aud_es_hdlr->ui8_lookup_length)
            {
                pt_aud_es_hdlr->b_too_big_file = TRUE;
                break;
            }
            
            if (AUD_ES_MINFO_AAC_SYNCWORD != (ui2_tmp_code & 0xfff0))
            {
                i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
                if (i4_ret != INPUTR_OK)
                {
                    if (INPUTR_EOF == i4_ret)
                    {
                        i4_ret = MINFOR_OK;
                    }
                    break;
                }
                continue;
            }
            ++ui4_frame_cnt;

            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            /*DBG_INFO(("{Pos[0x%llx], Frame[%u], Head:%2x %2x %2x ", 
                      pt_aud_es_hdlr->ui8_cur_parser_offset - (UINT64)3, 
                      ui4_frame_cnt,
                      (ui2_tmp_code & 0xFF00) >> 8, 
                      ui2_tmp_code & 0x00FF,
                      ui1_tmp_code));*/
            
            if (AUD_ES_MINFO_FRAME_REF_CNT == ui4_frame_cnt)
            {
                /* first two bits means profile
                    00 main profile AAC main
                    01 Low complexity(LC) AAC LC
                    10 Scalable sample rate profile(SSR) AAC SSR 
                    11 reserved AAC LTP
                    now we only support AAC LC*/
                if ((ui1_tmp_code & 0xC0) != 0x40)
                {
                    DBG_INFO(("aac adts profile not support, ui1_tmp_code = %d\n", ui1_tmp_code));
                    return MINFOR_FMT_NOT_SUPPORT;
                }
                if (((ui1_tmp_code & 0x3c) >> 2) < AUD_ES_MINFO_ADTS_FREQ_SAMPLING_MAX)
                {
                    ui4_sample_freq = _aud_es_minfo_adts_sampling_freq[(ui1_tmp_code & 0x3c) >> 2];
                }
                //DBG_INFO(("sample_freq = %u\r\n", ui4_sample_freq));
            }
            
            /*seek next frame*/
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui2_frame_len = ((UINT16)(ui1_tmp_code & 0x03) << 11);
            //DBG_INFO(("%2x ", ui1_tmp_code));
            
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui2_frame_len |= ((UINT16)(ui1_tmp_code) << 3);
            //DBG_INFO(("%2x ", ui1_tmp_code));
            
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui2_frame_len |= ((UINT16)(ui1_tmp_code & 0xE0) >> 5);
            //DBG_INFO(("%2x, frame_len = %u}\r\n", ui1_tmp_code, ui2_frame_len));
            if (ui2_frame_len <= AUD_ES_MINFO_ADTS_FRAME_SIZE_OFFSET)
            {
                i4_ret = MINFOR_INITIAL_ERR;
                break;
            }

            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                    (INT64)(ui2_frame_len - AUD_ES_MINFO_ADTS_FRAME_SIZE_OFFSET),
                                    MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            
            /*read next frame*/
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                if (INPUTR_EOF == i4_ret)
                {
                    i4_ret = MINFOR_OK;
                }
                break;
            }
            if (ui4_frame_cnt > 7 && pt_minfo_obj->b_filesize_non_gettable)
            {
                i4_ret = MINFOR_OK;              
                break;
            }

            if (pt_aud_es_hdlr->b_chk_parse_timer_out)
            {
                i4_ret = MINFOR_OK;
                break;
            }
        } while (TRUE);
        if (MINFOR_OK != i4_ret)
        {
            break;
        }
        
        /*calculate duration*/
        if ((ui4_sample_freq != 0) && !(pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(AUD_ES_MINFO_ADTS_FREQ_SAMPLE_CNT * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
            DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
            pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
            pt_aud_es_hdlr->t_generic.ui4_sz_max_acces_unt  = ui4_frame_cnt;
        }

        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_sampling_frequency = ui4_sample_freq;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc                  = MINFO_INFO_AUD_ENC_AAC;
        DBG_INFO(("_aud_es_aac_minfo_parse:end parse adts\r\n"));
        return MINFOR_OK;
        
    } while (0);
    if (MINFOR_OK != i4_ret)
    {
    }
    
    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name:  _aud_es_amr_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2012-11-1 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_amr_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT8                       ui1_tmp_code;
    UINT32                      ui4_bitrate    = 0;
    UINT8                       ui1_frame_type = 0;
    UINT32                      ui4_frame_size = 0;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
#ifndef __KERNEL__
        DBG_LOG_PRINT(("_aud_es_amr_minfo_parse: NULL\r\n"));
#endif
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }
#ifndef __KERNEL__
    DBG_LOG_PRINT(("_aud_es_amr_minfo_parse:begin parse amr\r\n"));
#endif
    
    
    i4_ret = MINFOR_OK;

    do
    {
        i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                 AMR_HEAD_LEN, 
                                 MEDIA_SEEK_BGN);
        if (i4_ret != INPUTR_OK)
        {
            return MINFOR_INTERNAL_ERR;
        }
    
        i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }

    #ifndef __KERNEL__
        DBG_LOG_PRINT(("_aud_es_amr_minfo_parse:ui1_tmp_code = %x\r\n",ui1_tmp_code));
    #endif
    
        ui1_frame_type = ((ui1_tmp_code & 0x78) >> 3)& 0x0F;

    #ifndef __KERNEL__
        DBG_LOG_PRINT(("_aud_es_amr_minfo_parse:ui1_frame_type = %d\r\n",ui1_frame_type));
    #endif
        
        switch(ui1_frame_type)
        {
            case 0:
            {
                ui4_bitrate = 5200;
                ui4_frame_size = 13;
            }
            break;
            case 1:
            {
                ui4_bitrate = 5600;
                ui4_frame_size = 14;
            }
            break;
            case 2:
            {
                ui4_bitrate = 6400;
                ui4_frame_size = 16;
            }
            break;
            case 3:
            {
                ui4_bitrate = 7200;
                ui4_frame_size = 18;
            }
            break;
            case 4:
            {
                ui4_bitrate = 8000;
                ui4_frame_size = 20;
            }
            break;
            case 5:
            {
                ui4_bitrate = 8400;
                ui4_frame_size = 21;
            }
            break;
            case 6:
            {
                ui4_bitrate = 10800;
                ui4_frame_size = 27;
            }
            break;
            case 7:
            {
                ui4_bitrate = 12800;
                ui4_frame_size = 32;
            }
            break;
            default:
            {
                ui4_bitrate = 1;
                ui4_frame_size = 0;
            }
            break;
        }
        
        /*calculate bitrate*/
        if (ui4_bitrate != 0)
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((pt_minfo_obj->z_buff_leng 
                                          - AMR_HEAD_LEN)*8, 
                                      (UINT64)ui4_bitrate, 
                                      NULL);
            
            pt_aud_es_hdlr->t_generic.ui8_pb_duration *= 90000;
        }
        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_align = ui4_frame_size;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc = MINFO_INFO_AUD_ENC_AMR;
        pt_aud_es_hdlr->ui8_data_offset = 6;
        return MINFOR_OK;
        
    } while (0);
    
    return i4_ret;

}
/*-----------------------------------------------------------------------------
 * Name:  _aud_es_ac3_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_ac3_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT16                      ui2_tmp_code = 0;
    UINT16                      ui2_frame_len = 0;
    UINT32                      ui4_frame_cnt = 0; 
    UINT8                       ui1_tmp_code = 0;
    UINT32                      ui4_sample_freq = 0;
    UINT32                      ui4_sample_idx = 0;
    UINT32                      ui4_frm_sz_code_idx = 0;
    UINT32                      ui4_bit_rate = 0;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("_aud_es_ac3_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }

    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                 0, 
                                 MEDIA_SEEK_BGN);
    if (i4_ret != INPUTR_OK)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    DBG_INFO(("_aud_es_ac3_minfo_parse:begin parse ac3\r\n"));
    do
    {
        i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        /*remove garbage bits*/
        while (AUD_ES_MINFO_AC3_SYNCWORD != ui2_tmp_code)
        {
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui2_tmp_code  = ((ui2_tmp_code << 8) | ui1_tmp_code);
        }
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        ui4_frame_cnt = 0;
        
        /*check frames*/
        do
        {
            if (AUD_ES_MINFO_AC3_SYNCWORD != ui2_tmp_code)
            {
                i4_ret = MINFOR_FCT_NOT_SUPPORT;
                break;
            }
            ++ui4_frame_cnt;

            /*ignore crc*/
            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, (INT64)2, MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            
            /*get sampling frequency*/
            ui4_sample_idx      = ((ui1_tmp_code & 0xc0) >> 6);
            ui4_frm_sz_code_idx = (ui1_tmp_code & 0x3f);
            
            if ((ui4_sample_idx >= AUD_ES_MINFO_AC3_FREQ_SAMPLING_MAX)
                || 
                (ui4_frm_sz_code_idx >= AUD_ES_MINFO_AC3_FRM_SIZE_COD_MAX))
            {
                i4_ret = MINFOR_INTERNAL_ERR;
                break;
            }
            
            if (AUD_ES_MINFO_FRAME_REF_CNT == ui4_frame_cnt)
            {
                ui4_sample_freq = _aud_es_minfo_ac3_sampling_freq[ui4_sample_idx];
        if (ui4_frm_sz_code_idx <= sizeof(_aud_es_minfo_ac3_bit_rate_table) * 2)
        {
                    ui4_bit_rate = _aud_es_minfo_ac3_bit_rate_table[ui4_frm_sz_code_idx/2];
        }
                DBG_INFO(("sample_freq = %u, ui4_bit_rate = %u\r\n", ui4_sample_freq, ui4_bit_rate));
            }
            
            ui2_frame_len = (UINT16)((_aud_es_minfo_ac3_frame_size_table
                                    [ui4_frm_sz_code_idx][ui4_sample_idx]) * 2);
            /*DBG_INFO(("{Pos[0x%llx], Frame[%u], Head:%2x %u}\r\n", 
                      pt_aud_es_hdlr->ui8_cur_parser_offset - (UINT64)5, 
                      ui4_frame_cnt,
                      ui1_tmp_code, 
                      ui2_frame_len));*/

            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                         (INT64)ui2_frame_len  
                                          - (INT64)AUD_ES_MINFO_AC3_FRAME_SIZE_OFFSET, 
                                         MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            /*next frame*/
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                if (INPUTR_EOF == i4_ret)
                {
                    i4_ret = MINFOR_OK;
                }
                break;
            }
            
            if (pt_minfo_obj->b_filesize_non_gettable && ui4_frame_cnt > 7)
            {
                break;
            }

            if (pt_aud_es_hdlr->b_chk_parse_timer_out)
            {
                i4_ret = MINFOR_OK;
                break;
            }
        } while (TRUE);
        if (i4_ret != MINFOR_OK)
        {
            break;
        }

        /*calculate duration*/
        if ((ui4_sample_freq != 0) && !(pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(AUD_ES_MINFO_AC3_FREQ_SAMPLE_CNT * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
            DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
            pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
            pt_aud_es_hdlr->t_generic.ui4_sz_max_acces_unt  = ui4_frame_cnt;
        }
        else if((ui4_bit_rate != 0) && (pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                             = _mm_div64(pt_aud_es_hdlr->ui8_file_size * 8, 
                                         ui4_bit_rate, 
                                         NULL); 
                DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
                pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
        }
        
        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_sampling_frequency = ui4_sample_freq;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc                  = MINFO_INFO_AUD_ENC_AC3;

        pt_aud_es_hdlr->t_es_audio_stm_attr.b_big_endian = FALSE;
        DBG_INFO(("_aud_es_ac3_minfo_parse:end parse ac3\r\n"));
        return MINFOR_OK;
        
    } while(0);
    if (MINFOR_OK != i4_ret)
    {
    }
    
    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name:  _aud_es_dts_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : min bao(mtk40384)
 *
 * History:
 *   (1)2011-06-14 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_dts_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT8                       ui1_tmp_code = 0;
    UINT32                      ui4_tmp_code = 0;
    UINT32                      ui4_frame_len = 0;
    UINT32                      ui4_frame_cnt = 0; 
    UINT16                      ui2_nblk = 0;
    UINT16                      ui2_tmp1 = 0;
    UINT16                      ui2_tmp2 = 0;
    UINT32                      ui4_sample_freq = 0;
    UINT32                      ui4_sample_idx = 0;
    UINT32                      ui4_sample_num = 0;
    UINT32                        ui4_bit_rate = 0;
    UINT32                        ui4_pos_offset = 0;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("_aud_es_dts_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    DBG_INFO(("_aud_es_dts_minfo_parse:begin parse ac3\r\n"));
    i4_ret = MINFOR_OK;
    do
    {
        i4_ret = _aud_es_minfo_input4byte(pt_minfo_obj, &ui4_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        /*remove garbage bits*/
        while (AUD_ES_MINFO_DTS_SYNCWORD != ui4_tmp_code)
        {
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui4_tmp_code  = ((ui4_tmp_code << 8) | ui1_tmp_code);
            ui4_pos_offset ++;
            if (ui4_pos_offset >= AUD_ES_MINFO_LOOKUP_LENGTH)
            {
                break;
            }
        }
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        ui4_frame_cnt = 0;
        
        /*check frames*/
        do
        {
            if (AUD_ES_MINFO_DTS_SYNCWORD != ui4_tmp_code)
            {
                i4_ret = MINFOR_FCT_NOT_SUPPORT;
                break;
            }
            ++ui4_frame_cnt;

            /*ignore crc*/
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp1);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            ui2_nblk = (ui2_tmp1 & 0x01fc) >> 2;
            /*get sampling num*/
            ui4_sample_num      += (ui2_nblk + 1)*32;
            
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp2);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            
            ui4_frame_len = ui2_tmp1 << 16 | ui2_tmp2;
            ui4_frame_len = ((ui4_frame_len & 0x0003fff0) >> 4) + 1;
            
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp1);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            if (AUD_ES_MINFO_FRAME_REF_CNT == ui4_frame_cnt)
            {
                ui4_sample_idx = (ui2_tmp1 & 0x3c00) >> 10;    
                switch (ui4_sample_idx)
                {
                    case 1:
                        ui4_sample_freq = 8000;
                        break;
                    case 2:
                        ui4_sample_freq = 16000;
                        break;
                    case 3:
                        ui4_sample_freq = 32000;
                        break;
                    case 6:
                        ui4_sample_freq = 11025;
                        break;
                    case 7:
                        ui4_sample_freq = 22050;
                        break;
                    case 8:
                        ui4_sample_freq = 44100;
                        break;
                    case 11:
                        ui4_sample_freq = 12000;
                        break;
                    case 12:
                        ui4_sample_freq = 24000;
                        break;
                    case 13:
                        ui4_sample_freq = 48000;
                        break;
                    default:
                        break;
                }                
                //DBG_INFO(("sample_freq = %u\r\n", ui4_sample_freq));

                ui2_tmp2 = (ui2_tmp1 & 0x03e0) >> 5;
                if (ui2_tmp2 < sizeof(_aud_es_minfo_dts_bit_rate))
                {
                    ui4_bit_rate = _aud_es_minfo_dts_bit_rate[ui2_tmp2];
                    break;
                }
                //DBG_INFO(("bit rate = %u\r\n", ui4_bit_rate));
            }
            
            
            /*DBG_INFO(("{Pos[0x%llx], Frame[%u], Head:%2x %u}\r\n", 
                      pt_aud_es_hdlr->ui8_cur_parser_offset - (UINT64)5, 
                      ui4_frame_cnt,
                      ui1_tmp_code, 
                      ui4_frame_len));*/

            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                         (INT64)ui4_frame_len  
                                          - (INT64)AUD_ES_MINFO_DTS_FRAME_SIZE_OFFSET, 
                                         MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            /*next frame*/
            i4_ret = _aud_es_minfo_input4byte(pt_minfo_obj, &ui4_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                if (INPUTR_EOF == i4_ret)
                {
                    i4_ret = MINFOR_OK;
                }
                break;
            }
            
            if (pt_minfo_obj->b_filesize_non_gettable && ui4_frame_cnt > 7)
            {
                break;
            }

            if (pt_aud_es_hdlr->b_chk_parse_timer_out)
            {
                i4_ret = MINFOR_OK;
                break;
            }
            
        } while (TRUE);
        if (i4_ret != MINFOR_OK)
        {
            break;
        }

        /*calculate duration*/
        if (ui4_bit_rate != 0 && !(pt_minfo_obj->b_filesize_non_gettable) && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                = _mm_div64(pt_aud_es_hdlr->ui8_file_size * 8, 
                                         ui4_bit_rate,
                                         NULL);
            pt_aud_es_hdlr->t_generic.ui8_pb_duration = 
                pt_aud_es_hdlr->t_generic.ui8_pb_duration * 90000;
        }
        else if ((ui4_sample_freq != 0) && !(pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)ui4_sample_num, 
                                      ui4_sample_freq, 
                                      NULL); 
            DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
            pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
            pt_aud_es_hdlr->t_generic.ui4_sz_max_acces_unt  = ui4_frame_cnt;
        }
        
        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_sampling_frequency = ui4_sample_freq;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc                  = MINFO_INFO_AUD_ENC_DTS;

        pt_aud_es_hdlr->t_es_audio_stm_attr.b_big_endian = TRUE;
        DBG_INFO(("_aud_es_dts_minfo_parse:end parse ac3\r\n"));
        return MINFOR_OK;
        
    } while(0);
    if (MINFOR_OK != i4_ret)
    {
    }
    
    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name:  _aud_es_dts_ex_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : min bao(mtk40384)
 *
 * History:
 *   (1)2012-03-19 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_dts_ex_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT8                       ui1_tmp1 = 0;
    //UINT8                ui1_tmp2 = 0;
    //UINT8                       ui1_tmp3 = 0;
    UINT8                        ui1_tmp4 = 0;
    UINT32                      ui4_tmp_code = 0;
    //UINT32                      ui4_frame_len = 0;
    UINT32                      ui4_frame_cnt = 0; 
    UINT16                      ui2_tmp1 = 0;
    UINT32                      ui4_sample_freq = 0;
    UINT32                      ui4_sample_idx = 0;
    UINT32                      ui4_sample_num = 0;
    UINT32                        ui4_bit_rate = 0;
    UINT32                        ui4_pos_offset = 0;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("_aud_es_dts_ex_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    DBG_INFO(("_aud_es_dts_ex_minfo_parse:begin parse dts\r\n"));
    i4_ret = MINFOR_OK;
    do
    {
        i4_ret = _aud_es_minfo_input4byte(pt_minfo_obj, &ui4_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        /*remove garbage bits*/
        while (AUD_ES_MINFO_DTS_SYNCWORD_EX != ui4_tmp_code)
        {
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp1);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui4_tmp_code  = ((ui4_tmp_code << 8) | ui1_tmp1);
            ui4_pos_offset ++;
            if (ui4_pos_offset >= AUD_ES_MINFO_LOOKUP_LENGTH)
            {
                break;
            }
        }
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        ui4_frame_cnt = 0;
        
        /*check frames*/
        do
        {
            if (AUD_ES_MINFO_DTS_SYNCWORD_EX != ui4_tmp_code)
            {
                i4_ret = MINFOR_FCT_NOT_SUPPORT;
                break;
            }
            ++ui4_frame_cnt;

            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp1);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            i4_ret = _aud_es_minfo_input4byte(pt_minfo_obj, &ui4_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            //ui1_tmp1 = (UINT8)((ui4_tmp_code & 0x00030000) >> 16);
            //ui1_tmp2 = (UINT8)((ui4_tmp_code & 0xff000000) >> 24);
            //ui1_tmp3 = (UINT8)((ui4_tmp_code & 0x000000ff));
            ui1_tmp4 = (UINT8)((ui4_tmp_code & 0x0000ff00) >> 8);

            //ui4_frame_len = ui1_tmp1 << 12 | ui1_tmp2 << 4 | ui1_tmp3 >> 4;
                        
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp1);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            ui1_tmp1 = (UINT8)(ui2_tmp1 & 0x00ff);

            if (AUD_ES_MINFO_FRAME_REF_CNT == ui4_frame_cnt)
            {
                ui4_sample_idx = (ui1_tmp4 & 0x0f);    
                switch (ui4_sample_idx)
                {
                    case 1:
                        ui4_sample_freq = 8000;
                        break;
                    case 2:
                        ui4_sample_freq = 16000;
                        break;
                    case 3:
                        ui4_sample_freq = 32000;
                        break;
                    case 6:
                        ui4_sample_freq = 11025;
                        break;
                    case 7:
                        ui4_sample_freq = 22050;
                        break;
                    case 8:
                        ui4_sample_freq = 44100;
                        break;
                    case 11:
                        ui4_sample_freq = 12000;
                        break;
                    case 12:
                        ui4_sample_freq = 24000;
                        break;
                    case 13:
                        ui4_sample_freq = 48000;
                        break;
                    default:
                        break;
                }                
                //DBG_INFO(("sample_freq = %u\r\n", ui4_sample_freq));

                ui2_tmp1 = (ui1_tmp1 & 0x3E) >> 1;
                if (ui2_tmp1 < sizeof(_aud_es_minfo_dts_bit_rate))
                {
                    ui4_bit_rate = _aud_es_minfo_dts_bit_rate[ui2_tmp1];
                    break;
                }
                //DBG_INFO(("bit rate = %u\r\n", ui4_bit_rate));
            }
        } while (0);
        
        if (i4_ret != MINFOR_OK)
        {
            break;
        }

        /*calculate duration*/
        if (ui4_bit_rate != 0 && !(pt_minfo_obj->b_filesize_non_gettable))
        {
        
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                = _mm_div64(pt_aud_es_hdlr->ui8_file_size * 8, 
                                         ui4_bit_rate,
                                         NULL);
            pt_aud_es_hdlr->t_generic.ui8_pb_duration = 
                pt_aud_es_hdlr->t_generic.ui8_pb_duration * 90000;
    
        }
        else if ((ui4_sample_freq != 0) && !(pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)ui4_sample_num, 
                                      ui4_sample_freq, 
                                      NULL); 
            DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
            pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
            pt_aud_es_hdlr->t_generic.ui4_sz_max_acces_unt  = ui4_frame_cnt;
        }
        
        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_sampling_frequency = ui4_sample_freq;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc                  = MINFO_INFO_AUD_ENC_DTS;

        pt_aud_es_hdlr->t_es_audio_stm_attr.b_big_endian = FALSE;
        DBG_INFO(("_aud_es_dts_minfo_parse:end parse ac3\r\n"));
        return MINFOR_OK;
        
    } while(0);
    if (MINFOR_OK != i4_ret)
    {
    }
    
    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name:  _aud_es_ec3_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : Min Bao(mtk40384)
 *
 * History:
 *   (1)2010-12-8 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_ec3_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT16                      ui2_tmp_code;
    UINT16                      ui2_frame_len;
    UINT32                      ui4_frame_cnt; 
    UINT8                       ui1_tmp_code;
    UINT8                       ui1_tmp;
    UINT32                      ui4_sample_freq = 0;
    UINT32                      ui4_sample_idx;
   /* UINT32                      ui4_frm_sz_code_idx;*/
    UINT8                       ui1_num_blk_scod;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("_aud_es_ec3_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }

    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                 0, 
                                 MEDIA_SEEK_BGN);
    if (i4_ret != INPUTR_OK)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    DBG_INFO(("_aud_es_ec3_minfo_parse:begin parse ac3\r\n"));
    i4_ret = MINFOR_OK;
    do
    {
        i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        /*remove garbage bits*/
        while (AUD_ES_MINFO_AC3_SYNCWORD != ui2_tmp_code)
        {
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui2_tmp_code  = ((ui2_tmp_code << 8) | ui1_tmp_code);
        }
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        ui4_frame_cnt = 0;
        
        /*check frames*/
        do
        {
            if (AUD_ES_MINFO_AC3_SYNCWORD != ui2_tmp_code)
            {
                i4_ret = MINFOR_FCT_NOT_SUPPORT;
                break;
            }
            

            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            /* I0 ----Shawn Hsiao provide*/
            ui1_tmp = (UINT8)((ui2_tmp_code & 0xF800) >> 11);
            if (ui1_tmp == 0)
            {
                ++ui4_frame_cnt;
            }

            /* fscod */
            ui2_tmp_code = ui2_tmp_code & 0x07FF;
            ui2_frame_len = (UINT16)((ui2_tmp_code + 1)*2);

            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui1_tmp = ((ui1_tmp_code & 0xc0) >> 6);
            if (ui1_tmp == 3)
            {
                ui4_sample_idx = ((ui1_tmp_code & 0x30) >> 4);
                ui1_num_blk_scod = 3;
            }
            else
            {
                ui4_sample_idx = ((ui1_tmp_code & 0xc0) >> 6);
                ui1_num_blk_scod = ((ui1_tmp_code & 0x30) >> 4);
            }
                
            /*get sampling frequency*/
            if (ui4_sample_idx >= AUD_ES_MINFO_AC3_FREQ_SAMPLING_MAX)
            {
                i4_ret = MINFOR_INTERNAL_ERR;
                break;
            }
            
            if (AUD_ES_MINFO_FRAME_REF_CNT == ui4_frame_cnt)
            {
                ui4_sample_freq = _aud_es_minfo_ac3_sampling_freq[ui4_sample_idx];
                
                DBG_INFO(("sample_freq = %u\r\n", ui4_sample_freq));
            }
            
            /*DBG_INFO(("{Pos[0x%llx], Frame[%u], Head:%2x %u}\r\n", 
                      pt_aud_es_hdlr->ui8_cur_parser_offset - (UINT64)5, 
                      ui4_frame_cnt,
                      ui1_tmp_code, 
                      ui2_frame_len));*/

            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                         (INT64)ui2_frame_len  
                                          - (INT64)AUD_ES_MINFO_AC3_FRAME_SIZE_OFFSET, 
                                         MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            /*next frame*/
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                if (INPUTR_EOF == i4_ret)
                {
                    i4_ret = MINFOR_OK;
                }
                break;
            }
            
            if (pt_minfo_obj->b_filesize_non_gettable && ui4_frame_cnt > 7)
            {
                break;
            }

            if (pt_aud_es_hdlr->b_chk_parse_timer_out)
            {
                i4_ret = MINFOR_OK;
                break;
            }
            
        } while (TRUE);
        if (i4_ret != MINFOR_OK)
        {
            break;
        }

        /*calculate duration*/
        if ((ui4_sample_freq != 0) && !(pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            switch (ui1_num_blk_scod)
            {
                case 0x00:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                case 0x01:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256* 2 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                case 0x2:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256* 3 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                case 0x3:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256 * 6 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                default:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration = 0;
                    break;
            }
            
            DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
            pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
            pt_aud_es_hdlr->t_generic.ui4_sz_max_acces_unt  = ui4_frame_cnt;
        }
        
        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_sampling_frequency = ui4_sample_freq;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc                  = MINFO_INFO_AUD_ENC_AC3;

        pt_aud_es_hdlr->t_es_audio_stm_attr.b_big_endian = FALSE;
        DBG_INFO(("_aud_es_ec3_minfo_parse:end parse ac3\r\n"));
        return MINFOR_OK;
        
    } while(0);
    
    return i4_ret;
}

/*-----------------------------------------------------------------------------
 * Name:  _aud_es_spec_ec3_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : Min Bao(mtk40384)
 *
 * History:
 *   (1)2010-12-8 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_spec_ec3_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT16                      ui2_tmp_code;
    UINT16                      ui2_frame_len;
    UINT32                      ui4_frame_cnt; 
    UINT8                       ui1_tmp_code;
    UINT8                       ui1_tmp;
    UINT32                      ui4_sample_freq = 0;
    UINT32                      ui4_sample_idx;
   /* UINT32                      ui4_frm_sz_code_idx;*/
    UINT8                       ui1_num_blk_scod;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("_aud_es_ec3_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }

    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                 0, 
                                 MEDIA_SEEK_BGN);
    if (i4_ret != INPUTR_OK)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    DBG_INFO(("_aud_es_ec3_minfo_parse:begin parse ac3\r\n"));
    i4_ret = MINFOR_OK;
    do
    {
        i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        /*remove garbage bits*/
        while (AUD_ES_MINFO_SPEC_AC3_SYNCWORD != ui2_tmp_code)
        {
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui2_tmp_code  = ((ui2_tmp_code << 8) | ui1_tmp_code);
        }
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        ui4_frame_cnt = 0;
        
        /*check frames*/
        do
        {
            if (AUD_ES_MINFO_SPEC_AC3_SYNCWORD != ui2_tmp_code)
            {
                i4_ret = MINFOR_FCT_NOT_SUPPORT;
                break;
            }
            

            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            /* I0 ----Shawn Hsiao provide*/
            ui2_tmp_code = LITTLE_TO_BIG_WORD(ui2_tmp_code);
            ui1_tmp = (UINT8)((ui2_tmp_code & 0xF800) >> 11);
            if (ui1_tmp == 0)
            {
                ++ui4_frame_cnt;
            }

            /* fscod */
            ui2_tmp_code = ui2_tmp_code & 0x07FF;
            ui2_frame_len = (UINT16)((ui2_tmp_code + 1)*2);

            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            
            ui1_tmp = ((ui1_tmp_code & 0xc0) >> 6);
            if (ui1_tmp == 3)
            {
                ui4_sample_idx = ((ui1_tmp_code & 0x30) >> 4);
                ui1_num_blk_scod = 3;
            }
            else
            {
                ui4_sample_idx = ((ui1_tmp_code & 0xc0) >> 6);
                ui1_num_blk_scod = ((ui1_tmp_code & 0x30) >> 4);
            }
                
            /*get sampling frequency*/
            if (ui4_sample_idx >= AUD_ES_MINFO_AC3_FREQ_SAMPLING_MAX)
            {
                i4_ret = MINFOR_INTERNAL_ERR;
                break;
            }
            
            if (AUD_ES_MINFO_FRAME_REF_CNT == ui4_frame_cnt)
            {
                ui4_sample_freq = _aud_es_minfo_ac3_sampling_freq[ui4_sample_idx];
                
                //DBG_INFO(("sample_freq = %u\r\n", ui4_sample_freq));
            }
            
            /*DBG_INFO(("{Pos[0x%llx], Frame[%u], Head:%2x %u}\r\n", 
                      pt_aud_es_hdlr->ui8_cur_parser_offset - (UINT64)5, 
                      ui4_frame_cnt,
                      ui1_tmp_code, 
                      ui2_frame_len));*/

            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                         (INT64)ui2_frame_len  
                                          - (INT64)AUD_ES_MINFO_SPEC_AC3_FRAME_SIZE_OFFSET, 
                                         MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            /*next frame*/
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                if (INPUTR_EOF == i4_ret)
                {
                    i4_ret = MINFOR_OK;
                }
                break;
            }
            
            if (pt_minfo_obj->b_filesize_non_gettable && ui4_frame_cnt > 7)
            {
                break;
            }

            if (pt_aud_es_hdlr->b_chk_parse_timer_out)
            {
                i4_ret = MINFOR_OK;
                break;
            }
            
        } while (TRUE);
        if (i4_ret != MINFOR_OK)
        {
            break;
        }

        /*calculate duration*/
        if ((ui4_sample_freq != 0) && !(pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            switch (ui1_num_blk_scod)
            {
                case 0x00:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                case 0x01:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256* 2 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                case 0x2:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256* 3 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                case 0x3:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(256 * 6 * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
                    break;
                default:
                    pt_aud_es_hdlr->t_generic.ui8_pb_duration = 0;
                    break;
            }
            
            DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
            pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
            pt_aud_es_hdlr->t_generic.ui4_sz_max_acces_unt  = ui4_frame_cnt;
        }
        
        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_sampling_frequency = ui4_sample_freq;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc                  = MINFO_INFO_AUD_ENC_AC3;

        pt_aud_es_hdlr->t_es_audio_stm_attr.b_big_endian = TRUE;
        DBG_INFO(("_aud_es_ec3_minfo_parse:end parse ac3\r\n"));
        return MINFOR_OK;
        
    } while(0);
    
    return i4_ret;
}


/*-----------------------------------------------------------------------------
 * Name:  _aud_es_spec_ac3_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_spec_ac3_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT16                      ui2_tmp_code;
    UINT16                      ui2_frame_len;
    UINT32                      ui4_frame_cnt; 
    UINT8                       ui1_tmp_code;
    UINT32                      ui4_sample_freq = 0;
    UINT32                      ui4_sample_idx;
    UINT32                      ui4_frm_sz_code_idx;
    UINT32            ui4_bit_rate = 0;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("_aud_es_ac3_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }

    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                 0, 
                                 MEDIA_SEEK_BGN);
    if (i4_ret != INPUTR_OK)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    DBG_INFO(("_aud_es_ac3_minfo_parse:begin parse ac3\r\n"));
    i4_ret = MINFOR_OK;
    do
    {
        i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        /*remove garbage bits*/
        while (AUD_ES_MINFO_SPEC_AC3_SYNCWORD != ui2_tmp_code)
        {
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            ui2_tmp_code  = ((ui2_tmp_code << 8) | ui1_tmp_code);
        }
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        ui4_frame_cnt = 0;
        
        /*check frames*/
        do
        {
            if (AUD_ES_MINFO_SPEC_AC3_SYNCWORD != ui2_tmp_code)
            {
                i4_ret = MINFOR_FCT_NOT_SUPPORT;
                break;
            }
            ++ui4_frame_cnt;

            /*ignore crc*/
            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, (INT64)2, MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            
            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }
            
            /*get sampling frequency*/
            ui4_sample_idx      = ((ui1_tmp_code & 0xc0) >> 6);
            ui4_frm_sz_code_idx = (ui1_tmp_code & 0x3f);
            
            if ((ui4_sample_idx >= AUD_ES_MINFO_AC3_FREQ_SAMPLING_MAX)
                || 
                (ui4_frm_sz_code_idx >= AUD_ES_MINFO_AC3_FRM_SIZE_COD_MAX))
            {
                i4_ret = MINFOR_INTERNAL_ERR;
                break;
            }
            
            if (AUD_ES_MINFO_FRAME_REF_CNT == ui4_frame_cnt)
            {
                ui4_sample_freq = _aud_es_minfo_ac3_sampling_freq[ui4_sample_idx];
                if (ui4_frm_sz_code_idx <= sizeof(_aud_es_minfo_ac3_bit_rate_table) * 2)
        {
                    ui4_bit_rate = _aud_es_minfo_ac3_bit_rate_table[ui4_frm_sz_code_idx/2];
        }
                //DBG_INFO(("sample_freq = %u, ui4_bit_rate = %u\r\n", ui4_sample_freq, ui4_bit_rate));
            }
            
            ui2_frame_len = (UINT16)((_aud_es_minfo_ac3_frame_size_table
                                    [ui4_frm_sz_code_idx][ui4_sample_idx]) * 2);
            /*DBG_INFO(("{Pos[0x%llx], Frame[%u], Head:%2x %u}\r\n", 
                      pt_aud_es_hdlr->ui8_cur_parser_offset - (UINT64)5, 
                      ui4_frame_cnt,
                      ui1_tmp_code, 
                      ui2_frame_len));*/

            i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                         (INT64)ui2_frame_len  
                                          - (INT64)AUD_ES_MINFO_SPEC_AC3_FRAME_SIZE_OFFSET, 
                                         MEDIA_SEEK_CUR);
            if (i4_ret != INPUTR_OK)
            {
                break;
            }

            /*next frame*/
            i4_ret = _aud_es_minfo_input2byte(pt_minfo_obj, &ui2_tmp_code);
            if (i4_ret != INPUTR_OK)
            {
                if (INPUTR_EOF == i4_ret)
                {
                    i4_ret = MINFOR_OK;
                }
                break;
            }
            
            if (pt_minfo_obj->b_filesize_non_gettable && ui4_frame_cnt > 7)
            {
                break;
            }

            if (pt_aud_es_hdlr->b_chk_parse_timer_out)
            {
                i4_ret = MINFOR_OK;
                break;
            }
            
        } while (TRUE);
        if (i4_ret != MINFOR_OK)
        {
            break;
        }

        /*calculate duration*/
        if ((ui4_sample_freq != 0) && !(pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                          = _mm_div64((UINT64)(AUD_ES_MINFO_AC3_FREQ_SAMPLE_CNT * ui4_frame_cnt), 
                                      ui4_sample_freq, 
                                      NULL); 
            DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
            pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
            pt_aud_es_hdlr->t_generic.ui4_sz_max_acces_unt  = ui4_frame_cnt;
        }
        else if((ui4_bit_rate != 0) && (pt_aud_es_hdlr->b_too_big_file) && !(pt_minfo_obj->b_filesize_non_gettable)
            && !(pt_aud_es_hdlr->b_chk_parse_timer_out))
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration
                             = _mm_div64(pt_aud_es_hdlr->ui8_file_size * 8, 
                                         ui4_bit_rate, 
                                         NULL); 
                DBG_INFO(("duration = %llu\r\n", pt_aud_es_hdlr->t_generic.ui8_pb_duration));
                 pt_aud_es_hdlr->t_generic.ui8_pb_duration      *= 90000;
        }
        
        pt_aud_es_hdlr->t_es_audio_stm_attr.ui4_sampling_frequency = ui4_sample_freq;
        pt_aud_es_hdlr->t_es_audio_stm_attr.e_enc                  = MINFO_INFO_AUD_ENC_AC3;

        pt_aud_es_hdlr->t_es_audio_stm_attr.b_big_endian = TRUE;
        DBG_INFO(("_aud_es_ac3_minfo_parse:end parse ac3\r\n"));
        return MINFOR_OK;
        
    } while(0);
    if (MINFOR_OK != i4_ret)
    {
    }
    
    return i4_ret;
}

static INT32 _aud_es_acc_parse_id3(MINFO_OBJ_T* pt_minfo_obj, MINFO_AUD_ES_HANDLER_T*pt_aud_es_hdlr)
{
    INT32               i4_ret  = MINFOR_OK;
    UINT32              ui4_tag = 0;
    UINT32              ui4_id3tag_size = 0;
    UINT32              ui4_frameID = 0;
    UINT32              ui4_framesize = 0;
    UINT32              ui4_idx = 0;
    UINT32              ui4_read_count = 0;
    UINT8*              pui1_cache_buff;
    UINT8               pui1_tag_header[ID3_TAG_HEADER_LENGTH];
    copybytes_fct       pf_copybytes;

    if (pt_minfo_obj == NULL || pt_aud_es_hdlr== NULL)
    {
        return MINFOR_INV_ARG;
    }
#ifndef __KERNEL__
    DBG_LOG_PRINT(("[debug] _aud_es_acc_parse_id3\r\n"));
#endif
    pf_copybytes = pt_minfo_obj->t_input_fct_tbl.pf_copybytes;
    
    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, (INT64)0, MEDIA_SEEK_BGN);
    if (i4_ret != INPUTR_OK)
    {
        return i4_ret;
    }

    /***************parse Id3 header**************************/
    x_memset(pui1_tag_header, 0, sizeof(pui1_tag_header));
    
    /*copy 10 bytes*/
    i4_ret = pf_copybytes(pt_minfo_obj, 
                              pui1_tag_header, 
                              10, 
                              10, 
                              &ui4_read_count);
    if ((i4_ret != INPUTR_OK) && (i4_ret != INPUTR_EOF))
    {
        return i4_ret;
    }
    
    /*Id3 tag*/
    ui4_tag = pui1_tag_header[0];
    ui4_tag <<= 8;
    ui4_tag += pui1_tag_header[1];
    ui4_tag <<= 8;
    ui4_tag += pui1_tag_header[2];
    
    DBG_INFO(("[%s] ID3tag: 0x%x\r\n", __FUNCTION__, ui4_tag));
    if (cui4_ID3 != ui4_tag)
    {
        return MINFOR_FMT_NOT_SUPPORT;
    }

    /*ID3 tag size*/
   
    ui4_id3tag_size = (pui1_tag_header[6] & 0x7F) * 0x200000 + (pui1_tag_header[7] & 0x7F) * 0x4000
                       + (pui1_tag_header[8] & 0x7F) * 0x80 + (pui1_tag_header[9] & 0x7F); 

    DBG_INFO(("[%s] ui4_id3tag_size: 0x%x\r\n", __FUNCTION__, ui4_id3tag_size));

    /***********************parse Frames************************/
    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, (INT64)10, MEDIA_SEEK_BGN);
    if (i4_ret != INPUTR_OK)
    {
        return i4_ret;
    }
    
    pui1_cache_buff = _mm_util_mem_alloc(sizeof(UINT8) * ui4_id3tag_size);
    if (NULL == pui1_cache_buff)
    {
        DBG_ERROR(("_aud_es_acc_parse_id3:pui1_cache_buff allocate mem(%d) failed\r\n", ui4_id3tag_size));
        return MINFOR_OUT_OF_MEMORY;
    }
    x_memset(pui1_cache_buff, 0, sizeof(UINT8) * ui4_id3tag_size);

    /*copy frames to buffer*/
    i4_ret = pf_copybytes(pt_minfo_obj, 
                              pui1_cache_buff, 
                              ui4_id3tag_size, 
                              ui4_id3tag_size, 
                              &ui4_read_count);
    if ((i4_ret != INPUTR_OK) && (i4_ret != INPUTR_EOF))
    {
        if (pui1_cache_buff != NULL)
        {
            _mm_util_mem_free(pui1_cache_buff);
            pui1_cache_buff = NULL;
        }
        return i4_ret;
    }
    
    while (ui4_idx  < ui4_id3tag_size)
    {
        /*frame id 4 byte*/
        ui4_frameID = pui1_cache_buff[ui4_idx ++];
        ui4_frameID <<= 8;
        ui4_frameID += pui1_cache_buff[ui4_idx ++];
        ui4_frameID <<= 8;
        ui4_frameID += pui1_cache_buff[ui4_idx ++];
        ui4_frameID <<= 8;
        ui4_frameID += pui1_cache_buff[ui4_idx ++];    

        DBG_INFO(("[%s] ui4_frameID: 0x%x\r\n", __FUNCTION__, ui4_frameID));

        /*frame size 4 byte*/
        ui4_framesize = pui1_cache_buff[ui4_idx ] * 0x100000000 +
                        pui1_cache_buff[ui4_idx + 1] * 0x10000 +
                        pui1_cache_buff[ui4_idx + 2] * 0x100 + 
                        pui1_cache_buff[ui4_idx + 3]; 

        DBG_INFO(("[%s] ui4_framesize %0x\r\n", __FUNCTION__, ui4_framesize));
        
        /*skip flag 3 byte + frame size 4 byte*/
        ui4_idx  += 7;

        if (ui4_framesize == 0)
        {
            continue;
        }
        
        if (ui4_frameID == cui4_TPE1)
        {
            pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist= (UTF16_T*)_mm_util_mem_alloc(sizeof(UTF16_T) * ui4_framesize);
            if (NULL == pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist)
            {
                DBG_ERROR(("aud_es_minfo_mopen:pwc_str_artist allocate memory failed\r\n"));
                i4_ret = MINFOR_OUT_OF_MEMORY;
                break;
            }
            x_memset(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist, 
                     0, 
                     sizeof(UTF16_T) * ui4_framesize);
            
            x_uc_ps_to_w2s((CHAR*)(pui1_cache_buff + ui4_idx ), pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist, (ui4_framesize-1));
            pt_aud_es_hdlr->t_es_acc_id3_attr.z_artist_len = ui4_framesize-1;
          
        }
        else if(ui4_frameID == cui4_TALB)
        {
            pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album = (UTF16_T*)_mm_util_mem_alloc(sizeof(UTF16_T) * ui4_framesize);
            if (NULL == pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album)
            {
                DBG_ERROR(("aud_es_minfo_mopen:pwc_str_album allocate memory failed\r\n"));
                i4_ret = MINFOR_OUT_OF_MEMORY;
                break;
            }
            x_memset(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album, 
                     0, 
                     sizeof(UTF16_T) * ui4_framesize);
            
            x_uc_ps_to_w2s((CHAR*)(pui1_cache_buff + ui4_idx), pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album, (ui4_framesize-1));
            pt_aud_es_hdlr->t_es_acc_id3_attr.z_album_len = ui4_framesize-1;
            
        }
        else if(ui4_frameID == cui4_TYER)
        {
            pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year= (UTF16_T*)_mm_util_mem_alloc(sizeof(UTF16_T) * ui4_framesize);
            if (NULL == pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year)
            {
                DBG_ERROR(("aud_es_minfo_mopen:pwc_str_year allocate memory failed\r\n"));
                i4_ret = MINFOR_OUT_OF_MEMORY;
                break;
            }
            x_memset(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year, 
                     0, 
                     sizeof(UTF16_T) * ui4_framesize);
          
            x_uc_ps_to_w2s((CHAR*)(pui1_cache_buff + ui4_idx), pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year, (ui4_framesize-1));
            pt_aud_es_hdlr->t_es_acc_id3_attr.z_album_len = ui4_framesize-1;

        }
        else if(ui4_frameID == cui4_TIT2)
        {
            pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title= (UTF16_T*)_mm_util_mem_alloc(sizeof(UTF16_T) * ui4_framesize);
            if (NULL == pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title)
            {
                DBG_ERROR(("aud_es_minfo_mopen:pwc_str_title allocate memory failed\r\n"));
                i4_ret = MINFOR_OUT_OF_MEMORY;
                break;
            }
            x_memset(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title, 
                     0, 
                     sizeof(UTF16_T) * ui4_framesize);
            
            x_uc_ps_to_w2s((CHAR*)(pui1_cache_buff + ui4_idx), pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title, (ui4_framesize-1));
            pt_aud_es_hdlr->t_es_acc_id3_attr.z_title_len = ui4_framesize-1;
            
        }
        else if(ui4_frameID == cui4_TCON)
        {
            pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre= (UTF16_T*)_mm_util_mem_alloc(sizeof(UTF16_T) * ui4_framesize);
            if (NULL == pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre)
            {
                DBG_ERROR(("aud_es_minfo_mopen:pwc_str_genre allocate memory failed\r\n"));
                i4_ret = MINFOR_OUT_OF_MEMORY;
                break;
            }
            x_memset(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre, 
                     0, 
                     sizeof(UTF16_T) * ui4_framesize);
            
           x_uc_ps_to_w2s((CHAR*)(pui1_cache_buff + ui4_idx), pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre, (ui4_framesize-1));
           pt_aud_es_hdlr->t_es_acc_id3_attr.z_genre_len = ui4_framesize-1;
        }
        else
        {
            DBG_INFO(("[debug] skip!!!\r\n"));
        }        

        ui4_idx += ui4_framesize-1;
    }

    if (pui1_cache_buff != NULL)
    {
        _mm_util_mem_free(pui1_cache_buff);
        pui1_cache_buff = NULL;
    }

    return i4_ret;     
}
/*-----------------------------------------------------------------------------
 * Name:  aud_es_aac_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                       has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
static INT32 _aud_es_aac_minfo_parse(VOID* pv_obj)
{
    INT32                       i4_ret         = MINFOR_OK;
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;
    UINT32                      ui4_head_code;
    UINT8                       ui1_temp;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("_aud_es_aac_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (!pt_aud_es_hdlr)
    {
        return MINFOR_INTERNAL_ERR;
    }
    
    i4_ret         = MINFOR_OK;

    /*parse ID3*/
    i4_ret = _aud_es_acc_parse_id3(pt_minfo_obj, pt_aud_es_hdlr);
    if (i4_ret != MINFOR_OK)
    {
        DBG_INFO(("ACC file ID3tag parse faild or not support ID3tag\r\n"));
    }
    
    do
    {
        i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, (INT64)0, MEDIA_SEEK_BGN);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }

        i4_ret = _aud_es_minfo_input4byte(pt_minfo_obj, &ui4_head_code);
        if (i4_ret != INPUTR_OK)
        {
            break;
        }
        
        do 
        {
            /*Check if AAC ADIF: audio data interchaneg format*/
            if (AUD_ES_MINFO_ADIF_ID == ui4_head_code)
            {
                i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, (INT64)-4, MEDIA_SEEK_CUR);
                if (i4_ret != INPUTR_OK)
                {
                    break;
                }
                i4_ret = _aud_es_adif_minfo_parse(pt_minfo_obj);
                break;
            }
            else if (AUD_ES_MINFO_AAC_SYNCWORD == (UINT16)((ui4_head_code & 0xfff00000) >> 16))
            {
                i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, (INT64)-4, MEDIA_SEEK_CUR);
                if (i4_ret != INPUTR_OK)
                {
                    break;
                }
                i4_ret = _aud_es_adts_minfo_parse(pt_minfo_obj);
                break;
            }
            else
            {
                i4_ret = _aud_es_minfo_input1byte(pt_minfo_obj, &ui1_temp);
                if (i4_ret != INPUTR_OK)
                {
                    break;
                }

                ui4_head_code = ((ui4_head_code << 8) | (UINT32)ui1_temp);
            }
        } while (TRUE);
        if (i4_ret != MINFOR_OK)
        {
            break;
        }
        
        return MINFOR_OK;
        
    } while (0);
    
    return i4_ret;

}

static VOID _aud_es_chk_parse_timeout_nfy(
                    HANDLE_T                    h_timer,
                    VOID*                       pv_tag
                    )
{
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr  = NULL;
    if (pv_tag == NULL)
    {
        DBG_ERROR(("[AUD_ES_MINFO] %s, pv_tag == NULL\n", __func__));
        return;
    }
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pv_tag;
    
    pt_aud_es_hdlr->b_chk_parse_timer_out = TRUE;
    DBG_INFO(("[AUD_ES_MINFO] _aud_es_chk_parse_timeout_nfy time out\n"));
    return;
}


/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_is_handler
 *
 * Description: This function confirm media format could be handled or not.
 *
 * Inputs:  ps_path_name specify the path name of the file
 *          pv_obj       specify the media info object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation 
 *                       has been completed.
 *          Other non-zero value :unsuccessful
 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_is_handler(VOID* pv_obj, MEDIA_FORMAT_T* pt_format)
{

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_obj) || (NULL == pt_format))
    {
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/    
    if ((MEDIA_TYPE_AUDIO_ES == pt_format->e_media_type) 
        && ((MEDIA_AUD_SUBTYPE_AAC == pt_format->t_media_subtype.u.e_aud_subtype)
            || (MEDIA_AUD_SUBTYPE_AMR == pt_format->t_media_subtype.u.e_aud_subtype)
            || (MEDIA_AUD_SUBTYPE_AC3 == pt_format->t_media_subtype.u.e_aud_subtype)))
    {
       return MINFOR_OK;
    }
    else
    {
       return MINFOR_FMT_NOT_SUPPORT;
    }
}

/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_fopen
 *
 * Description: This function open a handler for a file
 *
 * Inputs:  ps_path_name  specify the path name of the file
 *          pv_obj        specify the media info object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MINFOR_OK    Success. The routine was successful and 
                         the operation has been completed.
           other non-zero value :unsuccessful

 * Author : lianming lin(mcnmtk40234)
 *
 * History:
    (1)2009-7-29 : initial

 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_fopen(CHAR*  ps_path_name,  VOID* pv_obj)
{
    MINFO_OBJ_T*              pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*   pt_aud_es_hdlr = NULL;
    INT32                     i4_ret = 0;

    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_obj == NULL) || (ps_path_name == NULL))
    {
        DBG_ERROR(("aud_es_minfo_fopen: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    DBG_INFO(("aud_es_minfo_fopen begin!\n"));
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
    if (pt_minfo_obj->pv_handler_obj)
    {
        _mm_util_mem_free(pt_minfo_obj->pv_handler_obj);
        pt_minfo_obj->pv_handler_obj = NULL;
    }

    pt_aud_es_hdlr = _mm_util_mem_alloc(sizeof(MINFO_AUD_ES_HANDLER_T));
    if (NULL == pt_aud_es_hdlr)
    {
        DBG_ERROR(("aud_es_minfo_fopen:pv_handler_obj allocate mem(%d) failed\r\n",
                   sizeof(MINFO_AUD_ES_HANDLER_T)));
        return MINFOR_OUT_OF_MEMORY;
    }
    
    x_memset(pt_aud_es_hdlr, 0, sizeof(MINFO_AUD_ES_HANDLER_T));
    
    pt_minfo_obj->pv_handler_obj = pt_aud_es_hdlr;
    x_memset(pt_minfo_obj->pv_handler_obj, 0, sizeof(MINFO_AUD_ES_HANDLER_T));

    /*temp buffer used to store parse data , should be verified later. */
    pt_aud_es_hdlr->pui1_cache_buf = (UINT8*)
                  _mm_util_mem_alloc(sizeof(UINT8) * AUD_ES_MINFO_INIT_BUFF_LEN);
    if (NULL == pt_aud_es_hdlr->pui1_cache_buf)
    {
        DBG_ERROR(("aud_es_minfo_fopen:pui1_header_buf allocate memory failed\r\n"));
        _mm_util_mem_free(pt_minfo_obj->pv_handler_obj);
        pt_minfo_obj->pv_handler_obj = NULL;
        return MINFOR_OUT_OF_MEMORY;
    }
    x_memset(pt_aud_es_hdlr->pui1_cache_buf, 
             0, 
             sizeof(UINT8) * AUD_ES_MINFO_INIT_BUFF_LEN);
    
    pt_aud_es_hdlr->ui4_buf_length    = sizeof(UINT8) * AUD_ES_MINFO_INIT_BUFF_LEN;
    pt_aud_es_hdlr->ui8_file_size     = pt_minfo_obj->z_buff_leng;
    
    if (pt_minfo_obj->pt_media_format->b_is_network)
    {
        i4_ret = x_timer_create(&pt_aud_es_hdlr->h_chk_parse_timer);
        if (i4_ret != OSR_OK) {
            DBG_ERROR(("[AUD_ES_MINFO]:Create h_chk_parse_timer failed! ret=%d\r\n", i4_ret)); 
            return MINFOR_INITIAL_ERR;
        }
        pt_aud_es_hdlr->ui8_lookup_length = AUD_ES_MINFO_FS_SIZE_THRESHOLD * 2;
        DBG_INFO(("[fopen] network case, set timer=%d(ms), ui8_lookup_length=%llu\n", 
                AUD_ES_CHECK_PARSE_READY, pt_aud_es_hdlr->ui8_lookup_length));
    }
    else
    {
        DBG_INFO(("file size is %llu\n", pt_aud_es_hdlr->ui8_file_size));
        pt_aud_es_hdlr->ui8_lookup_length = AUD_ES_MINFO_FS_SIZE_THRESHOLD * 2;
    }
    
    return MINFOR_OK;
}

/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_mopen
 *
 * Description: This function open a handler for a memory buffer
 *
 * Inputs:  pv_mem_data  specify the address of the memory buffer
 *          z_mem_leng   specify the length of the buffer
 *          pv_obj       specify the media info object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation 
 *                       has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-7-29 : initial

 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_mopen(VOID*    pv_mem_data, 
                         SIZE_T   z_mem_leng, 
                         VOID*    pv_obj)
{
    MINFO_OBJ_T*              pt_minfo_obj   = NULL;
    INT32                     i4_ret;
    UINT64                    ui8_clip_pos;
    MINFO_AUD_ES_HANDLER_T*   pt_aud_es_hdlr = NULL;
    UINT64                    ui8_size = 0;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_obj == NULL) || (pv_mem_data == NULL))
    {
        DBG_ERROR(("aud_es_minfo_mopen:NULL\r\n"));
        return MINFOR_INV_ARG;
    }

    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;

    /*set input functions again for asf litter endian*/
    if (z_mem_leng != 0)
    {
        pt_minfo_obj->t_input_fct_tbl.pf_copybytes = mm_util_mcopybytes_fct;
        pt_minfo_obj->t_input_fct_tbl.pf_input1    = mm_util_minput1bytes_fct;
        pt_minfo_obj->t_input_fct_tbl.pf_input2    = mm_util_minput2bytes_b_fct;
        pt_minfo_obj->t_input_fct_tbl.pf_input3    = mm_util_minput3bytes_b_fct;
        pt_minfo_obj->t_input_fct_tbl.pf_input4    = mm_util_minput4bytes_b_fct;
        pt_minfo_obj->t_input_fct_tbl.pf_set_pos   = mm_util_msetpos_fct;
        pt_minfo_obj->t_input_fct_tbl.pf_get_pos   = mm_util_mgetpos_fct;
    }
    else
    {
        MEDIA_FEEDER_T* pt_src_feeder; 
        pt_src_feeder = (MEDIA_FEEDER_T*)pv_mem_data;
        pt_minfo_obj->h_src_feeder = pt_src_feeder->h_feeder;
        pt_minfo_obj->t_input_fct_tbl.pf_copybytes = 
               pt_src_feeder->t_feeder_le_fct_tbl.pf_copybytes;
        pt_minfo_obj->t_input_fct_tbl.pf_input1    = 
               pt_src_feeder->t_feeder_le_fct_tbl.pf_input1;
        pt_minfo_obj->t_input_fct_tbl.pf_input2    = 
               pt_src_feeder->t_feeder_le_fct_tbl.pf_input2;
        pt_minfo_obj->t_input_fct_tbl.pf_input3    = 
               pt_src_feeder->t_feeder_le_fct_tbl.pf_input3;
        pt_minfo_obj->t_input_fct_tbl.pf_input4    = 
               pt_src_feeder->t_feeder_le_fct_tbl.pf_input4;
        pt_minfo_obj->t_input_fct_tbl.pf_set_pos   = 
               pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos;
        pt_minfo_obj->t_input_fct_tbl.pf_get_pos   = 
               pt_src_feeder->t_feeder_le_fct_tbl.pf_get_pos;

        /*To get clip size*/
        i4_ret = pt_minfo_obj->t_input_fct_tbl.pf_set_pos(pt_src_feeder, 
                                                          0, 
                                                          MEDIA_SEEK_END);
        if (MINFOR_OK != i4_ret)
        {
            DBG_ERROR(("aud_es_minfo_mopen: set error\r\n"));
            return MINFOR_INITIAL_ERR;
        }
        /*get the end position*/
        pt_minfo_obj->t_input_fct_tbl.pf_get_pos(pt_src_feeder, &ui8_clip_pos);
        pt_minfo_obj->z_buff_leng = (SIZE_T)ui8_clip_pos;
        
    }

    /*allocate asf handler memory*/
    if (pt_minfo_obj->pv_handler_obj)
    {
        _mm_util_mem_free(pt_minfo_obj->pv_handler_obj);
        pt_minfo_obj->pv_handler_obj = NULL;
    }

    pt_aud_es_hdlr = _mm_util_mem_alloc(sizeof(MINFO_AUD_ES_HANDLER_T));
    if (NULL == pt_aud_es_hdlr)
    {
        DBG_ERROR(("aud_es_minfo_mopen:pv_handler_obj allo memory(%d) failed\r\n",
                   sizeof(MINFO_AUD_ES_HANDLER_T)));
        return MINFOR_OUT_OF_MEMORY;
    }
    pt_minfo_obj->pv_handler_obj = pt_aud_es_hdlr;
    x_memset(pt_minfo_obj->pv_handler_obj, 0, sizeof(MINFO_AUD_ES_HANDLER_T));

    /*temp buffer used to store parse data , should be verified later. */
    pt_aud_es_hdlr->pui1_cache_buf = (UINT8*)
                  _mm_util_mem_alloc(sizeof(UINT8) * AUD_ES_MINFO_INIT_BUFF_LEN);
    if (NULL == pt_aud_es_hdlr->pui1_cache_buf)
    {
        DBG_ERROR(("aud_es_minfo_mopen:pui1_header_buf allocate memory failed\r\n"));
        _mm_util_mem_free(pt_minfo_obj->pv_handler_obj);
        pt_minfo_obj->pv_handler_obj = NULL;
        return MINFOR_OUT_OF_MEMORY;
    }
    x_memset(pt_aud_es_hdlr->pui1_cache_buf, 
             0, 
             sizeof(UINT8) * AUD_ES_MINFO_INIT_BUFF_LEN);
    
    pt_aud_es_hdlr->ui4_buf_length    = sizeof(UINT8) * AUD_ES_MINFO_INIT_BUFF_LEN;
    pt_aud_es_hdlr->ui8_file_size     = pt_minfo_obj->z_buff_leng;
    pt_aud_es_hdlr->ui8_lookup_length = AUD_ES_MINFO_FS_SIZE_THRESHOLD * 2;
    
    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                    (INT64)0,
                                    MEDIA_SEEK_END);

    i4_ret = _aud_es_minfo_getpos(pt_minfo_obj, &ui8_size);
#ifndef __KERNEL__
    DBG_LOG_PRINT(("\n--------------ui8_size = %llu--------------------\n", ui8_size));
#endif
    if (ui8_size == (UINT64)(-1) || ui8_size == (UINT32)(-1))
    {
        DBG_INFO(("aud es get file size is -1 !\r\n"));
        pt_minfo_obj->b_filesize_non_gettable = TRUE;
    }

    i4_ret = _aud_es_minfo_setpos(pt_minfo_obj, 
                                 0, 
                                 MEDIA_SEEK_BGN);
    if (i4_ret != INPUTR_OK)
    {
        DBG_ERROR(("aud es set pos to 0 fail!!\r\n"));
    }
    
    return MINFOR_OK;
}    

/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_close
 *
 * Description: This function close a handler
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: NULL
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation 
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29: initial
 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_close(VOID* pv_obj)
{
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;  

   
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)  
    {
        DBG_ERROR(("aud_es_minfo_close:NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (NULL == pt_aud_es_hdlr)
    {
        DBG_ERROR(("aud_es_minfo_close:pt_aud_es_hdlr is NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    
    /*free temp buffer*/
    if (pt_aud_es_hdlr->pui1_cache_buf != NULL)
    {
        _mm_util_mem_free(pt_aud_es_hdlr->pui1_cache_buf);
        pt_aud_es_hdlr->pui1_cache_buf = NULL;
    }

    if (pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album != NULL)
    {
        _mm_util_mem_free(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album);
        pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album = NULL;
    }

    if (pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist != NULL)
    {
        _mm_util_mem_free(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist);
        pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist = NULL;
    }

    if (pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year != NULL)
    {
        _mm_util_mem_free(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year);
        pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year = NULL;
    }

    if (pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title!= NULL)
    {
        _mm_util_mem_free(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title);
        pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title = NULL;
    }

    if (pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre!= NULL)
    {
        _mm_util_mem_free(pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre);
        pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre = NULL;
    }

    if (x_handle_valid(pt_aud_es_hdlr->h_chk_parse_timer))
    {
        x_timer_stop(pt_aud_es_hdlr->h_chk_parse_timer);
        x_timer_delete(pt_aud_es_hdlr->h_chk_parse_timer);
        pt_aud_es_hdlr->h_chk_parse_timer = NULL_HANDLE;
    }
  
    /*free object handler*/
    if (pt_aud_es_hdlr)
    {
        _mm_util_mem_free(pt_aud_es_hdlr);
        pt_aud_es_hdlr = NULL;
    }
    
    return MINFOR_OK;
}



/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_parse
 *
 * Description: This function start parsing a audio es file
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_parse(VOID* pv_obj)
{
    INT32                 i4_ret       = MINFOR_OK;
    MINFO_OBJ_T*          pt_minfo_obj = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (pv_obj == NULL)
    {
        DBG_ERROR(("aud_es_minfo_parse: NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*)pv_obj;
    if (pt_minfo_obj->pt_media_format->e_media_type != MEDIA_TYPE_AUDIO_ES)
    {
        DBG_ERROR(("aud_es_minfo_parse: not audio es!\r\n"));
        return MINFOR_INV_ARG;
    }

    if (pt_minfo_obj->pv_handler_obj == NULL)
    {
        DBG_ERROR(("pt_minfo_obj->pv_handler_obj is NULL!\r\n"));
        return MINFOR_INV_ARG;
    }
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    if (x_handle_valid(pt_aud_es_hdlr->h_chk_parse_timer))
    {
        pt_aud_es_hdlr->b_chk_parse_timer_out = FALSE;
        i4_ret = x_timer_start(pt_aud_es_hdlr->h_chk_parse_timer,
                                       AUD_ES_CHECK_PARSE_READY,
                                       X_TIMER_FLAG_ONCE,
                                       _aud_es_chk_parse_timeout_nfy,
                                       (VOID*)pt_aud_es_hdlr);
        if ( i4_ret != OSR_OK)
        {
            DBG_ERROR(("[AUD_ES_MINFO]:Start h_chk_parse_timer failed! ret=%d\r\n", i4_ret));
            return MINFOR_INTERNAL_ERR;
        }
    }
    
    if (pt_minfo_obj->pt_media_format->t_media_subtype.u.e_aud_subtype 
        == MEDIA_AUD_SUBTYPE_AAC)
    {
        i4_ret = _aud_es_aac_minfo_parse(pv_obj);
    }
    else if (pt_minfo_obj->pt_media_format->t_media_subtype.u.e_aud_subtype 
             == MEDIA_AUD_SUBTYPE_AMR)
    {
        i4_ret = _aud_es_amr_minfo_parse(pv_obj);
    }
    else if (pt_minfo_obj->pt_media_format->t_media_subtype.u.e_aud_subtype 
             == MEDIA_AUD_SUBTYPE_AC3)
    {
        if (pt_minfo_obj->pt_media_format->ui8_format_magic == FMT_MAGIC_NUM_AC3)
        {
            i4_ret = _aud_es_ac3_minfo_parse(pv_obj);
        }
        else if (pt_minfo_obj->pt_media_format->ui8_format_magic == FMT_MAGIC_NUM_SPEC_AC3)
        {
            i4_ret = _aud_es_spec_ac3_minfo_parse(pv_obj);
        }
        else if (pt_minfo_obj->pt_media_format->ui8_format_magic == FMT_MAGIC_NUM_EC3)
        {
            i4_ret = _aud_es_ec3_minfo_parse(pv_obj);
        }
        else if (pt_minfo_obj->pt_media_format->ui8_format_magic == FMT_MAGIC_NUM_SPEC_EC3)
        {
            i4_ret = _aud_es_spec_ec3_minfo_parse(pv_obj);
        }
        else
        {
            i4_ret = MINFOR_INV_ARG;
        }
    }
    else if (pt_minfo_obj->pt_media_format->t_media_subtype.u.e_aud_subtype 
             == MEDIA_AUD_SUBTYPE_DTS)
    {
        if (pt_minfo_obj->pt_media_format->ui8_format_magic == FMT_MAGIC_NUM_DTS)
        {
            i4_ret = _aud_es_dts_minfo_parse(pv_obj);
        }
        else if (pt_minfo_obj->pt_media_format->ui8_format_magic == FMT_MAGIC_NUM_SPEC_DTS)
        {
            i4_ret = _aud_es_dts_ex_minfo_parse(pv_obj);
        }
        else
        {
            i4_ret = MINFOR_INV_ARG;
        }
    }

    if (x_handle_valid(pt_aud_es_hdlr->h_chk_parse_timer))
    {
        x_timer_stop(pt_aud_es_hdlr->h_chk_parse_timer);
    }
    
    return i4_ret;

} 

/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_get_num_recs
 *
 * Description:This function get information from a handler
 *
 * Inputs:  pv_obj       specify the media info object  
 *          e_info_type  
 *
 * Outputs: pui2_num_recs
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation 
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_get_num_recs(VOID*                pv_obj, 
                                MINFO_INFO_TYPE_T    e_info_type, 
                                UINT16*              pui2_num_recs)
{
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;  
    INT32                       i4_ret;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_obj ) || (NULL == pui2_num_recs ))
    {
        DBG_ERROR(("aud_es_minfo_get_num_recs:NULL\r\n"));
        return MINFOR_INV_ARG;
    }

    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj   = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)pt_minfo_obj->pv_handler_obj;
    
    if (pt_aud_es_hdlr)
    {
        /*whether parser is done? if not can not return info*/
        if (FALSE == pt_minfo_obj->b_parsing_successful)
        {
            DBG_ERROR(("aud_es_minfo_get_num_recs:minfo is not ready\r\n"));
            return MINFOR_INFO_NOT_READY;    
        }
        *pui2_num_recs = 0;
        i4_ret         = MINFOR_OK;
        switch (e_info_type)
        {
             case MINFO_INFO_TYPE_GENERIC:
                 break;                
             case MINFO_INFO_TYPE_VIDEO_ATTR:
                 break;
             case MINFO_INFO_TYPE_AUDIO_ATTR:
                *pui2_num_recs = 1;
                break;
             case MINFO_INFO_TYPE_SP_ATTR:
                break;
             case MINFO_INFO_TYPE_IDX_INFO:
                break;
             case MINFO_INFO_TYPE_META_DATA:
                break;
             default:
                ;                
        }
        return i4_ret;
    }
    else
    {
        return MINFOR_FMT_NOT_SUPPORT;
    }
}
                        

/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_get_info
 *
 * Description:This function get information from a handler
 *
 * Inputs:  pv_obj    specify the media info object  
 *           e_info_type
 *          ui2_index
 * Outputs: pv_buf   output media stream infomation
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation 
 *                        has been completed.
 *          other non-zero value :unsuccessful
 * Author : lianming lin(mtk40234)
 *
 * History:
 *   (1)2009-07-29 : initial
 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_get_info(VOID*               pv_obj, 
                            MINFO_INFO_TYPE_T   e_info_type, 
                            MINFO_REC_IDX_T     ui2_index,                    
                            VOID*               pv_buf, 
                            SIZE_T*             pz_buf_leng)
{
    MINFO_OBJ_T*                pt_minfo_obj   = NULL;
    MINFO_INFO_T*               pt_minfo_rec   = NULL;
    MINFO_AUD_ES_HANDLER_T*     pt_aud_es_hdlr = NULL;  
    MINFO_TYPE_GENERIC_T*       pt_generic     = NULL;
    MINFO_TYPE_STM_ATRBT_T*     pt_strm_atrbt  = NULL;
    INT32                       i4_ret;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_obj) || (NULL == pz_buf_leng))
    {
        DBG_ERROR(("aud_es_minfo_get_info:NULL\r\n"));
        return MINFOR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_minfo_obj = (MINFO_OBJ_T*) pv_obj;
    pt_aud_es_hdlr = (MINFO_AUD_ES_HANDLER_T*)(pt_minfo_obj->pv_handler_obj);
    if (NULL == pt_aud_es_hdlr)
    {
        DBG_ERROR(("aud_es_minfo_get_info:pt_asf_handler NULL\r\n"));
        return MINFOR_INTERNAL_ERR;
    }
    
    if (NULL == pv_buf)
    {   
        /*get length only*/
        *pz_buf_leng = sizeof (MINFO_INFO_T);
        return MINFOR_OK;
    }
    else
    {
        /* get data */
        if (*pz_buf_leng < sizeof (MINFO_INFO_T))
        {
            DBG_ERROR(("ERROR: aud_es_minfo_get_info,insufficient buffer!\r\n"));  
            return MINFOR_INSUFFICIENT_BUFFER;
        }
    }    
    
    pt_minfo_rec = (MINFO_INFO_T*)pv_buf;
    //x_memset(pt_minfo_rec, 0, sizeof(MINFO_INFO_T));
    
    if (NULL != pt_aud_es_hdlr)
    {
        if (FALSE == pt_minfo_obj->b_parsing_successful)
        {
            DBG_ERROR(("aud_es_minfo_get_info:minfo is not ready\r\n"));
            return MINFOR_INFO_NOT_READY;    
        }
        //x_memset(pt_minfo_rec, 0, sizeof(MINFO_INFO_T));
        pt_minfo_rec->t_format = *pt_minfo_obj->pt_media_format;
        
        pt_minfo_rec->e_info_type = e_info_type;
        pt_minfo_rec->ui1_rec_idx = (UINT8)ui2_index;

        i4_ret = MINFOR_OK;
        switch (e_info_type)
        {
            case MINFO_INFO_TYPE_GENERIC:
                pt_generic = &(pt_minfo_rec->u.t_general_info);
                x_memcpy(pt_generic, 
                        (VOID*)&(pt_aud_es_hdlr->t_generic),
                         sizeof(MINFO_TYPE_GENERIC_T));
                pt_generic->ui8_media_sz = pt_aud_es_hdlr->ui8_file_size;
                pt_generic->ui1_strm_ns  = 1;
                break;
                

            case MINFO_INFO_TYPE_AUDIO_ATTR:
                if(ui2_index > 1)
                {
                   return MINFOR_OUT_OF_RECORD;
                }
                pt_strm_atrbt = &(pt_minfo_rec->u.t_stm_attribute);
                pt_strm_atrbt->b_is_codec_support = TRUE;
                x_memcpy(&(pt_strm_atrbt->u.t_es_audio_stm_attr),
                         &(pt_aud_es_hdlr->t_es_audio_stm_attr),
                         sizeof(MINFO_ES_AUDIO_STM_ATRBT_T));
                break;
            case MINFO_INFO_TYPE_IDX_INFO:
                pt_minfo_rec->u.t_idx_info.ui4_idx_type    = MM_MINFO_IDX_TYPE_NONE;
                pt_minfo_rec->u.t_idx_info.ui8_data_offset = pt_aud_es_hdlr->ui8_data_offset;
                pt_minfo_rec->u.t_idx_info.ui8_data_sz     = 
                    pt_aud_es_hdlr->ui8_file_size - pt_aud_es_hdlr->ui8_data_offset;
                break;

            case MINFO_INFO_TYPE_META_DATA:
                //x_memset(&(pt_minfo_rec->u.t_meta_data), 0, sizeof(MINFO_TYPE_META_DATA_T));
                pt_minfo_rec->u.t_meta_data.ui8_pb_duration 
                    = pt_aud_es_hdlr->t_generic.ui8_pb_duration;
                /*artist*/
                COPY_DATA( pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_artist, 
                            pt_aud_es_hdlr->t_es_acc_id3_attr.z_artist_len, 
                            pt_minfo_rec->u.t_meta_data.pwc_str_artist, 
                            &(pt_minfo_rec->u.t_meta_data.z_artist_len));
                /*album*/
                COPY_DATA( pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_album, 
                           pt_aud_es_hdlr->t_es_acc_id3_attr.z_album_len, 
                           pt_minfo_rec->u.t_meta_data.pwc_str_album, 
                           &(pt_minfo_rec->u.t_meta_data.z_album_len));
                /*year*/
                COPY_DATA( pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_year, 
                            pt_aud_es_hdlr->t_es_acc_id3_attr.z_year_len, 
                            pt_minfo_rec->u.t_meta_data.pwc_str_year, 
                            &(pt_minfo_rec->u.t_meta_data.z_year_len));

                /*title*/
                COPY_DATA( pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_title, 
                            pt_aud_es_hdlr->t_es_acc_id3_attr.z_title_len, 
                            pt_minfo_rec->u.t_meta_data.pwc_str_title, 
                            &(pt_minfo_rec->u.t_meta_data.z_title_len));

                /*genre*/
                COPY_DATA( pt_aud_es_hdlr->t_es_acc_id3_attr.pwc_str_genre, 
                            pt_aud_es_hdlr->t_es_acc_id3_attr.z_genre_len, 
                            pt_minfo_rec->u.t_meta_data.pwc_str_genre, 
                            &(pt_minfo_rec->u.t_meta_data.z_genre_len));
                
                break;
            default:
                i4_ret = MINFOR_INFO_NOT_FOUND;
                break;
        }
        return i4_ret;
    }
    else
    {
        return MINFOR_FMT_NOT_SUPPORT;
    }
} 

/*-----------------------------------------------------------------------------
 * Name:  aud_es_minfo_set_info
 *
 * Description:This function set information from a handler
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: 
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 aud_es_minfo_set_info(VOID*               pv_obj, 
                        MINFO_INFO_TYPE_T   e_info_type, 
                        MINFO_REC_IDX_T     ui2_index,                    
                        VOID*               pv_buf, 
                        SIZE_T*             pz_buf_leng)
{
    MINFO_OBJ_T*            pt_minfo_obj    = NULL;
    MINFO_AUD_ES_HANDLER_T* pt_aud_es_hdlr  = NULL;
    
    if (pv_obj == NULL)
    {
        return MINFOR_INITIAL_ERR;
    }
    
    pt_minfo_obj = (MINFO_OBJ_T*)pv_obj;
    pt_aud_es_hdlr = pt_minfo_obj->pv_handler_obj;

    if (pt_aud_es_hdlr == NULL)
    {
        return MINFOR_INITIAL_ERR;
    }
    
    switch (e_info_type)
    {
#ifdef DLNA_SUPPORT
    case MINFO_INFO_TYPE_SET_DLNA_DURAION:
        if (pv_buf == NULL)
        {
            return MINFOR_INV_ARG;
        }
        if (*(UINT64 *)pv_buf != 0)
        {
            pt_aud_es_hdlr->t_generic.ui8_pb_duration = *(UINT64 *)pv_buf;
#ifndef __KERNEL__
            DBG_LOG_PRINT(("AUD_ES_MINFO: Get duration %lld from DLNA server!\n",
                                pt_aud_es_hdlr->t_generic.ui8_pb_duration));
#endif
        }
        return MINFOR_OK;
#endif
    default:
        break;
    }

    return MINFOR_FMT_NOT_SUPPORT;
} 


#ifdef __cplusplus
}
#endif

