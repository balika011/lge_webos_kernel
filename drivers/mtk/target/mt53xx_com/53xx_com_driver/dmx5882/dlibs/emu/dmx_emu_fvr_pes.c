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
#define FVR_PES_LEN_MAX             512
#define FVR_PES_LEN_MIN             256

#define TS_FVR_ES_ADDR              0x3000000
#define TS_FVR_PES_ADDR             0x3200000
#define TS_FVR_PACKET_ADDR          0x3400000
#define TS_FVR_RECORD_ADDR          0x3600000
#define TS_FVR_PLAY_PID_ADDR        0x3800000

#define TS_FVR_GOLDEN_ADDR          0x3A00000
#define TS_FVR_RESULT_ADDR          0x3C00000

#define TS_FVR_ES_SIZE              0x40000

#define TS_FVR_PID_SIZE             0x200000

#define MAX_START_CODE_COUNT        3
#define FVR_PES_PID                 0x10;


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT32 _u4PESOffset;
static UINT32 _u4TSPktAddr;
static UINT32 _u4TSResultAddr, _u4TSResultCount, _u4TSResultComCount;
static UINT32 _u4FVRESSize;

static UINT32 _u4SrcAddr_DDI = 0x0;
static UINT32 _u4DstAddr_DDI = 0x0;
static UINT32 _u4PesAddr_DDI = 0x0;
static UINT32 _u4PesSize_DDI = 0x0;


static UINT32 _au4StartCode[] =
{
    0x01000008,
    0x01000010,
    0x01000018
};

//static UINT32 _u4PullGoldenAddr, _u4PullCompCount;

static UINT8 _au1FVRPesHeader[] = { 0x00,0x00,0x01,0xe0,0x00,0x00, 0x85,0xc0,0x11,0x39,0x85,0xdb,
                                    0xe2,0x85,0xdb,0x9c,0x67,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,
                                    0xff,0xff
                                  };
static UINT32 _u4FVRPesHeaderLen = sizeof(_au1FVRPesHeader) / sizeof(UINT8);

static HANDLE_T _hDmxEmuDDIThread = NULL_HANDLE;
static HANDLE_T _hDmxEmuPVRThread = NULL_HANDLE;
static volatile BOOL _fgDmxEmuDDIEnable = FALSE;
static volatile BOOL _fgDmxEmuPVREnable = FALSE;

static HANDLE_T _hDmxEmuDDISema = NULL_HANDLE;
static HANDLE_T _hDmxEmuPVRSema = NULL_HANDLE;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Extern variables
//-----------------------------------------------------------------------------
// This extern variable is from dmx_emu_cmd.c
EXTERN UINT8 _u1TsIndex;

//-----------------------------------------------------------------------------
/** _DmxEmuLoopbackThread
 */
//-----------------------------------------------------------------------------
static VOID _DmxEmuDDIThread(VOID* pvArg)
{
    UINT32 u4Rp, u4Wp;

    UNUSED(pvArg);

    while (1)
    {
        if (_fgDmxEmuDDIEnable)
        {
            do
            {
                u4Rp = DDI_READ32(DDI_REG_DMA_RP);
                u4Wp = DDI_READ32(DDI_REG_DMA_WP);
            }
            while (u4Wp != u4Rp);

            VERIFY(x_sema_unlock(_hDmxEmuDDISema) == OSR_OK);
            Printf("DDI transfer done!\n");

            _DMX_Lock();
            _fgDmxEmuDDIEnable = FALSE;
            _DMX_Unlock();
        }

        x_thread_delay(10);
    }
}

//-----------------------------------------------------------------------------
/** _DmxEmuLoopbackThread
 */
//-----------------------------------------------------------------------------
static VOID _DmxEmuPVRThread(VOID* pvArg)
{
    DMX_PVR_PLAY_T rPlay;

    UNUSED(pvArg);

    while (1)
    {
        if (_fgDmxEmuPVREnable)
        {
            do
            {
                VERIFY(DMX_PVRPlay_Get(PVRPLAY_FLAGS_BUFFER, &rPlay));
            }
            while (rPlay.u4Rp != rPlay.u4Wp);

            VERIFY(x_sema_unlock(_hDmxEmuPVRSema) == OSR_OK);
            Printf("PVR transfer done!\n");

            _DMX_Lock();
            _fgDmxEmuPVREnable = FALSE;
            _DMX_Unlock();
        }
        x_thread_delay(10);
    }
}


//-----------------------------------------------------------------------------
/** _EmuPicSearchHandler
 */
//-----------------------------------------------------------------------------
static void _EmuPicSearchHandler(UINT8 u1Pidx, UINT8 u1Type, UINT32 u4PktIdx,
                                 const void* pvTag)
{
    UINT32 *pu4Addr;

    switch (u1Type)
    {
    case FVR_PIC_I_TYPE:
        Printf("Find I picture, pidx: %d, PktCount: %d\n", u1Pidx, u4PktIdx);
        //u1Pattern = 0;
        break;

    case FVR_PIC_P_TYPE:
        Printf("Find P picture, pidx: %d, PktCount: %d\n", u1Pidx, u4PktIdx);
        //u1Pattern = 1;
        break;

    case FVR_PIC_B_TYPE:
        Printf("Find B picture, pidx: %d, PktCount: %d\n", u1Pidx, u4PktIdx);
        //u1Pattern = 2;
        break;

    default:
        Printf("Unknown picture!\n");
        //u1Pattern = 3;
        break;
    }

    pu4Addr = (UINT32*)_u4TSResultAddr;
    if (u4PktIdx != pu4Addr[_u4TSResultComCount])
    {
        ASSERT(0);
    }

    _u4TSResultComCount++;

    UNUSED(pvTag);
}


//-----------------------------------------------------------------------------
/** _FormatPES
 */
