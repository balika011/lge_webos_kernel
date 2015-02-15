/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "oemcrypto_keybox_tz.h" 
#include "wvcrc32.h"
#include "aes.h"

 
#define BigLittleSwap32(A)  ((((uint32_t)(A)   & 0xff000000) >> 24)| \		
	(((uint32_t) (A)  & 0x00ff0000)>> 8 ) | \
	(((uint32_t) (A)  & 0x0000ff00) << 8) | \
	(((uint32_t) (A)  & 0x000000ff) << 24 ))

static UINT8 g_OEMKey[16] = { 0xC5, 0xC5, 0x93, 0xEB, 0x32, 0x89, 0x0A, 0xDA,
                               0x15, 0x3C, 0x9F, 0xA2, 0x1F, 0x0E, 0xDE, 0x01};


const WidevineKeybox kDefaultKeybox =
{
    // Sample keybox used for test vectors
    {
        // deviceID
        0x54, 0x65, 0x73, 0x74, 0x4b, 0x65, 0x79, 0x30, // TestKey01
        0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
    }, {
        // key
        0xfb, 0xda, 0x04, 0x89, 0xa1, 0x58, 0x16, 0x0e,
        0xa4, 0x02, 0xe9, 0x29, 0xe3, 0xb6, 0x8f, 0x04,
    }, {
        // data
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x10, 0x19,
        0x07, 0xd9, 0xff, 0xde, 0x13, 0xaa, 0x95, 0xc1,
        0x22, 0x67, 0x80, 0x53, 0x36, 0x21, 0x36, 0xbd,
        0xf8, 0x40, 0x8f, 0x82, 0x76, 0xe4, 0xc2, 0xd8,
        0x7e, 0xc5, 0x2b, 0x61, 0xaa, 0x1b, 0x9f, 0x64,
        0x6e, 0x58, 0x73, 0x49, 0x30, 0xac, 0xeb, 0xe8,
        0x99, 0xb3, 0xe4, 0x64, 0x18, 0x9a, 0x14, 0xa8,
        0x72, 0x02, 0xfb, 0x02, 0x57, 0x4e, 0x70, 0x64,
        0x0b, 0xd2, 0x2e, 0xf4, 0x4b, 0x2d, 0x7e, 0x39,
    }, {
        // magic
        0x6b, 0x62, 0x6f, 0x78,
    }, {
        // Crc
        0x0a, 0x7a, 0x2c, 0x35,
    }
};
WvKeybox g_keybox;
 
bool InstallKeybox(WvKeybox * dv_keybox,const uint8_t* buffer, size_t keyBoxLength)
{
	uint8_t au1ClearDeviceKey[16];
	aes_context ctx;
	
    if (keyBoxLength != 128)
    {
    	DMSG("keyBoxLength != 128  error\n");
        return false;
    }
	
    const WidevineKeybox* keybox
    = (const WidevineKeybox*)(buffer);

	memcpy_n2s_chk(dv_keybox->device_id_, keybox->device_id_,
           sizeof(keybox->device_id_),NULL);

	if(1)
	{
		DMSG("\nTZ InstallKeybox decrypt device_key_ \n ");
		// Decrypt [AES-128-ECB] the device key in the keybox with the MTK root key
		aes_setkey_dec(&ctx, g_OEMKey, (INT32)128);
		aes_crypt_ecb(&ctx, AES_DECRYPT, keybox->device_key_, au1ClearDeviceKey);

		memcpy_s2s_chk(dv_keybox->device_key_.keyvalue_, au1ClearDeviceKey, sizeof(au1ClearDeviceKey),NULL);
	}
	else
    {	
    	DMSG("\nTZ InstallKeybox not decrypt device_key_ \n ");
    	memcpy_n2s_chk(dv_keybox->device_key_.keyvalue_, keybox->device_key_,sizeof(keybox->device_key_),NULL);
	}
	
	dv_keybox->device_key_.valid_ = true;
	dv_keybox->device_key_.keydatalen_ = sizeof(keybox->device_key_);
    dv_keybox->device_key_.type_ = (KeyType)KEYTYPE_DEVICE;
	
    memcpy_n2s_chk(dv_keybox->key_data_, keybox->data_, sizeof(keybox->data_),NULL);
    memcpy_n2s_chk(dv_keybox->magic_, keybox->magic_, sizeof(keybox->magic_),NULL);
    memcpy_n2s_chk(dv_keybox->crc_, keybox->crc_, sizeof(keybox->crc_),NULL);
    dv_keybox->valid_ = true;

	dprintf("\nTZ InstallKeybox device_key_= " );

	for(int i = 0; i < 16; i++)
		dprintf("0x%x", dv_keybox->device_key_.keyvalue_[i]);
	
	dprintf("\n");

    return true;
}
bool InstallKeyboxTz(const uint8_t* buffer, size_t keyBoxLength)
{
	return InstallKeybox(&g_keybox,buffer,keyBoxLength);
}

