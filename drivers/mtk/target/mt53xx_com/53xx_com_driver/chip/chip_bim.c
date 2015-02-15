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
#include "x_typedef.h"
#include "x_debug.h"
#include "x_ckgen.h"
#include "x_bim.h"
#include "x_printf.h"
#include "x_chip_id.h"
#include "x_gpio.h"
#include "x_timer.h"
#include "x_hal_926.h"


INT32 CacheMeterQuery(UINT32 *pu4IReq, UINT32 *pu4IMiss, UINT32 *pu4IPenalty,
        UINT32 *pu4DReq, UINT32 *pu4DMiss, UINT32 *pu4DPenalty)
{
    UINT32 flag, i;
    HAL_TIME_T t1, t2, td;
    volatile UINT32 byte = 0;

    flag = HalCriticalStart();

#ifdef CC_MT5387
    BIM_WRITE32(0x158, 0x0);
    BIM_WRITE32(0x158, 0x11);
#endif

    HAL_GetTime(&t1);
    for (i=0; i<1000000; i++)
    {
        byte = *(volatile UINT32*)0x1000000;
        *(volatile UINT32*)0x1000000 = 1;
    }
    HAL_GetTime(&t2);

#ifdef CC_MT5387
    *pu4IReq = BIM_READ32(0x160);
    *pu4IMiss = BIM_READ32(0x164);
    *pu4IPenalty = BIM_READ32(0x168);
    *pu4DReq = BIM_READ32(0x16c);
    *pu4DMiss = BIM_READ32(0x170);
    *pu4DPenalty = BIM_READ32(0x174);
#endif

    HalCriticalEnd(flag);

    HAL_GetDeltaTime(&td, &t1, &t2);
    Printf("total time is %d.%06d seconds\n", td.u4Seconds, td.u4Micros);

    return 0;
}


INT32 CpiMeterQuery(UINT32 u4Action, UINT64 *pu8Instruction, UINT64 *pu8Cycle)
{
    UINT32 cpi0, cpi1, cpi2, cpi3;
    if (u4Action == 2)
    {
        BIM_WRITE32(REG_RW_CPICFG, CPICFG_CLR);
    }
    else if (u4Action == 0)
    {
        BIM_WRITE32(REG_RW_CPICFG, 0);
    }
    else if (u4Action == 1)
    {
        BIM_WRITE32(REG_RW_CPICFG, CPICFG_EN);
    }

    cpi0 = BIM_READ32(REG_RW_CPI0);
    cpi1 = BIM_READ32(REG_RW_CPI1);
    cpi2 = BIM_READ32(REG_RW_CPI2);
    cpi3 = BIM_READ32(REG_RW_CPI3);

    Printf("ins.=%08x%08x, cycle=%08x%08x\n", cpi1, cpi0, cpi3, cpi2);
    *pu8Instruction = (UINT64)cpi0 + (UINT64)((UINT64)cpi1<<32);
    *pu8Cycle = (UINT64)cpi2 + (UINT64)((UINT64)cpi3<<32);

    return 0;
}

INT32 CmdEfusePowerPinOn(INT32 i4Argc, const CHAR** aszArgv)
{
    return 0;
}
INT32 CmdEfusePowerPinOff(INT32 i4Argc, const CHAR** aszArgv)
{
    return 0;
}

INT32 CmdEfuseSecureID(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4SID0, u4SID1, u4SID2, u4SID3;

    HAL_WRITE32(0x20008688, 0x883);
    HAL_WRITE32(0x20008688, 0x1404);

    u4SID1 = HAL_READ32(0x2000867C);
    u4SID3 = HAL_READ32(0x20008684);
    u4SID0 = HAL_READ32(0x20008678);
    u4SID2 = HAL_READ32(0x20008680);

    Printf("Security ID 0: 0x%x\n", u4SID0);
    Printf("Security ID 1: 0x%x\n", u4SID1);
    Printf("Security ID 2: 0x%x\n", u4SID2);
    Printf("Security ID 3: 0x%x\n", u4SID3);

    return 0;
}

INT32 CmdEfuseSecureData(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4SDa6, u4SDa7, u4SDa8, u4SDa9;

    HAL_WRITE32(0x20008688, 0x883);
    HAL_WRITE32(0x20008688, 0x1404);

    u4SDa7 = HAL_READ32(0x20008698);
    u4SDa9 = HAL_READ32(0x200086A0);
    u4SDa6 = HAL_READ32(0x20008694);
    u4SDa8 = HAL_READ32(0x2000869C);

    Printf("Security Data 6: 0x%x\n", u4SDa6);
    Printf("Security Data 7: 0x%x\n", u4SDa7);
    Printf("Security Data 8: 0x%x\n", u4SDa8);
    Printf("Security Data 9: 0x%x\n", u4SDa9);

    return 0;
}

INT32 CmdEfuseDump(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 au4Array[17];
    UINT32 i = 0;

    //Read 0x8660 ~ 0x8674
    for(i = 0; i < 6; i++)
        au4Array[i] = HAL_READ32(0x20008660 + i * 4);

    au4Array[10] = HAL_READ32(0x20008688);
    au4Array[11] = HAL_READ32(0x2000868C);
    au4Array[12] = HAL_READ32(0x20008690);

    HAL_WRITE32(0x20008688, 0x883);
    HAL_WRITE32(0x20008688, 0x1404);

    au4Array[7] = HAL_READ32(0x2000867C);
    au4Array[9] = HAL_READ32(0x20008684);
    au4Array[6] = HAL_READ32(0x20008678);
    au4Array[8] = HAL_READ32(0x20008680);

    HAL_WRITE32(0x20008688, 0x883);
    HAL_WRITE32(0x20008688, 0x1404);

    au4Array[14] = HAL_READ32(0x20008698);
    au4Array[16] = HAL_READ32(0x200086A0);
    au4Array[13] = HAL_READ32(0x20008694);
    au4Array[15] = HAL_READ32(0x2000869C);

    for(i = 0; i < 17;)
    {
        Printf("0x%08X  ", au4Array[i++]);
        if(i % 4 == 0)
        {
            Printf("\n");
        }
    }

    return 0;
}




