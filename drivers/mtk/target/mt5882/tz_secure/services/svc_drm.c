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

//#include "aes.h"

#include "tzst.h"
#include "svc_registry.h"
#include "mtskb.h"
#ifdef CC_ENABLE_HDCP2
#include "mthdcp2.h"
#include "drm_hdcp2.h"
#define TEE_USE_MTDRM_H
#include "mtdrm.h"
#endif
#include "x_chipreg.h"
#include "tz_dmx_if.h"
#ifdef CC_ENABLE_WIDEVINE_EME
#include "svc_wv_if.h"
#endif

#include "tee_common.h"
#include <tomcrypt.h>
#include "tee_securestorage.h"



//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define WV_KEYBOX_DEVICE_ID_OFFSET          (0)
#define WV_KEYBOX_DEVICE_KEY_OFFSET         (32)
#define WV_KEYBOX_DEVICE_DATA_OFFSET        (48)
#define WV_KEYBOX_SIZE                      (128)
#define WIDEVINE_EME_MAPTBL_SIZE            (16)

#define HCI_MAPTBL_SIZE                     (16)
//#define HCI_MAPTBL_SIZE                     (1024) // For fishrod test
#define CC_HCI_HANDLE_MAP                   (1)


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT8 g_au1Keybox[256] = {0}; //ecnrypted device key

static UINT8 g_OEMKey[16] = { 0xC5, 0xC5, 0x93, 0xEB, 0x32, 0x89, 0x0A, 0xDA,
                               0x15, 0x3C, 0x9F, 0xA2, 0x1F, 0x0E, 0xDE, 0x01};

static UINT8 g_au1AssetKey[16] = {0};
static UINT8 g_au1ContorlWord[16] = {0};
static VOID* g_pvMapTbl[HCI_MAPTBL_SIZE] = {0};

static INT32 g_handle_count = 0;
//static INT32 g_handle_count_max = 0;

static UINT32 g_pvMapTbl_wveme[WIDEVINE_EME_MAPTBL_SIZE +1] = {0};
static INT32 g_handle_count_wveme = 0;

static hash_state t_hash_state;
static BYTE au1_seed[128]={
0xA1, 0xC2, 0x86, 0x84, 0x75, 0x66, 0xF9, 0x01, 0xC8, 0x31, 0xA4, 0x4E, 0xE8, 0xC4, 0xD3, 0x7C, 
0xD5, 0xE8, 0x6D, 0x1A, 0x79, 0xF7, 0xCF, 0xFC, 0xE1, 0x05, 0x86, 0xA8, 0x32, 0x1E, 0xAD, 0x2D, 
0x31, 0x13, 0xE4, 0xE3, 0x20, 0xEF, 0xB6, 0x89, 0x2A, 0xFB, 0x32, 0x41, 0xDB, 0x4D, 0x26, 0x03, 
0x51, 0x9A, 0xBB, 0xF7, 0x2B, 0x44, 0x89, 0x77, 0x24, 0x2E, 0x17, 0x04, 0x4F, 0x0F, 0xC5, 0xD2, /*replace this line with secure id*/ 
0x8E, 0xD0, 0x35, 0x54, 0x60, 0xB1, 0x86, 0xD4, 0x0A, 0xA1, 0x69, 0x32, 0xA3, 0x0C, 0x59, 0xDC, 
0x5A, 0xEE, 0xFD, 0xEC, 0x3A, 0xD7, 0x9B, 0x2B, 0x7E, 0x69, 0xE0, 0x8B, 0xF2, 0xAF, 0x8C, 0x0C, 
0xFE, 0xC3, 0x53, 0xB2, 0xE6, 0x42, 0xF6, 0x4E, 0x63, 0xE0, 0x48, 0xDF, 0xE5, 0xB7, 0xE5, 0x6E, 
0xED, 0x24, 0x94, 0x7E, 0x34, 0x9B, 0xBB, 0xB3, 0x4B, 0xA2, 0xC6, 0x71, 0x20, 0x91, 0xC1, 0xAF};
static BYTE au1_md[SHA512_DIGEST_LENGTH];



//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static BOOL _SvcEx_DRM_GetRomBytes(UINT32 u4Offset, UINT8* pu1Buf, UINT32 u4Size)
{
    memcpy_n2n_chk(pu1Buf, (void *)(0xf4000000 + u4Offset), u4Size, NULL);
    return TRUE;
}

static BOOL _SvcEx_DRM_GetSecureID(UINT8* pu1Buf, UINT32 u4Size)
{
	UINT8 i=0;
    UINT32 *pu4SID = (UINT32 *)pu1Buf;

    BIM_WRITE32(REG_EFUSE_KEY, 0x883);
    BIM_WRITE32(REG_EFUSE_KEY, 0x1404);

    pu4SID[1] = BIM_READ32(REG_EFUSE_SID_1);
    pu4SID[3] = BIM_READ32(REG_EFUSE_SID_3);
    pu4SID[0] = BIM_READ32(REG_EFUSE_SID_0);
    pu4SID[2] = BIM_READ32(REG_EFUSE_SID_2);

	/* Compute SHA-512 */
	for (i=0; i<4; i++)
	{
		au1_seed[32*i] = (BYTE)(pu4SID[i]&0xFF);		
		au1_seed[32*i+8] = (BYTE)((pu4SID[i]&0xFF00)>>8);		
		au1_seed[32*i+16] = (BYTE)((pu4SID[i]&0xFF0000)>>16);		
		au1_seed[32*i+24] = (BYTE)((pu4SID[i]&0xFF000000)>>24);
	}
	sha512_init(&t_hash_state);
	sha512_process(&t_hash_state, au1_seed, 128);
	sha512_done(&t_hash_state, au1_md);

	/* Select 16 bytes as SecureID*/
	for (i=0; i<4; i++)
	{
		pu4SID[i] = (au1_md[i*16+12]<<24)|(au1_md[i*16+8]<<16)
			|(au1_md[i*16+4]<<8)|(au1_md[i*16]);
		dprintf("SID[%u]:0x%x,0x%x,0x%x,0x%x\n", i, au1_md[i*16+12], au1_md[i*16+8],
		au1_md[i*16+4], au1_md[i*16]);
	}
	dprintf("pu4SID: 0x%08x,0x%08x,0x%08x,0x%08x\n", pu4SID[0], pu4SID[1],
		pu4SID[2], pu4SID[3]);

    return TRUE;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
BOOL SvcEx_DRM_WV_SetAssetkey(UINT8* pu1EncAssetKey, UINT32 u4KeyLen)
{
    UINT8 au1ClearDeviceKey[16];
    aes_context ctx;
    UINT8 i;

    //HalInvalidateDCacheMultipleLine((UINT32)pu1EncAssetKey, u4KeyLen);
    //Mtk_WVReadKeybox(g_au1Keybox, 256);

#if 1
	// Decrypt [AES-128-ECB] the device key in the keybox with the MTK root key
	aes_setkey_dec(&ctx, g_OEMKey, (INT32)128);
	aes_crypt_ecb(&ctx, AES_DECRYPT, g_au1Keybox + WV_KEYBOX_DEVICE_KEY_OFFSET, au1ClearDeviceKey);
#else
	// Philips TV's widevine content key already decrypt
	memcpy (au1ClearDeviceKey, g_au1Keybox + WV_KEYBOX_DEVICE_KEY_OFFSET, 16);
#endif
    //ShowKey("device key", au1ClearDeviceKey, 16);

    dprintf("\nSecure world clear device Key:\n");
    for(i = 0; i < 16; i++)
    {
        if((i) % 16 == 0)
            dprintf("\n\n");

        dprintf("\n0x%x ", *(UINT8 *)(au1ClearDeviceKey + i));
    }
    dprintf("\n\n");

    aes_setkey_dec(&ctx, au1ClearDeviceKey, (INT32)128);
    aes_crypt_ecb(&ctx, AES_DECRYPT, pu1EncAssetKey, g_au1AssetKey);

    dprintf("\nSecure world Asset Key:\n");
    for(i = 0; i < 16; i++)
    {
        if((i) % 16 == 0)
            dprintf("\n\n");

        dprintf("\n0x%x ", *(UINT8 *)(g_au1AssetKey + i));
    }
    dprintf("\n\n");

    return TRUE;
}


BOOL SvcEx_DRM_WV_DeriveCW(UINT8 *pu1Param, UINT32 u4ParmSize)
{
    UINT8 au1ClearEcm[32];
    UINT8 au1ECMIV[16];
    UINT8 i;
    aes_context ctx;
    UINT8* pu1Ecm;
    UINT8 au1Ecm[32];
    UINT32* pu4Flags;
    TZ_WV_DERIVECW_T *pWv;

    pWv = (TZ_WV_DERIVECW_T *)pu1Param;
    pu1Ecm = pWv->au1Ecm;
    pu4Flags = &pWv->u4Flags;

    for(i = 0; i < 16; i++)
    {
        au1ECMIV[i] = 0;
    }

    memcpy_n2s_chk(au1Ecm, pu1Ecm, 32, NULL);

    aes_setkey_dec(&ctx, g_au1AssetKey, 128);
    aes_crypt_cbc(&ctx, AES_DECRYPT, 32, au1ECMIV, au1Ecm, au1ClearEcm);

    //ShowKey("clear ecm", au1ClearEcm, 32);

    memcpy_s2n_chk(pu4Flags, au1ClearEcm, 4, NULL);
    memcpy_s2s_chk(g_au1ContorlWord, au1ClearEcm + 4, 16, NULL);

    GCPU_SetSecureSlot(AES_WV_SECURE_KEY_PTR, g_au1ContorlWord, 16);

    dprintf("\nSecure world Control word:\n");
    for(i = 0; i < 16; i++)
    {
        if((i) % 16 == 0)
            dprintf("\n\n");

        dprintf("\n0x%x ", *(UINT8 *)(g_au1ContorlWord + i));
    }
    dprintf("\n\n");

    dprintf("\nSecure world ECM: 0x%x\n", *(UINT32 *)pu4Flags);
    //ShowKey("control word", g_au1ContorlWord, 16);

    return TRUE;
}

BOOL SvcEx_DRM_WV_PassKeybox(UINT8* pu1Keybox, UINT32 u4Size)
{
    if(u4Size > 256)
    {
        return FALSE;
    }
    memcpy_n2s_chk(g_au1Keybox, pu1Keybox, u4Size, NULL);

    return TRUE;
}

///////////////////////////////////for hulu+dash+wv drm only begin//////////////
#ifdef CC_ENABLE_WIDEVINE_EME

BOOL _SvcEx_DRM_WV_CreateHandle(UINT32 sesscnt,UINT32* handle)
{
    UINT32 i = 1;
	
    if (sesscnt)
    {
        for (i = 1; i <= WIDEVINE_EME_MAPTBL_SIZE; i++)
        {
            if (g_pvMapTbl_wveme[i] == 0)
            {
                g_pvMapTbl_wveme[i] = sesscnt;
                *handle = i;
                return TRUE;
            }
        }
		
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return FALSE;
    }
    
}

UINT32 _SvcEx_DRM_WV_GetRef(UINT32 handle)
{
    UINT32 i = handle;

    if (i > WIDEVINE_EME_MAPTBL_SIZE)
    {
        dprintf("\n\n%s: ERROR*** Invalid handle\n", __FUNCTION__);
        return 0;
    }
    else if (i > 0)
    {
        return g_pvMapTbl_wveme[i];
    }
    else
        return 0;
}


BOOL _SvcEx_DRM_WV_FreeHandle(UINT32 handle)
{
    UINT32 i = handle;

    if (i > WIDEVINE_EME_MAPTBL_SIZE)
    {
        dprintf("\n\n%s: ERROR*** Invalid handle\n", __FUNCTION__);
        return FALSE;
    }else if (i > 0)
    {
        g_pvMapTbl_wveme[i] = 0;
        return TRUE;
    }
    else
        return FALSE;
}


BOOL SvcEx_DRM_WV_ValidateKeybox(UINT32 * retult)
{
	E_KeyboxError ret = NO_ERROR;
	
	ret = ValidateKeyboxTZ();
	
	*retult = ret;
	
	return TRUE;
}

BOOL SvcEx_DRM_WV_DestroySession(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_SESS_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	
	pWv = (WV_IOCTL_SESS_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->sessaddr);
	if(sesscnt)
	{
		DestroySessionTz(sesscnt);
		g_handle_count_wveme--;
		_SvcEx_DRM_WV_FreeHandle(pWv->sessaddr);
	}
	
	//dprintf("SvcEx_DRM_WV_DestroySession sesscnt = 0x%x", sesscnt);
	return TRUE;
}

BOOL SvcEx_DRM_WV_CreatSession(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_SESS_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	UINT32 handle = 0;
	
	pWv = (WV_IOCTL_SESS_T *)pu1Param;

	if (g_handle_count_wveme + 1 >= WIDEVINE_EME_MAPTBL_SIZE)
    {
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return false;
    }
	
	sesscnt = CreatSessionTz();

	if (sesscnt != NULL)
	{
		g_handle_count_wveme++;
        BOOL ret = _SvcEx_DRM_WV_CreateHandle((UINT32)sesscnt,&handle);
		if(ret)
		{
			pWv->sessaddr = (UINT32)handle;
			dprintf("SvcEx_DRM_WV_CreatSession sesscnt = 0x%x, handle = 0x%x \n", sesscnt,pWv->sessaddr);
			return TRUE;
		}
		else
		{
			dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
			pWv->sessaddr = 0;
			return false;
		}     
	}
	else
	{
		dprintf("SvcEx_DRM_WV_CreatSession fail \n ");
		pWv->sessaddr = 0;
		return false;
	}

}

BOOL SvcEx_DRM_WV_GenerateNonceSess(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_NONCE_T *pNonce = NULL;
	UINT32 nonce = 0;
	SessionContext * sesscnt = NULL;
	
	pNonce = (WV_IOCTL_NONCE_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pNonce->SessionContext);

	GenerateNonceSess(sesscnt, &nonce);
	pNonce->u4Nonce = nonce;

	return TRUE;
}

BOOL SvcEx_DRM_WV_GenerateDeriveKeys(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_DERIVEKEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_DERIVEKEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);
	
	if(pWv->master_key)
		TZ_SWD_BufferStart((UINT32)pWv->master_key, TZ_CACHE_ALIGN(pWv->mst_cnt_len));
	TZ_SWD_BufferStart((UINT32)pWv->mac_context, TZ_CACHE_ALIGN(pWv->mac_cnt_len));
	TZ_SWD_BufferStart((UINT32)pWv->enc_context, TZ_CACHE_ALIGN(pWv->enc_cnt_len));
	
	ret = DeriveKeys(sesscnt, pWv->master_key,pWv->mst_cnt_len, pWv->mac_context,pWv->mac_cnt_len, pWv->enc_context,pWv->enc_cnt_len);

	if(pWv->master_key)
		TZ_SWD_BufferEnd((UINT32)pWv->master_key, TZ_CACHE_ALIGN(pWv->mst_cnt_len));
	TZ_SWD_BufferEnd((UINT32)pWv->mac_context, TZ_CACHE_ALIGN(pWv->mac_cnt_len));
	TZ_SWD_BufferEnd((UINT32)pWv->enc_context, TZ_CACHE_ALIGN(pWv->enc_cnt_len));

	return ret;
}

