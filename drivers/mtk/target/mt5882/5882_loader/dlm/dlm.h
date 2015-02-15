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
 * $RCSfile: dlm.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision:  $
 * $SWAuthor:  $
 * $MD5HEX:  $
 *
 * Description:
 *---------------------------------------------------------------------------*/
#ifndef DLM_H
#define DLM_H

#define HDR_ENCRYPTED   1
#define DLM_HDR_MAGIC           "#DH@FiRm"
#define DLM_HDR_KEY             ((UINT32) (0x94102909))
#define DLM_HDR_IV              ((UINT32) (0x04BE7C75))
#define DLM_APPEND_HDR_MAGIC    "iMtK"
#define DLM_APPEND_HDR_MAGIC_SIZE  ((UINT32) (4))
#define DLM_MAX_PATH_LENGTH     ((UINT32) (1024))

#if (defined(DLM_USE_AES_MODE) || ((defined(LINUX_TURNKEY_SOLUTION)) && (!defined(CC_MTK_LOADER))))
#define DLM_APPEND_MAGIC            "reserved mtk inc"
#define APPEND_HEADER_SIZE          ((UINT32) (60))
#define DLM_CRYPT_BLOCK_SIZE        ((INT32) (16))
#define DLM_APPEND_HDR_MAGIC_RESERVE ((UINT32) (8))
#else
#define APPEND_HEADER_SIZE      ((UINT32) (12))
#define DLM_CRYPT_BLOCK_SIZE      ((INT32) (4))
#define DLM_APPEND_HDR_MAGIC_RESERVE ((UINT32) (0))
#endif
/* The offset from begin to our code file header */
#ifndef DLM_PLAIN_TEXT_OFFSET_PHILIPS
#define DLM_PLAIN_TEXT_OFFSET_PHILIPS   ((INT32) (64))
#endif

#define DLM_APPEND_DATA_IV      ((UINT32) (0x04BE7C72))





INT32  x_dlm_generate_code_file(DEFAULT_CODE_FILE_HDR_T *pt_hdr, Append_T *pt_append, CHAR *filename);
INT32  x_dlm_update_signature(CHAR *file, CHAR *p_sig);
INT32  x_dlm_generate_hash(CHAR *filename, UCHAR *digest);
VOID   dlm_dump_register_append_data(VOID);

#endif
