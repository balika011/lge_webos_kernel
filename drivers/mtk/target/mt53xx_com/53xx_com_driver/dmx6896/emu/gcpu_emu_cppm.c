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
#include "cprm_pattern.h"
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
UINT8 au1C2Key[] = {0x00, 0x2b, 0x08, 0xc4, 0xb6, 0xb2, 0x14, 0x59};
UINT8 au1C2Dat[] = {0xaa, 0x2b, 0x75, 0x75, 0x9a, 0x42, 0xb6, 0x49};
UINT8 au1C2Res[] = {0xce, 0xb1, 0xa2, 0x71, 0x39, 0x1b, 0x9d, 0x62};
UINT8 au1C2GRes[] = {0x64, 0x9a, 0xd7, 0x04, 0xa3, 0x59, 0x2b, 0x2b};

//-----------------------------------------------------------------------------
/** GCPU_EMU_CSSAV
 */
//-----------------------------------------------------------------------------
INT32 GCPU_EMU_C2D()
{
    C2_PARAM_T rParam;
    x_memcpy(rParam.au1Key, au1C2Key, sizeof(au1C2Key));
    x_memcpy(rParam.au1Dat, au1C2Dat, sizeof(au1C2Dat));

    GCPU_Cmd(0, GCPU_C2_D, &rParam);

    if(x_memcmp(rParam.au1Res, au1C2Res, sizeof(au1C2Res)) == 0)
    {
        Printf("CPPM C2D Pass \n");
    }
    else
    {
        Printf("CPPM C2D Fail \n");
        while (1);
    }

    return 0;
}

INT32 GCPU_EMU_C2E()
{
    C2_PARAM_T rParam;
    x_memcpy(rParam.au1Key, au1C2Key, sizeof(au1C2Key));
    x_memcpy(rParam.au1Dat, au1C2Res, sizeof(au1C2Res));

    GCPU_Cmd(0, GCPU_C2_E, &rParam);

    if(x_memcmp(rParam.au1Res, au1C2Dat, sizeof(au1C2Dat)) == 0)
    {
        Printf("CPPM C2E Pass \n");
    }
    else
    {
        Printf("CPPM C2E Fail \n");
        while (1);
    }

    return 0;
}


INT32 GCPU_EMU_C2G()
{
    C2_PARAM_T rParam;
    x_memcpy(rParam.au1Key, au1C2Key, sizeof(au1C2Key));
    x_memcpy(rParam.au1Dat, au1C2Res, sizeof(au1C2Res));

    GCPU_Cmd(0, GCPU_C2_G, &rParam);

    if(x_memcmp(rParam.au1Res, au1C2GRes, sizeof(au1C2GRes)) == 0)
    {
        Printf("CPPM C2G Pass \n");
    }
    else
    {
        Printf("CPPM C2G Fail \n");
        while (1);
    }

    return 0;
}

UINT8 au1C2HRes[] = {0xf1, 0x33, 0x97, 0xdb, 0xf1, 0xea, 0xcb, 0xa1};

INT32 GCPU_EMU_C2H()
{
    BYTE *pbSrcBufAddr = NULL;
    UINT32 u4SrcOffset, u4GoldenFileSize, u4BufSize;
    C2_HASH_PARAM_T rParam;

    u4GoldenFileSize = 2048;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);

    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, au1CPRMRaw, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DatLen = u4GoldenFileSize;
    rParam.fgFirstPacket = TRUE;
    rParam.fgLastPacket = FALSE;
    x_memset(rParam.au1Hash, 0, sizeof(rParam.au1Hash));

    GCPU_Cmd(0, GCPU_C2_H, &rParam);

    if(x_memcmp(rParam.au1Hash, au1C2HRes, sizeof(au1C2HRes)) == 0)
    {
        Printf("CPPM C2H Pass \n");
    }
    else
    {
        Printf("CPPM C2H Fail \n");
        while (1);
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));

    return 0;
}

UINT8 au1CPRMKey[] = {0x00, 0x0f, 0xb2, 0xe6, 0xfe, 0x33, 0x2c, 0xdd};

