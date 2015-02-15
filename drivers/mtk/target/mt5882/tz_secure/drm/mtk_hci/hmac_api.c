/*
 * Nautilus version 3.0
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Sony Corporation, All Rights Reserved.
 */
#include "basic_types.h"
#include "dbg_api.h"
#include "mem_api.h"
#include "binstr_api.h"


#include "hmac_api.h"
#include "sha1_api.h"
#include "sha256_api.h"
//#include "hci_api_util.h"
#include "cmn_api_sec.h"

#ifndef NULL
#  define NULL	(void *)0
#endif

#define L_BLOCKSIZE_SHA1	SHA1_DATASIZE
#define L_DIGESTSIZE_SHA1	SHA1_DIGESTSIZE
#define L_BLOCKSIZE_SHA256	SHA256_DATASIZE
#define L_DIGESTSIZE_SHA256	SHA256_DIGESTSIZE

#define L_IPAD	"\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36\x36"
#define L_OPAD	"\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c"

/*==========================================================================
 * 2. Definition of HMAC (RFC2104)
 *
 *   The definition of HMAC requires a cryptographic hash function, which
 *   we denote by H, and a secret key K. We assume H to be a cryptographic
 *   hash function where data is hashed by iterating a basic compression
 *   function on blocks of data.   We denote by B the byte-length of such
 *   blocks (B=64 for all the above mentioned examples of hash functions),
 *   and by L the byte-length of hash outputs (L=16 for MD5, L=20 for
 *   SHA-1).  The authentication key K can be of any length up to B, the
 *   block length of the hash function.  Applications that use keys longer
 *   than B bytes will first hash the key using H and then use the
 *   resultant L byte string as the actual key to HMAC. In any case the
 *   minimal recommended length for K is L bytes (as the hash output
 *   length). See section 3 for more information on keys.
 *
 *   We define two fixed and different strings ipad and opad as follows
 *   (the 'i' and 'o' are mnemonics for inner and outer):
 *
 *                  ipad = the byte 0x36 repeated B times
 *                  opad = the byte 0x5C repeated B times.
 *
 *   To compute HMAC over the data `text' we perform
 *
 *                  H(K XOR opad, H(K XOR ipad, text))
 *========================================================================*/
/*==========================================================================
 * HMAC with SHA-256
 *
 * Block size = 64
 * L = 32
 *
 *========================================================================*/
