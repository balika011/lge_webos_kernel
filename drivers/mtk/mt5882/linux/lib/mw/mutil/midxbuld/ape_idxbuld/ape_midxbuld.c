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
 * $RCSfile: ape_midxbuld.c,v $
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
#include "u_midxbuld.h"
#include "mutil/midxbuld/ape_idxbuld/ape_midxbuld.h"
#include "x_dbg.h"

#ifndef __NO_FM__
#include "x_fm.h"
#endif

#include "x_lnk_list.h"

#include "mutil/mm_common/rfc_ebml/rfc_ebml.h"
#include "mutil/mm_util_input.h"
#include "mutil/mm_util_memory.h"
#include "mutil/mm_util.h"

#else

#include "x_os.h"
#include "x_common.h"
#include "x_mm_common.h"
#include "handle.h"
#include "u_handle_grp.h"
#include "../u_midxbuld.h"
#include "ape_midxbuld.h"
#include "x_dbg.h"

#include "../../mm_util.h"


#include "x_lnk_list.h"
#include "../../mm_common/rfc_ebml/rfc_ebml.h"
#include "../../mm_util_input.h"
#include "../../mm_util_memory.h"

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
/*static UINT8        ui1_idx_range_id_last = 1;*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
static INT32 _ape_idx_set_pos(MIDXBULD_OBJ_T *pt_midxbuld_obj, 
                                 UINT64 ui8_file_offset, 
                                 UINT8 ui1_whence)
{
    setpos_fct      pf_set_pos  = pt_midxbuld_obj->t_input_fct_tbl.pf_set_pos; 
    MEDIA_FEEDER_T  t_feeder;
    /*feeder*/
    if(pt_midxbuld_obj->h_src_feeder != NULL_HANDLE)
    {
        t_feeder.h_feeder = pt_midxbuld_obj->h_src_feeder;
        return pf_set_pos(&t_feeder, (INT64)ui8_file_offset, ui1_whence);
    }
    /*file*/
    else
    {
        return pf_set_pos(pt_midxbuld_obj, (INT64)ui8_file_offset, ui1_whence);
    }
}

static INT32 _ape_idx_copy_bytes(MIDXBULD_OBJ_T *pt_midxbuld_obj, 
                                    void* pv_buf, 
                                    SIZE_T z_buf_leng, 
                                    SIZE_T z_copy_size, 
                                    UINT32* pui4_size)
{
    copybytes_fct      pf_copybyte  = pt_midxbuld_obj->t_input_fct_tbl.pf_copybytes; 
    MEDIA_FEEDER_T     t_feeder;
    /*feeder*/
    if(pt_midxbuld_obj->h_src_feeder != NULL_HANDLE)
    {
        t_feeder.h_feeder = pt_midxbuld_obj->h_src_feeder;
        return pf_copybyte(&t_feeder, pv_buf, z_buf_leng, z_copy_size, pui4_size);
    }
    /*file*/
    else
    {
        return pf_copybyte(pt_midxbuld_obj, pv_buf, z_buf_leng, z_copy_size, pui4_size);
    }
}

/*-----------------------------------------------------------------------------
 * Name:  ape_midxbuld_fopen
 *
 * Description: This function open a handler for a file
 *
 * Inputs:  ps_path_name: specify the path name of the file
 *          pt_key_info : specify the key info object  
 *          pv_obj      : specify the index builder object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MIDXBULDR_OK    Success. The routine was successful and 
 *                           the operation has been completed.
 *          Other non-zero values: Fail
 * Note:
 *
 * Author : lianming lin(mcn07123)
 * 
 * History:
 *   (1)2008-10-29 : initial
 ----------------------------------------------------------------------------*/