BOOL SvcEx_DRM_WV_GenerateSignature(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_SIG_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	size_t signature_length = 0;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_SIG_T *)pu1Param;
	signature_length = pWv->signature_length;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferStart((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));
	
	ret = GenerateSignature(sesscnt,pWv->message,pWv->message_length,pWv->signature,&signature_length);

	TZ_SWD_BufferEnd((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferEnd((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));
		
	pWv->signature_length = signature_length;
	
	return ret;
}

BOOL SvcEx_DRM_WV_ValidateMessage(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_SIG_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL ret = FALSE;

	//dprintf("SvcEx_DRM_WV_ValidateMessage entry \n");

	pWv = (WV_IOCTL_SIG_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferStart((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	ret = ValidateMessage(sesscnt,pWv->message,pWv->message_length,pWv->signature,pWv->signature_length);

	TZ_SWD_BufferEnd((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferEnd((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	return ret;
}

BOOL SvcEx_DRM_WV_InstallKey(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_KEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL  ret = FALSE;

	//dprintf("SvcEx_DRM_WV_InstallKey entry \n");

	pWv = (WV_IOCTL_KEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->key_id, TZ_CACHE_ALIGN(pWv->key_id_length));
	TZ_SWD_BufferStart((UINT32)pWv->key_data, TZ_CACHE_ALIGN(pWv->key_data_length));
	TZ_SWD_BufferStart((UINT32)pWv->key_data_iv, TZ_CACHE_ALIGN(pWv->key_data_iv_length));
	TZ_SWD_BufferStart((UINT32)pWv->key_control, TZ_CACHE_ALIGN(pWv->key_control_length));
	TZ_SWD_BufferStart((UINT32)pWv->key_control_iv, TZ_CACHE_ALIGN(pWv->key_control_iv_length));

	ret = InstallKey(sesscnt, pWv->key_id, pWv->key_id_length,
				   			  pWv->key_data, pWv->key_data_length,
				   			  pWv->key_data_iv, pWv->key_data_iv_length,
				   			  pWv->key_control, pWv->key_control_length,
				   			  pWv->key_control_iv, pWv->key_control_iv_length);

	TZ_SWD_BufferEnd((UINT32)pWv->key_id, TZ_CACHE_ALIGN(pWv->key_id_length));
	TZ_SWD_BufferEnd((UINT32)pWv->key_data, TZ_CACHE_ALIGN(pWv->key_data_length));
	TZ_SWD_BufferEnd((UINT32)pWv->key_data_iv, TZ_CACHE_ALIGN(pWv->key_data_iv_length));
	TZ_SWD_BufferEnd((UINT32)pWv->key_control, TZ_CACHE_ALIGN(pWv->key_control_length));
	TZ_SWD_BufferEnd((UINT32)pWv->key_control_iv, TZ_CACHE_ALIGN(pWv->key_control_iv_length));
	
	//dprintf("SvcEx_DRM_WV_InstallKey leave \n");

	return ret;
}

BOOL SvcEx_DRM_WV_FlushNoncesSess(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_SESS_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	
	pWv = (WV_IOCTL_SESS_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->sessaddr);

	FlushNoncesSess(sesscnt);

	return TRUE;
}

BOOL SvcEx_DRM_WV_UpdateMacKeys(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_MACKEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL  ret = FALSE;
	
	pWv = (WV_IOCTL_MACKEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->mac_keys, TZ_CACHE_ALIGN(pWv->mac_keys_length));
	TZ_SWD_BufferStart((UINT32)pWv->iv, TZ_CACHE_ALIGN(pWv->iv_length));

	ret = UpdateMacKeys(sesscnt, pWv->mac_keys, pWv->mac_keys_length, pWv->iv, pWv->iv_length);

	TZ_SWD_BufferEnd((UINT32)pWv->mac_keys, TZ_CACHE_ALIGN(pWv->mac_keys_length));
	TZ_SWD_BufferEnd((UINT32)pWv->iv, TZ_CACHE_ALIGN(pWv->iv_length)); 

	return ret;
}

BOOL SvcEx_DRM_WV_RefreshKey(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_KEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL  ret = FALSE;
	
	pWv = (WV_IOCTL_KEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	if(pWv->key_id)
		TZ_SWD_BufferStart((UINT32)pWv->key_id, TZ_CACHE_ALIGN(pWv->key_id_length));
	TZ_SWD_BufferStart((UINT32)pWv->key_control, TZ_CACHE_ALIGN(pWv->key_control_length));
	if(pWv->key_control_iv)
		TZ_SWD_BufferStart((UINT32)pWv->key_control_iv, TZ_CACHE_ALIGN(pWv->key_control_iv_length));
		

	ret = RefreshKey(sesscnt, pWv->key_id, pWv->key_id_length,
							  pWv->key_control, pWv->key_control_length,
							  pWv->key_control_iv, pWv->key_control_iv_length);
	if(pWv->key_id)
		TZ_SWD_BufferEnd((UINT32)pWv->key_id, TZ_CACHE_ALIGN(pWv->key_id_length));
	TZ_SWD_BufferEnd((UINT32)pWv->key_control, TZ_CACHE_ALIGN(pWv->key_control_length));
	if(pWv->key_control_iv)
		TZ_SWD_BufferEnd((UINT32)pWv->key_control_iv, TZ_CACHE_ALIGN(pWv->key_control_iv_length));

	return ret;
}

BOOL SvcEx_DRM_WV_SelectContentKey(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_CONTKEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	KeyId key_id = {0};
	BOOL  ret = FALSE;
	
	pWv = (WV_IOCTL_CONTKEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);
	
	TZ_SWD_BufferStart((UINT32)pWv->key_id, TZ_CACHE_ALIGN(pWv->key_id_length));

	memcpy_n2s_chk(key_id, pWv->key_id, pWv->key_id_length,NULL);

	ret = SelectContentKey(sesscnt, key_id, pWv->key_id_length);

	TZ_SWD_BufferEnd((UINT32)pWv->key_id, TZ_CACHE_ALIGN(pWv->key_id_length));

	return ret;
}

BOOL SvcEx_DRM_WV_CheckDecCTRContentKey(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_CHK_KEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	unsigned short  tmp_slot = 0;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_CHK_KEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);
	
	ret = CheckDecCTRContentKey(sesscnt, pWv->u4Curtime,pWv->buffer_type,&tmp_slot);
	pWv->tem_slot = tmp_slot;

	return ret;
}

BOOL SvcEx_DRM_WV_SaveKeybox(UINT8 *pu1Keybox, UINT32 u4Size)
{
	BOOL ret = FALSE;

	//memcpy(g_Keybox, pu1Keybox, u4Size);
	
	ret = InstallKeyboxTz(pu1Keybox, u4Size);

	return ret;
}

BOOL SvcEx_DRM_WV_GetRandomNum(UINT8 *randomData, UINT32 u4Size)
{
	BOOL ret = FALSE;

	ret = GetRandomNum(randomData, u4Size);

	return ret;
}


BOOL SvcEx_DRM_WV_RewrapDeviceRSAKeySess(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_RSAKEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	size_t wrapped_rsa_key_length = 0;
	OEMCryptoResult ret = OEMCrypto_SUCCESS;

	//dprintf("SvcEx_DRM_WV_RewrapDeviceRSAKeySess entry \n");
	
	pWv = (WV_IOCTL_RSAKEY_T *)pu1Param;
	wrapped_rsa_key_length = pWv->wrapped_rsa_key_length;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferStart((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));
	TZ_SWD_BufferStart((UINT32)pWv->enc_rsa_key, TZ_CACHE_ALIGN(pWv->enc_rsa_key_length));
	TZ_SWD_BufferStart((UINT32)pWv->enc_rsa_key_iv, TZ_CACHE_ALIGN(KEY_IV_SIZE));
	TZ_SWD_BufferStart((UINT32)pWv->wrapped_rsa_key, TZ_CACHE_ALIGN(pWv->wrapped_rsa_key_length));

	ret = RewrapDeviceRSAKeySess(sesscnt,
							   pWv->message,pWv->message_length,
							   pWv->signature,pWv->signature_length,
							   pWv->nonce,
							   pWv->enc_rsa_key,pWv->enc_rsa_key_length,
							   pWv->enc_rsa_key_iv,
							   pWv->wrapped_rsa_key,&wrapped_rsa_key_length);

	TZ_SWD_BufferEnd((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferEnd((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));
	TZ_SWD_BufferEnd((UINT32)pWv->enc_rsa_key, TZ_CACHE_ALIGN(pWv->enc_rsa_key_length));
	TZ_SWD_BufferEnd((UINT32)pWv->enc_rsa_key_iv, TZ_CACHE_ALIGN(KEY_IV_SIZE));
	TZ_SWD_BufferEnd((UINT32)pWv->wrapped_rsa_key, TZ_CACHE_ALIGN(pWv->wrapped_rsa_key_length));

	
	pWv->wrapped_rsa_key_length = wrapped_rsa_key_length;
	pWv->result = ret;

	//dprintf("SvcEx_DRM_WV_RewrapDeviceRSAKeySess leave. ret=%d \n",ret);

	if(OEMCrypto_SUCCESS == ret)
		return TRUE;
	else
		return FALSE;
}

BOOL SvcEx_DRM_WV_LoadDeviceRSAKeySessCtx(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_RSAKEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	OEMCryptoResult ret = OEMCrypto_SUCCESS;
	
	pWv = (WV_IOCTL_RSAKEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->wrapped_rsa_key, TZ_CACHE_ALIGN(pWv->wrapped_rsa_key_length));

	ret = LoadDeviceRSAKeySessCtx(sesscnt,pWv->wrapped_rsa_key,pWv->wrapped_rsa_key_length);

	TZ_SWD_BufferEnd((UINT32)pWv->wrapped_rsa_key, TZ_CACHE_ALIGN(pWv->wrapped_rsa_key_length));

	pWv->result = ret;

	if(OEMCrypto_SUCCESS == ret)
		return TRUE;
	else
		return FALSE;
}

BOOL SvcEx_DRM_WV_GenerateRSASignature(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_SIG_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	size_t signature_length = 0;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_SIG_T *)pu1Param;
	signature_length = pWv->signature_length;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferStart((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	ret = GenerateRSASignature(sesscnt,pWv->message,pWv->message_length, pWv->signature,&signature_length);

	TZ_SWD_BufferEnd((UINT32)pWv->message, TZ_CACHE_ALIGN(pWv->message_length));
	TZ_SWD_BufferEnd((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	pWv->signature_length = signature_length;
	

	return ret;
}

BOOL SvcEx_DRM_WV_RSADeriveKeys(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_RSADERIVEKEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_RSADERIVEKEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->enc_session_key, TZ_CACHE_ALIGN(pWv->enc_session_key_length));
	TZ_SWD_BufferStart((UINT32)pWv->mac_context, TZ_CACHE_ALIGN(pWv->mac_context_length));
	TZ_SWD_BufferStart((UINT32)pWv->enc_context, TZ_CACHE_ALIGN(pWv->enc_context_length));

	ret = RSADeriveKeys(sesscnt,pWv->enc_session_key, pWv->enc_session_key_length,
					   pWv->mac_context,pWv->mac_context_length,
					   pWv->enc_context, pWv->enc_context_length);
	
	TZ_SWD_BufferEnd((UINT32)pWv->enc_session_key, TZ_CACHE_ALIGN(pWv->enc_session_key_length));
	TZ_SWD_BufferEnd((UINT32)pWv->mac_context, TZ_CACHE_ALIGN(pWv->mac_context_length));
	TZ_SWD_BufferEnd((UINT32)pWv->enc_context, TZ_CACHE_ALIGN(pWv->enc_context_length));

	return ret;
}

BOOL SvcEx_DRM_WV_CheckEncDecCBCContentKey(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_CHK_KEY_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	unsigned short tmp_slot =0;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_CHK_KEY_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);
	
	ret = CheckEncDecCBCContentKey(sesscnt, pWv->u4Curtime, pWv->enc_dec, &tmp_slot);
	pWv->tem_slot = tmp_slot;

	return ret;
}

#if 0
BOOL SvcEx_DRM_WV_DecryptCTR(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_DEC_CTR_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_DEC_CTR_T *)pu1Param;
	sesscnt = (SessionContext *)pWv->SessionContext;

	TZ_SWD_BufferStart((UINT32)pWv->iv, TZ_CACHE_ALIGN(AES_FUN_BLK_LEN));
	TZ_SWD_BufferStart((UINT32)pWv->cipher_data, TZ_CACHE_ALIGN(pWv->cipher_data_length));
	TZ_SWD_BufferStart((UINT32)pWv->clear_data, TZ_CACHE_ALIGN(pWv->cipher_data_length));

	ret = DecryptCTR(sesscnt,
					pWv->iv,
					pWv->block_offset,
					pWv->cipher_data,
					pWv->cipher_data_length,
					pWv->is_encrypted,
					pWv->clear_data,
					pWv->buffer_type,pWv->u4Curtime);
	
	TZ_SWD_BufferEnd((UINT32)pWv->iv, TZ_CACHE_ALIGN(AES_FUN_BLK_LEN));
	TZ_SWD_BufferEnd((UINT32)pWv->cipher_data, TZ_CACHE_ALIGN(pWv->cipher_data_length));
	TZ_SWD_BufferEnd((UINT32)pWv->clear_data, TZ_CACHE_ALIGN(pWv->cipher_data_length));

	return ret;

}

BOOL SvcEx_DRM_WV_Generic_Encrypt(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_GEN_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_GEN_T *)pu1Param;
	sesscnt = (SessionContext *)pWv->SessionContext;

	TZ_SWD_BufferStart((UINT32)pWv->iv, TZ_CACHE_ALIGN(AES_FUN_BLK_LEN));
	TZ_SWD_BufferStart((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));
	TZ_SWD_BufferStart((UINT32)pWv->out_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));

	ret =  Generic_Encrypt(sesscnt, pWv->in_buffer,
                     pWv->buffer_length,
                     pWv->iv,
                     pWv->algorithm,
                     pWv->out_buffer, pWv->u4Curtime);
	
	TZ_SWD_BufferEnd((UINT32)pWv->iv, TZ_CACHE_ALIGN(AES_FUN_BLK_LEN));
	TZ_SWD_BufferEnd((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));
	TZ_SWD_BufferEnd((UINT32)pWv->out_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));

	return ret;

}

BOOL SvcEx_DRM_WV_Generic_Decrypt(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_GEN_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_GEN_T *)pu1Param;
	sesscnt = (SessionContext *)pWv->SessionContext;
	
	TZ_SWD_BufferStart((UINT32)pWv->iv, TZ_CACHE_ALIGN(AES_FUN_BLK_LEN));
	TZ_SWD_BufferStart((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));
	TZ_SWD_BufferStart((UINT32)pWv->out_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));

	ret =  Generic_Decrypt(sesscnt, pWv->in_buffer,
                     pWv->buffer_length,
                     pWv->iv,
                     pWv->algorithm,
                     pWv->out_buffer, pWv->u4Curtime);
	
	TZ_SWD_BufferEnd((UINT32)pWv->iv, TZ_CACHE_ALIGN(AES_FUN_BLK_LEN));
	TZ_SWD_BufferEnd((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));
	TZ_SWD_BufferEnd((UINT32)pWv->out_buffer, TZ_CACHE_ALIGN(pWv->buffer_length));

	return ret;

}
#endif

BOOL SvcEx_DRM_WV_Generic_Sign(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_GENERIC_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	size_t signature_length = 0;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_GENERIC_T *)pu1Param;
	signature_length = pWv->signature_length;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->in_buffer_length));
	TZ_SWD_BufferStart((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	ret = Generic_Sign(sesscnt, pWv->in_buffer,
					  pWv->in_buffer_length,
					  pWv->algorithm,
					  pWv->signature,
					  &signature_length, pWv->u4Curtime);
	
	TZ_SWD_BufferEnd((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->in_buffer_length));
	TZ_SWD_BufferEnd((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	pWv->signature_length = signature_length;
	
	return ret;
}

BOOL SvcEx_DRM_WV_Generic_Verify(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	WV_IOCTL_GENERIC_T *pWv = NULL;
	SessionContext * sesscnt = NULL;
	BOOL ret = FALSE;
	
	pWv = (WV_IOCTL_GENERIC_T *)pu1Param;
	sesscnt = (SessionContext *)_SvcEx_DRM_WV_GetRef(pWv->SessionContext);

	TZ_SWD_BufferStart((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->in_buffer_length));
	TZ_SWD_BufferStart((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	ret = Generic_Verify(sesscnt, pWv->in_buffer,
						pWv->in_buffer_length,
						pWv->algorithm,
					    pWv->signature,
					    pWv->signature_length, pWv->u4Curtime);
	
	TZ_SWD_BufferEnd((UINT32)pWv->in_buffer, TZ_CACHE_ALIGN(pWv->in_buffer_length));
	TZ_SWD_BufferEnd((UINT32)pWv->signature, TZ_CACHE_ALIGN(pWv->signature_length));

	return ret;
}
#endif
///////////////////////////////////for hulu+dash+wv drm only begin//////////////


BOOL SvcEx_DRM_DIVX_GetHwSecretKey0(UINT32 u4Addr, UINT32 u4Size)
{
    TZ_GET_HWSECRETKEY_T *pGetRomBytes = (TZ_GET_HWSECRETKEY_T *)u4Addr;
#ifdef CC_CUSTOMIZE_DIVX_KEY
    const UINT8 divxKey[] ={0x25,0x6f,0x90,0x6b,0xcf,0x29,0x85,0x4c,0x8f,0xf2,0x2e,0xbc,0x3c,0x88,0x3e,0x26,
         0x24,0xf6,0xa4,0x3b,0x06,0xf5,0xf4,0x12,0xc5,0xd8,0xf3,0xf9,0xdb,0x37,0xb0,0x0f};  

    memcpy_s2n_chk(pGetRomBytes->au1Buf, divxKey, sizeof(divxKey) , NULL);
    return TRUE;
#else
    _SvcEx_DRM_GetRomBytes(0x3C00, pGetRomBytes->au1Buf, 32);
    return TRUE;
#endif
}

BOOL SvcEx_DRM_DIVX_GetHwSecretKey1(UINT32 u4Addr, UINT32 u4Size)
{
    TZ_GET_HWSECRETKEY_T *pGetRomBytes = (TZ_GET_HWSECRETKEY_T *)u4Addr;
#ifdef CC_CUSTOMIZE_DIVX_KEY
    const UINT8 divxKey[] ={0x25,0x6f,0x90,0x6b,0xcf,0x29,0x85,0x4c,0x8f,0xf2,0x2e,0xbc,0x3c,0x88,0x3e,0x26,
         0x24,0xf6,0xa4,0x3b,0x06,0xf5,0xf4,0x12,0xc5,0xd8,0xf3,0xf9,0xdb,0x37,0xb0,0x0f}; 

    memcpy_s2n_chk(pGetRomBytes->au1Buf, divxKey, sizeof(divxKey) , NULL);
    return TRUE;
#else
    _SvcEx_DRM_GetRomBytes(0x3C20, pGetRomBytes->au1Buf, 32);
    return TRUE;
#endif    
}

BOOL SvcEx_DRM_GetSecureID(UINT32 u4Addr, UINT32 u4Size)
{
    TZ_GET_SECURE_ID_T *pGetSecureID = (TZ_GET_SECURE_ID_T *)u4Addr;

    if (pGetSecureID->u4Size != SECURE_ID_LEN)
    {
        return TZ_ERROR_GENERIC;
    }

    TZ_SWD_BufferStart((UINT32)(pGetSecureID->au1Buf), TZ_CACHE_ALIGN(pGetSecureID->u4Size));
    
    _SvcEx_DRM_GetSecureID(pGetSecureID->au1Buf, pGetSecureID->u4Size);

    TZ_SWD_BufferEnd((UINT32)(pGetSecureID->au1Buf), TZ_CACHE_ALIGN(pGetSecureID->u4Size));
    
	dprintf("pGetSecureID->au1Buf: 0x%08x,0x%08x,0x%08x,0x%08x\n", pGetSecureID->au1Buf[0], 
		pGetSecureID->au1Buf[1], pGetSecureID->au1Buf[2], pGetSecureID->au1Buf[3]);
    
    return TRUE;
}

#ifdef CC_ENABLE_HCI
BOOL _SvcEx_HCI_CreateHandle(void **pRef)
{
    //if (g_handle_count > g_handle_count_max)
    //    g_handle_count_max = g_handle_count;
#if CC_HCI_HANDLE_MAP
    UINT32 i = 1;
    if (*pRef)
    {
        for (i = 1; i < HCI_MAPTBL_SIZE; i++)
        {
            if (g_pvMapTbl[i] == NULL)
            {
                g_pvMapTbl[i] = *pRef;
                *pRef = (void*)i;
                return TRUE;
            }
        }
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        *pRef = NULL;
        return FALSE;
    }
    return TRUE;
#else
    return TRUE;
#endif
}

void* _SvcEx_HCI_GetRef(void *pHandle)
{
#if CC_HCI_HANDLE_MAP    
    UINT32 i = (UINT32)(pHandle);
    if (i >= HCI_MAPTBL_SIZE)
    {
        dprintf("\n\n%s: ERROR*** Invalid handle\n", __FUNCTION__);
        return NULL;
    }
    else if (i > 0)
    {
        return g_pvMapTbl[i];
    }
    else
        return NULL;
#else
    return pHandle;
#endif
}


BOOL _SvcEx_HCI_FreeHandle(void *pHandle)
{
#if CC_HCI_HANDLE_MAP    
    UINT32 i = (UINT32)pHandle;
    if (i >= HCI_MAPTBL_SIZE)
    {
        dprintf("\n\n%s: ERROR*** Invalid handle\n", __FUNCTION__);
        return FALSE;
    }else if (i > 0)
    {
        g_pvMapTbl[i] = NULL;
        return TRUE;
    }
    else
        return TRUE;
#else
    return TRUE;
#endif
}

BOOL SvcEx_HCI_Init(MTDMX_MWIF_HCI_INIT_T* pTrustzone)
{
    INT32 i = 0;
    pTrustzone->u4Ret = hci_Init();
    g_handle_count = 0;
    for (i = 0; i < HCI_MAPTBL_SIZE; i++)
    {
        if(g_pvMapTbl[i])
            g_pvMapTbl[i] = NULL;
    }
    //g_handle_count_max = 0;
    return TRUE;
}

BOOL SvcEx_HCI_Fin(MTDMX_MWIF_HCI_INIT_T* pTrustzone)
{
    hci_Fin();
    if (g_handle_count)
        dprintf("\n\n%s: ERROR*** handle not free (%d)\n", __FUNCTION__, g_handle_count);
    //else
    //    dprintf("\n\n%s: Max handle usage (%d)\n", __FUNCTION__, g_handle_count_max);
    return TRUE;
}

BOOL SvcEx_HCI_Lock(MTDMX_MWIF_HCI_LOCK_T* pTrustzone)
{
    pTrustzone->u4Ret = hci_Lock(&pTrustzone->pout_lock);
    //dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);
    return TRUE;
}

BOOL SvcEx_HCI_Unlock(MTDMX_MWIF_HCI_UNLOCK_T* pTrustzone)
{
    pTrustzone->u4Ret = hci_Unlock(pTrustzone->io_lock);
    //dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);
    return TRUE;
}

BOOL SvcEx_HCI_KeyRefCreate(MTDMX_MWIF_HCI_KEY_REF_T* pTrustzone)
{
    if (g_handle_count + 1 >= HCI_MAPTBL_SIZE)
    {
        pTrustzone->u4Ret = ERR_HCI_INTERNALERROR;
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return TRUE;
    }
    
    pTrustzone->u4Ret = hci_CreateKeyRef((hci_key_type_t)pTrustzone->u4KeyType,
        (hci_key_ref_t *)&pTrustzone->pKeyRef);
    if (pTrustzone->u4Ret == ERR_HCI_OK)
    {
        g_handle_count++;
        if(_SvcEx_HCI_CreateHandle(&pTrustzone->pKeyRef) != TRUE)
            pTrustzone->u4Ret = ERR_HCI_INTERNALERROR;
    }
    //    pTrustzone->pKeyRef = SvcEx_HCI_addrToHandle(pTrustzone->pKeyRef);

    //dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->pKeyRef);
    return TRUE;
}

BOOL SvcEx_HCI_KeyRefCheck(MTDMX_MWIF_HCI_KEY_REF_T* pTrustzone)
{
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef(pTrustzone->pKeyRef);
    
    pTrustzone->u4Ret = hci_CheckKeyRef(in_ref, &pTrustzone->bresult);
    
    //dprintf("\n\n\%s: 0x%x, bresult: %d\n", __FUNCTION__, pTrustzone->pKeyRef, pTrustzone->bresult);

    return TRUE;
}

BOOL SvcEx_HCI_KeyRefDestroy(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret = ERR_HCI_OK;
    void *pHandle = *((hci_key_ref_t *)ptCommonArg->apvArgument[0]);
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef(pHandle);
    
    if(in_ref)
    {
        ret = hci_DestroyKeyRef(in_ref);
        if (ret == ERR_HCI_OK)
        {
            g_handle_count--;
            _SvcEx_HCI_FreeHandle(pHandle);
        }
    }

    *((uint32_t *)ptCommonArg->apvArgument[1]) = ret;

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));
//    pTrustzone->u4Ret = hci_DestroyKeyRef((hci_key_ref_t)pTrustzone->pKeyRef);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_KEY_REF_T));

//    dprintf("\n\n\%s: ret = %d, 0x%x\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[1]), *((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    return TRUE;
}

BOOL SvcEx_HCI_DevIdGetNum(MTDMX_MWIF_HCI_ID_T* pTrustzone)
{
/*
    binstr_t **out_ids = NULL;
    UINT32 u4Num, i;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));
    pTrustzone->u4Ret = hci_DevIdsGet(&out_ids, &u4Num);
    pTrustzone->u4IdNum = u4Num;
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4IdNum);

    if (out_ids != NULL) {
        for (i = 0; i < u4Num; i++)
        {
            hci_BinstrFree(out_ids[i]);
        }
        tom_free(out_ids);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_DevIdGetSize(MTDMX_MWIF_HCI_ID_T* pTrustzone)
{

//    binstr_t **out_ids = NULL;
//    UINT32 u4Num, i;
    char *id = NULL;

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));
    pTrustzone->u4Ret = hci_GetDevID(pTrustzone->u4KeyType, &id);

    if (id != NULL)
    {
        pTrustzone->u4IdSize = strlen(id);
    }
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));

    //dprintf("\n\n\%s: ret = 0x%x, %d\n", __FUNCTION__, pTrustzone->u4Ret, pTrustzone->u4IdSize);

    hci_FreeMem(id);
/*
    if (out_ids != NULL) {
        for (i = 0; i < u4Num; i++)
        {
            hci_BinstrFree(out_ids[i]);
        }
        tom_free(out_ids);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_DevIdGetName(MTDMX_MWIF_HCI_ID_T* pTrustzone)
{

//    binstr_t **out_ids = NULL;
//    UINT32 u4Num, i;
    char *id = NULL;

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));
    TZ_SWD_BufferStart((UINT32)pTrustzone->pBuf, TZ_CACHE_ALIGN(pTrustzone->u4IdSize));
    pTrustzone->u4Ret = hci_GetDevID(pTrustzone->u4KeyType, &id);
    if(id != NULL)
    {
        memcpy_s2n_chk(pTrustzone->pBuf, id, strlen(id), NULL);
//        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pBuf, out_ids[pTrustzone->u4IdIdx]->len);
    }
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pBuf, TZ_CACHE_ALIGN(pTrustzone->u4IdSize));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));

    //dprintf("\n\n\%s: ret = 0x%x, %d\n", __FUNCTION__, pTrustzone->u4Ret, pTrustzone->u4IdSize);

    hci_FreeMem(id);

/*
    if (out_ids != NULL) {
        for (i = 0; i < u4Num; i++)
    {
            hci_BinstrFree(out_ids[i]);
        }
        tom_free(out_ids);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_ImportKey(MTDMX_MWIF_HCI_IMPORT_KEY_T* pTrustzone)
{
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef(pTrustzone->pKeyInRef);
    hci_key_ref_t rKeyRef;

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_KEY_T));
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOAEPParam, ((binstr_t *)pTrustzone->pOAEPParam)->len + sizeof(binstr_t));
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pKeyData, ((binstr_t *)pTrustzone->pKeyData)->len + sizeof(binstr_t));
    if (g_handle_count + 1 >= HCI_MAPTBL_SIZE)
    {
        pTrustzone->u4Ret = ERR_HCI_INTERNALERROR;
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return TRUE;
    }

    if (pTrustzone->pOAEPParam != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pOAEPParam, TZ_CACHE_ALIGN(pTrustzone->u4OAEPLen));
    }
    if (pTrustzone->pIv != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pIv, TZ_CACHE_ALIGN(pTrustzone->u4IvLen));
    }
    if (pTrustzone->pKeyData != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pKeyData, TZ_CACHE_ALIGN(pTrustzone->u4KeyLen));
    }

    pTrustzone->u4Ret = hci_ImportKey(in_ref, pTrustzone->u4ImpType, pTrustzone->u4EncAlg,
        pTrustzone->u4EncMode, pTrustzone->u4EncPad, pTrustzone->u4OAEPLen, pTrustzone->pOAEPParam,
        pTrustzone->u4IvLen, pTrustzone->pIv, pTrustzone->u4KeyLen, pTrustzone->pKeyData, &rKeyRef);

    if (pTrustzone->u4Ret == ERR_HCI_OK)
    {
        g_handle_count++;
        if(_SvcEx_HCI_CreateHandle(&rKeyRef) != TRUE)
            pTrustzone->u4Ret = ERR_HCI_INTERNALERROR;
        pTrustzone->pKeyOutRef = rKeyRef;
    }
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_KEY_T));

    if (pTrustzone->pOAEPParam != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pOAEPParam, TZ_CACHE_ALIGN(pTrustzone->u4OAEPLen));
    }
    if (pTrustzone->pIv != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pIv, TZ_CACHE_ALIGN(pTrustzone->u4IvLen));
    }
    if (pTrustzone->pKeyData != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pKeyData, TZ_CACHE_ALIGN(pTrustzone->u4KeyLen));
    }

    //dprintf("\n\n\%s: ret = 0x%x, 0x%x\n", __FUNCTION__, pTrustzone->u4Ret, pTrustzone->pKeyOutRef);
    return TRUE;
}

BOOL SvcEx_HCI_ExportKeySize(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T* pTrustzone)
{
/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));
    pTrustzone->u4Ret = hci_ExportKey(pTrustzone->pKeyRef, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data !=NULL))
    {
        pTrustzone->u4IdSize = out_data->len;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4IdSize);

    if(out_data != NULL)
    {
        hci_l_FreeKey(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_ExportKey(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t len = 0;
    uint32_t ret = ERR_HCI_INTERNALERROR;
    uint8_t *temp = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef(*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_ExportKey(in_ref,
                        *((uint32_t *)ptCommonArg->apvArgument[1]),
                        (ptCommonArg->ui4Length[2] == 0)?NULL:(uint8_t *)ptCommonArg->apvArgument[1],
                        &len, &temp);

    *((uint32_t *)ptCommonArg->apvArgument[3]) = len;
    if (ptCommonArg->abWrite[4] == 1)
    {
        if (ret == ERR_HCI_OK)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[4], temp, len, NULL);
        }
    }
    *((uint32_t *)ptCommonArg->apvArgument[5]) = ret;

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[5]), len);

    hci_FreeMem(temp);

/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));
    pTrustzone->u4Ret = hci_ExportKey(pTrustzone->pKeyRef, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data !=NULL))
    {
        memcpy(pTrustzone->pBuf, out_data->buf, out_data->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pBuf, out_data->len);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if(out_data != NULL)
    {
        hci_l_FreeKey(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_StarfishIdGetSize(MTDMX_MWIF_HCI_ID_T* pTrustzone)
{
/*
    binstr_t *out_id = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));
    pTrustzone->u4Ret = hci_StarfishIdGet(&out_id);
    if((pTrustzone->u4Ret == ERR_OK) && (out_id !=NULL))
    {
        pTrustzone->u4IdSize = out_id->len;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4IdSize);

    if(out_id !=NULL)
    {
        hci_BinstrFree(out_id);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_StarfishIdGet(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t len;
    uint32_t ret;
    uint8_t *temp = NULL;

    ret = hci_GetStarfishId(&temp);

    if (ret == ERR_HCI_OK)
    {
        len = strlen(temp);
        *((uint32_t *)ptCommonArg->apvArgument[0]) = len;
        if (ptCommonArg->abWrite[1] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[1], temp, len, NULL);
        }
    }

    *((uint32_t *)ptCommonArg->apvArgument[2]) = ret;

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[2]), len);

    hci_FreeMem(temp);

/*
    binstr_t *out_id;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));
    pTrustzone->u4Ret = hci_StarfishIdGet(&out_id);
    if((pTrustzone->u4Ret == ERR_OK) && (out_id !=NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pBuf, out_id->len);
        memcpy(pTrustzone->pBuf, out_id->buf, out_id->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pBuf, out_id->len);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_ID_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if(out_id !=NULL)
    {
        hci_BinstrFree(out_id);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_KeyDataGetIfStarfishSize(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T* pTrustzone)
{
/*
    binstr_t *out_data = NULL;

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone);
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));
    pTrustzone->u4Ret = hci_KeyDataGetIfStarfish(pTrustzone->pKeyRef, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data !=NULL))
    {
        pTrustzone->u4IdSize = out_data->len;
    }
    else
    {
        pTrustzone->u4IdSize = 0;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));

    dprintf("\n\n\%s: %d 0x%x\n", __FUNCTION__, pTrustzone->u4IdSize, pTrustzone->u4Ret);

    if(out_data !=NULL)
    {
        hci_l_FreeKey(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_KeyDataGetIfStarfish(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t len = 0;
    uint32_t ret = ERR_HCI_BASE;
    uint8_t *temp = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef(*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_GetKeyDataIfStarfish(in_ref,
                               &len, &temp);

    *((uint32_t *)ptCommonArg->apvArgument[1]) = len;
    if (ptCommonArg->abWrite[2] == 1)
    {
        if (ret == ERR_HCI_OK)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[2], temp, len, NULL);
        }
    }
    *((uint32_t *)ptCommonArg->apvArgument[3]) = ret;

    //dprintf("\n\n\%s: ref = 0x%x, ret = 0x%x, len = %d\n", __FUNCTION__, *((hci_key_ref_t *)ptCommonArg->apvArgument[0]), *((uint32_t *)ptCommonArg->apvArgument[3]), len);

    hci_FreeMem(temp);

/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));
    pTrustzone->u4Ret = hci_KeyDataGetIfStarfish(pTrustzone->pKeyRef, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data !=NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pBuf, out_data->len);
        memcpy(pTrustzone->pBuf, out_data->buf, out_data->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pBuf, out_data->len);
    }
    else
    {
        pTrustzone->u4IdSize = 0;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if(out_data !=NULL)
    {
        hci_l_FreeKey(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_CreateSACKeyRef(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    hci_key_ref_t keyref = NULL;

    if (g_handle_count + 1 >= HCI_MAPTBL_SIZE)
    {
        *((uint32_t *)ptCommonArg->apvArgument[2]) = ERR_HCI_INTERNALERROR;
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return TRUE;
    }
    
    ret = hci_CreateSACKeyRef(*((uint32_t *)ptCommonArg->apvArgument[0]),
                              &keyref);

    *((uint32_t *)ptCommonArg->apvArgument[2]) = ret;

    if (ret == ERR_HCI_OK)
    {
        g_handle_count++;
        if (_SvcEx_HCI_CreateHandle(&keyref) != TRUE)
            *((uint32_t *)ptCommonArg->apvArgument[2]) = ERR_HCI_INTERNALERROR;
        *((hci_key_ref_t *)ptCommonArg->apvArgument[1]) = keyref;
    }

    //dprintf("\n\n\%s: %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[2]));

    return TRUE;
}

BOOL SvcEx_HCI_GenerateContentKey(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    hci_key_ref_t keyref = NULL;

    if (g_handle_count + 1 >= HCI_MAPTBL_SIZE)
    {
        *((uint32_t *)ptCommonArg->apvArgument[4]) = ERR_HCI_INTERNALERROR;
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return TRUE;
    }
    
    ret = hci_GenerateContentKey(*((uint32_t *)ptCommonArg->apvArgument[0]),
                                 *((uint32_t *)ptCommonArg->apvArgument[1]),
                                 (ptCommonArg->abWrite[2]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[2],
                                 (ptCommonArg->ui4Length[3]==0)?NULL:&keyref);

    *((uint32_t *)ptCommonArg->apvArgument[4]) = ret;

    if (ret == ERR_HCI_OK)
    {
        g_handle_count++;
        if (_SvcEx_HCI_CreateHandle(&keyref) != TRUE)
            *((uint32_t *)ptCommonArg->apvArgument[4]) = ERR_HCI_INTERNALERROR;
        *((hci_key_ref_t *)ptCommonArg->apvArgument[3]) = keyref;
    }

//    dprintf("\n\n\%s: ret=%d, ref=0x%x\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[4]), *((hci_key_ref_t *)ptCommonArg->apvArgument[3]));

    return TRUE;
}

BOOL SvcEx_HCI_ImportStarfishKey(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    hci_key_ref_t keyref = NULL;

    if (g_handle_count + 1 >= HCI_MAPTBL_SIZE)
    {
        *((uint32_t *)ptCommonArg->apvArgument[11]) = ERR_HCI_INTERNALERROR;
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return TRUE;
    }
    
    ret = hci_ImportStarfishKey(*((uint32_t *)ptCommonArg->apvArgument[0]),
                                *((uint32_t *)ptCommonArg->apvArgument[1]),
                                *((uint32_t *)ptCommonArg->apvArgument[2]),
                                (ptCommonArg->ui4Length[3]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                                *((uint32_t *)ptCommonArg->apvArgument[4]),
                                (ptCommonArg->ui4Length[5]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[5],
                                *((uint32_t *)ptCommonArg->apvArgument[6]),
                                (ptCommonArg->ui4Length[7]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[7],
                                *((uint32_t *)ptCommonArg->apvArgument[8]),
                                (ptCommonArg->ui4Length[9]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[9],
                                (ptCommonArg->ui4Length[10]==0)?NULL:&keyref);

    *((uint32_t *)ptCommonArg->apvArgument[11]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->ui4Length[10]!=0)
        {
            g_handle_count++;
            if (_SvcEx_HCI_CreateHandle(&keyref) != TRUE)
                *((uint32_t *)ptCommonArg->apvArgument[11]) = ERR_HCI_INTERNALERROR;
            *((hci_key_ref_t *)ptCommonArg->apvArgument[10]) = keyref;
        }
    }

    //dprintf("\n\n\%s: ret = 0x%x, ref = 0x%x, keyref = 0x%x\n", __FUNCTION__, ret, *((hci_key_ref_t *)ptCommonArg->apvArgument[10]), keyref);

    return TRUE;

/*
    hci_key_ref_t rKeyRef;

    if (pTrustzone->pEncBK != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pEncBK, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferStart((UINT32)((binstr_t *)pTrustzone->pEncBK)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pEncBK)->len));
    }
    if (pTrustzone->pBKHash != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pBKHash, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferStart((UINT32)((binstr_t *)pTrustzone->pBKHash)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pBKHash)->len));
    }
    if (pTrustzone->pIv != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pIv, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferStart((UINT32)((binstr_t *)pTrustzone->pIv)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pIv)->len));
    }
    if (pTrustzone->pEncCK != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pEncCK, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferStart((UINT32)((binstr_t *)pTrustzone->pEncCK)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pEncCK)->len));
    }
*/

/*
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_STARFISH_KEY_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pEncBK, ((binstr_t *)pTrustzone->pEncBK)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pBKHash, ((binstr_t *)pTrustzone->pBKHash)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pIv, ((binstr_t *)pTrustzone->pIv)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pEncCK, ((binstr_t *)pTrustzone->pEncCK)->len + sizeof(binstr_t));
*/

/*
    pTrustzone->u4Ret = hci_ImportStarfishKey(pTrustzone->u4IterationNum, pTrustzone->u4KeyIndex,
        pTrustzone->pEncBK, pTrustzone->pBKHash, pTrustzone->pIv, pTrustzone->pEncCK,
        &rKeyRef);


    if (pTrustzone->pEncBK != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pEncBK, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferEnd((UINT32)((binstr_t *)pTrustzone->pEncBK)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pEncBK)->len));
    }
    if (pTrustzone->pBKHash != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pBKHash, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferEnd((UINT32)((binstr_t *)pTrustzone->pBKHash)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pBKHash)->len));
    }
    if (pTrustzone->pIv != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pIv, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferEnd((UINT32)((binstr_t *)pTrustzone->pIv)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pIv)->len));
    }
    if (pTrustzone->pEncCK != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pEncCK, TZ_CACHE_ALIGN(sizeof(binstr_t *)));
        TZ_SWD_BufferEnd((UINT32)((binstr_t *)pTrustzone->pEncCK)->buf, TZ_CACHE_ALIGN(((binstr_t *)pTrustzone->pEncCK)->len));
    }

    pTrustzone->pKeyOutRef = rKeyRef;
*/

/*
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_STARFISH_KEY_T));
*/
//    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->pKeyOutRef);
//    return TRUE;
}

BOOL SvcEx_HCI_DataSignSize(TEE_COMMON_ARG_T *ptCommonArg)
{
/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_DataSign(pTrustzone->pKeyInRef, pTrustzone->u4Alg,
        pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        pTrustzone->u4Size = out_data->len;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Size);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_DataSign(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t len;
    uint32_t ret;
    uint8_t *temp = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    if (ptCommonArg->ui4Length[7] == sizeof(uint32_t)) // only for get size
    {
        ret = hci_SignData(in_ref,
                            *((hci_sig_algo_t *)ptCommonArg->apvArgument[1]),
                            *((uint32_t *)ptCommonArg->apvArgument[2]),
                            (ptCommonArg->ui4Length[3] == 0)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                            (ptCommonArg->ui4Length[4]==0)?NULL:&len, &temp);
    }
    else
    {
        ret = hci_SignData(in_ref,
                            *((hci_sig_algo_t *)ptCommonArg->apvArgument[1]),
                            *((uint32_t *)ptCommonArg->apvArgument[2]),
                            (ptCommonArg->ui4Length[3] == 0)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                            (ptCommonArg->ui4Length[4]==0)?NULL:&len, (ptCommonArg->ui4Length[5]==0)?NULL:&temp);
    }

    if (ret == ERR_HCI_OK)
    {
        *((uint32_t *)ptCommonArg->apvArgument[4]) = len;
        if (ptCommonArg->ui4Length[5] != 0 && ptCommonArg->abWrite[5] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[5], temp, len, NULL);
        }
    }

    *((uint32_t *)ptCommonArg->apvArgument[6]) = ret;

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[6]), len);

    hci_FreeMem(temp);
/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_DataSign(pTrustzone->pKeyInRef, pTrustzone->u4Alg,
        pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        memcpy(pTrustzone->pOutData, out_data->buf, out_data->len);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_DataDecryptSize(MTDMX_MWIF_HCI_DATA_OP_T* pTrustzone)
{
/*
    binstr_t *out_data = NULL;
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInOAEPParam, ((binstr_t *)pTrustzone->pInOAEPParam)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_DataDecrypt(pTrustzone->pKeyInRef, pTrustzone->u4Alg, pTrustzone->u4Pad,
        pTrustzone->pInSaltBuf, pTrustzone->pInOAEPParam, pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        pTrustzone->u4Size = out_data->len;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Size);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

//BOOL SvcEx_HCI_DataDecrypt(MTDMX_MWIF_HCI_DATA_OP_T* pTrustzone)
BOOL SvcEx_HCI_DataDecrypt(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t len;
    uint32_t ret;
    uint8_t *temp = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));

    if (ptCommonArg->ui4Length[13] == sizeof(uint32_t))
    {
        ret = hci_DecryptData(in_ref,
                              *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                              *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                              *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                              *((uint32_t *)ptCommonArg->apvArgument[4]),
                              (ptCommonArg->ui4Length[5]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[5],
                              *((uint32_t *)ptCommonArg->apvArgument[6]),
                              (ptCommonArg->ui4Length[7]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[7],
                              *((uint32_t *)ptCommonArg->apvArgument[8]),
                              (ptCommonArg->ui4Length[9]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[9],
                              (ptCommonArg->ui4Length[10]==0)?NULL:&len,
                              (ptCommonArg->abWrite[12]==2)?NULL:&temp);
    }
    else
    {
        ret = hci_DecryptData(in_ref,
                              *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                              *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                              *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                              *((uint32_t *)ptCommonArg->apvArgument[4]),
                              (ptCommonArg->ui4Length[5]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[5],
                              *((uint32_t *)ptCommonArg->apvArgument[6]),
                              (ptCommonArg->ui4Length[7]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[7],
                              *((uint32_t *)ptCommonArg->apvArgument[8]),
                              (ptCommonArg->ui4Length[9]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[9],
                              (ptCommonArg->ui4Length[10]==0)?NULL:&len,
                              (ptCommonArg->ui4Length[12]==0)?NULL:&temp);
    }

    if (ret == ERR_HCI_OK)
    {
        *((uint32_t *)ptCommonArg->apvArgument[10]) = len;
        if (ptCommonArg->ui4Length[12] != 0 && ptCommonArg->abWrite[12] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[12], temp, len, NULL);
        }
    }
    *((uint32_t *)ptCommonArg->apvArgument[11]) = ret;

    //dprintf("\n\n\%s: oret = 0x%x, ret = 0x%x, len = %d\n", __FUNCTION__, ret, *((uint32_t *)ptCommonArg->apvArgument[11]), len);

    hci_FreeMem(temp);
/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInOAEPParam, ((binstr_t *)pTrustzone->pInOAEPParam)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_DataDecrypt(pTrustzone->pKeyInRef, pTrustzone->u4Alg, pTrustzone->u4Pad,
        pTrustzone->pInSaltBuf, pTrustzone->pInOAEPParam, pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
        memcpy(pTrustzone->pOutData, out_data->buf, out_data->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_StreamDecryptSize(MTDMX_MWIF_HCI_DATA_OP_T* pTrustzone)
{

/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInOAEPParam, ((binstr_t *)pTrustzone->pInOAEPParam)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_StreamDecrypt(pTrustzone->pKeyInRef, pTrustzone->u4Alg, pTrustzone->u4Pad,
        pTrustzone->pInSaltBuf, pTrustzone->pInOAEPParam, pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        pTrustzone->u4Size = out_data->len;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Size);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_DecryptStreamWithOffset(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t len;
    uint32_t ret;
    uint8_t buf[4 * L_AES128_KEY_SIZE];
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_DecryptStreamWithOffset(in_ref,
                              *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                              *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                              *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                              *((uint32_t *)ptCommonArg->apvArgument[4]),
                              *((uint32_t *)ptCommonArg->apvArgument[5]),
                              (ptCommonArg->ui4Length[6]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[6],
                              *((uint32_t *)ptCommonArg->apvArgument[7]),
                              (ptCommonArg->ui4Length[8]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[8],
                              (ptCommonArg->ui4Length[9]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[9],
                              (ptCommonArg->ui4Length[10]==0)?NULL:&buf);


    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->ui4Length[10] != 0 && ptCommonArg->abWrite[10] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[10], buf, *((uint8_t *)ptCommonArg->apvArgument[9]), NULL);
        }
    }
    *((uint32_t *)ptCommonArg->apvArgument[11]) = ret;

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[11]), *((uint8_t *)ptCommonArg->apvArgument[9]));
    return TRUE;
}

BOOL SvcEx_HCI_EncryptStreamWithIV(TEE_COMMON_ARG_T *ptCommonArg)
{
//    uint32_t len;
    uint32_t ret;
    uint8_t buf[3 * 1024];
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_EncryptStreamWithIV(in_ref,
                              *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                              *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                              *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                              *((hci_bool_t *)ptCommonArg->apvArgument[4]),
                              *((uint32_t *)ptCommonArg->apvArgument[5]),
                              (ptCommonArg->ui4Length[6]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[6],
                              *((uint32_t *)ptCommonArg->apvArgument[7]),
                              (ptCommonArg->ui4Length[8]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[8],
                              (ptCommonArg->ui4Length[9]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[9],
                              (ptCommonArg->ui4Length[10]==0)?NULL:&buf);


    if (ret == ERR_HCI_OK)
    {
//        *((uint32_t *)ptCommonArg->apvArgument[9]) = len;
        if (ptCommonArg->ui4Length[10] != 0 && ptCommonArg->abWrite[10] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[10], buf, *((uint32_t *)ptCommonArg->apvArgument[9]), NULL);
        }
    }
    *((uint32_t *)ptCommonArg->apvArgument[11]) = ret;

    //dprintf("\n\n\%s: oret = 0x%x, ret = 0x%x, len = %d\n", __FUNCTION__, ret, *((uint32_t *)ptCommonArg->apvArgument[11]), *((uint32_t *)ptCommonArg->apvArgument[9]));

    return TRUE;
}

BOOL SvcEx_HCI_StreamDecrypt(TEE_COMMON_ARG_T *ptCommonArg)
{
//    uint32_t len;
//    uint32_t ret;
//    uint8_t buf[3 * 1024];
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    *((uint32_t *)ptCommonArg->apvArgument[11]) = hci_DecryptStreamWithIV(in_ref,
                          *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                          *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                          *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                          *((hci_bool_t *)ptCommonArg->apvArgument[4]),
                          *((uint32_t *)ptCommonArg->apvArgument[5]),
                          (ptCommonArg->ui4Length[6]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[6],
                          *((uint32_t *)ptCommonArg->apvArgument[7]),
                          (ptCommonArg->ui4Length[8]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[8],
                          (ptCommonArg->ui4Length[9]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[9],
                          (ptCommonArg->ui4Length[10]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[10]);

/*
if (ret == ERR_HCI_OK)
{
    *((uint32_t *)ptCommonArg->apvArgument[9]) = len;
    if (ptCommonArg->ui4Length[10] != 0 && ptCommonArg->abWrite[10] == 1)
    {
        memcpy(ptCommonArg->apvArgument[10], buf, len);
    }
}
*/
//    *((uint32_t *)ptCommonArg->apvArgument[11]) = ret;

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[11]), *((uint32_t *)ptCommonArg->apvArgument[9]));

/*
    uint32_t len;
    uint32_t ret;
    uint8_t buf[3 * 1024];

    ret = hci_DecryptStreamWithIV((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]),
                              *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                              *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                              *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                              *((hci_bool_t *)ptCommonArg->apvArgument[4]),
                              *((uint32_t *)ptCommonArg->apvArgument[5]),
                              (ptCommonArg->ui4Length[6]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[6],
                              *((uint32_t *)ptCommonArg->apvArgument[7]),
                              (ptCommonArg->ui4Length[8]==0)?NULL:(uint8_t *)ptCommonArg->apvArgument[8],
                              (ptCommonArg->ui4Length[9]==0)?NULL:&len,
                              (ptCommonArg->ui4Length[10]==0)?NULL:&buf);


    if (ret == ERR_HCI_OK)
    {
        *((uint32_t *)ptCommonArg->apvArgument[9]) = len;
        if (ptCommonArg->ui4Length[10] != 0 && ptCommonArg->abWrite[10] == 1)
        {
            memcpy(ptCommonArg->apvArgument[10], buf, len);
        }
    }
    *((uint32_t *)ptCommonArg->apvArgument[11]) = ret;

    dprintf("\n\n\%s: oret = 0x%x, ret = 0x%x, len = %d\n", __FUNCTION__, ret, *((uint32_t *)ptCommonArg->apvArgument[11]), len);
*/


/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInOAEPParam, ((binstr_t *)pTrustzone->pInOAEPParam)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_StreamDecrypt(pTrustzone->pKeyInRef, pTrustzone->u4Alg, pTrustzone->u4Pad,
        pTrustzone->pInSaltBuf, pTrustzone->pInOAEPParam, pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
        memcpy(pTrustzone->pOutData, out_data->buf, out_data->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_KeyDataEncryptSize(MTDMX_MWIF_HCI_DATA_OP_T* pTrustzone)
{
/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInOAEPParam, ((binstr_t *)pTrustzone->pInOAEPParam)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_KeyDataEncrypt(pTrustzone->pKeyInRef, pTrustzone->pInOAEPParam,
        pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        pTrustzone->u4Size = out_data->len;
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Size);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_LoadKey(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    hci_key_ref_t keyref = NULL;

    if (g_handle_count + 1 >= HCI_MAPTBL_SIZE)
    {
        *((uint32_t *)ptCommonArg->apvArgument[3]) = ERR_HCI_INTERNALERROR;
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return TRUE;
    }
    
    if (ptCommonArg->ui4Length[1] != 0)
    {
        if (*((hci_load_key_type_t *)ptCommonArg->apvArgument[0]) == HCI_LOAD_KEY_TYPE_OCTPUB)
        {
            ((hci_key_data_t *)ptCommonArg->apvArgument[1])->pub_key.pub_n = (ptCommonArg->ui4Length[4]==0)?NULL:ptCommonArg->apvArgument[4];
            ((hci_key_data_t *)ptCommonArg->apvArgument[1])->pub_key.pub_e = (ptCommonArg->ui4Length[5]==0)?NULL:ptCommonArg->apvArgument[5];
        }

        if (*((hci_load_key_type_t *)ptCommonArg->apvArgument[0]) == HCI_LOAD_KEY_TYPE_BROADCASTKEY)
        {
            ((hci_key_data_t *)ptCommonArg->apvArgument[1])->raw_key.buf = (ptCommonArg->ui4Length[6]==0)?NULL:ptCommonArg->apvArgument[6];
        }
    }

    ret = hci_LoadKey(*((hci_load_key_type_t *)ptCommonArg->apvArgument[0]),
                      (ptCommonArg->ui4Length[1]==0)?NULL:(hci_key_data_t *)ptCommonArg->apvArgument[1],
                      &keyref);

    *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]) = ret;


    if (ret == ERR_HCI_OK)
    {
        g_handle_count++;
        if (_SvcEx_HCI_CreateHandle(&keyref) != TRUE)
            *((uint32_t *)ptCommonArg->apvArgument[3]) = ERR_HCI_INTERNALERROR;
        *((hci_key_ref_t *)ptCommonArg->apvArgument[2]) = keyref;
    }

    //dprintf("\n\n\%s: ret = 0x%x, ref = 0x%x\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[3]),
    //                                  *((hci_key_ref_t *)ptCommonArg->apvArgument[2]));

    return TRUE;
}

BOOL SvcEx_HCI_ImportBKBContainer(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    uint8_t *pContainer = NULL;
    uint8_t *pOutbk = NULL;

    ret = hci_ImportBKBContainer(*((uint32_t *)ptCommonArg->apvArgument[0]),
                                 (ptCommonArg->abWrite[1]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[1],
                                 *((hci_bool_t *)ptCommonArg->apvArgument[2]),
                                 *((uint32_t *)ptCommonArg->apvArgument[3]),
                                 (ptCommonArg->abWrite[4]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[4],
                                 (ptCommonArg->ui4Length[5]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[5],
                                 (ptCommonArg->abWrite[6]==2)?NULL:&pContainer,
                                 (ptCommonArg->ui4Length[7]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[7],
                                 (ptCommonArg->abWrite[8]==2)?NULL:&pOutbk);

    *((uint32_t *)ptCommonArg->apvArgument[9]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->abWrite[6] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[6], pContainer, *((uint32_t *)ptCommonArg->apvArgument[5]), NULL);
        }
        if (ptCommonArg->abWrite[8] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[8], pOutbk, *((uint32_t *)ptCommonArg->apvArgument[7]), NULL);
        }
    }

    hci_FreeMem(pContainer);
    hci_FreeMem(pOutbk);

    //dprintf("\n\n\%s: ret = 0x%x, conLen = %d, bklen = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[9]),
    //                                  *((hci_key_ref_t *)ptCommonArg->apvArgument[5]), *((hci_key_ref_t *)ptCommonArg->apvArgument[7]));

    return TRUE;
}

BOOL SvcEx_HCI_ImportPpprocData(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    uint8_t *pPPProc = NULL;

    ret = hci_ImportPpprocData(*((uint32_t *)ptCommonArg->apvArgument[0]),
                               (ptCommonArg->abWrite[1]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[1],
                               *((hci_ppproc_format_t *)ptCommonArg->apvArgument[2]),
                               (ptCommonArg->ui4Length[3]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[3],
                               (ptCommonArg->abWrite[4]==2)?NULL:&pPPProc);

    *((uint32_t *)ptCommonArg->apvArgument[5]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->abWrite[4] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[4], pPPProc, *((uint32_t *)ptCommonArg->apvArgument[3]), NULL);
        }
    }

    hci_FreeMem(pPPProc);

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[5]),
    //                                  *((uint32_t *)ptCommonArg->apvArgument[3]));

    return TRUE;
}

BOOL SvcEx_HCI_GenerateHMAC(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    uint8_t *pHmac = NULL;

    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_GenerateHMAC(in_ref,
                           *((hci_dig_algo_t *)ptCommonArg->apvArgument[1]),
                           *((uint32_t *)ptCommonArg->apvArgument[2]),
                           (ptCommonArg->abWrite[3]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                           (ptCommonArg->ui4Length[4]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[4],
                           (ptCommonArg->abWrite[5]==2)?NULL:&pHmac);

    *((uint32_t *)ptCommonArg->apvArgument[6]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->abWrite[5] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[5], pHmac, *((uint32_t *)ptCommonArg->apvArgument[4]), NULL);
        }
    }

    hci_FreeMem(pHmac);

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[6]),
    //                                  *((uint32_t *)ptCommonArg->apvArgument[4]));
    
    return TRUE;
}

BOOL SvcEx_HCI_GenerateHash(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    uint8_t *pHash = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_GenerateHash(in_ref,
                           *((hci_dig_algo_t *)ptCommonArg->apvArgument[1]),
                           *((uint32_t *)ptCommonArg->apvArgument[2]),
                           (ptCommonArg->abWrite[3]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                           (ptCommonArg->ui4Length[4]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[4],
                           (ptCommonArg->abWrite[5]==2)?NULL:&pHash);

    *((uint32_t *)ptCommonArg->apvArgument[6]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->abWrite[5] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[5], pHash, *((uint32_t *)ptCommonArg->apvArgument[4]), NULL);
        }
    }

    hci_FreeMem(pHash);

    //dprintf("\n\n\%s: ret = 0x%x, len = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[6]),
    //                                  *((uint32_t *)ptCommonArg->apvArgument[4]));

    return TRUE;
}

BOOL SvcEx_HCI_EncryptData(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint8_t *pIv = NULL;
    uint8_t *pOut_key = NULL;
    uint32_t ret;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_EncryptData(in_ref,
                          *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                          *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                          *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                          *((uint32_t *)ptCommonArg->apvArgument[4]),
                          (ptCommonArg->abWrite[5]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[5],
                          *((uint32_t *)ptCommonArg->apvArgument[6]),
                          (ptCommonArg->abWrite[7]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[7],
                          (ptCommonArg->ui4Length[8]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[8],
                          (ptCommonArg->abWrite[9]==2)?NULL:&pIv,
                          (ptCommonArg->ui4Length[10]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[10],
                          (ptCommonArg->abWrite[11]==2)?NULL:&pOut_key);

    *((hci_enc_pad_t *)ptCommonArg->apvArgument[12]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->abWrite[9]==1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[9], pIv, *((uint32_t *)ptCommonArg->apvArgument[8]), NULL);
        }
        if (ptCommonArg->abWrite[11]==1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[11], pOut_key, *((uint32_t *)ptCommonArg->apvArgument[10]), NULL);
        }
    }

    hci_FreeMem(pIv);
    hci_FreeMem(pOut_key);

    //dprintf("\n\n\%s: ret = 0x%x, ivlen = %d, keylen = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[12]),
    //                                  *((uint32_t *)ptCommonArg->apvArgument[8]),
    //                                  *((uint32_t *)ptCommonArg->apvArgument[10]));

    return TRUE;
}

BOOL SvcEx_HCI_KeyDataEncrypt(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint8_t *pIv = NULL;
    uint8_t *pOut_key = NULL;
    uint32_t ret;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    hci_key_ref_t in_ref2 = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[6]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[6]));
    
    ret = hci_EncryptKeyData(in_ref,
                              *((hci_enc_algo_t *)ptCommonArg->apvArgument[1]),
                              *((hci_enc_mode_t *)ptCommonArg->apvArgument[2]),
                              *((hci_enc_pad_t *)ptCommonArg->apvArgument[3]),
                              *((uint32_t *)ptCommonArg->apvArgument[4]),
                              (ptCommonArg->abWrite[5]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[5],
                              in_ref2,
                              (ptCommonArg->ui4Length[7]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[7],
                              (ptCommonArg->abWrite[8]==2)?NULL:&pIv,
                              (ptCommonArg->ui4Length[9]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[9],
                              (ptCommonArg->abWrite[10]==2)?NULL:&pOut_key);

    *((hci_enc_pad_t *)ptCommonArg->apvArgument[11]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (pIv && ptCommonArg->abWrite[8]==1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[8], pIv, *((uint32_t *)ptCommonArg->apvArgument[7]), NULL);
        }
        if (pOut_key && ptCommonArg->abWrite[10]==1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[10], pOut_key, *((uint32_t *)ptCommonArg->apvArgument[9]), NULL);
        }
    }

    hci_FreeMem(pIv);
    hci_FreeMem(pOut_key);

    //dprintf("\n\n\%s: ret = 0x%x, ivlen = %d, keylen = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[11]),
    //                                  *((uint32_t *)ptCommonArg->apvArgument[7]),
    //                                  *((uint32_t *)ptCommonArg->apvArgument[9]));
/*
    binstr_t *out_data = NULL;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInOAEPParam, ((binstr_t *)pTrustzone->pInOAEPParam)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_KeyDataEncrypt(pTrustzone->pKeyInRef, pTrustzone->pInOAEPParam,
        pTrustzone->pInData, &out_data);
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
        memcpy(pTrustzone->pOutData, out_data->buf, out_data->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_DATA_OP_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if(out_data != NULL)
    {
        hci_BinstrFree(out_data);
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_HmacCheck(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    uint8_t *pHash = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_CheckHMAC(in_ref,
                        *((hci_dig_algo_t *)ptCommonArg->apvArgument[1]),
                        *((uint32_t *)ptCommonArg->apvArgument[2]),
                        (ptCommonArg->abWrite[3]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                        *((uint32_t *)ptCommonArg->apvArgument[4]),
                        (ptCommonArg->abWrite[5]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[5],
                        (ptCommonArg->ui4Length[6]==0)?NULL:(hci_bool_t *)ptCommonArg->apvArgument[6]);

    *((uint32_t *)ptCommonArg->apvArgument[7]) = ret;

    //dprintf("\n\n\%s: ret = 0x%x, result = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[7]),
    //                                  *((hci_bool_t *)ptCommonArg->apvArgument[6]));
    return TRUE;

/*
    UINT32 u4Result;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_HASH_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInExpected, ((binstr_t *)pTrustzone->pInExpected)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_HmacCheck(pTrustzone->pKeyInRef, pTrustzone->u4Alg, pTrustzone->pInData,
        pTrustzone->pInExpected, &u4Result);
    pTrustzone->u4OutResult = u4Result;
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_HASH_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, u4Result);
*/
    return TRUE;
}

BOOL SvcEx_HCI_HashCheck(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
//    hci_bool_t result;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef((ptCommonArg->ui4Length[0]==0)?NULL:*((hci_key_ref_t *)ptCommonArg->apvArgument[0]));
    
    ret = hci_CheckHash(in_ref,
                        *((hci_dig_algo_t *)ptCommonArg->apvArgument[1]),
                        *((uint32_t *)ptCommonArg->apvArgument[2]),
                        (ptCommonArg->abWrite[3] == 2)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                        *((uint32_t *)ptCommonArg->apvArgument[4]),
                        (ptCommonArg->abWrite[5] == 2)?NULL:(uint8_t *)ptCommonArg->apvArgument[5],
                        (ptCommonArg->ui4Length[6]==0)?NULL:(hci_bool_t *)ptCommonArg->apvArgument[6]);

    *((uint32_t *)ptCommonArg->apvArgument[7]) = ret;
/*
    if (ret == ERR_HCI_OK)
    {
        *((hci_bool_t *)ptCommonArg->apvArgument[6]) = result;
    }
*/
    //dprintf("\n\n\%s: ret = 0x%x, result = %d\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[7]), *((hci_bool_t *)ptCommonArg->apvArgument[6]));

    return TRUE;

/*
    UINT32 u4Result;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_HASH_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInExpected, ((binstr_t *)pTrustzone->pInExpected)->len + sizeof(binstr_t));
    pTrustzone->u4Ret = hci_HashCheck(pTrustzone->pKeyInRef, pTrustzone->u4Alg, pTrustzone->pInData,
        pTrustzone->pInExpected, &u4Result);
    pTrustzone->u4OutResult = u4Result;
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_HASH_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, u4Result);
    return TRUE;
*/
}

BOOL SvcEx_HCI_MoveKeySize(MTDMX_MWIF_HCI_MOVEKEY_T* pTrustzone)
{
    uint8_t      *opv_buf    = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef(pTrustzone->pKeyRef);
//    binstr_t *out_data = NULL;

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
    TZ_SWD_BufferStart((UINT32)pTrustzone->pInData, TZ_CACHE_ALIGN(pTrustzone->u4InDataLen));
    pTrustzone->u4Ret = hci_MoveKeySize(in_ref, pTrustzone->u4InDataLen, pTrustzone->pInData,
                                        &pTrustzone->u4Size, &opv_buf);
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pInData, TZ_CACHE_ALIGN(pTrustzone->u4InDataLen));
/*
    if((pTrustzone->u4Ret == ERR_OK) && (pTrustzone->pOutData != NULL))
    {
        pTrustzone->u4Size = out_data->len;
    }
*/
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));

//    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Size);

    if(opv_buf != NULL)
    {
        hci_FreeMem(opv_buf);
//        hci_l_FreeKey(out_data);
    }

    return TRUE;
}

BOOL SvcEx_HCI_MoveKey(MTDMX_MWIF_HCI_MOVEKEY_T* pTrustzone)
{
    uint8_t      *opv_buf    = NULL;
    hci_key_ref_t in_ref = _SvcEx_HCI_GetRef(pTrustzone->pKeyRef);

//    binstr_t *out_data = NULL;
    TZ_SWD_BufferStart((UINT32)pTrustzone->pInData, TZ_CACHE_ALIGN(pTrustzone->u4InDataLen));
    TZ_SWD_BufferStart((UINT32)pTrustzone->pOutData, TZ_CACHE_ALIGN(pTrustzone->u4Size));

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, ((binstr_t *)pTrustzone->pInData)->len + sizeof(binstr_t));
//    pTrustzone->u4Ret = hci_MoveKey(pTrustzone->pKeyRef, pTrustzone->pInData, &out_data);
    pTrustzone->u4Ret = hci_MoveKey(in_ref, pTrustzone->u4InDataLen, pTrustzone->pInData,
                                        &pTrustzone->u4Size, &opv_buf);

    if(pTrustzone->u4Ret == ERR_HCI_OK)
    {
        memcpy_s2n_chk(pTrustzone->pOutData, opv_buf, pTrustzone->u4Size, NULL);
    }

    TZ_SWD_BufferEnd((UINT32)pTrustzone->pInData, TZ_CACHE_ALIGN(pTrustzone->u4InDataLen));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pOutData, TZ_CACHE_ALIGN(pTrustzone->u4Size));

/*
    if((pTrustzone->u4Ret == ERR_OK) && (out_data != NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
        memcpy(pTrustzone->pOutData, out_data->buf, out_data->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutData, out_data->len);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_MOVEKEY_T));
*/
//    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);


    if(opv_buf != NULL)
    {
        hci_FreeMem(opv_buf);
//        hci_l_FreeKey(out_data);
    }

    return TRUE;
}

