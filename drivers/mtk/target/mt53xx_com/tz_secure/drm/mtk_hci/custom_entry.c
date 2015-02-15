/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#include "x_typedef.h"
#include "x_assert.h"
#include "x_bim.h"

#include "basic_types.h"
#include "binstr_api.h"

//External header file
#ifndef CC_SECURE_WORLD
#include "x_dt.h"
#endif
#include "x_os.h"
#include "s_heap_func.h"

#include "dmx_drm_drvif.h"

//=========================================================================================
#define PD_KEY_SIZE (16)

static UINT8 au1ContentKey[32];
static UINT8 au1DeviceKey[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
static UINT32 u4DeviceKeySize = 16;

/* inital vector for platform dependent vendor cipher */
static UINT8 au1PdIv[PD_KEY_SIZE] =
{
    0xa3, 0x55, 0x50, 0x5c,
    0xb0, 0xfe, 0x23, 0xb3,
    0x78, 0x7a, 0x72, 0xc4,
    0x8c, 0xa9, 0xe5, 0xa7
};

/* inital vector for platform dependent vendor cipher */
static UINT8 au1PdKey[PD_KEY_SIZE] =
{
    0x91, 0x02, 0x25, 0x88,
    0x69, 0x57, 0xce, 0xd2,
    0xfa, 0x95, 0x7f, 0xc3,
    0x62, 0xf6, 0xd7, 0x8b
};


//=========================================================================================
void *TzSecureMemoryAllocate(size_t size)
{
    return (void *)x_mem_alloc (size);
}

void TzSecureMemoryRelease(void *ptr)
{
    if (ptr != NULL)
    {
        x_mem_free(ptr);
    }
    
    return;
}


/// MTK SHA256 Init Crypto API
/// \return None.
void x_mtk_tz_in_api_sha256_Init(
    UINT32* pbHandle            ///< [OUT] SHA1 CTX Handle
)
{
    SHA256_CONTEXT_T *prContext;

    *(pbHandle) = (UINT32) 0;

    prContext = (SHA256_CONTEXT_T *)x_alloc_s_dram_mem(sizeof(SHA256_CONTEXT_T));

    if (prContext == NULL)
    {
        ASSERT(0);
        return;
    }

    *(pbHandle) = (UINT32) prContext;
    SHA256_Reset((SHA256_CONTEXT_T *) *pbHandle);
}

/// MTK SHA256 Update Crypto API
/// \return None.
void x_mtk_tz_in_api_sha256_Update(
    UINT32* pbHandle,           ///< [IN] SHA256 CTX Handle
    UINT32 u4Len,               ///< [IN] SHA256 Message Length (Unit: Byte)
    UINT8* pbbuf                ///< [IN] SHA256 Message Buffer Address
)
{
    SHA256_Input((SHA256_CONTEXT_T*) *pbHandle, pbbuf, u4Len);
}

/// MTK SHA256 Final Crypto API
/// \return None.
void x_mtk_tz_in_api_sha256_Final(
    UINT32* pbHandle,           ///< [IN] SHA256 CTX Handle
    UINT8* pbout_digest         ///< [OUT] SHA256 Hash Result Buffer Address (32 Byte)
)
{
    SHA256_Result((SHA256_CONTEXT_T*) *pbHandle, pbout_digest);

    if (*pbHandle != 0)
    {
        x_free_s_dram_mem((void*) (*pbHandle));
    }
}

/// MTK SHA256 Crypto API
/// \return None.
void x_mtk_tz_in_api_sha256_MakeDigest(
    UINT8* pbbuf,               ///< [IN] Source Message Buffer Address
    UINT32 u4len,               ///< [IN] Source Message Data Length (Unit: Byte)
    UINT8* pbout_digest         ///< [OUT] Hash Result Buffer Address
)
{
    SHA256_CONTEXT_T rContext;

    SHA256_Reset(&rContext);
    SHA256_Input(&rContext, pbbuf, u4len);
    SHA256_Result(&rContext, pbout_digest);
}

/// MTK SHA1 Init Crypto API
/// \return None.
void x_mtk_tz_in_api_sha1_Init(
    UINT32* pbHandle            ///< [OUT] SHA1 CTX Handle
)
{
    SHA1_CONTEXT_T *prContext;

    *(pbHandle) = (UINT32) 0;

    prContext = (SHA1_CONTEXT_T *)x_alloc_s_dram_mem(sizeof(SHA1_CONTEXT_T));

    if (prContext == NULL)
    {
        ASSERT(0);
        return;
    }

    *(pbHandle) = (UINT32) prContext;
    SHA1_Reset((SHA1_CONTEXT_T*) *pbHandle);
}

/// MTK SHA1 Update Crypto API
/// \return None.
void x_mtk_tz_in_api_sha1_Update(
    UINT32* pbHandle,           ///< [IN] SHA1 CTX Handle
    UINT32 u4Len,               ///< [IN] SHA1 Message Length (Unit: Byte)
    UINT8* pbbuf                ///< [IN] SHA1 Message Buffer Address
)
{
    SHA1_Input((SHA1_CONTEXT_T*) *pbHandle, pbbuf, u4Len);
}

/// MTK SHA1 Final Crypto API
/// \return None.
void x_mtk_tz_in_api_sha1_Final(
    UINT32* pbHandle,           ///< [IN] SHA1 CTX Handle
    UINT8* pbout_digest         ///< [IN] SHA1 Hash Result Buffer Address (20 Byte)
)
{
    SHA1_Result((SHA1_CONTEXT_T*) *pbHandle, pbout_digest);

    if (*pbHandle != 0)
    {
        x_free_s_dram_mem((void*) (*pbHandle));
    }
}

/// MTK SHA1 Crypto API
/// \return None.
void x_mkt_tz_in_api_sha1_MakeDigest(
    UINT8* pbbuf,               ///< [IN] Source Message Buffer Address
    UINT32 u4len,               ///< [IN] Source Message Data Length (Unit: Byte)
    UINT8* pbout_digest         ///< [OUT] Hash Result Buffer Address
)
{
    SHA1_CONTEXT_T rContext;

    SHA1_Reset(&rContext);
    SHA1_Input(&rContext, pbbuf, u4len);
    SHA1_Result(&rContext, pbout_digest);
}

/// MTK AES 128Bit ECB Encryption Crypto API
/// \return None.
void x_mtk_tz_in_api_aes128e_ecb(
    UINT8* pbKey,               ///< [IN] Encruption Key Buffer Address
    UINT8* bpin_buf,            ///< [IN] Input Data Buffer Address
    UINT8* bpout_buf            ///< [OUT] Encryption Result Buffer Address
)
{
    aes_context rCtx;
    aes_setkey_enc(&rCtx, pbKey, 128);
    aes_crypt_ecb(&rCtx, AES_ENCRYPT, bpin_buf, bpout_buf);
}


/// MTK AES 128Bit ECB Decryption Crypto API
/// \return None.
void x_mtk_tz_in_api_aes128d_ecb(
    UINT8* pbKey,               ///< [IN] Decryption Key Buffer Address
    UINT8* bpin_buf,            ///< [IN] Input Data Buffer Address
    UINT8* bpout_buf            ///< [OUT] Decryption Result Buffer Address
)
{
    aes_context rCtx;
    aes_setkey_dec(&rCtx, pbKey, 128);
    aes_crypt_ecb(&rCtx, AES_DECRYPT, bpin_buf, bpout_buf);
}

/// MTK Random Number Generator
/// \return None.
void x_mtk_tz_in_api_rng(
    UINT8* pbRandomDataBuf,     ///< [OUT] Random Data Buffer Address
    UINT32 u4DataLength         ///< [IN] Random Data Length (Unit: Byte)
)
{
    // we don't have hardware random generator, so here is just a SW one.
    UINT32 i;

    if (pbRandomDataBuf == NULL)
    {
        return;
    }

    for (i = 0; i < u4DataLength; i++)
    {
        *(UINT8 *)(pbRandomDataBuf + i) = (UINT8)(rand() % 256);
    }
}


static BOOL _EncryptPdAes(UINT8 *pu1Dst, UINT8 *pu1Src, UINT32 u4Size)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16];
    UINT8* pu1SrcBuf = (UINT8*)pu1Src;
    UINT8* pu1DstBuf = (UINT8*)pu1Dst;
    BOOL fgCbc = TRUE;

    // Setup key scheduling
    aes_setkey_enc(&rCtx, au1PdKey, PD_KEY_SIZE * 8);

    if (fgCbc)
    {
        x_memcpy(au1InitVector, au1PdIv, PD_KEY_SIZE);
    }


    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        x_memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text

        // Encrypt
        if (fgCbc)
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

    x_memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back
    return TRUE;
}

