
/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file crypto_nf_ntba.h
 *
 * This source file defines the DDI functions related to crypto driver control with TEE
 *
 *  @author	yongjoon cho (yongjoon.cho@lge.com) 
 *  @version	0.1
 *  @date		2012.09.07
 *  @note		Additional information.
 *  @see		crypto_ddi.h
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	_CRYPTO_NF_NTBA_H_
#define	_CRYPTO_NF_NTBA_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#ifndef WIN32_SIM
#include "common.h"
#else
#include "common_win32.h"
#endif

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

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

/* 	Netflix	 				*/
/*	LG TEE Implementation		 */
/******************************************************************************
Function Name
    DDI_CRYPTO_NF_Init

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
DTV_STATUS_T DDI_CRYPTO_NF_Init(void);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_CleanUp

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
DTV_STATUS_T DDI_CRYPTO_NF_CleanUp(void);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_ClearKey

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
DTV_STATUS_T DDI_CRYPTO_NF_ClearKey(UINT8 *pAccountId);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_GenerateDiffieHellman

Function Description
    Generate Diffie-Hellman Key Private in TZ 

Function Parameters
    UINT8 *pPem 		[IN] 		Pem string to generate p,g 

Return Value
    If the function succeeds, the return value is DiffieHellman Context.
    If the function fails, the return value is NULL.

Remarks
    Must call DDI_CRYPTO_NF_DestroyDiffieHellman() when no more needed
******************************************************************************/
void*		 DDI_CRYPTO_NF_GenerateDiffieHellman(UINT8 *pPem);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_GetPubKey

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
DTV_STATUS_T DDI_CRYPTO_NF_GetPubKey(void *pDiffieHellman, UINT8 *pPubKey);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_SetKeys

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
DTV_STATUS_T DDI_CRYPTO_NF_SetKeys(UINT8 *pAccountId, void *pDiffieHellman, UINT8 *pServerPubKey, UINT32 size);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_DestroyDiffieHellman

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
DTV_STATUS_T DDI_CRYPTO_NF_DestroyDiffieHellman(void *pDiffieHellman);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_CreateCryptContext

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
void*		 DDI_CRYPTO_NF_CreateCryptContext(UINT8 *pAccountId);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_DestroyCryptContext

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
DTV_STATUS_T DDI_CRYPTO_NF_DestroyCryptContext(void *pContext);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_EncryptInit

Function Description
    Init AES128 with PKCS5 padding Encryption 

Function Parameters
    void *pContext		[IN]			Crypto Context 
    UINT8 *pIV			[IN]			Initial Vector
    
Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    if pIV is NULL, it notices AES128 ECB mode
******************************************************************************/
DTV_STATUS_T DDI_CRYPTO_NF_EncryptInit(void *pContext, UINT8 *pIV);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_DecryptInit

Function Description
    Init AES128 with PKCS5 padding Decryption 

Function Parameters
    void *pContext		[IN]			Crypto Context 
    UINT8 *pIV			[IN]			Initial Vector
    
Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
   if pIV is NULL, it notices AES128 ECB mode
******************************************************************************/
DTV_STATUS_T DDI_CRYPTO_NF_DecryptInit(void *pContext, UINT8 *pIV);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_Update

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
DTV_STATUS_T DDI_CRYPTO_NF_Update(void *pContext, UINT8 *pOut, UINT8 *pIn, int inLength);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_Final

Function Description
    Residual Process - PKCS5 Padding

Function Parameters
    void *pContext		[IN]			Crypto Context 
    UINT8 *pOut		[OUT]		Output Data
    
Return Value
    If the function succeeds, the return value is OK.
    If the function fails, the return value is NOT_OK.

Remarks
    None
******************************************************************************/
DTV_STATUS_T DDI_CRYPTO_NF_Final(void *pContext, UINT8 *pOut);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_Hmac

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
DTV_STATUS_T DDI_CRYPTO_NF_Hmac(void *pContext, UINT8 *pSign, UINT8 *pData, int length);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_WriteAppData

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
DTV_STATUS_T DDI_CRYPTO_NF_WriteAppData(char *pDataId, UINT8 *pData, int length);

/******************************************************************************
Function Name
    DDI_CRYPTO_NF_ReadAppData

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
DTV_STATUS_T DDI_CRYPTO_NF_ReadAppData(char *pDataId, UINT8 *pData, int length);

#endif			//#ifndef	_CRYPTO_NF_NTBA_H_
