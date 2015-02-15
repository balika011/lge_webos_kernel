#ifndef	_DRM_DDI_H_
#define	_DRM_DDI_H_

#include "emp_ddi.h"
#ifdef INCLUDE_NC30_SU1
#define AES_CBC  0
#define AES_CTR  1
#define AES_ECB  2
#define AES_ENCRYPT  0
#define AES_DESCRYPT 1
#define true 1
#define false 0
#endif
#define HDCP2_LC128_SIZE		16
#define HDCP2_RECEIVER_ID_SIZE	5
#define HDCP2_MODULUS_SIZE		128
#define HDCP2_PUBEXP_SIZE		3
#define HDCP2_PUBLIC_KEY_SIZE	(HDCP2_MODULUS_SIZE+HDCP2_PUBEXP_SIZE)
#define HDCP2_RESERVED_SIZE		2
#define HDCP2_ROOT_SIGN_SIZE	384		// 3072bits
#define HDCP2_CERT_SIZE			(HDCP2_RECEIVER_ID_SIZE+HDCP2_PUBLIC_KEY_SIZE+\
									HDCP2_RESERVED_SIZE+HDCP2_ROOT_SIGN_SIZE)
#define HDCP2_KPRIV_P_SIZE		64
#define HDCP2_KPRIV_Q_SIZE		64
#define HDCP2_KPRIV_DP_SIZE		64
#define HDCP2_KPRIV_DQ_SIZE		64
#define HDCP2_KPRIV_QINV_SIZE	64
#define HDCP2_KPRIV_CRT_SIZE	(HDCP2_KPRIV_P_SIZE+HDCP2_KPRIV_Q_SIZE+\
									HDCP2_KPRIV_DP_SIZE+HDCP2_KPRIV_DQ_SIZE+HDCP2_KPRIV_QINV_SIZE)


/* PlayReady */
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_SecureStoreGlobalPasswordSeed (char *pszPath, unsigned char *pbSecStorePassword, unsigned long cbSecStorePassword);
DTV_STATUS_T DDI_DRM_PLAYREADY_Set_SecureStoreGlobalPasswordSeed (char *pszPath);
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_ZPrivSig (char *KeyPath, unsigned char *pbZPrivSig, unsigned long *cbZPrivSig);
DTV_STATUS_T DDI_DRM_PLAYREADY_Set_ZPrivSig (char *KeyPath, unsigned char *pbZPrivSig, unsigned long cbZPrivSig);
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_ZPrivEnc (char *KeyPath, unsigned char *pbZPrivEnc, unsigned long *cbZPrivEnc);
DTV_STATUS_T DDI_DRM_PLAYREADY_Set_ZPrivEnc (char *KeyPath, unsigned char *pbZPrivEnc, unsigned long cbZPrivEnc);
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_DeviceCert (char *KeyPath, unsigned char *pbDevcert, unsigned long *cbDevCert);
DTV_STATUS_T DDI_DRM_PLAYREADY_Set_DeviceCert (char *KeyPath, unsigned char *pbDevcert, unsigned long cbDevCert);
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_BDeviceCert (char *KeyPath, unsigned char *pbBDevcert, unsigned long *cbBDevCert);
DTV_STATUS_T DDI_DRM_PLAYREADY_Set_BDeviceCert (char *KeyPath, unsigned char *pbBDevcert, unsigned long cbBDevCert);
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_BGroupCert (char *KeyPath, unsigned char *pbBGroupCert, unsigned long *cbBGroupCert);
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_ModelPrivateKey (char *KeyPath, unsigned char *pbModelPrivKey, unsigned long *cbModelPrivKey);
DTV_STATUS_T DDI_DRM_PLAYREADY_Get_GroupPrivateKey (char *KeyPath, unsigned char *pbGroupPrivKey, unsigned long *cbGroupPrivKey);

