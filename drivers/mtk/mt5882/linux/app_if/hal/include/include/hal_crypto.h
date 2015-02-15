/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file hal_crypto.h
 *
 * This source file defines the HAL functions related to crypto driver control
 *
 *  @author	Park Seonho (seonho.park@lge.com)
 *  @version	0.1
 *  @date		2010.08.09
 *  @note		additional information.
 *  @see		hal_crypto.h
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	_HAL_CRYPTO_H_
#define	_HAL_CRYPTO_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#ifndef WIN32_SIM
#include "hal_common.h"
#else
#include "common_win32.h"
#endif

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
#define HAL_CRYPTO_ARG_MAX 16

typedef struct _HAL_CRYPTO_ARG {
    UINT32 command;                          /* Command */
    UINT8  argc;                             /* Number of arguments */
    UINT8 *argv[HAL_CRYPTO_ARG_MAX]; /* Array of argument pointer */
    UINT32 argvLen[HAL_CRYPTO_ARG_MAX];   /* Array of argument length */
    BOOLEAN   out[HAL_CRYPTO_ARG_MAX];     /* Array of boolean value for out/in */
} HAL_CRYPTO_ARG_T;

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
   로컬 상수 정의 (Local Constant Definitions)
******************************************************************************/

/******************************************************************************
    로컬 형 정의 (Local Type Definitions)
******************************************************************************/

/******************************************************************************
    전역 형 정의 (Global Type Definitions)
******************************************************************************/

/******************************************************************************
    Extern 전역변수와 함수 prototype 선언
    (Extern Variables & Function Prototype Declarations)
******************************************************************************/

/******************************************************************************
    Static 변수와 함수 prototype 선언
    (Static Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    로컬 변수와 함수 prototype 선언
    (Local Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    전역 변수와 함수 prototype 선언
    (Global Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
	함수 정의 (Function Definitions)
******************************************************************************/

/* Netflix */
DTV_STATUS_T HAL_CRYPTO_NF_Initialize(char *pIDFilePath);
DTV_STATUS_T HAL_CRYPTO_NF_Finalize(void);
DTV_STATUS_T HAL_CRYPTO_NF_GetESN(UINT8 *pEsn);
DTV_STATUS_T HAL_CRYPTO_NF_Encrypt(UINT8 *pData, UINT32 nLength);
DTV_STATUS_T HAL_CRYPTO_NF_Decrypt(UINT8 *pData, UINT32 nLength);

/*	LG TEE Implementation		 */
/******************************************************************************
Function Name
    HAL_CRYPTO_NF_Init

Function Description
    Init Netflix Crypto

Function Parameters
    void

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
//DTV_STATUS_T HAL_CRYPTO_NF_Init(void);
DTV_STATUS_T HAL_CRYPTO_NF_Init(HAL_CRYPTO_ARG_T *tCommon);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_CleanUp

Function Description
    Clean up Netflix Crypto

Function Parameters
    void

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_CleanUp(void);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_ClearKey

Function Description
    Clear Key by pAccountId

Function Parameters
    UINT8 *pAccountId 	[IN] 		Device Account ID

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_ClearKey(UINT8 *pAccountId);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_GenerateDiffieHellman

Function Description
    Generate Diffie-Hellman Key Private in TZ

Function Parameters
    UINT8 *pPem 		[IN] 		Pem string to generate p,g

Return Value
    If the function succeeds, the return value is DiffieHellman Context.
    If the function fails, the return value is NULL.

Remarks
    Must call HAL_CRYPTO_NF_DestroyDiffieHellman() when no more needed
******************************************************************************/
void*		 HAL_CRYPTO_NF_GenerateDiffieHellman(UINT8 *pPem);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_GetPubKey

Function Description
    Compute Public Key

Function Parameters
    void *pDiffieHellman	[IN]			DiffieHellman Context
    UINT8 *pPubKey 	[OUT] 		Public Key

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
//DTV_STATUS_T HAL_CRYPTO_NF_GetPubKey(void *pDiffieHellman, UINT8 *pPubKey);
DTV_STATUS_T HAL_CRYPTO_NF_GetPubKey(HAL_CRYPTO_ARG_T *tCommon);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_SetKeys

Function Description
    Compute Shared Secret and set as a kce, kch into pAccountId