BOOL SvcEx_HCI_FreeMem(MTDMX_MWIF_HCI_FREEMEM_T* pTrustzone)
{
    hci_FreeMem(pTrustzone->pPtr);

    return TRUE;
}

BOOL SvcEx_HCI_PersonalityKeyLoad(MTDMX_MWIF_HCI_PERSONALITY_KEY_T* pTrustzone)
{
    if (pTrustzone->pInData != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pInData, TZ_CACHE_ALIGN(pTrustzone->u4InSize));
    }

/*
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInData, pTrustzone->u4InSize);
*/
    pTrustzone->u4Ret = hci_LoadPersonalityKey((hci_pers_key_type_t)pTrustzone->u4KeyType,
        pTrustzone->u4InSize, pTrustzone->pInData);

    if (pTrustzone->pInData != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pInData, TZ_CACHE_ALIGN(pTrustzone->u4InSize));
    }

//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));

    //dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    return TRUE;
}

BOOL SvcEx_HCI_PersonalityKeyCheck(TEE_COMMON_ARG_T *ptCommonArg)
{

    *((uint32_t *)ptCommonArg->apvArgument[1]) =
        hci_CheckPersonalityKey(*((hci_pers_key_type_t *)ptCommonArg->apvArgument[0]));

    //dprintf("\n\n\%s: ret = 0x%x\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[1]));

/*
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));
    pTrustzone->u4Ret = hci_PersonalityKeyCheck((hci_pers_key_type_t)pTrustzone->u4KeyType);
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_PERSONALITY_KEY_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);
*/
    return TRUE;
}

