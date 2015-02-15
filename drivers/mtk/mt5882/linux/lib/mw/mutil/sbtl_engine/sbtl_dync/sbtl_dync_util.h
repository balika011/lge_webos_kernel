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

#ifndef _SBTL_DYNC_UTIL_H_
#define _SBTL_DYNC_UTIL_H_

#include "mutil/sbtl_engine/sbtl_dync/sbtl_dync_reg.h"
 
#define dwBigFourCC(a,b,c,d)        ((((UINT32)a)<<24)|((b)<<16)|((c)<<8)|(d))

#define MP4_BOX_FTYP                dwBigFourCC('f', 't', 'y', 'p')
#define MP4_BOX_MOOV                dwBigFourCC('m', 'o', 'o', 'v')
#define MP4_BOX_MVHD                dwBigFourCC('m', 'v', 'h', 'd')
#define MP4_BOX_TRAK                dwBigFourCC('t', 'r', 'a', 'k')
#define MP4_BOX_MVEX                dwBigFourCC('m', 'v', 'e', 'x')
#define MP4_BOX_MEHD                dwBigFourCC('m', 'e', 'h', 'd')
#define MP4_BOX_TREX                dwBigFourCC('t', 'r', 'e', 'x')
#define MP4_BOX_MOOF                dwBigFourCC('m', 'o', 'o', 'f')
#define MP4_BOX_MDAT                dwBigFourCC('m', 'd', 'a', 't')
#define MP4_BOX_MFRA                dwBigFourCC('m', 'f', 'r', 'a')
#define MP4_BOX_MFHD                dwBigFourCC('m', 'f', 'h', 'd')
#define MP4_BOX_TRAF                dwBigFourCC('t', 'r', 'a', 'f')
#define MP4_BOX_TFHD                dwBigFourCC('t', 'f', 'h', 'd')
#define MP4_BOX_TRUN                dwBigFourCC('t', 'r', 'u', 'n')    
#define MP4_BOX_SIDX                dwBigFourCC('s', 'i', 'd', 'x')  



UINT8* get_words(UINT8* pui1_s, UINT32 ui4_len, UINT32* pui4_w_len);
UINT32 get_png_start(UINT8* pui1_buf, UINT32 ui4_len);
INT32  parse_png(MM_SBTL_IMG_INFO_T* pt_img, UINT8* pui1_buf, UINT32 ui4_len);
INT32  sbtl_get_timecode(CHAR* ps_str,UINT32 *pui4_time);
BOOL   valid_string(UINT8* ps_str);
BOOL   check_file_valid(MM_SBTL_LOAD_FILE_INFO_T* pt_file, UINT32 ui4_cur_pts);
INT32  free_one_file(MM_SBTL_LOAD_FILE_INFO_T* pt_file);
MM_SBTL_FILE_STACK_T* alloc_one_file(
    MM_SBTL_LOAD_FILE_INFO_T* pt_file);

INT32 sbtl_base64_encode(CHAR* f, CHAR* t, INT32 l);
INT32 sbtl_base64_decode(CHAR* f, CHAR* t, INT32 l);
VOID sbtl_base64_chomp(CHAR* p);
UINT32 sbtl_cur_time(VOID);



#endif /* _SBTL_SRT_PARSER_H_ */


