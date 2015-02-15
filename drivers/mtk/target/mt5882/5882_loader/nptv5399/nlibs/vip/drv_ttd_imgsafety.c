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
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_ttd.c $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/
#include "general.h"
#include "x_assert.h"
#include "hw_ttd.h"
#include "hw_sw.h"
#include "drv_ttd.h"
#include "nptv_debug.h"
#include "drv_video.h"
#include "drv_tdtv_drvif.h"
#include "panel.h"

/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/
static UINT8 g_u1FixDsprtForImageSafety = 0;
static UINT8 g_u1FixDsprtLevel[3] = {64, 48, 32};

void vDrvTTDCalculateSafetyScale(void)
{
    INT32 i4Safe, i4Dist;
    INT32 i4PixSize, i4DConvex, i4DConcave, i4DParallax, i4Factor;
    INT32 i4DLimMax, i4DLimMin, i4TDMax, i4TDMin;
    INT32 i4ScalePos, i4ScaleNeg, i4Scale;
    INT32 i4DisparitySaclar, i4GlobalShift;

    i4Safe = 0, i4Dist = 10;
    i4PixSize = 320, i4DConvex = 256, i4DConcave = 256, i4DParallax = 256, i4Factor = 50;
    i4DLimMax = 0, i4DLimMin = 0, i4TDMax= 0, i4TDMin = 0;
    i4ScalePos = 128, i4ScaleNeg = 128, i4Scale = 128;
    i4DisparitySaclar = 0, i4GlobalShift = 128;

    if (g_u1ImageSafetyLevel)
    {
        i4Safe = (INT32)g_u1ImageSafetyLevel;
        i4Dist = (INT32)g_u1DistanceToTV;

        // (1) Find i4DLimMax, i4DLimMin from Parallax and Convex/Concave 1 degree constraint
        i4Safe = (i4Safe > 3) ? 3 : ((i4Safe < 1) ? 1 : i4Safe);

        if (g_u1FixDsprtForImageSafety)
        {
            i4DLimMax = (i4Dist*((INT32)(g_u1FixDsprtLevel[i4Safe-1]))/18) >> 1;
            i4DLimMin = -i4DLimMax;
        }
        else
        {
            i4PixSize = (INT32)(PANEL_Get3DPixelSize());
            i4Factor = (INT32)STEREO_COMFORT_FACTOR;
            i4DParallax = (i4Factor * 1000 / i4PixSize) / 2;
            i4DConvex = (i4Dist * 5236 / i4PixSize) / 2; // (PI/180)*300*1000 = 5236
            i4DConcave = (i4Dist * 5236 / i4PixSize) / 2; // (PI/180)*300*1000 = 5236

            i4DLimMax = i4DConvex;
            i4DLimMin = (i4DConcave > i4DParallax) ? (-i4DParallax) : (-i4DConcave);

            i4DLimMax = (i4DLimMax*(5-i4Safe) + 2) >> 2;
            i4DLimMin = (i4DLimMin*(5-i4Safe) + 2) >> 2;
        }

        g_i4dLimMax = i4DLimMax;
        g_i4dLimMin = i4DLimMin;

        // (2) Find i4TDMax, i4TDMin
        i4DisparitySaclar = g_u1DepthOfField;
        i4GlobalShift = g_u4ProtrudeFromScreen;

        if (g_u1Input3D)
        {
            if (g_u1SupportDepthCtrl)
            {
                i4TDMax = 0;
                i4TDMin = 0;
            }
            else
            {
                i4TDMax = i4GlobalShift - 128;
                i4TDMin = i4GlobalShift - 128;
            }
        }
        else
        {
            // 2D-to-3D
            i4TDMax = i4GlobalShift - 128 + (i4DisparitySaclar+2)/4;
            i4TDMin = i4GlobalShift - 128 - (i4DisparitySaclar+2)/4;
        }

        // (3) Return i4Scale
        i4ScalePos = ((i4DLimMax>i4TDMax)||(i4TDMax==0)) ? 128 : (i4DLimMax*128/i4TDMax);
        i4ScaleNeg = ((i4DLimMin<i4TDMin)||(i4TDMax==0)) ? 128 : (i4DLimMin*128/i4TDMin);

        i4Scale = (i4ScalePos > i4ScaleNeg) ? i4ScaleNeg : i4ScalePos;
    }
    else
    {
        g_i4dLimMax = 255;
        g_i4dLimMin = -255;
        i4Scale = 128;
    }

    vIO32WriteFldAlign(TTD_FW_REG_17, i4Scale, TTD_IMGSAFETY_SCALE);

    if (IO32ReadFldAlign(TTD_FW_REG_00, TTD_FW_IMGSAFE_DBG_EN))
    {
        LOG(0, "==== vDrvTTDCalculateSafetyScale ====\n");
        LOG(0, "i4Safe = %d, i4Dist = %d\n", i4Safe, i4Dist);
        LOG(0, "i4PixSize = %d, i4DConvex = %d, i4DConcave = %d, i4DParallax = %d, i4Factor = %d\n", i4PixSize, i4DConvex, i4DConcave, i4DParallax, i4Factor);
        LOG(0, "i4DLimMax = %d, i4DLimMin = %d, i4TDMax = %d, i4TDMin = %d\n", i4DLimMax, i4DLimMin, i4TDMax, i4TDMin);
        LOG(0, "i4ScalePos = %d, i4ScaleNeg = %d, i4Scale = %d\n", i4ScalePos, i4ScaleNeg, i4Scale);
        LOG(0, "g_i4dLimMax = %d, g_i4dLimMin = %d\n", g_i4dLimMax, g_i4dLimMin);
    }
}

