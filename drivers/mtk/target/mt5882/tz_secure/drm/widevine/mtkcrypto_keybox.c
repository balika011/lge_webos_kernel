/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "type_c.h"

#include "mtkcrypto_keyctrlblk.h"
#include "mtkcrypto_keybox.h"
#include "log.h"
#include "wvcrc32.h"
#include "wv_keybox.h"
#include "mtkcrypto_glue.h"

#ifdef CC_WIDEVINE_SUPPORT_USAGE_REPORT    
#include "rpmb.h"
#endif

WidevineKeybox kDefaultKeybox = {
    // Sample keybox used for test vectors
    {
        // deviceID
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // TestKey01
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
    }, {
        // key
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }, {
        // data
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }, {
        // magic
        0x00, 0x00, 0x00, 0x00,
    }, {
        // Crc
        0x00, 0x00, 0x00, 0x00,
    }
};

char wrapkey[]= //echo 'mtk_device_ket' | openssl dgst -md5
{
    #include "custom_key.tbl"
};

bool WKBInstallKeyboxWithoutWrap(WvKeybox *this,
        const uint8_t* buffer,
        size_t keyBoxLength) {

    const WidevineKeybox* keybox = (const WidevineKeybox*)buffer;

    if (keyBoxLength != KEY_BOX_LENGTH) {
        return false;
    }

    if(this!=NULL)
    {
        VecAssign(&this->device_id,
                keybox->device_id_, sizeof(keybox->device_id_));

        KeysetValue(&(this->device_key),
                keybox->device_key_, sizeof(keybox->device_key_));

        KeysetType(&(this->device_key),KEYTYPE_DEVICE);

        memcpy(this->key_data, keybox->data_, sizeof(keybox->data_));
        memcpy(this->magic, keybox->magic_, sizeof(keybox->magic_));
        memcpy(this->crc, keybox->crc_, sizeof(keybox->crc_));
    }
    
    memcpy((void *)&kDefaultKeybox , buffer , keyBoxLength);

    return true;
}

bool WKBInstallKeybox(WvKeybox *this,
        const uint8_t* buffer,
        size_t keyBoxLength) {

    WidevineKeybox keybox ;

    uint8_t iv[16];

    if (keyBoxLength != KEY_BOX_LENGTH) {
        return false;
    }

    memset(iv,0,sizeof(iv));

    AES_CBC_DECRYPT(wrapkey, sizeof(wrapkey), iv, buffer, (uint8_t *) &keybox,  sizeof(keybox));

    return WKBInstallKeyboxWithoutWrap(this , (uint8_t *) &keybox, keyBoxLength);

}

bool WKBWrapKeybox(const uint8_t* keybox,
        size_t keyBoxLength,
        uint8_t* wrappedKeybox,
        size_t* wrappedKeyBoxLength){

    uint8_t iv[16];
    memset(iv,0,sizeof(iv));
    return AES_CBC_ENCRYPT(wrapkey, sizeof(wrapkey), iv, keybox, wrappedKeybox,  keyBoxLength);
}

static inline bool WvKeyboxPrepare(WvKeybox *this) {

    WKBInstallKeyboxWithoutWrap(this,
            (const uint8_t*)(&kDefaultKeybox), sizeof(kDefaultKeybox));
    this->valid = true;
    return this->valid;
}

void WKBInit(WvKeybox *this)
{
    this->valid=false;
    VecInit(&this->device_id);
    KeyInit(&this->device_key);
    WvKeyboxPrepare(this);
}

void WKBTerm(WvKeybox *this)
{
    this->valid=false;
    VecTerm(&this->device_id);
    KeyTerm(&this->device_key);
}

KeyboxError WKBValidate(WvKeybox *this) {
    uint32_t crc_computed;
    uint32_t* crc_stored;
    WidevineKeybox keybox;
    KeyboxError ret = NO_ERROR_PASS;
    WvKeybox defaultBox;
    WKBInit(&defaultBox);
    
    if(this==NULL)
    {
        this = &defaultBox;
    }

    crc_stored = (uint32_t*)this->crc;

    if (!this->valid) {
        LOGE("[KEYBOX NOT LOADED]");
        ret =  OTHER_ERROR;
        goto final;
    }
    
    if (strncmp((char *)(this->magic), "kbox", 4) != 0) {
        LOGE("[KEYBOX HAS BAD MAGIC]");
        ret =   BAD_MAGIC;
        goto final;
        
    }

    memset(&keybox, 0, sizeof(keybox));

    memcpy(keybox.device_id_, this->device_id.buf, this->device_id.size);

    memcpy(keybox.device_key_, this->device_key.value.buf, sizeof(keybox.device_key_));

    memcpy(keybox.data_, this->key_data, sizeof(keybox.data_));

    memcpy(keybox.magic_, this->magic, sizeof(keybox.magic_));

    crc_computed = net_to_host(wvcrc32((uint8_t*) &keybox,
                sizeof(keybox) - 4)); // Don't include last 4 bytes.

    if (crc_computed != *crc_stored) {
        LOGE("[KEYBOX CRC problem: computed = %08x,  stored = %08x]\n",
                crc_computed, *crc_stored);
        ret = BAD_CRC;
        goto final;        
    }

final:

    WKBTerm(&defaultBox);    
    return ret;;
}

