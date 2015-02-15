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

#include "dmx_emu.h"
#include "dmx_drm_drvif.h"
#include "dmx_mm.h"
#include "dmx_imem_drm.h"
#include "dmx_imem.h"
#include "dmx_drm_drvif.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"
#include "x_hal_5381.h"
#include "x_hal_arm.h"

#ifdef DMX_SUPPORT_CPRM
#include "c2_dcbc_source.h"
#include "c2_dcbc_golden.h"
#endif // DMX_SUPPORT_CPRM

extern void aes_crypt_cbc_scte52( aes_context *ctx,
                    INT32 mode,
                    INT32 length,
                    UINT8 iv[16],
                    UINT8 *input,
                    UINT8 *output );

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MAX_KEY_NO                      DMX_DMEM_CA_KEY_NUM

/// uCode type
#define DMX_UCODE_TYPE_TS               0x9
#define DMX_UCODE_TYPE_PS               0xb
#define DMX_UCODE_TYPE_DRM              0xd

// PDDRM key length
#ifndef DRMCIPHERKEYLEN
#define DRMCIPHERKEYLEN   7
#endif

// CPRM key length
#ifndef DRMCPRMKEYLEN
#define DRMCPRMKEYLEN   8
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define IMEM_READ32(offset)         IO_READ32(DEMUX1_BASE, (offset))
#define IMEM_WRITE32(offset, value) IO_WRITE32(DEMUX1_BASE, (offset), (value))
#define IMEM_REG32(offset)          IO_READ32(DEMUX1_BASE, (offset))

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

#ifdef DMX_SUPPORT_CPRM
//CPRM Decryption
//C2_D
static UINT32 CPM_C2_D_KEY[2] = {0xc4082b00, 0x5914b2b6};
static UINT32 CPM_C2_D_DATA[2] = {0x75752baa, 0x49b6429a};
static UINT32 CPM_C2_D_RESULT[2] = {0x71a2b1ce, 0x629d1b39};

//C2_E
static UINT32 CPM_C2_E_KEY[2] = {0xc4082b00, 0x5914b2b6};
static UINT32 CPM_C2_E_DATA[2] = {0x71a2b1ce, 0x629d1b39};
static UINT32 CPM_C2_E_RESULT[2] = {0x75752baa, 0x49b6429a};

//C2_G
static UINT32 CPM_C2_G_KEY[2] = {0xc4082b00, 0x5914b2b6};
static UINT32 CPM_C2_G_DATA[2] = {0x71a2b1ce, 0x629d1b39};
static UINT32 CPM_C2_G_RESULT[2] = {0x04d79a64, 0x2b2b59a3};

//C2D_CBC
static UINT32 CPM_2_DCBC_KEY[2] = {0xe6b20f00, 0xdd2c33fe};
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

BOOL _GetMicroCodeVersion(UINT32* pu4Version, UINT32* pu4Type)
{
    UINT32 u4Word0;

    // Halt uP first
    _DMX_EnableFTI(FALSE);

    u4Word0 = IMEM_READ32(0);
    if (pu4Version != NULL)
    {
        *pu4Version = u4Word0 & 0xffff;
    }

    if (pu4Type != NULL)
    {
        *pu4Type = (u4Word0 >> 26) & 0x3f;
    }

    return TRUE;
}


BOOL _DrmInit(DRM_PARAM_T *prParam)
{
    UINT32 u4Ctrl, u4Type;

    // Check uCode type
    if (!_GetMicroCodeVersion(NULL, &u4Type))
    {
        return FALSE;
    }

    if((prParam->eMode == DMX_DRM_MODE_BYPASS) ||
       (prParam->eMode == DMX_DRM_MODE_AES) ||
       (prParam->eMode == DMX_DRM_MODE_ND) ||
       (prParam->eMode == DMX_DRM_MODE_AES_ENCRYPTION))
    {
        if (!_DMX_LoadIMem(TRUE, FTI_IMEM, DRM_IMEM_LEN))
        {
            return FALSE;
        }
    }
    else
    {
        if (!_DMX_LoadIMem(TRUE, DRM_IMEM, DRM_IMEM_LEN))
        {
            return FALSE;
        }
    }

    // Enable FTI
    _DMX_EnableFTI(TRUE);

    // Set AES engine to be controlled by uP
    u4Ctrl = DMXCMD_READ32(DMX_REG_CONFIG1);
    u4Ctrl |= (1 << 11);
    DMXCMD_WRITE32(DMX_REG_CONFIG1, u4Ctrl);

    return TRUE;
}


//static UINT8  _u1PdSel;