BOOL SvcEx_HCI_ImportSasSize(MTDMX_MWIF_HCI_IMPORT_SAS_T* pTrustzone)
{
/*
    binstr_t *out_public_sas = NULL;
    hci_sas_keys_t *out_sas_keys = NULL;
    binstr_t **pp_out_public_sas;
    hci_sas_keys_t **pp_out_sas_keys;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_SAS_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInSAS, ((binstr_t *)pTrustzone->pInSAS)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInApp, ((binstr_t *)pTrustzone->pInApp)->len + sizeof(binstr_t));
    pp_out_public_sas = &out_public_sas;
    if(pTrustzone->pOutSAS == NULL)
    {
        pp_out_public_sas = NULL;
    }

    pp_out_sas_keys = &out_sas_keys;
    if(pTrustzone->pOutKeys == NULL)
    {
        pp_out_sas_keys = NULL;
    }

    pTrustzone->u4Ret = hci_ImportSAS(pTrustzone->pInSAS, pTrustzone->pInApp,
        pp_out_public_sas, pp_out_sas_keys);
    if((pTrustzone->u4Ret == ERR_OK) && (pTrustzone->pOutSAS != NULL) && (pTrustzone->pOutKeys != NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutSAS, sizeof(binstr_t));
        HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutKeys, sizeof(hci_sas_keys_t));
        memcpy(pTrustzone->pOutSAS, out_public_sas, sizeof(binstr_t));
        memcpy(pTrustzone->pOutKeys, out_sas_keys, sizeof(hci_sas_keys_t));
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutSAS, sizeof(binstr_t));
        HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->pOutKeys, sizeof(hci_sas_keys_t));
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_SAS_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if((pp_out_public_sas != NULL) && (*pp_out_public_sas != NULL))
    {
        hci_BinstrFree(*pp_out_public_sas);
        pp_out_public_sas = NULL;
    }
    if((pp_out_sas_keys != NULL) && (*pp_out_sas_keys != NULL))
    {
        hci_FreeSASKeys(*pp_out_sas_keys);
        pp_out_sas_keys = NULL;
    }
*/
    return TRUE;
}

#define L_MS3_CONTENT_ID_SIZE     20

BOOL SvcEx_HCI_ImportSas(TEE_COMMON_ARG_T *ptCommonArg)
{
    uint32_t ret;
    uint8_t *pSas = NULL;
    hci_sas_keys_t *pSasKey = NULL;
    uint8_t *pCur = NULL;
    int i;
/*
hci_ImportSAS(uint32_t          in_sas_len, 0
              uint8_t          *in_sas, 1
              uint32_t          in_private_data_len, 2
              uint8_t          *in_private_data, 3
              uint32_t         *out_public_sas_len, 4
              uint8_t         **out_public_sas, 5
              hci_sas_keys_t  **out_sas_keys) 6
*/
    if (g_handle_count + 1 >= HCI_MAPTBL_SIZE)
    {
        *((uint32_t *)ptCommonArg->apvArgument[7]) = ERR_HCI_INTERNALERROR;
        dprintf("\n\n%s: ERROR*** Mapping table is full\n", __FUNCTION__);
        return TRUE;
    }
    
    ret = hci_ImportSAS(*((uint32_t *)ptCommonArg->apvArgument[0]),
                        (ptCommonArg->abWrite[1]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[1],
                        *((uint32_t *)ptCommonArg->apvArgument[2]),
                        (ptCommonArg->abWrite[3]==2)?NULL:(uint8_t *)ptCommonArg->apvArgument[3],
                        (ptCommonArg->ui4Length[4]==0)?NULL:(uint32_t *)ptCommonArg->apvArgument[4],
                        (ptCommonArg->abWrite[5]==2)?NULL:&pSas,
                        (ptCommonArg->abWrite[6]==2)?NULL:&pSasKey);


    *((uint32_t *)ptCommonArg->apvArgument[7]) = ret;

    if (ret == ERR_HCI_OK)
    {
        if (ptCommonArg->abWrite[5] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[5], pSas, *((uint32_t *)ptCommonArg->apvArgument[4]), NULL);
        }
        if (ptCommonArg->abWrite[6] == 1)
        {
            memcpy_s2n_chk(ptCommonArg->apvArgument[6], pSasKey, sizeof(hci_sas_keys_t), NULL);
        }
        if (ptCommonArg->abWrite[8] == 1)
        {
            if (pSasKey->key_count > 0)
            {
                pCur = ptCommonArg->apvArgument[8];
                for (i=0; i<pSasKey->key_count; i++)
                {
                    memcpy_s2n_chk(pCur, pSasKey->contentids[i], L_MS3_CONTENT_ID_SIZE, NULL);
                    pCur+=L_MS3_CONTENT_ID_SIZE;
                    g_handle_count++;
                    if (_SvcEx_HCI_CreateHandle(&pSasKey->key_refs[i]) != TRUE)
                        *((uint32_t *)ptCommonArg->apvArgument[7]) = ERR_HCI_INTERNALERROR;
                    *((hci_key_ref_t*)pCur) = pSasKey->key_refs[i];
                    pCur+=sizeof(hci_key_ref_t);
                    //dprintf("TZ)Get cid: %s\n", pSasKey->contentids[i]);
                    //dprintf("TZ)Get key_refs: %x\n", (unsigned long)pSasKey->key_refs[i]);
                }
            }
            
            if ( pSasKey != NULL ) 
            {
                if ( pSasKey->contentids != NULL ) {
                    for ( i = 0; i < pSasKey->key_count; i++ ) {
                        hci_l_mem_Free(pSasKey->contentids[i]);
                    }
                    hci_l_mem_Free(pSasKey->contentids);
                }
                if ( pSasKey->key_refs != NULL ) {
                    hci_l_mem_Free(pSasKey->key_refs);
                }
                hci_l_mem_Free(pSasKey);
                pSasKey = NULL;
            }
        }
        else
        {
            hci_FreeSASKeys(pSasKey);
        }
        hci_FreeMem(pSas);
    }

    //dprintf("\n\n\%s: ret = 0x%x\n", __FUNCTION__, *((uint32_t *)ptCommonArg->apvArgument[7]));

/*
    binstr_t *out_public_sas = NULL;
    hci_sas_keys_t *out_sas_keys = NULL;
    binstr_t **pp_out_public_sas;
    hci_sas_keys_t **pp_out_sas_keys;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_SAS_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInSAS, ((binstr_t *)pTrustzone->pInSAS)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInApp, ((binstr_t *)pTrustzone->pInApp)->len + sizeof(binstr_t));
    pp_out_public_sas = &out_public_sas;
    if(pTrustzone->pOutSAS == NULL)
    {
        pp_out_public_sas = NULL;
    }

    pp_out_sas_keys = &out_sas_keys;
    if(pTrustzone->pOutKeys == NULL)
    {
        pp_out_sas_keys = NULL;
    }

    pTrustzone->u4Ret = hci_ImportSAS(pTrustzone->pInSAS, pTrustzone->pInApp,
        pp_out_public_sas, pp_out_sas_keys);
    if((pTrustzone->u4Ret == ERR_OK) && (pTrustzone->pOutSAS != NULL) && (pTrustzone->pOutKeys != NULL))
    {
        HalInvalidateDCacheMultipleLine((UINT32)((binstr_t *)pTrustzone->pOutSAS)->buf, out_public_sas->len);
        HalInvalidateDCacheMultipleLine((UINT32)((hci_sas_keys_t *)pTrustzone->pOutKeys)->key_refs, sizeof(hci_key_ref_t) * pTrustzone->u4OutKeysCount);
        memcpy(((binstr_t *)pTrustzone->pOutSAS)->buf, out_public_sas->buf, out_public_sas->len);
        memcpy(((hci_sas_keys_t *)pTrustzone->pOutKeys)->key_refs, out_sas_keys->key_refs, sizeof(hci_key_ref_t) * pTrustzone->u4OutKeysCount);
        HalFlushInvalidateDCacheMultipleLine((UINT32)((binstr_t *)pTrustzone->pOutSAS)->buf, out_public_sas->len);
        HalFlushInvalidateDCacheMultipleLine((UINT32)((hci_sas_keys_t *)pTrustzone->pOutKeys)->key_refs, sizeof(hci_key_ref_t) * pTrustzone->u4OutKeysCount);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_SAS_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if((pp_out_public_sas != NULL) && (*pp_out_public_sas != NULL))
    {
        hci_BinstrFree(*pp_out_public_sas);
        pp_out_public_sas = NULL;
    }
    if((pp_out_sas_keys != NULL) && (*pp_out_sas_keys != NULL))
    {
        hci_FreeSASKeys(*pp_out_sas_keys);
        pp_out_sas_keys = NULL;
    }
*/
    return TRUE;
}

