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

#ifndef _SBTL_DYNC_WIDGET_H_
#define _SBTL_DYNC_WIDGET_H_

typedef enum
{
    SBTL_COMM_TEXT,
    SBTL_COMM_IPCC,
    SBTL_COMM_IMG_RGBA,
    SBTL_COMM_IMG_FILE
}SBTL_COMM_TYPE_T;

typedef struct
{
    UINT16 ui2_vid_w;
    UINT16 ui2_vid_h;
    UINT16 ui2_log_w;
    UINT32 ui4_log_h;
}SBTL_ARGB_WDGT_INIT_PARAM_T;

typedef struct
{
    SBTL_COMM_TYPE_T e_comm_type;
    HANDLE_T         h_plane;
    GL_RECT_T        t_rect;
    BOOL             b_default;

    union
    {
        SBTL_ARGB_WDGT_INIT_PARAM_T t_argb;
        //SBTL_XML_COMMAND_T          t_ipcc_cmd[10];
    }u_param;
}SBTL_COMM_WDGT_INIT_PARAM_T;

typedef enum
{
    SBTL_COMM_ATTR_NONE = 0,
    SBTL_COMM_FONT_SIZE,
    SBTL_COMM_FONT_ALIGN,
    SBTL_COMM_FONT_INFO,
    SBTL_COMM_TEXT_ALIGN,
    SBTL_COMM_FONT_STYLE,
    SBTL_COMM_BK_FG_COLOR,
    SBTL_COMM_BK_BG_COLOR,
    SBTL_COMM_FONT_FG_COLOR,
    SBTL_COMM_FONT_BG_COLOR,
    SBTL_COMM_FONT_EDGE_COLOR,
    SBTL_COMM_TEXT_CONT,
    SBTL_COMM_TEXT_RECT,
    SBTL_COMM_ARGB_DATA
}SBTL_COMM_ATTR_TYPE_T;

typedef struct _SBTL_COMM_ATTR_LIST
{
    SBTL_COMM_ATTR_TYPE_T        e_comm_attr;
    VOID*                        pv_attr_value;
    SIZE_T                       z_attr_size;
    struct _SBTL_COMM_ATTR_LIST* pt_next_attr;   
}SBTL_COMM_ATTR_LIST;


typedef struct _SBTL_ARGB_INFO_LIST_T
{
    UINT8  ui1_clr_mode;
    UINT32 ui4_x;
    UINT32 ui4_y;
    UINT32 ui4_w;
    UINT32 ui4_h;
    UINT8* pui1_pal_data;
    UINT16 ui2_pal_len;
    UINT8* pui1_img_data;
    UINT32 ui4_img_len;
    struct _SBTL_ARGB_INFO_LIST_T* pt_next;
}SBTL_ARGB_INFO_LIST_T;

INT32 sbtl_comm_set_attr(HANDLE_T h_widget, SBTL_COMM_ATTR_LIST* pt_list);
INT32 sbtl_comm_get_attr(HANDLE_T h_widget, SBTL_COMM_ATTR_LIST* pt_list);
INT32 sbtl_comm_create_widget(
    SBTL_COMM_WDGT_INIT_PARAM_T* pt_param,
    HANDLE_T*                    ph_widget);

INT32 sbtl_comm_destroy_widget(HANDLE_T h_widget);

INT32 sbtl_comm_show( HANDLE_T h_widget);
INT32 sbtl_comm_hide( HANDLE_T h_widget);

INT32 sbtl_comm_calc_text_rect(
    SBTL_ENGINE_T*            pt_engine,
    GL_RECT_T*                pt_rect);

INT32 sbtl_comm_create_text_wdgt(
    SBTL_ENGINE_T*            pt_engine,
    HANDLE_T*                 ph_widget);


#endif /* _SBTL_SRT_PARSER_H_ */