//-----------------------------------------------------------------------------
static BOOL _FormatPES(UINT32 u4ESAddr, UINT32 u4ESSize, UINT32 u4PktCount,
                       UINT32 *pu4PesAddr, UINT32 *pu4PesSize, BOOL fgPicpat)
{
    static UINT32 _u4StartOffset = 0;
    UINT8 *pu1Ptr;
    UINT32 i;
    UINT32 u4StartCode, u4StartEnd;
    UINT32 *pu4Result;
    //UINT16 *pu2Ptr;

    u4StartCode = _au4StartCode[random(3)];
    pu1Ptr = (UINT8*)_u4PESOffset;

    for (i=0; i<_u4FVRPesHeaderLen; i++)
    {
        pu1Ptr[i] = _au1FVRPesHeader[i];
    }

    x_memcpy((void*)(_u4PESOffset + _u4FVRPesHeaderLen), (void*)u4ESAddr, u4ESSize);

    *pu4PesAddr = _u4PESOffset;
    *pu4PesSize = _u4FVRPesHeaderLen + u4ESSize;

    // PES length
//    pu2Ptr = (UINT16*)(_u4PESOffset + 4);
//    *pu2Ptr = *pu4PesSize - 6;

    if (fgPicpat)
    {
        _u4StartOffset++;
        _u4StartOffset = _u4StartOffset % 256;
        // 9 means pes start code to pes header pkt length
        pu1Ptr += (_u4FVRPesHeaderLen + 9 + _u4StartOffset);
        pu1Ptr[0] = 0x00;
        pu1Ptr[1] = 0x00;
        pu1Ptr[2] = (UINT8)((u4StartCode >> 24) & 0xFF);
        pu1Ptr[3] = (UINT8)((u4StartCode >> 16) & 0xFF);
        pu1Ptr[4] = (UINT8)((u4StartCode >> 8) & 0xFF);
        pu1Ptr[5] = (UINT8)(u4StartCode & 0xFF);

        // Save packet start code golden
        // 4 means ts packet header ,
        u4StartEnd = 4 + _u4FVRPesHeaderLen + 9 + _u4StartOffset;   //+5  // ucode use the packet index  where  fisrt finder 00  as the output  result, so we remove +5.
        pu4Result = (UINT32*)_u4TSResultAddr;
        pu4Result[_u4TSResultCount] = u4PktCount + (u4StartEnd / 188); // indicate the inserted picture start code is in which TS packet
        _u4TSResultCount++; // indicate how many picture start code do we insert
    }

    _u4PESOffset += (_u4FVRPesHeaderLen + u4ESSize);

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _CreateFVRTSHeader
 */
//-----------------------------------------------------------------------------
static void _CreateFVRTSHeader(UINT32 u4Addr, UINT8 *pu1Offset, BOOL fgPUSI)
{
    static UINT8 _u1CCCounter = 0;
    UINT8 *pu1Ptr;
    UINT8 u1AFLen, u1Offset, u1PUSI;

    pu1Ptr = (UINT8*)u4Addr;
    pu1Ptr[0] = 0x47;

    u1PUSI = 0x0;
    if (fgPUSI)
    {
        u1PUSI = 0x40;
    }
    pu1Ptr[1] = u1PUSI;
    pu1Ptr[2] = FVR_PES_PID;
    pu1Ptr[3] = _u1CCCounter & 0xF;

    _u1CCCounter++;

    u1AFLen = 0x0;
    /*
        if(_arPktHeader[u1PatIdx].fgAF)
        {
            Printf("Not support now\n");
            ASSERT(0);
        }
        else
    */
    {
        pu1Ptr[3] |= 0x10;
    }

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
static UINT32 _CreateFVRTSPacket(UINT32 u4PesAddr, UINT32 u4PesSize)
{
    BOOL fgFirst = TRUE;
    UINT8 *pu1Ptr, u1Offset;
    UINT32 u4CpSize, u4PktCount;

    u4PktCount = 0x0;

    while (u4PesSize > 0)
    {
        pu1Ptr = (UINT8*)_u4TSPktAddr;
        x_memset((VOID*)pu1Ptr, 0xFF, 188);
        _CreateFVRTSHeader(_u4TSPktAddr, &u1Offset, fgFirst);
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
    UINT32 u4PesAddr, u4PesSize;
    UINT32 u4PktCount;

    srand(BIM_READ32(REG_RW_TIMER0_LOW));

    //------------------------------------------
    // Generate ES
    //------------------------------------------
    pu4Ptr = (UINT32*)TS_FVR_ES_ADDR;
    for (i=0; i<(TS_FVR_ES_SIZE/4); i++)
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
    u4ESAddr = TS_FVR_ES_ADDR;
    u4ESLen = TS_FVR_ES_SIZE;
    u4PktCount = 1;     // start from 1 for fvr compare
    while (u4ESLen > 0)
    {
        if (u4ESLen < (((184 * 3) - _u4FVRPesHeaderLen) * 2))
        {
            break;
        }

        //u4Len = (rand() % FVR_PES_LEN_MAX) + FVR_PES_LEN_MIN;
        u4Len = (184 * 3) - _u4FVRPesHeaderLen;//320;        // Fixme

        // Format PES
        if (!_FormatPES(u4ESAddr, u4Len, u4PktCount, &u4PesAddr, &u4PesSize, fgPicpat))
        {
            return FALSE;
        }

        u4PktCount += _CreateFVRTSPacket(u4PesAddr, u4PesSize);

        u4ESLen -= u4Len;
        u4ESAddr += u4Len;
        _u4FVRESSize += u4Len;
    }

    _u4FVRESSize -= ((184 * 3) - _u4FVRPesHeaderLen);   // Last PES won't be golden, due to no next PUSI = 1

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _SetPVRPESTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetPVRPESTestPid(UINT8 u1PidIdx, BOOL fgPicSearch, UINT8 u1Tsidx)
{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1Tsidx;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = FALSE;

    if (fgPicSearch)
    {
        rFVRPid.ePidType = DMX_PID_TYPE_ES_VIDEO;
        rFVRPid.fgPicSearch = TRUE;
        rFVRPid.u1PicSearchMask = 0xF;
        rFVRPid.pfnPicSearchNotify = _EmuPicSearchHandler;
    }

    rFVRPid.u2Pid = FVR_PES_PID;

    if (!FVR_SetPid(u1PidIdx, FVR_PID_FLAG_ALL & (~FVR_PID_FLAG_KEY_LEN), &rFVRPid))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetPVRPESGBL
 */
//-----------------------------------------------------------------------------
static BOOL _SetPVRPESGBL(UINT8 u1TsIndex, BOOL fgTimestamp)
{
    FVR_GBL_T rGblPid;
    FVR_TIMESTAMP_T rTimeStamp;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = FALSE;
    rGblPid.u2TargetPktCount = 0xFFF0;
    rGblPid.u4BufStart = TS_FVR_RECORD_ADDR;
    rGblPid.u4BufSize = TS_FVR_PID_SIZE;
    //rGblPid.pfnNotifyFunc = NULL;
    //rGblPid.pvNotifyTag = NULL;

    if (!_FVR_SetGBL(u1TsIndex, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    if (fgTimestamp)
    {
        rTimeStamp.fgEnable = TRUE;
        rTimeStamp.u2ClkDiv = 8;
        rTimeStamp.u4Mask = 0xFFFFFFFF;
        rTimeStamp.u4Counter= 0x0;
        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _DMX_EMU_FVR_PES_Test
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_FVR_PES_Test(UINT8 u1TsIndex)
{
    BOOL fgRet = TRUE;
    UINT32 u4SrcDiff, u4DstDiff;

	ASSERT(u1TsIndex<=MAX_TS_INDEX);

    _u4PESOffset = TS_FVR_PES_ADDR;
    _u4TSPktAddr = TS_FVR_PACKET_ADDR;
    _u4TSResultAddr = TS_FVR_RESULT_ADDR;
    _u4TSResultCount = 0;
    _u4TSResultComCount = 0;
    _u4FVRESSize = 0;

    // Prepare ES
    if (!_DmxEmuGenPES(FALSE, TRUE))
    {
        return FALSE;
    }

    VERIFY(FVR_Init());
    VERIFY(_SetPVRPESTestPid(0, TRUE, u1TsIndex));
    VERIFY(_SetPVRPESGBL(u1TsIndex, FALSE));
    VERIFY(_FVR_Start(u1TsIndex));

	// set DBM input
    if (u1TsIndex == 3)
    {
        DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
    }

	if (u1TsIndex == 2)
	{
		DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
	}

    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        return FALSE;
    }

    if (!_DMX_EMU_DDI_Transfer(0, TS_FVR_PACKET_ADDR, _u4TSPktAddr - TS_FVR_PACKET_ADDR))
    {
        return FALSE;
    }

    x_thread_delay(50);

    if (_u4TSResultCount != _u4TSResultComCount)
    {
        Printf("%s,%d, start code num not match %d!=%d\n", __FUNCTION__, __LINE__,
               _u4TSResultCount, _u4TSResultComCount);
        fgRet = FALSE;
    }
	
    HalFlushInvalidateDCache();

    if (!_MemCmp(TS_FVR_PACKET_ADDR, TS_FVR_RECORD_ADDR, _u4TSPktAddr - TS_FVR_PACKET_ADDR,
                 &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }

    VERIFY(_FVR_Stop(u1TsIndex));

    VERIFY(_FVR_FreePid(0));
    VERIFY(_FVR_FreeGBL(u1TsIndex));

    return fgRet;
}


/*****************************************************************************/
/*****************************************************************************/

//-----------------------------------------------------------------------------
static BOOL _FVREmuGenTS(UINT8 u1TsIndex, UINT32 *pu4DataSize)
{
    FVR_GBL_T rGBL;
    HAL_TIME_T rTime1, rTime2, rTimeDiff;

    if (pu4DataSize == NULL)
    {
        ASSERT(0);
    }

    _u4PESOffset = TS_FVR_PES_ADDR;
    _u4TSPktAddr = TS_FVR_PACKET_ADDR;
    _u4TSResultAddr = TS_FVR_RESULT_ADDR;
    _u4TSResultCount = 0;
    _u4TSResultComCount = 0;
    _u4FVRESSize = 0;

    //-----------------------------------------
    // Generate TS
    //-----------------------------------------
    if (!_DmxEmuGenPES(TRUE, FALSE))
    {
        return FALSE;
    }

    //-----------------------------------------
    // Recording....
    //-----------------------------------------
    VERIFY(FVR_Init());
    VERIFY(_SetPVRPESTestPid(0, FALSE, u1TsIndex));
    VERIFY(_SetPVRPESGBL(u1TsIndex, TRUE));
    VERIFY(_FVR_Start(u1TsIndex));

    if(u1TsIndex==3)
    {
       DMX_SetDbm_InputSource(u1TsIndex,DMX_DBM_INPUT_FRAMER);
    }
	if (u1TsIndex==2)
	{
	   DMX_SetDbm_InputSource(u1TsIndex,DMX_DBM_INPUT_DDI);
	}

    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        return FALSE;
    }

#if 1
    // N/M Mbits/sec
    DDI_WRITE32(DDI_REG_PERIOD_M, 2 * 188 * 8 * 27);
    DDI_WRITE32(DDI_REG_PERIOD_N, 1);
    DDI_WRITE32(DDI_REG_RATE_CMD, 0x2);     // Update M and N
#endif

    HAL_GetTime(&rTime1);
    if (!_DMX_EMU_DDI_Transfer(0, TS_FVR_PACKET_ADDR, _u4TSPktAddr - TS_FVR_PACKET_ADDR))
    {
        return FALSE;
    }

    HAL_GetTime(&rTime2);

    HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
    Printf("Send time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);

    x_thread_delay(50);

    VERIFY(_FVR_Stop(u1TsIndex));

    VERIFY(_FVR_GetGBL(u1TsIndex, FVR_GBL_FLAG_BUFFER, &rGBL));
    *pu4DataSize = DATASIZE(rGBL.u4Rp, rGBL.u4Wp, rGBL.u4BufSize);

    VERIFY(_FVR_FreePid(0));
    VERIFY(_FVR_FreeGBL(u1TsIndex));

    Printf("Record pkt:0x%x, size:0x%x\n", TS_FVR_RECORD_ADDR, *pu4DataSize);

    return TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _FVREmuSetPlayPid(UINT8 u1TsIdx)
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
    rPid.u2Pid = FVR_PES_PID;
    rPid.u4BufAddr = TS_FVR_PLAY_PID_ADDR;
    rPid.u4BufSize = TS_FVR_PID_SIZE;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.u1ChannelId = 0;

    if (!_DMX_SetPid(0, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return  TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _FVREmuSetPVRPlay(UINT8 u1TsOutIdx, UINT32 u4DataSize)
{
    DMX_PVR_PLAY_T rPlay;

    // Play init
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = TS_FVR_RECORD_ADDR;
    rPlay.u4BufSize = u4DataSize;
    rPlay.u4ThresholdSize = 0x10000000;     // Not usable
    rPlay.pfnPVRNotify = NULL;

    if (!_DMX_PVRPlay_Set(u1TsOutIdx, (UINT32)(PVRPLAY_FLAGS_THRESHOLD |PVRPLAY_FLAGS_BUFFER |
                                   PVRPLAY_FLAGS_CALLBACK), &rPlay))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _FVREmuSetTimestamp(UINT8 u1TsOutIdx, BOOL fgOn)
{
    DMX_PVR_PLAY_T rPlay;

    if (fgOn)
    {
        rPlay.eMode = DMX_PVR_PLAY_STREAM;
    }
    else
    {
        rPlay.eMode = DMX_PVR_PLAY_SINGLEMOVE;
    }

    rPlay.fgContainTimeStamp = TRUE;
    rPlay.fgIgnoreTimeStamp = !fgOn;
    rPlay.u2TimeStampFreqDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
    if (!DMX_PVRPlay_SetEx(u1TsOutIdx, (UINT32)(PVRPLAY_FLAGS_MODE | PVRPLAY_FLAGS_TIMESTAMP), &rPlay))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
static BOOL _FVREmuCompareResult(UINT32 u4Test, UINT32 u4Golden, UINT32 u4Size)
{
    UINT8 *pu1Test, *pu1Golden;
    UINT32 i;

    // Compare data
    HalFlushInvalidateDCache();

    pu1Test = (UINT8*)u4Test;
    pu1Golden = (UINT8*)u4Golden;
    for (i=0; i<u4Size; i++)
    {
        if (pu1Test[i] != pu1Golden[i])
        {
            Printf("Compare error: idx: %d, %x!=%x\n", i, pu1Test[i], pu1Golden[i]);
            return FALSE;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** DMX_EMU_FVR_TimestampOnOffTest
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_FVR_TimestampOnOffTest(UINT8 u1TsIndex, BOOL fgTimeStampOn, UINT8 u1TsOutIdx,UINT8 u1_PlayTsIndex)
{
    DMX_MM_DATA_T rData;
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4DataSize, u4SendSize, u4Count, u4Addr;
    DMX_PID_T rPid;
    BOOL fgRet = TRUE;
    HAL_TIME_T rTime1, rTime2, rTimeDiff;

	ASSERT(u1_PlayTsIndex<=MAX_TS_INDEX);
	ASSERT(u1TsOutIdx<DMX_MAX_TSOUT);

    VERIFY(_FVREmuGenTS(u1TsIndex, &u4DataSize)); /*set record ts_idx*/

    VERIFY(_DMX_PVRPlay_Init());

    VERIFY(_FVREmuSetPVRPlay(u1TsOutIdx, u4DataSize));

    VERIFY(_FVREmuSetPlayPid(u1_PlayTsIndex));

	//Set TSout port,  why not tsindex 0,1????
	if (u1_PlayTsIndex == 2)
	{
	  _DMX_PVRPlay_SetPort(u1TsOutIdx, DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
	}
	else if (u1_PlayTsIndex == 3)
	{
	   _DMX_PVRPlay_SetPort(u1TsOutIdx, DMX_PVRPLAY_PORT_FRAMER3_BYPASS);
	}
	else if(u1_PlayTsIndex == 4)
	{
	  _DMX_PVRPlay_SetPort(u1TsOutIdx, DMX_PVRPLAY_PORT_DBM);
	}
	else
	{
	   ASSERT(0);
	}

    if (!fgTimeStampOn)
    {
        VERIFY(_FVREmuSetTimestamp(u1TsOutIdx,FALSE));

        HAL_GetTime(&rTime1);

        u4SendSize = u4DataSize;
        u4Addr = TS_FVR_RECORD_ADDR;
        while (u4SendSize > 0)
        {
            if (u4SendSize > 4096)
            {
                u4Count = ((random(20) * 192) + 192);
            }
            else
            {
                u4Count = u4SendSize;
            }

            x_memset((void*)&rData, 0, sizeof(rData));
            rData.u4BufStart = TS_FVR_RECORD_ADDR;
            rData.u4BufEnd = TS_FVR_RECORD_ADDR + TS_FVR_PID_SIZE;
            rData.u4StartAddr = u4Addr;
            rData.u4FrameSize = u4Count;
            VERIFY(_DMX_PVRPlay_SingleMove(u1TsOutIdx, &rData));

            u4Addr += u4Count;
            u4SendSize -= u4Count;
        }

        HAL_GetTime(&rTime2);
        HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
        Printf("Play time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);
    }
    else
    {
        VERIFY(_FVREmuSetTimestamp(u1TsOutIdx, TRUE));

        VERIFY(DMX_PVRPlay_SetBufferEx(u1TsOutIdx, TS_FVR_RECORD_ADDR, TS_FVR_RECORD_ADDR + TS_FVR_PID_SIZE - 1,
                                     TS_FVR_RECORD_ADDR + u4DataSize, TS_FVR_RECORD_ADDR));

        HAL_GetTime(&rTime1);
        VERIFY(DMX_PVRPlay_StartEx(u1TsOutIdx));

        while (1)
        {
            VERIFY(DMX_PVRPlay_GetEx(u1TsOutIdx, PVRPLAY_FLAGS_BUFFER, &rPlay));
            if (rPlay.u4Rp == rPlay.u4Wp)
            {
                break;
            }
            x_thread_delay(5);
        }

        VERIFY(DMX_PVRPlay_StopEx(u1TsOutIdx, TRUE));
        HAL_GetTime(&rTime2);
        HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
        Printf("Play time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);

    }

    VERIFY(_DMX_GetPid(0, DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_DATA_POINTERS, &rPid));
    if ( (rPid.u4Wp - rPid.u4BufAddr) != _u4FVRESSize)
    {
        fgRet = FALSE;
    }

    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(0));
    VERIFY(_FVR_FreePid(0));
    VERIFY(_DMX_PVRPlay_FreeBuf(u1TsOutIdx));

    if (fgRet)
    {
        fgRet = _FVREmuCompareResult(TS_FVR_PLAY_PID_ADDR, TS_FVR_ES_ADDR, _u4FVRESSize);
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_EMU_FVR_TimestampPauseTest
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_FVR_TimestampPauseTest(UINT8 u1TsIndex, UINT8 u1TsOutIdx,UINT8 u1_Play_TsIndex)
{
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4DataSize, u4PauseDelay;
    DMX_PID_T rPid;
    BOOL fgRet = TRUE;
    HAL_TIME_T rTime1, rTime2, rTimeDiff;
    UINT32 u4PauseCount = 0;
    FVR_GBL_T rGbl;
    DMX_PVRPLAY_PTR_T rPtr;

    VERIFY(_FVREmuGenTS(u1TsIndex, &u4DataSize));

    VERIFY(_FVREmuSetPVRPlay(u1TsOutIdx, u4DataSize));

    VERIFY(_FVREmuSetPlayPid(u1_Play_TsIndex));

    ////Set DBM input source, 
    if (u1_Play_TsIndex == 2)
   {
	  _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
   }
   else if (u1_Play_TsIndex == 3)
   {

	   _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER3_BYPASS);

   }
   else if(u1_Play_TsIndex == 4)
   {
	  _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_DBM);
   }
   else
   {
	   ASSERT(0);
   }

    VERIFY(_FVREmuSetTimestamp(u1TsOutIdx, TRUE));

    VERIFY(DMX_PVRPlay_SetBufferEx(u1TsOutIdx, TS_FVR_RECORD_ADDR, TS_FVR_RECORD_ADDR + TS_FVR_PID_SIZE - 1,
                                 TS_FVR_RECORD_ADDR + u4DataSize, TS_FVR_RECORD_ADDR));

    HAL_GetTime(&rTime1);
    VERIFY(DMX_PVRPlay_StartEx(u1TsOutIdx));

    while (1)
    {
        VERIFY(DMX_PVRPlay_GetEx(u1TsOutIdx, PVRPLAY_FLAGS_BUFFER, &rPlay));
        if (rPlay.u4Rp == rPlay.u4Wp)
        {
            break;
        }
        x_thread_delay(5);
        u4PauseDelay = random(200);
        x_thread_delay(u4PauseDelay);
        rPtr.u4Rp = rPlay.u4Rp;
        rPtr.u4Wp= rPlay.u4Wp;
        VERIFY(_DMX_PVRPlay_Pause(u1TsOutIdx, &rPtr));
        u4PauseCount++;
        Printf("Pause: %d\n", u4PauseCount);

        VERIFY(_FVR_GetGBL(0, FVR_GBL_FLAG_BUFFER, &rGbl));
        if (((rGbl.u4Rp - rGbl.u4BufStart) % 192) != 0)
        {
            Printf("Pause buffer not 192 alignment\n");
            fgRet = FALSE;
            break;
        }

        x_thread_delay(50);
        VERIFY(_DMX_PVRPlay_Resume(u1TsOutIdx, &rPtr));
    }

    VERIFY(DMX_PVRPlay_StopEx(u1TsOutIdx, TRUE));
    HAL_GetTime(&rTime2);
    HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
    Printf("Play time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);

    VERIFY(_DMX_GetPid(0, DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_DATA_POINTERS, &rPid));
    if ( (rPid.u4Wp - rPid.u4BufAddr) != _u4FVRESSize)
    {
        fgRet = FALSE;
    }

    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(0));
    VERIFY(_FVR_FreePid(0));
    VERIFY(_DMX_PVRPlay_FreeBuf(u1TsOutIdx));

    Printf("Pause count:%d\n", u4PauseCount);

    if (fgRet)
    {
        fgRet = _FVREmuCompareResult(TS_FVR_PLAY_PID_ADDR, TS_FVR_ES_ADDR, _u4FVRESSize);
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_EMU_FVR_TimestampErrTest
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_FVR_TimestampErrTest(UINT8 u1TsIndex, UINT8 u1TsOutIdx,UINT8 u1_Play_TsIndex)
{
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4DataSize;
    DMX_PID_T rPid;
    BOOL fgRet = TRUE;
    HAL_TIME_T rTime1, rTime2, rTimeDiff;
    UINT32 u4ErrTime, u4Addr;
    UINT32 u4Tmp, u4BaseTime, i;
    //UINT8 *u1p;

    VERIFY(_FVREmuGenTS(u1TsIndex, &u4DataSize));

    VERIFY(_FVREmuSetPVRPlay(u1TsOutIdx, u4DataSize));

    VERIFY(_FVREmuSetPlayPid(u1_Play_TsIndex));

	if (u1_Play_TsIndex == 2)
	{
		_DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
	}
	else if (u1_Play_TsIndex == 3)
	{
		_DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER3_BYPASS);
	}
	else if(u1_Play_TsIndex == 4)
	{
		_DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_DBM);
	}
	else
	{
		ASSERT(0);
	}
   
    VERIFY(_FVREmuSetTimestamp(u1TsOutIdx, TRUE));

    VERIFY(DMX_PVRPlay_SetBufferEx(u1TsOutIdx, TS_FVR_RECORD_ADDR, TS_FVR_RECORD_ADDR + TS_FVR_PID_SIZE - 1,
                                 TS_FVR_RECORD_ADDR + u4DataSize, TS_FVR_RECORD_ADDR));

    u4Tmp = _DMX_Align(u4DataSize / 2, 192);

    // Editing timestamp - overwrite packet 200 timestamp as packet 100, and reset the timestamp from the middle of data
    u4Addr = TS_FVR_RECORD_ADDR;
    u4BaseTime = *(UINT32*)(u4Addr + u4Tmp);
    for (i = u4Tmp; i < u4DataSize; i+=192)
    {
        *(UINT32*)(u4Addr + i) =  (*(UINT32*)(u4Addr + i)) - u4BaseTime;
    }
    u4ErrTime = *(UINT32*)(u4Addr + (192 * 100));
    *(UINT32*)(u4Addr + (192 * 200)) = u4ErrTime;
    Printf("Modify error timestamp\n");

    HalFlushInvalidateDCache();

    HAL_GetTime(&rTime1);
    VERIFY(DMX_PVRPlay_StartEx(u1TsOutIdx));

    while (1)
    {
        VERIFY(DMX_PVRPlay_GetEx(u1TsOutIdx, PVRPLAY_FLAGS_BUFFER, &rPlay));
        if (rPlay.u4Rp == rPlay.u4Wp)
        {
            break;
        }
        x_thread_delay(5);
    }

    VERIFY(DMX_PVRPlay_StopEx(u1TsOutIdx, TRUE));
    HAL_GetTime(&rTime2);
    HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
    Printf("Play time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);

    VERIFY(_DMX_GetPid(0, DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_DATA_POINTERS, &rPid));
    if ( (rPid.u4Wp - rPid.u4BufAddr) != _u4FVRESSize)
    {
        fgRet = FALSE;
    }

    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(0));
    VERIFY(_FVR_FreePid(0));
    VERIFY(_DMX_PVRPlay_FreeBuf(u1TsOutIdx));

    if (fgRet)
    {
        fgRet = _FVREmuCompareResult(TS_FVR_PLAY_PID_ADDR, TS_FVR_ES_ADDR, _u4FVRESSize);
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_EMU_FVR_TimestampResetTest
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_FVR_TimestampResetTest(UINT8 u1TsIndex, UINT8 u1TsOutIdx,UINT8 u1_Play_TsIndex)
{
    DMX_PVR_PLAY_T rPlay;
    UINT32 u4DataSize;
    DMX_PID_T rPid;
    BOOL fgRet = TRUE;
    HAL_TIME_T rTime1, rTime2, rTimeDiff;
    BOOL fgFirst = TRUE;

    VERIFY(_FVREmuGenTS(u1TsIndex, &u4DataSize));

    VERIFY(_FVREmuSetPVRPlay(u1TsOutIdx, u4DataSize));

    VERIFY(_FVREmuSetPlayPid(u1_Play_TsIndex));

   if (u1_Play_TsIndex == 2)
   {
	  _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);
   }
   else if (u1_Play_TsIndex == 3)
   {
	  _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER3_BYPASS);
   }
   else if(u1_Play_TsIndex == 4)
   {
	  _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_DBM);
   }
   else
   {
	  ASSERT(0);
   }

    VERIFY(_FVREmuSetTimestamp(u1TsOutIdx, TRUE));

    VERIFY(_DMX_PVRPlay_SetBuffer(u1TsOutIdx, TS_FVR_RECORD_ADDR, TS_FVR_RECORD_ADDR + TS_FVR_PID_SIZE - 1,
                                  TS_FVR_RECORD_ADDR + u4DataSize, TS_FVR_RECORD_ADDR));

    HAL_GetTime(&rTime1);
    VERIFY(DMX_PVRPlay_StartEx(u1TsOutIdx));

    while (1)
    {
        VERIFY(DMX_PVRPlay_GetEx(u1TsOutIdx, PVRPLAY_FLAGS_BUFFER, &rPlay));
        if (rPlay.u4Rp == rPlay.u4Wp)
        {
            break;
        }
        x_thread_delay(5);
        if (fgFirst)
        {
            x_thread_delay(random(1000));
            VERIFY(_DMX_PVRPlay_Stop(u1TsOutIdx, TRUE));
            Printf("Reset....\n");
            x_thread_delay(10);

            VERIFY(_DMX_FreePid(0));
            VERIFY(_FVREmuSetPlayPid(u1_Play_TsIndex));
            VERIFY(_DMX_PVRPlay_SetBuffer(u1TsOutIdx, TS_FVR_RECORD_ADDR, TS_FVR_RECORD_ADDR + TS_FVR_PID_SIZE - 1,
                                          TS_FVR_RECORD_ADDR + u4DataSize, TS_FVR_RECORD_ADDR));
            VERIFY(_DMX_PVRPlay_Start(u1TsOutIdx));
            fgFirst = FALSE;
            x_thread_delay(50);
        }
    }

    VERIFY(DMX_PVRPlay_StopEx(u1TsOutIdx, TRUE));
    HAL_GetTime(&rTime2);
    HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
    Printf("Play time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);

    VERIFY(_DMX_GetPid(0, DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_DATA_POINTERS, &rPid));
    if ( (rPid.u4Wp - rPid.u4BufAddr) != _u4FVRESSize)
    {
        fgRet = FALSE;
    }

    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(0));
    VERIFY(_FVR_FreePid(0));
    VERIFY(_DMX_PVRPlay_FreeBuf(u1TsOutIdx));

    if (fgRet)
    {
        fgRet = _FVREmuCompareResult(TS_FVR_PLAY_PID_ADDR, TS_FVR_ES_ADDR, _u4FVRESSize);
    }

    return fgRet;
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
static HANDLE_T _hPVRMTestSema = NULL_HANDLE;

static HAL_TIME_T _rPVRMTestTime;
static UINT32 _u4PVRMPktCount = 0;
//-----------------------------------------------------------------------------
static void _PVRMLPTimeCallback(FVR_GBL_NOTIFY_CODE_T eCode,
                                UINT32 u4Data, const void* pvTag)
{
    FVR_GBL_TIMENOTIFY_INFO_T *prTimeInfo;
    HAL_TIME_T rTime, rTimeDiff;
    UINT32 u4PktDiff;

    if (eCode == FVR_GBL_NFY_OVERFLOW)
    {
    	Printf("FVR_GBL_NFY_OVERFLOW!!!\n");
        VERIFY(x_sema_unlock(_hPVRMTestSema) == OSR_OK);
        return;
    }

    if (eCode != FVR_GBL_NFY_TIME)
    {
        return;
    }

    prTimeInfo = (FVR_GBL_TIMENOTIFY_INFO_T*)u4Data;

    HAL_GetTime(&rTime);
    HAL_GetDeltaTime(&rTimeDiff, &_rPVRMTestTime, &rTime);
    _rPVRMTestTime = rTime;

    u4PktDiff = prTimeInfo->u4PktCount - _u4PVRMPktCount;
    _u4PVRMPktCount = prTimeInfo->u4PktCount;

    LOG(1, "%s, %d:%06d, totalpktcnt:%d, pktdiff: %d, size: %d\n",
        __FUNCTION__, rTimeDiff.u4Seconds, rTimeDiff.u4Micros,
        _u4PVRMPktCount, u4PktDiff, prTimeInfo->u4Size);

    UNUSED(pvTag);
}


//-----------------------------------------------------------------------------
/** PVRM_Loopback_RecTimerTest
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_RecTimerTest(UINT32 u4BufSize)
{
    FVR_TIMESTAMP_T rTimeStamp;
    FVR_GBL_T rGBL;

    _u4PVRMPktCount = 0;

    if (_hPVRMTestSema == NULL_HANDLE)
    {
        VERIFY(OSR_OK == x_sema_create(&_hPVRMTestSema,
                                       X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
    }

    if (FVR_IsRecording(0))
    {
        LOG(3, "Please stop record first\n");
        return FALSE;
    }

    if (!FVR_FreeBuffer(0))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    rTimeStamp.fgEnable = TRUE;
    rTimeStamp.u2ClkDiv = 8;
    rTimeStamp.u4Mask = 0xFFFFFFFF;
    rTimeStamp.u4Counter= 0x0;
    FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);

    x_memset((void*)&rGBL, 0, sizeof(FVR_GBL_T));
    rGBL.fgAllocateBuffer = TRUE;
    rGBL.u2TargetPktCount = 0x0;
    rGBL.u4BufSize = u4BufSize;
    rGBL.pfnNotifyFunc = _PVRMLPTimeCallback;
    rGBL.pvNotifyTag = NULL;
    rGBL.u4TimeToNotify = 500;
    if (!FVR_SetGBL(0, (UINT32)(FVR_GBL_FLAG_TARGET_PKT | FVR_GBL_FLAG_BUFFER | FVR_GBL_FLAG_TIME_INFO |
                                FVR_GBL_FLAG_NOTIFY), &rGBL))
    {
        return FALSE;
    }

    if (!FVR_Start(0))
    {
        LOG(3, "Start fail\n");
        return FALSE;
    }

    HAL_GetTime(&_rPVRMTestTime);

    VERIFY(x_sema_lock(_hPVRMTestSema, X_SEMA_OPTION_WAIT) == OSR_OK);

    if (!FVR_Stop(0))
    {
        LOG(3, "Stop fail\n");
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** PVRM_Loopback_RecTimerTest2
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_RecTimerTest2(BOOL fgTimestamp, BOOL fgOverflow)
{
		FVR_GBL_T rGBL;
		UINT32 u4Reg;
		HAL_TIME_T rTime1, rTime2, rTimeDiff;

		UINT8 u1TsIndex = 0;
	
		_u4PESOffset = TS_FVR_PES_ADDR;
		_u4TSPktAddr = TS_FVR_PACKET_ADDR;
		_u4TSResultAddr = TS_FVR_RESULT_ADDR;
		_u4TSResultCount = 0;
		_u4TSResultComCount = 0;
		_u4FVRESSize = 0;

		_u4PVRMPktCount = 0;

		if (_hPVRMTestSema == NULL_HANDLE)
	    {
	        VERIFY(OSR_OK == x_sema_create(&_hPVRMTestSema,
	                                       X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
	    }
	
		//-----------------------------------------
		// Generate TS
		//-----------------------------------------
		if (!_DmxEmuGenPES(TRUE, FALSE))
		{
			return FALSE;
		}
	
		//-----------------------------------------
		// Recording....
		//-----------------------------------------
		VERIFY(FVR_Init());
		VERIFY(_SetPVRPESTestPid(0, FALSE, u1TsIndex));
		if (TRUE)
		{
		    FVR_GBL_T rGblPid;
		    FVR_TIMESTAMP_T rTimeStamp;
			UINT32 u4BufferSize = TS_FVR_PID_SIZE;

			// trigger overflow error interrupt if fgOverflow is TRUE
			if (fgOverflow)
			{
				if (fgTimestamp)
				{
					u4BufferSize = ((_u4TSPktAddr-TS_FVR_PACKET_ADDR)/188-1)*192;
				}
				else
				{
					u4BufferSize = _u4TSPktAddr-TS_FVR_PACKET_ADDR-188;
				}
			}

		    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
		    rGblPid.fgAllocateBuffer = FALSE;
		    rGblPid.u2TargetPktCount = 0;
		    rGblPid.u4BufStart = TS_FVR_RECORD_ADDR;
		    rGblPid.u4BufSize = u4BufferSize;  
		    rGblPid.pfnNotifyFunc = _PVRMLPTimeCallback;
			rGblPid.u4TimeToNotify = 100;
		    if (!_FVR_SetGBL(u1TsIndex, FVR_GBL_FLAG_ALL, &rGblPid))
		    {
		        return FALSE;
		    }

		    // Enabe Record timestamp
		    if (fgTimestamp)
	        {
		        rTimeStamp.fgEnable = TRUE;
		        rTimeStamp.u2ClkDiv = 8;
		        rTimeStamp.u4Mask = 0xFFFFFFFF;
		        rTimeStamp.u4Counter= 0x0;
		        _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);
		    }
		}
		VERIFY(_FVR_Start(u1TsIndex));
	
		if(u1TsIndex==3)
		{
		   DMX_SetDbm_InputSource(u1TsIndex,DMX_DBM_INPUT_FRAMER);
		}
		if (u1TsIndex==2)
		{
		   DMX_SetDbm_InputSource(u1TsIndex,DMX_DBM_INPUT_DDI);
		}
	
		if (!_DMX_EMU_DDI_Init(u1TsIndex))
		{
			return FALSE;
		}
	
#if 1
		// N/M Mbits/sec
		DDI_WRITE32(DDI_REG_PERIOD_M, 2 * 188 * 8 * 27);
		DDI_WRITE32(DDI_REG_PERIOD_N, 1);
		DDI_WRITE32(DDI_REG_RATE_CMD, 0x2); 	// Update M and N
#endif

		// no overflow interrupt in full pull mode, use half-push/full push mode instead
		_DMX_SetInputMode(DMX_HALF_PUSH);
	
		HAL_GetTime(&rTime1);
		if (!_DMX_EMU_DDI_Transfer(0, TS_FVR_PACKET_ADDR, _u4TSPktAddr - TS_FVR_PACKET_ADDR))
		{
			return FALSE;
		}
	
		HAL_GetTime(&rTime2);
	
		HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
		Printf("Send time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);
	
		x_thread_delay(50);

		if (fgOverflow)
		{
			VERIFY(x_sema_lock(_hPVRMTestSema, X_SEMA_OPTION_WAIT) == OSR_OK);
		}
	
		VERIFY(_FVR_Stop(u1TsIndex));
	
		VERIFY(_FVR_GetGBL(u1TsIndex, FVR_GBL_FLAG_BUFFER, &rGBL));
	
		VERIFY(_FVR_FreePid(0));
		VERIFY(_FVR_FreeGBL(u1TsIndex));
	
		Printf("Record pkt:0x%x, size:0x%x\n", TS_FVR_RECORD_ADDR,  
			DATASIZE(rGBL.u4Rp, rGBL.u4Wp, rGBL.u4BufSize));
	
		return TRUE;


}


BOOL _DMX_EMU_DBM_Auto_Switch_DDI_Init(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid)
{
    DMX_PID_T rPid;
    UINT32  u4DstEnd ,u4Flags, u4Reg;;
    const UINT8 u1Pidx = 0x1;
    BOOL fgRet = FALSE;
    UINT32 u4BufStart, u4BufEnd, u4Wp;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    if (!_DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }

    // Allocate source and destination buffers
    _u4SrcAddr_DDI = 0x3400000;
    _u4DstAddr_DDI = 0x3A00000;
    _u4PesAddr_DDI = 0x3E00000;
    _u4PesSize_DDI = 0x0;

    // Prepare data
    _GenerateRandomPattern(_u4SrcAddr_DDI, u4Size);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(_u4SrcAddr_DDI, u4Size, u2Pid, _u4PesAddr_DDI,
                                 &_u4PesSize_DDI))
    {
        goto _exit;
    }
    Printf("Auto_Switch_DDi.TS size %u, PES size = %u\n", u4Size, _u4PesSize_DDI);

    u4DstEnd = _DMX_Align(_u4DstAddr_DDI + u4Size + 1, DMX_ES_FIFO_ALIGNMENT);

    // Setup PID
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = _u4DstAddr_DDI;
    rPid.u4BufSize = u4DstEnd - _u4DstAddr_DDI;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Set FTI operation mode (pull push mode)
    u4Reg = DMXCMD_READ32(DMX_REG_CONFIG2);
    u4Reg &= 0x3fffffff;
    u4Reg |= ((UINT32)0x3 << 30);    // 0: full push, 1: half pull, 3: full pull
    DMXCMD_WRITE32(DMX_REG_CONFIG2, u4Reg);

    // Invoke DDI transfer
    if (!_DMX_IsAligned(_u4SrcAddr_DDI, DDI_BUF_ALIGNMENT))
    {
        // Incorrect address alignment
        goto _exit;
    }

    if (!_DMX_IsAligned(u4Size, DDI_POINTER_ALIGNMENT))
    {
        // Incorrect size alignment
        goto _exit;
    }

    u4BufStart = PHYSICAL(_u4SrcAddr_DDI);
    u4Wp = _DMX_Align(_u4SrcAddr_DDI + u4Size, DDI_POINTER_ALIGNMENT);
    u4BufEnd = _DMX_Align(u4Wp + 1, DDI_BUF_ALIGNMENT);

    // Flash and invalidate cache
    HalFlushInvalidateDCache();

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4BufStart);       // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    DMXCMD_WRITE32(49, 0); //0xf00170c4 FTUP TS PROCESSING RATE REGISTER 1
    DMXCMD_WRITE32(51, 0); //0xf00170cc FTUP TS PROCESSING RATE REGISTER 2

    fgRet = TRUE;
    return fgRet;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    return fgRet;
}

BOOL _DMX_EMU_DBM_Auto_Switch_Init(void)
{
    static BOOL _fgInit = FALSE;

    if (!_fgInit)
    {
        if (x_thread_create(&_hDmxEmuDDIThread, "DMXEMUDDI", 1024, 60, _DmxEmuDDIThread, 0, NULL) != OSR_OK)
        {
            Printf("Fail to create DMX emu ddi thread!\n");
            return FALSE;
        }

        if (x_thread_create(&_hDmxEmuPVRThread, "DMXEMUPVR", 1024, 60, _DmxEmuPVRThread, 0, NULL) != OSR_OK)
        {
            Printf("Fail to create DMX emu pvr thread!\n");
            return FALSE;
        }

        if (_hDmxEmuDDISema == NULL_HANDLE)
        {
            VERIFY(OSR_OK == x_sema_create(&_hDmxEmuDDISema,
                                           X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
        }

        if (_hDmxEmuPVRSema == NULL_HANDLE)
        {
            VERIFY(OSR_OK == x_sema_create(&_hDmxEmuPVRSema,
                                           X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));
        }
        _fgInit = TRUE;
    }

    _DMX_Lock();
    _fgDmxEmuDDIEnable = FALSE;
    _fgDmxEmuPVREnable = FALSE;
    _DMX_Unlock();

    return TRUE;
}

BOOL DMX_EMU_DBM_Auto_SwitchTest(UINT8 u1TsOutIdx,  BOOL fgEnable_AutoSwitch)
{
    //DMX_MM_DATA_T rData;
    UINT32 u4DataSize;
    DMX_PID_T rPid;
    BOOL fgRet = TRUE;
    UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4Reg;

    //init handle and sema
    VERIFY(_DMX_EMU_DBM_Auto_Switch_Init());

    VERIFY(_FVREmuGenTS(0, &u4DataSize));

    VERIFY(_FVREmuSetPVRPlay(u1TsOutIdx, u4DataSize));

	if(u1TsOutIdx == 2)
	{
	  VERIFY(_FVREmuSetPlayPid(4));
	}
	else
	{  
      VERIFY(_FVREmuSetPlayPid(2));
	}


    if (u1TsOutIdx == 0)
    {
        DMX_SetDbm_InputSource(2, DMX_DBM_INPUT_PB);
    }
    else if(u1TsOutIdx==1)
    {
        DMX_SetDbm_InputSource(2, DMX_DBM_INPUT_PB2);
    }
	else
	{
	   DMX_SetDbm_InputSource(4, DMX_DBM_INPUT_PB3);
	}

    VERIFY(_FVREmuSetTimestamp(u1TsOutIdx, FALSE));

    VERIFY(DMX_PVRPlay_SetBufferEx(u1TsOutIdx, TS_FVR_RECORD_ADDR, TS_FVR_RECORD_ADDR + TS_FVR_PID_SIZE - 1,
                                 TS_FVR_RECORD_ADDR + u4DataSize, TS_FVR_RECORD_ADDR));

    //ts index 0x0. size : 0x1004c . pid : 0x100.
    VERIFY(_DMX_EMU_DBM_Auto_Switch_DDI_Init(0x0, 0x14F9C, 0x100));

    //change DDI to Demux output byte delay to 0, let ddi move data faster to demux. make dbm error.
    u4Reg = DDI_READ32(DDI_REG_DMX_RX_CTRL);
    u4Reg &= ~(0xF);
    DDI_WRITE32(DDI_REG_DMX_RX_CTRL, u4Reg);

    //open auto switch or not
    if (fgEnable_AutoSwitch)
    {
        u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
        u4Reg |= ((UINT32)0x1 << 31);  //enable framer pre-full to control DDI and play back speed
        DMXCMD_WRITE32(DMX_REG_CONFIG4,u4Reg);
    }
    else
    {
        u4Reg = DMXCMD_READ32(DMX_REG_CONFIG4);
        u4Reg &= ~((UINT32)0x1 << 31);  //disable framer pre-full to control DDI and play back speed
        DMXCMD_WRITE32(DMX_REG_CONFIG4,u4Reg);
    }

    VERIFY(DMX_PVRPlay_StartEx(u1TsOutIdx));

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    _DMX_Lock();
    _fgDmxEmuPVREnable = TRUE;
    _fgDmxEmuDDIEnable = TRUE;
    _DMX_Unlock();

    //wait for complete
    VERIFY(x_sema_lock(_hDmxEmuPVRSema, X_SEMA_OPTION_WAIT) == OSR_OK);
    VERIFY(x_sema_lock(_hDmxEmuDDISema, X_SEMA_OPTION_WAIT) == OSR_OK);

    VERIFY(DMX_PVRPlay_StopEx(u1TsOutIdx, TRUE));

    //check DDI transfer result
    if (!_MemCmp(_u4PesAddr_DDI, _u4DstAddr_DDI, _u4PesSize_DDI, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        fgRet = FALSE;
    }


    VERIFY(_DMX_GetPid(0, DMX_PID_FLAG_BUFFER | DMX_PID_FLAG_DATA_POINTERS, &rPid));
    if ( (rPid.u4Wp - rPid.u4BufAddr) != _u4FVRESSize)
    {
        fgRet = FALSE;
    }

    //pvr play back
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(0, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(0));

    //handle ddi transfer
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(1, DMX_PID_FLAG_VALID, &rPid, FALSE));
    VERIFY(_DMX_FreePid(1));


    VERIFY(_FVR_FreePid(0));
    VERIFY(_DMX_PVRPlay_FreeBuf(u1TsOutIdx));

    if (fgRet)
    {
        fgRet = _FVREmuCompareResult(TS_FVR_PLAY_PID_ADDR, TS_FVR_ES_ADDR, _u4FVRESSize);
    }

    return fgRet;


}


