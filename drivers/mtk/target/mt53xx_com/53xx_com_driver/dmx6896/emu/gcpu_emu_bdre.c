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
#include "bdre_pattern.h"
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

UINT8 au1Krec[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

//-----------------------------------------------------------------------------
/** GCPU_EMU_CSSAV
 */
//-----------------------------------------------------------------------------
INT32 GCPU_EMU_BDREDBD()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    BDRE_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1bdre_bdraw_8520);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_BDRE_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1bdre_bdenc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1bdre_bdraw_8520, sizeof(au1bdre_bdraw_8520));
    x_memcpy(rParam.au1Krec, au1Krec, sizeof(au1Krec));
    rParam.u4Vbkg = 0x67452301;
    rParam.eMode = BDRE_ALWAYS_ON_MODE;

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_BDRE_DBD, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("BDRE DBD Fail\n");
        while(1);
    }
    else
    {
        Printf("BDRE DBD Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_BDREEBD()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    BDRE_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1bdre_bdenc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_BDRE_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1bdre_bdraw_8520, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1bdre_bdenc, sizeof(au1bdre_bdenc));
    x_memcpy(rParam.au1Krec, au1Krec, sizeof(au1Krec));
    rParam.u4Vbkg = 0x67452301;
    rParam.eMode = BDRE_ALWAYS_ON_MODE;

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_BDRE_EBD, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("BDRE EBD Fail\n");
        while(1);
    }
    else
    {
        Printf("BDRE EBD Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_BDREDTN()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    BDRE_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1bdre_tnraw_8520);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_BDRE_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1bdre_tnenc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1bdre_tnraw_8520, sizeof(au1bdre_tnraw_8520));
    x_memcpy(rParam.au1Krec, au1Krec, sizeof(au1Krec));
    rParam.u4Vbkg = 0x67452301;
    rParam.eMode = BDRE_ALWAYS_ON_MODE;

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_BDRE_DTN, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("BDRE DTN Fail\n");
        while(1);
    }
    else
    {
        Printf("BDRE DTN Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_BDREETN()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    BDRE_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1bdre_tnenc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_BDRE_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1bdre_tnraw_8520, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1bdre_tnenc, sizeof(au1bdre_tnenc));
    x_memcpy(rParam.au1Krec, au1Krec, sizeof(au1Krec));
    rParam.u4Vbkg = 0x67452301;
    rParam.eMode = BDRE_ALWAYS_ON_MODE;

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_BDRE_ETN, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("BDRE ETN Fail\n");
        while(1);
    }
    else
    {
        Printf("BDRE ETN Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

UINT8 au1Src0[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
UINT8 au1Src1[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
UINT8 au1Res0[] = {0x00, 0x02, 0x06, 0x04, 0x0c, 0x0e, 0x0a, 0x08};
UINT8 au1Res1[] = {0x02, 0x64, 0xce, 0xa8, 0x9a, 0xfc, 0x56, 0x30};

INT32 GCPU_EMU_BDREBE()
{
    BDRE_BE_PARAM_T rParam;
    UINT8 i, au1Res[8];

    for (i = 0; i < 2; i++)
    {
        if(i == 0)
        {
            x_memcpy(rParam.au1Dat, au1Src0, sizeof(au1Src0));
            x_memcpy(au1Res, au1Res0, sizeof(au1Res0));
        }
        else if(i == 1)
        {
            x_memcpy(rParam.au1Dat, au1Src1, sizeof(au1Src1));
            x_memcpy(au1Res, au1Res1, sizeof(au1Res1));
        }

        GCPU_Cmd(0, GCPU_BDRE_BE, &rParam);

        if(x_memcmp(rParam.au1Dat, au1Res, sizeof(au1Res)) != 0)
        {
            Printf("BDRE BE Err Mode %x \n", i);
            while(1);
        }
    }

    Printf("BDRE BE Pass \n");

    return 0;
}


