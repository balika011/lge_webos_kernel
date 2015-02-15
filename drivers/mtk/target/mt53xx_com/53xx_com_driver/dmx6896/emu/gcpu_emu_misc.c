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
#include "x_ckgen.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"

#include "../gcpu.h"
#include "../dmx_drm_drvif.h"
#include "tsdesc_pattern.h"

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

UINT8 au1Sha1[] = {0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e,
                   0xba, 0xae, 0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5,
                   0xe5, 0x46, 0x70, 0xf1};

INT32 GCPU_EMU_SHA1()
{
    BYTE *pbSrcBufAddr = NULL;
    UINT32 u4SrcOffset, u4GoldenFileSize, u4BufSize;
    MD_PARAM_T rParam;

    char *pstrsha1 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    u4GoldenFileSize = strlen(pstrsha1);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pstrsha1, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.fgFirstPacket = TRUE;
    rParam.fgLastPacket = TRUE;
    rParam.u8BitCnt = 0;

    GCPU_Cmd(0, GCPU_SHA_1, &rParam);

    if(x_memcmp(rParam.au1Hash, au1Sha1, sizeof(au1Sha1)) != 0)
    {
        Printf("SHA1 Fail \n");
        while(1);
    }
    else
    {
        Printf("SHA1 Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));

    return 0;
}

UINT8 au1Sha256[] = {0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
                     0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
                     0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
                     0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1};

INT32 GCPU_EMU_SHA256()
{
    BYTE *pbSrcBufAddr = NULL;
    UINT32 u4SrcOffset, u4GoldenFileSize, u4BufSize;
    MD_PARAM_T rParam;

    char *pstrsha1 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    u4GoldenFileSize = strlen(pstrsha1);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pstrsha1, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.fgFirstPacket = TRUE;
    rParam.fgLastPacket = TRUE;
    rParam.u8BitCnt = 0;

    GCPU_Cmd(0, GCPU_SHA_256, &rParam);

    if(x_memcmp(rParam.au1Hash, au1Sha256, sizeof(au1Sha256)) != 0)
    {
        Printf("SHA256 Fail \n");
        while(1);
    }
    else
    {
        Printf("SHA256 Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));

    return 0;
}

UINT8 au1md5[] = {0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55,
                  0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a};

INT32 GCPU_EMU_MD5()
{
    BYTE *pbSrcBufAddr = NULL;
    UINT32 u4SrcOffset, u4GoldenFileSize, u4BufSize;
    MD_PARAM_T rParam;

    char *pstrmd5 = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
    u4GoldenFileSize = strlen(pstrmd5);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pstrmd5, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.fgFirstPacket = TRUE;
    rParam.fgLastPacket = TRUE;
    rParam.u8BitCnt = 0;

    GCPU_Cmd(0, GCPU_MD5, &rParam);

    if(x_memcmp(rParam.au1Hash, au1md5, sizeof(au1md5)) != 0)
    {
        Printf("MD5 Fail \n");
        while(1);
    }
    else
    {
        Printf("MD5 Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));

    return 0;
}

UINT8 au1Sha224[] = {0x75, 0x38, 0x8b, 0x16, 0x51, 0x27, 0x76, 0xcc,
                     0x5d, 0xba, 0x5d, 0xa1, 0xfd, 0x89, 0x01, 0x50,
                     0xb0, 0xc6, 0x45, 0x5c, 0xb4, 0xf5, 0x8b, 0x19,
                     0x52, 0x52, 0x25, 0x25};

INT32 GCPU_EMU_SHA224()
{
    BYTE *pbSrcBufAddr = NULL;
    UINT32 u4SrcOffset, u4GoldenFileSize, u4BufSize;
    MD_PARAM_T rParam;

    char *pstrsha1 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    u4GoldenFileSize = strlen(pstrsha1);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0)
    {
        ASSERT(0);
    }
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pstrsha1, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.fgFirstPacket = TRUE;
    rParam.fgLastPacket = TRUE;
    rParam.u8BitCnt = 0;

    GCPU_Cmd(0, GCPU_SHA_224, &rParam);

    if(x_memcmp(rParam.au1Hash, au1Sha224, sizeof(au1Sha224)) != 0)
    {
        Printf("SHA224 Fail \n");
        while(1);
    }
    else
    {
        Printf("SHA224 Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));

    return 0;
}

INT32 GCPU_EMU_MEMCPY()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    MEMCPY_PARAM_T rParam;

    u4GoldenFileSize = 1 + rand() % 0x10000; // plus one to avoid from random number is 0
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    GCPU_Cmd(0, GCPU_MEMCPY, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("MEMCPY Fail\n");
        while(1);
    }
    else
    {
        Printf("MEMCPY Pass\n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

#define CHECK_MEM(addr, seed, temp) \
    if ((temp) != (seed)) \
    { \
        Printf("\n    Memory failed at 0x%08x, write: 0x%08x, read: 0x%08x\n", \
            (addr), (seed), (temp)); \
        while(1); \
    }

INT32 GCPU_EMU_DMA()
{
    BYTE *pbSrcBufAddr = NULL;
    UINT32 *pu4GoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4GoldenFileSize, u4BufSize, u4RandomValue;
    DMA_PARAM_T rParam;

    u4GoldenFileSize = GCPU_IMEM_RAM_SIZE * 4;
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pu4GoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0)
    {
        ASSERT(0);
    }

    //MODE 1
    for (i = 0; i < GCPU_DMEM_SIZE; i++)
    {
        u4RandomValue = rand();
        pu4GoldenBufAddr[i] = u4RandomValue;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pu4GoldenBufAddr, u4GoldenFileSize);

    rParam.u4DramStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4DramBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4DramBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4SramStartAddr = 0;
    rParam.u4DatLen = GCPU_DMEM_SIZE * 4 - 1; //hardware will add 1 byte
    rParam.eMode = DMA_MODE_FROM_DRAM_TO_DMEM;

    GCPU_Cmd(0, GCPU_DMA, &rParam);

    for (i = 0; i < GCPU_DMEM_SIZE; i++)
    {
        u4RandomValue = GCPUCMD_READ32(GCPU_REG_MEM_CMD + i);
        CHECK_MEM((UINT32)i, pu4GoldenBufAddr[i], u4RandomValue);
    }

    Printf("DMA Mode %d Pass\n", rParam.eMode);


    //MODE 5
    for (i = 0; i < GCPU_IMEM_RAM_SIZE; i++)
    {
        u4RandomValue = rand();
        u4RandomValue = u4RandomValue & 0x003FFFFF;   // 22-bit instruction length
        pu4GoldenBufAddr[i] = u4RandomValue;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pu4GoldenBufAddr, u4GoldenFileSize);

    rParam.u4DramStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4DramBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4DramBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4SramStartAddr = 0x2000;
    rParam.u4DatLen = GCPU_IMEM_RAM_SIZE * 4 - 1; //hardware will add 1 byte
    rParam.eMode = DMA_MODE_FROM_DRAM_TO_IMEM;

    GCPU_Cmd(0, GCPU_DMA, &rParam);

    GCPUCMD_WRITE32(GCPU_REG_MEM_ADDR, 0x00002000);
    for (i = 0; i < GCPU_IMEM_RAM_SIZE; i++)
    {
        u4RandomValue = GCPUCMD_READ32(GCPU_REG_MEM_DATA);
        CHECK_MEM((UINT32)i, pu4GoldenBufAddr[i], u4RandomValue);
    }

    Printf("DMA Mode %d Pass\n", rParam.eMode);


    //MODE 7
    GCPUCMD_WRITE32(GCPU_REG_MEM_ADDR, 0x80002000);
    for (i = 0; i < GCPU_IMEM_RAM_SIZE; i++)
    {
        u4RandomValue = rand();
        u4RandomValue = u4RandomValue & 0x003FFFFF;   // 22-bit instruction length
        pu4GoldenBufAddr[i] = u4RandomValue;
        GCPUCMD_WRITE32(GCPU_REG_MEM_DATA, u4RandomValue);
    }

    GCPUCMD_WRITE32(GCPU_REG_MEM_ADDR, 0x00002000);
    for (i = 0; i < GCPU_IMEM_RAM_SIZE; i++)
    {
        u4RandomValue = GCPUCMD_READ32(GCPU_REG_MEM_DATA);
        CHECK_MEM((UINT32)i, pu4GoldenBufAddr[i], u4RandomValue);
    }

    rParam.u4DramStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4DramBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4DramBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4SramStartAddr = 0x2000;
    rParam.u4DatLen = GCPU_IMEM_RAM_SIZE * 4 - 1; //hardware will add 1 byte
    rParam.eMode = DMA_MODE_FROM_IMEM_TO_DRAM;

    GCPU_Cmd(0, GCPU_DMA, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pu4GoldenBufAddr, pu4GoldenBufAddr,
        pu4GoldenBufAddr + u4GoldenFileSize, rParam.u4DramStartAddr,
        rParam.u4DramBufStart, rParam.u4DramBufEnd, GCPU_IMEM_RAM_SIZE * 4))
    {
        Printf("DMA Mode %d Fail\n", rParam.eMode);
        while(1);
    }
    else
    {
        Printf("DMA Mode %d Pass\n", rParam.eMode);
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pu4GoldenBufAddr));

    // recover IMEM
    _GCPU_Hw_Init(0);

    return 0;
}

INT32 GCPU_EMU_XOR()
{
    XOR_PARAM_T rParam;

    rParam.au1Dat[0] = 0x0;
    rParam.au1Dat[1] = 0x1;
    rParam.au1Dat[2] = 0x2;
    rParam.au1Dat[3] = 0x3;

    rParam.au1Xor[0] = 0xc;
    rParam.au1Xor[1] = 0xd;
    rParam.au1Xor[2] = 0xe;
    rParam.au1Xor[3] = 0xf;

    GCPU_Cmd(0, GCPU_MEM_XOR, &rParam);

    if (rParam.au1Dat[0] != 0xc || rParam.au1Dat[1] != 0xc ||
        rParam.au1Dat[2] != 0xc || rParam.au1Dat[3] != 0xc)
    {
        Printf("XOR Fail \n");
        while(1);
    }
    else
    {
        Printf("XOR Pass \n");
    }

    return 0;
}

INT32 GCPU_EMU_ROMBIST()
{
    ROBBIST_PARAM_T rParam;
    UINT32 u4Checksum;

    CKGEN_WRITE32(0x600, CKGEN_READ32(0x600) | 1);

    GCPU_Cmd(0, GCPU_ROM_BIST, &rParam);

    // software reset to stop GCPU before dump rom code
    GCPUCMD_WRITE32(GCPU_REG_CTL, 0x0);
    GCPUCMD_WRITE32(GCPU_REG_CTL, 0x7);

    u4Checksum = _GCPU_GetRomCodeChecksum();

    if(u4Checksum == rParam.u4Res)
    {
        Printf("ROM BIST Pass: 0x%x \n", rParam.u4Res);
    }
    else
    {
        Printf("ROM BIST Fail \n");
        while(1);
    }

    CKGEN_WRITE32(0x600, CKGEN_READ32(0x600) & ~(0x1));

    return 0;
}


INT32 GCPU_EMU_RNG()
{
    RNG_PARAM_T rParam;
    UINT32 i;

    for(i = 0; i < 16; i++)
    {
        rParam.au1Seed[i] = i;
    }

    GCPU_Cmd(0, GCPU_RNG, &rParam);

    Printf("RNG random number:  \n");
    for(i = 0; i < 16; i++)
    {
        Printf("%x ", rParam.au1Res[i]);
    }
    Printf("\n");

    return 0;
}


//===RC4
#if 0
#define RC4_TABLESIZE 256

typedef struct
{
	UINT8 S[(RC4_TABLESIZE)];       /* State table */
	UINT8 i, j;                     /* Indices */
} RC4_KEY_T;
#endif
#if 0
#define ADDR_INCR_IN_RING(addr, incr, ringstart, ringend)      \
    ((((addr) + (incr)) < (ringend)) ? ((addr) + (incr)) : (((addr) + (incr)) - ((ringend) - (ringstart))))

void RC4_Init(RC4_KEY_T *prRC4Key, UINT32 u4KeyLen, const UINT8 *pu1Key)
{
    UINT8 j;
    UINT8 k;
    UINT8 t;
    UINT32 i;

    for (i=0;i<RC4_TABLESIZE;i++)
    {
        prRC4Key->S[i] = i;
    }

    prRC4Key->i = 0;
    prRC4Key->j = 0;
    j      = 0;
    k      = 0;
    for (i=0;i<RC4_TABLESIZE;i++)
    {
        t = prRC4Key->S[i];
        j = (UINT8)((j + t + pu1Key[k]) % RC4_TABLESIZE);
        prRC4Key->S[i] = prRC4Key->S[j];
        prRC4Key->S[j] = t;
        k = (UINT8)((k + 1) % u4KeyLen);
    }
}

void RC4_Output(RC4_KEY_T *prRC4Key, UINT32 u4Size, UINT8 *pu1Buf)
{
    UINT8  i = prRC4Key->i;
    UINT8  j = prRC4Key->j;
    UINT8 *p = prRC4Key->S;
    UINT32 ib = 0;

    while (u4Size--)
    {
        UINT8 bTemp1 = 0;
        UINT8 bTemp2 = 0;

        i = ((i + 1) & (RC4_TABLESIZE - 1));
        bTemp1 = p[i];
        j = ((j + bTemp1) & (RC4_TABLESIZE - 1));

        p[i] = p[j];
        p[j] = bTemp1;
        bTemp2 = pu1Buf[ib];
        bTemp2 ^= p[(p[i] + bTemp1) & (RC4_TABLESIZE - 1)];
        pu1Buf[ib] = bTemp2;
        ib++;
    }

    prRC4Key->i = i;
    prRC4Key->j = j;
}
#endif

UINT8 au1RC4Key[12] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb};

INT32 GCPU_EMU_RC4()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL, *pbBuf = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize, u4PartialSize;
    UINT32 u4KeyLen = 8;
    RC4_KEY_T rKey;
    RC4_KEY_PARAM_T rKeyParam;
    RC4_PARAM_T rParam;

    u4GoldenFileSize = 0x1000; // plus one to avoid from random number is 0
    u4PartialSize = (rand() % u4GoldenFileSize);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbBuf = BSP_AllocAlignedDmaMemory(RC4_TABLESIZE, GCPU_FIFO_ALIGNMENT);
    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0 || pbBuf == 0)
    {
        ASSERT(0);
    }

    for(i = 0; i < 0x1000; i++)
    {
        *(UINT8 *)(pbGoldenBufAddr + i) = rand() % 0xff;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize);

    RC4_Init(&rKey, u4KeyLen, au1RC4Key);
    RC4_Output(&rKey, 0x1000, pbGoldenBufAddr);

    x_memcpy(rKeyParam.au1Key, au1RC4Key, u4KeyLen);
    rKeyParam.u4KeyLen = 8;
    rKeyParam.u4keyAddr = (UINT32)pbBuf;

    GCPU_Cmd(0, GCPU_RC4_KSA, &rKeyParam);

    // init value
    x_memcpy(rParam.au1Key, au1RC4Key, u4KeyLen);
    rParam.u4ResidualLen = 0x0;
    rParam.u4InitIJ = 0x0;
    rParam.au4ResidualBytes[0] = 0x0;
    rParam.au4ResidualBytes[1] = 0x0;
    rParam.u4keyAddr = (UINT32)pbBuf;

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4PartialSize;

    GCPU_Cmd(0, GCPU_RC4, &rParam);

    rParam.u4SrcStartAddr = (UINT32) ADDR_INCR_IN_RING(pbSrcBufAddr + u4SrcOffset, u4PartialSize,
        pbSrcBufAddr, pbSrcBufAddr + u4GoldenFileSize);
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) ADDR_INCR_IN_RING(pbDstBufAddr + u4DstOffset, u4PartialSize,
        pbDstBufAddr, pbDstBufAddr + u4GoldenFileSize);
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize - u4PartialSize;

    GCPU_Cmd(0, GCPU_RC4, &rParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, pbDstBufAddr + u4DstOffset,
        pbDstBufAddr, pbDstBufAddr + u4BufSize, u4GoldenFileSize))
    {
        Printf("RC4 decryption Fail\n");
        while(1);
    }
    else
    {
        Printf("RC4 decryption Pass\n");
    }

    BSP_FreeAlignedDmaMemory((void*)pbBuf);
    BSP_FreeAlignedDmaMemory((void*)pbSrcBufAddr);
    BSP_FreeAlignedDmaMemory((void*)pbDstBufAddr);
    BSP_FreeAlignedDmaMemory((void*)pbGoldenBufAddr);

    return 0;
}

extern void HalSet1MSectionTable(UINT32 u4PhyAddr, UINT32 u4VirtAdr);
extern void HalSet4KPageTable(UINT32 u4PhyAddr, UINT32 u4VirtAdr, UINT32* pu4PageTableAddress);

INT32 GCPU_EMU_IOMMU()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    MEMCPY_PARAM_T rParam;

    HalSet1MSectionTable(0x8100000, 0x9000000); //phy=129M, virt=144M
    HalSet1MSectionTable(0x8000000, 0x9100000); //phy=128M, virt=145M
    HalSet1MSectionTable(0x8300000, 0x9200000); //phy=131M, virt=146M
    HalSet1MSectionTable(0x8200000, 0x9300000); //phy=130M, virt=147M

    HalFlushInvalidateDCache();
/*
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0000000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG1, HalGetMMUTableAddress());
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG2, 0x00130011);
*/
    u4GoldenFileSize = 0x2000;
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbSrcBufAddr = 0x90FF000;
    pbDstBufAddr = 0x92FF000;
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    HalFlushInvalidateDCache();

    GCPU_Cmd(0, GCPU_MEMCPY, &rParam);

    HalFlushInvalidateDCache();

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("IOMMU 1M PA non-Continuous MEMCPY Fail \n");
        while(1);
    }
    else
    {
        Printf("IOMMU 1M PA non-Continuous MEMCPY Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_IOMMU2()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    MEMCPY_PARAM_T rParam;

    x_memset((void*)0xa000000, 0, 0x400);
    x_memset((void*)0xa000400, 0, 0x400);

    for(i = 0; i < 0x100000; i+=0x2000)
    {
        HalSet4KPageTable(0x8501000 + i, 0x9600000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M
        HalSet4KPageTable(0x8500000 + i, 0x9601000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M

        HalSet4KPageTable(0x8601000 + i, 0x9700000 + i, (UINT32*)0xa000400); //phy=134M, virt=151M
        HalSet4KPageTable(0x8600000 + i, 0x9701000 + i, (UINT32*)0xa000400); //phy=134M, virt=151M
    }

    HalFlushInvalidateDCache();
/*
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0000000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG1, HalGetMMUTableAddress());
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG2, 0x00130011);
*/
    u4GoldenFileSize = 0x2000;
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbSrcBufAddr = 0x9600000;
    pbDstBufAddr = 0x9700000;
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    HalFlushInvalidateDCache();

    GCPU_Cmd(0, GCPU_MEMCPY, &rParam);

    HalFlushInvalidateDCache();

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("IOMMU 4K PA non-Continuous MEMCPY Fail \n");
        while(1);
    }
    else
    {
        Printf("IOMMU 4K PA non-Continuous MEMCPY Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

INT32 GCPU_EMU_IOMMU3()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    MEMCPY_PARAM_T rParam;

    UINT32 u4SrcVA = 0xc0000000;
    UINT32 u4DesVA = 0xc0100000;

    HalSet1MSectionTable(0x8700000, u4SrcVA); //phy=135M, virt=3584M
    HalSet1MSectionTable(0x8800000, u4DesVA); //phy=136M, virt=3585M

    HalFlushInvalidateDCache();
/*
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0000000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG1, HalGetMMUTableAddress());
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG2, 0x00130011);
*/
    u4GoldenFileSize = 0x10000;
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);

    // This is SW workaround of rev.A IC for gcpu address issue bit[31] = 1 && src start address [7:4] is not multiply of 4
    if((BSP_GetIcVersion() == IC_VER_5396_AA) || (BSP_GetIcVersion() == IC_VER_5368_AA))
    {
        do
        {
            u4SrcOffset = rand() % u4BufSize;
        } while(((u4SrcOffset & 0xf0) >> 4) % 4 != 0);
    }
    else
    {
        u4SrcOffset = rand() % u4BufSize;
    }

    u4DstOffset = rand() % u4BufSize;
    pbSrcBufAddr = u4SrcVA;
    pbDstBufAddr = u4DesVA;
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    HalFlushInvalidateDCache();

    GCPU_Cmd(0, GCPU_MEMCPY, &rParam);

    HalFlushInvalidateDCache();

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
        rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("IOMMU Out of Physical Address SpaceMEMCPY Fail \n");
        while(1);
    }
    else
    {
        Printf("IOMMU Out of Physical Address Space MEMCPY Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}


INT32 GCPU_EMU_IOMMU4()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    MEMCPY_PARAM_T rParam;
    INT32 i4Ret;

    x_memset((void*)0xa000000, 0, 0x400);

    HalSet4KPageTable(0x8501000, 0x9600000, (UINT32*)0xa000000); //phy=133M, virt=150M
    HalSet4KPageTable(0x8500000, 0x9601000, (UINT32*)0xa000000); //phy=133M, virt=150M

    HalFlushInvalidateDCache();

    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0010000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG1, HalGetMMUTableAddress());
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG2, 0x00130011);

    u4GoldenFileSize = 0x1000; // 4K

    pbSrcBufAddr = 0x9601000; // 0x9602000 has no valid page table for overread happen
    pbDstBufAddr = 0x9600000;

    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG9, ((((UINT32)pbSrcBufAddr) & 0xfffff000) | 0x3));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFGA, (((UINT32)(pbSrcBufAddr + u4GoldenFileSize - 1) & 0xfffff000)));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFGB, ((((UINT32)pbSrcBufAddr) & 0xfffff000) | 0x1));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFGC, (((UINT32)(pbSrcBufAddr + u4GoldenFileSize - 1) & 0xfffff000)));

    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize);

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    HalFlushInvalidateDCache();

    i4Ret = GCPU_Cmd(0, GCPU_MEMCPY, &rParam);

    HalFlushInvalidateDCache();

    if (i4Ret == E_GCPU_PAGE_FAULT)
    {
        Printf("IOMMU Overread protection Fail \n");
        while(1);
    }
    else
    {
        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rParam.u4DstStartAddr,
            rParam.u4DstBufStart, rParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("IOMMU Overread protection Fail \n");
            while(1);
        }
        else
        {
            Printf("IOMMU Overread protection Pass \n");
        }
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;

}

