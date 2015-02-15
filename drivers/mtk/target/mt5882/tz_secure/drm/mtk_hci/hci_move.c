/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */
/* cmn/include */
#include "basic_types.h"
/* sec/include */
#include "cmn_api_sec.h"
/* cmn */
#include "dbg_api.h"
#include "binstr_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"

/*---------------------------------------------------------------------------*/
/**
 * @brief MTK private AES cipher
 *
 * @param
 */
/*---------------------------------------------------------------------------*/
#include "aes.h"

#define MS3_SIGNATURE "ms3:nemo"

typedef enum
{
    DMX_DRM_MODE_BYPASS,
    DMX_DRM_MODE_AES,
    DMX_DRM_MODE_AES_SCTE52,
    DMX_DRM_MODE_ND,
    DMX_DRM_MODE_PD,
    DMX_DRM_MODE_AES_ENCRYPTION,
    DMX_DRM_MODE_UNKNOWN
} DMX_DRM_MODE_T;

typedef struct
{
    UINT16 u2KeyLen;
    UINT8 au1Key[32];
    UINT8 au1InitVector[16];
    BOOL fgCbc;
    DMX_DRM_MODE_T eMode;
} DRM_PARAM_T;

UINT8 au1MtkSecretKey[32] = {0x09, 0xF8, 0x2D, 0xCF, 0x62, 0xCA, 0x2D, 0x9C, 0x0F, 0x9B, 0x01, 0x2E, 0xB3, 0x96, 0x67, 0xDE,
						0x72, 0x30, 0xBA, 0x44, 0x21, 0x84, 0x98, 0xEF, 0x66, 0x49, 0xF8, 0xCD, 0x18, 0xBE, 0x6C, 0xCD};

BOOL _MtkAesDecryption(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16], au1Save[32];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;
	DRM_PARAM_T rParam;
    UINT32 i;

	// setup parameter
	rParam.u2KeyLen = 256;
	rParam.fgCbc = TRUE;
	rParam.eMode = DMX_DRM_MODE_AES_SCTE52;

	for(i = 0; i < 32; i++)
	{
		rParam.au1Key[i] = au1MtkSecretKey[i];
	}

	for(i = 0; i < 16; i++)
	{
		rParam.au1InitVector[i] = i;
	}

    // Setup key scheduling
    aes_setkey_dec(&rCtx, rParam.au1Key, rParam.u2KeyLen);

    if (rParam.fgCbc)
    {
        memcpy(au1InitVector, rParam.au1InitVector, 16);
    }

    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text
		memcpy(au1Save, pu1SrcBuf + u4Offset, 16);

        // descrypt
        if (rParam.fgCbc)
        {
            aes_crypt_cbc(&rCtx, AES_DECRYPT, 16, au1InitVector, au1Text,
                au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_DECRYPT, au1Text, au1Text);
        }

        memcpy(pu1DstBuf + u4Offset, au1Text, 16);    // Copy back
    }

	if((rParam.eMode == DMX_DRM_MODE_AES_SCTE52) &&
        (rParam.fgCbc == TRUE) && ((u4Size - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        UINT8 au1Texto[32];
        u1Cnt = (UINT8)(u4Size - u4Offset);
        memcpy(au1Text, pu1SrcBuf + u4Offset, u1Cnt);   // Get plain text
		aes_setkey_enc(&rCtx, rParam.au1Key, rParam.u2KeyLen);
        aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1Save, au1Text, au1Texto);
        memcpy(pu1DstBuf + u4Offset, au1Texto, u1Cnt);  // Copy back
    }
    else
    {
        memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back
    }

    return TRUE;
}


static BOOL _MtkAesEncryption(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size)
{
    aes_context rCtx;
    UINT32 u4Offset;
	UINT8 au1Text[32], au1InitVector[16], au1Save[32];
    UINT8* pu1SrcBuf = (UINT8*)u4SrcAddr;
    UINT8* pu1DstBuf = (UINT8*)u4DstAddr;
	DRM_PARAM_T rParam;
    UINT32 i;

	// setup parameter
	rParam.u2KeyLen = 256;
	rParam.fgCbc = TRUE;
	rParam.eMode = DMX_DRM_MODE_AES_SCTE52;

	for(i = 0; i < 32; i++)
	{
		rParam.au1Key[i] = au1MtkSecretKey[i];
	}

	for(i = 0; i < 16; i++)
	{
		rParam.au1InitVector[i] = i;
	}

    // Setup key scheduling
    aes_setkey_enc(&rCtx, rParam.au1Key, rParam.u2KeyLen);

    if (rParam.fgCbc)
    {
        memcpy(au1InitVector, rParam.au1InitVector, 16);
    }

    for (u4Offset = 0; u4Offset <= u4Size - 16; u4Offset += 16)
    {
        memcpy(au1Text, pu1SrcBuf + u4Offset, 16);    // Get plain text
		memcpy(au1Save, pu1SrcBuf + u4Offset, 16);

        // descrypt
        if (rParam.fgCbc)
        {
            aes_crypt_cbc(&rCtx, AES_ENCRYPT, 16, au1InitVector, au1Text,
                au1Text);
        }
        else
        {
            aes_crypt_ecb(&rCtx, AES_ENCRYPT, au1Text, au1Text);
        }

        memcpy(pu1DstBuf + u4Offset, au1Text, 16);    // Copy back
    }

	if((rParam.eMode == DMX_DRM_MODE_AES_SCTE52) &&
        (rParam.fgCbc == TRUE) && ((u4Size - u4Offset) != 0))
    {
        UINT8 u1Cnt;
        UINT8 au1Texto[32];
        u1Cnt = (UINT8)(u4Size - u4Offset);
        memcpy(au1Text, pu1SrcBuf + u4Offset, u1Cnt);   // Get plain text
		aes_setkey_enc(&rCtx, rParam.au1Key, rParam.u2KeyLen);
        aes_crypt_cbc_scte52(&rCtx, AES_ENCRYPT, u1Cnt, au1Save, au1Text, au1Texto);
        memcpy(pu1DstBuf + u4Offset, au1Texto, u1Cnt);  // Copy back
    }
    else
    {
        memcpy(pu1DstBuf + u4Offset, pu1SrcBuf + u4Offset, u4Size - u4Offset);    // Copy back
    }

    return TRUE;
}


