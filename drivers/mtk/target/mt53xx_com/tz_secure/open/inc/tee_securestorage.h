/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: tee_securestorage.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef TEE_SECURESTORAGE_H
#define TEE_SECURESTORAGE_H
/* ============================================================================
    Includes
============================================================================ */

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define TEE_ROMDATA_START_ADDR          (0xf4000000)
#define TEE_SECURESTORAGE_HEADER_SIZE   (4)       /* 4 bytes */
#define TEE_SECURESTORAGE_SEC_KEY_SIZE  (128 / 8) /* 16 bytes */
#define TEE_SECURESTORAGE_HASH_KEY_SIZE (256 / 8) /* 32 bytes */
#define TEE_SECURESTORAGE_USERKEY_SIZE  (128 / 8) /* 16 bytes */
#define TEE_SECURESTORAGE_ECNID_SIZE    (128 / 8) /* 16 bytes */
#define TEE_SECURESTORAGE_ROMDATA_SIZE  (256)     /* 256 bytes */
#define TEE_SECURESTORAGE_SEED_SIZE     (TEE_SECURESTORAGE_USERKEY_SIZE + TEE_SECURESTORAGE_ROMDATA_SIZE + TEE_SECURESTORAGE_ECNID_SIZE) /* 288 bytes */
#define TEE_SECURESTORAGE_MAX_SIZE      (0x200000)/* 2MB bytes */
#define SHA384_DIGEST_LENGTH            (48)
#define SHA512_DIGEST_LENGTH            (64)

#define TEE_SECURESTORAGEV1_ECNID_SIZE      (8)
#define TEE_SECURESTORAGEV1_ROMDATA_SIZE    (128)
#define TEE_SECURESTORAGEV1_SEED_SIZE       (TEE_SECURESTORAGEV1_ECNID_SIZE + TEE_SECURESTORAGEV1_ROMDATA_SIZE)
#define TEE_SECURESTORAGEV1_HASH_KEY_SIZE   (160 / 8) /* 20 bytes */
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


/**
 * SecureStorage key.
 *
*/
typedef struct
{
    unsigned char au1EncKey[TEE_SECURESTORAGE_USERKEY_SIZE];
}TEE_SECURESTORAGE_USERKEY_T;

//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
UINT32 _teeSecureStorageInit();
UINT32 _teeSecureStorageEnc(TEE_SECURESTORAGE_USERKEY_T *ptUserKey, UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen);
UINT32 _teeSecureStorageSecretEnc(UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen);
UINT32 _teeSecureStorageDec(TEE_SECURESTORAGE_USERKEY_T *ptUserKey, UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen);
UINT32 _teeSecureStorageSecretDec(UINT8 *pu1SrcBuf, long lSrcLen, UINT8 *pu1DstBuf, long *plDstLen);

#endif
