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
 * $RCSfile: dmx_emu_psi.c,v $
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
#include "dmx_mm.h"
#include "fvr.h"
#include "dmx_debug.h"
#include "dmx_if.h"
#include "fvr_if.h"
#include "dmx_drm_drvif.h"

#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"
#ifdef CHANNEL_CHANGE_LOG
#include "x_timer.h"
#endif
#include "x_rand.h"
#include "x_bim.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define SECTION_LEN_MAX         2048//4096
#define SECTION_LEN_MIN         64

#define TS_SECTION_ADDR         0x2000000//0x3000000
#define TS_PACKET_ADDR          0x2800000//0x3400000
#define TS_GOLDEN_ADDR          0x3000000//0x3800000
#define TS_RESULT_ADDR          0x3800000//0x3C00000

#define TS_SECTION_SIZE         0x80000
#define TS_RESULT_PERPID_SIZE   0x8000

#ifndef CC_MT5890
#define PVR_SECTION_FILTER   
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    UINT16 u2PidNum;
    BOOL fgAF;
    UINT8 u1CCounter;
} TS_PKT_HEADER_T;

typedef struct
{
    UINT8 u1Pidx;
    UINT16 u2PidNum;
    UINT8 u1FilterIdx;
    UINT8 au1Pat[16];
    UINT8 au1Mask[16];
    UINT8 au1PosNeg[16];
    UINT8 u1Offset;

    UINT8 u1CCounter;
    DMX_FILTER_MODE_T eMode;
    UINT32 u4BufAddr;
    UINT32 u4BufSize;
    UINT32 u4SecSize;               // Golden size
    UINT32 u4GoldenAddr;
} SECT_PATTERN_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static UINT32 _u4TSPktAddr, _u4TSPktOffset;

static UINT32 au4_crc32[256] =
{
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
    0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
    0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
    0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
    0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
    0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
    0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
    0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
    0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
    0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
    0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
    0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
    0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
    0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
    0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
    0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
    0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
    0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
    0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
    0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
    0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
    0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
    0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
    0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
    0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
    0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
    0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
    0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
    0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
    0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
    0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
    0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
    0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
    0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
    0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
    0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
    0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
    0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
    0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

static UINT8 _u1DmxEmuTestPidNum = 32;

//static SECT_PATTERN_T _arPat[DMX_NUM_PID_INDEX];

//static UINT32 _au4GoldenOffset[DMX_NUM_PID_INDEX];
//static BOOL _afgCompareResult[DMX_NUM_PID_INDEX];

static SECT_PATTERN_T _arPat[128];

static UINT32 _au4GoldenOffset[128];
static BOOL _afgCompareResult[128];


static HANDLE_T _hDmxEmuLPThread = NULL_HANDLE;
static volatile BOOL _fgDmxEmuLPEnable = FALSE;
static UINT8 _fgDmxEmuLPTSIndex = 0;
static UINT8 _fgDmxEmuLPTSOutIndex = 0;


static UINT8 _u1DmxTestPcrPidx = 0;
static UINT8 _u1DmxTestPcrSrcPidx = 0;
static BOOL _fgDmxTestPCR = FALSE;
static BOOL _fgDmxTestGenPacket = FALSE;
static UINT32 _u4DmxTestPcrIntCount = 0;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _DmxEmuLoopbackThread
 */
//-----------------------------------------------------------------------------
static VOID _DmxEmuLoopbackThread(VOID* pvArg)
{
    UINT32 u4RecWp1, u4RecWp2, u4PlayRp;

    UNUSED(pvArg);

    while (1)
    {
        if (_fgDmxEmuLPEnable)
        {
            do
            {
                u4RecWp1 = FVR_GBL_ARY_W(_fgDmxEmuLPTSIndex, 3);
                u4RecWp2 = FVR_GBL_ARY_W(_fgDmxEmuLPTSIndex, 3);
            }
            while (u4RecWp1 != u4RecWp2);

            _DMX_Lock();

            if (_fgDmxEmuLPTSOutIndex == 0)
            {
                u4PlayRp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP);
                DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP, u4RecWp1);
            }
            else
            {
                u4PlayRp = DMXCMD_READ32(DMX_REG_PVR_PLAY_BUF_RP2);
                DMXCMD_WRITE32(DMX_REG_PVR_PLAY_BUF_WP2, u4RecWp1);
            }
            // Update new pointer
            FVR_GBL_ARY_W(_fgDmxEmuLPTSIndex, 4) = u4PlayRp;

            _DMX_Unlock();
        }

        x_thread_delay(10);
    }
}


//-----------------------------------------------------------------------------
/** _generate_crc32
 */
//-----------------------------------------------------------------------------
UINT32 _generate_crc32(const UINT8 *pui1_data, SIZE_T z_size)
{
    SIZE_T z_i;
    UINT32 ui4_crc32;

    ui4_crc32 = (UINT32) 0xffffffff;
    if (pui1_data != (UINT8 *) NULL)
    {
        for (z_i = (SIZE_T) 0; z_i < z_size; z_i++)
        {
            ui4_crc32 = (ui4_crc32 << 8) ^
                        au4_crc32[(ui4_crc32 >> 24) ^ pui1_data[z_i]];
        }
    }

    return(ui4_crc32);
}


//-----------------------------------------------------------------------------
/** _GenShortMatchPattern
 */
//-----------------------------------------------------------------------------
static void _GenShortMatchPattern(void)
{
    UINT32 i, j;

    for (i=0; i<_u1DmxEmuTestPidNum ; i++)
    {
        _arPat[i].au1Pat[0] = i;
        _arPat[i].au1Mask[0] = 0xFF;
        for (j=1; j<16; j++)
        {
            _arPat[i].au1Pat[j] = random(256);
            _arPat[i].au1Mask[j] = random(256);
        }
    }
}

//-----------------------------------------------------------------------------
/** _GenLongMatchPattern
 */
//-----------------------------------------------------------------------------
static void _GenLongMatchPattern(void)
{
    UINT32 i, j;

    for (i=0; i<_u1DmxEmuTestPidNum ; i++)
    {
        _arPat[i].au1Pat[0] = i;
        _arPat[i].au1Mask[0] = 0xFF;
        for (j=1; j<16; j++)
        {
            _arPat[i].au1Pat[j] = random(256);
            _arPat[i].au1Mask[j] = random(256);
        }
    }
}

//-----------------------------------------------------------------------------
/** _GenPosNegMatchPattern
 */
//-----------------------------------------------------------------------------
static void _GenPosNegMatchPattern(void)
{
    UINT32 i, j;

    for (i=0; i<_u1DmxEmuTestPidNum ; i++)
    {
        _arPat[i].au1Pat[0] = i;
        _arPat[i].au1Mask[0] = 0xFF;
        for (j=1; j<16; j++)
        {
            _arPat[i].au1Pat[j] = random(256);
        }

        for (j=0; j<4; j++)
        {
            _arPat[i].au1Mask[j] = 0xFF;    //random(256);
            _arPat[i].au1Mask[j+4] = 0x0;
            _arPat[i].au1Mask[j+8] = 0x0;
            _arPat[i].au1Mask[j+12] = 0xFF; //random(256);
        }
    }
}

//-----------------------------------------------------------------------------
/** _GenGenericMatchPattern
 */
//-----------------------------------------------------------------------------
static void _GenGenericMatchPattern(void)
{
    UINT32 i, j;

    for (i=0; i<_u1DmxEmuTestPidNum ; i++)
    {
        _arPat[i].au1Pat[0] = i;
        _arPat[i].au1Mask[0] = 0xFF;
        _arPat[i].au1PosNeg[0] = 0;

        _arPat[i].au1Pat[1] = random(256);
        _arPat[i].au1Mask[1] = 0xFF;
        _arPat[i].au1PosNeg[1] = 0;             // Positive
        _arPat[i].au1Pat[2] = random(256);
        _arPat[i].au1Mask[2] = 0xFF;
        _arPat[i].au1PosNeg[2] = 0xFF;          // Negative
        for (j=3; j<16; j++)    // 20101102
        {
            _arPat[i].au1Pat[j] = random(256);
            _arPat[i].au1Mask[j] = random(256);
            _arPat[i].au1PosNeg[j] = random(256);
        }
    }
}

//-----------------------------------------------------------------------------
/** _GenFilterPattern
 */
//-----------------------------------------------------------------------------
static void _GenFilterPattern(DMX_FILTER_MODE_T eMode)
{
    UINT32 i;

    srand(BIM_READ32(REG_RW_TIMER0_LOW));

    switch (eMode)
    {
    case DMX_FILTER_MODE_SHORT:
        _GenShortMatchPattern();
        break;

    case DMX_FILTER_MODE_14_2:
        break;

    case DMX_FILTER_MODE_LONG:
        _GenLongMatchPattern();
        break;

    case DMX_FILTER_MODE_POS_NEG:
        _GenPosNegMatchPattern();
        break;

    case DMX_FILTER_MODE_GENERIC:
        _GenGenericMatchPattern();
        break;

    default:
        ASSERT(0);
    }

    // Fill other property
    for (i=0; i<_u1DmxEmuTestPidNum; i++)
    {
        _arPat[i].u1Pidx = i;
        _arPat[i].u2PidNum = i;
        _arPat[i].u1FilterIdx = i;
        _arPat[i].u1Offset = (UINT8)random(13) + 3; // skip section length field
        _arPat[i].u1CCounter = 0;
        _arPat[i].eMode = eMode;
        _arPat[i].u4BufAddr = TS_RESULT_ADDR + i * TS_RESULT_PERPID_SIZE;
        _arPat[i].u4BufSize = TS_RESULT_PERPID_SIZE;
        _arPat[i].u4SecSize = 0x0;
        _arPat[i].u4GoldenAddr = TS_GOLDEN_ADDR + i * TS_RESULT_PERPID_SIZE;
    }

    if (i == 1)
    {
        _arPat[0].u4BufSize = 0x100000;
    }

}


//-----------------------------------------------------------------------------
/** _FillShortHeader
 */
//-----------------------------------------------------------------------------
static void _FillShortHeader(UINT8 u1PatIdx, UINT8* pu1Ptr, BOOL fgMatch)
{
    UINT32 i;
    UINT8 u1ErrPos, u1ErrOffset;

    for (i=1; i<16; i++)
    {
        pu1Ptr[_arPat[u1PatIdx].u1Offset + i - 1] =
            _arPat[u1PatIdx].au1Pat[i] & _arPat[u1PatIdx].au1Mask[i];
    }

    if (!fgMatch)
    {
        u1ErrPos = random(8);
        if (_arPat[u1PatIdx].au1Mask[u1ErrPos] == 0)
        {
            u1ErrPos = 0;
        }

        if (u1ErrPos == 0)
        {
            pu1Ptr[0] = ~pu1Ptr[0];
        }

        u1ErrOffset = _arPat[u1PatIdx].u1Offset + u1ErrPos - 1;
        pu1Ptr[u1ErrOffset] = ~pu1Ptr[u1ErrOffset];
    }
}


//-----------------------------------------------------------------------------
/** _FillLongHeader
 */