Function Parameters
    UINT8 *pAccountId 	[IN] 			Device Account ID
    void *pDiffieHellman	[IN]			DiffieHellman Context
    UINT8 *pPubKey 	[IN] 			Server Public Key
    UINT32 size 		[IN] 			Public Key size

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
//DTV_STATUS_T HAL_CRYPTO_NF_SetKeys(UINT8 *pAccountId, void *pDiffieHellman, UINT8 *pServerPubKey, UINT32 size);
DTV_STATUS_T HAL_CRYPTO_NF_SetKeys(HAL_CRYPTO_ARG_T *tCommon); // compute_sharedSecret

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_DestroyDiffieHellman

Function Description
    Destroy DiffieHellman Context

Function Parameters
    void *pDiffieHellman	[IN]			DiffieHellman Context

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_DestroyDiffieHellman(void *pDiffieHellman);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_CreateCryptContext

Function Description
    Create Crypto Context

Function Parameters
   UINT8 *pAccountId 	[IN] 			Device Account ID

Return Value
    If the function succeeds, the return value is Crypto Context
    If the function fails, the return value is NULL.

Remarks
    Must call LGSOC_NF_DestroyCryptContext() when no more needed
******************************************************************************/
void*		 HAL_CRYPTO_NF_CreateCryptContext(UINT8 *pAccountId);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_DestroyCryptContext

Function Description
    Destroy Crypto Context

Function Parameters
    void *pContext		[IN]			Crypto Context

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_DestroyCryptContext(void *pContext);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_EncryptInit

Function Description
    Init AES128 with PKCS5 paHALng Encryption

Function Parameters
    void *pContext		[IN]			Crypto Context
    UINT8 *pIV			[IN]			Initial Vector

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    if pIV is NULL, it notices AES128 ECB mode
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_EncryptInit(void *pContext, UINT8 *pIV);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_DecryptInit

Function Description
    Init AES128 with PKCS5 paHALng Decryption

Function Parameters
    void *pContext		[IN]			Crypto Context
    UINT8 *pIV			[IN]			Initial Vector

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
   if pIV is NULL, it notices AES128 ECB mode
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_DecryptInit(void *pContext, UINT8 *pIV);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_Update

Function Description
    Encrypt/Decrypt data by using KPE/KCE

Function Parameters
    void *pContext		[IN]			Crypto Context
    UINT8 *pOut		[OUT]		Output Data
    UINT8 *pOut		[IN]			Input Data
    int	inLength		[IN]			Input length

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
//DTV_STATUS_T HAL_CRYPTO_NF_Update(void *pContext, UINT8 *pOut, UINT8 *pIn, int inLength);
DTV_STATUS_T HAL_CRYPTO_NF_Update(HAL_CRYPTO_ARG_T *tCommon);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_Final

Function Description
    Residual Process - PKCS5 PaHALng

Function Parameters
    void *pContext		[IN]			Crypto Context
    UINT8 *pOut		[OUT]		Output Data

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_Final(void *pContext, UINT8 *pOut);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_Hmac

Function Description
    HMAC SHA-384 Signing.

Function Parameters
    void *pContext		[IN]			Crypto Context
    UINT8 *pSign		[OUT]		HMAC Sign
    UINT8 *pData		[IN]			Data
    int	length		[IN]			Data length

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
//DTV_STATUS_T HAL_CRYPTO_NF_Hmac(void *pContext, UINT8 *pSign, UINT8 *pData, int length);
DTV_STATUS_T HAL_CRYPTO_NF_Hmac(HAL_CRYPTO_ARG_T *tCommon);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_WriteAppData

Function Description
    Write App Data to Secure Store

Function Parameters
    void *pDataId		[IN]			Data Identifier
    UINT8 *pData		[IN]			Data
    int	length		[IN]			Data length

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_WriteAppData(char *pDataId, UINT8 *pData, int length);

/******************************************************************************
Function Name
    HAL_CRYPTO_NF_ReadAppData

Function Description
    Read App Data to Secure Store.

Function Parameters
    void *pDataId		[IN]			Data Identifier
    UINT8 *pData		[IN]			Data
    int	length		[IN]			Data length

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_NF_ReadAppData(char *pDataId, UINT8 *pData, int length);

/*
 * netflix mtk tee
 */
DTV_STATUS_T HAL_CRYPTO_NF_GetRandomNumber(HAL_CRYPTO_ARG_T *tCommon);
DTV_STATUS_T HAL_CRYPTO_NF_ReseedSslEntropy(HAL_CRYPTO_ARG_T *tCommon);


/*
 * Netflix 4.1
 */
