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
 * $RCSfile: $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_diag_ci.c
 *  Demux driver - TS_output for CI SLT program
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_mm.h"

#if defined(CC_DMX_EMULATION) || defined(__MODEL_slt__) || defined(CC_DMX_ENABLE_TSOUT)

#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_926.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_ckgen.h"
#include "x_pinmux.h"


LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
// This number of length will not be compared because some CI cards may drop
// some packets at the end of data transmission.
#define DMX_IGNORE_TRAILING_PACKETS         100
#define DMX_IGNORE_TRAILING_LEN             (188 * DMX_IGNORE_TRAILING_PACKETS)


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/** DMX_TsOut_SetLatchEdge
 *  negative means clock inversion, the clock rising edge is in the middle of data signal
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_SetLatchEdge(DMX_TSOUT_LATCH_EDGE_T eEdge)
{
    BOOL fgRet = FALSE;
    UINT32 u4Reg;

    switch (eEdge)
    {
    case DMX_TSOUT_LATCH_EDGE_POSITIVE:
        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg &= ~((UINT32)1 << 22);  // clear bit 22
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        fgRet = TRUE;
        break;

    case DMX_TSOUT_LATCH_EDGE_NEGATIVE:
        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg |= (UINT32)1 << 22;  // set bit 22
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        fgRet = TRUE;
        break;

    default:
        Printf("Incorrect TS-output latch edge!\n");
        fgRet = FALSE;
        break;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_TsOut_SetValidCycle
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_SetValidCycle(UINT32 u4Cycle)
{
    UINT32 u4Reg;

    if (u4Cycle >= 4)
    {
        Printf("Incorrect TS-output Valid cycle!\n");
        return FALSE;
    }

    u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
    u4Reg &= ~((UINT32)3 << 20);  // clear bit [21:20]
    u4Reg |= u4Cycle << 20;
    DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** DMX_TsOut_SetSyncCycle
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_SetSyncCycle(DMX_TSOUT_SYNC_CYCLE_T eSyncCycle)
{
    BOOL fgRet = FALSE;
    UINT32 u4Reg;

    switch (eSyncCycle)
    {
    case DMX_TSOUT_SYNC_CYCLE_8:
        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg &= ~((UINT32)1 << 19);  // clear bit 19
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        fgRet = TRUE;
        break;

    case DMX_TSOUT_SYNC_CYCLE_1:
        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg |= (UINT32)1 << 19;  // set bit 19
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        fgRet = TRUE;
        break;

    default:
        Printf("Incorrect TS-output Sync cycle!\n");
        fgRet = FALSE;
        break;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_TsOut_SetDataWidth
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_SetDataWidth(DMX_TSOUT_DATAWIDTH_T eDataWidth)
{
    BOOL fgRet = FALSE;
    UINT32 u4Reg;

    u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
    u4Reg &= ~0x02800000;

    switch (eDataWidth)
    {
    case DMX_TSOUT_DATAWIDTH_1:
        fgRet = TRUE;
        break;

    case DMX_TSOUT_DATAWIDTH_2:
        u4Reg |= 0x02800000;
        fgRet = TRUE;
        break;

    case DMX_TSOUT_DATAWIDTH_8:
        u4Reg |= 0x00800000;
        fgRet = TRUE;
        break;

    default:
        Printf("Incorrect TS-output endian mode!\n");
        fgRet = FALSE;
        break;
    }

    DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

    return fgRet;
}

//-----------------------------------------------------------------------------
/** DMX_TsOut_SetDataWidth
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_SetClockInvertGating(BOOL fgInvert, BOOL fgGating)
{
    UINT32 u4Reg;

#if defined(CC_MT5398)

    if (fgInvert)
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg |= (0x1 << 16);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

#ifdef CC_DMX_EMULATION //change framer sample edge default framer 0

        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);    // Framer
        if (((u4Reg >> 3) & 0x1) == 0x1)//positive
        {
            u4Reg &= ~(0x1 << 3);
            DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
        }
        else //negative
        {
            u4Reg |= (0x1 << 3);
            DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
        }

#endif
    }
    else
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg &= ~(0x1 << 16);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }

    if (fgGating)
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg |= (0x1 << 17);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }
    else
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg &= ~(0x1 << 17);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }

#elif defined(CC_MT5880)
    if (fgInvert)
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg |= (0x1 << 18);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

#ifdef CC_DMX_EMULATION //change framer sample edge default framer 0

        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);    // Framer
        if (((u4Reg >> 3) & 0x1) == 0x1)//positive
        {
            u4Reg &= ~(0x1 << 3);
            DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
        }
        else //negative
        {
            u4Reg |= (0x1 << 3);
            DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
        }

#endif
    }
    else
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg &= ~(0x1 << 18);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }

    if (fgGating)
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg |= (0x1 << 19);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }
    else
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg &= ~(0x1 << 19);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }

#elif defined(CC_MT5881)
    if (fgInvert)
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg |= (0x1 << 18);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

#ifdef CC_DMX_EMULATION //change framer sample edge default framer 0

        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);    // Framer
        if (((u4Reg >> 3) & 0x1) == 0x1)//positive
        {
            u4Reg &= ~(0x1 << 3);
            DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
        }
        else //negative
        {
            u4Reg |= (0x1 << 3);
            DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
        }

#endif
    }
    else
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg &= ~(0x1 << 18);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }

    if (fgGating)
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg |= (0x1 << 19);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }
    else
    {
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG);
        u4Reg &= ~(0x1 << 19);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
    }

#endif
    return TRUE;
}


BOOL DMX_TsIn_SetDataWidth(UINT8 u1TsIndex, DMX_TSOUT_DATAWIDTH_T eDataWidth)
{
    BOOL fgRet = FALSE;
    UINT32 u4Reg, u4Reg1, u4Reg2, u4Reg3;

    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= ~((UINT32)0xF << 24);

    //reset framer aynchornous ts formate
    u4Reg3 = DMXCMD_READ32(DMX_REG_FRAMER_CTRL3);
    u4Reg3 &= ~((UINT32)0x3);

    u4Reg1 = DMXCMD_READ32(DMX_REG_CONFIG1);
    u4Reg1 &= ~0x1C000000;

    u4Reg2 = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);    // Framer
    u4Reg2 &= ~0x222;

    switch (eDataWidth)
    {
    case DMX_TSOUT_DATAWIDTH_1:
        u4Reg2 |= 0xDDD;                       // serial input ,pos,external, enable
        fgRet = TRUE;
        break;

    case DMX_TSOUT_DATAWIDTH_2:
        // TS-in 2 bit
        if (u1TsIndex == 0)
        {
            u4Reg |= (UINT32)0x2 << 24;       // SYNC TS0
            u4Reg1 |= 0x04000000;
            u4Reg2 |= 0x0F;                   // parallel input enable framer 0
        }
        else if (u1TsIndex == 1)
        {
            u4Reg |= (UINT32)0x8 << 24;       // sync TS1
            u4Reg1 |= 0x08000000;
            u4Reg2 |= 0xF0;                   // parallel input enable framer 1

        }
        else if (u1TsIndex == 3)
        {
            u4Reg3 |= 0x2;                      // sync TS3
            u4Reg1 |= 0x10000000;
            u4Reg2 |= 0xF00;                   // parallel input enable framer 3

        }
        fgRet = TRUE;
        break;

    case DMX_TSOUT_DATAWIDTH_8:
        u4Reg2 |= 0xFFF;//0x222                           // parallel input
        fgRet = TRUE;
        break;

    default:
        Printf("Incorrect TS-output endian mode!\n");
        fgRet = FALSE;
        break;
    }

    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);
    DMXCMD_WRITE32(DMX_REG_CONFIG1, u4Reg1);
    DMXCMD_WRITE32(DMX_REG_FRAMER_CTRL3, u4Reg3);

    //disable framer before set framer again
    u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);
    u4Reg &= ~(0x111);
    DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);

    //set framer again
    DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg2);

    return fgRet;
}

//-----------------------------------------------------------------------------
/** DMX_TsOut_SetEndianess
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_SetEndianess(DMX_TSOUT_ENDIAN_T eEndian)
{
    BOOL fgRet = FALSE;
    UINT32 u4Reg;

    switch (eEndian)
    {
    case DMX_TSOUT_ENDIAN_MSB:
        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg &= ~((UINT32)1 << 18);  // clear bit 18
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        fgRet = TRUE;
        break;

    case DMX_TSOUT_ENDIAN_LSB:
        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg |= (UINT32)1 << 18;  // set bit 18
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        fgRet = TRUE;
        break;

    default:
        Printf("Incorrect TS-output endian mode!\n");
        fgRet = FALSE;
        break;
    }

    return fgRet;
}

BOOL DMX_SetTsOut(UINT8 u1TsIndex, DMX_TSOUT_PATH_T ePath)
{
    BOOL fgRet = FALSE;
    UINT32 u4Reg;

    switch (ePath)
    {
    case DMX_TSOUT_PATH_INTERNAL:
        if ((u1TsIndex == 0) || (u1TsIndex == 1) || (u1TsIndex == 3))
        {
            if (u1TsIndex == 0)
            {
                if (!DMX_PVRPlay_SetPort(DMX_PVRPLAY_PORT_FRAMER0))
                {
                    return FALSE;
                }
            }
            else if (u1TsIndex == 1)
            {
                if (!DMX_PVRPlay_SetPort(DMX_PVRPLAY_PORT_FRAMER1))
                {
                    return FALSE;
                }
            }
            else if (u1TsIndex == 3)
            {
                if (!DMX_PVRPlay_SetPort(DMX_PVRPLAY_PORT_FRAMER3))
                {
                    return FALSE;
                }
            }

            u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
            u4Reg |= (1 << 16);     // Enable TSOUT path
            DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
            _DMX_SetFrontEndEx(u1TsIndex, DMX_FE_TSOUT);
        }
        else if (u1TsIndex == 2)
        {
            if (!DMX_PVRPlay_SetPort(DMX_PVRPLAY_PORT_DBM))
            {
                return FALSE;
            }

            u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
            u4Reg |= (1 << 17);     // Enable DBM path
            DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        }
        else
        {
            Printf("Tsindex: 0x%d does not support tsout.\n", u1TsIndex);
            fgRet = FALSE;
        }
        fgRet = TRUE;
        break;

    case DMX_TSOUT_PATH_EXTERNAL:
        if (!DMX_PVRPlay_SetPort(DMX_PVRPLAY_PORT_TSOUT))
        {
            return FALSE;
        }

        if (!DMX_TsOut_SetLatchEdge(DMX_TSOUT_LATCH_EDGE_NEGATIVE))
        {
            return FALSE;
        }

        if (!DMX_TsOut_SetValidCycle(0))
        {
            return FALSE;
        }

        if (!DMX_TsOut_SetSyncCycle(DMX_TSOUT_SYNC_CYCLE_8))
        {
            return FALSE;
        }

        if (!DMX_TsOut_SetDataWidth(DMX_TSOUT_DATAWIDTH_1))
        {
            return FALSE;
        }

        if (!DMX_TsOut_SetEndianess(DMX_TSOUT_ENDIAN_MSB))
        {
            return FALSE;
        }

        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
        u4Reg |= (1 << 16);     // Enable TSOUT path
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_REG_EXTTS_CKCKG) & ~0x700;
        u4Reg |= 0x200; // 2: dmxout, 3: tsout, only demxout can config "positive/negative edge" and "serial/parallel" of TSOUT
        CKGEN_WRITE32(CKGEN_REG_EXTTS_CKCKG, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) & ~(1<<6);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        // tsout enable
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<13);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

#if defined(CC_MT5398)
        // pmux1[18:16], function 1
        // pmux1[22:20], function 1, not set in here now
        BSP_PinSet(PIN_PVR_TSCLK, 1);
        BSP_PinSet(PIN_PVR_TSDATA1, 1);
#elif defined(CC_MT5880)
        BSP_PinSet(PIN_CIGPIO48, 3);
        BSP_PinSet(PIN_CIGPIO45, 3);
        BSP_PinSet(PIN_CIGPIO34, 3);
        BSP_PinSet(PIN_CIGPIO36, 3);
        BSP_PinSet(PIN_CIGPIO37, 3);
        BSP_PinSet(PIN_CIGPIO41, 3);
#endif
        fgRet = TRUE;
        break;

    case DMX_TSOUT_PATH_DISABLE:
        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF; // clear dmx tsout
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

#if defined(CC_MT5398)
        // pmux1[18:16], function 0
        BSP_PinSet(PIN_PVR_TSCLK, 0);
        BSP_PinSet(PIN_PVR_TSDATA1, 0);
#elif defined(CC_MT5880)

        BSP_PinSet(PIN_CIGPIO48, 0);
        BSP_PinSet(PIN_CIGPIO45, 0);
        BSP_PinSet(PIN_CIGPIO34, 0);
        BSP_PinSet(PIN_CIGPIO36, 0);
        BSP_PinSet(PIN_CIGPIO37, 0);
        BSP_PinSet(PIN_CIGPIO41, 0);
#endif


        // tsout pad disable
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) & ~(1<<13);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        // recover to demod
        u4Reg = CKGEN_READ32(0x2B4) & ~0x700;
        CKGEN_WRITE32(0x2B4, u4Reg);

        break;
    case DMX_TSOUT_STEER_DBM:
        if (u1TsIndex == 2)
        {
            if (!DMX_PVRPlay_SetPort(DMX_PVRPLAY_PORT_DBM))
            {
                return FALSE;
            }

            u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
            u4Reg |= (1 << 17);     // Enable DBM path
            DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

            fgRet = TRUE;
        }
        else
        {
            fgRet = FALSE;
        }
        break;

    case DMX_TSOUT_STEER_TSOUT:
        if (!DMX_PVRPlay_SetPort(DMX_PVRPLAY_PORT_TSOUT))
        {
            return FALSE;
        }

        // set port to tsout is set to parallel, need to change back to serial
        if (!DMX_TsOut_SetDataWidth(DMX_TSOUT_DATAWIDTH_1))
        {
            return FALSE;
        }

        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
        u4Reg |= (1 << 16);     // Enable TSOUT path
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        fgRet = TRUE;
        break;
    default:
        Printf("Incorrect TS-output route!\n");
        fgRet = FALSE;
    }

    return fgRet;
}

#if defined(CC_DMX_EMULATION) || defined(__MODEL_slt__)
//-----------------------------------------------------------------------------
/** DMX_TsOut_SetRoute
 * _DMX_SetBypassMode() resets the registers in DMX.  Be sure to call it before
 * other operations are called.  This function must be called before the other
 * four functions above are called.
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_SetRoute(UINT8 u1TsIndex, DMX_TSOUT_ROUTE_T eRoute)
{
    BOOL fgRet = FALSE;
    UINT32 u4Reg;

    switch (eRoute)
    {
    case DMX_TSOUT_ROUTE_DBM_LOOPBACK:
        if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE))
        {
            Printf("Failed to set bypass mode\n");
            return FALSE;
        }

        if ((u1TsIndex == 0) || (u1TsIndex == 1) || (u1TsIndex == 3))
        {
            _DMX_SetFrontEndEx(u1TsIndex, DMX_FE_TSOUT);

            //disable PES/DBM path
            u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
            DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        }
        else if (u1TsIndex == 2)
        {
            u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL) & 0xFFFCFFFF;
            u4Reg |= (1 << 17);     // Enable PES/DBM path
            DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);
        }
        else
        {
            Printf("Tsindex: 0x%d does not support tsout.\n", u1TsIndex);
            return FALSE;
        }
        fgRet = TRUE;
        break;

    case DMX_TSOUT_ROUTE_EXT_LOOPBACK:
        if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE))
        {
            Printf("Failed to set bypass mode\n");
            return FALSE;
        }

        ///-----------------------------------
        //  0x304 --> REG_TSOUT_CKCFG
        //   0x158 --> CI_TSIF_CFG
        //   0X160 --> EXTTS_CKCKG
        CKGEN_WRITE32(CKGEN_TSOUT_CKCFG, 0x3);   // tsout clk: xtal  0: 24M  ,3:54M

        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) & ~(1<<6);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

#if defined(CC_MT5398)
        // pmux1[18:16], pmux1[22:20], function 1

        BSP_PinSet(PIN_PVR_TSCLK, 1);
        BSP_PinSet(PIN_PVR_TSDATA1, 1);

        // tsout enable
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<13);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_REG_EXTTS_CKCKG) & ~0x700;
        u4Reg |= 0x200; // 2: dmxout, 3: tsout, only demxout can config "positive/negative edge" and "serial/parallel" of TSOUT
        CKGEN_WRITE32(CKGEN_REG_EXTTS_CKCKG, u4Reg);

#elif defined(CC_MT5880)
#if 0
        //-----------------case 1  parallel out--------------------
        BSP_PinSet(PIN_CIGPIO48, 3);
        BSP_PinSet(PIN_CIGPIO45, 3);
        BSP_PinSet(PIN_CIGPIO34, 3);
        BSP_PinSet(PIN_CIGPIO36, 3);
        BSP_PinSet(PIN_CIGPIO37, 3);
        BSP_PinSet(PIN_CIGPIO41, 3);

#endif

#if 0
        //-----case 2  serial out(DMX_PINMUX_EXT_2_S --- outputpin)--------------------
        BSP_PinSet(PIN_CIGPIO48, 3);
        BSP_PinSet(PIN_CIGPIO45, 3);
        BSP_PinSet(PIN_CIGPIO34, 3);

        //clk gating and  clk invert
#if 0
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<16); //0xD158[16]  1: gating
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<17); //0xD158[17]  1: invert
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
#endif

#endif

    #if 1
        //-----case3  serial out(DMX_PINMUX_EXT2_S --- outputpin)--------------------
        // tsout enable
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (3<<14);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_REG_EXTTS_CKCKG) & ~0x7700;
        u4Reg |= 0x2000; // 2: dmxout, 3: tsout
        CKGEN_WRITE32(CKGEN_REG_EXTTS_CKCKG, u4Reg);

        //pinmux
        BSP_PinSet(PIN_CIGPIO36, 3); // out  ts clk select function 3
        BSP_PinSet(PIN_CIGPIO37, 3);

        //clk gating and  clk invert
        #if 0
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<18); //0xD158[18]  1: invert
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<19); //0xD158[19]  1: gating
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
        #endif

    #endif
#elif defined(CC_MT5881)
#if 0
            //-----------------case 1  parallel out--------------------
            BSP_PinSet(PIN_CIGPIO48, 3);
            BSP_PinSet(PIN_CIGPIO45, 3);
            BSP_PinSet(PIN_CIGPIO34, 3);
            BSP_PinSet(PIN_CIGPIO36, 3);
            BSP_PinSet(PIN_CIGPIO37, 3);
            BSP_PinSet(PIN_CIGPIO41, 3);

#endif

#if 0
            //-----case 2  serial out(DMX_PINMUX_EXT_2_S --- outputpin)--------------------
            BSP_PinSet(PIN_CIGPIO48, 3);
            BSP_PinSet(PIN_CIGPIO45, 3);
            BSP_PinSet(PIN_CIGPIO34, 3);

            //clk gating and  clk invert
#if 0
            u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<16); //0xD158[16]  1: gating
            CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

            u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<17); //0xD158[17]  1: invert
            CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
#endif

#endif

    #if 1
            //-----case3  serial out(DMX_PINMUX_EXT2_S --- outputpin)--------------------
            // tsout enable
            u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (3<<14);
            CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

            u4Reg = CKGEN_READ32(CKGEN_REG_EXTTS_CKCKG) & ~0x7700;
            u4Reg |= 0x2000; // 2: dmxout, 3: tsout
            CKGEN_WRITE32(CKGEN_REG_EXTTS_CKCKG, u4Reg);

            //pinmux
            BSP_PinSet(PIN_CIGPIO36, 3); // out  ts clk select function 3
            BSP_PinSet(PIN_CIGPIO37, 3);
            BSP_PinSet(PIN_CIGPIO38, 3);
            BSP_PinSet(PIN_CIGPIO39, 3);

            //clk gating and  clk invert
        #if 0
            u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<18); //0xD158[18]  1: invert
            CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

            u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<19); //0xD158[19]  1: gating
            CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);
        #endif

    #endif


#endif

#if 0
        if ((u1TsIndex == 0) || (u1TsIndex == 1))
        {
            // Not implement yet
#if defined(CC_MT5398)
            _DMX_SetFrontEndEx(u1TsIndex, DMX_FE_EXT2_1_S);
#endif
            ASSERT(0);
        }
        else
        {
            Printf("Tsindex: 0x%d does not support tsout.\n", u1TsIndex);
            return FALSE;
        }
#endif

        fgRet = TRUE;

        break;

    case DMX_TSOUT_ROUTE_EXTERNAL_CI:
        if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE))
        {
            Printf("Failed to set bypass mode\n");
            return FALSE;
        }

        if ((u1TsIndex == 0) || (u1TsIndex == 1))
        {
            //_DMX_SetFrontEndEx(u1TsIndex, DMX_FE_EXT_P);
            _DMX_SetFrontEndEx(u1TsIndex, DMX_FE_EXT_S);
        }

        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg |= (UINT32)1 << 16;       // Enable TS-output path
        u4Reg &= ~((UINT32)1 << 17);    // Disable PES/DBM path
        u4Reg &= ~((UINT32)1 << 23);    // TS serial mode
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

        ///-----------------------------------
        //  0x304 --> REG_TSOUT_CKCFG
        //   0x158 --> CI_TSIF_CFG
        //   0X160 --> EXTTS_CKCKG
        CKGEN_WRITE32(CKGEN_TSOUT_CKCFG, 0);   // tsout clk: xtal

        u4Reg = CKGEN_READ32(CKGEN_REG_EXTTS_CKCKG) & ~0x700;
        u4Reg |= 0x200; // 2: dmxout, 3: tsout, only demxout can config "positive/negative edge" and "serial/parallel" of TSOUT
        CKGEN_WRITE32(CKGEN_REG_EXTTS_CKCKG, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) & ~(1<<6);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        // tsout enable
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<13);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        fgRet = TRUE;
        break;

    case DMX_TSOUT_ROUTE_INTERNAL_CI:
        if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE))
        {
            Printf("Failed to set bypass mode\n");
            return FALSE;
        }

        if ((u1TsIndex == 0) || (u1TsIndex == 1))
        {
            _DMX_SetFrontEndEx(u1TsIndex, DMX_FE_CI);
        }

        u4Reg = DMXCMD_READ32(DMX_REG_FRAMER_CONTROL);
        u4Reg = (u4Reg & 0xFFFF00FF) | 0x0000FF00;  // reset Framer
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);
        u4Reg &= 0xFFFF00FF;                        // stop Framer reset
        DMXCMD_WRITE32(DMX_REG_FRAMER_CONTROL, u4Reg);

        // Disable both Framer's error handling mechanism
        DMXCMD_WRITE32(DMX_REG_FRAMER_ERROR_HANDLE, 0x0);

        // Set the TS-index to be bypassed.
        u4Reg = DMXCMD_READ32(DMX_REG_DBM_CONTROL) & ~(0x3C000000);
        u4Reg |= 1 << (u1TsIndex + 26);
        DMXCMD_WRITE32(DMX_REG_DBM_CONTROL, u4Reg);

        u4Reg = DMXCMD_READ32(DMX_REG_PES_DBM_STEER_CTRL);
        u4Reg |= (UINT32)1 << 16;       // Enable TS-output path
        u4Reg &= ~((UINT32)1 << 17);    // Disable PES/DBM path
        u4Reg |= (UINT32)1 << 23;       // TS parallel mode
        DMXCMD_WRITE32(DMX_REG_PES_DBM_STEER_CTRL, u4Reg);

        ///-----------------------------------
        //  0x304 --> REG_TSOUT_CKCFG
        //   0x158 --> CI_TSIF_CFG
        //   0X160 --> EXTTS_CKCKG
        CKGEN_WRITE32(CKGEN_TSOUT_CKCFG, 0);   // tsout clk: xtal

        u4Reg = CKGEN_READ32(CKGEN_REG_EXTTS_CKCKG) & ~0x700;
        u4Reg |= 0x200; // 2: dmxout, 3: tsout, only demxout can config "positive/negative edge" and "serial/parallel" of TSOUT
        CKGEN_WRITE32(CKGEN_REG_EXTTS_CKCKG, u4Reg);

        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) & ~(1<<6);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        // tsout enable
        u4Reg = CKGEN_READ32(CKGEN_TSIF_CFG) | (1<<13);
        CKGEN_WRITE32(CKGEN_TSIF_CFG, u4Reg);

        fgRet = TRUE;
        break;

    default:
        Printf("Incorrect TS-output route!\n");
        return FALSE;
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_TsOut_TransmitData
 */