static BOOL _SetDrmKeys(UINT8 u1Pidx, DRM_PARAM_T* prParam, UINT32 u4Size, UINT32 u4EncryptedAddr)
{
    UINT32 u4Key, u4Mode, u4Scheme, u4Flag, u4Ctrl, i, j;
    UINT8* p;
    UINT32 u4KeyBaseAddr;       // in word (not in byte)

    u4KeyBaseAddr = 852;    // Word offset

#ifdef DMX_SUPPORT_CPRM
    if(prParam->eMode == DMX_DRM_MODE_CPRM_C2D ||
       prParam->eMode == DMX_DRM_MODE_CPRM_C2E ||
       prParam->eMode == DMX_DRM_MODE_CPRM_C2G ||
       prParam->eMode == DMX_DRM_MODE_CPRM_C2DCBC)
    {
        // Set keys
        for (i = 0; i < 2; i++)
        {
            p = prParam->au1Key + (i * 4);
            u4Key = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
            PID_W((u4KeyBaseAddr + 1) - i) = u4Key;
        }
    }
    else
#endif // DMX_SUPPORT_CPRM
    {
        // Set keys
        for (i = 0; i < 8; i++)
        {
            p = prParam->au1Key + (i * 4);
            u4Key = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
            PID_W((u4KeyBaseAddr + 7) - i) = u4Key;
        }
    }


    u4Mode = 0;

    // Set initial vector
    if (prParam->fgCbc)
    {
        UINT32 au4Iv[4];

        for (i = 0; i < 4; i++)
        {
            au4Iv[i] = 0;

            for (j = 0; j < 4; j++)
            {
                au4Iv[i] |= (prParam->au1InitVector[(i * 4) + j] << ((3 - j) * 8));
            }
        }

        // 5368 remove
        ////_DMX_SetDmemAesIV(au4Iv);

        u4Mode |= (1 << 0);         // CBC
    }

    // Set key length
    switch (prParam->u2KeyLen)
    {
    case 128:
        u4Mode |= (0 << 2);
        break;

    case 192:
        u4Mode |= (1 << 2);
        break;

    case 256:
        u4Mode |= (2 << 2);
        break;

    case 56:    //PD-DRM key length =  DRMCIPHERKEYLEN * 8 || CPRM
        break;

    case 64:    //CPRM
        break;

    default:
        ASSERT(0);
        break;
    }

    u4Flag = 0;
    u4Flag |= (1 << 6);         // To CD-FIFO

    // Scheme
    switch (prParam->eMode)
    {
    case DMX_DRM_MODE_BYPASS:
        u4Scheme = 0;           // Bypass
        break;

    case DMX_DRM_MODE_AES:
        u4Scheme = 1;           // AES
        break;

    case DMX_DRM_MODE_AES_ENCRYPTION:
        u4Scheme = 1;           // AES
        u4Mode |= (1 << 1);     // Encrypt mode
        break;

    case DMX_DRM_MODE_AES_SCTE52:
        u4Scheme = 1;           // AES
        u4Mode |= (1 << 6);     // SCTE-52
        break;

    case DMX_DRM_MODE_ND:
        u4Scheme = 1;           // AES
        u4Mode |= (1 << 7);     // Counter mode
        u4Mode |= (1 << 1);     // Encrypt mode
        break;

#ifdef DMX_SUPPORT_CPRM
    case DMX_DRM_MODE_CPRM_C2D:
    case DMX_DRM_MODE_CPRM_C2E:
    case DMX_DRM_MODE_CPRM_C2G:
    case DMX_DRM_MODE_CPRM_C2DCBC:
        u4Scheme = 4;           // CPRM
        if(prParam->eMode == DMX_DRM_MODE_CPRM_C2D)
        {
            u4Mode = 3;
        }
        else if(prParam->eMode == DMX_DRM_MODE_CPRM_C2E)
        {
            u4Mode = 2;
        }
        else if(prParam->eMode == DMX_DRM_MODE_CPRM_C2G)
        {
            u4Mode = 6;
        }
        else if(prParam->eMode == DMX_DRM_MODE_CPRM_C2DCBC)
        {
            u4Mode = 9;
        }
        break;
#endif // DMX_SUPPORT_CPRM

    default:
        // Unknown
        ASSERT(0);
        return FALSE;
    }

    u4Ctrl = PID_W(0);
    u4Ctrl &= 0x00ff00ff;
    u4Ctrl |= (u4Scheme << 24); // Scheme
    u4Ctrl |= (u4Mode << 8);    // Mode
    u4Ctrl |= (u4Flag << 0);    // Flags

    u4Ctrl |= 0x4;  // Output mode enable
    u4Ctrl &= ~(0x10);
    PID_W(0) = u4Ctrl;

    PID_S_W(u1Pidx, 1) = u4Size;                    // Chunk length
    PID_S_W(u1Pidx, 2) = u4Size;                    // Encryption length
    PID_S_W(u1Pidx, 3) = 1;                         // Trigger
    PID_S_W(u1Pidx, 15) = 0;                        // AES Offset length

    PID_INDEX_TABLE(0) = 0x0;

    // Set sample id
    if (prParam->eMode == DMX_DRM_MODE_ND)
    {
        UINT32 u4SampleId;

        for (i = 0; i < 2; i++)
        {
            p = prParam->au1SampleId + (i * 4);
            u4SampleId = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

            PID_W((u4KeyBaseAddr + 13) - i) = u4SampleId;
        }
    }

    return TRUE;
}


static BOOL _EncryptBypass(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    x_memcpy((void*)u4DstAddr, (void*)u4SrcAddr, u4Size);

    return TRUE;
}


static BOOL _EncryptAes(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;

    // Setup key scheduling
    aes_setkey_enc(&rCtx, prParam->au1Key, prParam->u2KeyLen);

    if (prParam->fgCbc)
    {
        x_memcpy(au1InitVector, prParam->au1InitVector, 16);
    }


    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        x_memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text

        // Encrypt
        if (prParam->fgCbc)
        {
            aes_crypt_cbc(&rCtx, AES_ENCRYPT, 16, au1InitVector, au1Text,
                au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Text);
        }

        x_memcpy(pu1DstBuf + u4Offset, au1Text, 16);    // Copy back
    }

    if((prParam->eMode == DMX_DRM_MODE_AES_SCTE52) &&
        (prParam->fgCbc == TRUE) && ((u4Size - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        UINT8 au1Texto[32];
        u1Cnt = u4Size - u4Offset;
        x_memcpy(au1Text, pu1SrcBuf + u4Offset, u1Cnt);   // Get plain text
        //aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1InitVector, au1Texto);
        //for( i = 0; i < u1Cnt; i++ )
        //{
        //    au1Texto[i] = (UINT8)( au1Text[i] ^ au1Texto[i] );
        //}
        aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1InitVector, au1Text, au1Texto);
        x_memcpy(pu1DstBuf + u4Offset, au1Texto, u1Cnt);  // Copy back
    }
    else
    {
    x_memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back
    }

    //printf("AES Encrytped:\n");
    //DumpByteArray(u4DstAddr, u4Size);
    //printf("AES Decrypted:\n");
    //DumpByteArray(u4SrcAddr, u4Size);

    return TRUE;
}

static BOOL _DecryptAes(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;

    // Setup key scheduling
    aes_setkey_dec(&rCtx, prParam->au1Key, prParam->u2KeyLen);

    if (prParam->fgCbc)
    {
        x_memcpy(au1InitVector, prParam->au1InitVector, 16);
    }

    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        x_memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text

        // Encrypt
        if (prParam->fgCbc)
        {
            aes_crypt_cbc(&rCtx, AES_DECRYPT, 16, au1InitVector, au1Text,
                au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_DECRYPT, au1Text, au1Text);
        }

        x_memcpy(pu1DstBuf + u4Offset, au1Text, 16);    // Copy back
    }

    x_memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back

    return TRUE;
}


static void _hton64(UINT64 u8Src, UINT8* pu1Dst)
{
    UINT32 u4Bytes, i;
    UINT8* pu1Src;

    pu1Src = (UINT8*)&u8Src;
    u4Bytes = sizeof (UINT64);

    for (i = 0; i < u4Bytes; i++)
    {
        pu1Dst[i] = pu1Src[u4Bytes - 1 - i];
    }
}

static void _Xor(UINT8* pu1Dst, UINT8* pu1Src, UINT32 u4Size)
{
    UINT32 i;

    for (i = 0; i < u4Size; i++)
    {
        pu1Dst[i] ^= pu1Src[i];
    }
}

