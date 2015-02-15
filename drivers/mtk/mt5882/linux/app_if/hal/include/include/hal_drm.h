/******************************************************************************
 *   TV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2013 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file		hal_drm.h
 *
 *  Hardware Abstration Layer for DRM
 *
 *  @author		Yujin, Lee (yujin.lee@lge.com)
 *  @version	1.0
 *  @date		2013.03.04
 *  @note
 *  @see
 */

/******************************************************************************
    Header File Guarder
******************************************************************************/
#ifndef	_HAL_DRM_H_
#define	_HAL_DRM_H_

/******************************************************************************
    Control Constants
******************************************************************************/

/******************************************************************************
    File Inclusions
******************************************************************************/
#include "hal_common.h"

/******************************************************************************
    Constant Definitions
******************************************************************************/

/******************************************************************************
    Macro Definitions
******************************************************************************/

/******************************************************************************
    Type Definitions
******************************************************************************/

/******************************************************************************
    Function Declaration
******************************************************************************/
DTV_STATUS_T HAL_DRM_GetSecureData (char *pDataPath, unsigned char *pData, unsigned int *pLength);
DTV_STATUS_T HAL_DRM_SetSecureData (char *pDataPath, unsigned char *pData, unsigned int length);
DTV_STATUS_T HAL_DRM_DeleteSecureData (char *pDataPath);
DTV_STATUS_T HAL_DRM_GetHWRandomData (unsigned char *pData, unsigned int digit);
DTV_STATUS_T HAL_DRM_GetSecureClock (unsigned long long *pSecureClock);
DTV_STATUS_T HAL_DRM_SetSecureClock (unsigned long long secureClock);
DTV_STATUS_T DDI_DRM_GetOriginalDataLength (char *pDataPath, unsigned int *pLength);
DTV_STATUS_T HAL_DRM_GetClearDataLength (char *pDataPath, unsigned int *pLength);
DTV_STATUS_T HAL_DRM_AESHWInit (int mode, unsigned char *pKey, unsigned char *pIV, int operation, int isPadding);
DTV_STATUS_T HAL_DRM_AESHWUpdate (unsigned char *pOutData, unsigned int *pOutDataSize, unsigned char *pInData, unsigned int nInDataSize);
DTV_STATUS_T HAL_DRM_AESHWFinish (unsigned char *pOutData, unsigned int *pOutDataSize);
DTV_STATUS_T HAL_DRM_GetHWID(unsigned char *pOutData, unsigned int *pOutDataSize);
DTV_STATUS_T HAL_DRM_IsSecureDataExist(char *pDataPath);
DTV_STATUS_T HAL_DRM_InitDrv(void);
DTV_STATUS_T HAL_DRM_DIVXDRM_GetHWSecretKey(unsigned char *pOutData, unsigned int *pOutDataSize);
DTV_STATUS_T HAL_DRM_JPMARLIN_CheckSecretData(int secretDataType);
DTV_STATUS_T HAL_DRM_JPMARLIN_WriteSecretData(int secretDataType,
	unsigned char *pRootCert, int rootCertLen, unsigned char *pCaCert, int caCertLen,
	unsigned char *pClientCert, int clientCertLen, unsigned char *pClientKey, int clientKeyLen);
DTV_STATUS_T HAL_DRM_WriteWidevineKeyBox(UINT8 *pData, int dataLength);
DTV_STATUS_T HAL_DRM_GetWidevineDeviceID(UINT8 *pDeviceID,  int *pIdLength);
DTV_STATUS_T HAL_DRM_GetWidevineUDContentKey(UINT8 *pUdKey);
DTV_STATUS_T HAL_DRM_EncryptUDContent(UINT8 *pData, int length);
DTV_STATUS_T HAL_DRM_StartSecureUDTS(void);
DTV_STATUS_T HAL_DRM_StopSecureUDTS(void);

// SVP
DTV_STATUS_T HAL_DRM_SVP_GetProvisionedData(char *pszPath,
	unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectData);
DTV_STATUS_T HAL_DRM_SVP_GenKeyPair_ECCP256(unsigned char *pPubKey, unsigned char *pPrivKey);
DTV_STATUS_T HAL_DRM_SVP_SetKey_ECCP256(unsigned char *pbKeyBuffer,
	unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN isKeyProtected);
DTV_STATUS_T HAL_DRM_SVP_Sign_ECDSAP256(unsigned char *pbData, unsigned int cbData, unsigned char *pbSignature);
DTV_STATUS_T HAL_DRM_SVP_Decrypt_ECCP256(unsigned char *pbCipherText, unsigned char *pbPlainText, BOOLEAN bProtectPlainText);
DTV_STATUS_T HAL_DRM_SVP_SetKey_OMAC1(unsigned char *pbKeyBuffer,
	unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN isKeyProtected);
