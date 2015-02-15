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
 * $RCSfile: x_mfmtrecg.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/3 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 0a0e5908fac0e73885ce39efe82bcd61 $ Asa
 *
 * Description: 
 *         This file contains all the media format recognizer interface APIs  
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
#include "u_mfmtrecg.h"
#include "mutil/mfmtrecg/mfmtrecg.h"
#include "mutil/mfmtrecg/mfmtrecg_cli.h"
#ifndef __NO_FM__
#include "x_fm.h"
#endif

#ifdef DLNA_SUPPORT
#include "x_fm_dlnafs.h"
#endif

#include "x_dbg.h"
#include "u_dbg.h"
#include "mutil/mm_util.h"

#else

#include "x_os.h"
#include "x_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "u_mfmtrecg.h"
#include "mfmtrecg.h"
#include "mfmtrecg_cli.h"
#include "x_dbg.h"
#include "u_dbg.h"
#include "../mm_util.h"

#endif
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
 #define MAX_FILE_EXTENSION_NAME 256
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/
 extern HANDLE_T gh_mfmtrecg_obj_lock;
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

 static BOOL _mfmtrecg_free_handle (
     HANDLE_T        h_handle,
     HANDLE_TYPE_T   e_type,
     VOID*           pv_obj,
     VOID*           pv_tag,
     BOOL            b_req_handle)
 {
 
     if (pv_obj)
     {
          if ((e_type == HT_MFMTRECG_FILE_TYPE)
                 || (e_type == HT_MFMTRECG_MEM_TYPE))
         {
             _mm_util_mem_free(pv_obj);
             pv_obj = NULL;
             return TRUE;
         }
     }
     return FALSE;
 }


/*-----------------------------------------------------------------------------
 * Name:  x_mfmtrecg_get_file_fmt
 *
 * Description: This API retrieves the format of the media file.
 *
 * Inputs:  ps_name         specify the path name of the media file which want to retrieve the format and it is not case sensitive.
 *
 * Outputs: pt_format       retrieve the format of the current media file.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 x_mfmtrecg_get_file_fmt (
                                const CHAR*       ps_name,       
                                MEDIA_FORMAT_T*   pt_format)  
{
    return mfmtrecg_get_file_fmt(ps_name, pt_format);
}

 
/*-----------------------------------------------------------------------------
 * Name:  x_mfmtrecg_get_mem_fmt
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
INT32 x_mfmtrecg_get_mem_fmt ( 
                        const VOID*       pv_mem_data, 
                        SIZE_T            z_mem_leng,   
                        MEDIA_FORMAT_T*   pt_format)   
{
    return mfmtrecg_get_mem_fmt(pv_mem_data, z_mem_leng, pt_format);
}


/*-----------------------------------------------------------------------------
 * Name:  x_mfmtrecg_get_file_fmt_async
 *
 * Description: This API retrieves the format of the media file.
 *
 * Inputs:  ps_name         specify the path name of the media file which want to retrieve the format and it is not case sensitive.
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 x_mfmtrecg_get_file_fmt_async(
                                const CHAR*         ps_name,
                                MEDIA_FORMAT_T*     pt_format,
                                x_mfmtrecg_nfy_fct  pf_nfy,
                                VOID*               pv_tag,
                                HANDLE_T*           ph_mfmtrecg)
{
#ifndef __NO_FM__    
    MFMTRECG_ASYNC_MSG_T   t_notify_msg;
    MFMTRECG_OBJ_T*         pt_obj;
    INT32                   i4_ret = MFMTRECGR_INV_ARG;
    
    if (ph_mfmtrecg == NULL)
    {
        return MFMTRECGR_INV_ARG;
    }

    /* allocate elm_obj memory */
    pt_obj = _mm_util_mem_alloc(sizeof(MFMTRECG_OBJ_T));
    if (pt_obj == NULL)
    {
        return MFMTRECGR_OUT_OF_MEMORY;
    }
    x_memset(pt_obj, 0, sizeof(MFMTRECG_OBJ_T));

    if (HR_OK != handle_alloc(HT_MFMTRECG_FILE_TYPE, pt_obj, NULL, _mfmtrecg_free_handle, ph_mfmtrecg))
    {
        _mm_util_mem_free(pt_obj);
        return MFMTRECGR_OUT_OF_HANDLE;
    }

    pt_obj->h_mfmftrecg = *ph_mfmtrecg;
    pt_obj->pf_nfy = pf_nfy;
    pt_obj->pv_tag = pv_tag;
    pt_obj->pt_format = pt_format;
    
    if (pt_obj->h_sync_lock == NULL_HANDLE)
    {
        /* create semaphore */
        i4_ret = x_sema_create( &pt_obj->h_sync_lock,
                             X_SEMA_TYPE_BINARY,
                             X_SEMA_STATE_LOCK );

        if ( i4_ret != OSR_OK || pt_obj->h_sync_lock == NULL_HANDLE )
        {
            handle_free(*ph_mfmtrecg, TRUE);
            return MINFOR_INTERNAL_ERR;
        }
    }

    x_memset(&t_notify_msg, 0, sizeof(MFMTRECG_ASYNC_MSG_T));

    t_notify_msg.e_msg_type = MFMTRECG_MSG_TYPE_FILE;
    t_notify_msg.h_handle = pt_obj->h_mfmftrecg;
    t_notify_msg.u.t_fmtrecog_file_msg.ps_name = ps_name;
    
    return mfmtrecg_send_notify_msg (&t_notify_msg);
