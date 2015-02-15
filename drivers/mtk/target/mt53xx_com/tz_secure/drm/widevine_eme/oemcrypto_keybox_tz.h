/*******************************************************************************
 *
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * mock implementation of OEMCrypto APIs
 *
 ******************************************************************************/

#ifndef OEMCRYPTO_KEYBOX_TZ_H_
#define OEMCRYPTO_KEYBOX_TZ_H_

 #include "oemcrypto_key_tz.h"
 

#ifdef __cplusplus
extern "C"{
#endif
 

extern WvKeybox g_keybox;
bool InstallKeyboxTz(const uint8_t* buffer, size_t keyBoxLength);

E_KeyboxError ValidateKeyboxTZ();
//bool GetDeviceId(uint8_t * device_id, size_t *id_len);

//WvKey* DeviceKey();

//bool GetKeyData(uint8_t *key_buff, size_t *keydata_len);

#ifdef __cplusplus
}
#endif

#endif
