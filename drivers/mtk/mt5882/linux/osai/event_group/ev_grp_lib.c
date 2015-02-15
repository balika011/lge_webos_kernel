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
 * $RCSfile: ev_grp_lib.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description:
 *         This header file contains handle specific definitions, which are
 *         exported.
 *---------------------------------------------------------------------------*/


#include "os.h"
#include "x_linux.h"
#include "x_hal_arm.h"
#include "../thread/thread_lib.h"
#include "../timer/timer_lib.h"
#include "../msgQ/msgq_lib.h"
#include "ev_grp_lib.h"


OS_EV_GRP_T     *pt_event_group_head = NULL;
BOOL            b_event_group_inited = FALSE;


/*----------------------------------------------------------------------------
 * Function: ev_grp_init()
 *
 * Description:
 *      marks event group module as being initialized
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 *      OSR_OK : routine was successful.
 *---------------------------------------------------------------------------*/
INT32
ev_grp_init()
{
    b_event_group_inited = TRUE;
    return(OSR_OK);
}


/*----------------------------------------------------------------------------
 * Function: ev_grp_find_obj()
 *
 * Description:
 *      look for an event group from an existing list using its name
 *
 * Inputs:
 *      pt_name: name of an event group
 *
 * Outputs: -
 *
 * Returns:
 *      NULL : find failed
 *      non-NULL : reference to an event group
 *---------------------------------------------------------------------------*/
OS_EV_GRP_T *
ev_grp_find_obj(const CHAR     *ps_name)
{
    OS_EV_GRP_T *pt_tmp;


    pt_tmp = pt_event_group_head;
    while (pt_tmp != NULL)
    {
        if (x_strncmp(pt_tmp->s_name, ps_name, EV_GRP_NAME_LEN) == 0)
        {
            break;
        }

        pt_tmp = pt_tmp->pt_next;
    }

    return(pt_tmp);
}


/*----------------------------------------------------------------------------
 * Function: ev_grp_remove_obj()
 *
 * Description:
 *      this routine called to get a msg Q struct by its name. it performs
 *      1. check if msg Q mudule is initiated.
 *      2. guard global msg Q link list.
 *      3. go through global msg Q link list to look for a msg Q based on
 *         "ps_name", up to 16 characters.
 *
 * Inputs:
 *      pt_ev_grp: references an event group
 *
 * Outputs: -
 *
 * Returns:
 *      OSR_OR: routine is OK
 *      OSR_NOT_INIT : event group has not been initiated
 *---------------------------------------------------------------------------*/
INT32
ev_grp_remove_obj(OS_EV_GRP_T   *pt_ev_grp)
{
    OS_EV_GRP_T *pt_tmp, *pt_prev;
    INT32 i4_ret;


    if (b_event_group_inited != TRUE)
    {
        return(OSR_NOT_INIT);
    }

    i4_ret = OSR_OK;
    pt_prev = NULL;
    pt_tmp = pt_event_group_head;
    while (pt_tmp != NULL)
    {
        if (pt_tmp == pt_ev_grp)
        {
            if (pt_prev == NULL)
            {
                pt_event_group_head = pt_tmp->pt_next;
            }
            else
            {
                pt_prev->pt_next = pt_tmp->pt_next;
            }

            break;
        }

        pt_prev = pt_tmp;
        pt_tmp = pt_tmp->pt_next;
    }

    return(i4_ret);
}


/*----------------------------------------------------------------------------
 * Function: ev_group_wait_event()
 *
 * Description:
 *      an common routine for API x_ev_group_wait_event() and
 *      x_ev_group_wait_event_timeout()
 *
 * Inputs:
 *      h_hdl: handle of an event group to memory buffer.
 *      e_events_req: requested events to be received
 *      e_op: operation over "e_events_req".
 *            one of X_EV_OP_AND, X_EV_OP_AND_CONSUME, X_EV_OP_OR,
 *            X_EV_OP_OR_CONSUME
 *      ui4_time: time delay in milliseconds before bailing out
 *
 * Outputs:
 *      pe_events_got: actual events received
 *
 * Returns:
 *      OSR_OK : routine was successful.
 *      OSR_NOT_INIT : event group has not been initiated.
 *      OSR_INV_ARG: an error in the arguments of this API.
 *      OSR_INV_HANDLE: an error in handle operation.
 *      OSR_NOT_EXIST: error with OS thread.
 *      OSR_DELETED: event group has been deleted.
 *      OSR_TIMEOUT: bail out, time delay has passed.
 *---------------------------------------------------------------------------*/
