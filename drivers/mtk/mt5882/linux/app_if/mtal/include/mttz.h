/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * $RCSfile: mttz.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: drm
 *---------------------------------------------------------------------------*/

/** @file mttz.h
 *  This header file declares exported APIs of CI module.
 */

#ifndef _MTTZ_H_
#define _MTTZ_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mttype.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTTZ_COMMON_ARG_LENGTH (16)
#define MTTZ_SECURESTORAGE_USERKEYLEN (128 / 8)
#define MTTZ_ULI_ENCRYPTED_POTK_SIZE  (16 + 52) /* 68 bytes */

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
/**
 * Common interface for transfer parameter between user/trustzone
 *
 */
typedef struct
{
    UINT32 ui4Command;                          /* Command */
    UINT8  ui1Size;                             /* Number of arguments */
    UINT8 *apvArgument[MTTZ_COMMON_ARG_LENGTH]; /* Array of argument pointer */
    UINT32 ui4Length[MTTZ_COMMON_ARG_LENGTH];   /* Array of argument length */
    BOOL   abWrite[MTTZ_COMMON_ARG_LENGTH];     /* Array of argument write back */
} MTTZ_COMMON_ARG_T;

#define MTTZ_AESCTR_DECRYPT_DATA_IDX        1
#define MTTZ_AESCTR_DECRYPT_CMD_SIZE        3
#define MTTZ_AESCTR_DECRYPT_MULTI_CMD_SIZE  5
#define MTTZ_AESCTR_DECRYPT_MULTI_CMD       0x630f

/**
 * SecureStorage key.
 *
*/
typedef struct
{
    unsigned char au1EncKey[MTTZ_SECURESTORAGE_USERKEYLEN];
} MTTZ_SECURESTORAGE_USERKEY_T;

/**
 * DRM_AESCTR_CONTEXT
 */
typedef struct
{
    UINT64  qwInitializationVector;
    UINT64  qwBlockOffset;
    BYTE    bByteOffset;  /* Byte offset within the current block */
} MTTZ_PLAYREADY_AES_COUNTER_MODE_CONTEXT_T;

typedef struct
{
    UINT64  qwInitializationVector;
	UINT64  qwCounter;
    UINT64  qwBlockOffset;
    BYTE    bByteOffset;  /* Byte offset within the current block */
} MTTZ_PLAYREADYDASH_AES_COUNTER_MODE_CONTEXT_T;

/**
 * ULI encrypted POTK
 */
typedef struct
{
    UINT8 au1_key[MTTZ_ULI_ENCRYPTED_POTK_SIZE];
} MTTZ_ULI_ENCRYPTED_POTK_T;

/**
 * DTCP
 */
typedef struct
{
    UINT32 u4KeyIdx;        ///< The key index
    UINT8 *pu1EncKey;       ///< Pointer to encrypted local key
    UINT32 u4EncKeyLen;     ///< The length of encrypted key 
    UINT8 *pu1Iv;           ///< Pointer to IV
    UINT32 u4IvLen;         ///< The length of IV
} MTTZ_DTCP_CRYPTO_INFO_T;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
/*-----------------------------------------------------------------------------
 * Name: MTTZ_Common_Operate
 *
 * Description: This API provides a common operation for TrustZone using.
 *
 * Inputs: ptCommon   Reference to common arguments
 *
 * Outputs: ptCommon   Reference to common arguments
 *
 * Returns: 0       Success.
 *          Others  Fail.
 *
 * Sample code:
 *   UINT8 srcBuf[128];
 *   UINT8 dstBuf[128];
 *   MTTZ_COMMON_ARG_T tCommon;
 *
 *   MTAL_Init();
 *
 *   tCommon.ui4Command = 0x6000;
 *   tCommon.ui1Size = 2;
 *   tCommon.apvArgument[0] = srcBuf;
 *   tCommon.ui4Length[0] = 128;
 *   tCommon.abWrite[0] = true;
 *   tCommon.apvArgument[1] = dstBuf;
 *   tCommon.ui4Length[1] = 128;
 *   tCommon.abWrite[1] = true;
 *
 *   MTTZ_Common_Operate(&tCommon);
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Common_Operate (MTTZ_COMMON_ARG_T * ptCommon);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_MSL_Operate
 *
 * Description: This API provides a MSL operation for Netflix 4.1 with TrustZone using.
 *
 * Inputs: ptCommon   Reference to common arguments
 *         
 * Outputs: ptCommon   Reference to common arguments
 *
 * Returns: 0       Success. 
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_MSL_Operate (MTTZ_COMMON_ARG_T * ptCommon);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DTCP_Operate
 *
 * Description: This API provides DTCP operations.
 *
 * Inputs: ptCommon   Reference to common arguments
 *         
 * Outputs: ptCommon   Reference to common arguments
 *
 * Returns: 0       Success. 
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DTCP_Operate (MTTZ_COMMON_ARG_T * ptCommon);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_Decrypt_Init
 *
 * Description: This API provides an initial operation for AES decrypt progress.
 *
 * Inputs: ptCommon   Reference to common arguments
 *
 * Outputs: ptCommon   Reference to common arguments
 *
 * Returns: 0       Success.
 *          Others  Fail.
 *
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Decrypt_Init (MTTZ_COMMON_ARG_T * ptCommon);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_Decrypt_Operate
 *
 * Description: This API provides a common operation for AES decryption operation.
 *
 * Inputs: ptCommon   Reference to common arguments
 *
 * Outputs: ptCommon   Reference to common arguments
 *
 * Returns: 0       Success.
 *          Others  Fail.
 *
 * Sample code:
 *   UINT8 srcBuf[128];
 *   UINT8 dstBuf[128];
 *   MTTZ_COMMON_ARG_T tCommon;
 *
 *   MTAL_Init();
 *
 *   tCommon.ui4Command = 0x6000;
 *   tCommon.ui1Size = 2;
 *   tCommon.apvArgument[0] = srcBuf;
 *   tCommon.ui4Length[0] = 128;
 *   tCommon.abWrite[0] = true;
 *   tCommon.apvArgument[1] = dstBuf;
 *   tCommon.ui4Length[1] = 128;
 *   tCommon.abWrite[1] = true;
 *
 *   MTTZ_Common_Operate(&tCommon);
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Decrypt_Operate (MTTZ_COMMON_ARG_T * ptCommon);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_Decrypt_Deinit
 *
 * Description: This API provides an de-initial operation for AES decrypt progress.
 *
 * Inputs: ptCommon   Reference to common arguments
 *
 * Outputs: ptCommon   Reference to common arguments
 *
 * Returns: 0       Success.
 *          Others  Fail.
 *
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Decrypt_Deinit (MTTZ_COMMON_ARG_T * ptCommon);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_Init
 *
 * Description: This API provides an Init operation for AES TZ.
 *
 * Inputs: None
 *
 * Outputs: None
 *
 * Returns: 0       Success.
 *          Others  Fail.
 *
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Init (void);


/*-----------------------------------------------------------------------------
 * Name: MTTZ_SECURESTORAGE_Encrypt
 *
 * Description: This API encrypts the secure storage content based on memory.
 *
 * Inputs: ptUserKey   Reference to user key
 *         pu1SrcBuf   Reference to the content to be encrypted.
 *         lSrcLen     Size of the content to be encrypted.
 *         pu1DstBuf   Reference to retrieve the encrypted content.
 *         plDstLen    Reference to size of the encrypted content buffer.
 *
 * Outputs: pu1DstBuf  Reference to the encrypted content.
 *          plDstLen   Reference to size of the encrypted content.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_SECURESTORAGE_Encrypt (MTTZ_SECURESTORAGE_USERKEY_T *ptUserKey, UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_SECURESTORAGE_Decrypt
 *
 * Description: This API decrypts the secure storage content based on memory.
 *
 * Inputs: ptUserKey   Reference to user key
 *         pu1SrcBuf   Reference to the content to be decrypted.
 *         lSrcLen     Size of the content to be decrypted.
 *         pu1DstBuf   Reference to retrieve the decrypted content.
 *         plDstLen    Reference to size of the decrypted content buffer.
 *
 * Outputs: pu1DstBuf  Reference to the decrypted content.
 *          plDstLen   Reference to size of the decrypted content.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------**/