static BOOL _EncryptNd(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    aes_context rCtx;
    UINT64 u8BlockId = 0;
	UINT8 au1Text[32], au1Output[32];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;

    // Setup key scheduling
    aes_setkey_enc(&rCtx, prParam->au1Key, prParam->u2KeyLen);
//    aes_setkey_dec(&rCtx, prParam->au1Key, prParam->u2KeyLen);

    // Copy sample id as first half of text
    x_memcpy(au1Text, prParam->au1SampleId, 8);

    while (u4Size > 0)
    {
        UINT32 u4RoundSize;

        u4RoundSize = MIN(u4Size, 16);

        // Copy block id as second half of text
        _hton64(u8BlockId, au1Text + 8);

        aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Output);
//        aes_crypt_ecb(&rCtx, AES_DECRYPT, au1Text, au1Output);

        x_memcpy(pu1DstBuf, pu1SrcBuf, u4RoundSize);
        _Xor(pu1DstBuf, au1Output, u4RoundSize);

        pu1DstBuf += u4RoundSize;
        pu1SrcBuf += u4RoundSize;
        u4Size -= u4RoundSize;
        u8BlockId++;
    }

    return TRUE;
}

static BOOL _DecryptNd(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    return _EncryptNd(u4DstAddr, u4SrcAddr, u4Size, prParam);
}


#ifdef DMX_SUPPORT_CPRM
static BOOL _EncryptCprm(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    ASSERT(u4Size >= 8);

    switch (prParam->eMode)
    {
    case DMX_DRM_MODE_CPRM_C2D:
        x_memcpy((void *)u4SrcAddr, CPM_C2_D_RESULT, u4Size);
        x_memcpy((void *)u4DstAddr, CPM_C2_D_DATA, u4Size);
        x_memcpy((void *)prParam->au1Key, CPM_C2_D_KEY, prParam->u2KeyLen/8);
        break;
    case DMX_DRM_MODE_CPRM_C2E:
        x_memcpy((void *)u4SrcAddr, CPM_C2_E_RESULT, u4Size);
        x_memcpy((void *)u4DstAddr, CPM_C2_E_DATA, u4Size);
        x_memcpy((void *)prParam->au1Key, CPM_C2_E_KEY, prParam->u2KeyLen/8);
        break;
    case DMX_DRM_MODE_CPRM_C2G:
        x_memcpy((void *)u4SrcAddr, CPM_C2_G_RESULT, u4Size);
        x_memcpy((void *)u4DstAddr, CPM_C2_G_DATA, u4Size);
        x_memcpy((void *)prParam->au1Key, CPM_C2_G_KEY, prParam->u2KeyLen/8);
        break;
    case DMX_DRM_MODE_CPRM_C2DCBC:
        x_memcpy((void *)u4SrcAddr, au1C2CBC_Golden, u4Size);
        x_memcpy((void *)u4DstAddr, au1C2CBC_Source, u4Size);
        x_memcpy((void *)prParam->au1Key, CPM_2_DCBC_KEY, prParam->u2KeyLen/8);
        break;

    default:
        ASSERT(0);
        break;
    }

    return TRUE;
}
#endif //DMX_SUPPORT_CPRM


static BOOL _Encrypt(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size,
    DRM_PARAM_T* prParam)
{
    BOOL fgRet;

    ASSERT(prParam != NULL);

    switch (prParam->eMode)
    {
    case DMX_DRM_MODE_BYPASS:
        fgRet = _EncryptBypass(u4DstAddr, u4SrcAddr, u4Size, prParam);
        break;

    case DMX_DRM_MODE_AES:
    case DMX_DRM_MODE_AES_ENCRYPTION:
    case DMX_DRM_MODE_AES_SCTE52:
        fgRet = _EncryptAes(u4DstAddr, u4SrcAddr, u4Size, prParam);
        break;

    case DMX_DRM_MODE_ND:
        fgRet = _EncryptNd(u4DstAddr, u4SrcAddr, u4Size, prParam);
        break;

#ifdef DMX_SUPPORT_CPRM
    case DMX_DRM_MODE_CPRM_C2D:
    case DMX_DRM_MODE_CPRM_C2E:
    case DMX_DRM_MODE_CPRM_C2G:
    case DMX_DRM_MODE_CPRM_C2DCBC:
        fgRet = _EncryptCprm(u4DstAddr, u4SrcAddr, u4Size, prParam);
        break;
#endif // DMX_SUPPORT_CPRM

    default:
        fgRet = FALSE;
        break;
    }

    return fgRet;
}