#else
    return 0;
#endif
}

 
/*-----------------------------------------------------------------------------
 * Name:  x_mfmtrecg_get_mem_fmt_async
 *
 * Description: This API retrieves the format of the media memory buffer.
 *
 * Inputs:  pv_mem_data     specify the address of memory buffer which want to retrieve the format.
 *          z_mem_leng      specify the length of memory buffer which want to retrieve the format.

 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 x_mfmtrecg_get_mem_fmt_async ( 
                        const VOID*         pv_mem_data,
                        SIZE_T              z_mem_leng,
                        MEDIA_FORMAT_T*     pt_format,
                        x_mfmtrecg_nfy_fct  pf_nfy,
                        VOID*               pv_tag,
                        HANDLE_T*           ph_mfmtrecg)
{
    MFMTRECG_ASYNC_MSG_T   t_notify_msg;
    MFMTRECG_OBJ_T*         pt_obj;
    INT32                   i4_ret = MFMTRECGR_INV_ARG;

    if (ph_mfmtrecg == NULL)
    {
        return MFMTRECGR_INV_ARG;
    }

    /* allocate elm_obj memory */
    pt_obj = _mm_util_mem_alloc(sizeof(MFMTRECG_OBJ_T));
    if (pt_obj == NULL)
    {
        return MFMTRECGR_OUT_OF_MEMORY;
    }
    x_memset(pt_obj, 0, sizeof(MFMTRECG_OBJ_T));

    if (HR_OK != handle_alloc(HT_MFMTRECG_MEM_TYPE, pt_obj, NULL, _mfmtrecg_free_handle, ph_mfmtrecg))
    {
        _mm_util_mem_free(pt_obj);
        return MFMTRECGR_OUT_OF_HANDLE;
    }

    pt_obj->h_mfmftrecg = *ph_mfmtrecg;
    pt_obj->pf_nfy = pf_nfy;
    pt_obj->pv_tag = pv_tag;
    pt_obj->pt_format = pt_format;
    
    if (pt_obj->h_sync_lock == NULL_HANDLE)
    {
        /* create semaphore */
        i4_ret = x_sema_create( &pt_obj->h_sync_lock,
                             X_SEMA_TYPE_BINARY,
                             X_SEMA_STATE_LOCK );

        if ( i4_ret != OSR_OK || pt_obj->h_sync_lock == NULL_HANDLE )
        {
            handle_free(*ph_mfmtrecg, TRUE);
            return MINFOR_INTERNAL_ERR;
        }
    }

    x_memset(&t_notify_msg, 0, sizeof(MFMTRECG_ASYNC_MSG_T));
    
    t_notify_msg.e_msg_type = MFMTRECG_MSG_TYPE_MEM;
    t_notify_msg.h_handle = *ph_mfmtrecg;
    t_notify_msg.u.t_fmtrecog_mem_msg.pv_data = pv_mem_data;
    t_notify_msg.u.t_fmtrecog_mem_msg.z_size = z_mem_leng;
    
    return mfmtrecg_send_notify_msg (&t_notify_msg);
}

/*-----------------------------------------------------------------------------
 * Name:  x_mfmtrecg_close
 *
 * Description: This API closes the media recognize handle.
 *
 * Inputs:  h_mfmtrecg         specify the media recognize handle which will be closed.
 *
 * Outputs:
 *
 * Returns: MFMTRECGR_OK    Success. The routine was successful and the operation has been completed.
 ----------------------------------------------------------------------------*/
INT32 x_mfmtrecg_close (HANDLE_T h_mfmtrecg)
{
    INT32                           i4_ret;
    HANDLE_TYPE_T                   e_handle_type;
    VOID*                           pv_obj = NULL;
    MFMTRECG_OBJ_T*                 pt_obj;
    MFMTRECG_ASYNC_MSG_T            t_notify_msg;

    DBG_API(("[mfmtrecg] x_mfmtrecg_close: h_minfo:%x \n\r"
        , h_mfmtrecg));
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (h_mfmtrecg == NULL_HANDLE)
    {
        return MFMTRECGR_INV_ARG;
    }
    i4_ret = handle_get_type_obj(h_mfmtrecg, &e_handle_type, &pv_obj);
    if ((i4_ret != HR_OK)
        || ((e_handle_type != HT_MFMTRECG_FILE_TYPE) && (e_handle_type != HT_MFMTRECG_MEM_TYPE))
        || (pv_obj == NULL))
    {
        return MFMTRECGR_INV_HANDLE;
    }
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/

    pt_obj = (MFMTRECG_OBJ_T*)pv_obj;
    
    t_notify_msg.h_handle = pt_obj->h_mfmftrecg;
    t_notify_msg.e_msg_type = MFMTRECG_MSG_TYPE_CLOSED;
    i4_ret = mfmtrecg_send_notify_msg (&t_notify_msg);
    x_sema_lock(pt_obj->h_sync_lock, X_SEMA_OPTION_WAIT);
    if (pt_obj->h_sync_lock)
    {
        x_sema_delete(pt_obj->h_sync_lock);
        pt_obj->h_sync_lock = NULL_HANDLE;
    }
    x_sema_lock(gh_mfmtrecg_obj_lock, X_SEMA_OPTION_WAIT);
    handle_free(pt_obj->h_mfmftrecg, TRUE);
    x_sema_unlock(gh_mfmtrecg_obj_lock);
    
    return i4_ret;
}



