/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011,2012 Sony Corporation.
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



/* Example of obfuscating a key. It could be extended or replace by a
 * porter to another platform. For example the key could be encrypted
 * with a device key (scramble) and decrypted with the device key
 * (unscramble) before being used
 */


/* LOCAL FUNCTIONS */

/* pseudo-rand function */
static uint32_t
l_Rand(uint32_t  in_seed)
{
    return ((uint32_t)in_seed * 279470273UL) % 4294967291UL;
}

extern void
hci_l_DataScrub(
    uint8_t          *io_buf,
    uint32_t          in_buf_len)
{
    uint32_t  val = (size_t) io_buf;
    int        idx = 0;

    if (NULL != io_buf) {
        for (idx = 0; idx < in_buf_len; ++idx) {
            val = l_Rand(val);
            io_buf[idx] = (uint8_t)val;
        }
    }
}

extern uint32_t
hci_l_FreeKey(binstr_t *io_key)
{
    uint32_t  ret = ERR_HCI_OK;

    if (io_key != NULL) {
        hci_l_DataScrub(io_key->buf, io_key->len);
        hci_l_mem_binstrFree(io_key);
    }

    return ret;
}

typedef unsigned char u;
static const u scrd[11] = {0xf9,0xa3,0x97,0xa2,0x55,0xbe,0xf1,0x53,0xfc,0x6b,0x79};
#define SS data
#define SSS %013
#define S0(_) case (_):(--n)[SS]^=S[scrd];S=n[SS]SSS;
#define S1(_) S0((_)+1)S0(_)
#define S2(_) S1((_)+2)S1(_)
#define S4(_) S2((_)+4)S2(_)
#define DO_SCR(_) u S=0;switch((n-1)SSS){while(n>0){S4(3)S1(1)S0(0)}}


#define S00(_) case (_):_S=(--n)[SS]SSS;n[SS]^=S[scrd];S=_S;
#define S01(_) S00((_)+1)S00(_)
#define S02(_) S01((_)+2)S01(_)
#define S04(_) S02((_)+4)S02(_)
#define DO_USCR(_) u S=0,_S;switch((n-1)SSS){while(n>0){S04(3)S01(1)S00(0)}}

static void
l_obfus_scramble(u *data,uint32_t  n)
{
    DO_SCR(scrd);
}

static void
l_obfus_uscramble(u *data,uint32_t  n)
{
    DO_USCR(scrd);
}

extern uint32_t
hci_l_Scramble(binstr_t *io_key)
{
    uint32_t  ret = ERR_HCI_OK;
    MSGENT(("hci_l_Scramble"));

    if (io_key == NULL) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    l_obfus_scramble(io_key->buf, io_key->len);

EXIT:
    MSGEXT(("hci_l_Scramble"));
    return ret;
}

extern uint32_t
hci_l_Unscramble(binstr_t  *in_key,
                 binstr_t **out_key)
{
    uint32_t  ret = ERR_HCI_OK;
    binstr_t *tmp = NULL;

    MSGENT(("hci_l_Unscramble"));

    if ( NULL == out_key ) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    if ( NULL == in_key ) {
        RET(ERR_HCI_KEYNOTINREF);
    }

    RET( hci_l_mem_BinstrAllocWithCopy(binstr_Len(in_key),
                                       binstr_Buf(in_key),
                                       &tmp) );

    l_obfus_uscramble(tmp->buf, tmp->len);

    *out_key = tmp; tmp = NULL;

EXIT:
    hci_l_mem_binstrFree(tmp);

    MSGEXT(("hci_l_Unscramble"));
    return ret;
}

#if !defined(CC_ENABLE_NAUTILUS_ATK) && defined(CC_ENABLE_NAUTILUS_NEW_KEYFORMAT)
static const u scrd2[11] = {0xf8,0xce,0x57,0x8f,0x65,0xe8,0xd9,0x31,0x2b,0x3b,0x6b};
#define S00_(_) case (_):_S=(--n)[SS]SSS;n[SS]^=S[scrd2];S=_S;
#define S01_(_) S00_((_)+1)S00_(_)
#define S02_(_) S01_((_)+2)S01_(_)
#define S04_(_) S02_((_)+4)S02_(_)
#define DO_USCR_(_) u S=0,_S;switch((n-1)SSS){while(n>0){S04_(3)S01_(1)S00_(0)}}

static void
l_obfus_uscramble2(u *data,uint32_t  n)
{
    DO_USCR_(scrd2);
}

extern uint32_t
hci_l_Unscramble2(binstr_t  *io_key)
{
    uint32_t  ret = ERR_HCI_OK;
    MSGENT(("hci_l_Unscramble2"));

    if ( NULL == io_key ) {
        RET(ERR_HCI_KEYNOTINREF);
    }

    l_obfus_uscramble2(io_key->buf, io_key->len);

EXIT:
    MSGEXT(("hci_l_Unscramble2"));
    return ret;
}
#endif
