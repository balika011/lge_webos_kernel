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
 * $RCSfile: tz_dmx_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef TZ_DMX_IF_H
#define TZ_DMX_IF_H
/* ============================================================================
    Includes
============================================================================ */
#include "gcpu_if.h"
#include "dmx_drm_drvif.h"
#include "dmx_if.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef enum
{
    GCPU_KEY_TYPE_NONE = 0,
    GCPU_KEY_TYPE_DTCP_KL,
    GCPU_KEY_TYPE_MAX
} GCPU_KEY_TYPE_T;

//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
extern BOOL SvcEx_DMX_Init();
extern BOOL SvcEx_DMX_DMX_SetMMKey(UINT8 *pu1Param, UINT32 u4Size);
extern BOOL DMX_SetHDCPKeyIv(UINT8 u1Pidx, UINT8 u1KeyIdx, UINT8 *pu1Key, UINT8 *pu1Iv);
extern BOOL TZ_SWD_DMX_SetKey(DMX_CRYPTO_KEY_TYPE_T eKeyType, DMX_KEY_IDX_T eKeyIdx, UINT8 *pu1Key, UINT32 u4KeyLen);
extern BOOL TZ_SWD_DMX_GetKey(DMX_CRYPTO_KEY_TYPE_T eKeyType, DMX_KEY_IDX_T eKeyIdx, UINT8 *pu1Key, UINT32 u4KeyLen);
extern BOOL SvcEx_DMX_SetCryptoKey(UINT8 *pu1Param);
extern BOOL SvcEx_FVR_SetCryptoKey(UINT8 *pu1Param);

extern BOOL SvcEx_DMX_ReloadSecureKey(UINT8 *pu1Param);
extern BOOL Svc_DMX_MMSecurityInit(VOID);
/* DTCP IP application */
//extern BOOL DMX_DecryptSecureKey(const UINT8 * pu1Src, UINT32 u4Len, UINT8 * pu1Dst, DMX_KEY_TYPE_T eType);

extern BOOL SvcEx_GCPU_Hw_CmdRun(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_Hw_CmdReturn(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_EnableIOMMU(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_DisableIOMMU(void);
extern BOOL SvcEx_GCPU_Hw_SetSlot(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_ClearIRQ(void);
extern BOOL SvcEx_GCPU_ClearIommuIRQ(void);
extern BOOL SvcEx_GCPU_Hw_Reset(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_IrqHandle(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_IOMMU_IrqHandle(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_GetRomCodeChecksum(UINT32 u4Addr, UINT32 u4Size);
extern BOOL SvcEx_GCPU_Hw_RestoreSecureKeys(void);

extern INT32 TZ_SVP_AES_CTR(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, UINT8 *pu1Ctr, UINT8 *pu1RetCtr);

extern BOOL TZ_GCPU_ReloadSecureKeyIv(GCPU_KEY_TYPE_T eKeyType, UINT32 u4KeyIdx,
                                    UINT32 u4KeyPtr, UINT32 u4IvPtr);

extern BOOL TZ_GCPU_AES_CBC(const UINT8 *pu1Key, UINT32 u4Keylen, 
                        const UINT8 *pu1Iv, UINT8 *pu1RetIv,
                        const UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len,
                        BOOL fgEncrypt);

extern BOOL TZ_GCPU_SECURE_AES_CBC(const UINT32 u4KeyPtr, UINT32 u4Keylen,
                        const UINT32 u4IvPtr,
                        const UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len,
                        BOOL fgEncrypt);

extern BOOL TZ_GCPU_DECRYPT_TS(const UINT8 *pu1Key, UINT32 u4Keylen, const UINT8 *pu1Iv,
                        const UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, 
                        TSDESC_CIPHER_MODE eCipher, BOOL fgTTS);
#endif
