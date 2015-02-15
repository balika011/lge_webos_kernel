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
 * $RCSfile: img_slideshow.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: Photo slideshow library CLI
 *---------------------------------------------------------------------------*/

#include "x_img_slideshow.h"
#include "x_fm.h"
#include "u_cli.h"

static INT32 _img_cli_ss_init		(INT32 i4_argc, const CHAR** pps_argv);
static INT32 _img_cli_ss_start		(INT32 i4_argc, const CHAR** pps_argv);
static INT32 _img_cli_ss_stop		(INT32 i4_argc, const CHAR** pps_argv);

/* image library command table */
CLI_EXEC_T at_slideshow_cmd_tbl[] =
{
    {"init",				NULL,		_img_cli_ss_init,			NULL,		"Initial it",					CLI_SUPERVISOR},
    {"start",               NULL,       _img_cli_ss_start,        	NULL,       "Start slideshow",              CLI_SUPERVISOR},
    {"stop",                NULL,       _img_cli_ss_stop,        	NULL,       "Sop slideshow",                CLI_SUPERVISOR},

    END_OF_CLI_CMD_TBL
};


UCHAR	*src_Y=NULL, *src_C=NULL;
UCHAR	*dst_Y=NULL, *dst_C=NULL;
UCHAR	*cur_Y=NULL, *cur_C=NULL;
UINT32	gui4_buf_size = 720*480; //Y = C
IMG_SLIDE_SHOW_EFFECT_T e_effect = 0;
static UINT32	gui4_frame_idx = 1;

#define WIDTH		720
#define HEIGHT		480
#define TOTAL_TIME 	1000

#ifdef LINUX_TURNKEY_SOLUTION
#define SS_IMG_PATH_PREFIX     "/mnt/usb/sda1/720_480_422/"
#else
#define SS_IMG_PATH_PREFIX     "/mnt/usb/Mass-000/720_480_422/"
#endif


static INT32 _atoi( const CHAR* ps )
{
    INT32  i4_total;

    i4_total = 0;
    while ( *ps != 0 )
    {
        if ( *ps >= '0' && *ps <= '9' )
        {
            /* accumulate digit */
            i4_total = 10 * i4_total + (*ps - '0');
            ps++; /* get next char */
        }
        else /* not a digit char */
        {
            return (-1);
        }
    }

    return i4_total;
}

