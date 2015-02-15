/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright(c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp6.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp6psr.h
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#ifndef VDEC_VP6_SWPSR_H
#define VDEC_VP6_SWPSR_H

#define VDEC_VP6_SWMAX_WIDTH 640
#define VDEC_VP6_SWMAX_HEIGHT 480
#define VDEC_VP6_FBM_WIDTH  (VDEC_VP6_SWMAX_WIDTH+STRIDE_EXTRA)
#define VDEC_VP6_FBM_HEIGHT (VDEC_VP6_SWMAX_HEIGHT+STRIDE_EXTRA)
#define VDEC_VP6_SWWORKBUF_SIZE (0x80000)//(512*1024)

extern BOOL _VDEC_Vp6PsrSwHeaderInfo(VDEC_VP6_INFO_T *pDecInfo);
extern BOOL _VDEC_Vp6PsrSwInit(VDEC_VP6_INFO_T *prVp6DecInfo,INT32 i4DecId);
extern BOOL _VDEC_Vp6GetDecInfo(INT32 i4Id,VDEC_VP6_INFO_T **prVp6DecInfo);
extern BOOL _VDEC_Vp6PsrSwDecode(VDEC_VP6_INFO_T *pDecInfo);
extern BOOL _VDEC_Vp6PsrSwFin(VDEC_VP6_INFO_T *pDecInfo);
extern BOOL _VDEC_Vp6PsrSwRelease(VDEC_VP6_INFO_T *pDecInfo);
extern BOOL _VDEC_Vp6PsrSwGetPicInfo(VDEC_VP6_INFO_T *pDecInfo,UINT32 *pu4YAddr,UINT32 *pu4CAddr);

#endif

