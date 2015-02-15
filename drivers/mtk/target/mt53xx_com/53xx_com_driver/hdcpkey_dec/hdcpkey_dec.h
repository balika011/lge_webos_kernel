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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: mtk_edid.h
 *
 * Project:
 * --------
 *   MT5890
 *
 * Description:
 * ------------
 *   software descrypt HDCP 2.2 and 1.4 key
 *   
 * Author:
 * -------
 *   
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: hdcpkey_dec.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifndef _HDCPKEY_DEC_H
#define _HDCPKEY_DEC_H

#include "dmx_drm_drvif.h"
#include "eeprom_if.h"

#define HDMI20_PROM_ADDR 0xf002e3e0
#define HDMI20_PROM_DATA 0xf002e3e4

#define HDMI20_PRAM_ADDR 0xf002e3d8
#define HDMI20_PRAM_DATA 0xf002e3dc

#define HDMI20_HDCP22_KEY0_ADDR 0xf002e3c0
#define HDMI20_HDCP22_KEY0_DATA 0xf002e3c4

#define HDMI20_HDCP22_KEY1_ADDR 0xf002e3d0
#define HDMI20_HDCP22_KEY1_DATA 0xf002e3d4

#define HDMI14_HDCP14_KEY_ADDR 0xf002e3c8
#define HDMI14_HDCP14_KEY_DATA 0xf002e3cc

#define HDMI20_EDID_ADDR 0xf0028E1c
#define HDMI20_EDID_DATA 0xf0028E20

#define HDMI20_EDID_CTL3 0xf0028E18
    #define FLD_PHY_ADDR3 Fld(16,16,AC_FULLW32)//[31:16]
    #define FLD_OFFSET3 Fld(8,8,AC_FULLB1)//[15:8]
    #define FLD_CHKSUM3 Fld(8,0,AC_FULLB0)//[7:0]


#define MHL_EDID_ADDR 0xf0028E5c
#define MHL_EDID_DATA 0xf0028E60

#define PRIV_KEY_SIZE 16

typedef struct
{
	UINT8 u1P[64];
	UINT8 u1Q[64];
	UINT8 u1DP[64];
	UINT8 u1DQ[64];
	UINT8 u1QINV[64];
	UINT8 u1ReceiverID[5];
	UINT8 u1RxPub[131];
	UINT8 u1Rsv[2];
	UINT8 u1NotUse[54];
	
	UINT8 u1PubSignature[384];
	//UINT8 u1SHA[20];
}HDCP2_KEY_INFO_T;

typedef struct
{
	UINT8 u1BKSV[5];
	UINT8 u1CRCValue[2];
	UINT8 u1PrivateKey[280];
}HDCP1_KEY_INFO_T;

typedef struct
{
	UINT16 M0:1;
	UINT16 M1:1;
	UINT16 M2:1;
	UINT16 M3:1;
	UINT16 M4:1;
	UINT16 M5:1;
	UINT16 M6:1;
	UINT16 M7:1;
	UINT16 M8:1;
	UINT16 M9:1;
	UINT16 M10:1;
	UINT16 M11:1;
	UINT16 M12:1;
	UINT16 M13:1;
	UINT16 M14:1;
	UINT16 M15:1;
}HDCP1_KEY_MASK_T;

#ifdef CC_HDMI_2_0_HDCP_BIN
extern void vHDMI20HDCP2KeyWriteToEep(void);
extern void vHDMI14HDCP1KeyWriteToEep(void);
extern void vHDMI20EDIDWriteToEep(void);
extern void vMHL20EDIDWriteToEep(void);
extern void vHDMI20PromReadFromEep(UINT8 *aupProm);
extern void vHDMI20PramReadFromEep(UINT8 *aupPram);
//extern void vHDMI20EDIDReadFromEep(UINT8 *aupEdid);
//extern void vMHL20EDIDReadFromEep(UINT8 *aupEdid);
extern void vHDCP2XKeyReadFromEep(UINT8 *aupHdcp2);
extern void vHDCP1XKeyReadFromEep(UINT8 *aupHdcp1);
extern UINT8* bHDCP2XDecrypt(UINT8 *aupHdcp2);
extern void vHDMI20LoadPromToSram(void);
extern void vHDMI20LoadPramToSram(void);
extern void vHDMI20LoadHdcp2KeyToSram(void);
extern void vHDMILoadHdcp14KeyToSram(void);
extern void vHDMILoadEDID2ToSram(void);
extern void vHDMILoadMhlEdidToSram(void);
extern void vHDMI20SramMbistCheck(UINT8 u1Type,UINT8 u1Val);
extern void vHDMI20LoadHdcpKey(void);
#endif
#endif

