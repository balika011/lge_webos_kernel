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
#include "mem_api.h"
#include "binstr_api.h"
#include "byte_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"
#include "hci_crypto.h"

#if 1 == HCI_L_API_HCI_IMPORTPPPROCDATA

#define L_BOX_HEADER_SIZE           (sizeof(uint32_t)*2)

/* 4CC of Box type */
#define L_BOXTYPE_NPID byte_4CHARCODE('N','P','I','D')
#define L_BOXTYPE_NPPU byte_4CHARCODE('N','P','P','U')
#define L_BOXTYPE_NPPR byte_4CHARCODE('N','P','P','R')
#define L_BOXTYPE_NROL byte_4CHARCODE('N','R','O','L')
#define L_BOXTYPE_OPID byte_4CHARCODE('O','P','I','D')
#define L_BOXTYPE_OPPU byte_4CHARCODE('O','P','P','U')
#define L_BOXTYPE_OPPR byte_4CHARCODE('O','P','P','R')
#define L_BOXTYPE_INID byte_4CHARCODE('I','N','I','D')
#define L_BOXTYPE_INPU byte_4CHARCODE('I','N','P','U')
#define L_BOXTYPE_INPR byte_4CHARCODE('I','N','P','R')
#define L_BOXTYPE_LROL byte_4CHARCODE('L','R','O','L')
#define L_BOXTYPE_RROL byte_4CHARCODE('R','R','O','L')
#define L_BOXTYPE_IROL byte_4CHARCODE('I','R','O','L')
#define L_BOXTYPE_LOID byte_4CHARCODE('L','O','I','D')
#define L_BOXTYPE_LOPU byte_4CHARCODE('L','O','P','U')
#define L_BOXTYPE_LOPR byte_4CHARCODE('L','O','P','R')
#define L_BOXTYPE_ROID byte_4CHARCODE('R','O','I','D')
#define L_BOXTYPE_ROPU byte_4CHARCODE('R','O','P','U')
#define L_BOXTYPE_ROPR byte_4CHARCODE('R','O','P','R')
#define L_BOXTYPE_CDLC byte_4CHARCODE('C','D','L','C')
#define L_BOXTYPE_LDLC byte_4CHARCODE('L','D','L','C')
#define L_BOXTYPE_BKBC byte_4CHARCODE('B','K','B','C')
#define L_BOXTYPE_CRLD byte_4CHARCODE('C','R','L','D')

#define L_BOX_BODYSIZE(boxsize) (boxsize-(sizeof(uint32_t)*2))

#define L_HASH_SIZE              20

static uint32_t
l_GetPrivatePersType(uint32_t             in_boxtype,
                     hci_pers_key_type_t *out_privtype)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("l_GetPrivatePersType"));

    switch ( in_boxtype ) {
    case L_BOXTYPE_NPPR:
        *out_privtype = HCI_PERSONALITY_TYPE_DEVICE_NPPR;
        break;
    case L_BOXTYPE_OPPR:
        *out_privtype = HCI_PERSONALITY_TYPE_DEVICE_OPPR;
        break;
    case L_BOXTYPE_LOPR:
        *out_privtype = HCI_PERSONALITY_TYPE_IMPORT_LOPR;
        break;
    case L_BOXTYPE_INPR:
        *out_privtype = HCI_PERSONALITY_TYPE_IMPORT_INPR;
        break;
    case L_BOXTYPE_ROPR:
        *out_privtype = HCI_PERSONALITY_TYPE_IMPORT_ROPR;
        break;
    default:
        RET(ERR_HCI_ILLEGALPPPROC);
    }
EXIT:
    MSGEXT(("l_GetPrivatePersType"));
    return ret;
}

