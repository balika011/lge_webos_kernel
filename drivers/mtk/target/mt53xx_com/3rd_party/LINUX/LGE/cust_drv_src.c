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
/******************************************************************************
*[File]             cust_drv.c
*[Version]          v0.0
*[Revision Date]    2012-04-07
*[Description]
	customized confidential file
*[Copyright]
*    Copyright (C) 2012 MediaTek Incorporation. All Rights Reserved.
******************************************************************************/

#include "x_os.h"
#include "x_drv_cli.h"
#include "x_printf.h"
#include "x_serial.h"
#include "x_hal_io.h"
#include "x_hal_5381.h"
#include "x_stl_lib.h"
#include "x_assert.h"
#include "x_bim.h"
#include "drvcust_if.h"

//cust bw measurement cli

#include "srm_drvif.h"
#include "x_ckgen.h"

static UINT32 _u8DBWTotal = 0;
static UINT32 _u4DBWTotalByte= 0;
static UINT32 _u4DBWMaxValue= 0;

static void _DramMesaure(const CHAR* szAgent, UINT32 u4Channel, UINT32 u4DelaySecond)
{
    UINT32 u4Mod;
    UINT32 u4DramDomainClk;
    UINT32 u4DelayClock;
    UINT32 u4Result;
    UINT32 u4Byte;
    UINT64 u8Result;
    BOOL fg32Bit, fgHalfFreqMode;

    if (u4Channel == 0)
    {
        fg32Bit = ((DRVCUST_InitGet(eDramType)==DDR_III_x1) || (DRVCUST_InitGet(eDramType)==DDR_II_x1)) ? 0 : 1;
        fgHalfFreqMode = (((IO_REG32(DRAM_BASE, 0x18) & 0x80) == 1) ? 1 : 0);
    }
    else
    {
        fg32Bit = ((DRVCUST_InitGet(eDramType)==DDR_III_x4) || (DRVCUST_InitGet(eDramType)==DDR_II_x4)) ? 1 : 0;
        fgHalfFreqMode = (((IO_REG32(DRAM_CHB_BASE, 0x18) & 0x80) == 1) ? 1 : 0);
    }

    u4Mod = SRM_DRV_TOTAL;

    if (fgHalfFreqMode)
    {
        Printf("Agent(%s) not measured.\n", szAgent);
        return;
    }

    if (u4DelaySecond == 0)
    {
        u4DelaySecond = 1;
    }

    u4DramDomainClk = BSP_GetDomainClock(SRC_MEM_CLK);
    //u4DelayClock = (u4DramDomainClk >> ((DRVCUST_InitGet(eDramType)==DDR_III_x1) ? 1 : 0)) * u4DelaySecond;
    u4DelayClock = (u4DramDomainClk >> (fgHalfFreqMode ? 1 : 0)) * u4DelaySecond;
    u4Result = SRM_DramMonitor(u4Channel, u4Mod, u4DelayClock, u4DelaySecond * 1000);
    u4Byte = u4Result*((fg32Bit)?16:8)/u4DelaySecond;

    u8Result = (UINT64)u4Result;
    _u8DBWTotal += u8Result;
    u8Result *= 1000;
    u8Result = u8Div6432(u8Result, (UINT64)u4DramDomainClk, NULL);
    _u4DBWTotalByte += (UINT64)u4Byte;

    if (u8Result > _u4DBWMaxValue)
    {
        _u4DBWMaxValue = (UINT32) u8Result;
    }
    //Printf(" . ");
    Printf("=");
}

static INT32 _i4DramBandWidthQueryCmd (INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4DelaySecond = 1;
    UINT32 u4Loop = 1;
    UINT32 u4DramDomainClk;
    Printf("--------- Channel A ---------\n");
    _u8DBWTotal = 0;
    _u4DBWTotalByte = 0;
    _u4DBWMaxValue = 0;

    _DramMesaure("total", 0, u4DelaySecond);
    
    u4DramDomainClk = (BSP_GetDomainClock(SRC_MEM_CLK) * u4DelaySecond) / 1000;

    _u8DBWTotal = u8Div6432(_u8DBWTotal, (UINT64)u4DramDomainClk, NULL);
    Printf("Total = %d, Max = %d, TotalByte = %d\n", _u8DBWTotal / u4Loop, _u4DBWMaxValue, _u4DBWTotalByte/u4Loop/1024/1024);


    Printf("------------------------------\n");
    Printf("--------- Channel B ---------\n");
    _u8DBWTotal = 0;
    _u4DBWTotalByte = 0;
    _u4DBWMaxValue = 0;

    _DramMesaure("total", 1, u4DelaySecond);
    
    u4DramDomainClk = (BSP_GetDomainClock(SRC_MEM_CLK) * u4DelaySecond) / 1000;

    _u8DBWTotal = u8Div6432(_u8DBWTotal, (UINT64)u4DramDomainClk, NULL);
    Printf("------------------------------\n");
    Printf("Total = %d, Max = %d, TotalByte = %d\n", _u8DBWTotal / u4Loop, _u4DBWMaxValue, _u4DBWTotalByte/u4Loop/1024/1024);
    Printf("------------------------------\n");

    return 1;
}

CLI_EXEC_T _arCustCmdTbl[] =
{
    {"dram-band-width", "dbw", _i4DramBandWidthQueryCmd, NULL, "Dram Bandwidth query", CLI_GUEST},

    // last cli command record, NULL
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