INT32 ape_midxbuld_fopen(
                          CHAR*                ps_path_name, 
                          MIDXBULD_KEY_INFO_T* pt_key_info, 
                          VOID*                pv_obj )
{

#ifndef __NO_FM__

    INT32               i4_ret = 0;
    MIDXBULD_OBJ_T*     pt_midxbuld_obj = NULL;
    APE_IDXBULD_OBJ_T*  pt_ape_handler = NULL;
    UINT32              ui4_read_sz = 0;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_obj) || (NULL == ps_path_name) || (NULL == pt_key_info))
    {
        DBG_ERROR(("ape_midxbuld_fopen:NULL\r\n"));
        return MIDXBULDR_INV_ARG;
    }

    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    /*init local variables*/
    pt_midxbuld_obj = (MIDXBULD_OBJ_T*)pv_obj;
    
    /*set input functions*/
    pt_midxbuld_obj->t_input_fct_tbl.pf_copybytes = mm_util_fcopybytes_fct;
    pt_midxbuld_obj->t_input_fct_tbl.pf_input1    = mm_util_finput1bytes_fct;
    pt_midxbuld_obj->t_input_fct_tbl.pf_input3    = mm_util_finput3bytes_b_fct;
    pt_midxbuld_obj->t_input_fct_tbl.pf_input4    = mm_util_finput4bytes_b_fct;
    pt_midxbuld_obj->t_input_fct_tbl.pf_set_pos   = mm_util_fsetpos_fct;
    pt_midxbuld_obj->t_input_fct_tbl.pf_get_pos   = mm_util_fgetpos_fct;


    /*ape handler for index builder*/
    pt_ape_handler = (APE_IDXBULD_OBJ_T*)_mm_util_mem_alloc_with_tag(
                            sizeof(APE_IDXBULD_OBJ_T), MM_MEM_MIDX_TAG);
    if ( NULL == pt_ape_handler )
    {
        DBG_ERROR(("ape_midxbuld_fopen:pt_ape_handler is NULL\r\n"));
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pt_ape_handler, 0, sizeof(APE_IDXBULD_OBJ_T));
    
    /*set ape handler*/
    pt_midxbuld_obj->pv_handler_obj = pt_ape_handler;

    i4_ret = x_fm_open(FM_ROOT_HANDLE, 
                       ps_path_name,
                       FM_READ_ONLY, 
                       0777, FALSE, 
                       &pt_midxbuld_obj->h_file);
    if (i4_ret != FMR_OK)
    {
        DBG_ERROR(("ape_midxbuld_fopen:x_fm_open return %d\r\n", i4_ret));
        _mm_util_mem_free_with_tag(pt_ape_handler, MM_MEM_MIDX_TAG);
        pt_midxbuld_obj->pv_handler_obj = NULL;
        return MIDXBULDR_FILE_ERR;
    }

    /*read index table*/

    pt_ape_handler->pt_key_tbl_ext = (MIDXBULD_KEY_TBL_EXT_T*)_mm_util_mem_alloc_with_tag(
                                        sizeof(MIDXBULD_KEY_TBL_EXT_T), MM_MEM_MIDX_TAG);

    if ( NULL == pt_ape_handler->pt_key_tbl_ext )
    {
        DBG_ERROR(("ape_midxbuld_fopen:pt_ape_handler->pt_key_tbl_ext is NULL\r\n"));
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pt_ape_handler->pt_key_tbl_ext, 0, sizeof(MIDXBULD_KEY_TBL_EXT_T)); 
    
    pt_ape_handler->ui4_buf_length = pt_midxbuld_obj->t_src_info.ui8_idx_size;
    pt_ape_handler->pui1_header_buf = (UINT8*)_mm_util_mem_alloc_with_tag(
                                        pt_ape_handler->ui4_buf_length, MM_MEM_MIDX_TAG);
                                        
    if (pt_ape_handler->pui1_header_buf == NULL)
    {
        DBG_ERROR(("ape_midxbuld_fopen: alloc buffer fail!\r\n"));
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    _ape_idx_set_pos(pt_midxbuld_obj, pt_midxbuld_obj->t_src_info.ui8_idx_offset, FM_SEEK_BGN);
    i4_ret = _ape_idx_copy_bytes(pt_midxbuld_obj, 
                        pt_ape_handler->pui1_header_buf, 
                        (SIZE_T)pt_ape_handler->ui4_buf_length, 
                        (SIZE_T)pt_midxbuld_obj->t_src_info.ui8_idx_size, 
                        &ui4_read_sz);
    if ((i4_ret != INPUTR_OK) || (ui4_read_sz < pt_midxbuld_obj->t_src_info.ui8_idx_size))
    {
        DBG_ERROR(("APE_IDXBULD_ERR : read index data fail!\r\n"));
        return MIDXBULDR_KEY_TBL_NOT_READY;
    }

    
    
    return MIDXBULDR_OK;

#else
    return MIDXBULDR_INITIAL_ERR;
#endif
}

