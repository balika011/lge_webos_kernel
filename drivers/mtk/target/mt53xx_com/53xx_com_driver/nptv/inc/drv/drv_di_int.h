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
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_di_int.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file drv_di_int.h
 *  Brief of file drv_di_int.h.
 *  Details of file drv_di_int.h (optional).
 */

#ifndef DRV_DI_INT_H
#define DRV_DI_INT_H

void vDrvDIFreezeFlow(UINT8 u1VdpId);
void vDrvDICsPatch(UINT8 u1VdpId);

void vDrvDIReset(UINT8 u1VdpId, UINT8 bStart);
void vDrvDIInitQuality(UINT8 bPath);
void vDrvDIInitClock(UINT8 u1VdpId);

UINT32 u4DrvDIGetClkRate(UINT8 u1VdpId);
void vDrvDISetKernelClock(UINT8 u1VdpId, DI_KERNEL_CLOCK_TYPE eType);

void vDrvDISetSDE(UINT8 u1VdpId, UINT8 bOnOff, UINT16 u2N, UINT16 u2M);
void vDrvDISetODE(UINT8 u1VdpId, UINT8 bOnOff, UINT16 u2N, UINT16 u2M);
void vDrvDISetKDE(UINT8 u1VdpId, UINT16 u2N, UINT16 u2M);

void vDrvCSOnOff(UINT8 bPath, UINT8 u1OnOff);
void vDrvCSOnOffEx(UINT8 bPath, UINT8 u1OnOff);
void vDrvCSForceOnOff(UINT8 bPath, UINT8 u1OnOff);

void vDrvDISetQuality(UINT8 bPath);
void vDrvDIQtyMainLoop(UINT8 bPath);
void vDrvDISetQualityISR(void);

void vDrvDIBypass(UINT8 u1VdpId, UINT8 u1Bypass);
void vDrvDISetBgn(UINT8 u1VdpId, UINT8 u1S_V_Bgn, UINT8 u1K_H_Bgn, UINT8 u1K_V_Bgn);
void vDrvDISRCWOutIOnOff(UINT8 u1VdpId, UINT8 u1OnOff);
void vDrvDIProgModeOnOff(UINT8 u1VdpId, UINT8 u1OnOff);
void vDrvDIVSyncCntClr(UINT8 u1VdpId);
void vDrvDISetBufferProtect(UINT8 u1VdpId, UINT32 u4AddrYMsb, UINT32 u4AddrFlg, UINT32 u4AddrEnd);
void vDrvDISetWritePrecision(UINT8 u1VdpId, UINT8 b8bit);
void vDrvDISetBufferAddr(UINT8 u1VdpId, UINT32 u4AddrYMsb, UINT32 u4AddrYLsb, 
        UINT32 u4AddrUMsb, UINT32 u4AddrULsb, UINT32 u4AddrVMsb, UINT32 u4AddrVLsb, UINT32 u4AddrFlg);
void vDrvDIChkStatus(UINT8 u1VdpId);
        
#endif    // DRV_DI_INT_H
