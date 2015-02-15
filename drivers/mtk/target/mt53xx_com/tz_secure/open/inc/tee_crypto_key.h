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
 * $RCSfile: tee_dtcp.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef TEE_CRYPTO_KEY_H
#define TEE_CRYPTO_KEY_H
/* ============================================================================
    Includes
============================================================================ */

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define TEE_CRYPTO_SIGNATURE_SIZE_IN_BYTES          (32)

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _teeCryptoDecryptSecurePvrAesKey0
 *  Decrypt secure PVR AES key0. This key will be used by SONY and Turnkey.
 *
 *  @param  pu1CipherKey                Pointer to the cipher key
 *  @param  pu1PlainKey                 Pointer to the plain key
 *  @param  u4KeyLen                    The key length
 *
 *  @retval TZ_SUCCESS                  Succeed
 *          TZ_ERROR_ILLEGAL_ARGUMENT   Illegal argument
 *          TZ_ERROR_CRYPTO             crypto error
 */
//-----------------------------------------------------------------------------
UINT32 _teeCryptoDecryptSecurePvrAesKey0(const UINT8 *pu1CipherKey, UINT8 *pu1PlainKey, UINT32 u4KeyLen);

//-----------------------------------------------------------------------------
/** _teeCryptoSignWithKL
 *  Sign the message with local key.
 *
 *  @param  pu1Data                     Pointer to the message
 *  @param  u4Size                      message size
 *  @param  pu1Digest                   Pointer to a buffer that receives a generated signature
 *
 *  @retval TZ_SUCCESS                  Succeed
 *          TZ_ERROR_ILLEGAL_ARGUMENT   Illegal argument
 *          TZ_ERROR_CRYPTO             crypto error
 */
//-----------------------------------------------------------------------------
UINT32 _teeCryptoSignWithKL(const UINT8 *pu1Data, UINT32 u4Size, UINT8 pu1Digest[TEE_CRYPTO_SIGNATURE_SIZE_IN_BYTES]);

#endif
