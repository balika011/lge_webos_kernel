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
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: cust_common_interface.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file cust_common_interface.c
 *  Implement common interface for Sony frontend.
 */

#include "PI_Def_dbg.h"
#include "drvapi_tuner.h"
#include "tunerDebug.h"
#include "cust_common_interface.h"


#define MVID_EEPROM_ADDRESS 0x6cd
#define BV_EEPROM_ADDRESS   0x6ce
#define CI_SLOT_ADDRESS     0x6cf

void VDRBRDCST_GetSonyProductTypeInfo(SONYPRODUCT_TYPEINFO *pSonyProductTypeInfo)
{
    UINT8  u1ValMVID = 0xff;
    UINT8 u1ValBV = 0xff;
    UINT8 u1ValCISlot = 0xff;

    VERIFY(0 == EEPROM_Read(MVID_EEPROM_ADDRESS, (UINT32)(void *)&u1ValMVID, 1));
    VERIFY(0 == EEPROM_Read(BV_EEPROM_ADDRESS, (UINT32)(void *)&u1ValBV, 1));
    VERIFY(0 == EEPROM_Read(CI_SLOT_ADDRESS, (UINT32)(void *)&u1ValCISlot, 1));
    if(u1ValBV > BV_INVALID)
    {
        pSonyProductTypeInfo->eBoardVer = BV_INVALID;
    }
    else
    {
        pSonyProductTypeInfo->eBoardVer = u1ValBV;
    }
    pSonyProductTypeInfo->modelVariationID = u1ValMVID;
    pSonyProductTypeInfo->CISlot = u1ValCISlot;
    return;
}