#if 1 == HCI_L_API_HCI_MOVEKEY
uint32_t
hci_MoveKeySize(hci_key_ref_t   io_ref,
            uint32_t        in_private_data_len,
            uint8_t        *in_private_data,
            uint32_t       *out_private_data_len,
            uint8_t       **out_private_data)
{
    uint32_t         ret     = ERR_HCI_OK;
    hci_l_key_ref_t *ref     = io_ref;
    binstr_t        *key     = NULL;
    uint8_t         *key_buf = NULL;
    hci_bool_t       usable  = HCI_FALSE;

    MSGENT(("hci_MoveKey"));

    RET( hci_l_IsHciLocked() );

    /* in_private_data can be an empty string.
     * In this reference implementation, key data must be returned to the application,
     * so out_private_data must be returned to the caller */
    if ( (NULL == ref)                                         ||
         (NULL == out_private_data_len)                        ||
         (NULL == out_private_data)                            ||
         (0 == in_private_data_len && NULL != in_private_data) ||
         (0 != in_private_data_len && NULL == in_private_data) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if (NULL == ref->key_data) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    RET( hci_l_CheckKeyAttr(ref->key_type, HCI_L_ATTR_MOVE, &usable) );
    if ( HCI_TRUE != usable ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_Unscramble(ref->key_data, &key) );
    // encryption by mtk key
    if (NULL == in_private_data || memcmp(in_private_data, MS3_SIGNATURE, sizeof(MS3_SIGNATURE)-1))
        _MtkAesEncryption(key->buf, key->buf, key->len);    
    RET(hci_l_mem_AllocWithCopy(binstr_Len(key), binstr_Buf(key), &key_buf));

    *out_private_data_len = binstr_Len(key);
    *out_private_data     = key_buf;
    key_buf               = NULL;
/*
    hci_l_FreeKey(ref->key_data);
    ref->key_data = NULL;
*/
EXIT:
    if ( NULL != key_buf ) {
        hci_l_DataScrub(key_buf, binstr_Len(key));
        hci_FreeMem(key_buf);
    }
    hci_l_FreeKey(key);
    MSGEXT(("hci_MoveKey"));
    return ret;
}

uint32_t
hci_MoveKey(hci_key_ref_t   io_ref,
            uint32_t        in_private_data_len,
            uint8_t        *in_private_data,
            uint32_t       *out_private_data_len,
            uint8_t       **out_private_data)
{
    uint32_t         ret     = ERR_HCI_OK;
    hci_l_key_ref_t *ref     = io_ref;
    binstr_t        *key     = NULL;
    uint8_t         *key_buf = NULL;
    hci_bool_t       usable  = HCI_FALSE;

    MSGENT(("hci_MoveKey"));

    RET( hci_l_IsHciLocked() );

    /* in_private_data can be an empty string.
     * In this reference implementation, key data must be returned to the application,
     * so out_private_data must be returned to the caller */
    if ( (NULL == ref)                                         ||
         (NULL == out_private_data_len)                        ||
         (NULL == out_private_data)                            ||
         (0 == in_private_data_len && NULL != in_private_data) ||
         (0 != in_private_data_len && NULL == in_private_data) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if (NULL == ref->key_data) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    RET( hci_l_CheckKeyAttr(ref->key_type, HCI_L_ATTR_MOVE, &usable) );
    if ( HCI_TRUE != usable ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_Unscramble(ref->key_data, &key) );
	// encryption by mtk key
    if (NULL == in_private_data || memcmp(in_private_data, MS3_SIGNATURE, sizeof(MS3_SIGNATURE)-1))
        _MtkAesEncryption(key->buf, key->buf, key->len);
    RET(hci_l_mem_AllocWithCopy(binstr_Len(key), binstr_Buf(key), &key_buf));

    *out_private_data_len = binstr_Len(key);
    *out_private_data     = key_buf;
    key_buf               = NULL;
/*
    hci_l_FreeKey(ref->key_data);
    ref->key_data = NULL;
*/

EXIT:
    if ( NULL != key_buf ) {
        hci_l_DataScrub(key_buf, binstr_Len(key));
        hci_FreeMem(key_buf);
    }
    hci_l_FreeKey(key);
    MSGEXT(("hci_MoveKey"));
    return ret;
}
#endif /* #if 1 == HCI_L_API_HCI_MOVEKEY*/
