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
 * mp4_mfmtrecg.c,v
 * 1.1.2.1 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * BingsaiJin
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

#include "mutil/mm_util.h"

#include "x_common.h"
#include "handle/handle.h"
#include "u_handle_grp.h"
#include "u_mfmtrecg.h"
#include "x_dbg.h"
#include "u_dbg.h"

#ifndef __NO_FM__
#include "x_fm.h"
#endif

#include "mutil/mm_util_input.h"
#include "x_lnk_list.h"
#include "x_mm_common.h"

#else

#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "../u_mfmtrecg.h"
#include "x_dbg.h"
#include "u_dbg.h"

#include "../../mm_util.h"
#include "../../mm_util_input.h"

#include "x_lnk_list.h"
#include "x_mm_common.h"

#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define ape_dwBigFourCC(a,b,c,d)  ((((UINT32)a)<<24)|(((UINT32)b)<<16)|(((UINT32)c)<<8)|((UINT32)d))
#define APE_HEADER  ((' ' << 24) | ('C' << 16) | ('A' << 8) | ('M'))
#define ID3_TAG_HEADER (('I' << 16) | ('D' << 8) | ('3'))
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
* Name:  ape_minput4bytes_fct
*
* Description: 
*
* Inputs:  -
*          
* Outputs: -
*
* Returns: 
----------------------------------------------------------------------------*/ 
UINT32 ape_minput4bytes_fct (const VOID* pv_mem_data, UINT64* pui8_buff_pos)
{
    UINT32          ui4_ret = 0;
    UCHAR*          puc_buff = (UCHAR*)pv_mem_data;

    ui4_ret = (UINT32)(puc_buff[*pui8_buff_pos] << 24)
            | (UINT32)(puc_buff[*pui8_buff_pos + 1] << 16)
            | (UINT32)(puc_buff[*pui8_buff_pos + 2] << 8)
            | (UINT32)(puc_buff[*pui8_buff_pos + 3]);
    *pui8_buff_pos = *pui8_buff_pos + 4;
    
    return ui4_ret;
}

