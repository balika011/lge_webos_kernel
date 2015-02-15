/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */
/* cmn/include */
#include "basic_types.h"
/* sec/include */
#include "cmn_api_sec.h"
/* cmn */
#include "dbg_api.h"
#include "prof_api.h"
#include "mem_api.h"
/* sec/common */
#include "aes_api.h"
#include "crypto_api.h"


#define CRYPTO_BLOCKSIZE    AES_BLOCKSIZE

#define SHIFT_TO_ENC(x)        ((x) >> 16)
#define SHIFT_TO_DEC(x)        ((x) >> 16)

#  define BLOCK_ENCRYPT(type, key, ip, op)  (*blockEncrypt[type])(key, ip, op)
#  define BLOCK_DECRYPT(type, key, ip, op)  (*blockDecrypt[type])(key, ip, op)

/*
 * this breaks alignment rules in some platforms
 *
#define L_BLOCK_XOR(c,d,s) for(c=0; c<blockLen/sizeof(u_int32_t); c++){((u_int32_t*)d)[j]^=((u_int32_t*)s)[j];}
*/
#define L_BLOCK_XOR(c,d,s) for((c)=0; c<blockLen; c++){d[c]^=s[c];}

typedef void *crypto_key_t; /* an aes_handle_t */

typedef union {
    aes_handle_t    aes;
} crypto_type_key_t;

typedef struct {
    int32_t             type;
    int32_t             dir;
    int32_t             mode;
    int32_t             pad;
    u_int32_t           blockLen;
    u_int8_t            modedata[CRYPTO_BLOCKSIZE];
    u_int32_t           modedata_len;
    crypto_type_key_t   key;
} crypto_ctx_t;


typedef retcode_t (*block_func_t)(void *, const u_int8_t *, u_int8_t *);

static block_func_t blockEncrypt[] = {
    (block_func_t)0,
    &aes_BlockEncrypt,
};

static block_func_t blockDecrypt[] = {
    (block_func_t)0,
    &aes_BlockDecrypt,
};

static retcode_t
crypto_l_CheckContext(
    crypto_ctx_t        *in_crypto)
{
    retcode_t            ret = ERR_OK;
    MSGENT(("crypto_l_CheckContext"));

    /*
     * Check Cipher Type
     */
    switch ( in_crypto->type ) {
    case CRYPTO_TYPE_AES:
        MSGI3(("type : CRYPTO_TYPE_AES\n"));
        break;
    default:
        RETX(ERR_CRYPTO_STATUS,
             MSG1(("*** Unknown cipher type: [%d]\n", in_crypto->type)));
    }

    /*
     * Check Encryption Mode
     */
    switch ( in_crypto->mode ) {
    case CRYPTO_MODE_ECB:
        MSGI3(("mode : CRYPTO_MODE_ECB\n"));
        break;
    case CRYPTO_MODE_CBC:
        MSGI3(("mode : CRYPTO_MODE_CBC\n"));
        break;
    case CRYPTO_MODE_CTR:
        MSGI3(("mode : CRYPTO_MODE_CTR\n"));
        break;
    default:
        RETX(ERR_CRYPTO_STATUS,
             MSG1(("*** Unknown encryption mode: [%d]\n", in_crypto->mode)));
    }

    /*
     * Check Padding Mode
     */
    switch ( in_crypto->pad ) {
    case CRYPTO_PAD_NONE:
        MSGI3(("pad : CRYPTO_PAD_NONE\n"));
        break;
    case CRYPTO_PAD_PKCS5:
        MSGI3(("pad : CRYPTO_PAD_PKCS5\n"));
        break;
    case CRYPTO_PAD_NULL:
        MSGI3(("pad : CRYPTO_PAD_NULL\n"));
        break;
    case CRYPTO_PAD_SPACE:
        MSGI3(("pad : CRYPTO_PAD_SPACE\n"));
        break;
    default:
        RETX(ERR_CRYPTO_STATUS,
             MSG1(("*** Unknown padding mode: [%d]\n", in_crypto->pad)));
    }

EXIT:
    MSGEXT(("crypto_l_CheckContext"));
    return ret;
}

static retcode_t
crypto_l_GetPadBuf(
    crypto_ctx_t         *in_crypto,
    const u_int8_t       *in_padBuf,
    u_int32_t            *out_padLen)
{
    retcode_t            ret = ERR_OK;
    u_int32_t            blockLen = in_crypto->blockLen;
    u_int32_t            padLen = 0;
    MSGENT(("crypto_l_GetPadBuf"));

    switch ( in_crypto->pad ) {
    case CRYPTO_PAD_NONE:
        padLen = 0;
        break;
    case CRYPTO_PAD_PKCS5:
        padLen = in_padBuf[blockLen - 1];
        if ( padLen < 1 || padLen > blockLen )
            RETX(ERR_ILLEGALARGS,
                 MSG1(("*** Padding is invalid.\n")));
        break;
    case CRYPTO_PAD_NULL:
        for (padLen = 0; padLen < blockLen; padLen++)
            if ( in_padBuf[blockLen - 1 - padLen] != '\0' ) {
                break;
            }
        if ( padLen == blockLen )
            RETX(ERR_ILLEGALARGS,
                 MSG1(("*** Padding is invalid.\n")));
        break;
    case CRYPTO_PAD_SPACE:
        for (padLen = 0; padLen < blockLen; padLen++)
            if ( in_padBuf[blockLen - 1 - padLen] != ' ' ) {
                break;
            }
        if ( padLen == blockLen )
            RETX(ERR_ILLEGALARGS,
                 MSG1(("*** Padding is invalid.\n")));
        break;
    }
    *out_padLen = padLen;
    MSGI3(("Pad Len = %d [byte]\n", padLen));

EXIT:
    MSGEXT(("crypto_l_GetPadBuf"));
    return ret;
}

