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
 * $RCSfile: sbtl_dync_wdgt.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision: $
 * $SWAuthor: $
 * $MD5HEX: $
 *
 * Description: 
 *         
 *         
 *---------------------------------------------------------------------------*/

#ifndef _SBTL_DYNC_REG_H_
#define _SBTL_DYNC_REG_H_

#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_wdgt.h"


struct _SBTL_DYNC_REG_FUNC_T;
struct _MM_SBTL_LOAD_FILE_INFO_T;
typedef struct _SBTL_DYNC_REG_FUNC_T    SBTL_DYNC_REG_FUNC_T;
typedef struct _MM_SBTL_LOAD_FILE_INFO_T MM_SBTL_LOAD_FILE_INFO_T;



typedef struct
{
    UINT32 ui4_cur_pts;
    UINT32 ui4_start_pts;
    UINT32 ui4_end_pts;
    BOOL   b_found;
    VOID*  pv_data;
}SBTL_DYNC_GET_DATA_INFO_T;


typedef enum
{
    MM_SBTL_IMG_TYPE_NONE,
    MM_SBTL_IMG_TYPE_PNG
}MM_SBTL_IMG_TYPE;

typedef enum
{
    MM_SBTL_IMG_ENC_NONE,
    MM_SBTL_IMG_ENC_BASE64
}MM_SBTL_IMG_ENC_TYPE;


typedef struct
{
    MM_SBTL_IMG_TYPE     e_img_type;
    UINT8*               pui1_img_data;
    UINT32               ui4_img_size;
    UINT32               ui4_width;
    UINT32               ui4_height;
    HANDLE_T             h_img; 
    CHAR*                ps_image_id;
    MM_SBTL_IMG_ENC_TYPE e_encoding;
}MM_SBTL_IMG_INFO_T;

typedef struct
{
    UINT32           ui4_start_pts;
    UINT32           ui4_end_pts;
    UINT32           ui4_offset;
    UINT32           ui4_size;
}MM_SBTL_DYN_BASE_IDX_T;


typedef INT32 (*sbtl_dync_init_fct)(MM_SBTL_LOAD_FILE_INFO_T* pt_info);

typedef INT32 (*sbtl_dync_deinit_fct)(MM_SBTL_LOAD_FILE_INFO_T* pt_info);

typedef INT32 (*sbtl_dync_idx_build_fct)(MM_SBTL_LOAD_FILE_INFO_T* pt_info);

typedef INT32 (*sbtl_dync_get_data_fct)(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    SBTL_DYNC_GET_DATA_INFO_T* pt_data);

typedef INT32 (*sbtl_dync_parse_fct)(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT32                    ui4_idx);

typedef INT32 (*sbtl_dync_free_fct)(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT32                    ui4_idx);

typedef BOOL (*sbtl_dync_matched_fct)(
    UINT8*                    pui1_buf,
    UINT32                    ui4_len);

typedef INT32 (*sbtl_dync_show_fct)(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    BOOL                      b_show);

typedef INT32 (*sbtl_dync_set_fct)(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT32                    ui4_set_type,
    VOID*                     pv_set,
    SIZE_T                    z_set_size);

typedef INT32 (*sbtl_dync_get_fct)(
    MM_SBTL_LOAD_FILE_INFO_T* pt_info,
    UINT32                    ui4_get_type,
    VOID*                     pv_get,
    SIZE_T                    z_get_size);




struct _SBTL_DYNC_REG_FUNC_T
{
    sbtl_dync_matched_fct   pf_matched;
    sbtl_dync_init_fct      pf_init;
    sbtl_dync_deinit_fct    pf_deinit;
    sbtl_dync_get_data_fct  pf_get_data;
    sbtl_dync_idx_build_fct pf_build_idx;
    sbtl_dync_parse_fct     pf_parse;
    sbtl_dync_free_fct      pf_free;
    sbtl_dync_show_fct      pf_show;
    sbtl_dync_set_fct       pf_set;
    sbtl_dync_get_fct       pf_get;
    SBTL_COMM_TYPE_T        e_comm_type;
};



struct _MM_SBTL_LOAD_FILE_INFO_T
{
    
    CHAR*                 ps_file_path;  //<--
    UINT8*                pui1_file_buff;
    UINT32                ui4_file_size;
    UINT32                ui4_start_pts;
    UINT32                ui4_end_pts;
    SBTL_SHOW_TYPE_T      e_show_mode;  //-->basic file info
    
    HANDLE_T              h_sema;       //<--
    BOOL                  b_force_exit; //-->for protect and fast exit
    
    MEDIA_FEEDER_T        t_feeder;    //<--
    MM_SBTL_TYPE_T        e_sbtl_type;
    SBTL_DYNC_REG_FUNC_T* pt_reg_func;
    VOID*                 pv_private;  //--> matched type and related funcs, private data
    
    UINT32                ui4_sample_num;//<--
    VOID**                pv_samle_index;
    UINT32                ui4_cur_sample;//--> index table

    BOOL                  b_is_loading;
    BOOL                  b_has_loaded;//this flag means parse done or index build done
    
    SBTL_ENGINE_T*        pt_engine;   // get config values from engine
    HANDLE_T              h_widget;   //widget created
};

typedef struct _MM_SBTL_FILE_LIST_T
{
    MM_SBTL_LOAD_FILE_INFO_T     t_info;
    struct _MM_SBTL_FILE_LIST_T* pt_next;
}MM_SBTL_FILE_STACK_T;



INT32 sbtl_dyn_reg_parser(
    MM_SBTL_TYPE_T        e_sbtl_type,
    SBTL_DYNC_REG_FUNC_T* pt_fct_tbl);

SBTL_DYNC_REG_FUNC_T* sbtl_dync_mem_match_type(
    MM_SBTL_TYPE_T*          pe_sbtl_type,
    UINT8*                   pui1_data,
    UINT32                   ui4_len);


#endif /* _SBTL_SRT_PARSER_H_ */