/* Adobe Access 2.0 DRM */
DTV_STATUS_T DDI_DRM_FLASHACCESS_AES_HwInit (int AES_MODE, unsigned char* pKey, unsigned char* IV, int INIT_TYPE, int padding);
DTV_STATUS_T DDI_DRM_FLASHACCESS_AES_HwUpdate (unsigned char *pOutData, int *pOutDataSize, unsigned char *pInData, int nInDataSize);
DTV_STATUS_T DDI_DRM_FLASHACCESS_AES_HwFinish (unsigned char *pOutData, int *pOutDataSize);
DTV_STATUS_T DDI_DRM_FLASHACCESS_Get_HwRandomData (int digitToGen, unsigned char *outData);
unsigned long long DDI_DRM_FLASHACCESS_Get_SecuredClock (void);
int DDI_DRM_FLASHACCESS_Read_SecuredFile (char* pFileName, unsigned char* pDecryptedData, int readSize);
int DDI_DRM_FLASHACCESS_Write_SecuredFile (char* pFileName, unsigned char* pDecryptedData, int writeSize);
long int DDI_DRM_FLASHACCESS_Get_SecuredFileLength (char *pFileName);

/* MarlinDRM*/
DTV_STATUS_T DDI_DRM_MARLIN_SetTrafficKey (const void* trafficKey);

/* HDCP 2.0 DRM*/
DTV_STATUS_T DDI_HDCP2_SetEncLc128Key (int *pEncKey);
DTV_STATUS_T DDI_HDCP2_SetEncHdcp2Key (int *pEncHdcp2Key);
DTV_STATUS_T DDI_HDCP2_GetCertInfo (UINT8 *pReceiverID, UINT8 *pPubKey, UINT8 *pRootSign);
DTV_STATUS_T DDI_HDCP2_Decrypt_RSAES_OAEP (UINT8 *pEkpub_km);
DTV_STATUS_T DDI_HDCP2_Kd_Key_Derivation (UINT8 *pRtx);
DTV_STATUS_T DDI_HDCP2_Compute_H_Prime(UINT8 *pH_prime, UINT8 Repeater, UINT8 *pRtx);
DTV_STATUS_T DDI_HDCP2_Compute_L_Prime (UINT8 *pL_prime, UINT8 *pRn, UINT8 *pRrx);
DTV_STATUS_T DDI_HDCP2_Compute_Kh (void);
DTV_STATUS_T DDI_HDCP2_Encrypt_Km_using_Kh (UINT8 *pEkh_km, UINT8 *pM);
DTV_STATUS_T DDI_HDCP2_Decrypt_Km_using_Kh (UINT8 *pM, UINT8 *ekh_km);
DTV_STATUS_T DDI_HDCP2_Decrypt_EKs (UINT8 *pEks, UINT8 *pRtx, UINT8 *pRrx, UINT8 *pRn);
DTV_STATUS_T DDI_HDCP2_XOR_Ks_with_LC128 (void);
DTV_STATUS_T DDI_HDCP2_Data_Decrypt (UINT8 *pOutputbuf, UINT8 *pInputbuf, int length, UINT8 *pCounter);
DTV_STATUS_T DDI_HDCP2_WriteKeySet(UINT8 *pKeySet, UINT8 *dst_file_path, UINT32 KeyLength);
DTV_STATUS_T DDI_HDCP2_GetEncHdcp2Key(int *pEncHdcp2Key, UINT8 *src_file_path, UINT32 *EncKeyLength);
DTV_STATUS_T DDI_HDCP2_SHA256(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length);
DTV_STATUS_T DDI_HDCP2_AES_CTR(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pKey, int keyLength, UINT8 *pCounter);
DTV_STATUS_T DDI_HDCP2_Data_Encrypt(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pCounter);
DTV_STATUS_T DDI_HDCP2_Generate_Km(UINT8 *pMaskedDB, UINT8 *pDB, UINT8 *pDBmask);
DTV_STATUS_T DDI_HDCP2_Generate_Ks(UINT8 *pEks, UINT8 *pRrx, UINT8 *pRtx, UINT8 *pRn);
DTV_STATUS_T DDI_HDCP2_GetRootPublicKey (UINT8 *pRootPublicKey, UINT8 *src_file_path);
DTV_STATUS_T DDI_HDCP2_Select_StoredKm(UINT32 index);
DTV_STATUS_T DDI_HDCP2_Write_StoredKm(UINT32 index);

/* DivX DRM*/
DTV_STATUS_T DDI_DRM_DIVX_GetHWSecretKey(unsigned int outLength, unsigned char *pOutBuf);
DTV_STATUS_T DDI_DRM_DIVX_SetProcUseInfo(char *InfoPath, unsigned long fragmentNum, unsigned int bufferLength, unsigned char *pBuffer);
DTV_STATUS_T DDI_DRM_DIVX_GetProcUseInfo(char *InfoPath, unsigned long fragmentNum, unsigned int bufferLength, unsigned char *pBuffer);