static BOOL _DrmTest(UINT8 u1TsIndex, UINT32 u4Size, DRM_PARAM_T* prParam)
{
    UINT8 u1PktSize;
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4EncryptAddr, u4Flags;
    UINT16 u2Pid = 0;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    ASSERT(u1TsIndex <= MAX_TS_INDEX);
    u1PktSize=(random(241 - 32) + 32) & 0xfffffffc;

    // uP AES SCTE-52 only support that the last packet with residual
    // termination block(RTB) must be lenght of RTB plus 16 bytes
    // If not, we will bypass the pattern and return TRUE
    if(prParam->eMode == DMX_DRM_MODE_AES_SCTE52)
    {
        UINT32 u4LastBlockSize = u4Size % 16;
        UINT32 u4LastPktSize = u4Size % u1PktSize;
        if(u4LastPktSize < u4LastBlockSize + 16)
        {
            printf(" We should bypass this pattern, because uP does not support.\n");
            return TRUE;
        }
    }

    // Allocate source and destination buffers
    u4EncryptAddr = 0x2000000;
        //(UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT * u1PktSize);
    u4DstAddr = 0x2100000; //(UINT32)BSP_AllocAlignedDmaMemory(u4Size + DEF_BUF_FULL_GAP + 1,
        //DMX_ES_FIFO_ALIGNMENT);
    u4SrcAddr = 0x2200000; //(UINT32)x_mem_alloc(u4Size);
    if ((u4EncryptAddr == 0) || (u4DstAddr == 0) || (u4SrcAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    x_memset((VOID*)u4EncryptAddr, 0, u4Size);
    x_memset((VOID*)u4DstAddr, 0, u4Size);
    if (!_Encrypt(u4EncryptAddr, u4SrcAddr, u4Size, prParam))
    {
        return FALSE;
    }

    // Init
    if (!_DrmInit(prParam))
    {
        return FALSE;
    }

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, u1PktSize, TRUE, FALSE))
    {
        goto _exit;
    }

    u4DstEnd = DMX_Align(u4DstAddr + u4Size + DEF_BUF_FULL_GAP + 1, DMX_ES_FIFO_ALIGNMENT);

    // Setup PID
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4DstEnd - u4DstAddr;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_ES_OTHER;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.eDescMode = DMX_DESC_MODE_NONE;

    // Determine descramble mode
    u4Flags = DMX_PID_FLAG_ALL;
    if (!DMX_SetPid(u1Pidx, u4Flags, &rPid))
    {
        return FALSE;
    }

    // Set keys
    if (!_SetDrmKeys(u1Pidx, prParam, u4Size, u4EncryptAddr))
    {
        return FALSE;
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    // Enable all demux interrupts, this can test if uP will issue only 1 interrupt
    // when chunk length and encryption length are equal
    DMXCMD_WRITE32(DMX_REG_INT_MASK, 0xffffffff);

    Printf("Pkt Size: %u\n", u1PktSize);
	DDI_WRITE32(DDI_REG_PKT_QUADBYTE_LIMIT, u1PktSize / 4);

    if(prParam->eMode != DMX_DRM_MODE_AES_ENCRYPTION)
    {
        // Invoke DDI transfer
        if (!_DMX_EMU_DDI_Transfer(0, u4EncryptAddr, DMX_Align(u4Size + 1, DDI_BUF_ALIGNMENT * u1PktSize)))
        {
            // Error on transferring data
            goto _exit;
        }

        HalFlushInvalidateDCache();

        // Check result
        if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
        {
            Printf("Src: 0x%08x, Dst: 0x%08x, size: %u offset: %u\n", u4SrcAddr, u4DstAddr,
                u4Size, u4SrcDiff-u4SrcAddr);
            Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
            goto _exit;
        }
    }
    else
    {
        // Invoke DDI transfer
        if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, DMX_Align(u4Size + 1, DDI_BUF_ALIGNMENT * u1PktSize)))
        {
            // Error on transferring data
            goto _exit;
        }
        // Check result
        if (!_MemCmp(u4EncryptAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
        {
            Printf("Src: 0x%08x, Dst: 0x%08x, size: %u offset: %u\n", u4SrcAddr, u4DstAddr,
                u4Size, u4SrcDiff-u4SrcAddr);
            Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
            goto _exit;
        }
    }

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid));

    // Free PID
    VERIFY(DMX_FreePid(u1Pidx));

    // Free memory
    //x_mem_free((void*)u4SrcAddr);
    //VERIFY(BSP_FreeAlignedDmaMemory((void*)u4DstAddr));
    //VERIFY(BSP_FreeAlignedDmaMemory((void*)u4EncryptAddr));

    return fgRet;
}