/*-----------------------------------------------------------------------------
 * Name:  ape_midxbuld_mopen
 *
 * Description: This function open a handler for a memory buffer
 *
 * Inputs:  pv_mem_data  specify the address of the memory buffer
 *          z_mem_leng   specify the length of the buffer
 *          pv_obj       specify the media info object  
 *
 * Outputs: pv_obj       modify data in the media info object  
 *
 * Returns: MIDXBULDR_OK    Success. The routine was successful and the 
 *                            operation has been completed.
 *          Other non-zero values: Fail
 * Note:
 *
 * Author : lianming lin(mcn07123)
 * 
 * History:
 *   (1)2008-10-29 : initial

 ----------------------------------------------------------------------------*/
INT32 ape_midxbuld_mopen(
                         VOID*                pv_mem_data,
                         SIZE_T               z_mem_leng,
                         MIDXBULD_KEY_INFO_T* pt_key_info,
                         VOID*                pv_obj)
{
    INT32               i4_ret = 0;
    UINT32              ui4_read_sz = 0;
    MIDXBULD_OBJ_T*     pt_midxbuld_obj;
    APE_IDXBULD_OBJ_T*  pt_ape_handler;
    MEDIA_FEEDER_T*     pt_src_feeder;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_mem_data) || (NULL == pv_obj))
    {
        DBG_ERROR(("ape_midxbuld_mopen:NULL\r\n"));
        return MIDXBULDR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    pt_midxbuld_obj = (MIDXBULD_OBJ_T*)pv_obj;

    if (z_mem_leng == 0)
    {
    #if 1
        pt_src_feeder = (MEDIA_FEEDER_T*)pv_mem_data;
        pt_midxbuld_obj->t_input_fct_tbl.pf_copybytes =
            pt_src_feeder->t_feeder_le_fct_tbl.pf_copybytes;
        pt_midxbuld_obj->t_input_fct_tbl.pf_input1 =
            pt_src_feeder->t_feeder_le_fct_tbl.pf_input1;
        pt_midxbuld_obj->t_input_fct_tbl.pf_input3 =
            pt_src_feeder->t_feeder_le_fct_tbl.pf_input3;
        pt_midxbuld_obj->t_input_fct_tbl.pf_input4 =
            pt_src_feeder->t_feeder_le_fct_tbl.pf_input4;
        pt_midxbuld_obj->t_input_fct_tbl.pf_set_pos =
            pt_src_feeder->t_feeder_le_fct_tbl.pf_set_pos;
        pt_midxbuld_obj->t_input_fct_tbl.pf_get_pos =
            pt_src_feeder->t_feeder_le_fct_tbl.pf_get_pos;
    #endif
    }
    else
    {
        pt_midxbuld_obj->t_input_fct_tbl.pf_copybytes 
            = mm_util_mcopybytes_fct;
        pt_midxbuld_obj->t_input_fct_tbl.pf_input1    
            = mm_util_minput1bytes_fct;
        pt_midxbuld_obj->t_input_fct_tbl.pf_input3    
            = mm_util_minput3bytes_b_fct;
        pt_midxbuld_obj->t_input_fct_tbl.pf_input4    
            = mm_util_minput4bytes_b_fct;
        pt_midxbuld_obj->t_input_fct_tbl.pf_set_pos   
            = mm_util_msetpos_fct;
        pt_midxbuld_obj->t_input_fct_tbl.pf_get_pos   
            = mm_util_mgetpos_fct;
    }

    pt_midxbuld_obj->i8_buff_pos = 0;

    /*allocate memory for idex builder handler*/
    pt_ape_handler = (APE_IDXBULD_OBJ_T*)_mm_util_mem_alloc_with_tag(
                                          sizeof(APE_IDXBULD_OBJ_T),
                                          MM_MEM_MIDX_TAG);
    if (NULL == pt_ape_handler)
    {
        DBG_ERROR(("ape_midxbuld_mopen:pt_ape_handler NULL\r\n"));
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pt_ape_handler, 0, sizeof(APE_IDXBULD_OBJ_T));
    pt_midxbuld_obj->pv_handler_obj = pt_ape_handler;
            
    pt_ape_handler->pt_key_tbl_ext = (MIDXBULD_KEY_TBL_EXT_T*)_mm_util_mem_alloc_with_tag(
                                        sizeof(MIDXBULD_KEY_TBL_EXT_T), MM_MEM_MIDX_TAG);

    if ( NULL == pt_ape_handler->pt_key_tbl_ext )
    {
        DBG_ERROR(("ape_midxbuld_mopen:pt_ape_handler->pt_key_tbl_ext is NULL\r\n"));
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    x_memset(pt_ape_handler->pt_key_tbl_ext, 0, sizeof(MIDXBULD_KEY_TBL_EXT_T)); 
    
    pt_ape_handler->ui4_buf_length = pt_midxbuld_obj->t_src_info.ui8_idx_size;
    pt_ape_handler->pui1_header_buf = (UINT8*)_mm_util_mem_alloc_with_tag(
                                        pt_ape_handler->ui4_buf_length, MM_MEM_MIDX_TAG);
                                        
    if (pt_ape_handler->pui1_header_buf == NULL)
    {
        DBG_ERROR(("ape_midxbuld_mopen: alloc buffer fail!\r\n"));
        return MIDXBULDR_OUT_OF_MEMORY;
    }
    _ape_idx_set_pos(pt_midxbuld_obj, pt_midxbuld_obj->t_src_info.ui8_idx_offset, FM_SEEK_BGN);
    i4_ret = _ape_idx_copy_bytes(pt_midxbuld_obj, 
                        pt_ape_handler->pui1_header_buf, 
                        (SIZE_T)pt_ape_handler->ui4_buf_length, 
                        (SIZE_T)pt_midxbuld_obj->t_src_info.ui8_idx_size, 
                        &ui4_read_sz);
    if ((i4_ret != INPUTR_OK) || (ui4_read_sz < pt_midxbuld_obj->t_src_info.ui8_idx_size))
    {
        DBG_ERROR(("APE_IDXBULD_ERR : read index data fail!\r\n"));
        return MIDXBULDR_KEY_TBL_NOT_READY;
    }
    return MIDXBULDR_OK;    
}    

/*-----------------------------------------------------------------------------
 * Name:  ape_midxbuld_close
 *
 * Description: This function close a handler
 *
 * Inputs:  pv_obj       specify the media info object  
 *
 * Outputs: NULL
 *
 * Returns: MIDXBULDR_OK    Success. The routine was successful and the 
 *                            operation has been completed.
 *          Other non-zero values: Fail
 * Note:
 *
 * Author : lianming lin(mcn07123)
 * 
 * History:
 *   (1)2008-10-29 : initial
 ----------------------------------------------------------------------------*/
INT32 ape_midxbuld_close(VOID* pv_obj)
{
    MIDXBULD_OBJ_T*          pt_midxbuld_obj;
    APE_IDXBULD_OBJ_T*       pt_ape_handler;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if (NULL == pv_obj)  
    {
        DBG_ERROR(("ape_midxbuld_close NULL\r\n"));
        return MIDXBULDR_INV_ARG;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    /*get index object*/
    pt_midxbuld_obj = (MIDXBULD_OBJ_T*) pv_obj;
    /*get ape inde handler*/
    pt_ape_handler = (APE_IDXBULD_OBJ_T*)pt_midxbuld_obj->pv_handler_obj;
    if (NULL == pt_ape_handler)
    {
        DBG_ERROR(("ape_midxbuld_close pt_ape_handler NULL\r\n"));
        return MIDXBULDR_INV_ARG;
    }
    /*close file*/
    if (pt_midxbuld_obj->h_file)
    {
#ifndef __NO_FM__
        x_fm_close(pt_midxbuld_obj->h_file);
#endif
        pt_midxbuld_obj->h_file = NULL_HANDLE;
    }

    if (pt_ape_handler->pui1_header_buf != NULL)
    {
        _mm_util_mem_free_with_tag(pt_ape_handler->pui1_header_buf, MM_MEM_MIDX_TAG);
        pt_ape_handler->pui1_header_buf = NULL;
    }

    if (pt_ape_handler->pt_key_tbl_ext)
    {
        _mm_util_mem_free_with_tag(pt_ape_handler->pt_key_tbl_ext, MM_MEM_MIDX_TAG);
        pt_ape_handler->pt_key_tbl_ext = NULL;
    }
    
    /*free handler object*/
    if (pt_midxbuld_obj->pv_handler_obj)
    {
        _mm_util_mem_free_with_tag(pt_midxbuld_obj->pv_handler_obj, 
                                   MM_MEM_MIDX_TAG);
        pt_midxbuld_obj->pv_handler_obj = NULL;
    }

    return MIDXBULDR_OK;
}    


/*-----------------------------------------------------------------------------
 * Name:  ape_midxbuld_start_indexing
 *
 * Description: This function start indexing
 *
 * Inputs:  pv_obj        ,  specify the media info object 
 *           pui4_range_id,  specify which range 
 *
 * Outputs: NULL
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation
 *                        has been completed.
 *          Other non-zero values: Fail
 *
 * Note:  NULL
 * 
 * Author : lianming lin(mcn07123)
 * 
 * History:
 *   (1)2008-10-29 : initial
 ----------------------------------------------------------------------------*/
INT32 ape_midxbuld_start_indexing(VOID* pv_obj, UINT32* pui4_range_id)
{
    MIDXBULD_OBJ_T*             pt_midxbuld_obj = NULL;
    APE_IDXBULD_OBJ_T*          pt_ape_handler = NULL;
    MIDXBULD_ELMT_TBL_OBJ_T*    pt_elmt_tbl_obj = NULL;

    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_obj) || (NULL == pui4_range_id))
    {
        DBG_ERROR(("ape_midxbuld_start_indexing: NULL\r\n"));
        return MIDXBULDR_INITIAL_ERR;
    }

    /************************************************************************/
    /* Do                                                                   */
    /************************************************************************/
    
    pt_midxbuld_obj = (MIDXBULD_OBJ_T*) pv_obj;
    
    pt_ape_handler = (APE_IDXBULD_OBJ_T*)pt_midxbuld_obj->pv_handler_obj;
    
    if(pt_ape_handler && pt_ape_handler->pt_key_tbl_ext)
    {
    pt_elmt_tbl_obj = &(pt_ape_handler->pt_key_tbl_ext->t_elmt_tbl);
    }

    /*build index array*/
    if(pt_elmt_tbl_obj)
    {
        pt_elmt_tbl_obj->e_tbl_type = MIDXBULD_TBL_TYPE_AUDIO_KEY;
        pt_elmt_tbl_obj->ui4_number_of_entry = pt_ape_handler->ui4_buf_length/sizeof(int);
        pt_elmt_tbl_obj->ui8_base_offset = pt_midxbuld_obj->t_src_info.ui8_idx_offset;
        pt_elmt_tbl_obj->u_fmt.t_as_key.e_entry_type = MIDXBULD_AUD_OFFSET_ENTRY_TYPE;
        
        pt_elmt_tbl_obj->u_fmt.t_as_key.pui4_offset = (UINT32*)pt_ape_handler->pui1_header_buf; 
        pt_elmt_tbl_obj->u_fmt.t_as_key.t_info.e_es_type = MIDXBULD_ELEM_STRM_TYPE_KEY;
        //pt_elmt_tbl_obj->u_fmt.t_as_key.t_info = pt_midxbuld_obj->t_src_info.t_es_map[0];
        DBG_ERROR(("%s, build index table successfully, entry number %u\r\n", 
            __func__, pt_elmt_tbl_obj->ui4_number_of_entry));
    }
    return MIDXBULDR_OK;
}

