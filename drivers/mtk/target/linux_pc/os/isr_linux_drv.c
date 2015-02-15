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
 * $RCSfile: isr_linux_drv.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: 
 *         This header file contains handle specific definitions, which are
 *         exported.
 *---------------------------------------------------------------------------*/

/* driver-OS header files */
#include "x_start_common.h"
#include "driver_os.h"
#include "isr_linux_drv.h"

static ISR_VEC_T *pt_isr_vec = NULL;
static INT32 i4_ivec_entry;

extern void *malloc();
extern void free();


/*----------------------------------------------------------------------------
 * Function: x_os_drv_reg_isr()
 *
 * Description:
 *      this API registers an ISR with its vector id. it performs
 *      1. check is vector table is established.
 *      2. locate vector table entry from vector id.
 *      3. save the new ISR if the table entry is not occupied or is 
 *         occupied by the same vector, otherwise, search forward to
 *         find one until entire table is searched.
 *
 * Inputs:
 *      ui2_vec_id: an vector id to register an ISR.
 *      pf_isr: pointer to a ISR to set.
 *      ppf_old_isr: pointer to hold the current ISR setting.
 *
 * Outputs: -
 *
 * Returns:
 *      OSR_DRV_OK : routine was successful.
 *      OSR_DRV_NOT_INIT: invalid argument.
 *      OSR_DRV_FAIL: OS driver related error.
 *---------------------------------------------------------------------------*/
INT32
x_os_drv_reg_isr(UINT16             ui2_vec_id,
                 x_os_drv_isr_fct   pf_isr,
                 x_os_drv_isr_fct   *ppf_old_isr)
{
    CRIT_STATE_T t_crit;
    INT32 i4_index, i4_i, i4_ret;
    ISR_VEC_T *pt_tmp;

    if (pt_isr_vec == (ISR_VEC_T *) NULL)
    {
        return(OSR_DRV_NOT_INIT);
    }

    /* i4_index = ui2_vec_id & (ISR_VECTOR_SIZE - 1);   locate table entry */
    /* i4_i serves as the stopper for one round */
    /* i4_i =  (i4_index != 0) ? (i4_index - 1) : (ISR_VECTOR_SIZE - 1); */

    i4_ret = OSR_DRV_FAIL;

    /* locate table entry */
    i4_index = (INT32) (ui2_vec_id & (i4_ivec_entry - 1));
    i4_i =  (i4_index != 0) ? (i4_index - 1) : (i4_ivec_entry - 1);
    pt_tmp = (ISR_VEC_T *) (pt_isr_vec + i4_index);

    t_crit = x_os_drv_crit_start();

    do
    {
        /* find an empty one from i4_index and stops at i4_i */
        if ((pt_tmp->ui2_flag == ISR_FLAG_EMPTY) ||
            (pt_tmp->ui2_vec_id == ui2_vec_id))
        {
            *ppf_old_isr = pt_tmp->pf_isr;
            pt_tmp->pf_isr = pf_isr;
            pt_tmp->ui2_vec_id = ui2_vec_id;
            pt_tmp->ui2_flag = ISR_FLAG_USED;
            i4_ret = OSR_DRV_OK;
            break;
        }
        ++i4_index;
        ++pt_tmp;
    } while (i4_index != i4_i);

    x_os_drv_crit_end(t_crit);

    return(i4_ret);
}


/*----------------------------------------------------------------------------
 * Function: os_drv_isr_init()
 *
 * Description:
 *      this API registers an ISR with its vector id. it performs
 *      1. allocate vector table
 *      2. initialize each table entry.
 *
 * Inputs:
 *      i4_entry: vector table entry.
 *
 * Outputs: -
 *
 * Returns:
 *      OSR_DRV_OK : routine was successful.
 *      OSR_DRV_FAIL: OS driver related error.
 *---------------------------------------------------------------------------*/
INT32
os_drv_isr_init(INT32 i4_entry)
{
    INT32 i4_i;
    SIZE_T z_size;
    ISR_VEC_T *pt_tmp;

    if (pt_isr_vec != (ISR_VEC_T *) NULL)
    {
        free((VOID *) pt_isr_vec);
        pt_isr_vec = (ISR_VEC_T *) NULL;
    }

    i4_ivec_entry = (i4_entry > 0) ? i4_entry : ISR_VECTOR_SIZE;
    z_size = (SIZE_T) (sizeof(ISR_VEC_T) * i4_ivec_entry);
    pt_isr_vec = (ISR_VEC_T *) malloc(z_size);

    if (pt_isr_vec != (ISR_VEC_T *) NULL)
    {
        pt_tmp = pt_isr_vec;
        for (i4_i = 0; i4_i < i4_ivec_entry; i4_i++, pt_tmp++)
        {
            pt_tmp->pf_isr = NULL;
            pt_tmp->ui2_vec_id = (UINT16) 0;
            pt_tmp->ui2_flag = ISR_FLAG_EMPTY;
        }

        return(INITR_OK);
    }
    else
    {
        return(INITR_FAIL);
    }
}