retcode_t
hmac_Calculate(int             in_digest_algo,
               const binstr_t  *in_key,
               const binstr_t  *in_data,
               binstr_t       **out_hmac)
{
    retcode_t     ret       = ERR_OK;
    u_int32_t     digestsize;
    u_int32_t     blocksize;
    int           keyLen;
    u_int8_t      keypad[L_BLOCKSIZE_SHA256]; /* L_BLOCKSIZE + 1 (for '\0') */
    u_int8_t      keyIpad[L_BLOCKSIZE_SHA256]; /* L_BLOCKSIZE + 1 (for '\0') */
    u_int8_t      keyOpad[L_BLOCKSIZE_SHA256]; /* L_BLOCKSIZE + 1 (for '\0') */
    u_int8_t      digest[L_DIGESTSIZE_SHA256];
    binstr_t     *hmac      = NULL;
    sha1_handle_t  sha1     = NULL;
    sha256_handle_t sha256  = NULL;

    int i;

    MSGENT(("hmac_Calculate"));
    tom_memset(keypad, 0, L_BLOCKSIZE_SHA256);
    tom_memcpy(keyIpad, L_IPAD, L_BLOCKSIZE_SHA256);
    tom_memcpy(keyOpad, L_OPAD, L_BLOCKSIZE_SHA256);


    *out_hmac = NULL;

    switch (in_digest_algo) {
    case HMAC_ALGO_SHA1:
        blocksize = L_BLOCKSIZE_SHA1;
        digestsize = L_DIGESTSIZE_SHA1;
        break;
    case HMAC_ALGO_SHA256:
        blocksize = L_BLOCKSIZE_SHA256;
        digestsize = L_DIGESTSIZE_SHA256;
        break;
    default:
        RET(ERR_ILLEGALARGS);
        break;
    }

    keyLen  = hci_BinstrLen(in_key) > blocksize ? blocksize : hci_BinstrLen(in_key);

    if ((hmac = hci_BinstrAlloc(digestsize, NULL)) == NULL)
        RETX(ERR_NOMEM, MSG1(("*** hci_BinstrAlloc: Failed ...\n")));

    // generate keypad
    if(hci_BinstrLen(in_key) > blocksize)
    {
        switch (in_digest_algo) {
        case HMAC_ALGO_SHA1:
            RET(sha1_Init(&sha1));
            RET(sha1_Update(sha1, hci_BinstrBuf(in_key), hci_BinstrLen(in_key)));
            RET(sha1_Fin(sha1, digest));
            sha1 = NULL;
            break;
        case HMAC_ALGO_SHA256:
            RET(sha256_Init(&sha256));
            RET(sha256_Update(sha256, hci_BinstrBuf(in_key), hci_BinstrLen(in_key)));
            RET(sha256_Fin(sha256, digest));
            sha256 = NULL;
            break;
        default:
            break;
        }
        tom_memcpy(keypad, digest, digestsize);
    }
    else
    {
        tom_memcpy(keypad, hci_BinstrBuf(in_key), hci_BinstrLen(in_key));
    }
    /*
     *    (1) append zeros to the end of K to create a B byte string
     *        (e.g., if K is of length 20 bytes and B=64, then K will be
     *         appended with 44 zero bytes 0x00)
     *    (2) XOR (bitwise exclusive-OR) the B byte string computed in step
     *        (1) with ipad
     */
    for (i = 0; i < keyLen; i++)
        keyIpad[i] ^= keypad[i];

    /*
     *    (3) append the stream of data 'text' to the B byte string resulting
     *        from step (2)
     *    (4) apply H to the stream generated in step (3)
     */
    switch (in_digest_algo) {
    case HMAC_ALGO_SHA1:
        RET(sha1_Init(&sha1));
        RET(sha1_Update(sha1, keyIpad, blocksize));
        RET(sha1_Update(sha1, hci_BinstrBuf(in_data), hci_BinstrLen(in_data)));
        RET(sha1_Fin(sha1, digest));
        sha1 = NULL;
        break;
    case HMAC_ALGO_SHA256:
        RET(sha256_Init(&sha256));
        RET(sha256_Update(sha256, keyIpad, blocksize));
        RET(sha256_Update(sha256, hci_BinstrBuf(in_data), hci_BinstrLen(in_data)));
        RET(sha256_Fin(sha256, digest));
        sha256 = NULL;
        break;
    default:
        break;
    }

    /*
     *    (5) XOR (bitwise exclusive-OR) the B byte string computed in
     *        step (1) with opad
     */
    for (i = 0; i < keyLen; i++)
        keyOpad[i] ^= keypad[i];

    /*
     *    (6) append the H result from step (4) to the B byte string
     *        resulting from step (5)
     *    (7) apply H to the stream generated in step (6) and output
     *        the result
     */
    switch (in_digest_algo) {
    case HMAC_ALGO_SHA1:
        RET(sha1_Init(&sha1));
        RET(sha1_Update(sha1, keyOpad, blocksize));
        RET(sha1_Update(sha1, digest, digestsize));
        RET(sha1_Fin(sha1, hci_BinstrBuf(hmac)));
        sha1 = NULL;
        break;
    case HMAC_ALGO_SHA256:
        RET(sha256_Init(&sha256));
        RET(sha256_Update(sha256, keyOpad, blocksize));
        RET(sha256_Update(sha256, digest, digestsize));
        RET(sha256_Fin(sha256, hci_BinstrBuf(hmac)));
        sha256 = NULL;
        break;
    default:
        break;
    }

    *out_hmac = hmac;

  EXIT:
    if (*out_hmac == NULL) hci_BinstrFree(hmac);
    sha1_Fin(sha1, NULL);
    sha256_Fin(sha256, NULL);
    MSGEXT(("hmac_Calculate"));

    return ret;
}
