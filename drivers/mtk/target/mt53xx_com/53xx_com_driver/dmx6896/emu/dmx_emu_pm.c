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
 * $RCSfile: dmx_emu_pm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_pm.c
 *  Demux driver - Pattern Match emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "../dmx_mm.h"
#include "x_lint.h"

//LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_arm.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_bim.h"

//LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

// Make DST buffer slightly larger than SRC.
// Make sure the Source Buffer size is a multiple of 188 bytes.
#define DMX_EMU_PM_SRC_BUF_SIZE             0x0E174
#define DMX_EMU_PM_DST_BUF_SIZE             0x10000

// If PM_START_CODE_AT_HEAD is defined, a start code pattern will be generated
// at the head of the bit stream.
#define PM_START_CODE_AT_HEAD

// The maximum length of a start code pattern: MAX(4, 6) = 6.
#define PM_MAX_CODE_LEN             8// 6

// The Pattern/Mask DMEM can only accommodate at most ten start code patterns.
#define PM_MAX_SAMPLES              (10)

// Users can change this maximum number of start code patterns.
#define PM_MAX_NUM_START_CODE       100

#define PES_HEADER_ALIGNMENT        4
#define PES_HEADER_EXTRA_BYTES      12
#define PM_OFFSET_VERIFIED          0xFFFFFFFF


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    UINT8 u1Code[PM_MAX_CODE_LEN];
} PM_START_CODE_T;


typedef struct
{
    UINT8 u1Code[PM_MAX_CODE_LEN];
    UINT32 u4Offset;
    UINT32 u4Type;
} PM_CODE_OFFSET_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fgPMOverlappedPatterns;

// The sample space and the valid number of samples in the space.
static UINT8 _u1PMValidSamples;
static PM_START_CODE_T _aPMSampleSpace[PM_MAX_SAMPLES];

// The "event space" of the start code patterns.
static PM_CODE_OFFSET_T _aPMEventSpace[PM_MAX_NUM_START_CODE];

// The patterns detected by Demux and the associated number of patterns.
static UINT8 _u1PMDetectedCount;
static PM_CODE_OFFSET_T _aPMDetectedSpace[PM_MAX_NUM_START_CODE];

static UINT8 _u1CodeLen;

static UINT32 _u4MMatchCount = 0;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/* PM_GenCode
 */
//-----------------------------------------------------------------------------
static void PM_GenCode(UINT8 *pu1Code, UINT8 u1CodeLen)
{
    UINT8 u1Index = 0;
    //static UINT8 _u1Pat = 0;

    // The first two bytes are 0x0000 when code length is 6.    
    if (u1CodeLen == 6)
    {
        pu1Code[u1Index++] = 0;
        pu1Code[u1Index++] = 0;
    }

    while (u1Index < u1CodeLen)
    {
        if(u1Index == 0)
        {
            pu1Code[u1Index++] = (UINT8)random(0xFE);
        }
        else
        {
            pu1Code[u1Index++] = (UINT8)random(0x100); // eddie_test
            //pu1Code[u1Index++] = _u1Pat++;//(UINT8)random(0x100);
        }
    }
}


//-----------------------------------------------------------------------------
/* PM_IsCodeMatched
 */
