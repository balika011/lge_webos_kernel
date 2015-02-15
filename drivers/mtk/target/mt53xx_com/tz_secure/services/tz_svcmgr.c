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

/* ============================================================================
    Includes
============================================================================ */
#include <string.h>
#include "svc_registry.h"


/* ============================================================================
    Public Functions
============================================================================ */

UINT32 SvcMgr_Main(TZ_SMC_ARG_T const * pksArgs)
{
    return SvcEx_TZCTL(pksArgs);
}

