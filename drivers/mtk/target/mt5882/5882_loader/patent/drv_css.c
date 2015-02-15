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
#include "sv_const.h"
#include "hw_ospe.h"
#include "hw_sw.h"
#include "drv_scpos.h"
#include "drv_scaler.h"
#include "drv_scaler_psc.h"
#include "drv_mjc.h"
#include "drv_display.h"
#include "drv_css.h"
#include "mjc_debug.h"
#endif

/*----------------------------------------------------------------------------*
 * Definitions/Macros
 *----------------------------------------------------------------------------*/
#define CSS_FILTER_TAP_NUM      (64)
#define CSS_VERT_RANGE          (20)
#define CSS_HORI_RANGE          (32)
#define CLIP(val,lower,uppper)  ((val>uppper)?uppper:((val<lower)?lower:val))

INT32 i4CurrXOffset = 0;
INT32 i4CurrYOffset = 0;
UINT8 fgCSSOnOff = SV_OFF;

INT32 i4CSSOSDLevel = 0;

UINT32 u4CSSGmvIndex = 0;
CSS_GMV_T arCSSGmv[CSS_FILTER_TAP_NUM];
CSS_GMV_T rCSSGmvSum = {0, 0};
CSS_GMV_T rCSSGmvCurr;
CSS_GMV_T rCSSGmvComp;
CSS_GMV_T rCSSGmvError;

/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/
void vDrvCSSInitialize(void)
{
    UINT32 u4DisplayWidth, u4DisplayHeight;

    u4DisplayWidth = PANEL_GetPanelWidth() - CSS_MAX_COMPENSATION * 2;
    u4DisplayHeight = PANEL_GetPanelHeight() - CSS_MAX_COMPENSATION * 2;

    vIO32WriteFldAlign(CSS_CTRL_00, CSS_MAX_COMPENSATION, CSS_BOUND_UPPER);
    vIO32WriteFldAlign(CSS_CTRL_00, CSS_MAX_COMPENSATION - 1, CSS_BOUND_LOWER);
    vIO32WriteFldAlign(CSS_CTRL_00, SV_ON, CSS_UPDATE_GMV_TAB_AT_NUM1);

    i4CurrXOffset = 0;
    i4CurrYOffset = 0;
    rCSSGmvSum.i4GmvX = 0;
    rCSSGmvSum.i4GmvY = 0;
    rCSSGmvError.i4GmvX = 0;
    rCSSGmvError.i4GmvY = 0;

    u4CSSGmvIndex = 0;
    {
        UINT32 u4Index;
        for (u4Index = 0; u4Index < CSS_FILTER_TAP_NUM; u4Index++)
        {
            arCSSGmv[u4Index].i4GmvX = 0;
            arCSSGmv[u4Index].i4GmvY = 0;
        }
    }

    vPscSetInputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vPscSetOutputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vDrvMJCSetInputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vDrvMJCSetOutputWidthHeight(u4DisplayWidth, u4DisplayHeight);
    vDrvCSSCompensate(0, 0);
    vPscSetResToggle(SV_OFF);
    vPscSetResToggle(SV_ON);
    vPscSetData(SV_ON);
    vDrvCSSSetBorder(SV_ON);
}

void vDrvCSSOnOff(UINT32 fgOnOff)
{
    if (fgOnOff)
    {
        vDrvCSSInitialize();
    }
    else
    {
        UINT32 u4DisplayWidth, u4DisplayHeight;

        u4DisplayWidth = PANEL_GetPanelWidth();
        u4DisplayHeight = PANEL_GetPanelHeight();
        vPscTimegenImportEnable(SV_OFF);
        vPscSetResImportEnable(SV_OFF);
        vPscSetInputWidthHeight(u4DisplayWidth, u4DisplayHeight);
        vPscSetOutputWidthHeight(u4DisplayWidth, u4DisplayHeight);
        vDrvMJCSetInputWidthHeight(u4DisplayWidth, u4DisplayHeight);
        vDrvMJCSetOutputWidthHeight(u4DisplayWidth, u4DisplayHeight);
        vDrvCSSSetBorder(SV_OFF);
        vPscSetHmskStartOfst(0);
        vPscSetVmskStartOfst(0);
        vPscTimegenImportEnable(SV_ON);
        vPscSetResImportEnable(SV_ON);
    }

    fgCSSOnOff = fgOnOff;
}

