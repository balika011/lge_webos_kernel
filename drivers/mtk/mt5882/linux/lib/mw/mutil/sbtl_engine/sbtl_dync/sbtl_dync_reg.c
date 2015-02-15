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
 * $RCSfile: sbtl_dync_wdgt.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision: $
 * $SWAuthor: $
 * $MD5HEX: $
 *
 * Description:
 *
 * History:
 *
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
#include "x_dbg.h"
#include "x_mm_common.h"
#include "x_uc_str.h"
#include "x_lnk_list.h"

#include "mutil/mm_util.h"

#include "mutil/sbtl_engine/sbtl_dbg.h"
#include "mutil/sbtl_engine/sbtl_struct.h"
#include "mutil/sbtl_engine/sbtl_utils.h"
#include "mutil/sbtl_engine/sbtl_register.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_state.h"
#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_reg.h"


#define MAX_DYNC_REG_TBL_NUM (6)

typedef struct
{
    MM_SBTL_TYPE_T        e_sbtl_type;
    SBTL_DYNC_REG_FUNC_T* pt_reg_table;
}SBTL_DYNC_REG_INFO_T;


SBTL_DYNC_REG_INFO_T t_dync_reg_tbl[MAX_DYNC_REG_TBL_NUM]= {};

INT32 sbtl_dyn_reg_parser(
    MM_SBTL_TYPE_T        e_sbtl_type,
    SBTL_DYNC_REG_FUNC_T* pt_fct_tbl)
{
    UINT32 ui4_i;
    UINT32 ui4_sel = MAX_DYNC_REG_TBL_NUM;
    if (e_sbtl_type > MM_SBTL_TYPE_NONE &&
        e_sbtl_type < MM_SBTL_TYPE_END &&
        pt_fct_tbl)
    {
    }
    else
    {
        return MMSBTLR_INV_ARG;
    }
    for (ui4_i = 0; ui4_i < MAX_DYNC_REG_TBL_NUM; ui4_i++)
    {
        if (t_dync_reg_tbl[ui4_i].e_sbtl_type == e_sbtl_type)
        {
            ui4_sel = ui4_i;
            break;
        }
        if (t_dync_reg_tbl[ui4_i].pt_reg_table == NULL)
        {
            if (ui4_sel == MAX_DYNC_REG_TBL_NUM)
            {
                ui4_sel = ui4_i;
            }
        }
    }
    if (ui4_sel < MAX_DYNC_REG_TBL_NUM)
    {
        t_dync_reg_tbl[ui4_sel].e_sbtl_type = e_sbtl_type;
        t_dync_reg_tbl[ui4_sel].pt_reg_table = pt_fct_tbl;
    }
    else
    {
        return MMSBTLR_NOT_ALLOW;
    }
    return MMSBTLR_OK;
}

SBTL_DYNC_REG_FUNC_T* sbtl_dync_mem_match_type(
    MM_SBTL_TYPE_T*          pe_sbtl_type,
    UINT8*                   pui1_data,
    UINT32                   ui4_len)
{
    UINT32         ui4_i = 0;
    
    if (pe_sbtl_type && pui1_data && ui4_len)
    {
        if (*pe_sbtl_type > MM_SBTL_TYPE_NONE &&
            *pe_sbtl_type < MM_SBTL_TYPE_END)
        {
            for (ui4_i = 0; ui4_i < MAX_DYNC_REG_TBL_NUM; ui4_i++)
            {
                if (t_dync_reg_tbl[ui4_i].e_sbtl_type == *pe_sbtl_type &&
                    t_dync_reg_tbl[ui4_i].pt_reg_table &&
                    t_dync_reg_tbl[ui4_i].pt_reg_table->pf_matched)
                {
                   if (t_dync_reg_tbl[ui4_i].pt_reg_table->pf_matched(pui1_data, ui4_len))
                   {
                        return t_dync_reg_tbl[ui4_i].pt_reg_table;
                   }
                   break;
                }
            }
        }

        for (ui4_i = 0; ui4_i < MAX_DYNC_REG_TBL_NUM; ui4_i++)
        {
            if (t_dync_reg_tbl[ui4_i].e_sbtl_type != *pe_sbtl_type &&
                t_dync_reg_tbl[ui4_i].pt_reg_table &&
                t_dync_reg_tbl[ui4_i].pt_reg_table->pf_matched)
            {
               if (t_dync_reg_tbl[ui4_i].pt_reg_table->pf_matched(pui1_data, ui4_len))
               {
                    *pe_sbtl_type = t_dync_reg_tbl[ui4_i].e_sbtl_type;
                    return t_dync_reg_tbl[ui4_i].pt_reg_table;
               }
            }
        } 
    }
    return NULL;
}


#endif /*__KERNEL__*/