BOOL SvcEx_HCI_ImportSasIds(MTDMX_MWIF_HCI_IMPORT_SAS_T* pTrustzone)
{
/*
    binstr_t *out_public_sas = NULL;
    hci_sas_keys_t *out_sas_keys = NULL;
    binstr_t **pp_out_public_sas;
    hci_sas_keys_t **pp_out_sas_keys;
    UINT32 i;

    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_SAS_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInSAS, ((binstr_t *)pTrustzone->pInSAS)->len + sizeof(binstr_t));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->pInApp, ((binstr_t *)pTrustzone->pInApp)->len + sizeof(binstr_t));
    pp_out_public_sas = &out_public_sas;
    if(pTrustzone->pOutSAS == NULL)
    {
        pp_out_public_sas = NULL;
    }

    pp_out_sas_keys = &out_sas_keys;
    if(pTrustzone->pOutKeys == NULL)
    {
        pp_out_sas_keys = NULL;
    }

    pTrustzone->u4Ret = hci_ImportSAS(pTrustzone->pInSAS, pTrustzone->pInApp,
        pp_out_public_sas, pp_out_sas_keys);
    if(pTrustzone->u4Ret == ERR_OK)
    {
        for(i = 0; i < pTrustzone->u4OutKeysCount; i++)
        {
            if(pTrustzone->pOutKeys != NULL)
            {
                HalInvalidateDCacheMultipleLine(((hci_sas_keys_t *)pTrustzone->pOutKeys)->contentids[i]->buf, L_MS3_CONTENT_ID_SIZE);
                memcpy(((hci_sas_keys_t *)pTrustzone->pOutKeys)->contentids[i]->buf, out_sas_keys->contentids[i]->buf, L_MS3_CONTENT_ID_SIZE);
                HalFlushInvalidateDCacheMultipleLine(((hci_sas_keys_t *)pTrustzone->pOutKeys)->contentids[i]->buf, L_MS3_CONTENT_ID_SIZE);
            }
        }
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_HCI_IMPORT_SAS_T));

    dprintf("\n\n\%s: %d\n", __FUNCTION__, pTrustzone->u4Ret);

    if((pp_out_public_sas != NULL) && (*pp_out_public_sas != NULL))
    {
        hci_BinstrFree(*pp_out_public_sas);
        pp_out_public_sas = NULL;
    }
    if((pp_out_sas_keys != NULL) && (*pp_out_sas_keys != NULL))
    {
        hci_FreeSASKeys(*pp_out_sas_keys);
        pp_out_sas_keys = NULL;
    }
*/
    return TRUE;
}
#endif // CC_ENABLE_HCI


#ifdef CC_ENABLE_SKB




BOOL SvcEx_SKB_GetInstance(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T));
    pTrustzone->rRet = SKB_Engine_GetInstance(&pTrustzone->pEngine);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->pEngine);
    return TRUE;
}

BOOL SvcEx_SKB_Engine_Release(MTDMX_MWIF_SKB_ENGINE_RELEASE_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_RELEASE_T));
    pTrustzone->rRet = SKB_Engine_Release(pTrustzone->pEngine);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_RELEASE_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_GetInfo(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
	if(NULL != pTrustzone->pInfo)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

	
    pTrustzone->rRet = SKB_Engine_GetInfo(pTrustzone->pEngine, pTrustzone->pInfo);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));

	if(NULL != pTrustzone->pInfo)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

	if(NULL != pTrustzone->pInfo)
	    dprintf("\nproperty_count = %d\n", pTrustzone->pInfo->property_count); 
	    
    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}


/// to-do: fix
BOOL SvcEx_SKB_GetProertiesSize(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
	if(NULL != pTrustzone->pInfo)
		TZ_SWD_BufferStart((UINT32)pTrustzone->pInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

    pTrustzone->rRet = SKB_Engine_GetInfo(pTrustzone->pEngine, pTrustzone->pInfo);


	if(NULL != pTrustzone->pInfo && NULL != pTrustzone->pInfo->properties)
    {
    	pTrustzone->u4PropertiesNameSize  = x_strlen(pTrustzone->pInfo->properties[pTrustzone->u4PropertiesIndex].name); 
	    pTrustzone->u4PropertiesValueSize = x_strlen(pTrustzone->pInfo->properties[pTrustzone->u4PropertiesIndex].value);
	}
	
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
	if(NULL != pTrustzone->pInfo)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pInfo, TZ_CACHE_ALIGN(sizeof(SKB_EngineInfo)));

    dprintf("\n\n\%s: 0x%x, u4PropertiesNameSize=%d, u4PropertiesValueSize=%d\n", __FUNCTION__, pTrustzone->rRet, pTrustzone->u4PropertiesNameSize, pTrustzone->u4PropertiesValueSize);
    return TRUE;
}

/// to-do: fix

BOOL SvcEx_SKB_GetProertiesData(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T* pTrustzone)
{
    SKB_EngineInfo rInfo;

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
/*
    TZ_SWD_BufferStart((UINT32)pTrustzone->pProperties->name, TZ_CACHE_ALIGN(pTrustzone->u4PropertiesNameSize));
    dprintf("\nSvcEx_SKB_GetProertiesData 111111111111\n");
    TZ_SWD_BufferStart((UINT32)pTrustzone->pProperties->value, TZ_CACHE_ALIGN(pTrustzone->u4PropertiesValueSize));
    TZ_SWD_BufferStart((UINT32)pTrustzone->pProperties, TZ_CACHE_ALIGN(sizeof(SKB_EngineProperty)));
*/

	if(NULL != pTrustzone->pProperties->name)		
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pProperties->name, TZ_CACHE_ALIGN(pTrustzone->u4PropertiesNameSize));

	if(NULL != pTrustzone->pProperties->value)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pProperties->value, TZ_CACHE_ALIGN(pTrustzone->u4PropertiesValueSize));

    pTrustzone->rRet = SKB_Engine_GetInfo(pTrustzone->pEngine, &rInfo);

    pTrustzone->u4PropertiesNameSize = x_strlen(rInfo.properties[pTrustzone->u4PropertiesIndex].name);
    pTrustzone->u4PropertiesValueSize = x_strlen(rInfo.properties[pTrustzone->u4PropertiesIndex].value);


    memcpy_s2n_chk(pTrustzone->pProperties->name,
        rInfo.properties[pTrustzone->u4PropertiesIndex].name, pTrustzone->u4PropertiesNameSize, NULL);
    memcpy_s2n_chk(pTrustzone->pProperties->value,
        rInfo.properties[pTrustzone->u4PropertiesIndex].value, pTrustzone->u4PropertiesValueSize, NULL);

	if(NULL != pTrustzone->pProperties->name)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pProperties->name, TZ_CACHE_ALIGN(pTrustzone->u4PropertiesNameSize));

	if(NULL != pTrustzone->pProperties->value)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pProperties->value, TZ_CACHE_ALIGN(pTrustzone->u4PropertiesValueSize));

    dprintf("\nname = %s\n", pTrustzone->pProperties->name);
    dprintf("\nvalue = %s\n", pTrustzone->pProperties->value);

/*
    pTrustzone->u4PropertiesNameSize = x_strlen(rInfo.properties[pTrustzone->u4PropertiesIndex].name);
    pTrustzone->u4PropertiesValueSize = x_strlen(rInfo.properties[pTrustzone->u4PropertiesIndex].value);

    HalInvalidateDCacheMultipleLine(pTrustzone->pProperties->name, pTrustzone->u4PropertiesNameSize);
    memcpy(pTrustzone->pProperties->name,
        rInfo.properties[pTrustzone->u4PropertiesIndex].name, pTrustzone->u4PropertiesNameSize);
    HalFlushInvalidateDCacheMultipleLine(pTrustzone->pProperties->name, pTrustzone->u4PropertiesNameSize);

    HalInvalidateDCacheMultipleLine(pTrustzone->pProperties->value, pTrustzone->u4PropertiesValueSize);
    memcpy(pTrustzone->pProperties->value,
        rInfo.properties[pTrustzone->u4PropertiesIndex].value, pTrustzone->u4PropertiesValueSize);
    HalFlushInvalidateDCacheMultipleLine(pTrustzone->pProperties->value, pTrustzone->u4PropertiesValueSize);

    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_GET_INFO_T));
*/
    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_DataFromWrapped(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T));
	if(NULL == pTrustzone->wrapped)
	{
		pTrustzone->rRet = -1;
		dprintf("\n\n\%s: WRAPPED NULL\n", __FUNCTION__);
		return FALSE;
	}
	
	TZ_SWD_BufferStart((UINT32)pTrustzone->wrapped, TZ_CACHE_ALIGN(pTrustzone->wrapped_size));

    pTrustzone->rRet = SKB_Engine_CreateDataFromWrapped(pTrustzone->pEngine,
        pTrustzone->wrapped, pTrustzone->wrapped_size, pTrustzone->wrapped_type, pTrustzone->wrapped_format,
        pTrustzone->wrapping_algorithm, pTrustzone->wrapping_parameters, pTrustzone->unwrapping_key,
        &pTrustzone->data); 

	
	TZ_SWD_BufferEnd((UINT32)pTrustzone->wrapped, TZ_CACHE_ALIGN(pTrustzone->wrapped_size));

//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T));
    dprintf("\n\n\%s: 0x%x 0x%x\n", __FUNCTION__, pTrustzone->rRet, pTrustzone->data);
    return TRUE;
}

/// to-do: fix

BOOL SvcEx_SKB_DataFromExported(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T* pTrustzone)
{
	//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T));

	if(NULL != pTrustzone->exported)	
		TZ_SWD_BufferStart((UINT32)pTrustzone->exported, TZ_CACHE_ALIGN(pTrustzone->exported_size));

	pTrustzone->rRet = SKB_Engine_CreateDataFromExported(pTrustzone->pEngine, pTrustzone->exported, pTrustzone->exported_size,
	&pTrustzone->data);
	//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T));

	if(NULL != pTrustzone->exported)	
		TZ_SWD_BufferEnd((UINT32)pTrustzone->exported, TZ_CACHE_ALIGN(pTrustzone->exported_size));

	dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_CreateTransform(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T));

    switch(pTrustzone->transform_type)
    {
    case SKB_TRANSFORM_TYPE_DIGEST:
//        dprintf("\nSKB_TRANSFORM_TYPE_DIGEST alg = %d\n", ((SKB_DigestTransformParameters *)pTrustzone->transform_parameters)->algorithm);
		if(NULL != pTrustzone->transform_parameters)
			TZ_SWD_BufferStart((UINT32)pTrustzone->transform_parameters, TZ_CACHE_ALIGN(sizeof(SKB_DigestTransformParameters)));
        break;
    case SKB_TRANSFORM_TYPE_VERIFY:
		if(NULL != pTrustzone->transform_parameters)
        	TZ_SWD_BufferStart((UINT32)pTrustzone->transform_parameters, TZ_CACHE_ALIGN(sizeof(SKB_VerifyTransformParameters)));

		if(NULL != pTrustzone->transform_parameters && NULL != ((SKB_VerifyTransformParameters *)pTrustzone->transform_parameters)->signature)
        	TZ_SWD_BufferStart((UINT32)((SKB_VerifyTransformParameters *)pTrustzone->transform_parameters)->signature, TZ_CACHE_ALIGN(((SKB_VerifyTransformParameters *)pTrustzone->transform_parameters)->signature_size));
//        dprintf("\nSKB_TRANSFORM_TYPE_VERIFY\n");
        break;
    case SKB_TRANSFORM_TYPE_SIGN:
		if(NULL != pTrustzone->transform_parameters)
	        TZ_SWD_BufferStart((UINT32)pTrustzone->transform_parameters, TZ_CACHE_ALIGN(sizeof(SKB_SignTransformParameters)));
//        dprintf("\nSKB_TRANSFORM_TYPE_SIGN\n");
        break;
    }

    pTrustzone->rRet = SKB_Engine_CreateTransform(pTrustzone->pEngine, pTrustzone->transform_type,
                           pTrustzone->transform_parameters, &pTrustzone->transform);

    switch(pTrustzone->transform_type)
    {
    case SKB_TRANSFORM_TYPE_DIGEST:
		if(NULL != pTrustzone->transform_parameters)
	        TZ_SWD_BufferEnd((UINT32)pTrustzone->transform_parameters, TZ_CACHE_ALIGN(sizeof(SKB_DigestTransformParameters)));
//        dprintf("\nSKB_TRANSFORM_TYPE_DIGEST alg = %d\n", ((SKB_DigestTransformParameters *)pTrustzone->transform_parameters)->algorithm);
        break;
    case SKB_TRANSFORM_TYPE_VERIFY:
		if(NULL != pTrustzone->transform_parameters)
	        TZ_SWD_BufferEnd((UINT32)pTrustzone->transform_parameters, TZ_CACHE_ALIGN(sizeof(SKB_VerifyTransformParameters)));

		if(NULL != pTrustzone->transform_parameters && NULL != ((SKB_VerifyTransformParameters *)pTrustzone->transform_parameters)->signature)
			TZ_SWD_BufferEnd((UINT32)((SKB_VerifyTransformParameters *)pTrustzone->transform_parameters)->signature, TZ_CACHE_ALIGN(((SKB_VerifyTransformParameters *)pTrustzone->transform_parameters)->signature_size));
//        dprintf("\nSKB_TRANSFORM_TYPE_VERIFY\n");
        break;
    case SKB_TRANSFORM_TYPE_SIGN:
		if(NULL != pTrustzone->transform_parameters)
	        TZ_SWD_BufferEnd((UINT32)pTrustzone->transform_parameters, TZ_CACHE_ALIGN(sizeof(SKB_SignTransformParameters)));
//        dprintf("\nSKB_TRANSFORM_TYPE_SIGN\n");
        break;
    }
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_CreateCipher(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T));
    pTrustzone->rRet = SKB_Engine_CreateCipher(pTrustzone->pEngine, pTrustzone->cipher_algorithm,
        pTrustzone->cipher_direction, pTrustzone->cipher_flags, pTrustzone->cipher_parameters,
        pTrustzone->cipher_key, &pTrustzone->cipher);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
    }

BOOL SvcEx_SKB_SecureData_Release(MTDMX_MWIF_SKB_SECUREDATA_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    pTrustzone->rRet = SKB_SecureData_Release((SKB_SecureData *)pTrustzone->self);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_SecureDataGetInfo(MTDMX_MWIF_SKB_SECUREDATA_T* pTrustzone)
{
/*
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->info, sizeof(SKB_DataInfo));
*/
	if(NULL != pTrustzone->info)
   		TZ_SWD_BufferStart((UINT32)pTrustzone->info, TZ_CACHE_ALIGN(sizeof(SKB_DataInfo)));

	pTrustzone->rRet = SKB_SecureData_GetInfo(pTrustzone->self, pTrustzone->info);

	if(NULL != pTrustzone->info)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->info, TZ_CACHE_ALIGN(sizeof(SKB_DataInfo)));
/*
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->info, sizeof(SKB_DataInfo));
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_T));
*/
    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

/// to-do: fix maybe~~~~~~~~~~~~

BOOL SvcEx_SKB_SecureData_Export(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T* pTrustzone)
{
/*
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->buffer, *(SKB_Size*)pTrustzone->buffer_size);
*/
	if(NULL != pTrustzone->buffer_size)
		TZ_SWD_BufferStart((UINT32)pTrustzone->buffer_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

    if(NULL != pTrustzone->buffer && NULL != pTrustzone->buffer_size)
		TZ_SWD_BufferStart((UINT32)pTrustzone->buffer, TZ_CACHE_ALIGN(*(SKB_Size*)pTrustzone->buffer_size));
    

    pTrustzone->rRet = SKB_SecureData_Export(pTrustzone->self, pTrustzone->target,
            pTrustzone->target_parameters, pTrustzone->buffer, pTrustzone->buffer_size);


	if(NULL != pTrustzone->buffer_size)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->buffer_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));


    if(NULL != pTrustzone->buffer && NULL != pTrustzone->buffer_size)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->buffer, TZ_CACHE_ALIGN(*(SKB_Size*)pTrustzone->buffer_size));

/*
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->buffer, *(SKB_Size*)pTrustzone->buffer_size);
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T));
*/
	if(NULL != pTrustzone->buffer_size)
		dprintf("\n\n\%s: 0x%x, buffer size = %d\n", __FUNCTION__, pTrustzone->rRet, *(SKB_Size*)pTrustzone->buffer_size);
    return TRUE;
}

/// to-do: fix
BOOL SvcEx_SKB_SecureData_Derive(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T* pTrustzone)
{

	if(NULL != pTrustzone->parameters)
		TZ_SWD_BufferStart((UINT32)pTrustzone->parameters, TZ_CACHE_ALIGN(sizeof(SKB_SliceDerivationParameters)));

//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T));
    pTrustzone->rRet = SKB_SecureData_Derive(pTrustzone->self,
        pTrustzone->algorithm, pTrustzone->parameters, &pTrustzone->data);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T));

	if(NULL != pTrustzone->parameters)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->parameters, TZ_CACHE_ALIGN(sizeof(SKB_SliceDerivationParameters)));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_Transform_Release(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
    pTrustzone->rRet = SKB_Transform_Release(pTrustzone->self);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_Transform_AddBytes(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));
	if(NULL != pTrustzone->data)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->data, TZ_CACHE_ALIGN(pTrustzone->data_size));

	
    pTrustzone->rRet = SKB_Transform_AddBytes(pTrustzone->self, pTrustzone->data, pTrustzone->data_size);

	if(NULL != pTrustzone->data)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->data, TZ_CACHE_ALIGN(pTrustzone->data_size));
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_Transform_AddSecureData(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T));
    pTrustzone->rRet = SKB_Transform_AddSecureData(pTrustzone->self, pTrustzone->data);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

/// to-do: fix maybe
BOOL SvcEx_SKB_Transform_GetOutput(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T* pTrustzone)
{
/*
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->output, *(SKB_Size*)pTrustzone->output_size);
*/

	if(NULL != pTrustzone->output_size)	
    	TZ_SWD_BufferStart((UINT32)pTrustzone->output_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

	if(NULL != pTrustzone->output && NULL != pTrustzone->output_size)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->output, TZ_CACHE_ALIGN(*(SKB_Size*)pTrustzone->output_size));


	
    pTrustzone->rRet = SKB_Transform_GetOutput(pTrustzone->self, pTrustzone->output, pTrustzone->output_size);


	if(NULL != pTrustzone->output_size)	
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->output_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));


	if(NULL != pTrustzone->output && NULL != pTrustzone->output_size)	
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->output, TZ_CACHE_ALIGN(*(SKB_Size*)pTrustzone->output_size));


/*
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->output, *(SKB_Size*)pTrustzone->output_size);
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T));
*/
    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}