//-----------------------------------------------------------------------------
static BOOL PM_IsCodeMatched(UINT8 *pu1Code1, UINT8 *pu1Code2, UINT8 u1CodeLen)
{
    UINT8 u1Index;

    for (u1Index = 0; u1Index < u1CodeLen; u1Index++)
    {
        if (pu1Code1[u1Index] != pu1Code2[u1Index])
        {
            return FALSE;
        }
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/* PM_IsTypeMatched
 */
//-----------------------------------------------------------------------------
static BOOL PM_IsTypeMatched(UINT8 *pu1Code1, UINT32 u4Type, UINT8 u1CodeLen)
{
    UINT8 u1No, u1Index;
    UINT8 u1Count = 0;

    for (u1No = 0; u1No < PM_MAX_SAMPLES; u1No++)
    {
        for (u1Index = 0; u1Index < u1CodeLen; u1Index++)
        {
            if (pu1Code1[u1Index] != _aPMSampleSpace[u1No].u1Code[u1Index])
            {
                break;
            }
        }

        if(u1Index == u1CodeLen)
        {
            u1Count++;
            u4Type &= ~ (1<<u1No);
        }
    }

    if(u1Count > 1)
    {
        _u4MMatchCount++;
    }

    if(u4Type == 0)
    {
        return TRUE;
    }
    
    return FALSE;
}


//-----------------------------------------------------------------------------
/* PM_GenerateDistinctCode
 */
//-----------------------------------------------------------------------------
static void PM_GenerateDistinctCode(PM_START_CODE_T *pSampleSpace,
                        UINT8 u1ValidSamples, UINT8 *pu1Code, UINT8 u1CodeLen)
{
    UINT8 u1No;

    PM_GenCode(pu1Code, u1CodeLen);
    u1No = 0;
    while (u1No < u1ValidSamples)
    {
        if (PM_IsCodeMatched(pSampleSpace[u1No].u1Code, pu1Code, u1CodeLen))
        {
            PM_GenCode(pu1Code, u1CodeLen);
            u1No = 0;
        }
        else
        {
            u1No++;
        }
    }
}


//-----------------------------------------------------------------------------
/* PM_GenerateSampleSpace
 *
 * There are two start codes having the pattern "OO XX XX OO" in the following
 * sequence: OO XX XX OO XX XX OO.  They are called overlapped start codes here.
 * It can be easily seen that the first byte and the last byte in an overlapped
 * start codes are the same.
 */
//-----------------------------------------------------------------------------
static void PM_GenerateSampleSpace(UINT8 u1CodeLen)
{
    UINT8 u1No, u1MMatch;

    //_fgPMOverlappedPatterns = (random(2) ? TRUE : FALSE);
    _fgPMOverlappedPatterns = !_fgPMOverlappedPatterns;

    _u1PMValidSamples = 0;

    for (u1No = _u1PMValidSamples; u1No < PM_MAX_SAMPLES; u1No++)
    {
        PM_GenerateDistinctCode(_aPMSampleSpace, u1No, _aPMSampleSpace[u1No].u1Code, u1CodeLen);
        if (_fgPMOverlappedPatterns && (u1No == 0))
        {
            _aPMSampleSpace[u1No].u1Code[u1CodeLen - 1] = _aPMSampleSpace[u1No].u1Code[0];
        }
    }

    _u1PMValidSamples = u1No;

    // Test Multiple match
    u1MMatch = random(PM_MAX_SAMPLES - 1) + 1;
    for(u1No=0; u1No<u1CodeLen; u1No++)
    {
        _aPMSampleSpace[u1MMatch].u1Code[u1No] = _aPMSampleSpace[0].u1Code[u1No];
    }
}


//-----------------------------------------------------------------------------
/* PM_CopyCode
 *
 * Copy start codes to the global structure _aPMEventSpace and "source buffer".
 * The caller of this function have to check the address range of the input
 * arguments.
 */
//-----------------------------------------------------------------------------
static void PM_CopyCode(UINT8* pu1Code, UINT8 u1CodeLen, UINT8 u1No,
                        UINT32 u4SrcAddr, UINT32 u4Offset)
{
    UINT8 u1Index;

    for (u1Index = 0; u1Index < u1CodeLen; u1Index++)
    {
        _aPMEventSpace[u1No].u1Code[u1Index]   = pu1Code[u1Index];
        ((UINT8*)u4SrcAddr)[u4Offset + u1Index] = pu1Code[u1Index];
    }
}


//-----------------------------------------------------------------------------
/* PM_IsOffsetOverlapped
 *
 * Check for overlapped offset.
 */
//-----------------------------------------------------------------------------
static BOOL PM_IsOffsetOverlapped(UINT8 u1No, UINT32 u4Offset, UINT8 u1CodeLen)
{
    UINT8 n;
    UINT32 u4MaxRange, u4MinRange;

    u4MaxRange = u4Offset + u1CodeLen - 1;
    u4MinRange = (u4Offset < u1CodeLen) ? 0 : (u4Offset - u1CodeLen + 1);

    for (n = 0; n < u1No; n++)
    {
        if ((_aPMEventSpace[n].u4Offset <= u4MaxRange) &&
            (_aPMEventSpace[n].u4Offset >= u4MinRange))
        {
            return TRUE;        // Overlapped start codes
        }
    }
    return FALSE;
}


//-----------------------------------------------------------------------------
/* PM_SampleStartCodes
 *
 * Generate start codes and copy them to the global structure _aPMEventSpace and
 * "source buffer".
 */
//-----------------------------------------------------------------------------
static void PM_SampleStartCodes(UINT32 u4SrcAddr, UINT32 u4SrcSize,
                                UINT8 u1EventCount, UINT8 u1CodeLen)
{
    UINT32 u4Offset;
    UINT8 u1No, u1SampleIndex;

    ASSERT((2 <= u1EventCount) && (u1EventCount <= PM_MAX_NUM_START_CODE));
    ASSERT((PM_MAX_NUM_START_CODE * u1CodeLen) < u4SrcSize);

    for (u1No = 0; u1No < u1EventCount; u1No++)
    {
        u1SampleIndex = random(_u1PMValidSamples);
        if (u1No == 0)
        {
            // May generate overlapped start code in this case
            if (_fgPMOverlappedPatterns)
            {
                u1SampleIndex = 0;

#if defined(PM_START_CODE_AT_HEAD)
                u4Offset = 0;
#else
                u4Offset = random(u4SrcSize - (2 * u1CodeLen));
#endif  // PM_START_CODE_AT_HEAD

                _aPMEventSpace[u1No].u4Offset = u4Offset;
                PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code, u1CodeLen, u1No, u4SrcAddr, u4Offset);

                u1No++;
                u4Offset += u1CodeLen - 1;
                _aPMEventSpace[u1No].u4Offset = u4Offset;
                PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code, u1CodeLen, u1No, u4SrcAddr, u4Offset);
            }
            else
            {
                u4Offset = random(u4SrcSize - u1CodeLen);
                _aPMEventSpace[u1No].u4Offset = u4Offset;
                PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code, u1CodeLen, u1No, u4SrcAddr, u4Offset);
            }
        }
        else
        {
            do {
                u4Offset = random(u4SrcSize - u1CodeLen);
            } while (PM_IsOffsetOverlapped(u1No, u4Offset, u1CodeLen));

            _aPMEventSpace[u1No].u4Offset = u4Offset;
            PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code, u1CodeLen, u1No, u4SrcAddr, u4Offset);
        }
    }
}