#endif

#if (PLATFORM_TYPE == MTK_PLATFORM)
/* SVP DDI */ 
DTV_STATUS_T DDI_DRM_SVP_GetProvisionedData(char *pszPath, unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectData);
DTV_STATUS_T DDI_DRM_SVP_GenKeyPair_ECCP256(unsigned char *pPubKey, unsigned char *pPrivKey);
DTV_STATUS_T DDI_DRM_SVP_SetKey_ECCP256(unsigned char *pbKeyBuffer, unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN IsKeyProtected);
DTV_STATUS_T DDI_DRM_SVP_Sign_ECDSAP256(unsigned char *pbData, unsigned int cbData, unsigned char *pbSignature);
DTV_STATUS_T DDI_DRM_SVP_Decrypt_ECCP256(unsigned char *pbCipherText, unsigned char *pbPlainText, BOOLEAN bProtectPlainText);
DTV_STATUS_T DDI_DRM_SVP_SetKey_OMAC1(unsigned char *pbKeyBuffer, unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN IsKeyProtected);
DTV_STATUS_T DDI_DRM_SVP_Verify_OMAC1(unsigned char *pbData, unsigned int ibData, unsigned int cbData, unsigned char *pbSignature, unsigned int ibSignature);
DTV_STATUS_T DDI_DRM_SVP_Sign_OMAC1(unsigned char *pbData, unsigned int ibData, unsigned int cbData, unsigned char *pbSignature, BOOLEAN bProtectSignature);
DTV_STATUS_T DDI_DRM_SVP_CopyBytes(void *bpTo, unsigned int ibTo, void *pbFrom, unsigned int ibFrom, unsigned int cbFrom, unsigned int cbCount);
DTV_STATUS_T DDI_DRM_SVP_SetContentKey_AESCTR(unsigned char *pbKeyBuffer, unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN IsKeyProtected);
DTV_STATUS_T DDI_DRM_SVP_DecryptContent_AESCTR(void *pCtrContext, unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectContent);
DTV_STATUS_T DDI_DRM_SVP_SetKey_AESECB(unsigned char *pbKeyBuffer, unsigned int ibKeyBuffer, unsigned int cbKeyBuffer, BOOLEAN IsKeyProtected);
DTV_STATUS_T DDI_DRM_SVP_Encrypt_AESECB(unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectData);
DTV_STATUS_T DDI_DRM_SVP_Decrypt_AESECB(unsigned char *pbData, unsigned int cbData, BOOLEAN bProtectData);

/* DRM Common DDI */
DTV_STATUS_T DDI_DRM_GetSecureData (char *pDataPath, unsigned char *pData, unsigned int *pLength);
DTV_STATUS_T DDI_DRM_SetSecureData (char *pDataPath, unsigned char *pData, unsigned int length);
DTV_STATUS_T DDI_DRM_DeleteSecureData (char *pDataPath);
DTV_STATUS_T DDI_DRM_GetHWRandomData (unsigned char *pData, unsigned int digit);
DTV_STATUS_T DDI_DRM_GetSecureClock (unsigned long long *pSecureClock);
DTV_STATUS_T DDI_DRM_SetSecureClock (unsigned long long secureClock);
DTV_STATUS_T DDI_DRM_GetOriginalDataLength (char *pDataPath, unsigned int *pLength);
DTV_STATUS_T DDI_DRM_GetClearDataLength (char *pDataPath, unsigned int *pLength);
DTV_STATUS_T DDI_DRM_AESHWInit (int mode, unsigned char *pKey, unsigned char *pIV, int operation, int isPadding);
DTV_STATUS_T DDI_DRM_AESHWUpdate (unsigned char *pOutData, unsigned int *pOutDataSize, unsigned char *pInData, unsigned int nInDataSize);
DTV_STATUS_T DDI_DRM_AESHWFinish (unsigned char *pOutData, unsigned int *pOutDataSize);
DTV_STATUS_T DDI_DRM_GetHWID(unsigned char *pOutData, unsigned int *pOutDataSize);
DTV_STATUS_T DDI_DRM_IsSecureDataExist(char *pDataPath);

DTV_STATUS_T DDI_INIT_DRV(void);
#endif

