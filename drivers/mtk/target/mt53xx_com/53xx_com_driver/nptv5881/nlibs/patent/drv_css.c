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
 * $RCSfile: drv_css.c $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/
#ifdef _C_MODEL_
#include "sc98.h"
#include "fb.h"
#else
#include "drv_scaler_psc.h"
#include "drv_mjc.h"
#include "drv_display.h"
#include "drv_css.h"
#endif

/*----------------------------------------------------------------------------*
 * Definitions/Macros
 *----------------------------------------------------------------------------*/
#define CSS_MAX_COMPENSATION    (48)

INT32 i4CurrXOffset = 0;
INT32 i4CurrYOffset = 0;
UINT8 fgCSSOnOff = SV_OFF;

/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/
void vDrvCSSInitialize(void)
{
    UINT32 u4DisplayWidth, u4DisplayHeight;

    u4DisplayWidth = PANEL_GetPanelWidth() - CSS_MAX_COMPENSATION * 2;
    u4DisplayHeight = PANEL_GetPanelHeight() - CSS_MAX_COMPENSATION * 2;

    i4CurrXOffset = 0;
    i4CurrYOffset = 0;

    vPscSetInputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vPscSetOutputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vDrvMJCSetInputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vDrvMJCSetOutputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vDrvCSSCompensate(0, 0);
    vPscSetResToggle(SV_OFF);
    vPscSetResToggle(SV_ON);
    vPscSetData(SV_ON);
}

static INT32 i4DrvCSSClipGmv(INT32 i4Gmv)
{
    if (i4Gmv < (-CSS_MAX_COMPENSATION))
    {
        return (-CSS_MAX_COMPENSATION);
    }
    else if (i4Gmv > CSS_MAX_COMPENSATION)
    {
        return CSS_MAX_COMPENSATION;
    }
    else
    {
        return i4Gmv;
    }
}

static INT32 i4DrvCSSCalCompValue(INT32 i4Gmv)
{
    return i4Gmv;
}

void vDrvCSSProcess(void)
{
    INT32 i4GmvX, i4GmvY;

    vDrvMJCGetGMV(&i4GmvX, &i4GmvY);
    i4CurrXOffset += i4DrvCSSCalCompValue(i4GmvX);
    i4CurrYOffset += i4DrvCSSCalCompValue(i4GmvY);
    i4CurrXOffset = i4DrvCSSClipGmv(i4CurrXOffset);
    i4CurrYOffset = i4DrvCSSClipGmv(i4CurrYOffset);
    vDrvCSSCompensate(i4CurrXOffset, i4CurrYOffset);
}

void vDrvCSSCompensate(INT32 i4XOffset, INT32 i4YOffset)
{
    UINT32 u4XStart, u4YStart;

    u4XStart = (UINT32)(CSS_MAX_COMPENSATION + i4XOffset);
    u4YStart = (UINT32)(CSS_MAX_COMPENSATION + i4YOffset);

    vPscSetHmskStartOfst(u4XStart);
    vPscSetVmskStartOfst(u4YStart);
    vPscSetData(SV_ON);
}

