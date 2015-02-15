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
  /*************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: drv_predown.h $
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *
 *
 * Author:
 * -------
 *
 *
 * Last changed:
 * -------------
 * $Author: p4admin $
 *
 * $Date: 2015/02/15 $
 *
 * $Revision: #1 $
*************************************************************************/
#ifndef _DRV_Mon_Out_PREDOWN_H_
#define _DRV_Mon_Out_PREDOWN_H_
#include "general.h"
#include "video_def.h"
#include "vdp_if.h"
#include "vdo_rm.h"

typedef struct
{
    UINT8 bMonOutPDSModeChanged;
    UINT32 u4ForcedHPDS;
    UINT32 u4ForcedVPDS;
    UINT32 u4HPDSFactor;
    UINT32 u4VPDSFactor; 
    UINT32 u4InputWidth;			        //input width(pixel based)
    UINT32 u4InputHeight;                   //input height(pixel based)
    UINT32 u4OutputWidth;                   //output Width(pixel based)
    UINT32 u4OutputHeight;	                //output Height(pixel based)    
    UINT32 u43DMode;                        //3D mode
}MONOUTPREDOWN_INFO_T;


#define Mon_Out_PDS_WRITE_FLD(reg,value,fld) vIO32WriteFldAlign(reg, value, fld)
#define Mon_Out_PDS_READ_FLD(reg,fld) IO32ReadFldAlign(reg,fld)
#define getMonOutPDSInfo() (&_mon_out_pdsInfo)

EXTERN MONOUTPREDOWN_INFO_T _mon_out_pdsInfo;

void vMonOutPDSSwInit(void);
void vMonOutPDSHwInit(void);
void vMonOutPDSImportEn(UINT8 bOnOff);
void vMonOutPDSImportTrigger(void);
UINT32 u4MonOutPDSGetHPDSFactor(void);
UINT32 u4MonOutPDSGetVPDSFactor(void);
void vMonOutPDSSetHPDSFactor(UINT32 factor);
void vMonOutPDSSetVPDSFactor(UINT32 factor);
void _vMonOutPDSUpdateDispPrm(void);
void vMonOutPDS(void);
void vMonOutPDSSetModeChanged(void);

void vMonOutPDSSetHPSFactorReg(UINT32 u4Factor);
void vMonOutPDSSetVPSFactorReg(UINT32 u4Factor);
UINT32 u4MonOutPDSGetHPSFactorReg(void);
UINT32 u4MonOutPDSGetVPSFactorReg(void);
void vMonOutPDSSetInputResolutionReg(UINT32 u4InW, UINT32 u4InH);
void vMonOutPDSSetOutputResolutionReg(UINT32 u4InW, UINT32 u4InH);
UINT32 u4MonOutPDSGetInputWidthReg(void);
UINT32 u4MonOutPDSGetInputHeightReg(void);
UINT32 u4MonOutPDSGetOutputWidthReg(void);
UINT32 u4MonOutPDSGetOutputHeightReg(void);
void vMonOutPDSSetVCenActAtSync(UINT8 value);

UINT32 u4MonOutPDSGetHActive(UINT8 bInOut);
UINT32 u4MonOutPDSGetHTotal(UINT8 bInOut);
UINT32 u4MonOutPDSGetVActive(UINT8 bInOut);
UINT32 u4MonOutPDSGetVTotal(UINT8 bInOut);

BOOL bDrvMonOutPDSIsSetResDone(SET_RES_STATUS_TYPE_E eStatusType);
void vDrvMonOutPDSClrSetRes(SET_RES_STATUS_TYPE_E eStatusType);

void vDrvMonOutPDSInOutStatusDump(void);
#endif
