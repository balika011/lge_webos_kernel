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

#ifndef TEE_COMMON_H
#define TEE_COMMON_H
/* ============================================================================
    Includes
============================================================================ */

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define TEE_COMMON_ARG_LENGTH 16

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
    UINT8 *apvArgument[TEE_COMMON_ARG_LENGTH]; /* Array of argument pointer */
    UINT32 ui4Length[TEE_COMMON_ARG_LENGTH];   /* Array of argument length */
    BOOL   abWrite[TEE_COMMON_ARG_LENGTH];     /* Array of argument write back */
} TEE_COMMON_ARG_T;

//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------
UINT32 _teeCommon(UINT8 * pvArg1, UINT32 ui4ArgLen1, UINT8 * pvArg2, UINT32 ui4ArgLen2);
UINT32 _teeCommonEnter(TEE_COMMON_ARG_T *ptCommon);
UINT32 _teeCommonLeave(TEE_COMMON_ARG_T *ptCommon);
#endif