// This test will failed at HalDmaBufferStart((void *)prLParam->u4DstBufStart,......
// This address can not be access because of the invalid page table.
INT32 GCPU_EMU_IOMMU5()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    MEMCPY_PARAM_T rParam;
    INT32 i4Ret;

    x_memset((void*)0xa000000, 0, 0x400);

    HalSet4KPageTable(0x8501000, 0x9600000, (UINT32*)0xa000000); //phy=133M, virt=150M
    HalSet4KPageTable(0x8500000, 0x9601000, (UINT32*)0xa000000); //phy=133M, virt=150M

    HalFlushInvalidateDCache();

    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG4, 0x0010000A);
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG1, HalGetMMUTableAddress());
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG2, 0x00130011);

    u4GoldenFileSize = 0x3000; // 4K
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;

    pbSrcBufAddr = 0x9600000;
    pbDstBufAddr = 0x9603000;

    IOMMU_GCPU_WRITE32(REG_IOMMU_CFG9, ((((UINT32)pbSrcBufAddr) & 0xfffff000) | 0x3));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFGA, (((UINT32)(pbSrcBufAddr + u4GoldenFileSize - 1) & 0xfffff000)));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFGB, ((((UINT32)pbSrcBufAddr) & 0xfffff000) | 0x1));
    IOMMU_GCPU_WRITE32(REG_IOMMU_CFGC, (((UINT32)(pbSrcBufAddr + u4GoldenFileSize - 1) & 0xfffff000)));

    rParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;

    Printf("0x00000010 0x%08x 0x%08x 0x%08x\n", rParam.u4SrcStartAddr,
        rParam.u4DstStartAddr, rParam.u4DatLen);
    Printf("0x%08x 0x%08x 0x%08x 0x%08x\n", rParam.u4SrcBufStart, rParam.u4SrcBufEnd,
        rParam.u4DstBufStart, rParam.u4DstBufEnd);

    HalFlushInvalidateDCache();

    i4Ret = GCPU_Cmd(0, GCPU_MEMCPY, &rParam);

    HalFlushInvalidateDCache();

    if (i4Ret != E_GCPU_PAGE_FAULT)
    {
        Printf("IOMMU Page Fault Fail \n");
        while(1);
    }
    else
    {
        Printf("IOMMU Page Fault Pass \n");
    }

    return 0;
}


