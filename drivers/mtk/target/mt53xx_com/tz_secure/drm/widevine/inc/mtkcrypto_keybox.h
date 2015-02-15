/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#ifndef OEMCRYPTO_KEYBOX_MOCK_H_
#define OEMCRYPTO_KEYBOX_MOCK_H_

#include "mtkcrypto_key.h"

#define DEVICE_KEY_LENGTH  16
typedef uint8_t WvKeyboxKey[DEVICE_KEY_LENGTH];

#define  KEY_DATA_LENGTH 72
typedef uint8_t WvKeyboxKeyData[KEY_DATA_LENGTH];

#define KEY_BOX_LENGTH 128

typedef enum { 
    NO_ERROR_PASS, 
    BAD_CRC, 
    BAD_MAGIC, 
    OTHER_ERROR 
}KeyboxError ;

typedef struct  WvKeybox {

  bool valid;
  Vector device_id;
  Key device_key;
  WvKeyboxKeyData key_data;
  uint8_t magic[4];
  uint8_t crc[4];
}WvKeybox;

void WKBInit(WvKeybox *this);
void WKBTerm(WvKeybox *this);

KeyboxError WKBValidate(WvKeybox *this);

bool WKBInstallKeyboxWithoutWrap(WvKeybox *this,
        const uint8_t* buffer,
        size_t keyBoxLength);

bool WKBInstallKeybox(WvKeybox *this,
        const uint8_t* buffer,
        size_t keyBoxLength);

bool WKBWrapKeybox(const uint8_t* keybox,
        size_t keyBoxLength,
        uint8_t* wrappedKeybox,
        size_t* wrappedKeyBoxLength);

static inline size_t WKBkey_data_length(WvKeybox *this) { return KEY_DATA_LENGTH; }

#endif