BOOL SvcEx_SKB_Cipher_ProcessBuffer(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T* pTrustzone)
{
/*
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone->out_buffer, *(SKB_Size*)pTrustzone->out_buffer_size);
*/
	//fk@@@@@@@@@!!!!!!!!!!!!!!!!!!


	if(NULL == pTrustzone->out_buffer)
	{


		if(NULL != pTrustzone->out_buffer_size)
   			TZ_SWD_BufferStart((UINT32)pTrustzone->out_buffer_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));		
	}
	else
	{



		if(NULL != pTrustzone->out_buffer_size)
   			TZ_SWD_BufferStart((UINT32)pTrustzone->out_buffer_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));		


		
		


		if(NULL != pTrustzone->in_buffer)
			TZ_SWD_BufferStart((UINT32)pTrustzone->in_buffer, TZ_CACHE_ALIGN(pTrustzone->in_buffer_size));



		if(NULL != pTrustzone->out_buffer && NULL != pTrustzone->out_buffer_size)
	    	TZ_SWD_BufferStart((UINT32)pTrustzone->out_buffer, TZ_CACHE_ALIGN(*(SKB_Size *)pTrustzone->out_buffer_size));



		if(NULL != pTrustzone->iv)
			TZ_SWD_BufferStart((UINT32)pTrustzone->iv, TZ_CACHE_ALIGN(pTrustzone->iv_size));


	}

    pTrustzone->rRet = SKB_Cipher_ProcessBuffer(pTrustzone->self, pTrustzone->in_buffer, pTrustzone->in_buffer_size,
        pTrustzone->out_buffer, pTrustzone->out_buffer_size, pTrustzone->iv, pTrustzone->iv_size);


	if(NULL == pTrustzone->out_buffer)
	{

		
		if(NULL != pTrustzone->out_buffer_size)
			TZ_SWD_BufferEnd((UINT32)pTrustzone->out_buffer_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));

	}
	else
	{

		
		if(NULL != pTrustzone->out_buffer_size)
			TZ_SWD_BufferEnd((UINT32)pTrustzone->out_buffer_size, TZ_CACHE_ALIGN(sizeof(SKB_Size)));
	
		if(NULL != pTrustzone->in_buffer)
			TZ_SWD_BufferEnd((UINT32)pTrustzone->in_buffer, TZ_CACHE_ALIGN(pTrustzone->in_buffer_size));


			
		if(NULL != pTrustzone->out_buffer  && NULL != pTrustzone->out_buffer_size)
			TZ_SWD_BufferEnd((UINT32)pTrustzone->out_buffer, TZ_CACHE_ALIGN(*(SKB_Size *)pTrustzone->out_buffer_size));


			
		if(NULL != pTrustzone->iv)
			TZ_SWD_BufferEnd((UINT32)pTrustzone->iv, TZ_CACHE_ALIGN(pTrustzone->iv_size));

	}
/*
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone->out_buffer, *(SKB_Size*)pTrustzone->out_buffer_size);
    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
*/

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

BOOL SvcEx_SKB_Cipher_Release(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T* pTrustzone)
{
//    HalInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));
    pTrustzone->rRet = SKB_Cipher_Release(pTrustzone->self);
//    HalFlushInvalidateDCacheMultipleLine((UINT32)pTrustzone, sizeof(MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T));

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->rRet);
    return TRUE;
}

#endif // CC_ENABLE_SKB


#ifdef CC_ENABLE_FUJ_MARLIN
BOOL SvcEx_DRM_TZ_LoadSecretDataType(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T* pTrustzone, UINT32 u4Size)
{
    // already know the size of each buffer
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if(pTrustzone->pRootCertBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pRootCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4RootCertLen));
    }
    if(pTrustzone->pCaCertBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pCaCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4CaCertLen));
    }
    if(pTrustzone->pClientCertBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pClientCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4ClientCertLen));
    }
    if(pTrustzone->pClientKeyBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pClientKeyBuf, TZ_CACHE_ALIGN(pTrustzone->u4ClientKeyLen));
    }

    pTrustzone->i4Ret = MediaDRM_TZ_LoadSecretDataType(NULL, pTrustzone->i4SecretDataType,
        pTrustzone->pRootCertBuf, pTrustzone->u4RootCertBufLen, &pTrustzone->u4RootCertLen,
        pTrustzone->pCaCertBuf, pTrustzone->u4CaCertBufLen, &pTrustzone->u4CaCertLen,
        pTrustzone->pClientCertBuf, pTrustzone->u4ClientCertBufLen, &pTrustzone->u4ClientCertLen,
        pTrustzone->pClientKeyBuf, pTrustzone->u4ClientKeyBufLen, &pTrustzone->u4ClientKeyLen,
        pTrustzone->prErrorInf);
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->i4Ret);
    return TRUE;
}

BOOL SvcEx_DRM_TZ_SetSecretDataType(MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T* pTrustzone, UINT32 u4Size)
{
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if(pTrustzone->pRootCertBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pRootCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4RootCertBufLen));
    }
    if(pTrustzone->pCaCertBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pCaCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4CaCertBufLen));
    }
    if(pTrustzone->pClientCertBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pClientCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4ClientCertBufLen));
    }
    if(pTrustzone->pClientKeyBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pClientKeyBuf, TZ_CACHE_ALIGN(pTrustzone->u4ClientKeyBufLen));
    }

    pTrustzone->i4Ret = MediaDRM_TZ_SetSecretDataType(NULL, pTrustzone->i4SecretDataType,
        pTrustzone->pRootCertBuf, pTrustzone->u4RootCertBufLen, &pTrustzone->u4RootCertLen,
        pTrustzone->pCaCertBuf, pTrustzone->u4CaCertBufLen, &pTrustzone->u4CaCertLen,
        pTrustzone->pClientCertBuf, pTrustzone->u4ClientCertBufLen, &pTrustzone->u4ClientCertLen,
        pTrustzone->pClientKeyBuf, pTrustzone->u4ClientKeyBufLen, &pTrustzone->u4ClientKeyLen,
        pTrustzone->prErrorInf);

    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if(pTrustzone->pRootCertBuf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pRootCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4RootCertLen));
    }
    if(pTrustzone->pCaCertBuf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pCaCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4CaCertLen));
    }
    if(pTrustzone->pClientCertBuf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pClientCertBuf, TZ_CACHE_ALIGN(pTrustzone->u4ClientCertLen));
    }
    if(pTrustzone->pClientKeyBuf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pClientKeyBuf, TZ_CACHE_ALIGN(pTrustzone->u4ClientKeyLen));
    }

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->i4Ret);
    return TRUE;
}

BOOL SvcEx_DRM_TZ_Open(MTDMX_MWIF_DRM_TZ_OPEN_T* pTrustzone, UINT32 u4Size)
{
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if(pTrustzone->pExtractInfo != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pExtractInfo, TZ_CACHE_ALIGN(sizeof(DRM_ExtractInfo)));
    }

    pTrustzone->i4Ret = DecoderDRM_TZ_Open(&pTrustzone->i4DecSlot, NULL, pTrustzone->i4SecretDataType,
        pTrustzone->pLicenseObj, pTrustzone->u4LicenseObjLen, pTrustzone->pLid,
        pTrustzone->u4LidLen, pTrustzone->pExtractInfo, pTrustzone->prErrorInf);

    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if(pTrustzone->pExtractInfo != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pExtractInfo, TZ_CACHE_ALIGN(sizeof(DRM_ExtractInfo)));
    }

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->i4Ret);
    return TRUE;
}

BOOL SvcEx_DRM_TZ_Close(MTDMX_MWIF_DRM_TZ_OPEN_T* pTrustzone, UINT32 u4Size)
{
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    pTrustzone->i4Ret = DecoderDRM_TZ_Close(pTrustzone->i4DecSlot, pTrustzone->prErrorInf);
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }

    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->i4Ret);
    return TRUE;
}

BOOL SvcEx_DRM_TZ_DecodePacketsVOD(MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T* pTrustzone, UINT32 u4Size)
{
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if(pTrustzone->pOutBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pOutBuf,
            TZ_CACHE_ALIGN((pTrustzone->u4PacketSize) * (pTrustzone->u4InNumOfPackets)));
    }
    if(pTrustzone->pInBuf != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pInBuf,
            TZ_CACHE_ALIGN((pTrustzone->u4PacketSize) * (pTrustzone->u4InNumOfPackets)));
    }
    if(pTrustzone->pOffsets != NULL)
    {
        TZ_SWD_BufferStart((UINT32)pTrustzone->pOffsets, TZ_CACHE_ALIGN(sizeof(DecoderDRM_Offset) * (pTrustzone->u4InNumOfPackets)));
    }
    pTrustzone->i4Ret = DecoderDRM_TZ_DecodePackets_VOD(pTrustzone->i4DecSlot,
        pTrustzone->u4PacketSize, pTrustzone->pInBuf, pTrustzone->u4InNumOfPackets,
		pTrustzone->pOffsets, pTrustzone->pOutBuf, pTrustzone->u4OutBufLen,
		&pTrustzone->u4OutNumOfPackets, pTrustzone->prErrorInf);
    if(pTrustzone->prErrorInf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->prErrorInf, TZ_CACHE_ALIGN(sizeof(DRM_ErrorInf)));
    }
    if(pTrustzone->pOutBuf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pOutBuf,
            TZ_CACHE_ALIGN((pTrustzone->u4PacketSize) * (pTrustzone->u4OutNumOfPackets)));
    }
    if(pTrustzone->pInBuf != NULL)
    {
        TZ_SWD_BufferEnd((UINT32)pTrustzone->pInBuf,
            TZ_CACHE_ALIGN((pTrustzone->u4PacketSize) * (pTrustzone->u4OutNumOfPackets)));
    }
    dprintf("\n\n\%s: 0x%x\n", __FUNCTION__, pTrustzone->i4Ret);
    return TRUE;
}
#endif // CC_ENABLE_FUJ_MARLIN

#ifdef CC_ENABLE_HDCP2
BOOL SvcEx_MTDRM_HDCP2_SetEncLc128(UINT32 u4Addr, UINT32 u4Size)
{

    MTDRM_TZIF_HDCP2_SET_ENC_LC128_T* pTrustzone = (MTDRM_TZIF_HDCP2_SET_ENC_LC128_T*)u4Addr;

	if(MTDRM_HDCP2_KEYLEN_ENCLC128 != pTrustzone->u4Length)
	{
		dprintf("SvcEx_MTDRM_HDCP2_SetEncLc128 size(%d not ok\n", pTrustzone->u4Length);
		return FALSE;
	}


	if(NULL != pTrustzone->pEncKey)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pEncKey, TZ_CACHE_ALIGN(pTrustzone->u4Length));


	
    pTrustzone->rRet = DRV_HDCP2_SetEncLc128(pTrustzone->pEncKey,pTrustzone->u4Length);


	if(NULL != pTrustzone->pEncKey)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pEncKey, TZ_CACHE_ALIGN(pTrustzone->u4Length));

	
    return TRUE;


}



/*
	reserved
*/
BOOL SvcEx_MTDRM_HDCP2_SetEncKey(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_SET_ENC_KEY* pTrustzone = (MTDRM_TZIF_HDCP2_SET_ENC_KEY*)u4Addr;


#if 0 //#ifdef HDCP_A2_KEY_STRUCTURE
	if(ENCRYPTION_DATA_LENGTH != pTrustzone->u4length)
	{
		dprintf("SvcEx_MTDRM_HDCP2_SetEncKey size not ok\n");
		return FALSE;
	}
#endif

	if(NULL != pTrustzone->pEncKey)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pEncKey, TZ_CACHE_ALIGN(pTrustzone->u4length));
	
    pTrustzone->rRet = DRV_HDCP2_SetEncKey(pTrustzone->pEncKey,pTrustzone->u4length);

	if(NULL != pTrustzone->pEncKey)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pEncKey, TZ_CACHE_ALIGN(pTrustzone->u4length));
	

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_GetCertInfo(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_GET_CERT_INFO* pTrustzone = (MTDRM_TZIF_HDCP2_GET_CERT_INFO*)u4Addr;

	if(MTDRM_HDCP2_CERT_RX != pTrustzone->u4length)
	{
		dprintf("\n\n%s: ERROR*** \n", __FUNCTION__);	
		return FALSE;
	}

	if(NULL != pTrustzone->pData)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pData, TZ_CACHE_ALIGN(MTDRM_HDCP2_CERT_RX));
	
    pTrustzone->rRet = DRV_HDCP2_GetCertInfo(pTrustzone->pData, pTrustzone->u4length);

	if(NULL != pTrustzone->pData)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pData, TZ_CACHE_ALIGN(MTDRM_HDCP2_CERT_RX));
	

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_DecryptRSAESOAEP(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_DECRYPT_RSAESOAEP* pTrustzone = (MTDRM_TZIF_HDCP2_DECRYPT_RSAESOAEP*)u4Addr;

	if(MTDRM_HDCP2_KEYLEN_EKPUB_KM != pTrustzone->u4length)
	{
		dprintf("\n\n%s: ERROR*** \n", __FUNCTION__);	
		return FALSE;
	}


	if(NULL != pTrustzone->pEkpub_km)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pEkpub_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKPUB_KM));
	
    pTrustzone->rRet = DRV_HDCP2_DecryptRSAESOAEP(pTrustzone->pEkpub_km);

	if(NULL != pTrustzone->pEkpub_km)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pEkpub_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKPUB_KM));
	
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_KdKeyDerivation(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_KD_KEY_DERIVATION* pTrustzone = (MTDRM_TZIF_HDCP2_KD_KEY_DERIVATION*)u4Addr;

	if(MTDRM_HDCP2_KEYLEN_RTX != pTrustzone->u4length)
	{
		dprintf("\n\n%s: ERROR*** \n", __FUNCTION__);	
		return FALSE;
	}

	if(NULL != pTrustzone->pRtx)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));

	pTrustzone->rRet = DRV_HDCP2_KdKeyDerivation(pTrustzone->pRtx,pTrustzone->u4length);

	if(NULL != pTrustzone->pRtx)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));
	
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_ComputeHprime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_COMPUTE_H_PRIME* pTrustzone = (MTDRM_TZIF_HDCP2_COMPUTE_H_PRIME*)u4Addr;


	if(NULL != pTrustzone->pH_prime)	
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pH_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_HPRIME+1+MTDRM_HDCP2_KEYLEN_RTX));
    // or
    //TZ_SWD_BufferStart((UINT32)pTrustzone->pH_prime, CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_HPRIME));
    //TZ_SWD_BufferStart((UINT32)pTrustzone->pRtx, CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));
    pTrustzone->rRet = DRV_HDCP2_ComputeHprime(pTrustzone->pH_prime, pTrustzone->Repeater, pTrustzone->pRtx);

	if(NULL != pTrustzone->pH_prime)		
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pH_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_HPRIME+1+MTDRM_HDCP2_KEYLEN_RTX));

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_ComputeLprime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME* pTrustzone = (MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME*)u4Addr;



	if(NULL != pTrustzone->pL_prime)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pL_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LPRIME+MTDRM_HDCP2_KEYLEN_RRN+MTDRM_HDCP2_KEYLEN_RRX));
	
    pTrustzone->rRet = DRV_HDCP2_ComputeLprime(pTrustzone->pL_prime, pTrustzone->pRn, pTrustzone->pRrx);

	if(NULL != pTrustzone->pL_prime)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pL_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LPRIME+MTDRM_HDCP2_KEYLEN_RRN+MTDRM_HDCP2_KEYLEN_RRX));

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_ComputeKh(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_COMPUTE_KH* pTrustzone = (MTDRM_TZIF_HDCP2_COMPUTE_KH*)u4Addr;
    pTrustzone->rRet = DRV_HDCP2_ComputeKh ();
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_EncryptKmUsingKh(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_ENCRYPT_KM_USING_KH* pTrustzone = (MTDRM_TZIF_HDCP2_ENCRYPT_KM_USING_KH*)u4Addr;

	if(NULL != pTrustzone->pEkh_km)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pEkh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));

	if(NULL != pTrustzone->pM)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pM, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KM));
	
    pTrustzone->rRet = DRV_HDCP2_EncryptKmUsingKh (pTrustzone->pEkh_km,pTrustzone->pM);

	if(NULL != pTrustzone->pEkh_km)	
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pEkh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));

	if(NULL != pTrustzone->pM)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pM, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KM));


    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_DecryptKmUsingKh(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_DECRYPT_KM_USING_KH* pTrustzone = (MTDRM_TZIF_HDCP2_DECRYPT_KM_USING_KH*)u4Addr;


	if(NULL != pTrustzone->pM)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pM, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KM));


	if(NULL != pTrustzone->ekh_km)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));

	
    pTrustzone->rRet = DRV_HDCP2_DecryptKmUsingKh(pTrustzone->pM, pTrustzone->ekh_km);

	if(NULL != pTrustzone->pM)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pM, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KM));


	if(NULL != pTrustzone->ekh_km)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->ekh_km, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_EKH_KM));

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_DecryptEKs(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_DECRYPT_EKS* pTrustzone = (MTDRM_TZIF_HDCP2_DECRYPT_EKS*)u4Addr;

	if(NULL != pTrustzone->pEks)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pEks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));

	if(NULL != pTrustzone->pRn)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));

	if(NULL != pTrustzone->pRrx)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));

	if(NULL != pTrustzone->pRtx)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));

	
    pTrustzone->rRet = DRV_HDCP2_DecryptEKs(pTrustzone->pEks, pTrustzone->pRtx, pTrustzone->pRrx, pTrustzone->pRn);

	if(NULL != pTrustzone->pEks)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pEks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));

	if(NULL != pTrustzone->pRn)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));

	if(NULL != pTrustzone->pRrx)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));

	if(NULL != pTrustzone->pRtx)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));

	
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_KsXorLC128(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_KS_XOR_LC128* pTrustzone = (MTDRM_TZIF_HDCP2_KS_XOR_LC128*)u4Addr;
    pTrustzone->rRet = DRV_HDCP2_KsXorLC128 ();
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_SetRiv_Pid(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_SetRiv_Pid* pTrustzone = (MTDRM_TZIF_HDCP2_SetRiv_Pid*)u4Addr;

	if(NULL == pTrustzone->pRiv)
	{

		dprintf("%s invalid parameter\n", __FUNCTION__);
		return FALSE;

	}


	if(NULL != pTrustzone->pRiv)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pRiv, TZ_CACHE_ALIGN(PRIV_KEY_SIZE));

    pTrustzone->rRet = DRV_HDCP2_SetRiv_Pid (pTrustzone->pRiv, pTrustzone->pid);

	if(NULL != pTrustzone->pRiv)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pRiv, TZ_CACHE_ALIGN(PRIV_KEY_SIZE));
	
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_PresetKsLC128(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_PresetKsLC128* pTrustzone = (MTDRM_TZIF_HDCP2_PresetKsLC128*)u4Addr;

	if(NULL != pTrustzone->pKs)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pKs, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));

	if(NULL != pTrustzone->pLC128)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pLC128, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));
	
    pTrustzone->rRet = DRV_HDCP2_PresetKsLC128 (pTrustzone->pKs, pTrustzone->pLC128);

	if(NULL != pTrustzone->pKs)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pKs, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));

	if(NULL != pTrustzone->pLC128)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pLC128, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));
	
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_Generate_Km(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_GENERATE_KM* pTrustzone = (MTDRM_TZIF_HDCP2_GENERATE_KM*)u4Addr;

	if(NULL != pTrustzone->pMaskedDB)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pMaskedDB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_MASKED_DB));

	if(NULL != pTrustzone->pDB)	
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pDB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DB));

	if(NULL != pTrustzone->pDBmask)			
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pDBmask, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DBMASK));
		
    pTrustzone->rRet = DRV_HDCP2_Generate_Km(pTrustzone->pMaskedDB, pTrustzone->pDB, pTrustzone->pDBmask);

	if(NULL != pTrustzone->pMaskedDB)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pMaskedDB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_MASKED_DB));

	if(NULL != pTrustzone->pDB)		
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pDB, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DB));

	if(NULL != pTrustzone->pDBmask)			
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pDBmask, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_DBMASK));
		
    return TRUE;
}
BOOL SvcEx_MTDRM_HDCP2_Generate_Ks(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_GENERATE_KS* pTrustzone = (MTDRM_TZIF_HDCP2_GENERATE_KS*)u4Addr;

	if(NULL != pTrustzone->pEks)		
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pEks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));

	if(NULL != pTrustzone->pRrx)	
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));

	if(NULL != pTrustzone->pRtx)	
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));

	if(NULL != pTrustzone->pRn)	
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));

		
    pTrustzone->rRet = DRV_HDCP2_Generate_Ks(pTrustzone->pEks, pTrustzone->pRrx, pTrustzone->pRtx, pTrustzone->pRn);

	if(NULL != pTrustzone->pEks)
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pEks, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_KS));

	if(NULL != pTrustzone->pRrx)	
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));

	if(NULL != pTrustzone->pRtx)	
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));

	if(NULL != pTrustzone->pRn)		
	    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRn, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRN));    

		
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_SetEncKm(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_ENC_KM* pTrustzone = (MTDRM_TZIF_HDCP2_ENC_KM*)u4Addr;

	if(NULL != pTrustzone->pEncKm)	
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));

	
    pTrustzone->rRet = DRV_HDCP2_SetEncKm (pTrustzone->pEncKm);

	if(NULL != pTrustzone->pEncKm)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));
	
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_GetEncKm(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_ENC_KM* pTrustzone = (MTDRM_TZIF_HDCP2_ENC_KM*)u4Addr;

	if(NULL != pTrustzone->pEncKm)
		TZ_SWD_BufferStart((UINT32)pTrustzone->pEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));
	
    pTrustzone->rRet = DRV_HDCP2_GetEncKm (pTrustzone->pEncKm);

	if(NULL != pTrustzone->pEncKm)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pEncKm, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_ENCKM));
	
    return TRUE;
}







/*for HDCP2.2 */

BOOL SvcEx_MTDRM_HDCP2_2_KdKeyDerivation(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_2_KD_KEY_DERIVATION* pTrustzone = (MTDRM_TZIF_HDCP2_2_KD_KEY_DERIVATION*)u4Addr;



	if(NULL != pTrustzone->pRtx)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));

	if(NULL != pTrustzone->pRrx)
	    TZ_SWD_BufferStart((UINT32)pTrustzone->pRrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));	

	//pTrustzone->rRet = DRV_HDCP2_KdKeyDerivation(pTrustzone->pRtx,pTrustzone->u4length);
	pTrustzone->rRet = DRV_HDCP2_2_KdKeyDerivation(pTrustzone->pRtx, pTrustzone->pRrx, pTrustzone->u4length);


	if(NULL != pTrustzone->pRtx)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pRtx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));

	if(NULL != pTrustzone->pRrx)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pRrx, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RRX));	



	
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_2_ComputeHprime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_2_COMPUTE_H_PRIME* pTrustzone = (MTDRM_TZIF_HDCP2_2_COMPUTE_H_PRIME*)u4Addr;

	if(NULL != pTrustzone->pH_prime)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pH_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_HPRIME+1+MTDRM_HDCP2_KEYLEN_RTX));
    // or
    //TZ_SWD_BufferStart((UINT32)pTrustzone->pH_prime, CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_HPRIME));
    //TZ_SWD_BufferStart((UINT32)pTrustzone->pRtx, CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_RTX));
    pTrustzone->rRet = DRV_HDCP2_2_ComputeHprime(pTrustzone->pH_prime, 
												 pTrustzone->Repeater, 
												 pTrustzone->pRtx,
												 pTrustzone->rx_version,
												 pTrustzone->rx_cap_mask,
												 pTrustzone->tx_version,
								                 pTrustzone->tx_cap_mask);	
	
	if(NULL != pTrustzone->pH_prime)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pH_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_HPRIME+1+MTDRM_HDCP2_KEYLEN_RTX));

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_2_ComputeLprime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME* pTrustzone = (MTDRM_TZIF_HDCP2_COMPUTE_L_PRIME*)u4Addr;

	if(NULL != pTrustzone->pL_prime)
		TZ_SWD_BufferStart((UINT32)pTrustzone->pL_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LPRIME+MTDRM_HDCP2_KEYLEN_RRN+MTDRM_HDCP2_KEYLEN_RRX));

		
    pTrustzone->rRet = DRV_HDCP2_2_ComputeLprime(pTrustzone->pL_prime, pTrustzone->pRn, pTrustzone->pRrx);

	
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pL_prime, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LPRIME+MTDRM_HDCP2_KEYLEN_RRN+MTDRM_HDCP2_KEYLEN_RRX));

    return TRUE;
}



