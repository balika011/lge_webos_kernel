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
 * $RCSfile: start_rtos.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: 
 *         This file contains all RTOS startup implementations.
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/

#include "x_common.h"
#include "x_start_common.h"
#if 1 /* MHF Linux - Jun Zuo */
#include "mem_linux_drv.h"
#endif

extern INT32 os_drv_isr_init(INT32);
extern INT32 os_drv_crit_init(VOID);
extern INT32 os_drv_thread_init(VOID);
extern INT32 os_drv_create_tick_thread(VOID);

#if 1 /* MHF Linux - Jun Zuo */
typedef VOID * (* os_mem_alloc_fct)(VOID *pt_part, SIZE_T z_size);
typedef VOID (* os_mem_free_fct)(VOID *pt_part, VOID *pv_mem);
typedef VOID * (* os_mem_realloc_fct)(VOID* pt_part, VOID* pv_ptr, SIZE_T z_size);
typedef VOID * (* os_mem_calloc_fct)(VOID* pt_part, UINT32 ui4_count, SIZE_T z_size);

extern VOID os_init_def_part_ops (os_mem_alloc_fct        pf_alloc,
                           os_mem_realloc_fct      pf_realloc,
                           os_mem_calloc_fct       pf_calloc,
                           os_mem_free_fct         pf_free);
#endif

static BOOL b_rtos_init = FALSE;
/*-----------------------------------------------------------------------------
 * Name: x_rtos_init
 *
 * Description: This API initializes the RTOS. It performs
 *              1. initilize critical section protection mechanism. 
 *              2. initilize ISR vector table.
 *              3. initilize thread wait semaphore.
 *
 * Inputs:  pt_config  References the RTOS configuration structure.
 *
 * Outputs: -
 *
 * Returns:
 *      INITR_OK : this routine was successful.
 *      INITR_FAIL: this routine failed.
 ----------------------------------------------------------------------------*/
INT32 x_rtos_init (GEN_CONFIG_T*  pt_config)
{
    INT32 i4_ventry = 0;
    INT32 i4_ret;

    if (b_rtos_init)
    {
        return INITR_OK;
    }

    if (pt_config != NULL)
    {
        /* TBD, none for the time being */
    }

#ifdef LINUX_MEM_SOLUTION /* MHF Linux - Jun Zuo */
    /* init default part operations */
    os_init_def_part_ops (os_linux_mem_alloc,
                         os_linux_mem_realloc,
                         os_linux_mem_calloc,
                         os_linux_mem_free);
#endif
    /* init critical section */
    i4_ret = os_drv_crit_init();
    if (i4_ret != INITR_OK)
    {
        return(INITR_FAIL);
    }

    /* init vector table */
    i4_ret = os_drv_isr_init(i4_ventry);
    if (i4_ret != INITR_OK)
    {
        return(INITR_FAIL);
    }

    /* init thread wait semaphore */
    i4_ret = os_drv_thread_init();
    if (i4_ret != INITR_OK)
    {
        return(INITR_FAIL);
    }

    /* start the timer thread */
    i4_ret = os_drv_create_tick_thread();
    if (i4_ret != INITR_OK)
    {
        return(INITR_FAIL);
    }

    b_rtos_init = TRUE;

    return (INITR_OK);
}