/*-----------------------------------------------------------------------------
 * Name: ape_handler_file_confirm_fct
 *
 * Description: this function will ask favor handler which match extension and favor
 *
 * Inputs:  h_file         specify the handle of the file
 *
 * Outputs: pt_format      retrieve the format of the file
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 ape_handler_file_confirm_fct(
                            HANDLE_T            h_file,        
                            MEDIA_FORMAT_T*     pt_format)     
{

#ifndef __NO_FM__
    UINT64          ui8_cur_pos = 0;
    INT32           i4_ret = 0;
    char            ac_tmp[10];
    UINT32          ui4_read = 0;
    UINT32          ui4_temp = 0;
    UINT32          ui4_id3_lenth = 0;
    BOOL            b_has_tag_foot = FALSE;
    UINT32          ui4_scan_bytes = 0;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pt_format == NULL) || (h_file == NULL_HANDLE))
    {
        return MFMTRECGR_INV_ARG;
    }
    
    do
    {
        i4_ret = x_fm_lseek(h_file, 0, FM_SEEK_BGN, &ui8_cur_pos);
        if(i4_ret != FMR_OK)
        {
            i4_ret = MFMTRECGR_FILE_ERR;
            break;
        }
        
        i4_ret = x_fm_read(h_file, (VOID*)&ac_tmp, 10, &ui4_read);
        if(FMR_OK != i4_ret || ui4_read < 10)
        {
            i4_ret = MFMTRECGR_FILE_ERR;
            break;
        }
        /*ID3 tag*/
        if (ac_tmp[0] == 'I' && ac_tmp[1] == 'D' && ac_tmp[2] == '3')
        {                        
            ui4_id3_lenth =   ((ac_tmp[6] & 0x7F) << 21) 
                             |((ac_tmp[7] & 0x7F) << 14)
                             |((ac_tmp[8] & 0x7F) << 7)
                             |((ac_tmp[9] & 0x7F));

            if (ac_tmp[5] & 16) 
            {
                b_has_tag_foot = TRUE;
                ui4_id3_lenth = ui4_id3_lenth + 20;
            }
            else 
            {
                ui4_id3_lenth = ui4_id3_lenth + 10;
            }

            if (!b_has_tag_foot)
            {
                CHAR ui1_temp = 0;
                
                i4_ret = x_fm_lseek(h_file, ui4_id3_lenth, FM_SEEK_BGN, &ui8_cur_pos);
                if(i4_ret != FMR_OK)
                {
                    i4_ret = MFMTRECGR_FILE_ERR;
                    break;
                }
                
                i4_ret = x_fm_read(h_file, (VOID*)&ui1_temp, 1, &ui4_read);
                if(FMR_OK != i4_ret || ui4_read < 1)
                {
                    i4_ret = MFMTRECGR_FILE_ERR;
                    break;
                }
                while (ui1_temp == 0 && ui4_read == 1)
                {
                    ui4_id3_lenth ++;
                    i4_ret = x_fm_read(h_file, (VOID*)&ui1_temp, 1, &ui4_read);
                    if(FMR_OK != i4_ret || ui4_read < 1)
                    {
                        return MFMTRECGR_FILE_ERR;
                    }
                }
            }
        }
        /* move after ID3 tag */
        i4_ret = x_fm_lseek(h_file, ui4_id3_lenth, FM_SEEK_BGN, &ui8_cur_pos);
        if(i4_ret != FMR_OK)
        {
            i4_ret = MFMTRECGR_FILE_ERR;
            break;
        }

        i4_ret = x_fm_read(h_file, (VOID*)&ui4_temp, 4, &ui4_read);
        if(FMR_OK != i4_ret || ui4_read < 4)
        {
            return MFMTRECGR_FILE_ERR;
        }
        
        ui4_read = 1;
        while ((ui4_temp != APE_HEADER) && (ui4_read == 1) && (ui4_scan_bytes < 1024))
        {
            UCHAR   ui1_temp = 0;
            i4_ret = x_fm_read(h_file, (VOID*)&ui1_temp, 1, &ui4_read);
            if(FMR_OK != i4_ret || ui4_read < 1)
            {
                return MFMTRECGR_FILE_ERR;
            }
            ui4_temp = (((UINT32) ui1_temp) << 24) | (ui4_temp >> 8);
            ui4_id3_lenth ++;
            ui4_scan_bytes ++;
        }

        if (ui4_temp != APE_HEADER)
        {
            return MFMTRECGR_INTERNAL_ERR;
        }
        else
        {
            pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
            pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_APE;
            pt_format->ui8_format_magic = ui4_id3_lenth;
        }
        
        i4_ret = MFMTRECGR_OK;
    }while(0);

    return i4_ret;
#else

    return MFMTRECGR_INTERNAL_ERR;

#endif
}
                            
