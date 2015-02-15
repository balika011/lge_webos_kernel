/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#ifndef OEMCRYPTO_KEY_MOCK_H_
#define OEMCRYPTO_KEY_MOCK_H_

#include "type_c.h"
#include "mtkutil_vector.h"
#include "mtkcrypto_keyctrlblk.h"

typedef enum  {
  KEYTYPE_UNKNOWN,
  KEYTYPE_PREPROV,
  KEYTYPE_ROOT,
  KEYTYPE_DEVICE,
  KEYTYPE_CONTENT,
  KEYTYPE_CONTENT_AUDIO,
  KEYTYPE_CONTENT_VIDEO,
  KEYTYPE_MAX
}KeyType;

// AES-128 crypto key
typedef struct Key {

  bool valid;

  KeyType type;

  Vector  value;

  bool has_control;

  KeyControlBlock control;

}Key;

void KeyInit(Key *this);
void KeyTerm(Key *this);

void Keyclear(Key *this); 
bool KeysetValue(Key *this, const uint8_t *key_string, size_t key_string_length);
bool KeysetType(Key *this, KeyType ktype);
bool KeysetControl(Key *this ,KeyControlBlock *control);
bool KeyUpdateDuration(Key *this, const KeyControlBlock *control);
bool KeyisDeviceKey(Key *this);
bool KeyisRootKey(Key *this);
bool KeyisPreprovKey(Key *this);
bool KeyisContentKey(Key *this);

Key *KeyDup(const Key* this);
void KeyDestroy(Key* this);


#endif
