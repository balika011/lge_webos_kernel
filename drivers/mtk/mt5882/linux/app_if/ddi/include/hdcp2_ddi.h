 /******************************************************************************
  *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
  *   Copyright(c) 1999 by LG Electronics Inc.
  *
  *   All rights reserved. No part of this work may be reproduced, stored in a
  *   retrieval system, or transmitted by any means without prior written
  *   permission of LG Electronics Inc.
  *****************************************************************************/

/** @file 		hdcp2_ddi.h
 *
 *  header of HDCP2 DDI
 *
 *  @author		Kim, Suk (stan.kim@lge.com)
 *  @version	1.0
 *  @date		2011.8.
 *  @note		Additional information.
 *  @see
 */

#ifndef _HDCP2_DDI_H_
#define _HDCP2_DDI_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
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


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
// for HDCP2.0 RX
extern DTV_STATUS_T DDI_HDCP2_WriteKeySet(UINT8 *pKeySet);
extern DTV_STATUS_T DDI_HDCP2_GetCertInfo(UINT8 *pReceiverID, UINT8 *pPubKey, UINT8 *pRootSign);
extern DTV_STATUS_T DDI_HDCP2_Decrypt_RSAES_OAEP(UINT8 *pEKpubKm);
extern DTV_STATUS_T DDI_HDCP2_Kd_Key_Derivation(UINT8 *pRtx);
extern DTV_STATUS_T DDI_HDCP2_Compute_H_Prime(UINT8 *pHPrime, UINT8 repeater, UINT8 *pRtx);
extern DTV_STATUS_T DDI_HDCP2_Compute_L_Prime(UINT8 *pLPrime, UINT8 *pRn, UINT8 *pRrx);
extern DTV_STATUS_T DDI_HDCP2_Compute_Kh(void);
extern DTV_STATUS_T DDI_HDCP2_Encrypt_Km_using_Kh(UINT8 *pEKh_Km, UINT8 *pM);
extern DTV_STATUS_T DDI_HDCP2_Decrypt_Km_using_Kh(UINT8 *pM, UINT8 *pEKh_Km);
extern DTV_STATUS_T DDI_HDCP2_Decrypt_EKs(UINT8 *pEKs, UINT8 *pRtx, UINT8 *pRrx, UINT8 *pRn);
extern DTV_STATUS_T DDI_HDCP2_XOR_Ks_with_LC128(void);
extern DTV_STATUS_T DDI_HDCP2_Data_Decrypt(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pCounter);
extern DTV_STATUS_T DDI_HDCP2_SHA1(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length);

// for HDCP 2.0 TX
extern DTV_STATUS_T DDI_HDCP2_GetRootPublicKey(UINT8 *pKey);
extern DTV_STATUS_T DDI_HDCP2_SHA256(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length);
extern DTV_STATUS_T DDI_HDCP2_HMAC_SHA256(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pkey, int keylength);
extern DTV_STATUS_T DDI_HDCP2_AES_CTR(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pkey, int keylength, UINT8 *pCounter);
extern DTV_STATUS_T DDI_HDCP2_GetRivInfo(UINT8 *pRiv);
extern DTV_STATUS_T DDI_HDCP2_WriteRivInfo(UINT8 *pRiv);
extern DTV_STATUS_T DDI_HDCP2_Generate_Km(UINT8 *pMaskedDB, UINT8 *pDB, UINT8 *pDBMask);
extern DTV_STATUS_T DDI_HDCP2_Generate_Ks(UINT8 *pEKs, UINT8 *pRrx, UINT8 *pRtx, UINT8 *pRn);
extern DTV_STATUS_T DDI_HDCP2_Data_Encrypt(UINT8 *pOutputBuf, UINT8 *pInputBuf, int length, UINT8 *pCounter);
extern DTV_STATUS_T DDI_HDCP2_Write_StoredKm(UINT32 index);
extern DTV_STATUS_T DDI_HDCP2_Select_StoredKm(UINT32 index);
extern DTV_STATUS_T DDI_HDCP2_GetKeyForKs(UINT8 *pData);
extern DTV_STATUS_T DDI_HDCP2_GetKey(UINT8 *pData);



/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _HDCP2_DDI_H_ */

