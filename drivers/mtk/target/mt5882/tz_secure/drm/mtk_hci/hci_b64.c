/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */
/* cmn/include */
#include "basic_types.h"
/* sec/include */
#include "cmn_api_sec.h"
/* cmn */
#include "dbg_api.h"
#include "binstr_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"


static uint8_t  g_b64_dectab[256] = {
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 62, 99, 99, 99, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 99, 99, 99, 64, 99, 99,
    99,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 99, 99, 99, 99, 99,
    99, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
};


static uint32_t
l_b64Decode(const uint8_t  *in_buf,
            uint32_t        in_len,
            uint8_t        *out_buf,
            uint32_t       *out_len)
{
    uint32_t   ret  = ERR_HCI_OK;
    uint32_t   i    = 0;
    uint32_t   j    = 0;
    int32_t    cn   = 0;
    int32_t    inc;
    uint32_t   outc = 0;

    MSGENT(("l_b64Decode"));

    while ( i < in_len ) {
        inc = g_b64_dectab[in_buf[i++] & 255];
        if ( inc == 64 ) {
            break;
        }
        /* Illegal base64 character check */
        if ( inc > 64 ) {
            ret = ERR_HCI_INTERNALERROR;
            break;
        } else if ( inc < 64 ) {
            outc = (outc << 6) | inc;
            cn ++;
            if ( cn == 4 ) {
                out_buf[j++] = (outc >> 16) & 255;
                out_buf[j++] = (outc >> 8) & 255;
                out_buf[j++] = outc & 255;
                cn = 0;
                outc = 0;
            }
        }
    }

    switch ( cn ) {
    case 2:
        out_buf[j++] = (outc >> 4) & 255;
        break;
    case 3:
        out_buf[j++] = (outc >> 10) & 255;
        out_buf[j++] = (outc >> 2) & 255;
        break;
    case 1:
    default:
        break;
    }
    *out_len = j;

    MSGEXT(("l_b64Decode"));
    return ret;
}


uint32_t
hci_l_b64Decode(const binstr_t *in_str,
                binstr_t      **out_str)
{
    uint32_t        ret    = ERR_HCI_OK;
    uint32_t        len    = 0;
    uint32_t        tmplen = 0;
    const uint8_t  *buf;
    uint8_t        *tmpbuf = NULL;
    binstr_t       *newstr;

    MSGENT(("hci_l_b64Decode"));

    if ( !in_str || !out_str ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( binstr_Len(in_str) != 0 ) {
        buf = binstr_Buf(in_str);
        len = binstr_Len(in_str);
        tmplen = (len * 3 + 3) / 4;
        RET(hci_l_mem_Alloc(tmplen, &tmpbuf));
        RET(l_b64Decode(buf, len, tmpbuf, &tmplen));
    }

    RET(hci_l_mem_BinstrAllocWithCopy(tmplen, tmpbuf, &newstr));
    *out_str = newstr;

EXIT:
    hci_l_mem_Free(tmpbuf);
    MSGEXT(("hci_l_b64Decode"));
    return ret;
}