INT32 GCPU_EMU_CPPMDPAK()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    CPRM_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1CPPMEnc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, au1CPPMEnc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1CPPMRaw, sizeof(au1CPPMRaw));
    x_memcpy(rParam.au1Key, au1CPRMKey, sizeof(au1CPRMKey));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.eMode = CPRM_CIPHER_MODE_AUTO;
    rParam.u4APSTB = 0;

    GCPU_Cmd(0, GCPU_CPPM_DPAK, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("CPPM DPAK Fail\n");
        while(1);
    }
    else
    {
        Printf("CPPM DPAK Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_CPRMDPAK()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    UINT32 i, u4Mode, u4APSTB, u4PacketNum;
    CPRM_CIPHER_MODE eMode;
    CPRM_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1CPRMEnc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_CPRM_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1CPRMEnc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1CPRMRaw, sizeof(au1CPRMRaw));
    x_memcpy(rParam.au1Key, au1CPRMKey, sizeof(au1CPRMKey));

    u4PacketNum = (u4GoldenFileSize + (GCPU_CPRM_PACKET_SIZE-1))/GCPU_CPRM_PACKET_SIZE;

    for (i = 0; i < u4PacketNum; i++)
    {
        u4APSTB = 0;

        if (i < 4)
        {
            eMode = CPRM_CIPHER_MODE_ON;
            u4APSTB = i;
        }
        else if (i >= 4 && i <= 5)
        {
            eMode = CPRM_CIPHER_MODE_OFF;
        }
        else
        {
            eMode = CPRM_CIPHER_MODE_AUTO;
        }
        rParam.u4SrcStartAddr = (UINT32) ADDR_INCR_IN_RING(pbSrcBufAddr + u4SrcOffset,
            GCPU_CPRM_PACKET_SIZE * i, pbSrcBufAddr, pbSrcBufAddr + u4BufSize);
        rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rParam.u4DstStartAddr = (UINT32) (UINT32) ADDR_INCR_IN_RING(pbDstBufAddr + u4DstOffset,
            GCPU_CPRM_PACKET_SIZE * i, pbDstBufAddr, pbDstBufAddr + u4BufSize);
        rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rParam.u4DatLen  = GCPU_CPRM_PACKET_SIZE;
        rParam.eMode = eMode;
        rParam.u4APSTB = u4APSTB;

        GCPU_Cmd(0, GCPU_CPRM_DPAK, &rParam);
    }

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, pbDstBufAddr + u4DstOffset,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("CPRM DPAK Fail\n");
        while(1);
    }
    else
    {
        Printf("CPRM DPAK Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}


INT32 GCPU_EMU_CPRMEPAK()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    UINT32 i, u4Mode, u4APSTB, u4PacketNum;
    CPRM_CIPHER_MODE eMode;
    CPRM_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1CPRMEnc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_CPRM_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1CPRMRaw, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1CPRMEnc, sizeof(au1CPRMEnc));
    x_memcpy(rParam.au1Key, au1CPRMKey, sizeof(au1CPRMKey));

    u4PacketNum = (u4GoldenFileSize + (GCPU_CPRM_PACKET_SIZE-1))/GCPU_CPRM_PACKET_SIZE;

    for (i = 0; i < u4PacketNum; i++)
    {
        u4APSTB = 0;

        if (i < 4)
        {
            eMode = CPRM_CIPHER_MODE_ON;
            u4APSTB = i;
        }
        else if (i >= 4 && i <= 5)
        {
            eMode = CPRM_CIPHER_MODE_OFF;
        }
        else
        {
            eMode = CPRM_CIPHER_MODE_AUTO;
        }

        rParam.u4SrcStartAddr = (UINT32) ADDR_INCR_IN_RING(pbSrcBufAddr + u4SrcOffset,
            GCPU_CPRM_PACKET_SIZE * i, pbSrcBufAddr, pbSrcBufAddr + u4BufSize);
        rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rParam.u4DstStartAddr = (UINT32) ADDR_INCR_IN_RING(pbDstBufAddr + u4DstOffset,
            GCPU_CPRM_PACKET_SIZE * i, pbDstBufAddr, pbDstBufAddr + u4BufSize);
        rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rParam.u4DatLen  = GCPU_CPRM_PACKET_SIZE;
        rParam.eMode = eMode;
        rParam.u4APSTB = u4APSTB;

        GCPU_Cmd(0, GCPU_CPRM_EPAK, &rParam);
    }

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, pbDstBufAddr + u4DstOffset,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("CPRM EPAK Fail\n");
        while(1);
    }
    else
    {
        Printf("CPRM EPAK Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

UINT8 au1CPRMKT[] = {0x00, 0x56, 0x4f, 0xa6, 0xff, 0xff, 0xff, 0xff};
UINT8 au1CPRMDCI[] = {0x93, 0x24, 0x12, 0x34, 0x1f, 0xa1, 0xd1, 0xe3};
UINT8 au1CPRMDTKC[] = {0x34, 0x57, 0x97, 0x63, 0x73, 0xab, 0xfc, 0xd8};
UINT8 au1CPRMDCIRes[] = {0x5f, 0x1b, 0x31, 0x47, 0x31, 0xcb, 0x82, 0xf4};

INT32 GCPU_EMU_CCIVERIFY()
{
    CPRM_DCICCI_PARAM_T rParam;

    x_memcpy(rParam.au1KT, au1CPRMKT, sizeof(au1CPRMKT));
    x_memcpy(rParam.au1DCI, au1CPRMDCI, sizeof(au1CPRMDCI));
    x_memcpy(rParam.au1DTKC, au1CPRMDTKC, sizeof(au1CPRMDTKC));

    GCPU_Cmd(0, GCPU_CPRM_DCI_VFY, &rParam);

    if(x_memcmp(rParam.au1Res, au1CPRMDCIRes, sizeof(au1CPRMDCIRes)) == 0)
    {
        Printf("CPPM DCI/CCI Verify Pass \n");
    }
    else
    {
        Printf("CPPM DCI/CCI Verify Fail\n");
        while (1);
    }

    return 0;
}