DTV_STATUS_T HAL_DRM_SVP_Verify_OMAC1(unsigned char *pbData, unsigned int ibData, unsigned int cbData,
	unsigned char *pbSignature, unsigned int ibSignature);
DTV_STATUS_T HAL_DRM_SVP_Sign_OMAC1(unsigned char *pbData, unsigned int ibData, unsigned int cbData,
	unsigned char *pbSignature, BOOLEAN bProtectSignature);
DTV_STATUS_T HAL_DRM_SVP_CopyBytes(void *bpTo, unsigned int ibTo,
	void *pbFrom, unsigned int ibFrom, unsigned int cbFrom, unsigned int cbCount);
DTV_STATUS_T HAL_DRM_SVP_SetContentKey_AESCTR(unsigned char *pbKeyBuffer,
	unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN isKeyProtected);
DTV_STATUS_T HAL_DRM_SVP_GetUDContentKey(unsigned char *pUdKey);
DTV_STATUS_T HAL_DRM_SVP_DecryptContent_AESCTR(void *pCtrContext,
	unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectContent);
DTV_STATUS_T HAL_DRM_SVP_SetKey_AESECB(unsigned char *pbKeyBuffer,
	unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN isKeyProtected);
DTV_STATUS_T HAL_DRM_SVP_Encrypt_AESECB(unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectData);
DTV_STATUS_T HAL_DRM_SVP_Decrypt_AESECB(unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectData);

// For ProIdiom DRM
DTV_STATUS_T HAL_DRM_PI_SetSecureFilePath(char *pPath);
DTV_STATUS_T HAL_DRM_PI_Init(void);
DTV_STATUS_T HAL_DRM_PI_Release(void);
DTV_STATUS_T HAL_DRM_PI_Update(int bUpdateSF, int bUpdateSD);
DTV_STATUS_T HAL_DRM_PI_AES_ECB_SFSF(unsigned int offset1, unsigned int dat_loc, int operation,
	unsigned int offset2, unsigned int nInDataSize, unsigned int offset3);
DTV_STATUS_T HAL_DRM_PI_XOR_XXSFSF(unsigned int offset1, unsigned int dat_loc,
	unsigned int offset2, unsigned int nInDataSize, unsigned int offset3);
DTV_STATUS_T HAL_DRM_PI_XORAND_SFSFSFSF(unsigned int offset1, unsigned int offset2,
	unsigned int offset3, unsigned int nInDataSize, unsigned int offset4);
DTV_STATUS_T HAL_DRM_PI_SetSecureData_NWXX(unsigned char *pData, unsigned int length,
	unsigned int offset, unsigned int dat_loc);
DTV_STATUS_T HAL_DRM_PI_SetSecureData_XXXX(unsigned int offset1, unsigned int dat_loc1, unsigned int length,
	unsigned int offset2, unsigned int dat_loc2);
DTV_STATUS_T HAL_DRM_PI_GetSecureData_XX(unsigned char *pData, unsigned int offset,
	unsigned int dat_loc, unsigned long length);
DTV_STATUS_T HAL_DRM_PI_CheckIV(unsigned int offset);
DTV_STATUS_T HAL_DRM_PI_GetMsgTypes(unsigned int offset, unsigned char *pMType1, unsigned char *pMType2);
DTV_STATUS_T HAL_DRM_PI_SetXHFs(unsigned int mode, unsigned int offset1, unsigned int offset2);
DTV_STATUS_T HAL_DRM_PI_DecryptTS(unsigned char *pInData, unsigned int numPkts, unsigned int pktSize, unsigned int msg_pid);

 
//Secure Store in EMMC
DTV_STATUS_T HAL_DRM_GetSecureDataEMMC (char *pDataPath, unsigned char *pData, unsigned int *pLength);
DTV_STATUS_T HAL_DRM_SetSecureDataEMMC (char *pDataPath, unsigned char *pData, unsigned int length);
DTV_STATUS_T HAL_DRM_DeleteSecureDataEMMC (char *pDataPath);
DTV_STATUS_T HAL_DRM_GetClearDataLengthEMMC (char *pDataPath, unsigned int *pLength);
DTV_STATUS_T HAL_DRM_IsSecureDataExistEMMC(char *pDataPath);
DTV_STATUS_T HAL_DRM_CheckKeyIndexEMMC(void);
DTV_STATUS_T HAL_DRM_GetSecureDataProtected (char *pID, unsigned char *pData, unsigned int *pLength);
#endif /* _DILE_DRM_H_ */

