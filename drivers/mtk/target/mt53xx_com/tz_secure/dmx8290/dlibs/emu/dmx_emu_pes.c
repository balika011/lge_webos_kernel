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
 * $RCSfile: dmx_emu_fvr_pes.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_api.c
 *  Demux driver - main implementation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "dmx_emu_if.h"
#include "dmx.h"
#include "dmx_mm.h"
#include "dmx_debug.h"
#include "dmx_if.h"
#include "fvr.h"
#include "fvr_if.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"
#ifdef CHANNEL_CHANGE_LOG
#include "x_timer.h"
#endif
#include "x_rand.h"
#include "x_bim.h"
#include "x_hal_arm.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define TS_ES_SIZE          0x80000
#define TS_ES_ADDR          0x2800000//0x3400000
#define TS_RESULT_ADDR2     0x2C00000//0x3C00000
#define TS_GOLDEN_ADDR      0x3000000//0x3800000
#define TS_PES_ADDR         0x3400000//0x3800000
#define TS_RESULT_ADDR      0x3800000//0x3C00000
#define TS_PACKET_ADDR      0x3C00000
#define TS_RESULT_PERPID_SIZE   0x8000

#define MAX_TS_PACKET         30//4096
#define MIN_TS_PACKET         20


#define MAX_START_CODE_COUNT        3



typedef struct
{
    UINT16 u2PidNum;

    UINT32 u4BufAddr;
    UINT32 u4BufAddr2;
    UINT32 u4BufSize;

    UINT32 u4GoldenAddr;
    UINT32 u4EsSize;               // Golden size

    UINT32 u4PesAddr;              // PES size
    UINT32 u4PesSize;

    UINT32 u4PicInsert;

    UINT8 u1CCounter;

    BOOL fgOutRange;              //Video secure test
} PES_PATTERN_T;

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT32 _u4TSPktAddr;


static UINT8 _u1DmxTestPidNum = 32;

static PES_PATTERN_T _arDMXPES[DMX_NUM_PID_INDEX];

static UINT32 _au4StartCode[] =
{
    0x01000008,
    0x01000010,
    0x01000018
};

//static UINT32 _u4PullGoldenAddr, _u4PullCompCount;

static UINT8 _au1DMXPesHeader[] = { 0x00,0x00,0x01,0xe0,0x00,0x00, 0x85,0xc0,0x11,0x39,0x85,0xdb,
                                    0xe2,0x85,0xdb,0x9c,0x67,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,
                                    0xff,0xff
                                  };
static UINT32 _u4DMXPesHeaderLen = sizeof(_au1DMXPesHeader) / sizeof(UINT8);


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Extern variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/** _FormatPES
 */