DTV_STATUS_T HAL_CRYPTO_NF41_GetESN(unsigned char *pEsn);
DTV_STATUS_T HAL_CRYPTO_NF41_CleanUp(void);
void *HAL_CRYPTO_NF41_GenerateContext(unsigned int generator, unsigned char *pPrime, int length);
DTV_STATUS_T HAL_CRYPTO_NF41_DestroyContext(void *pContext);
int HAL_CRYPTO_NF41_GetPubKey(void *pContext, unsigned char *pPub);
DTV_STATUS_T HAL_CRYPTO_NF41_SetKeys(void *pContext, unsigned char *pServerPub, int length);
int HAL_CRYPTO_NF41_AesCbc_Encrypt(void *pContext, unsigned char *pIV, unsigned char *pOut, unsigned char *pIn, int inLength);
int HAL_CRYPTO_NF41_AesCbc_Decrypt(void *pContext, unsigned char *pIV, unsigned char *pOut, unsigned char *pIn, int inLength);
DTV_STATUS_T HAL_CRYPTO_NF41_Hmac(void *pContext, unsigned char *pSign, unsigned char *pData, int length);
DTV_STATUS_T HAL_CRYPTO_NF41_HmacVerify(void *pContext, unsigned char *pSign, unsigned char *pData, int length);
DTV_STATUS_T HAL_CRYPTO_NF41_ExportKdw(void *pContext, unsigned char *pKdw);
DTV_STATUS_T HAL_CRYPTO_NF41_ExportContext(void *pContext, void *pExport, int *pLength);
DTV_STATUS_T HAL_CRYPTO_NF41_WriteAppData(char *pPath, unsigned char *pData, int length);
DTV_STATUS_T HAL_CRYPTO_NF41_ReadAppData(char *pPath, unsigned char *pData, int length);



DTV_STATUS_T HAL_CRYPTO_NF41_Init_refTee(HAL_CRYPTO_ARG_T* pcomm);
DTV_STATUS_T HAL_CRYPTO_NF41_Operate(HAL_CRYPTO_ARG_T* pcomm);
UINT32 HAL_CRYPTO_NF41_Encrypt(UINT8 *pData, UINT32 length);
UINT32 HAL_CRYPTO_NF41_Decrypt(UINT8 *pData, UINT32 length);




/* General */
DTV_STATUS_T HAL_CRYPTO_OpenKeySlot(void);
DTV_STATUS_T HAL_CRYPTO_CloseKeySlot(void);
DTV_STATUS_T HAL_CRYPTO_GetHWRandomNumber(UINT8 *pData, UINT32 nLength);
DTV_STATUS_T HAL_CRYPTO_Encrypt(UINT8 *pData, UINT32 nLength);
DTV_STATUS_T HAL_CRYPTO_Decrypt(UINT8 *pData, UINT32 nLength);

/* NYX */
DTV_STATUS_T HAL_CRYPTO_NYX_AES_GenerateSecureKey(unsigned char *pSecureKey, int keyLength);
DTV_STATUS_T HAL_CRYPTO_NYX_AES_CryptCBC(unsigned char *pSecureKey, int keyLength, int bEncrypt,
	unsigned char *pSrc, int srcLength, unsigned char *pDst, int *pDstLength);
DTV_STATUS_T HAL_CRYPTO_NYX_RSA_TransformSecureKey(unsigned char *pSecureKey, int *pKeyLength,
	unsigned char *pRSAKey, int rsaKeyLength);
DTV_STATUS_T HAL_CRYPTO_NYX_RSA_Crypt(unsigned char *pSecureKey, int keyLength, int bEncrypt,
	unsigned char *pSrc, int srcLength, unsigned char *pDst, int *pDstLength);
DTV_STATUS_T HAL_CRYPTO_NYX_RSA_ExportPublicKey(unsigned char *pPublicKey, int *pPublicKeyLength,
	unsigned char *pSecureKey, int keyLength);

/* Secure Firmware Update */
DTV_STATUS_T HAL_CRYPTO_SFU_Initialize(char *pSeedPath);
DTV_STATUS_T HAL_CRYPTO_SFU_Finalize(void);
DTV_STATUS_T HAL_CRYPTO_SFU_GetRSAKey(UINT8 *pRsaKey);
DTV_STATUS_T HAL_CRYPTO_SFU_GetAESKey(UINT8 *pAesKey);

/* Secure Storage */
DTV_STATUS_T HAL_CRYPTO_WriteSecureData(char *pIdentifier, UINT8 *pData, int dataLength);
DTV_STATUS_T HAL_CRYPTO_ReadSecureData(char *pIdentifier, UINT8 *pData, int *pLength);
DTV_STATUS_T HAL_CRYPTO_Debug(void);

