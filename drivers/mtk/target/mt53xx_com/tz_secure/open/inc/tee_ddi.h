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

#ifndef TEE_DDI_H
#define TEE_DDI_H
/* ============================================================================
    Includes
============================================================================ */

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#define TEE_DDI_SEC_KEY_SIZE 16

#ifndef TEE_SECURESTORAGE_SEC_KEY_SIZE
#define TEE_SECURESTORAGE_SEC_KEY_SIZE 16
#endif

#define SE_SECURE_STORE_HMAC_OFFSET     4
#define SE_SECURE_STORE_HMAC_KEY_SIZE   16
#define SE_SECURE_STORE_HMAC_VALUE_SIZE 32

#define NULL_CHECK(x)   if(x == NULL) { DMSG("%s: %d,  %s is a null pointer\n", __FILE__, __LINE__, #x); return TZ_ERROR_MEMORY; }

//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
UINT32 _teeDDIPreEncA1Dec(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 *u4DstLen);
UINT32 _teeDDIPreEncA2Dec(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1DstBuf, UINT32 *u4DstLen);
UINT32 _teeDDISeStoreSign(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT8 *pu1HMACBuf);
UINT32 _teeDDISeStoreVerify(UINT8 *pu1SrcBuf, UINT32 u4SrcLen, UINT32 *u4Result);
UINT32 _teeDDISeStoreEncHMACKey(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len);
UINT32 _teeDDISeStoreDecHMACKey(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len);
#endif