INT32
ev_group_wait_event(HANDLE_T              h_hdl,
                    EV_GRP_EVENT_T        e_events_req,
                    EV_GRP_EVENT_T        *pe_events_got,
                    EV_GRP_OPERATION_T    e_op,
                    UINT32                ui4_timeout)
{
    OS_EV_GRP_T *pt_ev_grp;
    OS_THREAD_T *pt_th, *pt_prev, *pt_tmp;
    VOID *pv_th_id;
    UINT32 ui4_elapsed;
    INT32 i4_ret;
    EV_GRP_EVENT_T e_ev;
    CRIT_STATE_T t_crit;
    BOOL b_match, b_loop;


    if (b_event_group_inited != TRUE)
    {
        return(OSR_NOT_INIT);
    }

    if (((e_op != X_EV_OP_AND) && (e_op != X_EV_OP_OR) &&
         (e_op != X_EV_OP_AND_CONSUME) && (e_op != X_EV_OP_OR_CONSUME)) ||
        (pe_events_got == NULL))
    {
        return(OSR_INV_ARG);
    }

    i4_ret = os_check_handle(h_hdl,
                             HT_GROUP_OS_EVENT_GROUP,
                             (VOID *) &pt_ev_grp);
    if (i4_ret != OSR_OK)
    {
        return(OSR_INV_HANDLE);
    }

    (VOID) x_os_drv_thread_self(&pv_th_id, ((VOID**) &pt_th));
    if (pt_th == NULL)
    {
        return(OSR_NOT_EXIST);
    }

    pt_th->pt_ev_grp_waiting = NULL;

    i4_ret = OSR_OK;
    b_match = FALSE;
    b_loop = FALSE;
    ui4_elapsed = (UINT32) 0;
    *pe_events_got = (EV_GRP_EVENT_T) 0;
    pt_th->e_ev_grp_event_got = (EV_GRP_EVENT_T) 0;
    pt_th->e_ev_grp_wait_op = e_op;
    pt_th->e_ev_grp_event_wait = e_events_req;

    t_crit = x_crit_start();
    do
    {
        if ((b_loop == TRUE) && (i4_ret == OSR_TIMEOUT))
        {
            /* timeout occurred, dequeue this thread */
            pt_prev = NULL;
            pt_tmp = pt_ev_grp->pt_first_waiting;
            while (pt_tmp != NULL)
            {
                if (pt_tmp == pt_th)
                {
                    if (pt_prev == NULL)
                    {
                        pt_ev_grp->pt_first_waiting = pt_tmp->pt_ev_grp_waiting;
                    }
                    else
                    {
                        pt_prev->pt_ev_grp_waiting = pt_tmp->pt_ev_grp_waiting;
                    }

                    break;
                }

                pt_prev = pt_tmp;
                pt_tmp = pt_tmp->pt_ev_grp_waiting;
            }
        }

        e_ev = pt_ev_grp->e_cur_events & e_events_req;

        if ((e_op == X_EV_OP_AND) || (e_op == X_EV_OP_AND_CONSUME))
        {
            if (e_ev == e_events_req)
            {
                b_match = TRUE;
            }
        }
        else    /* X_EV_OP_OR || X_EV_OP_OR_CONSUME */
        {
            if (e_ev != (EV_GRP_EVENT_T) 0)
            {
                b_match = TRUE;
            }
        }

        if (b_match != TRUE)
        {
            if ((ui4_timeout == (UINT32) 0) || (ui4_elapsed < ui4_timeout))
            {
                /* enqueue this thread on WAIT option or */
                /* timeout has not occurred */
                pt_prev = NULL;
                pt_tmp = pt_ev_grp->pt_first_waiting;
                while (pt_tmp != NULL)
                {
                    if (pt_th->ui1_priority < pt_tmp->ui1_priority)
                    {
                        break;
                    }

                    pt_prev = pt_tmp;
                    pt_tmp = pt_tmp->pt_ev_grp_waiting;
                }

                pt_th->pt_ev_grp_waiting = pt_tmp;
                if (pt_prev == NULL)
                {
                    pt_ev_grp->pt_first_waiting = pt_th;
                }
                else
                {
                    pt_prev->pt_ev_grp_waiting = pt_th;
                }
            }

            if (ui4_timeout == (UINT32) 0xffffffff)      /* WAIT */
            {
                pt_ev_grp->ui1_num_waiting++;
                x_crit_end(t_crit);

                (VOID) x_sema_lock(pt_th->h_sema_hdl, X_SEMA_OPTION_WAIT);
                b_loop = TRUE;

                /* bail out if event group no longer exists */
                /* otherwise, make sure wait_req is met */
                if (!(pt_th->ui1_flag & FLAG_TH_EV_GRP_CLOSE))
                {
                    t_crit = x_crit_start();
                    pt_ev_grp->ui1_num_waiting--;
                    if (pt_th->e_ev_grp_event_got != (EV_GRP_EVENT_T) 0)
                    {
                        b_match = TRUE;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (ui4_elapsed < ui4_timeout)
                {
                    ui4_elapsed += ui4_timeout;
                    pt_ev_grp->ui1_num_waiting++;
                    x_crit_end(t_crit);

                    i4_ret = x_sema_lock_timeout(pt_th->h_sema_hdl, ui4_timeout);
                    b_loop = TRUE;

                    /* bail out if event group no longer exists */
                    /* otherwise, make sure wait_req is met */
                    if (!(pt_th->ui1_flag & FLAG_TH_EV_GRP_CLOSE))
                    {
                        t_crit = x_crit_start();
                        pt_ev_grp->ui1_num_waiting--;
                        if (pt_th->e_ev_grp_event_got != (EV_GRP_EVENT_T) 0)
                        {
                            b_match = TRUE;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    i4_ret = OSR_TIMEOUT;
#if 0
                    if (!(pt_th->ui1_flag & FLAG_TH_EV_GRP_CLOSE))
                    {
                        t_crit = x_crit_start();
                        pt_ev_grp->ui1_num_waiting--;
                        e_ev = pt_ev_grp->e_cur_events & e_events_req;
                    }
#endif

                    break;
                }
            }
        }
        else
        {
            pt_th->e_ev_grp_event_got = e_ev;
            if ((e_op == X_EV_OP_OR_CONSUME) || (e_op == X_EV_OP_AND_CONSUME))
            {
                pt_ev_grp->e_cur_events &= ~e_ev;
            }
            break;
        }
    } while (TRUE);

    if (!(pt_th->ui1_flag & FLAG_TH_EV_GRP_CLOSE))
    {
        if (b_match == TRUE)
        {
            *pe_events_got = pt_th->e_ev_grp_event_got;
        }
        else
        {
            pt_th->e_ev_grp_event_got = (EV_GRP_EVENT_T) 0;
            *pe_events_got = (EV_GRP_EVENT_T) 0;
        }

        x_crit_end(t_crit);
    }
    else
    {
        i4_ret = OSR_DELETED;
        pt_th->ui1_flag &= ~FLAG_TH_EV_GRP_CLOSE;
        pt_th->e_ev_grp_event_got = (EV_GRP_EVENT_T) 0;
        *pe_events_got = (EV_GRP_EVENT_T) 0;
    }

    return(i4_ret);
}


/*----------------------------------------------------------------------------
 * Function: ev_grp_free_cb()
 *
 * Description:
 *      free routine to free a handle object
 *
 * Inputs:
 *      h_hdl : handle.
 *      e_type : handle type.
 *      pv_obj : handle object.
 *      pv_tag : handle private tag.
 *      b_req_hdl : TRUE or FALSE.
 *
 * Outputs: -
 *
 * Returns:
 *      TRUE : object is freed
 *      FALSE : object is not freed
 *---------------------------------------------------------------------------*/
BOOL
ev_grp_free_cb(HANDLE_T       h_hdl,
               HANDLE_TYPE_T  e_type,
               VOID *         pv_obj,
               VOID *         pv_tag,
               BOOL           b_req_hdl)
{
    OS_EV_GRP_T *pt_ev_grp;
    OS_THREAD_T *pt_th, *pt_th_tmp;
    CRIT_STATE_T t_crit;


    if (e_type != HT_GROUP_OS_EVENT_GROUP)
    {
        return(FALSE);
    }

    pt_ev_grp = (OS_EV_GRP_T *) pv_obj;

    t_crit = x_crit_start();
    if (pt_ev_grp->ui1_flags & EV_GRP_FLAG_CLOSE)
    {
        pt_ev_grp->ui1_flags &= ~EV_GRP_FLAG_CLOSE;
        (VOID) handle_delink(&(pt_ev_grp->t_hdl_link), h_hdl);

        if (pt_ev_grp->ui1_refcount != (UINT8) 0)
        {
            pt_ev_grp->ui1_refcount--;

            if (pt_ev_grp->ui1_refcount == (UINT8) 0)
            {
                (VOID) ev_grp_remove_obj(pt_ev_grp);

                pt_th = pt_ev_grp->pt_first_waiting;
                pt_ev_grp->pt_first_waiting = NULL;
                while (pt_th != NULL)
                {
                    pt_th_tmp = pt_th->pt_ev_grp_waiting;
                    pt_th->pt_ev_grp_waiting = NULL;
                    pt_th->ui1_flag |= FLAG_TH_EV_GRP_CLOSE;

                    (VOID) x_sema_unlock(pt_th->h_sema_hdl);

                    pt_th = pt_th_tmp;
                }

                x_mem_free(pt_ev_grp);
            }
        }

        x_crit_end(t_crit);
        return(TRUE);
    }
    else
    {
        x_crit_end(t_crit);
        return(FALSE);
    }
}
