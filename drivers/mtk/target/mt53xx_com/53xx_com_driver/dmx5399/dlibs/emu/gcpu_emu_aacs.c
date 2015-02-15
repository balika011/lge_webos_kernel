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
#include "aacs_pattern.h"
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

UINT8 au1UK[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

//-----------------------------------------------------------------------------
/** GCPU_EMU_AACSBDAV
 */
//-----------------------------------------------------------------------------
INT32 GCPU_EMU_AACSBDAV()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AACS_AV_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1aacs_bdraw);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_AACS_BDMV_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1aacs_bdenc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1aacs_bdraw, sizeof(au1aacs_bdraw));
    x_memcpy(rParam.au1Uk, au1UK, sizeof(au1UK));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_AACS_DBD, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("AACS BDAV D Fail\n");
        while(1);
    }
    else
    {
        Printf("AACS BDAV D Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_AACSBEAV()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AACS_AV_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1aacs_bdenc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_AACS_BDMV_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1aacs_bdraw, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1aacs_bdenc, sizeof(au1aacs_bdenc));
    x_memcpy(rParam.au1Uk, au1UK, sizeof(au1UK));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_AACS_EBD, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("AACS BDAV E Fail\n");
        while(1);
    }
    else
    {
        Printf("AACS BDAV E Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_AACSDTN()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AACS_AV_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1aacs_tnraw);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_AACS_BDMV_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1aacs_tnenc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1aacs_tnraw, sizeof(au1aacs_tnraw));
    x_memcpy(rParam.au1Uk, au1UK, sizeof(au1UK));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_AACS_DTN, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("AACS DTN Fail\n");
        while(1);
    }
    else
    {
        Printf("AACS DTN Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}


INT32 GCPU_EMU_AACSETN()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AACS_AV_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1aacs_tnenc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_AACS_BDMV_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1aacs_tnraw, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1aacs_tnenc, sizeof(au1aacs_tnenc));
    x_memcpy(rParam.au1Uk, au1UK, sizeof(au1UK));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_AACS_ETN, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("AACS ETN Fail\n");
        while(1);
    }
    else
    {
        Printf("AACS ETN Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

UINT8 au1Kt[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

UINT8 au1Cpi[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                  0x88, 0x99, 0xaa, 0xbb};

INT32 GCPU_EMU_AACSDHD()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AACS_HDDVD_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1aacs_hdraw);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_AACS_HDVD_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1aacs_hdenc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1aacs_hdraw, sizeof(au1aacs_hdraw));
    x_memcpy(rParam.au1Kt, au1Kt, sizeof(au1Kt));
    x_memcpy(rParam.au1Cpi, au1Cpi, sizeof(au1Cpi));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.eMode = DESCRAMBLE_MODE_ON;

    GCPU_Cmd(0, GCPU_AACS_DHD, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("AACS DHD Fail\n");
        while(1);
    }
    else
    {
        Printf("AACS DHD Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_AACSEHD()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AACS_HDDVD_PARAM_T rParam;

    u4GoldenFileSize = sizeof(au1aacs_hdenc);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_AACS_HDVD_PACKET_SIZE);
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
        pbSrcBufAddr + u4BufSize, au1aacs_hdraw, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1aacs_hdenc, sizeof(au1aacs_hdenc));
    x_memcpy(rParam.au1Kt, au1Kt, sizeof(au1Kt));
    x_memcpy(rParam.au1Cpi, au1Cpi, sizeof(au1Cpi));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.eMode = DESCRAMBLE_MODE_ON;

    GCPU_Cmd(0, GCPU_AACS_EHD, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("AACS EHD Fail\n");
        while(1);
    }
    else
    {
        Printf("AACS EHD Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

UINT8 au1Kmsi[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
UINT8 au1Xfcr[] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
                   0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
UINT8 au1Dkder[] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0xab, 0xcd,
                    0xef, 0x01, 0x00, 0x00};
UINT8 au1Dkder2[] = {0x69, 0xd5, 0xc2, 0xeb, 0x2e, 0x2e, 0x62, 0x47,
                     0x50, 0x54, 0x00, 0x00};
UINT8 au1Dv[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                     0x01, 0x23, 0x00, 0x00};

INT32 GCPU_EMU_AACSDVCALC()
{
    UINT32 i, j, u4KmsiSlot, u4DvSlot, u4DVLSB10, u4CMP;
    UINT8 *p;
    AACS_DVCALC_PARAM_T rParam;

    u4KmsiSlot = _GCPU_AllocSecureSlot(16);
    u4DvSlot = _GCPU_AllocSecureSlot(12);

    for (i = 0; i < 3; i++)
    {
        //KMSI - Mode0/1/2
        x_memcpy(rParam.au1Kmsi, au1Kmsi, sizeof(au1Kmsi));
        //XFCR - Mode0/1/2
        x_memcpy(rParam.au1Xfcr, au1Xfcr, sizeof(au1Xfcr));

        //DKDER - Mode0/1
        if (i != 2)
        {
            x_memcpy(rParam.au1Dkder, au1Dkder, sizeof(au1Dkder));
        }
        else
        {
            x_memcpy(rParam.au1Dkder, au1Dkder2, sizeof(au1Dkder2));
        }

        //DV - Mode0/1/2
        x_memcpy(rParam.au1Dv, au1Dv, sizeof(au1Dv));

        for(j = 0; j < 4; j++)
        {
            p = rParam.au1Kmsi + j * 4;
            _GCPU_Hw_SetSlot(u4KmsiSlot + j, KEY_WRAPPER(p));
        }

        for(j = 0; j < 3; j++)
        {
            p = rParam.au1Dv + j * 4;
            _GCPU_Hw_SetSlot(u4DvSlot + j, KEY_WRAPPER(p));
        }

        rParam.u4Mode = i;
        rParam.u4KmsiSecureSlotPtr = u4KmsiSlot;
        rParam.u4DvSecureSlotPtr = u4DvSlot;

        GCPU_Cmd(0, GCPU_AACS_DV_CALC, &rParam);

        if (i == 0)
        {
            u4DVLSB10 = 0x355;
            u4CMP = 0x1;
        }
        else if (i == 1)
        {
            u4DVLSB10 = 0x276;
            u4CMP = 0x1;
        }
        else
        {
            u4DVLSB10 = 0x0;
            u4CMP = 0x0;
        }

        if ((rParam.u4DVLSB10 != u4DVLSB10) ||
        	  (rParam.u4Cmp != u4CMP))
        {
            Printf("AACS DVCALC Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AACS DVCALC Pass - Mode %d \n", i);
        }
    }

    _GCPU_FreeSecureSlot(u4KmsiSlot);
    _GCPU_FreeSecureSlot(u4DvSlot);

    return 0;
}
