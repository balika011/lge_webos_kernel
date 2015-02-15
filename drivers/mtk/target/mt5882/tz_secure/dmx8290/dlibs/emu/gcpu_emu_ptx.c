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
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2007, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: dmx_emu_drm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_des.c
 *  Demux driver - DES/3DES emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"

#include "gcpu.h"
#include "dmx_mm.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
//#define PM_DEBUG

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
#define PM_MAX_CODE_LEN             8
#define PM_MIN_CODE_LEN             4

// The Pattern/Mask DMEM can only accommodate at most ten start code patterns.
#define PM_MAX_SAMPLES              6

// Users can change this maximum number of start code patterns.
#define PM_MAX_NUM_START_CODE       100

#define PES_HEADER_ALIGNMENT        4
#define PES_HEADER_EXTRA_BYTES      12
#define PM_OFFSET_VERIFIED          0xFFFFFFFF

enum
{
    STREAM_TYPE_MPEG2,
    STREAM_TYPE_H264,
    STREAM_TYPE_VC1,
    STREAM_TYPE_MPEG4,
    STREAM_TYPE_H264_ALL,
    STREAM_TYPE_H263,
    STREAM_TYPE_PACKET,
    STREAM_TYPE_MAX
};
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    UINT8 u1Code[PM_MAX_CODE_LEN];
    UINT8 u1Mask[PM_MAX_CODE_LEN];
    UINT8 u1CodeLen;
} PM_START_CODE_T;


typedef struct
{
    UINT8 u1Code[PM_MAX_CODE_LEN];
    UINT8 u1Mask[PM_MAX_CODE_LEN];
    UINT8 u1CodeLen;
    UINT32 u4Offset;
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
static UINT8 u1StreamType = 0;

static PM_START_CODE_T _aPTXPattern[STREAM_TYPE_MAX][PM_MAX_SAMPLES] =
{
    //MPEG2
    {
        {{0x0, 0x0, 0x1, 0x0, 0x0, 0x0}, {0xff, 0xff, 0xff, 0xff, 0x0, 0x0}, 6},
        {{0x0, 0x0, 0x1, 0xb3}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0xb7}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0xb8}, {0xff, 0xff, 0xff, 0xff}, 4},
    },
    //H264 (1st MBs)
    {
        {{0x0, 0x0, 0x1, 0x06}, {0xff, 0xff, 0xff, 0x1f}, 4},
        {{0x0, 0x0, 0x1, 0x07}, {0xff, 0xff, 0xff, 0x17}, 4},
        {{0x0, 0x0, 0x1, 0x0A}, {0xff, 0xff, 0xff, 0x1f}, 4},
        {{0x0, 0x0, 0x1, 0x08}, {0xff, 0xff, 0xff, 0x1e}, 4},
        {{0x0, 0x0, 0x1, 0x01, 0x80}, {0xff, 0xff, 0xff, 0x1b, 0x80}, 5},
        {{0x0, 0x0, 0x1, 0x14, 0x0, 0x0, 0x0, 0x80}, {0xff, 0xff, 0xff, 0x1f, 0x0, 0x0, 0x0, 0x80}, 8},
    },
    //VC1
    {
        {{0x0, 0x0, 0x1, 0x0c}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0x0d}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0x0a}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0x0b, 0x0, 0x0}, {0xff, 0xff, 0xff, 0xff, 0x0, 0x0}, 6},
        {{0x0, 0x0, 0x1, 0x0e}, {0xff, 0xff, 0xff, 0xfe}, 4},
        {{0x0, 0x0, 0x1, 0x0c}, {0xff, 0xff, 0xff, 0xfe}, 4},
    },
    //MP4
    {
        {{0x0, 0x0, 0x1, 0x20}, {0xff, 0xff, 0xff, 0xf0}, 4},
        {{0x0, 0x0, 0x1, 0xb0}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0xb3}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0xb5}, {0xff, 0xff, 0xff, 0xff}, 4},
        {{0x0, 0x0, 0x1, 0xb6, 0x0}, {0xff, 0xff, 0xff, 0xff, 0x0}, 5},
    },
    //H264 (all MBs)
    {
        {{0x0, 0x0, 0x1, 0x06}, {0xff, 0xff, 0xff, 0x1f}, 4},
        {{0x0, 0x0, 0x1, 0x07}, {0xff, 0xff, 0xff, 0x17}, 4},
        {{0x0, 0x0, 0x1, 0x0A}, {0xff, 0xff, 0xff, 0x1f}, 4},
        {{0x0, 0x0, 0x1, 0x08}, {0xff, 0xff, 0xff, 0x1e}, 4},
        {{0x0, 0x0, 0x1, 0x01, 0x0}, {0xff, 0xff, 0xff, 0x1b, 0x00}, 5},
        {{0x0, 0x0, 0x1, 0x14, 0x0, 0x0, 0x0, 0x0}, {0xff, 0xff, 0xff, 0x1f, 0x0, 0x0, 0x0, 0x0}, 8},
    },
    //H263
    {
        {{0x0, 0x0, 0x80, 0x02, 0x0}, {0xff, 0xff, 0xfc, 0x03, 0x00}, 5},
    },
    //packet start code
    {
        {{0x0, 0x0, 0x1, 0xc0}, {0xff, 0xff, 0xff, 0xc0}, 4},
        {{0x0, 0x0, 0x1, 0xbc}, {0xff, 0xff, 0xff, 0xfc}, 4},
        {{0x0, 0x0, 0x1, 0xbb}, {0xff, 0xff, 0xff, 0xff}, 4},
    },
};

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/* PM_IsCodeMatched
 */
