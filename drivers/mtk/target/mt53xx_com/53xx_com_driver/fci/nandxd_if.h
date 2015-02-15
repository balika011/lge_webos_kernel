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
*
* $Author: p4admin $
* $Date: 2015/02/15 $
* $RCSfile: nandxd_if.h,v $
* $Revision: #1 $
*
*---------------------------------------------------------------------------*/

/** @file nandxd_if.h
 *  This header file declares the SM/XD basic hardware protocol functions.
 */

#ifndef NANDXD_IF_H
#define NANDXD_IF_H

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "fci_type.h"

#include "x_lint.h"

LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_typedef.h"

LINT_EXT_HEADER_END

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

/// page attribute, redundancy size in bytes.
#define NANDXD_RDNT_SIZE                     (16)
/// page attribute, page size in bytes.
#define NANDXD_PAGE_SIZE                (512)
/// page attribute, (redundancy + page) size in bytes.
#define NANDXD_TOTAL_PAGE_SIZE        ((NANDXD_PAGE_SIZE)+(NANDXD_RDNT_SIZE))

/// block attribute, number of pages in one block.
#define NANDXD_PAGE_PER_BLK          (32)
/// block attribute, first page index.
#define NANDXD_FIRST_PAGE_NO          (0)
/// block attribute, last page index.
#define NANDXD_LAST_PAGE_NO           (NANDXD_PAGE_PER_BLK-1)
/// block attribute, block size in bytes.
#define NANDXD_BLK_SIZE                      ((NANDXD_PAGE_SIZE)*(NANDXD_PAGE_PER_BLK))
/// block attribute, number of blocks in one physical zone.
#define NANDXD_PHY_BLK_PER_ZONE    (1024)
/// block attribute, number of blocks in one logical zone.
#define NANDXD_LOG_BLK_PER_ZONE    (1000)
///NANDXD_CHK_BLK_NO base on XD spec v1.2: check until 23 block is reached.
#define NANDXD_CHK_BLK_NO               (23)

/// index of name of manufacture in CIS field.
#define NANDXD_MANU_NAME         (0x59)
/// index of name of product in CIS field.
#define NANDXD_PROD_NAME        (0x61)
/// index of name of version in CIS field.
#define NANDXD_VERS_NAME       (0x66)

/// blank data byte in NANDXD card.
#define NANDXD_BLANK_BYTE        ((UINT8)0xFF)
/// blank data word (2 bytes) in NANDXD card.
#define NANDXD_BLANK_WORD        ((UINT16)0xFFFF)
/// blank data double word (4 bytes)  in NANDXD card.
#define NANDXD_BLANK_DWORD        ((UINT32)0xFFFFFFFF)

/// unused logical block number.
#define NANDXD_LOG_BLK_BLANK_NO       ((UINT16)0xFFFF)
/// unused physical block number.
#define NANDXD_PHY_BLK_BLANK_NO       ((UINT32)0xFFFFFFFF)

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
/** SM/xD card configuration structure.
 *  This data structure is used to store SM/xD page, block, card size.
 */

typedef struct __NANDXDMEMCFG_T
{
    UINT16 u2Size;   ///< card size, unit = MBytes.
    UINT16 u2PageSz; ///< page size, unit = Bytes.
    UINT16 u2BlkSz;  ///< nof pages/block.
    UINT32 u4BlkNs;  ///< total no of blocks.
} NANDXDMEMCFG_T;

/** SM/xD card device code structure.
 *  This data structure is used to store SM/xD card device code, card size.
 */
typedef struct __NANDXDDEVCODEMAP_T
{
    UINT16 u2DevCode; ///< device code.
    UINT16 u2Size;    ///< card size, unit = MBytes.
} NANDXDDEVCODEMAP_T;

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------
/* Export SM slot ECC functions */
EXTERN VOID _NANDXD_MakeEcc256(UINT8 *pu1EccBuf, UINT32 *pu4DataBuf);
#if 0
EXTERN NANDXDECCDIFF_T _NANDXD_ChkEcc(UINT8 *iEccdata1, UINT8 *iEccdata2, 
    const UINT8 *pPagedata, UINT32 pOffset, UINT8 pCorrected);
#endif
/* SM/XD Debug/emulation functions. */
EXTERN INT32 _NANDXD_ReadBlock(UINT32 u4StBlock, UINT32 u4MemPtr, UINT32 u4NoPage);

EXTERN INT32 _NANDXD_WriteBlock(UINT32 u4StBlock, UINT32 u4MemPtr, UINT32 u4NoPage);

EXTERN INT32 _NANDXD_Reset(VOID);

EXTERN INT32 _NANDXD_StressTest(VOID);

EXTERN INT32 _NANDXD_PageUnitTest(UINT32 u4Blk, UINT32 u4Page);

EXTERN INT32 _NANDXD_EraseAll(VOID);

EXTERN INT32 _NANDXD_ReadAll(VOID);

EXTERN INT32 _NANDXD_WriteMultiBlk(VOID);

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------
/* Export SM slot HW functions */
EXTERN INT32 NANDXDHW_EraseBlock(UINT32 u4Blk);

EXTERN INT32 NANDXDHW_SetStartPtr(VOID);

EXTERN INT32 NANDXDHW_ReadPage(UINT32 u4Blk, UINT16 u2Page, UINT8 *pbBuf, UINT32 u4PageSize);

EXTERN INT32 NANDXDHW_ReadRdnt(UINT32 u4Blk, UINT16 u2Page, UINT8 *pbBuf);

EXTERN INT32 NANDXDHW_WritePage(UINT32 u4Blk, UINT16 u2Page, UINT8 *pbBuf);

EXTERN INT32 NANDXDHW_CheckCard(VOID);

EXTERN INT32 NANDXDHW_Reset(VOID);

// under development.
//EXTERN INT32 NANDXDHW_WriteMultiBlkTest(const NANDXD_T *prNANDXD, UINT8 *pbBuf);

#endif // NANDXD_IF_H