//-----------------------------------------------------------------------------
static void _FillLongHeader(UINT8 u1PatIdx, UINT8* pu1Ptr, BOOL fgMatch)
{
    UINT32 i;
    UINT8 u1ErrPos, u1ErrOffset;

    for (i=1; i<16; i++)
    {
        pu1Ptr[_arPat[u1PatIdx].u1Offset + i - 1] =
            _arPat[u1PatIdx].au1Pat[i] & _arPat[u1PatIdx].au1Mask[i];
    }

    if (!fgMatch)
    {
        u1ErrPos = random(16);
        if (_arPat[u1PatIdx].au1Mask[u1ErrPos] == 0)
        {
            u1ErrPos = 0;
        }

        if (u1ErrPos == 0)
        {
            pu1Ptr[0] = ~pu1Ptr[0];
        }

        u1ErrOffset = _arPat[u1PatIdx].u1Offset + u1ErrPos - 1;

        pu1Ptr[u1ErrOffset] = ~pu1Ptr[u1ErrOffset];
    }
}

//-----------------------------------------------------------------------------
/** _FillPosNegHeader
 */
//-----------------------------------------------------------------------------
static void _FillPosNegHeader(UINT8 u1PatIdx, UINT8* pu1Ptr, BOOL fgMatch)
{
    UINT32 i;
    UINT8 u1ErrPos, u1ErrOffset;

    // Bank0
    for (i=1; i<4; i++)
    {
        pu1Ptr[_arPat[u1PatIdx].u1Offset + i - 1] =
            _arPat[u1PatIdx].au1Pat[i] & _arPat[u1PatIdx].au1Mask[i];
    }

    // Bank1
    for (i=4; i<8; i++)
    {
        pu1Ptr[_arPat[u1PatIdx].u1Offset + i - 1] =
            _arPat[u1PatIdx].au1Pat[i+8] & _arPat[u1PatIdx].au1Mask[i+8];
    }

    if (fgMatch)
    {
        u1ErrPos = random(4);
        u1ErrOffset = _arPat[u1PatIdx].u1Offset + 3 + u1ErrPos;
        pu1Ptr[u1ErrOffset] = ~pu1Ptr[u1ErrOffset];
    }
}


//-----------------------------------------------------------------------------
/** _FillGenericHeader
 */
//-----------------------------------------------------------------------------
static void _FillGenericHeader(UINT8 u1PatIdx, UINT8* pu1Ptr, BOOL fgMatch)
{
    UINT32 i;
    UINT8 u1ErrPos, u1ErrOffset, u1PosNeg, u1Data1, u1Data2;

    for (i=1; i<16; i++)
    {
        pu1Ptr[_arPat[u1PatIdx].u1Offset + i - 1] =
            _arPat[u1PatIdx].au1Pat[i] & _arPat[u1PatIdx].au1Mask[i];
    }

    if (fgMatch)
    {
        u1ErrPos = random(13) + 3;
        u1PosNeg = _arPat[u1PatIdx].au1PosNeg[u1ErrPos];
        if ((_arPat[u1PatIdx].au1Mask[u1ErrPos] & u1PosNeg) == 0)
        {
            u1ErrPos = 2;   // mask = 0xFF, posneg = 0xFF;  -> very important
            u1PosNeg = _arPat[u1PatIdx].au1PosNeg[2];
        }

        u1ErrOffset = _arPat[u1PatIdx].u1Offset + u1ErrPos - 1;
        u1Data1 = pu1Ptr[u1ErrOffset] & (~u1PosNeg);     // Keep positive
        u1Data2 =  ~(pu1Ptr[u1ErrOffset] & u1PosNeg) & u1PosNeg;    // Reverse negative
        pu1Ptr[u1ErrOffset] = u1Data1 | u1Data2;
    }

    /*
        u1ErrPos = random(7) + 1;   // skip table_id //random(13) + 3;
        if(_arPat[u1PatIdx].au1Mask[u1ErrPos] == 0)
        {
            u1ErrPos = 1;   // offset 1 always contain mask 0xFF
        }
        u1PosNeg = _arPat[u1PatIdx].au1PosNeg[u1ErrPos];
        u1ErrOffset = _arPat[u1PatIdx].u1Offset + u1ErrPos - 1;

        if(fgMatch)
        {
            if((_arPat[u1PatIdx].au1Mask[u1ErrPos] & u1PosNeg) != 0)    // Contain negative
            {
                u1Data1 = pu1Ptr[u1ErrOffset] & (~u1PosNeg);     // Keep positive
                u1Data2 =  ~(pu1Ptr[u1ErrOffset] & u1PosNeg) & u1PosNeg;    // Reverse negative
                pu1Ptr[u1ErrOffset] = u1Data1 | u1Data2;
            }
        }
        else
        {
            if((_arPat[u1PatIdx].au1Mask[u1ErrPos] & u1PosNeg) == 0)    // Contain positive
            {
                pu1Ptr[u1ErrOffset] = ~(pu1Ptr[u1ErrOffset] & _arPat[u1PatIdx].au1Mask[u1ErrPos]);
            }
        }
    */

}


//-----------------------------------------------------------------------------
/** _InsertError
 */
//-----------------------------------------------------------------------------
static void _FillHeader(UINT8 u1PatIdx, UINT8* pu1Ptr, BOOL fgMatch)
{
    switch (_arPat[u1PatIdx].eMode)
    {
    case DMX_FILTER_MODE_SHORT:
        _FillShortHeader(u1PatIdx, pu1Ptr, fgMatch);
        break;

    case DMX_FILTER_MODE_14_2:
        break;

    case DMX_FILTER_MODE_LONG:
        _FillLongHeader(u1PatIdx, pu1Ptr, fgMatch);
        break;

    case DMX_FILTER_MODE_POS_NEG:
        _FillPosNegHeader(u1PatIdx, pu1Ptr, fgMatch);
        break;

    case DMX_FILTER_MODE_GENERIC:
        _FillGenericHeader(u1PatIdx, pu1Ptr, fgMatch);
        break;

    default:
        ASSERT(0);
    }
}

//-----------------------------------------------------------------------------
/** _FillSectionHeader
 */
//-----------------------------------------------------------------------------
static void _FillSectionHeader(UINT8 u1PatIdx, UINT32 u4Addr, UINT32 u4Size, BOOL fgMatch)
{
    UINT32 u4SecLen, u4CRC, u4Offset;
    UINT8 *pu1Ptr;

    pu1Ptr = (UINT8*)u4Addr;
    u4SecLen = u4Size - 3;

    // Header
    pu1Ptr[0] = _arPat[u1PatIdx].au1Pat[0];     // Table ID
    pu1Ptr[1] = (UINT8)(((u4SecLen & 0xFFF) >> 8) & 0xFF) | 0x80;
    pu1Ptr[2] = (UINT8)(u4SecLen & 0xFF);

    _FillHeader(u1PatIdx, pu1Ptr, fgMatch);

    // CRC
    u4CRC = _generate_crc32((UINT8*)u4Addr, u4Size - 4);

    pu1Ptr[u4Size - 4] = (UINT8)((u4CRC >> 24) & 0xFF);
    pu1Ptr[u4Size - 3] = (UINT8)((u4CRC >> 16) & 0xFF);
    pu1Ptr[u4Size - 2] = (UINT8)((u4CRC >> 8) & 0xFF);
    pu1Ptr[u4Size - 1] = (UINT8)(u4CRC  & 0xFF);

    if (fgMatch)
    {
        u4Offset = _arPat[u1PatIdx].u4GoldenAddr + _arPat[u1PatIdx].u4SecSize;
        x_memcpy((VOID*)u4Offset, (VOID*)u4Addr, u4Size);
        _arPat[u1PatIdx].u4SecSize += u4Size;
    }
}


//-----------------------------------------------------------------------------
/** _CreateTSHeader
 */
//-----------------------------------------------------------------------------
static void _CreateTSHeader(UINT8 u1PatIdx, UINT32 u4Addr, UINT8 *pu1Offset, BOOL fgPUSI)
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
    pu1Ptr[1] = u1PUSI | (UINT8)((_arPat[u1PatIdx].u2PidNum & 0x1F00) >> 8);
    pu1Ptr[2] = (UINT8)(_arPat[u1PatIdx].u2PidNum & 0xFF);
    pu1Ptr[3] = _arPat[u1PatIdx].u1CCounter & 0xF;

    _arPat[u1PatIdx].u1CCounter++;

    u1AFLen = 0x0;

    if ((u1PatIdx == _u1DmxTestPcrSrcPidx) && _fgDmxTestPCR && !_fgDmxTestGenPacket)
    {
        pu1Ptr[3] |= 0x30;
        u1AFLen = 8;

        pu1Ptr[4] = 7;
        pu1Ptr[5] = 0x10;
        pu1Ptr[6] = 0xFF;
        pu1Ptr[7] = 0xFF;
        pu1Ptr[8] = 0xFF;
        pu1Ptr[9] = 0xFF;
        pu1Ptr[10] = 0xFF;
        pu1Ptr[11] = 0xFF;

        _fgDmxTestGenPacket = TRUE;
        LOG(3, "Generate PCR packet\n");
    }
    else
    {
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
    }

    if (fgPUSI)
    {
        pu1Ptr[4 + u1AFLen] = 0x0;  // Fixme
        u1Offset = 1 + u1AFLen;
    }
    else
    {
        u1Offset = 0x0;
    }

    *pu1Offset = u1Offset;
}


//-----------------------------------------------------------------------------
/** _CreateTSPacket
 */
//-----------------------------------------------------------------------------
static UINT32 _CreateTSPacket(UINT8 u1PatIdx, UINT32 u4SecAddr, UINT32 u4SecSize)
{
    BOOL fgFirst = TRUE;
    UINT8 *pu1Ptr, u1Offset;
    UINT32 u4CpSize, u4PktCount;

    u4PktCount = 0x0;

    while (u4SecSize > 0)
    {
        pu1Ptr = (UINT8*)_u4TSPktOffset;
        x_memset((VOID*)pu1Ptr, 0xFF, 188);
        _CreateTSHeader(u1PatIdx, _u4TSPktOffset, &u1Offset, fgFirst);
        if (fgFirst)
        {
            fgFirst = FALSE;
        }

        u4CpSize = 188 - 4 - u1Offset;
        if (u4CpSize > u4SecSize)
        {
            u4CpSize = u4SecSize;
        }
        x_memcpy((VOID*)(_u4TSPktOffset + 4 + u1Offset), (VOID*)u4SecAddr, u4CpSize);

        _u4TSPktOffset += 188;
        u4SecAddr += u4CpSize;
        u4SecSize -= u4CpSize;
        u4PktCount++;
    }

    return u4PktCount;
}

//-----------------------------------------------------------------------------
/** _CreateTSStream
 */