/* Read/Write Secret Values */
DTV_STATUS_T HAL_CRYPTO_ReadMskToSecureStore(UINT8 *pData, int *pLength);
DTV_STATUS_T HAL_CRYPTO_WriteMskToSecureStore(UINT8 *pData, int length);
DTV_STATUS_T HAL_CRYPTO_ReadNLPSecret(UINT8 *pSecret, char *pSecretID);
DTV_STATUS_T HAL_CRYPTO_BBC_ReadClientKey(UINT8 *pKey);
DTV_STATUS_T HAL_CRYPTO_ReadPrsSecret(UINT8 *pSecret);
DTV_STATUS_T HAL_CRYPTO_CIPLUS_GetCiPlusSslPubKey(UINT8 *pPath, UINT8 *dKey);
DTV_STATUS_T HAL_CRYPTO_GetDebugKey(UINT8 *pKey);
DTV_STATUS_T HAL_CRYPTO_ReadDVRDeviceSecret(UINT8 *pSecret);
DTV_STATUS_T HAL_CRYPTO_WriteDVRDeviceSecret (UINT8 *pSecret, int length);
DTV_STATUS_T HAL_CRYPTO_CIPLUS_GetCiCanalReadyKey(UINT8 *pPath, UINT8 *pKey);
DTV_STATUS_T HAL_CRYPTO_ReadRemoteDiagSecret(UINT8 *pSecret, int *pLength);
DTV_STATUS_T HAL_CRYPTO_TNM_ReadSecretKey (UINT8 *pSecret);
DTV_STATUS_T HAL_CRYPTO_TNM_WriteSecretKey (UINT8 *pSecret, int length);
DTV_STATUS_T HAL_CRYPTO_Multiple_ReadClientKey(UINT8 *pKey);
DTV_STATUS_T HAL_CRYPTO_ReadSkypeSecret(UINT8 *pSecret, int *pLength);
DTV_STATUS_T HAL_CRYPTO_SDI_ReadPrivKeyAndID(UINT8 *pPrivKeyBuf, int *pPrivKeySize, UINT8 *pIdBuf, int *pIdLength);
DTV_STATUS_T HAL_CRYPTO_ReadRDXSecret(UINT8 *pSecret, int *pLength);
DTV_STATUS_T HAL_CRYPTO_HYBRIDCAST_ReadSecret(UINT8 *pSecret);
DTV_STATUS_T HAL_CRYPTO_ReadMVPDSecret(char *pPath, UINT8 *pData);
DTV_STATUS_T HAL_CRYPTO_WriteMVPDSecret(char *pPath, UINT8 *pData, int length);
DTV_STATUS_T HAL_CRYPTO_SDP_ReadCommonKey(UINT8 *pKey, int *pLength);

/* Vudu */
/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_OpenM2mKeySlot

Function Description
    Allocate Memory to Memory Keyslot Handle

Function Parameters
    None

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_OpenM2mKeySlot(void);

/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_CloseM2mKeySlot

Function Description
    Free Memory to Memory Keyslot Handle

Function Parameters
    None

Return Value
    If the function succeeds, the return value is OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_CloseM2mKeySlot(void);

/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_SelectM2mKeyAlgorithm

Function Description
    Select Encryption/Decryption Algorithm

Function Parameters
    alg	[IN] 		Select Algorithm
    dir	[IN]		Select Mode

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_SelectM2mKeyAlgorithm(UINT8 alg, UINT8 dir);

/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_LoadClearM2mKey

Function Description
    Loads a Clear Key and a Clear IV to KeySlot.

Function Parameters
    keyLen	[IN] 		Key Length
    pKeyData	[IN]		Key Data
    ivLen		[IN]		Initial Vector Length
    pIvData	[IN]		Initial Vector Data

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_LoadClearM2mKey(const UINT32 keyLen, const UINT8 *pKeyData, const UINT32 ivLen, const UINT8 *pIvData);

/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_LoadWrappedM2mKey

Function Description
    Loads a Wrapped Key and a Wrapped IV to KeySlot

Function Parameters
    keyLen	[IN] 		Key Length of pKey4Data and pIvData
    pKey3Data	[IN]		Seed of Session Key
    pKey4Data	[IN]		Seed of Control Word
    pIvData	[IN]		Initial Vector Data

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_LoadWrappedM2mKey(const UINT32 keyLen, const UINT8 *pKey3Data, const UINT8 *pKey4Data, const UINT8 *pIvData);