static retcode_t
crypto_l_SetPadBuf(
    crypto_ctx_t        *in_crypto,
    const u_int8_t      *in_plainBuf,
    u_int32_t            in_plainLen,
    u_int8_t            *out_padBuf,
    u_int32_t           *out_padLen)
{
    retcode_t            ret = ERR_OK;
    u_int32_t            blockLen = in_crypto->blockLen;
    u_int32_t            nBlocks = in_plainLen / blockLen;
    u_int32_t            remLen = in_plainLen - nBlocks * blockLen;
    char                 padChar = '\0';
    MSGENT(("crypto_l_SetPadBuf"));

    if ( remLen > 0 ) {
        mem_Copy(out_padBuf, in_plainBuf + nBlocks * blockLen, remLen);
    }

    switch ( in_crypto->pad ) {
    case CRYPTO_PAD_NONE:
        remLen = 0;    /* Cut off */
        *out_padLen = 0;
        break;
    case CRYPTO_PAD_PKCS5:
        *out_padLen = blockLen - remLen;
        padChar = (char)*out_padLen;
        break;
    case CRYPTO_PAD_NULL:
        *out_padLen = remLen > 0 ? blockLen - remLen : 0;
        padChar = '\0';
        break;
    case CRYPTO_PAD_SPACE:
        *out_padLen = remLen > 0 ? blockLen - remLen : 0;
        padChar = ' ';
        break;
    }

    if ( *out_padLen > 0 ) {
        mem_Set(out_padBuf + remLen, padChar, *out_padLen);
    }

    MSGI3(("remLen = %d, padLen = %d, padChar = [%02d]\n",
           remLen, *out_padLen, padChar));

    MSGEXT(("crypto_l_SetPadBuf"));
    return ret;
}

static retcode_t
crypto_l_EcbEncrypt(
    crypto_ctx_t        *io_crypto,
    crypto_key_t         in_key,
    const u_int8_t      *in_plainBuf,
    np_size_t            in_plainLen,
    u_int8_t            *out_cipherBuf,
    np_size_t           *io_cipherLen,
    const u_int8_t      *in_padBuf,
    np_size_t            in_padLen)
{
    retcode_t            ret = ERR_OK;
    u_int32_t            blockLen = io_crypto->blockLen;
    u_int32_t            nBlocks = in_plainLen / blockLen;
    u_int32_t            remLen = in_plainLen - nBlocks * blockLen;
    const u_int8_t      *ip = in_plainBuf;
    u_int8_t            *op = out_cipherBuf;
    u_int32_t            i;

    MSGENT(("crypto_l_EcbEncrypt"));

    /* Check Output Buffer Length */
    if ( *io_cipherLen < nBlocks * blockLen + remLen + in_padLen )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Output buffer is too short.\n")));

    /* Encrypt */
    for (i = 0; i < nBlocks; i++) {
        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, ip, op),
             ERR_CRYPTO_ENCRYPT);
        ip += blockLen;
        op += blockLen;
    }

    /* Encrypt Last Block */
    if ( in_padLen > 0 ) {
        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, in_padBuf, op),
             ERR_CRYPTO_ENCRYPT);
    }

    /* Set Outputted Data Length */
    *io_cipherLen = nBlocks * blockLen + remLen + in_padLen;

    MSGI3(("Block Num = %d\n", nBlocks));
    MSGI3(("IN : Plain Len = %d [byte]\n", in_plainLen));
    MSGI3(("OUT: Cipher Len = %d [byte]\n", *io_cipherLen));

EXIT:
    MSGEXT(("crypto_l_EcbEncrypt"));
    return ret;
}