/*-----------------------------------------------------------------------------
 * Name:  ape_handler_mem_confirm_fct
 *
 * Description: this function will ask all handler
 *
 * Inputs:  pv_mem_data     specify the address of memory buffer which want to retrieve the format.
 *          z_mem_leng      specify the length of memory buffer which want to retrieve the format.
 *
 * Outputs: pt_format       retrieve the format of the media memory buffer.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 ape_handler_mem_confirm_fct(const VOID*       pv_mem_data, 
                                   SIZE_T            z_mem_leng,   
                                   MEDIA_FORMAT_T*   pt_format) 
{

    MEDIA_FEEDER_T* pt_src_feeder;
    INT32           i4_ret = 0;
    UINT32          ui4_read = 0;
    UINT32          ui4_temp = 0;
    UINT32          ui4_id3_lenth = 0;
    BOOL            b_has_tag_foot = FALSE;
    UINT32          ui4_scan_bytes = 0;
    
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((pv_mem_data == NULL) || (pt_format == NULL))
    {
        return MFMTRECGR_INV_ARG;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    if(z_mem_leng == 0)
    {
        pt_src_feeder = (MEDIA_FEEDER_T*)pv_mem_data;

        do
        {
            char ac_tmp[10];

            i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_set_pos(
                                &pt_src_feeder->h_feeder,
                                0,
                                MEDIA_SEEK_BGN);
            if(INPUTR_OK != i4_ret)
            {
                i4_ret =  MFMTRECGR_FILE_ERR;
                break;
            }
            i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_copybytes(&pt_src_feeder->h_feeder, 
                                                                     (VOID*)ac_tmp, 
                                                                     10, 
                                                                     10,
                                                                     &ui4_read);
            if(i4_ret != INPUTR_OK)
            {
                i4_ret =  MFMTRECGR_FILE_ERR;
                break;
            }

            /*ID3 tag*/
            if (ac_tmp[0] == 'I' && ac_tmp[1] == 'D' && ac_tmp[2] == '3')
            {                        
                ui4_id3_lenth =   ((ac_tmp[6] & 0x7F) << 21) 
                                 |((ac_tmp[7] & 0x7F) << 14)
                                 |((ac_tmp[8] & 0x7F) << 7)
                                 |((ac_tmp[9] & 0x7F));

                if (ac_tmp[5] & 16) 
                {
                    b_has_tag_foot = TRUE;
                    ui4_id3_lenth = ui4_id3_lenth + 20;
                }
                else 
                {
                    ui4_id3_lenth = ui4_id3_lenth + 10;
                }

                if (!b_has_tag_foot)
                {
                    UINT8 ui1_tmp = 0;

                    i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_set_pos(
                                &pt_src_feeder->h_feeder,
                                ui4_id3_lenth,
                                MEDIA_SEEK_BGN);
                    if(INPUTR_OK != i4_ret)
                    {
                        i4_ret =  MFMTRECGR_FILE_ERR;
                        break;
                    }

                    i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_copybytes(&pt_src_feeder->h_feeder, 
                                                                     (VOID*)&ui1_tmp, 
                                                                     1,
                                                                     1,
                                                                     &ui4_read);
                    if(i4_ret != INPUTR_OK)
                    {
                        i4_ret =  MFMTRECGR_FILE_ERR;
                        break;
                    }
                    
                    while (ui1_tmp == 0 && ui4_read == 1)
                    {
                        ui4_id3_lenth ++;
                        i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_copybytes(&pt_src_feeder->h_feeder, 
                                                                     (VOID*)&ui1_tmp, 
                                                                     1,
                                                                     1,
                                                                     &ui4_read);
                        if(INPUTR_OK != i4_ret || ui4_read < 1)
                        {
                            return MFMTRECGR_FILE_ERR;
                        }
                    }
                }
            }

            /* move after ID3 tag */
            i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_set_pos(
                                &pt_src_feeder->h_feeder,
                                ui4_id3_lenth,
                                MEDIA_SEEK_BGN);
            if(i4_ret != INPUTR_OK)
            {
                i4_ret = MFMTRECGR_FILE_ERR;
                break;
            }
            i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_copybytes(&pt_src_feeder->h_feeder, 
                                                                     (VOID*)&ui4_temp, 
                                                                     4,
                                                                     4,
                                                                     &ui4_read);
            if(INPUTR_OK != i4_ret || ui4_read < 4)
            {
                return MFMTRECGR_FILE_ERR;
            }
            
            ui4_read = 1;
            while ((ui4_temp != APE_HEADER) && (ui4_read == 1) && (ui4_scan_bytes < 1024))
            {
                UINT32   ui4_tmp = 0;
                i4_ret = pt_src_feeder->t_feeder_be_fct_tbl.pf_copybytes(&pt_src_feeder->h_feeder, 
                                                                     (VOID*)&ui4_tmp, 
                                                                     1,
                                                                     1,
                                                                     &ui4_read);
                if(INPUTR_OK != i4_ret || ui4_read < 1)
                {
                    return MFMTRECGR_FILE_ERR;
                }
                ui4_temp = ((ui4_tmp) << 24) | (ui4_temp >> 8);
                ui4_id3_lenth ++;
                ui4_scan_bytes ++;
            }

            if (ui4_temp != APE_HEADER)
            {
                return MFMTRECGR_INTERNAL_ERR;
            }
            else
            {
                pt_format->e_media_type = MEDIA_TYPE_AUDIO_ES;
                pt_format->t_media_subtype.u.e_aud_subtype = MEDIA_AUD_SUBTYPE_APE;
                pt_format->ui8_format_magic = ui4_id3_lenth;
            }
            
            i4_ret = MFMTRECGR_OK;
        }while(0);
    }
    else
    {
        return MFMTRECGR_INTERNAL_ERR;
    }

    return i4_ret;
}