//-----------------------------------------------------------------------------
BOOL DMX_TsOut_TransmitData(UINT8 u1TsIndex, UINT32 u4Size)
{
    BOOL fgRet = FALSE;
    UINT32 u4SrcBufSize, u4DstBufSize;
    UINT32 u4SrcAddr = 0, u4SrcDiff, u4SrcSize;
    UINT32 u4DstAddr = 0, u4DstDiff, u4DstEndAddr;
    UINT32 u4PesAddr = 0, u4PesSize;
    UINT32 u4Count;
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4Flags;
    UINT32 u4Rp, u4Wp, u4Reg;
    UINT32 i;

    if (u4Size <= DMX_IGNORE_TRAILING_LEN)
    {
        Printf("Length %u is too short!\n", u4Size);
        return FALSE;
    }

    // Init PVR
    if (!_DMX_PVRPlay_Init())
    {
        Printf("Failed to initialize PVR!\n");
        return FALSE;
    }

    if ((u1TsIndex == 0) || (u1TsIndex == 1) || (u1TsIndex == 3))
    {
        _DMX_PVRPlay_SetTSOut(TRUE);
    }

    if (u1TsIndex == 3)
    {
        //Set ExternalFramer DMX_EXT_FRAMER_2_DDI_3_EXT
        _DMX_SetExternalFramer(DMX_EXT_FRAMER_2_DDI_3_EXT);
    }

    // Allocate source buffer.  (Mini-PVR RP/WP 8-byte alignment, 188 * 4 = 752)
    u4SrcSize = _DMX_Align(u4Size, 752);
    u4SrcBufSize = u4SrcSize + DMX_CA_BUFFER_ALIGNMENT;
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4SrcBufSize, DMX_CA_BUFFER_ALIGNMENT);
    if (u4SrcAddr == 0)
    {
        Printf("Failed to allocate source buffer!\n");
        return FALSE;
    }

    // Allocate destination buffer
    // It's important to make the CA output buffer slightly larger.
    u4DstBufSize = _DMX_Align(u4SrcBufSize + 1000, DMX_CA_BUFFER_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstBufSize, DMX_CA_BUFFER_ALIGNMENT);
    if (u4DstAddr == 0)
    {
        Printf("Failed to allocate destination buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }
    for ( i = 0 ; i < u4DstBufSize/4 ; i++ )
    {
        ((UINT32*)u4DstAddr)[i] = 0;
    }
    u4DstEndAddr = u4DstAddr + u4DstBufSize;

    // Set PVR playback
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = u4SrcAddr;
    rPlay.u4BufSize = u4SrcBufSize;
    rPlay.u4ThresholdSize = u4SrcSize;  // FIXME
    rPlay.pfnPVRNotify = NULL;
    u4Flags = PVRPLAY_FLAGS_BUFFER | PVRPLAY_FLAGS_THRESHOLD |
              PVRPLAY_FLAGS_CALLBACK;
    if (!_DMX_PVRPlay_Set(u4Flags, &rPlay))
    {
        Printf("Failed to set PVR properties!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, u4DstAddr, u4DstEndAddr, u4SrcSize))
    {
        Printf("Failed to set CA output buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    _GenerateRandomPattern(u4SrcAddr, u4SrcSize);

    u4PesAddr = (UINT32)x_mem_alloc(u4SrcSize);
    if (u4PesAddr == 0)
    {
        Printf("Failed to allocate payload buffer!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    // The input to 8295 needs to be in TS packet format.
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4SrcSize, 0, u4PesAddr,
                                 &u4PesSize))
    {
        Printf("Failed to generate TS packets!\n");
        fgRet = FALSE;
        goto FailAndFreeMemory;
    }

    HalFlushInvalidateDCache();

    Printf("Src address: 0x%08x\n", u4SrcAddr);
    Printf("Dst address: 0x%08x\n", u4DstAddr);
    Printf("Original Src size: %u.  New Src Size: %u\n", u4Size, u4SrcSize);

    // Set Full pull mode
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg |= 0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    VERIFY(_DMX_PVRPlay_FlushBuf());
    VERIFY(_DMX_PVRPlay_UpdateWp(u4SrcAddr, u4SrcSize));
    VERIFY(_DMX_PVRPlay_Start());

    // Wait for some time.
    // x_thread_delay(3000);
    u4Count = 0;
    while (1)
    {
        u4Wp = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_WP);
        u4Rp = DMXCMD_READ32(DMX_REG_CA_OUT_BUF_RP);
        if ((u4Wp - u4Rp) >= (u4SrcSize - DMX_IGNORE_TRAILING_LEN))  // Do not consider wrap around for now.
        {
            break;
        }
        if (((++u4Count) >= 300))  // two outmost pairs of parenthesis are necessary
        {
            Printf("Exceed loop count %u!\n", u4Count);
            fgRet = FALSE;
            goto FailAndFreeMemory;
        }
        x_thread_delay(1);
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr,
                 u4SrcSize - DMX_IGNORE_TRAILING_LEN, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }
    else
    {
        fgRet = TRUE;
    }

FailAndFreeMemory:
    VERIFY(_DMX_PVRPlay_Stop(TRUE));
    VERIFY(_DMX_PVRPlay_FreeBuf());
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    if (u4PesAddr != 0)
    {
        x_mem_free((void*)u4PesAddr);
    }

    // Set back
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= ~0xC0000000;
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    return fgRet;
}
#endif  // defined(CC_DMX_EMULATION) || defined(__MODEL_slt__)
#endif  // defined(CC_DMX_EMULATION) || defined(__MODEL_slt__) || defined(CC_DMX_ENABLE_TSOUT)