static retcode_t
crypto_l_EcbDecrypt(
    crypto_ctx_t    *io_crypto,
    crypto_key_t     in_key,
    bool_t           in_is_final,
    const u_int8_t  *in_cipherBuf,
    np_size_t        in_cipherLen,
    u_int8_t        *out_plainBuf,
    np_size_t       *io_plainLen)
{
    retcode_t            ret = ERR_OK;
    u_int32_t            blockLen = io_crypto->blockLen;
    u_int32_t            nBlocks = in_cipherLen / blockLen;
    u_int8_t             padBuf[CRYPTO_BLOCKSIZE];
    u_int32_t            padLen = 0;
    u_int32_t            remLen = 0;
    const u_int8_t      *ip = in_cipherBuf;
    u_int8_t            *op = out_plainBuf;
    u_int32_t            i;

    MSGENT(("crypto_l_EcbDecrypt"));

    /* Check Input Buffer Length */
    if ( in_cipherLen % blockLen != 0 )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Input buffer lenght is strange: [0x%x]\n", in_cipherLen)));

    /* Check Output Buffer Length (without last block) */
    if ( *io_plainLen < (nBlocks - 1) * blockLen )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Output buffer is too short.\n")));

    /* Decrypt until Last Block */
    for (i = 0; i < nBlocks - 1; i++) {
        RETR(BLOCK_DECRYPT(io_crypto->type, in_key, ip, op),
             ERR_CRYPTO_DECRYPT);
        ip += blockLen;
        op += blockLen;
    }

    /* Decrypt Last Block */
    RETR(BLOCK_DECRYPT(io_crypto->type, in_key, ip, padBuf),
         ERR_CRYPTO_DECRYPT);

    /* Get Plain Length of Last Block */
    if ( in_is_final ) {
        RETR(crypto_l_GetPadBuf(io_crypto, padBuf, &padLen),
             ERR_CRYPTO_DECRYPT);
    }

    /* Check Output Buffer Length */
    if ( (remLen = blockLen - padLen) > 0 ) {
        if ( *io_plainLen < (nBlocks - 1) * blockLen + remLen ) {
            RETX(ERR_ILLEGALARGS,
                 MSG1(("*** Output buffer is too short.\n")));
        }
        mem_Copy(op, padBuf, remLen);
    }

    /* Set Outputted Data Length */
    *io_plainLen = (nBlocks - 1) * blockLen + remLen;

    MSGI3(("Block Num = %d\n", nBlocks));
    MSGI3(("IN : Cipher Len = %d [byte]\n", in_cipherLen));
    MSGI3(("OUT: Plain Len = %d [byte]\n", *io_plainLen));

EXIT:
    MSGEXT(("crypto_l_EcbDecrypt"));
    return ret;
}

static retcode_t
crypto_l_CbcEncrypt(
    crypto_ctx_t        *io_crypto,
    crypto_key_t         in_key,
    const u_int8_t      *in_plainBuf,
    np_size_t            in_plainLen,
    u_int8_t            *out_cipherBuf,
    np_size_t           *io_cipherLen,
    const u_int8_t      *in_padBuf,
    np_size_t            in_padLen)
{
    retcode_t            ret = ERR_OK;
    u_int8_t             blockBuf[CRYPTO_BLOCKSIZE];
    u_int32_t            blockLen = io_crypto->blockLen;
    u_int32_t            nBlocks = in_plainLen / blockLen;
    u_int32_t            remLen = in_plainLen - nBlocks * blockLen;
    const u_int8_t      *ip = in_plainBuf;
    u_int8_t            *op = out_cipherBuf;
    u_int32_t            i,j;
    MSGENT(("crypto_l_CbcEncrypt"));

    /* Check Output Buffer Length */
    if ( *io_cipherLen < nBlocks * blockLen + remLen + in_padLen )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Output buffer is too short.\n")));

    /* Initialize IV Buffer */
    mem_Copy(blockBuf, io_crypto->modedata, blockLen);

    /* Encrypt */
    for (i = 0; i < nBlocks; i++) {
        L_BLOCK_XOR(j, blockBuf, ip);

        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, blockBuf, op),
             ERR_CRYPTO_ENCRYPT);

        mem_Copy(blockBuf, op, blockLen);
        ip += blockLen;
        op += blockLen;
    }

    /* Encrypt Last Block */
    if ( in_padLen > 0 ) {
        L_BLOCK_XOR(j, blockBuf, in_padBuf);

        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, blockBuf, op),
             ERR_CRYPTO_ENCRYPT);
    }

    /* Set Outputted Data Length */
    *io_cipherLen = nBlocks * blockLen + remLen + in_padLen;

    MSGI3(("Block Num = %d\n", nBlocks));
    MSGI3(("IN : Plain Len = %d [byte]\n", in_plainLen));
    MSGI3(("OUT: Cipher Len = %d [byte]\n", *io_cipherLen));

EXIT:
    MSGEXT(("crypto_l_CbcEncrypt"));
    return ret;
}