/*-----------------------------------------------------------------------------
 * Name:  ape_midxbuld_get_info
 *
 * Description: This API retrieves the information of media idx builder handle. 
 *              It will return MINFO_NOT_READY, before parse media complete.
 *
 * Inputs:  pv_obj       specify the media info object  
 *          e_info_type  specify the information type of the media.
 *          pv_buf       specify the memory buffer which will contain 
 *                       the media information.
 *          pz_buf_leng  specify the length of the buffer.
 *
 * Outputs: pv_buf       specify the memory buffer which will contain 
 *                       the media information
 *          pz_buf_leng  If it is smaller than required, it will return 
 *                       MINFO_INSUFFICENT_MEM, and pz_buf_leng will contain
 *                       required length.
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation 
 *                       has been completed.
 *          Other non-zero values: Fail
 * Note:    NULL
 *
 * Author : lianming lin(mcn07123)
 * 
 * History:
 *   (1)2008-10-29 : initial

 ----------------------------------------------------------------------------*/
INT32 ape_midxbuld_get_info(
                            VOID*                    pv_obj,   
                            MIDXBULD_GET_INFO_TYPE_T e_info_type,  
                            VOID*                    pv_buf,     
                            SIZE_T*                  pz_buf_leng)
{
    MIDXBULD_OBJ_T*             pt_midxbuld_obj;
    APE_IDXBULD_OBJ_T*          pt_ape_handler;
    MIDXBULD_KEY_TBL_INFO_T*    pt_key_tbl_info;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_obj) || (NULL == pz_buf_leng))
    {
        DBG_ERROR(("ape_midxbuld_get_info:NULL\r\n"));
        return MIDXBULDR_INITIAL_ERR;
    }
    
    /************************************************************************/
    /* Do                                                                   */
    /************************************************************************/
    /*get index builder object*/
    pt_midxbuld_obj = (MIDXBULD_OBJ_T*) pv_obj;
    /*get ape index builder handler*/
    pt_ape_handler = (APE_IDXBULD_OBJ_T*)pt_midxbuld_obj->pv_handler_obj;
    if (NULL == pt_ape_handler) 
    {
        DBG_ERROR(("ape_midxbuld_get_info:pt_ape_handler NULL\r\n"));
        return MIDXBULDR_INITIAL_ERR;
    }
    
    switch(e_info_type)
    {
        case MIDXBULD_GET_INFO_TYPE_IDX_ELMT_TBL:
            if (*pz_buf_leng < sizeof(MIDXBULD_ELMT_TBL_INFO_T))
            {
                DBG_ERROR(("ape_midxbuld_get_info: get type %d, buffer is not enough (%d).\r\n",
                           (int)e_info_type, (int)*pz_buf_leng));
                return MIDXBULDR_INITIAL_ERR;
            }
            break;
        case MIDXBULD_GET_INFO_TYPE_KEY_IDX_TBL_LIST:
            if (pv_buf == NULL)
            {
                DBG_ERROR(("ape_midxbuld_get_info: pv_buf == null.\r\n"));
                return MIDXBULDR_INITIAL_ERR;
            }
            if (*pz_buf_leng < sizeof(MIDXBULD_KEY_TBL_INFO_T))
            {
                DBG_ERROR(("ape_midxbuld_get_info: get type %d, buffer is not enough (%d).\r\n",
                           (int)e_info_type, (int)*pz_buf_leng));
                return MIDXBULDR_INITIAL_ERR;
            }

            pt_key_tbl_info = (MIDXBULD_KEY_TBL_INFO_T*)pv_buf;
            pt_key_tbl_info->pt_key_tbl_ext = pt_ape_handler->pt_key_tbl_ext;
            
            break;
        default:
             return MIDXBULDR_INV_ARG;
    }

    return MIDXBULDR_OK;
}

