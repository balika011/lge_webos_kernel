/*
 * Copyright:
 * ----------------------------------------------------------------------------
 * This confidential and proprietary software may be used only as authorised
 * by a licensing agreement from ARM Limited.
 *      (C) COPYRIGHT 2008-2009 ARM Limited, ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised copies and
 * copies may only be made to the extent permitted by a licensing agreement
 * from ARM Limited.
 * ----------------------------------------------------------------------------
 * $LastChangedRevision: 2875 $
 * ----------------------------------------------------------------------------
 */

#ifndef SVC_REGISTRY_H
#define SVC_REGISTRY_H

/* ============================================================================
    Includes
============================================================================ */
#include "tzst.h"
#include "tz.h"

/* ============================================================================
   Export the function list to the core Secure World code.
============================================================================ */
EXTERN BOOL SvcEx_DRM_WV_SetAssetkey(UINT8* pu1EncAssetKey, UINT32 u4KeyLen);
EXTERN BOOL SvcEx_DRM_WV_DeriveCW(UINT8 *pu1Param, UINT32 u4ParmSize);
EXTERN BOOL SvcEx_DRM_WV_PassKeybox(UINT8* pu1Keybox, UINT32 u4Size);

EXTERN BOOL SvcEx_DRM_GetRomBytes(UINT32 u4Addr, UINT32 u4Size);
EXTERN BOOL SvcEx_DRM_GetSecureID(UINT32 u4Addr, UINT32 u4Size);

EXTERN UINT32 SvcEx_TZCTL(TZ_SMC_ARG_T const *prSMCArg);


#endif  /* SVC_REGISTRY_H */