//-----------------------------------------------------------------------------
static BOOL _DMX_FormatPES(UINT8 u1PidIdx, UINT32 u4ESAddr, UINT32 u4ESSize, UINT32 *pu4PesSize, BOOL fgPicpat)
{
    static UINT32 _u4StartOffset = 0;
    UINT8 *pu1Ptr;
    UINT32 i;
    UINT32 u4StartCode;

    u4StartCode = _au4StartCode[random(3)];
    pu1Ptr = (UINT8*)_arDMXPES[u1PidIdx].u4PesAddr;

    //Pes header
    for (i=0; i<_u4DMXPesHeaderLen; i++)
    {
        pu1Ptr[i] = _au1DMXPesHeader[i];
    }

    x_memcpy((void*)(_arDMXPES[u1PidIdx].u4PesAddr + _u4DMXPesHeaderLen), (void*)u4ESAddr, u4ESSize);

    //updata pes length
    _arDMXPES[u1PidIdx].u4PesSize += _u4DMXPesHeaderLen + u4ESSize;
    *pu4PesSize  = _u4DMXPesHeaderLen + u4ESSize;


    if (fgPicpat)
    {
        _u4StartOffset++;
        _u4StartOffset = _u4StartOffset % 256;
        // 9 means pes start code to pes header pkt length
        pu1Ptr += (_u4DMXPesHeaderLen + 9 + _u4StartOffset);
        pu1Ptr[0] = 0x00;
        pu1Ptr[1] = 0x00;
        pu1Ptr[2] = (UINT8)((u4StartCode >> 24) & 0xFF);
        pu1Ptr[3] = (UINT8)((u4StartCode >> 16) & 0xFF);
        pu1Ptr[4] = (UINT8)((u4StartCode >> 8) & 0xFF);
        pu1Ptr[5] = (UINT8)(u4StartCode & 0xFF);
        _arDMXPES[u1PidIdx].u4PicInsert++; // indicate how many picture start code do we insert
    }

    //update pes addr
    _arDMXPES[u1PidIdx].u4PesAddr += (_u4DMXPesHeaderLen + u4ESSize);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _CreateFVRTSHeader
 */
//-----------------------------------------------------------------------------
static void _CreateDMXTSHeader(UINT8 u1PidIdx, UINT32 u4Addr, UINT8 *pu1Offset, BOOL fgPUSI)
{
    UINT8 *pu1Ptr;
    UINT8 u1AFLen, u1Offset, u1PUSI;

    pu1Ptr = (UINT8*)u4Addr;
    pu1Ptr[0] = 0x47;

    u1PUSI = 0x0;
    if (fgPUSI)
    {
        u1PUSI = 0x40;
    }

    pu1Ptr[1] = u1PUSI | (UINT8)((_arDMXPES[u1PidIdx].u2PidNum & 0x1F00) >> 8);
    pu1Ptr[2] = (UINT8)(_arDMXPES[u1PidIdx].u2PidNum & 0xFF);
    pu1Ptr[3] = _arDMXPES[u1PidIdx].u1CCounter & 0xF;

    _arDMXPES[u1PidIdx].u1CCounter++;

    u1AFLen = 0x0;

    pu1Ptr[3] |= 0x10;

    if (fgPUSI)
    {
        pu1Ptr[4 + u1AFLen] = 0x0;  // Fixme
        //u1Offset = 1 + u1AFLen;
        u1Offset = u1AFLen;         // PES need not ptr field
    }
    else
    {
        u1Offset = 0x0;
    }

    *pu1Offset = u1Offset;
}


//-----------------------------------------------------------------------------
/** _CreateFVRTSPacket
 */
//-----------------------------------------------------------------------------
static UINT32 _CreateDMXTSPacket(UINT8 u1PidIdx, UINT32 u4PesAddr, UINT32 u4PesSize)
{
    BOOL fgFirst = TRUE;
    UINT8 *pu1Ptr, u1Offset;
    UINT32 u4CpSize, u4PktCount;

    u4PktCount = 0x0;

    while (u4PesSize > 0)
    {
        pu1Ptr = (UINT8*)_u4TSPktAddr;
        x_memset((VOID*)pu1Ptr, 0xFF, 188);
        _CreateDMXTSHeader(u1PidIdx, _u4TSPktAddr, &u1Offset, fgFirst);
        if (fgFirst)
        {
            fgFirst = FALSE;
        }

        u4CpSize = 188 - 4 - u1Offset;
        if (u4CpSize > u4PesSize)
        {
            u4CpSize = u4PesSize;
        }
        x_memcpy((VOID*)(_u4TSPktAddr + 4 + u1Offset), (VOID*)u4PesAddr, u4CpSize);

        _u4TSPktAddr += 188;
        u4PesAddr += u4CpSize;
        u4PesSize -= u4CpSize;
        u4PktCount++;
    }

    return u4PktCount;
}


//-----------------------------------------------------------------------------
/** _DmxEmuGenPES
 */
//-----------------------------------------------------------------------------
static BOOL _DmxEmuGenPES(BOOL fgRandom, BOOL fgPicpat)
{
    UINT32 *pu4Ptr, i;
    UINT32 u4ESAddr, u4ESLen, u4Len;
    UINT32 u4PktCount;
    UINT8  u1PidIdx;
    UINT32  u4PesSize;

    srand(BIM_READ32(REG_RW_TIMER0_LOW));

    //------------------------------------------
    // Alloc bufAddr , Size, Golden Addr
    //------------------------------------------
    for (i=0; i<_u1DmxTestPidNum; i++)
    {
        _arDMXPES[i].u2PidNum = i;

        //buffer size and addr
        _arDMXPES[i].u4BufAddr = TS_RESULT_ADDR + i * TS_RESULT_PERPID_SIZE;
        _arDMXPES[i].u4BufSize = TS_RESULT_PERPID_SIZE;
        _arDMXPES[i].u4BufAddr2 = TS_RESULT_ADDR2 + i * TS_RESULT_PERPID_SIZE;

        //Es size and addr
        _arDMXPES[i].u4GoldenAddr = TS_GOLDEN_ADDR + i * TS_RESULT_PERPID_SIZE;
        _arDMXPES[i].u4EsSize = 0;

        //Pes size and addr . more larger than es
        _arDMXPES[i].u4PesAddr = TS_PES_ADDR + i * (TS_RESULT_PERPID_SIZE + 0x30);
        _arDMXPES[i].u4PesSize = 0;

        //Pic number
        _arDMXPES[i].u4PicInsert = 0;

        _arDMXPES[i].u1CCounter = 0;

        //video secure test, default is TRUE
        _arDMXPES[i].fgOutRange = FALSE;


    }

    //------------------------------------------
    // Clear Result and Golden buffer
    //------------------------------------------
    x_memset((VOID*)TS_GOLDEN_ADDR, 0, TS_RESULT_PERPID_SIZE * _u1DmxTestPidNum);
    x_memset((VOID*)TS_RESULT_ADDR, 0, TS_RESULT_PERPID_SIZE * _u1DmxTestPidNum);
    x_memset((VOID*)TS_PES_ADDR, 0, TS_RESULT_PERPID_SIZE * _u1DmxTestPidNum);
    x_memset((VOID*)TS_RESULT_ADDR2, 0, TS_RESULT_PERPID_SIZE * _u1DmxTestPidNum);


    //------------------------------------------
    // Generate ES
    //------------------------------------------
    pu4Ptr = (UINT32*)TS_ES_ADDR;
    for (i=0; i<(TS_ES_SIZE/4); i++)
    {
        if (fgRandom)
        {
            //pu4Ptr[i] = rand();
            pu4Ptr[i] = i;
        }
        else
        {
            pu4Ptr[i] = 0xFFFFFFFF;
            //pu4Ptr[i] = (i + 0x10);
        }
    }

    //------------------------------------------
    // Generate PES and packets
    //------------------------------------------
    u4ESAddr = TS_ES_ADDR;
    u4ESLen = TS_ES_SIZE;
    u4PktCount = 0;     // start from 1 for fvr compare
    while (u4ESLen > 0)
    {
        //generate random es size
        u4Len = (UINT32)(random(MAX_TS_PACKET - MIN_TS_PACKET) + MIN_TS_PACKET) * 184 - _u4DMXPesHeaderLen;

        //check es size is enough
        if (u4Len > u4ESLen)   // Not enough buffer size
        {
            break;
        }

        //check random pid index size is enough
        while (1)
        {
            if (_u1DmxTestPidNum == 1)
            {
                u1PidIdx = 0;
            }
            else
            {
                u1PidIdx = (UINT8)(rand() % _u1DmxTestPidNum);

            }
            if ((_arDMXPES[u1PidIdx].u4BufSize - _arDMXPES[u1PidIdx].u4EsSize) > u4Len)
            {
                break;
            }

        }

        //update ES size
        _arDMXPES[u1PidIdx].u4EsSize += u4Len;
        x_memcpy((VOID*)_arDMXPES[u1PidIdx].u4GoldenAddr, (VOID*)u4ESAddr, u4Len);
        _arDMXPES[u1PidIdx].u4GoldenAddr += u4Len;
        // Format PES
        if (!_DMX_FormatPES(u1PidIdx, u4ESAddr, u4Len, &u4PesSize,  fgPicpat))
        {
            return FALSE;
        }

        u4PktCount += _CreateDMXTSPacket(u1PidIdx, _arDMXPES[u1PidIdx].u4PesAddr - u4PesSize, u4PesSize);

        u4ESLen -= u4Len;
        u4ESAddr += u4Len;
    }

   // _u4FVRESSize -= ((184 * 3) - _u4FVRPesHeaderLen);   // Last PES won't be golden, due to no next PUSI = 1
    Printf("TS packets: %d, \n",u4PktCount);
    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetVideoTestPid(UINT8 u1TsIdx, UINT8 u1PidIdx)
{
    DMX_PID_T rPid;

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = _arDMXPES[u1PidIdx].u2PidNum;
    rPid.u4BufAddr = _arDMXPES[u1PidIdx].u4BufAddr;
    rPid.u4BufSize = _arDMXPES[u1PidIdx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = u1PidIdx; //just for set 64 video pid.

    if (!_DMX_SetPid(u1PidIdx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return  TRUE;
}


//-----------------------------------------------------------------------------
/** _SetTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetSecureVideoTestPid(UINT8 u1TsIdx, UINT8 u1PidIdx ,BOOL fgOutRange)
{
    DMX_PID_T rPid;

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = _arDMXPES[u1PidIdx].u2PidNum;

    if (fgOutRange)
    {
        rPid.u4BufAddr = _arDMXPES[u1PidIdx].u4BufAddr2;
    }
    else
    {
        rPid.u4BufAddr = _arDMXPES[u1PidIdx].u4BufAddr;
    }

    rPid.u4BufSize = _arDMXPES[u1PidIdx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
	if(u1PidIdx%2 == 0)
	{ 
	  rPid.u1ChannelId = 0; //just for set 64 video pid.
	}
    else
    {
       rPid.u1ChannelId = 1;
    }

    if (!_DMX_SetPid(u1PidIdx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return  TRUE;
}

//-----------------------------------------------------------------------------
/** _FreeTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _FreeVideoTestPid(UINT8 u1PidIdx)
{
    DMX_PID_T rPid;

    // Setup PID
    rPid.fgEnable = FALSE;

    if (!_DMX_SetPid(u1PidIdx, DMX_PID_FLAG_VALID, &rPid, FALSE))
    {
        return FALSE;
    }

    _DMX_FreePid(u1PidIdx);

    return  TRUE;
}


BOOL DMX_VideoEstest (UINT8 u1TsIdx, PUL_MODE_T eMode, BOOL fgBypassFramer)
{
    UINT32 u4Reg, i;
    BOOL fgRet = TRUE;
    UINT32 u4SrcDiff, u4DstDiff;

    _u1DmxTestPidNum = DMX_NUM_PID_INDEX;
    _u4TSPktAddr = TS_PACKET_ADDR;

    _DmxEmuGenPES(TRUE, FALSE);

    Printf("Generate Video PES done!\n");

    //setup pid
    for (i=0; i<_u1DmxTestPidNum; i++)
    {
        _SetVideoTestPid(u1TsIdx, i);
    }

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIdx == 2) || (u1TsIdx == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIdx, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIdx, DMX_DBM_INPUT_FRAMER);
        }
    }

    // DDI
    if (!_DMX_EMU_DDI_Init(u1TsIdx))
    {
        return FALSE;
    }

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= (eMode << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktAddr - TS_PACKET_ADDR))
    {
        return FALSE;
    }

    //----------------------------------------
    // Compare result
    //----------------------------------------
    x_thread_delay(50);

    for (i=0; i<_u1DmxTestPidNum; i++)
    {

        if (_arDMXPES[i].u4EsSize > 0)
        {
            if (!_MemCmp(_arDMXPES[i].u4GoldenAddr - _arDMXPES[i].u4EsSize, _arDMXPES[i].u4BufAddr, _arDMXPES[i].u4EsSize,
                 &u4SrcDiff, &u4DstDiff))
            {
                Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
                fgRet = FALSE;
            }
        }
    }

    // Free PID
    for (i=0; i<_u1DmxTestPidNum; i++)
    {
        _FreeVideoTestPid(i);
    }

    return fgRet;
}


BOOL DMX_EMU_SecureBuffer_Test (UINT8 u1TsIdx, PUL_MODE_T eMode, BOOL fgBypassFramer)
{
    UINT32 u4Reg, i, j;
    BOOL fgRet = TRUE;
    UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4TotalSize;
    UINT8 *pu4BufAddr = NULL;

    _u1DmxTestPidNum = DMX_NUM_PID_INDEX;  //secure playback pid index support for 64.
    _u4TSPktAddr = TS_PACKET_ADDR;
    u4TotalSize = _u1DmxTestPidNum * TS_RESULT_PERPID_SIZE;

    //Generate video Pes
    _DmxEmuGenPES(TRUE, FALSE);

    Printf("Generate Video PES done!\n");

     //Set secure buffer region
    _DMX_SetSecurebufferRegion(TS_RESULT_ADDR, TS_RESULT_ADDR + u4TotalSize);
    _DMX_SetSecurebufferPattern(0xA5A5A5A5);
    _DMX_SetSecurebufferProtectEnable(TRUE);


    //setup pid
    for (i=0; i<_u1DmxTestPidNum; i++)
    {
        if (random(2) == 1)
        {
            _SetSecureVideoTestPid(u1TsIdx, i, TRUE);
            _arDMXPES[i].fgOutRange = TRUE;
            Printf("Pid Index : %d Out of Range!\n",i);
        }
        else
        {
            _SetSecureVideoTestPid(u1TsIdx, i, FALSE);
            _arDMXPES[i].fgOutRange = FALSE;
        }

        //Enable secure pid protect
        _DMX_SetSecurePidIndex(i, TRUE);
    }

    //Set DBM input source, only ts index 2 and 3 can config DBM input source for MT5890 and MT5861
    //only ts index 3 can config DBM source for MT5882
    if (u1TsIdx == 2)
    {
#if defined(CC_MT5890)
        if (!fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIdx, DMX_DBM_INPUT_FRAMER);
        }
        else
#endif
        {
            DMX_SetDbm_InputSource(u1TsIdx, DMX_DBM_INPUT_DDI);    
        }
    }
	
    if (u1TsIdx == 3)
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIdx, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIdx, DMX_DBM_INPUT_FRAMER);
        }
    }

#if defined(CC_MT5890)
    if(u1TsIdx == 4)
	{
	   DMX_SetDbm_InputSource(u1TsIdx, DMX_DBM_INPUT_DDI);
	}
#endif

    // DDI
    if (!_DMX_EMU_DDI_Init(u1TsIdx))
    {
        return FALSE;
    }

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= (eMode << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktAddr - TS_PACKET_ADDR))
    {
        return FALSE;
    }

    //----------------------------------------
    // Compare result
    //----------------------------------------
    x_thread_delay(50);

    for (i=0; i<_u1DmxTestPidNum; i++)
    {

        if (_arDMXPES[i].u4EsSize > 0)
        {
            if (!_arDMXPES[i].fgOutRange)
            {
                if (!_MemCmp(_arDMXPES[i].u4GoldenAddr - _arDMXPES[i].u4EsSize, _arDMXPES[i].u4BufAddr, _arDMXPES[i].u4EsSize,
                     &u4SrcDiff, &u4DstDiff))
                {
                    Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
                    fgRet = FALSE;
                }

            }
            else
            {
                if(i>=DMX_NUM_SECURE_PID_INDEX)
                {//secure protect fifo not be used
                    if (!_MemCmp(_arDMXPES[i].u4GoldenAddr - _arDMXPES[i].u4EsSize, _arDMXPES[i].u4BufAddr2, _arDMXPES[i].u4EsSize,
                     &u4SrcDiff, &u4DstDiff))
                   {
                    Printf("[Out of Range]Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
                    fgRet = FALSE;
                   }
                }
				else
				{
				  pu4BufAddr = (UINT8 *)_arDMXPES[i].u4BufAddr2;
                 for (j =0 ; j < _arDMXPES[i].u4EsSize ; j ++)
                 {
                    if (pu4BufAddr[j] != 0xA5)
                    {
                        Printf("Compare error, %d, %x != 0xA5\n", i, pu4BufAddr[j]);
                        fgRet = FALSE;
                    }
                 }
				}
                
            }
        }
    }

    // Free PID
    for (i=0; i<_u1DmxTestPidNum; i++)
    {
        _FreeVideoTestPid(i);
    }

    return fgRet;
}