/*-----------------------------------------------------------------------------
 * Name:  ape_midxbuld_set_info
 *
 * Description: This API retrieves the information of media idx builder handle. 
 *               It will return MINFO_NOT_READY, before parse media complete.
 *
 * Inputs:  pv_obj       specify the media info object  
 *          e_info_type  specify the information type of the media.
 *          pv_buf       specify the memory buffer which will contain the 
 *                        media information.
 *          pz_buf_leng  specify the length of the buffer.
 *
 * Outputs: pv_buf       specify the memory buffer which will contain the media 
 *                        information
 *          pz_buf_leng  If it is smaller than required, it will return 
 *                         MINFO_INSUFFICENT_MEM, and pz_buf_leng will contain 
 *                         required length.
 *
 * Returns: MINFOR_OK    Success. The routine was successful and the operation 
 *                       has been completed.
 *          Other non-zero values: Fail
 *
 * Note:  NULL
 *
 * Author : lianming lin(mcn07123)
 * 
 * History:
 *   (1)2008-10-29 : initial
 ----------------------------------------------------------------------------*/
INT32 ape_midxbuld_set_info(
                            VOID*                    pv_obj,   
                            MIDXBULD_SET_INFO_TYPE_T e_info_type,  
                            VOID*                    pv_buf,     
                            SIZE_T*                  pz_buf_leng)
{
    MIDXBULD_OBJ_T*     pt_midxbuld_obj;
    APE_IDXBULD_OBJ_T*  pt_ape_handler;
    
    /************************************************************************/
    /* Check parameter                                                      */
    /************************************************************************/
    if ((NULL == pv_obj) || (NULL == pz_buf_leng))
    {
        DBG_ERROR(("ape_midxbuld_set_info:NULL\r\n"));
        return MIDXBULDR_INITIAL_ERR;
    }
    
    /************************************************************************/
    /* do                                                                   */
    /************************************************************************/
    /*get index builder object*/
    pt_midxbuld_obj = (MIDXBULD_OBJ_T*) pv_obj;
    
    /*get ape index builder handler*/
    pt_ape_handler = (APE_IDXBULD_OBJ_T*)pt_midxbuld_obj->pv_handler_obj;
    if (NULL == pt_ape_handler) 
    {
        DBG_ERROR(("ape_midxbuld_set_info:pt_ape_handler NULL\r\n"));
        return MIDXBULDR_INITIAL_ERR;
    }
    
    switch (e_info_type)
    {
        case MIDXBULD_SET_INFO_TYPE_ALLOC_RANGE:
            /*The first stage do not come here,only support simple index object*/
            break;
        case MIDXBULD_SET_INFO_TYPE_FREE_RANGE:
            break;
        default:
            return MIDXBULDR_INV_ARG;
    }

    return MIDXBULDR_OK;
}

#ifdef __cplusplus
}
#endif
