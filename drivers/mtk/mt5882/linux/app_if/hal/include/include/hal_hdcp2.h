/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 		hal_hdcp2.h
 *
 *  header of Common Interface HAL Fuction.
 *
 *
 *  @author		kyungnam.bae
 *  @author
 *  @version	1.0
 *  @date		2013.3.5
 *  @date
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _COMMON_HAL_HDCP2_H_
#define _COMMON_HAL_HDCP2_H_

/******************************************************************************
    전역 제어 상수(Control Constants)
******************************************************************************/

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include "hal_common.h"

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/


/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
/* HDCP 2.0 DRM*/
DTV_STATUS_T HAL_HDCP2_GetCertInfo (UINT8 *pReceiverID, UINT8 *pPubKey, UINT8 *pRootSign);
DTV_STATUS_T HAL_HDCP2_GetCertInfo2(unsigned char *pReceiverID, unsigned char *pPubKey, unsigned char *pReserved, unsigned char *pRootSign);
DTV_STATUS_T HAL_HDCP2_Decrypt_RSAES_OAEP (UINT8 *pEkpub_km);
DTV_STATUS_T HAL_HDCP2_Kd_Key_Derivation (UINT8 *pRtx, UINT8 *pRrx, UINT32 version);
DTV_STATUS_T HAL_HDCP2_Compute_H_Prime(UINT8 *pHPrime, UINT8 repeater, UINT8 *pRtx, UINT8 *pInfo, UINT32 version);
DTV_STATUS_T HAL_HDCP2_Compute_L_Prime (UINT8 *pL_prime, UINT8 *pRn, UINT8 *pRrx, UINT32 version);
DTV_STATUS_T HAL_HDCP2_Compute_Kh (void);
DTV_STATUS_T HAL_HDCP2_Encrypt_Km_using_Kh (UINT8 *pEkh_km, UINT8 *pM);
DTV_STATUS_T HAL_HDCP2_Decrypt_Km_using_Kh (UINT8 *pM, UINT8 *ekh_km);
DTV_STATUS_T HAL_HDCP2_Decrypt_EKs (UINT8 *pEks, UINT8 *pRtx, UINT8 *pRrx, UINT8 *pRn, UINT32 version);
DTV_STATUS_T HAL_HDCP2_XOR_Ks_with_LC128 (void);
DTV_STATUS_T HAL_HDCP2_Data_Decrypt (UINT8 *pOutputbuf, UINT8 *pInputbuf, int length, UINT8 *pCounter);
DTV_STATUS_T HAL_HDCP2_WriteKeySet(UINT8 *pKeySet, UINT8 *dst_file_path, UINT32 KeyLength);
DTV_STATUS_T HAL_HDCP2_SHA256(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length);
DTV_STATUS_T HAL_HDCP2_AES_CTR(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pKey, int keyLength, UINT8 *pCounter);
DTV_STATUS_T HAL_HDCP2_Data_Encrypt(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pCounter);
DTV_STATUS_T HAL_HDCP2_Generate_Km(UINT8 *pMaskedDB, UINT8 *pDB, UINT8 *pDBmask);
DTV_STATUS_T HAL_HDCP2_Generate_Ks(UINT8 *pEks, UINT8 *pRrx, UINT8 *pRtx, UINT8 *pRn, UINT32 version);
DTV_STATUS_T HAL_HDCP2_GetRootPublicKey (UINT8 *pRootPublicKey, UINT8 *src_file_path);
DTV_STATUS_T HAL_HDCP2_Select_StoredKm(UINT32 index);
DTV_STATUS_T HAL_HDCP2_Write_StoredKm(UINT32 index);
DTV_STATUS_T HAL_HDCP2_HMAC_SHA256(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pkey, int keylength);
DTV_STATUS_T HAL_HDCP2_SHA1(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length);
DTV_STATUS_T HAL_HDCP2_GetProtectedDecryptionKey(UINT8 *pData, int *pLength);
DTV_STATUS_T HAL_HDCP2_SetProtectedDecryptionKey(UINT8 *pData, int length);
DTV_STATUS_T HAL_HDCP2_GetEncHdcp2Key(unsigned char *pEncHdcp2Key, unsigned char *src_file_path, unsigned char *pEncKeyLength);
DTV_STATUS_T HAL_HDCP2_SetEncHdcp2Key(unsigned char *pEncHdcp2Key);
#endif /*_COMMON_HAL_HDCP2_H_ */