//-----------------------------------------------------------------------------
static void _CreateTSStream(UINT32 u4Addr, UINT32 u4Size)
{
    UINT32 i, u4RemainSize, u4SecSize, u4SecAddr;
    UINT8 *pu1Ptr;
    BOOL fgMatch;
    UINT32 u4MatchCount, u4SectionCount, u4PktCount, u4TotalPktCount;
    UINT8 u1PatIdx;
//    static UINT8 _u1Index = 0;

    u4MatchCount = 0;
    u4SectionCount = 0;
    u4TotalPktCount = 0;

    srand(BIM_READ32(REG_RW_TIMER0_LOW));

    // Prepare buffer data
    pu1Ptr = (UINT8*)u4Addr;
    for (i=0; i<u4Size; i++)
    {
        pu1Ptr[i] = (UINT8)(rand() % 256);
        //pu1Ptr[i] = 0x5A; /*use fixed pattern to debug it*/
    }

    // Prepare section header
    u4RemainSize = u4Size;
    u4SecAddr = u4Addr;
    while (u4RemainSize > 0)
    {
        while (1)
        {
            u4SecSize = (UINT32)rand() %  SECTION_LEN_MAX;
            if (u4SecSize > SECTION_LEN_MIN)
            {
                break;
            }
        }

        if (u4SecSize > u4RemainSize)   // Not enough buffer size
        {
            break;
        }
#if 1
        if (_u1DmxEmuTestPidNum == 1)
        {
                fgMatch = TRUE;
		        u4MatchCount++;
        }
		else
		{
		    if ((rand() % 100) > 80)
		    {
		        fgMatch = FALSE;
		    }
		    else
		    {
		        fgMatch = TRUE;
		        u4MatchCount++;
		    }
		  
		}

#else
        fgMatch = FALSE;
#endif
        u4SectionCount++;

        while (1)
        {
#if 1
            if (_u1DmxEmuTestPidNum == 1)
            {
                u1PatIdx = 0;
            }
            else
            {
                u1PatIdx = (UINT8)(rand() % _u1DmxEmuTestPidNum);
                /*
                                u1PatIdx = _u1Index++;
                                if(_u1Index >= _u1DmxEmuTestPidNum)
                                {
                                    _u1Index = 0;
                                }
                */
            }
            if ((_arPat[u1PatIdx].u4BufSize - _arPat[u1PatIdx].u4SecSize) > u4SecSize)
            {
                break;
            }
#else
            u1PatIdx = 1;//(UINT8)(rand() % 32);  // eddie_test
            if ((_arPat[u1PatIdx].u4BufSize - _arPat[u1PatIdx].u4SecSize) > u4SecSize)
            {
                break;
            }
            // eddie_test
            else
            {
                u4RemainSize = 0;
                continue;
            }
#endif
        }

        _FillSectionHeader(u1PatIdx, u4SecAddr, u4SecSize, fgMatch);
        u4PktCount = _CreateTSPacket(u1PatIdx, u4SecAddr, u4SecSize);
        u4TotalPktCount += u4PktCount;
        /*Printf("u4SecAddr=0x%8X,u4SecSize=0x%8X,u1PatIdx=%d,fgMatch=%d.\r\n",
			   u4SecAddr,u4SecSize,u1PatIdx,fgMatch);*/
        u4SecAddr += u4SecSize;
        u4RemainSize -= u4SecSize;

    }

    Printf("TS packets: %d, Section: %d, Match section %d\n",
           u4TotalPktCount, u4SectionCount, u4MatchCount);

}