VOID _img_cli_slideshow_notify(SLIDESHOW_NFY_EVENT_TYPE_T	e_event)
{
    switch (e_event)
    {
        case SLIDESHOW_NFY_COMPLETE_ONE_FRAME:
        {
            CHAR    	ac_filename[64];
		    UINT32  	ui4_byte_write;
		    HANDLE_T 	h_file;
            INT32		i4_ret;

			//dst_Y
            //x_strcpy(ac_filename, SS_IMG_PATH_PREFIX);
            x_sprintf(ac_filename, "%s%d", SS_IMG_PATH_PREFIX, gui4_frame_idx);
            x_strcat(ac_filename, "dst_Y.raw");
            i4_ret = x_fm_open(FM_ROOT_HANDLE,
                				ac_filename,
                				FM_OPEN_CREATE | FM_READ_WRITE,
                				FM_MODE_PERM_MASK,
                				FALSE,
                				&h_file);
            if (i4_ret != FMR_OK)
            {
                DBG_ERROR(("[notify] Open file fail. (%d)\n", i4_ret));
                return;
            }
            i4_ret = x_fm_write(h_file,
                				dst_Y,
                				gui4_buf_size,
                				&ui4_byte_write);
            if (i4_ret != FMR_OK)
            {
                DBG_ERROR(("[notify] Write file fail. (%d)\n", i4_ret));
                return;
            }
            i4_ret = x_fm_close(h_file);
            if (i4_ret != FMR_OK)
            {
                DBG_ERROR(("[notify] Close file fail. (%d)\n", i4_ret));
                return;
            }

			//dst_C
            //x_strcpy(ac_filename, SS_IMG_PATH_PREFIX);
            x_sprintf(ac_filename, "%s%d", SS_IMG_PATH_PREFIX, gui4_frame_idx);
            x_strcat(ac_filename, "dst_C.raw");
            i4_ret = x_fm_open(FM_ROOT_HANDLE,
                				ac_filename,
                				FM_OPEN_CREATE | FM_READ_WRITE,
                				FM_MODE_PERM_MASK,
                				FALSE,
                				&h_file);
            if (i4_ret != FMR_OK)
            {
                DBG_ERROR(("[notify] Open file fail. (%d)\n", i4_ret));
                return;
            }
            i4_ret = x_fm_write(h_file,
                				dst_C,
                				gui4_buf_size,
                				&ui4_byte_write);
            if (i4_ret != FMR_OK)
            {
                DBG_ERROR(("[notify] Write file fail. (%d)\n", i4_ret));
                return;
            }
            i4_ret = x_fm_close(h_file);
            if (i4_ret != FMR_OK)
            {
                DBG_ERROR(("[notify] Close file fail. (%d)\n", i4_ret));
                return;
            }

            //exchange buffer
            UCHAR* tmp=NULL;
            tmp   = cur_Y;
            cur_Y = dst_Y;
            dst_Y = tmp;

            tmp   = cur_C;
            cur_C = dst_C;
            dst_C = tmp;

            gui4_frame_idx++;
            i4_ret = x_img_sync_slideshow(e_effect,
	        						src_Y,
	        						src_C,
	        						cur_Y,
	        						cur_C,
	        						dst_Y,
	        						dst_C,
	        						gui4_buf_size,
	        						gui4_buf_size,
	        						WIDTH,
	        						HEIGHT,
	        						TOTAL_TIME);
            if (i4_ret != SLIDESHOW_OK)
            {
                DBG_ERROR(("[Notify]Slideshow Fail.(%d)\n", i4_ret));
                return;
            }
            
            break;
        }
        case SLIDESHOW_NFY_COMPLETE_ONE_PIC:
        {
            x_img_stop_slideshow();
            x_mem_free(src_Y);
		    x_mem_free(src_C);
		    x_mem_free(dst_Y);
		    x_mem_free(dst_C);
		    x_mem_free(cur_Y);
		    x_mem_free(cur_C);
		    src_Y = NULL;
		    src_C = NULL;
		    dst_Y = NULL;
		    dst_C = NULL;
		    cur_Y = NULL;
		    cur_C = NULL; 
            gui4_frame_idx = 1;
            break;
        }
        case SLIDESHOW_NFY_STOP:
            break;
        default:
            break;
    }
}

img_slidshow_nfy_fct pf_nfy = _img_cli_slideshow_notify;

