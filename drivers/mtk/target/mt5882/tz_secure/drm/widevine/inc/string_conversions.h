// Copyright 2013 Google Inc. All Rights Reserved.

#ifndef OEMCRYPTO_STRING_CONVERSIONS_H_
#define OEMCRYPTO_STRING_CONVERSIONS_H_

#include "mtkutil_vector.h"

String a2b_hex(const String *b);
String b2a_hex(const String *b);
String HexEncode(const uint8_t* bytes, unsigned size);
String IntToString(int value);
String UintToString(unsigned int value);

#endif  // OEMCRYPTO_STRING_CONVERSIONS_H_