INT32 GCPU_EMU_TSDesc()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    TSDESC_PARAM_T rParam;
    INT32 i4Ret;
    UINT32 u4VirtualSrcAddr = 0, u4VirtualDesAddr = 0;

    GCPU_Init(0);

    u4GoldenFileSize = sizeof(au1Clear);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = 0;//rand() % u4BufSize;
    u4DstOffset = 0; //rand() % u4BufSize;

    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    u4VirtualSrcAddr = VIRTUAL(pbSrcBufAddr);
    u4VirtualDesAddr = VIRTUAL(pbDstBufAddr);

    x_memcpy(u4VirtualSrcAddr, au1Enc, sizeof(au1Clear));
    rParam.u4SrcStartAddr = (UINT32) u4VirtualSrcAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) u4VirtualSrcAddr;
    rParam.u4SrcBufEnd = (UINT32) u4VirtualSrcAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) u4VirtualDesAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) u4VirtualDesAddr;
    rParam.u4DstBufEnd = (UINT32) u4VirtualDesAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.eForm = TSDESC_188_PACKET_FORM;
    rParam.eMode = TSDESC_FIRST_PACKET_MODE;
    rParam.eScheme = TSDESC_DESC_SCHEME_CBC_SCTE52;
    rParam.u4TsState[0] = 0;
    rParam.u4TsState[1] = 0;
    rParam.u4TsState[2] = 0;
    rParam.u4TsState[3] = 0;

    for(i = 0; i < 16; i++)
    {
        rParam.au1Key[0][i] = 0x55;
    }

    for(i = 0; i < 16; i++)
    {
        rParam.au1Key[1][i] = 0xaa;
    }

    for(i = 0; i < 2; i++)
    {
        rParam.au1Iv[i][0] = 0x12;
        rParam.au1Iv[i][1] = 0x34;
        rParam.au1Iv[i][2] = 0x56;
        rParam.au1Iv[i][3] = 0x78;
        rParam.au1Iv[i][4] = 0xde;
        rParam.au1Iv[i][5] = 0xad;
        rParam.au1Iv[i][6] = 0xbe;
        rParam.au1Iv[i][7] = 0xef;
        rParam.au1Iv[i][8] = 0xba;
        rParam.au1Iv[i][9] = 0xbe;
        rParam.au1Iv[i][10] = 0xab;
        rParam.au1Iv[i][11] = 0xba;
        rParam.au1Iv[i][12] = 0xaa;
        rParam.au1Iv[i][13] = 0x55;
        rParam.au1Iv[i][14] = 0xaa;
        rParam.au1Iv[i][15] = 0x55;
    }

    HalFlushInvalidateDCache();

    i4Ret = GCPU_Cmd(0, GCPU_TSDESC, &rParam);

    HalFlushInvalidateDCache();

    if(x_memcmp(u4VirtualDesAddr, au1Clear, sizeof(au1Clear)) != 0)
    {
        Printf("Failed\n");
    }
    else
    {
        Printf("Pass\n");
    }

    BSP_FreeAlignedDmaMemory(pbSrcBufAddr);
    BSP_FreeAlignedDmaMemory(pbDstBufAddr);

    return 0;
}


