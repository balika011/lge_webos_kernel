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
 * $RCSfile: tee_securestorage.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/* ============================================================================
    Includes
============================================================================ */
#include "stdlib.h"
#include "string.h"

#include "tzst.h"
#include "svc_registry.h"
#include "tee_common.h"

#define TEE_DUMP(x,y,z) DMSG((x)); \
    for(i = 0; i < (y); i++) { \
        if(i % 16 == 0) \
            Printf("\n"); \
        Printf("0x%02x ", (z)[i]); \
    } \
    Printf("\n\n");

/* ============================================================================
    Global Variable
============================================================================ */

/* ============================================================================
    Internal Functions
============================================================================ */

/* ============================================================================
    Exported Entry Points
============================================================================ */
UINT32 _teeCommon(UINT8 * pvArg1, UINT32 ui4ArgLen1, UINT8 * pvArg2, UINT32 ui4ArgLen2)
{    
    int i;
    TEE_DUMP("pvArg1:", ui4ArgLen1, pvArg1);
    TEE_DUMP("pvArg2:", ui4ArgLen2, pvArg2);

    memset(pvArg1, 0x00, ui4ArgLen1);
    memset(pvArg2, 0xFF, ui4ArgLen2);

    return 0;
}

UINT32 _teeCommonEnter(TEE_COMMON_ARG_T *ptCommon)
{
    UINT8 i = 0;

    /* Call TZ_SWD_BufferStart() in start stage of secure call */
    for (i = 0; i < ptCommon->ui1Size; i++) {
        TZ_SWD_BufferStart((UINT32)ptCommon->apvArgument[i], TZ_CACHE_ALIGN(ptCommon->ui4Length[i]));
    }

    return 0;
}

UINT32 _teeCommonLeave(TEE_COMMON_ARG_T *ptCommon)
{
    UINT8 i = 0;

    /* Call TZ_SWD_BufferEnd() at end of secure call */
    for (i = 0; i < ptCommon->ui1Size; i++) {
        TZ_SWD_BufferEnd((UINT32)ptCommon->apvArgument[i], TZ_CACHE_ALIGN(ptCommon->ui4Length[i]));
    }
    
    return 0;
}