/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_LoadWrappedM2mKeyClearIv

Function Description
    Loads a Wrapped Key and a Clear IV to KeySlot

Function Parameters
    keyLen	[IN] 		Key Length of pKey4Data and pIvData
    pKey3Data	[IN]		Seed of Session Key
    pKey4Data	[IN]		Seed of Control Word
    ivLen		[IN]		Initial Vector Length
    pIvData	[IN]		Initial Vector Data

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_LoadWrappedM2mKeyClearIv(const UINT32 keyLen, 	const UINT8 *pKey3Data, const UINT8 *pKey4Data, const UINT32 ivLen, const UINT8 *pIvData);

/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_RunM2mKey

Function Description
    Start the Cipher Process

Function Parameters
    length		[IN] 		Length of Both Src and Dst
    pSrc		[IN]		Source Pointer
    pDst		[OUT]	Destination Pointer

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_RunM2mKey(const UINT32 length, const UINT8 *pSrc, UINT8 *pDst);


/* Play API */
/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_OpenCAKeySlotHandle

Function Description
    Allocate VUDU CA Descramble Handle

Function Parameters
    None

Return Value
    If the function succeeds, the return value is handle value.
    If the function fails, the return value is 0.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_OpenCAKeySlotHandle(void);


/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_CloseCAKeySlotHandle

Function Description
    Free VUDU CA Descramble Handle

Function Parameters
    None

Return Value
    If the function succeeds, the return value is 0.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_CloseCAKeySlotHandle(void);


/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_GetCAKeySlotHandle

Function Description
    Allocate VUDU CA Descramble Handle

Function Parameters
    None

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_GetCAKeySlotHandle(void);

/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_SetWrappedCAKey

Function Description
    Load Wrapping Key to CA Descrambler

Function Parameters
    pKey			[IN]		CA Initial Vector
    pUniqueKey	[IN]		CA Wrapping Key
    keyType		[IN]		CA Key Type

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_SetWrappedCAKey(const UINT8 *pKey, const UINT8 *pUniqueKey, UINT8 keyType);


/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_SetWrappedAllCAKey

Function Description
    Load Wrapping Keys to CA Descrambler

Function Parameters
    pOddKey		[IN]		CA Initial Odd Vector
    pEvenKey		[IN]		CA Initial Even Vector
    pUniqueKey	[IN]		CA Wrapping Key

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_SetWrappedAllCAKey(const UINT8 *pOddKey, const UINT8 *pEvenKey, const UINT8 *pUniqueKey);


/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_LoadCAClearKey

Function Description
    Load Clear Key to CA Descrambler

Function Parameters
    pKey			[IN]		CA Initial Vector
    keyType		[IN]		CA Key Type

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_LoadCAClearKey(const UINT8 *pKey, UINT8 keyType);


/******************************************************************************
Function Name
    HAL_CRYPTO_VUDU_StartTSDecrypt

Function Description
    Start the TS Decrypt

Function Parameters
    srcLength	[IN] 		Length of Src
    pSrc		[IN]		Source Pointer (Encrypeted VUDU TS stream)
    pDstLength	[OUT]	Length of Dst
    pDst		[OUT]	Destination Pointer (the Result should be clear TS)

Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T HAL_CRYPTO_VUDU_StartTSDecrypt(const UINT32 srcLength, const UINT8 *pSrc, UINT32 *pDstLength, UINT8 *pDst);

DTV_STATUS_T HAL_CRYPTO_VUDU_SetOddEvenCAKey(const UINT8 *pOddKey, const UINT8 *pEvenKey, UINT8 keyType);
//DTV_STATUS_T HAL_CRYPTO_VUDU_StartCADecrypt(void);

/* Vudu SVP : Security Level 1 */