//-----------------------------------------------------------------------------
static BOOL GCPU_PM_IsCodeMatched(UINT8 *pu1Code1, UINT8 *pu1Code2, UINT8 *pu1Mask, UINT8 u1CodeLen)
{
    UINT8 u1Index;

    for (u1Index = 0; u1Index < u1CodeLen; u1Index++)
    {
        if ((pu1Code1[u1Index] & pu1Mask[u1Index]) != (pu1Code2[u1Index] & pu1Mask[u1Index]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL GCPU_PM_IsCodeTotalMatched(UINT8 *pu1Code1, UINT8 *pu1Code2, UINT8 u1CodeLen)
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
/* PM_GenerateSampleSpace
 *
 * There are two start codes having the pattern "OO XX XX OO" in the following
 * sequence: OO XX XX OO XX XX OO.  They are called overlapped start codes here.
 * It can be easily seen that the first byte and the last byte in an overlapped
 * start codes are the same.
 */
//-----------------------------------------------------------------------------
static void GCPU_PM_GenerateSampleSpace(UINT8 u1StreamType)
{
    UINT8 u1No;

    _fgPMOverlappedPatterns = FALSE; //(random(2) ? TRUE : FALSE);

    _u1PMValidSamples = 0;

    //MPEG 2
    x_memcpy(_aPMSampleSpace, _aPTXPattern[u1StreamType], sizeof(_aPMSampleSpace));

    for(u1No = 0; u1No < PM_MAX_SAMPLES; u1No++)
    {
        if(_aPMSampleSpace[u1No].u1CodeLen != 0)
        {
            _u1PMValidSamples++;
        }
    }
}

static UINT8 GCPU_GenStartCodeByMask(UINT8 u1Code, UINT8 u1Mask)
{
    return (u1Code & u1Mask) | ((rand() % 0xff) & ~u1Mask);
}

//-----------------------------------------------------------------------------
/* GCPU_PM_CopyCode
 *
 * Copy start codes to the global structure _aPMEventSpace and "source buffer".
 * The caller of this function have to check the address range of the input
 * arguments.
 */
//-----------------------------------------------------------------------------
static void GCPU_PM_CopyCode(UINT8* pu1Code, UINT8* pu1Mask, UINT8 u1CodeLen, UINT8 u1No,
                        UINT32 u4SrcAddr, UINT32 u4Offset)
{
    UINT8 u1Index;

    for (u1Index = 0; u1Index < u1CodeLen; u1Index++)
    {
        _aPMEventSpace[u1No].u1Code[u1Index]   = GCPU_GenStartCodeByMask(pu1Code[u1Index], pu1Mask[u1Index]);//pu1Code[u1Index];
        _aPMEventSpace[u1No].u1Mask[u1Index]   = pu1Mask[u1Index];
        ((UINT8*)u4SrcAddr)[u4Offset + u1Index] = _aPMEventSpace[u1No].u1Code[u1Index];//pu1Code[u1Index];
    }
    _aPMEventSpace[u1No].u1CodeLen = u1CodeLen;
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
    UINT32 u4MaxRange, u4MinRange, i;

    u4MaxRange = u4Offset + u1CodeLen - 1;
    u4MinRange = u4Offset;

    for (n = 0; n < u1No; n++)
    {
        for(i = _aPMEventSpace[n].u4Offset; i < _aPMEventSpace[n].u4Offset + _aPMEventSpace[n].u1CodeLen; i++)
        {
            if ((i <= u4MaxRange) && (i >= u4MinRange))
            {
                return TRUE;        // Overlapped start codes
            }
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
static void GCPU_PM_SampleStartCodes(UINT32 u4SrcAddr, UINT32 u4SrcSize,
                                UINT8 u1EventCount)
{
    UINT32 u4Offset;
    UINT8 u1No, u1SampleIndex;

    ASSERT((2 <= u1EventCount) && (u1EventCount <= PM_MAX_NUM_START_CODE));
    ASSERT((PM_MAX_NUM_START_CODE * PM_MAX_CODE_LEN) < u4SrcSize);

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
                u4Offset = random(u4SrcSize - (2 * _aPMSampleSpace[u1SampleIndex].u1CodeLen));
#endif  // PM_START_CODE_AT_HEAD

                _aPMEventSpace[u1No].u4Offset = u4Offset;
                GCPU_PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code,
                    _aPMSampleSpace[u1SampleIndex].u1Mask, _aPMSampleSpace[u1SampleIndex].u1CodeLen,
                    u1No, u4SrcAddr, u4Offset);

                u1No++;
                u4Offset += _aPMSampleSpace[u1SampleIndex].u1CodeLen - 1;
                _aPMEventSpace[u1No].u4Offset = u4Offset;
                GCPU_PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code,
                    _aPMSampleSpace[u1SampleIndex].u1Mask, _aPMSampleSpace[u1SampleIndex].u1CodeLen,
                    u1No, u4SrcAddr, u4Offset);
            }
            else
            {
                u4Offset = random(u4SrcSize - _aPMSampleSpace[u1SampleIndex].u1CodeLen);
                _aPMEventSpace[u1No].u4Offset = u4Offset;
                GCPU_PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code,
                    _aPMSampleSpace[u1SampleIndex].u1Mask, _aPMSampleSpace[u1SampleIndex].u1CodeLen,
                    u1No, u4SrcAddr, u4Offset);
            }
        }
        else
        {
            do {
                u4Offset = random(u4SrcSize - _aPMSampleSpace[u1SampleIndex].u1CodeLen);
            } while (PM_IsOffsetOverlapped(u1No, u4Offset, _aPMSampleSpace[u1SampleIndex].u1CodeLen));

            _aPMEventSpace[u1No].u4Offset = u4Offset;
            GCPU_PM_CopyCode(_aPMSampleSpace[u1SampleIndex].u1Code, _aPMSampleSpace[u1SampleIndex].u1Mask,
                _aPMSampleSpace[u1SampleIndex].u1CodeLen, u1No, u4SrcAddr, u4Offset);
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
static BOOL GCPU_PM_IsFakeStartCode(UINT8 *pu1SrcAddr, UINT32 u4Offset,
                               UINT8 u1EventCount, UINT32 u4RemainLen, UINT8 u1CodeLen)
{
    UINT8 u1No;
    BOOL fgFake = FALSE;

    // check if it match the one of the sample
    for (u1No = 0; u1No < _u1PMValidSamples; u1No++)
    {
        if(_aPMSampleSpace[u1No].u1CodeLen > u4RemainLen)
        {
            continue; // code len is larger than remain len, we don't need to do anything
        }

        if (GCPU_PM_IsCodeMatched(&(pu1SrcAddr[u4Offset]), _aPMSampleSpace[u1No].u1Code,
            _aPMSampleSpace[u1No].u1Mask, _aPMSampleSpace[u1No].u1CodeLen))
        {
            fgFake = TRUE;
        }
    }

    if(!fgFake)
    {
        return fgFake;
    }

    // fgFake == TRUE, check if it is a real start code
    for (u1No = 0; u1No < u1EventCount; u1No++)
    {
        if(_aPMEventSpace[u1No].u1CodeLen > u4RemainLen)
        {
            continue; // code len is larger than remain len, we don't need to do anything
        }

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

    if (fgFake)
    {
        Printf("Found fake start code at 0x%08X.\n", (pu1SrcAddr + u4Offset));
    }

    return fgFake;
}


//-----------------------------------------------------------------------------
/* PM_DestoryFakeStartCodes
 *
 * Destroy fake start codes.
 */
//-----------------------------------------------------------------------------
static void GCPU_PM_DestoryFakeStartCodes(UINT32 u4SrcAddr, UINT32 u4SrcSize,
                                     UINT8 u1EventCount)
{
    UINT8 *pu1SrcAddr;
    UINT32 u4Offset;
    UINT8 u1FakeCodeLen;

    pu1SrcAddr = (UINT8*)u4SrcAddr;
    u4Offset = 0;
    while (u4Offset <= (u4SrcSize - PM_MIN_CODE_LEN))
    {
        if (GCPU_PM_IsFakeStartCode(pu1SrcAddr, u4Offset, u1EventCount,
            u4SrcSize - u4Offset, &u1FakeCodeLen))
        {
            UINT8 u1DemiseIndex = 0; //random(u1FakeCodeLen);

            pu1SrcAddr[u4Offset + u1DemiseIndex] = random(0x100);
            // Move backward and search again.
            if (u4Offset >= PM_MAX_CODE_LEN /*(u1FakeCodeLen - (1 + u1DemiseIndex))*/)
            {
                u4Offset -= PM_MAX_CODE_LEN; //(u1FakeCodeLen - (1 + u1DemiseIndex));
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
static void GCPU_PM_DebugStartCodes(UINT32 u4SrcAddr, UINT32 u4SrcSize,
                               UINT8 u1EventCount)
{
    UINT32 u4Offset;
    UINT8 u1No, index, *pu1SrcAddr = (UINT8*)u4SrcAddr;

    for (u4Offset = 0; u4Offset <= (u4SrcSize - PM_MIN_CODE_LEN); u4Offset++)
    {
        for (u1No = 0; u1No < u1EventCount; u1No++)
        {
            if(_aPMEventSpace[u1No].u1CodeLen > u4SrcSize - u4Offset)
            {
                continue; // code len is larger than remain len, we don't need to do anything
            }

            if (GCPU_PM_IsCodeTotalMatched(&(pu1SrcAddr[u4Offset]), _aPMEventSpace[u1No].u1Code,
                _aPMEventSpace[u1No].u1CodeLen))
            {
                Printf("Start code: 0x");
                for (index = 0; index < _aPMEventSpace[u1No].u1CodeLen; index++)
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

void GCPU_EventDump(UINT8 u1EventCount)
{
    UINT32 i, j;

    for(i = 0; i < DMX_EMU_PM_SRC_BUF_SIZE; i++)
    {
        for(j = 0; j < u1EventCount; j++)
        {
            if (_aPMEventSpace[j].u4Offset == i)
            {
                Printf("ES: 0x%x\n", _aPMEventSpace[j].u4Offset);
            }
        }
        for(j = 0; j < _u1PMDetectedCount; j++)
        {
            if (_aPMDetectedSpace[j].u4Offset == i)
            {
                Printf("DS: 0x%x\n", _aPMDetectedSpace[j].u4Offset);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/* PM_VerifyStartCodes
 */
//-----------------------------------------------------------------------------
static BOOL GCPU_PM_VerifyStartCodes(UINT8 u1EventCount)
{
    UINT8 u1EI;         // index of start codes in the "event space"
    UINT8 u1DI;         // index of start codes in the "detected space"

    if (u1EventCount != _u1PMDetectedCount)
    {
        GCPU_EventDump(u1EventCount);
        return FALSE;
    }

    for (u1EI = 0; u1EI < u1EventCount; u1EI++)
    {
        for (u1DI = 0; u1DI < _u1PMDetectedCount; u1DI++)
        {
            //if (_aPMDetectedSpace[u1DI].u4Offset == PM_OFFSET_VERIFIED)
            //{
            //    continue;
            //}

            if (_aPMEventSpace[u1EI].u4Offset == _aPMDetectedSpace[u1DI].u4Offset)
            {
                if (GCPU_PM_IsCodeTotalMatched(_aPMEventSpace[u1EI].u1Code, _aPMDetectedSpace[u1DI].u1Code,
                    _aPMEventSpace[u1EI].u1CodeLen))
                {
                    //_aPMDetectedSpace[u1DI].u4Offset = PM_OFFSET_VERIFIED;
                    break;
                }
                else
                {
                    UINT8 i;
                    Printf("ES: %d and PS: %d don't match?\n", u1EI, u1DI);

                    Printf("ES code: 0x");
                    for (i = 0; i < _aPMEventSpace[u1EI].u1CodeLen; i++)
                    {
                        Printf("%02X", _aPMEventSpace[u1EI].u1Code[i]);
                    }
                    Printf("\n");

                    Printf("PS code: 0x");
                    for (i = 0; i < _aPMEventSpace[u1EI].u1CodeLen; i++)
                    {
                        Printf("%02X", _aPMDetectedSpace[u1DI].u1Code[i]);
                    }
                    Printf("\n");

                    //ASSERT(0);

                    break;
                }
            }
        }
        if (u1DI == _u1PMDetectedCount)
        {
            GCPU_EventDump(u1EventCount);
            return FALSE;
        }
    }

    // Double check
    #if 0
    for (u1DI = 0; u1DI < _u1PMDetectedCount; u1DI++)
    {
        if (_aPMDetectedSpace[u1DI].u4Offset != PM_OFFSET_VERIFIED)
        {
            return FALSE;
        }
    }
    #endif

    return TRUE;
}

//-----------------------------------------------------------------------------
/* PM_InsertCodeAndOffset
 */
//-----------------------------------------------------------------------------
static void PM_InsertCodeAndOffset(UINT32 u4Offset, UINT32 u4FrameAddr, UINT8 u1CodeLen)
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
    _aPMDetectedSpace[_u1PMDetectedCount].u1CodeLen = u1CodeLen;
    _u1PMDetectedCount++;
}

UINT8 au1MPGOffset[4] = {5, 3, 3, 3};
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
static BOOL GCPU_PM_SetPatternMask(UINT32 u4SrcAddr, UINT32 u4SrcSize, UINT8 u1EventCount)
{
    PTX_PARAM_T rParam;
    UINT32 *pu4ComQ;
    UINT32 u4BufStart, u4BufEnd, u4BufSize = 0x500000, u4FstSendSize, u4HeaderAddr,
        u4HeaderBufferStart, u4HwPicNum, u4IntBufferStart, i, j, u4SrcDiff, u4DstDiff;
    UINT32 u4PicCountInOneAddr = 0;

    pu4ComQ = (UINT32)BSP_AllocAlignedDmaMemory(32, GCPU_FIFO_ALIGNMENT);
    u4IntBufferStart = (UINT32)BSP_AllocAlignedDmaMemory(0x1000, GCPU_FIFO_ALIGNMENT);
    u4HeaderBufferStart = (UINT32)BSP_AllocAlignedDmaMemory(0x1000, GCPU_FIFO_ALIGNMENT);
    u4BufStart = (UINT32)BSP_AllocAlignedDmaMemory(0x500000, DMX_ES_FIFO_ALIGNMENT);
    if (pu4ComQ == 0 || u4IntBufferStart == 0 || u4HeaderBufferStart == 0 || u4BufStart == 0)
    {
        LOG(3, "Can't allocate memory!\n");
        return FALSE;
    }

    u4BufEnd = u4BufStart + u4BufSize;
    u4FstSendSize = rand() % u4SrcSize;

    *(UINT32 *)(pu4ComQ + 0)= u4SrcAddr - (u4SrcAddr % 16);  //src start must be 16 bytes alignment
    *(UINT32 *)(pu4ComQ + 1) = u4SrcAddr + u4SrcSize + (16 - ((u4SrcAddr + u4SrcSize) % 16));  //src start must be 16 bytes alignment
    *(UINT32 *)(pu4ComQ + 2) = u4SrcAddr;
    *(UINT32 *)(pu4ComQ + 3) = u4FstSendSize / 2;

    *(UINT32 *)(pu4ComQ + 4) = u4SrcAddr - (u4SrcAddr % 16);  //src start must be 16 bytes alignment
    *(UINT32 *)(pu4ComQ + 5) = u4SrcAddr + u4SrcSize + (16 - ((u4SrcAddr + u4SrcSize) % 16));  //src start must be 16 bytes alignment
    *(UINT32 *)(pu4ComQ + 6) = u4SrcAddr + u4FstSendSize / 2;
    *(UINT32 *)(pu4ComQ + 7) = u4FstSendSize - u4FstSendSize / 2;

    Printf("1st command queue addr: 0x%x and 0x%x\n", u4SrcAddr, u4SrcAddr + u4FstSendSize / 2);

    rParam.u4PicStruct[0] = (1 << 7) | (u1StreamType << 1);
    rParam.u4PicStruct[1] = 0;
    rParam.u4PicStruct[2] = 0;
    rParam.u4PicStruct[3] = 0;
    rParam.u4PicStruct[4] = 0;

    rParam.u4PicStruct[5] = PHYSICAL(u4BufStart);
    rParam.u4PicStruct[6] = PHYSICAL(u4BufStart + u4BufSize);
    rParam.u4PicStruct[7] = PHYSICAL(u4BufStart);
    rParam.u4PicStruct[8] = PHYSICAL(u4BufStart);

    rParam.u4PicStruct[9] = PHYSICAL(u4BufStart);

    rParam.u4PicStruct[10] = PHYSICAL(u4HeaderBufferStart);
    rParam.u4PicStruct[11] = PHYSICAL(u4HeaderBufferStart + 0x1000);
    rParam.u4PicStruct[12] = PHYSICAL(u4HeaderBufferStart);
    rParam.u4PicStruct[13] = PHYSICAL(u4HeaderBufferStart);

    rParam.u4PicStruct[14] = 0;
    rParam.u4PicStruct[15] = 0;
    rParam.u4PicStruct[16] = 0;
    rParam.u4PicStruct[17] = 0;
    rParam.u4PicStruct[18] = 0;


    rParam.u4CmdAddr = pu4ComQ;
    rParam.u4CmdNum = 2;
    rParam.u4IntStartAddr = u4IntBufferStart;
    rParam.u4IntBufStart = u4IntBufferStart;
    rParam.u4IntBufEnd = u4IntBufferStart + 0x1000;
    rParam.eMode = PTX_FIRST_BYTE_MODE;

    HalFlushInvalidateDCache();

    GCPU_Cmd(0, GCPU_PTX, &rParam);

    u4HeaderAddr = *(UINT32 *)(rParam.u4IntStartAddr - 4);
    u4HwPicNum = (*(UINT32*)u4HeaderAddr) & 0xff;

    for (i = 0; i < u4HwPicNum; i++)
    {
        UINT32 u4PicInfoAddr, u4PicInfo;
        UINT32 u4FrameAddr, u4Addr;
        UINT32 u4Offset;
        INT8 i1BackwardLen;
        UINT32 u4PicType;

        u4PicInfoAddr = u4HeaderAddr + 12 + (i * 8);
        u4PicType = (*(UINT32 *)(u4PicInfoAddr + 4)) >> 26; // type
        for(j = 0; j < 6; j++)
        {
            if((u4PicType >> j) & 0x1)
            {
                u4PicCountInOneAddr++;
                ASSERT(u4PicCountInOneAddr <= 1);
                ASSERT(u4PicInfoAddr != 0);
                u4PicInfo = *(UINT32*)u4PicInfoAddr;
                u4Addr = VIRTUAL(u4PicInfo);
                ASSERT((u4Addr >= u4BufStart) && (u4Addr < u4BufEnd));
                // Printf("Pic, addr:0x%x\n", u4Addr);
                //i1BackwardLen = 1 - _u1CodeLen;     // Important! Be careful.
                u4FrameAddr = u4Addr - (_aPMSampleSpace[j].u1CodeLen - 1);
                    //au1MPGOffset[u4PicType]; //i1BackwardLen;
                u4Offset = VIRTUAL(u4FrameAddr) - u4BufStart;
                PM_InsertCodeAndOffset(u4Offset, u4FrameAddr, _aPMSampleSpace[j].u1CodeLen);
            }
        }
        u4PicCountInOneAddr = 0;
    }

    *(UINT32 *)(pu4ComQ + 0) = u4SrcAddr - (u4SrcAddr % 16);  //src start must be 16 bytes alignment
    *(UINT32 *)(pu4ComQ + 1) = u4SrcAddr + u4SrcSize + (16 - ((u4SrcAddr + u4SrcSize) % 16));  //src start must be 16 bytes alignment
    *(UINT32 *)(pu4ComQ + 2) = u4SrcAddr + u4FstSendSize;
    *(UINT32 *)(pu4ComQ + 3) = u4SrcSize - u4FstSendSize;

    Printf("2nd command queue addr: 0x%x\n", u4SrcAddr + u4FstSendSize);

    rParam.u4CmdAddr = pu4ComQ;
    rParam.u4CmdNum = 1;
    rParam.u4IntStartAddr = rParam.u4IntStartAddr;
    rParam.u4IntBufStart = u4IntBufferStart;
    rParam.u4IntBufEnd = u4IntBufferStart + 0x1000;
    rParam.eMode = PTX_SUCCESSIVE_BYTES_MODE;

    HalFlushInvalidateDCache();

    GCPU_Cmd(0, GCPU_PTX, &rParam);

    u4HeaderAddr = *(UINT32 *)(rParam.u4IntStartAddr - 4);
    u4HwPicNum = (*(UINT32*)u4HeaderAddr) & 0xff;

    for (i = 0; i < u4HwPicNum; i++)
    {
        UINT32 u4PicInfoAddr, u4PicInfo;
        UINT32 u4FrameAddr, u4Addr;
        UINT32 u4Offset;
        INT8 i1BackwardLen;
        UINT32 u4PicType;

        u4PicInfoAddr = u4HeaderAddr + 12 + (i * 8);
        u4PicType = (*(UINT32 *)(u4PicInfoAddr + 4)) >> 26; // type
        for(j = 0; j < 6; j++)
        {
            if((u4PicType >> j) & 0x1)
            {
                u4PicCountInOneAddr++;
                ASSERT(u4PicCountInOneAddr <= 1);
                ASSERT(u4PicInfoAddr != 0);
                u4PicInfo = *(UINT32*)u4PicInfoAddr;
                u4Addr = VIRTUAL(u4PicInfo);
                ASSERT((u4Addr >= u4BufStart) && (u4Addr < u4BufEnd));
                // Printf("Pic, addr:0x%x\n", u4Addr);
                //i1BackwardLen = 1 - _u1CodeLen;     // Important! Be careful.
                u4FrameAddr = u4Addr - (_aPMSampleSpace[j].u1CodeLen - 1);
                    //au1MPGOffset[u4PicType]; //i1BackwardLen;
                u4Offset = VIRTUAL(u4FrameAddr) - u4BufStart;
                PM_InsertCodeAndOffset(u4Offset, u4FrameAddr, _aPMSampleSpace[j].u1CodeLen);
            }
        }
        u4PicCountInOneAddr = 0;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4BufStart, u4SrcSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
    }

    // Verify start codes
    if (!GCPU_PM_VerifyStartCodes(u1EventCount))
    {
        Printf("Start codes differ!\n");
        ASSERT(0);
    }
    else
    {
        Printf("PTX command OK!\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory((void*)pu4ComQ));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4IntBufferStart));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4HeaderBufferStart));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4BufStart));

    return TRUE;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** GCPU_EMU_CSSAV
 */
//-----------------------------------------------------------------------------
INT32 GCPU_EMU_PTX()
{
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u1EventCount, u4SrcSize, u4DstSize;
    UINT16 u2Pid;
    UINT8 u1CodeLen, u1Pidx = 0;
    BOOL fgRet = FALSE;

    do
    {
        u1StreamType = random(STREAM_TYPE_MAX); //MPEG
    } while(u1StreamType == STREAM_TYPE_H263);

    u4SrcSize = DMX_EMU_PM_SRC_BUF_SIZE;
    u4DstSize = DMX_EMU_PM_DST_BUF_SIZE;
    u4SrcAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4SrcSize, 0x10000);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4DstSize, 0x10000);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0))
    {
        goto _exit;
    }

    _GenerateRandomPattern(u4SrcAddr, u4SrcSize);

    x_memset(_aPMSampleSpace, 0, sizeof(_aPMSampleSpace));
    x_memset(_aPMEventSpace, 0, sizeof(_aPMEventSpace));
    x_memset(_aPMDetectedSpace, 0, sizeof(_aPMDetectedSpace));
    _u1PMDetectedCount = 0;

    // Randomize the length (4 or 6 bytes) of a start code pattern.
    //_u1CodeLen = 4; //(random(2) == 0) ? 4 : 6;
    //u1CodeLen = _u1CodeLen;

    GCPU_PM_GenerateSampleSpace(u1StreamType);

    u1EventCount = random(PM_MAX_NUM_START_CODE - 1) + 2;
    GCPU_PM_SampleStartCodes(u4SrcAddr, u4SrcSize, u1EventCount);

    GCPU_PM_DestoryFakeStartCodes(u4SrcAddr, u4SrcSize, u1EventCount);

    // It's very important to flush invalidate D-cache.
    HalFlushInvalidateDCache();

    Printf("Src: 0x%08X, Dst: 0x%08X, start codes: %d, Overlapped: %s\n",
            u4SrcAddr, u4DstAddr, u1EventCount,
            (_fgPMOverlappedPatterns ? "YES" : "NO"));

#ifdef PM_DEBUG
    GCPU_PM_DebugStartCodes(u4SrcAddr, u4SrcSize, u1EventCount);
#endif  // PM_DEBUG

    GCPU_PM_SetPatternMask(u4SrcAddr, u4SrcSize, u1EventCount);

_exit:
    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4SrcAddr));

    return fgRet;
}
