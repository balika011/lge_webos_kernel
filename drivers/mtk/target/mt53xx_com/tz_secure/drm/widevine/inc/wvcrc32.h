/*********************************************************************
 * wvcrc32.h
 *
 * (c) Copyright 2011-2012 Google, Inc.
 *
 * Compte CRC32 Checksum. Needed for verification of WV Keybox.
 *********************************************************************/

#ifndef WV_CRC_32_H_
#define WV_CRC_32_H_

#include "type_c.h"

uint32_t wvcrc32(uint8_t* p_begin, int i_count);

#endif //  WV_CRC_32_H_
