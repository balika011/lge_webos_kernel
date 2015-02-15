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

#include "../gcpu.h"
#include "vcps_pattern.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

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
/** GCPU_EMU_CSSAV
 */
//-----------------------------------------------------------------------------
UINT8 au1HashKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

UINT8 au1HashDat[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

UINT8 au1HashRes[] = {0x69, 0xd5, 0xc2, 0xeb, 0x2e, 0x2e, 0x62, 0x47,
                    0x50, 0x54, 0x1d, 0x3b, 0xbc, 0x69, 0x2b, 0xa5};

INT32 GCPU_EMU_VCPSHash()
{
    VCPS_HASH_PARAM_T rParam;

    x_memcpy(rParam.au1Key, au1HashKey, sizeof(au1HashKey));
    x_memcpy(rParam.au1Dat, au1HashDat, sizeof(au1HashDat));

    _GCPU_VCPS_Cmd(0, GCPU_VCPS_HASH, &rParam);

    if(x_memcmp(rParam.au1Res, au1HashRes, sizeof(au1HashRes)) == 0)
    {
        Printf("VCPS Hash Pass \n");
    }
    else
    {
        Printf("VCPS Hash Fail\n");
        while (1);
    }

    return 0;
}


UINT8 au1KP[] = {0x8e, 0x26, 0x52, 0xc3, 0x39, 0x79, 0xb5, 0xf8,
                    0x2e, 0xa5, 0xfe, 0x3e, 0xd5, 0xee, 0x72, 0x45};

INT32 GCPU_EMU_VCPSDPAK()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    UINT32 i, u4Mode, u4APSTB, u4PacketNum;
    CPRM_CIPHER_MODE eMode;
    VCPS_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1vidi_raw);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_VCPS_PACKET_SIZE);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, au1vidi_enc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1vidi_raw, sizeof(au1vidi_raw));
    x_memcpy(rParam.au1Key, au1KP, sizeof(au1KP));

    u4PacketNum = (u4GoldenFileSize + (GCPU_VCPS_PACKET_SIZE-1))/GCPU_VCPS_PACKET_SIZE;

    for (i = 0; i < u4PacketNum; i++)
    {
        rParam.u4SrcStartAddr = (UINT32) ADDR_INCR_IN_RING(pbSrcBufAddr + u4SrcOffset,
            GCPU_VCPS_PACKET_SIZE * i, pbSrcBufAddr, pbSrcBufAddr + u4BufSize);
        rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rParam.u4DstStartAddr = (UINT32) (UINT32) ADDR_INCR_IN_RING(pbDstBufAddr + u4DstOffset,
            GCPU_VCPS_PACKET_SIZE * i, pbDstBufAddr, pbDstBufAddr + u4BufSize);
        rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rParam.u4DatLen  = GCPU_VCPS_PACKET_SIZE;
        rParam.eMode = DESCRAMBLE_MODE_ON;

        _GCPU_VCPS_Cmd(0, GCPU_VCPS_DPAK, &rParam);
    }

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, pbDstBufAddr + u4DstOffset,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("VCPS DPAK Fail\n");
        while(1);
    }
    else
    {
        Printf("VCPS DPAK Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_VCPSEPAK()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    UINT32 i, u4Mode, u4APSTB, u4PacketNum;
    CPRM_CIPHER_MODE eMode;
    VCPS_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1vidi_enc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_VCPS_PACKET_SIZE);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, au1vidi_raw, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1vidi_enc, sizeof(au1vidi_enc));
    x_memcpy(rParam.au1Key, au1KP, sizeof(au1KP));

    u4PacketNum = (u4GoldenFileSize + (GCPU_VCPS_PACKET_SIZE-1))/GCPU_VCPS_PACKET_SIZE;

    for (i = 0; i < u4PacketNum; i++)
    {
        rParam.u4SrcStartAddr = (UINT32) ADDR_INCR_IN_RING(pbSrcBufAddr + u4SrcOffset,
            GCPU_VCPS_PACKET_SIZE * i, pbSrcBufAddr, pbSrcBufAddr + u4BufSize);
        rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rParam.u4DstStartAddr = (UINT32) (UINT32) ADDR_INCR_IN_RING(pbDstBufAddr + u4DstOffset,
            GCPU_VCPS_PACKET_SIZE * i, pbDstBufAddr, pbDstBufAddr + u4BufSize);
        rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rParam.u4DatLen  = GCPU_VCPS_PACKET_SIZE;
        rParam.eMode = DESCRAMBLE_MODE_ON;

        _GCPU_VCPS_Cmd(0, GCPU_VCPS_EPAK, &rParam);
    }

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, pbDstBufAddr + u4DstOffset,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("VCPS EPAK Fail\n");
        while(1);
    }
    else
    {
        Printf("VCPS EPAK Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

UINT8 au1DKBHHashRes[] = {0x55, 0x45, 0x65, 0x63, 0x2c, 0xa9, 0xff, 0x37,
                    0x7e, 0xf5, 0x24, 0xdd, 0xc4, 0x74, 0x1f, 0x0d};

INT32 GCPU_EMU_VCPSDKBH()
{
    BYTE *pbSrcBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    UINT32 i, u4Mode, u4APSTB, u4PacketNum;
    VCPS_DKB_HASH_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1vidi_dkbh_raw);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, au1vidi_dkbh_raw, u4GoldenFileSize);
    x_memset(rParam.au1HashValue, 0, sizeof(rParam.au1HashValue));

    rParam.u4SrcStartAddr = (UINT32) (pbSrcBufAddr + u4SrcOffset);
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.eMode = CPRM_HASH_FIRST_PACKET_MODE;
    rParam.u4DatLen = 256 * 16;

    _GCPU_VCPS_Cmd(0, GCPU_VCPS_DKBH, &rParam);

    if(x_memcmp(rParam.au1HashValue, au1DKBHHashRes, sizeof(au1DKBHHashRes)) == 0)
    {
        Printf("VCPS DKBH Pass \n");
    }
    else
    {
        Printf("VCPS DKBH Fail\n");
        while (1);
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));

    return 0;
}

