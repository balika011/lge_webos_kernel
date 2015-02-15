/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __PLAYREADY_CRYPTO_H__
#define _PLAYREADY_CRYPTO_H__



/*
** Function Protos
*/


#define ECC_P256_INTEGER_SIZE_IN_BYTES     ( 32 )

UINT32 CRYPTO_ECC_GenKeyPair_P256( 
    UINT8  *f_pPubKey,
    UINT8  *f_pPrivKey);

UINT32 CRYPTO_ECC_GenKeyPairRestrictedPriv_P256( 
    UINT8  *f_pPubKey,
    UINT8  *f_pPrivKey);

UINT32 CRYPTO_ECDSA_Sign_P256(
    const  UINT8          f_rgbMessage[],
    const  UINT32        f_cbMessageLen,
    const  UINT8    	    *f_pPrivkey,
    UINT8                   *f_pSignature);

#endif 