// Call Sequnce Note :
//
//	[1. Initialize]
//
//	... Allocate and set CPB from/to VDEC (Note : Size must be 8 MBytes, and align value also must be 8 MBytes)
//	HAL_CRYPTO_VUDU_SVP_Init(base, size);
//
//
//	[2. Decrypt -> Reformatting -> Put into Secure CPB]
//
//	while (...) {	// Playback Loop
//
//		HAL_CRYPTO_VUDU_SVP_TSDecrypt(...);	// Decyrption
//
//  again :
//		... Get rPtr, wPtr from VDEC
//		switch (HAL_CRYPTO_VUDU_SVP_GetAU(...)) {	// Reformatting & Put into CPB
//			case HAL_CRYPTO_VUDU_SVP_OK : // Get AU
//				... Send AU and CPB Update Info to VDEC
//				goto again;
//
//			case HAL_CRYPTO_VUDU_SVP_NO_AU :
//				break;	// Get next data packet
//
//			case HAL_CRYPTO_VUDU_SVP_WAIT :
//				... sleep to give vdec a delay to decode and display
//				goto again;	// Try to put CPB again.
//
//			default :	// Error
//				... Error Handling
//		}
//		... Do others
//	}	// Playback Loop
//
//
//	[3. Release]
//
//	HAL_CRYPTO_VUDU_SVP_Release();
//
//
//	[4. Flush]
//
//	... Flush VDEC
//	HAL_CRYPTO_VUDU_SVP_Flush();

typedef enum {
	HAL_CRYPTO_VUDU_SVP_ERROR	= -1,
	HAL_CRYPTO_VUDU_SVP_OK		= 0,
	HAL_CRYPTO_VUDU_SVP_NO_AU	= 1,
	HAL_CRYPTO_VUDU_SVP_WAIT	= 2,
} HAL_CRYPTO_VUDU_SVP_T;

/**
 *	Initialize SVP for VUDU.
 *
 *	@param	base	[in]	Secure CPB base.
 *	@param	size	[in]	Secure CPB size.
 *	@return	success : OK, failure : NOT_OK.
 */
DTV_STATUS_T HAL_CRYPTO_VUDU_SVP_Init(UINT32 base, int size);

/**
 *	SVP TS Decrypt for VUDU.
 *
 *	@param	srcLength	[in]	Length of Src.
 *	@param	pSrc		[in]	Source Pointer (Encrypeted VUDU TS stream).
 *	@param	pDstLength	[out]	Length of Dst.
 *	@param	pDst		[out]	Destination Pointer (the Result should be clear TS and be removed Video ES Data).
 *	@return succeeded - OK, if not - NOT_OK.
 */
DTV_STATUS_T HAL_CRYPTO_VUDU_SVP_TSDecrypt(const UINT32 srcLength, const UINT8 *pSrc, UINT32 *pDstLength, UINT8 *pDst);

/**
 *	Get AU for VUDU
 *
 *	@param	pAuAddr		[out]		physical AU address in secure CPB.
 *	@param	pAuLength	[out]		physical AU length in secure CPB.
 *	@param	pAuType		[out]		AU type (It is not available in TVP_REFORMATTER_NONE).
 *	@param	pWPtr		[in/out]	Secure CPB Write Pointer.
 *	@param	rPtr		[in]		Secure CPB Read Pointer.
 *	@param	pPTS		[out]		Representative PTS value.
 *	@return HAL_CRYPTO_VUDU_SVP_NO_AU	: Proceed next.
 *			HAL_CRYPTO_VUDU_SVP_OK		: AU Out.
 *			HAL_CRYPTO_VUDU_SVP_WAIT	: Wait because of not enough CPB.
 *			HAL_CRYPTO_VUDU_SVP_ERROR	: Error.
 */
HAL_CRYPTO_VUDU_SVP_T HAL_CRYPTO_VUDU_SVP_GetAU(unsigned int *pAuAddr, int *pAuLength, int *pAuType,
	unsigned int *pWPtr, unsigned int rPtr, unsigned long long *pPTS);

/**
 *	Flush Reformatter & TS Parser for VUDU
 *
 *	@return	success : TVP_OK, failure : TVP_ERROR.
 */
DTV_STATUS_T HAL_CRYPTO_VUDU_SVP_Flush(void);

/**
 *	Release SVP for VUDU.
 *
 *	@return	success : OK, failure : NOT_OK.
 */
DTV_STATUS_T HAL_CRYPTO_VUDU_SVP_Release(void);

DTV_STATUS_T HAL_CRYPTO_Netflix_Secure_Store(char *dst_file_path, unsigned char *input, unsigned int inputlength);
DTV_STATUS_T HAL_CRYPTO_NF41_Init(void);
DTV_STATUS_T HAL_CRYPTO_NF41_GetESN(UINT8 *pEsn);
DTV_STATUS_T HAL_CRYPTO_CheckSeStoreIndex(void);
DTV_STATUS_T HAL_CRYPTO_NF41_GetNamedKey (UINT8 *pKpe, UINT8 *pKph, UINT8 *pKpw);

#endif		//_HAL_CRYPTO_H_


