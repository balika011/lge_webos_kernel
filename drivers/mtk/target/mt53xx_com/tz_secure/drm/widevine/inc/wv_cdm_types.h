// Copyright 2013 Google Inc. All Rights Reserved.

#ifndef OEMCRYPTO_WV_CDM_TYPES_H_
#define OEMCRYPTO_WV_CDM_TYPES_H_

//#include <map>
//#include <stdint.h>
//#include <string>
//
//typedef std::string CdmKeySystem;
//typedef std::string CdmInitData;
//typedef std::string CdmKeyMessage;
//typedef std::string CdmKeyResponse;
//typedef std::string KeyId;
//typedef std::string CdmSessionId;
//typedef std::string RequestId;
//typedef uint32_t CryptoResult;
//typedef uint32_t CryptoSessionId;
//typedef std::string CryptoKeyId;

enum CdmResponseType {
  NO_ERROR,
  UNKNOWN_ERROR,
  KEY_ADDED,
  KEY_ERROR,
  KEY_MESSAGE,
  NEED_KEY,
  KEY_CANCELED,
};

#define CORE_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

enum CdmEventType {
  LICENSE_EXPIRED,
  LICENSE_RENEWAL_NEEDED
};

// forward class references
//class KeyMessage;
//class Request;
//class Key;

#endif  // OEMCRYPTO_WV_CDM_TYPES_H_