UINT8 au1DHDKDat[] = {0xfb, 0x1d, 0x14, 0x04, 0x0b, 0x3d, 0xd6, 0xc9,
                      0x13, 0x0f, 0x96, 0x03, 0xed, 0x0c, 0x7e, 0x9a};
UINT8 au1DHDKAESDat[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
UINT8 au1DHDKAESGol[] = {0x78, 0xcf, 0x9c, 0x98, 0x7f, 0x9c, 0x7f, 0xeb,
                         0x51, 0x4f, 0xe4, 0xa4, 0x19, 0x7b, 0x72, 0x83};

INT32 GCPU_EMU_VCPSDHDK()
{
    VCPS_DHDK_PARAM_T rParam;
    SIM_AES_PARAM_T rAesParam;
    UINT32 u4Param0[20], i;

    rParam.u4SecureDatPtr = _GCPU_AllocSecureSlot(16);
    rParam.u4SecureResPtr = _GCPU_AllocSecureSlot(16);

    //Data
    u4Param0[0] = 0x04141dfb;
    u4Param0[1] = 0xc9d63d0b;
    u4Param0[2] = 0x03960f13;
    u4Param0[3] = 0x9a7e0ced;

    for(i = 0; i < 4; i++)
    {
        _GCPU_Hw_SetSlot(rParam.u4SecureDatPtr + i, u4Param0[i]);
    }

    _GCPU_VCPS_Cmd(0, GCPU_VCPS_DHDK, &rParam);

    x_memcpy(rAesParam.au1Dat, au1DHDKAESDat, sizeof(au1DHDKAESDat));

    rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
    rAesParam.u4SecureKeyPtr = rParam.u4SecureResPtr;

    _GCPU_AES_Cmd(0, GCPU_AES_E, &rAesParam);

    if (0 != x_memcmp(au1DHDKAESGol, rAesParam.au1Res, sizeof(au1DHDKAESGol)))
    {
        Printf("VCPS DHDK Fail \n", i);
        while(1);
    }
    else
    {
        Printf("VCPS DHDK Pass \n", i);
    }

    _GCPU_FreeSecureSlot(rParam.u4SecureDatPtr);
    _GCPU_FreeSecureSlot(rParam.u4SecureResPtr);

    return 0;
}

UINT8 au1CBCKey[] = {0xf7, 0x75, 0x4c, 0xd7, 0x19, 0x26, 0x99, 0x31,
                    0x72, 0x15, 0xc4, 0x75, 0xd3, 0xfc, 0xb9, 0x7a};

UINT8 au1CBCDat[] = {0x61, 0x32, 0x24, 0x5c, 0xaa, 0x56, 0xf9, 0x67,
                    0x1f, 0x8c, 0x30, 0x9b, 0x60, 0x78, 0xc6, 0x50,
                    0x1e, 0xf6, 0x05, 0x61, 0xeb, 0xc5, 0xcb, 0xeb,
                    0x22, 0x8a, 0x18, 0xf5, 0xbc, 0xf2, 0xba, 0x8f};

UINT8 au1CBCRes[] = {0xc0, 0x4b, 0x4d, 0x6a, 0x3b, 0x6c, 0x2d, 0x08,
                    0x6e, 0xe2, 0x32, 0xb9, 0x33, 0xad, 0xfa, 0xb4,
                    0x5b, 0x8c, 0x7b, 0xf4, 0x16, 0x13, 0xc6, 0x16,
                    0xcd, 0x48, 0xab, 0x9c, 0x8b, 0xbd, 0xc4, 0xab};

INT32 GCPU_EMU_VCPSDCBC()
{
    VCPS_AES_CBC_PARAM_T rParam;

    x_memcpy(rParam.au1Key, au1CBCKey, sizeof(au1CBCKey));
    x_memcpy(rParam.au1Dat, au1CBCDat, sizeof(au1CBCDat));

    _GCPU_VCPS_Cmd(0, GCPU_VCPS_DCBC, &rParam);

    if(x_memcmp(rParam.au1Dat, au1CBCRes, sizeof(au1CBCRes)) == 0)
    {
        Printf("VCPS DCBC Pass \n");
    }
    else
    {
        Printf("VCPS DCBC Fail\n");
        while (1);
    }

    return 0;
}

INT32 GCPU_EMU_VCPSECBC()
{
    VCPS_AES_CBC_PARAM_T rParam;

    x_memcpy(rParam.au1Key, au1CBCKey, sizeof(au1CBCKey));
    x_memcpy(rParam.au1Dat, au1CBCRes, sizeof(au1CBCRes));

    _GCPU_VCPS_Cmd(0, GCPU_VCPS_ECBC, &rParam);

    if(x_memcmp(rParam.au1Dat, au1CBCDat, sizeof(au1CBCDat)) == 0)
    {
        Printf("VCPS ECBC Pass \n");
    }
    else
    {
        Printf("VCPS ECBC Fail\n");
        while (1);
    }

    return 0;
}