static UINT32 _AesPerfTest(UINT8 u1TsIndex, UINT32 u4Size, DRM_PARAM_T* prParam, BOOL fgSw)
{
    UINT8 u1PktSize;
    DMX_PID_T rPid;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4DstEnd, u4SrcDiff, u4DstDiff;
    UINT32 u4EncryptAddr, u4Flags;
    UINT16 u2Pid = 0;
    const UINT8 u1Pidx = 0;
    HAL_TIME_T rStartTime;
    HAL_TIME_T rStopTime;
    HAL_TIME_T rDeltaTime;
    UINT32 u4Speed = 0x0;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);
  	u1PktSize= 188;

    // Allocate source and destination buffers
    u4EncryptAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT * u1PktSize);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + DEF_BUF_FULL_GAP + 1,
        DMX_ES_FIFO_ALIGNMENT);
    u4SrcAddr = (UINT32)x_mem_alloc(u4Size);
    if ((u4EncryptAddr == 0) || (u4DstAddr == 0) || (u4SrcAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    x_memset((VOID*)u4EncryptAddr, 0, u4Size);
    x_memset((VOID*)u4DstAddr, 0, u4Size);
    if (!_Encrypt(u4EncryptAddr, u4SrcAddr, u4Size, prParam))
    {
        ASSERT(0);
    }

    if(!fgSw)
    {
        // Init
        if (!_DrmInit(prParam))
        {
            ASSERT(0);
        }

        // Set to bypass mode
        if (!_DMX_SetBypassMode(u1TsIndex, u1PktSize, TRUE, FALSE))
        {
            goto _exit;
        }

        u4DstEnd = DMX_Align(u4DstAddr + u4Size + DEF_BUF_FULL_GAP + 1, DMX_ES_FIFO_ALIGNMENT);
        u4DstEnd += 0x1000;

        // Setup PID
        rPid.fgEnable = TRUE;
        rPid.fgAllocateBuffer = FALSE;
        rPid.fgPrimary = TRUE;
        rPid.u1TsIndex = u1TsIndex;
        rPid.u1DeviceId = 0;
        rPid.u1KeyIndex = 0;
        rPid.u1SteerMode = DMX_STEER_TO_FTUP;
        rPid.u2Pid = u2Pid;
        rPid.u4BufAddr = u4DstAddr;
        rPid.u4BufSize = u4DstEnd - u4DstAddr;
        rPid.ePcrMode = DMX_PCR_MODE_NONE;
        rPid.ePidType = DMX_PID_TYPE_ES_OTHER;
        rPid.pfnNotify = NULL;
        rPid.pvNotifyTag = NULL;
        rPid.pfnScramble = NULL;
        rPid.pvScrambleTag = NULL;
        rPid.eDescMode = DMX_DESC_MODE_NONE;

        u4Flags = DMX_PID_FLAG_ALL;
        if (!DMX_SetPid(u1Pidx, u4Flags, &rPid))
        {
            return FALSE;
        }

        // Set keys
        if (!_SetDrmKeys(u1Pidx, prParam, u4Size, u4EncryptAddr))
        {
            return FALSE;
        }

        HAL_GetTime(&rStartTime);

        // Init DDI
        if (!_DMX_EMU_DDI_Init(u1TsIndex))
        {
            goto _exit;
        }

        Printf("Pkt Size: %u\n", u1PktSize);
    	DDI_WRITE32(DDI_REG_PKT_QUADBYTE_LIMIT, u1PktSize / 4);


        if(prParam->eMode != DMX_DRM_MODE_AES_ENCRYPTION)
        {
            // Invoke DDI transfer
            if (!_DMX_EMU_DDI_Transfer(0, u4EncryptAddr, DMX_Align(u4Size + 1, DDI_BUF_ALIGNMENT * u1PktSize)))
            {
                // Error on transferring data
                goto _exit;
            }
        }
        else
        {
            // Invoke DDI transfer
            if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, DMX_Align(u4Size + 1, DDI_BUF_ALIGNMENT * u1PktSize)))
            {
                // Error on transferring data
                goto _exit;
            }
        }

        HAL_GetTime(&rStopTime);
        HAL_GetDeltaTime(&rDeltaTime, &rStartTime, &rStopTime);
    }
    else
    {
        if(prParam->eMode == DMX_DRM_MODE_AES)
        {
            HAL_GetTime(&rStartTime);
            _DecryptAes(u4DstAddr, u4EncryptAddr, u4Size, prParam);
            HAL_GetTime(&rStopTime);
        }
        else if(prParam->eMode == DMX_DRM_MODE_AES_ENCRYPTION)
        {
            HAL_GetTime(&rStartTime);
            _EncryptAes(u4DstAddr, u4SrcAddr, u4Size, prParam);
            HAL_GetTime(&rStopTime);
        }
        else if(prParam->eMode == DMX_DRM_MODE_ND)
        {
            HAL_GetTime(&rStartTime);
            _DecryptNd(u4DstAddr, u4EncryptAddr, u4Size, prParam);
            HAL_GetTime(&rStopTime);
        }

        HAL_GetDeltaTime(&rDeltaTime, &rStartTime, &rStopTime);
    }

    Printf("Time: %d : %d\n", rDeltaTime.u4Seconds, rDeltaTime.u4Micros);
    u4Speed = u4Size * 1000 / (rDeltaTime.u4Seconds * 1000 * 1000 + rDeltaTime.u4Micros) * 1000 / 1024;
    Printf("Speed: %d KB/sec\n", u4Speed);


        if(prParam->eMode != DMX_DRM_MODE_AES_ENCRYPTION)
        {
            // Check result
            if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
            {
                Printf("Src: 0x%08x, Dst: 0x%08x, size: %u offset: %u\n", u4SrcAddr, u4DstAddr,
                    u4Size, u4SrcDiff-u4SrcAddr);
                Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
                goto _exit;
            }
        }
        else
        {
            // Check result
            if (!_MemCmp(u4EncryptAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
            {
                Printf("Src: 0x%08x, Dst: 0x%08x, size: %u offset: %u\n", u4EncryptAddr, u4DstAddr,
                    u4Size, u4SrcDiff-u4SrcAddr);
                Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
                goto _exit;
            }
        }

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid));

    // Free PID
    VERIFY(DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4SrcAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4EncryptAddr));

    return u4Speed;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** DMX_EMU_Drm0
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_Drm0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid,
    DMX_DRM_MODE_T eMode)
{
    DRM_PARAM_T rParam;
    UINT32 i;
    CHAR* szMode;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    // DRM
    if (eMode == DMX_DRM_MODE_UNKNOWN)
    {
#ifdef CC_MT5360
        // MT5360 does not support PD mode
        eMode = (DMX_DRM_MODE_T)random(DMX_DRM_MODE_UNKNOWN - 1);
#else
	    eMode = (DMX_DRM_MODE_T)random(DMX_DRM_MODE_UNKNOWN);
#endif  // CC_MT5360
	}

    rParam.eMode = eMode;

/*
    if (!DMX_IsAligned(u4Size, 188))
    {
        return FALSE;
    }
*/
    // Generate DES parameters
    // ECB or CBC (and init vector)
    rParam.fgCbc = (random(2) == 0);
    if(rParam.eMode == DMX_DRM_MODE_AES_SCTE52) // SCTE52 only support CBC mode
    {
        rParam.fgCbc = TRUE;
    }

    if (rParam.fgCbc)
    {
        for (i = 0; i < 16; i++)
        {
            rParam.au1InitVector[i] = random(256);
        }
    }

    // Key length
    rParam.u2KeyLen = 128 + (random(3) * 64);

    // Generate keys
    for (i = 0; i < rParam.u2KeyLen / 8; i++)
    {
        rParam.au1Key[i] = random(256);
    }

    switch (eMode)
    {
    case DMX_DRM_MODE_BYPASS:
        szMode = "BYPASS";
        break;

    case DMX_DRM_MODE_AES:
    case DMX_DRM_MODE_AES_SCTE52:
        szMode = "AES";
        break;

    case DMX_DRM_MODE_AES_ENCRYPTION:
        szMode = "AES Encryption";
        break;

    case DMX_DRM_MODE_ND:
        szMode = "ND";
    	rParam.fgCbc = 0;
    	rParam.u2KeyLen = 128;
        break;

    default:
        szMode = "UNKNOWN";
        break;
    }

    // Show mode and key select
    Printf(" Key size: %u, mode: %s, %s\n",
        rParam.u2KeyLen, rParam.fgCbc ? "CBC" : "ECB", szMode);

    // Show keys
    Printf(" Key: ");
    for (i = 0; i < rParam.u2KeyLen / 8; i++)
    {
        if ((i != 0) && ((i % 8) == 0))
        {
            Printf(":");
        }
        Printf("%02x", rParam.au1Key[i]);
    }
    Printf("\n");

    // Show initial vector
    if (rParam.fgCbc)
    {
        Printf(" Init vector: ");
        for (i = 0; i < 16; i++)
        {
            if (i == 8)
            {
                Printf(":");
            }
            Printf("%02x", rParam.au1InitVector[i]);
        }
        Printf("\n");
    }

    // Sample ID
    if (eMode == DMX_DRM_MODE_ND)
    {
        Printf(" Sample id: ");
        for (i = 0; i < 8; i++)
        {
            rParam.au1SampleId[i] = random(256);
            Printf("%02x", rParam.au1SampleId[i]);
        }
        Printf("\n");
    }

    return _DrmTest(u1TsIndex, u4Size, &rParam);
}