static BOOL _DecryptPdAes(UINT8 *pu1Dst, UINT8 *pu1Src, UINT32 u4Size)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16];
    UINT8* pu1SrcBuf = (UINT8*)pu1Src;
    UINT8* pu1DstBuf = (UINT8*)pu1Dst;
    BOOL fgCbc = TRUE;

    // Setup key scheduling
    aes_setkey_dec(&rCtx, au1PdKey, PD_KEY_SIZE * 8);

    if (fgCbc)
    {
        x_memcpy(au1InitVector, au1PdIv, PD_KEY_SIZE);
    }

    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        x_memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text

        // Encrypt
        if (fgCbc)
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

BOOL x_mtk_tz_in_api_vendor_enc_pd0(
    UINT8* pui1_src,            ///< [IN]     source
    UINT8* pui1_des,            ///< [IN/OUT] destination
    UINT32 ui4_sz               ///< [IN]     size of source/destination, 16 byte alignment
)
{
    return _EncryptPdAes(pui1_des, pui1_src, ui4_sz);
}

BOOL x_mtk_tz_in_api_vendor_dec_pd0(
    UINT8* pui1_src,            ///< [IN]     source
    UINT8* pui1_des,            ///< [IN/OUT] destination
    UINT32 ui4_sz               ///< [IN]     size of source/destination, 16 byte alignment
)
{
    return _DecryptPdAes(pui1_des, pui1_src, ui4_sz);
}