//-----------------------------------------------------------------------------
/* PM_IsFakeStartCode
 *
 * Check for fake start code.
 * @retval  TRUE        the start code is fake.
 * @retval  FALSE       the start code is not fake.
 */
//-----------------------------------------------------------------------------
static BOOL PM_IsFakeStartCode(UINT8 *pu1SrcAddr, UINT32 u4Offset,
                               UINT8 u1EventCount, UINT8 u1CodeLen)
{
    UINT8 u1No;
    BOOL fgFake = FALSE;

    for (u1No = 0; u1No < u1EventCount; u1No++)
    {
        if (PM_IsCodeMatched(&(pu1SrcAddr[u4Offset]), _aPMEventSpace[u1No].u1Code, u1CodeLen))
        {
            if (_aPMEventSpace[u1No].u4Offset != u4Offset)
            {
                fgFake = TRUE;
                continue;
            }
            else
            {
                fgFake = FALSE;
                break;
            }
        }
    }

    if (fgFake)
    {
        Printf("Found fake start code at 0x%x.\n", (pu1SrcAddr + u4Offset));
    }

    return fgFake;
}


//-----------------------------------------------------------------------------
/* PM_DestoryFakeStartCodes
 *
 * Destroy fake start codes.
 */
//-----------------------------------------------------------------------------
static void PM_DestoryFakeStartCodes(UINT32 u4SrcAddr, UINT32 u4SrcSize,
                                     UINT8 u1EventCount, UINT8 u1CodeLen)
{
    UINT8 *pu1SrcAddr;
    UINT32 u4Offset;

    ASSERT(u1CodeLen < u4SrcSize);

    pu1SrcAddr = (UINT8*)u4SrcAddr;
    u4Offset = 0;
    while (u4Offset <= (u4SrcSize - u1CodeLen))
    {
        if (PM_IsFakeStartCode(pu1SrcAddr, u4Offset, u1EventCount, u1CodeLen))
        {
            UINT8 u1DemiseIndex = random(u1CodeLen);

            pu1SrcAddr[u4Offset + u1DemiseIndex] = random(0x100);
            // Move backward and search again.
            if (u4Offset >= (u1CodeLen - (1 + u1DemiseIndex)))
            {
                u4Offset -= (u1CodeLen - (1 + u1DemiseIndex));
            }
            else
            {
                u4Offset = 0;
            }
            continue;
        }
        u4Offset++;
    }
}


#ifdef PM_DEBUG
//-----------------------------------------------------------------------------
/* PM_DebugStartCodes
 */
//-----------------------------------------------------------------------------
static void PM_DebugStartCodes(UINT32 u4SrcAddr, UINT32 u4SrcSize,
                               UINT8 u1EventCount, UINT8 u1CodeLen)
{
    UINT32 u4Offset;
    UINT8 u1No, index, *pu1SrcAddr = (UINT8*)u4SrcAddr;

    for (u4Offset = 0; u4Offset <= (u4SrcSize - u1CodeLen); u4Offset++)
    {
        for (u1No = 0; u1No < u1EventCount; u1No++)
        {
            if (PM_IsCodeMatched(&(pu1SrcAddr[u4Offset]), _aPMEventSpace[u1No].u1Code, u1CodeLen))
            {
                Printf("Start code: 0x");
                for (index = 0; index < u1CodeLen; index++)
                {
                    Printf("%02X", _aPMEventSpace[u1No].u1Code[index]);
                }
                Printf(", Offset: 0x%08X\n", u4Offset);
                break;
            }
        }
    }
}
#endif  // PM_DEBUG