int checkCPUendian_wv()
{
	union{
		unsigned long int i;
		unsigned char s[4];
	}c;

	c.i = 0x12345678;

	if(0x12 == c.s[0] )
	{
//printf("TZ checkCPUendian Big endian \n");
		return 1;
	}
	else
	{ 
//printf("TZ checkCPUendian small endian \n");
		return 0;
	}
	 
}


uint32_t t_ntohl(uint32_t n)
{
 //
 return checkCPUendian_wv()?n:BigLittleSwap32(n);
}

E_KeyboxError ValidateKeyboxTZ()
{
#if 0
	if (g_keybox.valid_ != true)//for test only
	{
	 	DMSG("[ValidateKeyboxTZ] do not have keybox, install default keybox \n");
		InstallKeybox(&g_keybox,(uint8_t*)&kDefaultKeybox,sizeof(WidevineKeybox)); 
	}
#endif
	
    WvKeybox * dv_keybox = &g_keybox;

	if (!dv_keybox->valid_)
    {
        DMSG("[KEYBOX NOT LOADED]\n");
        return OTHER_ERROR;
    }

#if 0
	uint8_t magic_[4] = {0x6b, 0x62, 0x6f, 0x78};

	if (memcmp(dv_keybox->magic_, kDefaultKeybox.magic_, 4) != 0)
    {
        DMSG("[KEYBOX HAS BAD MAGIC]\n");
        return BAD_MAGIC;
    }
	
    uint32_t crc_computed;
    uint32_t* crc_stored = (uint32_t*)dv_keybox->crc_;
    WidevineKeybox keybox;
    memset(&keybox, 0, sizeof(keybox));
    memcpy(keybox.device_id_, &dv_keybox->device_id_[0], sizeof(dv_keybox->device_id_));
    memcpy(keybox.device_key_, &dv_keybox->device_key_.keyvalue_, sizeof(keybox.device_key_));
    memcpy(keybox.data_, dv_keybox->key_data_, sizeof(keybox.data_));
    memcpy(keybox.magic_, dv_keybox->magic_, sizeof(keybox.magic_));

    crc_computed = t_ntohl(wvcrc32((uint8_t*)(&keybox),
                                 sizeof(keybox) - 4)); // Don't include last 4 bytes.
    if (crc_computed != *crc_stored)
    {
        DMSG("[KEYBOX CRC problem: computed = %08x,  stored = %08x]\n",
             crc_computed, *crc_stored);
        return BAD_CRC;
    }
#endif
	
    return (E_KeyboxError)NO_ERROR;
}


#if 0
bool GetDeviceId(uint8_t * device_id, size_t *id_len)
{
    if (g_keybox.valid_ == true)
    {
        memcpy(device_id,g_keybox.device_id_,DEVICE_ID_LENGTH);
        *id_len = DEVICE_ID_LENGTH;
        return true;
    }
    return false;
}
WvKey* DeviceKey()
{
    return &(g_keybox.device_key_);
}
bool GetKeyData(uint8_t *key_buff, size_t *keydata_len)
{
    if (g_keybox.valid_ == true)
    {
        memcpy(key_buff,g_keybox.key_data_,KEY_DATA_LENGTH);
        *keydata_len = KEY_DATA_LENGTH;
		return true;

    }
    return false;
}

#endif