INT32 GCPU_EMU_TSScrm()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 i, u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    TSDESC_PARAM_T rParam;
    INT32 i4Ret;
    UINT32 u4VirtualSrcAddr = 0, u4VirtualDesAddr = 0;

    GCPU_Init(0);

    u4GoldenFileSize = sizeof(au1Clear);
    u4BufSize = _DMX_Align(u4GoldenFileSize, GCPU_FIFO_ALIGNMENT);
    u4SrcOffset = 0;//rand() % u4BufSize;
    u4DstOffset = 0; //rand() % u4BufSize;

    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    u4VirtualSrcAddr = VIRTUAL(pbSrcBufAddr);
    u4VirtualDesAddr = VIRTUAL(pbDstBufAddr);

    x_memcpy(u4VirtualSrcAddr, au1Clear, sizeof(au1Clear));
    rParam.u4SrcStartAddr = (UINT32) u4VirtualSrcAddr + u4SrcOffset;
    rParam.u4SrcBufStart = (UINT32) u4VirtualSrcAddr;
    rParam.u4SrcBufEnd = (UINT32) u4VirtualSrcAddr + u4BufSize;
    rParam.u4DstStartAddr = (UINT32) u4VirtualDesAddr + u4DstOffset;
    rParam.u4DstBufStart = (UINT32) u4VirtualDesAddr;
    rParam.u4DstBufEnd = (UINT32) u4VirtualDesAddr + u4BufSize;
    rParam.u4DatLen  = u4GoldenFileSize;
    rParam.eForm = TSDESC_188_PACKET_FORM;
    rParam.eMode = TSDESC_FIRST_PACKET_MODE;
    rParam.eScheme = TSDESC_DESC_SCHEME_EVEN_KEY;
    rParam.u4TsState[0] = 0;
    rParam.u4TsState[1] = 0;
    rParam.u4TsState[2] = 0;
    rParam.u4TsState[3] = 0;

    for(i = 0; i < 16; i++)
    {
        rParam.au1Key[0][i] = 0x55;
    }

    for(i = 0; i < 16; i++)
    {
        rParam.au1Key[1][i] = 0xaa;
    }

    for(i = 0; i < 2; i++)
    {
        rParam.au1Iv[i][0] = 0x12;
        rParam.au1Iv[i][1] = 0x34;
        rParam.au1Iv[i][2] = 0x56;
        rParam.au1Iv[i][3] = 0x78;
        rParam.au1Iv[i][4] = 0xde;
        rParam.au1Iv[i][5] = 0xad;
        rParam.au1Iv[i][6] = 0xbe;
        rParam.au1Iv[i][7] = 0xef;
        rParam.au1Iv[i][8] = 0xba;
        rParam.au1Iv[i][9] = 0xbe;
        rParam.au1Iv[i][10] = 0xab;
        rParam.au1Iv[i][11] = 0xba;
        rParam.au1Iv[i][12] = 0xaa;
        rParam.au1Iv[i][13] = 0x55;
        rParam.au1Iv[i][14] = 0xaa;
        rParam.au1Iv[i][15] = 0x55;
    }

    HalFlushInvalidateDCache();

    i4Ret = GCPU_Cmd(0, GCPU_TSSCRM, &rParam);

    HalFlushInvalidateDCache();

    // even scramble, the data is the same
    if(x_memcmp(u4VirtualDesAddr, au1Clear, sizeof(au1Clear)) == 0)
    {
        Printf("Failed\n");
        while(1);
    }
    else
    {
        Printf("OK\n");
    }

    BSP_FreeAlignedDmaMemory(pbSrcBufAddr);
    BSP_FreeAlignedDmaMemory(pbDstBufAddr);

    return 0;
}

