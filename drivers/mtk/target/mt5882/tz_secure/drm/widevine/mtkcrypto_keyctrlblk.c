/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "type_c.h"

#include "mtkcrypto_keyctrlblk.h"
#include "log.h"
#include "wv_cdm_constants.h"

void KCBInit(KeyControlBlock *this) {
    this->valid = false;
}

void KCBInitRefresh(KeyControlBlock *this, bool refresh){
    this->valid = false;
    this->refresh = refresh;
}


bool KCBValidate(KeyControlBlock *this) {

    this->valid = false;
    if ((0x6b63746c != this->verification)&&
        (0x6b633039 != this->verification)){  // kctl.
       LOGE("KCB: BAD verification string: %08X (not %08X)", this->verification,
                0x6b63746c);
        return false;
    }

    if (this->refresh) {
        if (this->control_bits & kControlObserveDataPath) {
            LOGW("KCB: data_path_type set for refresh.");
        }
        if (this->control_bits & kControlObserveHDCP) {
            LOGW("KCB: HDCP setting set for refresh.");
        }
        if (this->control_bits & kControlObserveCGMS) {
            LOGW("KCB: CGMS setting set for refresh.");
        }
    }

    this->valid = true;
    return this->valid;
}

// This extracts 4 bytes in network byte order to a 32 bit integer in
// host byte order.
static inline uint32_t KCBExtractField(const uint8_t *str, int idx) {
    //uint32_t *addr = ((uint32_t *)str)+ idx;
    //return ntohl(*addr);
    int bidx = idx * 4;
    uint32_t t = (unsigned char)(str[bidx]) << 24;
    t |= (unsigned char)(str[bidx + 1]) << 16;
    t |= (unsigned char)(str[bidx + 2]) << 8;
    t |= (unsigned char)(str[bidx + 3]);
    return t;
}

bool KCBSetFromString(KeyControlBlock *this ,const Vector * key_control_string) {

    if (key_control_string->size < KEY_CONTROL_SIZE) {
        LOGE("KCB: BAD Size: %d (not %d)",key_control_string->size, KEY_CONTROL_SIZE);
        return false;
    }

    this->verification = KCBExtractField(key_control_string->buf, 0);
    this->duration     = KCBExtractField(key_control_string->buf, 1);
    this->nonce        = KCBExtractField(key_control_string->buf, 2);
    this->control_bits = KCBExtractField(key_control_string->buf, 3);

    return KCBValidate(this);
}

void KCBInvalidate(KeyControlBlock *this)
{
    this->valid = false; 
}
