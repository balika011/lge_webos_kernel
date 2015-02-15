/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#include "mtkcrypto_key.h"

#include "log.h"
#include "wv_cdm_constants.h"

void KeyInit(Key *this)
{
  this->valid=false;
  this->type=KEYTYPE_UNKNOWN;
  this->has_control=false;
  VecInit(&this->value);
}

void KeyTerm(Key *this)
{
    Keyclear(this);
}

static inline bool KeyisValidType(Key *this) {
    return ((KEYTYPE_UNKNOWN < this->type) && (KEYTYPE_MAX > this->type));
}

void Keyclear(Key *this) { 
    VecTerm(&this->value);
    this->valid = false; 
}

Key *KeyDup(const Key* this)
{
    Key *ret = malloc(sizeof(Key));
    if(ret !=NULL){    
        memcpy((void *) ret , this, sizeof(Key));
        VecInit(&ret->value);
        VecCpy(&ret->value, &this->value);
    }
    return ret;
}

void KeyDestroy(Key* this)
{
    KeyTerm(this);
    free(this);
}

// Key is valid iff setValue(), setType(), and setControl() have been called

bool KeysetValue(Key *this, const uint8_t *key_string, size_t key_string_length) {

    this->valid = false;

    if (!key_string || key_string_length == 0) {
        return false;
    }

    VecAssign(&this->value,key_string, key_string_length);

    if (KeyisValidType(this) && this->has_control) {
        this->valid = true;
    }
    return this->valid;
}

bool KeysetType(Key *this, KeyType ktype) {
    this->valid = false;
    this->type = ktype;

    if (VecIsEmpty(&this->value)) {
        return false;
    }
    if (KeyisValidType(this) && this->has_control) {
        this->valid = true;
    }

    return this->valid;
}

bool KeysetControl(Key *this ,KeyControlBlock *control) {

    this->valid = false;
    if (!this-> control.valid) {
        return false;
    }

    memcpy(&this->control , control , sizeof(KeyControlBlock));
    this->has_control = true;

    if (KeyisValidType(this) && !VecIsEmpty(&this->value)) {
        this->valid = true;
    }
    return this->valid;
}

bool KeyUpdateDuration(Key *this,const KeyControlBlock *control) {

    this->valid = false;
    if (!control->valid || !this->has_control) {
        LOGE("UpdateDuration: control block not valid.");
        return false;
    }

    this->control.duration=control->duration;

    if (KeyisValidType(this) && !(VecIsEmpty(&this->value))) {
        this->valid = true;
    } else {
        LOGE("UpdateDuration: value or type bad.");
    }
    return this->valid;
}

bool KeyisDeviceKey(Key *this) { 
    return (KEYTYPE_DEVICE == this->type); 
}

bool KeyisRootKey(Key *this) {
    return (KEYTYPE_ROOT == this->type); 
}

bool KeyisPreprovKey(Key *this) { 
    return (KEYTYPE_PREPROV == this->type); 
}

bool KeyisContentKey(Key *this) {
    bool ctypes = (KEYTYPE_CONTENT == this->type) ||
        (KEYTYPE_CONTENT_AUDIO == this->type) ||
        (KEYTYPE_CONTENT_VIDEO == this->type);
    return ctypes;
}