static retcode_t
crypto_l_CbcDecrypt(
    crypto_ctx_t        *io_crypto,
    crypto_key_t         in_key,
    bool_t               in_is_final,
    const u_int8_t      *in_cipherBuf,
    np_size_t            in_cipherLen,
    u_int8_t            *out_plainBuf,
    np_size_t           *io_plainLen)
{
    retcode_t            ret = ERR_OK;
    u_int32_t            blockLen = io_crypto->blockLen;
    u_int32_t            nBlocks = in_cipherLen / blockLen;
    u_int8_t             padBuf[CRYPTO_BLOCKSIZE];
    u_int8_t             blockBuf[CRYPTO_BLOCKSIZE];
    u_int32_t            padLen = 0;
    u_int32_t            remLen = 0;
    const u_int8_t      *ip = in_cipherBuf;
    u_int8_t            *op = out_plainBuf;
    u_int32_t            i,j;

    MSGENT(("crypto_l_CbcDecrypt"));

    /* Check Input Buffer Length */
    if ( in_cipherLen % blockLen != 0 )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Input buffer lenght is strange: [0x%x]\n", in_cipherLen)));

    /* Check Output Buffer Length (without last block) */
    if ( *io_plainLen < (nBlocks - 1) * blockLen )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Output buffer is too short.\n")));

    /* Initialize IV Buffer */
    mem_Copy(blockBuf, io_crypto->modedata, blockLen);

    /* Decrypt */
    for (i = 0; i < nBlocks - 1; i++) {
        u_int8_t             tempBuf[CRYPTO_BLOCKSIZE];

        mem_Copy(tempBuf, ip, blockLen);    /* save inpuyt block, since .. */

        RETR(BLOCK_DECRYPT(io_crypto->type, in_key, ip, op),   /* .. it is overwritten here when ip is equal to op (in-place decryption) */
             ERR_CRYPTO_DECRYPT);

        L_BLOCK_XOR(j, op, blockBuf);

        mem_Copy(blockBuf, tempBuf, blockLen);
        ip += blockLen;
        op += blockLen;
    }

    /* Decrypt Last Block */
    RETR(BLOCK_DECRYPT(io_crypto->type, in_key, ip, padBuf),
         ERR_CRYPTO_DECRYPT);

    L_BLOCK_XOR(j, padBuf, blockBuf);

    MSGI3(("decryped data(last):\n"));
    DUMP3((blockLen, padBuf));
    MSGI3(("--\n"));

    /* Get Plain Length of Last Block */
    if ( in_is_final ) {
        RETR(crypto_l_GetPadBuf(io_crypto, padBuf, &padLen),
             ERR_CRYPTO_DECRYPT);
    }

    /* Check Output Buffer Length */
    if ( (remLen = blockLen - padLen) > 0 ) {
        if ( *io_plainLen < (nBlocks - 1) * blockLen + remLen ) {
            RETX(ERR_ILLEGALARGS,
                 MSG1(("*** Output buffer is too short.\n")));
        }
        mem_Copy(op, padBuf, remLen);
    }

    /* Set Outputted Data Length */
    *io_plainLen = (nBlocks - 1) * blockLen + remLen;

    MSGI3(("Block Num = %d\n", nBlocks));
    MSGI3(("IN : Cipher Len = %d [byte]\n", in_cipherLen));
    MSGI3(("OUT: Plain Len = %d [byte]\n", *io_plainLen));

EXIT:
    MSGEXT(("crypto_l_CbcDecrypt"));
    return ret;
}

static retcode_t
crypto_l_IncrementCounter(u_int8_t *io_counter)
{
    int32_t i = 0;

    MSGENT(("crypto_l_IncrementCounter"));

    for ( i = CRYPTO_BLOCKSIZE - 1; i >= 0; --i ) {
        if ( 0 != ++(io_counter[i]) ) {
            break;
        }
    }

    MSGEXT_V(("crypto_l_IncrementCounter"));
    return ERR_OK;
}


