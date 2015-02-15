/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * Reference implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "type_c.h"

#include "OEMCryptoCENC.h"
#include "mtkcrypto_engine.h"
#include "mtkcrypto_glue.h"
#include "wv_keybox.h"

#include "tomcrypt.h"
#include "sec_buf.h"

#define ASSET_KEY_SIZE 16
#define AES_BLOCK_SIZE 16
#define CTRL_WORD_SIZE 32

extern WidevineKeybox kDefaultKeybox;

static uint8_t assetKey[ASSET_KEY_SIZE];
static uint8_t ctrlword[CTRL_WORD_SIZE] ;

OEMCryptoResult OEMCrypto_SetEntitlementKey(const uint8_t *emmKey ,
        uint32_t emmKeyLength)
{
    symmetric_key skey;

    if(ASSET_KEY_SIZE !=emmKeyLength){
        goto fail;
    }

    if(OEMCrypto_SUCCESS == OEMCrypto_IsKeyboxValid()){

        if (aes_setup(kDefaultKeybox.device_key_, sizeof(kDefaultKeybox.device_key_), 0, &skey) != CRYPT_OK) {
            goto fail;
        }

        if (aes_ecb_decrypt(emmKey, assetKey, &skey) != CRYPT_OK){
            goto fail;
        }

        return OEMCrypto_SUCCESS;
    }

fail:    
    return OEMCrypto_ERROR_KEYBOX_INVALID;
}

OEMCryptoResult OEMCrypto_DeriveControlWord(const uint8_t *ecm,
        uint32_t length,
        uint32_t *flags)
{
    symmetric_CBC cbc;
    uint8_t iv[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int cipher_idx = register_cipher(&aes_desc);

    if(CTRL_WORD_SIZE != length ){
        goto fail;
   }

    if(-1 == cipher_idx){
        goto fail;
    }

    if (cbc_start(cipher_idx, iv , assetKey, ASSET_KEY_SIZE, 0 , &cbc) != CRYPT_OK){
        goto fail;
    }

    if (cbc_decrypt(ecm, ctrlword, length, &cbc)  != CRYPT_OK){
        goto fail;
    }

    if (cbc_done(&cbc) != CRYPT_OK){
        goto fail;
    }

    memcpy(flags , ctrlword , 4);

    return OEMCrypto_SUCCESS;

fail:    

    return OEMCrypto_ERROR_NO_CW;
}

OEMCryptoResult OEMCrypto_DecryptVideo(uint8_t *iv,
        uint8_t *input,
        uint32_t inputLength,
        uint32_t output_handle,
        uint32_t output_offset,
        uint32_t *outputLength)
{

    if(sec_buf_verify(output_handle , output_offset + inputLength ) == false){
        return OEMCrypto_ERROR_DECRYPT_FAILED;
    }

    if( AES_CBC_DECRYPT_UPDATE_IV(&(ctrlword[4]), ASSET_KEY_SIZE, iv,
                input, ((uint8_t *) output_handle) + output_offset , inputLength))
    {
        *outputLength = inputLength;
        return OEMCrypto_SUCCESS;
    }
    else
    {
        *outputLength = 0;
        return OEMCrypto_ERROR_DECRYPT_FAILED;
    }
}

static inline int isVideoFrame(uint8_t *frame, int len)
{
    if (frame == NULL || len < 3)
    {
        return false;
    }
    if (frame[0] == 0x00 && frame[1] == 0x00 && frame[2] == 0x01)
    {
        return true;
    }
    if (len < 4)
    {
        return false;
    }
    if (frame[0] == 0x00 && frame[1] == 0x00 && frame[2] == 0x00 && frame[3] ==
            0x01)
    {
        return true;
    }
    return false;
}

OEMCryptoResult OEMCrypto_DecryptAudio(uint8_t *iv,
        uint8_t *input,
        uint32_t inputLength,
        uint8_t *output,
        uint32_t *outputLength)
{
    uint8_t iv_clone[AES_BLOCK_SIZE];
    uint8_t buf[AES_BLOCK_SIZE];

    if(iv !=NULL){
        memcpy(iv_clone , iv , AES_BLOCK_SIZE);

        if(!AES_CBC_DECRYPT(&(ctrlword[4]), ASSET_KEY_SIZE, iv_clone, input, buf, AES_BLOCK_SIZE))
        {
            goto fail;
        }

        if(isVideoFrame(buf,AES_BLOCK_SIZE)){
            goto fail;
        }
    }
    if( AES_CBC_DECRYPT_UPDATE_IV(&(ctrlword[4]), ASSET_KEY_SIZE, iv, input, output, inputLength))
    {
        *outputLength = inputLength;
        return OEMCrypto_SUCCESS;
    }

fail:

    *outputLength = 0;
    return OEMCrypto_ERROR_DECRYPT_FAILED;
}