/* for HDCP2.X ALIGN */
BOOL SvcEx_MTDRM_HDCP2_SetEncKeySet(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET* pTrustzone = (MTDRM_TZIF_HDCP2_SET_ENC_KEY_SET*)u4Addr;

	if(pTrustzone->u4length != sizeof(MTDRM_HDCP2_KEYSET_ENC_T))
	{
		dprintf("\n\n%s: ERROR*** keyset len(%d)\n", __FUNCTION__, pTrustzone->u4length);
		return FALSE;
	}

	if(NULL == pTrustzone->pKeySet)
	{
		dprintf("\n\n%s: ERROR*** keyset %p\n", __FUNCTION__, pTrustzone->pKeySet);
		return FALSE;
	}

	//if(NULL != pTrustzone->pKeySet)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pKeySet, TZ_CACHE_ALIGN(pTrustzone->u4length));
	
    pTrustzone->rRet = DRV_HDCP2_SetEncKeySet(pTrustzone->pKeySet,pTrustzone->u4length);
	
	//if(NULL != pTrustzone->pKeySet)
    	TZ_SWD_BufferEnd((UINT32)pTrustzone->pKeySet, TZ_CACHE_ALIGN(pTrustzone->u4length));

    return TRUE;
}


/* for HDCP2.X TX */

BOOL SvcEx_MTDRM_HDCP2_RetrvEkh_km(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_RETRV_EKH_KM* pTrustzone = (MTDRM_TZIF_HDCP2_RETRV_EKH_KM*)u4Addr;

	TZ_SWD_BufferStart((UINT32)pTrustzone->pEkh_km, TZ_CACHE_ALIGN(PEKH_KM_SIZE));
	TZ_SWD_BufferStart((UINT32)pTrustzone->pm, TZ_CACHE_ALIGN(M_SIZE));
	TZ_SWD_BufferStart((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(KEYLEN_RXID));


	
    pTrustzone->rRet = DRV_HDCP2_RetrvEkh_km(pTrustzone->pEkh_km, pTrustzone->pm,  pTrustzone->pRX_ID);

	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pEkh_km, TZ_CACHE_ALIGN(PEKH_KM_SIZE));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pm, TZ_CACHE_ALIGN(M_SIZE));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(KEYLEN_RXID));	

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_SavePairingInfo(UINT32 u4Addr, UINT32 u4Size)
{

    MTDRM_TZIF_HDCP2_SaveRxPairingInfo* pTrustzone = (MTDRM_TZIF_HDCP2_SaveRxPairingInfo*)u4Addr;
    TZ_SWD_BufferStart((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(PEKH_KM_SIZE + M_SIZE + KEYLEN_RXID));
    pTrustzone->rRet = DRV_HDCP2_SavePairingInfo(pTrustzone->pRX_ID, pTrustzone->pEkh_km, pTrustzone->pm);
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(PEKH_KM_SIZE + M_SIZE + KEYLEN_RXID));

    return TRUE;
}



BOOL SvcEx_MTDRM_HDCP2_CheckRxID(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_CheckRxID* pTrustzone = (MTDRM_TZIF_HDCP2_CheckRxID*)u4Addr;
    TZ_SWD_BufferStart((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(KEYLEN_RXID));
    pTrustzone->rRet = DRV_HDCP2_CheckRxID(pTrustzone->pRX_ID);
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(KEYLEN_RXID));


    return TRUE;
}



BOOL SvcEx_MTDRM_HDCP2_ResetPairingInfo(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_CheckRxID* pTrustzone = (MTDRM_TZIF_HDCP2_CheckRxID*)u4Addr;
    TZ_SWD_BufferStart((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(KEYLEN_RXID));
    pTrustzone->rRet = DRV_HDCP2_ResetPairingInfo(pTrustzone->pRX_ID);
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pRX_ID, TZ_CACHE_ALIGN(KEYLEN_RXID));


    return TRUE;
}





BOOL SvcEx_MTDRM_HDCP2_ComputeMprime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_COMPUTE_M_PRIME* pTrustzone = (MTDRM_TZIF_HDCP2_COMPUTE_M_PRIME*)u4Addr;
	
    TZ_SWD_BufferStart((UINT32)pTrustzone->pM_prime, 
						TZ_CACHE_ALIGN(PM_PRIME_SIZE + seq_num_M_LEN + pTrustzone->STREAMID_TYPE_LEN));

	
	pTrustzone->rRet = DRV_HDCP2_ComputeMprime(pTrustzone->pM_prime,
												pTrustzone->STREAMID_TYPE,
												pTrustzone->seq_num_M, 
												pTrustzone->STREAMID_TYPE_LEN);

	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pM_prime, 
    				  TZ_CACHE_ALIGN(PM_PRIME_SIZE + seq_num_M_LEN + pTrustzone->STREAMID_TYPE_LEN));
	

    return TRUE;
}



BOOL SvcEx_MTDRM_HDCP2_ComputeVprime(UINT32 u4Addr, UINT32 u4Size)
{
	MTDRM_TZIF_HDCP2_0_CALVPRIME_T* pTrustzone = (MTDRM_TZIF_HDCP2_0_CALVPRIME_T*)u4Addr;
	
	TZ_SWD_BufferStart((UINT32)pTrustzone->pV_prime, 
						TZ_CACHE_ALIGN(PV_PRIME_SIZE + KEYLEN_RXID*pTrustzone->DEVICE_COUNT));

	
	pTrustzone->rRet = DRV_HDCP2_0_ComputeVprime(pTrustzone->pV_prime,
												pTrustzone->id_list,
												pTrustzone->DEPTH, 
												pTrustzone->DEVICE_COUNT,
												pTrustzone->MAX_DEVS_EXCEEDED,
												pTrustzone->MAX_CASCADE_EXCEEDED);

	
	TZ_SWD_BufferEnd((UINT32)pTrustzone->pV_prime, 
						TZ_CACHE_ALIGN(PV_PRIME_SIZE + KEYLEN_RXID*pTrustzone->DEVICE_COUNT));
	

	return TRUE;
}





BOOL SvcEx_MTDRM_HDCP2_2_ComputeVprime(UINT32 u4Addr, UINT32 u4Size)
{
	MTDRM_TZIF_HDCP2_2_CALVPRIME_T* pTrustzone = (MTDRM_TZIF_HDCP2_2_CALVPRIME_T*)u4Addr;
	
	TZ_SWD_BufferStart((UINT32)pTrustzone->pV_prime, 
						TZ_CACHE_ALIGN(PV_PRIME_SIZE + KEYLEN_RXID*pTrustzone->DEVICE_COUNT));

	
	pTrustzone->rRet = DRV_HDCP2_2_ComputeVprime(pTrustzone->pV_prime,
												pTrustzone->id_list,
												pTrustzone->DEPTH, 
												pTrustzone->DEVICE_COUNT,
												pTrustzone->MAX_DEVS_EXCEEDED,
												pTrustzone->MAX_CASCADE_EXCEEDED,
												pTrustzone->HDCP2_0_REPEATER_DOWNSTREAM,
												pTrustzone->HDCP1_DEVICE_DOWNSTREAM,
												pTrustzone->seq_num_V);

	
	TZ_SWD_BufferEnd((UINT32)pTrustzone->pV_prime, 
						TZ_CACHE_ALIGN(PV_PRIME_SIZE + KEYLEN_RXID*pTrustzone->DEVICE_COUNT));
	

	return TRUE;
}

	


BOOL SvcEx_MTDRM_HDCP2_VerifySignature(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_VERIFY_SIGNATURE* pTrustzone = (MTDRM_TZIF_HDCP2_VERIFY_SIGNATURE*)u4Addr;
    TZ_SWD_BufferStart((UINT32)pTrustzone->pKpub, TZ_CACHE_ALIGN(KEYLEN_ROOTSIGN+1));
	TZ_SWD_BufferStart((UINT32)pTrustzone->pCert, TZ_CACHE_ALIGN(KEYLEN_CERTRX));	

	pTrustzone->rRet = DRV_HDCP2_VerifySignature(pTrustzone->pKpub, pTrustzone->pCert);

	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pKpub, TZ_CACHE_ALIGN(KEYLEN_ROOTSIGN+1));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pCert, TZ_CACHE_ALIGN(KEYLEN_CERTRX));	


	/* reserved */
    return TRUE;
}






BOOL SvcEx_MTDRM_HDCP2_Generate_Km_for_tx(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_Generate_EKm* pTrustzone = (MTDRM_TZIF_HDCP2_Generate_EKm*)u4Addr;
    TZ_SWD_BufferStart((UINT32)pTrustzone->pEkpub_km, TZ_CACHE_ALIGN(PEKPUB_KM_SIZE));
    pTrustzone->rRet = DRV_HDCP2_Generate_km_for_tx(pTrustzone->pEkpub_km);
    TZ_SWD_BufferEnd((UINT32)pTrustzone->pEkpub_km, TZ_CACHE_ALIGN(PEKPUB_KM_SIZE));
    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_UseTestKey_InKernel(UINT32 u4Addr, UINT32 u4Size)
{
		MTDRM_TZIF_HDCP2_ENC_KM* pTrustzone = (MTDRM_TZIF_HDCP2_ENC_KM*)u4Addr;
		pTrustzone->rRet = DRV_HDCP2_SetTestKeyFlag(pTrustzone->pEncKm);

		return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_HMAC_SHA256(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_HMAC_SHA256* pTrustzone = (MTDRM_TZIF_HDCP2_HMAC_SHA256*)u4Addr;
    TZ_SWD_BufferStart((UINT32)pTrustzone->output, TZ_CACHE_ALIGN(HMAC_SHA256_LEN + pTrustzone->au1rKeyLen + pTrustzone->intputLen));
    pTrustzone->rRet = DRV_HDCP2_HMAC_SHA256(pTrustzone->output, 
											 pTrustzone->au1rKey, 
											 pTrustzone->au1rKeyLen,
											 pTrustzone->input,
											 pTrustzone->intputLen);
    TZ_SWD_BufferEnd((UINT32)pTrustzone->output, TZ_CACHE_ALIGN(HMAC_SHA256_LEN + pTrustzone->au1rKeyLen + pTrustzone->intputLen));

    return TRUE;
}



BOOL SvcEx_MTDRM_HDCP2_SW_DecryptData(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_DataDecrypt_T* pTrustzone = (MTDRM_TZIF_HDCP2_DataDecrypt_T*)u4Addr;
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_buf, TZ_CACHE_ALIGN(pTrustzone->data_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->out_buf, TZ_CACHE_ALIGN(pTrustzone->data_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->iv, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));
	
    pTrustzone->rRet = DRV_HDCP2_DataDecrypt(pTrustzone->out_buf, pTrustzone->in_buf, pTrustzone->data_len, pTrustzone->iv);
	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_buf, TZ_CACHE_ALIGN(pTrustzone->data_len));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_buf, TZ_CACHE_ALIGN(pTrustzone->data_len));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->iv, TZ_CACHE_ALIGN(MTDRM_HDCP2_KEYLEN_LC128));

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_enable_tz_key(UINT32 u4Addr, UINT32 u4Size)
{
		//MTDRM_TZIF_HDCP2_ENC_KM* pTrustzone = (MTDRM_TZIF_HDCP2_ENC_KM*)u4Addr;
		//pTrustzone->rRet = DRV_HDCP2_EnableTZKey(pTrustzone->pEncKm);

		return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_GetPDKey(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY* pTrustzone = (MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY*)u4Addr;

	if(NULL != pTrustzone->pData)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pData, TZ_CACHE_ALIGN(PROTECTED_KEY_MAX));
	
    pTrustzone->rRet = DRV_HDCP2_GetPDKey(pTrustzone->pData, &(pTrustzone->u4length));

	if(NULL != pTrustzone->pData)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pData, TZ_CACHE_ALIGN(PROTECTED_KEY_MAX));
	

    return TRUE;
}

BOOL SvcEx_MTDRM_HDCP2_SetPDKey(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY* pTrustzone = (MTDRM_TZIF_HDCP2_PROTECTED_DECRYPT_KEY*)u4Addr;

	if(NULL != pTrustzone->pData)
    	TZ_SWD_BufferStart((UINT32)pTrustzone->pData, TZ_CACHE_ALIGN(pTrustzone->u4length));
	
    pTrustzone->rRet = DRV_HDCP2_SetPDKey(pTrustzone->pData, pTrustzone->u4length);

	if(NULL != pTrustzone->pData)
		TZ_SWD_BufferEnd((UINT32)pTrustzone->pData, TZ_CACHE_ALIGN(pTrustzone->u4length));
	

    return TRUE;
}

#endif


#ifdef CC_ENABLE_IPTVES


/* SONY IPTVES */

/* SAC */
BOOL SvcEx_MTDRM_IPTVES_SAC_CONSTRUCT(UINT32 u4Addr, UINT32 u4Size)
{

    MTDRM_TZIF_IPTVES_SAC_CONSTRUCT* pTrustzone = (MTDRM_TZIF_IPTVES_SAC_CONSTRUCT*)u4Addr;
	
    TZ_SWD_BufferStart((UINT32)pTrustzone->req_lst, TZ_CACHE_ALIGN(pTrustzone->req_list_size));
	
	
	pTrustzone->rRet = sac_Construct(pTrustzone->id,
									 pTrustzone->req_num,
									 pTrustzone->req_lst,
									 &(pTrustzone->out_handle));
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->req_lst, TZ_CACHE_ALIGN(pTrustzone->req_list_size));


    return TRUE;
}



BOOL SvcEx_MTDRM_IPTVES_SAC_DESTRUCT(UINT32 u4Addr, UINT32 u4Size)
{
	
    MTDRM_TZIF_IPTVES_SAC_DESTRUCT* pTrustzone = (MTDRM_TZIF_IPTVES_SAC_DESTRUCT*)u4Addr;
	
  
	
	
	pTrustzone->rRet = sac_Destruct(pTrustzone->in_handle,
									 &(pTrustzone->out_success_num),
									 &(pTrustzone->out_trans_flag_status));
								
  


    return TRUE;

}



BOOL SvcEx_MTDRM_IPTVES_SAC_MKMSG(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_SAC_MakeMSG* pTrustzone = (MTDRM_TZIF_IPTVES_SAC_MakeMSG*)u4Addr;
	int io_msg_len = 0;
	
	io_msg_len = pTrustzone->io_msg_len;
	
    TZ_SWD_BufferStart((UINT32)pTrustzone->out_msg, TZ_CACHE_ALIGN(io_msg_len));

	
	pTrustzone->rRet = sac_MakeMessage(pTrustzone->in_handle,
									 pTrustzone->out_msg,
									 &(pTrustzone->io_msg_len),
									 &(pTrustzone->out_last_req_flag));
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_msg, TZ_CACHE_ALIGN(io_msg_len));


    return TRUE;

}




BOOL SvcEx_MTDRM_IPTVES_SAC_PROCESSMSG(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_SAC_ProcessMSG* pTrustzone = (MTDRM_TZIF_IPTVES_SAC_ProcessMSG*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_msg, TZ_CACHE_ALIGN(pTrustzone->in_msg_len));

	
	pTrustzone->rRet = sac_ProcessMessage(pTrustzone->in_handle,
									 pTrustzone->in_msg,
									 pTrustzone->in_msg_len,
									 &(pTrustzone->out_end_flag));
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_msg, TZ_CACHE_ALIGN(pTrustzone->in_msg_len));


    return TRUE;

}





/* sdi */
BOOL SvcEx_MTDRM_IPTVES_SDI_INIT(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_sdi_Init_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_Init_T*)u4Addr;
	
    pTrustzone->rRet = sdi_Init(pTrustzone->current_time_secs, pTrustzone->gmtoff);

    return TRUE;

}
          			     
BOOL SvcEx_MTDRM_IPTVES_SDI_FIN(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_sdi_Fin_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_Fin_T*)u4Addr;


	
    pTrustzone->rRet = sdi_Fin();
	
	Printf("%s pTrustzone->rRet[%d]\n", __FUNCTION__, pTrustzone->rRet);
	
    return TRUE;

}
          			     
BOOL SvcEx_MTDRM_IPTVES_SDI_CleanupNVS(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_sdi_CleanupNVS_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_CleanupNVS_T*)u4Addr;
    pTrustzone->rRet = sdi_CleanupNVS();
	
    return TRUE;
}
       		   
BOOL SvcEx_MTDRM_IPTVES_SDI_GetDeviceID(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_sdi_GetDeviceID_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_GetDeviceID_T*)u4Addr;


    TZ_SWD_BufferStart((UINT32)pTrustzone->out_devid, TZ_CACHE_ALIGN(pTrustzone->out_devid_len));




	
	pTrustzone->rRet = sdi_GetDeviceID(pTrustzone->in_thid,
									 pTrustzone->out_devid);

	

    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_devid, TZ_CACHE_ALIGN(pTrustzone->out_devid_len));



    return TRUE;

}
    			   
BOOL SvcEx_MTDRM_IPTVES_SDI_VerifyServiceMetaData(UINT32 u4Addr, UINT32 u4Size)
{
	
    MTDRM_TZIF_IPTVES_sdi_VerifyServiceMetaData_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_VerifyServiceMetaData_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_msg, TZ_CACHE_ALIGN(pTrustzone->in_msg_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_pkipath, TZ_CACHE_ALIGN(pTrustzone->in_pkipath_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_sig, TZ_CACHE_ALIGN(pTrustzone->in_sig_len));	

	
	pTrustzone->rRet = sdi_VerifyServiceMetaData(pTrustzone->in_thid,
									 			 pTrustzone->in_msg,
									 			 pTrustzone->in_msg_len,
									 			 pTrustzone->in_pkipath,
									 			 pTrustzone->in_pkipath_len,
									 			 pTrustzone->in_sig);
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_msg, TZ_CACHE_ALIGN(pTrustzone->in_msg_len));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_pkipath, TZ_CACHE_ALIGN(pTrustzone->in_pkipath_len));
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_sig, TZ_CACHE_ALIGN(pTrustzone->in_sig_len));


    return TRUE;

}
   
BOOL SvcEx_MTDRM_IPTVES_SDI_JudgeCRLUpdate(UINT32 u4Addr, UINT32 u4Size)
{
	
    MTDRM_TZIF_IPTVES_sdi_JudgeCRLUpdate_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_JudgeCRLUpdate_T*)u4Addr;
	
	pTrustzone->rRet = sdi_JudgeCRLUpdate(pTrustzone->in_thid,
									 	&(pTrustzone->out_is_requested));
	return TRUE;

}
      		 
BOOL SvcEx_MTDRM_IPTVES_SDI_GetCRLDstPoint(UINT32 u4Addr, UINT32 u4Size)
{
	
    MTDRM_TZIF_IPTVES_sdi_GetCRLDstPoint_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_GetCRLDstPoint_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->out_crl_dp, TZ_CACHE_ALIGN(pTrustzone->out_crl_dp_len));

	
	pTrustzone->rRet = sdi_GetCRLDstPoint(pTrustzone->in_thid,
									 pTrustzone->out_crl_dp);
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_crl_dp, TZ_CACHE_ALIGN(pTrustzone->out_crl_dp_len));

	
    return TRUE;
}
          
BOOL SvcEx_MTDRM_IPTVES_SDI_SetCRL(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_sdi_SetCRL_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_SetCRL_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_crl, TZ_CACHE_ALIGN(pTrustzone->in_crl_len));

	
	pTrustzone->rRet = sdi_SetCRL(pTrustzone->in_thid,
								  pTrustzone->in_crl,
								  pTrustzone->in_crl_len
								  &(pTrustzone->out_is_updated)
								 );
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_crl, TZ_CACHE_ALIGN(pTrustzone->in_crl_len));
	
    return TRUE;
}
                  
BOOL SvcEx_MTDRM_IPTVES_SDI_CheckTrustedTime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_sdi_CheckTrustedTime_T* pTrustzone = (MTDRM_TZIF_IPTVES_sdi_CheckTrustedTime_T*)u4Addr;
	
	pTrustzone->rRet = sdi_CheckTrustedTime(&(pTrustzone->out_is_valid));
								
	
    return TRUE;
}
        
/* ecm some err*/                                            
                                                 
/* ecm */                                        
BOOL SvcEx_MTDRM_IPTVES_ECM_ExtractScrambleKey(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_ecm_ExtractScrambleKey_T* pTrustzone = (MTDRM_TZIF_IPTVES_ecm_ExtractScrambleKey_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_ecm, TZ_CACHE_ALIGN(pTrustzone->in_ecm_len));

	
	pTrustzone->rRet = ecm_ExtractScrambleKey(pTrustzone->in_thid,
											  pTrustzone->in_rend_id,
											  pTrustzone->in_ecm);
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_ecm, TZ_CACHE_ALIGN(pTrustzone->in_ecm_len));	
	
    return TRUE;
}
      
BOOL SvcEx_MTDRM_IPTVES_ECM_GetScrambleKey(UINT32 u4Addr, UINT32 u4Size)
{

#ifndef IPTVES_PUBLISH_BUILD


    MTDRM_TZIF_IPTVES_ecm_GetScrambleKey_T* pTrustzone = (MTDRM_TZIF_IPTVES_ecm_GetScrambleKey_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_ecm, TZ_CACHE_ALIGN(pTrustzone->in_ecm_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->out_skey_even, TZ_CACHE_ALIGN(pTrustzone->out_skey_even_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->out_skey_odd, TZ_CACHE_ALIGN(pTrustzone->out_skey_odd_len));

	
	pTrustzone->rRet = ecm_GetScrambleKey(pTrustzone->in_thid,
										  pTrustzone->in_ecm,
										  pTrustzone->out_skey_even,
										  pTrustzone->out_skey_odd);
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_ecm, TZ_CACHE_ALIGN(pTrustzone->in_ecm_len	));	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_skey_even, TZ_CACHE_ALIGN(pTrustzone->out_skey_even_len));	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_skey_odd, TZ_CACHE_ALIGN(pTrustzone->out_skey_odd_len));	
	