//-----------------------------------------------------------------------------
/* PM_SetPatternMask
 *
 * The current micro-code implementation will trigger an interrupt whenever
 * a start code is found.  If the micro-code implementation is changed, it
 * will be necessary to search in _aPMEventSpace for the greatest value of
 * offset and set the corresponding start code to the Interruptable
 * pattern/mask.
 */
//-----------------------------------------------------------------------------
static void PM_SetPatternMask(UINT8 u1TsIndex, UINT8 u1Pidx, UINT8 u1CodeLen)
{
    UINT8 u1No, u1Index;
    UINT32 u4Word;
    UINT16 u2Control;

    u2Control = 0x0;

    u1Index = (u1CodeLen == 6) ? 2 : 0;

    for (u1No = 0; u1No < PM_MAX_SAMPLES; u1No++)
    {
        if((u1CodeLen == 4) || (u1CodeLen == 6))
        {
            u4Word = ((UINT32)(_aPMSampleSpace[u1No].u1Code[u1Index + 0])) << 24 |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[u1Index + 1])) << 16 |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[u1Index + 2])) << 8  |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[u1Index + 3]));

            DMX_VCODE_S_W(u1No, 0) = 0;            // Pattern0
            DMX_VCODE_S_W(u1No, 1) = u4Word;       // Pattern1
            if(u1CodeLen == PM_MAX_CODE_LEN)
            {
                DMX_VCODE_S_W(u1No, 2) = 0xFFFF;     // Mask0, currently, 6byte pattern always start from 0000
            }
            else
            {
                DMX_VCODE_S_W(u1No, 2) = 0;            // Mask0
            }
            DMX_VCODE_S_W(u1No, 3) = 0xFFFFFFFF; // Mask1
            u2Control |= (1 << u1No);
        }
        else if(u1CodeLen == PM_MAX_CODE_LEN)
        {
            u4Word = ((UINT32)(_aPMSampleSpace[u1No].u1Code[0])) << 24 |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[1])) << 16 |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[2])) << 8  |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[3]));
            DMX_VCODE_S_W(u1No, 0) = u4Word;       // Pattern0

            u4Word = ((UINT32)(_aPMSampleSpace[u1No].u1Code[4])) << 24 |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[5])) << 16 |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[6])) << 8  |
                     ((UINT32)(_aPMSampleSpace[u1No].u1Code[7]));
            DMX_VCODE_S_W(u1No, 1) = u4Word;       // Pattern1

            DMX_VCODE_S_W(u1No, 2) = 0xFFFFFFFF; // Mask1
            DMX_VCODE_S_W(u1No, 3) = 0xFFFFFFFF; // Mask1
            u2Control |= (1 << u1No);
        }
        else
        {
            ASSERT(0);
        }
     }

    // Write 0x1 to the "pattern changed" byte.
    DMX_VCODE_W(1) = ((UINT32)u2Control << 16) | 0xFFFF;

    // Set all the bytes in "Previous Payload Bytes" to 0xFF.
    PID_S_W(u1Pidx, 4)   = 0xFFFFFFFF;
    PID_S_W(u1Pidx, 18) |= 0xFFFFFFFF;      // This line is optional.

    DMX_VCODE_W(0) = 3 << (8 * u1TsIndex);     // User defined
 
    // FIXME: Clear the "Record Pattern Match" region.
}


//-----------------------------------------------------------------------------
/* PM_InsertCodeAndOffset
 */
//-----------------------------------------------------------------------------
static void PM_InsertCodeAndOffset(UINT32 u4Offset, UINT32 u4FrameAddr, UINT8 u1CodeLen, UINT32 u4Type)
{
    UINT8 u1Index, *pu1Data;

    ASSERT(_u1PMDetectedCount < PM_MAX_NUM_START_CODE);
    ASSERT(u4Offset != PM_OFFSET_VERIFIED);

    pu1Data = (UINT8*)u4FrameAddr;

    for (u1Index = 0; u1Index < u1CodeLen; u1Index++)
    {
        _aPMDetectedSpace[_u1PMDetectedCount].u1Code[u1Index] = pu1Data[u1Index];
    }
    _aPMDetectedSpace[_u1PMDetectedCount].u4Offset = u4Offset;
    _aPMDetectedSpace[_u1PMDetectedCount].u4Type = u4Type;
    _u1PMDetectedCount++;
}


//-----------------------------------------------------------------------------
/* PM_VerifyStartCodes
 */
