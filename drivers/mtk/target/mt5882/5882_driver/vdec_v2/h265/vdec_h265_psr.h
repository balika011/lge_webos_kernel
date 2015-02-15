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
 * $RCSfile: vdec_h265_psr.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

#ifndef VDEC_H265_PSR_H
#define VDEC_H265_PSR_H

#include "vdec_h265_dec.h"

//SPS syntax error
#define     SPS_OK                      0x00
#define     SPS_LAYER_CONFLICT          0xFF02
#define     SPS_ID_INVLID               0xFF03
#define     SPS_BITDEPTH_NOT_SUPPORT    0xFF04
#define     SPS_SL_SYNTAX_ERROR         0xFF05
#define     SPS_RPS_SYNTAX_ERROR        0xFF06

//PPS syntax error
#define PPS_OK                          0x00
#define PPS_ID_SYNTAX_ERROR             0xFE02
#define PPS_NO_SPS_INFO_ERROR           0xFE03
#define PPS_REF_NUM_ERROR               0xFE04
#define PPS_QP_ERROR                    0xFE05
#define PPS_SCL_ERROR                   0xFE06

//Slice syntax error
#define SLICE_OK                        0x00
#define SLICE_PPS_ID_ERROR              0xFD02
#define SLICE_ST_RPS_NULL               0xFD03
#define SLICE_ST_DROP                   0xFD04
#define GET_FB_TIMEOUT                  0xFD05
#define ALLOC_FBG_FAIL                  0xFD06
#define SLICE_RES_CHG                   0xFD07


INT32 vVDecH265InitROM(UINT32 u4InstID);
void vVDecH265InitSigLastScan(UINT32* pBuffD, UINT32* pBuffH, UINT32* pBuffV, INT32 iWidth, INT32 iHeight);
INT32 vVDecH265InitRPS(UINT32 u4InstID);
INT32 vVDecH265InitScalingList(UINT32 u4InstID);

//Nal Unit Header
BOOL fgVDecH265NalUnitHeader(UINT32 u4VDecID,UCHAR *ucNalType,UCHAR *ucLayerId,UCHAR *ucNuhTempId);

//SPS ref api
INT32 i4VDecH265SPSPsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue);
void vVDecH265PTL_Rbsp(UINT32 u4InstID, UINT32 u4VDecID,pH265_PTL_Data pPTL,INT32 i4MaxNumSubLayersMinus1);
void vVDecH265InitSPS(UINT32 u4InstID,H265_SPS_Data *prSPS);
void vVDecH265InitPicInfo(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo);
UINT32  vVDecH265SL_Rbsp(UINT32 u4InstID, UINT32 u4VDecID, H265_SL_Data* scalingList);
UINT32  vVDecH265RPS_Rbsp(UINT32 u4InstID,UINT32 u4VDecID,H265_SPS_Data* pSPS,H265_RPS_Data* pRPS,VDEC_INFO_H265_PIC_INFO_T* prPic,UINT32 u4Idx);
void vVDecH265VUI_Rbsp( UINT32 u4InstID,UINT32 u4VDecID,H265_VUI_Data* VUI,UINT32 MaxTLayers);
void vVDecH265HRD_Rbsp( UINT32 u4InstID,UINT32 u4VDecID,H265_HRD_Data* HRD,BOOL bCommonInfPresentFlag ,UINT32 MaxTLayersMinus1);

//PPS ref api
INT32 vVDecH265PPSPsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue);
void vVDecH265InitPPS(UINT32 u4InstID,H265_PPS_Data *prPPS);

//SEI ref api

//SLICE ref api
INT32 i4VDecH265SlimSlicePsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue);
INT32 i4VDecH265RestOfSlicePsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue);
void vVDecH265SlicePsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue);
void vVDecH265InitSliceHdr(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo);
void vVDecH265InitPicInfo(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo);
void vVDecH265CalcCurSlicePOC(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo);
void vVDecH265PrepareShortTermRPS(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo);
void vVDecH265PrepareLongTermRPS(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo);

#endif