MT_RESULT_T MTTZ_SECURESTORAGE_Decrypt (MTTZ_SECURESTORAGE_USERKEY_T *ptUserKey, UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DRM_GetSecureData
 *
 * Description: This API read a LGEA1 pre-encrypted file and return clear data.
 *
 * Inputs: pszPath      Path/filename of provisioned data.
 *         cbData       Size of data buffer.
 *
 * Outputs: pbData      Reference to data buffer pointer. (protected)
 *          cbData      Size of data
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DRM_GetSecureData(INT8 *pszPath, UINT8 *pbData, UINT32 *cbData);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_BaseUT
 *
 * Description: This API run base unit test.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_BaseUT(UINT8 *pu1Buf, UINT32 u4Len);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_GetKFKEY
 *
 * Description: This API read a file and return protected data.
 *
 * Inputs: pszPath      Path/filename of provisioned data.
 *         cbData       Size of data buffer.
 *         bProtectData Indicate that pbData need to protect or not.
 *
 * Outputs: pbData      Reference to data buffer pointer. (protected)
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_GetKFKEY(INT8 *pszPath, UINT8 *pbData, UINT32 cbData, BOOL bProtectData);


/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_GetProvisionedData
 *
 * Description: This API read a file and return protected data.
 *
 * Inputs: pszPath      Path/filename of provisioned data.
 *         cbData       Size of data buffer.
 *         bProtectData Indicate that pbData need to protect or not.
 *
 * Outputs: pbData      Reference to data buffer pointer. (protected)
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_GetProvisionedData(INT8 *pszPath, UINT8 *pbData, UINT32 cbData, BOOL bProtectData);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_SetKey_OMAC1
 *
 * Description: This API read a file and return protected data.
 *
 * Inputs: pbKeyBuffer      Key Buffer including  AES 128 key.
 *         ibKeyBuffer      Index of Actual Key.
 *         cbKeyBuffer      Size of whole Key Buffer.
 *         IsKeyProtected   Indicate that Key Buffer is protected or not.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_SetKey_OMAC1(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Verify_OMAC1
 *
 * Description: This API verify via OMAC1.
 *
 * Inputs: pbData      Signed Data Buffer.
 *         ibData      The starting byte offset of the data to verify.
 *         cbData      Size of whole Key Buffer.
 *         pbSignature Signature
 *         ibSignature The starting byte offset of the signature to verify.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Verify_OMAC1(UINT8 *pbData, UINT32 ibData, UINT32 cbData, UINT8 *pbSignature, UINT32 ibSignature);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Sign_OMAC1
 *
 * Description: This API sign via OMAC1.
 *
 * Inputs: pbData               The data to sign.
 *         ibData               The starting byte offset of the data to sign.
 *         cbData               The number of bytes to sign.
 *         bProtectSignature    Indicate signature need to protect or not.
 *
 * Outputs: pbSignature Signature
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Sign_OMAC1(UINT8 *pbData, UINT32 ibData, UINT32 cbData, UINT8 *pbSignature, BOOL bProtectSignature);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_SetContentKey_AESCTR
 *
 * Description: This API set key for AESCTR.
 *
 * Inputs: pbKeyBuffer          Key Buffer including  AES 128 key.
 *         ibKeyBuffer          Index of Actual Key.
 *         cbKeyBuffer          Size of whole Key Buffer.
 *         IsKeyProtected       Indicate that Key Buffer is protected or not.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_SetContentKey_AESCTR(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_SetContentKey_AESCTR_Audio
 *
 * Description: This API set key for AESCTR.
 *
 * Inputs: pbKeyBuffer          Key Buffer including  AES 128 key.
 *         ibKeyBuffer          Index of Actual Key.
 *         cbKeyBuffer          Size of whole Key Buffer.
 *         IsKeyProtected       Indicate that Key Buffer is protected or not.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_SetContentKey_AESCTR_Audio(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_DecryptContent_AESCTR
 *
 * Description: This API decrypt via AESCTR.
 *
 * Inputs: pCtrContext          AES CTR Context.
 *         pbData               Encrypted content.
 *         cbData               Size of encrypted content.
 *         bProtectContent      Indicate content need to protect or not.
 *
 * Outputs: pCtrContext         AES CTR Context.
 *          pbData              Encrypted content.
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_DecryptContent_AESCTR(void *pCtrContext, UINT8 *pbData, UINT32 cbData, BOOL bProtectSignature);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_DecryptMultiContent_AESCTR
 *
 * Description: This API decrypt multi segments via AESCTR.
 *
 * Inputs: pCtrContext          AES CTR Context.
 *         pbData               Encrypted content.
 *         cbData               Size of encrypted content.
 *         au4Seg               Segment information array.
 *         cu4Seg               Size of Segment information array.
 *         bProtectContent      Indicate content need to protect or not.
 * 
 * Outputs: pCtrContext         AES CTR Context.
 *          pbData              Encrypted content.
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_DecryptMultiContent_AESCTR(void *pCtrContext, UINT8 *pbData, UINT32 cbData, UINT32 *au4Seg, UINT32 cu4Seg, BOOL bProtectSignature);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_DecryptDashSample_AESCTR
 *
 * Description: This API decrypt multi Dash Sample segments via AESCTR.
 *
 * Inputs: pCtrContext          AES CTR Context.
 *         pbData               Encrypted content.
 *         cbData               Size of encrypted content.
 *         au4Seg               Segment information array.
 *         cu4Seg               Size of Segment information array.
 *         bProtectContent      Indicate content need to protect or not.
 * 
 * Outputs: pCtrContext         AES CTR Context.
 *          pbData              Encrypted content.
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_DecryptDashSample_AESCTR(void *pCtrContext, UINT8 *pbData, UINT32 cbData, UINT32 *au4Seg, UINT32 cu4Seg, BOOL bProtectSignature);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_DecryptDashSample_AESCTR_Audio
 *
 * Description: This API decrypt multi Dash Sample segments via AESCTR.
 *
 * Inputs: pCtrContext          AES CTR Context.
 *         pbData               Encrypted content.
 *         cbData               Size of encrypted content.
 *         au4Seg               Segment information array.
 *         cu4Seg               Size of Segment information array.
 *         bProtectContent      Indicate content need to protect or not.
 * 
 * Outputs: pCtrContext         AES CTR Context.
 *          pbData              Encrypted content.
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_DecryptDashSample_AESCTR_Audio(void *pCtrContext, UINT8 *pbData, UINT32 cbData, UINT32 *au4Seg, UINT32 cu4Seg, BOOL bProtectSignature);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_GenKeyPair_ECCP256
 *
 * Description: This API generate ECCP256 key pair.
 *
 * Inputs:
 *
 * Outputs:
 *         pbPubKey			ECC P256 Public Key
 *         pbPrivKey			ECC P256 Private Key (protected)
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_GenKeyPair_ECCP256(UINT8 *pbPubKey, UINT8 *pbPrivKey, BOOL bEncKey);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_SetKey_ECCP256
 *
 * Description: This API set ECCP256 key to TZ
 *
 * Inputs:
 *		pbKeyBuffer		Key Buffer including  ECC P256 key
 *		ibKeyBuffer		Index of Actual Key
 *		cbKeyBuffer		Size of whole Key Buffer
 *		IsKeyProtected	Indicate that Key Buffer is protected or not
 *
 * Outputs:
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_SetKey_ECCP256(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Sign_ECDSAP256
 *
 * Description: This API generate ECDSAP256 signature
 *
 * Inputs:
 *		pbData
 *		cbData
 *
 * Outputs:
 *		pbSignature
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Sign_ECDSAP256(UINT8 *pbData, UINT32 cbData, UINT8 * pbSignature);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Decrypt_ECCP256
 *
 * Description: This API decrypt cipher text
 *
 * Inputs:
 *		pbCipherText			ECC P256 Cipher Text
 *		bProtectPlainText		Indicate Plain Text need to protect or not
 *
 * Outputs:
 *		pbPlainText			ECC P256 Plain Text
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Decrypt_ECCP256(UINT8 *pbCipherText, UINT8 * pbPlainText, BOOL bProtectPlainText);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_LinkBytes
 *
 * Description: This API Link byte pbFrom1 and pbfrom2 to bpto
 *
 * Inputs: 
 *		ibTo			Index of destination buffer to include portion of protected data
 *		pbFrom		Source buffer which is including protected data
 *		ibFrom		Index of  actual data which is wanted to copy 
 *		cbFrom 		Size of whole source buffer
 *		cbCount		Size of want to copy
 * 
 * Outputs: 
 *		bpTo			Destination buffer to include portion of protected data 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_LinkBytes(UINT8 *bpTo, UINT32 ibTo, UINT8 * pbFrom1, UINT32 cbFrom1, UINT8 * pbFrom2, UINT32 cbFrom2);


/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_CopyBytes
 *
 * Description: This API copy byte from pbForm to pbTo
 *
 * Inputs:
 *		ibTo			Index of destination buffer to include portion of protected data
 *		pbFrom		Source buffer which is including protected data
 *		ibFrom		Index of  actual data which is wanted to copy
 *		cbFrom 		Size of whole source buffer
 *		cbCount		Size of want to copy
 *
 * Outputs:
 *		bpTo			Destination buffer to include portion of protected data
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_CopyBytes(UINT8 *bpTo, UINT32 ibTo, UINT8 * pbFrom, UINT32 ibFrom, UINT32 cbFrom, UINT32 cbCount);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_SetKey_AESECB
 *
 * Description: This API config AESECB key
 *
 * Inputs:
 *		pbKeyBuffer		Key Buffer including  AES 128 key
 *		ibKeyBuffer		Index of Actual Key
 *		cbKeyBuffer		Size of whole Key Buffer
 *		IsKeyProtected	Indicate that Key Buffer is protected or not
 *
 * Outputs:
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_SetKey_AESECB(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Encrypt_AESECB
 *
 * Description: This API config AESECB key
 *
 * Inputs:
 *		cbData			Size of Data
 *		bProtectData		Indicate that encrypted data need to protect or not
 *
 * Outputs:
 *		pbData			Data to encrypt
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Encrypt_AESECB(UINT8 *pbData, UINT32 cbData,  BOOL bProtectData);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Decrypt_AESECB
 *
 * Description: This API config AESECB key
 *
 * Inputs:
 *		cbData			Size of Data
 *		bProtectData		Indicate that decrypted data need to protect or not
 *
 * Outputs:
 *		pbData			Data to encrypt
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Decrypt_AESECB(UINT8 *pbData, UINT32 cbData, BOOL bProtectData);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_SetKey_AESCBC
 *
 * Description: This API config AESCBC key
 *
 * Inputs:
 *		pbKeyBuffer		Key Buffer including  AES 128 key
 *		ibKeyBuffer		Index of Actual Key
 *		cbKeyBuffer		Size of whole Key Buffer
 *		IsKeyProtected	Indicate that Key Buffer is protected or not
 *
 * Outputs:
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_SetKey_AESCBC(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Encrypt_AESCBC
 *
 * Description: This API config AESCBC key
 *
 * Inputs:
 *		cbData			Size of Data
 *		bProtectData		Indicate that encrypted data need to protect or not
 *
 * Outputs:
 *		pbData			Data to encrypt
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Encrypt_AESCBC(UINT8 *pbData, UINT32 cbData, UINT8* IV,UINT32 IVLen, BOOL bProtectData);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Decrypt_AESCBC
 *
 * Description: This API config AESECB key
 *
 * Inputs:
 *		cbData			Size of Data
 *		bProtectData		Indicate that decrypted data need to protect or not
 *
 * Outputs:
 *		pbData			Data to encrypt
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Decrypt_AESCBC(UINT8 *pbData, UINT32 cbData,UINT8* IV,UINT32 IVLen, BOOL bProtectData);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_Shuffle_ContentKey
 *
 * Description: This API shuffle byte from pbForm to pbTo
 *
 * Inputs:
 *		ibTo			Index of destination buffer to include portion of protected data
 *		pbFrom		Source buffer which is including protected data
 *		ibFrom		Index of  actual data which is wanted to copy
 *		cbFrom 		Size of whole source buffer
 *		cbCount		Size of want to copy
 *
 * Outputs:
 *		bpTo			Destination buffer to include portion of protected data
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_Shuffle_ContentKey(UINT8 *bpTo, UINT32 ibTo, UINT8 * pbFrom, UINT32 ibFrom, UINT32 cbFrom, UINT32 cbCount);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_PLAYREADY_PrepareContentKey_Prime
 *
 * Description: This API prepare the contentkey prime using byte from pbForm to pbTo
 *
 * Inputs:
 *		ibTo			Index of destination buffer to include portion of protected data
 *		pbFrom		Source buffer which is including protected data
 *		ibFrom		Index of  actual data which is wanted to copy
 *		cbFrom 		Size of whole source buffer
 *		cbCount		Size of want to copy
 *
 * Outputs:
 *		bpTo			Destination buffer to include portion of protected data
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_PLAYREADY_PrepareContentKey_Prime(UINT8 *bpTo, UINT32 ibTo, UINT8 * pbFrom, UINT32 ibFrom, UINT32 cbFrom, UINT32 cbCount);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_Nautilus_SetContentKey_AESCTR
 *
 * Description: This API set key for AESCTR.
 *
 * Inputs: pbKeyBuffer          Key Buffer including  AES 128 key.
 *         ibKeyBuffer          Index of Actual Key.
 *         cbKeyBuffer          Size of whole Key Buffer.
 *         IsKeyProtected       Indicate that Key Buffer is protected or not.
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Nautilus_SetContentKey_AESCTR(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_Nautilus_SetContentKey_AESCTR_Audio
 *
 * Description: This API set key for AESCTR.
 *
 * Inputs: pbKeyBuffer          Key Buffer including  AES 128 key.
 *         ibKeyBuffer          Index of Actual Key.
 *         cbKeyBuffer          Size of whole Key Buffer.
 *         IsKeyProtected       Indicate that Key Buffer is protected or not.
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Nautilus_SetContentKey_AESCTR_Audio(UINT8 *pbKeyBuffer, UINT32 ibKeyBuffer, UINT32 cbKeyBuffer, BOOL IsKeyProtected);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_Nautilus_DecryptContent_Init
 *
 * Description: This API init the decrypt progress
 *
 * Inputs: None
 * 
 * Returns: MTR_OK  Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Nautilus_DecryptContent_Init(void);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_Nautilus_DecryptContent_DeInit
 *
 * Description: This API deinit the decrypt progress
 *
 * Inputs: None
 * 
 * Returns: MTR_OK  Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/

#if 1
MT_RESULT_T MTTZ_PLAYREADY_Antirollback_Loadin(char* anti_clock , int length);

MT_RESULT_T MTTZ_PLAYREADY_Antirollback_Update(unsigned long time, char* anti_clock , int length);
MT_RESULT_T MTTZ_PLAYREADY_Antirollback_GetTime(unsigned long *time);


#ifndef TEE_SECRET_KEY_DEFINED
#define TEE_SECRET_KEY_DEFINED

#ifndef TEE_MAX_SIGNATURE_SIZE
#define TEE_MAX_SIGNATURE_SIZE                   256 // ECDSA-SHA256 signature
#endif


typedef struct {
    int id;
} TEE_SECRET_KEY;

typedef struct {
    unsigned char data[TEE_MAX_SIGNATURE_SIZE];
    int length;
} TEE_SIGNATURE;

#endif
MT_RESULT_T MTTZ_SECURESTOP_add_session( unsigned char *  session_id , int length);

MT_RESULT_T MTTZ_Nautilus_DecryptContent_DeInit(void);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_Nautilus_DecryptDashSample_AESCTR
 *
 * Description: This API decrypt multi Dash Sample segments via AESCTR.
 *
 * Inputs: pCtrContext          AES CTR Context.
 *         pbData               Encrypted content.
 *         cbData               Size of encrypted content.
 *         au4Seg               Segment information array.
 *         cu4Seg               Size of Segment information array.
 *         bProtectContent      Indicate content need to protect or not.
 * 
 * Outputs: pCtrContext         AES CTR Context.
 *          pbData              Encrypted content.
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Nautilus_DecryptDashSample_AESCTR(void *pCtrContext, UINT8 *pbData, UINT32 cbData, UINT32 *au4Seg, UINT32 cu4Seg, BOOL bProtectSignature);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_Nautilus_DecryptDashSample_AESCTR_Audio
 *
 * Description: This API decrypt multi Dash Sample segments via AESCTR.
 *
 * Inputs: pCtrContext          AES CTR Context.
 *         pbData               Encrypted content.
 *         cbData               Size of encrypted content.
 *         au4Seg               Segment information array.
 *         cu4Seg               Size of Segment information array.
 *         bProtectContent      Indicate content need to protect or not.
 * 
 * Outputs: pCtrContext         AES CTR Context.
 *          pbData              Encrypted content.
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_Nautilus_DecryptDashSample_AESCTR_Audio(void *pCtrContext, UINT8 *pbData, UINT32 cbData, UINT32 *au4Seg, UINT32 cu4Seg, BOOL bProtectSignature);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_CILPLUS_Extract_Cred
 *
 * Description: This API decrypt and extract all CI + Key raw data. 
 *
 * Inputs: 
 *            pbCredKey            Buffer including  CI + Key encryped by packing tool and securestorage mechanism  (read from securestorage file without decry )
 *            cbCredLen            
 * Outputs: 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/

MT_RESULT_T MTTZ_CILPLUS_Extract_Cred(UINT8 *pbCred,UINT32 cbCredLen,UINT8 *pbCustCode,UINT32 cbCustCodeLen);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_CILPLUS_Verify_Cred
 *
 * Description: This API verify the pbCred key match the ref or not
 *
 * Inputs: 
 *            pbCred            Buffer including  CI + Key encryped by packing tool and securestorage mechanism  (read from securestorage file without decry )
 *            cbCredLen      
 *            pbCredRef       Buffer including  CI +reference Key encryped by packing tool  
 *            cbCredLen    
 * Outputs: pbValid   Key buffered in pbCred is valid
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/

MT_RESULT_T MTTZ_CILPLUS_Verify_Cred(UINT8 *pbCred,UINT32 cbCredLen,UINT8 *pbCredRef,UINT32 cbCredRefLen,BOOL* pbValid);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_CILPLUS_Set_Encry_AKH
 *
 * Description: This API set encrypted AKH data . (AKH data from Perm file, protected by ss)
 *
 * Inputs: 
 *            pbAkh            Buffer including  AKH data encryped by securestorage mechanism 
 *                                  (read from securestorage file without decry )
 *            cbAkhLen            
 * Outputs: 
 * 
 * Returns: 0            Success. 
 *              Others     Fail.  
 ----------------------------------------------------------------------------*/

MT_RESULT_T MTTZ_CILPLUS_Set_Encry_AKH(UINT8 *pbAkh,UINT32 cbAkhLen);
/*-----------------------------------------------------------------------------
 * Name: MTTZ_CILPLUS_Get_Encry_AKH
 *
 * Description: This API get encrypted AKH data.(AKH data from TZ, will be burned to perm out of TZ)
 *
 * Inputs: 
 *            pbAkh            Buffer including  AKH data encryped by securestorage mechanism 
 *                             (read from securestorage file without decry )
 *            cbAkhLen            
 * Outputs: 
 * 
 * Returns: 0            Success. 
 *              Others     Fail.  
 ----------------------------------------------------------------------------*/

MT_RESULT_T MTTZ_CILPLUS_Get_Encry_AKH(UINT8 *pbAkh,UINT32* pbAkhLen);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_CILPLUS_Check_Cred
 *
 * Description: This API check the burned key by custom code. 
 *
 * Inputs: 
 *            pbCred            Buffer including  CI + Key encryped by packing tool and securestorage mechanism  (read from securestorage file without decry )
 *            cbCredLen      
 *            pbCustCode         Buffer including customer code
 *            cbCustCodeLen     
 * Outputs: pbexist   key valid ,cutomer code ,match
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_CILPLUS_Check_Cred(UINT8 *pbCred,UINT32 cbCredLen,UINT8 *pbCustCode,UINT32 cbCustCodeLen,BOOL *pbexist);

typedef UINT32 CI_CC_RESULT_T;
#define CI_CC_OK                      ((CI_CC_RESULT_T)    0)    /**< The routine is successful     */
#define CI_CC_FAILED                  ((CI_CC_RESULT_T)    -1)    /**<The routine is failed         */
#define CI_CC_INV_ARG                 ((CI_CC_RESULT_T)    -2)    /**< Indicates at least one argument is invalid. */
#define CI_CC_CERT_EXPIRED            ((CI_CC_RESULT_T)    -3)    /**< Indicates at certificate is expired. */
#define CI_CC_CERT_ERROR              ((CI_CC_RESULT_T)    -4)    /**< Indicates at certificate has error. */
#define CI_CC_NOT_ENOUGH_SPACE        ((CI_CC_RESULT_T)    -5)    /**< Indicates at input buffer size is not enough */

/***************************************************************************
    Description:
        To Initialize the CI PLus DRM Context.

    Input Parameters:
        None
    Output Parameters:
        drm_handle: DRM Context Handle

    Return Value:
        CI_CC_INV_ARG: input drm_handle is NULL
        CI_CC_NOT_ENOUGH_SPACE:memory is not enough
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully
**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Init(UINT32 *drm_handle);


/***************************************************************************
    Description:
        Removes the CI PLus DRM Context.

    Input Parameters:
        drm_handle: DRM Context Handle

    Output Parameters:
        Nil
    Return Value:    
        CI_CC_INV_ARG: input drm_handle is NULL
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Remove(UINT32 drm_handle);



/***************************************************************************
    Description:
        Gets the content of Host Certificate in Memory Buffer.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_cert_len:dev_certificate buffer length    

    Output Parameters:
        dev_certificate: Memory Buffer Pointer for Device Certificate
        dev_cert_len: actual Device Certificate Length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or dev_certificate,dev_cert_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:dev_cert_len is not enough, in this case dev_cert_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_HostDevCert(UINT32 drm_handle, UINT8 *dev_certificate, UINT16 *dev_cert_len);

/***************************************************************************
    Description:
        Gets the content of Host Brand Certificate in Memory Buffer.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_brand_cert_len:dev_brand_certificate buffer length

    Output Parameters:
        dev_brand_certificate: Memory Buffer Pointer for Brand Certificate
        dev_brand_cert_len: actual Brand Certificate Length
   Return Value:
       CI_CC_INV_ARG: input drm_handle is not valid or dev_brand_certificate,dev_brand_cert_len is NULL
       CI_CC_NOT_ENOUGH_SPACE:dev_cert_len is not enough, in this case dev_brand_cert_len will be output to actual needed buffer size
       CI_CC_FAILED:failed
       CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_HostBrandCert(UINT32 drm_handle, UINT8 *dev_brand_certificate, UINT16 *dev_brand_cert_len);

/***************************************************************************
    Description:
        Set TS UTC time.

    Input Parameters:
        drm_handle: DRM Context Handle
        epoch_time:TS UTC time
    Output Parameters:
        Nil
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid 
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Set_UTC_Time(UINT32 drm_handle,UINT32 epoch_time);

/***************************************************************************
    Description:
        Verifies the CICAM Module and Brand Certificate with Root Certificate.

    Input Parameters:
        drm_handle: DRM Context Handle
        dev_certificate: Buffer having Module Certificate
        dev_cert_len: Module Certificate Length
        dev_brand_certificate: Buffer having Brand Certificate
        dev_brand_cert_len: Brand Certificate Length

    Output Parameters:
        Nil
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or dev_certificate,dev_brand_certificate is NULL
        CI_CC_CERT_EXPIRED:CAM certificate expired
        CI_CC_CERT_ERROR:CAM cerdificate has error
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Verify_ModuleCert(UINT32 drm_handle, UINT8 *dev_certificate, UINT32 dev_cert_len, UINT8 *dev_brand_certificate, UINT32 dev_brand_cert_len);


/***************************************************************************
    Description:
        To sign a Message.

    Input Parameters:
        drm_handle: DRM Handle Context
        auth_nonce: Random Number
       auth_nonce_len:auth_nonce buffer length       
       Signlen: input SingMsg buffer length

    Output Parameters:
        SignMsg: Signed Message
        Signlen: Signed Message Length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or auth_nonce,SignMsg ,Signlen is NULL
        CI_CC_NOT_ENOUGH_SPACE:Signlen is not enough, in this case Signlen will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Sign_Message(UINT32 drm_handle, UINT8 *auth_nonce, UINT16 auth_nonce_len,UINT8 *SignMsg, UINT16 *Signlen);


/***************************************************************************
    Description:
        To Verify the Signature Message.

    Input Parameters:
        drm_handle: DRM Handle Context
        DHPM: CICAM DHPM
        DHPM_len:DHPM buffer length
        SignMsg: Signed Message
        Signlen: Signed Message Length

    Output Parameters:
        Nil
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or DHPM,SignMsg is NULL
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Verify_SignatureB(UINT32 drm_handle, UINT8 *DHPM,UINT16 DHPM_len, UINT8* SignMsg, UINT16 Signlen);


/***************************************************************************
    Description:
        To Get the DHPH.

    Input Parameters:
        drm_handle: DRM Handle Context
        DHPH_len:DHPH buffer length

    Output Parameters:
        DHPH: DHPH of Host
        DHPH_len:actual DHPH data length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or DHPH is NULL
        CI_CC_NOT_ENOUGH_SPACE:DHPH_len is not enough, in this case DHPH_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_Dhph(UINT32 drm_handle, UINT8* DHPH,UINT16 *DHPH_len);


/***************************************************************************
    Description:
        Get the Host Id from Host Device Certificate.

    Input Parameters:
        drm_handle: DRM Handle Context
        host_id_len:host_id buffer length

    Output Parameters:
        host_id: Host id from Host Certificate
        host_id_len:actual host_id data len
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or host_id,host_id_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:host_id_len is not enough, in this case host_id_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_HostId(UINT32 drm_handle, UINT8 *host_id,UINT16 *host_id_len);

/***************************************************************************
    Description:
        Get the CAM Id from CAM Device Certificate.

    Input Parameters:
        drm_handle: DRM Handle Context
        cam_id_len:host_id buffer length

    Output Parameters:
        cam_id: CAM id from CAM Certificate
        cam_id_len:actual cam_id data len
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or host_id,host_id_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:cam_id_len is not enough, in this case cam_id_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_CamId(UINT32 drm_handle, UINT8 *cam_id,UINT16* cam_id_len);

CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_CamBrandId(UINT32 drm_handle,UINT16 *brand_id);

/***************************************************************************
    Description:
        Generates DHSK and then finally AKH.

    Input Parameters:
        drm_handle: DRM Handle Context
        AKH_len: akh buffer length

    Output Parameters:
        AKH: AKH Generated
        AKH_len:actual AKH data length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or AKH,AKH_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:AKH_len is not enough, in this case AKH_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Generate_Akh(UINT32 drm_handle, UINT8 *AKH,UINT16 *AKH_len);

CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Generate_DHSK(UINT32 drm_handle);

/***************************************************************************
    Description:
        Generates SAC Keys.

    Input Parameters:
        drm_handle: DRM handle Context
        Ns_Module: Random Number from Module
        Ns_Module_len:Ns_Module buffer length
        CICAM_ID: CI Cam Id
        CICAM_ID_len:CICAM_ID buffer length
        
    Output Parameters:
        Nil
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or Ns_Module,CICAM_ID is NULL
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Generate_SacKeys(UINT32 drm_handle,UINT8 *Ns_Module, UINT16 Ns_Module_len,UINT8 *CICAM_ID,UINT16 CICAM_ID_len);


/***************************************************************************
    Description:
        Generates 64 Bits Random Numbers.

    Input Parameters:
        drm_handle: DRM Handle Context
        Ns_Host_len:Ns_Host buffer length

    Output Parameters:
        Ns_Host: random Number Generated
       Ns_Host_len:actual Ns_Host data length
    Return Value:
        CI_CC_INV_ARG: input drm_handle is not valid or Ns_Host,Ns_Host_len is NULL
        CI_CC_NOT_ENOUGH_SPACE:Ns_Host_len is not enough, in this case Ns_Host_len will be output to actual needed buffer size
        CI_CC_FAILED:failed
        CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_Ns(UINT32 drm_handle, UINT8 *Ns_Host,UINT16 *Ns_Host_len);


/***************************************************************************
    Description:

This function is used to encrypt and sign a SAC message to send it to the CICAM over the SAC.
After use, the encrypted message memory should be freed by the caller using free().

Input Parameters:
drm_handle: DRM Handle Context
clearmsg Pointer to buffer holding the clear message
clear_msg_len pointer to length of clear message
enc_msg_len: enc_msg buffer length

Output Parameters:
enc_msg  pointer to the encrypted and signed message.
enc_msg_len actual encrypted and signed message length
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or clearmsg,enc_msg ,enc_msg_len is NULL
    CI_CC_NOT_ENOUGH_SPACE:enc_msg_len is not enough, in this case enc_msg_len will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully

**************************************************************************/

CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Encrypt_MessageSac(UINT32 drm_handle,UINT8* clearmsg, UINT16 clear_msg_len, UINT8 *enc_msg, UINT16 *enc_msg_len);

/***************************************************************************
    Description:

This function is used to authenticate and decrypt a SAC message and return the decrypted message contents.
If the SAC message contains the Key precursor Kp used for CCK derivation, the decrypted message is not output. 
Instead the CCK operation will be processed. In this case clearmsg/clear_msg_length 
will carry the reply message to be sent to the CICAM, and the decrypted' output variable will be set to 0.
If the SAC message does not contain the Key precursor Kp, the message is decrypted and returned in clearmsg/clear_msg_length 
and the decrypted' output variable is set to 1

Input Parameters:
drm_handle Pointer to DRM handle
enc_msg Pointer to buffer holding the encrypted message 
enc_msg_len Pointer to length of encrypted message
dec_msg_length:dec_msg buffer length

Output Parameters:
dec_msg  pointer to the clear message buffer / reply message buffer
decrypted Pointer to the clear message length/reply message length
decrypted Pointer to variable indicating which operation was performed. 1=message decrypted 0=reply generated.
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or clearmsg,enc_msg ,enc_msg_len is NULL
    CI_CC_NOT_ENOUGH_SPACE:enc_msg_len is not enough, in this case enc_msg_len will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully
**************************************************************************/

CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Decrypt_MessageSac(UINT32 drm_handle,UINT8 *encmsg, UINT16 enc_msg_length, UINT8 *dec_msg, UINT16 *dec_msg_length, UINT8 *decrypted);

/***************************************************************************
    Description:

   This API returns key ID which shall be associated with DMX for content decryption 
   This function only needs to be called after the initial CCK establishment. Any subsequent CCK refresh will use the same KeyID. 
   The KeyID actually represents a key pair (ODD, EVEN), and key rotation will be handled automatically 
   Then processing SAC messages containing Key precursor Kp

Input Parameters:
        drm_handle pointer to DRM handle

        Output Parameters:
        KeyID  Key ID for use with AMP DMX.
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or puKeyID is NULL
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully
**************************************************************************/


CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_KeyId(UINT32 drm_handle, UINT32 * puKeyID);

/***************************************************************************
    Description:

This API generates a confirmation message for either the URI refresh protocol or the SRM transfer protocol. 
he output message will be the SHA-256 checksum of a message consisting of the input data and the UCK. The UCK is a SHA-256 checksum of the SAK

Input Parameters:
rm_handle Pointer to DRM handle
Data Pointer to data of message body
Data_length Length of message body
confirm_length:confirm buffer length

Output Parameters:
    Confirm  pointer to confirmation message
    Cofirm_length output actural length of confirmation message
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or data,confirm ,confirm_length is  NULL
    CI_CC_NOT_ENOUGH_SPACE:confirm_length is not enough, in this case confirm_length will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully

**************************************************************************/

CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Data_Confirm(UINT32 drm_handle, UINT8* data, UINT16 data_length, UINT8* confirm, UINT16* confirm_length);
/***************************************************************************
    Description:
        To query tv stored AKH, tv will maximum store 5 AKHs.

    Input Parameters:
        drm_handle: DRM Handle Context
        AKH_len: akh buffer length

    Output Parameters:
        AKH: AKH queried
        AKH_len:actual AKH data length
Return Value:
    CI_CC_INV_ARG: input drm_handle is not valid or AKH,AKH_len is NULL
    CI_CC_NOT_ENOUGH_SPACE:AKH_len is not enough, in this case AKH_len will be output to actual needed buffer size
    CI_CC_FAILED:failed
    CI_CC_OK:sucessfully

**************************************************************************/
CI_CC_RESULT_T MTTZ_CILPLUS_DRM_Get_NextAuthContext(UINT32 drm_handle,UINT8 *AKH,UINT16 *AKH_len,UINT8 *cam_id,UINT16 *cam_id_len,UINT16 *cam_brand_id);

MT_RESULT_T MTTZ_SECURESTOP_cancel_session( unsigned char *  session_id , int length);

MT_RESULT_T MTTZ_SECURESTOP_bind_license( unsigned char  *  session_id, int session_len,
                       unsigned char  * const kid, int kid_len);

MT_RESULT_T MTTZ_SECURESTOP_bind_session( TEE_SECRET_KEY *  secKey ,
                       unsigned char  *  kid, int kid_len, long update_time, unsigned char *SessionID);

MT_RESULT_T MTTZ_SECURESTOP_remove_session(int kid);
MT_RESULT_T MTTZ_SECURESTOP_reset_session(void);

MT_RESULT_T MTTZ_SECURESTOP_write_securestop(int kid);

MT_RESULT_T MTTZ_SECURESTOP_init_securestops( unsigned char *  inbuf, int     inlen);
MT_RESULT_T MTTZ_SECURESTOP_get_securestops(unsigned char *  outbuf, int *  outlen, int* return_flag);
MT_RESULT_T MTTZ_SECURESTOP_get_securestop_ids(unsigned char          *session_id,int *  outnum);
MT_RESULT_T MTTZ_SECURESTOP_get_securestop( unsigned char *  session_id, unsigned char *  outSecureStop,
                                         int *  outSecureStopLen, long update_time,TEE_SIGNATURE * sigout, int* return_flag);
MT_RESULT_T MTTZ_SECURESTOP_update_securestop( int key_id, int *  updated, long update_time);
MT_RESULT_T MTTZ_SECURESTOP_commit_securestop( unsigned char * session_id);
MT_RESULT_T MTTZ_SECURESTOP_reset_securestop(int * outNumRemoved);
MT_RESULT_T MTTZ_SECURESTOP_UnloadSecretKey(TEE_SECRET_KEY *  secKey);


#endif

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DTCP_EncryptHCI
 *
 * Description: This API encrypts HCI
 *
 * Inputs: 
 *		pu1Hci          Pointer to HCI
 *		u4Size          HCI size
 *		pu1EncHci       Pointer to encrypted HCI
 *
 * Outputs: 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DTCP_EncryptHCI(UINT8 *pu1Hci, UINT32 u4Size, UINT8 *pu1EncHci); 

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DTCP_DecryptHCI
 *
 * Description: This API encrypts HCI
 *
 * Inputs: 
 *		pu1EncHci       Pointer to encrypted HCI
 *		u4Size          HCI size
 *		pu1Hci          Pointer to HCI
 *
 * Outputs: 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DTCP_DecryptHCI(UINT8 *pu1EncHci, UINT32 u4Size, UINT8 *pu1Hci); 

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DTCP_SwapEncryptedHCI
 *
 * Description: This API swaps the encrypted HCI
 *
 * Inputs: 
 *		pu1EncDistHci   Pointer to encrypted HCI (By Kdist)
 *		u4Size          HCI size
 *		pu1Digest       Pointer to digest
 *		u4DigestLen     Digest length
 *		pu1EncHci       Pointer to encrypted HCI
 *
 * Outputs: 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DTCP_SwapEncryptedHCI(UINT8 *pu1EncDistHci, UINT32 u4Size,
                                    UINT8 *pu1Digest, UINT32 u4DigestLen,
                                    UINT8 *pu1EncHci);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DTCP_SetEncLocalKey
 *
 * Description: This API sets the encrypted local key
 *
 * Inputs: 
 *      prCryptoInfo            The crypto information
 *
 * Outputs: 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DTCP_SetEncLocalKey(const MTTZ_DTCP_CRYPTO_INFO_T *pCryptoInfo);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DTCP_DecryptHddData
 *
 * Description: This API decrypts HDD data
 *
 * Inputs: 
 *		pu1Input        Pointer to encrypted HDD data
 *		u4Size          HDD data size
 *		pu1Output       Pointer to HDD data
 *		pu1SecCode      Pointer to security code
 *		u4CodeSize      Security code size
 *
 * Outputs: 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DTCP_DecryptHddData(UINT8 *pu1Input, UINT32 u4Size, UINT8 *pu1Output,
                                    UINT8 *pu1SecCode, UINT32 u4CodeSize);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_DTCP_SwapEncryptedHddData
 *
 * Description: This API swaps the encrypted Hdd data
 *
 * Inputs: 
 *		pu1Input        Pointer to encrypted HDD data (By Khdd)
 *		u4Size          HDD data size (must be N*16)
 *		pu1Output       Pointer to encrypted HDD data (By Kl)
 *		u4KeyIdx        Key index
 *		fgReloadIv      IV needs to be reloaded or not
 *
 * Outputs: 
 * 
 * Returns: 0       Success. 
 *          Others  Fail.  
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_DTCP_SwapEncryptedHddData(UINT8 *pu1Input, UINT32 u4Size, UINT8 *pu1Output,
                                        UINT32 u4KeyIdx, BOOL fgReloadIv);
#ifdef ULI_LIB_SUPPORT
/*-----------------------------------------------------------------------------
 * Name: MTTZ_ULI_ObjectDecrypt
 *
 * Description: This API decrypt the ULI provisioned object
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_ULI_ObjectDecrypt(CHAR* ps_owner_name, CHAR* ps_object_name, UINT8* ps_in_buff, UINT32 ui4_in_buff_sz, UINT8* ps_out_buff, UINT32* pui4_out_buff_sz, MTTZ_ULI_ENCRYPTED_POTK_T* pt_encrypted_potk, UINT32* pui4_clear_obj_sz);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_ULI_ChkPotk
 *
 * Description: This API check the POTK is valid or not
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_ULI_ChkPotk(MTTZ_ULI_ENCRYPTED_POTK_T* pt_encrypted_potk, UINT8* pui1_encrypted_shared_secret_buffer);
#endif


/*-----------------------------------------------------------------------------
 * Name: MTTZ_SESTORE_ReadSecureData
 *
 * Description: This API read decrypted secure data from sestore.
 *
 * Inputs: pIdentifier Reference to Key ID.
 *         pLength     Reference to size of the decrypted content buffer.
 *
 * Outputs: pData      Reference to the decrypted content.
 *          pLength    Reference to size of the decrypted content.
 *
 * Returns: MTR_OK     Success.
 *          Others     Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_SESTORE_ReadSecureData(char *pIdentifier, UINT8 *pData, int *pLength);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_SESTORE_WriteSecureData
 *
 * Description: This API write secure data to sestore.
 *
 * Inputs: pIdentifier Reference to Key ID.
 *         pData       Reference to the content to be encrypted.
 *         dataLength  Size of the content to be encrypted.
 *
 * Outputs: 
 *
 * Returns: 0       Success.
 *          Others  Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_SESTORE_WriteSecureData(char *pIdentifier, UINT8 *pData, int dataLength);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_SESTORE_IsSecureDataExist
 *
 * Description: This API check whether designated Key ID is existed or not.
 *
 * Inputs: pIdentifier Reference to Key ID.
 *
 * Outputs: pResult    Reference to retrieve search result.
 *                     0 > data not found; 1 > data found
 *
 * Returns: MTR_OK     Success.
 *          Others     Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_SESTORE_IsSecureDataExist(char *pIdentifier, int *pResult);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_SESTORE_GetClearDataLength
 *
 * Description: This API get the length of decrypted secure data.
 *
 * Inputs: pIdentifier Reference to Key ID.
 *
 * Outputs: pLength    Reference to size of the decrypted content.
 *
 * Returns: MTR_OK     Success.
 *          Others     Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_SESTORE_GetClearDataLength(char *pIdentifier, int *pLength);

/*-----------------------------------------------------------------------------
 * Name: MTTZ_SESTORE_GetSecureDataProtected
 *
 * Description: This API read secure data from sestore without decryption.
 *
 * Inputs: pIdentifier Reference to Key ID.
 *         pLength     Reference to size of the decrypted content buffer.
 *
 * Outputs: pData      Reference to the decrypted content.
 *          pLength    Reference to size of the decrypted content.
 *
 * Returns: MTR_OK     Success.
 *          Others     Fail.
 ----------------------------------------------------------------------------*/
MT_RESULT_T MTTZ_SESTORE_GetSecureDataProtected(char *pIdentifier, UINT8 *pData, int *pLength);

#ifdef __cplusplus
}
#endif

#endif //_MTDRM_H_