//-----------------------------------------------------------------------------
static BOOL PM_VerifyStartCodes(UINT8 u1EventCount, UINT8 u1CodeLen)
{
    UINT8 u1EI;         // index of start codes in the "event space"
    UINT8 u1DI;         // index of start codes in the "detected space"

    if (u1EventCount != _u1PMDetectedCount)
    {
        return FALSE;
    }

    for (u1EI = 0; u1EI < u1EventCount; u1EI++)
    {
        for (u1DI = 0; u1DI < _u1PMDetectedCount; u1DI++)
        {
            if (_aPMDetectedSpace[u1DI].u4Offset == PM_OFFSET_VERIFIED)
            {
                continue;
            }
            if (_aPMEventSpace[u1EI].u4Offset == _aPMDetectedSpace[u1DI].u4Offset)
            {
                if (PM_IsCodeMatched(_aPMEventSpace[u1EI].u1Code, _aPMDetectedSpace[u1DI].u1Code, u1CodeLen))
                {
                    if(PM_IsTypeMatched(_aPMDetectedSpace[u1DI].u1Code, _aPMDetectedSpace[u1DI].u4Type,
                                        u1CodeLen))
                    {
                        _aPMDetectedSpace[u1DI].u4Offset = PM_OFFSET_VERIFIED;
                        break;
                    }
                }
            }
        }
        if (u1DI == _u1PMDetectedCount)
        {
            return FALSE;
        }
    }

    // Double check
    for (u1DI = 0; u1DI < _u1PMDetectedCount; u1DI++)
    {
        if (_aPMDetectedSpace[u1DI].u4Offset != PM_OFFSET_VERIFIED)
        {
            return FALSE;
        }
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/* PM_CollectStartCode
 */
//-----------------------------------------------------------------------------
static BOOL PM_CollectStartCode(UINT8 u1Pidx, BOOL fgComplete)
{
    PID_STRUCT_T* prPidStruct;
    UINT32 u4Wp, u4BufStart, u4BufEnd, u4HeaderSize, u4PicInfoAddr, i;
    UINT32 u4HeaderRp, u4HeaderWp, u4HeaderBufStart, u4HeaderBufEnd;
    UINT32 u4HwPicNum, u4HeaderByteCount, u4FrontHeaderSize, u4NextPic;
    UINT32 u4WpInfoAddr;

    // It's very important to flush invalidate D-cache.
    HalFlushInvalidateDCache();

    //---------------------------------------------------------------
    // Get header information
    //---------------------------------------------------------------
    u4HeaderRp = VIRTUAL(DMXCMD_READ32(DMX_REG_FTuP_NONERR_STATUS_REG2));   // 114
    ASSERT(u4HeaderRp == VIRTUAL(PID_S_W(u1Pidx, 14)));

    u4HwPicNum = (*(UINT32*)u4HeaderRp) & 0xff;
    u4HeaderWp = VIRTUAL(PID_S_W(u1Pidx, 13));
    ASSERT(_DMX_IsAligned(u4HeaderRp, PES_HEADER_ALIGNMENT));
    u4HeaderBufStart = VIRTUAL(PID_S_W(u1Pidx, 10));
    u4HeaderBufEnd = VIRTUAL(PID_S_W(u1Pidx, 11) + 1);
    ASSERT((u4HeaderRp >= u4HeaderBufStart) && (u4HeaderRp < u4HeaderBufEnd));
    u4HeaderByteCount = (*(UINT32*)u4HeaderRp) >> 16;
    u4FrontHeaderSize = _DMX_Align(u4HeaderByteCount, PES_HEADER_ALIGNMENT) + PES_HEADER_EXTRA_BYTES;
    u4HeaderSize = u4FrontHeaderSize + (u4HwPicNum * 8);
    u4WpInfoAddr = _DMX_AdvanceAddr(u4HeaderRp, 4, u4HeaderWp, u4HeaderBufStart, u4HeaderBufEnd);
    ASSERT(u4WpInfoAddr != 0);

    prPidStruct = _DMX_GetPidStruct(u1Pidx);
    prPidStruct->u4Wp = VIRTUAL(*(UINT32*)u4WpInfoAddr);
    u4Wp = VIRTUAL(prPidStruct->u4Wp);
    u4BufStart = VIRTUAL(prPidStruct->u4BufStart);
    u4BufEnd = VIRTUAL(prPidStruct->u4BufEnd + 1);
    ASSERT((u4Wp >= u4BufStart) && (u4Wp < u4BufEnd));

    //---------------------------------------------------------------
    // Process picture start code
    //---------------------------------------------------------------
    u4NextPic = prPidStruct->u4NextPic;
    ASSERT(u4NextPic <= u4HwPicNum);
    for (i = u4NextPic; i < u4HwPicNum; i++)
    {
        UINT32 u4PicInfo, u4Addr;
        UINT32 u4FrameAddr;
        UINT32 u4Offset, u4Type;
        INT8 i1BackwardLen;

        u4PicInfoAddr = _DMX_AdvanceAddr(u4HeaderRp, (INT32)(u4FrontHeaderSize + (i * 8)),
                                         u4HeaderWp, u4HeaderBufStart, u4HeaderBufEnd);
        ASSERT(u4PicInfoAddr != 0);
        u4PicInfo = *(UINT32*)u4PicInfoAddr;
        u4PicInfoAddr = _DMX_AdvanceAddr(u4HeaderRp, (INT32)(u4FrontHeaderSize + ((i * 8) + 4)),
                                         u4HeaderWp, u4HeaderBufStart, u4HeaderBufEnd);
        u4Type = *(UINT32*)u4PicInfoAddr;
        u4Addr = VIRTUAL(u4PicInfo & 0xffffffff);
        ASSERT((u4Addr >= u4BufStart) && (u4Addr < u4BufEnd));
        // Printf("Pic, addr:0x%x\n", u4Addr);
        i1BackwardLen = 1 - _u1CodeLen;     // Important! Be careful.
        u4FrameAddr = _DMX_AdvanceAddr(u4Addr, i1BackwardLen, u4Wp, u4BufStart, u4BufEnd);
        u4Offset = VIRTUAL(u4FrameAddr) - u4BufStart;
        PM_InsertCodeAndOffset(u4Offset, u4FrameAddr, _u1CodeLen, u4Type);
    }

    if (fgComplete)
    {
        // Update header read pointer
        u4HeaderRp = _DMX_AdvanceAddr(u4HeaderRp, (INT32)u4HeaderSize,
                                      u4HeaderWp, u4HeaderBufStart, u4HeaderBufEnd);
        ASSERT(u4HeaderRp != 0);
        PID_S_W(u1Pidx, 14) = PHYSICAL(u4HeaderRp);
        prPidStruct->u4NextPic = 0;
    }
    else
    {
        prPidStruct->u4NextPic = u4HwPicNum;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/* PM_DispatchFTuPInterrupt
 */
//-----------------------------------------------------------------------------
static void PM_DispatchFTuPInterrupt(void)
{
    while (1)
    {
        UINT32 u4Status;
        UINT8 u1Pidx, u1Type, u1PendInt;

        u4Status = DMXCMD_READ32(DMX_REG_FTuP_NONERR_STATUS_REG1);  // 113
        u1Type = GET_BYTE(u4Status, 0);         // 0: none, 1: PES, 2: PSI
        u1Pidx = GET_BYTE(u4Status, 2);         // PID index
        u1PendInt = GET_BYTE(u4Status, 3);      // Pending interrupts

        if (u1PendInt == 0)
        {
            break;
        }

        switch (u1Type)
        {
        case 0:                 // No status to report
            break;
        case 1:                 // PES
        case 5:                 // Chunk complete
            PM_CollectStartCode(u1Pidx, TRUE);
            break;
        case 3:                 // Partial PES
            PM_CollectStartCode(u1Pidx, FALSE);
            break;
        case 2:                 // PSI
        case 4:                 // Close PES
        case 16:    // incoming packet is not scrambled
        case 17:    // incoming packet is scrambled
        case 32:    // Pre-parsing interrupt
        case 64:    // packet count 
        case 65:    // I,P,B notification
        case 66:    // Timer notification (End Addr + 1)
        case 67:    // Timer notification (long packet count)
            Printf("FTuP non-error INT with reason code: 0x%02x", u1Type);
            break;
        default:
            Printf("FTuP non-error INT with invalid reason code 0x%02x", u1Type);
            break;
        }
        DMXCMD_WRITE32(DMX_REG_FTuP_NONERR_STATUS_REG1, 1);
    }
}


//-----------------------------------------------------------------------------
/* PM_DispatchInterrupt
 */
//-----------------------------------------------------------------------------
static void PM_DispatchInterrupt(UINT32 u4Status)
{
    // Check if spurious interrupt
    if (u4Status == 0)
    {
        Printf("Spurious interrupt!\n");
        return;
    }

    // Check if error interrupt
    if (u4Status & DMX_INT_ERR_MASK)
    {
        if (u4Status & DMX_INT_ERR_DBM)
        {
            u4Status = DMXCMD_READ32(DMX_REG_DBM_ERROR_STATUS_REG);
            DMXCMD_WRITE32(DMX_REG_DBM_ERROR_STATUS_REG, 1);
            Printf("DBM error INT: 0x%08X\n", u4Status);
        }
        if (u4Status & DMX_INT_ERR_DESCRAMBLER)
        {
            u4Status = DMXCMD_READ32(DMX_REG_DESCRAMBLER_ERROR_STATUS_REG);
            DMXCMD_WRITE32(DMX_REG_DESCRAMBLER_ERROR_STATUS_REG, 1);
            Printf("CA error INT: 0x%08X\n", u4Status);
        }
        if (u4Status & DMX_INT_ERR_PVR)
        {
            u4Status = DMXCMD_READ32(DMX_REG_PVR_ERROR_STATUS_REG);
            DMXCMD_WRITE32(DMX_REG_PVR_ERROR_STATUS_REG, 1);
            Printf("PVR error INT: 0x%08X\n", u4Status);
        }
        if (u4Status & DMX_INT_ERR_STERRING)
        {
            u4Status = DMXCMD_READ32(DMX_REG_STEER_ERROR_STATUS_REG);
            DMXCMD_WRITE32(DMX_REG_STEER_ERROR_STATUS_REG, 1);
            Printf("Steering error INT: 0x%08X\n", u4Status);
        }
        if (u4Status & DMX_INT_ERR_FTuP)
        {
            u4Status = DMXCMD_READ32(DMX_REG_FTuP_ERROR_STATUS_REG);
            DMXCMD_WRITE32(DMX_REG_FTuP_ERROR_STATUS_REG, 1);
            Printf("FTuP error INT: 0x%08X\n", u4Status);
        }
    }

    // Handle normal cases
    if (u4Status & DMX_INT_STATUS_MASK)
    {
        if (u4Status & DMX_INT_STATUS_FTuP)
        {
            PM_DispatchFTuPInterrupt();
        }
        if (u4Status & DMX_INT_STATUS_DESCRAMBLER)
        {
            u4Status = DMXCMD_READ32(DMX_REG_DESCRAMBLER_NONERR_STATUS_REG);
            DMXCMD_WRITE32(DMX_REG_DESCRAMBLER_NONERR_STATUS_REG, 1);
            Printf("CA non-error INT: 0x%08X\n", u4Status);
        }

        if (u4Status & DMX_INT_STATUS_PVR)
        {
            u4Status = DMXCMD_READ32(DMX_REG_PVR_NONERR_STATUS_REG);
            DMXCMD_WRITE32(DMX_REG_PVR_NONERR_STATUS_REG, 1);
            Printf("PVR non-error INT: 0x%08X\n", u4Status);
        }
    }
}


//-----------------------------------------------------------------------------
/* PM_IrqHandler
 *
 * @param  u2Vector        The IRQ vector
 * @retval VOID
 */
//-----------------------------------------------------------------------------
static VOID PM_IrqHandler(UINT16 u2Vector)
{
    static BOOL _fgISR = FALSE;

    ASSERT(!_fgISR);
    UNUSED(_fgISR);
    _fgISR = TRUE;

    ASSERT(u2Vector == VECTOR_DEMUX);
    if (!BIM_IsIrqPending(VECTOR_DEMUX))
    {
        Printf("Spurious Demux global interrupt!\n");
    }

    // Check if it's demux interrupt
    while (BIM_IsIrqPending(VECTOR_DEMUX))
    {
        UINT32 u4Status, u4Mask;

        u4Status = DMXCMD_READ32(DMX_REG_INT_STAUS);
        u4Mask = DMXCMD_READ32(DMX_REG_INT_MASK);
        u4Status &= u4Mask;
        if (u4Status)
        {
            PM_DispatchInterrupt(u4Status);
        }
        else
        {
            Printf("Spurious Demux local interrupt!\n");
        }
        // Clear interrupt
        VERIFY(BIM_ClearIrq(VECTOR_DEMUX));
    }
    _fgISR = FALSE;
}


//-----------------------------------------------------------------------------
/* PM_InitDemuxISR
 */
//-----------------------------------------------------------------------------
static void PM_InitDemuxISR(void)
{
    static BOOL _fgInited = FALSE;
    x_os_isr_fct pfnOldIsr;

    if (!_fgInited)
    {
        if (x_reg_isr(VECTOR_DEMUX, PM_IrqHandler, &pfnOldIsr) != OSR_OK)
        {
            Printf("Error: failed to register Demux PM ISR!\n");
        }
        UNUSED(pfnOldIsr);
        _fgInited = TRUE;
    }

    // Enable all demux interrupts
    DMXCMD_WRITE32(DMX_REG_INT_MASK, 0xFFFFFFFF);
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/* DMX_EMU_PM_Transfer
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_PM_Transfer(UINT8 u1TsIndex)
{
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4Reg, u1EventCount, u4SrcSize, u4DstSize;
    UINT16 u2Pid;
    UINT8 i, u1CodeLen, u1Pidx = 0;
    BOOL fgRet = FALSE;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    u4SrcSize = DMX_EMU_PM_SRC_BUF_SIZE;
    u4DstSize = DMX_EMU_PM_DST_BUF_SIZE;
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4SrcSize, DDI_BUF_ALIGNMENT);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstSize, DMX_ES_FIFO_ALIGNMENT);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    _GenerateRandomPattern(u4SrcAddr, u4SrcSize);

    _u4MMatchCount = 0;
    x_memset(_aPMSampleSpace, 0, sizeof(_aPMSampleSpace));
    x_memset(_aPMEventSpace, 0, sizeof(_aPMEventSpace));
    x_memset(_aPMDetectedSpace, 0, sizeof(_aPMDetectedSpace));
    _u1PMDetectedCount = 0;

    // Randomize the length (4 or 6 bytes) of a start code pattern.
    //_u1CodeLen = (random(2) == 0) ? 4 : 6;
    _u1CodeLen = random(3);
    if(_u1CodeLen == 0)
    {
        _u1CodeLen = 4;
    }
    else if(_u1CodeLen == 1)
    {
        _u1CodeLen = 6;
    }
    else
    {
        _u1CodeLen = 8;
    }
    
    u1CodeLen = _u1CodeLen;

    PM_GenerateSampleSpace(u1CodeLen);

    u1EventCount = random(PM_MAX_NUM_START_CODE - 1) + 2;
    PM_SampleStartCodes(u4SrcAddr, u4SrcSize, u1EventCount, u1CodeLen);

    PM_DestoryFakeStartCodes(u4SrcAddr, u4SrcSize, u1EventCount, u1CodeLen);

    // It's very important to flush invalidate D-cache.
    HalFlushInvalidateDCache();

    Printf("Src: 0x%08X, Dst: 0x%08X, %d-byte start codes: %d, Overlapped: %s\n",
            u4SrcAddr, u4DstAddr, u1CodeLen, u1EventCount,
            (_fgPMOverlappedPatterns ? "YES" : "NO"));

#ifdef PM_DEBUG
    PM_DebugStartCodes(u4SrcAddr, u4SrcSize, u1EventCount, u1CodeLen);
#endif  // PM_DEBUG

    // Data cache needs to be flushed, and it is done in _DMX_EMU_DDI_Transfer().

    PM_SetPatternMask(u1TsIndex, u1Pidx, u1CodeLen);

    if (!_DMX_SetBypassMode(u1TsIndex, 188, TRUE, TRUE))
    {
        goto _exit;
    }

    // Setup PID
    u2Pid = random(0x2000);
    u4DstEnd = _DMX_Align(u4DstAddr + u4DstSize, DMX_ES_FIFO_ALIGNMENT);
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4DstEnd - u4DstAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEOCLIP;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    if (!_DMX_SetPid(u1Pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        goto _exit;
    }

    if (!_DMX_EMU_SyncToggleBitInBypassCtrlWord(u1TsIndex, u1Pidx))
    {
        Printf("Cannot set the Toggle bit in the Bypass Control Word.\n");
        goto _exit;
    }
    
    u4Reg = PID_S_W(u1Pidx, 0);
    PID_S_W(u1Pidx, 0) = u4Reg | 0x4;       // Enable output mode
    PID_S_W(u1Pidx, 1) = u4SrcSize;
    PID_S_W(u1Pidx, 3) = (0 << 24) | 1;     // skip size = 0, and trigger
    
    // _DMX_SetBypassMode() resets FTI which in turn clear the "Interrupt Mask
    // Register".  All interrupts are turned on in PM_InitDemuxISR().
    PM_InitDemuxISR();

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4SrcSize))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4SrcSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    // Verify start codes
    if (!PM_VerifyStartCodes(u1EventCount, u1CodeLen))
    {
        Printf("Start codes differ!\n");
//        ASSERT(0);
        goto _exit;
    }

    Printf("Multiple match count: %d\n", _u4MMatchCount);

    fgRet = TRUE;

_exit:

    for (i = 0; i < DMX_INT_QUEUE_DEPTH; i++)
    {
        DMXCMD_WRITE32(DMX_REG_DBM_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_DBM_NONERR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_DESCRAMBLER_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_DESCRAMBLER_NONERR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_STEER_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_STEER_NONERR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_FTuP_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_FTuP_NONERR_STATUS_REG1, 1);
        DMXCMD_WRITE32(DMX_REG_PCR_ERROR_STATUS_REG, 1);
        DMXCMD_WRITE32(DMX_REG_PCR_NONERR_STATUS_REG1, 1);
    }
    
    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));
    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));
    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4SrcAddr));

    return fgRet;
}


