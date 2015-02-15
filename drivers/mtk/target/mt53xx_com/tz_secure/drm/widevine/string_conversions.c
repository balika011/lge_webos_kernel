// Copyright 2013 Google Inc. All Rights Reserved.


#include "type_c.h"
#include "string_conversions.h"
#include "log.h"

static bool CharToDigit(char ch, unsigned char* digit) {
    if (ch >= '0' && ch <= '9') {
        *digit = ch - '0';
    } else {
        if ((ch >= 'A') && (ch <= 'F'))
        {
            ch = ch + 'a' - 'A';
        }
        if ((ch >= 'a') && (ch <= 'f')) {
            *digit = ch - 'a' + 10;
        } else {
            return false;
        }
    }
    return true;
}

// converts an ascii hex string(2 bytes per digit) into a decimal byte string
Vector a2b_hex(const String *byte) {

    Vector array;
    unsigned int i;
    unsigned char msb;
    unsigned char lsb;

    unsigned int count = byte->size;

    VecInit( &array);
    VecPushBack( &array,0 );

    if (count == 0 || (count % 2) != 0) {
        LOGE("Invalid input size %u for string %s", count, byte->buf);
        return array;
    }

    for (i = 0; i < count / 2; ++i) {
        msb = 0;  // most significant 4 bits
        lsb = 0;  // least significant 4 bits
        if (!CharToDigit(byte->buf[i * 2], &msb) ||
                !CharToDigit(byte->buf[i * 2 + 1], &lsb))
        {
            LOGE("Invalid hex value %c%c at index %d", 
                    byte->buf[i * 2], byte->buf[i * 2 + 1], i);
            return array;
        }

        VecPushBack( &array, (msb << 4) | lsb);
    }

    return array;
}

String b2a_hex(const Vector *byte) {
    return HexEncode(byte->buf, byte->size);
}

String HexEncode(const uint8_t* in_buffer, unsigned int size) {

    static const char kHexChars[] = "0123456789ABCDEF";
    char byte; 
    unsigned int i;

    // Each input byte creates two output hex characters.
    Vector str;

    str.buf=malloc(size*2+1);
    
    if(str.buf ==NULL){
        str.size=0;
        goto final;

    }    
    str.size=size*2+1;

    *(str.buf+str.size-1)='\0';

    for (i = 0; i < size; ++i) {
        byte = in_buffer[i];
        str.buf[(i << 1)] = kHexChars[(byte >> 4) & 0xf];
        str.buf[(i << 1) + 1] = kHexChars[byte & 0xf];
    }
final:    
    return str;
}

// log10(2) ~= 0.3 bytes needed per bit or per byte log10(2**8) ~= 2.4.
// So round up to allocate 3 output characters per byte, plus 1 for '-'.
#define SIZE_OF_INT_STR (3 * sizeof(int) + 1+1)

String IntToString(int value) {

    char buffer[SIZE_OF_INT_STR];

    Vector out_string;
    VecInit( &out_string);

    memset(buffer, 0,SIZE_OF_INT_STR);
    //snprintf(buffer, SIZE_OF_INT_STR, "%d", value);

    VecAssign(&out_string, (uint8_t *)buffer , sizeof(buffer));

    return out_string;
}

// log10(2) ~= 0.3 bytes needed per bit or per byte log10(2**8) ~= 2.4.
// So round up to allocate 3 output characters per byte.
#define SIZE_OF_UINT_STR (3 * sizeof(int)+1)

String UintToString(unsigned int value) {
    char buffer[SIZE_OF_UINT_STR];
    Vector out_string;
    VecInit( &out_string);

    memset(buffer, 0, SIZE_OF_UINT_STR);
    //snprintf(buffer, SIZE_OF_UINT_STR, "%u", value);

    VecAssign(&out_string, (uint8_t  *)buffer , sizeof(buffer));

    return out_string;
}
