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

#ifndef TEE_VUDU_H
#define TEE_VUDU_H
/* ============================================================================
    Includes
============================================================================ */


/* Vudu */
UINT32 _tee_CRYPTO_VUDU_OpenM2mKeySlot(void);
UINT32 _tee_CRYPTO_VUDU_CloseM2mKeySlot(void);
UINT32 _tee_CRYPTO_VUDU_SelectM2mKeyAlgorithm(UINT8 alg, UINT8 dir);
UINT32 _tee_CRYPTO_VUDU_LoadClearM2mKey(const UINT32 keyLen, const UINT8 *pKeyData, const UINT32 ivLen, const UINT8 *pIvData);
UINT32 _tee_CRYPTO_VUDU_LoadWrappedM2mKey(const UINT32 keyLen, const UINT8 *pKey3Data, const UINT8 *pKey4Data, const UINT8 *pIvData);
UINT32 _tee_CRYPTO_VUDU_LoadWrappedM2mKeyClearIv(const UINT32 keyLen, 	const UINT8 *pKey3Data, const UINT8 *pKey4Data, const UINT32 ivLen, const UINT8 *pIvData);
UINT32 _tee_CRYPTO_VUDU_RunM2mKey(const UINT32 length, const UINT8 *pSrc, UINT8 *pDst);
UINT32 _tee_CRYPTO_VUDU_OpenCAKeySlotHandle(void);
UINT32 _tee_CRYPTO_VUDU_CloseCAKeySlotHandle(void);
UINT32 _tee_CRYPTO_VUDU_GetCAKeySlotHandle(void);
UINT32 _tee_CRYPTO_VUDU_SetWrappedCAKey(const UINT8 *pKey, const UINT8 *pUniqueKey, UINT8 keyType);
UINT32 _tee_CRYPTO_VUDU_SetWrappedAllCAKey(const UINT8 *pOddKey, const UINT8 *pEvenKey, const UINT8 *pUniqueKey);
UINT32 _tee_CRYPTO_VUDU_LoadCAClearKey(const UINT8 *pKey, UINT8 keyType);
UINT32 _tee_CRYPTO_VUDU_StartTSDecrypt(const UINT32 srcLength, const UINT8 *pSrc, UINT32 *pDstLength, UINT8 *pDst);
UINT32 _tee_CRYPTO_VUDU_SetOddEvenCAKey(const UINT8 *pOddKey, const UINT8 *pEvenKey, UINT8 keyType);
UINT32 _tee_CRYPTO_VUDU_SVP_Init(UINT32 base, int size);
UINT32 _tee_CRYPTO_VUDU_SVP_TSDecrypt(const UINT32 srcLength, const UINT8 *pSrc, UINT32 *pDstLength, UINT8 *pDst);
UINT32 _tee_CRYPTO_VUDU_SVP_GetAU(unsigned int *pAuAddr, int *pAuLength, int *pAuType,
	unsigned int *pWPtr, unsigned int rPtr, unsigned long long *pPTS);
UINT32 _tee_CRYPTO_VUDU_SVP_Flush(void);
UINT32 _tee_CRYPTO_VUDU_SVP_Release(void);
UINT32 _tee_CRYPTO_VUDU_SetM2mkey(const UINT32 keyLen, const UINT8 *pKeyData);
UINT32 _tee_CRYPTO_VUDU_SetCAkey(const UINT32 keyLen, const UINT8 *pKeyData);


#endif