#endif
	
    return TRUE;

}
    		   
                                                      
                                                 
/* drm */                                        
BOOL SvcEx_MTDRM_IPTVES_DRM_ExtractContentKey(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_drm_ExtractContentKey_T* pTrustzone = (MTDRM_TZIF_IPTVES_drm_ExtractContentKey_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len));

	
	pTrustzone->rRet = drm_ExtractContentKey(pTrustzone->in_thid,
											  pTrustzone->in_rend_id,
											  pTrustzone->in_key_ref);
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len	));	
	
    
    return TRUE;

}    


BOOL SvcEx_MTDRM_IPTVES_DRM_DelContentKey(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_drm_DelContentKey_T* pTrustzone = (MTDRM_TZIF_IPTVES_drm_DelContentKey_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len));

	
	pTrustzone->rRet = drm_DelContentKey(pTrustzone->in_thid,
										 pTrustzone->in_key_ref);
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len	));	
	
    
    return TRUE;

}
          	
BOOL SvcEx_MTDRM_IPTVES_DRM_DelUncommittedKey(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_drm_DelUncommittedKey_T* pTrustzone = (MTDRM_TZIF_IPTVES_drm_DelUncommittedKey_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len));

	
	pTrustzone->rRet = drm_DelUncommittedKey(pTrustzone->in_thid,
										 pTrustzone->in_key_ref);
								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len	));	
	
    
    return TRUE;

}
       
BOOL SvcEx_MTDRM_IPTVES_DRM_GetKeyInfo(UINT32 u4Addr, UINT32 u4Size)
{

    MTDRM_TZIF_IPTVES_drm_GetKeyInfo_T* pTrustzone = (MTDRM_TZIF_IPTVES_drm_GetKeyInfo_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->out_key_info, TZ_CACHE_ALIGN(pTrustzone->out_key_info_len));

	
	pTrustzone->rRet = drm_GetKeyInfo(pTrustzone->in_thid,
									  pTrustzone->in_key_ref,
									  pTrustzone->out_key_info);

								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len	));	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_key_info, TZ_CACHE_ALIGN(pTrustzone->out_key_info_len	));	
	
    	
    return TRUE;
}
   			     
BOOL SvcEx_MTDRM_IPTVES_DRM_GetKeyNum(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_drm_GetKeyNum_T* pTrustzone = (MTDRM_TZIF_IPTVES_drm_GetKeyNum_T*)u4Addr;

    //TZ_SWD_BufferStart((UINT32)pTrustzone->out_num, TZ_CACHE_ALIGN(pTrustzone->out_num_len));
	Printf("\n>>%s\n", __FUNCTION__);
	
	pTrustzone->rRet = drm_GetKeyNum(pTrustzone->in_thid,
									 pTrustzone->in_action_id,
									 &(pTrustzone->out_num));
	
    //TZ_SWD_BufferEnd((UINT32)pTrustzone->out_num, TZ_CACHE_ALIGN(pTrustzone->out_num_len));


	
    return TRUE;
}
     			     
BOOL SvcEx_MTDRM_IPTVES_DRM_GetKeyRefList(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_drm_GetKeyRefList_T* pTrustzone = (MTDRM_TZIF_IPTVES_drm_GetKeyRefList_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->io_num, TZ_CACHE_ALIGN(pTrustzone->io_num_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->out_key_ref_lst, TZ_CACHE_ALIGN(pTrustzone->out_key_ref_lst_len));


	
	pTrustzone->rRet = drm_GetKeyRefList(pTrustzone->in_thid,
								 		 pTrustzone->in_action_id,
										 pTrustzone->io_num,
									     pTrustzone->out_key_ref_lst);

								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->io_num, TZ_CACHE_ALIGN(pTrustzone->io_num_len));	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_key_ref_lst, TZ_CACHE_ALIGN(pTrustzone->out_key_ref_lst_len));	

		
    return TRUE;
}
      	   
BOOL SvcEx_MTDRM_IPTVES_DRM_GetContentKey(UINT32 u4Addr, UINT32 u4Size)
{
#ifndef IPTVES_PUBLISH_BUILD

		MTDRM_TZIF_IPTVES_drm_GetContentKey_T* pTrustzone = (MTDRM_TZIF_IPTVES_drm_GetContentKey_T*)u4Addr;
	
		
		TZ_SWD_BufferStart((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len));
		TZ_SWD_BufferStart((UINT32)pTrustzone->out_ckey, TZ_CACHE_ALIGN(pTrustzone->out_ckey_len));
		
		pTrustzone->rRet = drm_GetContentKey(pTrustzone->in_thid,
											 pTrustzone->in_key_ref,
											 pTrustzone->out_ckey);
									
		TZ_SWD_BufferEnd((UINT32)pTrustzone->in_key_ref, TZ_CACHE_ALIGN(pTrustzone->in_key_ref_len	)); 
		TZ_SWD_BufferEnd((UINT32)pTrustzone->out_ckey, TZ_CACHE_ALIGN(pTrustzone->out_ckey_len)); 		
		
		return TRUE;
#endif		

}
           



/* nvs mem */


/*============================================================================
 * Local Functions
 *==========================================================================*/


unsigned char svc_nvs_test[1457] = {
	
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9c, 0xe2, 0x27, 0xb8, 0x96, 0x7c, 0x52, 0x29, 0xdb, 0x93, 0xed, 0x2b, 0xee, 0x93, 0x4e, 0x53,
	0x7b, 0x14, 0x3f, 0xb9, 0x08, 0x2f, 0x10, 0x62, 0x85, 0x29, 0xd7, 0xb2, 0x34, 0xb9, 0x19, 0xf5,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7f, 0x00, 0x00, 0x00, 0x01,
	0x30, 0x82, 0x01, 0x7b, 0x30, 0x82, 0x01, 0x28, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
	0x04, 0x03, 0x02, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01,
	0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c,
	0x69, 0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61, 0x63, 0x74, 0x69,
	0x6f, 0x6e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x36, 0x30, 0x31, 0x30, 0x31, 0x30, 0x39, 0x30, 0x30,
	0x30, 0x30, 0x5a, 0x17, 0x0d, 0x33, 0x31, 0x30, 0x31, 0x30, 0x31, 0x30, 0x39, 0x30, 0x30, 0x30,
	0x30, 0x5a, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81,
	0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c, 0x69,
	0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f,
	0x6e, 0x30, 0x49, 0x30, 0x0b, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00,
	0x03, 0x3a, 0x00, 0x04, 0x9f, 0xf3, 0xc2, 0x4e, 0xea, 0xcf, 0xf4, 0x84, 0x65, 0xe8, 0x58, 0xf1,
	0x31, 0xea, 0x78, 0x9c, 0x4c, 0xb0, 0xf8, 0xb9, 0xff, 0x9b, 0x4b, 0x62, 0xe5, 0x07, 0x10, 0x4a,
	0xc9, 0x0f, 0xad, 0xf8, 0x47, 0x5e, 0x6c, 0x3a, 0xd3, 0x54, 0x5a, 0xb0, 0xba, 0xce, 0x0b, 0x10,
	0x78, 0xb2, 0x20, 0x3d, 0x31, 0x1b, 0x22, 0x44, 0x86, 0xc8, 0x64, 0x33, 0xa3, 0x42, 0x30, 0x40,
	0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x9d, 0xae, 0xe6, 0xfb, 0x9f,
	0x45, 0x5d, 0xe0, 0xa0, 0x42, 0x19, 0x24, 0x7d, 0x07, 0x85, 0x95, 0xd3, 0xc0, 0x6d, 0xe6, 0x30,
	0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06, 0x30,
	0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff,
	0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x41, 0x00, 0x30,
	0x3e, 0x02, 0x1d, 0x00, 0xb6, 0x2a, 0xd7, 0xd7, 0x7e, 0x9b, 0xe8, 0x68, 0xcb, 0x06, 0x05, 0x88,
	0x4f, 0x7a, 0x89, 0x0b, 0x60, 0x1f, 0x39, 0x08, 0x64, 0x5e, 0xd4, 0x42, 0xe2, 0xed, 0xe3, 0x56,
	0x02, 0x1d, 0x00, 0x8b, 0x5e, 0x61, 0xff, 0xb7, 0xbd, 0x66, 0xd2, 0xc7, 0x55, 0xa7, 0x9a, 0xbc,
	0xa4, 0x4a, 0x16, 0x9b, 0x15, 0x76, 0xaf, 0x7e, 0xc4, 0x1e, 0x13, 0x82, 0x0b, 0xc7, 0x89, 0x00,
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xb6, 0x00, 0x00, 0x00, 0x01, 0x30,
	0x82, 0x03, 0xb2, 0x30, 0x82, 0x02, 0x05, 0x30, 0x82, 0x01, 0xb3, 0xa0, 0x03, 0x02, 0x01, 0x02,
	0x02, 0x08, 0x29, 0xfd, 0x58, 0x8b, 0xea, 0xdd, 0xde, 0x64, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86,
	0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x2d, 0x31, 0x2b, 0x30, 0x29, 0x06, 0x0b, 0x2b, 0x06,
	0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x1a, 0x75, 0x72, 0x6e, 0x3a, 0x6d,
	0x61, 0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x70, 0x65, 0x65, 0x72, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x61,
	0x63, 0x74, 0x69, 0x6f, 0x6e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x37, 0x30, 0x31, 0x30, 0x39, 0x30,
	0x33, 0x34, 0x38, 0x34, 0x33, 0x5a, 0x17, 0x0d, 0x32, 0x37, 0x30, 0x31, 0x30, 0x39, 0x30, 0x33,
	0x34, 0x38, 0x34, 0x33, 0x5a, 0x30, 0x53, 0x31, 0x51, 0x30, 0x4f, 0x06, 0x0b, 0x2b, 0x06, 0x01,
	0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01, 0x01, 0x01, 0x0c, 0x40, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61,
	0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x63, 0x61, 0x3a, 0x69, 0x6e,
	0x74, 0x65, 0x72, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2d, 0x70, 0x64, 0x63, 0x2d, 0x63, 0x6c, 0x69,
	0x65, 0x6e, 0x74, 0x2d, 0x63, 0x61, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
	0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x30, 0x49, 0x30, 0x0b, 0x06, 0x07,
	0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00, 0x03, 0x3a, 0x00, 0x04, 0x00, 0x08, 0x14,
	0x07, 0x88, 0x1c, 0x25, 0x3f, 0xb0, 0x76, 0xd7, 0x40, 0xbf, 0x73, 0x3a, 0x6f, 0x96, 0x39, 0x6b,
	0xef, 0x78, 0x0e, 0x59, 0x1b, 0xaa, 0x7c, 0x2a, 0xc0, 0xa8, 0xa1, 0xe2, 0xde, 0x83, 0x96, 0x9e,
	0x76, 0xa3, 0x25, 0x79, 0x1e, 0x2c, 0xb4, 0x19, 0x0b, 0x1a, 0xf5, 0x55, 0x8d, 0x38, 0x7c, 0x55,
	0xc8, 0xe6, 0xa6, 0x6a, 0xaf, 0xa3, 0x81, 0xa6, 0x30, 0x81, 0xa3, 0x30, 0x1f, 0x06, 0x03, 0x55,
	0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x9d, 0xae, 0xe6, 0xfb, 0x9f, 0x45, 0x5d, 0xe0,
	0xa0, 0x42, 0x19, 0x24, 0x7d, 0x07, 0x85, 0x95, 0xd3, 0xc0, 0x6d, 0xe6, 0x30, 0x1d, 0x06, 0x03,
	0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xd6, 0xe1, 0x78, 0x3f, 0xf4, 0x22, 0xee, 0xb7, 0xc5,
	0x19, 0x57, 0x58, 0x75, 0x84, 0xfb, 0xdf, 0x4f, 0xba, 0xac, 0xf4, 0x30, 0x0e, 0x06, 0x03, 0x55,
	0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x02, 0x04, 0x30, 0x0f, 0x06, 0x03, 0x55,
	0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x40, 0x06, 0x03,
	0x55, 0x1d, 0x1f, 0x04, 0x39, 0x30, 0x37, 0x30, 0x35, 0xa0, 0x33, 0xa0, 0x31, 0x86, 0x2f, 0x68,
	0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x2d, 0x63, 0x72, 0x6c, 0x2e, 0x6d,
	0x61, 0x72, 0x6c, 0x69, 0x6e, 0x2d, 0x74, 0x6d, 0x6f, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x63, 0x72,
	0x6c, 0x2f, 0x69, 0x70, 0x74, 0x76, 0x63, 0x72, 0x6c, 0x73, 0x2e, 0x63, 0x72, 0x6c, 0x30, 0x0a,
	0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x40, 0x00, 0x30, 0x3d, 0x02,
	0x1d, 0x00, 0x87, 0x1b, 0x38, 0x4a, 0x62, 0x54, 0x41, 0x00, 0xcf, 0xde, 0xa6, 0x1d, 0x64, 0x8e,
	0xa9, 0xa0, 0x63, 0x57, 0x03, 0x5e, 0xae, 0x28, 0xd0, 0xb7, 0x23, 0x0a, 0xc7, 0xaf, 0x02, 0x1c,
	0x2f, 0x32, 0x14, 0xdb, 0xe8, 0xfa, 0x5e, 0x86, 0xa3, 0x81, 0x46, 0x0b, 0xfe, 0x53, 0x69, 0x9e,
	0x3f, 0x2b, 0x3e, 0x06, 0xc6, 0x7e, 0x40, 0x69, 0x57, 0x1c, 0x9e, 0x41, 0x30, 0x82, 0x01, 0xa5,
	0x30, 0x82, 0x01, 0x54, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x31, 0x0f, 0xd5, 0xc2, 0x6c,
	0x9c, 0x5e, 0x5b, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30,
	0x53, 0x31, 0x51, 0x30, 0x4f, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xb9, 0x2f, 0x01,
	0x01, 0x01, 0x0c, 0x40, 0x75, 0x72, 0x6e, 0x3a, 0x6d, 0x61, 0x72, 0x6c, 0x69, 0x6e, 0x3a, 0x63,
	0x6c, 0x69, 0x65, 0x6e, 0x74, 0x63, 0x61, 0x3a, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x74, 0x72, 0x75,
	0x73, 0x74, 0x2d, 0x70, 0x64, 0x63, 0x2d, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x2d, 0x63, 0x61,
	0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
	0x2e, 0x2e, 0x2e, 0x2e, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x37, 0x30, 0x32, 0x30, 0x31, 0x30, 0x32,
	0x30, 0x30, 0x34, 0x36, 0x5a, 0x17, 0x0d, 0x32, 0x37, 0x30, 0x32, 0x30, 0x31, 0x30, 0x32, 0x30,
	0x30, 0x34, 0x36, 0x5a, 0x30, 0x23, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04,
	0x01, 0x81, 0xb9, 0x2f, 0x03, 0x01, 0x01, 0x0c, 0x10, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x64, 0x38, 0x63, 0x30, 0x49, 0x30, 0x0b, 0x06, 0x07, 0x2a,
	0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x05, 0x00, 0x03, 0x3a, 0x00, 0x04, 0x89, 0x04, 0x62, 0x83,
	0xd6, 0x96, 0xdb, 0x95, 0x97, 0x78, 0x82, 0x90, 0x4d, 0xf6, 0x32, 0x1d, 0xe3, 0x7d, 0xe8, 0x49,
	0xdf, 0x53, 0x5b, 0x4d, 0xbb, 0xa8, 0xe4, 0x13, 0x3e, 0xc1, 0x5d, 0xcb, 0x81, 0x36, 0xd9, 0x7f,
	0x10, 0x4e, 0xb0, 0xdf, 0x4d, 0xd5, 0x4d, 0xe3, 0xf6, 0xb7, 0x25, 0xa3, 0x19, 0x66, 0x37, 0x12,
	0xfa, 0x92, 0xb2, 0x14, 0xa3, 0x52, 0x30, 0x50, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04,
	0x18, 0x30, 0x16, 0x80, 0x14, 0xd6, 0xe1, 0x78, 0x3f, 0xf4, 0x22, 0xee, 0xb7, 0xc5, 0x19, 0x57,
	0x58, 0x75, 0x84, 0xfb, 0xdf, 0x4f, 0xba, 0xac, 0xf4, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e,
	0x04, 0x16, 0x04, 0x14, 0x0a, 0xc7, 0xa5, 0x50, 0xcd, 0x30, 0x66, 0x25, 0x2c, 0xf2, 0xe3, 0x31,
	0x54, 0x2e, 0x59, 0x3b, 0x59, 0xfe, 0x03, 0x17, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01,
	0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
	0x3d, 0x04, 0x03, 0x02, 0x03, 0x3f, 0x00, 0x30, 0x3c, 0x02, 0x1c, 0x17, 0x70, 0xb2, 0x1b, 0x99,
	0x35, 0x28, 0xfb, 0x29, 0x70, 0xf2, 0x1b, 0xc4, 0x33, 0x05, 0xc1, 0xce, 0x3d, 0x66, 0x6d, 0x57,
	0x15, 0x9f, 0x01, 0x4d, 0x1d, 0xd1, 0x4d, 0x02, 0x1c, 0x45, 0xe2, 0xd2, 0x5c, 0x18, 0xff, 0xc4,
	0x79, 0x78, 0xf9, 0x48, 0xfc, 0x6e, 0xcc, 0x3c, 0xfc, 0xe1, 0x6d, 0x20, 0x50, 0xe3, 0x2b, 0x3a,
	0x84, 0x66, 0xfb, 0xe6, 0xe0, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1c, 0x00, 0x00, 0x00, 0x01, 0xbf, 0xa4, 0xd9, 0x46, 0xd8, 0xf7, 0x38, 0x40, 0x0e, 0x0b, 0x34,
	0x1d, 0x02, 0xce, 0x7f, 0x70, 0x3e, 0x56, 0xf9, 0xbf, 0x82, 0xae, 0x52, 0x4c, 0xe5, 0x7a, 0x5f,
	0x02


};




static 	void DumpHex(char *name, const unsigned char *data, int len)
	{
	
	
		int pos = 0;
		
		if (!data || !len)
		{
			return;
		}
		Printf("svc_Dumping %s:\n", name);
	
		//if (1 == iptv_tee_log_enable)
		{
			Printf("------------------------------");
			while(pos < len /*len*/) // only print message ID
			{
				if (pos%16 == 0)
					Printf("\n[%04x]|", pos);
				Printf("0x%02x, ", data[pos]); // secure data, cannot show in mtktool
				pos ++;
			}
			Printf("\n------------------------------\n");
		}
	
	
	
	}

BOOL SvcEx_MTDRM_IPTVES_NVS_MEM_Init(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_nvs_Init_T* pTrustzone = (MTDRM_TZIF_IPTVES_nvs_Init_T*)u4Addr;

	
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_ro, TZ_CACHE_ALIGN(pTrustzone->in_ro_len));
    TZ_SWD_BufferStart((UINT32)pTrustzone->in_rw, TZ_CACHE_ALIGN(pTrustzone->in_rw_len));


#if 0

	if(0!=memcmp(pTrustzone->in_ro, svc_nvs_test, pTrustzone->in_ro_len))
	{
		Printf("\t\n\n!!!!!! svc_nvs_ro init not ok \n");
		/* dump */
		
		DumpHex("svc_nvs_ro", pTrustzone->in_ro, pTrustzone->in_ro_len);

		//i4Ret = -77;

		
		//goto out;
	}
	else
	{
		Printf("\t!!!!!! svc_nvs_ro init  ok \n");
	}


	if(0!=memcmp(pTrustzone->in_rw, svc_nvs_test, pTrustzone->in_rw_len))
	{
		Printf("\t\n\n!!!!!! svc_nvs_rw init not ok \n");
		
		
		DumpHex("svc_nvs_rw", pTrustzone->in_rw, pTrustzone->in_rw_len);

		//i4Ret = -88;
				
		//goto out;
	}
	else
	{
		Printf("\t!!!!!! svc_nvs_rw init  ok \n");
		
	}



#endif





	
	pTrustzone->rRet = nvs_mem_init(pTrustzone->in_ro,
								 	pTrustzone->in_ro_len,
									pTrustzone->in_rw,
									pTrustzone->in_rw_len);

								
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_ro, TZ_CACHE_ALIGN(pTrustzone->in_ro_len));	
    TZ_SWD_BufferEnd((UINT32)pTrustzone->in_rw, TZ_CACHE_ALIGN(pTrustzone->in_rw_len));	

		
    return TRUE;
}




BOOL SvcEx_MTDRM_IPTVES_NVS_MEM_Fin(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_nvs_Fin_T* pTrustzone = (MTDRM_TZIF_IPTVES_nvs_Fin_T*)u4Addr;

	

    TZ_SWD_BufferStart((UINT32)pTrustzone->out_rw, TZ_CACHE_ALIGN(pTrustzone->out_rw_len));


	
	pTrustzone->rRet = nvs_mem_fin(pTrustzone->out_rw,
								   pTrustzone->out_rw_len);

								

    TZ_SWD_BufferEnd((UINT32)pTrustzone->out_rw, TZ_CACHE_ALIGN(pTrustzone->out_rw_len));	

		
    return TRUE;

}


BOOL SvcEx_MTDRM_IPTVES_NVS_MEM_GetRWLen(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_nvs_GetRWLen_T* pTrustzone = (MTDRM_TZIF_IPTVES_nvs_GetRWLen_T*)u4Addr;

	


	pTrustzone->rRet = nvs_mem_rw_len(&(pTrustzone->out_rw_len));

		
    return TRUE;
}
 


BOOL SvcEx_MTDRM_IPTVES_TIME_SetTrustedTime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_time_SetTrustedTime_T* pTrustzone = (MTDRM_TZIF_IPTVES_time_SetTrustedTime_T*)u4Addr;

	pTrustzone->rRet = time_SetTrustedTime(pTrustzone->in_tt);
		
    return TRUE;
}
 



BOOL SvcEx_MTDRM_IPTVES_TIME_GetTrustedTime(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_time_GetTrustedTime_T* pTrustzone = (MTDRM_TZIF_IPTVES_time_GetTrustedTime_T*)u4Addr;

	pTrustzone->rRet = time_GetTrustedTime(&(pTrustzone->out_tt));
		
    return TRUE;
}


BOOL SvcEx_MTDRM_IPTVES_TIME_Cleanup(UINT32 u4Addr, UINT32 u4Size)
{
    MTDRM_TZIF_IPTVES_time_Cleanup_T* pTrustzone = (MTDRM_TZIF_IPTVES_time_Cleanup_T*)u4Addr;

	pTrustzone->rRet = time_Cleanup(pTrustzone->in_type);
		
    return TRUE;
}
 

 
#endif  