static retcode_t
crypto_l_CtrEncrypt(
    crypto_ctx_t        *io_crypto,
    crypto_key_t         in_key,
    const u_int8_t      *in_plainBuf,
    np_size_t            in_plainLen,
    u_int8_t            *out_cipherBuf,
    np_size_t           *io_cipherLen,
    const u_int8_t      *in_padBuf,
    np_size_t            in_padLen)
{
    retcode_t            ret = ERR_OK;
    u_int8_t             counterBuf[CRYPTO_BLOCKSIZE];
    u_int8_t             keyStreamBuf[CRYPTO_BLOCKSIZE];
    u_int32_t            blockLen = io_crypto->blockLen;
    u_int32_t            nBlocks = in_plainLen / blockLen;
    u_int32_t            remLen = in_plainLen - nBlocks * blockLen;
    const u_int8_t      *ip = in_plainBuf;
    u_int8_t            *op = out_cipherBuf;
    u_int32_t            keyStreamOffset = io_crypto->modedata[io_crypto->modedata_len - 1] & 0x0F;
    u_int32_t            payloadId = 0;
    u_int32_t            i, j;

    MSGENT(("crypto_l_CtrEncrypt"));

    /* Check Output Buffer Length */
    if ( *io_cipherLen < in_plainLen )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Output buffer is too short.\n")));

    MSGI3(("keyStreamOffset = %d\n", keyStreamOffset));

    /* Initialize Counter */
    mem_Copy( counterBuf, io_crypto->modedata, sizeof counterBuf );

    /* Encrypt First Block */
    if ( nBlocks > 0 ) {
        MSGI3(("Enc Counter: [0]\n"));
        DUMP3((CRYPTO_BLOCKSIZE, counterBuf));

        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
             ERR_CRYPTO_ENCRYPT);

        if (in_plainLen >= (blockLen - keyStreamOffset)) {
            for (j = keyStreamOffset; j < blockLen; j++, payloadId++) {
                op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
            }
        } else {
            for (j = keyStreamOffset; j < keyStreamOffset+in_plainLen; j++, payloadId++) {
                op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
            }
        }
    }

    /* Encrypt Blocks */
    for (i = 1; i < nBlocks; i++) {
        /* Increment Counter */
        RET(crypto_l_IncrementCounter(counterBuf));

        MSGI3(("Enc Counter: [%d]\n", i));
        DUMP3((CRYPTO_BLOCKSIZE, counterBuf));

        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
             ERR_CRYPTO_ENCRYPT);

        for (j = 0; j < blockLen; j++, payloadId++) {
            op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
        }
    }

    if ((nBlocks > 0) && ( keyStreamOffset > 0 )) {

        /* Increment Counter */
        RET(crypto_l_IncrementCounter(counterBuf));

        MSGI3(("Enc Counter: [%d]\n", i));
        DUMP3((CRYPTO_BLOCKSIZE, counterBuf));

        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
             ERR_CRYPTO_ENCRYPT);

        for (j = 0; j < keyStreamOffset; j++, payloadId++) {
            op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
        }
    }

    /* Encrypt Last Block */
    if ( remLen > 0 ) {
        if ( nBlocks == 0 ) {
            RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
                 ERR_CRYPTO_ENCRYPT);
        }

        if ( remLen < blockLen - keyStreamOffset ) {
            for (j = 0; j < remLen; j++, payloadId++) {
                op[payloadId] = in_padBuf[j] ^ keyStreamBuf[j + keyStreamOffset];
            }
        } else {
            for (j = 0; j < blockLen - keyStreamOffset; j++, payloadId++) {
                op[payloadId] = in_padBuf[j] ^ keyStreamBuf[j + keyStreamOffset];
            }

            /* Increment Counter */
            RET(crypto_l_IncrementCounter(counterBuf));

            MSGI3(("Enc Counter: [%d]\n", i+1));
            DUMP3((CRYPTO_BLOCKSIZE, counterBuf));

            RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
                 ERR_CRYPTO_ENCRYPT);

            for (j = blockLen - keyStreamOffset; j < remLen; j++, payloadId++) {
                op[payloadId] = in_padBuf[j] ^ keyStreamBuf[j - (blockLen - keyStreamOffset)];
            }
        }
    }

    /* Set the context to have the next counter value for further rounds */
    RET(crypto_l_IncrementCounter(counterBuf));
    mem_Copy( io_crypto->modedata, counterBuf, io_crypto->modedata_len );

    /* Set Outputted Data Length */
    *io_cipherLen = in_plainLen;

    MSGI3(("Block Num = %d\n", nBlocks));
    MSGI3(("IN : Cipher Len = %d [byte]\n", in_plainLen));
    MSGI3(("OUT: Plain Len = %d [byte]\n", *io_cipherLen));

EXIT:
    MSGEXT(("crypto_l_CtrEncrypt"));
    return ret;
}


static retcode_t
crypto_l_CtrDecrypt(
    crypto_ctx_t    *io_crypto,
    crypto_key_t     in_key,
    bool_t           in_is_final,
    const u_int8_t  *in_cipherBuf,
    np_size_t        in_cipherLen,
    u_int8_t        *out_plainBuf,
    np_size_t       *io_plainLen)
{
    retcode_t            ret = ERR_OK;
    u_int8_t             counterBuf[CRYPTO_BLOCKSIZE];
    u_int8_t             keyStreamBuf[CRYPTO_BLOCKSIZE];
    u_int32_t            blockLen = io_crypto->blockLen;
    const u_int8_t      *ip = in_cipherBuf;
    u_int8_t            *op = out_plainBuf;
    u_int32_t            payloadId = 0;
    u_int32_t            nBlocks   = 0;
    u_int32_t            remLen    = 0;
    u_int32_t            i, j;

    MSGENT(("crypto_l_CtrDecrypt"));

    /* Check Output Buffer Length */
    if ( *io_plainLen < in_cipherLen )
        RETX(ERR_ILLEGALARGS,
             MSG1(("*** Output buffer is too short.\n")));

    if (in_cipherLen >= blockLen) {
        nBlocks = (in_cipherLen - blockLen) / blockLen + 1;
        remLen = in_cipherLen - nBlocks * blockLen;
    }

    MSGI3(("nBlocks = %d\n", nBlocks));

    /* Initialize Counter */
    mem_Copy( counterBuf, io_crypto->modedata, sizeof counterBuf );

    /* Encrypt First Block */
    MSGI3(("Dec Counter: [0]\n"));
    DUMP3((CRYPTO_BLOCKSIZE, counterBuf));

    RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
         ERR_CRYPTO_ENCRYPT);

    if (in_cipherLen >= blockLen) {
        for (j = 0; j < blockLen; j++, payloadId++) {
            op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
        }
    } else {
        for (j = 0; j < in_cipherLen; j++, payloadId++) {
            op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
        }
    }

    /* Encrypt Blocks */
    for (i = 1; i < nBlocks; i++) {
        /* Increment Counter */
        RET(crypto_l_IncrementCounter(counterBuf));

        MSGI3(("Dec Counter: [%d]\n", i));
        DUMP3((CRYPTO_BLOCKSIZE, counterBuf));

        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
             ERR_CRYPTO_ENCRYPT);

        for (j = 0; j < blockLen; j++, payloadId++) {
            op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
        }
    }

    /* Encrypt last block (remainder) */
    if ( remLen > 0 ) {
        /* Increment Counter */
        RET(crypto_l_IncrementCounter(counterBuf));

        MSGI3(("Dec Counter: [%d]\n", i));
        DUMP3((CRYPTO_BLOCKSIZE, counterBuf));

        RETR(BLOCK_ENCRYPT(io_crypto->type, in_key, counterBuf, keyStreamBuf),
             ERR_CRYPTO_ENCRYPT);

        for (j = 0; j < remLen; j++, payloadId++) {
            op[payloadId] = ip[payloadId] ^ keyStreamBuf[j];
        }
    }

    /* Set the context to have the next counter value for further rounds */
    RET(crypto_l_IncrementCounter(counterBuf));
    mem_Copy( io_crypto->modedata, counterBuf, io_crypto->modedata_len );

    /* Set Outputted Data Length */
    *io_plainLen = in_cipherLen;

    MSGI3(("Block Num = %d\n", nBlocks));
    MSGI3(("IN : Cipher Len = %d [byte]\n", in_cipherLen));
    MSGI3(("OUT: Plain Len = %d [byte]\n", *io_plainLen));