//-----------------------------------------------------------------------------
/** DMX_EMU_DRM_Perf
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_DRM_Perf(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid,
    DMX_DRM_MODE_T eMode, BOOL fgSw)
{
    DRM_PARAM_T rParam;
    UINT32 i, j, k, l;
    CHAR* szMode;
    UINT32 au4Speed[2][3] = {0};

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    rParam.eMode = eMode;

    if(eMode == DMX_DRM_MODE_AES || eMode == DMX_DRM_MODE_AES_ENCRYPTION)
    {
        for( k = 0; k < 2; k++) //ECB or CBC
        {
            for( l = 0; l < 3; l++) //key length
            {
                rParam.fgCbc = k; //(random(2) == 0);
                if (rParam.fgCbc)
                {
                    for (i = 0; i < 16; i++)
                    {
                        rParam.au1InitVector[i] = random(256);
                    }
                }

                // Key length
                rParam.u2KeyLen = 128 + (l * 64);

                // Generate keys
                for (i = 0; i < rParam.u2KeyLen / 8; i++)
                {
                    rParam.au1Key[i] = random(256);
                }

                switch (eMode)
                {
                case DMX_DRM_MODE_AES:
                    szMode = "AES";
                    break;

                case DMX_DRM_MODE_AES_ENCRYPTION:
                    szMode = "AES Encryption";
                    break;

                default:
                    szMode = "UNKNOWN";
                    break;
                }

                // Show mode and key select
                Printf(" Key size: %u, mode: %s, %s\n",
                    rParam.u2KeyLen, rParam.fgCbc ? "CBC" : "ECB", szMode);

                // Show keys
                Printf(" Key: ");
                for (i = 0; i < rParam.u2KeyLen / 8; i++)
                {
                    if ((i != 0) && ((i % 8) == 0))
                    {
                        Printf(":");
                    }
                    Printf("%02x", rParam.au1Key[i]);
                }
                Printf("\n");

                // Show initial vector
                if (rParam.fgCbc)
                {
                    Printf(" Init vector: ");
                    for (i = 0; i < 16; i++)
                    {
                        if (i == 8)
                        {
                            Printf(":");
                        }
                        Printf("%02x", rParam.au1InitVector[i]);
                    }
                    Printf("\n");
                }



                for( j = 0; j < 10; j++)
                {
                    au4Speed[k][l] += _AesPerfTest(u1TsIndex, u4Size, &rParam, fgSw);
                }
            }
        }

        // printf average speed
        Printf("Average Speed\n");
        for( k = 0; k < 2; k++) //ECB or CBC
        {
            for( l = 0; l < 3; l++) //key length
            {
                Printf("AES %s %d bit Speed: %d KB/s\n", k ? "CBC" : "ECB",
                    128 + (l * 64), au4Speed[k][l]/10);
            }
        }

    }
    else if(eMode == DMX_DRM_MODE_ND)
    {
        szMode = "ND";
    	rParam.fgCbc = 0;
    	rParam.u2KeyLen = 128;

        // Generate keys
        for (i = 0; i < rParam.u2KeyLen / 8; i++)
        {
            rParam.au1Key[i] = random(256);
        }

        Printf(" Sample id: ");
        for (i = 0; i < 8; i++)
        {
            rParam.au1SampleId[i] = random(256);
            Printf("%02x", rParam.au1SampleId[i]);
        }
        Printf("\n");

        for( j = 0; j < 10; j++)
        {
            au4Speed[0][0] += _AesPerfTest(u1TsIndex, u4Size, &rParam, fgSw);
        }

        // printf average speed
        Printf("Average ND Speed: %d KB/s\n", au4Speed[0][0]/10);
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** DMX_EMU_EncryptAes
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_EncryptAes(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size, DMX_MM_DRM_AES_T* prAES)
{
    DRM_PARAM_T rParam;
    UINT32 i;

    x_memset((void*)&rParam, 0, sizeof(rParam));
    rParam.eMode = DMX_DRM_MODE_AES;
    rParam.fgCbc = prAES->fgCBC;
    rParam.u2KeyLen = prAES->u4KeyBitLen;
    for(i=0; i<16; i++)
    {
        rParam.au1InitVector[i] = prAES->au1IV[i];
    }
    for(i=0; i<32; i++)
    {
        rParam.au1Key[i] = prAES->au1Key[i];
    }

    return _EncryptAes(u4DstAddr, u4SrcAddr, u4Size, &rParam);
}


//-----------------------------------------------------------------------------
// Self test
//-----------------------------------------------------------------------------

static UINT8 _au1CtrKey[16] =
{
    0x60, 0x09, 0x5e, 0xcd, 0xf6, 0x1e, 0xa4, 0x6b,
    0xac, 0x69, 0xc4, 0xab, 0x32, 0x5d, 0x72, 0x08
};

static UINT8 _au1CtrDataId[8] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

static UINT8 _au1CtrInput[48] =
{
    0x95, 0xdd, 0xad, 0xbf, 0x43, 0x03, 0xa6, 0x05,
    0xe7, 0x95, 0xbf, 0x06, 0x18, 0x46, 0xeb, 0xaf,
    0xff, 0x2a, 0xfe, 0xfa, 0xa7, 0x64, 0x4e, 0x19,
    0x94, 0x1d, 0xf4, 0x52, 0x43, 0x0a, 0xf7, 0x03,
    0x87, 0x9c, 0xd5, 0x35, 0xc3, 0x77, 0x09, 0x1a,
    0x2e, 0x7c, 0xfe, 0x8e, 0x86, 0x6e, 0x94, 0x45
};

static UINT8  _au1CtrOutput[48] =
{
    0xaa, 0x9b, 0x36, 0x83, 0xeb, 0x9d, 0xee, 0xab,
    0x06, 0xbe, 0xb3, 0x63, 0xa3, 0x90, 0x84, 0xee,
    0x29, 0x3c, 0xf9, 0x91, 0xf3, 0x7b, 0x59, 0xd5,
    0x63, 0x9e, 0x50, 0xeb, 0x1f, 0x70, 0x07, 0x3a,
    0xeb, 0x38, 0x50, 0x9d, 0xa4, 0x30, 0x6a, 0x8c,
    0x94, 0x04, 0x13, 0x96, 0x53, 0x7d, 0x4b, 0xb7
};

BOOL DMX_EMU_DrmNdSelfTest(void)
{
    DRM_PARAM_T rParam;
    UINT8 au1Output[48];

    rParam.u2KeyLen = 128;
    rParam.fgCbc = FALSE;
    rParam.eMode = DMX_DRM_MODE_ND;
    x_memcpy(rParam.au1Key, _au1CtrKey, 16);
    x_memcpy(rParam.au1SampleId, _au1CtrDataId, 8);

    if (!_EncryptNd((UINT32)au1Output, (UINT32)_au1CtrInput, 48, &rParam))
    {
        Printf("Error in _EncryptNd()!\n");
        return FALSE;
    }

    Printf("\nExpected output:");
    _DumpBuffer(_au1CtrOutput, 48, 16);

    Printf("\nReal output:");
    _DumpBuffer(au1Output, 48, 16);

    if (x_memcmp(au1Output, _au1CtrOutput, 48) != 0)
    {
        Printf("\nResults differ!\n");
        return FALSE;
    }

    return TRUE;
}


BOOL DMX_EMU_DrmNdMultipassTest(void)
{
#if 0
    DRM_PARAM_T rParam;
    UINT32 i;
    UINT32 u4Size;
    UINT8 u1PktSize = 188;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4EncryptAddr, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;

    UINT32 u4SentSize = 0;
    UINT32 u4ChunkLen = 1024;    // Send 1024 bytes at a time.

    DMX_MM_T rMM;
    DMX_MM_DRM_AES_T rDrm;


    u4Size = 1024 * (random(1024) + 1);

    // Show mode and key select
    Printf(" Size: %u\n", u4Size);

    // ND - DRM
    rParam.eMode = DMX_DRM_MODE_ND;
	rParam.fgCbc = 0;
	rParam.u2KeyLen = 128;

    // Generate keys
    for (i = 0; i < rParam.u2KeyLen / 8; i++)
    {
        rParam.au1Key[i] = random(256);
    }

    // Show mode and key select
    Printf(" Key size: %u\n", rParam.u2KeyLen);

    // Show keys
    Printf(" Key: ");
    for (i = 0; i < rParam.u2KeyLen / 8; i++)
    {
        if ((i != 0) && ((i % 8) == 0))
        {
            Printf(":");
        }
        Printf("%02x", rParam.au1Key[i]);
    }
    Printf("\n");

    // Sample ID
    Printf(" Sample id: ");
    for (i = 0; i < 8; i++)
    {
        rParam.au1SampleId[i] = random(256);
        Printf("%02x", rParam.au1SampleId[i]);
    }
    Printf("\n");


    // Allocate source and destination buffers
    u4EncryptAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, DDI_BUF_ALIGNMENT * u1PktSize);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + DEF_BUF_FULL_GAP + 1,
        DMX_ES_FIFO_ALIGNMENT);
    u4SrcAddr = (UINT32)x_mem_alloc(u4Size);
    if ((u4EncryptAddr == 0) || (u4DstAddr == 0) || (u4SrcAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    x_memset((VOID*)u4EncryptAddr, 0, u4Size);
    x_memset((VOID*)u4DstAddr, 0, u4Size);
    if (!_Encrypt(u4EncryptAddr, u4SrcAddr, u4Size, &rParam))
    {
        goto _exit;
    }

    if(!DMX_SelectInputType(DMX_IN_PLAYBACK_MM))
    {
        Printf("%s:%d: error!", __func__, __LINE__);
        goto _exit;
    }

    x_memset(&rMM, 0, sizeof(rMM));
    rMM.fgAllocBuf = FALSE;
    rMM.u4BufAddr = u4DstAddr;
    rMM.u4BufSize = u4Size << 1;
    rMM.u4Rp = u4DstAddr;
    rMM.u4Wp = u4DstAddr;
    rMM.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rMM.fgEnable = TRUE;
    if (!DMX_MM_Set(u1Pidx, DMX_MM_FLAG_ALL, &rMM))
    {
        Printf("%s:%d: error!", __func__, __LINE__);
        goto _exit;
    }

    // Set the DRM mode to ND-DRM.
    if (!DMX_MM_SetDRMMode(u1Pidx, DMX_DRM_MODE_ND))
    {
        Printf("%s:%d: error!", __func__, __LINE__);
        goto _exit;
    }

    // Set keys and the sample IDs for ND-DRM.
    x_memset(&rDrm, 0, sizeof(rDrm));
    x_memcpy(rDrm.au1Key, rParam.au1Key, rParam.u2KeyLen/8);
    x_memcpy(rDrm.au1SampleID, rParam.au1SampleId, sizeof(rParam.au1SampleId));
    u4Flags = DMX_MM_DRM_FLAG_KEY | DMX_MM_DRM_FLAG_SAMPLE_ID;
    if (!DMX_MM_SetNDDRM(u1Pidx, u4Flags, &rDrm))
    {
        Printf("%s:%d: error!", __func__, __LINE__);
        goto _exit;
    }

    while (u4Size > u4SentSize)
    {
        DMX_MM_DATA_T rData;
        UINT32 u4Timeout = 100;

        x_memset(&rData, 0, sizeof(rData));
        rData.fgTrigger = (u4SentSize == 0) ? TRUE : FALSE;
        rData.u4BufStart = u4EncryptAddr;
        rData.u4BufEnd = ((u4EncryptAddr + (u4Size << 1)) & 0xFFFFFFF0);
        rData.u4StartAddr = u4EncryptAddr + u4SentSize;

        if ((u4Size - u4SentSize) > u4ChunkLen)
        {
            rData.u4FrameSize = u4ChunkLen;
            // Make sure any intermediate chunk is a multiple of 16 bytes.
            if ((rData.u4FrameSize % 16) != 0)
            {
                Printf("%s:%d: error!", __func__, __LINE__);
                goto _exit;
            }
        }
        else
        {
            rData.u4FrameSize = u4Size - u4SentSize;
        }

        if ((rData.u4StartAddr + rData.u4FrameSize) > rData.u4BufEnd)
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }

        x_memset(&rDrm, 0, sizeof(rDrm));
        rDrm.u4ChunkLen = rData.u4FrameSize;
        rDrm.u4EncryLen = rData.u4FrameSize;
        u4Flags = DMX_MM_DRM_FLAG_CHUNK_LEN | DMX_MM_DRM_FLAG_ENCRYLEN;
        if (!DMX_MM_SetNDDRM(u1Pidx, u4Flags, &rDrm))
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }

        if (!DMX_NDDRM_MoveData(u1Pidx, &rData, u4Timeout))
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }
        u4SentSize += u4ChunkLen;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Src: 0x%08x, Dst: 0x%08x, size: %u offset: %u\n", u4SrcAddr, u4DstAddr,
            u4Size, u4SrcDiff-u4SrcAddr);
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Free memory
    x_mem_free((void*)u4SrcAddr);
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4EncryptAddr));
    DMX_MM_Free(u1Pidx);

    return fgRet;
#endif
    return TRUE;
}

// driver NDDRM interface verification with offset option
INT32 DMX_EMU_ND_Offset(void)
{
    DRM_PARAM_T rParam;
    UINT32 i;
    UINT32 u4Size;
    UINT8 u1PktSize = 188;
    UINT32 u4SrcAddr = 0, u4DstAddr = 0, u4SrcDiff, u4DstDiff;
    UINT32 u4EncryptAddr, u4Flags;
    UINT8 u1Pidx = 0, u1Inst;
    BOOL fgRet = FALSE;
    UINT32 u4VSrcVAddr = 0, u4DstVAddr = 0, u4EncryptVAddr = 0;

    UINT32 u4SentSize = 0;
    UINT32 u4ChunkLen = 1024;    // Send 1024 bytes at a time.
    UINT32 u4Offset = 10;
    UINT32 u4Remaining = 0;

    DMX_MM_T rMM;
    DMX_MM_DRM_AES_T rDrm;


    u4Size = 1024;

    // Show mode and key select
    Printf(" Size: %u\n", u4Size);

    // ND - DRM
    rParam.eMode = DMX_DRM_MODE_ND;
	rParam.fgCbc = 0;
	rParam.u2KeyLen = 128;

    // Generate keys
    for (i = 0; i < rParam.u2KeyLen / 8; i++)
    {
        rParam.au1Key[i] = 0; //random(256);
    }

    // Show mode and key select
    Printf(" Key size: %u\n", rParam.u2KeyLen);

    // Show keys
    Printf(" Key: ");
    for (i = 0; i < rParam.u2KeyLen / 8; i++)
    {
        if ((i != 0) && ((i % 8) == 0))
        {
            Printf(":");
        }
        Printf("%02x", rParam.au1Key[i]);
    }
    Printf("\n");

    // Sample ID
    Printf(" Sample id: ");
    for (i = 0; i < 8; i++)
    {
        rParam.au1SampleId[i] = 0; //random(256);
        Printf("%02x", rParam.au1SampleId[i]);
    }
    Printf("\n");


    // Allocate source and destination buffers
    u4EncryptAddr =
        (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 1, 16 * u1PktSize);
    u4DstAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4Size + 30 + 1,
        64);
    u4SrcAddr = (UINT32)x_mem_alloc(u4Size);
    if ((u4EncryptAddr == 0) || (u4DstAddr == 0) || (u4SrcAddr == 0))
    {
        goto _exit;
    }

    u4EncryptVAddr = VIRTUAL(u4EncryptAddr);
    u4DstVAddr = VIRTUAL(u4DstAddr);
    u4SrcAddr = (UINT32)x_mem_alloc(u4Size);

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4Size);

    x_memset((VOID*)u4EncryptVAddr, 0, u4Size);
    x_memset((VOID*)u4DstVAddr, 0, u4Size);
    x_memcpy((VOID*)u4EncryptVAddr, (VOID*)u4SrcAddr, u4Offset);
    if (!_Encrypt(u4EncryptVAddr + u4Offset, u4SrcAddr + u4Offset, u4Size - u4Offset - u4Remaining, &rParam))
    {
        goto _exit;
    }

    u1Inst = DMX_MUL_GetAvailableInst(DMX_CONN_TYPE_BUFAGENT_0);
    u1Pidx = DMX_MUL_GetAvailablePidx(u1Inst);

    x_memset(&rMM, 0, sizeof(rMM));
    rMM.fgAllocBuf = FALSE;
    rMM.u4BufAddr = u4DstVAddr;
    rMM.u4BufSize = u4Size << 1;
    rMM.u4Rp = u4DstVAddr;
    rMM.u4Wp = u4DstVAddr;
    rMM.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rMM.fgEnable = TRUE;
    if (!DMX_MM_Set(u1Pidx, DMX_MM_FLAG_ALL, &rMM))
    {
        Printf("%s:%d: error!", __func__, __LINE__);
        goto _exit;
    }

    // Set the DRM mode to ND-DRM.
    if (!DMX_MM_SetDRMMode(u1Pidx, DMX_DRM_MODE_ND))
    {
        Printf("%s:%d: error!", __func__, __LINE__);
        goto _exit;
    }

    // Set keys and the sample IDs for ND-DRM.
    x_memset(&rDrm, 0, sizeof(rDrm));
    x_memcpy(rDrm.au1Key, rParam.au1Key, rParam.u2KeyLen/8);
    x_memcpy(rDrm.au1SampleID, rParam.au1SampleId, sizeof(rParam.au1SampleId));
    rDrm.u4Offset = u4Offset;
    u4Flags = DMX_MM_DRM_FLAG_KEY | DMX_MM_DRM_FLAG_SAMPLE_ID | DMX_MM_DRM_FLAG_OFFSET;

    if (!DMX_MM_SetNDDRM(u1Pidx, u4Flags, &rDrm))
    {
        Printf("%s:%d: error!", __func__, __LINE__);
        goto _exit;
    }

    while (u4Size > u4SentSize)
    {
        DMX_MM_DATA_T rData;
        UINT32 u4Timeout = 100;

        x_memset(&rData, 0, sizeof(rData));
        rData.fgTrigger = (u4SentSize == 0) ? TRUE : FALSE;
        rData.u4BufStart = u4EncryptVAddr;
        rData.u4BufEnd = ((u4EncryptVAddr + (u4Size << 1)) & 0xFFFFFFF0);
        rData.u4StartAddr = u4EncryptVAddr + u4SentSize;
        rData.u1Idx = u1Pidx;

        if ((u4Size - u4SentSize) > u4ChunkLen)
        {
            rData.u4FrameSize = u4ChunkLen;
            // Make sure any intermediate chunk is a multiple of 16 bytes.
            if ((rData.u4FrameSize % 16) != 0)
            {
                Printf("%s:%d: error!", __func__, __LINE__);
                goto _exit;
            }
        }
        else
        {
            rData.u4FrameSize = u4Size - u4SentSize;
        }

        if ((rData.u4StartAddr + rData.u4FrameSize) > rData.u4BufEnd)
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }

        x_memset(&rDrm, 0, sizeof(rDrm));
        rDrm.u4ChunkLen = rData.u4FrameSize;
        rDrm.u4EncryLen = rData.u4FrameSize - u4Offset;
        u4Flags = DMX_MM_DRM_FLAG_CHUNK_LEN | DMX_MM_DRM_FLAG_ENCRYLEN;
        if (!DMX_MM_SetNDDRM(u1Pidx, u4Flags, &rDrm))
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }

        if(!DMX_MUL_SetVideoType(u1Inst, DMX_VIDEO_MPEG))
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }

        if(!DMX_MUL_SetInstType(u1Inst, DMX_IN_PLAYBACK_MM))
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }

        if(!DMX_MUL_MoveData(u1Inst, &rData))
        {
            Printf("%s:%d: error!", __func__, __LINE__);
            goto _exit;
        }

        u4SentSize += u4ChunkLen;
    }

    // Check result
    if (!_MemCmp(u4SrcAddr, u4DstVAddr, u4Size, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Src: 0x%08x, Dst: 0x%08x, size: %u offset: %u\n", u4SrcAddr, u4DstVAddr,
            u4Size, u4SrcDiff-u4SrcAddr);
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    Printf("OK.\n");

    fgRet = TRUE;

_exit:

    // Free memory
    x_mem_free((void*)u4SrcAddr);
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4DstAddr));
    VERIFY(BSP_FreeAlignedDmaMemory((void*)u4EncryptAddr));
    //DMX_MM_Free(u1Pidx);

    return 0;
}

