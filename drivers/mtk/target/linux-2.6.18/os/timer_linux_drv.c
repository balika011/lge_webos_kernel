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
 * $RCSfile: timer_linux_drv.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *         This header file contains handle specific definitions, which are
 *         exported.
 *---------------------------------------------------------------------------*/

/* Middleware header files */
#define __need_timespec

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>

#ifndef ANDROID_TOOLCHAIN
#include <sys/shm.h>
#else
#include <linux/shm.h>
#endif

#include "ioctl_param.h"
#include "x_start_common.h"
#include "driver_os.h"
#include "sema_linux_drv.h"
#include "thread_linux_drv.h"
#include "timer_linux_drv.h"

static INT32 i4_fdioctrl;
/* extern int errno; */

/*----------------------------------------------------------------------------
 * Function: x_os_drv_get_tick_period()
 *
 * Description:
 *      this API returns tick's resolution is millisecond (ms). it performs
 *      1. get underlying OS HZ definition.
 *      2. round up resolution.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 *      tick resolution in millisecond (ms).
 *---------------------------------------------------------------------------*/
UINT32
x_os_drv_get_tick_period()
{
    UINT32 ui4_res;
    long hz;
    
    hz = sysconf(_SC_CLK_TCK);
    ui4_res = 1000/hz;

    return(ui4_res);
}


/*----------------------------------------------------------------------------
 * Function: x_os_drv_get_fine_tick
 *
 * Description:
 *      This function should return the current tick value of hardware timer
 *      running at bus clock speed. However, this target does not support
 *      this feature.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 *      Always '0'.
 *---------------------------------------------------------------------------*/
UINT32
x_os_drv_get_fine_tick(VOID)
{
    UINT32 ui4_res;
    struct tms t_tms;
    ui4_res = times(&t_tms);

    return (ui4_res);
}


/*----------------------------------------------------------------------------
 * Function: os_drv_get_tick_thread()
 *
 * Description:
 *      this API provides a wrapper to start a tick thread which is running in
 *      place of a system tick ISR. it performs
 *      1. call OS driver thread create API.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns: -
 *---------------------------------------------------------------------------*/
INT32
os_drv_create_tick_thread()
{
    VOID *pv_tick_th_id;
    INT32 i4_ret;
    
    i4_fdioctrl=open("/dev/rmmgr",O_RDWR);

    i4_ret = x_os_drv_thread_create(&pv_tick_th_id,
                                    "drv_tick",
                                    OS_DRV_THREAD_STACKSIZE,   /* stacksize */
                                    OS_DRV_TICK_THREAD_PRI,    /* thread pri */
                                    0,
                                    NULL,
                                    (x_os_drv_thread_main_fct) os_drv_tick_thread);
    if (i4_ret)
    {
        return(i4_ret);
    }

    return(INITR_OK);
}


/*----------------------------------------------------------------------------
 * Function: os_drv_tick_thread()
 *
 * Description:
 *      this API simulates a system tick ISR. it performs
 *      1. delay 1 tick.
 *      2. call middleware tick function.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns: -
 *---------------------------------------------------------------------------*/
VOID
os_drv_tick_thread()
{
    struct timespec req;
    struct timespec rem;
    struct timespec* a;
    struct timespec* b;
    
    UINT32  ui4_res;
    extern int nanosleep();
    UINT32  ui4_tick;
    struct tms t_tms;
    long hz;

    ui4_res = x_os_drv_get_tick_period();    /* in ms */
        
    hz = sysconf(_SC_CLK_TCK);

    if (ui4_res >= 1000)
    {
        printf("\n\nWARNING!!! ui4_res no less than 1000, which will cause nanosleep failed.\n");
    }
    
    ui4_tick = times(&t_tms);
    do
    {
        a = &req;
        b = &rem;

        req.tv_sec = 0;                     /* tick won't have sec. resolution */
        req.tv_nsec = ui4_res * 1000000;    /* in nanosecond */

        while (nanosleep(a, b) && errno == EINTR)
        {
            struct timespec* tmp = a;
            a = b;
            b = tmp;
        }
        
        /*jiffies start form 0xfffb6c20 not 0, so we need to handle overflow issue*/
        while (ui4_tick!=times(&t_tms))
        {
            ui4_tick++;
            x_os_drv_tick();
        }
    } while (1);
}


VOID x_os_drv_set_timestamp(CHAR *szString)
{
    ioctl(i4_fdioctrl, IOCTL_SET_TIMESTAMP, szString);
}