EXIT:
    MSGEXT(("crypto_l_CtrDecrypt"));
    return ret;
}

typedef retcode_t(*mode_encrypt_t)(
    crypto_ctx_t *, crypto_key_t,
    const u_int8_t *, np_size_t,
    u_int8_t *, np_size_t *,
    const u_int8_t *, np_size_t);

static mode_encrypt_t modeEncrypt[] = {
    (mode_encrypt_t)0,
    &crypto_l_EcbEncrypt,
    &crypto_l_CbcEncrypt,
    &crypto_l_CtrEncrypt,
};

typedef retcode_t (*mode_decrypt_t)(
    crypto_ctx_t *, crypto_key_t, bool_t,
    const u_int8_t *, np_size_t,
    u_int8_t *, np_size_t *);

static mode_decrypt_t modeDecrypt[] = {
    (mode_decrypt_t)0,
    &crypto_l_EcbDecrypt,
    &crypto_l_CbcDecrypt,
    &crypto_l_CtrDecrypt,
};

/**
 * @brief Given the crypto context, choose the initialization method
 * from aes and call it.
 */
static retcode_t
l_Initialize(crypto_ctx_t *io_crypto, const u_int8_t *in_keyBuf, u_int32_t in_keyLen)
{
    retcode_t ret = ERR_OK;
    crypto_key_t key = NULL;
    u_int32_t enc_dir;

    MSGENT(("l_Initialize"));

    if (io_crypto == NULL) {
        RET(ERR_ILLEGALARGS);
    }

    if ( io_crypto->type == CRYPTO_TYPE_AES ) {
        if ( (io_crypto->dir == CRYPTO_DIR_ENC) || (io_crypto->dir == CRYPTO_DIR_CTR) ) {
            enc_dir = AES_ENCRYPT;
        } else {
            enc_dir = AES_DECRYPT;
        }

        RETR(aes_Init(&key, enc_dir, in_keyLen, in_keyBuf), CONVR(ERR_CRYPTO_KEY));
        io_crypto->key.aes = key;
    } else {
        RET(ERR_CRYPTO_STATUS);
    }

EXIT:
    if ( ret != ERR_OK && key != NULL ) {
        switch ( io_crypto->type ) {
        case CRYPTO_TYPE_AES:
            aes_Fin(key);
            break;
        default:
            break;
        }
    }
    MSGEXT(("l_Initialize"));
    return ret;
}