/// MTK Set Key API for Nautilius
/// \return If return value = FALSE, it's failed.
BOOL x_mtk_tz_in_api_cpsa_marlin_set_content_key (
  void* pv_priv,                ///< [IN] Private Data for MTK Internal Use (Get this data pointer from Get Content Key API)
  UINT8* pui1_content_key,      ///< [IN] Marlin Content Key Pointer
  UINT32* pui4_content_key_sz   ///< [IN] Maile Content Key Size
)
{
    if (*pui4_content_key_sz > 32)
    {
        return FALSE;
    }

    x_memcpy(au1ContentKey, pui1_content_key, *pui4_content_key_sz);

    return TRUE;
}

// query device key size
BOOL x_mtk_tz_in_api_marlin_get_dev_key_sz(
    UINT32* pui4_sz          ///< [IN/OUT] device key size
)
{
    *pui4_sz = u4DeviceKeySize;

    return TRUE;
}

BOOL x_mtk_tz_in_api_marlin_get_dev_key(
    UINT8* pui1_dev_key      ///< [IN/OUT] device key
)
{
    if (pui1_dev_key == NULL)
    {
        return;
    }

    x_memcpy(pui1_dev_key, au1DeviceKey, u4DeviceKeySize);

    return TRUE;
}

UINT32 HAL_GetFineTick()
{
#if 0
#ifndef CC_SECURE_WORLD
    HAL_RAW_TIME_T t1;

    HAL_GetRawTime(&t1);

    return t1.u4Cycles;
#else
    ///TODO: implemetation
    return 0;
#endif
#endif
    return 0;
}

np_size_t
npi_str_Len(const char *in_buf)
{
    np_size_t len = 0;
    while ( in_buf[len] ) {
        len ++;
    }
    return len;
}

int32_t
npi_str_NCmp(const char *in_str1, const char *in_str2, size_t in_n)
{
    int32_t i = 0;
    while ( i < in_n && in_str1[i] ) {
        if ( in_str1[i] != in_str2[i] ) {
            return (in_str1[i] > in_str2[i]) ? 1 : -1;
        }
        i ++;
    }
    return (i == in_n) ? 0 : in_str2[i] ? -1 : 0;
}

const char *
npi_str_Str(const char *in_str1, const char *in_str2)
{
    const char *p = in_str1;
    int32_t l = npi_str_Len(in_str2);
    while ( *p ) {
        if ( npi_str_NCmp(p, in_str2, l) == 0 ) {
            return p;
        }
        p ++;
    }
    return NULL;
}