void vDrvCSSSetBorder(UINT32 fgOnOff)
{
    vOSTGSetBorderOnOff(SV_VP_MAIN, fgOnOff);
    vOSTGSetBorderColor(SV_VP_MAIN, 0x404040);
    vIO32WriteFldMulti(WINDOW_02, 
        P_Fld(CSS_MAX_COMPENSATION * 2, WINDOW_1_HPOSLR) |
        P_Fld(0, WINDOW_1_HPOSLL));
    vIO32WriteFldMulti(WINDOW_03, 
        P_Fld(wDISPLAY_WIDTH, WINDOW_1_HPOSRR) |
        P_Fld(wDISPLAY_WIDTH - CSS_MAX_COMPENSATION * 2, WINDOW_1_HPOSRL));
    vIO32WriteFldMulti(WINDOW_04, 
        P_Fld(CSS_MAX_COMPENSATION * 2, WINDOW_1_VPOSUD) |
        P_Fld(0, WINDOW_1_VPOSUU));
    vIO32WriteFldMulti(WINDOW_05, 
        P_Fld(wDISPLAY_HEIGHT, WINDOW_1_VPOSDD) |
        P_Fld(wDISPLAY_HEIGHT - CSS_MAX_COMPENSATION * 2, WINDOW_1_VPOSDU));
}

INT32 i4DrvCSSClipGmv(INT32 i4Gmv)
{
    INT32 i4UpperBound;
    INT32 i4LowerBound;    

    i4UpperBound = IO32ReadFldAlign(CSS_CTRL_00, CSS_BOUND_UPPER);
    i4LowerBound = IO32ReadFldAlign(CSS_CTRL_00, CSS_BOUND_LOWER);

    if (i4UpperBound == 0)
        i4UpperBound = CSS_MAX_COMPENSATION;

    if (i4LowerBound == 0)
        i4LowerBound = CSS_MAX_COMPENSATION;

    if (i4Gmv < (-i4LowerBound))
    {
        return (-i4LowerBound);
    }
    else if (i4Gmv > i4UpperBound)
    {
        return i4UpperBound;
    }
    else
    {
        return i4Gmv;
    }
}

void vDrvCSSCalGmvMean(CSS_GMV_T * prCSSCurrGmv, CSS_GMV_T * prCSSFilterGmv)
{
    INT32 i4FilterTap;
    UINT32 u4Index;

    //rCSSGmvSum.i4GmvX += (prCSSCurrGmv->i4GmvX - arCSSGmv[u4CSSGmvIndex].i4GmvX);
    //rCSSGmvSum.i4GmvY += (prCSSCurrGmv->i4GmvY - arCSSGmv[u4CSSGmvIndex].i4GmvY);

    i4FilterTap = (INT32)IO32ReadFldAlign(CSS_CTRL_00, CSS_FILTER_TAP);
    i4FilterTap = i4FilterTap ? i4FilterTap : 64;

    arCSSGmv[u4CSSGmvIndex] = (*prCSSCurrGmv);

    rCSSGmvSum.i4GmvX = 0;
    rCSSGmvSum.i4GmvY = 0;
    for (u4Index = 0; u4Index < i4FilterTap; u4Index++)
    {
        rCSSGmvSum.i4GmvX += arCSSGmv[u4Index].i4GmvX;
        rCSSGmvSum.i4GmvY += arCSSGmv[u4Index].i4GmvY;
    }

    prCSSFilterGmv->i4GmvX = rCSSGmvSum.i4GmvX / i4FilterTap;
    prCSSFilterGmv->i4GmvY = rCSSGmvSum.i4GmvY / i4FilterTap;

    u4CSSGmvIndex ++;
    u4CSSGmvIndex = u4CSSGmvIndex % ((UINT32)i4FilterTap);
}

void vDrvCSSDetectOSD(void)
{
    UINT32 u4OSDCnt;

    u4OSDCnt = IO32ReadFldAlign(MJC_STA_PSG_06, MJC_OSD_CNT);

    if (u4OSDCnt > 0)
    {
        i4CSSOSDLevel = CLIP(i4CSSOSDLevel + 1, 0, 16);
    }
    else
    {
        i4CSSOSDLevel = CLIP(i4CSSOSDLevel - 1, 0, 16);
    }
}