retcode_t
crypto_Init(
    crypto_handle_t     *io_crypto,
    u_int32_t            in_type,
    const u_int8_t      *in_modedata,
    u_int32_t            in_modedata_len,
    const u_int8_t      *in_keyBuf,
    u_int32_t            in_keyLen)
{
    retcode_t            ret = ERR_OK;
    crypto_ctx_t        *crypto = NULL;
    int32_t              type, dir, mode, pad;
    MSGENT(("crypto_Init"));

    /* Clear Crypto Handle */
    *io_crypto = NULL;

    /* Allocate Crypto Context */
    if ( (crypto = mem_Alloc(sizeof(crypto_ctx_t))) == NULL )
        RETX(ERR_NOMEM,
             MSG1(("*** mem_Alloc: Failed ...\n")));

    mem_Set(crypto, 0, sizeof(crypto_ctx_t));

    /* Check Cipher Type */
    switch ( type = CRYPTO_TYPE_BASE & in_type ) {
    case CRYPTO_TYPE_AES:
        crypto->type = type;
        crypto->blockLen = AES_BLOCKSIZE;
        MSGI3(("type : CRYPTO_TYPE_AES\n"));
        break;
    default:
        RETX(ERR_CRYPTO_STATUS,
             MSG1(("*** Unknown cipher type: [%d]\n", type)));
    }

    /* Check Encryption Direction */
    switch ( dir = CRYPTO_DIR_BASE & in_type ) {
    case CRYPTO_DIR_ENC:
        crypto->dir = dir;
        MSGI3(("dir : CRYPTO_DIR_ENC\n"));
        break;
    case CRYPTO_DIR_DEC:
        crypto->dir = dir;
        MSGI3(("dir : CRYPTO_DIR_DEC\n"));
        break;
    case CRYPTO_DIR_CTR:
        crypto->dir = dir;
        MSGI3(("dir : CRYPTO_DIR_CTR\n"));
        break;
    default:
        RETX(ERR_CRYPTO_STATUS,
             MSG1(("*** Unknown cipher dir: [%d]\n", dir)));
    }

    /* Check Encryption Mode */
    switch ( mode = CRYPTO_MODE_BASE & in_type ) {
    case CRYPTO_MODE_ECB:
        crypto->mode = mode;
        MSGI3(("mode : CRYPTO_MODE_ECB\n"));
        break;
    case CRYPTO_MODE_CBC:
        crypto->mode = mode;
        MSGI3(("mode : CRYPTO_MODE_CBC\n"));
        break;
    case CRYPTO_MODE_CTR:
        crypto->mode = mode;
        if ( crypto->dir != CRYPTO_DIR_CTR )
            RETX(ERR_CRYPTO_STATUS,
                 MSG1(("*** Unknown encryption mode: [%d]\n", mode)));
        MSGI3(("mode : CRYPTO_MODE_CTR\n"));
        break;
    default:
        RETX(ERR_CRYPTO_STATUS,
             MSG1(("*** Unknown encryption mode: [%d]\n", mode)));
    }

    /* Check Padding Mode */
    switch ( pad = CRYPTO_PAD_BASE & in_type ) {
    case CRYPTO_PAD_NONE:
        MSGI3(("pad : CRYPTO_PAD_NONE\n"));
        crypto->pad = pad;
        break;
    case CRYPTO_PAD_PKCS5:
        MSGI3(("pad : CRYPTO_PAD_PKCS5\n"));
        crypto->pad = pad;
        break;
    case CRYPTO_PAD_NULL:
        MSGI3(("pad : CRYPTO_PAD_NULL\n"));
        crypto->pad = pad;
        break;
    case CRYPTO_PAD_SPACE:
        MSGI3(("pad : CRYPTO_PAD_SPACE\n"));
        crypto->pad = pad;
        break;
    default:
        RETX(ERR_CRYPTO_STATUS,
             MSG1(("*** Unknown padding pad: [%d]\n", pad)));
    }

    /* Set IV (Initial Vector) for CBC or counter for CTR modes */
    if ( in_modedata ) {
        if ( in_modedata_len > CRYPTO_BLOCKSIZE) {
            RETX(ERR_ILLEGALARGS, MSG1(("*** Illegal Args: Failed ...\n")));
        }
        mem_Copy(crypto->modedata, in_modedata, in_modedata_len);
        crypto->modedata_len = in_modedata_len;
    } else {
        mem_Set(crypto->modedata, 0, sizeof crypto->modedata);
        crypto->modedata_len = 0;
    }

    RET(l_Initialize(crypto, in_keyBuf, in_keyLen));

    /* Set Crypto Handle */
    *io_crypto = (crypto_handle_t)crypto;

EXIT:
    if ( *io_crypto == NULL ) {
        mem_Free(crypto);
    }

    MSGEXT(("crypto_Init"));
    return ret;
}

retcode_t
crypto_Fin(
    crypto_handle_t        io_crypto)
{
    retcode_t            ret = ERR_OK;
    crypto_ctx_t        *crypto = (crypto_ctx_t *)io_crypto;
    MSGENT(("crypto_Fin"));

    if ( crypto ) {
        switch ( crypto->type ) {
        case CRYPTO_TYPE_AES:
            aes_Fin(crypto->key.aes);
            break;
        }
        mem_Free(crypto);
    }

    MSGEXT(("crypto_Fin"));
    return ret;
}

retcode_t
crypto_Encrypt(
    crypto_handle_t      io_crypto,
    bool_t               in_is_final,
    const u_int8_t      *in_plainBuf,
    np_size_t            in_plainLen,
    u_int8_t            *out_cipherBuf,
    np_size_t           *io_cipherLen)
{
    retcode_t            ret = ERR_OK;
    crypto_ctx_t        *crypto = (crypto_ctx_t *)io_crypto;
    u_int8_t             padBuf[CRYPTO_BLOCKSIZE];
    u_int32_t            padLen = 0;
    crypto_key_t         key = NULL;
    MSGENT(("crypto_Encrypt"));

    key = crypto->key.aes;

    /* Check Context Information */
    RET(crypto_l_CheckContext(crypto));

    /* Make Padding Buffer for Last Block */
    if (in_is_final) {
        RET(crypto_l_SetPadBuf(crypto, in_plainBuf, in_plainLen, padBuf, &padLen));
    }

    /* Encrypt */
    RET((*modeEncrypt[SHIFT_TO_ENC(crypto->mode)])(crypto, key,
                                                   in_plainBuf, in_plainLen,
                                                   out_cipherBuf, io_cipherLen,
                                                   padBuf, padLen));

EXIT:
    MSGEXT(("crypto_Encrypt"));
    return ret;
}

