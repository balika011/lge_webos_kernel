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
#include "aes_pattern.h"
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
/** GCPU_EMU_AESCTRD
 */
//-----------------------------------------------------------------------------

static UINT8 au1CtrKey128[16] = {0x77, 0x00, 0x51, 0xC1, 0x7E, 0xD4, 0x7A, 0xAE,
                    0x5D, 0x03, 0x2B, 0xC2, 0x04, 0x50, 0xA8, 0x19};

static UINT8 au1CtrKey192[24] = {0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b};

static UINT8 au1CtrKey256[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};

static UINT8 au1CtrVal128[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static UINT8 au1CtrVal192[16] = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};

static UINT8 au1CtrVal256[16] = {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
                    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};

INT32 GCPU_EMU_AESCTRD()
{
    UINT8 u1Key[32], i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AES_PARAM_T rAesParam;

    for (i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            //Load Src - 128 bit
            u4GoldenFileSize = sizeof(au1aesctr_128_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aesctr_128_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aesctr_128_raw, sizeof(au1aesctr_128_raw));

            x_memcpy(rAesParam.au1Key, au1CtrKey128, sizeof(au1CtrKey128));
            x_memcpy(rAesParam.au1Iv, au1CtrVal128, sizeof(au1CtrVal128));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 1)
        {
            //Load Src - 192 bit
            u4GoldenFileSize = sizeof(au1aesctr_192_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aesctr_192_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aesctr_192_raw, sizeof(au1aesctr_192_raw));

            x_memcpy(rAesParam.au1Key, au1CtrKey192, sizeof(au1CtrKey192));
            x_memcpy(rAesParam.au1Iv, au1CtrVal192, sizeof(au1CtrVal192));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else if (i == 2)
        {
            //Load Src - 256 bit
            u4GoldenFileSize = sizeof(au1aesctr_256_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aesctr_256_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aesctr_256_raw, sizeof(au1aesctr_256_raw));

            x_memcpy(rAesParam.au1Key, au1CtrKey256, sizeof(au1CtrKey256));
            x_memcpy(rAesParam.au1Iv, au1CtrVal256, sizeof(au1CtrVal256));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_256;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
        rAesParam.fgEncrypt = FALSE;

        GCPU_Cmd(0, GCPU_AES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("AES CTR Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AES CTR Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}


static UINT8 au1HDCPKey128[16] = {0x60, 0x11, 0x47, 0x8f, 0xf7, 0x37, 0xe6, 0xc8,
                    0xa4, 0xf2, 0x03, 0xaf, 0xf3, 0x4f , 0xdd, 0x16};

static UINT8 au1HDCPVal128[16] = {0x9a, 0x6d, 0x11, 0x00, 0xa9, 0xb7, 0x6f, 0x64,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


INT32 GCPU_EMU_AESHDCP()
{
    UINT8 u1Key[32], i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AES_PARAM_T rAesParam;

    //Load Src - 128 bit
    u4GoldenFileSize = sizeof(au1aeshdcp_128_raw);
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
    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, au1aeshdcp_128_enc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1aeshdcp_128_raw, sizeof(au1aeshdcp_128_raw));

    x_memcpy(rAesParam.au1Key, au1HDCPKey128, sizeof(au1HDCPKey128));
    x_memcpy(rAesParam.au1Iv, au1HDCPVal128, sizeof(au1HDCPVal128));
    rAesParam.eKeyBitLen = KEY_BIT_LEN_128;

    rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rAesParam.u4DatLen  = u4GoldenFileSize;

    rAesParam.eMode = BLOCK_CIPHER_MODE_CTR;
    rAesParam.fgEncrypt = FALSE;

    GCPU_Cmd(0, GCPU_AES, &rAesParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
        rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("AES HDCP Fail - Mode %d \n", i);
        while(1);
    }
    else
    {
        Printf("AES HDCP Pass - Mode %d \n", i);
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

static UINT8 au1OfbKey128[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

static UINT8 au1OfbKey192[24] = {0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b};

static UINT8 au1OfbKey256[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};

static UINT8 au1OfbIv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

INT32 GCPU_EMU_AESOFBD()
{
    UINT8 u1Key[32], i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AES_PARAM_T rAesParam;

    for (i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            //Load Src - 128 bit
            u4GoldenFileSize = sizeof(au1aesofb_128_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aesofb_128_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aesofb_128_raw, sizeof(au1aesofb_128_raw));
            x_memcpy(rAesParam.au1Key, au1OfbKey128, sizeof(au1OfbKey128));
            x_memcpy(rAesParam.au1Iv, au1OfbIv, sizeof(au1OfbIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 1)
        {
            //Load Src - 192 bit
            u4GoldenFileSize = sizeof(au1aesofb_192_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aesofb_192_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aesofb_192_raw, sizeof(au1aesofb_192_raw));
            x_memcpy(rAesParam.au1Key, au1OfbKey192, sizeof(au1OfbKey192));
            x_memcpy(rAesParam.au1Iv, au1OfbIv, sizeof(au1OfbIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else if (i == 2)
        {
            //Load Src - 256 bit
            u4GoldenFileSize = sizeof(au1aesofb_256_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aesofb_256_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aesofb_256_raw, sizeof(au1aesofb_256_raw));
            x_memcpy(rAesParam.au1Key, au1OfbKey256, sizeof(au1OfbKey256));
            x_memcpy(rAesParam.au1Iv, au1OfbIv, sizeof(au1OfbIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_256;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_OFB;
        rAesParam.fgEncrypt = FALSE;

        GCPU_Cmd(0, GCPU_AES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("AES OFB Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AES OFB Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}


static UINT8 au1PAK128[16] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
                    0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};

static UINT8 au1PAK192[16] = {0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0,
                    0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91};

static UINT8 au1PAK256[16] = {0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
                    0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89};

static UINT8 au1PAKGloden[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

INT32 GCPU_EMU_AESDPAK()
{
    UINT8 u1Key[32], i, j;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AES_PARAM_T rAesParam;

    u4GoldenFileSize = 16;
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

    for (i=0; i<3; i++)
    {
        if (i == 0)
        {
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1PAK128, u4GoldenFileSize);
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 1)
        {
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1PAK192, u4GoldenFileSize);
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else if (i == 2)
        {
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1PAK256, u4GoldenFileSize);
            rAesParam.eKeyBitLen = KEY_BIT_LEN_256;
        }

        //Load Golden
        x_memcpy(pbGoldenBufAddr, au1PAKGloden, u4GoldenFileSize);

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = 16;

        for(j = 0; j < 32; j++)
        {
            rAesParam.au1Key[j] = j;
        }

        rAesParam.eMode = BLOCK_CIPHER_MODE_ECB;
        rAesParam.fgEncrypt = FALSE;

        GCPU_Cmd(0, GCPU_AES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("AES DPAK Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AES DPAK Pass - Mode %d \n", i);
        }
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}


INT32 GCPU_EMU_AESEPAK()
{
    UINT8 u1Key[32], i, j;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AES_PARAM_T rAesParam;

    u4GoldenFileSize = 16;
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

    for (i = 0; i < 3; i++)
    {
        //Load Src
        _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
            pbSrcBufAddr + u4BufSize, au1PAKGloden, u4GoldenFileSize);

        if (i == 0)
        {
            //Load Golden - 128 bit
            x_memcpy(pbGoldenBufAddr, au1PAK128, u4GoldenFileSize);
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 1)
        {
            //Load Golden - 192 bit
            x_memcpy(pbGoldenBufAddr, au1PAK192, u4GoldenFileSize);
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else if (i == 2)
        {
            //Load Golden - 256 bit
            x_memcpy(pbGoldenBufAddr, au1PAK256, u4GoldenFileSize);
            rAesParam.eKeyBitLen = KEY_BIT_LEN_256;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = 16;

        for(j = 0; j < 32; j++)
        {
            rAesParam.au1Key[j] = j;
        }

        rAesParam.eMode = BLOCK_CIPHER_MODE_ECB;
        rAesParam.fgEncrypt = TRUE;

        GCPU_Cmd(0, GCPU_AES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("AES EPAK Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AES EPAK Pass - Mode %d \n", i);
        }
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}


static UINT8 au1CbcKey128[16] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

static UINT8 au1CbcKey192[24] = {0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

static UINT8 au1CbcKey256[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

static UINT8 au1CbcIv[16] = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};


INT32 GCPU_EMU_AESDCBC()
{
    UINT8 u1Key[32], i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AES_PARAM_T rAesParam;

    for (i=0; i<3; i++)
    {
        if (i == 0)
        {
            //128 Bit Mode
            u4GoldenFileSize = sizeof(au1aescbc_128_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aescbc_128_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aescbc_128_raw, sizeof(au1aescbc_128_raw));

            x_memcpy(rAesParam.au1Key, au1CbcKey128, sizeof(au1CbcKey128));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 1)
        {
            //192 Bit Mode
            u4GoldenFileSize = sizeof(au1aescbc_192_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aescbc_192_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aescbc_192_raw, sizeof(au1aescbc_192_raw));

            x_memcpy(rAesParam.au1Key, au1CbcKey192, sizeof(au1CbcKey192));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else if (i == 2)
        {
            //256 Bit Mode
            u4GoldenFileSize = sizeof(au1aescbc_256_raw);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aescbc_256_enc, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aescbc_256_raw, sizeof(au1aescbc_256_raw));

            x_memcpy(rAesParam.au1Key, au1CbcKey256, sizeof(au1CbcKey256));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_256;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_CBC;
        rAesParam.fgEncrypt = FALSE;

        GCPU_Cmd(0, GCPU_AES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("AES DCBC Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AES DCBC Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}

INT32 GCPU_EMU_AESECBC()
{
    UINT8 u1Key[32], i;
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    AES_PARAM_T rAesParam;

    for (i=0; i<3; i++)
    {
        if (i == 0)
        {
            //128 Bit Mode
            u4GoldenFileSize = sizeof(au1aescbc_128_enc);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aescbc_128_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aescbc_128_enc, sizeof(au1aescbc_128_enc));

            x_memcpy(rAesParam.au1Key, au1CbcKey128, sizeof(au1CbcKey128));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
        }
        else if (i == 1)
        {
            //192 Bit Mode
            u4GoldenFileSize = sizeof(au1aescbc_192_enc);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aescbc_192_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aescbc_192_enc, sizeof(au1aescbc_192_enc));

            x_memcpy(rAesParam.au1Key, au1CbcKey192, sizeof(au1CbcKey192));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_192;
        }
        else if (i == 2)
        {
            //256 Bit Mode
            u4GoldenFileSize = sizeof(au1aescbc_256_enc);
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
            _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
                pbSrcBufAddr + u4BufSize, au1aescbc_256_raw, u4GoldenFileSize);
            x_memcpy(pbGoldenBufAddr, au1aescbc_256_enc, sizeof(au1aescbc_256_enc));

            x_memcpy(rAesParam.au1Key, au1CbcKey256, sizeof(au1CbcKey256));
            x_memcpy(rAesParam.au1Iv, au1CbcIv, sizeof(au1CbcIv));
            rAesParam.eKeyBitLen = KEY_BIT_LEN_256;
        }

        rAesParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
        rAesParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
        rAesParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
        rAesParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
        rAesParam.u4DstBufStart = (UINT32) pbDstBufAddr;
        rAesParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
        rAesParam.u4DatLen  = u4GoldenFileSize;

        rAesParam.eMode = BLOCK_CIPHER_MODE_CBC;
        rAesParam.fgEncrypt = TRUE;

        GCPU_Cmd(0, GCPU_AES, &rAesParam);

        if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
            pbGoldenBufAddr + u4GoldenFileSize, rAesParam.u4DstStartAddr,
            rAesParam.u4DstBufStart, rAesParam.u4DstBufEnd, u4GoldenFileSize))
        {
            Printf("AES ECBC Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AES ECBC Pass - Mode %d \n", i);
        }

        VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
        VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));
    }

    return 0;
}


INT32 GCPU_EMU_AESD()
{
    SIM_AES_PARAM_T rAesParam;
    INT32 i;

    for(i = 0; i < 32; i++)
    {
        rAesParam.au1Key[i] = i;
    }

    x_memcpy(rAesParam.au1Dat, au1PAK128, sizeof(au1PAK128));

    rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
    rAesParam.u4SecureKeyPtr = 0;

    GCPU_Cmd(0, GCPU_AES_D, &rAesParam);

    if (0 != x_memcmp(au1PAKGloden, rAesParam.au1Res, sizeof(au1PAKGloden)))
    {
        Printf("AES D Fail \n", i);
        while(1);
    }
    else
    {
        Printf("AES D Pass \n", i);
    }

    return 0;
}


INT32 GCPU_EMU_AESE()
{
    SIM_AES_PARAM_T rAesParam;
    INT32 i;

    for(i = 0; i < 32; i++)
    {
        rAesParam.au1Key[i] = i;
    }

    x_memcpy(rAesParam.au1Dat, au1PAKGloden, sizeof(au1PAKGloden));

    rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
    rAesParam.u4SecureKeyPtr = 0;

    GCPU_Cmd(0, GCPU_AES_E, &rAesParam);

    if (0 != x_memcmp(au1PAK128, rAesParam.au1Res, sizeof(au1PAK128)))
    {
        Printf("AES E Fail \n", i);
        while(1);
    }
    else
    {
        Printf("AES E Pass \n", i);
    }

    return 0;
}

INT32 GCPU_EMU_AESG()
{
    SIM_AES_PARAM_T rAesParam;
    INT32 i;
    UINT8 au1XORGol[16];

    for(i = 0; i < 32; i++)
    {
        rAesParam.au1Key[i] = i;
    }

    x_memcpy(rAesParam.au1Dat, au1PAK128, sizeof(au1PAK128));

    for(i = 0; i < 16; i++)
    {
        au1XORGol[i] = au1PAKGloden[i] ^ rAesParam.au1Dat[i];
    }

    rAesParam.eKeyBitLen = KEY_BIT_LEN_128;
    rAesParam.u4SecureKeyPtr = 0;

    GCPU_Cmd(0, GCPU_AES_G, &rAesParam);

    if (0 != x_memcmp(au1XORGol, rAesParam.au1Res, sizeof(au1XORGol)))
    {
        Printf("AES G Fail \n", i);
        while(1);
    }
    else
    {
        Printf("AES G Pass \n", i);
    }

    return 0;
}

static UINT8 au1DCmpKey[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
static UINT8 au1DCmpDat0[16] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
                                0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
static UINT8 au1DCmpDat1[16] = {0xe2, 0xc8, 0x12, 0x12, 0x0e, 0x7a, 0x44, 0x00,
                               0xe7, 0x0c, 0xc2, 0x16, 0x93, 0x55, 0x7d, 0x5e};
static UINT8 au1DCmpDat2[16] = {0xdb, 0xb9, 0x12, 0x08, 0x38, 0xcd, 0x71, 0xce,
                               0x7f, 0xbe, 0x6f, 0x35, 0xac, 0x02, 0xd8, 0x24};
static UINT8 au1DCmpRes0[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
static UINT8 au1DCmpRes1[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static UINT8 au1DCmpRes2[16] = {0xde, 0xad, 0xbe, 0xef, 0x01, 0x23, 0x00, 0x01,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

INT32 GCPU_EMU_AESDCMP()
{
    AES_DCMP_PARAM_T rParam;
    INT32 i;
    UINT8 au1Gol[16];

    x_memcpy(rParam.au1Key, au1DCmpKey, sizeof(au1DCmpKey));

    for (i = 0; i < 3; i++)
    {
        //Data
        if (i == 0)
        {
            //Case 1
            x_memcpy(rParam.au1Dat, au1DCmpDat0, sizeof(au1DCmpDat0));
            x_memcpy(au1Gol, au1DCmpRes0, sizeof(au1DCmpRes0));
        }
        else if (i == 1)
        {
            //Case 2
            x_memcpy(rParam.au1Dat, au1DCmpDat1, sizeof(au1DCmpDat1));
            x_memcpy(au1Gol, au1DCmpRes1, sizeof(au1DCmpRes1));
        }
        else if (i == 2)
        {
            //Case 3
            x_memcpy(rParam.au1Dat, au1DCmpDat2, sizeof(au1DCmpDat2));
            x_memcpy(au1Gol, au1DCmpRes2, sizeof(au1DCmpRes2));
        }

        rParam.eKeyBitLen = KEY_BIT_LEN_128;
        GCPU_Cmd(0, GCPU_AES_D_CMP, &rParam);

        if (0 != x_memcmp(au1Gol, rParam.au1Res, sizeof(au1Gol)))
        {
            Printf("AES DCMP Fail - Mode %d \n", i);
            while(1);
        }
        else
        {
            Printf("AES DCMP Pass - Mode %d \n", i);
        }
    }

    return 0;
}

static UINT8 au1CmacKey[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

static UINT8 au1CmacData[20] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
                    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                    0x30, 0xc8, 0x1c, 0x46};

static UINT8 au1CmacRes[16] = {0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44,
                    0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c};

static UINT8 au1CmacRes20[16] = {0x67, 0x2b, 0x5f, 0x7a, 0xef, 0x24, 0x02, 0xb5,
                    0x9b, 0x8b, 0xea, 0x84, 0xae, 0x2a, 0x83, 0x86};

INT32 GCPU_EMU_AESCMAC()
{
    AES_EMU_PARAM_T rAesParam;
    UINT32 u4KeyPtr;
    UINT8 i, *p;

    x_memcpy(rAesParam.au1Key, au1CmacKey, sizeof(au1CmacKey));
    x_memcpy(rAesParam.au1Dat, au1CmacData, sizeof(rAesParam.au1Dat));

    u4KeyPtr = _GCPU_AllocSecureSlot(GCPU_PARAM_AES_MAC_KEY_NUM * 4);
    //key
    for(i = 0; i < 4; i++)
    {
        p = au1CmacKey + i * 4;
        _GCPU_Hw_SetSlot(u4KeyPtr + i, KEY_WRAPPER(p));
    }
    rAesParam.u4SecureKeyPtr = u4KeyPtr;
    rAesParam.eMode = BLOCK_CIPHER_EMU_MODE_CMAC;
    rAesParam.eCmacMode = AES_CMAC_ONE_PASS_COMPLETE_MODE;

    _GCPU_AES_Emu_Cmd(&rAesParam);

    if (0 != x_memcmp(au1CmacRes, rAesParam.au1Res, sizeof(rAesParam.au1Res)))
    {
        Printf("AES CMAC Fail \n", i);
        while(1);
    }
    else
    {
        Printf("AES CMAC Pass \n", i);
    }

    _GCPU_FreeSecureSlot(u4KeyPtr);

    return 0;
}

#if 0
INT32 GCPU_EMU_AESCMAC2()
{
    INT32 i;
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    AES_Param_DEG rAesDParam;
    UINT32 u4SSlotPtr;

    u4Param0[0] = 0x16157e2b; //0x2b7e1516;
    u4Param0[1] = 0xa6d2ae28; //0x28aed2a6;
    u4Param0[2] = 0x8815f7ab; //0xabf71588;
    u4Param0[3] = 0x3c4fcf09; //0x09cf4f3c;

    u4Param1[0] = 0xe2bec16b; //0x6bc1bee2;
    u4Param1[1] = 0x969f402e; //0x2e409f96;
    u4Param1[2] = 0x117e3de9; //0xe93d7e11;
    u4Param1[3] = 0x2a179373; //0x7393172a;
    u4Param1[4] = 0x461CC830;

    rAesDParam.pbKey = (BYTE*) &u4Param0;
    rAesDParam.pbData = (BYTE*) &u4Param1;
    rAesDParam.pbResult = (BYTE*) &u4Param1;

    //Set Secure Key
    _u1GCPU_HWRes_Obtain(0);
    _i4Gcpu_Hw_GetSecureSlot(4, &u4SSlotPtr);
    _i4Gcpu_Hw_SetSlotData(u4SSlotPtr, 4 * 4, (BYTE*)&u4Param0);    // SLOT * 4

    rAesDParam.u2KeySlotHandle = (UINT16)(u4SSlotPtr | GCPU_SSLOT_MARK);
    rAesDParam.u2DataSlotHandle = 4;
    rAesDParam.u2ResSlotHandle = 8;
    rAesDParam.uFlag = 1;
    rAesDParam.uKeyLen = 0;
    rAesDParam.u4DatLen = 20;

    HalFlushInvalidateDCache();

    _i4GCPU_AES_CmdHandle(0, GCPU_AES_CMAC, &rAesDParam);

    u4Param2[0] = 0x7A5F2B67;
    u4Param2[1] = 0xB50224EF;
    u4Param2[2] = 0x84EA8B9B;
    u4Param2[3] = 0x86832AAE;

    for (i=0; i<4; i++)
    {
        if (u4Param1[i] != u4Param2[i])
        {
            UTIL_Printf("AES CMAC Fail \n");
            while (1);
        }
    }
    UTIL_Printf("AES CMAC Pass \n");

    _i4Gcpu_Hw_FreeSecureSlot(4, u4SSlotPtr);

    return 0;
}

///TODO: some time failed
/*
INT32 GCPU_EMU_AESCMAC3()
{
    AES_EMU_PARAM_T rAesParam;
    UINT32 u4KeyPtr;
    UINT8 i, *p;

    x_memcpy(rAesParam.au1Key, au1CmacKey, sizeof(au1CmacKey));
    x_memcpy(rAesParam.au1Dat, au1CmacData, sizeof(rAesParam.au1Dat));

    u4KeyPtr = _GCPU_AllocSecureSlot(GCPU_PARAM_AES_MAC_KEY_NUM * 4);
    //key
    for(i = 0; i < 4; i++)
    {
        p = au1CmacKey + i * 4;
        _GCPU_Hw_SetSlot(u4KeyPtr + i, KEY_WRAPPER(p));
    }
    rAesParam.u4SecureKeyPtr = u4KeyPtr;
    rAesParam.eMode = BLOCK_CIPHER_EMU_MODE_CMAC;
    rAesParam.eCmacMode = AES_CMAC_START_MODE;

    _GCPU_AES_Emu_Cmd(&rAesParam);

    x_memset(rAesParam.au1Dat, 0, sizeof(rAesParam.au1Dat));
    x_memcpy(rAesParam.au1Dat, au1CmacData + 16, sizeof(au1CmacData) - 16);
    rAesParam.au1Dat[4] = 0x80;

    rAesParam.eCmacMode = AES_CMAC_FINALIZE_REMAINDER_MODE;
    _GCPU_AES_Emu_Cmd(&rAesParam);

    if (0 != x_memcmp(au1CmacRes20, rAesParam.au1Res, sizeof(rAesParam.au1Res)))
    {
        Printf("AES CMAC Fail \n", i);
        while(1);
    }
    else
    {
        Printf("AES CMAC Pass \n", i);
    }

    _GCPU_FreeSecureSlot(u4KeyPtr);

    return 0;
}
*/
INT32  GCPU_EMU_AESPKDPAK()
{
    INT32 i;
    INT32 j;
    UINT32 u4Param0[20];
    //UINT32 u4Param1[20];
    //UINT32 u4Param2[20];
    AESPK_Param_PAK rAesPAKParam;
    UINT32 u4SSlotPtr = 0;

    UINT32* pu4SrcBufAddr;
    UINT32* pu4DstBufAddr;
    UINT32* pu4GoldenBufAddr;

    BYTE *pbSrcBufAddr;
    BYTE *pbDstBufAddr;
    BYTE *pbGoldenBufAddr;

    pbSrcBufAddr = x_mem_alloc(0x7D000);
    pbDstBufAddr = x_mem_alloc(0x7D000);
    pbGoldenBufAddr = x_mem_alloc(0x7D000);

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) VIRTUAL((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) VIRTUAL((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) VIRTUAL((UINT32)pbGoldenBufAddr);
    #else //CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) NONCACHE((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) NONCACHE((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) NONCACHE((UINT32)pbGoldenBufAddr);
    #endif //CONFIG_DRV_LINUX_DATA_CONSISTENCY

    HalFlushInvalidateDCache();

    for (j=0; j<2; j++)
    {
        if (j == 0)
        {
            //Mode0 Src
            pu4SrcBufAddr[0] = 0x8a9270d4;
            pu4SrcBufAddr[1] = 0x96be9db6;
            pu4SrcBufAddr[2] = 0x8c38c9c5;
            pu4SrcBufAddr[3] = 0x9832fd1f;
        }
        else if (j == 1)
        {
            //Mode1 Src
            pu4SrcBufAddr[0] = 0x73fd3758;
            pu4SrcBufAddr[1] = 0x4a2159f6;
            pu4SrcBufAddr[2] = 0x533f943d;
            pu4SrcBufAddr[3] = 0x963b9378;
        }

        //Mode0 Golden
        pu4GoldenBufAddr[0] = 0x33221100;
        pu4GoldenBufAddr[1] = 0x77665544;
        pu4GoldenBufAddr[2] = 0xbbaa9988;
        pu4GoldenBufAddr[3] = 0xffeeddcc;

         //Key
         u4Param0[0] = 0x43424140; //0x40414243;
         u4Param0[1] = 0x47464544; //0x44454647;
         u4Param0[2] = 0x4b4a4948; //0x48494a4b;
         u4Param0[3] = 0x4f4e4d4c; //0x4c4d4e4f;

         if (j == 1)
         {
             _u1GCPU_HWRes_Obtain(0);
             _i4Gcpu_Hw_GetSecureSlot(4, &u4SSlotPtr);
             _i4Gcpu_Hw_SetSlotData(u4SSlotPtr, 4 * 4, (BYTE*)&u4Param0);
         }

         rAesPAKParam.u4SrcSa = (UINT32) pu4SrcBufAddr;
         rAesPAKParam.u4DstSa = (UINT32) pu4DstBufAddr;
         rAesPAKParam.u4DatLen = 16;
         rAesPAKParam.pbKey = (BYTE*) &u4Param0;

         if (j == 0)
         {
             rAesPAKParam.u2KeySlotHandle = 0;
             rAesPAKParam.uFlag = 0;
         }
         else if (j == 1)
         {
             rAesPAKParam.u2KeySlotHandle = u4SSlotPtr | GCPU_SSLOT_MARK;
             rAesPAKParam.uFlag = 1;
         }

         rAesPAKParam.uMode = j;

        _i4GCPU_AES_CmdHandle(0, GCPU_AESPK_DPAK, &rAesPAKParam);

        for (i=0; i<4; i++)
        {
            if (pu4DstBufAddr[i] != pu4GoldenBufAddr[i])
            {
                UTIL_Printf("AES PKDPAK Fail - Mode %d \n", j);
                while (1);
            }
        }
        UTIL_Printf("AES PKDPAK Pass - Mode %d \n", j);

         if (j == 1)
         {
             _i4Gcpu_Hw_FreeSecureSlot(4, u4SSlotPtr);
         }
    }

    x_mem_free(pbSrcBufAddr);
    x_mem_free(pbDstBufAddr);
    x_mem_free(pbGoldenBufAddr);

    return 0;
}


INT32 GCPU_EMU_AESPKEPAK()
{
    INT32 i;
    INT32 j;
    UINT32 u4Param0[20];
    //UINT32 u4Param1[20];
    //UINT32 u4Param2[20];
    AESPK_Param_PAK rAesPAKParam;
    UINT32 u4SSlotPtr = 0;

    UINT32* pu4SrcBufAddr;
    UINT32* pu4DstBufAddr;
    UINT32* pu4GoldenBufAddr;

    BYTE *pbSrcBufAddr;
    BYTE *pbDstBufAddr;
    BYTE *pbGoldenBufAddr;

    pbSrcBufAddr = x_mem_alloc(0x7D000);
    pbDstBufAddr = x_mem_alloc(0x7D000);
    pbGoldenBufAddr = x_mem_alloc(0x7D000);

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) VIRTUAL((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) VIRTUAL((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) VIRTUAL((UINT32)pbGoldenBufAddr);
    #else //CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) NONCACHE((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) NONCACHE((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) NONCACHE((UINT32)pbGoldenBufAddr);
    #endif //CONFIG_DRV_LINUX_DATA_CONSISTENCY

    HalFlushInvalidateDCache();

    for (j=0; j<2; j++)
    {
        if (j == 0)
        {
            //Mode0 Golden
            pu4GoldenBufAddr[0] = 0x8a9270d4;
            pu4GoldenBufAddr[1] = 0x96be9db6;
            pu4GoldenBufAddr[2] = 0x8c38c9c5;
            pu4GoldenBufAddr[3] = 0x9832fd1f;
        }
        else if (j == 1)
        {
            //Mode1 Golden
            pu4GoldenBufAddr[0] = 0x73fd3758;
            pu4GoldenBufAddr[1] = 0x4a2159f6;
            pu4GoldenBufAddr[2] = 0x533f943d;
            pu4GoldenBufAddr[3] = 0x963b9378;
        }

        //Mode0 Src
        pu4SrcBufAddr[0] = 0x33221100;
        pu4SrcBufAddr[1] = 0x77665544;
        pu4SrcBufAddr[2] = 0xbbaa9988;
        pu4SrcBufAddr[3] = 0xffeeddcc;

         //Key
         u4Param0[0] = 0x43424140; //0x40414243;
         u4Param0[1] = 0x47464544; //0x44454647;
         u4Param0[2] = 0x4b4a4948; //0x48494a4b;
         u4Param0[3] = 0x4f4e4d4c; //0x4c4d4e4f;

         if (j == 1)
         {
             _u1GCPU_HWRes_Obtain(0);
             _i4Gcpu_Hw_GetSecureSlot(4, &u4SSlotPtr);
             _i4Gcpu_Hw_SetSlotData(u4SSlotPtr, 4 * 4, (BYTE*)&u4Param0);
         }

         rAesPAKParam.u4SrcSa = (UINT32) pu4SrcBufAddr;
         rAesPAKParam.u4DstSa = (UINT32) pu4DstBufAddr;
         rAesPAKParam.u4DatLen = 16;
         rAesPAKParam.pbKey = (BYTE*) &u4Param0;

         if (j == 0)
         {
             rAesPAKParam.u2KeySlotHandle = 0;
             rAesPAKParam.uFlag = 0;
         }
         else if (j == 1)
         {
             rAesPAKParam.u2KeySlotHandle = u4SSlotPtr | GCPU_SSLOT_MARK;
             rAesPAKParam.uFlag = 1;
         }

         rAesPAKParam.uMode = j;

        _i4GCPU_AES_CmdHandle(0, GCPU_AESPK_EPAK, &rAesPAKParam);

        for (i=0; i<4; i++)
        {
            if (pu4DstBufAddr[i] != pu4GoldenBufAddr[i])
            {
                UTIL_Printf("AES PKEPAK Fail - Mode %d \n", j);
                while (1);
            }
        }
        UTIL_Printf("AES PKEPAK Pass - Mode %d \n", j);

         if (j == 1)
         {
             _i4Gcpu_Hw_FreeSecureSlot(4, u4SSlotPtr);
         }
    }

    x_mem_free(pbSrcBufAddr);
    x_mem_free(pbDstBufAddr);
    x_mem_free(pbGoldenBufAddr);

    return 0;
}

INT32 GCPU_EMU_AESPKDCBC()
{
    UINT8 i;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];
    UINT32 u4Mode;

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    UINT32* pu4SrcBufAddr;
    UINT32* pu4DstBufAddr;
    UINT32* pu4GoldenBufAddr;

    BYTE *pbSrcBufAddr;
    BYTE *pbDstBufAddr;
    BYTE *pbGoldenBufAddr;

    pbSrcBufAddr = x_mem_alloc(0x7D000);
    pbDstBufAddr = x_mem_alloc(0x7D000);
    pbGoldenBufAddr = x_mem_alloc(0x7D000);

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) VIRTUAL((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) VIRTUAL((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) VIRTUAL((UINT32)pbGoldenBufAddr);
    #else //CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) NONCACHE((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) NONCACHE((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) NONCACHE((UINT32)pbGoldenBufAddr);
    #endif //CONFIG_DRV_LINUX_DATA_CONSISTENCY

    HalFlushInvalidateDCache();

    for (i=0; i<2; i++)
    {
        if (i == 0)
        {
            //Mode0 Src
            pu4SrcBufAddr[0] = 0x8591c4dd;
            pu4SrcBufAddr[1] = 0xb753483c;
            pu4SrcBufAddr[2] = 0x29b32158;
            pu4SrcBufAddr[3] = 0x2739daec;
            pu4SrcBufAddr[4] = 0xace9289d;
            pu4SrcBufAddr[5] = 0x94b6f33d;
            pu4SrcBufAddr[6] = 0x3b9a34bc;
            pu4SrcBufAddr[7] = 0x0ca75c64;
        }
        else if (i == 1)
        {
            //Mode1 Src
            pu4SrcBufAddr[0] = 0xd6d16fd8;
            pu4SrcBufAddr[1] = 0xef4adae6;
            pu4SrcBufAddr[2] = 0x0eca0daa;
            pu4SrcBufAddr[3] = 0x0fc2f3b6;
            pu4SrcBufAddr[4] = 0xf8db5205;
            pu4SrcBufAddr[5] = 0x9fe29577;
            pu4SrcBufAddr[6] = 0x6d65b877;
            pu4SrcBufAddr[7] = 0xc782bad6;
        }

        if (i == 0)
        {
            //Mode0 Golden
            pu4GoldenBufAddr[0] = 0x3dcd8d1e;
            pu4GoldenBufAddr[1] = 0x76f1233b;
            pu4GoldenBufAddr[2] = 0x0dcdd706;
            pu4GoldenBufAddr[3] = 0x01e3f300;
            pu4GoldenBufAddr[4] = 0xc62437e3;
            pu4GoldenBufAddr[5] = 0xf9e9fd7c;
            pu4GoldenBufAddr[6] = 0x8cf72d46;
            pu4GoldenBufAddr[7] = 0xed57d476;
        }
        else if (i == 1)
        {
            //Mode1 Golden
            pu4GoldenBufAddr[0] = 0x7772d2bb;
            pu4GoldenBufAddr[1] = 0xe5f7af72;
            pu4GoldenBufAddr[2] = 0xaad213d5;
            pu4GoldenBufAddr[3] = 0xc584f7e2;
            pu4GoldenBufAddr[4] = 0xce9676e7;
            pu4GoldenBufAddr[5] = 0xf2693079;
            pu4GoldenBufAddr[6] = 0x8fdbec47;
            pu4GoldenBufAddr[7] = 0x12d63289;
        }

        //Key
        u4Param0[0] = 0x43424140; //0x40414243;
        u4Param0[1] = 0x47464544; //0x44454647;
        u4Param0[2] = 0x4b4a4948; //0x48494a4b;
        u4Param0[3] = 0x4f4e4d4c; //0x4c4d4e4f;

        //IV
        u4Param1[0] = 0x98badcfe; //0xfedcba98;
        u4Param1[1] = 0x10325476; //0x76543210;
        u4Param1[2] = 0x33221100; //0x00112233;
        u4Param1[3] = 0x77665544; //0x44556677;

        if (i == 0)
        {
            //XORDAT    //Mode0
            u4Param2[0] = 0x9d28e9ac; //0xace9289d;
            u4Param2[1] = 0x3df3b694; //0x94b6f33d;
            u4Param2[2] = 0xbc349a3b; //0x3b9a34bc;
            u4Param2[3] = 0x645ca70c; //0x0ca75c64;
        }
        else if (i == 1)
        {
            //XORDAT    //Mode1
            u4Param2[0] = 0x0552dbf8; //0xf8db5205;
            u4Param2[1] = 0x7795e29f; //0x9fe29577;
            u4Param2[2] = 0x77b8656d; //0x6d65b877;
            u4Param2[3] = 0xd6ba82c7; //0xc782bad6;
        }

        u4CmdPtr[0] = 50;
        u4CmdPtr[1] = 7;
        u4CmdPtr[2] = 11;

        u4Mode = i;

        prGcpuCmd->ucDevId = u1DevId;
        prGcpuCmd->u4Mode = u4Mode;

        x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
        x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
        x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);

        prGcpuCmd->u4SrcBufAddr = (UINT32) PHYSICAL((UINT32) pbSrcBufAddr);
        prGcpuCmd->u4DstBufAddr = (UINT32) PHYSICAL((UINT32) pbDstBufAddr);
        prGcpuCmd->u4PacketNum = 2;
        prGcpuCmd->u4GcpuCmdMode = AESPK_DCBC;
        prGcpuCmd->fgNeedSetData = TRUE;
        i4GCPU_HAL_SetKeyProtect_Cmd(prGcpuCmd);
        _vGCPU_WaitCmdExec(0);

        HalFlushInvalidateDCache();

        if (TRUE == fgGcpuCompare(pbDstBufAddr, pbGoldenBufAddr, 32))
        {
            UTIL_Printf("AES PKDCBC Pass - Mode %d \n", i);
        }
        else
        {
            UTIL_Printf("AES PKDCBC Fail - Mode %d \n", i);
            while(1);
        }
    }

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    x_mem_free(pbSrcBufAddr);
    x_mem_free(pbDstBufAddr);
    x_mem_free(pbGoldenBufAddr);

    return 0;
}

INT32 GCPU_EMU_AESPKECBC()
{
    UINT8 i;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];
    UINT32 u4Mode;

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    UINT32* pu4SrcBufAddr;
    UINT32* pu4DstBufAddr;
    UINT32* pu4GoldenBufAddr;

    BYTE *pbSrcBufAddr;
    BYTE *pbDstBufAddr;
    BYTE *pbGoldenBufAddr;

    pbSrcBufAddr = x_mem_alloc(0x7D000);
    pbDstBufAddr = x_mem_alloc(0x7D000);
    pbGoldenBufAddr = x_mem_alloc(0x7D000);

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) VIRTUAL((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) VIRTUAL((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) VIRTUAL((UINT32)pbGoldenBufAddr);
    #else //CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) NONCACHE((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) NONCACHE((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) NONCACHE((UINT32)pbGoldenBufAddr);
    #endif //CONFIG_DRV_LINUX_DATA_CONSISTENCY

    for (i=0; i<2; i++)
    {
        if (i == 0)
        {
            //Mode0 Golden
            pu4GoldenBufAddr[0] = 0x8591c4dd;
            pu4GoldenBufAddr[1] = 0xb753483c;
            pu4GoldenBufAddr[2] = 0x29b32158;
            pu4GoldenBufAddr[3] = 0x2739daec;
            pu4GoldenBufAddr[4] = 0xace9289d;
            pu4GoldenBufAddr[5] = 0x94b6f33d;
            pu4GoldenBufAddr[6] = 0x3b9a34bc;
            pu4GoldenBufAddr[7] = 0x0ca75c64;
        }
        else if (i == 1)
        {
            //Mode1 Golden
            pu4GoldenBufAddr[0] = 0xd6d16fd8;
            pu4GoldenBufAddr[1] = 0xef4adae6;
            pu4GoldenBufAddr[2] = 0x0eca0daa;
            pu4GoldenBufAddr[3] = 0x0fc2f3b6;
            pu4GoldenBufAddr[4] = 0xf8db5205;
            pu4GoldenBufAddr[5] = 0x9fe29577;
            pu4GoldenBufAddr[6] = 0x6d65b877;
            pu4GoldenBufAddr[7] = 0xc782bad6;
        }

        if (i == 0)
        {
            //Mode0 Src
            pu4SrcBufAddr[0] = 0x3dcd8d1e;
            pu4SrcBufAddr[1] = 0x76f1233b;
            pu4SrcBufAddr[2] = 0x0dcdd706;
            pu4SrcBufAddr[3] = 0x01e3f300;
            pu4SrcBufAddr[4] = 0xc62437e3;
            pu4SrcBufAddr[5] = 0xf9e9fd7c;
            pu4SrcBufAddr[6] = 0x8cf72d46;
            pu4SrcBufAddr[7] = 0xed57d476;
        }
        else if (i == 1)
        {
            //Mode1 Src
            pu4SrcBufAddr[0] = 0x7772d2bb;
            pu4SrcBufAddr[1] = 0xe5f7af72;
            pu4SrcBufAddr[2] = 0xaad213d5;
            pu4SrcBufAddr[3] = 0xc584f7e2;
            pu4SrcBufAddr[4] = 0xce9676e7;
            pu4SrcBufAddr[5] = 0xf2693079;
            pu4SrcBufAddr[6] = 0x8fdbec47;
            pu4SrcBufAddr[7] = 0x12d63289;
        }

        //Key
        u4Param0[0] = 0x43424140; //0x40414243;
        u4Param0[1] = 0x47464544; //0x44454647;
        u4Param0[2] = 0x4b4a4948; //0x48494a4b;
        u4Param0[3] = 0x4f4e4d4c; //0x4c4d4e4f;

        //IV
        u4Param1[0] = 0x98badcfe; //0xfedcba98;
        u4Param1[1] = 0x10325476; //0x76543210;
        u4Param1[2] = 0x33221100; //0x00112233;
        u4Param1[3] = 0x77665544; //0x44556677;

        if (i == 0)
        {
            //XORDAT    //Mode0
            u4Param2[0] = 0x9d28e9ac; //0xace9289d;
            u4Param2[1] = 0x3df3b694; //0x94b6f33d;
            u4Param2[2] = 0xbc340a3b; //0x3b9a34bc;
            u4Param2[3] = 0x645ca70c; //0x0ca75c64;
        }
        else if (i == 1)
        {
            //XORDAT    //Mode1
            u4Param2[4] = 0x0552dbf8; //0xf8db5205;
            u4Param2[5] = 0x7795e29f; //0x9fe29577;
            u4Param2[6] = 0x77b8656d; //0x6d65b877;
            u4Param2[7] = 0xd6ba82c7; //0xc782bad6;
        }

        u4CmdPtr[0] = 50;
        u4CmdPtr[1] = 7;
        u4CmdPtr[2] = 11;

        u4Mode = i;

        prGcpuCmd->ucDevId = u1DevId;
        prGcpuCmd->u4Mode = u4Mode;

        x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
        x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
        x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);

        HalFlushInvalidateDCache();

        prGcpuCmd->u4SrcBufAddr = (UINT32) PHYSICAL((UINT32) pbSrcBufAddr);
        prGcpuCmd->u4DstBufAddr = (UINT32) PHYSICAL((UINT32) pbDstBufAddr);
        prGcpuCmd->u4PacketNum = 2;
        prGcpuCmd->u4GcpuCmdMode = AESPK_ECBC;
        prGcpuCmd->fgNeedSetData = TRUE;
        i4GCPU_HAL_SetKeyProtect_Cmd(prGcpuCmd);
        _vGCPU_WaitCmdExec(0);

        HalFlushInvalidateDCache();

        if (TRUE == fgGcpuCompare(pbDstBufAddr, pbGoldenBufAddr, 32))
        {
            UTIL_Printf("AES PKECBC Pass - Mode %d \n", i);
        }
        else
        {
            UTIL_Printf("AES PKECBC Fail - Mode %d \n", i);
            while(1);
        }
    }

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    x_mem_free(pbSrcBufAddr);
    x_mem_free(pbDstBufAddr);
    x_mem_free(pbGoldenBufAddr);

    return 0;
}

INT32 GCPU_EMU_AESPKD()
{
    UINT8 i, j;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;
    UINT32 u4Mode;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    for (j=0; j<2; j++)
    {
        //Key
        u4Param0[0] = 0x43424140; //0x40414243;
        u4Param0[1] = 0x47464544; //0x44454647;
        u4Param0[2] = 0x4B4A4948; //0x48494a4b;
        u4Param0[3] = 0x4F4E4D4C; //0x4c4d4e4f;

        if (j == 0)
        {
            //Data    //Mode0
            u4Param1[0] = 0x8A9270D4;
            u4Param1[1] = 0x96BE9DB6;
            u4Param1[2] = 0x8C38C9C5;
            u4Param1[3] = 0x9832FD1F;
        }
        else if (j == 1)
        {
            //Data    //Mode1
            u4Param1[0] = 0x73FD3758; //0x5837fd73;
            u4Param1[1] = 0x4A2159F6; //0xf659214a;
            u4Param1[2] = 0x533F943D; //0x3d943f53;
            u4Param1[3] = 0x963B9378; //0x78933b96;
        }

        u4Mode = j;

        u4CmdPtr[0] = 50;
        u4CmdPtr[1] = 54;
        u4CmdPtr[2] = 58;

        prGcpuCmd->ucDevId = u1DevId;
        prGcpuCmd->u4Mode = u4Mode;

        x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
        x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
        x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
        prGcpuCmd->u4GcpuCmdMode = AESPK_D;
        prGcpuCmd->fgNeedSetData = TRUE;
        i4GCPU_HAL_SetKeyProtect_Cmd(prGcpuCmd);
        _vGCPU_WaitCmdExec(0);


        u4Mode = 0;

        u4CmdPtr[0] = 58;
        u4CmdPtr[1] = 4;
        u4CmdPtr[2] = 8;

        u4Param1[0] = 0x03020100; //0x00010203;
        u4Param1[1] = 0x07060504; //0x04050607;
        u4Param1[2] = 0x0B0A0908; //0x08090a0b;
        u4Param1[3] = 0x0F0E0D0C; //0x0c0d0e0f;

        prGcpuCmd->ucDevId = u1DevId;
        x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
        x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
        x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
        prGcpuCmd->u4Mode = u4Mode;
        prGcpuCmd->u4GcpuCmdMode = AES_E;
        prGcpuCmd->fgNeedSetData = TRUE;
        prGcpuCmd->fgSkipParam0Set = TRUE;
        i4GCPU_HAL_SetAES_Cmd(prGcpuCmd);
        _vGCPU_WaitCmdExec(0);

        prGcpuCmd->u4GcpuCmdMode = AES_E;
        i4GCPU_HAL_GetAES_Result(prGcpuCmd);
        x_memcpy(u4Result, prGcpuCmd->u4Result, 80);

        u4Param2[0] = 0x4AB79F27;
        u4Param2[1] = 0x5E137275;
        u4Param2[2] = 0xF68E9B8F;
        u4Param2[3] = 0x03E0EED1;

        for (i=0; i<4; i++)
        {
            if (u4Result[i] != u4Param2[i])
            {
                UTIL_Printf("AES PKD Fail - Mode %d \n", j);
                while (1);
            }
        }
        UTIL_Printf("AES PKD Pass - Mode %d \n", j);
    }

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    return 0;
}

INT32 GCPU_EMU_AESPKE()
{
    UINT8 i, j;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;
    UINT32 u4Mode;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    for (j=0; j<2; j++)
    {
        //Key
        u4Param0[0] = 0x43424140; //0x40414243;
        u4Param0[1] = 0x47464544; //0x44454647;
        u4Param0[2] = 0x4B4A4948; //0x48494a4b;
        u4Param0[3] = 0x4F4E4D4C; //0x4c4d4e4f;

        //Data    //Mode1
        u4Param1[0] = 0x33221100; //0x00112233;
        u4Param1[1] = 0x77665544; //0x44556677;
        u4Param1[2] = 0xBBAA9988; //0x8899aabb;
        u4Param1[3] = 0xFFEEDDCC; //0xccddeeff;

        u4CmdPtr[0] = 50;
        u4CmdPtr[1] = 54;
        u4CmdPtr[2] = 58;

        u4Mode = j;

        prGcpuCmd->ucDevId = u1DevId;
        prGcpuCmd->u4Mode = u4Mode;

        x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
        x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
        x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
        prGcpuCmd->u4GcpuCmdMode = AESPK_E;
        prGcpuCmd->fgNeedSetData = TRUE;
        i4GCPU_HAL_SetKeyProtect_Cmd(prGcpuCmd);
        _vGCPU_WaitCmdExec(0);

        u4Mode = 0;

        u4CmdPtr[0] = 58;
        u4CmdPtr[1] = 4;
        u4CmdPtr[2] = 8;

        u4Param1[0] = 0x03020100; //0x00010203;
        u4Param1[1] = 0x07060504; //0x04050607;
        u4Param1[2] = 0x0B0A0908; //0x08090a0b;
        u4Param1[3] = 0x0F0E0D0C; //0x0c0d0e0f;

        prGcpuCmd->ucDevId = u1DevId;
        x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
        x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
        x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
        prGcpuCmd->u4Mode = u4Mode;
        prGcpuCmd->u4GcpuCmdMode = AES_E;
        prGcpuCmd->fgNeedSetData = TRUE;
        prGcpuCmd->fgSkipParam0Set = TRUE;
        i4GCPU_HAL_SetAES_Cmd(prGcpuCmd);
        _vGCPU_WaitCmdExec(0);

        prGcpuCmd->u4GcpuCmdMode = AES_E;
        i4GCPU_HAL_GetAES_Result(prGcpuCmd);
        x_memcpy(u4Result, prGcpuCmd->u4Result, 80);

        if (j == 0)
        {
            //Result    //Mode0
            u4Param2[0] = 0xE605D271; //0x71d205e6;
            u4Param2[1] = 0x4083E83C; //0x3ce88340;
            u4Param2[2] = 0xEEF3A86F; //0x6fa8f3ee;
            u4Param2[3] = 0x2E640D52; //0x520d642e;
        }
        else
        {
            //Result    //Mode1
            u4Param2[0] = 0x818DB811; //0x11b88d81;
            u4Param2[1] = 0xEA144CF8; //0xf84c14ea;
            u4Param2[2] = 0x065FD01D; //0x1dd05f06;
            u4Param2[3] = 0xD2D15D8B; //0x8b5dd1d2;
        }

        for (i=0; i<4; i++)
        {
            if (u4Result[i] != u4Param2[i])
            {
                UTIL_Printf("AES PKE Fail - Mode %d \n", j);
                while (1);
            }
        }
        UTIL_Printf("AES PKE Pass - Mode %d \n", j);
    }

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    return 0;
}


INT32 GCPU_EMU_AESEKD()
{
    UINT8 i;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    u4Param0[0] = 0x03020100;
    u4Param0[1] = 0x07060504;
    u4Param0[2] = 0x0b0a0908;
    u4Param0[3] = 0x0f0e0d0c;

    u4Param1[0] = 0xd8e0c469;
    u4Param1[1] = 0x30047b6a;
    u4Param1[2] = 0x80b7cdd8;
    u4Param1[3] = 0x5ac5b470;

    u4Param2[0] = 0x01234567;
    u4Param2[1] = 0x89abcdef;
    u4Param2[2] = 0x76543210;
    u4Param2[3] = 0xfedcba98;

    u4CmdPtr[0] = 4;
    u4CmdPtr[1] = 50;
    u4CmdPtr[2] = 54;
    u4CmdPtr[3] = 58;

    x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
    x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
    x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
    x_memcpy(prGcpuCmd->u4Param2, u4Param2, 80);
    prGcpuCmd->u4GcpuCmdMode = AESEK_D;
    prGcpuCmd->fgNeedSetData = TRUE;

    i4GCPU_HAL_SetAES_Cmd(prGcpuCmd);
    _vGCPU_WaitCmdExec(0);

    //Step3
    u4CmdPtr[0] = 58;
    u4CmdPtr[1] = 8;
    u4CmdPtr[2] = 12;

    //Check Data
    u4Param2[0] = 0x00224466;
    u4Param2[1] = 0x11335577;
    u4Param2[2] = 0x88AACCEE;
    u4Param2[3] = 0x99BBDDFF;

    x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
    x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
    x_memcpy(prGcpuCmd->u4Param1, u4Param2, 80);
    prGcpuCmd->u4GcpuCmdMode = AES_E;
    prGcpuCmd->u4Mode = 0;
    prGcpuCmd->fgNeedSetData = TRUE;

    prGcpuCmd->fgSkipParam0Set = TRUE;

    i4GCPU_HAL_SetAES_Cmd(prGcpuCmd);
    _vGCPU_WaitCmdExec(0);

    prGcpuCmd->u4GcpuCmdMode = AES_E;
    i4GCPU_HAL_GetAES_Result(prGcpuCmd);
    x_memcpy(u4Result, prGcpuCmd->u4Result, 80);

    u4Param3[0] = 0xF5687F88;
    u4Param3[1] = 0xB308BDCB;
    u4Param3[2] = 0x72A5960C;
    u4Param3[3] = 0xF5B52ACE;

    for (i=0; i<4; i++)
    {
        if (u4Param3[i] != u4Result[i])
        {
            UTIL_Printf("AES EKD Fail \n");
            while (1);
        }
    }
    UTIL_Printf("AES EKD Pass \n");

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    return 0;
}

INT32 GCPU_EMU_AESEKE()
{
    UINT8 i;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    u4Param0[0] = 0x03020100;
    u4Param0[1] = 0x07060504;
    u4Param0[2] = 0x0b0a0908;
    u4Param0[3] = 0x0f0e0d0c;

    u4Param1[0] = 0xd8e0c469;
    u4Param1[1] = 0x30047b6a;
    u4Param1[2] = 0x80b7cdd8;
    u4Param1[3] = 0x5ac5b470;

    u4Param2[0] = 0x01234567;
    u4Param2[1] = 0x89abcdef;
    u4Param2[2] = 0x76543210;
    u4Param2[3] = 0xfedcba98;

    u4CmdPtr[0] = 4;
    u4CmdPtr[1] = 50;
    u4CmdPtr[2] = 54;
    u4CmdPtr[3] = 58;

    x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
    x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
    x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
    x_memcpy(prGcpuCmd->u4Param2, u4Param2, 80);
    prGcpuCmd->u4GcpuCmdMode = AESEK_E;
    prGcpuCmd->fgNeedSetData = TRUE;

    i4GCPU_HAL_SetAES_Cmd(prGcpuCmd);
    _vGCPU_WaitCmdExec(0);

    //Step3
    u4CmdPtr[0] = 58;
    u4CmdPtr[1] = 8;
    u4CmdPtr[2] = 12;

    //Check Data
    u4Param2[0] = 0x00224466;
    u4Param2[1] = 0x11335577;
    u4Param2[2] = 0x88AACCEE;
    u4Param2[3] = 0x99BBDDFF;

    x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
    x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
    x_memcpy(prGcpuCmd->u4Param1, u4Param2, 80);
    prGcpuCmd->u4GcpuCmdMode = AES_E;
    prGcpuCmd->u4Mode = 0;
    prGcpuCmd->fgNeedSetData = TRUE;
    prGcpuCmd->fgSkipParam0Set = TRUE;
    i4GCPU_HAL_SetAES_Cmd(prGcpuCmd);
    _vGCPU_WaitCmdExec(0);

    prGcpuCmd->u4GcpuCmdMode = AES_E;
    i4GCPU_HAL_GetAES_Result(prGcpuCmd);
    x_memcpy(u4Result, prGcpuCmd->u4Result, 80);

    u4Param3[0] = 0x350D30AC;
    u4Param3[1] = 0xAD0B47EE;
    u4Param3[2] = 0x4FB2E969;
    u4Param3[3] = 0x612C9412;

    for (i=0; i<4; i++)
    {
        if (u4Param3[i] != u4Result[i])
        {
            UTIL_Printf("AES EKE Fail \n");
            while (1);
        }
    }
    UTIL_Printf("AES EKE Pass \n");

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    return 0;
}

INT32 GCPU_EMU_AESPKEKD()
{
    UINT8 i;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    u4CmdPtr[0] = 4;
    u4CmdPtr[1] = 8;
    u4CmdPtr[2] = 12;
    u4CmdPtr[3] = 16;

    //KeyA
    u4Param0[0] = 0x34239491;
    u4Param0[1] = 0xCDC5847C;
    u4Param0[2] = 0x43DCEBD2;
    u4Param0[3] = 0x133B046E;

    //Enc KeyB
    u4Param1[0] = 0x9A3DFF14;
    u4Param1[1] = 0x76230060;
    u4Param1[2] = 0xCB247F5B;
    u4Param1[3] = 0x1E3A5625;

    //Data
    u4Param2[0] = 0x8A9270D4;
    u4Param2[1] = 0x96BE9DB6;
    u4Param2[2] = 0x8C38C9C5;
    u4Param2[3] = 0x9832FD1F;

    prGcpuCmd->ucDevId = u1DevId;
    x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
    x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
    x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
    x_memcpy(prGcpuCmd->u4Param2, u4Param2, 80);
    prGcpuCmd->u4GcpuCmdMode = AESPK_EK_D;
    prGcpuCmd->fgNeedSetData = TRUE;
    i4GCPU_HAL_SetKeyProtect_Cmd(prGcpuCmd);
    _vGCPU_WaitCmdExec(0);

    prGcpuCmd->u4GcpuCmdMode = AESPK_EK_D;
    i4GCPU_HAL_GetKeyProtect_Result(prGcpuCmd);
    x_memcpy(u4Result, prGcpuCmd->u4Result, 80);

    //Result
    u4Param3[0] = 0x33221100;
    u4Param3[1] = 0x77665544;
    u4Param3[2] = 0xBBAA9988;
    u4Param3[3] = 0xFFEEDDCC;

    //Check Golden
    for (i=0; i<4; i++)
    {
        if (u4Result[i] != u4Param3[i])
        {
            UTIL_Printf("AESPK_EK_D Fail \n");
            while (1);
        }
    }

    UTIL_Printf("AESPK_EK_D Pass \n");

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    return 0;
}

INT32 GCPU_EMU_AESPKEKE()
{
    UINT8 i;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    u4CmdPtr[0] = 4;
    u4CmdPtr[1] = 8;
    u4CmdPtr[2] = 12;
    u4CmdPtr[3] = 16;

    //KeyA
    u4Param0[0] = 0x34239491;
    u4Param0[1] = 0xCDC5847C;
    u4Param0[2] = 0x43DCEBD2;
    u4Param0[3] = 0x133B046E;

    //Enc KeyB
    u4Param1[0] = 0x9A3DFF14;
    u4Param1[1] = 0x76230060;
    u4Param1[2] = 0xCB247F5B;
    u4Param1[3] = 0x1E3A5625;

    //Data
    u4Param2[0] = 0x33221100;
    u4Param2[1] = 0x77665544;
    u4Param2[2] = 0xBBAA9988;
    u4Param2[3] = 0xFFEEDDCC;

    prGcpuCmd->ucDevId = u1DevId;
    x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
    x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
    x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
    x_memcpy(prGcpuCmd->u4Param2, u4Param2, 80);
    prGcpuCmd->u4GcpuCmdMode = AESPK_EK_E;
    prGcpuCmd->fgNeedSetData = TRUE;
    i4GCPU_HAL_SetKeyProtect_Cmd(prGcpuCmd);
    _vGCPU_WaitCmdExec(0);

    prGcpuCmd->u4GcpuCmdMode = AESPK_EK_E;
    i4GCPU_HAL_GetKeyProtect_Result(prGcpuCmd);
    x_memcpy(u4Result, prGcpuCmd->u4Result, 80);

    //Result
    u4Param3[0] = 0x8A9270D4;
    u4Param3[1] = 0x96BE9DB6;
    u4Param3[2] = 0x8C38C9C5;
    u4Param3[3] = 0x9832FD1F;

    //Check Golden
    for (i=0; i<4; i++)
    {
        if (u4Result[i] != u4Param3[i])
        {
            UTIL_Printf("AESPK_EK_E Fail \n");
            while (1);
        }
    }

    UTIL_Printf("AESPK_EK_E Pass \n");

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    return 0;
}

INT32 GCPU_EMU_AESPKEKDCBC()
{
    UINT8 i;
    GCPU_HAL_CMD_T* prGcpuCmd;
    UCHAR u1DevId = 0x0;

    UINT32 u4CmdPtr[20];
    UINT32 u4Param0[20];
    UINT32 u4Param1[20];
    UINT32 u4Param2[20];
    UINT32 u4Param3[20];
    UINT32 u4Result[20];

    UINT32* pu4SrcBufAddr;
    UINT32* pu4DstBufAddr;
    UINT32* pu4GoldenBufAddr;

    BYTE *pbSrcBufAddr;
    BYTE *pbDstBufAddr;
    BYTE *pbGoldenBufAddr;

    pbSrcBufAddr = x_mem_alloc(0x7D000);
    pbDstBufAddr = x_mem_alloc(0x7D000);
    pbGoldenBufAddr = x_mem_alloc(0x7D000);

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) VIRTUAL((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) VIRTUAL((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) VIRTUAL((UINT32)pbGoldenBufAddr);
    #else //CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcBufAddr = (UINT32*) NONCACHE((UINT32)pbSrcBufAddr);
    pu4DstBufAddr = (UINT32*) NONCACHE((UINT32)pbDstBufAddr);
    pu4GoldenBufAddr = (UINT32*) NONCACHE((UINT32)pbGoldenBufAddr);
    #endif //CONFIG_DRV_LINUX_DATA_CONSISTENCY

    HalFlushInvalidateDCache();

    u1DevId = _u1GCPU_HWRes_Obtain(0);

    prGcpuCmd = (GCPU_HAL_CMD_T*) GCPUMemoryAlloc(sizeof(GCPU_HAL_CMD_T));
    x_memset(prGcpuCmd, 0x0, sizeof(GCPU_HAL_CMD_T));

    u4CmdPtr[0] = 8;
    u4CmdPtr[1] = 12;
    u4CmdPtr[2] = 16;

    pu4SrcBufAddr[0] = 0xEEEEEEEE;
    pu4SrcBufAddr[1] = 0xEEEEEEEE;
    pu4SrcBufAddr[2] = 0x11111111;
    pu4SrcBufAddr[3] = 0x11111111;
    pu4SrcBufAddr[4] = 0x22222222;
    pu4SrcBufAddr[5] = 0x33333333;
    pu4SrcBufAddr[6] = 0x8CF72D46;
    pu4SrcBufAddr[7] = 0xED57D476;

    //Enc Key
    u4Param0[0] = 0x0;
    u4Param0[1] = 0x0;
    u4Param0[2] = 0x0;
    u4Param0[3] = 0x0;

    //IV
    u4Param1[0] = 0x0;
    u4Param1[1] = 0x0;
    u4Param1[2] = 0x0;
    u4Param1[3] = 0x0;

    //XOR
    u4Param2[0] = 0x0;
    u4Param2[1] = 0x0;
    u4Param2[2] = 0x0;
    u4Param2[3] = 0x0;

    prGcpuCmd->ucDevId = u1DevId;
    x_memcpy(prGcpuCmd->u4CmdPtr, u4CmdPtr, 80);
    prGcpuCmd->u4SrcBufAddr = (UINT32) pu4SrcBufAddr;
    prGcpuCmd->u4DstBufAddr = (UINT32) pu4DstBufAddr;
    x_memcpy(prGcpuCmd->u4Param0, u4Param0, 80);
    x_memcpy(prGcpuCmd->u4Param1, u4Param1, 80);
    x_memcpy(prGcpuCmd->u4Param2, u4Param2, 80);
    prGcpuCmd->u4PacketNum = 2;
    prGcpuCmd->u4GcpuCmdMode = AESPK_EK_DCBC;
    prGcpuCmd->fgNeedSetData = TRUE;
    i4GCPU_HAL_SetKeyProtect_Cmd(prGcpuCmd);
    _vGCPU_WaitCmdExec(0);

    prGcpuCmd->u4GcpuCmdMode = AESPK_EK_DCBC;
    i4GCPU_HAL_GetKeyProtect_Result(prGcpuCmd);

    pu4GoldenBufAddr[0] = 0xF3D49FBE;
    pu4GoldenBufAddr[1] = 0x0D00CA8F;
    pu4GoldenBufAddr[2] = 0x6666771C;
    pu4GoldenBufAddr[3] = 0x77CC54E2;
    pu4GoldenBufAddr[4] = 0x3C143CC4;
    pu4GoldenBufAddr[5] = 0x2306D73E;
    pu4GoldenBufAddr[6] = 0xD689C163;
    pu4GoldenBufAddr[7] = 0x15F8AAFB;

    //Check Golden
    if (TRUE == fgGcpuCompare(pbDstBufAddr, pbGoldenBufAddr, 32))
    {
        UTIL_Printf("AES PK_EK_DCBC Pass\n", i);
    }
    else
    {
        UTIL_Printf("AES PK_EK_DCBC Fail\n", i);
        while(1);
    }

    x_mem_free(pbSrcBufAddr);
    x_mem_free(pbDstBufAddr);
    x_mem_free(pbGoldenBufAddr);

    vGCPU_HAL_HWRes_Release(u1DevId);

    GCPUMemoryFree(prGcpuCmd);

    return 0;
}
#endif