uint32_t
hci_l_DecryptPpprocData( uint32_t      in_len,
                         uint8_t      *in_buf,
                         uint32_t      in_key_len,
                         uint8_t      *in_key,
                         uint8_t      *in_iv,
                         hci_enc_pad_t in_pad_type,
                         uint32_t     *out_buf_len,
                         uint8_t     **out_buf )
{
    uint32_t         ret       = ERR_HCI_OK;
    binstr_t         key_b64   = { in_key_len, in_key };
    binstr_t        *key       = NULL;
    uint8_t         *plain_buf = NULL;
    uint32_t         plain_len = 0;

    MSGENT(("hci_l_DecryptPpprocData"));

    /* Check input parameters */
    if ((in_len      < HCI_L_AES_KEYSIZE) ||
        (in_buf      == NULL) ||
        (out_buf_len == NULL) ||
        (out_buf     == NULL)) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    /* Prepare key to decrypt. */
    RET(hci_l_b64Decode(&key_b64, &key));

    /* Decrypt. */
    plain_len = in_len;
    RET( hci_l_mem_AllocWithZeros(plain_len, &plain_buf) );
    RETR(hci_l_cryptoDecryptAES128CBC(in_pad_type,
                                      binstr_Buf(key),
                                      in_iv,
                                      in_len,
                                      in_buf,
                                      &plain_len,
                                      plain_buf),
         ERR_HCI_TOOSMALLBUFFER == ret ? ERR_HCI_INTERNALERROR : ret);

    /* Return decrypted buffer. */
    *out_buf_len = plain_len;
    *out_buf     = plain_buf;
    plain_buf    = NULL;

EXIT:
    hci_l_FreeKey(key);
    hci_l_DataScrub(plain_buf, plain_len);
    hci_l_mem_Free(plain_buf);
    MSGEXT(("hci_l_DecryptPpprocData"));
    return ret;
}

uint32_t
hci_l_ParsePpprocData( uint32_t             in_len,
                       uint8_t             *in_buf,
                       uint32_t            *out_buf_len,
                       uint8_t            **out_buf )
{
    uint32_t             ret             = ERR_HCI_OK;
    uint32_t             len_out         = 0;
    uint32_t             boxsize;
    uint32_t             boxtype;
    uint8_t             *boxbody         = NULL;
    uint32_t             idx;
    uint8_t             *ptr_in;
    uint8_t             *ptr_out;
    uint8_t             *buf             = NULL;
    uint8_t              box_type_str[5];
    hci_pers_key_type_t  priv_pers_type  = 0;
    binstr_t             bkbc            = { 0, NULL };

    MSGENT(("hci_l_ParsePpprocData"));

    RET(hci_l_mem_Alloc(in_len, &buf));

    idx     = 0;
    ptr_in  = in_buf;
    ptr_out = buf;

    while ( idx < in_len ) {
        if ( in_len < L_BOX_HEADER_SIZE ) {
            RET( ERR_HCI_ILLEGALPPPROC );
        }

        /* box size */
        boxsize = byte_BE4R(ptr_in);
        idx += boxsize;
        if ( (idx > in_len) || (boxsize < L_BOX_HEADER_SIZE) ) {
            RET( ERR_HCI_ILLEGALPPPROC );
        }

        /* box type */
        boxtype = byte_BE4R(ptr_in + sizeof(uint32_t));

        /* box body */
        boxbody = (ptr_in + L_BOX_HEADER_SIZE);

        /* Log in human readable way */
        mem_Set(box_type_str, 0, sizeof box_type_str);
        byte_BE4W(box_type_str, boxtype);
        MSGI3(("%s[0x%x]:\n", box_type_str, boxsize));

        switch ( boxtype ) {
        case L_BOXTYPE_NPID:
        case L_BOXTYPE_OPID:
        case L_BOXTYPE_LOID:
        case L_BOXTYPE_INID:
        case L_BOXTYPE_ROID:
        case L_BOXTYPE_NPPU:
        case L_BOXTYPE_NROL:
        case L_BOXTYPE_OPPU:
        case L_BOXTYPE_LOPU:
        case L_BOXTYPE_INPU:
        case L_BOXTYPE_IROL:
        case L_BOXTYPE_RROL:
        case L_BOXTYPE_LROL:
        case L_BOXTYPE_CDLC:
        case L_BOXTYPE_LDLC:
        case L_BOXTYPE_ROPU:
        case L_BOXTYPE_CRLD:
            MSG3(("add pub box idx:%d\n", idx));
            mem_Copy(ptr_out, ptr_in, boxsize);
            ptr_out += boxsize;
            len_out += boxsize;
            break;
        case L_BOXTYPE_BKBC:
            /* BKB Container (has BK in it) should be protected on platform */
            MSG3(("add scrambled bkbc box idx:%d\n", idx));
            mem_Copy(ptr_out, ptr_in, boxsize);
            /* check if bkb container is non-empty */
            if (L_BOX_BODYSIZE(boxsize) > 0) {
                binstr_Buf(&bkbc) = ptr_out + L_BOX_HEADER_SIZE;
                binstr_Len(&bkbc) = L_BOX_BODYSIZE(boxsize);
                RET(hci_l_Scramble(&bkbc));
            }
            ptr_out += boxsize;
            len_out += boxsize;
            break;
        case L_BOXTYPE_NPPR:
        case L_BOXTYPE_OPPR:
        case L_BOXTYPE_LOPR:
        case L_BOXTYPE_INPR:
        case L_BOXTYPE_ROPR:
            MSG3(("priv box idx:%d\n", idx));
            RET(l_GetPrivatePersType(boxtype, &priv_pers_type));
            RET(hci_l_LoadPersonalityKey(priv_pers_type, L_BOX_BODYSIZE(boxsize), boxbody));
            break;
        default:
            RET( ERR_HCI_ILLEGALPPPROC );
            break;
        }

        ptr_in += boxsize;
    }

    *out_buf     = buf;
    *out_buf_len = len_out;
    buf          = NULL;

EXIT:
    hci_l_mem_Free(buf);
    MSGEXT(("hci_l_ParsePpprocData"));
    return ret;
}

