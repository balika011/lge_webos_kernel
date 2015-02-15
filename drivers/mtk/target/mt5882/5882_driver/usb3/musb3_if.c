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
 * $RCSfile: musb3_if.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file musb3_if.c
 *  MUSB module api function.
 */

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_lint.h"
#include "musb3_if.h"
#include "x_pinmux.h"
#include "x_timer.h"
#include "x_pdwnc.h"
#include "x_assert.h"


//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------
extern unsigned long usb_stor_read(int device, unsigned long blknr, 
                                   unsigned long blkcnt, void *buffer);
extern unsigned long usb_stor_getblksize(int lun);
extern unsigned long usb_stor_getblknum(int lun);

//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
//-------------------------------------------------------------------------
/** _Musb_InitLock.
*/
//-------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** MUSB3_Init
 *  Set USB host or device mode function.
 *  @param  u4Mode		1: host, 0: device.
 *  @retval  0 	SUCCESS.
 *  @retval  -1	FAIL. 
 */
//-------------------------------------------------------------------------
INT32 MUSB3_Init(UINT32 u4Mode)
{
    UNUSED(u4Mode);
    return 0;
}

//-------------------------------------------------------------------------
/** MUSB3_Read
*/
//-------------------------------------------------------------------------
INT32 MUSB3_Read(UINT64 u8Offset, UINT32 u4MemPtr, UINT32 u4MemLen)
{
    UINT32 u4BlkNum, u4BlkCount, u4BlkSize;
    
    ASSERT(u4MemPtr > 0);

    if(u4MemLen == 0)
    {
        return 0;
    }

    u4BlkSize = usb_stor_getblksize(0);
    if(u4BlkSize == 0)
    {
        return -1;
    }
    
    u4BlkNum = (UINT32)(u8Offset / (UINT64)u4BlkSize);
    u4BlkCount = u4MemLen / u4BlkSize;
    
    if(usb_stor_read(0, u4BlkNum, u4BlkCount, (void*)u4MemPtr) != u4BlkCount)
    {
        return -1;
    }
    
    return 0;
}


//-------------------------------------------------------------------------
/** MUSB3_Write
*  MUSB write function.
*  @param  u8Offset     write offset from card, u8Offset should be multiple of block size.
*  @param  u4MemPtr      user's write buffer address.
*  @param  u4MemLen      write length, u4MemLen should be multiple of block size.
*  @retval S_OK   Success.
*  @retval others  Fail.
*/
//-------------------------------------------------------------------------
INT32 MUSB3_Write(UINT64 u8Offset, UINT32 u4MemPtr, UINT32 u4MemLen)
{
    UNUSED(u8Offset);
    UNUSED(u4MemPtr);

    if(u4MemLen != 0)
    {
        ASSERT(0);
    }

    return -1;
}


//-------------------------------------------------------------------------
/** MUSB3_GetSize
*  MUSB get card size function.
*  @param  VOID.
*  @return  the card size. Unit in sector:
*                512 bytes or 2048 bytes depend on pMediumInfo->dwBlockSize.
*/
//-------------------------------------------------------------------------
INT64 MUSB3_GetSize(VOID)
{
    return (INT64)usb_stor_getblknum(0);
}


//-------------------------------------------------------------------------
/** MUSB3_GetBlockSize
*  MUSB get block size function.
*  @param  VOID.
*  @return  the card block size in bytes.
*/
//-------------------------------------------------------------------------
INT32 MUSB3_GetBlockSize(VOID)
{
    return usb_stor_getblksize(0);
}


//-------------------------------------------------------------------------
/** MUSB3_GetStatus
*  MUSB get usb status.
*  @param  VOID.
*  @return  the card status in STORG_DEV_STATUS_T.
*/
//-------------------------------------------------------------------------
INT32 MUSB3_GetStatus(VOID)
{
    // BLKDEV_STATUS_READY = 2
    return 2;
}