//-----------------------------------------------------------------------------
static BOOL _DmxEmuPSINotify(UINT8 u1Pidx, DMX_NOTIFY_CODE_T eCode,
                             UINT32 u4Data, const void* pvNotifyTag)
{
    DMX_NOTIFY_INFO_PSI_T *prPsi;
    UINT8 *pu1Test, *pu1Golden;
    UINT32 i, j, u4Len, u4BufEnd;

    if (_fgDmxTestPCR && (u1Pidx == _arPat[_u1DmxTestPcrPidx].u1Pidx))
    {
        ASSERT(0);
    }

    prPsi = (DMX_NOTIFY_INFO_PSI_T*)u4Data;

    if (!_afgCompareResult[u1Pidx])
    {
        if (prPsi->fgFVR)
        {
            VERIFY(_FVR_PSI_UnlockBuffer(u1Pidx, prPsi->u1SerialNumber, prPsi->u4SecLen, prPsi->u4SecAddr));
        }
        else
        {
            VERIFY(_DMX_PSI_UnlockBuffer(u1Pidx, prPsi->u1SerialNumber, prPsi->u4SecLen, prPsi->u4SecAddr));
        }
        return TRUE;
    }

    pu1Golden = (UINT8*)(_arPat[u1Pidx].u4GoldenAddr + _au4GoldenOffset[u1Pidx]);

    u4BufEnd = _arPat[u1Pidx].u4BufAddr + _arPat[u1Pidx].u4BufSize;
    u4Len = MIN(prPsi->u4SecLen, (u4BufEnd - prPsi->u4SecAddr));

    pu1Test = (UINT8*)(prPsi->u4SecAddr);
    for (i=0; i<u4Len; i++)
    {
        if (pu1Test[i] != pu1Golden[i])
        {
            Printf("Compare error, %d, %x != %x\n", u1Pidx, pu1Test[i], pu1Golden[i]);
            _afgCompareResult[u1Pidx] = FALSE;
            return TRUE;
        }
    }

    u4Len = prPsi->u4SecLen - u4Len;
    if (u4Len > 0)
    {
        pu1Test = (UINT8*)(_arPat[u1Pidx].u4BufAddr);
        for (j=0; j<u4Len; j++)
        {
            if (pu1Test[j] != pu1Golden[i + j])
            {
                Printf("Compare error, %d, %x != %x\n", u1Pidx, pu1Test[i], pu1Golden[i +j]);
                _afgCompareResult[u1Pidx] = FALSE;
                return TRUE;
            }
        }
    }

    _au4GoldenOffset[u1Pidx] += prPsi->u4SecLen;

    if (prPsi->fgFVR)
    {
        VERIFY(_FVR_PSI_UnlockBuffer(u1Pidx, prPsi->u1SerialNumber, prPsi->u4SecLen, prPsi->u4SecAddr));
    }
    else
    {
        VERIFY(_DMX_PSI_UnlockBuffer(u1Pidx, prPsi->u1SerialNumber, prPsi->u4SecLen, prPsi->u4SecAddr));
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetTestPid(UINT8 u1TsIdx, UINT8 u1PatIdx)
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
    rPid.u2Pid = _arPat[u1PatIdx].u2PidNum;
    rPid.u4BufAddr = _arPat[u1PatIdx].u4BufAddr;
    rPid.u4BufSize = _arPat[u1PatIdx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PSI;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = _DmxEmuPSINotify;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    if (!_DMX_SetPid(_arPat[u1PatIdx].u1Pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return  TRUE;
}


//-----------------------------------------------------------------------------
/** _ClearTestFilter
 */
//-----------------------------------------------------------------------------
static BOOL _ClearTestFilter(UINT8 u1TsIdx)
{
    DMX_FILTER_T rFilter;
    UINT32 i;

    x_memset(&rFilter, 0, sizeof(rFilter));

    rFilter.fgEnable = FALSE;
    rFilter.u1Tsidx = u1TsIdx;

    for (i=0; i<DMX_NUM_FILTER_INDEX; i++)
    {
        if (!DMX_SetFilter(i, 0, DMX_FILTER_FLAG_VALID, &rFilter))
        {
            return FALSE;
        }
        if (!DMX_SetFilter(i, 1, DMX_FILTER_FLAG_VALID, &rFilter))
        {
            return FALSE;
        }
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _SetTestFilter
 */
//-----------------------------------------------------------------------------
static BOOL _SetTestFilter(UINT8 u1TsIdx,UINT8 u1PatIdx,UINT32 u4Offset)
{
    DMX_FILTER_T rFilter;

    x_memset(&rFilter, 0, sizeof(rFilter));

    rFilter.fgEnable = TRUE;

    rFilter.u1Tsidx = u1TsIdx;
    rFilter.u1Pidx = _arPat[u1PatIdx].u1Pidx;
    rFilter.u1Offset = _arPat[u1PatIdx].u1Offset;
    rFilter.eMode = _arPat[u1PatIdx].eMode;
    rFilter.fgCheckCrc = TRUE;

    rFilter.au4Data[0] = (_arPat[u1PatIdx].au1Pat[0] << 24 ) |
                         (_arPat[u1PatIdx].au1Pat[1] << 16)  |
                         (_arPat[u1PatIdx].au1Pat[2] << 8)   |
                         _arPat[u1PatIdx].au1Pat[3];

    rFilter.au4Data[1] = (_arPat[u1PatIdx].au1Pat[4] << 24 ) |
                         (_arPat[u1PatIdx].au1Pat[5] << 16)  |
                         (_arPat[u1PatIdx].au1Pat[6] << 8)   |
                         _arPat[u1PatIdx].au1Pat[7];

    rFilter.au4Mask[0] = (_arPat[u1PatIdx].au1Mask[0] << 24 ) |
                         (_arPat[u1PatIdx].au1Mask[1] << 16)  |
                         (_arPat[u1PatIdx].au1Mask[2] << 8)   |
                         _arPat[u1PatIdx].au1Mask[3];

    rFilter.au4Mask[1] = (_arPat[u1PatIdx].au1Mask[4] << 24 ) |
                         (_arPat[u1PatIdx].au1Mask[5] << 16)  |
                         (_arPat[u1PatIdx].au1Mask[6] << 8)   |
                         _arPat[u1PatIdx].au1Mask[7];

    if (!DMX_SetFilter(_arPat[u1PatIdx].u1FilterIdx+u4Offset, 0, DMX_FILTER_FLAG_ALL, &rFilter))
    {
        return FALSE;
    }

    if ((rFilter.eMode == DMX_FILTER_MODE_LONG) ||
            (rFilter.eMode == DMX_FILTER_MODE_POS_NEG) ||
            (rFilter.eMode == DMX_FILTER_MODE_14_2) ||
            (rFilter.eMode == DMX_FILTER_MODE_GENERIC))
    {
        rFilter.au4Data[0] = (_arPat[u1PatIdx].au1Pat[8] << 24 ) |
                             (_arPat[u1PatIdx].au1Pat[9] << 16)  |
                             (_arPat[u1PatIdx].au1Pat[10] << 8)  |
                             _arPat[u1PatIdx].au1Pat[11];

        rFilter.au4Data[1] = (_arPat[u1PatIdx].au1Pat[12] << 24 ) |
                             (_arPat[u1PatIdx].au1Pat[13] << 16)  |
                             (_arPat[u1PatIdx].au1Pat[14] << 8)   |
                             _arPat[u1PatIdx].au1Pat[15];

        rFilter.au4Mask[0] = (_arPat[u1PatIdx].au1Mask[8] << 24 ) |
                             (_arPat[u1PatIdx].au1Mask[9] << 16)  |
                             (_arPat[u1PatIdx].au1Mask[10] << 8)  |
                             _arPat[u1PatIdx].au1Mask[11];

        rFilter.au4Mask[1] = (_arPat[u1PatIdx].au1Mask[12] << 24 ) |
                             (_arPat[u1PatIdx].au1Mask[13] << 16)  |
                             (_arPat[u1PatIdx].au1Mask[14] << 8)   |
                             _arPat[u1PatIdx].au1Mask[15];
		
	    if (!DMX_SetFilter((_arPat[u1PatIdx].u1FilterIdx+u4Offset), 1, DMX_FILTER_FLAG_ALL, &rFilter))
        {
            return FALSE;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetTestFilter
 */
//-----------------------------------------------------------------------------
static BOOL _PVRSetTestFilter(UINT8 u1TsIdx,UINT8 u1PatIdx)
{
    DMX_FILTER_T rFilter;

    x_memset(&rFilter, 0, sizeof(rFilter));

    rFilter.fgEnable = TRUE;

    rFilter.u1Tsidx = u1TsIdx;
    rFilter.u1Pidx = _arPat[u1PatIdx].u1Pidx;
    rFilter.u1Offset = _arPat[u1PatIdx].u1Offset;
    rFilter.eMode = _arPat[u1PatIdx].eMode;
    rFilter.fgCheckCrc = TRUE;
    rFilter.fgIsFvr = TRUE;

    rFilter.au4Data[0] = (_arPat[u1PatIdx].au1Pat[0] << 24 ) |
                         (_arPat[u1PatIdx].au1Pat[1] << 16)  |
                         (_arPat[u1PatIdx].au1Pat[2] << 8)   |
                         _arPat[u1PatIdx].au1Pat[3];

    rFilter.au4Data[1] = (_arPat[u1PatIdx].au1Pat[4] << 24 ) |
                         (_arPat[u1PatIdx].au1Pat[5] << 16)  |
                         (_arPat[u1PatIdx].au1Pat[6] << 8)   |
                         _arPat[u1PatIdx].au1Pat[7];

    rFilter.au4Mask[0] = (_arPat[u1PatIdx].au1Mask[0] << 24 ) |
                         (_arPat[u1PatIdx].au1Mask[1] << 16)  |
                         (_arPat[u1PatIdx].au1Mask[2] << 8)   |
                         _arPat[u1PatIdx].au1Mask[3];

    rFilter.au4Mask[1] = (_arPat[u1PatIdx].au1Mask[4] << 24 ) |
                         (_arPat[u1PatIdx].au1Mask[5] << 16)  |
                         (_arPat[u1PatIdx].au1Mask[6] << 8)   |
                         _arPat[u1PatIdx].au1Mask[7];

    if (!DMX_SetFilter(_arPat[u1PatIdx].u1FilterIdx, 0, DMX_FILTER_FLAG_ALL, &rFilter))
    {
        return FALSE;
    }

    if ((rFilter.eMode == DMX_FILTER_MODE_LONG) ||
            (rFilter.eMode == DMX_FILTER_MODE_POS_NEG) ||
            (rFilter.eMode == DMX_FILTER_MODE_14_2) ||
            (rFilter.eMode == DMX_FILTER_MODE_GENERIC))
    {
        rFilter.au4Data[0] = (_arPat[u1PatIdx].au1Pat[8] << 24 ) |
                             (_arPat[u1PatIdx].au1Pat[9] << 16)  |
                             (_arPat[u1PatIdx].au1Pat[10] << 8)  |
                             _arPat[u1PatIdx].au1Pat[11];

        rFilter.au4Data[1] = (_arPat[u1PatIdx].au1Pat[12] << 24 ) |
                             (_arPat[u1PatIdx].au1Pat[13] << 16)  |
                             (_arPat[u1PatIdx].au1Pat[14] << 8)   |
                             _arPat[u1PatIdx].au1Pat[15];

        rFilter.au4Mask[0] = (_arPat[u1PatIdx].au1Mask[8] << 24 ) |
                             (_arPat[u1PatIdx].au1Mask[9] << 16)  |
                             (_arPat[u1PatIdx].au1Mask[10] << 8)  |
                             _arPat[u1PatIdx].au1Mask[11];

        rFilter.au4Mask[1] = (_arPat[u1PatIdx].au1Mask[12] << 24 ) |
                             (_arPat[u1PatIdx].au1Mask[13] << 16)  |
                             (_arPat[u1PatIdx].au1Mask[14] << 8)   |
                             _arPat[u1PatIdx].au1Mask[15];

        if (!DMX_SetFilter(_arPat[u1PatIdx].u1FilterIdx, 1, DMX_FILTER_FLAG_ALL, &rFilter))
        {
            return FALSE;
        }
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _ClearTestGenericFilter
 */
//-----------------------------------------------------------------------------
static BOOL _ClearTestGenericFilter(UINT8 u1TsIdx)
{
    DMX_FILTER_GENERIC_T rFilter;
    UINT32 i;

    x_memset(&rFilter, 0, sizeof(rFilter));
    rFilter.fgEnable = FALSE;
    rFilter.u1Tsidx = u1TsIdx;

    for (i=0; i<DMX_NUM_FILTER_INDEX; i++)
    {
        if (!DMX_SetGenFilter(i, DMX_FILTER_FLAG_VALID, &rFilter))
        {
            return FALSE;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetTestGenericFilter
 */
//-----------------------------------------------------------------------------
static BOOL _SetTestGenericFilter(UINT8 u1TsIdx, UINT8 u1PatIdx,UINT32 u4Offset)
{
    DMX_FILTER_GENERIC_T rFilter;
    UINT32 i;

    x_memset(&rFilter, 0, sizeof(rFilter));
    rFilter.fgEnable = TRUE;

    rFilter.u1Pidx = _arPat[u1PatIdx].u1Pidx;
    rFilter.u1Offset = _arPat[u1PatIdx].u1Offset;
    rFilter.fgCheckCrc = TRUE;

    for (i=0; i<16; i++)
    {
        rFilter.au1Data[i] = _arPat[u1PatIdx].au1Pat[i];
        rFilter.au1Mask[i] = _arPat[u1PatIdx].au1Mask[i];
        rFilter.au1PosNeg[i] = _arPat[u1PatIdx].au1PosNeg[i];
    }

    rFilter.eMode = DMX_GENFILTER_8_BYTE;
    rFilter.u1Tsidx = u1TsIdx;
    /*for general ,use the (48,96+48)*/
    if (!DMX_SetGenFilter((_arPat[u1PatIdx].u1FilterIdx+u4Offset), DMX_FILTER_FLAG_ALL, &rFilter))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetVCTPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetVCTPid(UINT8 u1TsIdx, UINT8 u1Pidx, UINT16 u2Pid)
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
    rPid.u2Pid = u2Pid;
    rPid.eInputType = DMX_IN_BROADCAST_TS;
    rPid.u4BufAddr = TS_RESULT_ADDR;
    rPid.u4BufSize = TS_RESULT_PERPID_SIZE;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PSI;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = _DmxEmuPSINotify;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    if (!_DMX_SetPid(u1Pidx, DMX_PID_FLAG_ALL, &rPid, FALSE))
    {
        return FALSE;
    }

    return  TRUE;
}


//-----------------------------------------------------------------------------
/** _SetTestGenericFilter
 */
//-----------------------------------------------------------------------------
static BOOL _SetVCTFilter(UINT8 u1TsIdx, UINT8 u1PidIdx,UINT32 u4FilterIdx)
{
    DMX_FILTER_GENERIC_T rFilter;

    x_memset(&rFilter, 0, sizeof(rFilter));
    rFilter.fgEnable = TRUE;

    rFilter.u1Pidx = u1PidIdx;
    rFilter.u1Offset = 1;
    rFilter.fgCheckCrc = TRUE;

    rFilter.au1Data[0] = 0xC8;
    rFilter.au1Data[1] = 0xC0;
    rFilter.au1Mask[0] = 0xFF;
    rFilter.au1Mask[1] = 0xC0;

    rFilter.eMode = DMX_GENFILTER_8_BYTE;
    rFilter.u1Tsidx = u1TsIdx;
    /*for general ,use the (48,96+48)*/
    if (!DMX_SetGenFilter(u4FilterIdx, DMX_FILTER_FLAG_ALL, &rFilter))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetTestGenericFilter
 */
//-----------------------------------------------------------------------------
static BOOL _PVRSetTestGenericFilter(UINT8 u1TsIdx, UINT8 u1PatIdx)
{
    DMX_FILTER_GENERIC_T rFilter;
    UINT32 i;

    x_memset(&rFilter, 0, sizeof(rFilter));
    rFilter.fgEnable = TRUE;
    rFilter.fgIsFvr = TRUE;
    rFilter.u1Pidx = _arPat[u1PatIdx].u1Pidx;
    rFilter.u1Offset = _arPat[u1PatIdx].u1Offset;
    rFilter.fgCheckCrc = TRUE;

    for (i=0; i<16; i++)
    {
        rFilter.au1Data[i] = _arPat[u1PatIdx].au1Pat[i];
        rFilter.au1Mask[i] = _arPat[u1PatIdx].au1Mask[i];
        rFilter.au1PosNeg[i] = _arPat[u1PatIdx].au1PosNeg[i];
    }

    rFilter.eMode = DMX_GENFILTER_8_BYTE;
    rFilter.u1Tsidx = u1TsIdx;

#if defined(CC_MT5890)
    if (!DMX_SetGenFilter(_arPat[u1PatIdx].u1FilterIdx, DMX_FILTER_FLAG_ALL, &rFilter))
#else  //MT5882
    if (!DMX_SetGenFilter((_arPat[u1PatIdx].u1FilterIdx+48), DMX_FILTER_FLAG_ALL, &rFilter))
#endif
    {
        return FALSE;
    }

    return TRUE;
}



//-----------------------------------------------------------------------------
/** _FreeTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _FreeTestPid(UINT8 u1PatIdx)
{
    DMX_PID_T rPid;

    // Setup PID
    rPid.fgEnable = FALSE;

    if (!_DMX_SetPid(_arPat[u1PatIdx].u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE))
    {
        return FALSE;
    }

    _DMX_FreePid(_arPat[u1PatIdx].u1Pidx);

    return  TRUE;
}


//-----------------------------------------------------------------------------
/** _SetPVRGBL
 */
//-----------------------------------------------------------------------------
static BOOL _SetPVRGBL(UINT8 u1TsIndex)
{
    FVR_GBL_T rGblPid;

    x_memset((VOID*)&rGblPid, 0x0, sizeof(FVR_GBL_T));
    rGblPid.fgAllocateBuffer = TRUE;
    rGblPid.u2TargetPktCount = 0x2000;
    rGblPid.u4BufSize = 0x100000;

    if (!_FVR_SetGBL(u1TsIndex, FVR_GBL_FLAG_ALL, &rGblPid))
    {
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _SetPVRTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetPVRTestSectionPid(UINT8 u1TsIndex, UINT8 u1PatIdx)
{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIndex;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = FALSE;
    rFVRPid.fgPicSearch = FALSE;
    rFVRPid.u1PicSearchMask = 0x0;
    rFVRPid.pfnPicSearchNotify = NULL;
    rFVRPid.u2Pid = _arPat[u1PatIdx].u2PidNum;

    rFVRPid.fgDesc = TRUE;
    rFVRPid.u1DescKeyidx = 11;

    //section filter
    rFVRPid.fgAllocateBuffer = FALSE;
    rFVRPid.fgDoSFilter = TRUE;
    rFVRPid.u4BufAddr = _arPat[u1PatIdx].u4BufAddr;
    rFVRPid.u4BufSize = _arPat[u1PatIdx].u4BufSize;
    rFVRPid.ePidType = DMX_PID_TYPE_PSI;
    rFVRPid.pfnNotify = _DmxEmuPSINotify;
    rFVRPid.pvNotifyTag = NULL;

    if (!FVR_SetPid(u1PatIdx, FVR_PID_FLAG_ALL & ~FVR_PID_FLAG_KEY_LEN, &rFVRPid))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetPVRTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetPVRTestPid(UINT8 u1TsIndex, UINT8 u1PatIdx)
{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIndex;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = FALSE;
    rFVRPid.fgPicSearch = FALSE;
    rFVRPid.u1PicSearchMask = 0x0;
    rFVRPid.pfnPicSearchNotify = NULL;
    rFVRPid.u2Pid = _arPat[u1PatIdx].u2PidNum;

    rFVRPid.fgDesc = TRUE;
    rFVRPid.u1DescKeyidx = 11;

    if (!FVR_SetPid(u1PatIdx, FVR_PID_FLAG_ALL & ~FVR_PID_FLAG_KEY_LEN, &rFVRPid))
    {
        return FALSE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _SetPVRTestPid
 */
//-----------------------------------------------------------------------------
static BOOL _SetPVRTestPidEx(UINT8 u1TsIndex, UINT8 u1PatIdx, BOOL fgDecrypt, UINT8 u1KeyIdx)
{
    FVR_PID_T rFVRPid;

    x_memset((void*)&rFVRPid, 0, sizeof(rFVRPid));
    rFVRPid.u1TsIndex = u1TsIndex;
    rFVRPid.fgEnable = TRUE;
    rFVRPid.eKeyType= FVR_KEY_EVEN;
    rFVRPid.fgPktSwap = FALSE;
    rFVRPid.pfnSwapNotify = NULL;
    rFVRPid.fgScramble = FALSE;
    rFVRPid.fgPicSearch = FALSE;
    rFVRPid.u1PicSearchMask = 0x0;
    rFVRPid.pfnPicSearchNotify = NULL;
    rFVRPid.u2Pid = _arPat[u1PatIdx].u2PidNum;

    rFVRPid.fgDesc = fgDecrypt;
    rFVRPid.u1DescKeyidx = u1KeyIdx;

    if (!FVR_SetPid(u1PatIdx, FVR_PID_FLAG_ALL & ~FVR_PID_FLAG_KEY_LEN, &rFVRPid))
    {
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _DMX_EMU_PSI_Test
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_PSI_Test(UINT8 u1TsIdx, BOOL fgGenPat, DMX_FILTER_MODE_T eMode, BOOL fgBypassFramer,UINT32 u4_offset)
{
    UINT32 i, u4PidCount;
    UINT8 *pu1Golden;
    BOOL fgRet = TRUE;
    UINT32 u4DstStart,u4DstEnd;
   //just has 96 playback pid ,but there's 192 sectin filter ,so set the max num=96.
    #if 1
    _u1DmxEmuTestPidNum = DMX_NUM_PID_INDEX;
    u4PidCount = DMX_NUM_PID_INDEX;
	#else 
	_u1DmxEmuTestPidNum = 1;
    u4PidCount = 1;
	#endif

    _u4TSPktAddr = TS_PACKET_ADDR;

    if (eMode == DMX_FILTER_MODE_GENERIC)
    {
        VERIFY(_ClearTestGenericFilter(u1TsIdx));
    }
    else
    {
        VERIFY(_ClearTestFilter(u1TsIdx));
    }

    if (fgGenPat)
    {
        _u4TSPktOffset = TS_PACKET_ADDR;
        _GenFilterPattern(eMode);

        x_memset((VOID*)TS_GOLDEN_ADDR, 0, TS_RESULT_PERPID_SIZE * u4PidCount);

        _CreateTSStream(TS_SECTION_ADDR, TS_SECTION_SIZE);//TS_SECTION_SIZE,just use 8k to test it .
    }

    for (i=0; i<u4PidCount; i++)
    {
        pu1Golden = (UINT8*)(_arPat[i].u4GoldenAddr);
        if (_arPat[i].u4SecSize > 0)
        {
            if (pu1Golden[0] != i)
            {
                Printf("Error, idx: %d, addr: 0x%x, 0x%x\n", i,
                       (UINT32)pu1Golden , pu1Golden[0]);
                return FALSE;
            }
        }
    }

    for (i=0; i<u4PidCount; i++)
    {
        _SetTestPid(u1TsIdx, i);
        if (eMode == DMX_FILTER_MODE_GENERIC)
        {
            _SetTestGenericFilter(u1TsIdx, i,u4_offset);
        }
        else
        {
            _SetTestFilter(u1TsIdx, i,u4_offset);
        }
    }

    // Set to bypass mode
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

    for (i=0; i<u4PidCount; i++)
    {
        _au4GoldenOffset[i] = 0x0;
        _afgCompareResult[i] = TRUE;
    }

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktOffset - _u4TSPktAddr))
    {
        return FALSE;
    }

    //----------------------------------------
    // Compare result
    //----------------------------------------
    for (i=0; i<u4PidCount; i++)
    {
        if (_arPat[i].u4SecSize != _au4GoldenOffset[i])
        {
            Printf("Size not match: idx:%d, org:%d,new:%d\n", i,
                   _arPat[i].u4SecSize, _au4GoldenOffset[i]);
            fgRet = FALSE;
            //break;
        }
    }

    // Free PID
    for (i=0; i<u4PidCount; i++)
    {
        _FreeTestPid(i);
    }

    for (i=0; i<u4PidCount; i++)
    {
        if (!_afgCompareResult[i])
        {
            return FALSE;
        }
    }

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_PSI_Rec_Test
 * Record only
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_PSI_Rec_Test(UINT8 u1TsIdx, BOOL fgGenPat, DMX_FILTER_MODE_T eMode, BOOL fgBypassFramer)
{
    UINT32 i;
    UINT8 *pu1Test, *pu1Golden;
    UINT32 u4DataSize;
    BOOL fgRet = TRUE;
    FVR_GBL_T rGBL;
    UINT32 u4PidCount;

    // Init
    // Although palyback have 64 PID filter ,but section filter only has 48
    // HW section filter , so , we test the max pid number is DMX_NUM_FILTER_INDEX
    _u1DmxEmuTestPidNum = FVR_NUM_PID_INDEX;
    u4PidCount = FVR_NUM_PID_INDEX;

    _u4TSPktAddr = TS_PACKET_ADDR;

    if (eMode == DMX_FILTER_MODE_GENERIC)
    {
        VERIFY(_ClearTestGenericFilter(u1TsIdx));
    }
    else
    {
        VERIFY(_ClearTestFilter(u1TsIdx));
    }

    if (fgGenPat)
    {
        _u4TSPktOffset = TS_PACKET_ADDR;
        _GenFilterPattern(eMode);

        x_memset((VOID*)TS_GOLDEN_ADDR, 0, TS_RESULT_PERPID_SIZE * u4PidCount);

        _CreateTSStream(TS_SECTION_ADDR, TS_SECTION_SIZE);
    }

    for (i=0; i<u4PidCount; i++)
    {
        pu1Golden = (UINT8*)(_arPat[i].u4GoldenAddr);
        if (_arPat[i].u4SecSize > 0)
        {
            if (pu1Golden[0] != i)
            {
                Printf("Error, idx: %d, addr: 0x%x, 0x%x\n", i,
                       (UINT32)pu1Golden , pu1Golden[0]);
                return FALSE;
            }
        }
    }

    VERIFY(FVR_Init());

    _SetPVRGBL(u1TsIdx);

    for (i=0; i<u4PidCount; i++)
    {
#ifdef PVR_SECTION_FILTER
        VERIFY(_SetPVRTestSectionPid(u1TsIdx, i));
        if (eMode == DMX_FILTER_MODE_GENERIC)
        {
            _PVRSetTestGenericFilter(u1TsIdx, i);
        }
        else
        {
            _PVRSetTestFilter(u1TsIdx, i);
        }
#else
        VERIFY(_SetPVRTestPid(u1TsIdx, i));
#endif
    }

    VERIFY(_FVR_Start(u1TsIdx));

    // Set to bypass mode
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

    // DDI
    if (!_DMX_EMU_DDI_Init(u1TsIdx))
    {
        return FALSE;
    }

#ifdef PVR_SECTION_FILTER
    for (i=0; i<u4PidCount; i++)
    {
        _au4GoldenOffset[i] = 0x0;
        _afgCompareResult[i] = TRUE;
    }
#endif

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktOffset - _u4TSPktAddr))
    {
        return FALSE;
    }

#ifdef PVR_SECTION_FILTER
    //----------------------------------------
    // Compare result
    //----------------------------------------
    for (i=0; i<u4PidCount; i++)
    {
        if (_arPat[i].u4SecSize != _au4GoldenOffset[i])
        {
            Printf("Size not match: idx:%d, org:%d,new:%d\n", i,
                   _arPat[i].u4SecSize, _au4GoldenOffset[i]);
            fgRet = FALSE;
            //break;
        }
    }
#endif

    // Compare record result
    if (!_FVR_GetGBL(u1TsIdx, FVR_GBL_FLAG_BUFFER, &rGBL))
    {
        Printf("Get record buffer error\n");
        return FALSE;
    }

    pu1Test = (UINT8*)rGBL.u4BufStart;
    pu1Golden = (UINT8*)TS_PACKET_ADDR;
    u4DataSize = DATASIZE(rGBL.u4Rp, rGBL.u4Wp, rGBL.u4BufSize);
    if (u4DataSize == 0)
    {
        Printf("Record don't have any data\n");
        fgRet = FALSE;
    }

    for (i=0; i<u4DataSize; i++)
    {
        if (pu1Test[i] != pu1Golden[i])
        {
            Printf("Record comapre error, idx:0x%x, 0x%x, 0x%x\n",
                   i, pu1Test[i], pu1Golden[i]);
            fgRet = FALSE;
            break;
        }
    }

    VERIFY(_FVR_Stop(u1TsIdx));

    // Free PID
    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(FVR_FreePid(i));
    }

    // Free GBL
    if (!_FVR_FreeGBL(u1TsIdx))
    {
        Printf("Free FVR GBL fail\n");
        fgRet = FALSE;
    }

#ifdef PVR_SECTION_FILTER
    for (i=0; i<u4PidCount; i++)
    {
        if (!_afgCompareResult[i])
        {
            Printf("_afgCompareResult fail\n");
            return FALSE;
        }
    }
#endif

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_PSI_PlayRec_Test
 *  Both play and record
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_PSI_PlayRec_Test(UINT8 u1TsIdex, BOOL fgGenPat, DMX_FILTER_MODE_T eMode, BOOL fgBypassFramer)
{
    UINT32 i;
    UINT8 *pu1Test, *pu1Golden;
    UINT32 u4DataSize;
    BOOL fgRet = TRUE;
    FVR_GBL_T rGBL;
    UINT32 u4PidCount;
    UINT32 u4Offset =0;
    // Init
    // Although palyback have 64 PID filter ,but section filter only has 48
    // HW section filter , so , we test the max pid number is DMX_NUM_FILTER_INDEX
    _u1DmxEmuTestPidNum = DMX_NUM_PID_INDEX;
    u4PidCount = DMX_NUM_PID_INDEX;

    _u4TSPktAddr = TS_PACKET_ADDR;

    if (eMode == DMX_FILTER_MODE_GENERIC)
    {
        VERIFY(_ClearTestGenericFilter(u1TsIdex));
    }
    else
    {
        VERIFY(_ClearTestFilter(u1TsIdex));
    }

    if (fgGenPat)
    {
        _u4TSPktOffset = TS_PACKET_ADDR;
        _GenFilterPattern(eMode);

        x_memset((VOID*)TS_GOLDEN_ADDR, 0, TS_RESULT_PERPID_SIZE * u4PidCount);

        _CreateTSStream(TS_SECTION_ADDR, TS_SECTION_SIZE);
    }

    for (i=0; i<u4PidCount; i++)
    {
        pu1Golden = (UINT8*)(_arPat[i].u4GoldenAddr);
        if (_arPat[i].u4SecSize > 0)
        {
            if (pu1Golden[0] != i)
            {
                Printf("Error, idx: %d, addr: 0x%x, 0x%x\n", i,
                       (UINT32)pu1Golden , pu1Golden[0]);
                return FALSE;
            }
        }
    }

    for (i=0; i<u4PidCount; i++)
    {
        _SetTestPid(u1TsIdex, i);
        if (eMode == DMX_FILTER_MODE_GENERIC)
        {
            _SetTestGenericFilter(u1TsIdex, i,u4Offset);
        }
        else
        {
            _SetTestFilter(u1TsIdex ,i,0);
        }
    }

    VERIFY(FVR_Init());

    _SetPVRGBL(u1TsIdex);

    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(_SetPVRTestPid(u1TsIdex, i));
    }

    VERIFY(_FVR_Start(u1TsIdex));

    // Set to bypass mode
    //Set DBM input source, only ts index 2 and 3 can config DBM input source for MT5890 and MT5861
    //only ts index 3 can config DBM source for MT5882
    if (u1TsIdex == 2)
    {
#if defined(CC_MT5890)
        if (!fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIdex, DMX_DBM_INPUT_FRAMER);
        }
        else
#endif
        {
            DMX_SetDbm_InputSource(u1TsIdex, DMX_DBM_INPUT_DDI);
        }
    }

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if (u1TsIdex == 3)
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIdex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIdex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // DDI
    if (!_DMX_EMU_DDI_Init(u1TsIdex))
    {
        return FALSE;
    }

    for (i=0; i<u4PidCount; i++)
    {
        _au4GoldenOffset[i] = 0x0;
        _afgCompareResult[i] = TRUE;
    }

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktOffset - _u4TSPktAddr))
    {
        return FALSE;
    }

    for (i=0; i<u4PidCount; i++)
    {
        if (_arPat[i].u4SecSize != _au4GoldenOffset[i])
        {
            Printf("%s,%d, Section size not match: %d\n", __FUNCTION__, __LINE__, i);
            fgRet = FALSE;
            break;
        }
    }

    // Compare record result
    if (!_FVR_GetGBL(u1TsIdex, FVR_GBL_FLAG_BUFFER, &rGBL))
    {
        Printf("Get record buffer error\n");
        return FALSE;
    }
    pu1Test = (UINT8*)rGBL.u4BufStart;
    pu1Golden = (UINT8*)TS_PACKET_ADDR;
    u4DataSize = DATASIZE(rGBL.u4Rp, rGBL.u4Wp, rGBL.u4BufSize);
    if (u4DataSize == 0)
    {
        Printf("Record don't have any data\n");
        fgRet = FALSE;
    }

    for (i=0; i<u4DataSize; i++)
    {
        if (pu1Test[i] != pu1Golden[i])
        {
            Printf("Record comapre error, idx:0x%x, 0x%x, 0x%x\n",
                   i, pu1Test[i], pu1Golden[i]);
            fgRet = FALSE;
            break;
        }
    }

    VERIFY(_FVR_Stop(u1TsIdex));

    // Free PID
    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(FVR_FreePid(i));
    }

    // Free GBL
    if (!_FVR_FreeGBL(u1TsIdex))
    {
        Printf("Free FVR GBL fail\n");
        fgRet = FALSE;
    }

    // Free PID
    for (i=0; i<u4PidCount; i++)
    {
        _FreeTestPid(i);
    }

    for (i=0; i<u4PidCount; i++)
    {
        if (!_afgCompareResult[i])
        {
            return FALSE;
        }
    }

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_PSI_Query
 */
//-----------------------------------------------------------------------------
void _DMX_EMU_PSI_Query(void)
{
    UINT32 i, u4GoldenSize;

    Printf("TS pkt: 0x%08x ~ 0x%08x\n", _u4TSPktAddr, _u4TSPktOffset -1);

    for (i=0; i<_u1DmxEmuTestPidNum; i++)
    {
        u4GoldenSize = _arPat[i].u4SecSize;
        Printf("idx %d, golden size 0x%x\n", i, u4GoldenSize);
    }
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_PSICombo_Test
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_PSICombo_Test(UINT8 u1TsIdx, BOOL fgGenPat, DMX_FILTER_MODE_T eMode, UINT32 u4Hw, BOOL fgBypassFramer)
{
    UINT32 u4_offset =0;
    if (u4Hw == 0)
    {
        return _DMX_EMU_PSI_Test(u1TsIdx, fgGenPat, eMode, fgBypassFramer,u4_offset);
    }
    else if (u4Hw == 1)
    {
        return _DMX_EMU_PSI_Rec_Test(u1TsIdx, fgGenPat, eMode ,fgBypassFramer);
    }
    else if (u4Hw == 2)
    {
        return _DMX_EMU_PSI_PlayRec_Test(u1TsIdx, fgGenPat, eMode, fgBypassFramer);
    }
    else
    {
        return FALSE;
    }
}

BOOL _DMX_EMU_VCTTest(UINT8 u1TsIndex, UINT8 u1Pidx, UINT16 u2Pid)
{
    BOOL fgRet = FALSE;

    VERIFY(_ClearTestGenericFilter(u1TsIndex));
    _DMX_SetFrontEndEx(0, DMX_FE_INTERNAL);
    _SetVCTPid(u1TsIndex, u1Pidx, u2Pid);
    _SetVCTFilter(u1TsIndex, u1Pidx, 0);

    UINT8 u1VctState = PID_S_W(u1Pidx,2) & 0x000000FF;
    UINT32 u4Counter = 0;
    
    while(1)
    {
        u1VctState = PID_S_W(u1Pidx,2) & 0x000000FF;
        if(u1VctState != 0)
        {
            Printf("PID_S_W(u1Pidx,2)=0x%x\n", PID_S_W(u1Pidx,2));
            fgRet = TRUE;
            break;
        }
        u4Counter++;
        if(u4Counter > 50000)
        {
            fgRet = TRUE;
            Printf("u4Counter > 50000!\n");
            break;
        }
    }

    return fgRet;
}

//-----------------------------------------------------------------------------
static BOOL _FVR_LPSetEx(UINT8 u1TsIndex, UINT8 u1TsOutIdx)
{
    static BOOL _fgInit = FALSE;
    DMX_PVR_PLAY_T rPlay;
    FVR_GBL_T rGBL;
    FVR_TIMESTAMP_T rTimeStamp;

    if (!_fgInit)
    {
        if (x_thread_create(&_hDmxEmuLPThread, "DMXLP", 1024, 60, _DmxEmuLoopbackThread, 0, NULL) != OSR_OK)
        {
            LOG(1, "Fail to create DMX emu test thread!\n");
            return FALSE;
        }
        _fgInit = TRUE;
    }

    _fgDmxEmuLPTSIndex = u1TsIndex;
    _fgDmxEmuLPTSOutIndex = u1TsOutIdx;
    _fgDmxEmuLPEnable = FALSE;

    if ((_FVR_IsRecording(u1TsIndex)) || (_DMX_PVRPlay_GetState(u1TsOutIdx) == DMX_PVRPLAY_STATE_PLAYING))
    {
        LOG(5, "Please stop first\n");
        return FALSE;
    }

    //------------------------------------------------------
    // Set PVR play and record
    //------------------------------------------------------
    if (!_FVR_FreeBuffer(u1TsIndex))
    {
        return FALSE;
    }

    if (!_DMX_PVRPlay_FreeBuf(u1TsOutIdx))
    {
        return FALSE;
    }

    // Capture init
    x_memset((void*)&rGBL, 0, sizeof(rGBL));
    rGBL.fgAllocateBuffer = TRUE;
    rGBL.u2TargetPktCount = 0;
    rGBL.u4BufSize = 0x100000;
    rGBL.pfnNotifyFunc = NULL;
    rGBL.pvNotifyTag = NULL;
    rGBL.u4TimeToNotify = 0;//1000;

    if (!_FVR_SetGBL(u1TsIndex, (UINT32)(FVR_GBL_FLAG_TARGET_PKT | FVR_GBL_FLAG_BUFFER |
                                         FVR_GBL_FLAG_TIME_INFO | FVR_GBL_FLAG_NOTIFY), &rGBL))
    {
        return FALSE;
    }

    if (!_FVR_GetGBL(u1TsIndex, FVR_GBL_FLAG_BUFFER, &rGBL))
    {
        return FALSE;
    }

    // Play init
    rPlay.fgAllocBuf = FALSE;
    rPlay.u4BufStart = rGBL.u4BufStart;
    rPlay.u4BufSize = rGBL.u4BufSize;
    rPlay.u4ThresholdSize = 0x10000000;     // Not usable
    rPlay.pfnPVRNotify = NULL;

    if (!_DMX_PVRPlay_Set(u1TsOutIdx, (UINT32)(PVRPLAY_FLAGS_THRESHOLD |PVRPLAY_FLAGS_BUFFER |
                                   PVRPLAY_FLAGS_CALLBACK), &rPlay))
    {
        return FALSE;
    }

    // Enabe Record timestamp
    rTimeStamp.fgEnable = TRUE;
    rTimeStamp.u2ClkDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
    rTimeStamp.u4Mask = 0xFFFFFFFF;
    rTimeStamp.u4Counter= 0x0;
    _FVR_SetTimestamp(FVR_TIMESTAMP_FLAG_ALL, &rTimeStamp);

    // Enable PLAY timestamp
    rPlay.fgContainTimeStamp = TRUE;
    rPlay.fgIgnoreTimeStamp = FALSE;
    rPlay.u2TimeStampFreqDiv = DMX_PVRPLAY_TIMESTAMP_DIV_BASE;
    if (!_DMX_PVRPlay_Set(u1TsOutIdx, (UINT32)PVRPLAY_FLAGS_TIMESTAMP, &rPlay))
    {
        return FALSE;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
static BOOL _FVR_LPEnableEx(UINT8 u1TsIndex, BOOL fgEnable, UINT8 u1TsOutIdx)
{
    if (fgEnable)
    {
        if (!_FVR_Start(u1TsIndex))
        {
            return FALSE;
        }

        x_thread_delay(20);

        if (!_DMX_PVRPlay_Start(u1TsOutIdx))
        {
            return FALSE;
        }

    }
    else
    {
        if (!_FVR_Stop(u1TsIndex))
        {
            return FALSE;
        }

        if (!_DMX_PVRPlay_Stop(u1TsOutIdx, TRUE))
        {
            return FALSE;
        }
    }

    _fgDmxEmuLPEnable = fgEnable;

    return TRUE;
}


//-----------------------------------------------------------------------------
static BOOL _FVR_LPFreeEx(UINT8 u1TsIndex, UINT8 u1TsOutIdx)
{
    if (!_FVR_FreeGBL(u1TsIndex))
    {
        return FALSE;
    }

    if (!_DMX_PVRPlay_Free(u1TsOutIdx))
    {
        return FALSE;
    }
    return TRUE;
}


//-----------------------------------------------------------------------------
static BOOL _FVR_LPSetKey(void)
{
    UINT32 i, j;
    UINT32 au4Key[8], u4KeyLen;
    UINT32 au4IV[4], u4Key;
    BOOL fgCBC = FALSE;
    FVR_PID_T rFvrPid;
    DMX_PID_T rPid;
    UINT8 u1KeyIdx;
    BOOL afgKeyIdxUsed[8];
    UINT32 u4Ctrl;

    u4KeyLen = 128 + (random(3) * 64);//256; // should be random
    fgCBC = (BOOL)random(2);   // should be random

    // Generate key
    for (i=0; i<4; i++)
    {
        VERIFY(_DMX_SetAesKeyLen(i, u4KeyLen));
        afgKeyIdxUsed[i] = FALSE;

        for (j=0; j<8; j++)
        {
            au4Key[j] = (UINT32)rand();
        }
        if(!_DMX_SetAesKey(i, TRUE, au4Key))
        {
            return FALSE;
        }

        for (j=0; j<8; j++)
        {
            u4Key = au4Key[j];
            au4Key[j] = (GET_BYTE(u4Key, 0) << 24) | (GET_BYTE(u4Key, 1) << 16) |
                        (GET_BYTE(u4Key, 2) << 8) | (GET_BYTE(u4Key, 3));
        }
        if (!_FVR_SetAesKey(i, TRUE, (UINT8*)au4Key))
        {
            return FALSE;
        }

        for (j=0; j<8; j++)
        {
            au4Key[j] = (UINT32)rand();
        }
        if(!_DMX_SetAesKey(i, FALSE, au4Key))
        {
            return FALSE;
        }

        for (j=0; j<8; j++)
        {
            u4Key = au4Key[j];
            au4Key[j] = (GET_BYTE(u4Key, 0) << 24) | (GET_BYTE(u4Key, 1) << 16) |
                        (GET_BYTE(u4Key, 2) << 8) | (GET_BYTE(u4Key, 3));
        }
        if (!_FVR_SetAesKey(i, FALSE, (UINT8*)au4Key))
        {
            return FALSE;
        }
    }

    if (fgCBC)
    {
        for (i=0; i<4; i++)
        {
            au4IV[i] = (UINT32)rand();
        }

        for (i=0; i<4; i++)
        {
            _DMX_SetAesIV(i, au4IV, TRUE);
            _DMX_SetAesIV(i, au4IV, FALSE);

            VERIFY(_FVR_SetAesEvenOddIV(i, TRUE, au4IV));
            VERIFY(_FVR_SetAesEvenOddIV(i, FALSE, au4IV));
        }
    }

    // Set record key
    for (i=0; i<_u1DmxEmuTestPidNum; i++)
    {
        u1KeyIdx = random(4);
        afgKeyIdxUsed[u1KeyIdx] = TRUE;

        rFvrPid.u4KeyLen = u4KeyLen;
        rFvrPid.eKeyType = FVR_KEY_EVEN; //(random(2) == 1) ? FVR_KEY_EVEN : FVR_KEY_ODD;
        rFvrPid.fgCBCMode = fgCBC;
        rFvrPid.fgScramble = TRUE;
        //rFvrPid.fgScramble = FALSE;     // eddie_test
        rFvrPid.u1KeyIdx = u1KeyIdx;
        if (!_FVR_SetPid(i, FVR_PID_FLAG_CBC_MODE | FVR_PID_FLAG_KEY_TYPE | FVR_PID_FLAG_KEY_LEN |
                         FVR_PID_FLAG_KEY_INDEX | FVR_PID_FLAG_SCRAMBLE_ENABLE, &rFvrPid))
        {
            return FALSE;
        }

        rPid.u1KeyIndex = u1KeyIdx;
        if (!_DMX_SetPid(i, DMX_PID_FLAG_KEY_INDEX, &rPid, FALSE))
        {
            return FALSE;
        }

        //after set descmode none,dmx_setpid would't set descrambling scheme.
        //so we should set this after dmx_setpid
        _DMX_Lock();
        u4Ctrl = PID_INDEX_TABLE(i);
        u4Ctrl |= 0x400; //set ca descramble enable
        PID_INDEX_TABLE(i) = u4Ctrl;
        _DMX_Unlock();

        if(!_DMX_SetCA_Mode(u1KeyIdx, DMX_CA_MODE_AES, (fgCBC ? DMX_CA_FB_CBC : DMX_CA_FB_ECB), FALSE))
        {
            return FALSE;
        }
    }

    Printf("Key len:%d, CBC:%s, Idx:", u4KeyLen, fgCBC ? "Yes" : "No");
    for (i=0; i<8; i++)
    {
        if (afgKeyIdxUsed[i])
        {
            Printf("%d,",i);
        }
    }
    Printf("\n");

    return TRUE;
}

//-----------------------------------------------------------------------------
/** DMX_EMU_PSILoopback_Test
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_PSILoopback_Test(UINT8 u1TsIdex, BOOL fgGenPat, DMX_FILTER_MODE_T eMode,  UINT8 u1TsOutIdx)
{
    UINT32 i;
    BOOL fgRet = TRUE;
    UINT32 u4PidCount;

    // Although palyback have 64 PID filter ,but section filter only has 48
    // HW section filter , so , we test the max pid number is DMX_NUM_FILTER_INDEX
    _u1DmxEmuTestPidNum = DMX_NUM_PID_INDEX;
    u4PidCount = DMX_NUM_PID_INDEX;

    _u4TSPktAddr = TS_PACKET_ADDR;

    if (eMode == DMX_FILTER_MODE_GENERIC)
    {
        VERIFY(_ClearTestGenericFilter(u1TsIdex));
    }
    else
    {
        VERIFY(_ClearTestFilter(u1TsIdex));
    }

    for (i=0; i<u4PidCount; i++)
    {
        _au4GoldenOffset[i] = 0x0;
        _afgCompareResult[i] = TRUE;
    }

    //----------------------------------------
    // Generate pattern
    //----------------------------------------
    if (fgGenPat)
    {
        _u4TSPktOffset = TS_PACKET_ADDR;
        _GenFilterPattern(eMode);

        x_memset((VOID*)TS_GOLDEN_ADDR, 0, TS_RESULT_PERPID_SIZE * u4PidCount);

        _CreateTSStream(TS_SECTION_ADDR, TS_SECTION_SIZE);
    }

    //----------------------------------------
    // Set PID and filter
    //----------------------------------------
    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(_SetTestPid(2, i));
        if (eMode == DMX_FILTER_MODE_GENERIC)
        {
            _SetTestGenericFilter(2, i,0);
        }
        else
        {
            _SetTestFilter(2, i,0);
        }
    }

    //----------------------------------------
    // Set loopback and record PID
    //----------------------------------------
    VERIFY(FVR_Init());
    VERIFY(_FVR_LPSetEx(u1TsIdex,u1TsOutIdx));

    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(_SetPVRTestPidEx(u1TsIdex, i, FALSE, 0));
    }

    VERIFY(_FVR_LPSetKey());
    VERIFY(_FVR_LPEnableEx(u1TsIdex, TRUE, u1TsOutIdx));

    //Set Pvrplay port
    _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);

    //----------------------------------------
    // DDI
    //----------------------------------------
    if (!_DMX_EMU_DDI_Init(u1TsIdex))
    {
        return FALSE;
    }

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktOffset - _u4TSPktAddr))
    {
        return FALSE;
    }

    x_thread_delay(500);

    VERIFY(_FVR_LPEnableEx(u1TsIdex, FALSE, u1TsOutIdx));

    //----------------------------------------
    // Compare result
    //----------------------------------------
    for (i=0; i<u4PidCount; i++)
    {
        if (_arPat[i].u4SecSize != _au4GoldenOffset[i])
        {
            Printf("%s,%d, Section size not match: %d\n", __FUNCTION__, __LINE__, i);
            fgRet = FALSE;
            break;
        }
    }

    // Free play PID
    for (i=0; i<u4PidCount; i++)
    {
        _FreeTestPid(i);
    }

    // Free record PID
    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(FVR_FreePid(i));
    }

    VERIFY(_FVR_LPFreeEx(u1TsIdex, u1TsOutIdx));

    for (i=0; i<u4PidCount; i++)
    {
        if (!_afgCompareResult[i])
        {
            return FALSE;
        }
    }

    return fgRet;
}

static BOOL _DmxEmuSetPCRPid(UINT8 u1TsIdx)
{
    DMX_PID_T rPid;

    _FreeTestPid(_u1DmxTestPcrPidx);

    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = FALSE;
    rPid.u1TsIndex = u1TsIdx;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    //rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = _arPat[_u1DmxTestPcrSrcPidx].u2PidNum;//_arPat[_u1DmxTestPcrPidx].u2PidNum;
    rPid.u4BufAddr = _arPat[_u1DmxTestPcrPidx].u4BufAddr;
    rPid.u4BufSize = _arPat[_u1DmxTestPcrPidx].u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NEW;
    rPid.ePidType = DMX_PID_TYPE_NONE;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = _DmxEmuPSINotify;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;

    if (!_DMX_SetPid(_arPat[_u1DmxTestPcrPidx].u1Pidx, (DMX_PID_FLAG_ALL & ~DMX_PID_FLAG_STEER), &rPid, FALSE))
    {
        return FALSE;
    }

    return TRUE;
}


void _DMX_EMU_MulMatch_ISR(void)
{
    UINT32 u4Status1;
    UINT8 u1Pidx;

    u4Status1 = DMXCMD_READ32(DMX_REG_PCR_NONERR_STATUS_REG1);
    u1Pidx = (u4Status1 >> 16) & 0xFF;
    /*
        if(u1Pidx != _arPat[_u1DmxTestPcrPidx].u1Pidx)
        {
            ASSERT(0);
        }
    */
    LOG(3, "PCR int: %d\n", u1Pidx);

    _u4DmxTestPcrIntCount++;

    // Clear interrupt
    DMXCMD_WRITE32(DMX_REG_PCR_NONERR_STATUS_REG1, 1);
    DMXCMD_WRITE32(DMX_REG_PCR2_NONERR_STATUS_REG1, 1);
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_MulMatch_Test
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_MulMatch_Test(UINT8 u1TsIdx)
{
    BOOL fgGenPat = TRUE;
    DMX_FILTER_MODE_T eMode= DMX_FILTER_MODE_SHORT;
    UINT32 i, u4PidCount;
    UINT8 *pu1Golden;
    BOOL fgRet = TRUE;
    static UINT8 _u1PCRPidx = 0;
    UINT32 u4Ctrl;
    static BOOL _fgToggleBit3 = TRUE;

    // Init
    _u1DmxEmuTestPidNum = DMX_NUM_FILTER_INDEX;
    u4PidCount = DMX_NUM_PID_INDEX;

    _u4TSPktAddr = TS_PACKET_ADDR;


    if (eMode == DMX_FILTER_MODE_GENERIC)
    {
        VERIFY(_ClearTestGenericFilter(u1TsIdx));
    }
    else
    {
        VERIFY(_ClearTestFilter(u1TsIdx));
    }

    _u1DmxTestPcrSrcPidx = _u1PCRPidx;
    while (1)
    {
        _u1DmxTestPcrPidx = random(u4PidCount);
        if (_u1DmxTestPcrPidx != _u1PCRPidx)
        {
            break;
        }
    }
    _u1PCRPidx++;
    if (_u1PCRPidx >= u4PidCount)
    {
        _u1PCRPidx = 0;
    }
    _fgDmxTestGenPacket = FALSE;
    _fgDmxTestPCR = TRUE;
    _u4DmxTestPcrIntCount = 0;

    Printf("PSI idx:%d, PCR idx:%d\n", _u1DmxTestPcrSrcPidx, _u1DmxTestPcrPidx);

    if (fgGenPat)
    {
        _u4TSPktOffset = TS_PACKET_ADDR;
        _GenFilterPattern(eMode);

        x_memset((VOID*)TS_GOLDEN_ADDR, 0, TS_RESULT_PERPID_SIZE * u4PidCount);

        _CreateTSStream(TS_SECTION_ADDR, TS_SECTION_SIZE);
    }

    for (i=0; i<u4PidCount; i++)
    {
        pu1Golden = (UINT8*)(_arPat[i].u4GoldenAddr);
        if (_arPat[i].u4SecSize > 0)
        {
            if (pu1Golden[0] != i)
            {
                Printf("Error, idx: %d, addr: 0x%x, 0x%x\n", i,
                       (UINT32)pu1Golden , pu1Golden[0]);
                return FALSE;
            }
        }
    }

    for (i=0; i<u4PidCount; i++)
    {
        _SetTestPid(u1TsIdx, i);
        if (eMode == DMX_FILTER_MODE_GENERIC)
        {
            _SetTestGenericFilter(u1TsIdx ,i,0);
        }
        else
        {
            _SetTestFilter(u1TsIdx, i,0);
        }
    }

    // Set PCR PID for multiple match test
    if (!_DmxEmuSetPCRPid(u1TsIdx))
    {
        _fgDmxTestPCR = FALSE;
        return FALSE;
    }

    //-----------------------------------------
    _DMX_Lock();
    u4Ctrl = PID_INDEX_TABLE(_u1DmxTestPcrPidx);
    u4Ctrl &= ~0x8;
    u4Ctrl |= (_fgToggleBit3 ? 0x8 : 0);
    _fgToggleBit3 = !_fgToggleBit3;
    PID_INDEX_TABLE(_u1DmxTestPcrPidx) = u4Ctrl;
    _DMX_Unlock();
    //-----------------------------------------

    // DDI
    if (!_DMX_EMU_DDI_Init(u1TsIdx))
    {
        _fgDmxTestPCR = FALSE;
        return FALSE;
    }

    for (i=0; i<u4PidCount; i++)
    {
        _au4GoldenOffset[i] = 0x0;
        _afgCompareResult[i] = TRUE;
    }

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktOffset - _u4TSPktAddr))
    {
        _fgDmxTestPCR = FALSE;
        return FALSE;
    }

    x_thread_delay(100);

    if (_fgDmxTestGenPacket && (_u4DmxTestPcrIntCount != 1))
    {
        Printf("No PCR interrupt\n");
        fgRet = FALSE;
    }

    //----------------------------------------
    // Compare result
    //----------------------------------------
    for (i=0; i<u4PidCount; i++)
    {
        if (i == _u1DmxTestPcrPidx)
        {
            continue;
        }
        if (_arPat[i].u4SecSize != _au4GoldenOffset[i])
        {
            Printf("Size not match: idx:%d, org:%d,new:%d\n", i,
                   _arPat[i].u4SecSize, _au4GoldenOffset[i]);
            fgRet = FALSE;
            //break;
        }
    }

    // Free PID
    for (i=0; i<u4PidCount; i++)
    {
        _FreeTestPid(i);
    }

    for (i=0; i<u4PidCount; i++)
    {
        if (!_afgCompareResult[i])
        {
            _fgDmxTestPCR = FALSE;
            return FALSE;
        }
    }

    _fgDmxTestPCR = FALSE;

    return fgRet;
}


static BOOL _DmxPSIEncryptAesPacket(UINT8* pu1Buf, AES_PARAM_T* prParam)
{
    aes_context rCtx;
    UINT8* pu1Skip4 = pu1Buf + 4;
    UINT32 u4Offset, u4KeyLen;
    UINT8 au1Text[32], au1InitVector[16];
    UINT8 u1AdaptField = 0, u1PayloadOffset = 0;
    BOOL fgEven = TRUE;
    UINT8* au1Key = au1Text;

    ASSERT(prParam->eFeedbackMode == DMX_CA_FB_ECB);
    ASSERT(pu1Buf != NULL);
    ASSERT(prParam != NULL);
    ASSERT((prParam->u2KeyLen == 128) || (prParam->u2KeyLen == 192) ||
           (prParam->u2KeyLen == 256));

    u1AdaptField = (pu1Buf[3] >> 4) & 0x3;
    if (0 == (u1AdaptField & 0x1))  // 00 and 10 => skip
    {
        goto _exit;
    }
    else if (u1AdaptField == 0x3)   // adaptation + payload
    {
        u1PayloadOffset = 1 + pu1Buf[4];
    }
    else                            // 0x1 payload only
    {
        u1PayloadOffset = 0;
    }

    switch (prParam->eKeySelect)
    {
    case KEY_SELECT_EVEN:
        fgEven = TRUE;
        break;

    case KEY_SELECT_ODD:
        fgEven = FALSE;
        break;

    case KEY_SELECT_RANDOM:
        fgEven = ((rand() & 1) == 0) ? TRUE : FALSE;
        break;

    default:
        ASSERT(0);
        break;
    }

    u4KeyLen = prParam->u2KeyLen / 8;

    if (fgEven) // Even
    {
        x_memcpy(au1Key, prParam->arKey[0].au1Even, u4KeyLen);
        pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0x80;
    }
    else        // Odd
    {
        x_memcpy(au1Key, prParam->arKey[0].au1Odd, u4KeyLen);
        pu1Buf[3] = (pu1Buf[3] & 0x3f) | 0xc0;
    }

    // Setup key scheduling
    aes_setkey_enc(&rCtx, au1Key, prParam->u2KeyLen);

    if (prParam->eFeedbackMode == DMX_CA_FB_CBC)
    {
        x_memcpy(au1InitVector, prParam->au1InitVector, 16);
    }

    for (u4Offset = u1PayloadOffset; u4Offset <= (TS_PACKET_SIZE - 20);
            u4Offset += 16)
    {
        x_memcpy(au1Text, pu1Skip4 + u4Offset, 16);   // Get plain text

        // Encrypt
        if (prParam->eFeedbackMode == DMX_CA_FB_CBC)
        {
            aes_crypt_cbc(&rCtx, AES_ENCRYPT, 16, au1InitVector, au1Text, au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Text);
        }

        x_memcpy(pu1Skip4 + u4Offset, au1Text, 16);  // Copy back
    }
    /*
        if((prParam->eFeedbackMode == DMX_CA_FB_ECB) && ((184 - u4Offset) != 0))
        {
            UINT8 u1Cnt;
            UINT8 au1Texto[32];
            u1Cnt = 184 - u4Offset;
            x_memcpy(au1Text, pu1Skip4 + u4Offset, u1Cnt);   // Get plain text
            aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1InitVector, au1Text, au1Texto);
            x_memcpy(pu1Skip4 + u4Offset, au1Texto, u1Cnt);  // Copy back
        }
    */
_exit:

    return TRUE;
}


static BOOL _EncryptPSICAPackets(UINT32 u4Addr, UINT32 u4Size, AES_PARAM_T* prParam)
{
    ASSERT(_DMX_IsAligned(u4Size, 188));

    while (u4Size > 0)
    {
        if (!_DmxPSIEncryptAesPacket((UINT8*)u4Addr, prParam))
        {
            return FALSE;
        }

        u4Addr += 188;
        u4Size -= 188;
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** DMX_EMU_PSICALoopback_Test
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_PSICALoopback_Test(UINT8 u1TsIdex, BOOL fgGenPat, DMX_FILTER_MODE_T eMode, UINT8 u1TsOutIdx)
{
    UINT32 i;
    BOOL fgRet = TRUE;
    UINT32 u4PidCount;
    AES_PARAM_T rParam;
    UINT32 au4EvenKey[8], au4OddKey[8];
    UINT8* p;
    UINT8 u1KeyIdx = 7;

    // Although palyback have 64 PID filter ,but section filter only has 48
    // HW section filter , so , we test the max pid number is DMX_NUM_FILTER_INDEX
    _u1DmxEmuTestPidNum = DMX_NUM_PID_INDEX;
    u4PidCount = DMX_NUM_PID_INDEX;

    _u4TSPktAddr = TS_PACKET_ADDR;

    if (eMode == DMX_FILTER_MODE_GENERIC)
    {
        VERIFY(_ClearTestGenericFilter(u1TsIdex));
    }
    else
    {
        VERIFY(_ClearTestFilter(u1TsIdex));
    }

    rParam.eFeedbackMode = DMX_CA_FB_ECB;
    rParam.eRtb = DMX_DESC_RTB_MODE_CLEAR;
    rParam.u2KeyLen = 256;
    rParam.eKeySelect = KEY_SELECT_EVEN;
    for (i=0; i<32; i++)
    {
        rParam.arKey[0].au1Even[i] = random(256);
        rParam.arKey[0].au1Odd[i] = random(256);
    }

    for (i=0; i<u4PidCount; i++)
    {
        _au4GoldenOffset[i] = 0x0;
        _afgCompareResult[i] = TRUE;
    }

    //----------------------------------------
    // Generate pattern
    //----------------------------------------
    if (fgGenPat)
    {
        _u4TSPktOffset = TS_PACKET_ADDR;
        _GenFilterPattern(eMode);

        x_memset((VOID*)TS_GOLDEN_ADDR, 0, TS_RESULT_PERPID_SIZE * u4PidCount);

        _CreateTSStream(TS_SECTION_ADDR, TS_SECTION_SIZE);

        // Encrypt TS
        if (!_EncryptPSICAPackets(TS_PACKET_ADDR, _u4TSPktOffset - _u4TSPktAddr, &rParam))
        {
            return FALSE;
        }
    }

    //----------------------------------------
    // Set PID and filter
    //----------------------------------------
    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(_SetTestPid(2, i));
        if (eMode == DMX_FILTER_MODE_GENERIC)
        {
            _SetTestGenericFilter(2, i,0);
        }
        else
        {
            _SetTestFilter(2, i,0);
        }
    }

    //----------------------------------------
    // Set loopback and record PID
    //----------------------------------------
    VERIFY(FVR_Init());
    VERIFY(_FVR_LPSetEx(u1TsIdex, u1TsOutIdx));

    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(_SetPVRTestPidEx(u1TsIdex, i, TRUE,u1KeyIdx));
    }

    VERIFY(_FVR_LPSetKey());
    VERIFY(_FVR_LPEnableEx(u1TsIdex, TRUE, u1TsOutIdx));

    //Set Pvrplay port
    _DMX_PVRPlay_SetPort(u1TsOutIdx,DMX_PVRPLAY_PORT_FRAMER2_BYPASS);

    // Set CA key
    for (i = 0; i < 8; i++)
    {
        p = rParam.arKey[0].au1Even + (i * 4);
        au4EvenKey[i] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

        p = rParam.arKey[0].au1Odd + (i * 4);
        au4OddKey[i] = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    }

    // Set keys
    VERIFY(_DMX_SetAesKey(u1KeyIdx, TRUE, au4EvenKey));
    VERIFY(_DMX_SetAesKey(u1KeyIdx, FALSE, au4OddKey));
    VERIFY(_DMX_SetAesKeyLen(u1KeyIdx, rParam.u2KeyLen));
    VERIFY(_DMX_SetCA_Mode(u1KeyIdx, DMX_CA_MODE_AES, DMX_CA_FB_ECB, FALSE));
    _DMX_SetRtbMode(u1KeyIdx, rParam.eRtb);

    //----------------------------------------
    // DDI
    //----------------------------------------
    if (!_DMX_EMU_DDI_Init(u1TsIdex))
    {
        return FALSE;
    }

#if 0
    // N/M Mbits/sec
    DDI_WRITE32(DDI_REG_PERIOD_M, 16 * 188 * 8 * 27);
    DDI_WRITE32(DDI_REG_PERIOD_N, 1);
    DDI_WRITE32(DDI_REG_RATE_CMD, 0x2);     // Update M and N
#endif

    if (!_DMX_EMU_DDI_Transfer(0, TS_PACKET_ADDR, _u4TSPktOffset - _u4TSPktAddr))
    {
        return FALSE;
    }

    x_thread_delay(500);

    VERIFY(_FVR_LPEnableEx(u1TsIdex, FALSE, u1TsOutIdx));

    //----------------------------------------
    // Compare result
    //----------------------------------------
    for (i=0; i<u4PidCount; i++)
    {
        if (_arPat[i].u4SecSize != _au4GoldenOffset[i])
        {
            Printf("%s,%d, Section size not match: %d\n", __FUNCTION__, __LINE__, i);
            fgRet = FALSE;
            break;
        }
    }

    // Free play PID
    for (i=0; i<u4PidCount; i++)
    {
        _FreeTestPid(i);
    }

    // Free record PID
    for (i=0; i<u4PidCount; i++)
    {
        VERIFY(FVR_FreePid(i));
    }

    VERIFY(_FVR_LPFreeEx(u1TsIdex, u1TsOutIdx));

    for (i=0; i<u4PidCount; i++)
    {
        if (!_afgCompareResult[i])
        {
            return FALSE;
        }
    }

    return fgRet;
}