uint32_t
hci_l_ParseMoaPpprocData( uint32_t             in_moa_ppproc_data_len,
                          uint8_t             *in_moa_ppproc_data,
                          uint32_t            *out_public_ppproc_len,
                          uint8_t            **out_public_ppproc )
{
    uint32_t  ret             = ERR_HCI_OK;
    uint8_t  *ppproc_data     = in_moa_ppproc_data;
    uint32_t  ppproc_len      = 0;
    uint32_t  digest_len      = HCI_L_SHA1_DIGESTSIZE;
    uint8_t   digest[HCI_L_SHA1_DIGESTSIZE];

    MSGENT(("hci_l_ParseMoaPpprocData"));

    if ( in_moa_ppproc_data_len <= (L_HASH_SIZE + 2) ) {
        RET( ERR_HCI_ILLEGALPPPROC );
    }

    /* get the ppproc length */
    ppproc_len = byte_BE2R(ppproc_data);
    if ((ppproc_len == 0) || (ppproc_len > (in_moa_ppproc_data_len - (L_HASH_SIZE + 2)))) {
        RET( ERR_HCI_ILLEGALPPPROC );
    }
    ppproc_data += 2; /* skip length bytes */

    /* check hash */
    RET(hci_l_cryptoMakeDigest(HCI_DIG_ALGO_SHA1, ppproc_len, ppproc_data, &digest_len, digest));
    if ( 0 != mem_Cmp(ppproc_data + ppproc_len, digest, HCI_L_SHA1_DIGESTSIZE) ) {
        RET( ERR_HCI_ILLEGALPPPROC );
    }

    RET(hci_l_ParsePpprocData(ppproc_len,
                              ppproc_data,
                              out_public_ppproc_len,
                              out_public_ppproc));

EXIT:
    MSGEXT(("hci_l_ParseMoaPpprocData"));
    return ret;
}

#endif /* #if 1 == HCI_L_API_HCI_IMPORTPPPROCDATA */
