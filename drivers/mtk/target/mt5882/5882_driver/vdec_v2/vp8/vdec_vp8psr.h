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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp8psr.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp8psr.h
 *  This source file include vp8 decode flow and memory contral 
 */

 

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#ifndef VDEC_VP8_PSR_H
#define VDEC_VP8_PSR_H

#if defined(CC_MT5398) || defined(CC_MT5396)
#define VP8DEC_SUPPORT_HEIGH_RESOLUTION
#endif

#if defined(VP8DEC_SUPPORT_HEIGH_RESOLUTION)
#define VDEC_VP8_HWMAX_WIDTH 4080
#define VDEC_VP8_HWMAX_HEIGHT 4080
#else
#define VDEC_VP8_HWMAX_WIDTH 1920
#define VDEC_VP8_HWMAX_HEIGHT 1088
#endif

#if defined(VP8DEC_SUPPORT_HEIGH_RESOLUTION)
#define VDEC_VP8_WRAPPER_LIMIT_W 4080
#define VDEC_VP8_WRAPPER_LIMIT_H 4080
#else
#define VDEC_VP8_WRAPPER_LIMIT_W 1904
#define VDEC_VP8_WRAPPER_LIMIT_H 1904
#endif
// corba is 4080, viper is 1920x1088

#define VDEC_WEBP_MAX_WIDTH 16368
#define VDEC_WEBP_MAX_HEIGHT 16368

#define VDEC_VP8_HWMIN_WIDTH (1)
#define VDEC_VP8_HWMIN_HIGHT (1)
#define MULTI_SEGMENT_ID_BUFFER (((VDEC_VP8_HWMAX_HEIGHT+15)/16)*64)  // (4096/16) * 64
#ifdef VP8_SUPPORT_MULTIINSTANCE
#define VDEC_VP8_HWWORKBUF_SIZE MULTI_SEGMENT_ID_BUFFER
#else 
#define VDEC_VP8_HWWORKBUF_SIZE (0)
#endif

extern BOOL _VDEC_Vp8PsrGetEmptyFrameBuffer(VP8_DEC_PARAM_T *prDecParam);
#if 0
extern BOOL _VDEC_Vp8PsrInstanceChange(VDEC_VP8_INFO_T *prVpDecInfo,BOOL fgChg2Multi);
#endif
extern VOID _VDEC_Vp8PsrFlush(VDEC_VP8_INFO_T * prDecInfo);
extern BOOL _VDEC_Vp8PsrHeaderInfo(VDEC_VP8_INFO_T *pDecInfo);
extern BOOL _VDEC_Vp8PsrInit(VDEC_VP8_INFO_T *prVp6DecInfo,INT32 i4DecId);
extern BOOL _VDEC_Vp8PsrDecode(VDEC_VP8_INFO_T *pDecInfo);
extern BOOL _VDEC_Vp8PsrFin(VDEC_VP8_INFO_T *pDecInfo);
extern BOOL _VDEC_Vp8PsrGetPicInfo(VDEC_VP8_INFO_T *pDecInfo,UINT32 *pu4YAddr,UINT32 *pu4CAddr);
extern VOID _VDEC_Vp8PsrIsrInit(VDEC_VP8_INFO_T *prVpDecInfo);
#if 0
extern VOID _VDEC_Vp8PsrDumpReg(VDEC_VP8_INFO_T *prVpDecInfo);
#endif
extern BOOL _VDEC_Vp8PsrWebPDecode(VDEC_VP8_INFO_T *prVpDecInfo,UINT32 u4PicMode);
extern UINT32 _VDEC_Vp8PsrWebPProgress(VDEC_VP8_INFO_T *prVpDecInfo);
extern VOID _VDEC_Vp8PsrUpdateFrame(VDEC_VP8_INFO_T *prVdecInfo,BOOL fgDecResult);
#endif  // VDEC_VP8_PSR_H

