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
 * Copyright (c) 2005, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: dmx_descrambler.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_descrambler.c
 *  Demux driver - descrambler
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "tzst.h"
#include "dmx.h"
#include "fvr.h"
#include "x_assert.h"
#include "tee_crypto_key.h"
#include "tz_dmx_if.h"
#include "tz.h"

#include "tomcrypt.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define DMX_DESC_MAX_KEY_LEN                32

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
/* Descrambler algorithms */
typedef enum
{
    DESC_CIPHER_DES_ECB,           /* DES-56-ECB*/
    DESC_CIPHER_AES_128_CBC        /* AES-128-CCB*/
}   DESC_CIPHER_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define DWORDSWAP(u4Tmp) (((u4Tmp & 0xff) << 24) | ((u4Tmp & 0xff00) << 8) | ((u4Tmp & 0xff0000) >> 8) | ((u4Tmp & 0xff000000) >> 24))

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static UINT8 _u1ProtectedPidx = 0;
static UINT8 _au1DmxKey[DMX_KEY_IDX_MAX][DMX_DESC_MAX_KEY_LEN];

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static BOOL _DmxDataSwap(UINT32 *pu4Dest, UINT32 *pu4Src, UINT32 u4Size, UINT32 u4Mode)
{
     int i = 0;
     UINT32 u4Tmp;

    if (!pu4Dest || !pu4Src)
    {
        return FALSE;
    }
 
     if(u4Mode == 0)
     {
         for(i =0; i < u4Size; i++) //memcpy
         {
             *(pu4Dest + i) = *(pu4Src + i);
         }
     }
     else if(u4Mode == 1) //Endien Change
     {
         for(i =0; i < u4Size; i++)
         {
             u4Tmp = *(pu4Src + i);
             u4Tmp = DWORDSWAP(u4Tmp);
             *(pu4Dest + i) = u4Tmp;
         }
     }
    else if(u4Mode == 2) //Head Swap
    {
        for(i =0; i < u4Size; i++)
        {
            *(pu4Dest + u4Size - 1 - i) = *(pu4Src + i);
        }
    }
    else if(u4Mode == 3) //Head Swap + Endien Change
    {
        for(i =0; i < u4Size; i++)
        {
            u4Tmp = *(pu4Src + i);
            u4Tmp = DWORDSWAP(u4Tmp);
            *(pu4Dest + u4Size - 1 - i) = u4Tmp;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetCA_Mode
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetCA_Mode(UINT8 u1Idx, DMX_CA_MODE_T eMode, DMX_CA_FEEDBACK_MODE_T eFBMode, BOOL fgEncrypt)
{
    UINT32 u4Ctrl;

    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    u4Ctrl = DMX_DMEM_CA_CTRL(u1Idx);
    switch (eMode)
    {
    case DMX_CA_MODE_DES:
        u4Ctrl &= ~0xFF00FF4F;
        u4Ctrl |=  (((fgEncrypt ? 1 : 0) << 1) |       // Dec:0, Enc:1
                    (0 << 2) |                          // DES:0, 3DES:1
                    (1 << 27));
        break;

    case DMX_CA_MODE_3DES:
        u4Ctrl &= ~0xFF00FF4F;
        u4Ctrl |=  (((fgEncrypt ? 1 : 0) << 1) |       // Dec:0, Enc:1
                    (1 << 2)  |                         // DES:0, 3DES:1
                    (1 << 27));
        break;

    case DMX_CA_MODE_DVB:
        ASSERT(!fgEncrypt);
        u4Ctrl = 0x10000100;
        break;

    case DMX_CA_MODE_DVB_CONF:
        ASSERT(!fgEncrypt);
        u4Ctrl = 0x10000101;
        break;

    case DMX_CA_MODE_MULTI2_BIG:
    case DMX_CA_MODE_MULTI2_LITTLE:
        ASSERT(!fgEncrypt);
        u4Ctrl &= ~0xFF000000;
        u4Ctrl |= 0x20000100;
        break;

    case DMX_CA_MODE_AES:
        u4Ctrl &= ~0xFF00FF43;
        u4Ctrl |=  (((fgEncrypt ? 1 : 0) << 1) |       // Dec:0, Enc:1
                    (1 << 24));
        break;

    case DMX_CA_MODE_SMS4:
        u4Ctrl &= ~0xFF00FFFF;
        u4Ctrl |=  (((fgEncrypt ? 1 : 0) << 1) |
                    (1 << 30));
        break;

    case DMX_CA_MODE_NONE:
        break;

    default:
        return FALSE;
    }

    switch (eFBMode)
    {
    case DMX_CA_FB_ECB:
        break;

    case DMX_CA_FB_CBC:
        u4Ctrl |= 1;
        break;

    case DMX_CA_FB_CFB:
        u4Ctrl |= (3 |
                   (0xBC << 8) |                      // CFB
                   ((fgEncrypt ? 1 : 0) << 14));       // CFB encrypt or decrypt
        break;

    case DMX_CA_FB_OFB:
        u4Ctrl |= (3 |
                   (0xDC << 8) |                      // OFB
                   ((fgEncrypt ? 1 : 0) << 13));       // OFB encrypt or decrypt
        break;

    case DMX_CA_FB_CTR:
        u4Ctrl |= ((1 << 8) | (1 << 6));
        break;

    default:
        return FALSE;
    }

    DMX_DMEM_CA_CTRL(u1Idx) = u4Ctrl;

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _Dmx_SetDescMode
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetDescMode(UINT8 u1Idx, DMX_DESC_MODE_T eMode)
{
    DMX_CA_MODE_T eCaMode;
    DMX_CA_FEEDBACK_MODE_T eFBMode;
    BOOL fgEncrypt;

    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    switch (eMode)
    {
    case DMX_DESC_MODE_NONE:
        return TRUE;
    case DMX_DESC_MODE_DES_ECB:
        eCaMode = DMX_CA_MODE_DES;
        eFBMode = DMX_CA_FB_ECB;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_DES_CBC:
        eCaMode = DMX_CA_MODE_DES;
        eFBMode = DMX_CA_FB_CBC;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_3DES_ECB:
        eCaMode = DMX_CA_MODE_3DES;
        eFBMode = DMX_CA_FB_ECB;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_3DES_CBC:
        eCaMode = DMX_CA_MODE_3DES;
        eFBMode = DMX_CA_FB_CBC;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_DVB:
        eCaMode = DMX_CA_MODE_DVB;
        eFBMode = DMX_CA_FB_ECB;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_DVB_CONF:
        eCaMode = DMX_CA_MODE_DVB_CONF;
        eFBMode = DMX_CA_FB_ECB;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_MULTI2_BIG:
        eCaMode = DMX_CA_MODE_MULTI2_BIG;
        eFBMode = DMX_CA_FB_ECB;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_MULTI2_LITTLE:
        eCaMode = DMX_CA_MODE_MULTI2_LITTLE;
        eFBMode = DMX_CA_FB_ECB;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_AES_ECB:
        eCaMode = DMX_CA_MODE_AES;
        eFBMode = DMX_CA_FB_ECB;
        fgEncrypt = FALSE;
        break;
    case DMX_DESC_MODE_AES_CBC:
        eCaMode = DMX_CA_MODE_AES;
        eFBMode = DMX_CA_FB_CBC;
        fgEncrypt = FALSE;
        break;
    default:
        DMSG("A unsupported mode %d\n", eMode);
        return FALSE;
    }

    return _Dmx_SetCA_Mode(u1Idx, eCaMode, eFBMode, fgEncrypt);
}


//-----------------------------------------------------------------------------
/** _Dmx_SetCA_CTR
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetCA_CTR(UINT8 u1Idx, UINT64 u8Nonce, UINT64 u8Counter, BOOL fgEven)
{
    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    if (fgEven)
    {
        DMX_DMEM_CA_EVEN_IV(u1Idx, 0) = (UINT32)(u8Counter & 0xFFFFFFFF);
        DMX_DMEM_CA_EVEN_IV(u1Idx, 1) = (UINT32)((u8Counter >> 32) & 0xFFFFFFFF);
        DMX_DMEM_CA_EVEN_IV(u1Idx, 2) = (UINT32)(u8Nonce & 0xFFFFFFFF);
        DMX_DMEM_CA_EVEN_IV(u1Idx, 3) = (UINT32)((u8Nonce >> 32) & 0xFFFFFFFF);
    }
    else
    {
        DMX_DMEM_CA_ODD_IV(u1Idx, 0) = (UINT32)(u8Counter & 0xFFFFFFFF);
        DMX_DMEM_CA_ODD_IV(u1Idx, 1) = (UINT32)((u8Counter >> 32) & 0xFFFFFFFF);
        DMX_DMEM_CA_ODD_IV(u1Idx, 2) = (UINT32)(u8Nonce & 0xFFFFFFFF);
        DMX_DMEM_CA_ODD_IV(u1Idx, 3) = (UINT32)((u8Nonce >> 32) & 0xFFFFFFFF);
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _Dmx_SetCTR_Rtb
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetCTR_Rtb(UINT8 u1Idx, BOOL fgDoRtb)
{
    UINT32 u4Ctrl;

    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    u4Ctrl = DMX_DMEM_CA_CTRL(u1Idx);
    u4Ctrl &= ~0x180;
    if (fgDoRtb)
    {
        u4Ctrl |= 0x100;
    }

    DMX_DMEM_CA_CTRL(u1Idx) = u4Ctrl;

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _Dmx_SetDesIV
 *  Set initial vector of DES
 *
 *  @param  u4IvHi          The high word of the initial vector
 *  @param  u4IvLo          The low word of the initial vector
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetDesIV(UINT8 u1Idx, UINT32 u4IvHi, UINT32 u4IvLo, BOOL fgEven)
{
    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    if (fgEven)
    {
        DMX_DMEM_CA_EVEN_IV(u1Idx, 2) = u4IvLo;
        DMX_DMEM_CA_EVEN_IV(u1Idx, 3) = u4IvHi;
    }
    else
    {
        DMX_DMEM_CA_ODD_IV(u1Idx, 2) = u4IvLo;
        DMX_DMEM_CA_ODD_IV(u1Idx, 3) = u4IvHi;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetDesKey
 *  Set a desrambler key
 *
 *  @param  u1Index         The key index
 *  @param  fg3Des          TRUE: 3DES, FALSE: DES
 *  @param  fgEven          TRUE: even key, FALSE: odd key
 *  @param  au4Key          The key
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetDesKey(UINT8 u1Index, BOOL fg3Des, BOOL fgEven, const UINT32 au4Key[6])
{
    UINT8 i;
    //UINT32 au4DesMap[6] = { 0, 1, 2, 3, 4, 5};
    //UINT32 au43DesMap[6] = { 0, 1, 2, 3, 4, 5};  //{ 4, 5, 2, 3, 0, 1};  // Swap key-1 and key-3
    //UINT32 u4MappedIndex;

    ASSERT(au4Key != NULL);

    if (u1Index >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    for (i=0; i<6; i++)
    {
        //u4MappedIndex = fg3Des ? au43DesMap[i] : au4DesMap[i];
        if (fgEven)
        {
            DMX_DMEM_CA_EVEN_KEY(u1Index, 7 - i) = au4Key[i];
        }
        else
        {
            DMX_DMEM_CA_ODD_KEY(u1Index, 7 - i) = au4Key[i];
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetAesIV
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetAesIV(UINT8 u1Idx, const UINT32 au4Iv[4], BOOL fgEven)
{
    UINT8 i;

    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    for (i=0; i<4; i++)
    {
        if (fgEven)
        {
            DMX_DMEM_CA_EVEN_IV(u1Idx, 3 - i) = au4Iv[i];
        }
        else
        {
            DMX_DMEM_CA_ODD_IV(u1Idx, 3 - i) = au4Iv[i];
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetAesKeyLen
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetAesKeyLen(UINT8 u1Idx, UINT32 u4KeyLen)
{
    UINT32 u4Ctrl;

    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    if ((u4KeyLen != 128) && (u4KeyLen != 192) && (u4KeyLen != 256))
    {
        return FALSE;
    }

    u4Ctrl = DMX_DMEM_CA_CTRL(u1Idx);
    u4Ctrl &= ~0xC;

    if (u4KeyLen == 128)
    {
        u4Ctrl |= 0x0;
    }
    else if (u4KeyLen == 192)
    {
        u4Ctrl |= 0x4;
    }
    else if (u4KeyLen == 256)
    {
        u4Ctrl |= 0x8;
    }

    DMX_DMEM_CA_CTRL(u1Idx) = u4Ctrl;

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetSMS4Key
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetSMS4Key(UINT8 u1Index, BOOL fgEven, const UINT32 au4Key[4])
{
    UINT8 i;

    ASSERT(au4Key != NULL);

    if (u1Index >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    for (i=0; i<4; i++)
    {
        if (fgEven)
        {
            DMX_DMEM_CA_EVEN_KEY(u1Index, 7 - i) = au4Key[i];
        }
        else
        {
            DMX_DMEM_CA_ODD_KEY(u1Index, 7 - i) = au4Key[i];
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetDVBKey
 *  Set a DVB desrambler key
 *
 *  @param  u1Index         The key index
 *  @param  fgEven          TRUE: even key, FALSE: odd key
 *  @param  au4Key          The key
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetDVBKey(UINT8 u1Index, BOOL fgEven, const UINT32 au4Key[2])
{
    UINT8 i;

    ASSERT(au4Key != NULL);

    if (u1Index >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    for (i=0; i<2; i++)
    {
        if (fgEven)
        {
            DMX_DMEM_CA_EVEN_KEY(u1Index, 7 - i) = au4Key[i];
        }
        else
        {
            DMX_DMEM_CA_ODD_KEY(u1Index, 7 - i) = au4Key[i];
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetDVBIV
 *  Set initial vector of DVB
 *
 *  @param  u4IvHi          The high word of the initial vector
 *  @param  u4IvLo          The low word of the initial vector
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetDVBIV(UINT8 u1Idx, UINT32 u4IvHi, UINT32 u4IvLo, BOOL fgEven)
{
    if (u1Idx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    if (fgEven)
    {
        DMX_DMEM_CA_EVEN_IV(u1Idx, 2) = u4IvLo;
        DMX_DMEM_CA_EVEN_IV(u1Idx, 3) = u4IvHi;
    }
    else
    {
        DMX_DMEM_CA_ODD_IV(u1Idx, 2) = u4IvLo;
        DMX_DMEM_CA_ODD_IV(u1Idx, 3) = u4IvHi;
    }
    return TRUE;
}

#ifdef CC_DMX_SUPPORT_MULTI2
//-----------------------------------------------------------------------------
/** DMX_SetMulti2Key
 *  Set a Multi2 desrambler key
 *
 *  @param  u1Index         The key index
 *  @param  fgEven          TRUE: even key, FALSE: odd key
 *  @param  au4Key          The key
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetMulti2Key(UINT8 u1Index, BOOL fgEven, const UINT32 au4Key[2])
{
    UINT8 i;

    ASSERT(au4Key != NULL);

    if (u1Index >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    for (i=0; i<2; i++)
    {
        if (fgEven)
        {
            DMX_DMEM_CA_KEY(u1Index, 9 - i) = au4Key[i];
        }
        else
        {
            DMX_DMEM_CA_KEY(u1Index, 21 - i) = au4Key[i];
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _Dmx_SetMulti2CbcIV
 *  Set initial vector of MULTI-2 CBC mode
 *
 *  @param  u4IvHi          The high word of the initial vector
 *  @param  u4IvLo          The low word of the initial vector
 *
 *  @retval bool
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetMulti2CbcIV(UINT8 u1Index,UINT32 u4IvHi, UINT32 u4IvLo)
{
    DMX_DMEM_CA_KEY(u1Index, 10) = u4IvLo;        // Even
    DMX_DMEM_CA_KEY(u1Index, 11) = u4IvHi;        // Even
    DMX_DMEM_CA_KEY(u1Index, 22) = u4IvLo;        // Odd
    DMX_DMEM_CA_KEY(u1Index, 23) = u4IvHi;        // Odd

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _Dmx_SetMulti2OfbIV
 *  Set initial vector of MULTI-2 OFB mode
 *
 *  @param  u4IvHi          The high word of the initial vector
 *  @param  u4IvLo          The low word of the initial vector
 *
 *  @retval bool
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetMulti2OfbIV(UINT8 u1Index,UINT32 u4IvHi, UINT32 u4IvLo)
{
    // Not test yet, org
    //DMXCMD_WRITE32(DMX_REG_M2_OFB_IV_LO, u4IvLo);
    //DMXCMD_WRITE32(DMX_REG_M2_OFB_IV_HI, u4IvHi);

    // 5368 new
    DMX_DMEM_CA_KEY(u1Index, 10) = u4IvLo;        // Even
    DMX_DMEM_CA_KEY(u1Index, 11) = u4IvHi;        // Even
    DMX_DMEM_CA_KEY(u1Index, 22) = u4IvLo;        // Odd
    DMX_DMEM_CA_KEY(u1Index, 23) = u4IvHi;        // Odd

    return TRUE;
}


//-----------------------------------------------------------------------------
/** _DMX_SetMulti2SysKey
 *  Set system keys of MULTI-2
 *
 *  @param  au4Key          The system keys
 *
 *  @retval bool
 */
//-----------------------------------------------------------------------------
static BOOL _Dmx_SetMulti2SysKey(UINT8 u1Idx, const UINT32 au4Key[8])
{
    UINT32 i;

    ASSERT(au4Key != NULL);

    for (i = 0; i < 8; i++)
    {
        DMX_DMEM_CA_KEY(u1Idx, i) = au4Key[i];      // Even
        DMX_DMEM_CA_KEY(u1Idx, 12 + i) = au4Key[i];     // Odd
    }

    return TRUE;
}

#endif  // CC_DMX_SUPPORT_MULTI2

static BOOL _DmxSetSecurePvrAesKey0(UINT8 u1KeyIdx, BOOL fgEven, const UINT8 au1CipherKey[32])
{
    UINT32 au4PlainKey[8];
    BOOL fgRet = FALSE;

    if (TZ_SUCCESS == _teeCryptoDecryptSecurePvrAesKey0(au1CipherKey, (UINT8 *)au4PlainKey, sizeof(au4PlainKey))) {

        TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_SECURE_PVR_AES_0, DMX_KEY_IDX_SECURE_PVR, au4PlainKey, sizeof(au4PlainKey));

        _DmxDataSwap(au4PlainKey, au4PlainKey, 8, 1);
        fgRet = DMX_SetAesKey(u1KeyIdx, fgEven, au4PlainKey);
    }

    return fgRet;
}

static BOOL _FvrSetSecurePvrAesKey0(UINT8 u1KeyIdx, BOOL fgEven, const UINT8 au1CipherKey[32])
{
    UINT8 au1PlainKey[32];
    BOOL fgRet = FALSE;

    if (TZ_SUCCESS == _teeCryptoDecryptSecurePvrAesKey0(au1CipherKey, au1PlainKey, sizeof(au1PlainKey))) {
        fgRet = FVR_SetAesKey(u1KeyIdx, fgEven, au1PlainKey);
    }

    return fgRet;
}

static BOOL _DmxSetMMIv(UINT8 u1KeyIdx, const UINT8 *pu1Iv, UINT32 u4IvLen)
{
    UINT32 au4TmpIv[4];
    UINT32 i, j;

    if (u4IvLen != 16)
    {
        return FALSE;
    }

    for(i=0; i<4; i++)
    {
        au4TmpIv[i] = 0;
        for(j=0; j<4; j++)
        {
            au4TmpIv[i] |= ((pu1Iv[(i*4)+j]) << ((3 - j) * 8));
        }
    }

    for (i = 0; i < 4; i++)
    {
        *(volatile UINT32*)(DMX_DMEM_MM_IV_BASE + (4 * i)) = au4TmpIv[3 - i];
    }

    UNUSED(u1KeyIdx);

    return TRUE;
}

static BOOL _DmxReloadCIPlusKey(UINT8 u1KeyIndex, UINT32 u4SecureKeyIdx)
{
#ifdef CC_CI_PLUS_TZ_SUPPORT
    UINT16 u2KeyLen, u2IvLen;
    UINT32 au4Key[8], au4Iv[4];
    UINT8  u1Cipher, u1fgEven;
    BOOL fgRet = TRUE;

    if (u1KeyIndex >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    u2KeyLen = 16;
    u2IvLen = 16;
    fgRet = _teeCIPLUSGetCCK((UINT8)u4SecureKeyIdx, &u1Cipher, &u1fgEven, (UINT8 *)au4Key, &u2KeyLen, (UINT8 *)au4Iv, &u2IvLen);
    if(fgRet == FALSE)
    {
        DMSG("DMX Get CA Secure Key Fail!\n");
        return FALSE;
    }

    _DmxDataSwap(au4Key, au4Key, 8, 1);
    _DmxDataSwap(au4Iv, au4Iv, 4, 1);

    //Check Cipher
    if(u1Cipher == DESC_CIPHER_DES_ECB)
    {
        u1Cipher = DMX_DESC_MODE_DES_ECB;
    }
    else if(u1Cipher == DESC_CIPHER_AES_128_CBC)
    {
        u1Cipher = DMX_DESC_MODE_AES_CBC;
    }
    else
    {
        DMSG("DMX Cipher Unknow!\n");
        return FALSE;
    }
    fgRet = _Dmx_SetDescMode(u1KeyIndex,(DMX_DESC_MODE_T)u1Cipher);
    if(fgRet == FALSE)
    {
        DMSG("DMX SetDescMode Fail!\n");
        return FALSE;
    }

    if((DMX_DESC_MODE_T)u1Cipher == DMX_DESC_MODE_DES_ECB)
    {
        DMX_SetAesKey(u1KeyIndex, (BOOL)(u1fgEven?0:1), au4Key);
    }
    else if((DMX_DESC_MODE_T)u1Cipher == DMX_DESC_MODE_AES_CBC)
    {
        _Dmx_SetAesIV(u1KeyIndex, au4Iv, (BOOL)(u1fgEven?0:1));
        _Dmx_SetAesKeyLen(u1KeyIndex, 128);
        DMX_SetAesKey(u1KeyIndex, (BOOL)(u1fgEven?0:1), au4Key);
    }
    else
    {
        DMSG("DMX Cipher not support!\n");
        return FALSE;
    }
#endif

    return TRUE;
}

static BOOL _DmxReloadMarlinVODKey(UINT8 u1KeyIndex, UINT32 u4SecureKeyIdx)
{
    UINT32 au4Key[8];
    DMX_KEY_IDX_T eKeyIdx = (DMX_KEY_IDX_T)u4SecureKeyIdx;
    BOOL fgEven, fgRet = TRUE;

    if (!TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MARLIN_VOD, eKeyIdx, (UINT8 *)au4Key, sizeof(au4Key)))
    {
        return FALSE;
    }

    _DmxDataSwap(au4Key, au4Key, 8, 1);
    fgEven = (DMX_KEY_IDX_MARLIN_EVEN == eKeyIdx) ? TRUE : FALSE;
    fgRet = DMX_SetAesKey(u1KeyIndex, fgEven, au4Key);

    return fgRet;
}

static BOOL _DmxReloadMMKey(UINT8 u1Pidx, UINT8 u1KeyIndex, UINT32 u4SecureKeyIdx)
{
    DMX_KEY_IDX_T eKeyIdx = (DMX_KEY_IDX_T)u4SecureKeyIdx;
    UINT8 au1SecureKey[32];
    UINT32 i;

    if (!TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MM_DRM, eKeyIdx, au1SecureKey, sizeof(au1SecureKey)))
    {
        return FALSE;
    }

    for(i = 0; i < 8; i++)
    {
        *(volatile UINT32*)(DMX_DMEM_MM_KEY_BASE + ((7 - i) * 4)) =
                                         au1SecureKey[(i*4) + 3] |
                                        (au1SecureKey[(i*4) + 2] << 8) |
                                        (au1SecureKey[(i*4) + 1] << 16) |
                                        (au1SecureKey[i*4] << 24);
    }

    _u1ProtectedPidx = u1Pidx;

#ifdef CC_TVP_SUPPORT
    TZ_TVP_Protection(TZ_TVP_PROT_FLAG_DMX_SET_KEY);
#endif

    UNUSED(u1KeyIndex);

    return TRUE;
}

static BOOL _DmxReloadMMIv(UINT8 u1KeyIndex, UINT32 u4SecureKeyIdx)
{
    DMX_KEY_IDX_T eKeyIdx = (DMX_KEY_IDX_T)u4SecureKeyIdx;
    UINT8 au1SecureIv[16];
    UINT32 au4IV[4];
    UINT32 i, j;

    if (!TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TZ_MM_DRM_IV, eKeyIdx, au1SecureIv, sizeof(au1SecureIv)))
    {
        return FALSE;
    }

    for(i=0; i<4; i++)
    {
        au4IV[i] = 0;
        for(j=0; j<4; j++)
        {
            au4IV[i] |= ((au1SecureIv[(i*4)+j]) << ((3 - j) * 8));
        }
    }

    for (i = 0; i < 4; i++)
    {
        *(volatile UINT32*)(DMX_DMEM_MM_IV_BASE + (4 * i)) = au4IV[3 - i];
    }

    UNUSED(u1KeyIndex);

    return TRUE;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL SvcEx_DMX_Init()
{
    UINT32 u4Value;

    //clear CA KEY and Ctrl
    x_memset((void*)DMX_DMEM_CA_KEY_BASE, 0, 96 * DMX_DMEM_CA_KEY_NUM);
    x_memset((void*)DMX_DMEM_CA_CTRL_BASE, 0, 4 * DMX_DMEM_CA_KEY_NUM);

    //clear DMX_DMEM_MM_KEY
#if (defined(CC_MT5398)|| defined(CC_MT5880)|| defined(CC_MT5881))     
    x_memset((void*)DMX_DMEM_MM_KEY_BASE, 0, DMX_DMEM_MM_KEY_SIZE * 4);
#else
    x_memset((void*)DMX_DMEM_MM_KEY_BASE, 0, DMX_DMEM_MM_KEY_SIZE * 4);
    x_memset((void*)DMX_DMEM_MM_KEY_BASE1, 0, DMX_DMEM_MM_KEY_SIZE * 4);
#endif

    //MT5399 DMX DRAM Zone 0x19000 ~ 0x1CFFF + 0x54000 ~ 0x54FFF
#ifdef CC_MT5399
    UINT32 u4Reg;
    if (IS_IC_5399_ES1())
    {
        //Max protect zone in ES1 is 0xFFF
        u4Value =  0xFFF;

        //Disable FTup data memory security accesss
        u4Reg = DMXCMD_READ32(5) & 0xFFFFFFFD;
        u4Reg |= 0x2;
        DMXCMD_WRITE32(5,u4Reg);
    }
    else
    {
        //Max protect zone in ES2 is 0x1FFF
        //Only MM Key Need Protection
        //0x4000 is 0x19000 ~ 0x1CFFF
        //Need Enalbe FTUP data memory security access
        u4Value =  (DMX_DMEM_MM_KEY_BASE - DEMUX7_BASE + 0x4000) / 4;
    }
#elif defined(CC_MT5890)//CC_MT5890 will include CC_MT5861
    // 0x5000 is 0x19000 ~ 0x1CFFF, 0x54000 ~ 0x54FFF
    // Protect from 0x6AE00 (6016)
    u4Value =  (DMX_DMEM_MM_KEY_BASE - DEMUX9_BASE + 0x5000) / 4;
#elif defined(CC_MT5882)
    // 0x4000 is 0x19000 ~ 0x1BD50, 0x54000 ~ 0x54EA4
    // Protect from 0x0x54EA4 (5033)
    u4Value =  (DMX_DMEM_MM_KEY_BASE - DEMUX7_BASE + 0x4000) / 4;
#else
    u4Value =  (DMX_DMEM_MM_KEY_BASE - DEMUX2_BASE) / 4;
#endif

    // Protect the DMEM MM key region
    DMXCMD_WRITE32(5, (DMXCMD_READ32(5) & 0x0000FFFF) | (u4Value << 16));

    Svc_DMX_MMSecurityInit();

	return TRUE;
}

#define DMX_KEY_WRAPPER(p)  ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]))

BOOL DMX_SetHDCPKeyIv(UINT8 u1Pidx, UINT8 u1KeyIdx, UINT8 *pu1Key, UINT8 *pu1Iv)
{
    UINT8 i;
    UINT8 *p;
    ASSERT(pu1Key != NULL);

    if(u1KeyIdx >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    PID_S_W(u1Pidx, 0) = (PID_S_W(u1Pidx, 0) & (~(0x7 << 0)));

    for (i = 0; i < 4; i++)
    {
        p = pu1Key + i * 4;
        DMX_DMEM_CA_EVEN_KEY(u1KeyIdx, 7 - i) = DMX_KEY_WRAPPER(p);
    }

    p = pu1Iv;
    DMX_DMEM_CA_EVEN_KEY(u1KeyIdx, 11) = DMX_KEY_WRAPPER(p);
    p = pu1Iv + 4;
    DMX_DMEM_CA_EVEN_KEY(u1KeyIdx, 10) = DMX_KEY_WRAPPER(p);

    return TRUE;
}

BOOL DMX_FreeSecureKeyIv(UINT8 u1Idx)
{
    UINT32 i;

    if(_u1ProtectedPidx != u1Idx)
	{
        return TRUE;
	}

	// The current playready module set key/iv flow is not correct ,
	// we just don't clean the key/iv here, you may fix unmark it
	// when it fixed.

    //memset((VOID*)au1SecureKey, 0, sizeof(au1SecureKey));
    //memset((VOID*)au1SecureIv, 0, sizeof(au1SecureIv));

    for (i = 0; i < 4; i++)
    {
        *(volatile UINT32*)(DMX_DMEM_MM_IV_BASE + (4 * i)) = 0;
    }

    for(i = 0; i < 8; i++)
    {
        *(volatile UINT32*)(DMX_DMEM_MM_KEY_BASE + ((7 - i) * 4)) = 0;
    }

#ifdef CC_TVP_SUPPORT
    TZ_TVP_UnProtection(TZ_TVP_PROT_FLAG_DMX_SET_KEY);
#endif

    return TRUE;
}

BOOL TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TYPE_T eKeyType, DMX_KEY_IDX_T eKeyIdx, UINT8 *pu1Key, UINT32 u4KeyLen)
{
    if (eKeyIdx > DMX_KEY_IDX_MAX)
    {
        return FALSE;
    }

    if (!pu1Key)
    {
        return FALSE;
    }

    if (u4KeyLen > DMX_DESC_MAX_KEY_LEN)
    {
        return FALSE;
    }

    switch (eKeyType)
    {
    case DMX_CRYPTO_KEY_TZ_MARLIN_VOD:
        if ((eKeyIdx != DMX_KEY_IDX_MARLIN_EVEN) || (eKeyIdx != DMX_KEY_IDX_MARLIN_ODD))
        {
            return FALSE;
        }
        break;
    case DMX_CRYPTO_KEY_SECURE_PVR_AES_0:
        if (eKeyIdx != DMX_KEY_IDX_SECURE_PVR)
        {
            return FALSE;
        }
        break;
    case DMX_CRYPTO_KEY_TZ_MM_DRM:
        if (eKeyIdx != DMX_KEY_IDX_MM_DRM_PR)
        {
            return FALSE;
        }
        break;
    case DMX_CRYPTO_KEY_TZ_MM_DRM_IV:
        if (eKeyIdx != DMX_KEY_IDX_MM_DRM_PR_IV)
        {
            return FALSE;
        }
        break;
    default:
        DMSG("keyType=%d not supported!\n", eKeyType);
        return FALSE;
    }

    memcpy_s2s_chk(_au1DmxKey[eKeyIdx], pu1Key, u4KeyLen, NULL);

    return TRUE;
}

BOOL TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TYPE_T eKeyType, DMX_KEY_IDX_T eKeyIdx, UINT8 *pu1Key, UINT32 u4KeyLen)
{
    if (eKeyIdx > DMX_KEY_IDX_MAX)
    {
        return FALSE;
    }

    if (!pu1Key)
    {
        return FALSE;
    }

    if (u4KeyLen > DMX_DESC_MAX_KEY_LEN)
    {
        return FALSE;
    }

    switch (eKeyType)
    {
    case DMX_CRYPTO_KEY_TZ_MARLIN_VOD:
        if ((eKeyIdx != DMX_KEY_IDX_MARLIN_EVEN) || (eKeyIdx != DMX_KEY_IDX_MARLIN_ODD))
        {
            return FALSE;
        }
        break;
    case DMX_CRYPTO_KEY_SECURE_PVR_AES_0:
        if (eKeyIdx != DMX_KEY_IDX_SECURE_PVR)
        {
            return FALSE;
        }
        break;
    case DMX_CRYPTO_KEY_TZ_MM_DRM:
        if (eKeyIdx != DMX_KEY_IDX_MM_DRM_PR)
        {
            return FALSE;
        }
        break;
    case DMX_CRYPTO_KEY_TZ_MM_DRM_IV:
        if (eKeyIdx != DMX_KEY_IDX_MM_DRM_PR_IV)
        {
            return FALSE;
        }
        break;
    default:
        DMSG("keyType=%d not supported!\n", eKeyType);
        return FALSE;
    }

    memcpy_s2s_chk(pu1Key, _au1DmxKey[eKeyIdx], u4KeyLen, NULL);

    return TRUE;
}

BOOL SvcEx_DMX_DMX_SetMMKey(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_IO_T *pIo;

    pIo = (TZ_IO_T *)pu1Param;
    if((pIo->u4Base >= DMX_DMEM_MM_KEY_BASE) && (pIo->u4Base < (DMX_DMEM_MM_KEY_BASE + 14 * 4)))
    {
        *(volatile UINT32*)(pIo->u4Base) = pIo->u4Val;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** DMX_SetAesKey
 */
//-----------------------------------------------------------------------------
BOOL DMX_SetAesKey(UINT8 u1Index, BOOL fgEven, const UINT32 au4Key[8])
{
    UINT8 i;

    ASSERT(au4Key != NULL);

    if (u1Index >= DMX_DMEM_CA_KEY_NUM)
    {
        return FALSE;
    }

    //dprintf("[TZ]SetAesKey\n");
    for (i=0; i<8; i++)
    {
        if (fgEven)
        {
            DMX_DMEM_CA_EVEN_KEY(u1Index, 7 - i) = au4Key[i];
        }
        else
        {
            DMX_DMEM_CA_ODD_KEY(u1Index, 7 - i) = au4Key[i];
        }
        //dprintf("%08x ", au4Key[i]);
    }
    //dprintf("\n");

    return TRUE;
}

BOOL SvcEx_DMX_SetCryptoKey(UINT8 *pu1Param)
{
    BOOL fgRet = TRUE, fgEven, fg3Des;
    UINT8 *pu1Key, u1KeyIdx, *pu1Iv, u1Pidx;
    UINT32 u4KeyLen, u4IvHi, u4IvLo;
    UINT64 u8Nonce, u8Counter;
    DMX_CRYPTO_KEY_TYPE_T eKeyType;
    DMX_CRYPTO_KEY_INFO_T *prKeyInfo = (DMX_CRYPTO_KEY_INFO_T *)pu1Param;

    if (!prKeyInfo)
    {
        return FALSE;
    }

    u1KeyIdx = prKeyInfo->u1KeyIdx;
    fgEven = prKeyInfo->fgEven;
    eKeyType = prKeyInfo->eKeyType;

    switch (eKeyType)
    {
    case DMX_CRYPTO_KEY_AES:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rAesKey.au4Key;
        fgRet = DMX_SetAesKey(u1KeyIdx, fgEven, (const UINT32 *)pu1Key);
        break;

    case DMX_CRYPTO_KEY_SECURE_PVR_AES_0:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rAesKey.au4Key;
        fgRet = _DmxSetSecurePvrAesKey0(u1KeyIdx, fgEven, pu1Key);
        break;

    case DMX_CRYPTO_KEY_MM_DRM_IV:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rAesIv.au4Iv;
        u4KeyLen = prKeyInfo->u4KeyLen;
        fgRet = _DmxSetMMIv(u1KeyIdx, pu1Key, u4KeyLen);
        break;

    case DMX_CRYPTO_KEY_AES_IV:
        pu1Iv = (UINT8 *)prKeyInfo->uKey.rAesIv.au4Iv;
        fgRet = _Dmx_SetAesIV(u1KeyIdx, (const UINT32 *)pu1Iv, fgEven);
        break;
    case DMX_CRYPTO_KEY_CA_CTR:
        u8Nonce = prKeyInfo->uKey.rCtrIv.u8Nonce;
        u8Counter = prKeyInfo->uKey.rCtrIv.u8Counter;
        fgRet = _Dmx_SetCA_CTR(u1KeyIdx, u8Nonce, u8Counter, fgEven);
        break;
    case DMX_CRYPTO_KEY_DES_IV:
        u4IvHi = prKeyInfo->uKey.rDesIv.u4IvHi;
        u4IvLo = prKeyInfo->uKey.rDesIv.u4IvLo;
        fgRet = _Dmx_SetDesIV(u1KeyIdx, u4IvHi, u4IvLo, fgEven);
        break;
    case DMX_CRYPTO_KEY_DES:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rDesKey.au4Key;
        fg3Des = prKeyInfo->uKey.rDesKey.fg3Des;
        fgRet = _Dmx_SetDesKey(u1KeyIdx, fg3Des, fgEven, (const UINT32 *)pu1Key);
        break;
    case DMX_CRYPTO_KEY_SMS4:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rSMS4Key.au4Key;
        fgRet = _Dmx_SetSMS4Key(u1KeyIdx, fgEven, (const UINT32 *)pu1Key);
        break;
    case DMX_CRYPTO_KEY_DVB:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rDVBKey.au4Key;
        fgRet = _Dmx_SetDVBKey(u1KeyIdx, fgEven, (const UINT32 *)pu1Key);
        break;
    case DMX_CRYPTO_KEY_DVB_IV:
        u4IvHi = prKeyInfo->uKey.rDVBIv.u4IvHi;
        u4IvLo = prKeyInfo->uKey.rDVBIv.u4IvLo;
        fgRet = _Dmx_SetDVBIV(u1KeyIdx, u4IvHi, u4IvLo, fgEven);
        break;
    case DMX_CRYPTO_KEY_MULTI2:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rMulti2Key.au4Key;
        fgRet = _Dmx_SetMulti2Key(u1KeyIdx, fgEven, (const UINT32 *)pu1Key);
        break;
    case DMX_CRYPTO_KEY_MULTI2_CBC_IV:
        u4IvHi = prKeyInfo->uKey.rMulti2Iv.u4IvHi;
        u4IvLo = prKeyInfo->uKey.rMulti2Iv.u4IvLo;
        fgRet = _Dmx_SetMulti2CbcIV(u1KeyIdx, u4IvHi, u4IvLo);
        break;
    case DMX_CRYPTO_KEY_MULTI2_OFB_IV:
        u4IvHi = prKeyInfo->uKey.rMulti2Iv.u4IvHi;
        u4IvLo = prKeyInfo->uKey.rMulti2Iv.u4IvLo;
        fgRet = _Dmx_SetMulti2OfbIV(u1KeyIdx, u4IvHi, u4IvLo);
        break;
    case DMX_CRYPTO_KEY_MULTI2_SYS:
        pu1Key = (UINT8 *)prKeyInfo->uKey.rMulti2SysKey.au4Key;
        fgRet = _Dmx_SetMulti2SysKey(u1KeyIdx, (const UINT32 *)pu1Key);
        break;
    case DMX_CRYPTO_KEY_HDCP:
        u1Pidx = prKeyInfo->uKey.rHDCPKey.u1Pidx;
        pu1Key = (UINT8 *)prKeyInfo->uKey.rHDCPKey.au4Key;
        pu1Iv = (UINT8 *)prKeyInfo->uKey.rHDCPKey.au4Iv;
        fgRet = DMX_SetHDCPKeyIv(u1Pidx, u1KeyIdx, pu1Key, pu1Iv);
        break;

    default:
        fgRet = FALSE;
        DMSG("keyType=%d not supported!\n", eKeyType);
        break;
    }

    return fgRet;
}

BOOL FVR_SetAesKey(UINT8 u1KeyIdx, BOOL fgEven, UINT8 *pu1Key)
{
    UINT32 i;

    if (pu1Key == NULL)
    {
        return FALSE;
    }

    if (u1KeyIdx >= FVR_KEY_COUNT)
    {
        return FALSE;
    }

    dprintf("[TZ]SetFvrKey\n");
    if (fgEven)
    {
        for (i=0; i<8; i++)
        {
            FVR_KEY_EVEN_KEY(u1KeyIdx, 7 - i) =
                pu1Key[(i*4) + 3] |
                (pu1Key[(i*4) + 2] << 8) |
                (pu1Key[(i*4) + 1] << 16) |
                (pu1Key[i*4] << 24);

            dprintf("%08x ", FVR_KEY_EVEN_KEY(u1KeyIdx, 7 - i));
        }
    }
    else
    {
        for (i=0; i<8; i++)
        {
            FVR_KEY_ODD_KEY(u1KeyIdx, 7 - i) =
                pu1Key[(i*4) + 3] |
                (pu1Key[(i*4) + 2] << 8) |
                (pu1Key[(i*4) + 1] << 16) |
                (pu1Key[i*4] << 24);

            dprintf("%08x ", FVR_KEY_ODD_KEY(u1KeyIdx, 7 - i));
        }
    }
    dprintf("\n");

    return TRUE;
}

BOOL SvcEx_FVR_SetCryptoKey(UINT8 *pu1Param)
{
    BOOL fgRet;

    FVR_CRYPTO_KEY_INFO_T *prKeyInfo = (FVR_CRYPTO_KEY_INFO_T *)pu1Param;

    if (!prKeyInfo)
    {
        return FALSE;
    }

    switch (prKeyInfo->eKeyType)
    {
    case FVR_CRYPTO_KEY_AES:
        fgRet = FVR_SetAesKey(prKeyInfo->u1KeyIdx, prKeyInfo->fgEven,
                            (UINT8 *)prKeyInfo->uKey.rAesKey.au4Key);
        break;

    case FVR_CRYPTO_KEY_SECURE_PVR_AES_0:
        fgRet = _FvrSetSecurePvrAesKey0(prKeyInfo->u1KeyIdx, prKeyInfo->fgEven,
                                        (const UINT8 *)prKeyInfo->uKey.rAesKey.au4Key);
        break;

    default:
        fgRet = FALSE;
        DMSG("keyType=%d not supported!\n", prKeyInfo->eKeyType);
        break;
    }

    return fgRet;
}

BOOL SvcEx_DMX_ReloadSecureKey(UINT8 *pu1Param)
{
    UINT8 u1KeyIndex, u1Pidx;
    UINT32 u4SecureKeyIdx;
    BOOL fgRet = TRUE;
    DMX_CRYPTO_KEY_TYPE_T eKeyType;
    DMX_CA_SECURE_KEY_INFO_T *prKeyInfo = (DMX_CA_SECURE_KEY_INFO_T *)pu1Param;

    if (!prKeyInfo)
    {
        return FALSE;
    }

    u1Pidx = prKeyInfo->u1Pidx;
    u1KeyIndex = prKeyInfo->u1KeyIdx;
    u4SecureKeyIdx = prKeyInfo->u4SecureKeyIdx;
    eKeyType = prKeyInfo->eKeyType;

    switch (eKeyType)
    {
    case DMX_CRYPTO_KEY_TZ_DTV_CIPLUS:
        fgRet = _DmxReloadCIPlusKey(u1KeyIndex, u4SecureKeyIdx);
        break;
    case DMX_CRYPTO_KEY_TZ_MARLIN_VOD:
        fgRet = _DmxReloadMarlinVODKey(u1KeyIndex, u4SecureKeyIdx);
        break;
    case DMX_CRYPTO_KEY_TZ_MM_DRM:
        fgRet = _DmxReloadMMKey(u1Pidx, u1KeyIndex, u4SecureKeyIdx);
        break;
    case DMX_CRYPTO_KEY_TZ_MM_DRM_IV:
        fgRet = _DmxReloadMMIv(u1KeyIndex, u4SecureKeyIdx);
        break;
    default:
        return FALSE;
    }

    return fgRet;
}

