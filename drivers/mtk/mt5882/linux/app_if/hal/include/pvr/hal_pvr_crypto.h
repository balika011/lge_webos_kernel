/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file		hal_pvr_crypto.h
 *
 *  Header for PVR Crypto.
 *
 *  @author		Kim, Suk (stan.kim@lge.com)
 *  @version    1.0
 *  @date       2011.2
 *  @note       Additional information.
 */

#ifndef	_PVR_CRYPTO_H_
#define	_PVR_CRYPTO_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "pvr_common.h"

#include "pvr_kadp.h"
#include "pvr_kapi.h"

/* for decryption */
#include "sdec_kadp.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define PVR_CIPHER_KEY_SIZE 16

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
 * Key Structure
 * This strcuture is used for setting pid
 */
typedef struct
{
	UINT32							keySize;							/**< key size in bytes */
	UINT8							*ivKey;								/**< A possible Initialization Vector for ciphering */
	UINT8							*cipherKey;							/**< cipher key used for PVR TS encrypt/decrypt */
} LX_PVR_KEY_T;

/**
 * PVR Cipher Control
 * Cipher context can be shared. Encrypt/Decrypt should be paired.
 */
typedef struct
{
	BOOLEAN							bCrypto;							/**< is decryption enabled ? */
	LX_SDEC_BLK_MODE_T				eBlkMode;							/**< Blk Mode (ECB/CBC). used at @HAL_PVR_SetConfig */
	LX_SDEC_CIPHER_KEY_T			eKeyMode;							/**< Key Mode (odd/even). used at @HAL_PVR_SetCipherKeys */
	LX_PVR_KEY_T					keyInfo[PVR_CIPHER_OPERATION_NUM];	/**< Information of encryption/decryption keys */
} LX_PVR_CIPHER_CTRL_T, *P_LX_PVR_CIPHER_CTRL_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

#if 0
DTV_STATUS_T PVR_CRYPTO_Initialize(void);
DTV_STATUS_T PVR_CRYPTO_GetSecureKey(UINT8 *pKey);
UINT8 *PVR_CRYPTO_ProtectData(UINT32 size, UINT8 *pInputBuf);
UINT8 *PVR_CRYPTO_UnprotectData(UINT32 size, UINT8 *pInputBuf);

void	PVR_CRYPTO_SetCipherAlgorithm(PVR_CIPHER_T cipherAlgorithm);
void	PVR_CRYPTO_SetEncryptionState(BOOLEAN encryptEnable);
BOOLEAN	PVR_CRYPTO_IsEncryptionEnabled(void);
DTV_STATUS_T PVR_CRYPTO_SetCipherKeys(PVR_CRYPTO_TYPE_T cryptoInfo);
DTV_STATUS_T PVR_CRYPTO_GetCipherKeys(PVR_CIPHER_OPERATION_T cipherOperation, PVR_CRYPTO_TYPE_T *pCryptoInfo);
DTV_STATUS_T PVR_CRYPTO_SetKeyToSE(PVR_CRYPTO_TYPE_T cryptoInfo);
#endif

DTV_STATUS_T LXPVR_CRYPTO_Initialize(void);

DTV_STATUS_T LXPVR_CRYPTO_GetSecureKey(UINT8 *pKey);

UINT8 *LXPVR_CRYPTO_ProtectData(UINT32 size, UINT8 *pInputBuf, BOOLEAN bPhysicalAddr);

UINT8 *LXPVR_CRYPTO_UnprotectData(UINT32 size, UINT8 *pInputBuf, BOOLEAN bPhysicalAddr);

DTV_STATUS_T LXPVR_CRYPTO_SetCipherAlgorithm(PVR_CIPHER_T cipherAlgorithm);

BOOLEAN	LXPVR_CRYPTO_IsEncryptionEnabled(void);

DTV_STATUS_T LXPVR_CRYPTO_SetEncryptionState(BOOLEAN encryptEnable);

DTV_STATUS_T LXPVR_CRYPTO_SetCipherKeys(PVR_CRYPTO_TYPE_T cryptoInfo);

DTV_STATUS_T LXPVR_CRYPTO_GetCipherKeys(PVR_CIPHER_OPERATION_T cipherOperation, PVR_CRYPTO_TYPE_T *pCryptoInfo);
DTV_STATUS_T LXPVR_CRYPTO_SetKeyToSE(PVR_CRYPTO_TYPE_T cryptoInfo);


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PVR_CRYPTO_H_ */