static INT32 _img_cli_ss_init(INT32 i4_argc, const CHAR** pps_argv)
{
    INT32		i4_ret;
    CHAR    	ac_filename[64];
    UINT32  	ui4_byte_read;
    HANDLE_T 	h_file;
    
    i4_ret = x_img_init_slideshow(pf_nfy);
    if (i4_ret != SLIDESHOW_OK)
    {
        DBG_ERROR(("Initial slideshow fail. (%d)\n", i4_ret));
        return i4_ret;
    }

	src_Y = (UCHAR*)x_mem_alloc(gui4_buf_size * sizeof(UCHAR));
    src_C = (UCHAR*)x_mem_alloc(gui4_buf_size * sizeof(UCHAR));
    dst_Y = (UCHAR*)x_mem_alloc(gui4_buf_size * sizeof(UCHAR));
    dst_C = (UCHAR*)x_mem_alloc(gui4_buf_size * sizeof(UCHAR));
    cur_Y = (UCHAR*)x_mem_alloc(gui4_buf_size * sizeof(UCHAR));
    cur_C = (UCHAR*)x_mem_alloc(gui4_buf_size * sizeof(UCHAR));
    if (src_Y == NULL || src_C == NULL ||
        dst_Y == NULL || dst_C == NULL ||
        cur_Y == NULL || cur_C == NULL)
    {
        if (src_Y != NULL)
            x_mem_free(src_Y);
        if (src_C != NULL)
            x_mem_free(src_C);
        if (dst_Y != NULL)
            x_mem_free(dst_Y);
        if (dst_C != NULL)
            x_mem_free(dst_C);
        if (cur_Y != NULL)
            x_mem_free(cur_Y);
        if (cur_C != NULL)
            x_mem_free(cur_C);
        return SLIDESHOW_CLI_OUT_OF_MEMORY;
    }
    x_memset(src_Y, 0, gui4_buf_size);
    x_memset(src_C, 0, gui4_buf_size);
    x_memset(dst_Y, 0, gui4_buf_size);
    x_memset(dst_C, 0, gui4_buf_size);
    x_memset(cur_Y, 0, gui4_buf_size);
    x_memset(cur_C, 0, gui4_buf_size);

    //Y.raw
    x_strcpy(ac_filename,  SS_IMG_PATH_PREFIX);
    x_strcat(ac_filename, "Y.raw");

    i4_ret = x_fm_open(FM_ROOT_HANDLE,
        				ac_filename,
        				FM_READ_WRITE,
        				FM_MODE_PERM_MASK,
        				FALSE,
        				&h_file);
    if (i4_ret != FMR_OK)
    {
        DBG_ERROR(("File open fail.(%d)\n", i4_ret));
       goto EXIT;
    }

    i4_ret = x_fm_read(h_file,
        				src_Y,
        				gui4_buf_size,
        				&ui4_byte_read);
    if (i4_ret != FMR_OK || ui4_byte_read != gui4_buf_size)
    {
        DBG_ERROR(("File read fail.(%d)\n", i4_ret));
        goto EXIT;
    }

    i4_ret = x_fm_close(h_file);
    if (i4_ret != FMR_OK)
    {
        DBG_ERROR(("File close fail.(%d)\n", i4_ret));
        goto EXIT;
    }

	//C.raw
    x_strcpy(ac_filename,  SS_IMG_PATH_PREFIX);
    x_strcat(ac_filename, "C.raw");

    i4_ret = x_fm_open(FM_ROOT_HANDLE,
        				ac_filename,
        				FM_READ_WRITE,
        				FM_MODE_PERM_MASK,
        				FALSE,
        				&h_file);
    if (i4_ret != FMR_OK)
    {
        DBG_ERROR(("File open fail.(%d)\n", i4_ret));
        goto EXIT;
    }

    i4_ret = x_fm_read(h_file,
        				src_C,
        				gui4_buf_size,
        				&ui4_byte_read);
    if (i4_ret != FMR_OK || ui4_byte_read != gui4_buf_size)
    {
        DBG_ERROR(("File read fail.(%d)\n", i4_ret));    
        goto EXIT;
    }

    i4_ret = x_fm_close(h_file);
    if (i4_ret != FMR_OK)
    {
        DBG_ERROR(("File close fail.(%d)\n", i4_ret));
        goto EXIT;
    }
    
	return SLIDESHOW_OK;
    
EXIT:
    x_mem_free(src_Y);
    x_mem_free(src_C);
    x_mem_free(dst_Y);
    x_mem_free(dst_C);
    x_mem_free(cur_Y);
    x_mem_free(cur_C);
    src_Y = NULL;
    src_C = NULL;
    dst_Y = NULL;
    dst_C = NULL;
    cur_Y = NULL;
    cur_C = NULL;
    return i4_ret;
}

static INT32 _img_cli_ss_start	(INT32 i4_argc, const CHAR** pps_argv)
{
    INT32	i4_type;
    INT32	i4_ret;

    if (i4_argc != 2)
    {
        DBG_ERROR(("CLI format: ss type(0-59)\n"));
        return SLIDESHOW_CLI_INV_ARG;
    }

    i4_type = _atoi(pps_argv[1]);
    if (i4_type >= IMG_SLIDE_SHOW_EFFECT_MAX || i4_type < 0)
    {
        DBG_ERROR(("The type value is invalid. (0-59)\n"));
        return SLIDESHOW_CLI_INV_ARG;
    }
	e_effect = (IMG_SLIDE_SHOW_EFFECT_T)i4_type;
    
    i4_ret = x_img_async_slideshow(	e_effect,
        						src_Y,
        						src_C,
        						cur_Y,
        						cur_C,
        						dst_Y,
        						dst_C,
        						gui4_buf_size,
        						gui4_buf_size,
        						WIDTH,
        						HEIGHT,
        						TOTAL_TIME);
    if (i4_ret != SLIDESHOW_OK)
    {
        DBG_ERROR(("Slideshow fail. type = %d. (%d)\n", i4_type, i4_ret));
    }
    
    return i4_ret;
}


static INT32 _img_cli_ss_stop(INT32 i4_argc, const CHAR** pps_argv)
{
    if (src_Y != NULL)
        x_mem_free(src_Y);
    if (src_C != NULL)
        x_mem_free(src_C);
	if (cur_Y != NULL)
        x_mem_free(cur_Y);
    if (cur_C != NULL)
        x_mem_free(cur_C);
    if (dst_Y != NULL)
        x_mem_free(dst_Y);
    if (dst_C != NULL)
        x_mem_free(dst_C);
    
    return x_img_stop_slideshow();
}





