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
/*
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_dvi.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/**
 * @file DVI/HDMI/CCIR decoder driver interface
 *
 * @author daniel_hsiao@mtk.com.tw
 */

#ifndef _DRV_DVI2_H_
#define _DRV_DVI2_H_

#include "typedef.h"

typedef struct
{
	
	UINT8   _bDviChkState;
	UINT8   _bDviModeChgFlg;
	UINT8	_bDviVclk;
	UINT32	_wDviHclk; // timing table: table value; otherwise, the value used by timing search
	UINT32	_wDviPixclk;
	UINT32	_wDviWidth;
	UINT32	_wDviHeight;
	UINT32	_wDviHtotal;
	UINT32	_wDviVtotal;
	UINT8	_bDviTiming;
	UINT8   _bWidthStableCnt;
	UINT8   _bHeightStableCnt;
	UINT8   _bWidthStableFlg;
	UINT8   _bHeightStableFlg;

	UINT8  _bModeChgCnt;
	UINT8  _bDeChgCnt;
	UINT8  _bPixelClkChgCnt;
	UINT8  _bColorChgCnt;
	UINT8  _bNoSigalCnt;
	UINT8  _bNoConnectCnt;
	UINT8  _bInfoChgCnt;
}DviStatus_t;

extern void vCCIR2SetAvStart(UINT16 wStart);
extern void vCCIR2SetNewAvStart(UINT16 wStart);
extern void vCCIR2SetAvWidth(UINT16 wWidth);
extern void vCCIR2SetNewAvWidth(UINT16 wWidth);
extern void vCCIR2SetVsStart(UINT16 wStart);
extern void vCCIR2SetNewVsStart(UINT16 wStart);
extern void vCCIR2SetVdeWidth(UINT16 wStart);
extern void vCCIR2SetNewVdeWidth(UINT16 wStart);
extern UINT16 wCCIR2GetAvStart(void);
extern UINT16 wCCIR2GetNewAvStart(void);
extern UINT16 wCCIR2GetAvWidth(void);
extern UINT16 wCCIR2GetNewAvWidth(void);
extern UINT16 wCCIR2GetVsStart(void);
extern UINT16 wCCIR2GetNewVsStart(void);
extern UINT16 wCCIR2GetVdeWidth(void);
extern UINT16 wCCIR2GetNewVdeWidth(void);
extern void vDvi2Initial(UINT8 bchannel);
extern void vDvi2Connect(UINT8 bchannel, UINT8 fgIsOn);
extern void vCCIRSelW2D(UINT8 u1Type, BOOL bSelW2d);
extern void vCCIR2GetStatus(UINT8 bchannel);
extern UINT16 wDvi2InputHTotal(UINT8 u1Type);
extern UINT16 wDvi2InputVTotal(UINT8 u1Type);
extern UINT16 wDvi2InputWidth(UINT8 u1Type);
extern UINT16 wDvi2InputHeight(UINT8 u1Type);
extern void vDvi2SetModeDone(UINT8 u1Type);
extern void vDvi2SetModeChange(UINT8 u1Type);
extern UINT8 bDvi2RefreshRate(UINT8 u1Type);
extern UINT8 bDvi2SigStatus(UINT8 u1Type);
extern UINT8 bDvi2Interlace(UINT8 u1Type);
extern BOOL fgIsDvi2VideoTiming(UINT8 u1Type);
extern void vDvi2ModeDetect(UINT8 u1Type);
extern void vDvi2ChkModeChange(UINT8 u1Type);
extern BOOL fgDvi2IsModeChg(UINT8 bchannel);
extern UINT8 bDvi23DPacketVaild(UINT8 u1Type);
extern UINT32 dwDvi2Get3DVtotal(UINT8 u1Type);
extern UINT32 dwDvi2Get3DVactive(UINT8 u1Type);
extern UINT32 dwDvi2Get3DHtotal(UINT8 u1Type);
extern UINT32 dwDvi2Get3DHactive(UINT8 u1Type);
extern UINT8 bDvi2Get3DRefreshRate(UINT8 u1Type);
extern void vDvi2GetDebugMessage(UINT8 u1Type);
extern UINT32 wDvi2QueryAP3DSetting(UINT8 u1Type);
extern UINT8 bDvi2GetInterlaced(UINT8 u1Type);
extern UINT16 wDvi2GetPorch(UINT8 bPath, UINT8 bPorchType);
extern void vDvi2SetPorch(UINT8 bPath, UINT8 bPorchType, UINT16 wValue);

#endif