void vDrvCSSProcess(INT32 i4Num, INT32 i4Den)
{
    INT32 i4GmvX, i4GmvY;
    INT32 i4NewXOffset, i4NewYOffset;
    CSS_GMV_T rCSSGmvMean;

    if (i4Den == 0)
        i4Den = 1;

    vDrvMJCGetGMV(&i4GmvX, &i4GmvY);

    rCSSGmvCurr.i4GmvX = CLIP(i4GmvX, -CSS_HORI_RANGE, CSS_HORI_RANGE);
    rCSSGmvCurr.i4GmvY = CLIP(i4GmvY, -CSS_VERT_RANGE, CSS_VERT_RANGE);

    vDrvCSSDetectOSD();

    if ((i4Num == 1) || (IO32ReadFldAlign(CSS_CTRL_00, CSS_UPDATE_GMV_TAB_AT_NUM1) == 0))
    {
        vDrvCSSCalGmvMean(&rCSSGmvCurr, &rCSSGmvMean);

        if (IO32ReadFldAlign(CSS_CTRL_00, CSS_DIRECT_COMP))
        {
            rCSSGmvComp.i4GmvX = rCSSGmvCurr.i4GmvX;
            rCSSGmvComp.i4GmvY = rCSSGmvCurr.i4GmvY;
        }
        else
        {
            rCSSGmvComp.i4GmvX = (rCSSGmvCurr.i4GmvX - rCSSGmvMean.i4GmvX);
            rCSSGmvComp.i4GmvY = (rCSSGmvCurr.i4GmvY - rCSSGmvMean.i4GmvY);
        }
    }

    if (i4CSSOSDLevel || arMJCPrm.u1EffectParam == 0)
    {
        LOG(2, "vDrvCSSProcess: i4CSSOSDLevel=%d\n", i4CSSOSDLevel);
        return;
    }

    if (IO32ReadFldAlign(CSS_CTRL_00, CSS_UPDATE_GMV_TAB_AT_NUM1) == 0)
    {
        i4NewXOffset = i4DrvCSSClipGmv(i4CurrXOffset + (((rCSSGmvComp.i4GmvX + rCSSGmvError.i4GmvX)) / i4Den));
        i4NewYOffset = i4DrvCSSClipGmv(i4CurrYOffset + (((rCSSGmvComp.i4GmvY + rCSSGmvError.i4GmvY)) / i4Den));
        rCSSGmvError.i4GmvX = (rCSSGmvComp.i4GmvX + rCSSGmvError.i4GmvX) % i4Den;
        rCSSGmvError.i4GmvY = (rCSSGmvComp.i4GmvY + rCSSGmvError.i4GmvY) % i4Den;
        i4CurrXOffset = i4NewXOffset;
        i4CurrYOffset = i4NewYOffset;
    }
    else
    {
        if (i4Num == 0)
        {
            i4NewXOffset = i4DrvCSSClipGmv(i4CurrXOffset + rCSSGmvComp.i4GmvX);
            i4NewYOffset = i4DrvCSSClipGmv(i4CurrYOffset + rCSSGmvComp.i4GmvY);
            i4CurrXOffset = i4NewXOffset;
            i4CurrYOffset = i4NewYOffset;
        }
        else
        {
            i4NewXOffset = i4DrvCSSClipGmv(i4CurrXOffset + (rCSSGmvComp.i4GmvX * i4Num) / i4Den);
            i4NewYOffset = i4DrvCSSClipGmv(i4CurrYOffset + (rCSSGmvComp.i4GmvY * i4Num) / i4Den);
        }
    }

    LOG(2, "vDrvCSSProcess: [%d/%d] (%4d, %4d) (%4d, %4d) (%4d, %4d)\n",
        i4Num, i4Den,
        rCSSGmvCurr.i4GmvX, rCSSGmvCurr.i4GmvY,
        rCSSGmvComp.i4GmvX, rCSSGmvComp.i4GmvY,
        i4NewXOffset, i4NewYOffset);

    vDrvCSSCompensate(i4NewXOffset, i4NewYOffset);
}

void vDrvCSSCompensate(INT32 i4XOffset, INT32 i4YOffset)
{
    UINT32 u4XStart, u4YStart;

    //LOG(2, "vDrvCSSCompensate: (%4d, %4d)\n", i4XOffset, i4YOffset);

    u4XStart = (UINT32)(CSS_MAX_COMPENSATION + i4XOffset);
    u4YStart = (UINT32)(CSS_MAX_COMPENSATION + i4YOffset);

    vPscSetHmskStartOfst(u4XStart);
    vPscSetVmskStartOfst(u4YStart);
    //set register import 
    vPscSetResToggle(SV_OFF);
    vPscSetResToggle(SV_ON);
}