retcode_t
crypto_Decrypt(
    crypto_handle_t  io_crypto,
    bool_t           in_is_final,
    const u_int8_t  *in_cipherBuf,
    np_size_t        in_cipherLen,
    u_int8_t        *out_plainBuf,
    np_size_t       *io_plainLen)
{
    retcode_t            ret = ERR_OK;
    crypto_ctx_t        *crypto = (crypto_ctx_t *)io_crypto;
    crypto_key_t         key = NULL;
    MSGENT(("crypto_Decrypt"));

    key = crypto->key.aes;

    /* Check Context Information */
    RET(crypto_l_CheckContext(crypto));

    /* Decrypt */
    RET((*modeDecrypt[SHIFT_TO_DEC(crypto->mode)])(crypto,
                                                   key,
                                                   in_is_final,
                                                   in_cipherBuf,
                                                   in_cipherLen,
                                                   out_plainBuf,
                                                   io_plainLen));

EXIT:
    MSGEXT(("crypto_Decrypt"));
    return ret;
}

retcode_t
crypto_GetAes128Counter(
    crypto_handle_t  in_crypto,
    u_int32_t        in_buflen,
    u_int8_t        *out_buf)
{
    retcode_t ret = ERR_OK;
    crypto_ctx_t *crypto = in_crypto;

    MSGENT(("crypto_GetAes128Counter"));

    if ( NULL == crypto                  ||
         CRYPTO_MODE_CTR != crypto->mode ||
         CRYPTO_TYPE_AES != crypto->type  ) {
        RET( ERR_ILLEGALHANDLE );
    }

    if ( CRYPTO_BLOCKSIZE != in_buflen || NULL == out_buf ) {
        RET( ERR_ILLEGALARGS );
    }

    mem_Copy( out_buf, crypto->modedata, in_buflen );

EXIT:
    MSGEXT(("crypto_GetAes128Counter"));
    return ret;
}

retcode_t
crypto_EncryptAES128CBC(
    const u_int8_t *in_ivBuf,
    const u_int8_t *in_keyBuf,
    const u_int8_t *in_plainBuf,
    np_size_t       in_plainLen,
    u_int8_t       *out_cipherBuf,
    np_size_t      *io_cipherLen)
{
    retcode_t ret;
    crypto_handle_t handle = (crypto_handle_t)0;

    PROF_TIMEINIT();
    MSGENT(("crypto_EncryptAES128CBC"));
    PROF_TIMESTART("crypto_EncryptAES128CBC");
    RET( crypto_Init(&handle,
                     CRYPTO_TYPE_AES|CRYPTO_DIR_ENC|CRYPTO_MODE_CBC|CRYPTO_PAD_PKCS5,
                     in_ivBuf, 16, in_keyBuf, 16) );
    RET( crypto_Encrypt(handle, TRUE, in_plainBuf, in_plainLen, out_cipherBuf, io_cipherLen) );
EXIT:
    crypto_Fin(handle);
    PROF_TIMESTOP();
    PROF_TIMEPRINT();
    PROF_TIMEFIN();
    MSGEXT(("crypto_EncryptAES128CBC"));
    return ret;
}

retcode_t
crypto_DecryptAES128CBC(
    const u_int8_t *in_ivBuf,
    const u_int8_t *in_keyBuf,
    const u_int8_t *in_cipherBuf,
    np_size_t       in_cipherLen,
    u_int8_t       *out_plainBuf,
    np_size_t      *io_plainLen)
{
    retcode_t ret;
    crypto_handle_t handle = (crypto_handle_t)0;

    PROF_TIMEINIT();
    MSGENT(("crypto_DecryptAES128CBC"));
    PROF_TIMESTART("crypto_DecryptAES128CBC");
    RET( crypto_Init(&handle,
                     CRYPTO_TYPE_AES|CRYPTO_DIR_DEC|CRYPTO_MODE_CBC|CRYPTO_PAD_PKCS5,
                     in_ivBuf, 16, in_keyBuf, 16) );
    RET( crypto_Decrypt(handle, TRUE, in_cipherBuf, in_cipherLen, out_plainBuf, io_plainLen) );
EXIT:
    crypto_Fin(handle);
    PROF_TIMESTOP();
    PROF_TIMEPRINT();
    PROF_TIMEFIN();
    MSGEXT(("crypto_DecryptAES128CBC"));
    return ret;
}

retcode_t
crypto_DecryptAES128(
    const u_int8_t *in_keyBuf,
    const u_int8_t *in_cipherBuf,
    u_int8_t       *out_plainBuf)
{
    retcode_t ret;
    crypto_handle_t handle = (crypto_handle_t)0;
    np_size_t len = 16;

    MSGENT(("crypto_DecryptAES128CBC"));
    RET( crypto_Init(&handle,
                     CRYPTO_TYPE_AES|CRYPTO_DIR_DEC|CRYPTO_MODE_ECB|CRYPTO_PAD_NONE,
                     NULL, 16, in_keyBuf, 16) );
    RET( crypto_Decrypt(handle, TRUE, in_cipherBuf, 16, out_plainBuf, &len) );
EXIT:
    crypto_Fin(handle);
    MSGEXT(("crypto_DecryptAES128CBC"));
    return ret;
}
